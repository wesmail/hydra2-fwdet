#ifndef HMDCGEOMOBJ_H
#define HMDCGEOMOBJ_H

#include "TObject.h"
#include "TObjArray.h"
#include "TArrayI.h"
#include "hgeomvector.h"
#include "hgeomtransform.h"
#include "TMath.h"

class HMdcPointPlane : public TObject {
  protected:
    Double_t x;
    Double_t y;
  public:
    HMdcPointPlane(Double_t xi=0, Double_t yi=0) {x=xi; y=yi;}
    ~HMdcPointPlane(){}
    void     clear()                               {x=y=0.0;}
    void     copy(HMdcPointPlane &point) const     {point.x=x; point.y=y;}
    void     setX(Double_t xn)                     {x=xn;}
    void     setY(Double_t yn)                     {y=yn;}
    void     set(const HMdcPointPlane& point)      {x=point.x; y=point.y;}
    void     set(Double_t xo, Double_t yo)         {x=xo; y=yo;}
    void     setVector(Double_t* vc)               {x=vc[0]; y=vc[1];}
    Double_t getX() const                          {return x;}
    Double_t getY() const                          {return y;}
    void     get(Double_t &xo, Double_t &yo) const {xo=x; yo=y;}
    void     getVector(Double_t* vc) const         {vc[0]=x; vc[1]=y;}
    void     print() const;
  protected:

  ClassDef(HMdcPointPlane,1)
};

class HMdcTrapPlane : public TObject {
  protected:
    HMdcPointPlane points[16];
    Int_t          nPoints;     // number points
    UChar_t        xMinPoint;   // point number with minimal x
    UChar_t        xMaxPoint;   // point number with maximal x
    UChar_t        yMinPoint;   // point number with minimal y
    UChar_t        yMaxPoint;   // point number with maximal y
    Char_t         dir;
  public:
    HMdcTrapPlane() {nPoints=4; dir=0;}
    HMdcTrapPlane(const HMdcTrapPlane& otrap);
    ~HMdcTrapPlane() {}
    HMdcPointPlane& operator[](Int_t i);
    void     print() const;
    void     copy(HMdcTrapPlane& otrap);
    void     set(const HMdcTrapPlane& otrap);
    void     clear();
    void     clearNPoints(void);
    void     addPoint(const HGeomVector& v);
    void     addPoint(const HMdcPointPlane& p);
    Int_t    getNPoints(void) const      {return nPoints;}
    void     getXYMinMax(Int_t& xMin,Int_t& xMax,Int_t& yMin,Int_t& yMax) const;
    Bool_t   getLineInd(Int_t line, Int_t& p1,Int_t& p2) const;
    Bool_t   getXCross(Int_t line,Double_t y, Double_t& x) const;
    Bool_t   getXminXmax(Double_t y, Double_t& x1,Double_t& x2);
    Bool_t   getXminXmax(Double_t y1i,Double_t y2i,Double_t& x1,Double_t& x2);
    Int_t    getXMinPoint(void) const    {return xMinPoint;}
    Int_t    getXMaxPoint(void) const    {return xMaxPoint;}
    Int_t    getYMinPoint(void) const    {return yMinPoint;}
    Int_t    getYMaxPoint(void) const    {return yMaxPoint;}
    Double_t getXMin(void) const         {return points[xMinPoint].getX();}
    Double_t getXMax(void) const         {return points[xMaxPoint].getX();}
    Double_t getYMin(void) const         {return points[yMinPoint].getY();}
    Double_t getYMax(void) const         {return points[yMaxPoint].getY();}
    void     calcDir(void);
    Int_t    twoContoursSum(const HMdcTrapPlane& c1i,
                            const HMdcTrapPlane& c2i);
  protected:
    Double_t calcX(Int_t ip1,Int_t ip2,Double_t y) const;
    Int_t    nextP(Int_t p) const        {p++; return p<nPoints ? p:0;}
    Int_t    prevP(Int_t p) const        {if(p<=0) p=nPoints; return --p;}
    // For nextPXMax and nextPXMin calcDir must be called before.
    Int_t    nextXMaxP(Int_t p) const    {return dir==1 ? nextP(p):prevP(p);}
    Int_t    nextXMinP(Int_t p) const    {return dir==1 ? prevP(p):nextP(p);}
    Int_t    prevXMaxP(Int_t p) const    {return dir==1 ? prevP(p):nextP(p);}
    Int_t    prevXMinP(Int_t p) const    {return dir==1 ? nextP(p):prevP(p);}
  ClassDef(HMdcTrapPlane,1)
};

