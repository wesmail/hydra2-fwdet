//*-- AUTHOR : Ilse Koenig
//*-- Modified : 19/01/2001 by I. Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HMdcParRootFileIo
//
//  Class for Mdc parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
// This class replaces the former class HMdcParFileIo.
/////////////////////////////////////////////////////////////
using namespace std;
#include "hmdcparrootfileio.h"
#include "hparrootfileio.h"
#include "hades.h"
#include "hmdcrawstruct.h"
#include "hmdcgeomstruct.h"
#include "hmdccalparraw.h"
#include "hmdctdcthreshold.h"
#include "hmdctdcchannel.h"
#include "hmdccal2parsim.h"
#include "hmdccelleff.h"
#include "hmdclookupgeom.h"
#include "hmdclayergeompar.h"
#include "hmdcgeompar.h"
#include "hmdctimecut.h"
#include <iostream> 
#include <iomanip>


ClassImp(HMdcParRootFileIo)

HMdcParRootFileIo::HMdcParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f) {
  // constructor sets the name of the detector I/O "HMdcParIo"
  fName="HMdcParIo";
  initModules=new TArrayI(24);
  initModuleTypes=new TArrayI(4);
}


HMdcParRootFileIo::~HMdcParRootFileIo() {
  // destructor
  if (modulesFound) {
    delete modulesFound;
    modulesFound=0;
  }
  if (initModules) {
    delete initModules;
    initModules=0;
  }
  if (initModuleTypes) {
    delete initModuleTypes;
    initModuleTypes=0;
  }
}


Bool_t HMdcParRootFileIo::init(HParSet* pPar,Int_t* set) {
  // initializes a container called by name, but only the modules
  // defined in the array 'set'
  // calls the special read function for this container
  // If it is called the first time it reads the setup found in the file
  if (!isActiv) readModules("Mdc");
  const Text_t* name=pPar->GetName();
  if (pFile) {
    if (strcmp(name,"MdcRawStruct")==0) return read((HMdcRawStruct*)pPar,set);
    if (strcmp(name,"MdcGeomStruct")==0) return read((HMdcGeomStruct*)pPar,set);
    if (strncmp(name,"MdcCalParRaw",strlen("MdcCalParRaw"))==0) return read((HMdcCalParRaw*)pPar,set);
    if (strncmp(name,"MdcTdcThreshold",strlen("MdcTdcThreshold"))==0) return read((HMdcTdcThreshold*)pPar,set);
    if (strncmp(name,"MdcTdcChannel",strlen("MdcTdcChannel"))==0) return read((HMdcTdcChannel*)pPar,set);
    if (strcmp(name,"MdcCal2ParSim")==0) return read((HMdcCal2ParSim*)pPar,set);
    if (strncmp(name,"MdcCellEff",strlen("MdcCellEff"))==0) return read((HMdcCellEff*)pPar,set);
    if (strncmp(name,"MdcTimeCut",strlen("MdcTimeCut"))==0) return read((HMdcTimeCut*)pPar,set);
    if (strcmp(name,"MdcLookupGeom")==0) return read((HMdcLookupGeom*)pPar,set);
    if (strcmp(name,"MdcLayerGeomPar")==0) return read((HMdcLayerGeomPar*)pPar,set);
    if (strcmp(name,"MdcGeomPar")==0) return HDetParRootFileIo::read((HMdcGeomPar*)pPar,set);
  }
  Error("HMdcParRootFileIo::init()","initialization of %s not possible from ROOT file!",name);
  return kFALSE;
}


