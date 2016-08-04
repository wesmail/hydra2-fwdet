#include "htofclusterf.h"
#include "htofcluster.h"
#include "hades.h"
#include "htofhit.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hdebug.h"
#include "tofdef.h"
#include "hevent.h"
#include "heventheader.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hgeomvolume.h"
#include "hgeomcompositevolume.h"
#include "hgeomtransform.h"
#include "htofgeompar.h"
#include "hdetgeompar.h"
#include "hgeomvector.h"
#include "hspecgeompar.h"
#include "htofclusterfpar.h"
#include "TMath.h"
#include <cstring>

//*-- Author :  1/12/2001 D.Zovinec
//*-- Modified: 06/03/2002 D.Zovinec
//*-- Modified: 23/09/2002 D.Zovinec

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////////////
// HTofClusterF Tof cluster finder
//
// Iterates over the hit level of Tof data and finds a cluster-candidates.
// The cluster-candidate is constructed from set of hits when the following
// binary conditions are satisfied for each pair of hits consecutively
// read from HTofHit based category:
//
// 1.) the hits are close to each other (same sector, adjacent rod)
// 2.) time difference of two hits is less than value taken from HTofClusterFPar
// 3.) xposition difference (in module coordinate system) of two hits is
//     less than value taken from HTofClusterFPar
//
// Above binary conditions construct the chain of clustered hits
// from which the output information stored in the HTofCluster category
// is calculated.
//
// The data members inherited from HTofHit class, i.e. tof, xpos, xposAdc,
// lAmp, rAmp, xlab, ylab, zlab is calculated for the cluster here as a weighted
// mean of values appropriate to individual hits participating on cluster-candidate.
// The weight is energy deposited in the rod, i.e. edep.
// The edep of the cluster is sum of the edep's of hits.
// The flagAdc is 2 when all flagAdc flags of appropriate hits
// participating on cluster are = 2. In opposite case flagAdc = 0.
//
// !!! Important: Since the tof rod coordinate systems are shifted each
//                other the weighted mean results concerning xpos data member
//                would be wrong as it is evaluated in HTofHit relative to this
//                reference system. Therefore the appropriate HTofCluster data
//                member is evaluated from the position of individual hits
//                relative to module reference system. The result stored in
//                the HTofCluster is thus given also in module reference system.
//                Therefore do not combine the xpos from HTofHit and HTofCluster
//                objects without appropriate transformation.
//
// Additional data member is evaluated, i.e. cluster-candidate size
// (clustSize see HTofCluster)  that is defined as the number of the hits
// participating on cluster-candidate.
//
// "Simple" hits are also included in the output as the cluster-candidates
// with clustSize = 1.
//
// The probability function for the cluster-candidates of clustSize = 2 with
// tof < tLimit is calculated by using energy deposited of the cluster-candidate.
// Two probability numbers are thus provided, i.e. clustProbAll for
// cluster-candidates related to all ToF hits and clustProbLep for those
// cluster-candidates related to ToF-RICH correlated hits (leptons).
// Both the probability numbers depend on the parameters stored in
// parameter container HTofClusterFPar.
//
//////////////////////////////////////////////////////////////////////////////////////

HTofClusterF::HTofClusterF(void) {
  fHitCat = fClusterCat = NULL;
  fLoc.set(3,0,0,0);
  fpLoc.set(3,0,0,0);
  fCLoc.set(3,0,0,0);
  iterh = NULL;
  iterc = NULL;
}

HTofClusterF::HTofClusterF(const Text_t *name,const Text_t *title) : HReconstructor (name,title) {
  fHitCat = fClusterCat = NULL;
  fLoc.set(3,0,0,0);
  fpLoc.set(3,0,0,0);
  fCLoc.set(3,0,0,0);
  iterh = NULL;
  iterc = NULL;
}

HTofClusterF::~HTofClusterF(void) {
  if(iterh) delete iterh;
  if(iterc) delete iterc;
}

