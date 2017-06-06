#include "hmdcgetcontainers.h"
#include "hmatrixcategory.h"
#include "hlinearcategory.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdclayergeompar.h"
#include "hmdcgeompar.h"
#include "hmdcdetector.h"
#include "hspecgeompar.h"
#include "hmdcgeomstruct.h"
#include "hmdcrawstruct.h"
#include "hmdclookupgeom.h"
#include "hgeomcompositevolume.h"
#include "hdetgeompar.h"
#include "hgeomtransform.h"
#include "hgeomvolume.h"
#include "hparset.h"
#include "hevent.h"
#include "heventheader.h"
#include "hmdccal1sim.h"
#include "hmdcclussim.h"
#include "hmdchitsim.h"
#include "hmdcsegsim.h"
#include "hmdclayercorrpar.h"
#include "hmdcsetup.h"
#include "hmdctrackdset.h"

//*-- AUTHOR : Pechenov Vladimir
//*-- Modified : 19/06/2007 by V.Pechenov
//*-- Modified : 07/05/2002 by V.Pechenov
//*-- Modified : 08/05/2001
//*-- Modified : 29/09/2000

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
// HMdcGetContainers
//
// The lookup table for geting of pointers to param.&data containers
// Using:
//    HMdcGetContainers* fGetCont=HMdcGetContainers::getObject();
// If the cont. HMdcGetContainers not exist it will automaticaly 
// created and added to RuntimeDb
//
//    HMdcGeomPar* fgeomPar=fGetCont->getMdcGeomPar(Bool_t init=kFALSE);
// If cont. HMdcGeomPar don't exist & init=kTURUE it will created and initialized
//    ...
//
// Transformations:
// lab <--> sector
//    const HGeomTransform& getLabTransSec(Int_t sec, Bool_t init=kFALSE);
//    Bool_t getLabTransSec(HGeomTransform& trans, Int_t sec, 
//        Bool_t init=kFALSE);
//
// lab <--> module
//    const HGeomTransform& getLabTransMod(Int_t sec,Int_t mod,
//        Bool_t init=kFALSE);
//    Bool_t getLabTransMod(HGeomTransform& trans, Int_t sec, Int_t mod,
//        Bool_t init=kFALSE);
//
// sector <--> module
//    Bool_t getSecTransMod(HGeomTransform& trans, Int_t sec, Int_t mod,
//        Bool_t init=kFALSE);
// 
// Next methods use cell thickness from HMdcLayerGeomPar for
// calculating of mdc layer position and don't use HGeomCompositeVolume 
// layer parameters:
//
// lab <--> layer
//    Bool_t getLabTransLayer(HGeomTransform& trans, Int_t sec,Int_t mod,
//                            Int_t lay, Bool_t init=kFALSE);
// sector <--> layer
//    Bool_t getSecTransLayer(HGeomTransform& trans, Int_t sec,Int_t mod,
//                              Int_t lay, Bool_t init=kFALSE);
// module <--> layer
//    HGeomTransform& getModTransLayer(Int_t sec, Int_t mod, Int_t lay,
//                                     Bool_t init=kFALSE);
//////////////////////////////////////////////////////////////////

ClassImp(HMdcGetContainers)


HMdcGetContainers* HMdcGetContainers::fMdcGetContainers=0;

HMdcGetContainers* HMdcGetContainers::getObject() {
  if(!fMdcGetContainers) fMdcGetContainers=new HMdcGetContainers();
  return fMdcGetContainers;
}