Bool_t HMdcParRootFileIo::read(HMdcRawStruct* pPar,Int_t* set) {
  // reads and fills the container "MdcRawStruct"
  // ( names of the motherboards (characterizes the position) and
  //   the maximum number of TDCs on each of it )
  Text_t* name=(Char_t*)pPar->GetName();
  Int_t version=getMaxVersion(name);
  if (version<=0)
  {
      pPar->setInputVersion(-1,inputNumber);
      return kFALSE;
  }    // not in ROOT file
  if (pPar->getInputVersion(inputNumber)==version
        && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HMdcRawStruct* r=(HMdcRawStruct*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();
  HMdcRawStruct& rMdc=*r;
  HMdcRawStruct& pMdc=*pPar;
  for(Int_t s=0;s<pMdc.getSize();s++) {
    HMdcRawSecStru& rSec=rMdc[s];
    HMdcRawSecStru& pSec=pMdc[s];
    for(Int_t m=0;m<pSec.getSize();m++) {
      HMdcRawModStru& rMod=rSec[m];
      HMdcRawModStru& pMod=pSec[m];
      pos=s*4+m;
      if (set[pos]) {
        if (modulesFound->At(pos)) {
          for(Int_t n=0;n<pMod.getSize();n++) {
            pMod[n].SetName(rMod[n].GetName());
            pMod[n].setNTdcs(rMod[n].getNTdcs());
          }
          set[pos]=0;
          initModules->AddAt(pos+1,pos);
        } else allFound=kFALSE;
      }
    }
  }
  pPar->setInputVersion(version,inputNumber);
  pPar->setChanged();

  Char_t message[200];
  sprintf(message,"%s%s",name,": module(s) initialized from Root file ");
  printInfo(message);

  delete r;
  return allFound;
}


Bool_t HMdcParRootFileIo::read(HMdcGeomStruct* pPar,Int_t* set) {
  // reads and fills the container "MdcGeomStruct"
  // (number of the cells in each layer)
  Text_t* name=(Char_t*)pPar->GetName();
  Int_t version=getMaxVersion(name);
   if (version<=0)
  {
      pPar->setInputVersion(-1,inputNumber);
      return kFALSE;
  }    // not in ROOT file
  if (pPar->getInputVersion(inputNumber)==version
        && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HMdcGeomStruct* r=(HMdcGeomStruct*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();
  HMdcGeomStruct& pP=*pPar;
  HMdcGeomStruct& rP=*r;
  for(Int_t i=0;i<pP.getSize();i++) {
    HMdcGeomSecStru& pSec=pP[i];
    HMdcGeomSecStru& rSec=rP[i];
    for(Int_t j=0;j<pSec.getSize();j++) {
      pos=i*4+j;
      if (set[pos]) {
        if (modulesFound->At(pos)) {
          HMdcGeomModStru& pMod=pSec[j];
          HMdcGeomModStru& rMod=rSec[j];
          for(Int_t k=0;k<pMod.getSize();k++) pMod.fill(k,rMod[k]);
          set[pos]=0;
          initModules->AddAt(pos+1,pos);
        } else allFound=kFALSE;
      }
    }
  }
  pPar->setInputVersion(version,inputNumber);
  pPar->setChanged();

  Char_t message[200];
  sprintf(message,"%s%s",name,": module(s) initialized from Root file ");
  printInfo(message);

  delete r;
  return allFound;
}


Bool_t HMdcParRootFileIo::read(HMdcCalParRaw* pCalPar,Int_t* set) {
  // reads and fills the container "MdcCalParRaw" for calibration parameters
  // on raw level
  Text_t* name=(Char_t*)pCalPar->GetName();
  Int_t version=findInputVersion(name);
   if (version<=0)
  {
      pCalPar->setInputVersion(-1,inputNumber);
      return kFALSE;
  }    // not in ROOT file
  if (pCalPar->getInputVersion(inputNumber)==version
        && pCalPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HMdcCalParRaw* r=(HMdcCalParRaw*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();
  HMdcCalParRaw& rMdc=*r;
  HMdcCalParRaw& pMdc=*pCalPar;
  for(Int_t s=0;s<pMdc.getSize();s++) {
    HMdcCalParRawSec& rSec=rMdc[s];
    HMdcCalParRawSec& pSec=pMdc[s];
    for(Int_t m=0;m<pSec.getSize();m++) {
      HMdcCalParRawMod& rMod=rSec[m];
      HMdcCalParRawMod& pMod=pSec[m];
      pos=s*4+m;
      if (set[pos]) {
        for(Int_t l=0;l<pMod.getSize();l++) {
          HMdcCalParMbo& rMbo=rMod[l];
          HMdcCalParMbo& pMbo=pMod[l];
          Int_t pMboSize=pMbo.getSize();
          if (pMboSize>0) {
            if (rMbo.getSize()==pMboSize) {
              for(Int_t c=0;c<pMboSize;c++) pMbo[c].fill(rMbo[c]);
              set[pos]=0;
              initModules->AddAt(pos+1,pos);
            } else allFound=kFALSE;
          }
        }
      }
    }
  }
  pCalPar->setInputVersion(version,inputNumber);
  pCalPar->setChanged();

  Char_t message[200];
  sprintf(message,"%s%s",name,": module(s) initialized from Root file ");
  printInfo(message);

  delete r;
  return allFound;
}

Bool_t HMdcParRootFileIo::read(HMdcTdcThreshold* pCalPar,Int_t* set) {
  // reads and fills the container "MdcTdcThreshold" for threshold parameters of Mdc
  Text_t* name=(Char_t*)pCalPar->GetName();
  Int_t version=findInputVersion(name);
   if (version<=0)
  {
      pCalPar->setInputVersion(-1,inputNumber);
      return kFALSE;
  }    // not in ROOT file
  if (pCalPar->getInputVersion(inputNumber)==version
        && pCalPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HMdcTdcThreshold* r=(HMdcTdcThreshold*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();
  HMdcTdcThreshold& rMdc=*r;
  HMdcTdcThreshold& pMdc=*pCalPar;
  for(Int_t s=0;s<pMdc.getSize();s++) {
    HMdcTdcThresholdSec& rSec=rMdc[s];
    HMdcTdcThresholdSec& pSec=pMdc[s];
    for(Int_t m=0;m<pSec.getSize();m++) {
      HMdcTdcThresholdMod& rMod=rSec[m];
      HMdcTdcThresholdMod& pMod=pSec[m];
      pos=s*4+m;
      if (set[pos]) {
        for(Int_t l=0;l<pMod.getSize();l++) {
          HMdcTdcThresholdMbo& rMbo=rMod[l];
          HMdcTdcThresholdMbo& pMbo=pMod[l];
          Int_t pMboSize=pMbo.getSize();
          if (pMboSize>0) {
            if (rMbo.getSize()==pMboSize) {
              for(Int_t c=0;c<pMboSize;c++) pMbo[c].fill(rMbo[c]);
              set[pos]=0;
              initModules->AddAt(pos+1,pos);
            } else allFound=kFALSE;
          }
        }
      }
    }
  }
  pCalPar->setInputVersion(version,inputNumber);
  pCalPar->setChanged();

  Char_t message[200];
  sprintf(message,"%s%s",name,": module(s) initialized from Root file ");
  printInfo(message);

  delete r;
  return allFound;
}

Bool_t HMdcParRootFileIo::read(HMdcTdcChannel* pCalPar,Int_t* set) {
  // reads and fills the container "MdcTdcChannel" for threshold parameters of Mdc
  Text_t* name=(Char_t*)pCalPar->GetName();
  Int_t version=findInputVersion(name);
   if (version<=0)
  {
      pCalPar->setInputVersion(-1,inputNumber);
      return kFALSE;
  }    // not in ROOT file
  if (pCalPar->getInputVersion(inputNumber)==version
        && pCalPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HMdcTdcChannel* r=(HMdcTdcChannel*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();
  HMdcTdcChannel& rMdc=*r;
  HMdcTdcChannel& pMdc=*pCalPar;
  for(Int_t s=0;s<pMdc.getSize();s++) {
    HMdcTdcChannelSec& rSec=rMdc[s];
    HMdcTdcChannelSec& pSec=pMdc[s];
    for(Int_t m=0;m<pSec.getSize();m++) {
      HMdcTdcChannelMod& rMod=rSec[m];
      HMdcTdcChannelMod& pMod=pSec[m];
      pos=s*4+m;
      if (set[pos]) {
        for(Int_t l=0;l<pMod.getSize();l++) {
          HMdcTdcChannelMbo& rMbo=rMod[l];
          HMdcTdcChannelMbo& pMbo=pMod[l];
          Int_t pMboSize=pMbo.getSize();
          if (pMboSize>0) {
            if (rMbo.getSize()==pMboSize) {
              for(Int_t c=0;c<pMboSize;c++) pMbo[c].fill(rMbo[c]);
              set[pos]=0;
              initModules->AddAt(pos+1,pos);
            } else allFound=kFALSE;
          }
        }
      }
    }
  }
  pCalPar->setInputVersion(version,inputNumber);
  pCalPar->setChanged();

  Char_t message[200];
  sprintf(message,"%s%s",name,": module(s) initialized from Root file ");
  printInfo(message);

  delete r;
  return allFound;
}

Bool_t HMdcParRootFileIo::read(HMdcTimeCut* pTimeCut,Int_t* set) {
  // reads and fills the container "MdcTimeCut" for time cut parameters
  Text_t* name=(Char_t*)pTimeCut->GetName();
  Int_t version=findInputVersion(name);
   if (version<=0)
  {
      pTimeCut->setInputVersion(-1,inputNumber);
      return kFALSE;
  }    // not in ROOT file
  if (pTimeCut->getInputVersion(inputNumber)==version
        && pTimeCut->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HMdcTimeCut* r=(HMdcTimeCut*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();

  HMdcTimeCut& rMdc=*r;
  HMdcTimeCut& pMdc=*pTimeCut;

  for(Int_t s=0;s<pMdc.getSize();s++) {
    HMdcTimeCutSec& rSec=rMdc[s];
    HMdcTimeCutSec& pSec=pMdc[s];
    for(Int_t m=0;m<pSec.getSize();m++) {
      HMdcTimeCutMod& rMod=rSec[m];
      HMdcTimeCutMod& pMod=pSec[m];
      pos=s*4+m;
      if (set[pos]) {
	    pMod.fill(rMod);
            set[pos]=0;
            initModules->AddAt(pos+1,pos);
      }
    }
  }
  pTimeCut->setInputVersion(version,inputNumber);
  pTimeCut->setChanged();

  Char_t message[200];
  sprintf(message,"%s%s",name,": module(s) initialized from Root file ");
  printInfo(message);

  delete r;
  return allFound;
}
Bool_t HMdcParRootFileIo::read(HMdcCal2ParSim* pCalPar,Int_t* set) {
  // reads and fills the container "MdcCal2ParSim" for calibration parameters
  Text_t* name=(Char_t*)pCalPar->GetName();
  Int_t version=findInputVersion(name);
   if (version<=0)
  {
      pCalPar->setInputVersion(-1,inputNumber);
      return kFALSE;
  }    // not in ROOT file
  if (pCalPar->getInputVersion(inputNumber)==version
        && pCalPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HMdcCal2ParSim* r=(HMdcCal2ParSim*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();
  HMdcCal2ParSim& rMdc=*r;
  HMdcCal2ParSim& pMdc=*pCalPar;
  for(Int_t s=0;s<pMdc.getSize();s++) {
    HMdcCal2ParSecSim& rSec=rMdc[s];
    HMdcCal2ParSecSim& pSec=pMdc[s];
    for(Int_t m=0;m<pSec.getSize();m++) {
      HMdcCal2ParModSim& rMod=rSec[m];
      HMdcCal2ParModSim& pMod=pSec[m];
      pos=s*4+m;
      if (set[pos]) {
        for(Int_t l=0;l<18;l++) {
          HMdcCal2ParAngleSim& rAngle=rMod[l];
          HMdcCal2ParAngleSim& pAngle=pMod[l];
            pAngle.fillTime1(rAngle);
            pAngle.fillTime2(rAngle);
            pAngle.fillTime1Error(rAngle);
            pAngle.fillTime2Error(rAngle);
	    set[pos]=0;
            initModules->AddAt(pos+1,pos);
        }
      }
    }
  }
  pCalPar->setInputVersion(version,inputNumber);
  pCalPar->setChanged();

  Char_t message[200];
  sprintf(message,"%s%s",name,": module(s) initialized from Root file ");
  printInfo(message);

  delete r;
  return allFound;
}

Bool_t HMdcParRootFileIo::read(HMdcCellEff* pCellEff,Int_t* set) {
  // reads and fills the container "MdcCellEffPar" for efficiency cuts of the digitizer
  Text_t* name=(Char_t*)pCellEff->GetName();
  Int_t version=findInputVersion(name);
   if (version<=0)
  {
      pCellEff->setInputVersion(-1,inputNumber);
      return kFALSE;
  }    // not in ROOT file
  if (pCellEff->getInputVersion(inputNumber)==version
        && pCellEff->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HMdcCellEff* r=(HMdcCellEff*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModuleTypes->Reset();
  HMdcCellEff& rMdc=*r;
  HMdcCellEff& pMdc=*pCellEff;
  for(Int_t m=0;m<pMdc.getSize();m++) {
      HMdcCellEffMod& rMod=rMdc[m];
      HMdcCellEffMod& pMod=pMdc[m];
      pos=m;
      if (set[pos]) {
	  for(Int_t l=0;l<18;l++) {
	      HMdcCellEffAngle& rAngle=rMod[l];
	      HMdcCellEffAngle& pAngle=pMod[l];
	      pAngle.fill(rAngle);
	      set[pos]=0;
	      initModuleTypes->AddAt(pos+1,pos);
	  }
      }
  }
  pCellEff->setInputVersion(version,inputNumber);
  pCellEff->setChanged();

  Char_t message[200];
  sprintf(message,"%s%s",name,": module(s) initialized from Root file ");
  printInfo(message);
  delete r;
  return allFound;
}

Bool_t HMdcParRootFileIo::read(HMdcLookupGeom* pPar,Int_t* set) {
  // reads and fills the container "MdcLookupGeom" for mapping of
  Text_t* name=(Char_t*)pPar->GetName();
  Int_t version=findInputVersion(name);
  if (version<=0)
  {
      pPar->setInputVersion(-1,inputNumber);
      return kFALSE;
  }    // not in ROOT file
  if (pPar->getInputVersion(inputNumber)==version
        && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HMdcLookupGeom* r=(HMdcLookupGeom*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();
  HMdcLookupGeom& rMdc=*r;
  HMdcLookupGeom& pMdc=*pPar;
  for(Int_t s=0;s<pMdc.getSize();s++) {
    HMdcLookupGSec& rSec=rMdc[s];
    HMdcLookupGSec& pSec=pMdc[s];
    for(Int_t m=0;m<pSec.getSize();m++) {
      HMdcLookupGMod& rMod=rSec[m];
      HMdcLookupGMod& pMod=pSec[m];
      pos=s*4+m;
      if (set[pos]) {
        for(Int_t l=0;l<pMod.getSize();l++) {
          HMdcLookupMoth& rMoth=rMod[l];
          HMdcLookupMoth& pMoth=pMod[l];
          Int_t pMothSize=pMoth.getSize();
          if (pMothSize>0) {
            if (rMoth.getSize()==pMothSize) {
              for(Int_t c=0;c<pMothSize;c++) pMoth[c].fill(rMoth[c]);
              set[pos]=0;
              initModules->AddAt(pos+1,pos);
            } else allFound=kFALSE;
          }
        }
      }
    }
  }
  pPar->setInputVersion(version,inputNumber);
  pPar->setChanged();

  Char_t message[200];
  sprintf(message,"%s%s",name,": module(s) initialized from Root file ");
  printInfo(message);

  delete r;
  return allFound;
}

Bool_t HMdcParRootFileIo::read(HMdcLayerGeomPar* pPar,Int_t* set) {
  // reads and fills the container "MdcLayerGeomPar"
  Text_t* name=(Char_t*)pPar->GetName();
  Int_t version=findInputVersion(name);
   if (version<=0)
  {
      pPar->setInputVersion(-1,inputNumber);
      return kFALSE;
  }    // not in ROOT file
  if (pPar->getInputVersion(inputNumber)==version
      && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HMdcLayerGeomPar* r=(HMdcLayerGeomPar*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();
  HMdcLayerGeomPar& rMdc=*r;
  HMdcLayerGeomPar& pMdc=*pPar;
  for(Int_t s=0;s<6;s++) {
    HMdcLayerGeomParSec& rSec=rMdc[s];
    HMdcLayerGeomParSec& pSec=pMdc[s];
    for(Int_t m=0;m<4;m++) {
      HMdcLayerGeomParMod& rMod=rSec[m];
      HMdcLayerGeomParMod& pMod=pSec[m];
      pos=s*4+m;
      if (set[pos]) {
        Int_t pModSize=pMod.getSize();
        if (pModSize && rMod.getSize()==pModSize) {
          for(Int_t l=0;l<pModSize;l++) pMod[l].fill(rMod[l]);
          set[pos]=0;
          initModules->AddAt(pos+1,pos);
        } else allFound=kFALSE;
      }
    }
  }
  pPar->setInputVersion(version,inputNumber);
  pPar->setChanged();

  Char_t message[200];
  sprintf(message,"%s%s",name,": module(s) initialized from Root file ");
  printInfo(message);

  delete r;
  return allFound;
}
