#ifndef HGEOMVOLUME_H
#define HGEOMVOLUME_H

#include "hgeomtransform.h"
#include "TNamed.h"
#include "TObjArray.h"

class HGeomVolume : public TNamed {
protected:
  TString shape;            // Geant shape of the volume
  TString mother;           // name of the mother volume
  TObjArray* points;        // array of points (type HGeomVector)
  HGeomTransform transform; // transformation relative to the mother system
  Int_t nPoints;              // number of points
public:
  inline HGeomVolume();
  HGeomVolume(HGeomVolume&);
  inline virtual ~HGeomVolume();
  const TString& getName() const { return fName; }
  const TString& getShape() const { return shape; }
  const TString& getMother() const { return mother; }
  HGeomTransform& getTransform() { return transform; }
  Int_t getNumPoints() { return nPoints;}
  inline HGeomVector* getPoint(const Int_t n);
  inline void setName(const Text_t* s);
  void setVolumePar(HGeomVolume&);
  inline void setShape(const Text_t* s);
  inline void setMother(const Text_t* s);
  void createPoints(const Int_t);
  void setPoint(const Int_t,const Double_t,const Double_t,const Double_t);
  void setPoint(const Int_t,const HGeomVector&);
  virtual void clear();
  virtual void print();
  ClassDef(HGeomVolume,1) // basic geometry parameters of a volume
};

// -------------------- inlines --------------------------

inline HGeomVolume::HGeomVolume() {
  nPoints=0;
  points=0;
}

inline HGeomVolume::~HGeomVolume() {
  if (points) {
    points->Delete();
    delete points;
    points=0;
  }
}

inline HGeomVector* HGeomVolume::getPoint(const Int_t n) {
   if (points && n<nPoints) return (HGeomVector*)points->At(n);
   else return 0;
}

inline void HGeomVolume::setName(const Text_t* s) {
  fName=s;
  fName.ToUpper();
}

inline void HGeomVolume::setShape(const Text_t* s) {
  shape=s;
  shape.ToUpper();
}

inline void HGeomVolume::setMother(const Text_t* s) {
  mother=s;
  mother.ToUpper();
}

#endif /* !HGEOMVOLUME_H */
