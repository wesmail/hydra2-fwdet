//*-- Author : M. Sanchez
//*-- Modified : 07.03.2001 (M. Sanchez)
using namespace std;
#include  "hgeomvertexfit.h"
#include <iostream> 
#include <iomanip>

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

HGeomVertexFit::HGeomVertexFit(void)  {
  // The default constructor
}

HGeomVertexFit::~HGeomVertexFit(void) {
  // Everything that is constructed has to be destructed
}

void HGeomVertexFit::addLine(const HGeomVector &r, const HGeomVector &alpha,
			  const Double_t w) {
  // Function to add lines to the fit.
  // Input
  //   r --> A point in the straight line
  //   alpha --> normalized direction vector
  //   w --> weight of this line in the fit

  fM(0,0)= w * (alpha.getY() * alpha.getY() + alpha.getZ() * alpha.getZ());
  fM(0,1)=-w * (alpha.getX() * alpha.getY());
  fM(0,2)=-w * (alpha.getX() * alpha.getZ());
  //  fM(1,0)=-(alpha.X() * alpha.Y());
  fM(1,1)= w * (alpha.getX() * alpha.getX() + alpha.getZ() * alpha.getZ());
  fM(1,2)=-w * (alpha.getY() * alpha.getZ());
  //fM(2,0)=-(alpha.X() * alpha.Z());
  //fM(2,1)=-(alpha.Y() * alpha.Z());
  fM(2,2)= w * (alpha.getY() * alpha.getY() + alpha.getX() * alpha.getX());

  fSys(0,0)+=fM(0,0);
  fSys(0,1)+=fM(0,1);
  fSys(0,2)+=fM(0,2);
  fSys(1,1)+=fM(1,1);
  fSys(1,2)+=fM(1,2);
  fSys(2,2)+=fM(2,2);

  fB.X()+=fM(0,0) * r.getX() + fM(0,1) * r.getY() + fM(0,2) * r.getZ();
  fB.Y()+=fM(0,1) * r.getX() + fM(1,1) * r.getY() + fM(1,2) * r.getZ();
  fB.Z()+=fM(0,2) * r.getX() + fM(1,2) * r.getY() + fM(2,2) * r.getZ();
}

void HGeomVertexFit::getVertex(HGeomVector &out) {
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

void HGeomVertexFit::reset(void) {
  // Resets the fitting procedure. That is, the class goes to the
  //state where no line was added
  for (Int_t i=0;i<3;i++) {
    for (Int_t j=0;j<3;j++) fM(i,j)=fSys(i,j)=0.0;
  }
  fB.setXYZ(0.0,0.0,0.0);
}

ClassImp(HGeomVertexFit)



