#ifndef HMDCTARGETMON_H
#define HMDCTARGETMON_H

#include "hreconstructor.h"
#include "hgeomvector.h"
#include "TStopwatch.h"
#include "TString.h"
#include "hlocation.h"

class HIterator;
class HCategory;
class HMdcTimeCut;
//class HMdcGetContainers;
//class HMdcSizesCells;
class TH1F;
class TH2F;
class TFile;
class HEventHeader;
class HGeomTransform;
class TPad;
class TPaveLabel;
class TCanvas;
class HldGrepFileSource;
class HldRemoteSource;

class HMdcTargetMon : public HReconstructor {
  protected:
    Bool_t              mode;                  // kTRUE - show canvases 
                                               // in "finalize()" only
                                               // kFALSE - update canvases
                                               // each "timeStep" seconds
    TString            typePrintFile;          // "" - ps, ".gif", ...
    TString            formatEpsTo;            // format for converting eps to
                                               // gif or other gr.format
    Bool_t             deleteEpsFile;          // delete *.eps file after conv.
    HCategory*         fMdcHitCat;             // HMdcHit data category
    HIterator*         iterMdcHit;             // iterator for MdcHitCat
    HLocation          locHit;                 // location for MdcHitCat

    HEventHeader*      evHeader;               // event header
    HGeomVector        target[6];              // target middle point for each sector
    Bool_t             useRtdbTarger[6];       // target param from Rtdb (for each sector)
                                               // otherwise is setted by
                                               // setTargPos(xt,yt,zt)
    HldGrepFileSource *pHldGrepFSrc;           // !=0 - HldGrepFileSource is used
    HldRemoteSource   *pHldRemoteSrc;          // !=0 - HldRemoteSource is used

    Bool_t             isCoilOff;              // kTRUE - magnet off

    Bool_t             firstEvent;             // =kTRUE - befor first ev.
    UInt_t             dateFEvent;             // date of first event
    UInt_t             timeFEvent;             // time of first event
    UInt_t             dateLEvent;             // date of last event
    UInt_t             timeLEvent;             // time of last event
    UInt_t             runId;                  // runId of current run
    TString            fileName;               // list of file names
    TString            fileNameR;              // file name of current run
    TString            dateTime;               // string with date and time

    HGeomTransform*    transf[24];             // mdc mod. <-> lab. transfor.

    Int_t              nPads[6];               // [comb] - pads num. in canvas
    TPad*              fPads[6][18];           // [comb][pad]
    TPaveLabel*        label;                  // canvas label: fileName+...
    Int_t              mdcInSec[6];            // [sec] - mdcs num. in sector
    Bool_t             listMdc[24];            // mdcs list
    Int_t              selectEvByTBits;
    
    // Mdc hits collection [mdc][indx] (for one event):
    Double_t           phi[24][1000];          // phi(deg.) for each hit in lab.
    Double_t           x[24][1000];            // x(mm) hits positions in lab.
    Double_t           y[24][1000];            // y(mm) hits positions in lab.
    Double_t           z[24][1000];            // z(mm) hits positions in lab.
    Short_t            nhits[24];              // hits num. in each mdc
    
    struct TrackPar {
      Double_t z;                              // z of min. dist. to beam line
      Double_t theta;                          // theta of track
      Double_t x0;                             // cross with plane Ztarget
      Double_t y0;                             // cross with plane Ztarget  
    };
    TrackPar           trArr[10000];
    Bool_t             normalizeHists;         // fill with wt=1./nTracks

    Double_t           dPhiCut;                // dPhi cut
    Double_t           r0Cut;                  // r0 cut

    Int_t              combToMdc[6][2];
    TH1F*              dPhiH[6];               // [6] - combinations of MDC,s:
    TH1F*              r0H[6];                 // 1-2,1-3,1-4,2-3,2-4,3-4
    TH1F*              zVerH[6][6];            // [sec][comb]
    TH1F*              zVerDscH[6][6][10];     // [s][c][Downsc.fl.|| trig.Cond]
    TH2F*              zVerVsEventH[6][6];     // [sec][comb]
    Int_t              histsMode;              // =0 - one hist., 
                                               // =1 - 2 hist downsc.flag
                                               // =2 - trigger cond.
    Int_t              nZmBins;                // histogramms parameters
    Double_t           zmin;                   // ...
    Double_t           zmax;                   // ...
    Int_t              canvasWidth;            // canvas width in pixels
    Int_t              canvasHeight;           // canvas height in pixels
    Char_t             cnComb[6][100];         // canvases names
    Char_t             ctComb[6][100];         // canvases titles
    TCanvas*           canvs[6];               // pointers to canvases
    TString            addCnName;              // users part of canvas title

    TStopwatch         timer;                  // timer
    UInt_t             plotType;               // plot type (1,2 or 3)

    Double_t           timeStep;               // canvases updating time step
    
    Stat_t             stat;                   // num.tracks added to hist.
    Bool_t             sumOfFiles;             // =kTRUE - all runs in one hist.
                                               // =kFALSE - hist. for each run
    Int_t              eventCounter;           // number of calculated events 
    Int_t              numEventsSave;          // save canv. each 
                                               // "numEventsSave" events
    Int_t              numRunEvents;           // number of calculated events in one run 
    Int_t              numNRunsEvents;         // number of calculated events in collectNRuns runs

