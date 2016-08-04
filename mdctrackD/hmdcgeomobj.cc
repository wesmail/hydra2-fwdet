//*--- AUTHOR : Pechenov Vladimir
//*--- Modified: 12.02.08 V.Pechenov
//*--- Modified: 26.11.07 V.Pechenov
//*--- Modified: 16.08.05 V.Pechenov
//*--- Modified: 07.05.02 V.Pechenov
//*--- Modified: 25.02.99

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcPointPlane - poin on the plane
// HMdcTrap - trapeze in vol. is used for mdc cell sensitive volume
// HMdcTrapPlane - polygon on the plane is used for cell projection calculation
// HMdcPlane - param. of project planes
//
// HMdcPointOnPlane 
//
//     This class keep point on the plane HMdcPlane
//
// HMdcLineParam
//
//     This class keep straight line parameters.
//     Parameters are two point on the two planes
//     (two HMdcPointOnPlane objects).
//
///////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "hmdcgeomobj.h"
//#include <iostream> 
//#include <iomanip>

ClassImp(HMdcPointPlane)
ClassImp(HMdcTrap)
ClassImp(HMdcTrapPlane)
ClassImp(HMdcPlane)
ClassImp(HMdcPointOnPlane)
ClassImp(HMdcLineParam)

void HMdcPointPlane::print() const {
  printf("x=%7f y=%7f\n",x,y);
}

//-----------------------------------------------------
HMdcTrap::HMdcTrap(const HMdcTrap& otrap):TObject(otrap) {
  // constructor creates the copy of obj. Trap
  for(Int_t i=0; i<8; i++) points[i] = otrap.points[i];
  dbPoint = otrap.dbPoint;
}

HGeomVector& HMdcTrap::operator[](Int_t i) {
  if(i<0 || i>7) {
    Error("operator[]","index=%i out of bounds! Set index=0", i);
    return points[0];
  }
  return points[i];
}

void HMdcTrap::copy(HMdcTrap& otrap) const {
  // copy obj "this" to otarp
  for(Int_t i=0; i<8; i++) otrap.points[i]=points[i];
  otrap.dbPoint = dbPoint;
}

void HMdcTrap::set(const HMdcTrap& otrap) {
  // copy obj "this" to otrap
  for(Int_t i=0; i<8; i++) points[i]=otrap.points[i];
  dbPoint = otrap.dbPoint;
}

void HMdcTrap::clear() {
  // copy obj "this" to otrap
  for(Int_t i=0; i<8; i++) points[i].clear();
  dbPoint = -1;
}

void HMdcTrap::transFrom(const HGeomTransform &s) {
  for(Int_t i=0; i<8; i++) points[i]=s.transFrom(points[i]);
}

void HMdcTrap::transTo(const HGeomTransform &s) {
  for(Int_t i=0; i<8; i++) points[i]=s.transTo(points[i]);
}

Bool_t HMdcTrap::getRibInd(Int_t rib,Int_t& ind1,Int_t& ind2) {
  if(rib<0 || rib>11) return kFALSE;
  if(rib < 4) {
    if(dbPoint>=0 && rib==dbPoint) return kFALSE;
    ind1 = rib;
    ind2 = nextPoint(ind1);
    if(ind2 == dbPoint) ind2 = nextPoint(ind2);
  } else if(rib < 8) {
    if(dbPoint>=0 && rib==dbPoint+4) return kFALSE;
    ind1 = rib;
    ind2 = nextPoint(ind1);
    if(ind2 == dbPoint+4) ind2 = nextPoint(ind2);
  } else {
    if(dbPoint>=0 && rib==dbPoint+8) return kFALSE;
    ind1 = rib-8;
    ind2 = ind1+4;
  }
  return kTRUE;
}

