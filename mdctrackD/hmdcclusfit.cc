//*--- AUTHOR: Vladimir Pechenov
//*--- Modified: Vladimir Pechenov 05/04/2005

using namespace std;
#include "hmdcclusfit.h"
#include <iostream>
#include <iomanip>

#include "TBuffer.h"

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HMdcClusFit
//
// Container class keep fit information for debuging.
//
// void setAddress(Char_t sc, Char_t sg, Char_t md)
// void setSec(Char_t sc)
//               Set sector.
// void setIOSeg(Char_t sg)
//               Set segment. It can be equal 0,1 (segment fit) or 3 sector fit.
// void setMod(Char_t m)
//               Set module. It can be equal 0,1,2,3 - one module fit,
//               or -1 - segment fit.
// void setFitAuthor(Char_t v)
//               Old name of function. Use setFitVersion(Char_t v) instead it.
// void setFitVersion(Char_t v)
//               Set fit version. =1 - old Alexander's fit version
// void setDistTimeVer(Char_t v)
//               Set distence=>drift_time calc. version
//               Now only one version exist (=1-by J.Market's func.)
// void setFitType(Char_t v)
//               Set fit type.
//               =0 - segment (or sector) fit, =1 - one mdc fit
// void setNParam(Char_t v)
//               Set number of parameters in fit.
// void setFunMin(Float_t v)
//               Set value of functional.
// void setNumOfWires(Int_t v)
//               Set number of wires passed fit.
// void setNumOfLayers(Int_t v)
//               Set number of layers.
// void setX1(Float_t v)
// void setY1(Float_t v)
// void setZ1(Float_t v)
// void setX2(Float_t v)
// void setY2(Float_t v)
// void setZ2(Float_t v)
//               Set track parameters. It is two point in sector coor.sys. [mm]
// void setNumIter(Short_t v)
//               Set number of iterations
// void setFirstWireFitInd(Int_t v)
//               Set index of the first HMdcWireFit object in catMdcWireFit
//               for this track.
// void setLastWireFitInd(Int_t v)
//               Set index of the last HMdcWireFit object in catMdcWireFit
//               for this track.
// void setClustIndex(Int_t v)
//               Set index of HMdcClus object in catMdcClus for this track.
// void setTimeOff(const Double_t* tos)
//               Set time offsets for each MDC in fit.
//  void seFitStatus(Bool_t stat)
//               Set fit status. kTRUE if fit result is accepted. kFALSE if not.
//
//
// void    getAddress(Int_t& sc, Int_t& sg, Int_t& md)
// Char_t  getSec(void)
//               Get sector.
// Char_t  getIOSeg(void)
//               Get segment. Return 0,1 for segment fit or 3 for sector fit.
// Char_t  getMod(void)
//               Get module. It can be equal 0,1,2,3 for one module fit,
//               or -1 - segment or sector fit.
// Char_t  getFitAuthor(void)
//               Old name of function. Use getFitVersion() instead it.
// Char_t  getFitVersion(void)
//               Get fit version. =1 - old Alexander's fit version
// Char_t  getDistTimeVer(void)
//               Get distence=>drift_time calc. version
//               Now only one version exist (=1-by J.Market's func.)
// Char_t  getFitType(void)
//               Get fit type.
//               =0 - segment (or sector) fit, =1 - one mdc fit
// Char_t  getNParam(Char_t v)
//               Get number of parameters in fit.
// Float_t getFunMin(void)
//               Get value of functional.
// Float_t getChi2(void)
//               Get chi2 per one deg. of freedom.
// Float_t getTimeOff(Int_t mod);
//               Old name of function. Use getTimeOffset(Int_t mod) instead it.
// Float_t getTimeOffset(Int_t mod)
//               Get time offset for module =mod.
//               return -1000. if mod is not valid (<0 or >3).
// Float_t getTimeOffsetMdc1(void)
//               Get time offset for module 1 (from 1)
// Float_t getTimeOffsetMdc2(void)
//               Get time offset for module 2 (from 1)
// Float_t getTimeOffsetMdc3(void)
//               Get time offset for module 3 (from 1)
// Float_t getTimeOffsetMdc4(void)
//               Get time offset for module 4 (from 1)
// Short_t getNumOfWires(void)
//               Get number of wires passed fit.
// Short_t getTotNumOfWires(void)
//               Get total number of wires in fit.
// Char_t  getNumOfLayers(void)
//               Get number of layers.
// Float_t getX1(void)
// Float_t getY1(void)
// Float_t getZ1(void)
// Float_t getX2(void)
// Float_t getY2(void)
// Float_t getZ2(void)
//               Get track parameters. It is two point in sector coor.sys. [mm]
// Float_t getPhi(void)
//               Get angle phi of track [rad]
// Float_t getTheta(void)
//               Get angle theta of track [rad]
// Short_t getNumIter(void)
//               Get number of iterations
// Int_t   getIndf(void)
//               Old name of function. Use getFirstWireFitInd() instead it.
// Int_t   getIndl(void)
//               Old name of function. Use getLastWireFitInd() instead it.
// Int_t   getFirstWireFitInd(void)
//               Get index of the first HMdcWireFit object in catMdcWireFit.
//               for this track.
// Int_t   getLastWireFitInd(void)
//               Get index of the last HMdcWireFit object in catMdcWireFit
//               for this track.
// void    getIndexis(Int_t &indexf,Int_t &indexl)
//               Get indexis of the first and last HMdcWireFit 
//               objects in catMdcWireFit.
// Int_t   getClustIndex(void)
//               Get index of HMdcClus object in catMdcClus for this track.
//               Index here is the third index in cluster location:
//               [sector][segment][index]
// void    getClustAddress(Int_t& sc, Int_t& sg, Int_t& ind)
//               Get location of cluster in 
// Int_t   getNextIndex(Int_t ind)
//               Get index of the next HMdcWireFit object for this track.
//               Using:
//               Int_t ind=-1;
//               while((ind=pHMdcClusFit->getNextIndex(ind)) >=0) {
//                 HMdcWireFit* pWireFit = 
//                           (HMdcWireFit*)pCatWireFit->getObect(ind);
//                 ... 
//               }
//  Bool_t  geFitStatus(void)
//               Return kTRUE if fit result was accepted, kFALSE if not.
//
// void print()
//              Print container.
// Bool_t isGeant()
//              Return kFALSE
//
//////////////////////////////////////////////////////////////////////////////

