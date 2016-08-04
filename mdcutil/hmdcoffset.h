#ifndef HMDCOFFSET_H
#define HMDCOFFSET_H

#include "hreconstructor.h"
#include "hstartdef.h"
#include "hmdctimecut.h"
#include "hlocation.h"

typedef Int_t   MyInt;
typedef Float_t MyFloat;
typedef TH1F    MyHist;

class TFile;
class HCategory;
class HIterator;
class HMdcCalPar;
class HMdcCalParRaw;
class HMdcLookupGeom;
class HMdcLookupRaw;
class HMdcTimeCut;
class HMdcSizesCells;
class TH2F;
class TNtuple;
class TF1;
class TString;

typedef MyInt MyField[6][4][16][96][2048];

class HMdcOffset : public HReconstructor {
protected:
    static const Int_t nbin;            // Number of bins ih the histogramms
    static const Int_t nbinm1;          // nbin - 1
    static const Int_t nbinp1;          // nbin + 1
    static const Int_t nSubEvents;      // The max. number of objects per event
    HCategory*         rawCat;          // pointer to the raw data
    HCategory*         hitStartCat;     // pointer to the cal data
    HCategory*         clusCat;         // pointer to the cluster data
    HIterator*         iter;            // iterator on raw data
    HIterator*         iter_start;      // iterator on start cal data.
    HIterator*         iter_clus;       // iterator on clus data.
    HLocation          locraw;          //
    HMdcCalParRaw*     calparraw;       // pointer to HMdcCalParRaw container
    HMdcTimeCut*       timecut;         // pointer to HMdcTimeCut container
    HMdcLookupGeom*    lookupgeom;      // pointer to HMdcLookupGeom container
    HMdcLookupRaw*     lookupraw;       // pointer to HMdcLookupRaw container
    HMdcSizesCells*    sizescells;      // pointer to HMdcSizesCells container

    Float_t   signalspeed;              //! speed of the signal on the wire [ns/mm]
    Int_t     eventcounter;
    Int_t     skipcounter;
    Int_t     nSkipEvents;              // number of skipped events per file
    Int_t     nStep;                    // step size for print events
    Float_t   validRange;               // allowed interval arround mean of offsets
    Bool_t    isPulserFile;             // flag for a external calibration file (pulser)
    Bool_t    noStart;                  // flag if starttime should not be used
    Bool_t    useTimeCuts;              // switch on/off time cuts usage
    Bool_t    useClusters;              // switch on/off clusters/raw
    Bool_t    useWireOffset;            // switch on/off substraction of signal time on wire
    Bool_t    useTof;                   // switch on/off substraction of minimum tof
    Bool_t    fillHistsOnly;            // switch to fill hists only (no offset calculation)
    Bool_t    readHists;                // switch to read hists from root file and than calculate offset
    Bool_t    perMBO;                   // switch to calc offsets per MBO
    Bool_t    perMBOafterSingle;        // switch to calc offsets per MBO from single offsets
    Float_t   filterwindow1;            // wide window arround mean to filter (perMBOafterSingle)
    Float_t   filterwindow2;            // narrow window arround mean to filter (perMBOafterSingle)
    Int_t     filenumber;               // counter for the read histogram files
    Char_t*   fNameAsciiOffset;         // file name of ascii output file
    Char_t*   fNameRootOffset;          // file name of root output file
    FILE*     ferrorlog;                // File pointer to errorlog
    TNtuple*  offsetTuple;              // pointer to NTuple for offset
    TNtuple*  offsetPulserTuple;        // pointer to NTuple for multiple peaks of pulser files
    Float_t   minfitthreshold;          // minimum threshold for linear fit (y-range)
    Float_t   maxfitthreshold;          // maximum threshold for linear fit (y-range)

    Int_t     offsetfitNoise;           // offset of the fit range for the subtraction of the noise (start at yequalzero going to the left)
    Int_t     widthfitNoise;            // width of the fit range
    Bool_t    fitNoise;                 // fit noise or don't

    Int_t     rangeGauss;               // fit range of the gaussian fit around calculated offset

    Int_t     cleanThreshold;           // threshold to clean hinv : bins with less than the threshold will be set to 0

