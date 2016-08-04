//*-- Author : R.Schicker
//*-- Modified : M.Sanchez (01.06.2000)

#include "hsymmat.h"
#include <math.h>

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////
// HSymMat 
//
//  Implements a generic symetric matrix
//  In order to allow statically sized matrixes
//the HSymMat class cannot be instantiated directly
//but one of the derived classes HSymMat4, HSymMat5 ...
//can be instantiated.
//
//  Each of these classes corresponds to a 
//symetric matrix of fixed dimension; i.e. HSymMat5 is
//a symetric matrix of dimension 5.
//
//  The HSymMatX classes themselves have no useful
//methods, but inherit all their functionality from
//HSymMat.
//
//  Elements in the matrix can be accessed both via
//a row and column number or with a linear index. The
//relation between both of them is:
//linear_index = row * dimension + column
/////////////////////////////////////////////

HSymMat::HSymMat(void) {
  //Constructor. 
  size = 0;
  pData = 0;
}

HSymMat::HSymMat(Int_t idim) {
  //Constructor with specific dimension.
  size = (idim * (idim + 1) / 2);
  pData = 0;
  dim = idim;
}

HSymMat::~HSymMat(void) { 
}

Float_t HSymMat::convolution(Float_t v1[], Float_t v2[]) {
  Float_t r=0.;
  
  for (Int_t i=0;i<dim;i++) {
    for (Int_t j=0;j<dim;j++) {
      r += v1[i]*getElement(i,j)*v2[j];
    }
  }

  return r;
}

void HSymMat::transform(const HSymMat &m, Float_t der[]) {
  Int_t cols = m.getDim();
  Float_t sum;

  for (Int_t i=0;i<dim;i++) {
    for (Int_t j=i; j<dim; j++) {
      sum = 0.;
      for (Int_t k=0;k<cols;k++) {
	for (Int_t l=0;l<cols;l++) {
	  sum += m.getElement(k,l)*der[i*cols + k]*der[j*cols + l];
	}
      }
      setElement(i,j,sum);
    }
  }
}


void HSymMat::Clear(void){
  //Clears the matrix
  for (Int_t i=0;i<size;i++) pData[i]=0.F;
}

void HSymMat::setElement(const Int_t i,const Float_t elel){
  //Sets the element with linear index i to the value elel.
  if (i<size)
    pData[i]=elel;
  else
    Error("setElement","Index %i out of bounds",i);
}

void HSymMat::setElement(const Int_t i,const Int_t j,Float_t elel) {
  //Sets element at row i and column j to the value elel
  if (i >= dim || j >= dim) {
    Error("setElement","Out of bounds! ");
  } else {
    pData[getLinear(i,j)]=elel;
  } 
}

void HSymMat::setErr(const Int_t i,const Float_t elel){
  //Sets element (i,i) to the value elel
  if (i >= dim) {
    Error("setErr","Out of bounds! ");
  } else {
    pData[getLinear(i,i)]=elel*elel;
  } 
}

void HSymMat::setCov(HSymMat& a){
  //Copies the matrix a
  if( dim != a.dim){
    Error("setCov","Dimensions don't match! "); 
  } else {
    for(Int_t ind=0; ind<size; ind++) {
      pData[ind]=a.pData[ind];
    }
  }
}

Float_t HSymMat::getElement(const Int_t i)  const {
  //Return the element given by the linear index i
  if (i<size) 
    return pData[i];
  else
    Error("getElement","Index %i out of bounds",i);
  return 0.F;
}

Float_t HSymMat::getElement(const Int_t i,const Int_t j) const {
  //Returns the element at (i,j)
  Int_t ind;

  if (i >= dim || j >= dim) {
    Error("getElement","Out of bounds! ");
  } else {
    ind=getLinear(i,j);
    return pData[ind];
  } 
  return 0;
}

Float_t HSymMat::getErr(const Int_t i){
  //Returns the element at (i,i)

  if (i >= dim) {
    Error("getErr","Out of bounds! ");
  } else {
    return sqrt(pData[getLinear(i,i)]);
  }
  return 0;
}

void HSymMat::getCov(HSymMat& a){
  //Copies this matrix into a
  if( dim != a.dim) {
    Error("getCov","Dimensions don't match! "); 
  } else {
    for(Int_t ind=0; ind<size; ind++){
      a.pData[ind]=pData[ind];
    }
  }
}
 
void HSymMat::print(void){
  //Prints out the matrix elements
  printf("Covariance matrix dimension: %i \n",dim);
  for(Int_t ir=0; ir<dim; ir++) {
    for(Int_t ic=0; ic<dim; ic++) {
      printf("%11.4g ",getElement(ir,ic));
    }
    printf(" \n");
  }
}

ClassImp(HSymMat) // HSymMat 
ClassImp(HSymMat2)
ClassImp(HSymMat3)
ClassImp(HSymMat4)
ClassImp(HSymMat5)
ClassImp(HSymMat6)










