//
//*-- Author: M. Jurkovic martin.jurkovic@ph.tum.de
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HStart2Hit
//
//  This class contains START2 hit data
//
/////////////////////////////////////////////////////////////

#include "hstart2hit.h"

#include "TBuffer.h"

ClassImp(HStart2Hit)


HStart2Hit::HStart2Hit(void)
{

    fModule      = -1,
    fStrip       = -1;
    fMultiplicity= 0;
    fTime        = -1000000.;
    fWidth       = -1000000.;
    fFlag        = kFALSE;
    fCorrFlag    = -1;
    fResolution  = -1000;
    fSimWidth    = -1000;
    fTrack       = -1;
    fTime2       = -1000000.;
    fIteration   = 0;
    resetClusterStrip(2);
}

void  HStart2Hit::resetClusterStrip(UInt_t firstOrSecond)
{

    if(firstOrSecond>1 || firstOrSecond == 0 ) nFirstCluster  = 0;
    if(firstOrSecond>1 || firstOrSecond == 1 ) nSecondCluster = 0;
    for(Int_t i=0;i<MAXNSTRIP;i++) {
	if(firstOrSecond>1 || firstOrSecond == 0 ) fFirstCluster [i]=-1;
	if(firstOrSecond>1 || firstOrSecond == 1 ) fSecondCluster[i]=-1;
    }
}

Bool_t  HStart2Hit::setClusterStrip (UInt_t firstOrSecond, Int_t stripindex,Int_t hitindex)
{
    Int_t st = stripindex*1000+hitindex;
    return setClusterStrip (firstOrSecond, st);
}

Bool_t  HStart2Hit::setClusterStrip (UInt_t firstOrSecond, Int_t stripandhitindex)
{
    if(firstOrSecond>1) {
	Error("setClusterStrip()","Cluster selection too large (%i)",firstOrSecond);
	return kFALSE;
    }
    if(firstOrSecond==0 && nFirstCluster<MAXNSTRIP-1){

        fFirstCluster[nFirstCluster] = stripandhitindex;
	nFirstCluster++;
        return kTRUE;
    }
    if(firstOrSecond==1 && nSecondCluster<MAXNSTRIP-1){

        fSecondCluster[nSecondCluster] = stripandhitindex;
	nSecondCluster++;
        return kTRUE;
    }
    return kFALSE;
}

Int_t  HStart2Hit::getClusterStrip (UInt_t firstOrSecond, Int_t num)
{
    if(firstOrSecond>1) {
	Error("getClusterStrip()","Cluster selection too large (%i)",firstOrSecond);
	return -1;
    }
    if(firstOrSecond==0 && num<nFirstCluster) {  return fFirstCluster [num]; }
    if(firstOrSecond==1 && num<nSecondCluster){  return fSecondCluster[num]; }
    return -1;
}

Int_t  HStart2Hit::getClusterStrip (UInt_t firstOrSecond, Int_t num, Int_t& stripindex,Int_t& hitindex)
{
    hitindex=stripindex=-1;
    Int_t st= getClusterStrip ( firstOrSecond, num );
    if(st>-1){
        stripindex = st/1000;
        hitindex  = st - stripindex*1000;
        return st;
    }
    return -1;
}

void HStart2Hit::Streamer(TBuffer &R__b)
{
   // Stream an object of class HStart2Hit.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> fModule;
      R__b >> fStrip;
      R__b >> fMultiplicity;
      R__b >> fTime;
      R__b >> fWidth;
      R__b >> fFlag;

      if (R__v > 1) R__b >> fCorrFlag;
      else                  fCorrFlag = -1;

      if (R__v > 2) R__b >> fResolution;
      else                  fResolution = -1000;
      if (R__v > 5) R__b >> fSimWidth;
      else                  fSimWidth = -1000;

      if (R__v > 3) R__b >> fTrack;
      else                  fTrack = -1;

      if (R__v > 4){
          R__b >> fIteration;
	  R__b >> fTime2;
	  R__b >> nFirstCluster;
	  R__b >> nSecondCluster;
	  R__b.ReadStaticArray((Int_t*)fFirstCluster);
	  R__b.ReadStaticArray((Int_t*)fSecondCluster);

      } else {
          fIteration = 0;
	  resetClusterStrip(2);
      }
      R__b.CheckByteCount(R__s, R__c, HStart2Hit::IsA());
   } else {
      R__c = R__b.WriteVersion(HStart2Hit::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << fModule;
      R__b << fStrip;
      R__b << fMultiplicity;
      R__b << fTime;
      R__b << fWidth;
      R__b << fFlag;
      R__b << fCorrFlag;
      R__b << fResolution;
      R__b << fSimWidth;
      R__b << fTrack;
      R__b << fIteration;
      R__b << fTime2;
      R__b << nFirstCluster;
      R__b << nSecondCluster;
      R__b.WriteArray((Int_t*)fFirstCluster,MAXNSTRIP);
      R__b.WriteArray((Int_t*)fSecondCluster,MAXNSTRIP);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

