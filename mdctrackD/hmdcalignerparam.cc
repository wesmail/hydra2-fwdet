//*-- Author : G.Agakishiev

using namespace std;
#include <iostream>
#include <iomanip>

#include "hmdcalignerparam.h"
#include "hmdcalignerd.h"
#include "hmdclookuptb.h"
#include "hmdcgetcontainers.h"
#include "hgeomcompositevolume.h"

ClassImp(HMdcAlignerParam)

HMdcAlignerParam::HMdcAlignerParam(void) {

   cluster = kFALSE;
   offset = kFALSE;
   firstFile = kTRUE;
   firstMod = -1;
   lastMod  = -1;

}
HMdcAlignerParam::HMdcAlignerParam(const HMdcAlignerParam& ap):TObject(ap) {

   copy(ap);
   cluster = kFALSE;
   offset = kFALSE;
   firstFile = kTRUE;

   
}

HMdcAlignerParam::HMdcAlignerParam(Int_t alsec, Double_t * par, Double_t * st, Int_t * fl, HMdcLookUpTb * look) {

   alignSec = alsec;
   fLookUpTb = look;
   cluster = kFALSE;
   offset = kFALSE;
   firstFile = kTRUE;
   
   setAlignParams(par);
   setSteps(st);
   setFlags(fl);
   setMinParams();
   setPlanes();
   setNewPosition();
   
}

void HMdcAlignerParam::init() {

   fLookUpTb=HMdcLookUpTb::getObject();
   if(firstFile) {
      setMinParams();
      setPlanes();
      setOriginalTransforms();
      setActualTransforms();
      setNewPosition(minParams,offset);
      firstFile = kFALSE;
   }

}
   
