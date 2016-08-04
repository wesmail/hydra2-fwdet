//*--Author A.Rustamov
//
//Input position vector,output field vector
//In LAB coordinate system
//
//               |Y             
//               |
//        ***************
//         *     |     * 
//          *    |    *
//           *   |   *
//            *  |  *
//             * | *
//              ***
//               | 
//   -------------             
//   X
//
// UNITS: TESLA,cm, GeV for track momentum,
// polarity is reverse compared to HYDRA (defined by Eve ..)
// switching is done automatically

#include "hedfield.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hmdctrackgfieldpar.h"

#include "TEveVSDStructs.h"
#include "TSystem.h"
#include "TMath.h"
#include <math.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

ClassImp(HEDField)

HEDField::HEDField(const Char_t *name, const Char_t *title,Double_t fPolarity)
    : TNamed(name,title) , TEveMagField()
{

    //----------------------------------
    // eve
    gEDField=this;
    fFieldConstant = kFALSE;
    //----------------------------------

    fpol  = -fPolarity;
    dconv = 0.0001; // 0.0001 [Tesla]

    p_tzfl  = new Double_t[708400]; //161*176*25
    p_tpfl  = new Double_t[708400];
    p_trfl  = new Double_t[708400];
    nfz     = 161;
    nfr     = 176;
    nfp     = 25;
    nfz_nfr = nfz*nfr;

    one_sixtyth = 1.0 / 60.0;
    
}

HEDField::~HEDField()
{
    delete [] p_tzfl;
    delete [] p_trfl;
    delete [] p_tpfl;
}
void HEDField::Clear()
{
}

void HEDField::ReadAscii(TString infile)
{

    if(gSystem->AccessPathName(infile.Data()) != 0){
	Error("ReadAscii()","Could not find fieldmap %s !",infile.Data());
        exit(1);
    }

    ifstream input;
    input.open(infile.Data(),ios::in);
    while(!input.eof())
    {

	input>>zflmin ;
	input>>zflmax ;
	input>>zfldel;
	input>>rflmin ;
	input>>rflmax ;
	input>>rfldel ;
	input>>pflmin ;
	input>>pflmax ;
	input>>pfldel ;
	for (Int_t i1 = 0; i1 < nfp; i1 ++){
	    for (Int_t i2 = 0; i2 < nfr; i2 ++){
		for (Int_t i3 = 0;i3 < nfz; i3 ++){input>>p_tzfl[i3 + i2 * nfz + i1 * nfz * nfr];}
		for (Int_t i3 = 0;i3 < nfz; i3 ++){input>>p_trfl[i3 + i2 * nfz + i1 * nfz * nfr];}
		for (Int_t i3 = 0;i3 < nfz; i3 ++){input>>p_tpfl[i3 + i2 * nfz + i1 * nfz * nfr];}
	    }
	}
    }
    cout<<"HEDField::READING FIELDMAP FINISHED!"<<endl;
    input.close();
    step1z = 1.0 / zfldel;
    step1r = 1.0 / rfldel;
    step1p = 1.0 / pfldel;
}

void HEDField::WriteAscii(TString outfile)
{
    ofstream output;
    output.open(outfile.Data(),ios::out);

    output<<zflmin<<endl;
    output<<zflmax<<endl;
    output<<zfldel<<endl;
    output<<rflmin<<endl;
    output<<rflmax<<endl;
    output<<rfldel<<endl;
    output<<pflmin<<endl;
    output<<pflmax<<endl;
    output<<pfldel<<endl;
    Int_t ct = 0;
    for (Int_t i1 = 0; i1 < nfp; i1 ++){
	for (Int_t i2 = 0; i2 < nfr; i2 ++){
	    for (Int_t i3 = 0;i3 < nfz; i3 ++){ct++; output<<" "<<setw(13)<<p_tzfl[i3 + i2 * nfz + i1 * nfz * nfr]; if(ct%10 == 0) output<<endl; }
	    for (Int_t i3 = 0;i3 < nfz; i3 ++){ct++; output<<" "<<setw(13)<<p_trfl[i3 + i2 * nfz + i1 * nfz * nfr]; if(ct%10 == 0) output<<endl; }
	    for (Int_t i3 = 0;i3 < nfz; i3 ++){ct++; output<<" "<<setw(13)<<p_tpfl[i3 + i2 * nfz + i1 * nfz * nfr]; if(ct%10 == 0) output<<endl; }
	}
    }
    cout<<"HEDField::WRITING FIELDMAP FINISHED!"<<endl;
    output.close();
}
	  	  
