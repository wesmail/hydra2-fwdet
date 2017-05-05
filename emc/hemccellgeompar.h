#ifndef HEMCCELLGEOMPAR_H
#define HEMCCELLGEOMPAR_H

#include "hparset.h"
#include "TObjArray.h"
#include "hgeomtransform.h"

class HEmcDetector;
class HEmcGeomPar;
class HGeomVector;

class HEmcCellGeomPar : public HParSet {
protected:
  TObjArray*    labPos;    // lab position of cell center
  HEmcDetector* pDet;      // pointer to EMC detector
  HEmcGeomPar*  pGeomPar;  // pointer to EMC geometry detector
  Int_t         nMaxCells; // maximum number of cells in one module
  HGeomTransform trMod2Lab[6]; // transformation lab.sys. <==> module sys. for 6 sectors 
public:
  HEmcCellGeomPar(const Char_t* name="EmcCellGeomPar",
                  const Char_t* title="Emc cell geometry parameters",
                  const Char_t* context="GeomProduction");
  ~HEmcCellGeomPar(void);

  void   clear(void);
  Bool_t init(HParIo*,Int_t*);
  Int_t  getSize(void) {return labPos->GetEntries();}
  void   printParam(void);

  const HGeomVector*  getLabPos(Int_t,Int_t) const ;
  const HGeomVector*  getLabPos(Int_t,Int_t,Int_t) const ;
  
  const HEmcDetector*   getEmcDetector(void) const     {return pDet;}
  const HGeomTransform* getTransMod2Lab(Int_t s) const {return s>=0&&s<6 ? &trMod2Lab[s] : NULL;}

  ClassDef(HEmcCellGeomPar,0) // Class for EMC cell geometry
};

#endif /* !HEMCCELLGEOMPAR_H */
