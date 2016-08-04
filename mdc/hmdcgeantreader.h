#ifndef HMDCGEANTREADER_H
#define HMDCGEANTREADER_H

#include "TTree.h"
#include "hgeantreader.h"
#include "hmatrixcategory.h"
class HGeantMdc;
class HLocation;

class HMdcGeantReader : public HGeantReader {
private:
  TTree* t;                        //  Pointer to the root tree
  HMatrixCategory* fGeantMdcCat;   //! MDC HGeant input data
  Int_t fEventId;                  //! event counter
public:
  HMdcGeantReader(void);
  ~HMdcGeantReader(void);
  Bool_t init(void); 
  Bool_t execute(void); 
  inline HMatrixCategory* getGeantMdcCat(void) {return fGeantMdcCat;}
  inline HGeantMdc* getGeantMdc(HLocation locate) {
     return (HGeantMdc*)(fGeantMdcCat->getObject(locate));
  }
  ClassDef(HMdcGeantReader,1) // MDC reader for HGeant Root file
};

#endif /* !HMDCGEANTREADER_H */




