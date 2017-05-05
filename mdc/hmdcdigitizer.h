#ifndef HMDCDITIZER_H
#define HMDCDITIZER_H
using namespace std;
#include "hreconstructor.h"
#include "hcategory.h"
#include "TRandom.h"
#include "TNtuple.h"
#include "TVector2.h"
#include <vector>
#include <map>
#include <iostream> 
#include <iomanip>
#include <stdlib.h>
#include <cmath>

#include "hlocation.h"
#include "hmdccal1sim.h"

class HIterator;
class HMdcLayerGeomPar;
class HMdcDigitPar;
class HMdcCal2ParSim;
class HMdcCal1Sim;
class HMdcCellEff;
class HMdcWireStat;
class HMdcDeDx2;
class HMdcTimeCut;
class HMdcGeomStruct;
class HMdcSizesCells;
class HMdcGeantCell;
class HGeantKine;
class TFile;
class TGraph;


#define  NMAXHITS 15

class HMdcDigiLayEff {
public:
    Int_t trackNum;
    Float_t eff[6][4][6];
    Int_t   ct [6][4][6];
    // vars for leyer eff calculation dpendending on impact angle
    Int_t   cmin[6][4][6];       // cell of min track length
    Int_t   cmax[6][4][6];       // cell of max track length
    Float_t Lmin[6][4][6];       // min track length
    Float_t Lmax[6][4][6];       // max track length

    HMdcDigiLayEff(Int_t tr=-1){
	trackNum = tr;
	fill();
	memset(&ct [0][0][0],0,6*4*6*sizeof(Int_t));
    }


    void clear(){
        trackNum = -1;
        memset(&eff[0][0][0],0,6*4*6*sizeof(Float_t));
        memset(&ct [0][0][0],0,6*4*6*sizeof(Int_t));
    }

    void fill(){
	for(Int_t s=0;s<6;s++){
	    for(Int_t m=0;m<4;m++){
		for(Int_t l=0;l<6;l++){
		    eff[s][m][l] = gRandom->Rndm();
		}
	    }
	}

    }
};



class HMdcDigitizer : public HReconstructor {
private:
  HCategory*        fGeantMdcCat;  //! MDC HGeant input data
  HCategory*        fGeantKineCat; //! HGeantKine input data
  HLocation         loc;           //! Location for new object
  HLocation         locnoise;      //! Location for new object

  HCategory*        fGeantCellCat; //! Pointer to sim data category
  HCategory*        fCalCat;       //! Pointer to cal data category
  HMdcGeantCell*    hit;           //! Pointer to HMdcGeantCell hit
  HMdcGeantCell*    hitReal;       //! Pointer to HMdcGeantCell hit
  HMdcLayerGeomPar* fDigitGeomPar; //! Digitisation "geom" parameters
  HMdcDigitPar*     fDigitPar;     //! Digitisation "phys" parameters
  HMdcCal2ParSim*   fCal2ParSim;   //! pointer to cal2 parameter container
  HMdcCellEff*      fCellEff;      //! pointer to cell efficiency parameter container
  HMdcWireStat*     fWireStat;     //! pointer to wire status parameter container
  HMdcTimeCut*      fTimeCut;      //! pointer to time cut parameter container
  HMdcSizesCells*   fsizescells;   //! pointer to hmdcsizescells parameter container
  HMdcDeDx2*        fdEdX;         //! pointer to MdcDeDx2 parameter container
  HMdcGeomStruct*   geomstruct;    //! pointer to hmdcgeomstruct parameter container
  HMdcCal1Sim*      fCal;          //! pointer to data
  HMdcCal1Sim*      fCalnoise;     //! pointer to noise data
  HMdcGeantCell*    fCell;         //! pointer to Container for HMdcGeantCell
  HIterator*        iterin;        //! Iterator over input category
  HIterator*        itercell;      //! Iterator over cell category
  HIterator*        itercal1;      //! Iterator over cal1 category
  Int_t             fEventId;      //! Number of current event
  Float_t           yDist;         //! Distance to the sence wire
  Float_t           pi;            //! Il y quatre pis a une vache

