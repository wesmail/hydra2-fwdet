//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcSlope
//
// Calculates the Slope parameter for MDC calibration. Uses cal1 container
// as an input data
//
///////////////////////////////////////////////////////////////////////////////

// File: $RCSfile: hmdcslope.cc,v $
//
// *-- AUTHOR           : P.Zumbruch, J.Kempter, T.Bretz
// *-- Modified         : 26-06-2000 P.Zumbruch
// *-- Last Modification: 08-12-2004 P.Zumbruch
//
// Version: $Revision: 1.22 $
// Modified by $Author: halo $ on $Date: 2009-07-15 11:42:10 $
////////////////////////////////////////////////////////////

using namespace std;

#include <stdlib.h>
#include <fstream> 
#include <iomanip> 
#include <iostream> 

#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TNtuple.h"
#include "TGraphErrors.h"
#include "TStopwatch.h"

#include "heventheader.h"
#include "hmdcslope.h"
#include "hmdcdef.h"
#include "hmdcraw.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hevent.h"
#include "hcategory.h"
#include "hlocation.h"
#include "hmdccal1.h"
#include "hmdclookupgeom.h"
#include "hmdccalparraw.h"
#include "hmdcslopes.h"
#define HMDC_SLOPE_NALGORITHM 3
#define HMDC_SLOPE_NFITHISTOGRAMMMETHODS 2

ClassImp(HMdcSlope)

HMdcSlope::HMdcSlope()
{
    rawCat   = NULL;
    slopeCat = NULL;
    iter     = NULL;
    file     = NULL;

    avgSlope               = (MyFloatField*)new MyFloatField;
    avgSlopeErr            = (MyFloatField*)new MyFloatField;
    nEvt                   = (MyIntField*)  new MyIntField;

    linRegActive           = kTRUE;
    histFitActive          = kFALSE;
    graphFitActive         = kFALSE;

    selectorActive         = kFALSE;

    createNtupleActive     = kFALSE;

    debugActive            = 0;
    trendCalculationActive = kFALSE;
    calibrationAlgorithm   = 1;
    nTimeValues            = 6;
}

HMdcSlope::HMdcSlope(const Text_t* name,const Text_t* title) : HReconstructor(name,title)
{
    slopeCat = NULL;
    rawCat   = NULL;
    iter     = NULL;
    file     = NULL;

    avgSlope               = (MyFloatField*)new MyFloatField;
    avgSlopeErr            = (MyFloatField*)new MyFloatField;
    nEvt                   = (MyIntField*)  new MyIntField;

    linRegActive           = kTRUE;
    histFitActive          = kFALSE;
    graphFitActive         = kFALSE;

    selectorActive         = kFALSE;

    createNtupleActive     = kFALSE;

    debugActive            = 0;
    trendCalculationActive = kFALSE;
    calibrationAlgorithm   = 1;
    nTimeValues            = 6;
}

HMdcSlope::~HMdcSlope()
{
    // destructor deletes the iterator
    if (iter)       delete iter;
    if (fNameRoot)  delete fNameRoot;
    if (avgSlope)   delete[] avgSlope;
    if (avgSlopeErr)delete[] avgSlopeErr;
    if (nEvt)       delete[] nEvt;
    iter = NULL;
}

void HMdcSlope::setOutputRoot(Char_t *c)
{
  if (fNameRoot) delete fNameRoot;
  fNameRoot = new Char_t[strlen(c)+1];
  strcpy(fNameRoot, c);
  return;
}

void HMdcSlope::setMbSelector(Int_t i)
{ 
  mbSelector = 0;
  if (i >= 0 && i <=16)
    {
      mbSelector = i;
    }
  return;
}

void HMdcSlope::selectCalibrationAlgorithm(Int_t i)
{
  calibrationAlgorithm=0;
  if (i >= 0 && i <= HMDC_SLOPE_NALGORITHM) 
      {
	calibrationAlgorithm=i;
      }
  return;
}

void HMdcSlope::selectHistFitMethod(Int_t i)
{
  calibrationAlgorithm=0;
  if (i >= 0 && i <= HMDC_SLOPE_NFITHISTOGRAMMMETHODS) 
      {
	fitHistMethod=i;
      }
  return;
}

void HMdcSlope::setnTimeValues(Int_t i)
{
  nTimeValues=6;
  if (i >= 2 && i <= 6) 
      {
	nTimeValues=i;
      }
}

void HMdcSlope::setTrendCalculationActive(Bool_t b)
{
  trendCalculationActive = b;
  return;
}

void HMdcSlope::setTdcSelector(Int_t i)
{
  tdcSelector = 0;
  if (i >= 0 && i <= 95)
    {
      tdcSelector = i;
    }
  return;
}

void HMdcSlope::setBinError(Int_t i)
{
  binError=0;
  if (i >= 0)
    {
      binError = i;
    }
  return;
}

void HMdcSlope::setSelectorActive(Bool_t b)
{
  selectorActive = b;
}

void HMdcSlope::setCreateNtupleActive(Bool_t b)
{
  createNtupleActive = b;
  if (createNtupleActive) cout << "setting NtupleActive" << endl;
}

