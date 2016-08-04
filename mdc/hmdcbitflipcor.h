#ifndef HMDCBITFLIPCOR_H
#define HMDCBITFLIPCOR_H

#include "hreconstructor.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"

class TFile;
class TLine;
class HMdcRaw;
class HMdcRawCor;
class HEvent;
class Hades;

class HRuntimeDb;
class HCategory;
class HEventHeader;
class HMdcRawEventHeader;
class HIterator;
class HMdcGeomPar;
class HSpecGeomPar;
class HMdcDetector;
class HMdcBitFlipCorPar;


class HMdcBitFlipCor : public HReconstructor  {
  protected:
  HRuntimeDb*          rtdb; 
  HCategory*           catmHMdcRaw;   //! HMdcSeg Category
  HMdcRaw*             mdcraw;         //! seg data level
  HIterator*           mdcrawiter;     //! seg iterator
  HCategory*           catmHMdcRawEventHeader;   //! HMdcSeg Category
  HMdcRawEventHeader*  rawREH;         //! seg data level
  HIterator*           iterREH;     //! seg iterator
 
  HCategory* rawcorCat;       //! pointer to the raw data
  HMdcRawCor* rawcor;            //! pointer to raw data word
  HLocation loc;           //! location for new object.
  
  HMdcBitFlipCorPar* fMdcBitFlipCorPar; 

  TFile*  rootoutputfile;  
  TString rootoutputfilename;

  FILE*  asciioutputfile;
  TString asciioutputfilename;

  Char_t title[300];          //! histrogram title
  Char_t name[300];           //! histogram name
  Char_t xtitle[300];         //! histogram xaxis title
  Char_t ytitle[300];         //! histogram yaxis title
  Char_t ztitle[300];         //! histogram zaxis title
 
  TH1F*    histtime1b[6][4][16];
  TH1F*    histtime1g[6][4][16];
  TH1F*    histtime2b[6][4][16];
  TH1F*    histtime2g[6][4][16];

  Int_t sector;
  Int_t module;
  Int_t mbo;
  Int_t tdc;
  Int_t time1;
  Int_t time2;
  Int_t trigtype;
  Int_t paramsbitcorrection[6][4][16][9];
  Int_t phist;
  Int_t prawcor;

  Int_t counterevents[6][4][16];
  Int_t countersavedtime1[6][4][16];
  Int_t countersavedtime2[6][4][16];
  Int_t ratiosaved;

 void resetVar()
   {
     sector=-1;
     module=-1;
     mbo=-1;
     tdc=-1;
     time1=-1;
     time2=-1;
     trigtype=-1;
    }


  private:
 
 TFile* openHistFile(const Char_t* flag);

  public:

    HMdcBitFlipCor(const Text_t *name,const Text_t *title, Int_t PHist=0, Int_t PRawCor=0);
    HMdcBitFlipCor(void);
    ~HMdcBitFlipCor(void);

    Bool_t init(void);
    Bool_t reinit(void);
    Bool_t finalize(void);
    Int_t  execute(void);
    
    void setRootOutputFilename(TString);
    void setAsciiOutputFilename(TString);
    void getParameterContainers();
    void createHists();
    void initVariables();
    Bool_t fparamsbitcorrection();
 
    ClassDef(HMdcBitFlipCor,0)
};

#endif
