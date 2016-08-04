#include "hgeominterface.h"
#include "hgeomasciiio.h"
#include "hgeommedia.h"
#include "hgeommedium.h"
#include "hgeomset.h"
#include "hgeomnode.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "hdetector.h"
#include "hfwdetdetector.h"
#include "hparasciifileio.h"
#include "hfwdetgeompar.h"
#include "hgeomcompositevolume.h"

#include "TString.h"
#include "TFile.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

Int_t fill_fwdetgeompar() {

  // Geant config file
  TString configFile = "/misc/ilse/svn/hydra2-fwdet/fwdet/macros/geom/geainifwdettest.dat";

  // Parameter output ASCII file
  TString paramFile = "fwdetgeompar.txt";

  // run id for initialization
  Int_t runId = 14500;        // fwdet16sim_mediumfieldalign_pimc

  // module setup of Forward Detector
  Int_t modSetup[9]= {1,1,0,0,1,0,1,0,0};

  //-------------------------------------------------------------------------------------

  // Read the Geant config file
  HGeomInterface* interface=new HGeomInterface;
  HGeomSet* pSet = 0;
  Bool_t rc = interface->readGeomConfig(configFile.Data());
  if (!rc) printf("Read of GEANT config file failed!\n");

  // Read the full geometry from the files and find the Forward Detector
  if (rc) rc=interface->readAll();
  if (rc) pSet = interface->findSet("fwdet");
  if (!pSet) {
    printf("Read of geometry failed!\n");
    return 0;
  }

  // Create Hades
  Hades* myHades = new Hades;
  HRuntimeDb* rtdb = gHades->getRuntimeDb();
  HSpectrometer* spec = gHades->getSetup();

  // Define the detector and the detector setup
  HFwDetDetector* fwdet = new HFwDetDetector;
  fwdet->setModules(-1,modSetup);
  spec->addDetector(fwdet);

  // Parameter output
  HParAsciiFileIo* output=new HParAsciiFileIo;
  output->open(paramFile.Data(),"out");
  rtdb->setOutput(output);

  // Create the parameter container
  HFwDetGeomPar* pPar = (HFwDetGeomPar*)(rtdb->getContainer("FwDetGeomPar"));
  if (!pPar) {
    printf("Creation of geometry parameter container failed!\n");
    return 0;
  }

  // Loop on GEANT geometry of the Forward Detector and fill the parameter container  
  TList* volList=pSet->getListOfVolumes();
  TListIter iter(volList);
  HGeomNode* node;
  Int_t n=0;
  HGeomTransform transform;
  while(rc&&(node=(HGeomNode*)iter.Next())) {
    n++;
    TString volName=node->GetName();
    TString moName=(node->getMother());
    TString shape=node->getShape();
    HGeomVolume* volume = 0;
    if (node->isModule()) {
      Int_t nMod = pPar->getModNumInMod(volName);
      HModGeomPar* mod=0;
      if (modSetup[nMod] && (mod=pPar->getModule(nMod))) {
        mod->SetName(volName);
        mod->setRefName(volName);
        transform = node->getTransform();
        mod->getLabTransform().setTransform(transform);
        HGeomCompositeVolume* refMod = pPar->getRefVolume(nMod);
        if (refMod==0) {
          Int_t num = pPar->getNumComponents();
          refMod = new HGeomCompositeVolume(num);
          pPar->addRefVolume(refMod,nMod);
        }
        mod->setVolume(refMod);
        volume = refMod;      
      }
    } else {
      if (node->isSensitive()) {
        Int_t nMod = pPar->getModNumInComp(volName);
        HGeomCompositeVolume* refMod = pPar->getRefVolume(nMod);
        Int_t nCell = 0;
        if (isdigit(volName[3])) {
          nCell = (Int_t)(volName[3]-'0')-1;
	} else {
          nCell = node->getCopyNo()-1;
        }
        volume=refMod->getComponent(nCell);
        HGeomNode* currNode = node;
        transform = node->getTransform();
        HGeomNode* pm;
        while (currNode) {
          pm = currNode->getMotherNode();
          if (!pm) {
            printf("Mother volume of %s not found!", pm->GetName());
            return 0;
          }
          if (pm->isModule()) break;
          transform.transFrom(pm->getTransform());
        }
      }
    }
    if (volume) {
      volume->SetName(volName);
      volume->setShape(shape);
      volume->setMother(moName);
      Int_t nPoints=node->getNumPoints();
      volume->createPoints(nPoints);
      for (Int_t i=0;i<nPoints;i++) volume->setPoint(i,*(node->getPoint(i)));
       volume->getTransform().setTransform(transform);
    }
  }
  //  pPar->printParam();


  // Needed to write output via the runtime database
  rtdb->addRun(runId);
  pPar->setChanged(kTRUE);
  pPar->setInputVersion(1,1);

  delete interface;
  delete myHades;
  return 1;
}