HMdcGetContainers::HMdcGetContainers() {
  // seting default values
  if(gHades) {
    rtdb   = gHades->getRuntimeDb();
    isRtdb = kTRUE;
    spectr = gHades->getSetup();
  } else {
    Warning("HMdcGetContainers","gHades=%i",0);
    rtdb   = 0;
    spectr = 0;
    isRtdb = kFALSE;   
  }
  
  fMdcLayerGeomPar  = NULL;
  isMdcLayerGeomPar = kFALSE;
  fMdcGeomPar       = NULL;
  isMdcGeomPar      = kFALSE;
  fMdcDetector      = NULL;
  isMdcDetector     = kFALSE;
  fSpecGeomPar      = NULL;
  isSpecGeomPar     = kFALSE;
  fMdcGeomStruct    = NULL;
  isMdcGeomStruct   = kFALSE;
  fMdcRawStruct     = NULL;
  isMdcRawStruct    = kFALSE;
  fMdcLookupGeom    = NULL;
  isMdcLookupGeom   = kFALSE;
  fMdcLayerCorrPar  = NULL;
  isMdcLayerCorrPar = kFALSE;
  
  pMdcCal1Cat       = NULL;
  pGeantKineCat     = NULL;
  pGeantMdcCat      = NULL;
  pMdcClusCat       = NULL;
  pMdcClusFitCat    = NULL;
  pMdcWireFitCat    = NULL;
  pMdcClusInfCat    = NULL;
  pMdcTrkCandCat    = NULL;
  
  isMdcSetupInited  = kFALSE;
  isSimulation      = -1;
}

void HMdcGetContainers::deleteCont() {
  if(fMdcGetContainers) {
    delete fMdcGetContainers;
    fMdcGetContainers=0;
  }
}

Bool_t HMdcGetContainers::initCont(HParSet *par) {
  if( !par ) return kFALSE;
  if( !isInited(par) || !par->isStatic() ) return kTRUE;
  return par->init();
}

Bool_t HMdcGetContainers::isInited(HParSet *par) {
 if( !par ) return kFALSE;
 return par->getInputVersion(1)>=0 || par->getInputVersion(2)>=0;
}

HMdcLayerGeomPar* HMdcGetContainers::getMdcLayerGeomPar(Bool_t init) {
  if( !fMdcLayerGeomPar && isRtdb )
    fMdcLayerGeomPar=(HMdcLayerGeomPar*)rtdb->getContainer("MdcLayerGeomPar");
  if( init && !isMdcLayerGeomPar ) isMdcLayerGeomPar=initCont(fMdcLayerGeomPar);
  return fMdcLayerGeomPar;
}

HMdcLayerCorrPar* HMdcGetContainers::getMdcLayerCorrPar(Bool_t init) {
  if( !fMdcLayerCorrPar && isRtdb )
    fMdcLayerCorrPar=(HMdcLayerCorrPar*)rtdb->getContainer("MdcLayerCorrPar");
  if( init && !isMdcLayerCorrPar ) isMdcLayerCorrPar=initCont(fMdcLayerCorrPar);
  return fMdcLayerCorrPar;
}

HMdcGeomPar* HMdcGetContainers::getMdcGeomPar(Bool_t init) {
  if( !fMdcGeomPar && isRtdb )
    fMdcGeomPar=(HMdcGeomPar*)rtdb->getContainer("MdcGeomPar");
  if( init && !isMdcGeomPar ) isMdcGeomPar=initCont(fMdcGeomPar);
  return fMdcGeomPar;
}
    
HMdcDetector* HMdcGetContainers::getMdcDetector() {
  if(fMdcDetector==0 && gHades) fMdcDetector = (HMdcDetector*)gHades->getSetup()->getDetector("Mdc");
  return fMdcDetector;
}

HSpecGeomPar* HMdcGetContainers::getSpecGeomPar(Bool_t init) {
  if( !fSpecGeomPar && isRtdb )
    fSpecGeomPar=(HSpecGeomPar*)rtdb->getContainer("SpecGeomPar");
  if( init&& !isSpecGeomPar ) isSpecGeomPar=initCont(fSpecGeomPar);
  return fSpecGeomPar;
}


HMdcGeomStruct* HMdcGetContainers::getMdcGeomStruct(Bool_t init) {
  if( !fMdcGeomStruct && isRtdb )
    fMdcGeomStruct=(HMdcGeomStruct*)rtdb->getContainer("MdcGeomStruct");
  if( init && !isMdcGeomStruct ) isMdcGeomStruct=initCont(fMdcGeomStruct);
  return fMdcGeomStruct;
}


HMdcRawStruct* HMdcGetContainers::getMdcRawStruct(Bool_t init) {
  if( !fMdcRawStruct && isRtdb )
    fMdcRawStruct=(HMdcRawStruct*)rtdb->getContainer("MdcRawStruct");
  if( init && !isMdcRawStruct ) isMdcRawStruct=initCont(fMdcRawStruct);
  return fMdcRawStruct;
}