class HMdcTrap : public TObject {
  protected:
    HGeomVector points[8];
    Char_t      dbPoint;     // double point
  public:
    HMdcTrap()  {}
    HMdcTrap(const HMdcTrap& otrap);
    ~HMdcTrap() {}
    HGeomVector& operator[](Int_t i);
    void   print();
    void   copy(HMdcTrap& otrap) const;
    void   set(const HMdcTrap& otrap);
    void   clear();
    void   transFrom(const HGeomTransform &s);
    void   transTo(const HGeomTransform &s);
    Bool_t getRibInd(Int_t rib,Int_t& ind1,Int_t& ind2);
    void   setDbPointNum(Char_t n)                 {dbPoint = n;}
    Char_t getDbPointNum(void) const               {return dbPoint;}
    Bool_t getRibInXYContour(Int_t rib,Int_t& i,Int_t& j);
    Int_t  getXYContour(HMdcTrapPlane& tr);
  protected:
    Int_t  nextPoint(Int_t i)             {return i==3||i==7 ? i-3 : i+1;}

  ClassDef(HMdcTrap,1)
};

class HMdcPlane : public TObject {
  protected:
    Double_t parA;  // Plane: parA*x+parB*y+c*z=parD  (c=1 !!!)
    Double_t parB;  // parA*parA+parB*parB+c*c must be > 0 and c != 0 !!!
    Double_t parD;
  public:
    HMdcPlane() : parA(0.),parB(0.),parD(0.) {}
    virtual ~HMdcPlane(){;}
    inline HMdcPlane& operator = (const HMdcPlane& p);
    void        print(void) const;
    inline void setPlanePar(const HMdcPlane& p);
    inline void setPlanePar(Double_t al, Double_t bl, Double_t cl, Double_t dl);
    void        setPlanePar(const HGeomTransform& tr);
    Double_t    A() const                                {return parA;}
    Double_t    B() const                                {return parB;}
    Double_t    C() const                                {return 1.;}
    Double_t    D() const                                {return parD;}
    Double_t    getZOnPlane(Double_t x,Double_t y) const {return parD-parA*x-parB*y;}
    Double_t    getYOnPlane(Double_t x,Double_t z) const {return parB!=0.?(parD-parA*x-z)/parB:0.;}
    Double_t    getXOnPlane(Double_t y,Double_t z) const {return parA!=0.?(parD-parB*y-z)/parA:0.;}
    Double_t    dZdX(void) const                         {return -parA;}
    Double_t    dZdY(void) const                         {return -parB;}
    Double_t    calcMinDistance(Double_t x,Double_t y,Double_t z) const;
    Double_t    calcMinDistance(const HGeomVector& p) const;
    Double_t    calcMinDistanceAndErr(const HGeomVector& p,const HGeomVector& dp,Double_t& err) const;
    Double_t    normalLength(void) const                 {return TMath::Abs(calcMinDistance(0,0,0));}
    HGeomVector getNormalVector(void) const;
    HGeomVector getNormalUnitVector(void) const;
    void        transTo(const HGeomTransform* trans);
    void        transFrom(const HGeomTransform* trans);
    inline void getPlanePar(HMdcPlane& p) const;
    inline void getPlanePar(Double_t& a,Double_t& b,Double_t& d) const;
    inline void calcIntersection(const HGeomVector &r,const HGeomVector &dir,
                                  HGeomVector &out) const;
    inline void calcIntersection(const HGeomVector &r,const HGeomVector &dir,
                                 Double_t& x, Double_t& y) const;
    inline void calcIntersection(Double_t x1,Double_t y1,Double_t z1,
                                 Double_t x2, Double_t y2, Double_t z2,
                                 Double_t& x, Double_t& y, Double_t& z) const;
    inline void calcSegIntersec(Float_t z1,Float_t r,Float_t theta,Float_t phi,
                                Float_t& x, Float_t& y, Float_t& z) const;
    inline void calcSegIntersec(Double_t z1,Double_t r,Double_t theta,
                                Double_t phi,Double_t& x, Double_t& y, Double_t& z) const;

