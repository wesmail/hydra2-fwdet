//*-- AUTHOR : Y.C.Pachmayer
//*-- Modified :
//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcBitFlipCor
// corrects bitflips of a whole mbo
//
///////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hmdcbitflipcor.h"
#include "TROOT.h"
#include "hmdcdef.h"
#include "hmdcbitflipcorpar.h"
#include "hmdcgeompar.h"
#include "hspecgeompar.h"
#include "hiterator.h"
#include "hcategory.h"
#include "hruntimedb.h"
#include "hmdcdetector.h"
#include "hades.h"
#include "heventheader.h"
#include "hevent.h"
#include "hmdcraweventheader.h"
#include "hmdcraw.h"
#include "hmdcrawcor.h"
#include "hlocation.h"
#include "hspectrometer.h" 
#include "hmatrixcategory.h"

ClassImp(HMdcBitFlipCor)


HMdcBitFlipCor::HMdcBitFlipCor(const Text_t *name,const Text_t *title, Int_t PHist, Int_t PRawCor) : HReconstructor(name,title) {
  initVariables();
  phist=PHist;
  prawcor=PRawCor;
}

HMdcBitFlipCor::HMdcBitFlipCor(void) : HReconstructor("HMdcBitFlipCor","HMdcBitFlipCor") {
  initVariables();
  phist=0;
  prawcor=0;
}

HMdcBitFlipCor::~HMdcBitFlipCor(void){
  if (mdcrawiter) delete mdcrawiter;
}

void HMdcBitFlipCor::setRootOutputFilename(TString name)
{

  if(name.CompareTo("")==0)
    {
      Error("HMdcCosmicsSimul:setRootOutPutFilename()","NO ROOT OUTPUT SPEZIFIED!");
      exit(1);
    }
  else
    {
      rootoutputfilename=name;
    }
}

void HMdcBitFlipCor::setAsciiOutputFilename(TString name)
{

  if(name.CompareTo("")==0)
    {
      Error("HMdcCosmicsSimul:setAsciiOutPutFilename()","NO ASCII OUTPUT SPEZIFIED!");
      exit(1);
    }
  else
    {
      asciioutputfilename=name;
    }
}

void HMdcBitFlipCor::initVariables()
{
  catmHMdcRaw=0;
  mdcraw=0;
  mdcrawiter=0;


  rawREH=0;
  iterREH=0;

  rawcorCat=0;
  rawcor=0;
  loc.set(4,0,0,0,0);

  asciioutputfile=0;
  rootoutputfile=0;

  for(Int_t s=0;s<6;s++)
    {
      for(Int_t m=0; m<4; m++)
	{
	  for(Int_t mbo=0; mbo<16;mbo++)
	    {
	      for(Int_t v=0; v<9;v++)
		{
		  paramsbitcorrection[s][m][mbo][v]=-10;
		}
	    }
	}
    }

  for(Int_t s=0;s<6;s++)
    {
      for(Int_t m=0; m<4; m++)
	{
	  for(Int_t mbo=0; mbo<16; mbo++)
	    {
	      counterevents[s][m][mbo]=0;
	      countersavedtime1[s][m][mbo]=0;
	      countersavedtime2[s][m][mbo]=0;

	      histtime1b[s][m][mbo]=0;
	      histtime1g[s][m][mbo]=0;
	      histtime2b[s][m][mbo]=0;
	      histtime2g[s][m][mbo]=0;
	    }
	}
    }

}