void HEDField::CalcField(Double_t *xv,Double_t *btos) const
{
    Double_t xloc,yloc,zloc,rhog,phigd,phil,bz,br,bp;
    Double_t m1,m2,m3;
    Double_t dc,ds,a,b,c,ac,ab,bc,abc,w1,w2,w3,w4,w5,w6,w7,w8;
    Double_t spol;
    Double_t mult;
    Int_t ifz,ifr,ifp,ifr1,ifp1;
    Int_t i1,i2,i3,i4,i5,i6,i7,i8;
    Double_t phig;
    zloc = xv[2];

    if(zloc < zflmin || zloc > zflmax) {
	btos[0] = 0.; btos[1] = 0.;btos[2] = 0.;
	return;
    }
    xloc = xv[0];
    yloc = xv[1];
    rhog = sqrt(xloc*xloc+yloc*yloc);

    if(rhog > rflmax){
	btos[0] = 0.;
	btos[1] = 0.;
	btos[2] = 0.;
	return;
    }
    if(rhog > 0.01){

	phig = atan2(yloc,xloc);
	if(phig < 0.) phig = phig + TMath::Pi() * 2;
	phigd = phig * TMath::RadToDeg();
	dc = TMath::Cos(phig);
	ds = TMath::Sin(phig);

    } else {
	rhog  = 0.;
	phigd = 0.;
	dc    = 0.;
	ds    = 0.;
    }

    phil=fmod(phigd,60.);

    if(phil > 30.0){
	spol = -1.0;
	phil = 60.0 - phil;
    } else {
	spol = 1.0;
    }


    m1 = (zloc - zflmin) * step1z;
    m2 = (rhog - rflmin) * step1r;
    m3 = (phil - pflmin) * step1p;

    ifz = (Int_t)(m1);
    ifr = (Int_t)(m2);
    ifp = (Int_t)(m3);

    a = m1 - ifz;
    b = m2 - ifr;
    c = m3 - ifp;

    ifz ++;
    ifr  = (ifr + 1) * nfz;
    ifp  = (ifp + 1) * nfz_nfr;
    ifr1 = ifr - nfz;
    ifp1 = ifp - nfz_nfr;

    ab  = a * b;
    ac  = a * c;
    bc  = b * c;
    abc = a * bc;
    w1  = 1.0 - a - b + ab - c + ac + bc - abc;
    w2  = b - ab - bc + abc;
    w3  = bc - abc;
    w4  = c - ac - bc + abc;
    w5  = a - ab - ac + abc;
    w6  = ab - abc;
    w7  = abc;
    w8  = ac - abc;

    i1 = ifz -1+ ifr1 + ifp1;
    i2 = ifz -1+ ifr  + ifp1;
    i3 = ifz -1+ ifr  + ifp;
    i4 = ifz -1+ ifr1 + ifp;
    i5 = ifz   + ifr1 + ifp1;
    i6 = ifz   + ifr  + ifp1;
    i7 = ifz   + ifr  + ifp;
    i8 = ifz   + ifr1 + ifp;

    bz =  w1 * p_tzfl[i1] + w2 * p_tzfl[i2]
	+ w3 * p_tzfl[i3] + w4 * p_tzfl[i4]
	+ w5 * p_tzfl[i5] + w6 * p_tzfl[i6]
	+ w7 * p_tzfl[i7] + w8 * p_tzfl[i8];

    br =  w1 * p_trfl[i1] + w2 * p_trfl[i2]
	+ w3 * p_trfl[i3] + w4 * p_trfl[i4]
	+ w5 * p_trfl[i5] + w6 * p_trfl[i6]
	+ w7 * p_trfl[i7] + w8 * p_trfl[i8];

    bp =  w1 * p_tpfl[i1] + w2 * p_tpfl[i2]
	+ w3 * p_tpfl[i3] + w4 * p_tpfl[i4]
	+ w5 * p_tpfl[i5] + w6 * p_tpfl[i6]
	+ w7 * p_tpfl[i7] + w8 * p_tpfl[i8];



    mult = fpol * dconv;
    bz = -bz * spol * mult;
    br = -br * spol * mult;
    bp = -bp * mult;

    btos[0] = (dc * br - ds * bp);
    btos[1] = (ds * br + dc * bp);
    btos[2] = bz;

}

