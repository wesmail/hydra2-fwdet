//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
// HPTools
// Hydra Processing Tools
//
//            Author: Peter W.Zumbruch
//           Contact: P.Zumbruch@gsi.de
//           Created: Mar 21, 2002
// Last modification: Oct 08, 2005
//
// File: $RCSfile: hptools.cc,v $
// Version: $Revision: 1.41 $
// Modified by $Author: wuestenf $ on $Date: 2008-09-23 13:21:45 $
////////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <unistd.h>

#include "TApplication.h"
#include "TCanvas.h"
#include "TError.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THStack.h"
#include "TLatex.h"
#include "TMath.h"
#include "TObject.h"
#include "TPaveText.h"
#include "TPad.h"

#ifdef RFIO_SUPPORT
//#include "TRFIOFile.h"
#include "htrfiofile.h"
#include "hrfiorootsource.h"
#include "hldrfiofilesource.h"
#endif

#include "TROOT.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TString.h"
#include "TStyle.h"
#include "TTree.h"
#include "TUnixSystem.h"

#include "hades.h"
#include "haddef.h"
#include "hcategory.h"
#include "hevent.h"
#include "hldfilesource.h"
#include "hmdcdetector.h"
#include "hrootsource.h"
#include "hspectrometer.h"

#ifdef ORACLE_SUPPORT
  #include "hparora2io.h"
#endif

#include "hptools.h"

ClassImp(HPTools)


HPTools::HPTools()
{
    ;
}

HPTools::~HPTools()
{
    ;
}

Int_t HPTools::addSourceList(HDataSource *source, TString inputFile, TString inDir, Int_t refid)
{
  // expects either a HRootSource (inheritant) or HldFileSource (inheritant) as source
  // adds to this given HDataSource source:
  //
  //   (a) inputFile ends with ".root or .hld": the root/hld inputfile
  //       to be found in inDir with referenceId refId (default:-1)
  //
  //   (b) inputFile ends with ".txt": the list of root/hldfiles contained in inputFile
  //       to be found in inDir with referenceId refId (default:-1)
  //
  // if inputFile is not valid exit with exit(EXIT_FAILURE);
  //
  // in case of an HRootSource it is check whether a Tree is contained
  //  (no)  exits with with exit(EXIT_FAILURE);
  //  (yes) adds all entries together
  //
  // return values:
  // RootSource: number of Events (default:-1)
  // HldSource: (default:-1)
  //
  // inputfile example:
  // # comment line
  // file1.root
  // #file2.root
  // file3.root

  Int_t nEvents = -1;
  if (inputFile.EndsWith(".txt"))
    {
      ifstream infile;
      infile.open((inDir+"/"+inputFile).Data());
      if (!infile)
	{
	  ::Error("addSourceList","could not open \"%s\" ... exiting", (inDir+"/"+inputFile).Data());
	  exit(EXIT_FAILURE);
	}

      Int_t ctr=0;
      while(!infile.eof())
	{
	  Char_t line[200]="#";
	  Char_t name[200]="#";

	  infile.getline(line,200);
	  sscanf(line,"%s",name);
	  if (strncmp(name,"#",1)==0) continue;
	  if (strncmp(name," ",1)==0) continue;
	  if (strncmp(name,"",1)==0) continue;
	  if (source->InheritsFrom("HRootSource"))
	    {
	      if (((HRootSource*)source)->addFile((Text_t *)name))
		{
		  ctr++;
		  cout << ctr << ":" << flush;
		  Info("addSourceList()"," added file %s successfully to list of source files",name);
		}
	      else
		{
		  ::Warning("addSourceList","could not open \"%s\" ... exiting", name);
		}
	    }
#ifdef RFIO_SUPPORT
	  else
	    {// needed up to hydra v6_14, where HRFIORootSource wasn't directly inherited from HRootSource
	      if (source->InheritsFrom("HRFIORootSource"))
		{
		  if (((HRFIORootSource*)source)->addFile((Text_t *)name))
		    {
		      ctr++;
		      cout << ctr << ":" << flush;
		      Info("addSourceList()"," added file %s successfully to list of source files",name);
		    }
		  else
		    {
		      ::Warning("addSourceList","could not open \"%s\" ... exiting", name);
		    }
		}
	    }
#endif
	  if (source->InheritsFrom("HldFileSource"))
	    {
	      TString a = name;
	      if ((!source->InheritsFrom("HldRFIOFileSource")) && a.CompareTo(gSystem->BaseName(name)) == 0)
		{
		  ((HldFileSource*)source)->setDirectory(".");
		}
	      cout << "name: \"" << name << "\"" << endl;
	      ((HldFileSource*)source)->addFile((Text_t *)name,refid);
	      ctr++;
	      cout << ctr << ":" << flush;
	      Info("addSourceList()"," added file %s to list of source files",name);
	    }
	}
      infile.close();
    }
  else
    {
      if (source->InheritsFrom("HRootSource"))
	{
	  ((HRootSource*)source)->setDirectory((Text_t *)inDir.Data());
	  if (((HRootSource*)source)->addFile((Text_t *)inputFile.Data()))
	    {
	      Info("addSourceList()"," added file %s successfully to list of source files",inputFile.Data());
	    }
	  else
	    {
	      ::Error("addSourceList","could not open \"%s\" ... exiting", (inDir+"/"+inputFile).Data());
	      exit(EXIT_FAILURE);
	    }
	}
#ifdef RFIO_SUPPORT
      else
	{// needed up to hydra v6_14, where HRFIORootSource wasn't directly inherited from HRootSource
	  if (source->InheritsFrom("HRFIORootSource"))
	    {
	      ((HRFIORootSource*)source)->setDirectory((Text_t *)inDir.Data());
	      if (((HRFIORootSource*)source)->addFile((Text_t *)inputFile.Data()))
		{
		  Info("addSourceList()"," added file %s successfully to list of source files",inputFile.Data());
		}
	    }
	}
#endif
      if (source->InheritsFrom("HldFileSource"))
	{
	  ((HldFileSource*)source)->setDirectory((Text_t *)inDir.Data());
	  ((HldFileSource*)source)->addFile((Text_t *)inputFile.Data(),refid);
	  Info("addSourceList()"," added file %s to list of source files",inputFile.Data());
	}
    }

  if (source->InheritsFrom("HRootSource"))
    {
      Info("addSourceList()"," summing up entries of all inputfiles ...");
      if(((HRootSource*)source)->getTree())
	{
	  nEvents = (Int_t) (((HRootSource*)source)->getTree()->GetEntries());
	  Info("addSourceList()","In total: %i events are available.",nEvents);
	}
      else
	{
	  Warning("addSourceList()","no Tree in File ! ... makes no sense to continue ... exiting!");
	  exit(EXIT_FAILURE);
	}
    }

#ifdef RFIO_SUPPORT
  if (source->InheritsFrom("HRFIORootSource"))
    {
      Info("addSourceList()"," summing up entries of all inputfiles ...");
      if(((HRFIORootSource*)source)->getTree())
	{
	  nEvents = (Int_t) (((HRFIORootSource*)source)->getTree()->GetEntries());
	  Info("addSourceList()","In total: %i events are available.",nEvents);
	}
      else
	{
	  Warning("addSourceList()","no Tree in File ! ... makes no sense to continue ... exiting!");
	  exit(EXIT_FAILURE);
	}
    }
#endif

  return nEvents;
}