Bool_t HMdcTrap::getRibInXYContour(Int_t rib,Int_t& i,Int_t& j) {
  // For rib in X-Y contour all others points must be from one size of rib
  if(!getRibInd(rib,i,j)) return kFALSE;
  Double_t x2 = points[j].getX();
  Double_t y2 = points[j].getY();
  Double_t dX = points[i].getX()-x2;
  Double_t dY = points[i].getY()-y2;
  if(fabs(dX) > fabs(dY)) {
    if(dX == 0.) return kFALSE;
    Double_t coeff = dY/dX;
    Short_t fl = 0;
    for(Int_t k=0;k<8;k++) if(k!=i && k!=j) {
      // For all k  Yk-Yrib(Xk) must have the same sign
      Double_t yt = (points[k].getX()-x2)*coeff + y2 - points[k].getY();
      if(yt > 0.0)      fl |= 1;
      else if(yt < 0.0) fl |= 2;
      if(fl==3) return kFALSE;
    }
  } else {
    if(dY == 0.) return kFALSE;
    Double_t coeff = dX/dY;
    Short_t fl = 0;
    for(Int_t k=0;k<8;k++) if(k!=i && k!=j) {
      Double_t xt = (points[k].getY()-y2)*coeff + x2 - points[k].getX();
      if(xt > 0.0)      fl |= 1;
      else if(xt < 0.0) fl |= 2;
      if(fl==3) return kFALSE;
    }
  }
  return kTRUE;
}

Int_t HMdcTrap::getXYContour(HMdcTrapPlane& tr) {
  // Return number of lines(corners) in contour on XY plane
  // and fill "tr" object by contour points.
  Int_t p1[12],p2[12];
  Int_t nRibs = 0;
  Int_t i,j;
  for(Int_t rib=0;rib<12;rib++) if(getRibInXYContour(rib,i,j)) {
    p1[nRibs] = i;
    p2[nRibs] = j;
    nRibs++;
  }
  tr.clearNPoints();
  Int_t pnt0  = p1[0];
  Int_t pnt   = p2[0];
  tr.addPoint(points[pnt0]);
  tr.addPoint(points[pnt]);
  Int_t nPOut = 2;
  for(Int_t nr=1;nr<nRibs;nr++) if(p1[nr]>=0) {
    if     (p1[nr] == pnt) pnt = p2[nr];
    else if(p2[nr] == pnt) pnt = p1[nr];
    else continue;
    if(pnt == pnt0) break;
    tr.addPoint(points[pnt]);
    nPOut++;
    p1[nr] = -1;
    nr = 0;
  }
  tr.calcDir();
  return nPOut;
}

void HMdcTrap::print() {
  // print Trap;
  printf("\nTrap. from 8 points:\n");
  for(Int_t i=0; i<8; i++) {
    printf("%2i) ",i);
    points[i].print();
  }
}

//------------------------------------------------------
HMdcTrapPlane::HMdcTrapPlane(const HMdcTrapPlane& otrap):TObject(otrap) {
  // constructor creates the copy of obj. Trap
  nPoints   = otrap.nPoints;
  xMinPoint = otrap.xMinPoint;
  xMaxPoint = otrap.xMaxPoint;
  yMinPoint = otrap.yMinPoint;
  yMaxPoint = otrap.yMaxPoint;
  dir       = otrap.dir;
  for(Int_t i=0; i<nPoints; i++) points[i].set(otrap.points[i]);
}

HMdcPointPlane& HMdcTrapPlane:: operator[](Int_t i) {
  if(i<0 || i>=nPoints) {
    Error("operator[]","index=%i out of bounds! Set index=0", i);
    return points[0];
  }
  return points[i];
}

void HMdcTrapPlane::copy(HMdcTrapPlane& otrap) {
  // copy obj "this" to otarp
  otrap.nPoints   = nPoints;
  otrap.xMinPoint = xMinPoint;
  otrap.xMaxPoint = xMaxPoint;
  otrap.yMinPoint = yMinPoint;
  otrap.yMaxPoint = yMaxPoint;
  otrap.dir       = dir;
  for(Int_t i=0; i<nPoints; i++) points[i].copy(otrap.points[i]);
}

void HMdcTrapPlane::set(const HMdcTrapPlane& otrap) {
  // copy obj "this" to otrap
  nPoints   = otrap.nPoints;
  xMinPoint = otrap.xMinPoint;
  xMaxPoint = otrap.xMaxPoint;
  yMinPoint = otrap.yMinPoint;
  yMaxPoint = otrap.yMaxPoint;
  dir       = otrap.dir;
  for(Int_t i=0; i<nPoints; i++) points[i].set(otrap.points[i]);
}