  Int_t    modetdc;                //! 2 leading edges or leading and trailing edge of the signal
  Int_t    setup[6][4];            //! setup of Mdc (sec,mod)

  Float_t  time1;                  //! drift time1 calculated by HMdcCal2ParSim
  Float_t  time1Error;             //! drift time1 error calculated by HMdcCal2ParSim
  Float_t  time2;                  //! drift time2 calculated by HMdcCal2ParSim
  Float_t  time2Error;             //! drift time2 error calculated by HMdcCal2ParSim
  Float_t  myalpha;                //! impact angle of the track in coordinate system of HMdcCal2ParSim
  Bool_t   useError;               //! flag for use/don't use error in time1/time2 in output
  Bool_t   useWireOffset;          //! flag for use/don't use error in time1/time2 in output
  Bool_t   useDeDx2;               //! flag for use/don't use MdcDeDx2 container for t2-t1 simulation
  Bool_t   useTimeCut;             //! flag for use/don't use MdcTimeCut container

  TFile*   myoutput;               //! file pointer for NTuple
  TNtuple* distance_time;          //! nTuple for internal information of the digitizer
  Bool_t   fntuple;                //!  switch for use/not use NTuple

  Bool_t   useTof;                 //! switch for use/not use of tof in output
  Float_t  offsets[4];             //! offsets are used to substract min tof
  Bool_t   useOffsets;             //! switch for use/not use offset substraction in output

  TGraph*  fbetadEdx;              //! dedx as function of beta for scaling
  Float_t  fBetaLow;               //! lower beta range for scaling
  Bool_t   useDeDxScaling;         //! switch on/off efficiency scaling with energyloss
  Bool_t   useDeDxTimeScaling;     //! switch on/off time error scaling with energyloss
  Float_t  effLevel[4];            //! level of requiered maximum charge to create a signal (for example 20 (=20%))
  Bool_t   useCellEff;             //! switch for use/not use cell efficiency cut
  Bool_t   useWireStat;            //! switch for use/not use wire stat container
  Bool_t   useWireStatEff;         //! switch for use/not use eff from wire stat container (default = kTRUE)
  Bool_t   useWireStatOffset;      //! switch for use/not use offset from wire stat container (default = kTRUE)
  Bool_t   useLayerThickness;      //! switch for use/not use layer thisckness eff loss
  Bool_t   useDeltaElectrons;      //! switch for use/not use delta electron time smearing
  Bool_t   useDeltaMomSelection;   //! switch for use/not use momentum below momMaxDeltaElecCut for primary electrons to identify delta electrons
  Int_t    ionID;                  //! beam ion (au ==109)
  Float_t  fProbDeltaAccepted;     //! 0 - 1 probability to accept a delta electron (yield adjustment)
  Float_t  t1minDeltaElec;         //! delta electron smearing lower time range for t1 [ns]
  Float_t  t1maxDeltaElec;         //! delta electron smearing upper time range for t1 [ns]
  Float_t  momMaxDeltaElecCut;     //! delta electron smearing : primary electrons below this mom are considdered to be delta electrons  [MeV/c]
  Float_t  momMinDeltaCut[6];      //! min mom cut per sector (account for different mirror materials) [MeV/c]

  map<HGeantKine*,Float_t> mDeltaTrackT0;    //! map delta electron candidates to t1 offsets
  map<HGeantKine*,Float_t>::iterator itDelta;//! map delta electron candidates to t1 offsets

