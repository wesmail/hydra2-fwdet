//*-- Author : M. Sanchez
//*-- Modified : 07.03.2001 (M. Sanchez)

#include  "hgeomvertexfit2.h"
#include <iostream>

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////
//HGeomVertexFit
//
//  Calculates the point of maximun approach to any given
//set of n tracks with user specified weights.
//
//  To start the operation the function reset() is called, then
//addLine() should be used on all tracks on the sample and 
//finally getVertex() will provide the desired point.
//
//  Note that getVertex() is not destructive, you can continue
//adding lines after getVertex() was called
////////////////////////////////////////////////

HGeomVertexFit2::HGeomVertexFit2(void)  {
  // The default constructor
}

HGeomVertexFit2::~HGeomVertexFit2(void) {
  // Everything that is constructed has to be destructed
}

void HGeomVertexFit2::addLine(const HGeomVector &r, const HGeomVector &alpha,
			  HSymMat4 &cov,Float_t zv,const Double_t w) {
  // Function to add lines to the fit.
  // Input
  //   r --> A point in the straight line
  //   alpha --> Direction vector
  //   w --> weight of this line in the fit
  //   cov --> Covariance matrix with elements
  //   		sigma(x, y, x', y')
  //   zv --> Stimation of z coord. of vertex
  Float_t sx = alpha.getX() / alpha.getZ();
  Float_t sy = alpha.getY() / alpha.getZ();
  Float_t x0 = r.getX() - r.getZ()*sx;
  Float_t y0 = r.getY() - r.getZ()*sy;

  //Extrapolate covariance matrix
  Float_t cov_x_x = cov(0,0) - alpha.getZ()*(2*cov(0,2) - alpha.getZ()*cov(2,2));
  Float_t cov_y_y = cov(1,1) - alpha.getZ()*(2*cov(1,3) - alpha.getZ()*cov(3,3));
  Float_t cov_sx_sx = cov(2,2);
  Float_t cov_sy_sy = cov(3,3);
  Float_t cov_x_y = cov(0,1) - alpha.getZ()*(cov(1,2) + cov(0,3) - alpha.getZ()*cov(2,3));
  Float_t cov_sx_sy = cov(2,3);
  Float_t cov_x_sx = cov(0,2) - alpha.getZ()*cov(2,2);
  Float_t cov_x_sy = cov(0,3) - alpha.getZ()*cov(2,3);
  Float_t cov_y_sx = cov(1,2) - alpha.getZ()*cov(2,3);
  Float_t cov_y_sy = cov(1,3) - alpha.getZ()*cov(3,3);

  //Covariance matrix for vertex: V
  HSymMat2 V;
  V(0,0) = cov_x_x + zv*(2*cov_x_sx + zv*cov_sx_sx);
  V(0,1) = cov_x_y + zv*(cov_y_sx + cov_x_sy + zv*cov_sx_sy);
  V(1,1) = cov_y_y + zv*(2*cov_y_sy + zv*cov_sy_sy); 

  Float_t det = V(0,0)*V(1,1) - V(0,1)*V(0,1);
  Float_t Sxx = V(0,0) / det;
  Float_t Syy = V(1,1) / det;
  Float_t Sxy = -V(0,1) / det;
  //Sxx = 1; Syy=1; Sxy=0;

  fM(0,0)= Sxx;
  fM(0,1)= Sxy;
  fM(0,2)= -(Sxx*sx + Sxy*sy);
  fM(1,1)= Syy;
  fM(1,2)= -(Sxy*sx + Syy*sy);
  fM(2,2)= -(Sxx*sx*sx + Syy*sy*sy + 2*Sxy*sx*sy);

  fSys(0,0)+=fM(0,0);
  fSys(0,1)+=fM(0,1);
  fSys(0,2)+=fM(0,2);
  fSys(1,1)+=fM(1,1);
  fSys(1,2)+=fM(1,2);
  fSys(2,2)+=fM(2,2);

  fB.X()+=Sxx*x0 + Sxy*y0;
  fB.Y()+=Sxy*x0 + Syy*y0;
  fB.Z()+=Sxx*x0*sx + Syy*y0*sy + Sxy*(x0*sx + sx*y0);
}

void HGeomVertexFit2::getVertex(HGeomVector &out) {
  // This method fills the vector "out" with the coordinates
  //of the point of maximun approach to the lines added with
  //addLine()
  Double_t det=0;

  det=fSys(0,0)*fSys(1,1)*fSys(2,2) + fSys(0,1)*fSys(1,2)*fSys(0,2) 
    + fSys(0,1)*fSys(1,2)*fSys(0,2) - fSys(0,2)*fSys(1,1)*fSys(0,2) 
    - fSys(0,1)*fSys(0,1)*fSys(2,2) - fSys(1,2)*fSys(1,2)*fSys(0,0);

  fM(0,0)=fSys(1,1) * fSys(2,2) - fSys(1,2) * fSys(1,2);
  fM(0,1)=fSys(1,2) * fSys(0,2) - fSys(0,1) * fSys(2,2);
  fM(0,2)=fSys(0,1) * fSys(1,2) - fSys(1,1) * fSys(0,2);
  fM(1,1)=fSys(0,0) * fSys(2,2) - fSys(0,2) * fSys(0,2);
  fM(1,2)=fSys(0,1) * fSys(0,2) - fSys(0,0) * fSys(1,2);
  fM(2,2)=fSys(0,0) * fSys(1,1) - fSys(0,1) * fSys(0,1);
  fM(1,0)=fM(0,1);
  fM(2,0)=fM(0,2);
  fM(2,1)=fM(1,2);
  if(det==0){
      out.setXYZ(-1000.,-1000.,-1000.);
      return;
  }
  fM/=(det);

  #if DEBUG_LEVEL>1
  cout << "Det= " << det << endl;
  cout << "Equation system: \n";
  for (Int_t i=0;i<3;i++) {
    for (Int_t j=0;j<3;j++) cout << fSys(i,j) << "\t";
    cout << fB(i) << endl;
  }
  cout << "Inverse system matrix: \n";
  for (Int_t i=0;i<3;i++) {
    for (Int_t j=0;j<i;j++) cout << "\t";
    for (Int_t j=i;j<3;j++) cout << fM(i,j) << "\t";
    cout << endl;
  }
  #endif

  out=fM*fB;
}

void HGeomVertexFit2::reset(void) {
  // Resets the fitting procedure. That is, the class goes to the
  //state where no line was added
  for (Int_t i=0;i<3;i++) {
    for (Int_t j=0;j<3;j++) fM(i,j)=fSys(i,j)=0.0;
  }
  fB.setXYZ(0.0,0.0,0.0);
}

ClassImp(HGeomVertexFit2)