void HMdcAlignerParam::setNewPosition(Double_t * minPar, Int_t offset) {
   
   HMdcSizesCells* fSizesCells   =  HMdcSizesCells::getObject();

   HMdcSizesCellsSec& fSCSec = (*fSizesCells)[alignSec];
   
   if(minPar == 0) minPar = minParams;

   setNewAlignParams(minPar);
   
   Double_t alignPar[6]; 
   Double_t corr[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
   HGeomTransform alignTrans;

   switch(offset) {
   case 0:
      alignPar[0] = alignParams[0];
      alignPar[1] = alignParams[1];
      alignPar[2] = alignParams[2];
      alignPar[3] = alignParams[3];
      alignPar[4] = alignParams[4];
      alignPar[5] = alignParams[5];
      HMdcSizesCells::setTransform(alignPar, alignTrans);
      if(fSCSec.modStatus(0)) fSizesCells->fillModCont(alignSec,0,&alignTrans,1);
      alignPar[0] = alignParams[6];
      alignPar[1] = alignParams[7];
      alignPar[2] = alignParams[8];
      alignPar[3] = alignParams[9];
      alignPar[4] = alignParams[10];
      alignPar[5] = alignParams[11];
      HMdcSizesCells::setTransform(alignPar, alignTrans);
      if(fSCSec.modStatus(1)) fSizesCells->fillModCont(alignSec,1,&alignTrans,1);
      alignPar[0] = alignParams[12];
      alignPar[1] = alignParams[13];
      alignPar[2] = alignParams[14];
      alignPar[3] = alignParams[15];
      alignPar[4] = alignParams[16];
      alignPar[5] = alignParams[17];
      HMdcSizesCells::setTransform(alignPar, alignTrans);
      if(fSCSec.modStatus(2)) fSizesCells->fillModCont(alignSec,2,&alignTrans,1);
      alignPar[0] = alignParams[18];
      alignPar[1] = alignParams[19];
      alignPar[2] = alignParams[20];
      alignPar[3] = alignParams[21];
      alignPar[4] = alignParams[22];
      alignPar[5] = alignParams[23];
      HMdcSizesCells::setTransform(alignPar, alignTrans);
      if(fSCSec.modStatus(3)) fSizesCells->fillModCont(alignSec,3,&alignTrans,1);
      break;
   case 1:
      corr[0] = alignParams[0];
      corr[1] = alignParams[1];
      corr[2] = alignParams[2];
      corr[3] = alignParams[3];
      corr[4] = alignParams[4];
      corr[5] = alignParams[5];
      if(fSCSec.modStatus(0)) fSizesCells->fillModCont(alignSec,0,corr);
      corr[0] = alignParams[6];
      corr[1] = alignParams[7];
      corr[2] = alignParams[8];
      corr[3] = alignParams[9];
      corr[4] = alignParams[10];
      corr[5] = alignParams[11];
      if(fSCSec.modStatus(1)) fSizesCells->fillModCont(alignSec,1,corr);
      corr[0] = alignParams[12];
      corr[1] = alignParams[13];
      corr[2] = alignParams[14];
      corr[3] = alignParams[15];
      corr[4] = alignParams[16];
      corr[5] = alignParams[17];
      if(fSCSec.modStatus(2)) fSizesCells->fillModCont(alignSec,2,corr);
      corr[0] = alignParams[18];
      corr[1] = alignParams[19];
      corr[2] = alignParams[20];
      corr[3] = alignParams[21];
      corr[4] = alignParams[22];
      corr[5] = alignParams[23];
      if(fSCSec.modStatus(3)) fSizesCells->fillModCont(alignSec,3,corr);
      break;
   case 2:
      corr[6] = alignParams[0];
      corr[7] = alignParams[1];
      corr[8] = alignParams[2];
      corr[9] = alignParams[3];
      corr[10] = alignParams[4];
      corr[11] = alignParams[5];
      if(fSCSec.modStatus(0)) fSizesCells->fillModCont(alignSec,0,corr);
      corr[6] = alignParams[6];
      corr[7] = alignParams[7];
      corr[8] = alignParams[8];
      corr[9] = alignParams[9];
      corr[10] = alignParams[10];
      corr[11] = alignParams[11];
      if(fSCSec.modStatus(1)) fSizesCells->fillModCont(alignSec,1,corr);
      corr[6] = alignParams[12];
      corr[7] = alignParams[13];
      corr[8] = alignParams[14];
      corr[9] = alignParams[15];
      corr[10] = alignParams[16];
      corr[11] = alignParams[17];
      if(fSCSec.modStatus(2)) fSizesCells->fillModCont(alignSec,2,corr);
      corr[6] = alignParams[18];
      corr[7] = alignParams[19];
      corr[8] = alignParams[20];
      corr[9] = alignParams[21];
      corr[10] = alignParams[22];
      corr[11] = alignParams[23];
      if(fSCSec.modStatus(3)) fSizesCells->fillModCont(alignSec,3,corr);
      break;
   case 3:
      corr[12] = alignParams[0] + alignParams[1] + alignParams[2];
      corr[13] = alignParams[1] + alignParams[2];
      corr[14] = alignParams[2];
      corr[15] = alignParams[3];
      corr[16] = alignParams[4] + alignParams[3];
      corr[17] = alignParams[5] + alignParams[4] + alignParams[3];
      if(fSCSec.modStatus(0)) fSizesCells->fillModCont(alignSec,0,corr);
      corr[12] = alignParams[6] + alignParams[7] + alignParams[8];
      corr[13] = alignParams[7] + alignParams[8];
      corr[14] = alignParams[8];
      corr[15] = alignParams[9];
      corr[16] = alignParams[10] + alignParams[9];
      corr[17] = alignParams[11] + alignParams[10] + alignParams[9];
      if(fSCSec.modStatus(1)) fSizesCells->fillModCont(alignSec,1,corr);
      corr[12] = alignParams[12] + alignParams[13] + alignParams[14];  
      corr[13] = alignParams[13] + alignParams[14];		    
      corr[14] = alignParams[14];
      corr[15] = alignParams[15];
      corr[16] = alignParams[16] + alignParams[15];		    
      corr[17] = alignParams[17] + alignParams[16] + alignParams[15];
      if(fSCSec.modStatus(2)) fSizesCells->fillModCont(alignSec,2,corr);
      corr[12] = alignParams[18] + alignParams[19] + alignParams[20];;
      corr[13] = alignParams[19] + alignParams[20];		     ;
      corr[14] = alignParams[20];
      corr[15] = alignParams[21];
      corr[16] = alignParams[22] + alignParams[21];		     ;
      corr[17] = alignParams[23] + alignParams[22] + alignParams[21];;
      if(fSCSec.modStatus(3)) fSizesCells->fillModCont(alignSec,3,corr);
      break;
   }

   fLookUpTb->initContainer();
   
}

Bool_t HMdcAlignerParam::setPlanes() {
   
  HMdcSizesCells* fSizesCells   =  HMdcSizesCells::getObject();
  HMdcSizesCellsSec& fSizesCellsSec = (*fSizesCells)[alignSec];
  
  firstMod = -1;
  lastMod = -1;
  for(Int_t iMod = 0; iMod < 4; iMod++) {
     if(fSizesCellsSec.modStatus(iMod) && firstMod < 0) firstMod = iMod;
     if(fSizesCellsSec.modStatus(iMod) && firstMod >= 0) lastMod = iMod;
  }
  
  if(firstMod < 0 || lastMod < 0) return kFALSE;
  
  fSizesCellsFirstMod = &(fSizesCellsSec[firstMod]); 
  fSizesCellsLastMod  = &(fSizesCellsSec[lastMod]);

  return kTRUE;
  
}

void HMdcAlignerParam::printTransforms(Double_t * targetPar) {

   HMdcSizesCells* fSizesCells   =  HMdcSizesCells::getObject();
   HMdcSizesCellsSec& fSCSec = (*fSizesCells)[alignSec];
   const HGeomTransform * sysRLab = fSCSec.getLabTrans();
   cout.precision(10);

      
//    HGeomVector chamberVol[4][8];  // Drift gas vol. (8-points)
//    for(Int_t cham=0; cham<3; cham++) {
//       HGeomCompositeVolume * fComVol = fGetCont->getGeomCompositeVolume(cham);
//       for(Int_t point=0; point<8; point++) {
// 	 chamberVol[cham][point] = *(fComVol->getPoint(point));
// 	 cout << cham << " " << point << " " << chamberVol[cham][point] << endl;
//       }
//    }
//    HGeomVector chamberVol[4][8];  // Drift gas vol. (8-points)
//    for(Int_t cham=0; cham<3; cham++) {
//       HGeomCompositeVolume * fComVol = fGetCont->getGeomCompositeVolume(cham);
//       for(Int_t point=0; point<8; point++) {
// 	 chamberVol[cham][point] = *(fComVol->getPoint(point));
// 	 cout << cham << " " << point << " " << chamberVol[cham][point] << endl;
//       }
//    }
	 


   HGeomTransform targetTrans;
   if(targetPar != 0) HMdcSizesCells::setTransform(targetPar, targetTrans);

   cout << " original sec transforms " << endl;
   
   for(Int_t modNum=0; modNum<4; modNum++) sysRSecOrg[modNum].print();

   cout << " original lab transforms " << endl;
   
   for(Int_t modNum=0; modNum<4; modNum++) sysRSecOrg[modNum].transFrom(*sysRLab);

   for(Int_t modNum=0; modNum<4; modNum++) sysRSecOrg[modNum].print();

   setActualTransforms();
   
   cout << " actual sector transforms " << endl;

   for(Int_t modNum=0; modNum<4; modNum++) sysRSec[modNum].print();
   

//    cout << " target transforms " << endl;

//    targetTrans.print();

//    cout << " actual sector transforms corrected for target position" << endl;

//    sysRSec0.transTo(targetTrans);
//    sysRSec1.transTo(targetTrans);
//    sysRSec2.transTo(targetTrans);
//    sysRSec3.transTo(targetTrans);
   
//    sysRSec0.print();
//    sysRSec1.print();
//    sysRSec2.print();
//    sysRSec3.print();
   
   cout << " actual lab transforms " << endl;
   
   for(Int_t modNum=0; modNum<4; modNum++) sysRSec[modNum].transFrom(*sysRLab);

   for(Int_t modNum=0; modNum<4; modNum++) sysRSec[modNum].print();
   
   HMdcGetContainers * fGetCont = HMdcGetContainers::getObject();
   HGeomVector chamberVol[4][8];  // Drift gas vol. (8-points)
   for(Int_t cham=0; cham<4; cham++) {
      HGeomCompositeVolume * fComVol = fGetCont->getGeomCompositeVolume(cham);
      if(!fComVol) continue;
      for(Int_t point=0; point<8; point++) {
	 chamberVol[cham][point] = *(fComVol->getPoint(point));
	 chamberVol[cham][point] = sysRSec[cham].transFrom(chamberVol[cham][point]);
	 cout << alignSec << " " << cham << " " << point << " " << chamberVol[cham][point] << endl;
      }
   }
//    sysRSec0.transFrom(*sysRLab);
//    sysRSec1.transFrom(*sysRLab);
//    sysRSec2.transFrom(*sysRLab);
//    sysRSec3.transFrom(*sysRLab);

//    sysRSec0.print();
//    sysRSec1.print();
//    sysRSec2.print();
//    sysRSec3.print();
   
   
//    cout << " final transforms" << endl;

//    sysRSec0.transTo(sysRSec0Org);
//    sysRSec1.transTo(sysRSec1Org);
//    sysRSec2.transTo(sysRSec2Org);
//    sysRSec3.transTo(sysRSec3Org);
	 
//    sysRSec0.print();
//    sysRSec1.print();
//    sysRSec2.print();
//    sysRSec3.print();

}
   
