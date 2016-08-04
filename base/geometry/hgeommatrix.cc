//*-- Author : M. Sanchez
//*-- Modified : 07.03.2001

#include "hgeommatrix.h"

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////
// HGeomMatrix
//
//   Simple 3D matrix interface for use with HGeomVector.
//   
//   Note:
//     This class is completely incomplete. Features will be
//     added as needed
/////////////////////////////////////////////////////////


HGeomMatrix::HGeomMatrix(void) {
  // Initializes the matrix to 0
  for (Int_t i=0;i<9;i++) fM[i]=0.0;
}

HGeomMatrix::~HGeomMatrix(void) {
}

Double_t HGeomMatrix::det(void) {
  // Computes de determinat of the 3D matrix
  return (fM[0] * fM[4] * fM[8] + fM[1] * fM[5] * fM[6] + fM[3] *fM[7] * fM[2] -
	  fM[2] * fM[4] * fM[6] - fM[1] * fM[3] * fM[8] - fM[5] *fM[7] * fM[0]);
}

HGeomVector HGeomMatrix::operator*(HGeomVector &v) {
  // Matrix multiplication
  HGeomVector vo;
  vo.setX(fM[0] * v.getX() + fM[1] * v.getY() + fM[2] * v.getZ());
  vo.setY(fM[3] * v.getX() + fM[4] * v.getY() + fM[5] * v.getZ());
  vo.setZ(fM[6] * v.getX() + fM[7] * v.getY() + fM[8] * v.getZ());
  return vo;
}

HGeomMatrix &HGeomMatrix::operator/=(Double_t d) {
  // Matrix division by a constant. Divides each element on the
  //matrix by the constant "d"
  for (Int_t i=0;i<9;i++) fM[i]/=d;
  return *this;
}

ClassImp(HGeomMatrix)
