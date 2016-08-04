//*--AUTHOR A.RUSTAMOV
using namespace std;
#include "hmdctrackgcorrpar.h"
#include "hmdctrackgfieldpar.h"
#include "hmdctrackgcorrections.h"
#include "hgeomvector.h"
#include "hmdctrackgspline.h"
#include "hmdctrackgfield.h"
#include "hparamlist.h"
#include <iostream>
#include "stdlib.h"

ClassImp(HMdcTrackGCorrPar);


HMdcTrackGCorrPar::HMdcTrackGCorrPar(const Char_t* name, const Char_t* title, const Char_t* context)
    :HParCond(name,title,context)
{
    fieldmap = new HMdcTrackGFieldPar("FieldmapParameters","FieldmapParameters");
    corr  = new HMdcTrackGCorrections("Corrections","Corrections");
    spline= new HMdcTrackGSpline("SplineMomentum","SplineMomentum");
    corr1=new HMdcTrackGCorrections("Corrections1","Corrections1");
    corrScan0=new HMdcTrackGCorrections("scan0","scan0");
    corrScan1=new HMdcTrackGCorrections("scan1","scan1");
    corrScan2=new HMdcTrackGCorrections("scan2","scan2");
    
}

HMdcTrackGCorrPar::~HMdcTrackGCorrPar()
{
    if(fieldmap)  delete fieldmap;
    if(corr)   delete corr;
    if(corr1) delete corr1;
    if(corrScan0) delete corrScan0;
    if(corrScan1) delete corrScan1;
    if(corrScan2) delete corrScan2;
    if(spline) delete spline;
}

void HMdcTrackGCorrPar::putParams(HParamList* l)
{
    // Puts all params of HMdcDigitPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("doCorrections",doCorrections);
    l->addObject("corr",         corr);
    l->addObject("corr1",        corr1);
    l->addObject("corrScan0",    corrScan0);
    l->addObject("corrScan1",    corrScan1);
    l->addObject("corrScan2",    corrScan2);
}
Bool_t HMdcTrackGCorrPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("doCorrections",&doCorrections))) return kFALSE;
    if(!( l->fillObject("corr",corr))) return kFALSE;
    if(!( l->fillObject("corr1",corr1))) return kFALSE;
    if(!( l->fillObject("corrScan0",corrScan0))){cout<<"no coorrScan0, but it is Ok"<<endl; }//return kFALSE;
    if(!( l->fillObject("corrScan1",corrScan1))){cout<<"no coorrScan1, but it is Ok"<<endl; }// return kFALSE;
    if(!( l->fillObject("corrScan2",corrScan2))){cout<<"no coorrScan2, but it is Ok"<<endl; }// return kFALSE;				
    return kTRUE;
}
void HMdcTrackGCorrPar::clear()
{
///    field->clear();
    corr ->clear();
}
//void HMdcTrackGCorrPar::initFieldMap(TString input)
//{
//  field->init(input);
//}

void HMdcTrackGCorrPar:: initCorrectionsScan(TString *inputE,TString *inputP,TString *inputE3,TString *inputP3)
{
    corrScan0->init(inputE[0],inputP[0],inputE3[0],inputP3[0]);
    corrScan1->init(inputE[1],inputP[1],inputE3[1],inputP3[1]);
    corrScan2->init(inputE[2],inputP[2],inputE3[2],inputP3[2]);
    
}
void HMdcTrackGCorrPar::getCorrScan(HMdcTrackGCorrections *corrScanAll[])
{
   corrScanAll[0]=corrScan0;
   corrScanAll[1]=corrScan1;
   corrScanAll[2]=corrScan2;
}

void HMdcTrackGCorrPar:: initCorrections(TString inputE,TString inputP,TString inputE3,TString inputP3)
{
    corr->init(inputE,inputP,inputE3,inputP3);
}



void HMdcTrackGCorrPar:: initCorrections1(TString inputE,TString inputP,TString inputE3,TString inputP3)
{
    corr1->init(inputE,inputP,inputE3,inputP3);
}
    

Double_t HMdcTrackGCorrPar:: calcMomentum(HGeomVector* vector,Bool_t cond,Double_t target)
{
    return spline->calcMomentum(vector,cond,target);
}
Double_t HMdcTrackGCorrPar:: calcMomentum(HMdcSeg *seg[2] ,Bool_t cond,Double_t target)
{
  return spline->calcMomentum(seg,cond,target);
}



Double_t HMdcTrackGCorrPar:: calcMomentum123(HMdcSeg *seg[2] ,Bool_t cond,Double_t target)
{
  return spline->calcMomentum123(seg,cond,target);
}
void HMdcTrackGCorrPar::getDistance(Double_t * dist)
{
   spline->getDistance(dist);
}

Double_t HMdcTrackGCorrPar::getMetaDistance(Double_t x,Double_t y,Double_t z)
{
  return ( spline->getMetaDistance(x,y,z));
}


void HMdcTrackGCorrPar::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcTrackGCorrPar.
   
      UInt_t R__s, R__c;
         if (R__b.IsReading()) {
	    Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
	    HParCond::Streamer(R__b);
	    R__b >> doCorrections;
	    R__b >> corr;
	    
	    if(R__v>2)
	    {
	    R__b >> corr1;
	    R__b >> corrScan0;
	    R__b >> corrScan1;
	    R__b >> corrScan2;
	    
	    }
	     else if(R__v>1)
		 {
		    R__b >> corr1;
		    if(corrScan0) delete corrScan0;
		    corrScan0=new HMdcTrackGCorrections(*corr1);//corr1;
		    if(corrScan1) delete corrScan1;
		    corrScan1=new HMdcTrackGCorrections(*corr1);//corr1;
		    if(corrScan2) delete corrScan2;
		    corrScan2=new HMdcTrackGCorrections(*corr1);//corr1;
		 }
	    else{
	       if(corr1) delete corr1;
	       corr1=new HMdcTrackGCorrections(*corr);//corr;
	       if(corrScan0) delete corrScan0;
	       corrScan0=new HMdcTrackGCorrections(*corr);//corr;
	       if(corrScan1) delete corrScan1;
	       corrScan1=new HMdcTrackGCorrections(*corr);//corr;
	       if(corrScan2) delete corrScan2;
	       corrScan2=new HMdcTrackGCorrections(*corr);//corr;
	       
	    }
	    R__b.CheckByteCount(R__s, R__c, HMdcTrackGCorrPar::IsA());
	 } else {
	    R__c = R__b.WriteVersion(HMdcTrackGCorrPar::IsA(), kTRUE);
	 HParCond::Streamer(R__b);
	 R__b << doCorrections;
	 R__b << corr;
	 R__b << corr1;
	 R__b << corrScan0;
	 R__b << corrScan1;
	 R__b << corrScan2;
	 R__b.SetByteCount(R__c, kTRUE);
	 }
}

