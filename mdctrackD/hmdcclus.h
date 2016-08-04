#ifndef HMDCCLUS_H
#define HMDCCLUS_H

#include "hmdclistgroupcells.h"
#include "hlocation.h"
#include "hgeomvector.h"
#include "TMath.h"
#include "hsymmat.h"

class HMdcClus : public HMdcList12GroupCells {
protected:
  UChar_t sec;
  UChar_t seg;
  Int_t   typeClFinder; // =0 - finding in all mdc in sec.
                        // =1 - finding in each mdc indep.
                        // =2 - mixed ver.
                        // If bit 8 (from 1) eq.1 - floatLevel of clust.finder
                        // If bit 9 (from 1) eq.1 - ampl.cut was done for
                        //                          segment but not for each MDC
  Short_t mod;          // =-1 if typeClFinder=0
  Int_t   index;        // >=0 - OWN index in clus.cat
                        // =-1 - index not defined
  Float_t x;            // x-coordinate of cluster
  Float_t y;            // y-coordinate of cluster
  Float_t xTarg;        // x-target(Mdc1-2) or point on kick plane (Mdc3-4)
  Float_t yTarg;        // y-target(Mdc1-2) or point on kick plane (Mdc3-4)
  Float_t zTarg;        // z-target(Mdc1-2) or point on kick plane (Mdc3-4)
  Float_t errX;         // Error of x-coordinate of cluster
  Float_t errY;         // Error of y-coordinate of cluster
  Float_t errXTarg;     // Error of xTarg(Mdc1-2) or point on kickplane (Mdc3-4)
  Float_t errYTarg;     // Error of yTarg(Mdc1-2) or point on kickplane (Mdc3-4)
  Float_t errZTarg;     // Error of zTarg(Mdc1-2) or point on kickplane (Mdc3-4)
  Float_t sumWt;        // Sum. of bins' weights in cluster
  Int_t   nBins;        // Num. of bins in cluster
  UChar_t minCl1;       // minCl1 mdc 1or3, minCl2 mdc 2or4 (depending on seg.)
  UChar_t minCl2;       // clusters with bin content >= maxCl were searched
  UChar_t realLevel;    // real level of cluster finder for this cluster
  Short_t nMergedClus;  // Number of merged clusters
  Int_t   indexPar;     // index of parent cluster in catMdcClus (def.: =-1)
  Int_t   indCh1;       // indexes region of childs in catMdcClus (def.: = -1)
  Int_t   indCh2;       //                                        (def.: = -2)
  Float_t parA;         // Project plane: parA*x+parB*y+z=parD
  Float_t parB;         //
  Float_t parD;         //
  Short_t status;       // user var.
  UChar_t fakeFlag;     // Flag of fake suppression code

  UChar_t clFnLevelM1;  // level of cluster finder in mod.1
  Short_t clusSizeM1;   // number of bins in mod.1 cluster
  Short_t nDrTimesM1;   // number of drift times in cluster 
  Short_t nMergClusM1;  // number of merged clusters 
  Float_t sigma1M1;     // cluster shape mod.1:
  Float_t sigma2M1;     // sigma1 - long axis, sigma2 - short axis
  Float_t alphaM1;      // angle (deg.) of long axis and X axis

  UChar_t clFnLevelM2;  // level of cluster finder in mod.2
  Short_t clusSizeM2;   // Num. of bins in mod.2 cluster
  Short_t nDrTimesM2;   // number of drift times in cluster 
  Short_t nMergClusM2;  // number of merged clusters
  Float_t sigma1M2;     // cluster shape mod.2:
  Float_t sigma2M2;     // sigma1 - long axis, sigma2 - short axis
  Float_t alphaM2;      // angle (deg.) of long axis and X axis
  