void HMdcSlope::setOutputNtuple(Char_t *c)
{
  if (fNameNtuple) delete fNameNtuple;
  fNameNtuple = new Char_t[strlen(c)+1];
  strcpy(fNameNtuple, c);
  return;
}

void HMdcSlope::setLinRegActive(Bool_t b)
{
  linRegActive = b;
}

void HMdcSlope::setHistFitActive(Bool_t b)
{
  histFitActive = b;
}

void HMdcSlope::setGraphFitActive(Bool_t b)
{
  graphFitActive = b;
}

void HMdcSlope::setDebugActive(Int_t i)
{
  debugActive = 0;
  if (i >=0)
    {
      debugActive = i;
    }
  return;
}

void HMdcSlope::fitHistogramm(Float_t* slope, Float_t* sigma,
			      Int_t sector, Int_t module,
			      Int_t motherboard, Int_t tdc, Int_t method)
{
  switch(method)
    {
    case 1: 
      fitHistogrammMaxCondition (slope, sigma, sector, module, motherboard, tdc);
      break;
    case 2:
      fitHistogrammMeanCondition(slope, sigma, sector, module, motherboard, tdc);
      break;
    default:
      cout << "wrong histogramm fitting method" << endl;
      return;
      break;
    }
 // check for finite values
 if (!TMath::Finite(*slope))
 {
  *slope = 0.;
 }
 if (!TMath::Finite(*sigma))
 {
  *sigma = 100.;
 }

  return;
}

void HMdcSlope::fitHistogrammMaxCondition(Float_t* slope, Float_t* sigma,
					  Int_t sector, Int_t module, Int_t motherboard, Int_t tdc)
{
  static Char_t tmp[80];
  sprintf(tmp,"fithist_sec%i_mod%i_mb%i_tdc%i",sector,module,motherboard,tdc);

  if (fNameRoot) file->cd();

  TH1F *proj = new TH1F(tmp,tmp,5000,0.3,0.8);

  for (Int_t myiter=0;myiter<5000;myiter++)
    { 
      proj->SetBinContent(myiter,slopeTrend[sector][module][motherboard][tdc][myiter]);
    }

  static Float_t xminfitbin, xmaxfitbin;
  xminfitbin=(proj->GetMaximum())-0.001;
  xmaxfitbin=(proj->GetMaximum())+0.001;

  TF1 *f = new TF1("f1","gaus", xminfitbin, xmaxfitbin);
  proj->Fit("f1","RNQ");
  
  *slope = (Float_t)f->GetParameter(1);
  *sigma = (Float_t)f->GetParameter(2);
 
  if ((debugActive & 0x20)) cout << endl << "Slope " << *slope << endl;

if (fNameRoot)
  {
    file->cd();  
    proj->SetFillColor(2);
    proj->Write();
    if ((debugActive & 0x20)) cout << "writing file" << endl;
  }

  delete proj;
  delete f;
  return;
}

void HMdcSlope::fitHistogrammMeanCondition(Float_t* slope, Float_t* sigma,
					   Int_t sector, Int_t module, Int_t motherboard, Int_t tdc)
{
  Char_t tmp[80];
  sprintf(tmp,"fithist_sec%i_mod%i_mb%i_tdc%i",sector,module,motherboard,tdc);

  if (fNameRoot) file->cd();

  TH1F *proj = new TH1F(tmp,tmp,5000,0.3,0.8);

  for (Int_t myiter=0;myiter<5000;myiter++)
    { 
      proj->SetBinContent(myiter,slopeTrend[sector][module][motherboard][tdc][myiter]);
    }
  /*
  static Float_t xminfitbin, xmaxfitbin;
  xminfitbin=(proj->GetMean())-10*0.0001;
  xmaxfitbin=(proj->GetMean())+10*0.0001;
  TF1 *f = new TF1("f1","gaus", xminfitbin, xmaxfitbin); 
  proj->Fit("f1","RNQ");
  */
  *slope = (Float_t)proj->GetMean();
  *sigma = (Float_t)proj->GetRMS();
 
  if ((debugActive & 0x20)) cout << endl << "Slope " << *slope << endl;

if (fNameRoot)
  {
    file->cd();  
    proj->SetFillColor(2);
    proj->Write();
    if ((debugActive & 0x20)) cout << "writing file" << endl;
  }

  delete proj;
  //delete f;
  return;
}