void HMdcTrapPlane::clear() {
  xMinPoint = 0;
  xMaxPoint = 0;
  yMinPoint = 0;
  yMaxPoint = 0;
  dir       = 0;
  for(Int_t i=0; i<nPoints; i++) points[i].clear();
}

void HMdcTrapPlane::clearNPoints(void)  {
  nPoints   = 0;
  xMinPoint = 0;
  xMaxPoint = 0;
  yMinPoint = 0;
  yMaxPoint = 0;
  dir       = 0;
}


void HMdcTrapPlane::print() const {
  // print Trap;
  printf("\nPolygon on the plane from %i points:\n",nPoints);
  for(Int_t i=0; i<nPoints; i++) {
    printf("%2i) ",i);
    points[i].print();
  }
}

void HMdcTrapPlane::addPoint(const HGeomVector& v) {
  if(nPoints<16) {
    points[nPoints].set(v.getX(),v.getY());
    if(nPoints>0) {
      if(points[yMinPoint].getY() > v.getY()) yMinPoint = nPoints;
      if(points[yMaxPoint].getY() < v.getY()) yMaxPoint = nPoints;
      if(points[xMinPoint].getX() > v.getX()) xMinPoint = nPoints;
      if(points[xMaxPoint].getX() < v.getX()) xMaxPoint = nPoints;
    }
    nPoints++;
  } else Error("addPoint","Too many points (>16).");
}

void HMdcTrapPlane::addPoint(const HMdcPointPlane& p) {
  if(nPoints<16) {
    points[nPoints].set(p);
    if(nPoints>0) {
      if(points[yMinPoint].getY() > p.getY()) yMinPoint = nPoints;
      if(points[yMaxPoint].getY() < p.getY()) yMaxPoint = nPoints;
      if(points[xMinPoint].getX() > p.getX()) xMinPoint = nPoints;
      if(points[xMaxPoint].getX() < p.getX()) xMaxPoint = nPoints;
    }
    nPoints++;
  } else Error("addPoint","Too many points (>16).");
}

void HMdcTrapPlane::calcDir(void) {
  if(nPoints<3) dir = 0;
  else {
    Int_t p1 = prevP(yMinPoint);
    Int_t p2 = nextP(yMinPoint);
    Double_t y1 = points[p1].getY();
    Double_t y2 = points[p2].getY();
    Double_t x1 = points[p1].getX();
    Double_t x2 = points[p2].getX();
    if(y1<y2)      x2 = calcX(yMinPoint,p2,y1);
    else if(y1>y2) x1 = calcX(yMinPoint,p1,y2);
    dir = x1 < x2 ? 1 : -1;
  }
}

void HMdcTrapPlane::getXYMinMax(Int_t& xMin,Int_t& xMax,
                                Int_t& yMin,Int_t& yMax) const {
  // Return points number of maximal and minimal value of X and Y.
  xMin = yMin = 0;
  xMax = yMax = 0;
  for(Int_t p=1;p<nPoints;p++) {
    if(points[yMin].getY() > points[p].getY()) yMin = p;
    if(points[yMax].getY() < points[p].getY()) yMax = p;
    if(points[xMin].getX() > points[p].getX()) xMin = p;
    if(points[xMax].getX() < points[p].getX()) xMax = p;
  }
}

Int_t HMdcTrapPlane::twoContoursSum(const HMdcTrapPlane& c1i,
                                    const HMdcTrapPlane& c2i) {
  // Function combine two countours in one
  clearNPoints();
  Bool_t c1imin = c1i.getYMin() < c2i.getYMin();
  const HMdcTrapPlane& c1 = c1imin ? c1i : c2i;
  const HMdcTrapPlane& c2 = c1imin ? c2i : c1i;
  for(Int_t p=c1.yMinPoint;p!=c1.xMaxPoint;p=c1.nextXMaxP(p)) addPoint(c1.points[p]);
  addPoint(c1.points[c1.xMaxPoint]);  
  for(Int_t p=c2.xMaxPoint;p!=c2.xMinPoint;p=c2.nextXMaxP(p)) addPoint(c2.points[p]);
  addPoint(c2.points[c2.xMinPoint]);
  for(Int_t p=c1.xMinPoint;p!=c1.yMinPoint;p=c1.prevXMinP(p)) addPoint(c1.points[p]);
  calcDir(); 
  return nPoints;
}