  Bool_t   hasPrinted;             //! flag is set , if printStatus() is called
  Float_t  noiseLevel[4];          //! level of randon noise for each module type
  Bool_t   useNoise;               //! switch for use/not use of noise generator
  Int_t    arrayNoise[5];          //! temp array for status of noise
  Int_t    noiseRangeLo[4];        //! lower range of noise for each mdc type
  Int_t    noiseRangeHi[4];        //! upper range of noise for each mdc type
  Float_t  time1noise;             //! time1 generated by the noise generator
  Float_t  time2noise;             //! time2 generated by the noise generator
  Int_t    noisemode;              //! switch for different noise modes
  Int_t    firstHit;               //! number of first valid hit
  Int_t    secondHit;              //! number of second valid hit
  Float_t  firstTime2;             //! time2 of first valid hit
  Int_t    endList1;               //! end of the list of hits belonging to the first valid hit
  Int_t    firstsec;
  Int_t    firstmod;
  Int_t    firstlay;
  Int_t    firstcell;
  Float_t  noisebandwidth;                 //! width of t2-t1 band in noise
  Float_t  noisewhitewidth;                //! width of t2-t1 large region in noise
  Float_t  noisewhiteratio;                //! ration between large region and band in noise
  static  Float_t dTime              [NMAXHITS]; //! drift time1 + tof
  static  Float_t dTime2             [NMAXHITS]; //! drift time2 + tof
  static  Float_t dTimeErr           [NMAXHITS]; //! error of drift time1
  static  Float_t dTime2Err          [NMAXHITS]; //! error of drift time2
  static  Float_t minimumdist        [NMAXHITS]; //! minimum distance to wire
  static  Int_t track                [NMAXHITS]; //! track numbers
  static  Float_t timeOfFlight       [NMAXHITS]; //! tof
  static  Float_t angle              [NMAXHITS]; //! impact angle in coordinate system of the cell
  static  Int_t statusflag           [NMAXHITS]; //! flag for efficiency
  static  Float_t fractionOfmaxCharge[NMAXHITS]; //! value for fraction of maximum charge
  static  Bool_t cutEdge             [NMAXHITS]; //! flag for minimum distance point out of cell
  static  Float_t wireOffset         [NMAXHITS]; //! time for signal propagation on the wire
  static  Float_t efficiency         [NMAXHITS]; //! efficiency  of track in layer
  static  Float_t theta              [NMAXHITS]; //! theta impact

  Float_t time1Real;                 //! drift time1 from real data
  Float_t time2Real;                 //! drift time2 from real data
  Int_t   nHitsReal;                 //! number of hists from real data
  Int_t   embeddingmode;             //  switch for keeping geant hits / realistic embedding
  Float_t signalSpeed;               //  speed of signal on the wire
  Float_t scaleError[4];             //! scaler for error of time per module type
  Float_t scaleErrorMIPS[4];         //! scaler for error of time per module type

  Float_t rndmoffsets[6][4][6][220]; //! random offsets to simulate calibrated offsets
  Float_t sigmaoffsets;              //! sigma of the gausian random offset distribution
  Bool_t  createoffsets;             //! switch kTRUE: create offsets,kFALSE: read from file
  Bool_t  offsetsCreated;            //! remember if offsets were created or read from ascii file
  Float_t scaletime;                 //! simple scaler for manipulating drift times (as done by tdc slopes)
  vector< HMdcDigiLayEff > vLayEff;  //! layer eff random numbers per track
  HMdcDigiLayEff           layEff;   //! layer eff object for calulation of eff depending on impact angle


