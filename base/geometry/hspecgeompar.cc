//*-- AUTHOR : Ilse Koenig
//*-- Modified : 26/04/02 by Ilse Koenig
//*-- Modified : 10/12/01 by Ilse Koenig
//*-- Modified : 30/06/99
//*-- Modified : 10/05/01 by Dan Magestro
//*-- Modified : 13/01/05 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HSpecGeomPar
//
// Parameter container for the geometry of the cave, the sector(s) and the 
// target(s)
//
///////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "hspecgeompar.h"
#include "hgeomshapes.h"
#include "hgeomvolume.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HSpecGeomPar)

HSpecGeomPar::HSpecGeomPar(const Char_t* name,const Char_t* title,
              const Char_t* context,const Int_t maxSec)
            : HParSet(name,title,context) {
  // constructor takes a pointer to the shape classes
  // creates an empty array 'sectors' of size maxSec (default 0)
  strcpy(detName,"Spectrom");  // max 10 char!
  isFirstInit=kTRUE;
  if (maxSec>0) sectors=new TObjArray(maxSec);
  else sectors=0;
  cave=0;
  targets=0; 
  if (gHades) shapes=gHades->getSetup()->getShapes();
  else shapes=0;
}


HSpecGeomPar::~HSpecGeomPar() {
  // destructor deletes the arrays
  if (cave) delete cave;
  if (sectors) {
    sectors->Delete();
    delete sectors;
  }
  if (targets) {
    targets->Delete();
    delete targets;
  }
}


Int_t HSpecGeomPar::getNumSectors() {
  // returns the maximum index of the sectors
  if (sectors) return  (sectors->GetSize());
  return 0;
}


Int_t HSpecGeomPar::getNumTargets() {
  // returns the number the targets
  if (targets) return  (targets->GetEntries());
  return 0;
}


HGeomVolume* HSpecGeomPar::getSector(const Int_t n) {
  // returns a pointer to the sector volume with index n
  if (sectors) return (HGeomVolume*)sectors->At(n);
  return 0;
}


HGeomVolume* HSpecGeomPar::getTarget(const Int_t n) {
  // returns a pointer to the target volume with index n
  if (targets) return (HGeomVolume*)targets->At(n);
  return 0;
}


void HSpecGeomPar::createTargets(const Int_t n) {
  // creates an empty array 'targets' of size n
  // if n==0 the array is deleted
  if (targets) {
    targets->Delete();
    delete targets;
    targets=0;
  }
  if (n>0) targets=new TObjArray(n);
}


void HSpecGeomPar::addCave(HGeomVolume* v) {
  if (!v) return;
  if (cave) delete cave;
  cave=new HGeomVolume(*v);
  cave->setMother("");
}


void HSpecGeomPar::addSector(HGeomVolume* v) {
  // Adds the given sector volume in the array 'sectors'.
  // The position in the array is retrieved from the name.
  // If the array does not exist it is created.
  // If the array it too small it is expanded.
  if (!v) return;
  Int_t n=getSectorIndex(v->GetName());
  if (sectors==0) sectors=new TObjArray(n+1);
  else {
    if (n>=sectors->GetSize()) sectors->Expand(n+1);
    else {
      HGeomVolume* s=(HGeomVolume*)sectors->At(n);
      if (s) delete s;
      s=0;
    }
  }
  sectors->AddAt(new HGeomVolume(*v),n);
}


void HSpecGeomPar::addTarget(HGeomVolume* v) {
  // Adds the given target volume in the array 'target'.
  // The position in the array is retrieved from the name.
  // If the array does not exist it is created.
  // If the array it too small it is expanded.
  if (!v) return;
  Int_t n=getTargetIndex(v->GetName());
  if (targets==0) targets=new TObjArray(n+1);
  else {
    if (n>=targets->GetSize()) targets->Expand(n+1);
    else {
      HGeomVolume* t=(HGeomVolume*)targets->At(n);
      if (t) delete t;
      t=0;
    }
  }
  targets->AddAt(new HGeomVolume(*v),n);
}


Bool_t HSpecGeomPar::init(HParIo* io) {
  // Initializes the container from the input(s) in the runtime database.
  // The sector or target informations have to be initialized completely from
  // one input. It is possible to take the sector informations from the first
  // input and the target informations from the second input, but it is not
  // possible to take the sector informations from two inputs. 
  if (versions[1]!=-1 && versions[2]!=-1) clear();
  Int_t set[3]={1,1,1};
  Bool_t allFound=kFALSE;
  if (io) {
    allFound=init(io,set);
  } else {
    HRuntimeDb* rtdb=gHades->getRuntimeDb();
    io=rtdb->getFirstInput();
    if (io) allFound=init(io,set);
    if (!allFound) {
      io=rtdb->getSecondInput();
      if (io) allFound=init(io,set);
    } else setInputVersion(-1,2);
  }
  if (allFound) return kTRUE;
  cerr<<"********  "<<GetName()<<"  not initialized  ********"<<endl;
  return kFALSE;
}


Bool_t HSpecGeomPar::init(HParIo* inp,Int_t* set) {
  // initializes the container from an input
  HDetParIo* input=inp->getDetParIo("HSpecParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}


Int_t HSpecGeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HSpecParIo");
  if (out) return out->write(this);
  return -1;
}


void HSpecGeomPar::clear() {
  // deletes the targets, but does not change cave and sectors
  if (targets) {
    targets->Delete();
    delete targets;
    targets=0;
  }
  status=kFALSE;
  resetInputVersions();
}


void HSpecGeomPar::printParam() {
  // prints the parameters
  if (cave) cave->print();
  if (sectors) {
    for(Int_t i=0;i<sectors->GetSize();i++) {
      HGeomVolume* p=(HGeomVolume*)sectors->At(i);  
      if (p) p->print();
    }
  }
  if (targets) {
    for(Int_t i=0;i<targets->GetSize();i++) {
      HGeomVolume* p=(HGeomVolume*)targets->At(i);  
      if (p) p->print();
    }
  }
}


Int_t HSpecGeomPar::getSectorIndex(const Text_t* name) {
  // returns the module index retrieved from the module name SECx
  return (Int_t)(name[3]-'0')-1;
}


Int_t HSpecGeomPar::getTargetIndex(const Text_t* name) {
  // returns 0 if the target name has only 4 characters (only 1 target)
  // returns the index retrieved from the target name TARGxx or TXxx
  Int_t l=strlen(name);
  Int_t idx=-1;
  if (strncmp(name,"TARG",4)==0) {
    if (l==4) {
      idx=0;
    } else if (l==5) {
      idx= (Int_t)(name[4]-'0')-1;
    } else if (l==6) {
      idx=(Int_t)(name[4]-'0')*10+(Int_t)(name[5]-'0')-1; 
    }
  } else if (l==4 && strncmp(name,"TX",2)==0) {
    idx=(Int_t)(name[2]-'0')*10+(Int_t)(name[3]-'0')-1;
  }
  return idx;  
}