HMdcLookupGeom* HMdcGetContainers::getMdcLookupGeom(Bool_t init) {
  if( !fMdcLookupGeom && isRtdb )
    fMdcLookupGeom=(HMdcLookupGeom*)rtdb->getContainer("MdcLookupGeom");
  if( init && !isMdcLookupGeom ) isMdcLookupGeom=initCont(fMdcLookupGeom);
  return fMdcLookupGeom;
}

//------------------------------------------------------------------------

HGeomCompositeVolume* HMdcGetContainers::getGeomCompositeVolume(
    Int_t mod, Bool_t init) {
  if(!fMdcGeomPar && isMdcGeomPar ) getMdcGeomPar(init);
  return (fMdcGeomPar) ? fMdcGeomPar->getRefVolume(mod) : 0;
}

HMdcLayerGeomParLay* HMdcGetContainers::getMdcLayerGeomParLay(
    Int_t sec,Int_t mod,Int_t lay, Bool_t init) {
  getMdcLayerGeomPar(init);
  if( !fMdcLayerGeomPar ) return 0;
  HMdcLayerGeomParSec* fsec=&((*fMdcLayerGeomPar)[sec]);
  if( !fsec ) return 0;
  HMdcLayerGeomParMod* fmod=&((*fsec)[mod]);
  if( !fmod ) return 0;
  return &((*fmod)[lay]);
}

HModGeomPar* HMdcGetContainers::getModGeomPar(Int_t sec,Int_t mod,Bool_t init) {
  getMdcGeomPar(init);
  return (fMdcGeomPar) ? fMdcGeomPar->getModule(sec,mod) : 0;
}

const HGeomTransform& HMdcGetContainers::getLabTransSec(Int_t sec,Bool_t init) {
  // return transformation sector<-->lab.
  HSpecGeomPar* fSpecGeomPar=getSpecGeomPar(init);
  if(fSpecGeomPar) {
    HGeomVolume* fVol=fSpecGeomPar->getSector(sec);
    if(fVol) return fVol->getTransform();
  }
  Error("getLabTransSec","can't get the transformation lab.<-->sec. (S%i)",sec+1);
  return *((HGeomTransform*)NULL);
}

Bool_t HMdcGetContainers::getLabTransSec(HGeomTransform& trans, Int_t sec,
    Bool_t init) {
  const HGeomTransform& tr=getLabTransSec(sec,init);
  if( !&tr ) return kFALSE;
  trans=tr;
  return kTRUE;
}

const HGeomTransform& HMdcGetContainers::getLabTransMod(Int_t sec, Int_t mod,
    Bool_t init) {
  //return transformation module<-->lab.
  HModGeomPar* fMod=getModGeomPar(sec,mod,init);
  if(fMod) return fMod->getLabTransform();
  Error("getLabTransMod","can't get the transformation lab.<-->mod. (S%iM%i)",
      sec+1,mod+1);
  return *((HGeomTransform*)0);
}

Bool_t HMdcGetContainers::getLabTransMod(HGeomTransform& trans, Int_t sec,
    Int_t mod, Bool_t init) {
  const HGeomTransform& tr=getLabTransMod(sec,mod,init);
  if( !&tr ) return kFALSE;
  trans=tr;
  return kTRUE;
}

Bool_t HMdcGetContainers::getSecTransMod(HGeomTransform& trans, Int_t sec, 
    Int_t mod, Bool_t init) {
  //return transformation module<-->sector.
  const HGeomTransform& tr=getLabTransMod(sec,mod,init);
  if( !&tr ) return kFALSE;
  const HGeomTransform& fSec=getLabTransSec(sec,init);
  if( &fSec==0 ) return kFALSE;
  trans=tr;
  trans.transTo(fSec);
  return kTRUE;
}