// -----------------------------------------------------------------------------------------

void HPTools::geantPidAxis(TH1 *hist, TString axis)
{
  // replaces for a given histogramm hist with geant pid numbers on axis
  // the numbers by the pid name
  axis.ToLower();

  TAxis *ax = 0;
  if (axis.Contains("x"))
    {
      ax = hist->GetXaxis();
      plotGeantAxis(ax);
      ax->LabelsOption("V");
    }
  if (axis.Contains("y"))
    {
      ax = hist->GetYaxis();
      plotGeantAxis(ax);
      ax->LabelsOption("H");
    }
}

// -----------------------------------------------------------------------------------------

void HPTools::plotGeantAxis(TAxis *axis)
{
 // replaces for a the active histogramm with geant pid numbers on axis
 // the numbers by the pid name

 if (axis)
   {
     Int_t nbinsx = axis->GetNbins();

     for (Int_t binx=1;binx<=nbinsx;binx++)
       {
	 Double_t width = axis->GetBinWidth(binx);
	 if (width < 1.0 || width > 1.0)
	   {
	     Warning("geantPidAxis()"," bin width not equal 1");
	     return;
	   }
       }

     if (axis->GetBinLowEdge(axis->GetLast()) < 0 || (axis->GetBinLowEdge(axis->GetFirst())+1 > 50) )
       {
	 Warning("geantPidAxis()"," labels not in the range of the histogram");
	 return;
       }

     for (Int_t binx=1;binx<=nbinsx;binx++)
       {
	 if ((axis->GetBinLowEdge(binx))<0) continue;
	 if ((axis->GetBinLowEdge(binx))>50) break;

	 Int_t lowEdge = (Int_t) axis->GetBinLowEdge(binx);
	 if (lowEdge > 0) axis->SetBinLabel(binx,(getGeantPidNameLatex(lowEdge)).Data());
       }
   }
}

// -----------------------------------------------------------------------------------------

void HPTools::geantProcessAxis(TH1 *hist, TString axis)
{
  // replaces for a given histogramm hist with geant process numbers on axis
  // the numbers by the process name

  axis.ToLower();

  TAxis *ax = 0;
  if (axis.Contains("x"))
    {
      ax = hist->GetXaxis();
      plotGeantProcessAxis(ax);
      ax->LabelsOption("V");
    }
  if (axis.Contains("y"))
    {
      ax = hist->GetYaxis();
      plotGeantProcessAxis(ax);
      ax->LabelsOption("H");
    }
}


// -----------------------------------------------------------------------------------------