Bool_t HMdcTrapPlane::getLineInd(Int_t line, Int_t& p1,Int_t& p2) const {
  // Return points indices for line number "line".
  if(line>=nPoints) return kFALSE;
  p1 = line;
  p2 = p1+1;
  if(p2 == nPoints) p2 = 0;
  return kTRUE;
}

Bool_t HMdcTrapPlane::getXCross(Int_t line,Double_t y, Double_t& x) const {
  // Function calculate X for Y="y" on the piece of line number "line".
  // Return kFALSE if it don't cross this line piece.
  Int_t ip1,ip2;
  if( !getLineInd(line,ip1,ip2) ) return kFALSE;       // no line number "line"
  const HMdcPointPlane& p1 = points[ip1];
  const HMdcPointPlane& p2 = points[ip2];
  Double_t y1 = p1.getY();
  Double_t y2 = p2.getY();
  if((y<y1 && y<y2) || (y>y1 && y>y2)) return kFALSE;  // no cross point
  x = (y-y1)/(y1-y2)*(p1.getX()-p2.getX()) + p1.getX();
  return kTRUE;
}

Double_t HMdcTrapPlane::calcX(Int_t ip1,Int_t ip2,Double_t y) const {
  // Function calculate X for Y="y" on the piece of line p1-p2.
  const HMdcPointPlane& p1 = points[ip1];
  const HMdcPointPlane& p2 = points[ip2];
  Double_t y1 = p1.getY();
  Double_t y2 = p2.getY();
  Double_t x1 = p1.getX();
  Double_t x2 = p2.getX();
  return (y-y1)/(y1-y2)*(x1-x2) + x1;
}

Bool_t HMdcTrapPlane::getXminXmax(Double_t y, Double_t& x1,Double_t& x2) {
  // Function calculate X for Y="y" on the piece of line number "line".
  // Return kFALSE if it don't cross this line piece.
  if(y>points[yMaxPoint].getY() || y<points[yMinPoint].getY()) return kFALSE;
  if(dir == 0) calcDir();
  
  Int_t p1,p2;
  for(p2 = nextXMinP(p1=yMinPoint); p2!=yMaxPoint; p2=nextXMinP(p1=p2))
                                               if(y < points[p2].getY()) break;
  x1 = calcX(p1,p2,y);
  for(p2 = nextXMaxP(p1=yMinPoint); p2!=yMaxPoint; p2=nextXMaxP(p1=p2))
                                               if(y < points[p2].getY()) break;
  x2 = calcX(p1,p2,y);
  return kTRUE;
}

Bool_t HMdcTrapPlane::getXminXmax(Double_t y1i,Double_t y2i,
                                  Double_t& x1,Double_t& x2) {
  // Function calculate X minimal (x1) and X maximal (x2) 
  // in Y reagion from y1i to y2i.
  // Return kFALSE if Y region outside of polygon.
  if(dir == 0) calcDir();
  Double_t& y1 = y1i < y2i ? y1i : y2i;
  Double_t& y2 = y1i < y2i ? y2i : y1i;
  if(y1>points[yMaxPoint].getY() || y2<points[yMinPoint].getY()) return kFALSE;
  if(y1 <= points[yMinPoint].getY() && y2 >= points[yMaxPoint].getY()) {
    // Full projection inside region y1 - y2
    x1 = points[xMinPoint].getX();
    x2 = points[xMaxPoint].getX();
    return kTRUE;
  }
    
  if(y2 > points[yMaxPoint].getY()) x1 = x2 = points[yMaxPoint].getX();
  else {
    x1 = +1.0e+20;
    x2 = -1.0e+20;
  }
  // Finding of x1:
  Int_t p1 = yMinPoint;
  for(Int_t p2 = nextXMinP(p1); p1!=yMaxPoint; p2=nextXMinP(p1=p2)) {
    Double_t yp2 = points[p2].getY();
    if(y1 > yp2) continue;
    Double_t yp1 = points[p1].getY();
    if(y1 >= yp1) x1 = TMath::Min(x1,calcX(p1,p2,y1));
    else if(yp1<=y2 && x1>points[p1].getX()) x1 = points[p1].getX();      
    if(y2 > yp2) continue;
    if(y2 >= yp1) x1 = TMath::Min(x1,calcX(p1,p2,y2));
    break;
  }
  // Finding of x2:
  p1  = yMinPoint;
  for(Int_t p2 = nextXMaxP(p1); p1!=yMaxPoint; p2=nextXMaxP(p1=p2)) {
    Double_t yp2 = points[p2].getY();
    if(y1 > yp2) continue;
    Double_t yp1 = points[p1].getY();
    if(y1 >= yp1) x2 = TMath::Max(x2,calcX(p1,p2,y1));
    else if(yp1<=y2 && x2<points[p1].getX()) x2 = points[p1].getX();
    if(y2 > yp2) continue;
    if(y2 >= yp1) x2 = TMath::Max(x2,calcX(p1,p2,y2));
    break;
  }
  return kTRUE;
}
 
