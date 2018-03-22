using namespace std;
#include<iostream>
#include<fstream>
#include"hmdctrackgcorrections.h"

#include "TBuffer.h"
#include<math.h>
ClassImp(HMdcTrackGCorrections)
   
HMdcTrackGCorrections::HMdcTrackGCorrections(const Char_t* name, const Char_t* title):
      TNamed(name,title)
{
   for(Int_t i=0; i<34; i++)
      tetaI[i]=18+2*i;
   for(Int_t i=0; i<16; i++)
      phiI[i]=60+2*i;
}

HMdcTrackGCorrections::HMdcTrackGCorrections(HMdcTrackGCorrections &corr):TNamed(corr)
{
   for(Int_t i=0; i<16; i++)
      {
	phiI[i]=corr.getPhiI(i);
      }
   for(Int_t i=0; i<34; i++)
      {
	 tetaI[i]=corr.getThetaI(i);
      }
   
   for(Int_t i=0; i<60; i++)
      {
	 precI[i]=corr.getPrecI(i);
      }
   
   for(Int_t i=0; i<34; i++)
      for(Int_t j=0; j<16; j++)
	 for(Int_t k=0; k<60; k++)
	    {
	       corrE[i][j][k]=corr.getCorrE(i,j,k);
	       corrP[i][j][k]=corr.getCorrP(i,j,k);
	       corrE1[i][j][k]=corr.getCorrE1(i,j,k);
	       corrP1[i][j][k]=corr.getCorrP1(i,j,k);
	    }
}


HMdcTrackGCorrections::~HMdcTrackGCorrections()
{
   ;
}
void HMdcTrackGCorrections::clear()
{
}
void HMdcTrackGCorrections::init(TString infile1, TString infile2,TString infile3,TString infile4)
{
   //infile1 electrons 4 chamber mode
   //infile2 positrons 4 chamber mode
   //infile3 electrons 3 chamber mode
   //infile4 positrons 3 chamber mode
   Double_t pp1,pp2,pp3;
   ifstream input1, input2,input3,input4;
   
   input1.open(infile1.Data(),ios::in);
   input2.open(infile2.Data(),ios::in);
   input3.open(infile3.Data(),ios::in);
   input4.open(infile4.Data(),ios::in);
   
   for(Int_t i=0; i<60; i++)
      {
	 input1>>precI[i];
	 input2>>pp1;  
	 input3>>pp2;
	 input4>>pp3;
      }
   for (Int_t k=0; k<34; k++)
      for (Int_t i=0; i<16; i++)
	 for(Int_t j=0; j<60; j++)
	    {
	       input1>>corrE[k][i][j];
	       input2>>corrP[k][i][j];
	       input3>>corrE1[k][i][j];
	       input4>>corrP1[k][i][j];
      }  
   input1.close();
   input2.close();
   input3.close();
   input4.close();   
}

void HMdcTrackGCorrections::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcTrackGCorrections.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TNamed::Streamer(R__b);
      R__b.ReadStaticArray(phiI);
      R__b.ReadStaticArray(tetaI);
      R__b.ReadStaticArray(precI);
      R__b.ReadStaticArray((Double_t*)corrE);
      R__b.ReadStaticArray((Double_t*)corrP);
     
      if(R__v==2)
      {
      R__b.ReadStaticArray((Double_t*)corrE1);
      R__b.ReadStaticArray((Double_t*)corrP1);
      }
     
     
      else 
      if(R__v<=1)
      {
         for (Int_t k=0; k<34; k++)
	  for (Int_t i=0; i<16; i++)
	   for(Int_t j=0; j<60; j++)
	    {
	     corrE1[k][i][j]=corrE[k][i][j];
	     corrP1[k][i][j]=corrP[k][i][j];
	    }
	}
	
	
	
	
       R__b.CheckByteCount(R__s, R__c, HMdcTrackGCorrections::IsA());
       }
   else {
      R__c = R__b.WriteVersion(HMdcTrackGCorrections::IsA(), kTRUE);
      TNamed::Streamer(R__b);
      R__b.WriteArray(phiI, 16);
      R__b.WriteArray(tetaI, 34);
      R__b.WriteArray(precI, 60);
      R__b.WriteArray((Double_t*)corrE, 32640);
      R__b.WriteArray((Double_t*)corrP, 32640);
      R__b.WriteArray((Double_t*)corrE1, 32640);
      R__b.WriteArray((Double_t*)corrP1, 32640);
      
      R__b.SetByteCount(R__c, kTRUE);
	 }
   }