Double_t HMdcGetContainers::getSensWireThick(Int_t mod, Int_t lay) {
  HGeomCompositeVolume* fMod = getGeomCompositeVolume(mod);
  if(fMod == 0) return 0.;
  HGeomVolume* fVol=fMod->getComponent(lay);
  if(fVol == 0) return 0.;
  HGeomVector* p1 = fVol->getPoint(0);
  HGeomVector* p2 = fVol->getPoint(4);
  if(p1==0 || p2==0) return 0.;
  Double_t thick = p2->getZ()-p1->getZ();
  if(thick<0) return 0; // must be at list positive!
  return thick;
}

Bool_t HMdcGetContainers::getModTransGeantLayer(HGeomTransform& trans,
    Int_t sec,Int_t mod,Int_t lay) {
  // Plane of HMdcGeant hits is entrance plane of sensitive wires volume!
  if(isSimData()) {
//!!! For simulation data geometry is not corrected yet...
    HGeomCompositeVolume* fMod=getGeomCompositeVolume(mod);
    if(fMod == 0) return kFALSE;
    HGeomVolume* fVol=fMod->getComponent(lay);
    if(fVol == 0) return kFALSE;
    trans = fVol->getTransform();
  } else {
    getMdcLayerGeomPar();
    HMdcLayerGeomParLay& fLayerGeomParLay = (*fMdcLayerGeomPar)[sec][mod][lay];
    if(&fLayerGeomParLay == 0) return kFALSE;
    trans = fLayerGeomParLay.getLayerPos(); 
  }
  Double_t thick = getSensWireThick(mod,lay);
  if(thick<0.001) return kFALSE;
  HGeomVector tr(trans.getTransVector());
  tr.setZ(tr.getZ()-thick/2.);
  trans.setTransVector(tr);
  return kTRUE;
}

const HGeomTransform& HMdcGetContainers::getModTransLayer(Int_t sec, Int_t mod,
    Int_t lay, Bool_t init) {
  //return transformation layer<-->module calculated in HMdcLayerGeomPar data
  if(isSimData()) {
//!!! For simulation data geometry is not corrected yet...
    HGeomCompositeVolume* fMod=getGeomCompositeVolume(mod,init);
    if(fMod) {
      HGeomVolume* fVol=fMod->getComponent(lay);
      if(fVol) return fVol->getTransform();
    }
    return *((HGeomTransform*)NULL);
  } else {
    getMdcLayerGeomPar(init);
    HMdcLayerGeomParLay& fLayerGeomParLay = (*fMdcLayerGeomPar)[sec][mod][lay];
    if(&fLayerGeomParLay == 0) return *((HGeomTransform*)NULL);
    return fLayerGeomParLay.getLayerPos();
  }
}

Bool_t HMdcGetContainers::getLabTransLayer(HGeomTransform& trans, 
    Int_t sec,Int_t mod, Int_t lay, Bool_t init) {
  //return transformation layer<-->lab. (do shift of the transfomation
  // vector along Z to real position of the sens. wires plane).
  const HGeomTransform& fLabMod=getLabTransMod(sec,mod,init);
  if( !&fLabMod ) return kFALSE; 
  const HGeomTransform& layTrans = getModTransLayer(sec,mod,lay,init);
  if(&layTrans == 0) return kFALSE;
  trans = layTrans;
  trans.transFrom(fLabMod);
  return kTRUE;
}

Bool_t HMdcGetContainers::getLabTransGeantLayer(HGeomTransform& trans, 
    Int_t sec,Int_t mod, Int_t lay) {
  // Plane of HMdcGeant hits is entrance plane of sensitive wires volume!
  // return transformation geant_hits_plane<-->lab.
  const HGeomTransform& fLabMod=getLabTransMod(sec,mod);
  if( !&fLabMod ) return kFALSE;
  if( !getModTransGeantLayer(trans,sec,mod,lay) ) return kFALSE;
  trans.transFrom(fLabMod);
  return kTRUE;
}

Bool_t HMdcGetContainers::getSecTransLayer(HGeomTransform& trans, 
    Int_t sec,Int_t mod, Int_t lay, Bool_t init) {
  //return transformation layer<-->sector (do shift of the transfomation
  // vector along Z to real position of the sens. wires plane).
  const HGeomTransform& fSec=getLabTransSec(sec,init);
  if( !&fSec ) return kFALSE;
  if( !getLabTransLayer(trans,sec,mod,lay,init) )return kFALSE;
  trans.transTo(fSec); 
  return kTRUE;
}

