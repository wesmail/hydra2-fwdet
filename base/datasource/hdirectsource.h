#ifndef HDIRECTSOURCE_H
#define HDIRECTSOURCE_H

#include "hdatasource.h"
#include "TList.h"

class HGeantReader;

class HDirectSource : public HDataSource {
private:
  TList fReaderList; //List of active Geant readers.
  Int_t fCurrentRunId;
public:
  HDirectSource(void);
  HDirectSource(HDirectSource &s);
  ~HDirectSource(void);
  Bool_t addGeantReader(HGeantReader *r);
  EDsState getNextEvent(void);
  Bool_t init(void);
  Bool_t finalize(void);
  void setCurrentRunId(Int_t Id) {fCurrentRunId = Id;}
  Int_t getCurrentRunId(void) {return fCurrentRunId;}
  Int_t getCurrentRefId(void) {return fCurrentRunId;}        
  Text_t const *getCurrentFileName(void) {return "";}
  ClassDef(HDirectSource,1) // Data source to operate under HGeant control
};
#endif /* !HDIRECTSOURCE_H */