    ClassDef(HMdcPlane,1)
};

class HMdcPointOnPlane : public HGeomVector {
  protected:
    HMdcPlane pl;     // plane
  public:
    HMdcPointOnPlane(){}
    HMdcPointOnPlane(HMdcPlane* p);
    HMdcPointOnPlane(HMdcPointOnPlane& p);
    virtual ~HMdcPointOnPlane(){}
    
    void setPlane(const HMdcPlane* p)        {pl=*p;}
    void setX(Double_t xi)                   {x=xi; calcZ();}
    void setY(Double_t yi)                   {y=yi; calcZ();}
    void setXY(Double_t xi, Double_t yi)     {x=xi; y=yi; calcZ();}
    void setXY(const HMdcPointOnPlane& pi)   {x=pi.x; y=pi.y; calcZ();}
    void setXYZ(const HMdcPointOnPlane& pi)  {x=pi.x; y=pi.y; z=pi.z;}
    void addToX(Double_t xa)                 {x+=xa; calcZ();}
    void addToY(Double_t ya)                 {y+=ya; calcZ();}
    void addToXY(Double_t xa,Double_t ya)    {x+=xa; y+=ya; calcZ();}
    void subFromXY(Double_t xs,Double_t ys)  {x-=xs; y-=ys; calcZ();}
    void calcPoint(Double_t x1,Double_t y1,Double_t z1,
                   Double_t x2,Double_t y2,Double_t z2) {
                                pl.calcIntersection(x1,y1,z1,x2,y2,z2,x,y,z);}
    void transTo(const HGeomTransform* trans);
    void transFrom(const HGeomTransform* trans);
    
    void calcPoint(const HGeomVector& pnt,
                   const HGeomVector& dir)   {pl.calcIntersection(pnt,dir,*this);}
    const HGeomVector& getVector(void) const {return *this;}
    HMdcPlane* getPlane(void)                {return &pl;}
    Double_t getX(void) const                {return x;}
    Double_t getY(void) const                {return y;}
    Double_t getZ(void) const                {return z;}
    Double_t X(void) const                   {return x;}
    Double_t Y(void) const                   {return y;}
    Double_t Z(void) const                   {return z;}
    Double_t dZdX(void) const                {return pl.dZdX();}
    Double_t dZdY(void) const                {return pl.dZdY();}
    void     getVector(HGeomVector& v) const {v.setXYZ(x,y,z);}

    inline HMdcPointOnPlane& operator = (HMdcPointOnPlane& p);
    void print() const;
      
  protected:
    void calcZ(void)                         {z=pl.getZOnPlane(x,y);}

    ClassDef(HMdcPointOnPlane,0)
};