Bool_t HMdcSlope::init(void)
{
  // creates the parameter containers MdcCalPar and MdcLookup if they do not
  // exist and adds them to the list of parameter containers in the runtime
  // database
  
  calparraw=(HMdcCalParRaw*)gHades->getRuntimeDb()->getContainer("MdcCalParRaw");
  
  // creates the MdcRaw category and adds them to the current
  // event
  
  rawCat=gHades->getCurrentEvent()->getCategory(catMdcRaw);
  if (!rawCat) 
    {
      rawCat=gHades->getSetup()->getDetector("Mdc")->buildCategory(catMdcRaw);
      if (!rawCat) return kFALSE;
      else gHades->getCurrentEvent()->addCategory(catMdc,rawCat,"Mdc");
    }

  // creates the MdcSlopes category and adds them to the current
  // event

  slopeCat=gHades->getCurrentEvent()->getCategory(catMdcSlope);
  if (!slopeCat) 
    {
      slopeCat=gHades->getSetup()->getDetector("Mdc")->buildCategory(catMdcSlope);
      if (slopeCat)
	{
	  gHades->getCurrentEvent()->addCategory(catMdcSlope,rawCat,"Mdc");
	}
    }

  // creates an iterator which loops over all fired cells

  iter=(HIterator *)rawCat->MakeIterator();

  if (trendCalculationActive)
    {
      fitHistMethod=2;
    }

  if (fNameRoot) 
    {
      file = new TFile(fNameRoot,"RECREATE");
      file->cd();
      if ((debugActive & 0x02)) printf("File %s aktiv \n",fNameRoot);
      hSlope = new TH1F("Slope", "Slope", 800, 0,0.8);
      hTime  = new TH1F("Time" , "Time", 4096, 0,4096);
    } 

  if (createNtupleActive)
    {
      if ((debugActive & 0x02)) printf("generating Ntuple \n");
       if (fNameNtuple) ntupleOutputFile = new TFile(fNameNtuple,"RECREATE");
       else ntupleOutputFile = new TFile("Slope-ntuple.root","RECREATE");
       ntupleOutputFile->cd();
       ntuple = new TNtuple("ntuple",
			    "ntuple",
			    "index:time:evtSeqNr:sector:module:mbo:tdc:slope:error:t1:t2:t3:t4:t5:t6");
    }

  
 fActive=kTRUE;
 return kTRUE;
}

Bool_t HMdcSlope::finalize(void)
{

  HDetector *mdcDet = gHades->getSetup()->getDetector("Mdc");

  if (trendCalculationActive)
  {
      Info("finalize","trendCalculation may take some minutes");
  }

  if (!mdcDet)
    {
	Error("finalize()",
	      "Mdc-Detector setup (gHades->getSetup()->getDetector(\"Mdc\")) missing.");
    }
  else
    {
      Float_t slope, sigma;
      //loop over sectors
      for(Int_t s=0; s<6; s++) 
	{  
	  //loop over modules
	  for(Int_t mo=0; mo<4; mo++) 
	    {  
	      if (!mdcDet->getModule(s, mo)) continue;
	      static Int_t nMb;
              nMb=(*calparraw)[s][mo].getSize();
	      //loop over motherboards
	      for(Int_t mb=0; mb<nMb; mb++) 
		{ 
		  static Int_t nTdc;
                  nTdc=(*calparraw)[s][mo][mb].getSize();
		  for(Int_t t=0; t<nTdc; t++) 
		    {
		      static Int_t n;
		      n=(*nEvt)[s][mo][mb][t];
		 
		      // 
		      // Trend Calculation 
		      // 
		      if (trendCalculationActive)
			{
			  if (n)
			    {
			      fitHistogramm(&slope, &sigma, s, mo, mb, t, 
					    fitHistMethod);
			      if (slope >0 && slope<1)
				{
				  (*calparraw)[s][mo][mb][t].setSlope(slope);
				  (*calparraw)[s][mo][mb][t].setSlopeErr(sigma);
				  (*calparraw)[s][mo][mb][t].setSlopeMethod(2);
				}
			      else
				{
				  (*calparraw)[s][mo][mb][t].setSlope(0.5);
				  (*calparraw)[s][mo][mb][t].setSlopeErr(100);
				  (*calparraw)[s][mo][mb][t].setSlopeMethod(3);
				} 
			    }
			  else
			    {
			      (*calparraw)[s][mo][mb][t].setSlope(0.5);
			      (*calparraw)[s][mo][mb][t].setSlopeErr(100);
			      (*calparraw)[s][mo][mb][t].setSlopeMethod(4);
			    }
			}
		      // 
		      // Online Calculation !!!!!!!!!!! 
		      // 
		      else 
			{
			  (*calparraw)[s][mo][mb][t].setSlope(n?((*avgSlope)[s][mo][mb][t]/n):0);
			  (*calparraw)[s][mo][mb][t].setSlopeErr(n?((*avgSlopeErr)[s][mo][mb][t]/n):0);
			}
		    }
		}
	    }
	}
    }
  // writing to file 

  if (file)
    {
      file->cd();
      hSlope->Write();
      hTime-> Write();

      file->Save();
      file->Close();
      delete file;
    }

  if (createNtupleActive)
    {
      ntupleOutputFile->cd();
      ntuple->Write();
      ntupleOutputFile->Save();
      ntupleOutputFile->Close();
      delete ntupleOutputFile;
    }

  // free memory space;//??

  if (trendCalculationActive)
    {
    }
  
  return kTRUE;
}