Int_t HTofClusterF::execute(void) {

#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HTofClusterF::execute");
#endif

  Float_t maxTd, maxXd;

  cluster=NULL;
  hit=NULL;
  fpLoc[0] = -1;
  fpLoc[1] = -1;
  fpLoc[2] = -1;
  size_c=1;
  indexHit1=-1;
  indexHit2=-1;

  tof_ph     =0; // in first loop this value has to have default value!
  xposMod_ph =0; // in first loop this value has to have default value!
  iterh->Reset();
  while ( (hit=(HTofHit *)iterh->Next())!=NULL) {
    readHit();
    maxTd=fClusterFPar->getMaxTDiff();
    maxXd=fClusterFPar->getMaxXDiff();
    absTd_h=TMath::Abs(tof_h-tof_ph);
    absXd_h=TMath::Abs(xposMod_h-xposMod_ph);

    if((fLoc[0] == fpLoc[0])&&
       (((fLoc[1]*8)+fLoc[2])==((fpLoc[1]*8)+fpLoc[2]+1))&&
       (absTd_h < maxTd)&&
       (absXd_h < maxXd)){
      if(size_c==1){  // If cluster has been found its location is identical with 1st hit in cluster.
        fCLoc[0]=fpLoc[0];
        fCLoc[1]=fpLoc[1];
	fCLoc[2]=fpLoc[2];

	indexHit1 = fHitCat->getIndex(fpLoc);
        indexHit2 = fHitCat->getIndex(fLoc);


      }
      calcWeightedMean();
      size_c++;
    } else {
      if(size_c > 1){
        writeCluster(1);
      }
      writeCluster(0);
      size_c=1;
      fillClusterData();
    }
    fillPreviousData();
  }
  // if the last hit sentence was cluster than store it
  if(size_c>1){
    writeCluster(1);
  }
  writeProb();

#if DEBUG_LEVEL>2
  gDebuger->leaveFunc("HTofClusterF::execute");
#endif
  return 0;
}

void HTofClusterF::writeProb(void){
// Call the function
// Float_t HTofClusterF::calcProb(Float_t edep, Char_t* aset)
// that calculates probability to be a cluster for cluster-candidates
// of size = 2 and tof < mipLimit stored in HTofClusterFPar.
// Default probabilitites are delivered in this version for
// cluster-candidates of size = 2 that are not in MIP region.
//   clustProbAll = 0.896
//   clustProbLep = 0.948
// The numbers have been evaluated from the simulation.
// Probability is 1 when cluster-candidate size = 1.
// Probability is -1 when cluster-candidate size > 2.
// Probability is -1 when (cluster-candidate size = 2) & (flagAdc != 2)
//

  Float_t probAll_c, probLep_c, dist_c, tofcorr_c, tlimit;

  tlimit=fClusterFPar->getMIPLimit();
  iterc->Reset();
  while ((cluster=(HTofCluster *)iterc->Next())!=NULL) {
    probAll_c=-1.0;
    probLep_c=-1.0;
    size_c=cluster->getClusterSize();
    flagAdc_c=cluster->getAdcFlag();
    if(size_c==1){
      probAll_c=1.0;
      probLep_c=1.0;
    }
    if((size_c==2)&&(flagAdc_c==2)){
      tof_c=cluster->getTof();
      cluster->getDistance(dist_c);
      tofcorr_c = tof_c - (dist_c-2098.5472)/299.792458; // correction on different distances.
      if(tofcorr_c<tlimit){
        edep_c=cluster->getEdep();
        probAll_c=calcProb(edep_c,"all");
        probLep_c=calcProb(edep_c,"lep");
      } else {
        probAll_c=0.896; // hardwired default number from sim
        probLep_c=0.948; // hardwired default number from sim
      }
    }
    cluster->setClusterProbAll(probAll_c);
    cluster->setClusterProbLep(probLep_c);
  }
}

Float_t HTofClusterF::calcProb(Float_t edep,const  Char_t* aset) {
// Calculates the probability for the cluster-candidates of clustSize = 2
// with tof < mipLimit parameter of HTofClusterFPar.
// The probability is evaluated from the energy deposited in the
// cluster-candidate. Evaluation is based on idea that two hits
// caused by two MIP will deposit energy = 2 in ADC calibrated
// spectrum while in case of one incident MIP causing two hits
// the energy deposited is = 1.
// Two probability numbers are stored in the HTofCluster,
// i.e. clustProbAll (evaluated for all hits in ToF)
// and clustProbLep (evaluated for ToF-RICH correlated hits).
// Both the probability numbers depend on the parameters stored
// in HTofClusterFPar that is used here as input.
//

  Float_t sL1=0.0, mL1=0.0, sL2=0.0, mL2=0.0, ratC=0.0, prob;

  if(strncmp(aset,"all",strlen(aset)) == 0){
    sL1=fClusterFPar->getSigma1("all");
    mL1=fClusterFPar->getMPV1("all");
    sL2=fClusterFPar->getSigma2("all");
    mL2=fClusterFPar->getMPV2("all");
    ratC=fClusterFPar->getConstRatio("all");
  }
  if(strncmp(aset,"lep",strlen(aset)) == 0){
    sL1=fClusterFPar->getSigma1("lep");
    mL1=fClusterFPar->getMPV1("lep");
    sL2=fClusterFPar->getSigma2("lep");
    mL2=fClusterFPar->getMPV2("lep");
    ratC=fClusterFPar->getConstRatio("lep");
  }

  if((TMath::Landau(edep,mL1,sL1))!=0.0){
    prob=(1.0/(1.0+ratC*((TMath::Landau(edep,mL2,sL2))/(TMath::Landau(edep,mL1,sL1)))));
  } else {
    prob=0.0;
  }

  return prob;
}