  Int_t   segIndex;     // index of HMdcSeg object
 
public:
  HMdcClus(void)                           {setDefValues();}
  HMdcClus(HMdcList12GroupCells& lcells) : 
      HMdcList12GroupCells(lcells)         {setDefValues();}
  ~HMdcClus(void) {}
  virtual Bool_t isGeant(void) const       {return kFALSE;}
  void    clear(void);
  void    setDefValues(void);
  void    clearMod1Par(void)               {nMergClusM1=nDrTimesM1=clusSizeM1=0;
                                            sigma1M1=sigma2M1=alphaM1=0.;
                                            clFnLevelM1=0;}
  void    clearMod2Par(void)               {nMergClusM2=nDrTimesM2=clusSizeM2=0;
                                            sigma1M2=sigma2M2=alphaM2=0.;
                                            clFnLevelM2=0;}
  void    setSec(Int_t sc)                 {sec=sc;}
  void    setMod(Int_t m)                  {mod=m;}
  void    setIOSeg(Int_t sg)               {seg=sg;}
  void    setOwnIndex(Int_t ind)           {index=ind;}
  void    setSecSegInd(Int_t sc, Int_t sg,
                       Int_t ind)          {sec=sc; seg=sg; index=ind;}
  void    setMinCl(Int_t m1or3,
                   Int_t m2or4)            {minCl1=m1or3; minCl2=m2or4;}
  void    setSumWt(Float_t swt)            {sumWt=swt;}
  void    setRealLevel(UChar_t rl)         {realLevel=rl;}
  void    setNBins(Int_t nBn)              {nBins=nBn;}
  void    setIndexParent(Int_t lp)         {indexPar=lp;}
  void    setIndexChilds(Int_t l1,
                         Int_t l2)         {indCh1=l1; indCh2=l2;}
  void    setTypeClFinder(Int_t type)      {typeClFinder=type;}
  void    setNMergClust(Short_t nmrc)      {nMergedClus=nmrc;}
  void    setXY(Float_t xl, Float_t errx,
                Float_t yl, Float_t erry)  {x=xl; errX=errx; y=yl; errY=erry;}
  void    setXTarg(Float_t xl,Float_t err) {xTarg=xl; errXTarg=err;}
  void    setYTarg(Float_t yl,Float_t err) {yTarg=yl; errYTarg=err;}
  void    setZTarg(Float_t zl,Float_t err) {zTarg=zl; errZTarg=err;}
  void    setTarg(Float_t xl, Float_t yl,
                  Float_t zl)              {xTarg=xl; yTarg=yl; zTarg=zl;}
  void    setTarg(const HGeomVector& v)    {xTarg=v.getX();yTarg=v.getY();
                                            zTarg=v.getZ();}
  void    setErrTarg(const HGeomVector& v) {errXTarg=v.getX();
                                            errYTarg=v.getY();
                                            errZTarg=v.getZ();}
  void    setTarget(Float_t xl,Float_t ex,
                    Float_t yl,Float_t ey,
                    Float_t zl,Float_t ez) {xTarg=xl; errXTarg=ex;
                                            yTarg=yl; errYTarg=ey;
                                            zTarg=zl; errZTarg=ez;}
  void    setPrPlane(Float_t a,
                     Float_t b,
                     Float_t d)            {parA=a;parB=b; parD=d;}
  void    setClFnLevelM1(UChar_t lv)       {clFnLevelM1=lv;}
  void    setClusSizeM1(Short_t clSz)      {clusSizeM1=clSz;}
  void    setNDrTimesM1(Short_t nDrTm)     {nDrTimesM1=nDrTm;}
  void    setNMergClustM1(Short_t nmrc)    {nMergClusM1=nmrc;}
  void    setShapeM1(Float_t s1,
                     Float_t s2,
                     Float_t al)           {sigma1M1=s1;sigma2M1=s2;alphaM1=al;}
  void    setClFnLevelM2(UChar_t lv)       {clFnLevelM2=lv;}
  void    setClusSizeM2(Short_t clSz)      {clusSizeM2=clSz;}
  void    setNDrTimesM2(Short_t nDrTm)     {nDrTimesM2=nDrTm;}
  void    setNMergClustM2(Short_t nmrc)    {nMergClusM2=nmrc;}
  void    setShapeM2(Float_t s1,
                     Float_t s2,
                     Float_t al)           {sigma1M2=s1;sigma2M2=s2;alphaM2=al;}
  void    setSegIndex(Int_t si)            {segIndex = si;}
  void    setFakeFlag(UChar_t fl)          {fakeFlag = fl;}

