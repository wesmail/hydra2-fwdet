#ifndef HSTART2GEOMPAR_H
#define HSTART2GEOMPAR_H

#include "hdetgeompar.h"
#include "TObjArray.h"

class HGeomShapes;

class HStart2GeomPar : public HDetGeomPar {
private:
  Int_t      maxComponents; // maximum number of Geant diamonds
  Int_t      maxCells;      // maximum number of cells/strips
  TObjArray* labPos;        // lab position of cell center
public:
  HStart2GeomPar(const Char_t* name="Start2GeomPar",
                 const Char_t* title="Start2 geometry parameters",
                 const Char_t* context="GeomProduction");
  ~HStart2GeomPar();
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  Int_t getModNumInMod(const TString&)  {return 0;}
  Int_t getModNumInComp(const TString&) {return 0;}
  Int_t getMaxComponents(void)          {return maxComponents;}
  Int_t getMaxCells(void)               {return maxCells;}
  inline Int_t getCompNum(const TString&);
  Int_t getCellIndex(Int_t compNum, Float_t xPos, Float_t yPos);  
  inline const HGeomVector* getLabPos(Int_t n) const ;
  void calcLabPositions(void);
  void printLabPositions(void);
  ClassDef(HStart2GeomPar,1) // Container for the Start geometry parameters
};

inline Int_t HStart2GeomPar::getCompNum(const TString& name) {
  // returns the cell index retrieved from the component name VSTDx
  return ((Int_t)(name[4]-'0')-1);
}  

inline const HGeomVector*  HStart2GeomPar::getLabPos(Int_t n) const {
  // returns the lab position of a Start cell/strip
  if (n>=0 && n<maxCells) return (HGeomVector*)(labPos->At(n));
  return NULL;
}

#endif /* !HSTART2GEOMPAR_H */
