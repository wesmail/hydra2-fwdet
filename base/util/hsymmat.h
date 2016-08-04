/////////////////////////////////////////////////////////////////////////
//
// class for symmetric matrix
// R.Schicker 15-May-00
//
/////////////////////////////////////////////////////////////////////////
#ifndef HSYMMAT_H
#define HSYMMAT_H
using namespace std;
#include "TObject.h"
#include "TArrayF.h"
#include <iostream> 
#include <iomanip>

class HSymMat : public TObject {
protected:
  Float_t *pData;   //! Matrix elements
  Int_t size;
  Int_t dim;

  HSymMat(Int_t dim);
  Int_t getLinear(Int_t i,Int_t j) const {
    return (i<j)?(i*(2*dim-i-1))/2+j:(j*(2*dim-j-1))/2+i;
  }
public:
  HSymMat(void);
  ~HSymMat(void);

  void Clear(void);

  inline Float_t &operator()(Int_t i,Int_t j);
  void setElement(const Int_t,const Float_t);
  void setElement(const Int_t,const Int_t,const Float_t);
  void setErr(const Int_t,const Float_t);
  void setCov(HSymMat&);

  Float_t getElement(const Int_t) const;
  Float_t getElement(const Int_t,const Int_t) const;
  Float_t getErr(const Int_t);
  Int_t getDim(void) const { return dim; }
  void getCov(HSymMat&);
  void print(void);

  Float_t convolution(Float_t v1[],Float_t v2[]);
  void transform(const HSymMat &m,Float_t *der);

  ClassDef(HSymMat,1)  // Symmetric matrix  
};

inline Float_t &HSymMat::operator()(Int_t i,Int_t j) {
  if (i<dim && j<dim) {
    return pData[getLinear(i,j)];
  } else Error("operator()","Out of bounds!");
  return pData[0];
}

///////////////////////////////////////////////
class HSymMat2 : public HSymMat {
protected:
  Float_t data[2*(2+1)/2];
public:
  HSymMat2(void) : HSymMat(2) { pData=data; }
  ~HSymMat2(void) {}
  ClassDef(HSymMat2,1) //Symmetric matrix dimension 2
};

class HSymMat3 : public HSymMat {
protected:
  Float_t data[3*(3+1)/2];
public:
  HSymMat3(void) : HSymMat(3) { pData=data; }
  ~HSymMat3(void) {}
  ClassDef(HSymMat3,1) //Symmetric matrix dimension 3
};

class HSymMat4 : public HSymMat {
protected:
  Float_t data[4*(4+1)/2];
public:
  HSymMat4(void) : HSymMat(4) { pData=data; }
  ~HSymMat4(void) {}
  ClassDef(HSymMat4,1) //Symmetric matrix dimension 4
};

class HSymMat5 : public HSymMat {
protected:
  Float_t data[5*(5+1)/2];
public:
  HSymMat5(void) : HSymMat(5) { pData=data; }
  ~HSymMat5(void) {}
  ClassDef(HSymMat5,1) //Symmetric matrix dimension 5
};

class HSymMat6 : public HSymMat {
protected:
  Float_t data[6*(6+1)/2];
public:
  HSymMat6(void) : HSymMat(6) { pData=data; }
  ~HSymMat6(void) {}
  ClassDef(HSymMat6,1) //Symmetric matrix dimension 6
};

#endif  /* HSYMMAT_H */














