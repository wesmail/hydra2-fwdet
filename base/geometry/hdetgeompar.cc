//*-- AUTHOR : Ilse Koenig
//*-- Last modified : 26/04/02 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HGeomDetPar
//
// Container class for the basic geometry parameters of a detector
//
// This container can hold the information about the sizes and positions of
// volumes forming a geometry tree with 2 levels: modules and components in
// these modules.
// 
// The information is stored in 2 arrays.
// The array "modules" is a linear array of maxSectors*maxModules pointers to
// detector modules (type HModGeomPar) each having a name and a lab
// transformation which describes the position and orientation of the internal
// module coordinate system relative to the cave coordinate system.
// Each module has a pointer to a reference module describing the type of this
// module. These module types (objects of class HGeomCompositeVolume) are
// stored in the second array "refVolumes". (Normally the modules in the six
// sectors are identical and the information has to be stored only once. Only
// their position is different.)
// Each reference module is a volume with a detector dependent number of
// components which are volumes themselves. Each volume has a name, a shape, a
// mother, a shape dependant number of points describing the size and a
// transformation. The transformation of a module describes the detector
// coordinate system (ideal position in a sector) and the transformation of a
// component shows the position and orientation relative to  this detector
// coordinate system. 
//  
// inline functions of class HDetGeomPar:
//
//   Int_t getNumComponents()
//       returns the number of components in a module
//   Int_t getMaxSectors()
//       returns the maximum number of sectors
//   Int_t getMaxModules()
//       returns the maximum number of modules
//   HGeomShapes* getShapes()
//       return the pointer to the shape classes
//
// virtual functions of class HDetGeomPar which have to be implemented by the
// detector classes:
//   virtual Bool_t init(HParIo*,Int_t*)
//       initilizes the container from an input 
//   virtual Int_t write(HParIo*)
//       writes the container to an output
//   virtual Int_t getSecNum(const TString&)
//       retrieves the sector number from the name of a module 
//   virtual Int_t getModNumInMod(const TString&) {return -1;}
//       retrieves the module number from the name of a module 
//   virtual Int_t getModNumInComp(const TString&) {return -1;}
//       retrieves the module number from the name of a component 
//   virtual Int_t getCompNum(const TString&) {return -1;}
//       retrieves the component number from the name of a component
//
// inline functions of class HModGeomPar:
//
//   void setVolume(HGeomCompositeVolume* p)
//       sets the pointer to the reference volume
//   HGeomCompositeVolume* getRefVolume()
//       returns a pointer to the reference volume
//   HGeomTransform& getLabTransform()
//       returns the lab transformation of a module
//   const Text_t* getRefName() const
//       sets the name of the reference volume
//
///////////////////////////////////////////////////////////////////////////////

#include "hdetgeompar.h"
#include "hgeomcompositevolume.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hpario.h"

ClassImp(HDetGeomPar)
ClassImp(HModGeomPar)

void HModGeomPar::setRefName(const Text_t* s) {
  //sets the name of the reference volume
  refName=s;
  refName.ToUpper();
}

void HModGeomPar::print() {
  // prints the name of the module volume, the name of the reference module
  // volume and the lab transformation
  cout<<fName<<"  "<<refName<<'\n';
  transform.print();
  printf("\n");
}

void HModGeomPar::clear() {
  // clears the module
  refName="";
  refVolume=0;
  transform.clear();
}

HDetGeomPar::HDetGeomPar(const Char_t* name,const Char_t* title,
               const Char_t* context,const Char_t* detectorName)
           : HParSet(name,title,context) {
  // The constructor creates an array of maxSectors*maxModules pointers of
  // type HModGeomPar
  // The pointers to modules, which are not active (taken from the setup),
  // are NULL-pointers.
  // It creates also an array of size maxModules, which is filled which the 
  // reference modules during the first initialisation
  HDetector* setup=0;
  strcpy(detName,detectorName);
  isFirstInit=kTRUE;
  if (gHades && (setup=gHades->getSetup()->getDetector(detName))) {
    shapes=gHades->getSetup()->getShapes();
    maxSectors=setup->getMaxSectors();
    maxModules=setup->getMaxModules();
    numComponents=setup->getMaxComponents();
    if (maxModules>0) {
      refVolumes=new TObjArray(maxModules);
      Int_t* set=setup->getModules();
      Int_t n=0;
      if (maxSectors>0) n=maxSectors*maxModules;
      else n=maxModules;
      modules=new TObjArray(n);
      for (Int_t i=0;i<n;++i) {
        if (set[i]!=0) modules->AddAt(new HModGeomPar(),i);
        else modules->RemoveAt(i);
      }
    }
  } else {
    if (gHades) Error("HDetGeomPar(...)",
                      "Detector not found");
    modules=0;
    refVolumes=0;
    shapes=0;
    maxSectors=maxModules=numComponents=0;
  }
}