  Float_t effLayerThickness(Float_t xcoor,Float_t ycoor,Float_t th,Float_t ph,Int_t s,Int_t m,Int_t l) ;
  Int_t findTrack(Int_t trk);


public:
  HMdcDigitizer(void);
  HMdcDigitizer(const Text_t* name,const Text_t* title);
  HMdcDigitizer(const Text_t* name,const Text_t* title,Int_t,Bool_t);
  ~HMdcDigitizer(void);
  void    setOffsets            (Float_t off0,Float_t off1,Float_t off2,Float_t off3,Int_t on_off = 1);
  void    setEffLevel           (Float_t eff0,Float_t eff1,Float_t eff2,Float_t eff3,Int_t on_off = 1);
  void    setEffScaling         (Float_t betalow,Bool_t usescaling) {fBetaLow = betalow, useDeDxScaling = usescaling;}
  void    setTimeErrScaling     (Bool_t usescaling) { useDeDxTimeScaling = usescaling;}
  //---------------------noise simulation-------------------------------------------
  void    setNoiseLevel         (Float_t noise0,Float_t noise1,Float_t noise2,Float_t noise3,Int_t on_off = 1);
  void    setNoiseRange         (Int_t rangeLo0,Int_t rangeLo1,Int_t rangeLo2,Int_t rangeLo3,
				 Int_t rangeHi0,Int_t rangeHi1,Int_t rangeHi2,Int_t rangeHi3);
  void    setNoiseBandWidth     (Float_t w)              {noisebandwidth    = w;     }
  void    setNoiseWhiteWidth    (Float_t w)              {noisewhitewidth   = w;     }
  void    setNoiseWhiteRatio    (Float_t w)              {noisewhiteratio   = w;     }
  Float_t getNoiseBandWidth     ()                       {return noisebandwidth;     }
  Float_t getNoiseWhiteWidth    ()                       {return noisewhitewidth;    }
  Float_t getNoiseWhiteRatio    ()                       {return noisewhiteratio;    }
  //---------------------switches for cal1sim output--------------------------------
  void    setErrorUse           (Bool_t use)             {useError          = use;   }
  void    setTofUse             (Bool_t use)             {useTof            = use;   }
  void    setWireOffsetUse      (Bool_t use)             {useWireOffset     = use;   }
  void    setOffsetsUse         (Bool_t use)             {useOffsets        = use;   }
  void    setCellEffUse         (Bool_t use)             {useCellEff        = use;   }
  void    setWireStatUse        (Bool_t use)             {useWireStat       = use;   }
  void    setNoiseUse           (Bool_t use)             {useNoise          = use;   }
  void    setDeDxUse            (Bool_t use)             {useDeDx2          = use;   }
  void    setTimeCutUse         (Bool_t use)             {useTimeCut        = use;   }
  Bool_t  getErrorUse           ()                       {return useError;           }
  Bool_t  getTofUse             ()                       {return useTof;             }
  Bool_t  getWireOffsetUse      ()                       {return useWireOffset;      }
  Bool_t  getOffsetsUse         ()                       {return useOffsets;         }
  Bool_t  getCellEffUse         ()                       {return useCellEff;         }
  Bool_t  getWireStatUse        ()                       {return useWireStat;        }
  Bool_t  getNoiseUse           ()                       {return useNoise;           }
  Bool_t  getDeDxUse            ()                       {return useDeDx2;           }
  Bool_t  getTimeCutUse         ()                       {return useTimeCut;         }
  //---------------------privat ntuple out------------------------------------------
  void    setNTuple             (Bool_t ntuple)          {fntuple           = ntuple;}
  void    setTdcMode            (Int_t mode)             {modetdc           = mode;  }
  void    setNoiseMode          (Int_t mode)             {noisemode         = mode;  }
  void    setEmbeddingMode      (Int_t mode)             {embeddingmode     = mode;  }
  Bool_t  getNTuple             ()                       {return fntuple;            }
  Int_t   getTdcMode            ()                       {return modetdc;            }
  Int_t   getNoiseMode          ()                       {return noisemode;          }
  Int_t   getEmbeddingMode      ()                       {return embeddingmode;      }
  //---------------------signal speed for wireoffsets-------------------------------
  void    setSignalSpeed        (Float_t speed)          {signalSpeed       = speed; }
  Float_t getSignalSpeed        ()                       {return signalSpeed;        }

  //-----------manipulate drift times (tdc slope...)--------------------------------
  void    setScaleTime          (Float_t scale)          {scaletime         = scale; }
  Float_t getScaleTime          ()                       {return scaletime;}
  void    setScalerTime1Err     (Float_t m0 = 0,Float_t m1 = 0,Float_t m2 = 0,Float_t m3 = 0);
  //-----------creating offsets to simulate calibrated offsets----------------------
  void    setWireStatOffsetUse  (Bool_t use)             {useWireStatOffset = use;   }
  Bool_t  getWireStatOffsetUse  ()                       {return useWireStatOffset;  }
  void    initOffsets           (TString filename = "");
  void    setSigmaOffsets       (Float_t sig)            {sigmaoffsets      = sig;   }
  void    setCreateOffsets      (Bool_t create = kTRUE)  {createoffsets     = create;}
  Float_t getSigmaOffsets       ()                       {return sigmaoffsets;}
  Bool_t  getCreateOffsets      ()                       {return createoffsets;}