TEveVector HEDField::GetField(Float_t x, Float_t y, Float_t z) const
{

    Double_t xv[3] = {x,y,z}; // cm input from GEANT/EVENTDISPLAY
    Double_t b [3];
    CalcField(xv,b);

    TEveVector val((Float_t)b[0],(Float_t)b[1],(Float_t)b[2]);

    return val;

}

TEveVector  HEDField::GetField(const TEveVector& v) const { return HEDField::GetField(v.fX,v.fY,v.fZ);};


void HEDField::Streamer(TBuffer &R__b) {
  // Stream an object of class HEDField.
  // The data in the 1-dimensional arrays are streamed into/from the old
  // 3-dimensional arrays (no change of class version)
  UInt_t R__s, R__c;
  if (R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
    TNamed::Streamer(R__b);
    R__b >> nfz;
    R__b >> nfr;
    R__b >> nfp;
    R__b >> zflmin;
    R__b >> zflmax;
    R__b >> zfldel;
    R__b >> rflmin;
    R__b >> rflmax;
    R__b >> rfldel;
    R__b >> pflmin;
    R__b >> pflmax;
    R__b >> pfldel;
    R__b >> fpol;
      
    Int_t n;
    R__b >> n;
    for (Int_t i3 = 0; i3 < nfz; i3 ++)
    for (Int_t i2 = 0; i2 < nfr; i2 ++)  
    for (Int_t i1 = 0; i1 < nfp; i1 ++) 
      R__b >> p_tzfl[i3+i2*nfz+i1*nfz*nfr];

    R__b >> n;
    for (Int_t i3 = 0; i3 < nfz; i3 ++)
    for (Int_t i2 = 0; i2 < nfr; i2 ++)  
    for (Int_t i1 = 0; i1 < nfp; i1 ++) 
      R__b >> p_trfl[i3+i2*nfz+i1*nfz*nfr];

    R__b >> n;
    for (Int_t i3 = 0; i3 < nfz; i3 ++)
    for (Int_t i2 = 0; i2 < nfr; i2 ++)  
    for (Int_t i1 = 0; i1 < nfp; i1 ++) 
      R__b >> p_tpfl[i3+i2*nfz+i1*nfz*nfr];

    step1z=1.0/zfldel;
    step1r=1.0/rfldel;
    step1p=1.0/pfldel;

    fFieldConstant = kFALSE;

    R__b.CheckByteCount(R__s, R__c, HEDField::IsA());
      
  } else {
    R__c = R__b.WriteVersion(HEDField::IsA(), kTRUE);
    TNamed::Streamer(R__b);
    R__b << nfz;
    R__b << nfr;
    R__b << nfp;
    R__b << zflmin;
    R__b << zflmax;
    R__b << zfldel;
    R__b << rflmin;
    R__b << rflmax;
    R__b << rfldel;
    R__b << pflmin;
    R__b << pflmax;
    R__b << pfldel;
    R__b << fpol;

    Int_t n=nfz*nfr*nfp;
    R__b << n;
    for (Int_t i3 = 0; i3 < nfz; i3 ++)
    for (Int_t i2 = 0; i2 < nfr; i2 ++)  
    for (Int_t i1 = 0; i1 < nfp; i1 ++) 
      R__b << p_tzfl[i3+i2*nfz+i1*nfz*nfr];

    R__b << n;
    for (Int_t i3 = 0; i3 < nfz; i3 ++)
    for (Int_t i2 = 0; i2 < nfr; i2 ++)  
    for (Int_t i1 = 0; i1 < nfp; i1 ++) 
      R__b << p_trfl[i3+i2*nfz+i1*nfz*nfr];

    R__b << n;
    for (Int_t i3 = 0; i3 < nfz; i3 ++)
    for (Int_t i2 = 0; i2 < nfr; i2 ++)  
    for (Int_t i1 = 0; i1 < nfp; i1 ++) 
      R__b << p_tpfl[i3+i2*nfz+i1*nfz*nfr];

    R__b.SetByteCount(R__c, kTRUE);
  }
}

HEDField *gEDField;