HDetGeomPar::~HDetGeomPar() {
  // destructor deletes the arrays
  if (modules) modules->Delete();
  delete modules;
  if (refVolumes) refVolumes->Delete();
  delete refVolumes;
}

Int_t HDetGeomPar::getNumModules() {
  // returns the maximum number of the modules
  if (modules) return  (modules->GetSize());
    return 0;
}

Int_t HDetGeomPar::getNumRefModules() {
  // returns the maximum number of the reference modules
  if (refVolumes) return  (refVolumes->GetSize());
    return 0;
}

HModGeomPar* HDetGeomPar::getModule(const Int_t s,const Int_t m) {
  // returns a pointer to the module with index m in sector with index s
  if (modules) {
    if (s>=0) return (HModGeomPar*)modules->At(s*maxModules+m);
    else return (HModGeomPar*)modules->At(m);
  }
  return 0;
}

HModGeomPar* HDetGeomPar::getModule(const Int_t n) {
  // returns a pointer to the module at position n in the array
  if (modules && n<modules->GetSize()) return (HModGeomPar*)modules->At(n);
  return 0;
}

void HDetGeomPar::getSector(TObjArray* array,const Int_t s) {
  // fills the given array with the pointers to all modules in the sector with
  // index s
  Int_t l=0;
  if (array && (l=array->GetSize())) {
    Int_t i=s*maxModules;
    for(Int_t n=0;n<l&&n<maxModules;n++) {
      array->AddAt(modules->At(i),n);
      i++;
    }
  }
}

HGeomCompositeVolume* HDetGeomPar::getRefVolume(const Int_t m) {
  // returns a pointer to the reference module with index m
  if (refVolumes && m<refVolumes->GetSize()) return (HGeomCompositeVolume*)refVolumes->At(m);
  return 0;
}

void HDetGeomPar::addRefVolume(HGeomCompositeVolume* v,const Int_t n) {
  // adds the given reference volume at postion n in the array refVolumes
  if (refVolumes==0) refVolumes=new TObjArray(n+1);
  if (n>=refVolumes->GetSize()) refVolumes->Expand(n+1);
  refVolumes->AddAt(v,n);
}

void HDetGeomPar::clear() {
  // clears the parameter container completely as Long_t as it was not
  //   initialized completly
  // otherwise it clears only the module transformation, but not the
  //   reference volume and its inner parts 
  // This function is virtual and could be overloaded in the derived class if needed.
  if (isFirstInit) {
    for(Int_t i=0;i<modules->GetSize();i++) {
      HModGeomPar* p=(HModGeomPar*)modules->At(i);
      if (p) p->clear();
    }
    for(Int_t i=0;i<refVolumes->GetSize();i++) {
      HGeomCompositeVolume* p=(HGeomCompositeVolume*)refVolumes->At(i);
      if (p) p->clear();
    }
  } else {
    for(Int_t i=0;i<modules->GetSize();i++) {
      HModGeomPar* p=(HModGeomPar*)modules->At(i);  
      if (p) p->getLabTransform().clear();
    }
  }
  status=kFALSE;
  resetInputVersions();
}

void HDetGeomPar::printParam() {
  // prints the parameters
  // This function is virtual and could be overloaded in the derived class if
  // needed.
  for(Int_t i=0;i<modules->GetSize();i++) {
    HModGeomPar* p=(HModGeomPar*)modules->At(i);  
    if (p) p->print();
  }
  for(Int_t i=0;i<refVolumes->GetSize();i++) {
    HGeomCompositeVolume* p=(HGeomCompositeVolume*)refVolumes->At(i);  
    if (p) p->print();
  }
}

/*
void HDetGeomPar::Streamer(TBuffer &R__b)
{
   // Stream an object of class HDetGeomPar.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      if (R__v > 1) {
	HDetGeomPar::Class()->ReadBuffer(R__b,this,R__v,R__s,R__c);
      } else {
	HParSet::Streamer(R__b);
	R__b >> modules;
	R__b >> refVolumes;
	R__b >> maxSectors;
	R__b >> maxModules;
	R__b >> numComponents;
	R__b.CheckByteCount(R__s, R__c, HDetGeomPar::IsA());
      }
   } else {
     HDetGeomPar::Class()->WriteBuffer(R__b,this);
   }
}
*/