class HMdcLineParam : public TObject {
  protected:
    HMdcPointOnPlane point1;         // First point on the line
    HMdcPointOnPlane point2;         // Second point on the line
    HGeomVector      dir;            // Line direction
    Int_t            sec;            // line in coor.sys. of this sector
                                     // -1 - lab.system
                                     // -2 - not setted system
    Int_t            mod;            // line in coor.sys. of this module
                                     // -1 - lab. or sector system
                                     // -2 - not setted system
  public:
    HMdcLineParam() {sec=-1;mod=-1;}
    ~HMdcLineParam(){}
    inline void setParam(Double_t x1, Double_t y1, Double_t z1,
                         Double_t x2, Double_t y2, Double_t z2);
    inline void setParam(const HGeomVector& point, const HGeomVector& dir); 
    void setCoorSys(Int_t s,Int_t m=-1);
    void setFirstPlane(HMdcPlane* fp)  {point1.setPlane(fp);}
    void setSecondPlane(HMdcPlane* sp) {point2.setPlane(sp);}
    inline void   copyPlanes(HMdcLineParam& lp);
    inline void   copyParam(const HMdcLineParam& lp);
    inline Bool_t compare(const HMdcLineParam& tp, Double_t* lim);
    inline Bool_t compare(const HMdcLineParam& tp,Double_t* lim,Double_t fact);
    inline void   setParam(Int_t ip, Double_t val);          // ip>=0 && ip<=3
    inline void   addToParam(Int_t ip, Double_t add);        // ip>=0 && ip<=3
    inline void   addToParam(Int_t ip1,Double_t add1,Int_t ip2,Double_t add2);
    inline void   setScParam(const Double_t* v1,Double_t s,const Double_t* v2);
    inline void   setScParam(const HMdcLineParam& tp,const Double_t* v1, 
                             Double_t s, const Double_t* v2);
    inline void   getLinePar(Double_t *v);
    void setSegmentLine(Double_t r, Double_t z, Double_t theta, Double_t phi);
    inline HMdcLineParam& operator = (HMdcLineParam& lp);
    
    void transTo(const HGeomTransform* tr,Int_t sec=-3,Int_t mod=-3);
    void transFrom(const HGeomTransform* tr,Int_t sec=-3,Int_t mod=-3);
    
    Int_t getSec(void) const         {return sec;}
    Int_t getMod(void) const         {return mod;}
    HMdcPlane* getFirstPlane(void)   {return point1.getPlane();}
    HMdcPlane* getSecondPlane(void)  {return point2.getPlane();}
    const HMdcPointOnPlane& getFisrtPoint(void)  const {return point1;}
    const HMdcPointOnPlane& getSecondPoint(void) const {return point2;}
    const HGeomVector& getDir(void) const              {return dir;}
    Double_t x1(void) const          {return point1.X();}
    Double_t y1(void) const          {return point1.Y();}
    Double_t z1(void) const          {return point1.Z();}
    Double_t x2(void) const          {return point2.X();}
    Double_t y2(void) const          {return point2.Y();}
    Double_t z2(void) const          {return point2.Z();}
    Double_t dX(void) const          {return dir.getX();}
    Double_t dY(void) const          {return dir.getY();}
    Double_t dZ(void) const          {return dir.getZ();}
    Double_t getThetaRad(void) const {return TMath::ATan2(dXY(),dZ());}
    Double_t getPhiRad(void) const;
    Double_t getThetaDeg(void) const {return getThetaRad()*TMath::RadToDeg();}
    Double_t getPhiDeg(void) const   {return getPhiRad()*TMath::RadToDeg();}
    
    // Derivatives:
    Double_t dZ1dX1(void) const      {return point1.dZdX();}
    Double_t dZ2dX2(void) const      {return point2.dZdX();}
    Double_t dZ1dY1(void) const      {return point1.dZdY();}
    Double_t dZ2dY2(void) const      {return point2.dZdY();}
    Double_t dDirXdX1(void) const    {return -1.;}
    Double_t dDirXdX2(void) const    {return +1.;}
    Double_t dDirYdY1(void) const    {return -1.;}
    Double_t dDirYdY2(void) const    {return +1.;}
    
