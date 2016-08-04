#ifndef HMDCMACROMAKER_H
#define HMDCMACROMAKER_H
#include "TNamed.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TArrayI.h"
#include "TArrayF.h"

class TString;
class HMdcGarMacroMaker : public TNamed {
protected:

    TString fNameAsciiOut;         //! file name of ascii output file
    TString fNameSignalOut;        //! file name of output file for signals
    TString fNameGasIn;            //! file name of gas Input file
    FILE *output;                  //! file pointer to out put
    Bool_t postscript;             //! print pictures to ps
    TString postscriptfile;        //! file name of ps output file
    TString setup;                 //! contains the print options
    static Float_t cathodth[4];    //! thickness of cathod per chamber type
    static Float_t senseth [4];    //! thickness of sense per chamber type
    static Float_t fieldth [4];    //! thickness of field per chamber type

    static Float_t cathodpitch[4]; //! pitch of cathod per chamber type
    static Float_t sensepitch [4]; //! pitch of sense per chamber type
    static Float_t fieldpitch [4]; //! pitch of field per chamber type

    static Int_t cathodhv [4];     //! hv of cathod per chamber type
    static Int_t sensehv  [4];     //! hv of sense per chamber type
    static Int_t fieldhv  [4];     //! hv of field per chamber type

    static Int_t cathodhvmod [4];  //! hv of cathod for modification
    static Int_t sensehvmod  [4];  //! hv of sense  for modification
    static Int_t fieldhvmod  [4];  //! hv of field for modification

    static Int_t ncathod [4];      //! number of cathod wires per chamber type
    static Int_t nsense  [4];      //! number of sense wires  per chamber type
    static Int_t nfield  [4];      //! number of field wires per chamber type

    static Float_t dc_s  [4];      //! distance cathod and sense wire plane
    static Float_t cathodx [4];    //! x of cathod per chamber type
    static Float_t sensex  [4];    //! x of sense per chamber type
    static Float_t fieldx  [4];    //! x pitch of field per chamber type

    static Float_t area [4][4];    //! window of interest
    static Int_t cwire[4];         //! wire which is selected for read out
    static Int_t cwire_foil[4];    //! wire which is selected for read out if cathode foil is used
    static Float_t time_up [4];    //! upper limit of time window
    static Float_t time_low[4];    //! lower limit of time window

    static Int_t maxLoop[4][18];   //! upper limit for loop over distance
    static Int_t minLoop[4][18];   //! lower limit for loop over distance

    static Float_t B_components[3];//! components of the magnetic field

    Int_t mdc;                     //! module type
    Int_t temperature;             //! temperature of gas
    Int_t pressure;                //! pressure of gas
    Int_t argon;                   //! fraction of argon in gas mixture (in %)      (allowed in combination with co2)
    Int_t helium;                  //! fraction of helium in gas mixture (in %)     (allowed in combination with ibutane)
    Int_t ibutane;                 //! fraction of iso-butan in gas mixture (in %)  (allowed in combination with helium)
    Int_t co2;                     //! fraction of co2 in gas mixture (in %)  (allowed in combination with argon)
    Float_t mobility;              //! mobility for magboltz
    Int_t avalanche;               //!
    Int_t collisions;              //! number of collisions for monte carlo
    static Double_t maxstep[4];    //! maxstepsize of RungeKutta
    static Double_t mc_dist[4];    //! maxstepsize of monte carlo integration distance
    Double_t trap_radius;          //! trap radius around th wire
    TString particle;              //!
    Float_t energy;                //! particle energy in GeV
    Double_t range;                   //! max track length;
    Float_t timebin;               //! bin size of time window;
    Int_t nTimeBin;                //! number of bins for time window;
    Float_t angleDeg;              //! impact angle of track;
    Int_t angleStep;               //! impact angle of track (step of stepsizeAngle);
    Float_t angleStepSize;         //!
    Bool_t printclusters;          //! switch print option in SIGNAL
    Int_t nSignals;                //! number of signal per sample point
    Bool_t writeData;              //! flag for writing out datasets
    Bool_t printcell;              //! switch print on/off for cell
    Bool_t printoption;            //! switch print on/off for option
    Bool_t printgas;               //! switch print on/off for gas
    Bool_t printfield;             //! switch print on/off for field
    Bool_t printdrift;             //! switch print on/off for drift
    Bool_t printsignal;            //! switch print on/off for signal
    Bool_t print3d;                //! switch print on/off for 3d
    Bool_t cathode_foils;          //! use cathode foils instead of cathode wires
    Bool_t doRungeKutta;           //! use rungeKutta by default, otherwise MC
    Float_t  isochrones;           //! distance of isochrones in musec
    Int_t  nLines;                 //! number of lines for isochrones plot
    void   initVariables();
    void   putOut(const Char_t*);
    void   openOutPut();
    void   closeOutPut();
    void   printCell();
    void   printOptions();
    void   printGeometry();
    void   printField();
    void   printMagnetic();
    void   printGas();
    void   printDrift();
    void   printSignal();
    void   print3D();
    void   printMetaHeader();
    void   printMetaTrailer();
    void   parseArguments(TString);
    void   printKeyWords();
public:
    HMdcGarMacroMaker(const Char_t* name="",const Char_t* title="");
    ~HMdcGarMacroMaker();
    void   setPrintSetup(TString);
    void   setFileNameOut(TString);
    void   setFileNameSignals(TString);
    void   setFileNameGasFile(TString);
    void   setPsFileName(TString);
    void   setWriteDataSets(Bool_t write=kTRUE){writeData=write;}
    void   setMdcType(Int_t type) {mdc        =type;}
    void   setAngle(Float_t a)
    {
	angleDeg   =a;
        angleStep  =(Int_t)(angleDeg/angleStepSize);
    }
    void   setCathodeFoil(Bool_t foil){cathode_foils=foil;}
    void   setAngleStepSize(Float_t s){angleStepSize=s;}
    void   setBFieldComponents(Float_t,Float_t,Float_t);
    void   setTemperature(Int_t t){temperature=t;}
    void   setPressure(Int_t p)   {pressure   =p;}

