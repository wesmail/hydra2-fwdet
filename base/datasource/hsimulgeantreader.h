#ifndef HSIMULGEANTREADER_H
#define HSIMULGEANTREADER_H

#include "TTree.h"
#include "hgeantreader.h"
#include "hrecevent.h"
class HLocation;

class HSimulGeantReader : public HGeantReader {
private:
  Int_t fEventId;          //  Current event number
  TTree* t;                //  Pointer to the root tree
  HRecEvent* fSimEv;       //! HGeant input data

public:
  HSimulGeantReader(void);
  ~HSimulGeantReader(void);
  Bool_t init(void); 
  Bool_t execute(void); 

  ClassDef(HSimulGeantReader,1) // Simul reader for HGeant root file
};

#endif /* !HSIMULGEANTREADER_H */