    // collecting of runs:
    Int_t              collectNRuns;           // collect and save 
                                               // each "collectNRuns" runs
    Int_t              runsCounter;            // counter of runs
    TString            firstRunName;           // fileName of the first run
    TString            lastRunName;            // fileName of the last run
    TH1F*              dPhiForNR[6];           // [6] - combinations of MDC,s:
    TH1F*              r0ForNR[6];             // 1-2,1-3,1-4,2-3,2-4,3-4
    TH1F*              zVerForNR[6][6];        // [sec][comb]
    TH1F*              zVerDscForNR[6][6][10]; // [s][c][Downsc.fl.|| trig.Cond]
    TH2F*              zVerVsEventForNR[6][6]; // -/-
    TCanvas*           canvsForNR[6];          // pointers to canvases
    Char_t             cnCombForNR[6][100];    // canvases names
    Char_t             ctCombForNR[6][100];    // canvases titles
    TPad*              fPadsForNR[6][12];      // [comb][pad]
    
    // Histogram tracks_in_target vs run 
    Bool_t             drawInOutTracks;        // =kTRUE - do this hist.
    Int_t              nRunsDraw;              // num.of runs in hist.
    Double_t           firstTarPnt[6];         // [sec] Z region of target
    Double_t           lastTarPnt[6];          // [sec] ...
    Double_t*          inTargetArr;            // n.tracks in target region
    Double_t*          allTargetArr;           // totale num. of tracks
    Int_t              inOutTargCurrRun;       // counter of runs
    TH1F*              inOutTargetHist;        // histogram
    
    TString            pathHistOneRun;         // path for histogram
    TString            pathHistSumRun;         // path for histogram
    
    Bool_t             fillSeg;                // kTRUE - fill HMdcSeg category
    HCategory*         fSegCat;                // HMdcSeg category
    HLocation          locSeg;                 // location for HMdcSeg object
    
    TCanvas           *currCanvas;             // current canvas
    TPad              *currPad;                // current pad
    Int_t              pad;                    // current pad number
    Int_t              comb;                   // current combination
  public:
    HMdcTargetMon(Bool_t isClOff=kFALSE, UInt_t pt=0);
    HMdcTargetMon(const Text_t *name,const Text_t *title, Bool_t isClOff=kFALSE,
        UInt_t pt=0);
    ~HMdcTargetMon(void);
    virtual Int_t execute(void);
    virtual Bool_t init(void);
    virtual Bool_t reinit(void);
    virtual Bool_t finalize(void);
    
    void   drawCanvases(void);
    void   drawCanvasesRunsSum(void);
    void   setMode(Bool_t m,const Char_t* fileType=0);
    void   setHistPar(Int_t nb, Double_t z1, Double_t z2);
    void   setTargPos(Double_t xt, Double_t yt, Double_t zt,Int_t sec=-1);
    void   setTimeStep(Double_t tst)                {timeStep  = (tst<3.) ? 3.:tst;}
    void   setHistsMode(Int_t hMode)                {histsMode = hMode;}
    void   drawDownScalHist(void)                   {histsMode = 1;}
    void   drawTrigCondHist(void)                   {histsMode = 2;}
    void   setFillSegFlag(Bool_t f=kTRUE)           {fillSeg = f;}
    void   setRCut(Double_t rCut)                   {r0Cut = rCut;}
    void   setDPhiCut(Double_t dPh)                 {dPhiCut = dPh;}
    void   setDrawEachFile(void)                    {sumOfFiles = kFALSE;}
    void   setDrawSumOfFiles(void)                  {sumOfFiles = kTRUE;}
    void   setEachNEventsSave(Int_t nev)            {numEventsSave = nev;}
    void   setCollectNRuns(Int_t nf)                {collectNRuns = nf;}
    void   addToCanvasName(const Char_t* add)       {addCnName = add;}
    TH1F*  setTargetZRegion(Double_t fpoint,Double_t lpoint,Int_t nRuns=100,Int_t sec=-1);
    void   setCanvasSize(Int_t ww, Int_t wh);
    void   setPathOneRunHits(const Char_t* p)       {pathHistOneRun = p;}
    void   setPathSumRunsHits(const Char_t* p)      {pathHistSumRun = p;}
    void   setConvertFormat(const Char_t* p,Bool_t delEps=kFALSE);
    void   selectEventWithTBits(Int_t tb)           {selectEvByTBits = tb;}
    void   doNormalization(Bool_t fl=kTRUE)         {normalizeHists = fl;}

    TH1F** getDPhiH(void)                           {return dPhiH;}          
    TH1F** getZ0H(void)                             {return r0H;}
    TH1F** getZVerH(void)                           {return zVerH[0];}       
    TH1F** getZVerDscH(void)                        {return zVerDscH[0][0];}
    TH2F** getZVerVsEventH(void)                    {return zVerVsEventH[0];}
    
  private:
    void   setDefParam(UInt_t pt=0);
    Bool_t initHists(void);
    void   initRunsSumHists(void);
    void   extractFileInfo(void);
    void   fillDateTime(void);
    void   setCanvName(void);
    void   setCanvNameForNR(void);
    Int_t  eventTime(void);
    void   saveCanvases(void);
    void   saveCanvasesForNR(void);
    void   saveCanvas(TCanvas* canv,TString& path);
    void   resetHists(void);
    void   resetHistsRunsSum(void);
    void   makePDF(const Char_t* psFile);
    void   addHistsToRunsSum(void);
    void   shiftArrays(void);
    void   fillInOutTargetHist(void);
    void   fillInOutTarHistBin(Int_t bin);
    void   setEntriesInOutTHist(void);
    void   drawPad(Double_t x1,Double_t y1,Double_t x2,Double_t y2);
    void   drawPadS(Double_t x1,Double_t y1,Double_t x2,Double_t y2);

  ClassDef(HMdcTargetMon,0)
};

#endif
