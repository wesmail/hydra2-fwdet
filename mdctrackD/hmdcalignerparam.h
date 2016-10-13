#ifndef HMDCALIGNERPARAM_H
#define HMDCALIGNERPARAM_H

#include "TObject.h"
#include "hmdcsizescells.h"

class HMdcLookUpTb;
class HGeomTransform;

class HMdcAlignerParam : public TObject {

  protected:
   
   Int_t alignSec;                     // sector to align
   
   Bool_t firstFile;                   // flag to avoid reinizalisation of original transform

   Bool_t cluster;                     // flag to make alignment by cluster
   
   Bool_t scan   ;                     // flag to choose fitter with or without target scan
   
   Int_t offset;                       // flag to switch between chamber alignment layer offsets and layer orientation
   
   Int_t nMinParams;                   // number of parameters to iterate
   
   TString rootFile;                   // name of root file to keep monitor NTuple
   
   Double_t alignParams[24];           // alignmet parameters:  additional 3 transition and 3 rotation to initial
                                       // transform matrix of each 4 module in sector
   Double_t steps[24];                 // iteration step for each parameter
   
   Double_t flags[24];                 // flag for each parameter: 0 - module don't included to alignment;
                                       // 1 - module included in alignment but parameter doesn't iterate        
                                       // 2 - module included in alignment and parameter willl be iterate
   Double_t minParams[24];             // parameters to iterate
   Double_t minSteps[24];              // steps of parameters to iterate
   Int_t minIndex[24];              // indexes of parameters tp iterate

   HMdcLookUpTb  * fLookUpTb;      // clus. finder for MDC1&2 (and 3&4 for type=1)


  protected:
   
   HMdcSizesCellsMod* fSizesCellsFirstMod;    // pointer to middle plane of first aligned mdc 
   HMdcSizesCellsMod* fSizesCellsLastMod;     // pointer to middle plane of last aligned mdc 
   Int_t firstMod;                            // first module
   Int_t lastMod;                             // last module

  protected:

   HGeomTransform sysRSecOrg[4];
   
   HGeomTransform sysRSec[4];

   const Double_t * tfSysRSec[4];
   
  public:
   
   HMdcAlignerParam(void);
   HMdcAlignerParam(Int_t alignSec, Double_t * params, Double_t * steps, Int_t * flags, HMdcLookUpTb * fLookUpTb);
   HMdcAlignerParam(const HMdcAlignerParam& tp);
   ~HMdcAlignerParam(void) {}
   void init(void);

  public:
   
   Int_t getNMinParams() { return nMinParams; }
   Double_t * getMinParams() { return minParams; }
   Double_t * getMinSteps() { return minSteps; }
   void setNewPosition(Double_t * minParams = 0, Int_t offset = 0);
   Bool_t setPlanes(void);
   void printTransforms(Double_t * targetParams = 0);
   void setRootFile(TString file) { rootFile = file; }
   TString getRootFile(void) { return rootFile; }
   void setAlignSec(Int_t aSec) { alignSec = aSec; }
   Int_t getAlignSec(void) { return alignSec; }
   void setCluster(Bool_t clus) { cluster = clus; }
   void setScan(Bool_t scn) { scan = scn; }
   void setOffset(Int_t off) { offset = off; }
   Bool_t getCluster(void) { return cluster; }
   Bool_t getScan(void) { return scan; }
   Int_t getOffset(void) { return offset; }
   const Double_t * getTfSysRSec(Int_t mod) {return tfSysRSec[mod];}
   Int_t getFirstMod(void) const {return firstMod;}
   Int_t getLastMod(void) const  {return lastMod;}

  public:
   
   inline void copyPlanes(const HMdcAlignerParam& ap);
   inline void copyAlignParams(const HMdcAlignerParam& ap);
   inline void setAlignParams(Double_t * par);
   inline void setNewAlignParams(Double_t * minPar);
   inline void copySteps(const HMdcAlignerParam& ap);
   inline void setSteps(Double_t * st);
   inline void copyFlags(const HMdcAlignerParam& ap);
   inline void setFlags(Int_t * fl);
   inline void copyMinParams(const HMdcAlignerParam& ap);
   inline void setMinParams(void);
   inline void copyMinSteps(const HMdcAlignerParam& ap);
   inline void setOriginalTransforms(void);
   inline void setActualTransforms(void);
   inline void copy(const HMdcAlignerParam& ap);
   inline void getSecTrackParams(Double_t & x1, Double_t & y1, Double_t & z1,
	              	   Double_t & x2, Double_t & y2, Double_t & z2);
   inline void getModTrackParams(Double_t x1, Double_t y1, Double_t z1,
						   Double_t x2, Double_t y2, Double_t z2,
						   Double_t & xFirstMod, Double_t & yFirstMod, 
						   Double_t & xLastMod,  Double_t & yLastMod);
   inline void print(TString header);
   

   ClassDef(HMdcAlignerParam,0)
};