void HMdcBitFlipCor::createHists()
{
  // hists are created

  for(Int_t s=0;s<6;s++)
    {
      for(Int_t m=0; m<4; m++)
	{
	  for(Int_t mbo=0; mbo<16; mbo++)
	    {
	      sprintf(title, "time1 raw before bitshift corrs%im%imbo%i",s,m,mbo);
	      sprintf(name, "time1rawbeforebitcors%im%imbo%i",s,m,mbo);
	      sprintf(xtitle, "time1 [ch]");
	      sprintf(ytitle, "#");
	      histtime1b[s][m][mbo] = new TH1F(name, title, 1024, 0, 2048);
	      histtime1b[s][m][mbo] -> SetXTitle(xtitle);
	      histtime1b[s][m][mbo] -> SetYTitle(ytitle);
	      
	      sprintf(title, "time1 raw after bitshift corrs%im%imbo%i",s,m,mbo);
	      sprintf(name, "time1rawafterbitcors%im%imbo%i",s,m,mbo);
	      sprintf(xtitle, "time1 [ch]");
	      sprintf(ytitle, "#");
	      histtime1g[s][m][mbo] = new TH1F(name, title, 1024, 0, 2048);
	      histtime1g[s][m][mbo] -> SetXTitle(xtitle);
	      histtime1g[s][m][mbo] -> SetYTitle(ytitle);
	      
	      
	      sprintf(title, "time2 raw before bitshift corrs%im%imbo%i",s,m,mbo);
	      sprintf(name, "time2rawbeforebitcors%im%imbo%i",s,m,mbo);
	      sprintf(xtitle, "time2 [ch]");
	      sprintf(ytitle, "#");
	      histtime2b[s][m][mbo] = new TH1F(name, title, 1024, 0, 2048);
	      histtime2b[s][m][mbo] -> SetXTitle(xtitle);
	      histtime2b[s][m][mbo] -> SetYTitle(ytitle);
	      
	      sprintf(title, "time2 raw after bitshift corrs%im%imbo%i",s,m,mbo);
	      sprintf(name, "time2rawafterbitcors%im%imbo%i",s,m,mbo);
	      sprintf(xtitle, "time2 [ch]");
	      sprintf(ytitle, "#");
	      histtime2g[s][m][mbo] = new TH1F(name, title, 1024, 0, 2048);
	      histtime2g[s][m][mbo] -> SetXTitle(xtitle);
	      histtime2g[s][m][mbo] -> SetYTitle(ytitle);
	      
	    }
	}
    }
  
 

}

Bool_t HMdcBitFlipCor::init(void) {
  
  rtdb=gHades->getRuntimeDb();

  catmHMdcRaw=gHades->getCurrentEvent()->getCategory(catMdcRaw);
  if (!catmHMdcRaw) {
    Error("HMdcBitFlipCor::init()","NO HMDCRAW FOUND!");
    return kFALSE;
  }
  mdcrawiter=(HIterator *)catmHMdcRaw->MakeIterator();
  if(!mdcrawiter)
    {
      return kFALSE;
    }

  catmHMdcRawEventHeader=gHades->getCurrentEvent()->getCategory(catMdcRawEventHeader);
  if (!catmHMdcRawEventHeader) {
    Error("HMdcBitFlipCor::init()","NO HMDCRAW FOUND!");
    return kFALSE;
  }
  iterREH=(HIterator *)catmHMdcRawEventHeader->MakeIterator();
  if(!iterREH)
    {
      return kFALSE;
    }

  if(phist==1)
    {
      createHists();
    }

  getParameterContainers();

 if(prawcor==1)
   {
     rawcorCat=(HCategory*)((HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector("Mdc"))->buildCategory(catMdcRawCor));   
     if (!rawcorCat) return kFALSE;
     else ((HEvent*)(gHades->getCurrentEvent()))->addCategory(catMdcRawCor,rawcorCat,"Mdc");
   }
     

  return kTRUE;

}

void HMdcBitFlipCor::getParameterContainers()
{
  fMdcBitFlipCorPar=(HMdcBitFlipCorPar*)rtdb->getContainer("MdcBitFlipCorPar");  
  if(!fMdcBitFlipCorPar)
    {
      Error("HMdcBitFlipCorPar:getParameterContainer()","ZERO POINTER FOR HMDCBITFLIPCORPAR RECIEVED!");
      exit(1);
    }
}

Bool_t HMdcBitFlipCor::reinit(void) {
  fparamsbitcorrection();
  
  return kTRUE;
}