ClassImp(HMdcClusFit)

void HMdcClusFit::print(void) const {
  // Printing of container.
  printf("-HMdcClusFit- Sec.%i",sec+1);
  if(mod<0) printf(" Seg.%i:",seg+1);
  else printf(" Mod.%i:",mod+1);
  printf("    Ver.%i, dist-time ver.%i, fit type %i, num.of param.=%i\n",
      fitAuthor,distTimeVer,fitType,nParam);
  printf(" Fun.=%5.1f Nwires=%2i Nw(WT>0)=%2i Nlay.(WT>0)=%2i Chi2=%5.2f",
      functional,indl-indf+1,numOfWires,numOfLayers,getChi2());
  printf(" Niter.=%i TOF=%.1f|%.1f|%.1f|%.1f\n",numIter,
      timeOffMdc1,timeOffMdc2,timeOffMdc3,timeOffMdc4);
  printf(" Track line:          (%8.2f,%8.2f,%7.2f) - (%8.2f,%8.2f,%8.2f)\n",
      x1,y1,z1,x2,y2,z2);
}

void HMdcClusFit::setTimeOff(const Double_t* tos) {
  // Set time offset for MDC modules
  timeOffMdc1=tos[0];
  timeOffMdc2=tos[1];
  timeOffMdc3=tos[2];
  timeOffMdc4=tos[3];
}

