#ifndef HTOFGEANTREADER_H
#define HTOFGEANTREADER_H

#include "TTree.h"
#include "hgeantreader.h"
#include "hmatrixcategory.h"
class HGeantTof;
class HLocation;

class HTofGeantReader : public HGeantReader {
private:
  TTree* t;                        //  Pointer to the root tree
  HMatrixCategory* fGeantTofCat;   //! TOF HGeant input data
  Int_t fEventId;                  //! event counter
public:
  HTofGeantReader(void);
  ~HTofGeantReader(void);
  Bool_t init(void); 
  Bool_t execute(void); 
  inline HMatrixCategory* getGeantTofCat(void) {return fGeantTofCat;}
  inline HGeantTof* getGeantTof(HLocation locate) {
     return (HGeantTof*)(fGeantTofCat->getObject(locate));
  }
  ClassDef(HTofGeantReader,1) // TOF reader for HGeant Root file
};

#endif /* !HTOFGEANTREADER_H */




