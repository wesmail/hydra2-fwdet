//*-- AUTHOR : A. Rustamov

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HField
// Reads Field map from TOSCA file.Calculates Field strength in a given
// space point in Lab coordinate system.
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <iostream>
#include <iomanip>
#include <fstream>
#include "hfield.h"
#include <math.h>
#include <cstdlib>

ClassImp(HField)

HField::HField(const Char_t* name,const Char_t* title)
    : TNamed(name,title)
{
    // constructor for HField
}
HField::~HField()
{
  // destructor of HField
}
void HField::initVariables()
{
    nfz=161;
    nfr=176;
    nfp=25;
    for (Int_t i1 = 0; i1 < nfp; i1 ++){
	for (Int_t i2 = 0; i2 < nfr; i2 ++){
	    for (Int_t i3 = 0;i3 < nfz; i3 ++){
		tzfl[i3][i2][i1]=0.;
		trfl[i3][i2][i1]=0.;
		tpfl[i3][i2][i1]=0.;
	    }
	}
    }
}
void HField::init(TString infile)
{
    // infile : tosca field map (for GEANT)

    Double_t a[9];
    ifstream input;

    if(infile.CompareTo("")!=0)
    {
	input.open(infile.Data(),ios::in);
	cout<<"READING FIELDMAP FROM: "<<infile.Data()<<endl;
    }
    else
    {
	Error("HField::init()","NO INPUTFILE DEFINED!");
	exit(1);
    }
    initVariables();
    while(!input.eof())
    {
	for(Int_t i = 0; i < 9; i ++) input>>a[i];
	zflmin = a[0];
	zflmax = a[1];
	zfldel = a[2];
	rflmin = a[3];
	rflmax = a[4];
	rfldel = a[5];
	pflmin = a[6];
	pflmax = a[7];
	pfldel = a[8];
	for (Int_t i1 = 0; i1 < nfp; i1 ++){
	    for (Int_t i2 = 0; i2 < nfr; i2 ++){
		for (Int_t i3 = 0;i3 < nfz; i3 ++){input>>tzfl[i3][i2][i1];}
		for (Int_t i3 = 0;i3 < nfz; i3 ++){input>>trfl[i3][i2][i1];}
		for (Int_t i3 = 0;i3 < nfz; i3 ++){input>>tpfl[i3][i2][i1];}
	    }
	}
    }
    cout<<"READING FIELDMAP FINISHED!"<<endl;
    input.close();
}
void HField::calcField(Double_t *xv,Double_t *btos,Int_t fpol )
{	
    // xv[3]  : position vector
    // btos[3]: Field vector
    // fpol   : Field polarity (1 default , -1 inverse)

    Double_t xloc,yloc,zloc,rhog,rho2g,phig,phigd,phil,bz,br,bp;
    Double_t delz,delr,delp,delz1,delr1,delp1,delz2,delr2,delp2;
    Double_t dc,ds,wz,wr,wp,wzi,wri,wpi,w1,w2,w3,w4,w5,w6,w7,w8;
    Double_t dsec,dhsec,eps,radd,twopi,spol,dconv;
    Int_t ifz,ifr,ifp;

    zloc=xv[2];

    if(zloc<zflmin||zloc>zflmax)
    {   // outside range of z
	btos[0]=0.;
	btos[1]=0.;
	btos[2]=0.;
    }
    else
    {   // inside range of z
	xloc=xv[0];
	yloc=xv[1];

	rho2g=xloc*xloc+yloc*yloc;
	rhog=sqrt(rho2g);

	if(rhog>rflmax)
	{   // outside range of r
	    btos[0]=0.;
	    btos[1]=0.;
	    btos[2]=0.;
	}
	else
	{   // inside range of r
	    eps   =0.01;
	    radd  =57.29577951;
	    twopi =6.283185308;
	    dsec  =60.;
	    dhsec =.5*dsec;
	    dconv =0.001;

	    if(rhog>eps)
	    {
		phig=atan2(yloc,xloc);
		if(phig<0.)phig+=twopi;
		phigd=radd*phig;
		dc=cos(phig);
		ds=sin(phig);
	    }
	    else
	    {
		rhog=0.;
		phig=0.;
		phigd=0.;
		dc=0.;
		ds=0.;
	    }

	    phil=phigd-(Int_t)(phigd/dsec)*dsec;

	    if(phil<=dhsec)spol=1.;
	    else
	    {
		spol=-1.;
		phil=dsec-phil;
	    }

	    delz=zloc-zflmin;
	    delr=rhog-rflmin;
	    delp=phil-pflmin;

	    delz2=delz-(Int_t)(delz/zfldel)*zfldel;
	    delr2=delr-(Int_t)(delr/rfldel)*rfldel;
	    delp2=delp-(Int_t)(delp/pfldel)*pfldel;
      
	    delz1=delz-delz2;
	    delr1=delr-delr2;
	    delp1=delp-delp2;

	    wzi=delz2/zfldel;
	    wri=delr2/rfldel;
	    wpi=delp2/pfldel;
	    wz=1.-wzi;
	    wr=1.-wri;
	    wp=1.-wpi;

	    ifz=(Int_t)((delz1+eps)/zfldel)+1;
	    ifr=(Int_t)((delr1+eps)/rfldel)+1;
	    ifp=(Int_t)((delp1+eps)/pfldel)+1;

	    w1=wz*wr*wp;
	    w2=wz*wri*wp;
	    w3=wz*wri*wpi;
	    w4=wz*wr*wpi;
	    w5=wzi*wr*wp;
	    w6=wzi*wri*wp;
	    w7=wzi*wri*wpi;
	    w8=wzi*wr*wpi;

	    bz=w1*tzfl[ifz-1][ifr-1][ifp-1]+w2*tzfl[ifz-1][ifr][ifp-1]
		+w3*tzfl[ifz-1][ifr][ifp]+w4*tzfl[ifz-1][ifr-1][ifp]
		+w5*tzfl[ifz][ifr-1][ifp-1]+w6*tzfl[ifz][ifr][ifp-1]
		+w7*tzfl[ifz][ifr][ifp]+w8*tzfl[ifz][ifr-1][ifp];

	    br=w1*trfl[ifz-1][ifr-1][ifp-1]+w2*trfl[ifz-1][ifr][ifp-1]
		+w3*trfl[ifz-1][ifr][ifp]+w4*trfl[ifz-1][ifr-1][ifp]
		+w5*trfl[ifz][ifr-1][ifp-1]+w6*trfl[ifz][ifr][ifp-1]
		+w7*trfl[ifz][ifr][ifp]+w8*trfl[ifz][ifr-1][ifp];

	    bp=w1*tpfl[ifz-1][ifr-1][ifp-1]+w2*tpfl[ifz-1][ifr][ifp-1]
		+w3*tpfl[ifz-1][ifr][ifp]+w4*tpfl[ifz-1][ifr-1][ifp]
		+w5*tpfl[ifz][ifr-1][ifp-1]+w6*tpfl[ifz][ifr][ifp-1]
		+w7*tpfl[ifz][ifr][ifp]+w8*tpfl[ifz][ifr-1][ifp];

	    bz=-bz*fpol*spol*dconv;
	    br=-br*fpol*spol*dconv;
	    bp=-bp*fpol*dconv;

	    btos[0]=dc*br-ds*bp;
	    btos[1]=ds*br+dc*bp;
	    btos[2]=bz;
	}
    }
}
