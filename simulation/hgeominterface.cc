//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/03 by Ilse Koenig
//*-- Modified : 09/05/2006 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HGeomInterface
//
// Manager class for geometry used by simulation
//
///////////////////////////////////////////////////////////////////////////////

using namespace std;
#include <iostream>
#include <iomanip>
#include <time.h>
#include "hgeominterface.h"
#include "hgeomio.h"
#include "hgeomasciiio.h"
#include "hgeommedia.h"
#include "hgeomshapes.h"
#include "hgeomset.h"
#include "hgeomhit.h"
#include "hgeombuilder.h"
#include "hgeomcave.h"
#include "hgeomsectors.h"
#include "hgeommdc.h"
#include "hgeomcoils.h"
#include "hgeomtof.h"
#include "hgeomrich.h"
#include "hgeomshower.h"
#include "hgeomframes.h"
#include "hgeomtarget.h"
#include "hgeomstart.h"
#include "hgeomfwall.h"
#include "hgeomrpc.h"
#include "hgeomemc.h"
#include "hgeomfwdet.h"
#include "hgeomuser.h"
#include "hgeomnode.h"
#include "hspecgeompar.h"
#include "hdetgeompar.h"
#include "TClass.h"

ClassImp(HGeomInterface)

HGeomInterface::HGeomInterface() {
  // Constructor
  fileInput=0;
  oraInput=0;
  output=0;
  media=new HGeomMedia();
  shapes=new HGeomShapes();
  masterNodes=new TList();
  nSets=15;
  sets=new TObjArray(nSets);
  nActualSets=0;
  nFiles=0;
  geoBuilder=0;
}

HGeomInterface::~HGeomInterface() {
  // Destructor
  if (fileInput) {
    delete fileInput;
    fileInput=0;
  }
  if (oraInput) {
    delete oraInput;
    oraInput=0;
  }
  if (output) {
    delete output;
    output=0;
  }
  if (geoBuilder) {
    delete geoBuilder;
    geoBuilder=0;
  }
  if (masterNodes) {
    masterNodes->Delete();
    delete masterNodes;
    masterNodes=0;
  }
  if (sets) {
    sets->Delete();
    delete sets;
    sets=0;
  }
  if (media) {
    delete media;
    media=0;
  }
  if (shapes) {
    delete shapes;
    shapes=0;
  }
}

