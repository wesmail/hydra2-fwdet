//*--Author A.Rustamov
//
//Input position vector,output field vector
//In the Sector coordinate system
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

using namespace std;
#include <iostream>
#include <iomanip>
#include <fstream>
#include "hmdctrackgfield.h"
#include "TTimer.h"
#include "TBuffer.h"
#include <math.h>
#include <unistd.h>

ClassImp(HMdcTrackGField)

HMdcTrackGField::HMdcTrackGField(const Char_t* name, const Char_t* title) :
TNamed(name,title)
{
    dconv=0.0001;

    p_tzfl=new Double_t[708400]; //161*176*25
    p_tpfl=new Double_t[708400];
    p_trfl=new Double_t[708400];
    nfz=161;
    nfr=176;
    nfp=25;
    nfz_nfr=nfz*nfr;

    one_sixtyth=1.0/60.0;
    
//    printf("building acos table of size %d points...",ACOS_TABLE_SIZE);
    acos_table=new Double_t[ACOS_TABLE_SIZE*2+1];
    Double_t temp;
    Double_t t1,t2;
    t2=Double_t(ACOS_TABLE_SIZE);
    Double_t radd=57.29577951;
//fill negative arguments:
    for(Int_t i=0;i<ACOS_TABLE_SIZE;i++)
    {
	t1=Double_t(i-ACOS_TABLE_SIZE);
	temp=t1/t2;
	acos_table[i]=acos(temp)*radd;
    }

//fill zero argument:
    acos_table[ACOS_TABLE_SIZE]=acos(0.0)*radd;

//fill positive arguments:
    for(Int_t i=ACOS_TABLE_SIZE+1;i<(ACOS_TABLE_SIZE*2+1);i++)
    {
	t1=Double_t(i-ACOS_TABLE_SIZE);
	temp=t1/t2;
	acos_table[i]=acos(temp)*radd;
    }
//    printf("Ok\n");

}

HMdcTrackGField::~HMdcTrackGField()
{
    delete [] p_tzfl;
    delete [] p_trfl;
    delete [] p_tpfl;
    delete [] acos_table;
}
void HMdcTrackGField::clear()
{
}

void HMdcTrackGField::init(TString infile)
{
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
		for (Int_t i3 = 0;i3 < nfz; i3 ++){input>>p_tzfl[i3+i2*nfz+i1*nfz*nfr];}
		for (Int_t i3 = 0;i3 < nfz; i3 ++){input>>p_trfl[i3+i2*nfz+i1*nfz*nfr];}
		for (Int_t i3 = 0;i3 < nfz; i3 ++){input>>p_tpfl[i3+i2*nfz+i1*nfz*nfr];}
	    }
	}
    }
    cout<<"READING FIELDMAP FINISHED!"<<endl;
    input.close();
    step1z=1.0/zfldel;
    step1r=1.0/rfldel;
    step1p=1.0/pfldel;
}	  
	  	  