void HPTools::plotGeantProcessAxis(TAxis *axis)
{
  // replaces for the active histogramm hist with geant process numbers on axis
  // the numbers by the process name

  if (axis)
   {
     Int_t nbinsx = axis->GetNbins();

     for (Int_t binx=1;binx<=nbinsx;binx++)
       {
	 Double_t width = axis->GetBinWidth(binx);
	 if (width < 1.0 || width > 1.0)
	   {
	     Warning("geantProcessAxis()"," bin width not equal 1");
	     return;
	   }
       }

     if (axis->GetBinLowEdge(axis->GetLast()) < 0 || (axis->GetBinLowEdge(axis->GetFirst())+1 > 109) )
       {
	 Warning("geantProcessAxis()"," labels not in the range of the histogram");
	 return;
       }

     for (Int_t binx=1;binx<=nbinsx;binx++)
       {
	 if ((axis->GetBinLowEdge(binx))<0) continue;
	 if ((axis->GetBinLowEdge(binx))>109) break;

	 Int_t lowEdge = (Int_t) axis->GetBinLowEdge(binx);
	 if (lowEdge > 0) axis->SetBinLabel(binx,(getGeantProcessNameLatex(lowEdge)).Data());
       }
   }
}

// -----------------------------------------------------------------------------------------

void HPTools::plotPolarSectorGrid()
{
  // plots onto a polar plot a grid corresponding to the HADES orientation
  TLine *lDeg[6];
  TLine *lDeg2[6];
  TString label[6]={"180#circ","120#circ","60#circ","0#circ","300#circ","240#circ"};
  TLatex * tex[6];

  for (Int_t iter = 0; iter < 6; iter++)
    {
      Float_t radius = 0.65;
      lDeg[iter] = new TLine(0,0,radius*cos(iter*60.*acos(-1.)/180.),radius*sin(iter*60.*acos(-1.)/180.));
      //lDeg[iter]->SetLineStyle(2);
      lDeg[iter]->SetLineWidth(2);
      lDeg[iter]->SetLineColor(10);
      lDeg[iter]->Draw();
      lDeg2[iter] = new TLine(0,0,radius*cos(iter*60.*acos(-1.)/180.),radius*sin(iter*60.*acos(-1.)/180.));
      //      lDeg2[iter]->SetLineStyle(2);
      lDeg2[iter]->SetLineWidth(1);
      lDeg2[iter]->SetLineColor(1);
      lDeg2[iter]->Draw();

      radius+=0.07;
      tex[iter]  = new TLatex(radius*cos(iter*60.*acos(-1.)/180.),radius*sin(iter*60.*acos(-1.)/180.),label[iter].Data());
      tex[iter]->Draw();
    }
}

// -----------------------------------------------------------------------------------------

Double_t HPTools::ptinv(Double_t pt,Double_t theta, Double_t mass)
{
  // function returning the relation of pt to theta for a given mass
  Double_t result = 0.;
  Double_t ratio = 0.;
  Double_t denom2=0.;
  if (pt!=0)
    {
      ratio = sin(theta)*mass/pt;
      denom2 = (ratio*ratio)+1.;
      if (denom2!=0) result = sqrt((cos(theta)*cos(theta))/denom2);
    }

  ratio = ((1.-result)!=0.)?(1.+result)/(1.-result):0.;

  if (ratio==1.) return -100.;
  else
    {
      if (ratio==0.) return -100.;
      else
	{
	  return 0.5*log(ratio);
	}
    }
}

// -----------------------------------------------------------------------------------------

Double_t HPTools::PZf_pty1(Double_t *x, Double_t *par)
{
  Double_t result=0.;
  Double_t mass = par[0];
  Double_t y=x[0];
  //  Double_t v=TMath::Exp(2*y);
  Double_t dummy = 0;

  switch ((int)par[2])
    {
    case 0:
      {
	Double_t theta = par[1];
	Double_t nom    = mass*sin(theta);
	Double_t ratio ;
	ratio = (tanh(y)==0)?1:cos(theta)/tanh(y);
	Double_t denom2 = (ratio*ratio)-1;
	{
	  result = (sqrt(denom2)>0)?nom/sqrt(denom2):1e6;
	}
      }
      break;
    case 1:
      {
	result = 0;
	Double_t p = par[1];
	dummy = p*p - (p*p+mass*mass)*tanh(y)*tanh(y);
	if (dummy>=0) result = sqrt(dummy);
      }
      break;
    case 2:
      {
	// invers of case 0
	Double_t pt=x[0];
	Double_t theta = par[1];
	if (pt!=0)
	  {
	    Double_t ratio = sin(theta)*mass/pt;
	    Double_t denom2 = (ratio*ratio)+1;
	    Double_t nom2 = cos(theta)*cos(theta);
	    result = (denom2>0)?sqrt(nom2/denom2):0;
	  }
      }
	default:
      result=0.0;
    }
  return result;
}

// --------------------------------------------------------------------------------

