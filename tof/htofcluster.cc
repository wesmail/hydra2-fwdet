#include "htofcluster.h"

#include "TBuffer.h"

//*-- Author : D.Zovinec
//*-- Modified: 14/02/2002 D.Zovinec
//*-- Modified: 23/09/2002 D.Zovinec

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////
//HTofCluster
//
// Class with a TOF Cluster's data, i.e. clustered Hit's data.
//
/////////////////////////////

HTofCluster::HTofCluster(HTofHit *hit){
// HTofCluster Constructor
//
// Creates HTofCluster object and makes the copy of HTofHit object
// data members. The rest of the data members of HTofCluster object
// is set to:
//   clustSize=1
//   clustProbAll=1.0
//   clustProbLep=1.0
//

  setTof(hit->getTof());
  setXpos(hit->getXpos());
  setXposAdc(hit->getXposAdc());
  setEdep(hit->getEdep());
  setLeftAmp(hit->getLeftAmp());
  setRightAmp(hit->getRightAmp());
  Float_t xl,yl,zl,d,ph,th;
  hit->getXYZLab(xl,yl,zl);
  setXYZLab(xl,yl,zl);
  hit->getDistance(d);
  setDistance(d);
  hit->getPhi(ph);
  setPhi(ph);
  hit->getTheta(th);
  setTheta(th);
  setSector(hit->getSector());
  setModule(hit->getModule());
  setCell(hit->getCell());
  setAdcFlag(hit->getAdcFlag());

  clustSize=1;
  clustProbAll=1.0;
  clustProbLep=1.0;
  indexhit2   =-1;
}

HTofCluster::HTofCluster(HTofHit *hit, Int_t cls, Float_t clpa, Float_t clpl){
// HTofCluster Constructor
//
// Creates HTofCluster object and makes the copy of HTofHit object
// data members. The rest of the data members of HTofCluster object
// is set to:
//   clustSize=cls
//   clustProbAll=clpa
//   clustProbLep=clpl
//

  setTof(hit->getTof());
  setXpos(hit->getXpos());
  setXposAdc(hit->getXposAdc());
  setEdep(hit->getEdep());
  setLeftAmp(hit->getLeftAmp());
  setRightAmp(hit->getRightAmp());
  Float_t xl,yl,zl,d,ph,th;
  hit->getXYZLab(xl,yl,zl);
  setXYZLab(xl,yl,zl);
  hit->getDistance(d);
  setDistance(d);
  hit->getPhi(ph);
  setPhi(ph);
  hit->getTheta(th);
  setTheta(th);
  setSector(hit->getSector());
  setModule(hit->getModule());
  setCell(hit->getCell());
  setAdcFlag(hit->getAdcFlag());
  clustSize=cls;
  clustProbAll=clpa;
  clustProbLep=clpl;
  indexhit2 =-1;
}

void HTofCluster::Streamer(TBuffer &R__b)
{
  UInt_t R__s, R__c;
  if (R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
    HTofHit::Streamer(R__b);
    R__b >> clustSize;
    if(R__v <= 1){
      clustProbAll = 0.;
      clustProbLep = 0.;
    }
    if((R__v >= 2)&&(R__v <= 3)){
      R__b >> clustProbAll;
      R__b >> clustProbLep;
    }
    if(R__v >3) R__b >> indexhit2;  else indexhit2 =-1;
    R__b.CheckByteCount(R__s, R__c, HTofCluster::IsA());
  } else {
    R__c = R__b.WriteVersion(HTofCluster::IsA(), kTRUE);
    HTofHit::Streamer(R__b);
    R__b << clustSize;
    R__b << clustProbAll;
    R__b << clustProbLep;
    R__b << indexhit2;
    R__b.SetByteCount(R__c, kTRUE);
  }
}

ClassImp(HTofCluster)