Float_t HMdcClusFit::getTimeOff(Int_t mod) const {
  // Return time offset for MDC module number "mod"
  if(mod==0) return timeOffMdc1;
  if(mod==1) return timeOffMdc2;
  if(mod==2) return timeOffMdc3;
  if(mod==3) return timeOffMdc4;
  return -1000.;
}

void HMdcClusFit::calcRZtoLineXY(Float_t &zm, Float_t &r0,
    Float_t x1, Float_t y1, Float_t z1, Float_t x2, Float_t y2, Float_t z2, 
    Float_t xBm, Float_t yBm) const {
  // zm= z1 - cos(theta)/sin(theta) * ((x1-xBm)*cos(phi)+(y1-yBm)*sin(phi))
  // r0=(y1-yBm)*cos(phi)-(x1-xBm)*sin(phi)
  //
  // If (x1,y1,z1)=(x2,y2,z2) dZ will eq.1.! 
  Float_t dX=x2-x1;
  Float_t dY=y2-y1;
  Float_t dZ=z2-z1;
  Float_t lenXY2=dX*dX+dY*dY;
  Float_t lenXY=sqrt(lenXY2);
  if(lenXY<2.E-5) {
    dX =x2 * 1.E-5/sqrt(x2*x2+y2*y2);
    dY =y2 * 1.E-5/sqrt(x2*x2+y2*y2);
    lenXY=1.E-5;            //dX*dX+dY*dY;
    lenXY2=lenXY*lenXY;
    dZ=1.;
  }
  x1-=xBm;  // Shifting to the line (xBm,yBm)
  y1-=yBm;  // Shifting to the line (xBm,yBm)
  
  zm=z1-dZ*(x1*dX+y1*dY)/lenXY2;
  r0=(y1*dX-x1*dY)/lenXY;
}

void HMdcClusFit::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcClusFit.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> sec;
      R__b >> seg;
      R__b >> mod;
      R__b >> fitAuthor;
      R__b >> distTimeVer;
      R__b >> fitType;
      R__b >> nParam;
      R__b >> functional;
      R__b >> timeOffMdc1;
      R__b >> timeOffMdc2;
      R__b >> timeOffMdc3;
      R__b >> timeOffMdc4;
      R__b >> numOfWires;
      R__b >> numOfLayers;
      R__b >> x1;
      R__b >> y1;
      R__b >> z1;
      R__b >> x2;
      R__b >> y2;
      R__b >> z2;
      R__b >> numIter;
      R__b >> indf;
      R__b >> indl;
      R__b >> clustIndex;
      R__b >> exitFlag;
      R__b >> sigmaChi2;
      R__b >> fitStatus;
      if(R__v > 1) {
        R__b >> dx1;
        R__b >> dy1;
        R__b >> dx2;
        R__b >> dy2;
      } else {
        dx1 = -1.;
        dy1 = -1.;
        dx2 = -1.;
        dy2 = -1.;
      }
      R__b.CheckByteCount(R__s, R__c, HMdcClusFit::IsA());
   } else {
      R__c = R__b.WriteVersion(HMdcClusFit::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << sec;
      R__b << seg;
      R__b << mod;
      R__b << fitAuthor;
      R__b << distTimeVer;
      R__b << fitType;
      R__b << nParam;
      R__b << functional;
      R__b << timeOffMdc1;
      R__b << timeOffMdc2;
      R__b << timeOffMdc3;
      R__b << timeOffMdc4;
      R__b << numOfWires;
      R__b << numOfLayers;
      R__b << x1;
      R__b << y1;
      R__b << z1;
      R__b << x2;
      R__b << y2;
      R__b << z2;
      R__b << numIter;
      R__b << indf;
      R__b << indl;
      R__b << clustIndex;
      R__b << exitFlag;
      R__b << sigmaChi2;
      R__b << fitStatus;
      R__b << dx1;
      R__b << dy1;
      R__b << dx2;
      R__b << dy2;
      R__b.SetByteCount(R__c, kTRUE);
   }
}