void HMdcTrackGField::calcField(Double_t *xv,Double_t *btos,Double_t fpol )
{	
  Double_t xloc,yloc,zloc,rhog,phigd,phil,bz,br,bp;
  Double_t m1,m2,m3;
  Double_t dc,ds,a,b,c,ac,ab,bc,abc,w1,w2,w3,w4,w5,w6,w7,w8;
  Double_t spol;
  Double_t mult;
  Int_t ifz,ifr,ifp,ifr1,ifp1;
  Int_t i1,i2,i3,i4,i5,i6,i7,i8;
    
  zloc=xv[2]; 

  if(zloc<zflmin||zloc>zflmax) {
  btos[0]=0.; btos[1]=0.;btos[2]=0.;
//  printf("WARN!! z returned,xv=%f %f %f, zloc=%f \n",xv[0],xv[1],xv[2]);
  return;
  }
    xloc=xv[0];
    yloc=xv[1];    
    rhog=sqrt(xloc*xloc+yloc*yloc);
    if(rhog>rflmax){
	btos[0]=0.;
	btos[1]=0.;
	btos[2]=0.;
//	printf("WARN!!! r returned xv=%f %f %f rhog=%f\n",xv[0],xv[1],xv[2]);
	return;
      }
  if(rhog>0.01){
    dc=xloc/rhog;
    ds=yloc/rhog;
    phigd=acos_table[Int_t(dc*ACOS_TABLE_SIZE+ACOS_TABLE_SIZE)];

    }else{
      rhog=0.;
      phigd=0.;
      dc=0.;
      ds=0.;
//      printf("WARN!!!! rhog<=eps!!!\n");
    }
    
    phil=phigd-(Int_t)(phigd*one_sixtyth)*60.0;
//phil=phigd-60.0;    

      if(phil>30.0){
	spol=-1.0;
	phil=60.0-phil;
      }else {
        spol=1.0;
	}
    
            
    m1=(zloc-zflmin)*step1z;
    m2=(rhog-rflmin)*step1r;
    m3=(phil-pflmin)*step1p;

    ifz=(Int_t)(m1);
    ifr=(Int_t)(m2);
    ifp=(Int_t)(m3);

    a=m1-ifz;
    b=m2-ifr;
    c=m3-ifp;

    ifz++;
    ifr=(ifr+1)*nfz;
    ifp=(ifp+1)*nfz_nfr;
    ifr1=ifr-nfz;
    ifp1=ifp-nfz_nfr;

      ab=a*b;
      ac=a*c;
      bc=b*c;
      abc=a*bc;
      w1=1.0-a-b+ab-c+ac+bc-abc;
      w2=b-ab-bc+abc;
      w3=bc-abc;
      w4=c-ac-bc+abc;
      w5=a-ab-ac+abc;
      w6=ab-abc;
      w7=abc;
      w8=ac-abc;
      
    i1=ifz-1+ifr1+ifp1;
    i2=ifz-1+ifr+ifp1;
    i3=ifz-1+ifr+ifp;
    i4=ifz-1+ifr1+ifp;
    i5=ifz+ifr1+ifp1;
    i6=ifz+ifr+ifp1;
    i7=ifz+ifr+ifp;
    i8=ifz+ifr1+ifp;

       bz=w1*p_tzfl[i1]+w2*p_tzfl[i2]
      +w3*p_tzfl[i3]+w4*p_tzfl[i4]
     +w5*p_tzfl[i5]+w6*p_tzfl[i6]
    +w7*p_tzfl[i7]+w8*p_tzfl[i8];
							 
       br=w1*p_trfl[i1]+w2*p_trfl[i2]
      +w3*p_trfl[i3]+w4*p_trfl[i4]
     +w5*p_trfl[i5]+w6*p_trfl[i6]
    +w7*p_trfl[i7]+w8*p_trfl[i8];
							
           bp=w1*p_tpfl[i1]+w2*p_tpfl[i2]
      +w3*p_tpfl[i3]+w4*p_tpfl[i4]
     +w5*p_tpfl[i5]+w6*p_tpfl[i6]
    +w7*p_tpfl[i7]+w8*p_tpfl[i8];



    mult=fpol*dconv;
      bz=-bz*spol*mult;
      br=-br*spol*mult;
      bp=-bp*mult;

      btos[0]=(dc*br-ds*bp);
      btos[1]=(ds*br+dc*bp);
      btos[2]=bz;	  
//      printf("xv=%f %f %f, btos=%f %f %f\n",xv[0],xv[1],xv[2],btos[0],btos[1],btos[2]);
} 
	  	  

void HMdcTrackGField::Streamer(TBuffer &R__b) {
  // Stream an object of class HMdcTrackGField.
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
      
    R__b.ReadStaticArray((Double_t*)Pvector);
    R__b.ReadStaticArray((Double_t*)Fvector);

    R__b.CheckByteCount(R__s, R__c, HMdcTrackGField::IsA());
      
  } else {
    R__c = R__b.WriteVersion(HMdcTrackGField::IsA(), kTRUE);
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

    R__b.WriteArray(Pvector, 3);
    R__b.WriteArray(Fvector, 3);
    R__b.SetByteCount(R__c, kTRUE);
  }
}

