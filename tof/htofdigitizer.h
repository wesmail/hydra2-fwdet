#ifndef HTOFDIGITIZER_H
#define HTOFDIGITIZER_H
using namespace std;
#include "hreconstructor.h"
#include <iostream> 
#include <iomanip>
#include "hlocation.h"
#include "hgeanttof.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TString.h"
class TF1;
class HIterator;
class HCategory;
class HLinearCategory;
class HTofDigiPar;
class HTofCalPar;
class HTofWalkPar;


class HTofDigitizer : public HReconstructor {

private:
  HLocation fLoc;                       //! Location for new object
  HCategory* fGeantCat;                 //! Pointer to Geant data category
  HLinearCategory* fGeantKineCat;       //! Pointer to GeantKine data category
  HCategory* fRawCat;                   //! Pointer to raw data category
  HCategory* fRawCatTmp;                //! Pointer to tmp raw data category
  HTofDigiPar *fDigiPar;                //! Digitization parameters
  HTofWalkPar *fTofWalkPar;             //! walk correction parameters
  HTofCalPar *fTofCalPar;               //! cal parameters
  HIterator* iterGeant;                 //! Iterator over GeantCell category
  HIterator* iterTofRaw;                //! Iterator over HTofRawSim category
  HIterator* iterTofRawTmp;             //! Iterator over tmp HTofRawSim category
  static Float_t timeResZero;           //! hardwired resolotion of tof (240 ps)
  //--------------------------------------------------------------------------
  Int_t storeFirstTrack;         //! flag:
                                 //        0  = realistic (secondaries included)
                                 //        1 primary particle is stored
                                 //        2 (default) the track number entering the tof in SAME SECTOR is stored , not the
                                 //          secondaries created in TOF itself (delta electrons etc)
                                 //        3 as 2 but in SAME MODULE
                                 //        4 as 2 but condition on SAME ROD
  Bool_t debug;                  //! change stored tracknumbers (when storeFirstTrack!=0)
  TNtuple* out;                  //! ntuple pointer
  TFile*   outFile;              //! output file pointer
  static HTofDigitizer* pTofDigi;//! pointer to this
  Bool_t useOld;                 //! flag: kTRUE:use old execute function (before embedding)

#define MAXCHRGCH    10000
  //--------------------------------------------------------------------------

  Int_t findFirstHitInTof(HGeantTof* pOld,HGeantTof** pNew,Int_t* count);
  void  fillNtuple       (HGeantTof* pOld,HGeantTof*  pNew,Int_t count);
  void  fillArray();
  void  doFinalCheckOnArray();
  void  fillOutput();
public:
  HTofDigitizer(const Text_t* name="HTofDigitizer",const Text_t* title="HTofDigitizer");
  ~HTofDigitizer(void);
  Bool_t initParContainer();
  Bool_t init(void);
  Bool_t finalize(void);
  Int_t  execute(void);
  Int_t  executeOld(void);
  static HTofDigitizer* getTofDigtizer(){return pTofDigi;}
  void   setStoreFirstTrack(Int_t flag) {storeFirstTrack=flag;}
  void   setDebug(Bool_t flag)          {debug=flag;}
  void   setOutputFile(TString outname="");
  void   setUseOld(Bool_t flag)         {useOld=flag;}
  static void setTimeResolution(Float_t res=0.240) {timeResZero=res;}
public:
  ClassDef(HTofDigitizer,0) //Digitizer of TOF data.
};

#endif