    void   setArgon(Int_t ar);
    void   setHelium(Int_t he);
    void   setIButane(Int_t b);
    void   setCO2(Int_t co);
    void   setArgonCO2(Int_t ar,Int_t co);
    void   setArgonIButane(Int_t ar,Int_t ib);
    void   setHeliumIButane(Int_t he,Int_t ib);

    void   setMobility(Float_t m) {mobility   =m;}
    void   setParticle(TString p) {particle   =p;}
    void   setEnergy(Float_t e)   {energy     =e;}
    void   setRange(Int_t r)      {range      =r;}
    void   setAvalanche(Int_t a)  {avalanche  =a;}
    void   setNCollisions(Int_t c){collisions =c;}
    void   setTrapRadius(Double_t d) {trap_radius=d;}
    void   setUseRungeKutta(Bool_t doit) { doRungeKutta = doit; }
    void   setRungeKuttaMaxStep(Double_t m0,Double_t m1,Double_t m2,Double_t m3) {maxstep[0]=m0;maxstep[1]=m1;maxstep[2]=m2;maxstep[3]=m3;}
    void   setMCDistance(Double_t m0,Double_t m1,Double_t m2,Double_t m3)            {mc_dist[0]=m0;mc_dist[1]=m1;mc_dist[2]=m2;mc_dist[3]=m3;}

    void   setNSignals(Int_t nS)  {nSignals   =nS;}
    void   setNLines(Int_t ni)    {nLines     =ni;}
    void   setIsochrones(Float_t ni){isochrones =ni;}
    void   setCathodHvMod(Int_t hv,Int_t i){cathodhvmod[i]=hv;}
    void   setSenseHvMod (Int_t hv,Int_t i){sensehvmod [i]=hv;}
    void   setFieldHvMod (Int_t hv,Int_t i){fieldhvmod [i]=hv;}
    void   setCathodTh   (Float_t* c){for(Int_t i=0;i<4;i++)cathodth   [i]=c[i];}
    void   setSenseTh    (Float_t* c){for(Int_t i=0;i<4;i++)senseth    [i]=c[i];}
    void   setFieldTh    (Float_t* c){for(Int_t i=0;i<4;i++)fieldth    [i]=c[i];}
    void   setCathodPitch(Float_t* c){for(Int_t i=0;i<4;i++)cathodpitch[i]=c[i];}
    void   setSensePitch (Float_t* c){for(Int_t i=0;i<4;i++)sensepitch [i]=c[i];}
    void   setFieldPitch (Float_t* c){for(Int_t i=0;i<4;i++)fieldpitch [i]=c[i];}
    void   setCathodHv   (Int_t* c)  {for(Int_t i=0;i<4;i++)cathodhv   [i]=c[i];}
    void   setSenseHv    (Int_t* c)  {for(Int_t i=0;i<4;i++)sensehv    [i]=c[i];}
    void   setFieldHv    (Int_t* c)  {for(Int_t i=0;i<4;i++)fieldhv    [i]=c[i];}
    void   setNCathod    (Int_t* c)  {for(Int_t i=0;i<4;i++)ncathod    [i]=c[i];}
    void   setNSense     (Int_t* c)  {for(Int_t i=0;i<4;i++)nsense     [i]=c[i];}
    void   setNField     (Int_t* c)  {for(Int_t i=0;i<4;i++)nfield     [i]=c[i];}
    void   setCathodX    (Float_t* c){for(Int_t i=0;i<4;i++)cathodx    [i]=c[i];}
    void   setSenseX     (Float_t* c){for(Int_t i=0;i<4;i++)sensex     [i]=c[i];}
    void   setFieldX     (Float_t* c){for(Int_t i=0;i<4;i++)fieldx     [i]=c[i];}
    void   setArea       (Float_t* c)
    {
	for(Int_t i=0;i<4;i++){
	    for(Int_t j=0;j<4;j++){
		area[i][j]=c[i*4+j];
	    }
	}
    }
    void   setMaxLoop   (Int_t* c)
    {
	for(Int_t i=0;i<4;i++){
	    for(Int_t j=0;j<18;j++){
		maxLoop[i][j]=c[i*18+j];
	    }
	}
    }
    void   setMinLoop   (Int_t* c)
    {
	for(Int_t i=0;i<4;i++){
	    for(Int_t j=0;j<18;j++){
		minLoop[i][j]=c[i*18+j];
	    }
	}
    }
    void   setCWire  (Int_t* c)  {for(Int_t i=0;i<4;i++)cwire    [i]=c[i];}
    void   setCWireFoil(Int_t* c){for(Int_t i=0;i<4;i++)cwire_foil[i]=c[i];}
    void   setTimeUp (Float_t* c){for(Int_t i=0;i<4;i++)time_up  [i]=c[i];}
    void   setTimeLow(Float_t* c){for(Int_t i=0;i<4;i++)time_low [i]=c[i];}
    void   setNTimeBin(Int_t number){nTimeBin=1000;}
    void   setDC_S   (Float_t* c){for(Int_t i=0;i<4;i++)dc_s[i]=c[i];}

    void   setPrintClusters(Bool_t opt){printclusters=opt;}
    void   printMacro();
    void   printStatus(void);
    ClassDef(HMdcGarMacroMaker,1) // Container for the MDC Calibrater1 parameters
};
#endif  /*!HMDCGARMACROMAKER_H*/