    MyField*  hreverse;                 // array for the drift-time (Time1) to be filled in eventloop
    MyHist*   hinv;                     // drift-time histograms to hold collected data
    MyHist*   hint;                     // integrated drift-time histograms
    MyHist*   htime1temp;               // temp drift-time histogram
    TH2F*     htime1_mbo        [16];   // 2-D hist time1 vers tdc
    TH2F*     htime1_lay         [6];   // 2-D hist time1 vers cell
    TH2F*     htime1_lay_inv_norm[6];   // 2-D hist time1 vers cell normalized to counts
    TH2F*     htime1_lay_int     [6];   // 2-D hist integrated time1 vers cell
    TH2F*     htime1_lay_int_norm[6];   // 2-D hist integrated time1 vers cell normalized to counts
    TF1*      toffunc         [4][6];   // TF1 fits for minimum tof
    MyFloat   yequalzero;               // intersection point of the first  fit and the x-axis
    MyFloat   crosspointX;              // x-value of the intersection point of the two linear fits->offset

    MyFloat   fitpar0;                  // first fit parameter of the first linear fit
    MyFloat   fitpar0error;             // error of the first fit parameter of the first linear fit
    MyFloat   fitpar1;                  // second fit parameter of the first linear fit
    MyFloat   fitpar1error;             // error of the second fit parameter of the first linear fit

    MyFloat   fitparNoise0;             // first fit parameter of the second linear fit
    MyFloat   fitparNoise0error;        // error of the first fit parameter of the second linear fit
    MyFloat   fitparNoise1;             // second fit parameter of the second linear fit
    MyFloat   fitparNoise1error;        // error of the second fit parameter of the second linear fit
    MyFloat   totalsigma;               // sigma of the offset calculated from the sigmas of the two linear fits


    MyFloat   fitGaussMean;             // mean of the gaussian fit
    MyFloat   fitGaussSigma;            // sigma of the gaussian fit

    Float_t   meanhOffset;
    Float_t   offsets     [6][4][16][96];
    Float_t   offsetErr   [6][4][16][96];
    Float_t   offset1     [6][4][16][96];
    Int_t     integral    [6][4][16][96];
    Float_t   fitslope1   [6][4][16][96];
    Float_t   fitslope2   [6][4][16][96];
    Float_t   offsetpulser[6][4][16][96][5];
    Float_t   myoffset;
    Float_t   myerror;
    Bool_t    debug;                     //! debug flag
public:
    HMdcOffset();
    HMdcOffset(const Text_t* name,const Text_t* title);
    ~HMdcOffset();
    
    Bool_t           init();
    Bool_t           reinit();

    Bool_t           finalize();
    Int_t            execute();

