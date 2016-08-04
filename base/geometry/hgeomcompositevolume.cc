//*-- AUTHOR : Ilse Koenig
//*-- Modified : 20/04/99

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomCompositeVolume
//
//
/////////////////////////////////////////////////////////////

#include "hgeomcompositevolume.h"

ClassImp(HGeomCompositeVolume)

HGeomCompositeVolume::HGeomCompositeVolume(Int_t nComp)
    : HGeomVolume() {
  if (nComp) {
    components=new TObjArray(nComp);
    for(Int_t i=0;i<nComp;i++) components->AddAt(new HGeomVolume(),i);
  }
  else components=0; 
}

HGeomCompositeVolume::~HGeomCompositeVolume() {
  if (components) components->Delete();
  delete components;
}

Int_t HGeomCompositeVolume::getNumComponents() {
   if (components) return components->GetSize();
   else return 0;
}

HGeomVolume* HGeomCompositeVolume::getComponent(const Int_t n) {
  return (HGeomVolume*)components->At(n);
}

void HGeomCompositeVolume::createComponents(const Int_t n) {
  if (components) components->Delete();
  components->Expand(n);
}

void HGeomCompositeVolume::setComponent(HGeomVolume* v,const Int_t n) {
  if (components) components->AddAt(v,n);
  else Error("HGeomCompositeVolume::setComponent","no array of components");
}

void HGeomCompositeVolume::clear() {
  HGeomVolume::clear();
  Int_t n=getNumComponents();
  for(Int_t i=0;i<n;i++) ((HGeomVolume*)components->At(i))->clear();
}
  
void HGeomCompositeVolume::print() {
  HGeomVolume::print();
  HGeomVolume* v;
  for(Int_t i=0;i<components->GetSize();i++) {
    v=(HGeomVolume*)components->At(i);
    if (v && strlen(v->GetName())>0) v->print();
  } 
} 