void HGeomInterface::addInputFile(const Char_t* file) {
  // Adds the input file and creates the corresponding detector object
  HGeomSet* pSet=0;
  TString setName(file);
  setName.ToLower();
  Int_t l=setName.Last('/')+1;
  setName=setName(l,setName.Length()-l);

  if (setName.Contains("media")) {
    media->setInputFile(file);
  } else {
    if (setName.Contains("cave")) {
      pSet=(HGeomSet*)sets->At(kHGeomCave);
      if (!pSet) {
        pSet=new HGeomCave;
        sets->AddAt(pSet,kHGeomCave);
      }
    } else if (setName.Contains("sect")) {
      pSet=(HGeomSet*)sets->At(kHGeomSect);
      if (!pSet) {
        pSet=new HGeomSectors;
        sets->AddAt(pSet,kHGeomSect);
      }
    } else if (setName.Contains("rich")) {
      pSet=(HGeomSet*)sets->At(kHGeomRich);
      if (!pSet&&!setName.Contains(".hit")) {
        pSet=new HGeomRich;
        sets->AddAt(pSet,kHGeomRich);
      }
    } else if (setName.Contains("target")) {
      pSet=(HGeomSet*)sets->At(kHGeomTarget);
      if (!pSet) {
        pSet=new HGeomTarget;
        sets->AddAt(pSet,kHGeomTarget);
      }
    } else if (setName.Contains("mdc")) {
      pSet=(HGeomSet*)sets->At(kHGeomMdc);
      if (!pSet&&!setName.Contains(".hit")) {
        pSet=new HGeomMdc;
        sets->AddAt(pSet,kHGeomMdc);
      }
    } else if (setName.Contains("tof")) {
      pSet=(HGeomSet*)sets->At(kHGeomTof);
      if (!pSet&&!setName.Contains(".hit")) {
        pSet=new HGeomTof;
        sets->AddAt(pSet,kHGeomTof);
      }
    } else if (setName.Contains("shower")) {
      pSet=(HGeomSet*)sets->At(kHGeomShower);
      if (!pSet&&!setName.Contains(".hit")) {
        pSet=new HGeomShower;
        sets->AddAt(pSet,kHGeomShower);
      }
    } else if (setName.Contains("coils")) {
      pSet=(HGeomSet*)sets->At(kHGeomCoils);
      if (!pSet) {
        pSet=new HGeomCoils;
        sets->AddAt(pSet,kHGeomCoils);
      }
    } else if (setName.Contains("frames")) {
      pSet=(HGeomSet*)sets->At(kHGeomFrames);
      if (!pSet) {
        pSet=new HGeomFrames;
        sets->AddAt(pSet,kHGeomFrames);
      }
    } else if (setName.Contains("start")) {
      pSet=(HGeomSet*)sets->At(kHGeomStart);
      if (!pSet&&!setName.Contains(".hit")) {
        pSet=new HGeomStart;
        sets->AddAt(pSet,kHGeomStart);
      }
    } else if (setName.Contains("wall")) {
     pSet=(HGeomSet*)sets->At(kHGeomFWall);
      if (!pSet&&!setName.Contains(".hit")) {
        pSet=new HGeomFWall;
        sets->AddAt(pSet,kHGeomFWall);
      }
    } else if (setName.Contains("rpc")) {
      pSet=(HGeomSet*)sets->At(kHGeomRpc);
      if (!pSet&&!setName.Contains(".hit")) {
        pSet=new HGeomRpc;
        sets->AddAt(pSet,kHGeomRpc);
      }
    } else if (setName.Contains("emc")) {
      pSet=(HGeomSet*)sets->At(kHGeomEmc);
      if (!pSet&&!setName.Contains(".hit")) {
        pSet=new HGeomEmc;
        sets->AddAt(pSet,kHGeomEmc);
      }
    } else if (setName.Contains("fwdet")) {
      pSet=(HGeomSet*)sets->At(kHGeomFwDet);
      if (!pSet&&!setName.Contains(".hit")) {
        pSet=new HGeomFwDet;
        sets->AddAt(pSet,kHGeomFwDet);
      }
    } else if (setName.Contains("user")) {
      pSet=(HGeomSet*)sets->At(kHGeomUser);
      if (!pSet&&!setName.Contains(".hit")) {
        pSet=new HGeomUser;
        sets->AddAt(pSet,kHGeomUser);
      }
    } else Error("addInputFile","Unknown detector set: %s",file);
    if (pSet) {
      pSet->setShapes(shapes);
      pSet->setMasterNodes(masterNodes);
      if (setName.Contains(".hit")) pSet->setHitFile(file);
      else {
        if (strlen(pSet->getGeomFile())==0) pSet->setGeomFile(file);
	nActualSets++;
      }
    }
  }
  if (setName.Contains(".geo")||setName.Contains(".hit")) nFiles++;
}

HGeomSet* HGeomInterface::findSet(const Char_t* name) {
  // Find the detector set in the actual setup
  HGeomSet* pSet=0;
  TString setName(name);
  setName.ToLower();
  Int_t l=setName.Last('/')+1;
  setName=setName(l,setName.Length()-l);
  if (setName.Contains("cave"))        pSet=(HGeomSet*)sets->At(kHGeomCave);
  else if (setName.Contains("sect"))   pSet=(HGeomSet*)sets->At(kHGeomSect);
  else if (setName.Contains("rich"))   pSet=(HGeomSet*)sets->At(kHGeomRich);
  else if (setName.Contains("target")) pSet=(HGeomSet*)sets->At(kHGeomTarget);
  else if (setName.Contains("mdc"))    pSet=(HGeomSet*)sets->At(kHGeomMdc);
  else if (setName.Contains("tof"))    pSet=(HGeomSet*)sets->At(kHGeomTof);
  else if (setName.Contains("shower")) pSet=(HGeomSet*)sets->At(kHGeomShower);
  else if (setName.Contains("coils"))  pSet=(HGeomSet*)sets->At(kHGeomCoils);
  else if (setName.Contains("frames")) pSet=(HGeomSet*)sets->At(kHGeomFrames);
  else if (setName.Contains("start"))  pSet=(HGeomSet*)sets->At(kHGeomStart);
  else if (setName.Contains("wall"))   pSet=(HGeomSet*)sets->At(kHGeomFWall);
  else if (setName.Contains("rpc"))    pSet=(HGeomSet*)sets->At(kHGeomRpc);
  else if (setName.Contains("emc"))    pSet=(HGeomSet*)sets->At(kHGeomEmc);
  else if (setName.Contains("fwdet"))  pSet=(HGeomSet*)sets->At(kHGeomFwDet);
  else if (setName.Contains("user"))   pSet=(HGeomSet*)sets->At(kHGeomUser);
  return pSet;
}