    void             setSignalSpeed     (Float_t speed)             { signalspeed     = speed; }
    void             setCounter         (Int_t size)                { nStep           = size; }
    void             setOutputAscii     (const Char_t*);
    void             setOutputRoot      (const Char_t*);
    void             setPulserFile      ()                          { isPulserFile    = kTRUE; }
    void             setNotUseStart     (Bool_t nouse)              { noStart         = nouse; }
    void             setUseClusters     (Bool_t use)                { useClusters     = use; }
    void             setUseWireOffset   (Bool_t use)                { useWireOffset   = use; }
    void             setUseTof          (TString inp);
    void             setFillHistsOnly   (Bool_t fill)               { fillHistsOnly   = fill; }
    void             setReadHists       (TString inp,Bool_t print=kFALSE);
    void             setFillPerMBO      (Bool_t fillPerMBO)         { perMBO          = fillPerMBO; }
    void             setFillPerMBOafterSingle(Bool_t fillPerMBO)    { perMBOafterSingle = fillPerMBO; }
    void             setFilterWindow1   (Float_t window)            { filterwindow1   = window;}
    void             setFilterWindow2   (Float_t window)            { filterwindow2   = window;}
    void             setNSkipEvents     (Int_t nskipevents)         { nSkipEvents     = nskipevents; }
    void             setValidOffsetRange(Float_t range)             { validRange      = range; }
    void             setUseTimeCuts     (Bool_t cut)                { useTimeCuts     = cut; }
    void             setNoiseOffset     (Int_t i)                   { offsetfitNoise  = i; }
    void             setNoiseWidth      (Int_t i)                   { widthfitNoise   = i; }
    void             setThresholdMin    (Float_t f)                 { minfitthreshold = f; }
    void             setThresholdMax    (Float_t f)                 { maxfitthreshold = f; }
    void             setNoise           (Int_t o, Int_t w)          { offsetfitNoise  = o; widthfitNoise = w; }
    void             setFitNoise        (Bool_t fit)                { fitNoise        = fit;}
    void             setThreshold       (Float_t min, Float_t max)  { minfitthreshold = min; maxfitthreshold = max; }
    void             setRangeGauss      (Int_t i)                   { rangeGauss      = i; }
    void             setDebug           ()                          { debug           = kTRUE; }
    void             setCleanThreshold  (Int_t thresh)              { cleanThreshold  = thresh; }
protected:
    Int_t            executeRaw         ();
    Int_t            executeClus        ();
    void             setDefault         ();
    ofstream*        openAsciiFile      ();
    TDirectory*      Mkdir              (TDirectory *,const Char_t *, Int_t, Int_t p=1);
    void             createHist         (TFile*,Int_t, Int_t, Int_t, Int_t, Bool_t = kFALSE);
    void             createHist_2D      (Int_t, Int_t, Bool_t = kFALSE);
    void             fillHist           (Int_t, Int_t, Int_t, Int_t);
    void             fillHist_2D        (Int_t, Int_t, Int_t, Int_t);
    Int_t            fitHist            (Int_t, Int_t, Int_t, Int_t);
    void             writeHist          (TFile*);
    void             writeHist_2D       (Int_t s, Int_t m);
    void             deleteHist         ();
    void             deleteHist_2D      ();
    void             fillNTuples        (Int_t, Int_t, Int_t, Int_t);
    Float_t          getstarttime       ();
    void             writeAscii         (ofstream&, Int_t, Int_t, Int_t, Int_t);
    void             initVariables      ();
    void             findMultiplePeaks  (Int_t,Int_t,Int_t,Int_t);
    void             initMemory         ()
    {
	for(Int_t s = 0;s < 6; s ++){
	    for(Int_t m = 0; m < 4; m ++){
		for(Int_t mb = 0; mb < 16; mb ++){
		    for(Int_t t = 0; t <96; t ++){
			for(Int_t bin = 0; bin < 2048; bin ++){
			    (*hreverse) [s][m][mb][t][bin] = 0;
			}
		    }
		}
	    }
	}
    }
    void              initArrays         ()
    {
	for(Int_t s = 0; s < 6; s ++){
	    for(Int_t mo = 0; mo < 4; mo ++){
		for(Int_t mb = 0; mb < 16; mb ++){
		    for(Int_t t = 0; t < 96; t ++){
			offsets  [s][mo][mb][t] = 0;
			offsetErr[s][mo][mb][t] = 0;
			offset1  [s][mo][mb][t] = 0;
			integral [s][mo][mb][t] = 0;
                        fitslope1[s][mo][mb][t] = 0;
                        fitslope2[s][mo][mb][t] = 0;
			for(Int_t j = 0; j < 5; j ++)
			{
			    offsetpulser[s][mo][mb][t][j] = 0;
			}
		    }
		}
	    }
	}
    }
    Bool_t            testTimeCuts       (Int_t s, Int_t m,Float_t t1,Float_t t2)
    {
      if(timecut->cutTime1   (s,m,t1)  &&
	 timecut->cutTime2   (s,m,t2)  &&
	 timecut->cutTimesDif(s,m,t2,t1)){ return kTRUE; }
      else { return kFALSE; }
    }
    void             fillArrays          (TH1F*,Int_t, Int_t, Int_t, Int_t);
    void             fillCalParRaw       (TH1F*,Int_t, Int_t, Int_t, Int_t);
    void             printStatus         ();

public: // This has to be placed at the end (--> root dox)
    ClassDef(HMdcOffset, 0)  // class for the calculation of the offsets per tdc channel
};

#endif /* !HMDCOFFSET_H */