  //----------- using efficiency ----------------------
  void    setWireStatEffUse     (Bool_t use)             {useWireStatEff    = use;   }
  Bool_t  getWireStatEffUse     ()                       {return useWireStatEff;     }
  void    setLayerThicknessEffUse(Bool_t use)            {useLayerThickness = use;   }
  Bool_t  getLayerThicknessEffUse()                      {return useLayerThickness;  }


  //----------- using delta electrons -----------------
  void   setDeltaElectronUse(Bool_t use, Bool_t useDeltaMomSel=kFALSE, Int_t ionId=109,Float_t t1min=-950.,Float_t t1max=400.,Float_t momCut=20.,Float_t probDelta=2.){  useDeltaElectrons = use;useDeltaMomSelection = useDeltaMomSel; ionID=ionId; t1minDeltaElec = t1min;  t1maxDeltaElec = t1max; momMaxDeltaElecCut = momCut; fProbDeltaAccepted = probDelta;}
  Bool_t getDeltaElectronUse() { return useDeltaElectrons;}
  void   setDeltaElectronMinMomCut(Float_t s0=2.,Float_t s1=2.,Float_t s2=4.5,Float_t s3=2.,Float_t s4=2.,Float_t s5=4.5) { momMinDeltaCut[0]=s0; momMinDeltaCut[1]=s1; momMinDeltaCut[2]=s2;  momMinDeltaCut[3]=s3;  momMinDeltaCut[4]=s4; momMinDeltaCut[5]=s5; }
  void   printStatus           ();

  void setTimeCutFlags(HMdcCal1Sim* cal1);


   //----------- standard task functions --------------
  Bool_t  init                  (void);
  Bool_t  reinit                (void);
  Int_t   execute               (void);
  Bool_t  finalize              ();

protected:
  void    setParContainers      ();
  void    initVariables         ();
  Bool_t  transform             (Float_t,Float_t,Float_t,Float_t,Float_t,Int_t);
  void    storeCell             (Float_t,Float_t,Float_t,Int_t,Bool_t,Float_t,Float_t,Float_t);
  void    select                (Int_t);
  //---------------------noise simulation-------------------------------------------
  void    fillNoise             (Int_t, Int_t, Int_t, Int_t,Int_t, Int_t, Int_t, Int_t);
  void    handleOverFlow        (Int_t, Int_t, Int_t, Int_t);
  void    setLoopVariables      (Int_t,Int_t,Int_t,Int_t,Bool_t check = kFALSE);
  Float_t fillTime1Noise        (Int_t);
  Float_t fillTime2Noise        (Int_t);
  void    fillNoiseLists        (HMdcCal1Sim* cal1,Int_t,Int_t);
  void    fillNoiseToGeantCells (Int_t,HMdcCal1Sim* p);
  //---------------------privat ntuple out------------------------------------------
  void    fillNTuple            (Int_t ,Int_t ,Int_t ,Int_t , Int_t, HMdcGeantCell* ,TNtuple*);
  void    fillNTuple            (Int_t, Int_t, Int_t, Int_t, Float_t, Float_t, Int_t);
  void    setNTuples            (void);
  Bool_t  evalWireStat          (Int_t, Int_t, Int_t, Int_t);
  //---------------------handling data arrays--------------------------------------
  void    initArrays            ();
  void    fillArrays            (Int_t,Int_t,Int_t,HMdcGeantCell*);
  void    fillArraysReal        (Int_t i);

  void    resetListVariables    ();
  void    resetCal1Real         ();

