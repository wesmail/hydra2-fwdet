//*-- AUTHOR :  Kirill Lapidus
//*-- Created :
//*-- modified : 20/02/2017 by Vladimir Pechenov

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HEmcDetector
//
//  The Emc detector class
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hemcdetector.h"
#include "emcdef.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hgeantmaxtrk.h"
#include "hades.h"
#include "hevent.h"
#include "hpario.h"
#include "hparrootfileio.h"
#include "hparasciifileio.h"
#include "hdetpario.h"
#include "hemcparrootfileio.h"
#include "hemcparasciifileio.h"
#include "TClass.h"
#include <iostream> 
#include <iomanip>

ClassImp(HEmcDetector) // Emc detector class

HEmcDetector::HEmcDetector(void) : HDetector("Emc","The Emc detector") {
  // constructor
  fName="Emc";
  maxSectors=6;
  maxModules=1;
  maxComponents=emcMaxRows*emcMaxColumns; // including spares (needed for parameter containers)
  numCells=163;
  modules = new TArrayI(getMaxSectors());
}

HEmcDetector::~HEmcDetector(void) {
  delete modules;
  modules = 0;
}

HCategory* HEmcDetector::buildLinearCategory(const Text_t *classname, Float_t fillRate) {
  HLinearCategory* category=new HLinearCategory(classname,(Int_t)(numCells*fillRate));
  return category;
}

HCategory *HEmcDetector::buildMatrixCategory(const Text_t* classname, Float_t fillRate) {
  //making the categories for different types of data levels
  HMatrixCategory* category = NULL;
  Int_t* sizes2  = new Int_t[2];
  if (strcmp(classname,"HGeantEmc")==0) {
    sizes2[0]= getMaxSectors();
    sizes2[1]= MAXTRKEMC;
    category = new HMatrixCategory(classname,2,sizes2,fillRate);   
  } else if (strcmp(classname,"HEmcCal")==0 || strcmp(classname,"HEmcCalSim")==0) {
    sizes2[0]=getMaxSectors();
    sizes2[1]=emcMaxRows*emcMaxColumns;
    category = new HMatrixCategory(classname,2,sizes2,fillRate);
  } else if (strcmp(classname,"HEmcCluster")==0 || strcmp(classname,"HEmcClusterSim")==0) {
    sizes2[0]=getMaxSectors();
    sizes2[1]=emcMaxRows*emcMaxColumns;
    category = new HMatrixCategory(classname,2,sizes2,fillRate);
  } else {
    sizes2[0]=getMaxSectors();
    sizes2[1]=maxComponents;
    category = new HMatrixCategory(classname,2,sizes2,fillRate);
  }
  delete [] sizes2;
  return category;
}

HCategory *HEmcDetector::buildCategory(Cat_t cat) {
  // builds the categories for the Emc
  // gets the category if existing
  // builts and adds if not existing
  // returns the pointer to the category or zero
  HCategory *pcat = gHades->getCurrentEvent()->getCategory(cat);
  if (pcat) return (pcat);  // already existing
  switch (cat) {
    case catEmcRaw :
      pcat = buildMatrixCategory("HEmcRaw",0.5);
      break;
    case catEmcCal :
      pcat = buildMatrixCategory("HEmcCal",0.5);
      break;
    case catEmcCluster :
//      pcat = buildLinearCategory("HEmcCluster",0.5);
      pcat = buildMatrixCategory("HEmcCluster",0.5);
      break;
    default :
      pcat = NULL;
  }
  if (pcat) gHades->getCurrentEvent()->addCategory(cat, pcat, "Emc");
  return (pcat);
}

void HEmcDetector::activateParIo(HParIo* io) {
  // activates the input/output class for the Emc parameters
  if (strcmp(io->IsA()->GetName(),"HParOraIo")==0) {
    io->setDetParIo("HEmcParIo");
    return;
  }
  if (strcmp(io->IsA()->GetName(),"HParRootFileIo")==0) {
    HEmcParRootFileIo* p=new HEmcParRootFileIo(((HParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"HParAsciiFileIo")==0) {
    HEmcParAsciiFileIo* p=new HEmcParAsciiFileIo(((HParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
  }
}

Bool_t HEmcDetector::write(HParIo* output) {
  // writes the Emc setup to output
  HDetParIo* out=output->getDetParIo("HEmcParIo");
  if (out) return out->write(this);
  return kFALSE;
}

Int_t HEmcDetector::getCell(const Char_t row, const Char_t col) {
  // returns the cell index
  if (row<0 || row>=emcMaxRows || col<0 || col>=emcMaxColumns) return -1;
  else return row*emcMaxColumns + col;
}

void HEmcDetector::getRowCol(const Int_t cell, Char_t& row, Char_t& col) {
  // returns the row and column indexes
  if (cell>=0 && cell<emcMaxComponents) {
    row=(Char_t)(cell/emcMaxColumns);
    col=(Char_t)(cell%emcMaxColumns);
  } else row=col=-1;
} 

Int_t HEmcDetector::getMaxSecInSetup(void) {
  // returns the maximum number of modules in the actual setup
  Int_t maxSec = -1;
  for (Int_t i = 0; i < maxSectors; i++) {
    if (modules->At(i)) maxSec = (i > maxSec) ? i : maxSec;
  }
  maxSec++;
  return maxSec;
}