Bool_t HGeomInterface::readSet(HGeomSet* pSet) {
  // Reads the geometry for the detector part
  Bool_t rc=kFALSE;
  HGeomIo* input=0;
  if (pSet) {
    input=connectInput(pSet->getGeomFile());
    if (input) rc=input->read(pSet,media);
    HGeomHit* pHit=pSet->getHit();
    if (rc&&pHit) {
      TString f=pHit->getHitFile();
      if (f.Length()>0) input=connectInput(pHit->getHitFile());
      else input=connectInput("_gdb");
      if (input) rc=input->read(pHit);
    }
  }
  return rc;
}

Bool_t HGeomInterface::writeSet(HGeomSet* pSet) {
  // Writes the geometry for the detector part to output
  Bool_t rc=kFALSE;
  if (pSet&&connectOutput(pSet->GetName())) {
    rc=output->write(pSet);
    HGeomHit* pHit=pSet->getHit();
    if (rc&&pHit) {
      if (connectOutput(pSet->GetName(),"hit")) rc=output->write(pHit);
    }
  }
  return rc;
}

Bool_t HGeomInterface::writeSet(HGeomSet* pSet, const Char_t* filename) {
  // Writes the geometry for the detector part to file
  if (pSet&&output) {
    output->open(filename,"out");
    if (output->isOpen()) return output->write(pSet);
  }
  return kFALSE;
}

void HGeomInterface::deleteSet(HGeomSet* pSet) {
  // Deletes the set
  if (pSet) {
    sets->Remove(pSet);
    delete pSet;
    pSet=0;
  }
}

Bool_t HGeomInterface::readMedia() {
  // Reads the media
  Bool_t rc=kFALSE;
  HGeomIo* input=connectInput(media->getInputFile());
  if (input) {
    rc=input->read(media);
  }
  return rc;  
}

Bool_t HGeomInterface::writeMedia() {
  // Writes the media to output
  if (connectOutput("media")) return output->write(media);
  return kFALSE;
}

Bool_t HGeomInterface::writeMedia(const Char_t* filename) {
  // Writes the media to file
  if (output) {
    output->open(filename,"out");
    if (output->isOpen()) return output->write(media);
  }
  return kFALSE;
}

Bool_t HGeomInterface::readAll() {
  // Reads the media and the whole geometry and hit definitions
  Bool_t rc=kTRUE;
  if (nFiles==0&&oraInput) {
    rc=oraInput->readGeomConfig(this);
    if (rc) addInputFile("media_gdb");
  } 
  if(rc) rc=readSetupFile();
  if(rc) rc=readMedia();  
  HGeomSet* pSet=0;
  for(Int_t i=0;i<nSets&&rc;i++) {
    pSet=(HGeomSet*)sets->At(i);
    if (pSet) {
      cout<<"Read "<<pSet->GetName()<<endl;
      rc=readSet(pSet);
    }
  }
  return rc;
}

Bool_t HGeomInterface::writeAll() {
  // Reads the media and the whole geometry to output
  Bool_t rc=kTRUE;
  HGeomSet* pSet=0;
  rc=writeMedia();
  for(Int_t i=0;i<nSets&&rc;i++) {
    pSet=(HGeomSet*)sets->At(i);
    if (pSet) rc=writeSet(pSet);
  }
  return rc;
}

Bool_t HGeomInterface::createAll(Bool_t withCleanup) {
  // Creates the whole geometry
  // (if withCleanup==kTRUE, the geometry input is deleted afterwards)
  Bool_t rc=kTRUE;
  Int_t n=0;  
  if (geoBuilder) {
    HGeomSet* pSet=0;
    for(Int_t i=0;i<nSets&&rc;i++) {
      pSet=(HGeomSet*)sets->At(i);
      if (pSet) {
        cout<<"Create "<<pSet->GetName()<<endl;
        rc=pSet->create(geoBuilder, paramFile, media);
        n++;
        if (withCleanup) {
          sets->RemoveAt(i);
          delete pSet;
        }
      }
    }
    if (rc&&n>0) geoBuilder->finalize();
  } else Error("createAll()","No Builder");
  return rc;
}

