#ifndef HMdcTrackGField_H
#define HMdcTrackGField_H

//Read the field
//Input: 3 dimensional Position vector 
//Output:3 dimensional Field vector
//created 16/01/2002/ 
//********************************************
//  nfz      number of steps in z-direction 
//  nfr      number of steps in rho-direction 
//  nfp      number of steps in phi-direction 
//  zflmin   minimum value of z-coordinate
//  zflmax   maximum value of z-coordinate
//  zfldel   step value of z-coordinate
//  rflmin   minimum value of rho-coordinate
//  rflmax   maximum value of rho-coordinate
//  rfldel   step value of rho-coordinate
//  pflmin   minimum value of phi-coordinate
//  pflmax   maximum value of phi-coordinate
//  pfldel   step value of phi-coordinate

#include "TNamed.h"
#include "TString.h"

//size of acos table. 50000 - is enough, as was found out
#define ACOS_TABLE_SIZE 50000

class HMdcTrackGField : public TNamed{
  //private:
public:
  Int_t nfz,nfr,nfp;
  Double_t zflmin,zflmax,zfldel;
  Double_t rflmin,rflmax,rfldel;
  Double_t pflmin,pflmax,pfldel;
  Double_t* p_tzfl;
  Double_t* p_trfl;
  Double_t* p_tpfl;
  Double_t* acos_table;          //!

  Double_t Pvector[3];
  Double_t Fvector[3];
  
  Double_t step1z,step1r,step1p; //!
  Double_t dconv;                //!
  Int_t nfz_nfr;                 //!
  Double_t one_sixtyth;          //!

public:
    HMdcTrackGField(const Char_t *name="",const Char_t *title="");
    virtual ~HMdcTrackGField();
    void init(TString infile);
    void clear();
    void calcField(Double_t* xv,Double_t *btos,Double_t fpol);
    ClassDef(HMdcTrackGField,1)
};

#endif /*!HMdcTrackGField_H*/


