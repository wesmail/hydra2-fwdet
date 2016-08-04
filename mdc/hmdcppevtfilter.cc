//*-- AUTHOR : Y.C.Pachmayer
//*-- Modified :
//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcPPEvtFilter
//
//  pp elastic events will be selected  
//  all other events will be skipt.
//  selection criteria:
//  1. Trig Bit - only opposite sector triggered events
//  2. Multiplicity>=2
//  3. fabs((phi[i]-phi[j])-phimean) < phiwidth
//  4. fabs(tantheta[i]*tantheta[j]) < tanthetawidth
//
//
//  input level:
//  default is mdccluster level
//
//  if you want to have mdcseg level as input put flagcat in constructor !=0
//
///////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hmdcppevtfilter.h"
#include "TH1.h"
#include "TF1.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"

ClassImp(HMdcPPEvtFilter)


HMdcPPEvtFilter::HMdcPPEvtFilter(const Text_t *name,const Text_t *title, Int_t flagcat) : HReconstructor(name,title) {
  flagcategory=flagcat;
  initVariables();
}

HMdcPPEvtFilter::HMdcPPEvtFilter(void) : HReconstructor("HMdcPPEvtFilter","HMdcPPEvtFilter"){
	flagcategory=0;
	initVariables();
}

HMdcPPEvtFilter::~HMdcPPEvtFilter(void){
  if (mdcsegiter) delete mdcsegiter;
  if (mdcclusiter) delete mdcclusiter;
}

void HMdcPPEvtFilter::initVariables()
{
  catmHMdcClus=0;
  mdcclus=0;
  mdcclusiter=0;
  
  catmHMdcSeg=0;
  mdcseg=0;
  mdcsegiter=0;

  histphidiff=0;
  histtanthetamult=0;
  histphidiffcut=0;
  histtanthetamultcut=0;
  histchiphi=0;
  histchiphicut=0;
  histchitheta=0;
  histchithetacut=0;

  // default values for phimean, tanthetamean and the corresponding width
  phimean=180;
  tanthetamean=0.65;
  phiwidth=20;  
  tanthetawidth=0.15;

  setHistFile(".","pphs_","NEW");
  saveHist=saveCanv=savePSFile=kTRUE;
}

void HMdcPPEvtFilter::createHists()
{
  // hists are created
  
  sprintf(title, "phi difference");
  sprintf(name, "phidifference");
  sprintf(xtitle, "phi diff");
  sprintf(ytitle, "#");
  histphidiff = new TH1F(name, title, 720, 0, 360);
  histphidiff -> SetXTitle(xtitle);
  histphidiff -> SetYTitle(ytitle);

  sprintf(title, "tantheta_tantheta");
  sprintf(name, "tantheta_tantheta");
  sprintf(xtitle, "tantheta_tantheta");
  sprintf(ytitle, "#");
  histtanthetamult = new TH1F(name, title, 100, 0.0, 1.0);
  histtanthetamult -> SetXTitle(xtitle);
  histtanthetamult -> SetYTitle(ytitle);

  sprintf(title, "phi difference after cut");
  sprintf(name, "phidifferenceaftercut");
  sprintf(xtitle, "phi diff after cut");
  sprintf(ytitle, "#");
  histphidiffcut = new TH1F(name, title, 720, 0, 360);
  histphidiffcut -> SetXTitle(xtitle);
  histphidiffcut -> SetYTitle(ytitle);

  sprintf(title, "tantheta_tantheta after cut");
  sprintf(name, "tantheta_tanthetaaftercut");
  sprintf(xtitle, "tantheta_tantheta after cut");
  sprintf(ytitle, "#");
  histtanthetamultcut = new TH1F(name, title, 100, 0.0, 1.0);
  histtanthetamultcut -> SetXTitle(xtitle);
  histtanthetamultcut -> SetYTitle(ytitle);
  if(flagcategory!=0)
    {
      sprintf(title, "chi2 vs phi");
      sprintf(name, "chi2vsphi");
      sprintf(xtitle, "phi");
      sprintf(ytitle, "chi2");
      histchiphi = new TH2F(name, title, 360, 0, 360, 620, -10, 300);
      histchiphi -> SetXTitle(xtitle);
      histchiphi -> SetYTitle(ytitle);
      
      sprintf(title, "chi2 vs phi after cut");
      sprintf(name, "chi2vsphiaftercut");
      sprintf(xtitle, "phi");
      sprintf(ytitle, "chi2");
      histchiphicut = new TH2F(name, title, 360, 0, 360, 620, -10, 300);
      histchiphicut -> SetXTitle(xtitle);
      histchiphicut -> SetYTitle(ytitle);
      
      sprintf(title, "chi2 vs theta");
      sprintf(name, "chi2vstheta");
      sprintf(xtitle, "theta");
      sprintf(ytitle, "chi2");
      histchitheta = new TH2F(name, title, 540, 0, 540, 620, -10, 300);
      histchitheta -> SetXTitle(xtitle);
      histchitheta -> SetYTitle(ytitle);
      
      sprintf(title, "chi2 vs theta after cut");
      sprintf(name, "chi2vsthetaaftercut");
      sprintf(xtitle, "theta");
      sprintf(ytitle, "chi2");
      histchithetacut = new TH2F(name, title, 540, 0, 540, 620, -10, 300);
      histchithetacut -> SetXTitle(xtitle);
      histchithetacut -> SetYTitle(ytitle);
    }
  
}

