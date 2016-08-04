#ifndef __EDFIELD__
#define __EDFIELD__

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
#include "TEveTrackPropagator.h"

class  HEDField ;

R__EXTERN HEDField *gEDField;

class HEDField : public TNamed , public TEveMagField {
private:
    Int_t nfz,nfr,nfp;
    Double_t zflmin,zflmax,zfldel;
    Double_t rflmin,rflmax,rfldel;
    Double_t pflmin,pflmax,pfldel;
    Double_t fpol;
    Double_t* p_tzfl;
    Double_t* p_trfl;
    Double_t* p_tpfl;

    Double_t step1z,step1r,step1p; //!
    Double_t dconv;                //!
    Int_t nfz_nfr;                 //!
    Double_t one_sixtyth;          //!
public:
    HEDField(const Char_t *name="HADES_FIELD_MAP",const Char_t *title="HADES_FIELD_MAP",Double_t fPolarity = 1);
    virtual ~HEDField();
    // hades --------------------------------
    void       SetPolarity(Double_t fPolarity) { fpol = -fPolarity; }
    void       Clear();
    void       CalcField(Double_t* xv,Double_t *btos) const;
    void       ReadAscii (TString infile);
    void       WriteAscii(TString outfile);

    // eve   --------------------------------
    virtual TEveVector GetField(Float_t x, Float_t y, Float_t z) const;
    virtual TEveVector GetField(const TEveVector& v) const;
    virtual Float_t GetMaxFieldMag() const {return 4.;}
    ClassDef(HEDField,1)
};
#endif /*!HEDField_H*/



