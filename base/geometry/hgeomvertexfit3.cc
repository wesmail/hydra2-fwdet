//*-- Author : M. Sanchez
//*-- Modified : 07.03.2001 (M. Sanchez)

#include  "hgeomvertexfit3.h"
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

HGeomVertexFit3::HGeomVertexFit3(void)  {
  // The default constructor
}

HGeomVertexFit3::~HGeomVertexFit3(void) {
  // Everything that is constructed has to be destructed
}

void HGeomVertexFit3::addLine(const HGeomVector &r, const HGeomVector &alpha,
			  const Double_t w) {
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

  fM(0,0) = w * 1 ;
  fM(0,1) = w * 0;
  fM(0,2) = w * -sx;
  fM(1,1) = w * 1;
  fM(1,2) = w * -sy;
  fM(2,2) = w * sx*sx + w * sy*sy;

  fSys(0,0)+=fM(0,0);
  fSys(0,1)+=fM(0,1);
  fSys(0,2)+=fM(0,2);
  fSys(1,1)+=fM(1,1);
  fSys(1,2)+=fM(1,2);
  fSys(2,2)+=fM(2,2);

  fB.X() += w * x0;
  fB.Y() += w * y0;
  fB.Z() += - w * x0*sx - w * y0*sy;
}

void HGeomVertexFit3::getVertex(HGeomVector &out) {
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

void HGeomVertexFit3::reset(void) {
  // Resets the fitting procedure. That is, the class goes to the
  //state where no line was added
  for (Int_t i=0;i<3;i++) {
    for (Int_t j=0;j<3;j++) fM(i,j)=fSys(i,j)=0.0;
  }
  fB.setXYZ(0.0,0.0,0.0);
}

ClassImp(HGeomVertexFit3)