void HPTools::plot_pty_PZ(Double_t mass)
{
  // plotting grid of constant lab momentum and constant angles for a given mass
  // over the active histogramm assuming that it represents a pty distribution

  Double_t pi = TMath::Pi();
  Double_t ylim[2];
  ylim[0]=0;
  ylim[1]=3;
  Double_t th, p;
  TF1 *f=new TF1("f",PZf_pty1,ylim[0],ylim[1],3);
  f->SetLineWidth(2);
  f->SetLineColor(16);

  for (Float_t myiter=0; myiter<90; myiter+=5){ // feste labor winkel
    th = myiter/180.0*pi; // winkel ist laufvariable

    TString angletext="#circ";
    TString angle="";
    angle += myiter;
    angletext = angle + angletext;

    Double_t xpos =  ptinv(1600,th,mass);

    TLatex *tex = new TLatex(xpos+0.05,1530,angletext.Data());
    tex->SetTextFont(42);
    tex->SetTextSize(0.025);
    tex->SetTextAngle(90);
    tex->SetLineWidth(2);
    tex->Draw();
  }

  for (Float_t myiter=0; myiter<=90; myiter+=5){ // feste labor winkel
    th = myiter/180.0*pi; // winkel ist laufvariable
    f->SetParameters(mass,th,0);
    f->DrawCopy("same");

  }

  for (Float_t myiter=100; myiter<=6000; myiter+=100){ // feste labor impulse
    p = myiter ; // impuls ist laufvariable
    f->SetParameters(mass,p,1.0);
    f->SetLineColor(42);
    f->DrawCopy("same");
  }

  for (Float_t myiter=0; myiter<=180/2; myiter+=5){ // feste labor winkel
    th = myiter/180.0*pi; // winkel ist laufvariable
    f->SetParameters(mass,th,0);
    f->DrawCopy("same");
  }

}

// --------------------------------------------------------------------------------

void HPTools::printAnalysisFileInfo(Int_t startEvt,TString inputDir,TString outFile, TString outDir)
{
  // depending on the startEvt number
  // the file information used in my analysis scripts is printed to stdout
  // startEvt == -1111: >> "outputName outputFile"
  // startEvt == -2222: >> "outputDir outputDirectory"
  // startEvt == -3333: >> "inputDir inputDirectory"
  // used to get information how the parameters about files are treated and processed before
  // running the macro itself completely

  if (startEvt == -1111)
    {
      printf("outputName %s\n",gSystem->BaseName(outFile.Data()));
      exit(EXIT_SUCCESS);
    }
  if (startEvt == -2222)
    {
      printf("outputDir %s\n",outDir.Data());
      exit(EXIT_SUCCESS);
    }
  if (startEvt == -3333)
    {
      printf("inputDir %s\n",inputDir.Data());
      exit(EXIT_SUCCESS);
    }
}

// --------------------------------------------------------------------------------

Bool_t HPTools::mdcDetectorSetup(TString beamtime)
{
  // function to add mdc detector to hades detector
  // valid beamtime names are:
  //                           nov01
  //                           nov02
  //                           sep03
  //                           jan04
  //                           aug04
  //                           jan05
  //                           sep05
  //                           apr06
  //                           may06
  //                           apr07
	//                           oct07

  static Int_t nov01_mdcMods[6][4] = { {0,1,1,1},
				       {1,1,0,0},
				       {1,1,0,0},
				       {1,1,1,0},
				       {1,1,0,0},
				       {1,1,0,0} };

  static Int_t nov02_mdcMods[6][4] = { {1,1,1,1},
				       {1,1,0,0},
				       {1,1,1,0},
				       {1,1,1,1},
				       {1,1,0,0},
				       {1,1,1,0} };

  static Int_t sep03_mdcMods[6][4] = { {1,1,1,1},
				       {1,1,1,1},
				       {1,1,1,0},
				       {1,1,1,1},
				       {1,1,1,1},
				       {1,1,1,0} };

  static Int_t apr06_mdcMods[6][4] = { {1,1,0,1},
				       {1,1,0,1},
				       {1,1,1,1},
				       {1,1,1,1},
				       {1,1,1,1},
				       {1,1,1,0} };

	static Int_t apr07_mdcMods[6][4] = { {1,1,1,1},
				       {1,1,1,1},
				       {1,1,1,1},
				       {1,1,1,1},
				       {1,1,1,1},
				       {1,1,1,1} };

	static Int_t sep08_mdcMods[6][4] = { {1,1,1,1},
				       {1,1,1,1},
				       {1,1,0,1},
				       {1,1,1,1},
				       {1,1,1,1},
				       {1,1,1,1} };
	

  beamtime.ToLower();
  if (beamtime.CompareTo("nov01")==0)
    {
      return mdcDetectorSetup(nov01_mdcMods);
    }
  if (beamtime.CompareTo("nov02")==0)
    {
      return mdcDetectorSetup(nov02_mdcMods);
    }
  if (beamtime.CompareTo("sep03")==0)
    {
      return mdcDetectorSetup(sep03_mdcMods);
    }
  if (beamtime.CompareTo("jan04")==0)
    {
      return mdcDetectorSetup(sep03_mdcMods); // sep is also used in jan04
    }
  if (beamtime.CompareTo("aug04")==0)
    {
      return mdcDetectorSetup(sep03_mdcMods); // sep is also used in jan04
    }
  if (beamtime.CompareTo("jan05")==0)
    {
      return mdcDetectorSetup(sep03_mdcMods); // sep is also used in jan05
    }
  if (beamtime.CompareTo("sep05")==0)
    {
      return mdcDetectorSetup(sep03_mdcMods); // sep is also used in sep05
    }
  if (beamtime.CompareTo("apr06")==0)
  {
      return mdcDetectorSetup(apr06_mdcMods);
  }
  if (beamtime.CompareTo("may06")==0)
  {
      return mdcDetectorSetup(apr06_mdcMods);
  }
  if (beamtime.CompareTo("apr07")==0)
  {
      return mdcDetectorSetup(apr07_mdcMods);
  }
  if (beamtime.CompareTo("oct07")==0)
  {
      return mdcDetectorSetup(apr07_mdcMods);
  }
	if (beamtime.CompareTo("sep08")==0)
	{
		return mdcDetectorSetup(sep08_mdcMods);
	}
	::Error("mdcDetectorSetup","setting for \"%s\" not available.",beamtime.Data());

  return kFALSE;
}