Bool_t HGeomInterface::createGeometry(Bool_t withCleanup) {
  // Reads and creates the whole geometry
  // (if withCleanup==kTRUE, the geometry input is deleted after creation)
  Bool_t rc=kTRUE;  
  Int_t n=0;  
  if (geoBuilder) {
    if (nFiles==0&&oraInput) {
      rc=oraInput->readGeomConfig(this);
      if (rc) addInputFile("media_gdb");
    }
    if (rc) rc=readSetupFile();
    if (rc) rc=readMedia();
    HGeomSet* pSet=0;
    for(Int_t i=0;i<nSets&&rc;i++) {
      pSet=(HGeomSet*)sets->At(i);
      if (pSet) {
	cout<<"Read and create "<<pSet->GetName()<<endl;
        rc=readSet(pSet);
        if (rc) {
          rc=pSet->create(geoBuilder, paramFile, media);
          n++;
        }
        if (withCleanup) {
          sets->RemoveAt(i);
          delete pSet;
        }
      }
    }
    if (rc&&n>0) geoBuilder->finalize();
  } else Error("createAll()","No Builder");
  return rc;
}

HGeomIo* HGeomInterface::connectInput(const Char_t* filename) {
  // Connects the input (ASCII or Oracle)
  HGeomIo* currentIo=0;
  if (filename) {
    TString s(filename);
    s=s.Strip();
    if (s.EndsWith(".dat")||s.EndsWith(".geo")||s.EndsWith(".hit")
        ||s.EndsWith(".setup")) {
      if (!fileInput) fileInput=new HGeomAsciiIo();
      fileInput->open(filename);
      currentIo=fileInput;
    } else if (s.EndsWith("_gdb")) currentIo=oraInput;
  }
  if (currentIo && currentIo->isOpen()) return currentIo;
  return 0; 
}

Bool_t HGeomInterface::connectOutput (const Char_t* name,TString pType) {
  // Connects the output (ASCII or Oracle)
  if (output) {
    if (strcmp(output->IsA()->GetName(),"HGeomAsciiIo")==0) {
      TString fName(name);
      Char_t buf[80];
      struct tm* newtime;
      time_t t;
      time(&t);
      newtime=localtime(&t);
      if (newtime->tm_mday<10) sprintf(buf,"_0%i",newtime->tm_mday);
      else sprintf(buf,"_%i",newtime->tm_mday);
      fName=fName+buf;
      if (newtime->tm_mon<9) sprintf(buf,"0%i",newtime->tm_mon+1);
      else sprintf(buf,"%i",newtime->tm_mon+1);
      fName=fName+buf;
      Int_t y=newtime->tm_year-100;
      if (y<10) sprintf(buf,"0%i",y);
      else sprintf(buf,"%i",y);
      fName=fName+buf;
      if (newtime->tm_hour<10) sprintf(buf,"0%i",newtime->tm_hour);
      else sprintf(buf,"%i",newtime->tm_hour);
      fName=fName+buf;
      if (newtime->tm_min<10) sprintf(buf,"0%i",newtime->tm_min);
      else sprintf(buf,"%i",newtime->tm_min);
      fName=fName+buf;
      if (newtime->tm_sec<10) sprintf(buf,"0%i",newtime->tm_sec);
      else sprintf(buf,"%i",newtime->tm_sec);
      fName=fName+buf+"."+pType;
      output->open(fName,"out");
      cout<<"Output file for "<<name<<":  "
          <<((HGeomAsciiIo*)output)->getFilename()<<endl;
    }
    if (output->isOpen()&&output->isWritable()) return kTRUE;
  } else Error("connectOutput","No output open");
  return kFALSE;
}

Bool_t HGeomInterface::createSet(HGeomSet* pSet) {
  // Creates the geometry of detector part
  if (!geoBuilder) return kFALSE;
  return pSet->create(geoBuilder, paramFile, media);
}

