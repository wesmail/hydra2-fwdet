#pragma implementation
using namespace std;
#include "hshowerdetector.h"
#include "hades.h"
#include "hpario.h"
#include "hparrootfileio.h"
#include "hdetpario.h"
#include "hparasciifileio.h"
#include "hcategory.h"
#include "hmatrixcategory.h"
#include "hlinearcategory.h"
#include "showerdef.h"
#include "hshowercalibrater.h"
#include "hshowerhit.h"
#include "hshowerhitfinder.h"
#include "hshowerparasciifileio.h"
#include "hshowerparrootfileio.h"
#include "htaskset.h"
#include "TClass.h"
#include <iostream> 
#include <iomanip>
HShowerDetector::HShowerDetector(void) : HDetector("Shower","The SHOWER detector") {
  maxModules=3;
  maxRows=32;
  maxColumns=32;
  maxComponents =1;
  modules=new TArrayI(getMaxSectors()*maxModules);

  //  createTaskSet("ShowerTaskSet", "Shower list of tasks");
}

HShowerDetector::~HShowerDetector(void) {
}

Bool_t HShowerDetector::init(const Text_t*) {
  return kTRUE;
}

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////
// initialization according to Ilse's scheme

void HShowerDetector::activateParIo(HParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Shower
  if (strcmp(io->IsA()->GetName(),"HOra2Io")==0) {
    io->setDetParIo("HShowerParIo");
    return;
  }

  if (strcmp(io->IsA()->GetName(),"HParRootFileIo")==0) {
    HShowerParRootFileIo* p=new HShowerParRootFileIo(((HParRootFileIo*)io)
                                                        ->getParRootFile());
    io->setDetParIo(p);
  }

  if (strcmp(io->IsA()->GetName(),"HParAsciiFileIo")==0) {
    HShowerParAsciiFileIo* p=new HShowerParAsciiFileIo(((HParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
  }

}

Bool_t HShowerDetector::write(HParIo* output) {
  // writes the Shower setup to output
  HDetParIo* out=output->getDetParIo("HShowerParIo");
  if (out) return out->write(this);
  return kFALSE;
}
//////////////////////////////////////////////////////////

/*
HTask *HShowerDetector::buildTask(Text_t *name,Option_t *opt="") {
  HTask *r=NULL;
  if (strcmp(name,"calibrate")==0) {
    r=new HShowerCalibrater("shower.calibrater","calibrater");
  } else if (strcmp(name,"hitF")==0) {
    r=new HShowerHitFinder("shower.hitF","Hit finder");
    if (strcmp(opt,"raw")==0) {
      HTaskSet *ts=new HTaskSet("shower.hitF","Hit finder");
      ts->connect(new HShowerCalibrater("shower.calibrater","shower.calibrater"));
      if (!ts->connect(r,"shower.calibrater")) 
	Error("buildTask","connection failed");
      ts->connect(NULL,r);
      r=ts;
    }
  }
  return r;
} 
*/

HTask *HShowerDetector::buildTask(const Text_t *name,const Option_t *opt) {
 //name bedzie pozniej nazwa klasy tworzacej liste zadan dla operacji
 //opisanych przez ta nazwe


 if (name && strcmp(name, "default")==0)  name = NULL;
  
 HTaskSet *pDetTasks = new HTaskSet();

  if (!name) //default list of tasks
  { 
      HShowerCalibrater *cal = new HShowerCalibrater("ShowerCal","shower.cal");
      HShowerHitFinder *hit=new HShowerHitFinder("ShowerHitFinder","Shower hit finder");

      pDetTasks->connect(cal);
      pDetTasks->connect(hit, "ShowerCal");
      pDetTasks->connect((HTask*)NULL, "ShowerHitFinder");

      return pDetTasks;
  }

return NULL;
}

HCategory *HShowerDetector::buildLinearCat(const Text_t *classname) {
  HLinearCategory *category=NULL;

		// allocate 10% of maximum
  Int_t size = (Int_t)0.1*32*32*3*getShowerSectors();     
  category=new HLinearCategory(classname, size);
  return category;
}


HCategory *HShowerDetector::buildMatrixCat(const Text_t *classname,Float_t fillRate, Int_t nLevels, Int_t* sizes) {
  return new HMatrixCategory(classname,nLevels,sizes,fillRate);
}
 
HCategory *HShowerDetector::buildMatrixCat(const Text_t *classname,Float_t fillRate, Int_t nLevels) {
  Int_t maxSector=0,maxModule=0,mod=0;
  Int_t sec=0;
  Int_t i,sizes[4];
  HMatrixCategory *category=NULL;

  for (i=0;i<modules->fN;i++) {
    mod=modules->At(i)-1;
    if (mod>-1) {
      mod = i;
      sec=mod/maxModules;
      mod=mod%maxModules;
      maxSector=((sec)>maxSector)?(sec):maxSector;
      maxModule=((mod)>maxModule)?(mod):maxModule;
    }
  }
  maxSector++; maxModule++;
  sizes[0]=maxSector; sizes[1]=maxModule;
  sizes[2] = sizes[3] = 32;

  category=(HMatrixCategory*)buildMatrixCat(classname,fillRate, nLevels, sizes);
  return category;
}

Int_t HShowerDetector::getShowerSectors() {
  Int_t sect, mod;
  getMaxValues(&sect, &mod);

  return sect;
}

Int_t HShowerDetector::getShowerModules() {
  Int_t sect, mod;
  getMaxValues(&sect, &mod);

  return mod;
}

void HShowerDetector::getMaxValues(Int_t* pMaxSector, Int_t *pMaxModule) {
  Int_t mod,sec=0;
  Int_t i;
  Int_t maxSector=0;
  Int_t maxModule=0;

  for (i=0;i<modules->fN;i++) {
    mod=modules->At(i)-1;
    if (mod>-1) {
      mod = i;
      sec=mod/maxModules;
      mod=mod%maxModules;
      maxSector=((sec)>maxSector)?(sec):maxSector;
      maxModule=((mod)>maxModule)?(mod):maxModule;
    }
  }
  *pMaxSector = ++maxSector; 
  *pMaxModule = ++maxModule;
}

HCategory *HShowerDetector::buildCategory(Cat_t cat) {
  Int_t wireSizes[] = {6,600};
  
  switch (cat) {
//  case catShowerRaw : return buildMatrixCat("HShowerRaw",1, 4);
//  case catShowerCal : return buildMatrixCat("HShowerCal",1, 4);
  case catShowerRaw : return buildLinearCat("HShowerRaw");
  case catShowerCal : return buildLinearCat("HShowerCal");
  case catShowerRawMatr : return buildMatrixCat("HShowerRawMatr",1, 4);

  case catShowerHit : return buildLinearCat("HShowerHit");
  case catShowerPID : return buildLinearCat("HShowerPID");
  case catShowerTrack : return buildLinearCat("HShowerTrack");
  case catShowerGeantRaw : return buildMatrixCat("HShowerGeantHit", 0.2, 2, wireSizes);
 
  case catShowerGeantWire : return buildLinearCat("HShowerGeantWire");
  case catShowerHitHdr : return buildMatrixCat("HShowerHitHeader", 1, 2);
  case catShowerPIDTrack : return buildLinearCat("HShowerPIDTrack");

  default : return NULL;
  }
}

ClassImp(HShowerDetector)
