#ifndef HSHOWGEANTREADER_H
#define HSHOWGEANTREADER_H

#include "TTree.h"
#include "hgeantreader.h"
#include "hmatrixcategory.h"
class HGeantShower;
class HLocation;

class HShowerGeantReader : public HGeantReader {
private:
  TTree* t;                         //  Pointer to the root tree
  HMatrixCategory* fGeantShowCat;   //! SHOWER HGeant input data
  Int_t fEventId;                   //! event counter
public:
  HShowerGeantReader(void);
  ~HShowerGeantReader(void);
  Bool_t init(void); 
  Bool_t execute(void); 
  inline HMatrixCategory* getGeantShowerCat(void) {return fGeantShowCat;}
  inline HGeantShower* getGeantShower(HLocation locate) {
     return (HGeantShower*)(fGeantShowCat->getObject(locate));
  }
  ClassDef(HShowerGeantReader,0) // SHOWER reader for HGeant Root file
};

#endif /* !HSHOWGEANTREADER_H */




