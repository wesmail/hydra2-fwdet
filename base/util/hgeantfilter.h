#ifndef HGEANTFILTER
#define HGEANTFILTER

#include "hevent.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hreconstructor.h"
#include "hfilter.h"
#include <stdio.h>

class HEventHeader;
class HHitFilter;

class HGeantFilter : public HReconstructor {
 private:
  Int_t particleId;
  Float_t accepted;

  HCategory* catKine;        //!
  HCategory* catMdc;         //!
  HCategory* catTof;         //!
  HCategory* catShower;      //!
  HCategory* catRichPhoton;  //!
  HCategory* catRichDirect;  //!
  HCategory* catRichMirror;  //!
  HGeantFilter(HGeantFilter &filter);
 public:
  HGeantFilter(Text_t *name,Text_t *title, Int_t id, Float_t acc);
  ~HGeantFilter(void);
  void setID(Int_t id) {particleId=id;}
  void setAcceptance(Float_t acc) {accepted=acc;}
  Int_t execute(void);
  Bool_t init(void);
  Bool_t reinit(void);
  Bool_t finalize(void);
  ClassDef(HGeantFilter,1) // Filter HGeant-generated input  
};


class HHitFilter : public HFilter {
 private:
  HCategory* pKineCat;

 public:
  HHitFilter(HCategory* pCat=NULL);
  ~HHitFilter(void);
  void setKine(HCategory* pCat) {pKineCat = pCat;}
  virtual Bool_t check(TObject *obj);
  ClassDef(HHitFilter,1) // HGeant hit filter
};

#endif
