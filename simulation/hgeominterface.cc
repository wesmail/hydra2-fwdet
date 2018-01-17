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
  addDateTime = kTRUE;
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

Bool_t HGeomInterface::writeSet(const Char_t* name,const Char_t* author,const Char_t* descr) {
  HGeomSet* set = findSet(name);
  TString v;
  if (set) {
    set->setAuthor(v=author);
    set->setDescription(v=descr);
    return writeSet(set);
  } 
  Error("writeSet","Set %s is not found!",name);
  return kFALSE;
}

Bool_t HGeomInterface::writeMedia(const Char_t* author,const Char_t* descr) {
  TString v;
  if (media != NULL) {
    media->setAuthor(v=author);
    media->setDescription(v=descr);
    return writeMedia();
  } 
  Error("writeMedia","Media is not defined!");
  return kFALSE;
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
      if(addDateTime) {
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
        fName=fName+buf;
      }
      fName=fName+"."+pType;
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
  Double_t shiftFDSR[2] = {1000.,1000.};
  Int_t nModules=pPar->getNumModules();
  for (Int_t i=0;i<nModules;i++) {
    HModGeomPar* mod=pPar->getModule(i);
    if (mod) {
      HGeomNode* node=pDet->getVolume(mod->GetName());
      if (node) {
        TString mo(node->getMother());
        if (!mo.Contains("CAVE")&&!mo.Contains("SEC")) node=pDet->getVolume(mo.Data());  
        HGeomTransform &modtr = mod->getLabTransform();
          
        node->setLabTransform(modtr);
        
        // Correction of MDCIV barrels lab position in according to the mdc alignment:
        if( detName.EqualTo("mdc") ) {
          TString modName(mod->GetName());
          if( modName.BeginsWith("DR4") ) {
            TString bar1name("FDB1");
            TString bar2name("FDB2");
            bar1name += modName[4];  // Sector
            bar2name += modName[4];  // Sector
            HGeomSet*  frames = (HGeomSet*)sets->At(kHGeomFrames);
            if( frames != NULL) {
              HGeomNode* bar1 = frames->getVolume(bar1name.Data());
              HGeomNode* bar2 = frames->getVolume(bar2name.Data());
              bar1->setLabTransform(modtr);
              bar2->setLabTransform(modtr);
            } else Error("addAlignment","No frames loaded!");
          } else if( modName.BeginsWith("DR1") || modName.BeginsWith("DR2")) {
            //  Adjust position of FD1S1...6, FD1R1...6, FD2S1...6, FD2R1...6 according to MDCI,II alignment:
            //  find shift of MDC frustum without overlap with MDC
            Int_t mdcpl = modName.BeginsWith("DR1") ? 0:1;
            
            HGeomVector zVect(0.,0.,1.);                   // direction of z-axis in the lab.system
            zVect = modtr.getRotMatrix().inverse()*zVect;  // rotation of zVect to MDC module coor.sys.
            Double_t A = zVect.getX()/zVect.getZ();
            Double_t B = zVect.getY()/zVect.getZ();
            HGeomVolume* cv = (HGeomVolume*)mod->getRefVolume();
            Double_t zMdc = cv->getPoint(0)->getZ();
            Double_t yMdc = cv->getPoint(0)->getY();
            
            HGeomSet*  frames = (HGeomSet*)sets->At(kHGeomFrames);
            TString FDSname(mdcpl==0 ? "FD1S":"FD2S");
            TString FDRname(mdcpl==0 ? "FD1R":"FD2R");
            FDSname += modName[4];  // Sector
            FDRname += modName[4];  // Sector
            HGeomNode* pFDS = frames->getVolume(FDSname.Data());
            
            Double_t phi  = pFDS->getPoint(1)->getX()*TMath::DegToRad();
            // Points was selected by knowing of MDC frustum volumes.
            // In the case of changing volumes need to be rechecked!
            HGeomVector *par1 = pFDS->getPoint(mdcpl==0 ? 5:4);  // 4
            HGeomVector *par2 = pFDS->getPoint(3);

            HGeomVector point[4];  // Coordinates of points (on frustum volume) which closest TO MDC
            point[0].setXYZ( par1->getZ()/TMath::Tan(phi), par1->getZ(), par1->getX());
            point[1].setXYZ(-par1->getZ()/TMath::Tan(phi), par1->getZ(), par1->getX());
            point[2].setXYZ( par2->getZ()/TMath::Tan(phi), par2->getZ(), par2->getX());
            point[3].setXYZ(-par2->getZ()/TMath::Tan(phi), par2->getZ(), par2->getX());
            for(Int_t p=0;p<4;p++) {
              // Calculate distance from point to the MDC in direction of zVect
              HGeomVector pnt(pFDS->getLabTransform()->transFrom(point[p]));
              pnt = modtr.transTo(pnt);
              Double_t shift = (zMdc-pnt.getZ())*TMath::Sqrt(A*A+B*B+1.);
              if(shift < shiftFDSR[mdcpl]) shiftFDSR[mdcpl] = shift;
            }
            
            
            HGeomNode* pFDR = frames->getVolume(FDRname.Data());
            HGeomVector pnt(0.,pFDR->getPoint(1)->getY(),pFDR->getPoint(2)->getZ());
            pnt = pFDR->getLabTransform()->transFrom(pnt);
            pnt = modtr.transTo(pnt);
            Double_t yCr = pnt.getY() + B*(zMdc-pnt.getZ());
            if(yCr < yMdc) continue;                         // No cross with MDC
            Double_t shift = (zMdc-pnt.getZ())*TMath::Sqrt(A*A+B*B+1.);
            if(shift < 0.) shift -= 0.010;  // -10 mkm for savety rison
            if(shift < shiftFDSR[mdcpl]) shiftFDSR[mdcpl] = shift;
          }
        }
      }
    }
  }
  
  cout<<"Alignment for "<<pPar->getDetectorName()<<" added"<<endl;
  
  if(shiftFDSR[0] < 999.) {
    if(shiftFDSR[0] < 0. || shiftFDSR[0] > 1.) {        // Ignore gap < 1. mm
      shiftNode6sect(kHGeomFrames,"FD1S",shiftFDSR[0]);
      shiftNode6sect(kHGeomFrames,"FD1R",shiftFDSR[0]);
    } else printf("*** FD1S,FD1R - MDCI gap (%f mm) is ignored\n",shiftFDSR[0]);
  }
  if(shiftFDSR[1] < 999.) {
    if(shiftFDSR[1] < 0. || shiftFDSR[1] > 1.) {       // Ignore gap < 1. mm
      shiftNode6sect(kHGeomFrames,"FD2S",shiftFDSR[1]);
      shiftNode6sect(kHGeomFrames,"FD2R",shiftFDSR[1]);
    } else  printf("*** FD2S,FD2R - MDCII gap (%f mm) is ignored\n",shiftFDSR[1]);
  }
  return kTRUE;
}