Bool_t  HMdcBitFlipCor::fparamsbitcorrection()
{
  if(fMdcBitFlipCorPar==0){printf("Error fMdcBitFlipCorPar ... not found \n");}
  for(Int_t s=0;s<6;s++)
    {
      for(Int_t m=0; m<4; m++)
	{
	  for(Int_t mbo=0; mbo<16;mbo++)
	    {
	      for(Int_t v=0; v<9;v++)
		{
		  paramsbitcorrection[s][m][mbo][v]=fMdcBitFlipCorPar->getBitFlipCorParameter(s,m,mbo,v);
		}
	    }
	}
    }
  //fMdcBitFlipCorPar->printParam();
  return kTRUE;
     
}

Int_t HMdcBitFlipCor::execute(void) {

  resetVar();
  iterREH->Reset();
  while ( (rawREH=(HMdcRawEventHeader *)iterREH->Next()) != 0 ){ 
    trigtype=rawREH->getTriggerType();
  }

  mdcrawiter->Reset();
  while ( (mdcraw=(HMdcRaw *)mdcrawiter->Next()) != 0 )
    {
      if(trigtype!=9)
	{
	  mdcraw->getAddress(sector, module, mbo, tdc);
	 
	  loc[0]=sector;
	  loc[1]=module;
	  loc[2]=mbo;
	  loc[3]=tdc;

	  if(prawcor==1)
	    {
	      rawcor=(HMdcRawCor *)rawcorCat->getSlot(loc);
	      rawcor=new(rawcor)HMdcRawCor;
	      rawcor->setAddress(sector, module, mbo, tdc);
	    }

	  time1=mdcraw->getTime(1);
	  time2=mdcraw->getTime(2);
	  if(paramsbitcorrection[sector][module][mbo][0]>0)
	    {  
	      counterevents[sector][module][mbo]++;
	      if(phist==1)
		{
		  histtime1b[sector][module][mbo]->Fill(time1);
		  histtime2b[sector][module][mbo]->Fill(time2);
		}
	      
	      if(time1>paramsbitcorrection[sector][module][mbo][2])
		{
		  if(prawcor==1)
		    {
		      rawcor->setTimeShift(paramsbitcorrection[sector][module][mbo][4],1);
		      rawcor->setFlagRaw(1);
		    }
		  mdcraw->setTimeNew(time1+paramsbitcorrection[sector][module][mbo][4],1);
		  if(phist==1)
		    {
		      histtime1g[sector][module][mbo]->Fill(time1+paramsbitcorrection[sector][module][mbo][4]);
		    }
		  countersavedtime1[sector][module][mbo]++;
		}
	      else
		{
		  if(time1<paramsbitcorrection[sector][module][mbo][1])
		    {
		      if(prawcor==1)
			{
			  rawcor->setTimeShift(paramsbitcorrection[sector][module][mbo][3],1);
			  rawcor->setFlagRaw(1);
			}
		      mdcraw->setTimeNew(time1+paramsbitcorrection[sector][module][mbo][3],1);
		      if(phist==1)
			{
			  histtime1g[sector][module][mbo]->Fill(time1+paramsbitcorrection[sector][module][mbo][3]);
			}
		      countersavedtime1[sector][module][mbo]++;
		    }
		  else
		    {
		       if(prawcor==1)
			 {
			   rawcor->setTimeShift(0,1);
			   rawcor->setFlagRaw(0);
			 }
		      if(phist==1)
			{
			  histtime1g[sector][module][mbo]->Fill(time1);
			}
		    }
		}
	      
	       
	      if(time2>paramsbitcorrection[sector][module][mbo][6])
		{
		   if(prawcor==1)
		    {
		      rawcor->setTimeShift(paramsbitcorrection[sector][module][mbo][8],2);
		      if(rawcor->getFlagRaw()>-1)
			{
			  rawcor->setFlagRaw(3);
			}
		      else
			{
			  rawcor->setFlagRaw(2);
			}
		    }
		  mdcraw->setTimeNew(time2+paramsbitcorrection[sector][module][mbo][8],2);

		  if(phist==1)
		    {
		      histtime2g[sector][module][mbo]->Fill(time2+paramsbitcorrection[sector][module][mbo][8]);
		    }
		  countersavedtime2[sector][module][mbo]++;
		}
	      else
		{
		  if(time2<paramsbitcorrection[sector][module][mbo][5])
		    {
		       if(prawcor==1)
			 {
			   rawcor->setTimeShift(paramsbitcorrection[sector][module][mbo][7],2);
			   if(rawcor->getFlagRaw()>-1)
			     {
			       rawcor->setFlagRaw(3);
			     }
			   else
			     {
			       rawcor->setFlagRaw(2);
			     }
			 }
		      mdcraw->setTimeNew(time2+paramsbitcorrection[sector][module][mbo][7],2);
		      if(phist==1)
			{
			  histtime2g[sector][module][mbo]->Fill(time2+paramsbitcorrection[sector][module][mbo][7]);
			}
		      countersavedtime2[sector][module][mbo]++;
		    }
		  else
		    {
		       if(prawcor==1)
			 {
			   rawcor->setTimeShift(0,2);
			   rawcor->setFlagRaw(0);
			 }
		      if(phist==1)
			{
			  histtime2g[sector][module][mbo]->Fill(time2);
			}
		    }
		}
	    }
	  else
	    {
	       if(prawcor==1)
		    {
		      rawcor->setFlagRaw(0);
		      rawcor->setTimeShift(0,1);
		      rawcor->setTimeShift(0,2);
		    }
	    }
	  
	}
    }

  return 0;
  
}