void HMdcSlope::calcSlope(Int_t t0, Int_t t1, Int_t t2, Int_t t3, Int_t t4, Int_t t5,
			  Float_t delta,
			  Float_t* myslope,
			  Float_t* myerror,
			  Float_t* chiSquare,
			  Int_t nvals,
			  Int_t myAlgorithm)
{
    // depending on the value of myAlgorithm
    // calc
    // calc5
    // calcCHI
    // calcGraph
    // are called to caculate the values for the slopes

    // set default values
      *myslope  = -1000.;
      *myerror  = -1000.;
      *chiSquare= -1000.;

    // TODO: replace by pointer array to fcn pointers

 switch(myAlgorithm)
   {
   case 1: 
     calc     (t0,t1,t2,t3,t4,t5, delta, myslope, myerror);
     break;   
   case 2: 
     calc5    (t0,t1,t2,t3,t4,t5, delta, myslope, myerror);
     break;   
   case 3:
     calcCHI  (t0,t1,t2,t3,t4,t5, delta, myslope, myerror);
     break;   
   case 4:
     calcGraph(t0,t1,t2,t3,t4,t5, delta, myslope, myerror);
     break;   
   default:
     *myslope  =0.;
     *myerror  =-99.;
     *chiSquare=-99.;
     break;
   }

 // check for finite values
 if (!TMath::Finite(*myslope))
 {
  *myslope = 0.;
 }
 if (!TMath::Finite(*myerror))
 {
  *myerror = 100.;
 }

return;
}

void HMdcSlope::calc(Int_t t0, Int_t t1, Int_t t2, Int_t t3, Int_t t4, Int_t t5,
		     Float_t delta, Float_t *myslope, Float_t *myerror)
{
  // Linear regression
  // with 6 equal distanced base points, (t[i] = delta * i, i = 1...6) 
  // f(t) = a0 + a1 * t
  // 
  // s2a1: (standard deviation)^2 of mya1
  // slope = -1/mya1;
  //                                sqrt(s2a1) 
  // standard deviation of slope = --------------
  //                                  (mya1)^2
  //
  
  if  (t5<0 || t4<0 || t3<0 || t2<0 || t1<0 || t0<0)  
    {
      *myslope = 0.;
      *myerror = 0.;
      return;
    }
  
  // correct error calculation has to be improved
  
  static Double_t sumt ;     // t0   + ... + t5
  static Double_t sumkt;     // t0*1 + ... + t5*6
  static Double_t s2y  ;     // variance of y
  static Double_t sa1  ;     // standard deviation of a1
  static Double_t mya0 ;     // a0
  static Double_t mya1 ;     // a1
  static Double_t difference[6];
  static Int_t ybin[6];
  
  // default values
  sumt =0.;
  sumkt=0.;
  s2y  =0.;
  sa1  =0.;
  mya0 =0.;
  mya1 =0.;
  difference[0] = 0.;
  difference[1] = 0.;
  difference[2] = 0.;
  difference[3] = 0.;
  difference[4] = 0.;
  difference[5] = 0.;

  // copying the values (???)
  ybin[0]=t0;
  ybin[1]=t1;
  ybin[2]=t2;
  ybin[3]=t3;
  ybin[4]=t4;
  ybin[5]=t5;
  
  sumt = ybin[0] +ybin[1]  +ybin[2]  +ybin[3]  +ybin[4]  +ybin[5];

  //         n          
  //        ---         
  //        \                                                                  .
  // sumt  = > ybin[k-1]
  //        /           
  //        ---         
  //        k=1         

  sumkt= ybin[0] +ybin[1]*2+ybin[2]*3+ybin[3]*4+ybin[4]*5+ybin[5]*6;

  //         n  
  //        --- 
  //        \                                                                  .
  // sumkt = > k * ybin[k-1]
  //        /  
  //        ---
  //        k=1
  
  mya0=(13/15)*sumt-0.2*sumkt;
  
  //                      ---          n                n              ---
  //                      |           ---              ---               |
  //              1       |           \                \                 |
  // mya0 = ------------- |  2(2n + 1) > ybin[k-1] - 6  > k * ybin[k-1]  |
  //         n * (n - 1)  |           /                /                 |
  //                      |           ---              ---               |
  //                      ---         k=1              k=1             ---
  //
  //              1                  
  //      = ------------- (  2(2n + 1) sumt - 6 * sumkt) 
  //         n * (n - 1)             
  //                                 
  //                                                    , n=number of Points (=6)
  //
  //    (n=6)     1                                         13
  //      = ------------- ( 2(2*6 + 1) sumt - 6 * sumkt) = ---- sumt - 0.2 sumkt 
  //         6 * (6 - 1)                                    15
  //                                 

 
  mya1=(2*sumkt-7*sumt)/(35*delta);

  //                               ---   n                        n            ---   
  //                               |    ---                      ---             |
  //                   1           |    \                        \               |
  // mya1 = ---------------------- | 12  > k * ybin[k-1] - 6(n+1) > ybin[k-1]    |
  //         delta * n * (n^2 - 1) |    /                        /               |
  //                               |    ---                      ---             |
  //                               ---  k=1                      k=1           ---
  //
  //                   1               
  //      = ---------------------- ( 12 * sumkt - 6(n+1) * sumt )
  //         delta * n * (n^2 - 1)     
  //
  //                                                    , n=number of Points (=6)
  //
  //    (n=6)          1               
  //      = ---------------------- (12 * sumkt - 6(6+1) * sumt)  
  //         delta * 6 * (6^2 - 1)     
  //
  //              1                
  //      = ----------- (2 * sumkt - 7 * sumt)  
  //         delta * 35     
  //

  
  for (Int_t k=1;k<=6;k++)
    { 
      difference[k-1]=(ybin[k-1]-mya0-mya1*delta*k)*(ybin[k-1]-mya0-mya1*delta*k);
    }

  s2y=(difference[0]+difference[1]+difference[2]+difference[3]+difference[4]+difference[5])/4;

  // s2y:s^2y: variance in y:
  //
  //                    n
  //                   ---  
  //           1       \                                    2
  //  s^2y = ----- *    >  (ybin[k-1] - a0 - a1 * k * delta) 
  //         n - 2     /
  //                   ---
  //                   k=1
  //
  //                    n
  //                   ---  
  //           1       \                  2
  //       = ----- *    >  difference[k-1]                     , n=number of Points (=6)
  //         n - 2     /
  //                   ---
  //                   k=1
  //

  
  sa1=(sqrt(s2y/10)/delta); 

  // sa1: standard deviation of a1: 
  //
  //            ------------------------------
  //           /                  12        
  // sa1 =    / (sy^2)*  ---------------------  , n=number of Points (=6)
  //       \ /           delta * n * (n^2 - 1)
  //        V
  //
  //
  //            ------------------------------
  //           /                  12        
  //     =    / (sy^2)*  ---------------------  
  //       \ /           delta * 6 * (6^2 - 1)
  //        V
  //
      
  *myslope=(Float_t) (-1/mya1);

  //            -1
  // myslope = ----
  //           mya1

  *myerror=(Float_t) ((sa1)/(mya1*mya1));
 
  //
  //             sa1
  // myerror = -------
  //                2
  //            mya1

  return; 
}

