//*-- AUTHOR :  Pablo Cabanelas
//*-- Created : 10/10/2005
//*-- Modified: 11/09/2006
//*-- Modified: 29/08/2007

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HRpcDetector
//
//  The Rpc Time-of-Flight detector class
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hrpcdetector.h"
#include "rpcdef.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hrpcraw.h"
#include "hrpccal.h"
#include "hrpchit.h"
#include "hrpccluster.h"
#include "hades.h"
#include "hpario.h"
#include "hparrootfileio.h"
#include "hparasciifileio.h"
#include "hdetpario.h"
#include "hrpcparrootfileio.h"
#include "hrpcparasciifileio.h"
#include "hgeantmaxtrk.h"
#include "TClass.h"
#include "TArrayI.h"
#include <iostream> 
#include <iomanip>

ClassImp(HRpcDetector) // Rpc Time-of-Flight detector class


HRpcDetector::HRpcDetector(void) : HDetector("Rpc","The Rpc detector") {
  // constructor
  maxModules=1;
  maxColumns=6;
  maxCells=32;
  maxEmptyChannels=40; //From 40-45 (DBOin spares) and from 50-70 (TRB spares)
  maxComponents=maxColumns*maxCells;  //geometry components per module
	
  modules = new TArrayI(getMaxSectors());
}

HRpcDetector::~HRpcDetector(void) {
}


HCategory* HRpcDetector::buildLinearCategory(const Text_t *classname,Float_t fillRate) {

  Int_t total_components;
  total_components = getMaxSectors()*maxModules*maxColumns*(maxCells+maxEmptyChannels);

  HLinearCategory* category=new HLinearCategory(classname,(Int_t)(total_components*fillRate));

  return category;
}


HCategory *HRpcDetector::buildMatrixCategory(const Text_t* classname,Float_t fillRate) {

  HMatrixCategory* category=NULL;
  Int_t* sizes   = new Int_t[3];
  Int_t* sizes2  = new Int_t[2];

  if (strcmp(classname,"HRpcClusterSim")==0 || strcmp(classname,"HRpcCluster")==0) {
    sizes2[0]=getMaxSectors();
    sizes2[1]=maxColumns*maxCells;                                    
    category=new HMatrixCategory(classname,2,sizes2,fillRate);   
  }

  else if (strcmp(classname,"HRpcRaw")==0) {
    sizes[0]=getMaxSectors();
    sizes[1]=maxColumns;
    sizes[2]=maxCells+maxEmptyChannels;  
    category=new HMatrixCategory(classname,3,sizes,fillRate);   
  }
  else if (strcmp(classname,"HGeantRpc")==0) {
    sizes2[0]=getMaxSectors();
    sizes2[1]=MAXTRKRPC;
    category=new HMatrixCategory(classname,2,sizes2,fillRate);   
  }
  else {
    sizes[0]=getMaxSectors();
    sizes[1]=maxColumns;
    sizes[2]=maxCells;  
    category=new HMatrixCategory(classname,3,sizes,fillRate);
  }

  delete [] sizes;
  delete [] sizes2;

  return category;
}

HCategory *HRpcDetector::buildCategory(Cat_t cat) {
  // builds the categories for the Rpc
  switch (cat) {
  case catRpcRaw      : return buildMatrixCategory("HRpcRaw",0.5);
  case catRpcCal      : return buildMatrixCategory("HRpcCal",0.5);
  case catRpcHit      : return buildMatrixCategory("HRpcHit",0.5);
  case catRpcCluster  : return buildMatrixCategory("HRpcCluster",0.5);
  case catRpcGeantRaw : return buildMatrixCategory("HGeantRpc",0.5); //FIXME: Only 2 locs for this Category
  default : return NULL;
  }
}

void HRpcDetector::activateParIo(HParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Rpc detector
  if (strcmp(io->IsA()->GetName(),"HParOraIo")==0) {
    io->setDetParIo("HRpcParIo");
    return;
  }
  if (strcmp(io->IsA()->GetName(),"HParRootFileIo")==0) {
    HRpcParRootFileIo* p=new HRpcParRootFileIo(((HParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"HParAsciiFileIo")==0) {
    HRpcParAsciiFileIo* p=new HRpcParAsciiFileIo(((HParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
  }
}

Bool_t HRpcDetector::write(HParIo* output) {
  // writes the Rpc detector setup to the output
  HDetParIo* out=output->getDetParIo("HRpcParIo");
  if (out) return out->write(this);

  return kFALSE;
}