//-----------------------------------------------------
void HMdcPlane::setPlanePar(const HGeomTransform& tr) { 
  const HGeomRotation& rt = tr.getRotMatrix();
  const HGeomVector&   tv = tr.getTransVector();
  Double_t c = rt(0)*rt(4)-rt(3)*rt(1);
  if(c == 0.) return;
  parA = (rt(3)*rt(7)-rt(6)*rt(4))/c;
  parB = (rt(6)*rt(1)-rt(0)*rt(7))/c;
  parD = parA*tv.getX()+parB*tv.getY()+tv.getZ();
}

void HMdcPlane::print(void) const {
  printf("Plane equation: %g*x%+g*y+z=%g\n",parA,parB,parD);
}

void HMdcPlane::transTo(const HGeomTransform* trans) {
  HGeomVector p1(  0.,   0.,                parD); // x=0; y= 0; z=D;
  HGeomVector p2(parD, parD, parD*(1.-parA-parB)); // x=D; y= D; z=D*(1-A-B);
  HGeomVector p3(parD,-parD, parD*(1.-parA+parB)); // x=D; y=-D; z=D*(1-A+B);
  p1 = trans->transTo(p1);
  p2 = trans->transTo(p2);
  p3 = trans->transTo(p3);
  HGeomVector p12(p1-p2);
  HGeomVector p13(p1-p3);
  parB = (p12.Z()*p13.X()-p13.Z()*p12.X())/(p13.Y()*p12.X()-p12.Y()*p13.X());
  if(p12.X() != 0.) parA = -(parB*p12.Y() + p12.Z())/p12.X();
  else              parA = -(parB*p13.Y() + p13.Z())/p13.X();
  parD = parA*p1.X() + parB*p1.Y() + p1.Z();
}

void HMdcPlane::transFrom(const HGeomTransform* trans) {
  HGeomVector p1(  0.,   0.,                parD); // x=0; y= 0; z=D;
  HGeomVector p2(parD, parD, parD*(1.-parA-parB)); // x=D; y= D; z=D*(1-A-B);
  HGeomVector p3(parD,-parD, parD*(1.-parA+parB)); // x=D; y=-D; z=D*(1-A+B);
  p1 = trans->transFrom(p1);
  p2 = trans->transFrom(p2);
  p3 = trans->transFrom(p3);
  HGeomVector p12(p1-p2);
  HGeomVector p13(p1-p3);
  parB = (p12.Z()*p13.X()-p13.Z()*p12.X())/(p13.Y()*p12.X()-p12.Y()*p13.X());
  if(p12.X() != 0.) parA = -(parB*p12.Y() + p12.Z())/p12.X();
  else              parA = -(parB*p13.Y() + p13.Z())/p13.X();
  parD = parA*p1.X() + parB*p1.Y() + p1.Z();
}

Double_t HMdcPlane::calcMinDistance(Double_t x, Double_t y, Double_t z) const {
  // Return minimal distance from x,y,z to the plane
  return (parA*x+parB*y+z - parD) / TMath::Sqrt(parA*parA+parB*parB+1.);
}

