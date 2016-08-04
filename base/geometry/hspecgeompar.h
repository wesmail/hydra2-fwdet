#ifndef HSPECGEOMPAR_H
#define HSPECGEOMPAR_H

#include "hparset.h"
#include "TNamed.h"
#include "TObjArray.h"

class HGeomVolume;
class HGeomShapes;

class HSpecGeomPar :  public HParSet {
protected:
  HGeomVolume* cave;   // volume describing the cave
  TObjArray* sectors;  // array of sector volumes (type HGeomVolume)
  TObjArray* targets;  // array of target volumes (type HGeomVolume)
  HGeomShapes* shapes; //! pointer to the shape classes
  Bool_t isFirstInit;  //! kTRUE before first initialization
public:
  HSpecGeomPar(const Char_t* name="SpecGeomPar",
               const Char_t* title="Geometry of cave, sectors and target",
               const Char_t* context="GeomProduction",
               const Int_t maxSec=6);
  ~HSpecGeomPar(void);
  Int_t getNumSectors(void);
  Int_t getNumTargets(void);
  HGeomVolume* getCave(void) {return cave;}
  HGeomVolume* getSector(const Int_t n);
  HGeomVolume* getTarget(const Int_t n);
  Bool_t isFirstInitialization() {return isFirstInit;}
  void setNotFirstInit() {isFirstInit=kFALSE;}
  void addCave(HGeomVolume* v);
  void createTargets(const Int_t);
  void addTarget(HGeomVolume*);
  void addSector(HGeomVolume*);
  Bool_t init(HParIo*);
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  void clear(void);
  void printParam(void); 
  Int_t getSectorIndex(const Text_t*);
  Int_t getTargetIndex(const Text_t*);
  HGeomShapes* getShapes(void) {return shapes;}
  ClassDef(HSpecGeomPar,1) // geometry container for cave, sectors, targets
};

#endif /* !HSPECGEOMPAR_H */
