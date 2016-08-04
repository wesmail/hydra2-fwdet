#ifndef HGEANTSOURCE_H
#define HGEANTSOURCE_H

#include "hdatasource.h"
#include "TList.h"
#include "THashTable.h"

class HGeantReader;

class HGeantSource : public HDataSource {
private:
  TList fReaderList; //List of active geant readers.
  THashTable fFileTable; //Hash table with input files
public:
  HGeantSource(void);
  HGeantSource(HGeantSource &s);
  ~HGeantSource(void);
  Bool_t addGeantReader(HGeantReader *r,const Text_t *inputFile);
  EDsState getNextEvent(void);
  Bool_t init(void);
  Bool_t reinit(void) {return kTRUE; }
  Bool_t finalize(void);
  Int_t getCurrentRunId(void) {return 0;}
  Int_t getCurrentRefId(void) {return 0;} 
  Text_t const *getCurrentFileName(void) {return "";}
  ClassDef(HGeantSource,1) //Data source to read GEANT ouput
};
#endif /* !HGEANTSOURCE_H */
