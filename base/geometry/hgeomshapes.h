#ifndef HGEOMSHAPES_H
#define HGEOMSHAPES_H
using namespace std;
#include <iostream>
#include <iomanip>

#include "hgeomtransform.h"
#include "TObject.h"
#include "TString.h"
#include "TList.h"
#include <fstream>

class HGeomVolume;
class HGeomBasicShape;

class  HGeomShapes : public TObject {
protected:
  TList* shapes;  // list of already created shape classes
public :
  HGeomShapes();
  ~HGeomShapes();
  HGeomBasicShape* selectShape(HGeomVolume *);  
  HGeomBasicShape* selectShape(const TString&);  
  Int_t readPoints(fstream*,HGeomVolume*);
  Bool_t writePoints(fstream*,HGeomVolume*);
  void printPoints(HGeomVolume* volu);
  ClassDef(HGeomShapes,0) // manager class for geometry shapes
};

#endif  /* !HGEOMSHAPES_H */
