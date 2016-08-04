#ifndef HSHOWERGEOMETRY_H
#define HSHOWERGEOMETRY_H

#include "hparcond.h"
#include "hlocation.h"
#include "hshowerpad.h"
#include "hshowerwiretab.h"
#include "hshowerframe.h"
#include "hgeomtransform.h"
#include "hgeomvector2.h"

class HShowerGeomPar;

class HShowerGeometry: public HParCond {
private:
  Int_t nSectors; //! number of sectors
  Int_t nModules; //! number of modules
  Int_t nRows;    //! number of rows
  Int_t nColumns; //! number of columns

  HGeomTransform sectorTransform[6];  //! lab coordinates of module 0 wire planes
  HGeomTransform moduleTransform[18]; //! lab coordinates of pad planes

  HShowerFrame frames[3];   //! definitions of local coordinates of frames
  HShowerWireTab wires[3];  // definitions of local coordinates of wires
  HShowerPadTab pads[3];    // definitions of local coordinates of pads
  HShowerGeomPar* pGeomPar; //! pointer to ShowerGeomPar parameter container
public:
  HShowerGeometry(const Char_t* name="ShowerGeometry",
                  const Char_t* title="Shower geometry parameters",
                  const Char_t* context="GeomProduction");
  ~HShowerGeometry();

  Bool_t init(HParIo*,Int_t*);
  void   clear();
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);
  Bool_t fillFromGeomPar();
  void printParams();

  Int_t getSectors() {return nSectors;}
  Int_t getModules() {return nModules;}
  Int_t getRows()    {return nRows;}
  Int_t getColumns() {return nColumns;}

  HShowerFrame*   getFrame(Int_t nMod)     {return &frames[nMod];}
  HShowerWireTab* getWireTab(Int_t nMod)   {return &wires[nMod];}
  HShowerPadTab*   getPadParam(Int_t nMod) {return &pads[nMod];}

  void getLocalCoord(HLocation& loc, HGeomVector& v);
  void getLabCoord(HLocation& loc, HGeomVector2& v);
  void getSphereCoord(HLocation& loc, HGeomVector2& v, HGeomVector *pvTarget = NULL);
  void transLabToSphereCoord(HGeomVector2& labIn, HGeomVector2& sphereOut, HGeomVector *pvTarget);
  void transVectToLab(HLocation& loc, HGeomVector vLocal, HGeomVector2& vLab); 

  const HGeomTransform& getTransform(Int_t sec) {
    return sectorTransform[sec];
  }
  const HGeomTransform& getTransform(Int_t sec, Int_t mod) {
    return moduleTransform[sec*nModules+mod];
  }

  Bool_t transPadToLab();

  ClassDef(HShowerGeometry,3)//Class for PreShower geometry container
};

#endif /* !HSHOWERGEOMETRY_H */