Bool_t HMdcGetContainers::getSecTransGeantLayer(HGeomTransform& trans, 
    Int_t sec,Int_t mod, Int_t lay) {
  // Plane of HMdcGeant hits is entrance plane of sensitive wires volume!
  // return transformation geant_hits_plane<-->sector
  const HGeomTransform& fSec=getLabTransSec(sec);
  if( !&fSec ) return kFALSE;
  if( !getLabTransGeantLayer(trans,sec,mod,lay) )return kFALSE;
  trans.transTo(fSec); 
  return kTRUE;
}

Bool_t HMdcGetContainers::getSecTransCathLayer(HGeomTransform& trans1, 
    HGeomTransform& trans2, Int_t sec,Int_t mod, Int_t lay, Bool_t init) {
  // return transformations of two cathode layers<-->sector 
  // (do shift of the transfomation vector along Z to real position of 
  // the sens. wires plane).
  const HGeomTransform& fSec=getLabTransSec(sec,init);
  if( !&fSec ) return kFALSE;
  const HGeomTransform& fLabMod=getLabTransMod(sec,mod,init);
  if( !&fLabMod ) return kFALSE;
  HMdcLayerGeomPar* fLayerGeomPar = getMdcLayerGeomPar(init);
  if(!fLayerGeomPar) return kFALSE;
  HMdcLayerGeomParLay& fLayerGeomParLay = (*fLayerGeomPar)[sec][mod][lay];
  if(!&fLayerGeomParLay) return kFALSE;
  Double_t halfCatDist       = fLayerGeomParLay.getCatDist()*0.5;
  const HGeomTransform& tr = getModTransLayer(sec,mod,lay,init);
  if(&tr == 0) return kFALSE;
  trans1=tr;
  trans2=tr;
  HGeomVector trVec1(tr.getTransVector());
  HGeomVector trVec2(trVec1);
  trVec1.setZ(trVec1.getZ() - halfCatDist);
  trVec2.setZ(trVec2.getZ() + halfCatDist);
  trans1.setTransVector(trVec1);
  trans2.setTransVector(trVec2);
  
  trans1.transFrom(fLabMod);
  trans2.transFrom(fLabMod);
  
  trans1.transTo(fSec);
  trans2.transTo(fSec);
  return kTRUE;
}

//------Categories:-------------------------

HCategory* HMdcGetContainers::getCatMdcCal1(void) {
  if(pMdcCal1Cat) return pMdcCal1Cat;
  if(gHades) pMdcCal1Cat = gHades->getCurrentEvent()->getCategory(catMdcCal1);
  return pMdcCal1Cat;
}
  
HCategory* HMdcGetContainers::getCatGeantKine(void) {
  if(pGeantKineCat) return pGeantKineCat;
  if(gHades) pGeantKineCat=gHades->getCurrentEvent()->getCategory(catGeantKine);
  return pGeantKineCat;
}

HCategory* HMdcGetContainers::getCatGeantMdc(void) {
  if(pGeantMdcCat) return pGeantMdcCat;
  if(gHades) pGeantMdcCat=gHades->getCurrentEvent()->getCategory(catMdcGeantRaw);
  return pGeantMdcCat;
}

HCategory* HMdcGetContainers::getCatMdcClus(Bool_t create) {
  if(pMdcClusCat) return pMdcClusCat;
  if(gHades==0) return 0;
  pMdcClusCat = gHades->getCurrentEvent()->getCategory(catMdcClus);
  if(!pMdcClusCat && create) {
    Int_t sizes[3]={6,2,8000};   // sec, seg, clust.(maximum 500 clusters in seg)
    if(isGeant()) pMdcClusCat = new HMatrixCategory("HMdcClusSim",3,sizes,0.25);
    else          pMdcClusCat = new HMatrixCategory("HMdcClus",3,sizes,0.25);
    gHades->getCurrentEvent()->addCategory(catMdcClus,pMdcClusCat,"Mdc");
  }
  return pMdcClusCat;
}