// --------------------------------------------------------------------------------

Bool_t HPTools::mdcDetectorSetup(Int_t (*mdcMods)[4])
{
  // sets mdc Detector setup with the 6*4 matrix given via mdcMods
  //  i.e. sep03_mdcMods[6][4] = { {1,1,1,1},
  //                               {1,1,1,1},
  // 			           {1,1,1,0},
  // 				   {1,1,1,1},
  // 				   {1,1,1,1},
  // 				   {1,1,1,0} };

  Hades *myHades = gHades;
  if (!myHades)
    {
      ::Error("mdcDetectorSetup","Hades instance does not exist ...!");
      return kFALSE;
    }

  HSpectrometer* spec = myHades->getSetup();

  if (!spec  )
    {
      ::Error("mdcDetectorSetup","spectrometer instance does not exist ...!");
      return kFALSE;
    }

  HMdcDetector *mdc = new HMdcDetector;

  if (! spec->getDetector(mdc->GetName()))
    {
      spec->addDetector(mdc);
    }
  for (Int_t is=0; is<6; is++)
    {
      spec->getDetector(mdc->GetName())   ->setModules(is,mdcMods[is]);
    }
  return kTRUE;
}
// --------------------------------------------------------------------------------

void HPTools::printCounter(Int_t eventiter,Int_t nEvents, Float_t stepsize)
{
  // print a counter message if eventiter is x = eventiter*stepsize %
  // where stepsize is given in percentage
  // default stepsize 5% = 0.05

  Int_t counterstep;
  if (nEvents > 1000000) stepsize/=10;
  counterstep=(int)(nEvents*stepsize);
  if (counterstep==0) counterstep = 1;

  if(eventiter%10000==0) cout<<"."<< flush;
  if(eventiter%100000==0) cout<<"|"<< flush;
  if(eventiter%counterstep==0)
    {
      cout << endl;
      gSystem->Exec("echo -n $(date +%H:%M:%S)\" \"");
      if (counterstep == 1)
        {
          cout <<eventiter<< " Events processed: " << (Double_t)(100.*eventiter/nEvents) << "%\t" << endl;
        }
      else
        {
          cout <<eventiter<< " Events processed: " << ((int)(eventiter/counterstep)*stepsize*100) << "%\t" << endl;
        }
    }
}

// --------------------------------------------------------------------------------
Bool_t HPTools::hadesInit()
{
  // calls hades::init and checks the output
  // if it has failed the hades object is deleted and the function exits with exit_code -1
  // returns kFALSE if init has failed
  // kTRUE otherwise
  if (gHades)
    {
      printf("gHades->init()\n");
      if (!gHades->init())
	{
	  Error("hadesInit()","failed ... exiting!");
	  Info("hadesInit()","calling delete of hades object");
	  delete gHades;
	  Info("hadesInit()","finished");

	  exit(-1);
	  return kFALSE;
	}
      return kTRUE;
    }
  else
    {
      Error("hadesInit()","no instance of hades ... exiting!");
      exit(-1);
      return kFALSE;
    }
}

// --------------------------------------------------------------------------------

Int_t HPTools::hadesEventLoop(Int_t nEvents, Int_t startEvt)
{
  // calls hades event loop
  // generates info output
  // returns number of processed events

  if (gHades)
    {
      Int_t evN;
      if (nEvents<1)
	{
	  printf("Processing events...\n");
	  evN=gHades->eventLoop();
	}
      else
	{
	  if (startEvt<=0) {printf("Processing %i event(s)...\n",nEvents);}
	  else printf("Processing %i event(s) starting from event %i ...\n",nEvents,startEvt);
	  evN=gHades->eventLoop(nEvents,startEvt);
	}
      return evN;
    }
  else
    {
      Error("hadesInit()","no instance of hades ... exiting!");
      exit(-1);
      return 0;
    }
}