Bool_t HGeomInterface::addAlignment(HSpecGeomPar* pPar) {
  // Adds the target alignment (not yet implemented for sectors)
  if (!pPar) return kFALSE;
  HGeomSet* pTarget=(HGeomSet*)sets->At(kHGeomTarget);
  HGeomSet* pRich=(HGeomSet*)sets->At(kHGeomRich);
  if (!pTarget||!pRich) {
    Error("addAlignment(HSpecGeomPar*",
          "Target or RICH not found in GEANT geometry!");
    return kFALSE;
  }
  Int_t nTargets=pPar->getNumTargets();
  Double_t meanShiftGeo=0,meanShiftAlign=0,dshift=0;
  Int_t nt=0;
  for (Int_t i=0;i<nTargets;i++) {
    HGeomVolume* vol=pPar->getTarget(i);
    if (vol) {
      HGeomNode* node=pTarget->getVolume(vol->GetName());
      if (!node) {
        Error("addAlignment(HSpecGeomPar*","GEANT Volume %s not found!",
	      vol->GetName()); 
        return kFALSE;
      }
      nt++;
      HGeomTransform& tv=vol->getTransform();
      meanShiftAlign+=tv.getTransVector().getZ();
      HGeomTransform* tn=node->getLabTransform();
      if(!tn) return kFALSE;      
      meanShiftGeo+=tn->getTransVector().getZ();
    }
  }
  dshift=(meanShiftAlign-meanShiftGeo)/nt;
  cout<<"Target alignment: mean shift "<<dshift<<endl;
  HGeomNode* node=pRich->getVolume("RICH");
  HGeomTransform& t=node->getTransform();
  HGeomVector pos(t.getTransVector());
  pos.setZ(t.getTransVector().getZ()+dshift);
  t.setTransVector(pos);
  //  node=pRich->getVolume("RTAM"); 
  //  HGeomNode* rNode=(HGeomNode*)masterNodes->FindObject("RTAM");
  //  HGeomTransform* nTransform=node->getLabTransform();
  //  if (node&&rNode&&nTransform) rNode->setLabTransform(*(nTransform));
/*  Actually Sectors are not changed by the alignment!
  HGeomSet* pSectors=(HGeomSet*)sets->At(kHGeomSect);
  Int_t nSectors=pPar->getNumSectors();
  if (nSectors>0&&!pSectors) {
    Error("addAlignment(HSpecGeomPar*",
          "Sectors not found in GEANT geometry!");
    return kFALSE;
  }
  for (Int_t i=0;i<nSectors;i++) {
    HGeomVolume* vol=pPar->getSector(i);
    if (vol) {
      HGeomNode* node=pSectors->getVolume(vol->GetName());
      if (node) {
        Int_t np=vol->getNumPoints();
        node->createPoints(np);
	for (Int_t i=0;i<np;i++) node->setPoint(i,*(vol->getPoint(i)));
        node->setLabTransform(vol->getTransform());
        HGeomNode* rNode=(HGeomNode*)masterNodes->FindObject(vol->GetName());
        if (rNode) rNode->setLabTransform(vol->getTransform());
      }
    }
  }
*/
  return kTRUE;
}

Bool_t HGeomInterface::addAlignment(HDetGeomPar* pPar) {
  // Adds the detector alignment
  if (!pPar) return kFALSE;
  TString detName=pPar->getDetectorName();
  detName.ToLower();
  HGeomSet* pDet=findSet(detName);
  if (!pDet) {
    Error("addAlignment","Detector %s not defined!",pPar->getDetectorName());
    return kFALSE;
  }
  Int_t nModules=pPar->getNumModules();
  for (Int_t i=0;i<nModules;i++) {
    HModGeomPar* mod=pPar->getModule(i);
    if (mod) {
      HGeomNode* node=pDet->getVolume(mod->GetName());
      if (node) {
        TString mo(node->getMother());
        if (!mo.Contains("CAVE")&&!mo.Contains("SEC"))
          node=pDet->getVolume(mo.Data());  
        node->setLabTransform(mod->getLabTransform());
	//cout<<"Lab Transform"<<endl;
	//node->getLabTransform()->print();
	//cout<<"Local Transform"<<endl;
	//node->getTransform().print();
      }
    }
  }
  cout<<"Alignment for "<<pPar->getDetectorName()<<" added"<<endl;
  return kTRUE;
}

void HGeomInterface::print() {
  // Prints the media and the list of detector parts
  media->list();
  cout<<"********************************************************************\n";
  cout<<"List of detector parts:\n";
  HGeomSet* pSet=0;
  for(Int_t i=0;i<nSets;i++) {
    pSet=(HGeomSet*)sets->At(i);
    if (pSet) cout<<"  "<<pSet->GetName()<<":\t"<<pSet->getGeomFile()<<'\n';
  }
  cout<<"********************************************************************\n";
}

Bool_t HGeomInterface::readGeomConfig(const Char_t* configFile) {
  // Reads the GEANT configuration file
  Bool_t rc=kFALSE;
  HGeomIo* input=0;
  if (configFile) {
    input=connectInput(configFile);
    if (input) rc=input->readGeomConfig(this);
  }
  return rc;
}

Bool_t HGeomInterface::readSetupFile() {
  // Reads the detector setups, needed for create only subsets
  Bool_t rc=kTRUE;
  if (setupFile.Length()>0) {
    HGeomAsciiIo* input=(HGeomAsciiIo*)(connectInput(setupFile));
    if (input) rc=input->readDetectorSetup(this);
  }
  return rc;
}
