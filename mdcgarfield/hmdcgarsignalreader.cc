//*-- AUTHOR : J. Markert

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HMdcGarSignalReader
// Reads Ascii files containing signals produced by GARFIELD.
// Hists for Signal, integrated Signal,ditribution of Time1 and Time2,
// summed signals per sample point and summed integrated signals per sample point
// can be filled and written to the output file. Hists for maximum charge per
// mdc (angle,sample) or (angle,sample,signal) are filled to define the max value
// for the threshold calulations. The main output hists for time1 and time2 as function
// of distance from wire including the errors are filled from the distribtution hists
// for time1 and time2 (mean+RMS) for different threshold values. Those hists are input
// to HMdcGarCal2Maker and HMdcGarEffMaker to produce the CAL2 and efficiency
// parameters for MDC.
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>
#include <iostream> 
#include <iomanip>
#include "hmdcgarsignalreader.h"
#include "htool.h"
#include "TString.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TROOT.h"
#include "TKey.h"
#include "TStyle.h"
#include "TMath.h"

ClassImp(HMdcGarSignalReader)

HMdcGarSignalReader::HMdcGarSignalReader(const Char_t* name,const Char_t* title)
    : TNamed(name,title)
{
    // constructor for HMdcGarSignalReader
    initVariables();
}
HMdcGarSignalReader::~HMdcGarSignalReader()
{
  // destructor of HMdcGarSignalReader
}
void HMdcGarSignalReader::setFileNameOut(TString myfile)
{
    // Sets root output of HMdcGarSignalReader
    fNameRootOut=myfile;
    if(fNameRootOut.CompareTo("")==0)
    {
	Error("HMdcGarSignalReader:setFileNameOut()","NO OUTPUT FILE SEPCIFIED!");
        exit(1);
    };
    cout<<"HMdcGarSignalReader::setFileNameOut(): OUTPUT FILE= "<<fNameRootOut.Data()<<endl;
}
void HMdcGarSignalReader::setFileNameIn(TString myfile)
{
    // Sets root input of HMdcGarSignalReader
    fNameAsciiIn=myfile;
    if(fNameAsciiIn.CompareTo("")==0)
    {
	Error("HMdcGarSignalReader:setFileNameIn() ","NO INPUT  FILE SEPCIFIED!");
        exit(1);
    };
    cout<<"HMdcGarSignalReader::setFileNameIn() : INPUT  FILE= "<<fNameAsciiIn.Data()<<endl;
}
void HMdcGarSignalReader::initVariables()
{
    // inits all variables
    version           =704;
    fNameAsciiIn      ="";
    inputAscii        =0;
    fNameRootOut      ="";
    outputRoot        =0;
    setNBinSignal(1000);
    setRangeSignal(1000);
    setNSignals(100);
    setNBinDist(100);
    setBinSizeDist(0.1);
    setNStep(10);
    setStepSize(0.02);
    initHistArrays();
    setMinSignal(-60000,-65000,-380000,-530000);   // default for simple integration
    setWriteHists(1,1,1,1,1,1);
    hresponse=0;
    setSimResponse(kTRUE);
    setSigma1(5.);
    setSigma2(20.);

    myconfig=new TPaveText(0,0,0.995,0.995);
    myconfig->SetTextAlign(11);
    myconfig->SetBorderSize(0);
    myconfig->SetName("Configuration");
    nmake=0;


    for(Int_t i=0;i<4;i++){
	hminCharge   [i]=NULL;
	hminChargeSum[i]=NULL;
    }

}
void HMdcGarSignalReader::response(Float_t mean,Float_t scale)
{
    hresponse->Reset();
    Int_t nbins =getNBinSignal()+100;
    Float_t x;
    Float_t val;
    for(Int_t bin=0;bin<nbins;bin++)
    {
        x=hresponse->GetBinCenter(bin);
        if(x<mean){
            val= scale* ((Float_t)(-TMath::Gaus(x,mean,getSigma1())));
            if(val<-1)hresponse->SetBinContent(bin,val);
        }else{
            val= scale* ((Float_t)(-TMath::Gaus(x,mean,getSigma2())));
            if(val<-1)hresponse->SetBinContent(bin,val);
        }
    }
}
void HMdcGarSignalReader::simulateResponse(TH1F* hsignal,TH1F* hint_diff)
{
    Double_t mult=0;
    Int_t nbins =hsignal->GetNbinsX();
    Double_t x;
    for(Int_t i=0;i<nbins;i++)
    {
	mult=hsignal->GetBinContent(i);
        if(mult<0)
        {
            x=hsignal->GetBinCenter(i);
            response(x,-mult);
            hint_diff->Add(hresponse);
        }
    }
}
void HMdcGarSignalReader::createChargeHists()
{
    // creates hists for maximum charge per mdc,angle,sample,signal (per signal)
    // or mdc,angle,sample (per summed signal)
    Char_t namehist[300];
    for(Int_t mdc=0;mdc<4;mdc++)
    {
	if(hminCharge[mdc]==0)
	{
	    sprintf(namehist,"%s%i%s","hminCharge[",mdc,"]");
	    hminCharge[mdc]=new TH1F(namehist,namehist,18*getNBinDist()*getNSignals(),0,18*getNBinDist()*getNSignals());
	    hminCharge[mdc]->SetLineColor(2);
	    hminCharge[mdc]->SetDirectory(0);

	    sprintf(namehist,"%s%i%s","hminCharge_sum[",mdc,"]");
	    hminChargeSum[mdc]=new TH1F(namehist,namehist,18*getNBinDist(),0,18*getNBinDist());
	    hminChargeSum[mdc]->SetLineColor(2);
	    hminChargeSum[mdc]->SetDirectory(0);
	}
    }
}
void HMdcGarSignalReader::fillChargeHists(Int_t mdc,Int_t angle,Int_t sample,Int_t signal,Int_t type,TH1F* hsignal,TH1F* hsum)
{
    // fills hists for maximum charge per mdc,angle,sample,signal (per signal)
    // or mdc,angle,sample (per summed signal)

    if(type==0)hminCharge   [mdc]->SetBinContent(angle*getNBinDist()*getNSignals()+sample*getNSignals()+ signal +1,(Float_t)hsignal->GetMinimum());
    if(type==1)hminChargeSum[mdc]->SetBinContent(angle*getNBinDist() + sample +1                                  ,(Float_t)hsum   ->GetMinimum());
}
void HMdcGarSignalReader::writeChargeHists()
{
    // writes hists for charge to file
    if(outputRoot)
    {
        outputRoot->cd();
	for(Int_t mdc=0;mdc<4;mdc++)
	{
	    writeHist(hminCharge   [mdc],5);
	    writeHist(hminChargeSum[mdc],5);
	}
    }
}
void HMdcGarSignalReader::deleteChargeHists()
{
   // delete charge hists
   for(Int_t mdc=0;mdc<4;mdc++)
   {
       HTool::deleteObject(hminCharge   [mdc]);
       HTool::deleteObject(hminChargeSum[mdc]);
   }
}
void HMdcGarSignalReader::printStatus(void)
{
    // prints the parameters to the screen
    printf ("HMdcGarSignalReader::printStatus()\n");
    printf ("GARFIELD Version                  :  %i\n",version);
    printf ("AsciiInput                        :  %s\n",fNameAsciiIn .Data());
    printf ("RootOutput                        :  %s\n",fNameRootOut.Data());
    printf ("WriteHists                        :  %i %i %i %i %i %i \n"
	    ,(Int_t)writehists[0],(Int_t)writehists[1],(Int_t)writehists[2]
	    ,(Int_t)writehists[3],(Int_t)writehists[4],(Int_t)writehists[5]);
    printf ("SimResponse                       :  %i\n",(Int_t)simResponse);
    printf ("Sigma1 , Sigma2                   :  %5.1f , %5.1f \n",sigma1,sigma2);
 
}
void HMdcGarSignalReader::make(Int_t mdc,Int_t angle,TString inputfile)
{
    // function to be called from macro.
    // opens input and output file, loops over input

    setFileNameIn(inputfile);
    createChargeHists();

    if(!outputRoot)
    {
	HTool::open(&outputRoot,fNameRootOut,"RECREATE");
    }
    else
    {
	if(!outputRoot->IsOpen())
	{
	    Error("HMdcGarSignalReader::make()","NON ZERO FILE POINTER, BUT FILE NOT OPEN!");
            exit(1);
	}
    }
    if(!outputRoot->IsOpen())
    {
	Error("HMdcGarSignalReader::make()","NON ZERO FILE POINTER, BUT FILE NOT OPEN!");
	exit(1);
    };
    outputRoot->cd();
    Char_t mytext[300];
    if(nmake==0)
    {  // write config only the first time
        
        sprintf (mytext,"GARFIELD Version  %i\n",version);
        myconfig->AddText(mytext);
        sprintf (mytext,"RootOutput  %s\n",fNameRootOut.Data());
        myconfig->AddText(mytext);
        sprintf (mytext,"WriteHists  %i %i %i %i %i %i \n"
                ,(Int_t)writehists[0],(Int_t)writehists[1],(Int_t)writehists[2]
                ,(Int_t)writehists[3],(Int_t)writehists[4],(Int_t)writehists[5]);
        myconfig->AddText(mytext);
        sprintf (mytext,"SimResponse  %i\n",(Int_t)simResponse);
        myconfig->AddText(mytext);
        sprintf (mytext,"Sigma1 , Sigma2 %5.1f, %5.1f \n",sigma1,sigma2);
        myconfig->AddText(mytext);
        sprintf (mytext,"nSignals %i \n",getNSignals());
        myconfig->AddText(mytext);
        sprintf (mytext,"nBinSignal %i \n",getNBinSignal());
        myconfig->AddText(mytext);
        sprintf (mytext,"rangeSignal %9.3f \n",getRangeSignal());
        myconfig->AddText(mytext);
        sprintf (mytext,"nBinDist %i \n",getNBinDist());
        myconfig->AddText(mytext);
        sprintf (mytext,"binSizeDist %9.3f \n",getBinSizeDist());
        myconfig->AddText(mytext);
        sprintf (mytext,"nSteps  %i \n",getNStep());
        myconfig->AddText(mytext);
        sprintf (mytext,"stepSize  %9.3f \n",getStepSize());
        myconfig->AddText(mytext);
        sprintf (mytext,"minSignal    %9.3f, %9.3f, %9.3f, %9.3f\n",getMinSignal(0),getMinSignal(1),getMinSignal(2),getMinSignal(3));
        myconfig->AddText(mytext);
    }
    nmake++;
    sprintf (mytext,"%i. input %s\n",nmake,inputfile.Data());
    myconfig->AddText(mytext);
    cout<<"--------------------------------------------------------------"<<endl;
    printStatus();
    cout<<"--------------------------------------------------------------"<<endl;
    hresponse=new TH1F("response","response",getNBinSignal()+100,-100,getRangeSignal());
    readInput(mdc,angle);
    if(hresponse) delete hresponse;
    cout<<"--------------------------------------------------------------"<<endl;
}
void HMdcGarSignalReader::readInput(Int_t mdc,Int_t angle)
{
    // reads from ascii file containing signals produced by GARFIELD
    // Creates subdirectory structure in output file, fills and writes hists for
    // signals, time1 and time2 and max charge
    gROOT->Reset();
    gStyle->SetPalette(50);
    //######################## reading ascii from garfield ################
    TH1::AddDirectory(kFALSE);

    Char_t line[200];
    Float_t  f_time ,f_current;

    Int_t sample    =0;            // counter for sample point
    Int_t sampleold =0;            // counter for sample point
    Int_t block     =0;            // helper counter
    Int_t signal    =0;            // counter for signals of sample point
    Int_t bin       =0;            // counter for bin of hist

    TH1F* hsignal   =0;
    TH1F* hint_diff =0;
    TH1F* hsum      =0;
    TH1F* hsumint   =0;

    TDirectory* dirMdc=0;
    if(outputRoot)dirMdc=HTool::Mkdir(outputRoot,"mdc",mdc);

    TDirectory* dirAngle=0;
    if(dirMdc)dirAngle=HTool::Mkdir(dirMdc,"angle",angle*5,2);

    createHistArrays(mdc,angle,sample,1);

    TDirectory* dirSample=0;
    if(dirAngle)dirSample=HTool::Mkdir(dirAngle,"sample",sample,2);

    createHists(sample,signal,0,&hsignal,&hint_diff,&hsum,&hsumint);
    //--------------------------------- create hists per sample point ----------------
    createHistArrays(mdc,angle,sample,2);
    createHists(sample,signal,1,&hsignal,&hint_diff,&hsum,&hsumint);
    //--------------------------------------------------------------------------------



    inputAscii = fopen(fNameAsciiIn.Data(),"r");
    if (inputAscii == NULL)
    {
	Error("HMdcGarSignalReader::readInput()","Cannot open %s\n",fNameAsciiIn.Data());
	exit(1);
    }
    cout<<"HMdcGarSignalReader::readInput()   : Reading from "<<fNameAsciiIn.Data()<<endl;

    Char_t Tag []=" + VALUE_SCALE_FACTOR";
    Char_t Tag1[]="% Created";
    Char_t Tag3[]=" +";
    Int_t signalcount=0;
    Bool_t condition=kFALSE;
    Bool_t res=kTRUE;

    Int_t samplePoint = 0;
    Int_t signalNr    = 0;

    while(!feof(inputAscii))
    {   // end of file condition
	//--------------------------------- search first value ---------------------------
	while(1)
	{
	    if(feof(inputAscii)) break;
	    res=fgets(line, sizeof(line), inputAscii);
	    
	    if (!strncmp(line, Tag ,strlen(Tag))) break;  // break if " + VALUE_SCALE_FACTOR" has been found
	}
	//--------------------------------------------------------------------------------
	while(1)
	{
	    if (feof(inputAscii)) break;
	    res=fgets(line, sizeof(line), inputAscii);
	    if(!res)cout<<"readInput: could not read next line!"<<endl;
	    if(strncmp(line,Tag1,strlen(Tag1)) && !strncmp(line,Tag3,strlen(Tag3)) ) // read until "% Created" has been found
	    {
                //--------------------------------- read signal ----------------------------------
		sscanf(&line[5],"%e%e%*s",&f_time, &f_current);

		condition=kFALSE;
                if      (version==704)                    condition=kTRUE;
                else if (version==708&&(signalcount%2==0))condition=kTRUE;

                if(condition)
                {
                    hsignal->SetBinContent(bin+100,f_current);
                    bin++;
                }
	        //--------------------------------------------------------------------------------
	    }
	    else
	    {
		sscanf(&line[50],"%*s %d %*s %d%*s",&samplePoint, &signalNr);  //% Created 10/07/14 At 11.45.25 < none > SIGNAL   "p 1 n 1
                signalcount++;

                condition=kFALSE;
                if      (version==704)                        condition=kTRUE;
                else if (version==708&&((signalcount-1)%2==0))condition=kTRUE;

                if(condition)
                {

                    //--------------------------------- calculation + storage ------------------------

                    //--------------------------------------------------------------------------------
                    // old signal processing
		    if(!simResponse){
                        integrateHists(mdc,angle,sample,signal,hsignal,hint_diff);
                    }else{
                        simulateResponse(hsignal,hint_diff);
                    }
                    findThresholds(mdc,hint_diff);
                    fillChargeHists(mdc,angle,sample,signal,0,hint_diff,hsum);

                    hsum   ->Add(hsignal);
                    hsumint->Add(hint_diff);
                    //--------------------------------------------------------------------------------

                    bin=0;
                    block++;
                   // sample=(Int_t)((block-(block%getNSignals()))/getNSignals()); // count data points per curve
                   // signal=(Int_t)((block-sample*getNSignals())%getNSignals());
                    sample=samplePoint; // count data points per curve
                    signal=signalNr;
                    //--------------------------------- delete old and create new hist----------------
                    writeHist(hsignal  ,0);
                    writeHist(hint_diff,1);

                    HTool::deleteObject(hsignal);
                    HTool::deleteObject(hint_diff);

                    createHists(sample,signal,0,&hsignal,&hint_diff,&hsum,&hsumint);
                    //--------------------------------- change directorys ----------------------------
                    if(sample!=sampleold)
                    {
                        cout<<"mdc "<<mdc<<" angle "<<angle<<" sample "<<sample<<endl;
                        fillDriftTimeVersusDistance(mdc,angle,sample-1);
                        fillChargeHists(mdc,angle,sample,signal,1,hint_diff,hsumint);

                        writeHist (hsum   ,2);
                        writeHist (hsumint,2);
                        HTool::deleteObject(hsum);
                        HTool::deleteObject(hsumint);

                        writeHistArrays (2);
                        deleteHistArrays(2);

			if(dirSample){
			    dirSample->TDirectory::Cd("..");
                            dirSample->Close();
			}
			if(dirAngle)dirSample=HTool::Mkdir(dirAngle,"sample",sample,2);

                        createHistArrays(mdc,angle,sample,2);
                        createHists(sample,signal,1,&hsignal,&hint_diff,&hsum,&hsumint);

                    }
                    sampleold=sample;
                    //--------------------------------------------------------------------------------
                }
                break;
            }
        }
    }
    //---------------- very last signal of the file ----------------------------------

    if(!simResponse){
        integrateHists(mdc,angle,sample,signal,hsignal,hint_diff);
    }else{
        simulateResponse(hsignal,hint_diff);
    }

    findThresholds(mdc,hint_diff);
    fillChargeHists(mdc,angle,sample,signal,0,hint_diff,hsum);
    fillDriftTimeVersusDistance(mdc,angle,sample);

    hsum   ->Add(hsignal);
    hsumint->Add(hint_diff);

    writeHist (hsignal  ,0);
    writeHist (hint_diff,1);
    writeHist (hsum   ,2);
    writeHist (hsumint,2);

    HTool::deleteObject(hsignal);
    HTool::deleteObject(hint_diff);
    HTool::deleteObject(hsum);
    HTool::deleteObject(hsumint);

    writeHistArrays (2);
    deleteHistArrays(2);

    if(dirSample){
	dirSample->TDirectory::Cd("..");
        dirSample->Close();
    }
    writeHistArrays (1);
    deleteHistArrays(1);

    //--------------------------------------------------------------------------------

    if(dirAngle){
	dirAngle->TDirectory::Cd("..");
        dirAngle->Close();
    }
    if (dirMdc) dirMdc->TDirectory::Cd("..");

    dirSample=0;
    dirMdc   =0;
    fclose(inputAscii);
}
void HMdcGarSignalReader::fillDriftTimeVersusDistance(Int_t mdc,Int_t angle,Int_t sample)
{
    // fills main hist for time1 and time2 including errors by mean and RMS of the distribution
    // signals of time1 and time2.
    for(Int_t step=0;step<getNStep();step++)
    {
	if(sample<getNBinDist())
	{
	    if(check(mdc,angle,sample,step,hdist_1[step]))
	    {
		htime1[step]->SetBinContent(1+sample,hdist_1[step]->GetMean());
		htime1[step]->SetBinError  (1+sample,hdist_1[step]->GetRMS());
	    }
	    if(check(mdc,angle,sample,step,hdist_2[step]))
	    {
		htime2[step]->SetBinContent(1+sample,hdist_2[step]->GetMean());
		htime2[step]->SetBinError  (1+sample,hdist_2[step]->GetRMS());
	    }
	}
	else
	{
	    Error("MdcGarSignalReader::fillDifttimeVersusDistance()","MORE BINS THAN DEFINED in mdc %i angle %02i sample %02i step %i !",
		  mdc,angle,sample,step);
	    exit(1);
	}
    }
}
Bool_t HMdcGarSignalReader::check(Int_t mdc,Int_t angle,Int_t sample,Int_t step,TH1F* hdist)
{
    // checks if hist is not empty
    if(hdist->Integral()!=0 &&
       hdist->GetMean ()!=0 &&
       hdist->GetRMS  ()!=0 ) return kTRUE;
    else
    {
	Warning("MdcGarSignalReader::check()","INTGRAL, MEAN OR RMS EQUAL ZERO in mdc %i angle %02i sample %02i step %i !",
	       mdc,angle,sample,step);
	return kFALSE;
    }
}
void HMdcGarSignalReader::findThresholds(Int_t mdc,TH1F* hint_diff)
{
    // loops over signals hist to find the time value at threshold
    // for different thresholds
    Int_t bintime1,bintime2;
    Float_t percent;
    Int_t counttime1;
    Int_t counttime2;

    Int_t nbin=getNBinSignal()+100;

    for(Int_t step=0;step<getNStep();step++)
    {
	percent=getStepSize()*step;
	counttime1=0;
	counttime2=0;
	bintime1=0;
	bintime2=0;

	for(Int_t bin=1;bin<nbin;bin++)
	{
	    if(hint_diff->GetBinContent(bin)     <percent*getMinSignal(mdc))counttime1++;
	    if(hint_diff->GetBinContent(nbin-bin)<percent*getMinSignal(mdc))counttime2++;

	    if(counttime1==1)
	    {
		bintime1++;
                if(bintime1==1){
                    hdist_1[step]->SetBinContent(bin,hdist_1[step]->GetBinContent(bin)+1);
                }
            }
	    if(counttime2==1)
	    {
		bintime2++;
                if(bintime2==1)hdist_2[step]->SetBinContent(nbin-bin,hdist_2[step]->GetBinContent(nbin-bin)+1);
            }
	}
    }
}
void HMdcGarSignalReader::createHists(Int_t sample,Int_t signal,Int_t type,
				     TH1F** hsignal,TH1F** hint_diff,TH1F** hsum,TH1F** hsumint)
{
    // create hists for signal/integrated signal (type==0 per signal)
    // or sum/integrated sum(type==1 per sample)

    Int_t nbin=getNBinSignal()+100;  // changed range
    Float_t xlow=-100.;              // starting from -100 insted of 0 because of respones
    Float_t xup= getRangeSignal();
    Char_t namehist[300];
    Char_t titlehist[300];
    if(type==0)
    { // per signal
	sprintf(namehist,"%s%02i%s","hsignal[",signal,"]");
	*hsignal= new TH1F(namehist,namehist,nbin,xlow,xup); //changed
	(*hsignal)->SetFillColor(38);

	sprintf(namehist ,"%s%02i%s","hintegral_diff[",signal,"]");
	sprintf(titlehist,"%s%02i%s","hint_diff["     ,signal,"]");
	(*hint_diff) =new TH1F(namehist,titlehist,nbin,xlow,xup);
	(*hint_diff)->SetFillColor(30);
	(*hint_diff)->SetFillStyle(3013);
    }
    if(type==1)
    {   // per sample

	sprintf(namehist ,"%s%02i%s","hsum[",sample,"]");
	*hsum=new TH1F(namehist,namehist,nbin,xlow,xup);
	(*hsum)->SetFillColor(30);

	sprintf(namehist ,"%s%02i%s","hsumint[",sample,"]");
	*hsumint=new TH1F(namehist,namehist,nbin,xlow,xup);
	(*hsumint)->SetFillColor(30);
    }
}
void HMdcGarSignalReader::createHistArrays(Int_t mdc,Int_t angle,Int_t sample,Int_t type)
{
    // creates his arrays for time1 and time2 as function of distance to wire
    // for different thresholds (type==1 per angle)
    // or hists for distribution of time1 and time2 (type==2 per sample)
    //--------------------------------- create hists per sample point ----------------
    Char_t namehist[300];

    Int_t nbin=getNBinSignal()+100;
    Float_t xlow=-100.;
    Float_t xup =getRangeSignal();

    for(Int_t step=0;step<getNStep();step++)
    {
	if(type==1||type==3)
	{
	    sprintf(namehist,"%s%i%s%02i%s%02i%s","hmeandrift_time1[",mdc,"][",step,"][",angle*5,"]");
	    htime1[step]=new TH1F(namehist,namehist,getNBinDist(),0,getNBinDist()*getBinSizeDist());
	    htime1[step]->SetMarkerStyle(29);
	    htime1[step]->SetMarkerColor(2);
	    htime1[step]->SetMarkerSize(.8);

	    sprintf(namehist,"%s%i%s%02i%s%02i%s","hmeandrift_time2[",mdc,"][",step,"][",angle*5,"]");
	    htime2[step]=new TH1F(namehist,namehist,getNBinDist(),0,getNBinDist()*getBinSizeDist());
	    htime2[step]->SetMarkerStyle(29);
	    htime2[step]->SetMarkerColor(4);
	    htime2[step]->SetMarkerSize(.8);
	}
	if(type==2||type==3)
	{
	    sprintf(namehist ,"%s%i%s","hdist_1[",step,"]");
	    hdist_1[step]=new TH1F(namehist,namehist,nbin,xlow,xup);
	    hdist_1[step]->SetFillColor(46);

	    sprintf(namehist ,"%s%i%s","hdist_2[",step,"]");
	    hdist_2[step]=new TH1F(namehist,namehist,nbin,xlow,xup);
	    hdist_2[step]->SetFillColor(38);
	}
    }
}
void HMdcGarSignalReader::integrateHists(Int_t mdc,Int_t angle,Int_t sample,Int_t signal,TH1F* hsignal,TH1F* hint_diff)
{
    // ingetrate signal hist
    Int_t nbin=getNBinSignal();
    Float_t xlow=-100.;
    Float_t xup=getRangeSignal();

    TH1F* hint_time1= new TH1F("htime1","htime1",nbin,xlow,xup);
    TH1F* hint_time2= new TH1F("htime2","htime2",nbin,xlow,xup);

    hint_time1->SetBinContent(1     ,hsignal->GetBinContent(1));
    hint_time2->SetBinContent(nbin-1,hsignal->GetBinContent(nbin-1));

    for(Int_t bin=2;bin<nbin;bin++)
    {
	hint_time1->SetBinContent(bin     ,hint_time1->GetBinContent(bin-1)       +hsignal->GetBinContent(bin));
	hint_time2->SetBinContent(nbin-bin,hint_time2->GetBinContent(nbin-(bin-1))+hsignal->GetBinContent(nbin-bin));
    }
    for(Int_t bin=1;bin<nbin;bin++)
    {
	Float_t bintime2=hint_time2->GetBinContent(bin);
	Float_t bintime1=hint_time1->GetBinContent(bin);

	if (bintime2-bintime1<0)
	{
	    hint_diff->SetBinContent(bin,bintime1);
	}
	else
	{
	    hint_diff->SetBinContent(bin,bintime2);
	}
    }
    HTool::deleteObject(hint_time1);
    HTool::deleteObject(hint_time2);
}
void HMdcGarSignalReader::closeOutput()
{
    // writes charge hist and closes output file
    myconfig->AddText("");
    myconfig->Write();
    writeChargeHists();
    deleteChargeHists();
    HTool::close(&outputRoot);
}