void HMdcSlope::calc5(Int_t t0, Int_t t1, Int_t t2, Int_t t3, Int_t t4, Int_t t5,
		      Float_t delta, Float_t *myslope, Float_t *myerror)
{
  // Linear regression
  // with 5 equal distanced base points, (t[i] = delta * i, i = 1...5) 
  // f(t) = a0 + a1 * t
  // 
  // s2a1: (standard deviation)^2 of mya1
  // slope = -1/mya1;
  //                                sqrt(s2a1) 
  // standard deviation of slope = --------------
  //                                  (mya1)^2
  //

  if (debugActive &0x10) cout << "." << flush;

  if  (t4<0 || t3<0 || t2<0 || t1<0 || t0<0)
    {
      *myslope = 0.;
      *myerror = 100.;
      return;
    }
  
  // correct error calculation has to be improved
  
  static Double_t sumt ;     // t0   + ... + t1
  static Double_t sumkt;     // t0*1 + ... + t4*5
  static Double_t s2y  ;     // variance of y
  static Double_t sa1  ;     // standard deviation of a1
  static Double_t mya0 ;     // a0
  static Double_t mya1 ;     // a1
  static Double_t difference[5];
  static Int_t ybin[5];
  
  // default values
  sumt =0.;
  sumkt=0.;
  s2y  =0.;
  sa1  =0.;
  mya0 =0.;
  mya1 =0.;
  difference[0] = 0.;
  difference[1] = 0.;
  difference[2] = 0.;
  difference[3] = 0.;
  difference[4] = 0.;

  // copy values
  ybin[0]=t0;
  ybin[1]=t1;
  ybin[2]=t2;
  ybin[3]=t3;
  ybin[4]=t4;
  
  sumt =ybin[0]+ybin[1]  +ybin[2]  +ybin[3]  +ybin[4];
  sumkt=ybin[0]+ybin[1]*2+ybin[2]*3+ybin[3]*4+ybin[4]*5;
  
  mya0=+1.1*sumt-0.3*sumkt;
  
  //                      ---          n                n              ---
  //                      |           ---              ---               |
  //              1       |           \                \                 |
  // mya0 = ------------- |  2(2n + 1) > ybin[k-1] - 6  > k * ybin[k-1]  |
  //         n * (n - 1)  |           /                /                 |
  //                      |           ---              ---               |
  //                      ---         k=1              k=1             ---
  //
  //              1                  
  //      = ------------- (  2(2n + 1) sumt - 6 * sumkt) 
  //         n * (n - 1)             
  //                                 
  //                                                    , n=number of Points (=5)
  //
  //    (n=5)     1                  
  //      = ------------- ( 2(2*5 + 1) sumt - 6 * sumkt) = 1.1 sumt - 0.3 sumkt 
  //         5 * (5 - 1)             
  //                                 

 
  mya1=(sumkt-3*sumt)/(10*delta);

  //                               ---   n                        n            ---   
  //                               |    ---                      ---             |
  //                   1           |    \                        \               |
  // mya1 = ---------------------- | 12  > k * ybin[k-1] - 6(n+1) > ybin[k-1]    |
  //         delta * n * (n^2 - 1) |    /                        /               |
  //                               |    ---                      ---             |
  //                               ---  k=1                      k=1           ---
  //
  //                   1               
  //      = ---------------------- ( 12 * sumkt - 6(n+1) * sumt )
  //         delta * n * (n^2 - 1)     
  //
  //                                                    , n=number of Points (=5)
  //
  //    (n=5)          1               
  //      = ---------------------- (12 * sumkt - 6(5+1) * sumt)  
  //         delta * 5 * (5^2 - 1)     
  //
  //              1                
  //      = ----------- (sumkt - 3 sumt)  
  //         delta * 10     
  //
  
  for (Int_t k=1;k<=5;k++)
    { 
      difference[k-1]=(ybin[k-1]-mya0-mya1*delta*k)*(ybin[k-1]-mya0-mya1*delta*k);
    }

  s2y=(difference[0]+difference[1]+difference[2]+difference[3]+difference[4])/3;

  // s2y:s^2y: variance in y:
  //
  //                    n
  //                   ---  
  //           1       \                                    2
  //  s^2y = ----- *    >  (ybin[k-1] - a0 - a1 * k * delta) 
  //         n - 2     /
  //                   ---
  //                   k=1
  //                    n
  //                   ---  
  //           1       \                  2
  //       = ----- *    >  difference[k-1]                     , n=number of Points (=5)
  //         n - 2     /
  //                   ---
  //                   k=1
  

  sa1=(sqrt(s2y/10)/delta); 

  // sa1: standard deviation of a1: 
  //
  //            ------------------------------
  //           /                  12        
  // sa1 =    / (sy^2)*  ---------------------  , n=number of Points (=5)
  //       \ /           delta * n * (n^2 - 1)
  //        V
  //
      
  *myslope=(Float_t) (-1/mya1);

  //            -1
  // myslope = ----
  //           mya1

  *myerror=(Float_t) ((sa1)/(mya1*mya1));
 
  //
  //             sa1
  // myerror = -------
  //                2
  //            mya1

if ((debugActive & 0x10))
  {
    cout << endl;
    cout << "LS" << endl;
    cout <<  (-1/mya1) <<" " << ((sa1)/(mya1*mya1)) << endl;
  }

  return; 
}