    Double_t dDirZdX1(void) const    {return -point1.dZdX();}
    Double_t dDirZdX2(void) const    {return +point2.dZdX();}
    Double_t dDirZdY1(void) const    {return -point1.dZdY();}
    Double_t dDirZdY2(void) const    {return +point2.dZdY();}
  private:
    void     calcDir(void);
    void     calcDirXZ(void);
    void     calcDirYZ(void);
    Double_t dXY(void) const         {return TMath::Sqrt(dX()*dX()+dY()*dY());}
    
  ClassDef(HMdcLineParam,0)
};

// ---- inlines--------------------

inline HMdcPointOnPlane& HMdcPointOnPlane::operator = (HMdcPointOnPlane& p) {
  pl = p.pl;
  x  = p.x;
  y  = p.y;
  z  = p.z;
  return *this;
}
  
inline HMdcLineParam& HMdcLineParam::operator = (HMdcLineParam& lp) {
  point1 = lp.point1;
  point2 = lp.point2;
  dir    = lp.dir;
  sec    = lp.sec;
  mod    = lp.mod;
  return *this;
}

inline void HMdcLineParam::getLinePar(Double_t *v) {
  v[0] = x1();
  v[1] = y1();
  v[2] = z1();
  v[3] = dX();
  v[4] = dY();
  v[5] = dZ();
}

inline void HMdcLineParam::copyPlanes(HMdcLineParam& lp) {
  point1.setPlane(lp.point1.getPlane()); 
  point2.setPlane(lp.point2.getPlane());
  sec = lp.sec;
  mod = lp.mod;
}

inline void HMdcLineParam::copyParam(const HMdcLineParam& lp) {
  point1.setXYZ(lp.point1);
  point2.setXYZ(lp.point2); 
  dir = lp.dir;
}
  
inline void HMdcLineParam::setParam(Double_t x1i, Double_t y1i, Double_t z1i,
                                    Double_t x2i, Double_t y2i, Double_t z2i) {
  point1.calcPoint(x1i,y1i,z1i,x2i,y2i,z2i);
  point2.calcPoint(x1i,y1i,z1i,x2i,y2i,z2i);
  calcDir();
}

inline void HMdcLineParam::setParam(const HGeomVector& point,
                                    const HGeomVector& dir) {
  point1.calcPoint(point,dir);
  point2.calcPoint(point,dir);
  calcDir();
}

inline Bool_t HMdcLineParam::compare(const HMdcLineParam& tp, Double_t* lim) {
  if(TMath::Abs(point1.X()-tp.point1.X()) > lim[0]) return kTRUE;
  if(TMath::Abs(point1.Y()-tp.point1.Y()) > lim[1]) return kTRUE;
  if(TMath::Abs(point2.X()-tp.point2.X()) > lim[2]) return kTRUE;
  if(TMath::Abs(point2.Y()-tp.point2.Y()) > lim[3]) return kTRUE;
  return kFALSE;
}

inline Bool_t HMdcLineParam::compare(const HMdcLineParam& tp, Double_t* lim,
    Double_t fac) {
  if(TMath::Abs(point1.X()-tp.point1.X()) > lim[0]*fac) return kTRUE;
  if(TMath::Abs(point1.Y()-tp.point1.Y()) > lim[1]*fac) return kTRUE;
  if(TMath::Abs(point2.X()-tp.point2.X()) > lim[2]*fac) return kTRUE;
  if(TMath::Abs(point2.Y()-tp.point2.Y()) > lim[3]*fac) return kTRUE;
  return kFALSE;
}

inline void HMdcLineParam::setParam(Int_t ip, Double_t val) {
  if(ip==0)      {point1.setX(val); calcDirXZ();}
  else if(ip==1) {point1.setY(val); calcDirYZ();}
  else if(ip==2) {point2.setX(val); calcDirXZ();}
  else if(ip==3) {point2.setY(val); calcDirYZ();}
}

