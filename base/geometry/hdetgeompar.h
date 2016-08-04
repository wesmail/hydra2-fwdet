#ifndef HDETGEOMPAR_H
#define HDETGEOMPAR_H

#include "hgeomtransform.h"
#include "hparset.h"
#include "TNamed.h"
#include "TObjArray.h"

class HGeomCompositeVolume;
class HGeomShapes;

class HModGeomPar :  public TNamed {
protected:
  TString refName;                 // name of the reference module volume
  HGeomCompositeVolume* refVolume; //! pointer to the reference module volume
  HGeomTransform transform;        // lab transformation
public:
  HModGeomPar() {refVolume=0;}
  ~HModGeomPar() {refVolume=0;}
  HGeomCompositeVolume* getRefVolume() {return refVolume;} 
  HGeomTransform& getLabTransform() {return transform;}
  const Text_t* getRefName() const { return refName; }
  void setRefName(const Text_t* s);
  void clear();
  void print();
  void setVolume(HGeomCompositeVolume* p) {refVolume=p;}
  ClassDef(HModGeomPar,1) // Module level of the basic geometry container
};

class HDetGeomPar : public HParSet {
protected:
  TObjArray* modules;    // linear array of all modules (type HModGeomPar)
  TObjArray* refVolumes; // array of module types (type HGeomCompositeVolume)
  Int_t maxSectors;      // maximum number of sectors
  Int_t maxModules;      // maximum number of modules in a sector
  Int_t numComponents;   // number of components in a sector 
  HGeomShapes* shapes;   //! pointer to the shape classes
  Bool_t isFirstInit;    //! kTRUE before first initialization
public:
  HDetGeomPar(const Char_t* name="",const Char_t* title="",const Char_t* context="",
              const Char_t* detectorName="");
  virtual ~HDetGeomPar();
  Int_t getNumModules();
  Int_t getNumRefModules();
  Int_t getNumComponents() {return numComponents;}
  Int_t getMaxSectors() {return maxSectors;}  
  Int_t getMaxModules() {return maxModules;}
  Bool_t isFirstInitialization() {return isFirstInit;}
  void setNotFirstInit() {isFirstInit=kFALSE;}  
  HModGeomPar* getModule(const Int_t,const Int_t);
  HModGeomPar* getModule(const Int_t);
  void getSector(TObjArray*,const Int_t);
  HGeomCompositeVolume* getRefVolume(const Int_t);
  void addRefVolume(HGeomCompositeVolume*,const Int_t);
  virtual Bool_t init(HParIo*,Int_t*) {return kFALSE;}
  virtual Int_t write(HParIo*) {return kFALSE;}
  virtual void clear();
  virtual void printParam(); 
  virtual Int_t getSecNum(const TString&) {return -1;}
  virtual Int_t getModNumInMod(const TString&) {return -1;}
  virtual Int_t getModNumInComp(const TString&) {return -1;}
  virtual Int_t getCompNum(const TString&) {return -1;}
  HGeomShapes* getShapes() {return shapes;}
  ClassDef(HDetGeomPar,2) // basic geometry container of a detector
};

#endif /* !HDETGEOMPAR_H */