void HMdcSlope::calcCHI(Int_t t0, Int_t t1, Int_t t2, Int_t t3, Int_t t4, Int_t t5,
			Float_t delta, Float_t *myslope, Float_t *myerror)
{
  // calculate slopes using histogram based fit

  if  (t5<0 || t4<0 || t3<0 || t2<0 || t1<0 || t0<0)  
    {
      *myslope = 0.;
      *myerror = 100.;
      return;
    }
  
  TH1F *hFitarea = new TH1F("fit","fit",5,0.5,5.5);
  hFitarea->SetBinContent((Int_t) 1 ,t0);
  hFitarea->SetBinContent((Int_t) 2 ,t1);
  hFitarea->SetBinContent((Int_t) 3 ,t2);
  hFitarea->SetBinContent((Int_t) 4 ,t3);
  hFitarea->SetBinContent((Int_t) 5 ,t4);
  
  for (Int_t myiter=1;myiter <=5; myiter++)
    {
      hFitarea->SetBinError((Int_t) myiter, binError);
    }
  
  // calculation of slope & chisquare
  
  TF1 *f = new TF1("f1","pol1", 0,6); // call fit function poynom 1.order in x-range
  hFitarea->Fit("f1","RNQ");
  
  delete hFitarea;
  
  *myslope = (Float_t) ((-1)/f->GetParameter(1)*delta) ;
  *myerror = (Float_t) (f->GetParError(1)/((*myslope)*(*myslope)*delta));
  //  *chi2= (Float_t) (f->GetChisquare()/f->GetNDF());

  if (debugActive & 0x10)
    {
      cout << endl;
      cout << "chi" << endl;
      cout << *myslope <<" " << *myerror << endl;
    }

  delete f;

  return; 
}


void HMdcSlope::calcGraph(Int_t t0, Int_t t1, Int_t t2, Int_t t3, Int_t t4, Int_t t5,
			  Float_t delta, Float_t *myslope, Float_t *myerror)
{
 // calculate slopes using graph based fit

  static Int_t nrOfPts=5;
  static Float_t x[5];
  static Float_t y[5];
  static Float_t ex[5];
  static Float_t ey[5];
      
  for (Int_t myiter=0;myiter<nrOfPts; myiter++)
    {
	x[myiter] =(myiter+1)*delta;
	y[myiter] =0.;
	ex[myiter]=0.;
	ey[myiter]=binError;
    }

  y[0]= t0;
  y[1]= t1;
  y[2]= t2;
  y[3]= t3;
  y[4]= t4;
 
  TGraphErrors *gFitGraph = new TGraphErrors(nrOfPts,x,y,ex,ey);
   
  // calculation of slope & chisquare
  
  TF1 *f = new TF1("f1","pol1", 0,6*delta); // call fit function poynom 1.order in x-range
  gFitGraph->Fit("f1","RNQ");
  
  delete gFitGraph;
  
  *myslope = (Float_t) ((-1)/f->GetParameter(1)) ;
  *myerror = (Float_t) (f->GetParError(1)/((*myslope)*(*myslope)));
  //  *chi2= (Float_t) (f->GetChisquare()/f->GetNDF());

  if(debugActive & 0x10)
  {
      cout << endl;
      cout << "Graph" << endl;
      cout << *myslope <<" " << *myerror << endl;
  }

  delete f;
  return; 
}