HCategory* HMdcGetContainers::getCatMdcTrkCand(Bool_t create) {
  if(pMdcTrkCandCat) return pMdcTrkCandCat;
  if(gHades==0) return 0; 
  pMdcTrkCandCat = gHades->getCurrentEvent()->getCategory(catMdcTrkCand);
  if(!pMdcTrkCandCat && create) {
    if(getMdcDetector() == 0) return 0;
    pMdcTrkCandCat = fMdcDetector->buildCategory(catMdcTrkCand);
    if (!pMdcTrkCandCat) return 0;
    gHades->getCurrentEvent()->addCategory(catMdcTrkCand,pMdcTrkCandCat,"Mdc");
  }
  return pMdcTrkCandCat;
}

HCategory* HMdcGetContainers::getCatMdcClusFit(Bool_t create) {
  if(pMdcClusFitCat) return pMdcClusFitCat;
  if(gHades==0) return 0; 
  pMdcClusFitCat = gHades->getCurrentEvent()->getCategory(catMdcClusFit);
  if(!pMdcClusFitCat && create) {  
    if(isGeant()) pMdcClusFitCat = new HLinearCategory("HMdcClusFitSim",15000);  
    else          pMdcClusFitCat = new HLinearCategory("HMdcClusFit",15000);
    gHades->getCurrentEvent()->addCategory(catMdcClusFit,pMdcClusFitCat,"Mdc");
  }
  return pMdcClusFitCat;
}

HCategory* HMdcGetContainers::getCatMdcWireFit(Bool_t create) {
  if(pMdcWireFitCat) return pMdcWireFitCat;
  if(gHades==0) return 0; 
  pMdcWireFitCat = gHades->getCurrentEvent()->getCategory(catMdcWireFit);
  if(!pMdcWireFitCat && create) {
    if(isGeant()) pMdcWireFitCat = new HLinearCategory("HMdcWireFitSim",200000);  
    else          pMdcWireFitCat = new HLinearCategory("HMdcWireFit",   200000);
    gHades->getCurrentEvent()->addCategory(catMdcWireFit,pMdcWireFitCat,"Mdc");
  }
  return pMdcWireFitCat;
}

HCategory* HMdcGetContainers::getCatMdcClusInf(Bool_t create) {
  if(pMdcClusInfCat) return pMdcClusInfCat;
  if(gHades==0) return 0; 
  pMdcClusInfCat = gHades->getCurrentEvent()->getCategory(catMdcClusInf);
  if(!pMdcClusInfCat && create) {    
    if(isGeant()) pMdcClusInfCat = new HLinearCategory("HMdcClusInfSim",15000);
    else          pMdcClusInfCat = new HLinearCategory("HMdcClusInf",15000);
    gHades->getCurrentEvent()->addCategory(catMdcClusInf,pMdcClusInfCat,"Mdc");
  }
  return pMdcClusInfCat;
}

void HMdcGetContainers::getFileName(TString& fileName) {
  // Return file name without dir.name and extension (.hld, .root, ...)
  getFileNameWInd(fileName);
  Int_t ind = fileName.Last('.');
  if(ind >= 0) fileName.Remove(ind);
}

void HMdcGetContainers::getFileNameWInd(TString& fileName) {
  // Return file name without dir.name
  if(gHades==0) {
    fileName = "";
    return; 
  }
  fileName  = gHades->getRuntimeDb()->getCurrentFileName();
  Int_t ind = fileName.Last('/');
  if(ind >= 0) fileName.Remove(0,ind+1);
}

void HMdcGetContainers::getRunId(TString& runId) {
  // Return runId
  runId = "";
  if(gHades) {
    HEventHeader* evHeader = gHades->getCurrentEvent()->getHeader();
    if(evHeader) runId += evHeader->getEventRunNumber();
  }
}

void HMdcGetContainers::getEventDate(TString& eventDate) {
  // Return current event date
  eventDate = "";
  if(gHades) {
    HEventHeader* evHeader = gHades->getCurrentEvent()->getHeader();
    if(evHeader) {
      UInt_t date = evHeader->getDate();
      UChar_t* dt = (UChar_t*)&date;
      eventDate.Form("%02i-%02i-%4i",dt[0],dt[1]+1,1900+dt[2]);
    }
  }
}

