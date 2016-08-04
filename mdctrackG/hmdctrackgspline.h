#ifndef HMDCTRACKGSPLINE_H
#define HMDCTRACKGSPLINE_H
#include "TNamed.h"
#include <iostream>
#include <fstream>
#include "TString.h"
#include "TMath.h" 
#include "hmdctrackgfield.h"
#include "hmdctrackgcorrections.h"
#include "hmdctrackgcorrpar.h"
#include "hgeomvector.h"
#include "hmdcseg.h"
#include "hmdckickplane.h"
#include "hmdcgetcontainers.h"
#include "heventheader.h"
class HMdcTrackGSpline : public TNamed{
private:
   Double_t pi;
   Double_t XX,YY,ZZ;
   Double_t XXdir,YYdir,ZZdir;
   Double_t distfield;
   Double_t errorY;
   Int_t polarity;//polarity of particle
   Double_t target;//targets z position in mm
   Double_t A1,B1,D1;
   Double_t A2,B2,D2;
   Double_t A3,B3,D3;
   Double_t A4,B4,D4;
   Int_t N,N1,N2;//number of points in the field region
   HGeomVector *equationXY;
   Double_t *x;
   Double_t *Bx,*By,*Bz;//Field components
   Double_t phi,teta;//in radian
   Double_t *XZ,*YZ,*XZP,*YZP;//parameters of spline fit
   Double_t *FXZ,*FYZ,*FXZP,*FYZP;//parameters of spline fit
   Double_t *dydz,*dxdz,*d2ydz,*d2xdz, *curv;
   Double_t *x2,*zz2,*z2,*y2;
   Double_t precon,preconErr; //reconstructed momentum
   Double_t precon111;
   Double_t *dist;
   Double_t metaDistance;
   Double_t tetadig,phidig,phidigg,teta2dig,phi2dig;//in degrees
   HMdcTrackGField*  b;
   HMdcKickPlane *kickplane;
   HMdcTrackGCorrections* c;
   HMdcTrackGCorrections *corrScan[3];
   HGeomVector *point; //Intitial points from MDC's
   HGeomVector *fieldpoints,*secDer,*Tfieldpoints,*BB;//points selected in field region
   HGeomVector *field;//field components 
   Float_t fScal;
   Bool_t isInitialized; //! if middle Plane params have been set =kTRUE
   Bool_t isKickIsInitialized;
   void getPoint();
   void transteta();
   void transPhi();
   // void spline(HGeomVector *,Double_t *,Double_t *,Double_t *,Double_t *,Int_t);
   void init1();
   void getpoints();
   void fieldequation();
   void solveMomentum();
   Int_t getMomentum(Bool_t,Int_t);
   void transFieldpoints();
   void getcorrE(Int_t,Int_t,Int_t,Double_t &,Int_t);
   void getcorrP(Int_t,Int_t,Int_t,Double_t &,Int_t);
   
   Double_t qSpline;
   void initParams(Int_t,Int_t);
   void clearParams();
   void remove(Double_t *);
   void remove(HGeomVector *);
   Double_t middleA,middleB,middleD;
   Double_t middleA4,middleB4,middleD4;                     
   void calcErrors(Double_t *);
   //  Double_t SplineMinimize(HGeomVector *eq,Int_t size);
   HGeomVector SegmentPoints;
   Float_t zGlobal;
   Bool_t isSecDerY;
 

   HGeomVector geomParams[6][4];
   Double_t    middleGA[6][4];
   Double_t    middleGB[6][4];
   Double_t    middleGD[6][4];
public:
   Float_t calcIOMatching(HMdcSeg *segments[2]);
   void checkSecDer(Double_t *,Double_t * );
   void getXYpoint(Float_t *,Float_t *,Float_t *);
   void getField(Float_t *,Float_t *,Float_t *);
   Double_t SplineMinimize(HGeomVector *,HGeomVector *,Int_t size);
   void SolveDiffEquation(HGeomVector *,HGeomVector *,Int_t ,Float_t *,Float_t *);
   Double_t det3(Double_t a[3][3]);
   void spline(HGeomVector *,Double_t *,Double_t *,Double_t *,Double_t *,Int_t);
   
