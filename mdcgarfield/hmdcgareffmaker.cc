//*-- AUTHOR : J. Markert

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HMdcGarEffMaker
// Class to produce parameters for HMdcCellEff
// The input file is produced by HMdcGarSignalReader and contains the
// the needed Hists to perform the calculations. The final parameter file
// is written to the ascii output and a root output containing the created
// and fitted hists is written too.
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>
#include <iostream> 
#include <iomanip>
#include "hmdcgareffmaker.h"
#include "htool.h"
#include "TString.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TROOT.h"
#include "TKey.h"
#include "TStyle.h"
#include "TMath.h"
#include "TList.h"
#include "TObjString.h"
#include "TF1.h"

Int_t HMdcGarEffMaker::colors[10]={1,2,4,6,8,30,38,28,46,44};

ClassImp(HMdcGarEffMaker)

HMdcGarEffMaker::HMdcGarEffMaker(const Char_t* name,const Char_t* title)
    : TNamed(name,title)
{
    // constructor for HMdcGarEffMaker
    initVariables();
}
HMdcGarEffMaker::~HMdcGarEffMaker()
{
  // destructor of HMdcGarEffMaker
}
void HMdcGarEffMaker::setFileNameAsciiOut(TString myfile)
{
    // Sets root output of HMdcGarEffMaker
    fNameAsciiOut=myfile;
    if(fNameAsciiOut.CompareTo("")==0)
    {
	Error("HMdcGarEffMaker:setFileNameAsciiOut()","NO ASCII OUTPUT FILE SEPCIFIED!");
        exit(1);
    };
    cout<<"HMdcGarEffMaker::setFileNameAsciiOut(): OUTPUT FILE= "<<fNameAsciiOut.Data()<<endl;
}
void HMdcGarEffMaker::setFileNameRootOut(TString myfile)
{
    // Sets root output of HMdcGarEffMaker
    fNameRootOut=myfile;
    if(fNameRootOut.CompareTo("")==0)
    {
	Error("HMdcGarEffMaker:setFileNameRootOut()","NO ROOT OUTPUT FILE SEPCIFIED!");
        exit(1);
    };
    cout<<"HMdcGarEffMaker::setFileNameRootOut() : OUTPUT FILE= "<<fNameRootOut.Data()<<endl;
}
void HMdcGarEffMaker::setFileNameRootIn(TString myfile)
{
    // Sets root input of HMdcGarEffMaker
    fNameRootIn=myfile;
    if(fNameRootIn.CompareTo("")==0)
    {
	Error("HMdcGarEffMaker:setFileNameRootIn() ","NO ROOT INPUT  FILE SEPCIFIED!");
        exit(1);
    };
    cout<<"HMdcGarEffMaker::setFileNameRootIn()  : INPUT  FILE= "<<fNameRootIn.Data()<<endl;
}
void HMdcGarEffMaker::initVariables()
{
    // inits all variables
    fNameAsciiOut     ="";
    outputAscii       =0;
    fNameRootIn       ="";
    inputRoot         =0;
    outputRoot        =0;
    fNameAsciiOut="effmaker.txt";
    fNameRootOut ="effmaker.root";
   //setMinSignal(-60000,-65000,-380000,-530000);
    setMinSignal(- 22583900, - 22762300,-107363000,-159070000);
    resetParams();
    setNSamples(100);
    setNSteps(10);
    setVersion(2);
}
void HMdcGarEffMaker::printStatus(void)
{
    // prints the parameters to the screen
    printf ("HMdcGarEffMaker::printStatus()\n");
    printf ("Version                           :  %i\n",getVersion());
    printf ("AsciiOutput                       :  %s\n",fNameAsciiOut.Data());
    printf ("RootOutput                        :  %s\n",fNameRootOut .Data());
    printf ("RootInput                         :  %s\n",fNameRootIn  .Data());
    printf ("MinSignal                         :  %i %i %i %i \n"
	    ,(Int_t)getMinSignal(0),(Int_t)getMinSignal(1)
	    ,(Int_t)getMinSignal(2),(Int_t)getMinSignal(3));
}
void HMdcGarEffMaker::writeAscii()
{
    // Writes the produced parameters for HMdcCellEff to an
    // ascii output file

    cout<<"HMdcGarEffMaker::writeAscii()"<<endl;

    outputAscii=fopen(fNameAsciiOut.Data(),"w");

    fprintf(outputAscii,"#############################################################\n");
    fprintf(outputAscii,"# Cell efficiency parameter for HMdcDigitizer                \n");
    fprintf(outputAscii,"# format: MinSignal :  %i %i %i %i \n"
	    ,(Int_t)getMinSignal(0),(Int_t)getMinSignal(1)
	    ,(Int_t)getMinSignal(2),(Int_t)getMinSignal(3));
    fprintf(outputAscii,"# module angle p0 p1                                         \n");
    fprintf(outputAscii,"#############################################################\n");
    fprintf(outputAscii,"[MdcCellEff]\n");
    for(Int_t mdc=0;mdc<4;mdc++)
    {
	for(Int_t angle=0;angle<18;angle++)
	{
	    fprintf(outputAscii,"%i %2i  %7.5f  %7.5f \n",mdc,angle,fitpars[mdc][angle][0],fitpars[mdc][angle][1]);
	}
    }
    fprintf(outputAscii,"#############################################################\n");
    fclose(outputAscii);
}
void HMdcGarEffMaker::createHists(Int_t mdc,Int_t angle,Int_t type)
{
    // Creates th needed hists
    // type==0 -> per mdc
    // type==1 -> per angle

    Char_t namehist[300];
    Char_t titlehist[300];
    if(type==0)
    {   // per mdc
	sprintf(namehist ,"%s%i%s","heff_max[",mdc,"]");
	sprintf(titlehist,"%s%i%s","heff_max[",mdc,"]");
	heff_max= new TH1D(namehist,titlehist,20,0,20);
	heff_max->SetLineColor(colors[mdc]);

	sprintf(namehist ,"%s%i%s","heff_max_distance[",mdc,"]");
	sprintf(titlehist,"%s%i%s","heff_max_distance[",mdc,"]");
	heff_max_distance= new TH1D(namehist,titlehist,20,0,20);
	heff_max_distance->SetLineColor(colors[mdc]);

	for(Int_t i=0;i<getNSteps();i++)
	{
	    sprintf(namehist ,"%s%02i%s%i%s","heff_cut_[",(i+1)*5,"][",mdc,"]");
	    sprintf(titlehist,"%s%02i%s%i%s","heff_cut_[",(i+1)*5,"][",mdc,"]");
	    heff_cut[i]= new TH1D(namehist,titlehist,20,0,20);
	    heff_cut[i]->SetLineColor(colors[i]);
	}
    }
    if(type==1)
    {   // per angle
	sprintf(namehist ,"%s%i%s%02i%s","heff_cut_trend[",mdc,"][",angle,"]");
	sprintf(titlehist,"%s%i%s%02i%s","heff_cut_trend[",mdc,"][",angle,"]");
	heff_cut_trend= new TH1D(namehist,titlehist,60,0,60);
	heff_cut_trend->SetLineColor(colors[mdc]);

	sprintf(namehist ,"%s%i%s%02i%s","heff_time1[",mdc,"][",angle,"]");
	sprintf(titlehist,"%s%i%s%02i%s","heff_time1[",mdc,"][",angle,"]");
	heff_time1= new TH1D(namehist,titlehist,getNSamples(),0,getNSamples());
	heff_time1->SetLineColor(colors[mdc]);
	heff_time1->SetMarkerStyle(29);
	heff_time1->SetMarkerColor(2);
	heff_time1->SetMarkerSize(.8);
    }
}
void HMdcGarEffMaker::writeHists(Int_t type)
{
    // writes the created hists to an output Root file
    // type==0 -> per mdc
    // type==1 -> per angle
    if(type==0)
    {   // per mdc
	HTool::writeObject(heff_max);
	HTool::writeObject(heff_max_distance);

	for(Int_t i=0;i<getNSteps();i++)
	{
	    HTool::writeObject(heff_cut[i]);
	}
    }
    if(type==1)
    {   // per angle
	HTool::writeObject(heff_cut_trend);
	HTool::writeObject(heff_time1);
    }
}
void HMdcGarEffMaker::deleteHists(Int_t type)
{
    // Delete the created hists
    // type==0 -> per mdc
    // type==1 -> per angle
    if(type==0)
    {   // per mdc
	HTool::deleteObject(heff_max);
	HTool::deleteObject(heff_max_distance);

	for(Int_t i=0;i<getNSteps();i++)
	{
	    HTool::deleteObject(heff_cut[i]);
	}
    }
    if(type==1)
    {   // per angle
	HTool::deleteObject(heff_cut_trend);
	HTool::deleteObject(heff_time1);
    }
}
void HMdcGarEffMaker::make()
{
    // function to be called from macro.
    // opens input and output file, loops over input
    gROOT->SetBatch();
    cout<<"--------------------------------------------------------------"<<endl;
    printStatus();
    cout<<"--------------------------------------------------------------"<<endl;
    readInput();
    cout<<"--------------------------------------------------------------"<<endl;
    writeAscii();
    cout<<"--------------------------------------------------------------"<<endl;
    HTool::close(&outputRoot);
    cout<<"--------------------------------------------------------------"<<endl;
}
void HMdcGarEffMaker::readInput()
{
    // reads from root file containing signals produced by HMdcGarSignalReader
    cout<<"HMdcGarEffMaker:readInput()"<<endl;

    Char_t name[300];

    if(getVersion()==1)
    {
	if(!HTool::open(&outputRoot,fNameRootOut,"RECREATE"))
	{
	    exit(1);
	}

	outputRoot->cd();

	Char_t mypath[300];

	TDirectory* dir=0;
	for(Int_t mdc=0;mdc<4;mdc++)
	{
	    sprintf(mypath,"%s%i","mdc ",mdc);
	    dir=HTool::changeToDir(mypath);

	    createHists(mdc,0,0);
	    for(Int_t angle=0;angle<18;angle++)
	    {
		cout<<"mdc "<<mdc<<" angel "<<angle<<endl;

		sprintf(name,"%s%i%s%i%s%i%s","/u/kempter/scratchlocal1/jochen/garfield/signals/mdc",mdc+1,"/s",angle*5,"-mdc",mdc+1,"_merge.root");
                fNameRootIn=name;

		if(!HTool::open(&inputRoot,fNameRootIn,"READ"))
		{
		    exit(1);
		}
		createHists(mdc,angle,1);
		fillEffHist   (mdc,angle);
		fillCutHist   (mdc,angle);
		fillMaxHists  (angle);
		fillTrendHists(angle);
		fitTrendHists (mdc,angle);
		outputRoot->cd();
                dir=HTool::changeToDir(mypath);
		writeHists (1);
		deleteHists(1);

		HTool::close(&inputRoot);
	    }
	    outputRoot->cd();
	    dir=HTool::changeToDir(mypath);
	    writeHists (0);
	    deleteHists(0);

	    dir->cd("..");
	}
	
    }
    if(getVersion()==2)
    {
	if(!HTool::open(&outputRoot,fNameRootOut,"RECREATE"))
	{
	    exit(1);
	}

	if(!HTool::open(&inputRoot,fNameRootIn,"READ"))
	{
	    exit(1);
	}

	outputRoot->cd();

	Char_t mypath[300];

	TDirectory* dir=0;
	for(Int_t mdc=0;mdc<4;mdc++)
	{
	    sprintf(mypath,"%s%i","mdc ",mdc);
	    dir=HTool::changeToDir(mypath);

	    createHists(mdc,0,0);
	    for(Int_t angle=0;angle<18;angle++)
	    {

		cout<<"mdc "<<mdc<<" angel "<<angle<<endl;

		createHists(mdc,angle,1);
		fillEffHist   (mdc,angle);
		fillCutHist   (mdc,angle);
		fillMaxHists  (angle);
		fillTrendHists(angle);
		fitTrendHists (mdc,angle);
		writeHists (1);
		deleteHists(1);

	    }
	    writeHists (0);
	    deleteHists(0);

	    dir->cd("..");
	}
	HTool::close(&inputRoot);
    }
}
TH1F* HMdcGarEffMaker::getHist(TFile* input,TString namehist)
{
    // Gets hist from file
    TH1F* hsum =(TH1F*)input->Get(namehist.Data());
    return hsum;
}
void HMdcGarEffMaker::fillEffHist(Int_t mdc,Int_t angle)
{
    // Fills the efficiency hists from the hist read from input file
    Char_t mypath[300];
    Char_t namehist[300];
    TString mydir="";
    TH1F* hsum =0;
    for(Int_t sample=0;sample<getNSamples()-1;sample++)  // loop over samples
    {
	if(getVersion()==1)
	{
	    sprintf(mypath  ,"%s%02i","sample ",sample);
            sprintf(namehist,"%s%s%02i%s",mypath,"/hsum[",sample,"]");

	}
	if(getVersion()==2)
	{
	    sprintf(mypath  ,"%s%i%s%02i%s%02i"  ,"mdc ",mdc,"/angle ",angle*5,"/sample ",sample);
	    sprintf(namehist,"%s%s%02i%s",mypath,"/hsum[",sample,"]");
	    sprintf(mypath  ,"%s%i%s%02i%s%02i%s","mdc ",mdc,"/angle ",angle*5,"/sample ",sample,";1");
	}

        mydir=mypath;
	if(HTool::checkDir(mydir,inputRoot))
	{
	    hsum =getHist(inputRoot,namehist);
	    if(hsum)
	    {
		heff_time1->SetBinContent(sample+1,(Double_t)hsum->Integral(0,1000));
	    }
	    HTool::deleteObject(hsum);

	}
	else continue;
    }
}
void HMdcGarEffMaker::fillCutHist(Int_t mdc,Int_t angle)
{
    // Fills the cut hists from efficiency hists
    Int_t i=1;
    for(Int_t step=0;step<getNSteps();step++)
    {
        i=1;
	while( (heff_time1->GetBinContent(getNSamples()-i)) > (getMinSignal(mdc)*(step+1)*0.05) && i<100 )
	{
	    heff_cut[step]->SetBinContent(angle+1,(getNSamples()-1-i));
            i++;
	}
    }
}
void HMdcGarEffMaker::fillMaxHists(Int_t angle)
{
    // Fills the max hists
    heff_max         ->SetBinContent(angle+1,heff_time1->GetMinimum());
    heff_max_distance->SetBinContent(angle+1,heff_time1->GetMinimumBin());
}

void HMdcGarEffMaker::fillTrendHists(Int_t angle)
{
    // Fill the trend hists from the cut hists
    for(Int_t step=0;step<getNSteps();step++) // percent cut
    {
	heff_cut_trend->SetBinContent((5*step)+1,(heff_cut[step]->GetBinContent(angle+1))*.1);
    }
}
void HMdcGarEffMaker::fitTrendHists(Int_t mdc,Int_t angle)
{
    // Fit trend hists to get the parameters for HMdcCellEff
    TF1 *fitlinear=new TF1("linearfit","pol1",0,10);
    heff_cut_trend->Fit("linearfit","Q");
    fitpars[mdc][angle][0]=fitlinear->GetParameter(0);// y offset
    fitpars[mdc][angle][1]=fitlinear->GetParameter(1);// slope
}
