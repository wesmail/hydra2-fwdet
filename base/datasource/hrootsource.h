#ifndef HROOTSOURCE_H
#define HROOTSOURCE_H

#include "hdatasource.h"
#include "haddef.h"
#include "hgeantdef.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include <map>
#include "TString.h"
#include "TEventList.h"

class HRootSource : public HDataSource {
protected:
  TChain *fInput;         //TTree to be read.
  TString fDirectory;
  Stat_t fEntries;
  Bool_t fPersistency;
  Bool_t fMerge;
  Int_t fCursor;          //Number of next event.
  Int_t fSplitLevel;      //Split level of input tree
  Int_t fCurrentRunId;
  Int_t fCurrentRefId;
  Int_t fGlobalRefId;
  std::map<Int_t,Int_t> fRefIds;//!
  HEvent *fEventInFile;   //! Pointer to the event structure in file
  TEventList *fEventList;
  Int_t fLastRunId;  //! last run number (needed to detect switch to new run)
  Bool_t overwriteVersion; //! flag for modifying version
  Int_t replaceVersion;    //! value for replacing version is overwriteVersion=kTRUE

  Bool_t fileExists(const TString &name);
  virtual TFile * getFile(TString name);
  virtual TString getFileName(const Text_t file[]);
public:
  HRootSource(Bool_t fPersistent=kTRUE, Bool_t fMerge=kFALSE);
  ~HRootSource(void);
  void setEventList(TEventList *el) { fEventList = el; }
  virtual EDsState getNextEvent(Bool_t doUnpack=kTRUE);
  void setCursorToPreviousEvent();
  virtual EDsState skipEvents(Int_t nEv);
  Bool_t rewind() {fCursor=0; return kTRUE;}
  Bool_t init(void);
  Bool_t reinit(void) {return kTRUE; }
  Bool_t finalize(void) {return kTRUE;}
  Int_t getCurrentRunId(void) {return fCurrentRunId;}
  Int_t getCurrentRefId(void) {return fCurrentRefId;}
  Int_t getGlobalRefId(void) {return fGlobalRefId;}
  void  setCurrentRunId(Int_t id) {fCurrentRunId = id;}
  void setRefId(Int_t runId, Int_t refId) {
     fRefIds[runId]=refId;
     if(runId==fCurrentRunId) fCurrentRefId = refId;
  }
  void setGlobalRefId (Int_t refId) {
     fGlobalRefId=refId;
     if(fCurrentRefId==-1) fCurrentRefId = fGlobalRefId; 
  }
  Text_t const *getCurrentFileName(void);
  virtual Bool_t getEvent(Int_t eventN);
  void setDirectory(const Text_t dirName[]);
  Bool_t addFile(const Text_t file[]);
  Bool_t setInput(const Text_t *fileName,const Text_t *treeName);
  Bool_t disableCategory(Cat_t aCat);
  void deactivateBranch(const Text_t *branchName);
  Bool_t disablePartialEvent(Cat_t aCat);
  TTree *getTree(void);
  TChain* getChain(){return fInput;}
  Int_t getSplitLevel(void);
  virtual void Clear(void);
  void Print() {
    HDataSource::Print();
    if(fInput) {fInput->GetEntries(); fInput->ls();}
  } 
  void replaceHeaderVersion(Int_t vers,Bool_t replace=kTRUE){replaceVersion=vers;overwriteVersion=replace;}
  ClassDef(HRootSource,1) //Data source to read Root files.
};

#endif /* !HROOTSOURCE_H */