Bool_t HMdcBitFlipCor::finalize(void) {
  if(phist==1)
    {
      asciioutputfile = fopen(asciioutputfilename.Data(),"w");
    }
     
  for(Int_t s=0;s<6;s++)
    {
      for(Int_t m=0; m<4; m++)
	{
	  for(Int_t mbo=0; mbo<16; mbo++)
	    {
	      if(counterevents[s][m][mbo]!=0)
		{
		  cout << " sector " << s << " module " << m << " mbo " << mbo << endl;
		  cout << " number of events " << counterevents[s][m][mbo] << endl;
		  cout << " number of events which were rescued due to time1 correction " << countersavedtime1[s][m][mbo] << endl;
		  cout << " number of events which were rescued due to time2 correction " << countersavedtime2[s][m][mbo] << endl;
		  Float_t savedevents1=(Float_t) countersavedtime1[s][m][mbo]/counterevents[s][m][mbo];
		  Float_t savedevents2=(Float_t) countersavedtime2[s][m][mbo]/counterevents[s][m][mbo];
		  cout << " % of rescued time1 events " << savedevents1*100  << endl;
		  cout << " % of rescued time2 events " << savedevents2*100  << endl;
	
		  if(phist==1)
		    {
		      fprintf(asciioutputfile,"%s%i%s%i%s%i%s%i%s%i%s%i\n","sector ",s," module ",m," mbo ", mbo," no of events " , counterevents[s][m][mbo], " saved time1 " , countersavedtime1[s][m][mbo], " saved time2 ", countersavedtime2[s][m][mbo]);
		    }
		}
	    }
	}
    }

  if(phist==1)
    {
      fclose(asciioutputfile);
      
      
      
      rootoutputfile=new TFile(rootoutputfilename.Data(),"RECREATE");
      rootoutputfile->cd();


      for(Int_t s=0;s<6;s++)
	{
	  for(Int_t m=0; m<4; m++)
	    {
	      for(Int_t mbo=0; mbo<16; mbo++)
		{
		  if((histtime1b[s][m][mbo]->GetEntries())>0)
		    {
		      histtime1b[s][m][mbo]->Write();
		    }
		  if(histtime1g[s][m][mbo]->GetEntries()>0)
		    {
		      histtime1g[s][m][mbo]->Write();
		    }
		  if(histtime2b[s][m][mbo]->GetEntries()>0)
		    {
		      histtime2b[s][m][mbo]->Write();
		    }
		  if(histtime2g[s][m][mbo]->GetEntries()>0)
		    {
		      histtime2g[s][m][mbo]->Write();
		    }
		}
	    }
	}
      
      rootoutputfile->Save();
      rootoutputfile->Close();
    }

  return kTRUE;

}