void HMdcPPEvtFilter::setThetaCuts(Float_t Theta, Float_t DTheta) {
  // this function sets the mean for tantheta[i]*tantheta[j] and its width 
  tanthetamean=Theta;
  tanthetawidth=DTheta;
}

void HMdcPPEvtFilter::setPhiCuts(Float_t Phi, Float_t DPhi) {
 // this function sets the mean for phi[i]-phi[j] and its width 
  phimean=Phi;
  phiwidth=DPhi;
}

Bool_t HMdcPPEvtFilter::init(void) {
  rtdb=gHades->getRuntimeDb();


 if(flagcategory==0)
    {
      catmHMdcClus=gHades->getCurrentEvent()->getCategory(catMdcClus);
      if (!catmHMdcClus) {
	Error("HMdcPPEvtFilterClusLevel::init()","NO HMDCCLUS FOUND!");
	return kFALSE;
      }
      mdcclusiter=(HIterator *)catmHMdcClus->MakeIterator();
      if(!mdcclusiter)
	{
	  return kFALSE;
	}
    }
 
 else
   {
     catmHMdcSeg=gHades->getCurrentEvent()->getCategory(catMdcSeg);
     if (!catmHMdcSeg) {
       Error("HMdcPPEvtFilter::init()","NO HMDCSEG FOUND!");
       return kFALSE;
     }
  mdcsegiter=(HIterator *)catmHMdcSeg->MakeIterator();
  if(!mdcsegiter)
    {
      return kFALSE;
    }
   }

  createHists();

  return kTRUE;

}

Bool_t HMdcPPEvtFilter::reinit(void) {
  return kTRUE;
}


Int_t HMdcPPEvtFilter::execute(void) {
 
  // done for testing
  //  Int_t nEvtId       = 0; //Evt Id
  //   Int_t nEvt         = 0; //
  //   nEvtId = gHades->getCurrentEvent()->getHeader()->getId();
  //   nEvt=gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
  //   // if(nEvt==10)
  //   if(TMath::Even(nEvt))
  //     { return 0;}
  //   else
  //     {return kSkipEvent;}  // kSkipEvent=-9
  // end testing


  resetCounters();
  resetArrays();
  IsElasticFlag=0;
  phishift[0]=0;
  phishift[1]=60;
  phishift[2]=120;
  phishift[3]=180;
  phishift[4]=240;
  phishift[5]=-60;

  TrigBit=gHades->getCurrentEvent()->getHeader()->getTBit();

  if((TrigBit & (1<<5)) == 0) 
    { 
      return  kSkipEvent;  
    }
  else
      {
	if(flagcategory==0)
	  {
	    mdcclusiter->Reset();
	    while ( (mdcclus=(HMdcClus *)mdcclusiter->Next()) != 0 )
	      {
		if((mdcclus->getMod()<2)&&(mdcclus->getMod()>-1))
		  { 	    
		    if(multiplicity > 200){
		      return  kSkipEvent;
		    }
		    sector[multiplicity]=mdcclus->getSec();
		    phi[multiplicity]=TMath::RadToDeg()*(mdcclus->getPhi())+phishift[(sector[multiplicity])];
		    theta[multiplicity]=TMath::RadToDeg()*(mdcclus->getTheta()); 

		    multiplicity++;
		  }	      
	      }
	  }
	else
	  {
	    mdcsegiter->Reset();
	    while ( (mdcseg=(HMdcSeg *)mdcsegiter->Next()) != 0 )
	      {
		if((mdcseg->getIOSeg())==0)
		  {
		    if(multiplicity > 200){
		      return  kSkipEvent;
		    }
		    sector[multiplicity]=mdcseg->getSec();
		    phi[multiplicity]=TMath::RadToDeg()*(mdcseg->getPhi())+phishift[(sector[multiplicity])];
		    theta[multiplicity]=TMath::RadToDeg()*(mdcseg->getTheta()); 
		    chi2[multiplicity]=mdcseg->getChi2();
		    histchiphi->Fill(phi[multiplicity],chi2[multiplicity]);
		    histchitheta->Fill((theta[multiplicity]+(sector[multiplicity])*90),chi2[multiplicity]);
		    multiplicity++;
		  }
	      }
	  }

	
	if(multiplicity>=2)
	  {
	    for(Int_t i=0; i<multiplicity; i++)
	      {
		for(Int_t j=(i+1); j<multiplicity; j++)
		  {
		    phidiff=fabs(phi[j]-phi[i]);
		    tanthetamult=(TMath::Tan(TMath::DegToRad()*theta[j]))*(TMath::Tan(TMath::DegToRad()*theta[i]));
		    histphidiff->Fill(phidiff);
		    histtanthetamult->Fill(tanthetamult);
		    if((fabs(phidiff-phimean)<phiwidth)&&(fabs(tanthetamult-tanthetamean)<tanthetawidth))
		      {
			histphidiffcut->Fill(phidiff);
			histtanthetamultcut->Fill(tanthetamult);
			if(flagcategory!=0)
			  {
			    histchiphicut->Fill(phi[i],chi2[i]);
			    histchiphicut->Fill(phi[j],chi2[j]);
			    histchithetacut->Fill((theta[i]+(sector[i])*90),chi2[i]);
			    histchithetacut->Fill((theta[j]+(sector[j])*90),chi2[j]);
			  }
			IsElasticFlag=1;
		      }
		  }
	      }
	  }

	if((multiplicity<2) || (IsElasticFlag==0))
	  {
	    return  kSkipEvent;
	  }
      }

  return 0;

}