void HTofClusterF::readHit(void) {
// Auxiliary function.
// It reads data from HTofHit based category.
//

  HGeomVector  vecRod, vecMod;

  fLoc[0]=hit->getSector();
  fLoc[1]=hit->getModule();
  fLoc[2]=hit->getCell();

  tof_h=hit->getTof();
  flagAdc_h=hit->getAdcFlag();

  HModGeomPar *module=fTofGeometry->getModule(fLoc[0],fLoc[1]);
  HGeomVolume *rodVol=module->getRefVolume()->getComponent(fLoc[2]);
  HGeomTransform &rodTrans=rodVol->getTransform();
  vecRod.setXYZ((hit->getXpos()),0.0,0.0);
  vecMod=rodTrans.transFrom(vecRod);
  xposMod_h=vecMod.getX();

  if(flagAdc_h==2){
    vecRod.setXYZ((hit->getXposAdc()),0.0,0.0);
    vecMod=rodTrans.transFrom(vecRod);
    xposModAdc_h=vecMod.getX();
  } else {
    xposModAdc_h=0.0;
  }
  hit->getXYZLab(xlab_h, ylab_h, zlab_h);
  edep_h=hit->getEdep();
  lAmp_h=hit->getLeftAmp();
  rAmp_h=hit->getRightAmp();
}

void HTofClusterF::writeCluster(Int_t mode) {
// Auxiliary function.
// It stores the date into the HTofCluster based category.
//

  if(mode==0){
    Float_t phi_h, theta_h, distance_h;
    hit->getPhi(phi_h);
    hit->getTheta(theta_h);
    hit->getDistance(distance_h);
    cluster = (HTofCluster *)fClusterCat->getSlot(fLoc);
    if(cluster){
      cluster = new(cluster) HTofCluster;
      cluster->setSector(fLoc[0]);
      cluster->setModule(fLoc[1]);
      cluster->setCell(fLoc[2]);
      cluster->setTof(tof_h);
      cluster->setXpos(xposMod_h);
      cluster->setXposAdc(xposModAdc_h);
      cluster->setEdep(edep_h);
      cluster->setLeftAmp(lAmp_h);
      cluster->setRightAmp(rAmp_h);
      cluster->setXYZLab(xlab_h, ylab_h, zlab_h);
      cluster->setDistance(distance_h);
      cluster->setTheta(theta_h);
      cluster->setPhi(phi_h);
      cluster->setClusterSize(1);
      cluster->setAdcFlag(flagAdc_h);
      cluster->setIndexHit2(-1);
    }
  }

  if(mode==1){
    HGeomVector r;
    Float_t distance_c, theta_c, phi_c;
    Float_t rad2deg = 180./TMath::Pi();

    r.setX(xlab_c);
    r.setY(ylab_c);
    r.setZ(zlab_c);
    distance_c = r.length();
    theta_c = (distance_c>0.) ? (rad2deg * TMath::ACos(r.getZ() / distance_c)) : 0.;
    phi_c = rad2deg * TMath::ATan2( r.getY(), r.getX());
    if (phi_c < 0.) phi_c += 360.;

    cluster = (HTofCluster *)fClusterCat->getSlot(fCLoc);

    HTofHit* hit1 = (HTofHit*)fHitCat->getObject(indexHit1);
    HTofHit* hit2 = (HTofHit*)fHitCat->getObject(indexHit2);
    if( (hit1 && hit2) &&
       (
	( hit1->getSector()!=hit2->getSector() )
	||
	( hit1->getModule()*8 + hit1->getCell() + 1 !=  hit2->getModule()*8 + hit2->getCell() )
	||
	( hit1->getSector()!= fCLoc[0])
	||
	( hit1->getModule()*8 + hit1->getCell() != fCLoc[1]*8+fCLoc[2])

       )
      )
    {
	cout<<"Error: HTofClusterF::writeCluster() : cluster does not match ! index "<<indexHit1<<" "<<indexHit2
	    <<" sec "<< (Int_t)hit1->getSector() << " "<<(Int_t) hit1->getSector()
	    <<" cell "<< hit1->getModule()*8 + hit1->getCell() << " "<< hit2->getModule()*8 + hit2->getCell()
	    <<" clus "<<fCLoc[0]<<" "<<fCLoc[1]*8+fCLoc[2]
	    <<endl;
    }


    if(cluster){
      cluster = new(cluster) HTofCluster;
      cluster->setSector(fCLoc[0]);
      cluster->setModule(fCLoc[1]);
      cluster->setCell(fCLoc[2]);
      cluster->setTof(tof_c);
      cluster->setXpos(xposMod_c);
      cluster->setXposAdc(xposModAdc_c);
      cluster->setEdep(edep_c);
      cluster->setLeftAmp(lAmp_c);
      cluster->setRightAmp(rAmp_c);
      cluster->setXYZLab(xlab_c, ylab_c, zlab_c);
      cluster->setDistance(distance_c);
      cluster->setTheta(theta_c);
      cluster->setPhi(phi_c);
      cluster->setClusterSize(size_c);
      cluster->setAdcFlag(flagAdc_c);
      cluster->setIndexHit2(indexHit2);
    }
  }
  if((mode!=0) && (mode!=1)){
    Warning("writeCluster","wrong scenario of cluster writing has been chosen !!!\ncluster not written!!!\n");
  }
}