// --------------------------------------------------------------------------------

Bool_t HPTools::usageFileInDirOutDirNevtStart(const Char_t *name)
{
  // prints the usage for an executable: name
  // of the following syntax
  //    executable inputfile inDir outputDir [number of events [start evt]]
  //
  // returns always kTRUE;

  Info(gSystem->BaseName(name),"Usage:");
#ifdef __CINT__
  TString es=".cc(";
  TString sp=", ";
  TString e=")";
#else
  TString es=" ";
  TString sp=" ";
  TString e=" ";
#endif
  Info("usage",
       "           %s%sinputfile.root     %sinDir         %soutputDir [%snumber of events [%sstart evt]]%s"
       ,gSystem->BaseName(name),es.Data(),sp.Data(),sp.Data(),sp.Data(),sp.Data(),e.Data());
  Info("usage",
       "           %s%sinputfile-list.txt %sinDir_of_list %soutputDir [%snumber of events [%sstart evt]]%s"
       ,gSystem->BaseName(name),es.Data(),sp.Data(),sp.Data(),sp.Data(),sp.Data(),e.Data());
  Info("usage","          get output file name: %s%sinputfile%sOUTPUTNAME%s",
       gSystem->BaseName(name),es.Data(),sp.Data(),e.Data());
  Info("usage","          get output dir name : %s%sinputfile%sOUTPUTDIR%s",
       gSystem->BaseName(name),es.Data(),sp.Data(),e.Data());
  Info("usage","          get input  dir name : %s%sinputfile%sINPUTDIR%s",
       gSystem->BaseName(name),es.Data(),sp.Data(),e.Data());
  Info("usage","          get executable path : %s%sinputfile%sEXECUTABLE%s",
       gSystem->BaseName(name),es.Data(),sp.Data(),e.Data());

  return EXIT_SUCCESS;
}


// --------------------------------------------------------------------------------

Int_t HPTools::mainFileInDirOutDirNevtStar(Int_t argc, Char_t **argv,
					 Int_t (*function) (TString outDir,
							  TString inputDir,
							  TString inputFile,
							  Int_t nEvents,
							  Int_t startEvt))
{
  // main function part for compiled macros
  // the macro function must have the syntax
  //
  // Int_t function(TString outDir, TString inputDir, TString inputFile, Int_t nEvents=0, Int_t startEvt=0)
  // the function should also contain a call to the function HPTools::printAnalysisFileInfo
  //
  // passing this its function pointer (i.e. just its name without anything else)
  // together with argc and argv from the main function
  // to this function
  // delivers you the following behavior
  //
  //     (suppose the compiled executable's name is executable)
  //
  //      executable accepts the following usage:
  //
  //      executable inputfile inDir outputDir [number of events [start evt]]
  //      in addition if either 'number of events' or 'start evt' is set to
  //      'OUTPUTNAME', 'OUTPUTDIR', 'INPUTDIR', 'EXECUTABLE' (case insensitive)
  //      the corresponding information is printed to stdout
  //
  //
  // Behaviour
  //
  // (0) in any (yet known) case the return value is EXIT_SUCCESS
  //
  // (1) passing less than 3 or more than 5 arguments to the function results in a
  //     call to HPTools::usageFileInDirOutDirNevtStart
  //     to explain right behavior
  //
  // (1a) if called with option -h HPTools::usageFileInDirOutDirNevtStart will be called (help)
  //
  // (2) function is called corresponding to aboves definition of executable
  //
  // (3) if either 'number of events' or 'start evt' is set to
  //     'OUTPUTNAME', 'OUTPUTDIR', 'INPUTDIR', 'EXECUTABLE' (case insensitive)
  //     the corresponding information is printed to stdout
  //     by calling the function with predefined negative 'start evt' values
  //     making usage of HPTools::printAnalysisFileInfo
  //

  TROOT MacroFileInDirOutDirNevtStart("Macro","compiled macro(TString outDir, TString inputDir, TString inputFile, Int_t nEvents=0, Int_t startEvt=0)");
  if (getopt(argc,argv,"h")>=0) {return usageFileInDirOutDirNevtStart(gSystem->BaseName(argv[0]));}

  switch (argc)
    {
    case 1:
      HPTools::usageFileInDirOutDirNevtStart(argv[0]);
      return EXIT_SUCCESS;
      break;
    case 2:
      HPTools::usageFileInDirOutDirNevtStart(argv[0]);
      return EXIT_SUCCESS;
      break;
    case 3:
      HPTools::usageFileInDirOutDirNevtStart(argv[0]);
      return EXIT_SUCCESS;
      break;
    case 4:
      function(TString(argv[3]),TString(argv[2]),TString(argv[1]),0,0);
      return EXIT_SUCCESS;
      break;
    case 5:
      if (atoi(argv[4])!=0)
        {
          return function(TString(argv[3]),TString(argv[2]),TString(argv[1]),atoi(argv[4]),0);
        }
      else
        {
          TString name = argv[4];
          name.ToUpper();
          if (name.CompareTo("OUTPUTNAME")==0)
            {
              return function(TString(argv[3]),TString(argv[2]),TString(argv[1]),-1111,-1111);
            }
          if (name.CompareTo("OUTPUTDIR")==0)
            {
              return function(TString(argv[3]),TString(argv[2]),TString(argv[1]),-2222,-2222);
            }
          if (name.CompareTo("INPUTDIR")==0)
            {
              return function(TString(argv[3]),TString(argv[2]),TString(argv[1]),-3333,-3333);
            }
          if (name.CompareTo("EXECUTABLE")==0)
            {
              printf("executable %s\n",gSystem->BaseName(argv[0]));
              return EXIT_SUCCESS;
            }
          return HPTools::usageFileInDirOutDirNevtStart(argv[0]);
        }
      return EXIT_SUCCESS;
      break;
    case 6:
      if (atoi(argv[5])!=0)
        {
	  function(TString(argv[3]),TString(argv[2]),TString(argv[1]),atoi(argv[4]),atoi(argv[5]));
	  return EXIT_SUCCESS;
        }
      else
        {
          TString name = argv[5];
          name.ToUpper();
          if (name.CompareTo("OUTPUTNAME")==0)
            {
              return function(TString(argv[3]),TString(argv[2]),TString(argv[1]),atoi(argv[4]),-1111);
            }
          if (name.CompareTo("OUTPUTDIR")==0)
            {
              return function(TString(argv[3]),TString(argv[2]),TString(argv[1]),atoi(argv[4]),-2222);
            }
          if (name.CompareTo("INPUTDIR")==0)
            {
              return function(TString(argv[3]),TString(argv[2]),TString(argv[1]),atoi(argv[4]),-3333);
            }
          if (name.CompareTo("EXECUTABLE")==0)
            {
              printf("executable %s\n",gSystem->BaseName(argv[0]));
              return EXIT_SUCCESS;
            }
          return HPTools::usageFileInDirOutDirNevtStart(argv[0]);
        }
      return EXIT_SUCCESS;
      break;
    default:
      HPTools::usageFileInDirOutDirNevtStart(argv[0]);
      return EXIT_SUCCESS;
    }
}