  void    setTime1Real          (Float_t t1)             {time1Real = t1;  }
  void    setTime2Real          (Float_t t2)             {time2Real = t2;  }
  void    setNHitsReal          (Int_t i)                {nHitsReal = i;   }
  Float_t getTime1Real          ()                       {return time1Real;}
  Float_t getTime2Real          ()                       {return time2Real;}
  Int_t   getNHitsReal          ()                       {return nHitsReal;}
  void    getCal1Real           ();
  void    setEfficiencyFlags    (Int_t,Int_t,Int_t,Int_t);
  void    setTimeCutFlags       (Int_t,Int_t,Int_t,Int_t);
  void    fillTrackList         (HMdcCal1Sim*);
  void    findFirstValidHit     ();
  void    findSecondValidHit    ();
  Int_t   findNonValidHit       ();
  void    getMdcSetup           ();
  Bool_t  testMdcSetup          (Int_t s, Int_t m);
  void    setTime1Noise         (Float_t time)           {time1noise = time; }
  void    setTime2Noise         (Float_t time)           {time2noise = time; }
  Float_t getTime1Noise         ()                       {return time1noise; }
  Float_t getTime2Noise         ()                       {return time2noise; }
  void    setFirstHit           (Int_t hit1)             {firstHit   = hit1; }
  void    setSecondHit          (Int_t hit2)             {secondHit  = hit2; }
  void    setFirstTime2         (Float_t time2)          {firstTime2 = time2;}
  void    setEndList1           (Int_t end)              {endList1   = end;  }
  Int_t   getFirstHit           ()                       {return firstHit;   }
  Int_t   getSecondHit          ()                       {return secondHit;  }
  Float_t getFirstTime2         ()                       {return firstTime2; }
  Int_t   getEndList1           ()                       {return endList1;   }
  //----------------------handling drift cell informations--------------------------
  void    setDTime1             (Int_t i,Float_t time)   {dTime              [i] = time;   }
  void    setDTime2             (Int_t i,Float_t time)   {dTime2             [i] = time;   }
  void    setDTime1Err          (Int_t i,Float_t timeErr){dTimeErr           [i] = timeErr;}
  void    setDTime2Err          (Int_t i,Float_t timeErr){dTime2Err          [i] = timeErr;}
  void    setMinimumDist        (Int_t i,Float_t dist)   {minimumdist        [i] = dist;   }
  void    setTrackN             (Int_t i,Int_t number)   {track              [i] = number; }
  void    setTof                (Int_t i,Float_t tof)    {timeOfFlight       [i] = tof;    }
  void    setAngle              (Int_t i,Float_t a)      {angle              [i] = a;      }
  void    setStatus             (Int_t i,Int_t stat)     {statusflag         [i] = stat;   }
  void    setFractionOfmaxCharge(Int_t i,Float_t f)      {fractionOfmaxCharge[i] = f;      }
  void    setCutEdge            (Int_t i,Bool_t cut)     {cutEdge            [i] = cut;    }
  void    setWireOffset         (Int_t i,Float_t off)    {wireOffset         [i] = off;    }
  void    setEfficiency         (Int_t i,Float_t eff)    {efficiency         [i] = eff;    }
  void    setTheta              (Int_t i,Float_t th)     {theta              [i] = th;    }
  Float_t getDTime1             (Int_t i)                {return dTime              [i];   }
  Float_t getDTime2             (Int_t i)                {return dTime2             [i];   }
  Float_t getDTime1Err          (Int_t i)                {return dTimeErr           [i];   }
  Float_t getDTime2Err          (Int_t i)                {return dTime2Err          [i];   }
  Float_t getMinimumDist        (Int_t i)                {return minimumdist        [i];   }
  Int_t   getTrackN             (Int_t i)                {return track              [i];   }
  Float_t getTof                (Int_t i)                {return timeOfFlight       [i];   }
  Float_t getAngle              (Int_t i)                {return angle              [i];   }
  Int_t   getStatus             (Int_t i)                {return statusflag         [i];   }
  Float_t getFractionOfmaxCharge(Int_t i)                {return fractionOfmaxCharge[i];   }
  Bool_t  getCutEdge            (Int_t i)                {return cutEdge            [i];   }
  Float_t getWireOffset         (Int_t i)                {return wireOffset         [i];   }
  Float_t getEfficiency         (Int_t i)                {return efficiency         [i];   }
  Float_t getTheta              (Int_t i)                {return theta              [i];   }
  Float_t getCellEffLevel       (Int_t i)                {return effLevel           [i];   }
  Float_t getNoiseLevel         (Int_t i)                {return noiseLevel         [i];   }
  Float_t getOffset             (Int_t i)                {return offsets            [i];   }
  Int_t   getNoiseRangeHi       (Int_t i)                {return noiseRangeHi       [i];   }
  Int_t   getNoiseRangeLo       (Int_t i)                {return noiseRangeLo       [i];   }

  ClassDef(HMdcDigitizer,0) // Digitizer of MDC data
};

#endif