void HGeomInterface::adjustSecGeom(void) {
  HGeomSet* pRich  = (HGeomSet*)sets->At(kHGeomRich);
  HGeomSet* pSec   = (HGeomSet*)sets->At(kHGeomSect);
  HGeomSet* frames = (HGeomSet*)sets->At(kHGeomFrames);
  if (pRich==NULL || pSec==NULL || frames==NULL) {
    Error("adjustSecGeom","Sector, RICH or frames not found in GEANT geometry!");
    return;
  }
  HGeomNode*      node     = pRich->getVolume("RICH");
  HGeomTransform& t        = node->getTransform();
  Double_t        zRichEnd = t.getTransVector().getZ()+node->getPoint(4)->getX();
  Double_t        zSect    = pSec->getVolume("SEC1")->getPoint(5)->getX();
  Double_t        zShift   = zRichEnd-zSect;  // >0 - overlap
  
  if(zShift<=0. && zShift > -2.0) {        // Ignore gap < 2. mm
    if(zShift < 0.) printf("*** RICH - Sector gap (%f mm) is ignored\n", -zShift);
    return;
  }
  
  for(Int_t s=1;s<=6;s++) {
    TString secNm("SEC");
    secNm += s;
    HGeomVector *p0 = pSec->getVolume(secNm.Data())->getPoint(0);
    Int_t npnt = p0->getX()+1.1;
    for(Int_t ip=3;ip<=npnt;ip++) {    
      HGeomVector *p = pSec->getVolume(secNm.Data())->getPoint(ip);
      p->setX(p->getX()+zShift);
    }
  }
  shiftNode6sect(kHGeomFrames,"FIWL",zShift);
  shiftNode6sect(kHGeomFrames,"FOWL",zShift);
  shiftNode6sect(kHGeomFrames,"FINC",zShift);
  shiftNode6sect(kHGeomFrames,"FOUC",zShift);
  shiftNode(kHGeomFrames,"FINA",zShift);
  shiftNode(kHGeomFrames,"FIWR",zShift);
  shiftNode(kHGeomFrames,"FOTU",zShift);
  shiftNode(kHGeomFrames,"FTWL",zShift);
  shiftNode(kHGeomFrames,"FTWR",zShift);
  // Test veto:
  HGeomSet* start = (HGeomSet*)sets->At(kHGeomStart);
  if(start != NULL && start->getVolume("VVET") != NULL) {
    HGeomNode* pVVSH = frames->getVolume("VVSH");
    if(pVVSH != 0) shiftNode(kHGeomStart,"VVSH",zShift);
    else shiftNode6sect(kHGeomStart,"VSHS",zShift);
  }
}

void HGeomInterface::shiftNode6sect(Int_t set,const Char_t* name, Double_t zShift) {
  for(Int_t s=1;s<=6;s++) {
    TString nm(name);
    nm += s;
    shiftNode(set,nm.Data(),zShift);
  }
}

void HGeomInterface::shiftNode(Int_t set,const Char_t* name, Double_t zShift) {
  HGeomNode* pNode = ((HGeomSet*)sets->At(set))->getVolume(name);
  if(pNode == NULL) {
    printf("Volume %s is absent in the current setup\n",name);
    return;
  }
  HGeomTransform trNode = ((*(pNode->getLabTransform())));
  HGeomVector pos = trNode.getTransVector();
  pos.setZ(pos.getZ() + zShift);
  trNode.setTransVector(pos);
  pNode->setLabTransform(trNode);
  printf("Volume %s shifted by %f mm\n",name,zShift);
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