// --------------------------------------------------------------------------------
#ifdef ORACLE_SUPPORT
HParOra2Io* HPTools::getOracleParIo(Bool_t exitOnFailure)
{
  // opens HParOraIo and checks if connection is existing
  // if so the pointer to HParOraIo is returned
  // if not NULL is returned
  // if exitOnFailure is kTRUE then function exits

     HParOra2Io *ora=new HParOra2Io() ;
     ora->open();
     if (!ora->check())
       {
	 delete ora;
	 if(exitOnFailure)
	   {
	     Error("ora->check","no connection to Oracle\nexiting...!");
	     exit(EXIT_FAILURE);
	   }
	 else
	   {
	     Warning("ora->check","no connection to Oracle\n but continuing...!");
	     return NULL;
	   }
       }
     return ora;
}
#endif
// --------------------------------------------------------------------------------

void HPTools::setPersistency(Bool_t b, Cat_t *persistentArray, UInt_t nPersistent, UInt_t clean)
{
  // sets the persistency for the categories given in persistentArray[nPersistent]
  // to value of b
  // if clean > 0:
  // 1. setting the first 'clean' categories' persistency to the inverse of b
  // 2. sets the categories listet in the array to b
  // if not only item 2 is executed

  if (!gHades)
    {
      Error("setPersistency()","no instance of hades ... exiting!");
      exit(-1);
    }

  HEvent *event = gHades->getCurrentEvent();

  if (!event)
    {
      Error("setPersistency()","no event existing ... exiting!");
      exit(-1);
    }

  for(UInt_t i=0;i<clean;i++)
    {
      HCategory *categ = event->getCategory((Cat_t) i);
      if (categ) categ->setPersistency(!b);
    }

  for(UInt_t i=0;i<nPersistent;i++)
    {
      HCategory *categ = event->getCategory(persistentArray[i]);
      if (categ) categ->setPersistency((b));
    }
}

// --------------------------------------------------------------------------------