Double_t HMdcPlane::calcMinDistance(const HGeomVector& p) const {
  // Return minimal distance from x,y,z to the plane
  return calcMinDistance(p.getX(),p.getY(),p.getZ());
}

Double_t HMdcPlane::calcMinDistanceAndErr(const HGeomVector& p,const HGeomVector& dp,
                                          Double_t& err) const {
  // Return minimal distance from x,y,z to the plane with error
  // HGeomVector dp = (errX,errY,errZ) of (x,y,z) in HGeomVector p
  Double_t norm = TMath::Sqrt(parA*parA+parB*parB+1.);
  HGeomVector errV(parA*dp.getX(),parB*dp.getY(),dp.getZ());
  err = errV.length()/norm;
  return (parA*p.getX()+parB*p.getY()+p.getZ()-parD) / norm;
}

HGeomVector HMdcPlane::getNormalVector(void) const {
  // Return normal vector to the plane
  HGeomVector nv(getNormalUnitVector());
  nv *= normalLength();
  return nv;
}

HGeomVector HMdcPlane::getNormalUnitVector(void) const {
  // Return unit vector of the perpendicular to the plane
  HGeomVector nv(parA,parB,1.);
  nv /= nv.length();
  if(parD<0) nv *= -1;
  return nv;
}

//-----------------------------------------------------
HMdcPointOnPlane::HMdcPointOnPlane(HMdcPlane* p) {
  pl = *p;
}
  
HMdcPointOnPlane::HMdcPointOnPlane(HMdcPointOnPlane& p) : 
    HGeomVector(p.x,p.y,p.z) {
  pl = p.pl;
}

void HMdcPointOnPlane::print(void) const {
  printf("Point x=%7f y=%7f z=%7f on the plane %g*x%+g*y+z=%g\n",
      x,y,z,pl.A(),pl.B(),pl.D());
}
    
void HMdcPointOnPlane::transTo(const HGeomTransform* trans) {
  pl.transTo(trans);
  *((HGeomVector*)this)  = trans->transTo(*((HGeomVector*)this));
}

void HMdcPointOnPlane::transFrom(const HGeomTransform* trans) {
  pl.transFrom(trans);
  *((HGeomVector*)this) = trans->transFrom(*((HGeomVector*)this));
}

//-----------------------------------------------------
void HMdcLineParam::setCoorSys(Int_t s,Int_t m) {
  if(s<-1 || s>5) {
    sec = -2;
    mod = -2;
  } else {
    sec = s;
    if(sec==-1 || m<0 || m>3) mod = -1;
    else mod = m;
  }
}

void HMdcLineParam::setSegmentLine(Double_t r, Double_t z,
    Double_t theta, Double_t phi) {
  Double_t cosPhi = TMath::Cos(phi);
  Double_t sinPhi = TMath::Sin(phi);
  Double_t x1     = -r*sinPhi;   
  Double_t y1     =  r*cosPhi; 
  Double_t dZ     = TMath::Cos(theta);
  Double_t dxy    = TMath::Sqrt(1.-dZ*dZ)*100.;
  Double_t x2     = dxy*cosPhi+x1;
  Double_t y2     = dxy*sinPhi+y1;
  Double_t z2     = dZ*100.+z;
  point1.calcPoint(x1,y1,z,x2,y2,z2);
  point2.calcPoint(x1,y1,z,x2,y2,z2);
}

Double_t HMdcLineParam::getPhiRad(void) const {
  Double_t ph = TMath::ATan2(dY(),dX());
  if(ph >= 0.) return ph;
  return TMath::TwoPi()+ph;
}

void HMdcLineParam::transTo(const HGeomTransform* tr,Int_t s,Int_t m) {
  // if s<-2 - don't change sec
  // if m<-2 - don't change mod
  point1.transTo(tr);
  point2.transTo(tr);
  calcDir();
  if(sec>=-2) sec = s;
  if(mod>=-2) mod = m;
}

void HMdcLineParam::transFrom(const HGeomTransform* tr,Int_t s,Int_t m) {
  // if s<-2 - don't change sec
  // if m<-2 - don't change mod
  point1.transFrom(tr);
  point2.transFrom(tr);
  calcDir();
  if(sec>=-2) sec = s;
  if(mod>=-2) mod = m;
}