inline void HMdcAlignerParam::copyPlanes(const HMdcAlignerParam& ap) { 
  fSizesCellsFirstMod=ap.fSizesCellsFirstMod;
  fSizesCellsLastMod=ap.fSizesCellsLastMod;
  firstMod = ap.firstMod;
  lastMod =  ap.lastMod;
}

inline void HMdcAlignerParam::copyAlignParams(const HMdcAlignerParam& ap) {
  for(Int_t i=0; i<24; i++) alignParams[i] = ap.alignParams[i];
}

inline void HMdcAlignerParam::setAlignParams(Double_t * par) {
  for(Int_t i=0; i<24; i++) alignParams[i] = par[i];
}

inline void HMdcAlignerParam::setNewAlignParams(Double_t * minPar) {
  for(Int_t i=0; i<nMinParams; i++) {
     minParams[i] = minPar[i];
     alignParams[minIndex[i]] = minPar[i];
  }
}

inline void HMdcAlignerParam::copySteps(const HMdcAlignerParam& ap) {
  for(Int_t i=0; i<24; i++) steps[i] = ap.steps[i];
}

inline void HMdcAlignerParam::setSteps(Double_t * st) {
  for(Int_t i=0; i<24; i++) steps[i] = st[i];
}

inline void HMdcAlignerParam::copyFlags(const HMdcAlignerParam& ap) {
  for(Int_t i=0; i<24; i++) flags[i] = ap.flags[i];
}

inline void HMdcAlignerParam::setFlags(Int_t * fl) {
  for(Int_t i=0; i<24; i++) flags[i] = fl[i];
}

inline void HMdcAlignerParam::copyMinParams(const HMdcAlignerParam& ap) {
  for(Int_t i=0; i<nMinParams; i++) minParams[i] = ap.alignParams[i];
}

inline void HMdcAlignerParam::setMinParams(void) {
  nMinParams = 0;
  for(Int_t i=0; i<24; i++) {
     if(flags[i] == 2) {
	minParams[nMinParams] = alignParams[i];
	minSteps[nMinParams] = steps[i];
	minIndex[nMinParams] = i;
	nMinParams++;
     }
  }
}

inline void HMdcAlignerParam::copyMinSteps(const HMdcAlignerParam& ap) {
  for(Int_t i=0; i<nMinParams; i++) minSteps[i] = ap.minSteps[i];
}

inline void HMdcAlignerParam::setOriginalTransforms(void) {
   
   HMdcSizesCells* fSizesCells   =  HMdcSizesCells::getObject();
   
   HMdcSizesCellsSec& fSCSec = (*fSizesCells)[alignSec];

   for(Int_t modNum=0; modNum<4; modNum++) 
      if(fSCSec.modStatus(modNum)) sysRSecOrg[modNum] = *(fSCSec[modNum].getSecTrans());
  
}

inline void HMdcAlignerParam::setActualTransforms(void) {
   
   HMdcSizesCells* fSizesCells   =  HMdcSizesCells::getObject();
   
   HMdcSizesCellsSec& fSCSec = (*fSizesCells)[alignSec];

   for(Int_t modNum=0; modNum<4; modNum++) 
      if(fSCSec.modStatus(modNum)) {
	 sysRSec[modNum] = *(fSCSec[modNum].getSecTrans());
	 tfSysRSec[modNum] = fSCSec[modNum].getTfSysRSec();
      }
  
}

inline void HMdcAlignerParam::copy(const HMdcAlignerParam& ap) { 
  copyAlignParams(ap);
  copySteps(ap);
  copyFlags(ap);
  copyMinParams(ap);
  copyMinSteps(ap);
  copyPlanes(ap);
  nMinParams = ap.nMinParams;
  fLookUpTb = ap.fLookUpTb;
  cluster = ap.cluster;
}

inline void HMdcAlignerParam::getSecTrackParams(Double_t & x1, Double_t & y1, Double_t & z1,
						Double_t & x2, Double_t & y2, Double_t & z2) {

   fSizesCellsFirstMod->transFrom(x1, y1, z1); 
   fSizesCellsLastMod->transFrom(x2, y2, z2); 

}

inline void HMdcAlignerParam::getModTrackParams(Double_t x1, Double_t y1, Double_t z1,
						Double_t x2, Double_t y2, Double_t z2,
						Double_t & xFirstMod, Double_t & yFirstMod, 
						Double_t & xLastMod,  Double_t & yLastMod) {

   fSizesCellsFirstMod->calcInterTrMod(x1, y1, z1, x2, y2, z2, xFirstMod, yFirstMod);
   fSizesCellsLastMod->calcInterTrMod (x1, y1, z1, x2, y2, z2, xLastMod,  yLastMod );
 
}

inline void HMdcAlignerParam::print(TString header) {

   cout << header << " " ;
   for(Int_t i=0; i<nMinParams; i++) cout << minParams[i] << " " ;

}

#endif