TH1F* HPTools::getPairPidHistogramm(Axis_t min, Axis_t max, Int_t extBins, Double_t extBinning)
{
  // returns a histogram with a variable binning
  // to plot geant pair ids like 8009 14014 ...
  // the axis is labeled correspondingly
  // min and max set a range
  // if extBins is set, extBins with extBinning [default=1] are added

  Double_t *list = new Double_t[49*49+extBins+1];
  Int_t index=-1;
  Double_t val=0.;
  Int_t aMax = 18;
  Int_t bMax = 18;

  for (Int_t a = 0; a<aMax; a++)
    {
      for (Int_t b = 0; b<bMax; b++)
	{
	  if (b<a) {continue;}

	  val = a * 1000. + b;

	  if ((min < max) && (val < min)) continue;
	  if ((min < max) && (val > max)) continue;

	  index++;

	  list[index] = val;
	  //	  cout << list[index] << endl;
	}
    }

  index++;
  list[index] = val+1.;

  for (Int_t bin=0; bin < extBins+1; bin++)
    {
      index++;
      list[index] = list[index-1]+extBinning;
    }

  TH1F *hist = new TH1F("pairPids","pairPids",index, list);
  index--;
  Int_t pid =0;
  for (Int_t listIndex = 0; listIndex <index; listIndex++)
    {
      if (list[listIndex] > (aMax*1000+bMax)) {continue;}
      pid = (int)((list[listIndex]-(((int)(list[listIndex]))%1000))/1000);
      //      cout << list[listIndex] << " " << pid << " ";
      TString a = getGeantPidNameLatex(pid);
      if (a.IsNull()) { a = "_";}
      pid = ((int)(list[listIndex]))%1000;
      //      cout << pid << " " ;
      TString b = getGeantPidNameLatex(pid);
      if (b.IsNull()) { b = "_";}

      TString c = a + " " + b;
      //      cout << c.Data() << ": " << hist->GetXaxis()->FindBin(list[listIndex]) << endl;

      hist->GetXaxis()->SetBinLabel(hist->GetXaxis()->FindBin(list[listIndex]),c.Data());
    }
  return hist;
}

// --------------------------------------------------------------------------------

TString HPTools::getGeantPidNameLatex(Int_t index)
{
  // provides for geant pid index corresponding pid name in latex representation

 static const TString pidname[]=
   {
     " ",
     "#gamma",
     "e^{+}",
     "e^{-}",
     "#nu",
     "#mu^{+}",
     "#mu^{-}",
     "#pi^{0}",
     "#pi^{+}",
     "#pi^{-}",
     "K^{0}_{L}",
     "K^{+}",
     "K^{-}",
     "n",
     "p",
     "#bar{p}",
     "K^{0}_{s}",
     "#eta",
     "#Lambda",
     "#Sigma^{+}",
     "#Sigma^{0}",
     "#Sigma^{-}",
     "#Xi^{0}",
     "#Xi^{-}",
     "#Omega^{-}",
     "#bar{n}",
     "#bar{#Lambda}",
     "#bar{#Sigma}^{-}",
     "#bar{#Sigma}^{0}",
     "#bar{#Sigma}^{+}",
     "#bar{#Xi}^{0}",
     "#bar{#Xi}^{+}",
     "#bar{#Omega}^{+}",
     " ",
     " ",
     " ",
     " ",
     " ",
     " ",
     " ",
     " ",
     " ",
     " ",
     " ",
      " ",
     "d",
     "t",
     "#alpha",
     "Geantino",
     "^{3}He",
     "Cerenkov"
   };

 if ((index >= 0) && (index < (int) (sizeof(pidname)/sizeof(TString))))
   {
     return TString(pidname[index]);
   }
 else
   {
     ::Error("getGeantPidNameLatex(Int_t index)","index %i out of range [%i,%lu]",index, 0, sizeof(pidname)/sizeof(TString));
     return TString("false index");
   }
}

// --------------------------------------------------------------------------------

TString HPTools::getGeantProcessNameLatex(Int_t index)
{
 // provides for geant process index corresponding process name in latex representation

 TString procname[]=
    {
      "primary particle ",
      "NEXT: Boundary reached ",
      "multiple scattering ",
      "continous energy loss ",
      "bending in magnetic field ",
      "particle decay ",
      "PAIR ",
      "compton scattering ",
      "photoelectric effect ",
      "bremsstrahlung ",
      "#delta -ray production ",
      "e^{+} annihilation ",
      "hadr. interaction ",
      "hadr. elastic coherent scat. ",
      "nuclear evaporation ",
      "nuclear fission ",
      "nuclear absorption ",
      "#bar{p} annihilation ",
      "neutron capture ",
      "hadr. inelastic coherent scat. ",
      "hadr. inelastic scat. ",
      "#mu -nuclear interaction ",
      "exceeded TOF cut ",
      "nuclear photo fission ",
      "SCUT ",
      "Rayleigh effects ",
      "PARA ",
      "PRED ",
      "",
      "NULL ",
      "STOP ",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "Cherenkov photon absorption ",
      "Cherenkov photon refl/fraction ",
      "SMAX ",
      "SCOR ",
      "Cherenkov photon generation ",
      "Cherenkov photon reflection ",
      "Cherenkov photon refraction ",
      "Syncrotron radiation "
    };

if ((index >= 0) && (index < (int) (sizeof(procname)/sizeof(TString))))
   {
     return TString(procname[index]);
   }
 else
   {
     ::Error("getGeantProcessNameLatex(Int_t index)","index %i out of range [%i,%lu]",index, 0, sizeof(procname)/sizeof(TString));
     return TString("false index");
   }
 return TString("false index");

}

// --------------------------------------------------------------------------------