void HMdcPPEvtFilter::setHistFile(const Char_t* dir,const Char_t* suf,const Char_t* option) {
  // the hist output file is set
    TString tmp=dir;
    if(tmp.Length()>0) {
	histFileDir=dir;
	if(histFileDir.Length()>0 && histFileDir(histFileDir.Length()-1)!='/')
	    histFileDir+='/';
    }
    tmp=suf;
    if(tmp.Length()>0) histFileSuffix=suf;
    tmp=option;
    if(tmp.Length()>0) histFileOption=option;
}


Bool_t HMdcPPEvtFilter::finalize(void) {

 canv1 = new TCanvas("canv1","Phi and Theta",140,60,800,900);
 canv1->Divide(1,2);
 canv1->cd(1);
 histphidiff->DrawCopy();
 canv1->cd(2);
 histtanthetamult->DrawCopy();  
 canv1->Update();

 canv2 = new TCanvas("canv2","Phi and Theta after Cut",140,60,800,900);
 canv2->Divide(1,2);
 canv2->cd(1);
 histphidiffcut->DrawCopy();
 canv2->cd(2);
 histtanthetamultcut->DrawCopy();  
 canv2->Update();

 if(flagcategory!=0)
   {
     canv3 = new TCanvas("canv3","Chi2 vs Phi and Theta",140,60,800,900);
     canv3->Divide(2,2);
     canv3->cd(1);
     histchiphi->DrawCopy("colz");
     canv3->cd(2);
     histchiphicut->DrawCopy("colz");  
     canv3->cd(3);
     histchitheta->DrawCopy("colz");
     canv3->cd(4);
     histchithetacut->DrawCopy("colz");
     canv3->Update();
   }

 // writing hists to root file
 Bool_t fitFlag=kTRUE;
 const Char_t* flag=(fitFlag) ? "":"x_";
 TFile* file=0;
 if(saveHist || saveCanv) file=openHistFile(flag);
 if(file) {
   if(saveHist) {
     if(histphidiff) histphidiff->Write();
     if(histtanthetamult) histtanthetamult->Write();
     if(histphidiffcut) histphidiffcut->Write();
     if(histtanthetamultcut) histtanthetamultcut->Write();
     if(flagcategory!=0)
       {
	 if(histchiphi) histchiphi->Write();
	 if(histchitheta) histchitheta->Write();
	 if(histchiphicut) histchiphicut->Write();
	 if(histchithetacut) histchithetacut->Write();
       }
   }
   if(saveCanv) {
     canv1->Write();
     canv2->Write();
     if(flagcategory!=0)
       {
	 canv3->Write();
       }
   }
   
   file->Close();
   delete file;
 }
 
 // writing hists to ps-file
 if(savePSFile) {
   TString tPsFile=psFile+"(";
   canv1->Print(tPsFile.Data());
   tPsFile=psFile+")";
   canv2->Print(tPsFile.Data());
 }


 return kTRUE;

}


TFile* HMdcPPEvtFilter::openHistFile(const Char_t* flag) {
  // hist output file is opened
  histFile=histFileDir+flag+histFileSuffix+flNmWoExt+".root";
  TFile* file=new TFile(histFile.Data(),histFileOption.Data());
  if(file==0 || file->IsOpen()) {
    psFile=histFileDir+flag+histFileSuffix+flNmWoExt+".ps";
    return file;
  }
  printf("File %s already exists!\n",histFile.Data());
  const  Char_t* ind="123456789";
  for(Int_t i=0;i<9;i++){
    delete file;
    histFile=histFileDir+flag+histFileSuffix+flNmWoExt+"_"+ind[i]+".root";
    file=new TFile(histFile.Data(),histFileOption.Data());
    if(file==0 || file->IsOpen()) {
      psFile=histFileDir+flag+histFileSuffix+flNmWoExt+"_"+ind[i]+".ps";
      return file;
    }
  }
  Error("openHistFile","Can not open histograms file!");
  delete file;
  return 0;
}