inline void HMdcLineParam::addToParam(Int_t ip1, Double_t add1, Int_t ip2,
    Double_t add2) {
  addToParam(ip1,add1); 
  addToParam(ip2,add2);
}

inline void HMdcLineParam::addToParam(Int_t ip, Double_t add) {
  if(ip==0)      {point1.addToX(add); calcDirXZ();}
  else if(ip==1) {point1.addToY(add); calcDirYZ();}
  else if(ip==2) {point2.addToX(add); calcDirXZ();}
  else if(ip==3) {point2.addToY(add); calcDirYZ();}
}

inline void HMdcLineParam::setScParam(const Double_t* v1, Double_t s,
    const Double_t* v2) {
  // x = x_old - v1[0]*s*v2[0]
  // y = y_old - v1[1]*s*v2[1]
  // ...
  point1.subFromXY(v1[0]*s*v2[0],v1[1]*s*v2[1]);
  point2.subFromXY(v1[2]*s*v2[2],v1[3]*s*v2[3]);
  calcDir();
}

inline void HMdcLineParam::setScParam(const HMdcLineParam& tp,
    const Double_t* v1, Double_t s, const Double_t* v2) {
  // x = x_tp - v1[0]*s*v2[0]
  // y = y_tp - v1[1]*s*v2[1]
  // ...
  point1.setXY(tp.point1.getX()-v1[0]*s*v2[0],tp.point1.getY()-v1[1]*s*v2[1]);
  point2.setXY(tp.point2.getX()-v1[2]*s*v2[2],tp.point2.getY()-v1[3]*s*v2[3]);
  calcDir();
}

inline void HMdcLineParam::calcDir(void) {
  dir.setXYZ(point2.X()-point1.X(),point2.Y()-point1.Y(),point2.Z()-point1.Z());
}

inline void HMdcLineParam::calcDirXZ(void) {
  dir.setX(point2.X()-point1.X());
  dir.setZ(point2.Z()-point1.Z());
}

inline void HMdcLineParam::calcDirYZ(void) {
  dir.setY(point2.Y()-point1.Y());
  dir.setZ(point2.Z()-point1.Z());
}

inline void HMdcPlane::setPlanePar(const HMdcPlane& p) {
  parA = p.parA;
  parB = p.parB;
  parD = p.parD;
}

inline void HMdcPlane::setPlanePar(Double_t al, Double_t bl, Double_t cl, Double_t dl) {
  if(cl != 0.) {
    parA = al/cl;
    parB = bl/cl;
    parD = dl/cl;
  }
}
inline HMdcPlane& HMdcPlane::operator = (const HMdcPlane& p) {
  parA = p.parA;
  parB = p.parB;
  parD = p.parD;
  return *this;
}

inline void HMdcPlane::getPlanePar(HMdcPlane& p) const {
  p.parA = parA;
  p.parB = parB;
  p.parD = parD;
}

inline void HMdcPlane::getPlanePar(Double_t& a,Double_t& b,Double_t& d) const {
  a = parA;
  b = parB;
  d = parD;
}
    
inline void HMdcPlane::calcIntersection(const HGeomVector &r, const HGeomVector &dir,
					HGeomVector &out) const {
  // Calculating a cross of line r,dir (point in vol. and direction)
  // with plane parA*x+parB*y+z=parD
  Double_t x1  = r.getX();
  Double_t y1  = r.getY();
  Double_t z1  = r.getZ();
  Double_t dX  = dir.getX();
  Double_t dY  = dir.getY();
  Double_t dZ  = dir.getZ();
  Double_t del = 1./(parA*dX+parB*dY+dZ);
  Double_t x   = (dX*(parD-z1-parB*y1)+x1*(parB*dY+dZ))*del;
  Double_t y   = (dY*(parD-z1-parA*x1)+y1*(parA*dX+dZ))*del;
  out.setXYZ(x,y,-parA*x-parB*y+parD);
}