  Short_t getMod(void) const               {return mod;}
  Int_t   getTypeClFinder(void) const      {return typeClFinder&127;}
  UChar_t getSec(void) const               {return sec;}
  UChar_t getIOSeg(void) const             {return seg;}
  Short_t getOwnIndex(void) const          {return index;}
  Int_t   getMinCl(void) const             {return Int_t(minCl1+minCl2);}
  UChar_t getMinCl(Int_t mod) const        {return (mod&1)==0 ? minCl1:minCl2;}
  UChar_t getRealLevel(void) const         {return realLevel;}
  Float_t getX(void) const                 {return x;}
  Float_t getY(void) const                 {return y;}
  Float_t getZ(void) const                 {return parD-parA*x-parB*y;}
  Float_t getXTarg(void) const             {return xTarg;}
  Float_t getYTarg(void) const             {return yTarg;}
  Float_t getZTarg(void) const             {return zTarg;}
  Float_t getErrX(void) const              {return errX;}
  Float_t getErrY(void) const              {return errY;}
  Float_t getErrXTarg(void) const          {return errXTarg;}
  Float_t getErrYTarg(void) const          {return errYTarg;}
  Float_t getErrZTarg(void) const          {return errZTarg;}
  Float_t getTheta(void) const             {return TMath::ATan2(TMath::Sqrt((x-xTarg)*(x-xTarg)+
                                                               (y-yTarg)*(y-yTarg)), getZ()-zTarg);}
  Float_t getPhi(void) const               {return TMath::ATan2(y-yTarg,x-xTarg);}
  Float_t getSumWt(void) const             {return sumWt;}
  Int_t   getNBins(void) const             {return nBins;}
  void    getPoint(HGeomVector& v) const   {v.setXYZ(x,y,parD-parA*x-parB*y);}
  void    getTarg(HGeomVector& v) const    {v.setXYZ(xTarg,yTarg,zTarg);}
  Int_t   getIndexParent(void) const       {return indexPar;}
  void    getIndexRegChilds(Int_t& first,
                        Int_t& last) const {first=indCh1; last=indCh2;}
  Int_t   getNextIndexChild(Int_t ind) const { //using: ind=-1;ind=getNextIndexChild(ind)
                                              return ind<indCh1 ? indCh1:(ind<indCh2 ? ind+1:-1);}
  Short_t getNMergClust(void) const        {return nMergedClus;}
  Float_t A(void) const                    {return parA;}
  Float_t B(void) const                    {return parB;}
  Float_t C(void) const                    {return 1.;}
  Float_t D(void) const                    {return parD;}
  Float_t getZOnPrPlane(Float_t x,
                        Float_t y) const   {return parD-parA*x-parB*y;}
  Float_t getYOnPrPlane(Float_t x,
                        Float_t z) const   {return parB != 0. ? (parD-parA*x-z)/parB : 0.;}
  Bool_t  getNextWire(Int_t& mod, Int_t& lay, Int_t& cell) const;
  Bool_t  getNextWire(HLocation& loc) const;

  Short_t getClusSizeM1(void) const        {return clusSizeM1;}
  Short_t getNDrTimesM1(void) const        {return nDrTimesM1;} 
  Short_t getNMergClustM1(void) const      {return nMergClusM1;}
  void    getShapeM1(Float_t& s1,
                     Float_t& s2,
                     Float_t& al) const    {s1=sigma1M1;s2=sigma2M1;al=alphaM1;}
  UChar_t getClFnLevelM1(void) const       {return clFnLevelM1;}
  Float_t getAlphaM1(void) const           {return alphaM1;}
  Float_t getSigma1M1(void) const          {return sigma1M1;}
  Float_t getSigma2M1(void) const          {return sigma2M1;}
  Float_t getRatioM1(void) const           {return sigma1M1 > 0. ? sigma2M1/sigma1M1 : 0.;}
  
  UChar_t getClFnLevelM2(void) const       {return clFnLevelM2;}
  Short_t getClusSizeM2(void) const        {return clusSizeM2;}
  Short_t getNDrTimesM2(void) const        {return nDrTimesM2;} 
  Short_t getNMergClustM2(void) const      {return nMergClusM2;}
  void    getShapeM2(Float_t& s1,
                     Float_t& s2,
                     Float_t& al) const    {s1=sigma1M2;s2=sigma2M2;al=alphaM2;}
  Float_t getAlphaM2(void) const           {return alphaM2;}
  Float_t getSigma1M2(void) const          {return sigma1M2;}
  Float_t getSigma2M2(void) const          {return sigma2M2;}
  Float_t getRatioM2(void) const           {return sigma1M2 > 0. ? sigma2M2/sigma1M2 : 0.;}
  Int_t   getSegIndex(void) const          {return segIndex;}
  UChar_t getFakeFlag(void) const          {return fakeFlag;}

  void    print(Bool_t fl=kTRUE) const;
  void    printPos(void) const;
  virtual void printCont(Bool_t fl) const;
  void    setStatus(Short_t st)            {status=st;}
  Short_t getStatus(void) const            {return status;}
  Float_t getBParKickPl(void)              {return errYTarg > 0.00001 ? errZTarg/errYTarg : 1.218;}
  Bool_t  isFixedLevel(void) const         {return (typeClFinder&128)==0;}
  Bool_t  isSegmentAmpCut(void) const      {return (typeClFinder&256)!=0;}
  
  void    calcIntersection(const HGeomVector &r,const HGeomVector &dir,HGeomVector &out) const;
  void    calcIntersection(const HGeomVector &p1,const HGeomVector &p2,Float_t& x,Float_t& y) const;
  void    calcIntersection(const HGeomVector &p,Float_t& x, Float_t& y) const;
  void    fillErrMatClus(Bool_t isCoilOff,HSymMat4& errMatClus);

  ClassDef(HMdcClus,1)
};
  
#endif
