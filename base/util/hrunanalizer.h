#ifndef HRUNANALIZER
#define HRUNANALIZER

#include "hreconstructor.h"
#include <stdio.h>

class HEventHeader;

class HRunAnalizer : public HReconstructor {
 private:
  HEventHeader *fEventHeader; //Current event's header.
  Int_t fTotEvents; //Total number of events (excluding start/stop event)
  Int_t fCalEvents; //Number of calibration events
  Bool_t fFirstRun;    //True if no run was analyzed before.
  FILE *fOut; // Output file.
  Bool_t fFirstEvent; //True if first event in run.
  UInt_t fRunStartDate,fRunStartTime;   //Run start time
  UInt_t fRunStopDate,fRunStopTime;   //Run start time
  UInt_t fRunId;      //Run Id
  TString fFileName; 
  void extractStartInfo(void);
  void writeInfo(void);
  void writeHeader(void);
  HRunAnalizer(HRunAnalizer &analizer);
 public:
  HRunAnalizer(Text_t *name,Text_t *title,Text_t *fileName=0);
  ~HRunAnalizer(void);
  Int_t execute(void);
  Bool_t init(void);
  Bool_t reinit(void);
  Bool_t finalize(void);
  ClassDef(HRunAnalizer,0) //Extract info from run files;
};

#endif