   Float_t calcTarDist(HVertex &,HMdcSeg *SEG,HGeomTransform *);
   Float_t calcTarDist(HGeomVector &,HGeomVector &,HGeomVector &);
   HGeomVector getSegmentPoints() {return SegmentPoints;}
   Double_t getFieldDistance();
   
   
   Double_t getqSpline() const {return qSpline;}
   Double_t getErrP() {return preconErr;}
   void calcSegPoints(HMdcSeg *seg[2], HGeomVector *);
   void calcSegPoints(HGeomVector *,HMdcSeg *seg);
   void mixPoints(HGeomVector &, HGeomVector &);
   void calcSegPoints123(HMdcSeg *seg[2], HGeomVector *);
   void calcSegPoints123P4(HMdcSeg *seg[2], HGeomVector *);
   Int_t getPolarity() const {return polarity;}
   void setMagnetScaling(Float_t FScal) {fScal=FScal;}
   void equationofLine(Double_t*  ,Double_t*,Double_t ,Double_t &);
   void equationofLine3D(HGeomVector,HGeomVector,Double_t,Double_t &,Double_t &); 
   HGeomVector calcMetaDir(HMdcSeg *, HGeomVector );
   //Calculates direction from kickplane to META, META hits are suposed
   //to be in sector coordinate system;
     HGeomVector calcKickIntersection(HMdcSeg *);
   
   //calculates intersection of segment with kickplane;
   void calcKickIntersectionErr(HMdcSeg *,HGeomVector &, HGeomVector &);
   Double_t getDistField() const {return distfield;}
   HGeomVector getPointOne() const {return point[1];}
   void calcInter(Double_t,Double_t,Double_t,HGeomVector,HGeomVector,HGeomVector &);
   HMdcTrackGSpline(const Char_t * name="Spline",const Char_t * title="Spline");
   ~HMdcTrackGSpline();
   void setDataPointer(HMdcTrackGField* field = 0,HMdcTrackGCorrections* corr = 0);
   void setCorrScan(HMdcTrackGCorrections* corr[]);
   
   void setF(HMdcTrackGField* field) {b=field;}
   void setCorrPointer(HMdcTrackGCorrections* corr=0);
   void setKickPointer(HMdcKickPlane *kickpointer=0);
   Double_t calcMomentum(HGeomVector *,Bool_t cond=kTRUE,Double_t targetz=0.,Int_t nch=4);
   Double_t calcMomentum(HMdcSeg *seg[2],Bool_t cond=kTRUE,Double_t targetz=0.);
   Double_t calcMomentum123(HMdcSeg *seg[2],Bool_t cond=kTRUE,Double_t targetz=0.);
   Double_t calcMomentum123P4(HMdcSeg *seg[2],Bool_t cond=kTRUE,Double_t targetz=0.);
      void getDistance(Double_t  *);
   Double_t getMetaDistance(Double_t, Double_t, Double_t);
   void transSpline(Double_t,Double_t,HGeomVector &,HGeomVector &);
   void transSplineB(Double_t,Double_t,HGeomVector &,HGeomVector &);
   void transSplineLocal(Double_t ,Double_t , HGeomVector &in,HGeomVector & out);
   HMdcTrackGCorrections *scanCorrections();      
   void initPlanes();
   void setZGlobal(Float_t _zGlobal) {zGlobal=_zGlobal;}
   void initMiddleParams(HGeomVector );
   void initMiddleParamsP4(HGeomVector );
   HGeomVector takeMiddleParams( HMdcGetContainers * ,Int_t ,Int_t);
   Int_t takeMiddleParams(const HGeomTransform* , Int_t , Int_t);
   void initMiddleParamsAll();
   Bool_t splineIsInitialized() {return isInitialized;}
   Bool_t splineKickIsInitialized() {return isKickIsInitialized;}
   
   ClassDef(HMdcTrackGSpline,0)
       };
#endif
       
       

     
     