void HMdcGetContainers::getEventTime(TString& eventTime) {
  // Return current event time
  eventTime = "";
  if(gHades) {
    HEventHeader* evHeader = gHades->getCurrentEvent()->getHeader();
    if(evHeader) {
      UInt_t time = evHeader->getTime();
      UChar_t* tm = (UChar_t*)&time;
      eventTime.Form("%02i:%02i:%02i",tm[2],tm[1],tm[0]);
    }
  }
}

Bool_t HMdcGetContainers::isSimData(void) {
  if(isSimulation>=0) return isSimulation > 0; // ==2 ???embedding
  if(gHades != NULL && gHades->getEmbeddingMode()>0) {
    isSimulation = 2;
    return kTRUE;
  }
  if(isRtdb) {
    HMdcSetup* setup = (HMdcSetup*)rtdb->getContainer("MdcSetup");
    if(setup != NULL) {
      HMdcCommonSetup* mdcCommonSetup = setup->getMdcCommonSet();
      if(mdcCommonSetup != NULL) {
        isSimulation = mdcCommonSetup->getIsSimulation();
        return isSimulation > 0;  // ==2 ???embedding
      }
    }
  }

  isSimulation = 0;
  if(getCatMdcCal1()) {
    if(pMdcCal1Cat->getClass()==HMdcCal1Sim::Class()) isSimulation = 1;
  } else if(getCatMdcClus()) {
    if(pMdcClusCat->getClass()==HMdcClusSim::Class()) isSimulation = 1;
  } else {
    HEvent*&   event  = gHades->getCurrentEvent();
    HCategory* catHit = event->getCategory(catMdcHit);
    if(catHit!=NULL) {
      if(catHit->getClass()==HMdcHitSim::Class())     isSimulation = 1;
    } else {
      HCategory* catSeg=event->getCategory(catMdcSeg);
      if(catSeg!=NULL) {
        if(catSeg->getClass()==HMdcSegSim::Class())   isSimulation = 1;
      }
    }
  }

  return isSimulation > 0;  
}

void HMdcGetContainers::initMdcSetup(void) {
  for(Int_t s=0;s<6;s++) {
    numMods[s] = 0;
    for(Int_t m=0;m<4;m++) {
      if(getMdcDetector() != NULL) modSetup[s][m] = HMdcTrackDSet::isModActive(s,m) && getMdcDetector()->getModule(s,m)!=0;
      else                         modSetup[s][m] = HMdcTrackDSet::isModActive(s,m);
      if(modSetup[s][m]) numMods[s]++;
    }
    segSetup[s][0] = modSetup[s][0] || modSetup[s][1];
    segSetup[s][1] = modSetup[s][2] || modSetup[s][3];
  }
  isMdcSetupInited = kTRUE;
}

void HMdcGetContainers::setModList(const Int_t (*modList)[4]) {
  for(Int_t s=0;s<6;s++) {
    numMods[s] = 0;
    for(Int_t m=0;m<4;m++) {
      modSetup[s][m] = modList[s][m];
      if(modSetup[s][m]) numMods[s]++;
    }
    segSetup[s][0] = modSetup[s][0] || modSetup[s][1];
    segSetup[s][1] = modSetup[s][2] || modSetup[s][3];
  }
  isMdcSetupInited = kTRUE;
}

Bool_t HMdcGetContainers::isModActive(UInt_t sec,UInt_t mod) {
  if( !isMdcSetupInited ) initMdcSetup();
  return sec<6 && mod<4 ? modSetup[sec][mod] : kFALSE;
}

Bool_t HMdcGetContainers::isSegActive(UInt_t sec,UInt_t seg) {
  if( !isMdcSetupInited ) initMdcSetup();
  return sec<6 && seg<2 ?  segSetup[sec][seg] : kFALSE;
}

Bool_t HMdcGetContainers::isSecActive(UInt_t sec) {
  if( !isMdcSetupInited ) initMdcSetup();
  return sec<6&&numMods[sec]>0 ?  kTRUE : kFALSE;
}

Short_t HMdcGetContainers::getNumModulesInSec(UInt_t sec) {
  if( !isMdcSetupInited ) initMdcSetup();
  return sec<6 ? numMods[sec]:0;
}
