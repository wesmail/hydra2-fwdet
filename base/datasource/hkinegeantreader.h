#ifndef HKINEGEANTREADER_H
#define HKINEGEANTREADER_H

#include "TTree.h"
#include "hgeantreader.h"
#include "hlinearcategory.h"
class HGeantKine;
class HLocation;

class HKineGeantReader : public HGeantReader {
private:
  Int_t fEventId;                   //  Current event number
  TTree* t;                         //  Pointer to the root tree
  HLinearCategory* fGeantKineCat;   //! KINE HGeant input data

public:
  HKineGeantReader(void);
  ~HKineGeantReader(void);
  Bool_t init(void); 
  Bool_t execute(void); 
  inline HLinearCategory* getGeantKineCat(void) {return fGeantKineCat;}
  inline HGeantKine* getGeantKine(HLocation locate) {
     return (HGeantKine*)(fGeantKineCat->getObject(locate));
  }
  ClassDef(HKineGeantReader,1) // KINE reader for HGeant root file
};

#endif /* !HKINEGEANTREADER_H */