inline void HMdcPlane::calcIntersection(const HGeomVector &r, const HGeomVector &dir,
                                        Double_t& x, Double_t& y) const {
  // Calculating a cross of line r,dir (point in vol. and direction)
  // with plane parA*x+parB*y+z=parD
  Double_t x1  = r.getX();
  Double_t y1  = r.getY();
  Double_t z1  = r.getZ();
  Double_t dX  = dir.getX();
  Double_t dY  = dir.getY();
  Double_t dZ  = dir.getZ();
  Double_t del = 1./(parA*dX+parB*dY+dZ);
  x = (dX*(parD-z1-parB*y1)+x1*(parB*dY+dZ))*del;
  y = (dY*(parD-z1-parA*x1)+y1*(parA*dX+dZ))*del;
}

inline void HMdcPlane::calcSegIntersec(Float_t z1,Float_t r,Float_t theta,Float_t phi,
				       Float_t& x, Float_t& y, Float_t& z) const {
  // Calculating a cross of MdcSeg line with plane parA*x+parB*y+z=parD
  Float_t cosPhi = TMath::Cos(phi);
  Float_t sinPhi = TMath::Sin(phi);
  Float_t x1     = -r*sinPhi;   
  Float_t y1     = r*cosPhi; 
  Float_t dZ     = TMath::Cos(theta);
  Float_t dxy    = TMath::Sqrt(1.-dZ*dZ);
  Float_t dX     = dxy*cosPhi;
  Float_t dY     = dxy*sinPhi;
  Float_t del    = 1/(parA*dX+parB*dY+dZ);
  x = (dX*(parD-z1-parB*y1)+x1*(parB*dY+dZ))*del;
  y = (dY*(parD-z1-parA*x1)+y1*(parA*dX+dZ))*del;
  //  z = (dZ*(parD-parA*x1-parB*y1)+z1*(parA*dX+parB*dY))*del; 
  z = parD-parA*x-parB*y;
}

inline void HMdcPlane::calcSegIntersec(Double_t z1,Double_t r,Double_t theta,
				       Double_t phi,Double_t& x, Double_t& y, Double_t& z) const {
  // Calculating a cross of MdcSeg line with plane parA*x+parB*y+z=parD
  Double_t cosPhi = TMath::Cos(phi);
  Double_t sinPhi = TMath::Sin(phi);
  Double_t x1     = -r*sinPhi;   
  Double_t y1     =  r*cosPhi; 
  Double_t dZ     = TMath::Cos(theta);
  Double_t dxy    = sqrt(1.-dZ*dZ);
  Double_t dX     = dxy*cosPhi;
  Double_t dY     = dxy*sinPhi;
  Double_t del    = 1/(parA*dX+parB*dY+dZ);
  x = (dX*(parD-z1-parB*y1)+x1*(parB*dY+dZ))*del;
  y = (dY*(parD-z1-parA*x1)+y1*(parA*dX+dZ))*del;
  //  z = (dZ*(parD-parA*x1-parB*y1)+z1*(parA*dX+parB*dY))*del; 
  z = parD-parA*x-parB*y;
}

inline void HMdcPlane::calcIntersection(Double_t x1,Double_t y1,Double_t z1,
					Double_t x2, Double_t y2, Double_t z2,
					Double_t& x, Double_t& y, Double_t& z) const {
  // Calcul. a cross of the line with plane parA*x+parB*y+c*z=parD
  Double_t dX  = x2-x1;
  Double_t dY  = y2-y1;
  Double_t dZ  = z2-z1;
  Double_t del = 1/(parA*dX+parB*dY+dZ);
  x = (dX*(parD-z1-parB*y1)+x1*(parB*dY+dZ))*del;
  y = (dY*(parD-z1-parA*x1)+y1*(parA*dX+dZ))*del;
  z = parD-parA*x-parB*y;
}

#endif  /*!HMDCGEOMOBJ_H*/