Int_t HMdcSlope::calcBinNr(Double_t value, Double_t min, Double_t max, Int_t nbins)
{
  // calculate bin number for value in given binned ranged
  if (min<max && value >= min && value <= max)
    {
      return ((Int_t)((nbins/(max-min))*(value - min)));
    }
  return 0;
}

Int_t HMdcSlope::execute(void)
{
    // evaluates the calibration events in the data stream
    // based on the input from HMdcRaw
    // slopes are determined
    // Two modes of slope evaluation do exist
    // a) TrendCalculation (offline procedure)
    // b) Online Calculation
    //
    //  // a) TrendCalculation
    //  //
    //  // the idea of this offline procedure is to first fill
    //  // the calculated slopes of the whole dataset into
    //  // a histogramm (SlopeTrend) and to derive the slope value
    //  // as the result of a Gaussian Fit in the finalize function.
    //  // Where the slope corresponds to the first moment
    //  // and the error to the sigma
    //  //
    //  //
    //  // b) Online Calculation
    //  //
    //  // For each event the slope is calculated with the chosen algorithm
    //  // and its content is written to the calparraw container
    //  //

  static HMdcRaw* raw=NULL;
  static Int_t s, mo, mb, t;
  static Int_t index;

  static UInt_t DaqTime     ;
  static Int_t DaqHour      ;
  static Int_t DaqMin       ;
  static Int_t DaqSec       ;
  static Float_t DaqJustSec ;
  static Float_t delta      ;
  index++;

  if (debugActive& 0x04){if (!(index%10)) cout << index << endl;}

  DaqTime     = (gHades->getCurrentEvent()->getHeader()->getTime());
  DaqHour      =        ((DaqTime & 0x00ff0000) >> 16);
  DaqMin       =        ((DaqTime & 0x0000ff00) >> 8);
  DaqSec       =          DaqTime & 0x000000ff;
  DaqJustSec = 3600.*DaqHour + 60.*DaqMin + DaqSec;
  
  delta = 401.5/2; // delta of 401.5/2 should be somewhen retrieved from the database !!!!
  iter->Reset();

  // 
  // TrendCalculation
  // 

  if (trendCalculationActive)
    { 
      // 
      // TrendCalculation
      // 
      // the idea of this offline procedure is to first fill 
      // the calculated slopes of the whole dataset into 
      // a histogramm (SlopeTrend) and to derive the slope value 
      // as the result of a Gaussian Fit.
      // Where the slope corresponds to the first moment
      // and the error to the sigma
      //

      static Float_t slope, error, chiSquare; 

      while ((raw=(HMdcRaw*)iter->Next()))
	{
	  slope     = -100.;
          error     = -100.;
          chiSquare = -100.;
	  raw->getAddress(s, mo, mb, t);
              
          Int_t time1 = raw->getTime(1);
          Int_t time2 = raw->getTime(2);
          Int_t time3 = raw->getTime(3);
          Int_t time4 = raw->getTime(4);
          Int_t time5 = raw->getTime(5);
          Int_t time6 = raw->getTime(6);       
	  time1 |= 2048;
	  if(time3 < time4) time3 |= 2048;
	  if(time2 < time3) time2 |= 2048;
          
	  calcSlope(time1,
		    time2,
		    time3,
		    time4,
		    time5,
		    time6,
		    delta,
		    &slope,
		    &error,
		    &chiSquare,
		    nTimeValues,
		    calibrationAlgorithm);
	
	  slopeTrend[s][mo][mb][t][calcBinNr(slope,0.3,0.8,5000)]++;
	  
	  if(slopeCat)
	    {
	      static HLocation loc;
	      loc.set(4,0,0,0,0);

	      //set location indexes 
	      
	      loc[0] = s;
	      loc[1] = mo;
	      loc[2] = mb;
	      loc[3] = t;

	      // filling the data to the location
	      // first get a free or existing slot

	      static HMdcSlopes *pMdcSlopes;
              pMdcSlopes = NULL;
	      pMdcSlopes = (HMdcSlopes*) slopeCat->getSlot(loc);
	      if (pMdcSlopes) 
		{
		  pMdcSlopes = new (pMdcSlopes) HMdcSlopes();

		  pMdcSlopes->setAddress(loc[0],loc[1],loc[2],loc[3]);
		  pMdcSlopes->setSlope(slope);
		  pMdcSlopes->setError(error);
		  pMdcSlopes->setMethode(-1);
		}
	      else
		{
		  Warning("execute","couldn't get slot for HMdcSlopes");
		}
	    }

	  (*nEvt)[s][mo][mb][t]++;

	  if(file) 
	    {
	      file->cd();
	      hSlope->Fill(slope);
              hTime->Fill(time1);
              hTime->Fill(time2);
              hTime->Fill(time3);
              hTime->Fill(time4);
              hTime->Fill(time5);
              hTime->Fill(time6);
	    }

	  if (createNtupleActive)
	    {
	      ntupleOutputFile->cd();
	      if (!selectorActive || 
		  (selectorActive && 
		   (mbSelector == mb || mbSelector == -1) &&
		   (tdcSelector == t || tdcSelector == -1) ))
		{
		  ntuple->Fill(index,
			       DaqJustSec,
			       gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),
			       s,mo,mb,t,slope,error,
			       time1,time2,time3,time4,time5,time6);
		}
	    }
	}
      return 0;
    }

  //
  // Online Calculation
  //
  else
    {
      //
      // Online Calculation
      //
      // For each event the slope is calculated with the chosen algorithm
      // and its content is written to the calparraw container
      //

      static Float_t slope[3], error[3], chiSquare[3]; 

      static Bool_t algorithmActive[3] = {linRegActive, histFitActive, graphFitActive};

      // default values
      for (Int_t index=0; index < 3; index++)
      {
	  slope    [index]=-100.;
	  error    [index]=-100.;
          chiSquare[index]=-100.;
      }
      while ((raw=(HMdcRaw*)iter->Next()))
	{
	  raw->getAddress(s, mo, mb, t);
	  
	  if (!selectorActive || 
	      (selectorActive && (mbSelector==mb || mbSelector == -1) && (tdcSelector == t || tdcSelector == -1) ))
	    {
              
              Int_t time1 = raw->getTime(1);
              Int_t time2 = raw->getTime(2);
              Int_t time3 = raw->getTime(3);
              Int_t time4 = raw->getTime(4);
              Int_t time5 = raw->getTime(5);
              Int_t time6 = raw->getTime(6);       
	      time1 |= 2048;
	      if(time3 < time4) time3 |= 2048;
	      if(time2 < time3) time2 |= 2048;
              
	      for (Int_t methodIter=0; methodIter < HMDC_SLOPE_NALGORITHM; methodIter++)
		{
		  if (algorithmActive[methodIter]==kTRUE)
		    {
                      // default values
		      slope    [methodIter]=-100.;
		      error    [methodIter]=-100.;
		      chiSquare[methodIter]=-100.;
		      calcSlope(time1,
				time2,
				time3,
				time4,
				time5,
				time6,
				delta,
				&slope[methodIter],
				&error[methodIter],
				&chiSquare[methodIter],
				nTimeValues,
				methodIter+1);
		    }
		}
	      
	      (*avgSlope)   [s][mo][mb][t] += slope[0];
	      (*avgSlopeErr)[s][mo][mb][t] += error[0];
	      (*nEvt)       [s][mo][mb][t] ++;
	      
	      if(file) 
		{
		  hSlope->Fill(slope[0]);
                  hTime->Fill(time1);
                  hTime->Fill(time2);
                  hTime->Fill(time3);
                  hTime->Fill(time4);
                  hTime->Fill(time5);
                  hTime->Fill(time6);
		}
	      
	      if(slopeCat)
		{
		  static HLocation loc;
		  loc.set(4,0,0,0,0);
		  
		  //set location indexes 
		  
		  loc[0] = s;
		  loc[1] = mo;
		  loc[2] = mb;
		  loc[3] = t;
		  
		  // filling the data to the location
		  // first get a free or existing slot
		  
		  static HMdcSlopes *pMdcSlopes;
                  pMdcSlopes = NULL;
		  pMdcSlopes = (HMdcSlopes*) slopeCat->getSlot(loc);
		  if (pMdcSlopes) 
		    {
		      pMdcSlopes = new (pMdcSlopes) HMdcSlopes();

		      pMdcSlopes->setAddress(loc[0],loc[1],loc[2],loc[3]);
		      pMdcSlopes->setSlope(slope[0]);
		      pMdcSlopes->setError(error[0]);
		      if (slope[0]>0. && slope[0] <1.) { pMdcSlopes->setMethode(1); }
		    }
		  else
		    {
		      Warning("execute","couldn't get slot for HMdcSlopes");
		    }
		}
	      
	      if (slope[0]>0 && slope[0] <1)
		{	   
		  (*calparraw)[s][mo][mb][t].setSlope   (slope[0]);
		  (*calparraw)[s][mo][mb][t].setSlopeErr(error[0]);
		  (*calparraw)[s][mo][mb][t].setSlopeMethod(1);
		}
	      else if(slope[0]==0)
		{
		  // in later versions the values should be set to a default or former value
		  (*calparraw)[s][mo][mb][t].setSlope(0.5);
		  (*calparraw)[s][mo][mb][t].setSlopeErr(100.);
		  (*calparraw)[s][mo][mb][t].setSlopeMethod(4);
		}
	      else if(slope[0]>1)
		{
		  (*calparraw)[s][mo][mb][t].setSlope(0.);
		  (*calparraw)[s][mo][mb][t].setSlopeErr(0.);
		  (*calparraw)[s][mo][mb][t].setSlopeMethod(3); // manually set
		}
	    }
	}
      return 0;
    }
}

