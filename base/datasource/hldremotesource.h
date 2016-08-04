#ifndef HLDREMOTESOURCE_H
#define HLDREMOTESOURCE_H


#include "hldsource.h"
#include "TString.h"

class HldRemoteSource : public HldSource {

protected:
  Int_t    refId;     // Reference run id for initialization
  TString  currNodeName;
  TString  fileName;  // dummy filename composed from time
  Int_t    runId;     // store runId to compare in next event
  TIterator * iter;
public:
  HldRemoteSource(void);
  HldRemoteSource(const Text_t *nodeName);
  ~HldRemoteSource(void);
   Bool_t init();  
   Int_t getCurrentRunId(void);
   Int_t getCurrentRefId(void) {return refId;}
   void setRefId(Int_t r) {refId=r;}
   EDsState getNextEvent(Bool_t doUnpack=kTRUE);
   Text_t const *getCurrentFileName(void) {return fileName.Data();}
   Text_t* getNodeName(){return (Text_t*)currNodeName.Data();}

  ClassDef(HldRemoteSource,0) //Data source to read rpc buffers;
};
#endif /* !HLDREMOTESOURCE_H */