void HTofClusterF::calcWeightedMean() {
// Auxiliary function.
// Calculates weighted mean.

    Float_t w1, w2;
    if((edep_c != 0.0) && (edep_h != 0.0)) {
      w1=edep_c;
      w2=edep_h;
      edep_c=edep_c+edep_h;
    } else {
      w1=1.0;
      w2=1.0;
    }

    tof_c=((tof_c*w1)+(tof_h*w2))/(w1+w2);
    xposMod_c=((xposMod_c*w1)+(xposMod_h*w2))/(w1+w2);
    xposModAdc_c=((xposModAdc_c*w1)+(xposModAdc_h*w2))/(w1+w2);
    lAmp_c=((lAmp_c*w1)+(lAmp_h*w2))/(w1+w2);
    rAmp_c=((rAmp_c*w1)+(rAmp_h*w2))/(w1+w2);
    xlab_c=((xlab_c*w1)+(xlab_h*w2))/(w1+w2);
    ylab_c=((ylab_c*w1)+(ylab_h*w2))/(w1+w2);
    zlab_c=((zlab_c*w1)+(zlab_h*w2))/(w1+w2);
    if((flagAdc_c==2)&&(flagAdc_h==2)) flagAdc_c=2;
    else flagAdc_c=0;
}

void HTofClusterF::fillPreviousData(void) {
// Auxiliary function.
//

  fpLoc=fLoc;
  tof_ph=tof_h;
  xposMod_ph=xposMod_h;
}

void HTofClusterF::fillClusterData(void) {
// Auxiliary function.
//

  tof_c=tof_h;
  xposMod_c=xposMod_h;
  xposModAdc_c=xposModAdc_h;
  edep_c=edep_h;
  lAmp_c=lAmp_h;
  rAmp_c=rAmp_h;
  xlab_c=xlab_h;
  ylab_c=ylab_h;
  zlab_c=zlab_h;
  flagAdc_c=flagAdc_h;
}

void HTofClusterF::initParContainer(HSpectrometer *spec, HRuntimeDb *rtdb) {
  // Adds to the runtime database "rtdb" the containers needed by the Cluster Finder
  //spec is used to get information of the spectrometer setup.

  fTofGeometry=(HTofGeomPar *)rtdb->getContainer("TofGeomPar");
  fClusterFPar=(HTofClusterFPar *)rtdb->getContainer("TofClusterFPar");
}

Bool_t HTofClusterF::init(void) {
  Bool_t r=kTRUE; // Function's return value
  HSpectrometer *spec = gHades->getSetup();
  HRuntimeDb *rtdb = gHades->getRuntimeDb();
  HEvent *ev = gHades->getCurrentEvent(); // Event structure

  if (spec && rtdb && ev) {
    HDetector *tof = spec->getDetector("Tof");

    if (tof) {
      // Parameter containers initialization.
      initParContainer(spec,rtdb);

      fHitCat = ev->getCategory(catTofHit);
      if (!fHitCat) {
        fHitCat = tof->buildCategory(catTofHit);
        if (!fHitCat) return kFALSE;
        else ev->addCategory(catTofHit,fHitCat,"Tof");
      }
      iterh=(HIterator*)fHitCat->MakeIterator("native");

      fClusterCat = ev->getCategory(catTofCluster);
      if (!fClusterCat) {
        fClusterCat = tof->buildCategory(catTofCluster);
        if (!fClusterCat) return kFALSE;
        else ev->addCategory(catTofCluster,fClusterCat,"Tof");
      }
      iterc=(HIterator*)fClusterCat->MakeIterator("native");
    } else {
      Error("init","TOF setup is not defined");
      r = kFALSE; // Notify error to task manager
    }
  } else {
    Error("init","Setup, RuntimeDb or Event structure not found");
    r = kFALSE; // Notify error to task manager
  }
  return r;
}

ClassImp(HTofClusterF)











