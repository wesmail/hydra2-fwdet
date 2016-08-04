//*-- AUTHOR : J. Markert

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HMdcGarCal2Maker
// Reads Histograms for time1/time2 versus distance from wire from root file
// produced with HMdcGarSignalReader. The values for time1 and time2 and the
// corresponding errors are processed (spike removing, smoothing, cut at max
// value per mdc) for all MDC's and all impact angles for time1, time2,
// error of time1 and error of time2. A root file with the control hists is
// written and an ascii file containing the parameters of HMdcCal2ParSim is
// produced.
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>
#include <iostream> 
#include <iomanip>
#include "hmdcgarcal2maker.h"
#include "htool.h"
#include "TString.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TROOT.h"
#include "TKey.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMath.h"
#include "TLine.h"


Int_t HMdcGarCal2Maker ::maxbinNumber[72]={25,28,28,31,33,34,  //25   // 6% treshold
                                           35,35,36,37,36,35,  //55
					   35,34,33,31,29,27,

                                           29,32,33,35,36,37,
					   40,39,39,39,38,38,
					   37,31,34,32,30,28, //65:36->31

					   60,63,65,68,69,70, //36           // 10:66->65 20:70->69 25:71->70
					   71,71,71,70,68,66,
					   64,61,57,53,49,44,                // 75:54->53 85: 45->44

					   70,75,78,81,83,85,
					   86,87,86,85,84,82,
					   79,75,71,67,62,56 };

Float_t HMdcGarCal2Maker::cutMax     [4]={110,120,260,370};

ClassImp(HMdcGarCal2Maker)

HMdcGarCal2Maker::HMdcGarCal2Maker(const Char_t* name,const Char_t* title)
    : TNamed(name,title)
{
    // constructor for HMdcGarCal2Maker
    // The parameters are initialized with non valid values.
    initVariables();
}
HMdcGarCal2Maker::~HMdcGarCal2Maker()
{
    // destructor of HMdcGarCal2Maker
    outputHists->Write();
    outputHists->Close();
}
void HMdcGarCal2Maker::setFileNameOut(TString myfile)
{
    // Sets ascii output of HMdcGarCal2Maker
    fNameAsciiOut=myfile;
    if(fNameAsciiOut.CompareTo("")==0)
    {
	Error("HMdcGarCal2Maker:setFileNameOut()","NO OUTPUT FILE SEPCIFIED!");
        exit(1);
    };
    cout<<"HMdcGarCal2Maker::setFileNameOut(): OUTPUT FILE= "<<fNameAsciiOut.Data()<<endl;
}
void HMdcGarCal2Maker::setFileNameIn(TString myfile)
{
    // Sets root input of HMdcGarCal2Maker
    fNameRootIn=myfile;
    if(fNameRootIn.CompareTo("")==0)
    {
	Error("HMdcGarCal2Maker:setFileNameIn() ","NO INPUT  FILE SEPCIFIED!");
        exit(1);
    };
    cout<<"HMdcGarCal2Maker::setFileNameIn() : INPUT  FILE= "<<fNameRootIn.Data()<<endl;
}
void HMdcGarCal2Maker::initVariables()
{
    // inits all variables
    fNameAsciiOut      ="";
    output             =0;
    fNameRootIn        ="";
    inputRoot          =0;
    htime1             =0;
    htime2             =0;
    HTool::open(&outputHists,"cal2makerHists.root","RECREATE");
    initArrays();
    setThreshold(5);
    setStepSize(2);
    setSlope(10);
    setBatchMode(kFALSE);
    setPrintMode(kFALSE);
    setWriteHists(kTRUE);
    setWriteAscii(kTRUE);
    setMaxErrTime1(30);
    setMaxErrTime2(50);
    setSpikeTypes(1,1,1,1);
    setSpikeThresholds(0.5,5.0);
    setNTimes(10,10,10,10);
    setNBin1D_Dist(110);
    setNBin2D_Dist(100);
    setNBin2D_Angle(19);
    setMinCopy(90);
    setMaxCopy(4);
    setHists_1D(1,1,1,1);
    setHists_2D(1,1,1,1);
    setLevelColors(2,4,8);
    setVoltage(1750,1800,2000,2400);
    setMakeMdc(1,1,1,1);
    setMakeType(1,1,1,1);
    setVersion(2);
}
void HMdcGarCal2Maker::printStatus(void)
{
    // prints the parameters to the screen
    printf ("HMdcGarCal2Maker::printStatus()\n");
    printf ("Version                           :  %i \n",getVersion());
    printf ("WriteHists                        :  %i \n",(Int_t)getWriteHists());
    printf ("WriteAscii                        :  %i \n",(Int_t)getWriteAscii());
    printf ("Make Mdc                          :  %i %i %i  %i \n",getMakeMdc(0),getMakeMdc(1),getMakeMdc(2),getMakeMdc(3));
    printf ("Make Type                         :  %i %i %i  %i \n",getMakeType(0),getMakeType(1),getMakeType(2),getMakeType(3));
    printf ("BatchMode                         :  %i \n",(Int_t)getBatchMode());
    printf ("Threshold                         :  %i%%\n",getStepSize()*getThreshold());
    printf ("Cut Max                           :  %i ns %i ns %i ns %i ns \n",(Int_t)getCutMax(0),(Int_t)getCutMax(1),(Int_t)getCutMax(2),(Int_t)getCutMax(3));
    printf ("MaxBinNumber   mdc 0 ------------ :  %i %i %i %i %i %i \n",maxbinNumber[0] ,maxbinNumber[1] ,maxbinNumber[2] ,maxbinNumber[3] ,maxbinNumber[4] ,maxbinNumber[5]);
    printf ("                                  :  %i %i %i %i %i %i \n",maxbinNumber[6] ,maxbinNumber[7] ,maxbinNumber[8] ,maxbinNumber[9] ,maxbinNumber[10],maxbinNumber[11]);
    printf ("                                  :  %i %i %i %i %i %i \n",maxbinNumber[12],maxbinNumber[13],maxbinNumber[14],maxbinNumber[15],maxbinNumber[16],maxbinNumber[17]);
    printf ("               mdc 1 ------------ :  %i %i %i %i %i %i \n",maxbinNumber[18],maxbinNumber[19],maxbinNumber[20],maxbinNumber[21],maxbinNumber[22],maxbinNumber[23]);
    printf ("                                  :  %i %i %i %i %i %i \n",maxbinNumber[24],maxbinNumber[25],maxbinNumber[26],maxbinNumber[27],maxbinNumber[28],maxbinNumber[29]);
    printf ("                                  :  %i %i %i %i %i %i \n",maxbinNumber[30],maxbinNumber[31],maxbinNumber[32],maxbinNumber[33],maxbinNumber[34],maxbinNumber[35]);
    printf ("               mdc 2 ------------ :  %i %i %i %i %i %i \n",maxbinNumber[36],maxbinNumber[37],maxbinNumber[38],maxbinNumber[39],maxbinNumber[40],maxbinNumber[41]);
    printf ("                                  :  %i %i %i %i %i %i \n",maxbinNumber[42],maxbinNumber[43],maxbinNumber[44],maxbinNumber[45],maxbinNumber[46],maxbinNumber[47]);
    printf ("                                  :  %i %i %i %i %i %i \n",maxbinNumber[48],maxbinNumber[49],maxbinNumber[50],maxbinNumber[51],maxbinNumber[52],maxbinNumber[53]);
    printf ("               mdc 3 ------------ :  %i %i %i %i %i %i \n",maxbinNumber[54],maxbinNumber[55],maxbinNumber[56],maxbinNumber[57],maxbinNumber[58],maxbinNumber[59]);
    printf ("                                  :  %i %i %i %i %i %i \n",maxbinNumber[60],maxbinNumber[61],maxbinNumber[62],maxbinNumber[63],maxbinNumber[64],maxbinNumber[65]);
    printf ("                                  :  %i %i %i %i %i %i \n",maxbinNumber[66],maxbinNumber[67],maxbinNumber[68],maxbinNumber[69],maxbinNumber[70],maxbinNumber[71]);
    printf ("Slope                             :  %3.1f ns/mm\n",getSlope()/0.1);
    printf ("MaxErrTime1                       :  %3.1f ns\n",getMaxErrTime1());
    printf ("MaxErrTime2                       :  %3.1f ns\n",getMaxErrTime2());
    printf ("Spike Types                       :  %i %i %i  %i \n",getSpikeTypes(0),getSpikeTypes(1),getSpikeTypes(2),getSpikeTypes(3));
    printf ("Spike thresholds                  :  %3.1f %3.1f\n",getSpikeThreshold1(),getSpikeThreshold2());
    printf ("MaxCopy and MinCopy               :  %i %i\n",getMaxCopy(),getMinCopy());
    printf ("Active Hists 1D                   :  %i %i %i\n",getHists_1D(0),getHists_1D(1),getHists_1D(2));
    printf ("Active Hists 2D                   :  %i %i %i\n",getHists_2D(0),getHists_2D(1),getHists_2D(2));
    printf ("RootInPut                         :  %s\n",fNameRootIn .Data());
    printf ("AsciiOutPut                       :  %s\n",fNameAsciiOut.Data());
    printf ("HistOutPut                        :  %s\n",outputHists->GetName());
}
Bool_t HMdcGarCal2Maker::check(Int_t m,Int_t t)
{
    // Checks if the Mdc m and the type t (t1,t1err,t2,t2err)
    // is active in the current setup
    Bool_t test=kFALSE;

    if(t!=-99&&m!=-99)
    {
	if(getMakeMdc(m)==1&&getMakeType(t)==1)test=kTRUE;
	else test=kFALSE;
    }
    if(t==-99&&m!=-99)
    {
	if(getMakeMdc(m)==1)test=kTRUE;
	else test=kFALSE;
    }
    if(t!=-99&&m==-99)
    {
	if(getMakeType(t)==1)test=kTRUE;
	else test=kFALSE;
    }
    return test;
}
void HMdcGarCal2Maker::calcBounderies(Int_t mdc,Int_t a,Float_t* cellEdge,Float_t* maxDist)
{
    // Calulates the bounderies of the different cell types of the MDC's underr
    // certain impact angle "a". "cellEdge" is the max distances of a track with
    // minimum distance at the cell border whereas "maxDist" is the max minimum
    // distance of a track just hitting the cell.

    Double_t cellY[4]={2.5,3.0,6.0,7.0};
    Double_t cellX[4]={2.5,2.6,4.0,5.0};
    Double_t alphaRad=((a*5)/180.*TMath::Pi());
    Double_t celledgeY=cellY[mdc]/cos(alphaRad);
    Double_t celledgeX;
    (a==0)? celledgeX=cellY[mdc] : celledgeX=cellX[mdc]/sin(alphaRad);
    (celledgeY>celledgeX)? *cellEdge=(Float_t)celledgeX : *cellEdge=(Float_t)celledgeY;

    if(a!=0)
    {
	Double_t alphaRad2=((a*5-90)/180.*TMath::Pi()); // angle of track
	Double_t b= cellX[mdc]-(tan(alphaRad2)*cellY[mdc]);
	Double_t y=-b/(tan(alphaRad2)-tan(alphaRad));
	Double_t x=tan(alphaRad)*y;
	*maxDist =(Float_t)sqrt((y*y)+(x*x));
    }
    else *maxDist=(Float_t)cellY[mdc];
}
void HMdcGarCal2Maker::make()
{
    // Main function to be called from the macro.
    cout<<"--------------------------------------------------------------"<<endl;
    printStatus();
    cout<<"--------------------------------------------------------------"<<endl;
    readInput();
    cout<<"--------------------------------------------------------------"<<endl;
    for(Int_t mdc=0;mdc<4;mdc++){
	for(Int_t type=0;type<4;type++){
            if(!check(mdc,type))continue;
	    if(getHists_2D(0)==1)fillControl2D(mdc,type,"raw");
            if(getHists_1D(3)==1)createGraph(mdc,type,0);
	}
    }
    for(Int_t mdc=0;mdc<4;mdc++){
	for(Int_t type=0;type<4;type++){
	    if(!check(mdc,type))continue;
	    if(getHists_1D(0)==1)fillControl1D(mdc,type,"raw");
	}
    }

    if(getWriteHists())
    {
       cout<<"HMdcGarCal2Maker::make()          : Control Hists Raw written to file"<<endl;
    }
    cout<<"--------------------------------------------------------------"<<endl;
    cout<<"HMdcGarCal2Maker::findMax()       : Find Max values in Time1 err"<<endl;
    cout<<"HMdcGarCal2Maker::removeSpikes()  : Find and remove spikes in Time1 err"<<endl;

    for(Int_t mdc=0;mdc<4;mdc++){
	if(!check(mdc,1))continue;
	findMax(mdc);
        removeSpikes(mdc);
	if(getHists_1D(1)==1)fillControl1D(mdc,1,"spike");
        if(getHists_2D(1)==1)fillControl2D(mdc,1,"spike");

	for(Int_t type=0;type<4;type++){
	    if(!check(mdc,type))continue;
	    if(getHists_1D(3)==1)createGraph(mdc,type,1);
	}
    }
    if(getWriteHists())
    {
	cout<<"HMdcGarCal2Maker::make()          : Control Hists Spike written to file"<<endl;
    }
    cout<<"--------------------------------------------------------------"<<endl;
    cout<<"HMdcGarCal2Maker::copyToHist()    : Copy array to temp hist"<<endl;
    cout<<"HMdcGarCal2Maker::smoothHist()    : Smooth temp Hist"<<endl;
    cout<<"HMdcGarCal2Maker::copyToArray()   : copy smoothed temp hist to array"<<endl;

    for(Int_t mdc=0;mdc<4;mdc++){
	for(Int_t type=0;type<4;type++){
	    if(!check(mdc,type))continue;


	    if(type == 0 ){
		do2dSavitzkyGolayFiltering(&mtime1_corr[mdc][0][0],18,100,kFALSE) ;
	    }
	    if(type == 1){
		//------------------------------------------------------
		// create a fake neighbour bin for time1err 2d smoothing
		Float_t smooth_time_err[19][100];
		for(Int_t a1=0;a1<18;a1++){
		    for(Int_t b=0;b<100;b++){
			smooth_time_err[a1+1][b]=mtime1_err_corr[mdc][a1][b];
		    }
		}
		for(Int_t b=0;b<100;b++){
		    smooth_time_err[0][b]=mtime1_err_corr[mdc][1][b];
		}
		//------------------------------------------------------
                Int_t ntimes = 1;
		if(mdc == 0 || mdc == 1 ) ntimes=1;
                else                      ntimes=10;
		for(Int_t n=0;n<ntimes;n++){
		    do2dSavitzkyGolayFiltering(&smooth_time_err[0][0],19,100,kFALSE) ;
		}
		//------------------------------------------------------
		// copy back to ariginal array
		for(Int_t a1=0;a1<18;a1++){
		    for(Int_t b=0;b<100;b++){
			mtime1_err_corr[mdc][a1][b] = smooth_time_err[a1+1][b];
		    }
		}
		//------------------------------------------------------

	    }

	    for(Int_t a=0;a<18;a++){
	
		TH1F *hlocal=new TH1F("temp","temp",getNBin1D_Dist(),0,getNBin1D_Dist()*0.1);

		copyToHist (hlocal,mdc,a,type);
		if(type == 2 || type == 3){
		    smoothHist (hlocal,type);
		}
		if(type == 0 || type == 1 ){
		    fitHist(hlocal,mdc,a,type);
		    if(type == 1 ) {

			Float_t cellEdge;
			Float_t maxDist;
			calcBounderies(mdc,a,&cellEdge,&maxDist);
			Int_t lastBin = 0;
			if(mdc < 2 ){
			    lastBin = Int_t(cellEdge*0.6*10);
			} else {
			    lastBin = Int_t(cellEdge*0.75*10);
			}
			HTool::smooth(hlocal,1000,1,lastBin);
		    }
		}
		replaceBins(hlocal,mdc,a,type);
		copyToArray(hlocal,mdc,a,type);
	      
		HTool::deleteObject(hlocal);
	    }
	    if(getHists_1D(2)==1)fillControl1D(mdc,type,"smooth");
	    if(getHists_2D(2)==1)fillControl2D(mdc,type,"smooth");
            if(getHists_1D(3)==1)createGraph(mdc,type,2);


	}
    }
    if(getWriteHists())
    {
	cout<<"HMdcGarCal2Maker::make()          : Control Hists Smooth written to file"<<endl;
    }
    cout<<"--------------------------------------------------------------"<<endl;
    cout<<"HMdcGarCal2Maker::plotOverLay()   : Overlay TGraph for different levels"<<endl;
    for(Int_t mdc=0;mdc<4;mdc++){
	for(Int_t type=0;type<4;type++){
	    if(!check(mdc,type))continue;
	    if(getHists_1D(3)==1)plotOverlay(mdc,type);
	}
    }
    if(getWriteHists())
    {
	cout<<"HMdcGarCal2Maker::make()          : Overlay plot written to file"<<endl;
    }
    cout<<"--------------------------------------------------------------"<<endl;
    writeAscii();
    cout<<"--------------------------------------------------------------"<<endl;
  

}
void HMdcGarCal2Maker::fillArrays(TH1F* htime1,TH1F* htime2,Int_t mdctype,Int_t angle)
{
    // Fills values for time1, time2 and the corresponding errors from the hists into
    // working arrays.
    if(getVersion()==1)
    {
	for(Int_t sample=0;sample<100;sample++)
	{
	    if(sample<=maxbinNumber[(mdctype*18)+angle])
	    {  // fill all valid values

		//######################################## FILL MATRIX ############################################################
		if(sample==1)
		{  // fill first bin
		    mtime1    [mdctype][angle][0]=htime1->GetBinContent(sample);
		    mtime2    [mdctype][angle][0]=htime2->GetBinContent(sample);
		    mtime1_err[mdctype][angle][0]=htime1->GetBinError(sample);
		    mtime2_err[mdctype][angle][0]=htime2->GetBinError(sample);

		    mtime1_corr    [mdctype][angle][0] = mtime1    [mdctype][angle][0];
		    mtime1_err_corr[mdctype][angle][0] = mtime1_err[mdctype][angle][0];
		    mtime2_corr    [mdctype][angle][0] = mtime2    [mdctype][angle][0];
		    mtime2_err_corr[mdctype][angle][0] = mtime2_err[mdctype][angle][0];

		}

		if( (htime1->GetBinContent(sample))<getCutMax(mdctype))
		{   // fill if smaller than max time
		    mtime1    [mdctype][angle][sample]=htime1->GetBinContent(sample);
		    mtime2    [mdctype][angle][sample]=htime2->GetBinContent(sample);

		    mtime1_err[mdctype][angle][sample]=htime1->GetBinError(sample);
		    mtime2_err[mdctype][angle][sample]=htime2->GetBinError(sample);

		    lastvalidBin[mdctype][angle]=sample;
		}
		else
		{
		    if (lastvalidBin[mdctype][angle]<=getMaxBinNumber((mdctype*18)+angle))
		    {   // fill range between lastvalid and maxnumber of bins

			mtime1    [mdctype][angle][sample]=getCutMax(mdctype) +(lastvalidBin[mdctype][angle]-sample)*-getSlope();
			mtime2    [mdctype][angle][sample]=getCutMax(mdctype) +(lastvalidBin[mdctype][angle]-sample)*-getSlope();

			mtime1_err[mdctype][angle][sample]=mtime1_err[mdctype][angle][lastvalidBin[mdctype][angle]];
			mtime2_err[mdctype][angle][sample]=mtime2_err[mdctype][angle][lastvalidBin[mdctype][angle]];
		    }
		}
		//#################################################################################################################

	    }
	    else
	    {   // fill rest of Matrix
		// fill range larger max number of bins

		mtime1    [mdctype][angle][sample]=mtime1[mdctype][angle][getMaxBinNumber((mdctype*18)+angle)] + (getMaxBinNumber((mdctype*18)+angle)-sample)*-getSlope();
		mtime2    [mdctype][angle][sample]=mtime2[mdctype][angle][getMaxBinNumber((mdctype*18)+angle)] + (getMaxBinNumber((mdctype*18)+angle)-sample)*-getSlope();

		if(htime1->GetBinError(getMaxBinNumber((mdctype*18)+angle))>0)
		{
		    mtime1_err[mdctype][angle][sample]=getMaxErrTime1();
		}
		else
		{
		    mtime1_err[mdctype][angle][sample]=getMaxErrTime1();
		}
		if(htime2->GetBinError(getMaxBinNumber((mdctype*18)+angle))>0)
		{
		    mtime2_err[mdctype][angle][sample]=getMaxErrTime2();
		}
		else
		{
		    mtime2_err[mdctype][angle][sample]=getMaxErrTime2();
		}
		//#################################################################################################################
	    }

	    //coppy array
	    mtime1_corr    [mdctype][angle][sample] = mtime1    [mdctype][angle][sample];
	    mtime1_err_corr[mdctype][angle][sample] = mtime1_err[mdctype][angle][sample];
	    mtime2_corr    [mdctype][angle][sample] = mtime2    [mdctype][angle][sample];
	    mtime2_err_corr[mdctype][angle][sample] = mtime2_err[mdctype][angle][sample];
	}
    }
    if(getVersion()==2)
    {
	for(Int_t sample=0;sample<100;sample++)
	{
	    if(sample<=maxbinNumber[(mdctype*18)+angle])
	    {  // fill all valid values

		//######################################## FILL MATRIX ############################################################
		if(sample==1)
		{  // fill first bin
		    mtime1    [mdctype][angle][0]=htime1->GetBinContent(sample+1);
		    mtime2    [mdctype][angle][0]=htime2->GetBinContent(sample+1);
		    mtime1_err[mdctype][angle][0]=htime1->GetBinError(sample+1);
		    mtime2_err[mdctype][angle][0]=htime2->GetBinError(sample+1);

		    mtime1_corr    [mdctype][angle][0] = mtime1    [mdctype][angle][0];
		    mtime1_err_corr[mdctype][angle][0] = mtime1_err[mdctype][angle][0];
		    mtime2_corr    [mdctype][angle][0] = mtime2    [mdctype][angle][0];
		    mtime2_err_corr[mdctype][angle][0] = mtime2_err[mdctype][angle][0];

		}

		if( (htime1->GetBinContent(sample+1))<getCutMax(mdctype))
		{   // fill if smaller than max time
		    mtime1    [mdctype][angle][sample]=htime1->GetBinContent(sample+1);
		    mtime2    [mdctype][angle][sample]=htime2->GetBinContent(sample+1);

		    mtime1_err[mdctype][angle][sample]=htime1->GetBinError(sample+1);
		    mtime2_err[mdctype][angle][sample]=htime2->GetBinError(sample+1);

		    lastvalidBin[mdctype][angle]=sample;
		}
		else
		{
		    if (lastvalidBin[mdctype][angle]<=getMaxBinNumber((mdctype*18)+angle))
		    {   // fill range between lastvalid and maxnumber of bins

			mtime1    [mdctype][angle][sample]=getCutMax(mdctype) +(lastvalidBin[mdctype][angle]-sample)*-getSlope();
			mtime2    [mdctype][angle][sample]=getCutMax(mdctype) +(lastvalidBin[mdctype][angle]-sample)*-getSlope();

			mtime1_err[mdctype][angle][sample]=mtime1_err[mdctype][angle][lastvalidBin[mdctype][angle]];
			mtime2_err[mdctype][angle][sample]=mtime2_err[mdctype][angle][lastvalidBin[mdctype][angle]];
		    }
		}
		//#################################################################################################################

	    }
	    else
	    {   // fill rest of Matrix
		// fill range larger max number of bins

		mtime1    [mdctype][angle][sample]=mtime1[mdctype][angle][getMaxBinNumber((mdctype*18)+angle)] + (getMaxBinNumber((mdctype*18)+angle)-sample)*-getSlope();
		mtime2    [mdctype][angle][sample]=mtime2[mdctype][angle][getMaxBinNumber((mdctype*18)+angle)] + (getMaxBinNumber((mdctype*18)+angle)-sample)*-getSlope();

		if(htime1->GetBinError(getMaxBinNumber((mdctype*18)+angle +1))>0)
		{
		    mtime1_err[mdctype][angle][sample]=getMaxErrTime1();
		}
		else
		{
		    mtime1_err[mdctype][angle][sample]=getMaxErrTime1();
		}
		if(htime2->GetBinError(getMaxBinNumber((mdctype*18)+angle +1))>0)
		{
		    mtime2_err[mdctype][angle][sample]=getMaxErrTime2();
		}
		else
		{
		    mtime2_err[mdctype][angle][sample]=getMaxErrTime2();
		}
		//#################################################################################################################
	    }

	    //coppy array
	    mtime1_corr    [mdctype][angle][sample] = mtime1    [mdctype][angle][sample];
	    mtime1_err_corr[mdctype][angle][sample] = mtime1_err[mdctype][angle][sample];
	    mtime2_corr    [mdctype][angle][sample] = mtime2    [mdctype][angle][sample];
	    mtime2_err_corr[mdctype][angle][sample] = mtime2_err[mdctype][angle][sample];
	}
    }


}
void HMdcGarCal2Maker::findMax(Int_t m)
{
    // finds maximum values inside the array of
    // time1err and replacing values after max by
    // a constant value.

    Float_t cellEdge;
    Float_t maxDist;
    Int_t   startBin;



    for(Int_t a=0;a<18;a++)
    {
        // start finding spikes at 80% of the drift
        // cell size (10 bins = 1 mm)
        calcBounderies(m,a,&cellEdge,&maxDist);
	startBin=Int_t(cellEdge*0.8*10);

	// reset arrays
	max1[m][a] = -1.;
        max2[m][a] = -1.;
	max1bin[m][a] = 100;
	max2bin[m][a] = 100;

	// Max1
	for(Int_t i=startBin;i<100;i++)
	{   // find max1 and max1bin
	    if(mtime1_err_corr[m][a][i]>max1[m][a])
	    {
		max1   [m][a]=mtime1_err_corr[m][a][i];
		max1bin[m][a]=i;
	    }
	}
	for(Int_t i=max1bin[m][a];i<100;i++)
	{   // replace bins >max1bin with max1 value
	    mtime1_err_corr[m][a][i]=max1[m][a];
	}

	for(Int_t i=0;i<100;i++)
	{   // replace bins >maxErrTime1 by maxErrTime1
	    if(mtime1_err_corr[m][a][i]>getMaxErrTime1()){
               mtime1_err_corr[m][a][i]=getMaxErrTime1();
	    }
	}
    }
}

void HMdcGarCal2Maker::removeSpikes(Int_t m)
{
    // Removes spikes from time1err

    //-------------------remove spikes -------------------------------
    Float_t slopeSpike=0;
    Float_t oldslopeSpike=0;
    if(!getPrintMode())cout<<"mdc "<<m<<" :"<<flush;
     
    for(Int_t a=0;a<18;a++)
    {
        if(!getPrintMode())cout<<" "<<a<<flush;
        if( getPrintMode())cout<<m<<" "<<a<<endl;

	for(Int_t i=1;i<99;i++)
	{
	    oldslopeSpike= mtime1_err_corr[m][a][i]  -mtime1_err_corr[m][a][i-1];
	    slopeSpike   = mtime1_err_corr[m][a][i+1]-mtime1_err_corr[m][a][i];

	    if((slopeSpike<0 && oldslopeSpike>0) &&
	       (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i])  >getSpikeThreshold1()) &&
	       (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+1])<getSpikeThreshold2()) &&
                i<98
	      )
	    {// 1 bin spike

		if(getPrintMode())cout<<"     1bin "<<i
			<<"\n      y[i-1] "<<mtime1_err_corr[m][a][i-1]
			<<"\n      y[i]   "<<mtime1_err_corr[m][a][i]
			<<"\n      y[i+1] "<<mtime1_err_corr[m][a][i+1]
			<<endl;

		mtime1_err_corr[m][a][i]=(mtime1_err_corr[m][a][i-1]+mtime1_err_corr[m][a][i+1])*0.5;

		if(getPrintMode())cout<<"     1bin "<<i
			<<"\n      y[i-1] "<<mtime1_err_corr[m][a][i-1]
			<<"\n      y[i]   "<<mtime1_err_corr[m][a][i]
			<<"\n      y[i+1] "<<mtime1_err_corr[m][a][i+1]
			<<endl;
	    }
	    if(i<98)
	    {
		oldslopeSpike= mtime1_err_corr[m][a][i]  -mtime1_err_corr[m][a][i-1];
		slopeSpike   = mtime1_err_corr[m][a][i+2]-mtime1_err_corr[m][a][i];

		if( (slopeSpike<0 && oldslopeSpike>0) &&
		   (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i])  >getSpikeThreshold1()) &&
		   (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+1])>getSpikeThreshold1()) &&
		   (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+2])<getSpikeThreshold2()) &&
		   i<97)
		{ // 2 bin spike

		    if(getPrintMode())cout<<"     2bin "<<i
			<<"\n      y[i-1] "<<mtime1_err_corr[m][a][i-1]
			    <<"\n      y[i]   "<<mtime1_err_corr[m][a][i]
			    <<"\n      y[i+1] "<<mtime1_err_corr[m][a][i+1]
			    <<"\n      y[i+2] "<<mtime1_err_corr[m][a][i+2]
			    <<endl;

		    mtime1_err_corr[m][a][i]  =mtime1_err_corr[m][a][i-1]-(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+2])*0.3333;
		    mtime1_err_corr[m][a][i+1]=mtime1_err_corr[m][a][i-1]-(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+2])*0.3333*2;

		    if(getPrintMode())cout<<"     2bin "<<i
			<<"\n      y[i-1] "<<mtime1_err_corr[m][a][i-1]
			    <<"\n      y[i]   "<<mtime1_err_corr[m][a][i]
			    <<"\n      y[i+1] "<<mtime1_err_corr[m][a][i+1]
			    <<"\n      y[i+2] "<<mtime1_err_corr[m][a][i+2]
			    <<endl;

		}
		if(i<97)
		{
		    oldslopeSpike= mtime1_err_corr[m][a][i]  -mtime1_err_corr[m][a][i-1];
		    slopeSpike   = mtime1_err_corr[m][a][i+3]-mtime1_err_corr[m][a][i];

		    if(  (slopeSpike<0 && oldslopeSpike>0) &&
		       (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i])  >getSpikeThreshold1()) &&
		       (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+1])>getSpikeThreshold1()) &&
		       (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+2])>getSpikeThreshold1()) &&
		       (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+3])<getSpikeThreshold2()) &&
		       i<96)
		    { // 3 bin spike

			if(getPrintMode())cout<<"     3bin "<<i
			    <<"\n      y[i-1] "<<mtime1_err_corr[m][a][i-1]
				<<"\n      y[i]   "<<mtime1_err_corr[m][a][i]
				<<"\n      y[i+1] "<<mtime1_err_corr[m][a][i+1]
				<<"\n      y[i+2] "<<mtime1_err_corr[m][a][i+2]
				<<"\n      y[i+3] "<<mtime1_err_corr[m][a][i+3]
				<<endl;

			mtime1_err_corr[m][a][i]  =mtime1_err_corr[m][a][i-1]-(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+3])*0.25;
			mtime1_err_corr[m][a][i+1]=mtime1_err_corr[m][a][i-1]-(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+3])*0.25*2;
			mtime1_err_corr[m][a][i+2]=mtime1_err_corr[m][a][i-1]-(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+3])*0.25*3;

			if(getPrintMode())cout<<"     3bin "<<i
			    <<"\n      y[i-1] "<<mtime1_err_corr[m][a][i-1]
				<<"\n      y[i]   "<<mtime1_err_corr[m][a][i]
				<<"\n      y[i+1] "<<mtime1_err_corr[m][a][i+1]
				<<"\n      y[i+2] "<<mtime1_err_corr[m][a][i+2]
				<<"\n      y[i+3] "<<mtime1_err_corr[m][a][i+3]
				<<endl;
		    }

		    if(i<96)
		    {
			oldslopeSpike= mtime1_err_corr[m][a][i]  -mtime1_err_corr[m][a][i-1];
			slopeSpike   = mtime1_err_corr[m][a][i+4]-mtime1_err_corr[m][a][i];

			if(  (slopeSpike<0 && oldslopeSpike>0) &&
			   (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i])  >getSpikeThreshold1()) &&
			   (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+1])>getSpikeThreshold1()) &&
			   (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+2])>getSpikeThreshold1()) &&
			   (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+3])>getSpikeThreshold1()) &&
			   (fabs(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+4])<getSpikeThreshold2()) &&
			   i<95)
			{ // 4 bin spike

			    if(getPrintMode())cout<<"     4bin "<<i
				<<"\n      y[i-1] "<<mtime1_err_corr[m][a][i-1]
				    <<"\n      y[i]   "<<mtime1_err_corr[m][a][i]
				    <<"\n      y[i+1] "<<mtime1_err_corr[m][a][i+1]
				    <<"\n      y[i+2] "<<mtime1_err_corr[m][a][i+2]
				    <<"\n      y[i+3] "<<mtime1_err_corr[m][a][i+3]
				    <<"\n      y[i+4] "<<mtime1_err_corr[m][a][i+4]
				    <<endl;

			    mtime1_err_corr[m][a][i]  =mtime1_err_corr[m][a][i-1]-(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+4])*0.2;
			    mtime1_err_corr[m][a][i+1]=mtime1_err_corr[m][a][i-1]-(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+4])*0.2*2;
			    mtime1_err_corr[m][a][i+2]=mtime1_err_corr[m][a][i-1]-(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+4])*0.2*3;
			    mtime1_err_corr[m][a][i+3]=mtime1_err_corr[m][a][i-1]-(mtime1_err_corr[m][a][i-1]-mtime1_err_corr[m][a][i+4])*0.2*4;

			    if(getPrintMode())cout<<"     4bin "<<i
				<<"\n      y[i-1] "<<mtime1_err_corr[m][a][i-1]
				    <<"\n      y[i]   "<<mtime1_err_corr[m][a][i]
				    <<"\n      y[i+1] "<<mtime1_err_corr[m][a][i+1]
				    <<"\n      y[i+2] "<<mtime1_err_corr[m][a][i+2]
				    <<"\n      y[i+3] "<<mtime1_err_corr[m][a][i+3]
				    <<"\n      y[i+4] "<<mtime1_err_corr[m][a][i+4]
				    <<endl;
			}
		    } //<96
		} // <97
	    } // <98
	}
    }
    if(!getPrintMode())cout<<" "<<endl;
}

void HMdcGarCal2Maker::copyToHist(TH1F* h,Int_t m,Int_t a,Int_t type)
{
    // Copys workings arrays to hists
    if(type==0)for(Int_t i=0;i<100;i++){
	h->SetBinContent(i+1,mtime1_corr    [m][a][i]);
    }
    if(type==1)for(Int_t i=0;i<100;i++){
	h->SetBinContent(i+1,mtime1_err_corr[m][a][i]);
    }
    if(type==2)for(Int_t i=0;i<100;i++){
	h->SetBinContent(i+1,mtime2_corr    [m][a][i]);
    }
    if(type==3)for(Int_t i=0;i<100;i++){
	h->SetBinContent(i+1,mtime2_err_corr[m][a][i]);
    }
}
void HMdcGarCal2Maker::copyToArray(TH1F* h,Int_t m,Int_t a,Int_t type)
{
    // Copy working hists to arrays
    if(type==0)for(Int_t i=0;i<100;i++){
	mtime1_corr    [m][a][i]=h->GetBinContent(i+1);

	if(mtime1_corr[m][a][i]==0&& i<1)
	{
	    Warning("HMdcGarCal2Maker::copyToArray()",
		    "Bin content 0 in time 1 of %i %02i %02i",m,a,i);
	}
    }
    if(type==1)for(Int_t i=0;i<100;i++){
	mtime1_err_corr[m][a][i]=h->GetBinContent(i+1);

	if(mtime1_err_corr[m][a][i]==0)
	{
	    Warning("HMdcGarCal2Maker::copyToArray()",
		    "Bin content 0 in time1 err of %i %02i %02i",m,a,i);
	}
    }
    if(type==2)for(Int_t i=0;i<100;i++){
	mtime2_corr    [m][a][i]=h->GetBinContent(i+1);

	if(mtime2_corr[m][a][i]==0)
	{
	    Warning("HMdcGarCal2Maker::copyToArray()",
		    "Bin content 0 in time2  of %i %02i %02i",m,a,i);
	}
    }
    if(type==3)for(Int_t i=0;i<100;i++){
	mtime2_err_corr[m][a][i]=h->GetBinContent(i+1);

	if(mtime2_err_corr[m][a][i]==0)
	{
	    Warning("HMdcGarCal2Maker::copyToArray()",
		    "Bin content 0 in time2 err  of %i %02i %02i",m,a,i);
	}
    }
}
Float_t HMdcGarCal2Maker::coefSavGol2(Int_t i, Int_t j, Int_t k, Int_t ni, Int_t nj, Int_t nk) {
    //
    // Compute 2nd order Savitzky-Golay polynomial coefficient c=c(i,j,k,ni,nj,nk)
    //
    if (abs(i)>ni || abs(j)>nj || abs(k)>nk) return 0.;
    Float_t Nijk = (2*ni+1)*(2*nj+1)*(2*nk+1);
    Float_t gi = (ni*(ni+1))/3.;
    Float_t gj = (nj*(nj+1))/3.;
    Float_t gk = (nk*(nk+1))/3.;
    Float_t termi = 5.*((Float_t)(i*i)-gi)/(4.*gi-1.);
    Float_t termj = 5.*((Float_t)(j*j)-gj)/(4.*gj-1.);
    Float_t termk = 5.*((Float_t)(k*k)-gk)/(4.*gk-1.);
    return (1.-termi-termj-termk)/Nijk;
}

void HMdcGarCal2Maker::do2dSavitzkyGolayFiltering(Float_t *data, Int_t xchan, Int_t ychan, Bool_t clip) {
    //
    // Do a 2dim symmetric second-order Savitzky-Golay noise filtering on data
    //
    //   - 2dim, because data=data(i,j)
    //   - symmetric, because nR = nL = n
    //   - second-order, because m=2
    //
    Int_t n=2;                          // fix filter window to [-2,2]
    Float_t work[xchan][ychan];         // temporary work matrix
    Float_t c[2*n+1][2*n+1];            // coefficient matrix

    Float_t sum = 0.;
    for(Int_t i=-n; i<=n; i++) {        // tabulate filter coefficients
	for(Int_t j=-n; j<=n; j++) {
	    sum += c[i+n][j+n] = coefSavGol2(i,j,0,n,n,0);
	}
    }
    if(sum != 1){
	cout << "Sum of Savitzky-Golay coefficients Not 1 : " << sum <<endl;
    }
    Float_t val;
    Int_t ic, jc;
    for(Int_t i=n; i<xchan-n; i++) {     // loop over data
	for(Int_t j=n; j<ychan-n; j++) {   // and filter
	    val = 0;
	    ic = -n;
	    for(Int_t ip=i-n; ip<=i+n; ip++, ic++) {
		jc = -n;
		for(Int_t jp=j-n; jp<=j+n; jp++, jc++) {
		    val += c[ic+n][jc+n]*data[ip*ychan+jp];
		}
	    }
	    work[i][j] = val;
	}
    }

    for(Int_t i=n; i<xchan-n; i++) {    // clip and copy back filtered data
	for(Int_t j=n; j<ychan-n; j++) {
	    if (clip) data[i*ychan+j] = TMath::Max(TMath::Min(work[i][j],1.0F),0.F);
	    else data[i*ychan+j] = work[i][j];
	}
    }

    return;
}

void HMdcGarCal2Maker::smoothHist(TH1F* h,Int_t type)
{
    // Smooth working hists by number of cycles defined
    if(type==0) {
	HTool::smooth(h,getNTimes(type));
	HTool::smooth(h,1000,1,10);
    }
    if(type==1) HTool::smooth(h,getNTimes(type));
    if(type==2) HTool::smooth(h,getNTimes(type));
    //if(type==3) HTool::smooth(h,getNTimes(type));
}
void HMdcGarCal2Maker::fitHist(TH1F* h,Int_t mdc,Int_t a, Int_t type)
{
    // Smooth time1 errors by fit function

    Float_t cellEdge;
    Float_t maxDist;
    Int_t   nrange   = 0;
    calcBounderies(mdc,a,&cellEdge,&maxDist);
    TF1* fit = 0;

    if(type == 0){
        // for time1 replace the firts 3 bins by fit values

	nrange = 11;
	fit = new TF1("fit","pol2");

	Float_t x1,x2;
	x1 = h->GetXaxis()->GetBinCenter( 3);
	x2 = h->GetXaxis()->GetBinCenter( nrange + 1);
	fit->SetRange(x1,x2);
	h->Fit(fit,"NQR");

	for(Int_t bin = 0; bin < 3; bin ++)
	{   // now replace the bin in range by fitted values
	    h->SetBinContent(bin + 1,fit->Eval(h->GetXaxis()->GetBinCenter(bin+1)));
	}

    }

    if(type == 1){

        nrange = (Int_t)(cellEdge*1.*10*.3);	fit = new TF1("fit","pol2");

	Float_t x1,x2;
	x1 = h->GetXaxis()->GetBinCenter(1);
	x2 = h->GetXaxis()->GetBinCenter(nrange + 1);
	fit->SetRange(x1,x2);
	h->Fit(fit,"NQR");

	for(Int_t bin = 0; bin <= nrange; bin ++)
	{   // now replace the bin in range by fitted values
	    h->SetBinContent(bin + 1,fit->Eval(h->GetXaxis()->GetBinCenter(bin+1)));
	}
    }
    delete fit;
}
void HMdcGarCal2Maker::replaceBins(TH1F* h,Int_t m,Int_t a,Int_t type)
{
    // Replace the first bins and the bins after the maximum bin of the
    // workings hists after smoothing by the original bins of the working arrays.
    if(type==1)
    {
	for(Int_t i=0;i<100;i++){ // do not allow over shoot
	    if(h->GetBinContent(i+1)>getMaxErrTime1()){
		h->SetBinContent(i+1,getMaxErrTime1());
	    }
	}
    }
    if(type==2)
    {
	for(Int_t i=0;i<getMaxCopy();i++){// first bins
	    h->SetBinContent(i+1,mtime2_corr[m][a][i]);
	}
        for(Int_t i=getMinCopy();i<100;i++){// bins after max
	    h->SetBinContent(i+1,mtime2_corr[m][a][i]);
	}
    }
}
void HMdcGarCal2Maker::createGraph(Int_t m,Int_t t,Int_t l)
{
    // Create Graphs , fill them with the working array and write
    // them to disk
    gStyle->SetPalette(50);
    Float_t x[100];
    Float_t y[100];

    TDirectory *dirTemp=NULL;

    if(getWriteHists())
    {
	outputHists->cd();
	if (outputHists) dirTemp=HTool::Mkdir(outputHists, "module",m);
    }

    for(Int_t a=0;a<18;a++){
	if(t==0)for(Int_t i=0;i<100;i++){
	    y[i]=mtime1_corr[m][a][i];
	    x[i]=0.1*i;
	}
	if(t==1)for(Int_t i=0;i<100;i++){
	    y[i]=mtime1_err_corr[m][a][i];
	    x[i]=0.1*i;
	}
	if(t==2)for(Int_t i=0;i<100;i++){
	    y[i]=mtime2_corr[m][a][i];
	    x[i]=0.1*i;
	}
	if(t==3)for(Int_t i=0;i<100;i++){
	    y[i]=mtime2_err_corr[m][a][i];
	    x[i]=0.1*i;
	}

	Char_t namegraph[300];

	sprintf(namegraph,"%s%i%s%02i%s%i%s%i%s","g[",m,"][",a,"][",t,"][",l,"]");
	g[m][a][t][l]=new TGraph(100,x,y);
	g[m][a][t][l]->SetName(namegraph);
	g[m][a][t][l]->SetLineColor(getLevelColors(l));

	if(getWriteHists())
	{
	    HTool::writeObject(g[m][a][t][l]);
	}
    }
    if(getWriteHists())
    {
	dirTemp->TDirectory::Cd("..");
    }
}
void HMdcGarCal2Maker::fillControl2D(Int_t mdctype,Int_t type,TString context)
{
    // Fills a 2D Control hist from the working arrays and writes the hist to disk.
    if(type==0)cout<<"HMdcGarCal2Maker::fillControl2D() : Filling Hist for MDC"<<mdctype<<" time1"<<endl;
    if(type==1)cout<<"HMdcGarCal2Maker::fillControl2D() : Filling Hist for MDC"<<mdctype<<" error time1"<<endl;
    if(type==2)cout<<"HMdcGarCal2Maker::fillControl2D() : Filling Hist for MDC"<<mdctype<<" time1"<<endl;
    if(type==3)cout<<"HMdcGarCal2Maker::fillControl2D() : Filling Hist for MDC"<<mdctype<<" error time1"<<endl;


    if(getWriteHists()) outputHists->cd();

    gStyle->SetPalette(1);
    if(getBatchMode())gROOT->SetBatch();

    Char_t namecanvas[300];
    TString histtype;
    if(type==0)histtype="2D time1";
    if(type==1)histtype="2D error of time1";
    if(type==2)histtype="2D time2";
    if(type==3)histtype="2D error of time2";

    sprintf(namecanvas,"%s%s%i%s%s",histtype.Data()," for MDC",mdctype," after ",context.Data());
    TCanvas *result =new TCanvas(namecanvas ,namecanvas,1000,800);
    result->cd();
    result->Draw();

    Char_t namematrix[200];

    if(type==0)sprintf(namematrix,"%s%i%s%s","MDC",mdctype,"_t1_",context.Data());
    if(type==1)sprintf(namematrix,"%s%i%s%s","MDC",mdctype,"_t1err_",context.Data());
    if(type==2)sprintf(namematrix,"%s%i%s%s","MDC",mdctype,"_t2_",context.Data());
    if(type==3)sprintf(namematrix,"%s%i%s%s","MDC",mdctype,"_t2err_",context.Data());

    TH2F* hmatrix;

    Char_t nameHist[200];
    sprintf(nameHist,"%s%i%s%i%s%s","hmatrix[",mdctype,"][",type,"]_",context.Data());

    hmatrix=new TH2F(nameHist,namematrix,getNBin2D_Angle(),0,getNBin2D_Angle(),getNBin2D_Dist(),0,getNBin2D_Dist()*0.1);
    hmatrix->SetXTitle("angle [step(5 degree)]");
    hmatrix->SetYTitle("distance from wire [mm]");
    hmatrix->SetOption("lego2");

    if(type==0 || type==2)hmatrix->SetZTitle("drift time [ns]");
    if(type==1 || type==3)hmatrix->SetZTitle("error of drift time [ns]");

    hmatrix->SetLabelSize(0.03,"x");
    hmatrix->SetLabelSize(0.03,"y");
    hmatrix->SetLabelSize(0.03,"z");
    hmatrix->SetTitleOffset(1.5,"x");
    hmatrix->SetTitleOffset(1.5,"y");
    hmatrix->SetTitleOffset(1. ,"z");

    for(Int_t angle=0;angle<18;angle++)
    {
	for(Int_t sample=0;sample<100;sample++)
	{
	    if(type==0)hmatrix->SetBinContent(angle+1,sample+1,mtime1_corr    [mdctype][angle][sample]);
	    if(type==1)hmatrix->SetBinContent(angle+1,sample+1,mtime1_err_corr[mdctype][angle][sample]);
	    if(type==2)hmatrix->SetBinContent(angle+1,sample+1,mtime2_corr    [mdctype][angle][sample]);
	    if(type==3)hmatrix->SetBinContent(angle+1,sample+1,mtime2_err_corr[mdctype][angle][sample]);
	}
    }
    hmatrix->DrawCopy();

    if(getWriteHists())
    {
	HTool::writeObject(hmatrix);
    }

    HTool::deleteObject(hmatrix);
}
void HMdcGarCal2Maker::fillControl1D(Int_t mdctype,Int_t type,TString context)
{
    // Fills 1D Control hists (for each impact angle) and writes the hists to disk.
    if(type==0)cout<<"HMdcGarCal2Maker::fillControl1D() : Filling Hist for MDC"<<mdctype<<" time1"<<endl;
    if(type==1)cout<<"HMdcGarCal2Maker::fillControl1D() : Filling Hist for MDC"<<mdctype<<" error time1"<<endl;
    if(type==2)cout<<"HMdcGarCal2Maker::fillControl1D() : Filling Hist for MDC"<<mdctype<<" time1"<<endl;
    if(type==3)cout<<"HMdcGarCal2Maker::fillControl1D() : Filling Hist for MDC"<<mdctype<<" error time1"<<endl;

    TDirectory *dirTemp=NULL;

    if(getWriteHists())
    {
	outputHists->cd();
	if (outputHists) dirTemp=HTool::Mkdir(outputHists, "module",mdctype);
    }
    gStyle->SetPalette(50);
    if(getBatchMode())gROOT->SetBatch();

    Char_t namecanvas[300];
    TString histtype;
    if(type==0)histtype="1D time1";
    if(type==1)histtype="1D error of time1";
    if(type==2)histtype="1D time2";
    if(type==3)histtype="1D error of time2";

    sprintf(namecanvas,"%s%s%i%s%s",histtype.Data()," for MDC",mdctype," after ",context.Data());
    TCanvas *result =new TCanvas(namecanvas ,namecanvas,1000,800);
    result->Divide(6,3);
    result->cd();
    result->Draw();

    Char_t namematrix[200];

    if(type==0)sprintf(namematrix,"%s%i%s%s","MDC",mdctype,"_t1_"   ,context.Data());
    if(type==1)sprintf(namematrix,"%s%i%s%s","MDC",mdctype,"_t1err_",context.Data());
    if(type==2)sprintf(namematrix,"%s%i%s%s","MDC",mdctype,"_t2_"   ,context.Data());
    if(type==3)sprintf(namematrix,"%s%i%s%s","MDC",mdctype,"_t2err_",context.Data());

    TH1F* h[18];

    Char_t nameHist[200];
 
    for(Int_t angle=0;angle<18;angle++)
    {
	sprintf(nameHist,"%s%i%s%i%s%02i%s%s","h[",mdctype,"][",type,"][",angle,"]_",context.Data());

	//h[angle]=new TH1F(nameHist,namematrix,15,0,15*0.1);
	h[angle]=new TH1F(nameHist,namematrix,getNBin1D_Dist(),0,getNBin1D_Dist()*0.1);
	h[angle]->SetYTitle("distance from wire [mm]");
	h[angle]->SetMarkerStyle(8);
	h[angle]->SetMarkerSize(0.5);

	h[angle]->SetOption("LP");
       
      
	if(type==0 || type==1)h[angle]->SetMarkerColor(2);
        if(type==2 || type==3)h[angle]->SetMarkerColor(4);
	if(type==0 || type==2)h[angle]->SetYTitle("drift time [ns]");
	if(type==1 || type==3)h[angle]->SetYTitle("error of drift time [ns]");

	for(Int_t sample=0;sample<100;sample++)
	{
	    if(type==0)h[angle]->SetBinContent(sample+1,mtime1_corr    [mdctype][angle][sample]);
	    if(type==1)h[angle]->SetBinContent(sample+1,mtime1_err_corr[mdctype][angle][sample]);
	    if(type==2)h[angle]->SetBinContent(sample+1,mtime2_corr    [mdctype][angle][sample]);
	    if(type==3)h[angle]->SetBinContent(sample+1,mtime2_err_corr[mdctype][angle][sample]);
	}
        result->cd(angle+1);

	if(type==1||type==3)h[angle]->SetMaximum(100);
        if(type==0||type==2)h[angle]->SetMaximum(1000); // 1000

	h[angle]->DrawCopy();
        h[angle]->SetOption("");
	h[angle]->DrawCopy("same");

	drawBorders(mdctype,angle,type);

	if(getWriteHists())
	{
	    HTool::writeObject(h[angle]);
	}
	HTool::deleteObject(h[angle]);
    }
    if(getWriteHists())
    {
	HTool::writeObject(result);
	dirTemp->TDirectory::Cd("..");
    }
}
void HMdcGarCal2Maker::plotOverlay(Int_t mdctype,Int_t type)
{
    // Plots overlay of Graphs after the differrent working steps
    // (raw, spike remove, smooth and replacement of bins).
    if(type==0)cout<<"HMdcGarCal2Maker::plotOverLay()   : Overlaying Graphs for MDC"<<mdctype<<" time1"<<endl;
    if(type==1)cout<<"HMdcGarCal2Maker::plotOverLay()   : Overlaying Graphs for MDC"<<mdctype<<" error time1"<<endl;
    if(type==2)cout<<"HMdcGarCal2Maker::plotOverLay()   : Overlaying Graphs for MDC"<<mdctype<<" time1"<<endl;
    if(type==3)cout<<"HMdcGarCal2Maker::plotOverLay()   : Overlaying Graphs for MDC"<<mdctype<<" error time1"<<endl;

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    TH1F* dummy=new TH1F("dummy","",getNBin1D_Dist(),0,getNBin1D_Dist()*0.1);
    if(type==0||type==2)dummy->SetMaximum(1000);
    if(type==1||type==3)dummy->SetMaximum(100);

    TDirectory *dirTemp=NULL;

    if(getWriteHists())
    {
	outputHists->cd();
	if (outputHists) dirTemp=HTool::Mkdir(outputHists, "module",mdctype);
    }
    gStyle->SetPalette(50);
    if(getBatchMode())gROOT->SetBatch();

    Char_t namecanvas[300];
    TString histtype;
    if(type==0)histtype="Overlay 1D time1";
    if(type==1)histtype="Overlay 1D error of time1";
    if(type==2)histtype="Overlay 1D time2";
    if(type==3)histtype="Overlay 1D error of time2";

    sprintf(namecanvas,"%s%s%i",histtype.Data()," for MDC",mdctype);
    TCanvas *result =new TCanvas(namecanvas ,namecanvas,1000,800);
    result->Divide(6,3);
    result->cd();
    result->Draw();
    for(Int_t angle=0;angle<18;angle++)
    {
	result->cd(angle+1);
        dummy->DrawCopy();
	for(Int_t l=0;l<3;l++)
	{
	    if(g[mdctype][angle][type][l])
	    {
		g[mdctype][angle][type][l]->SetLineColor(getLevelColors(l));
		g[mdctype][angle][type][l]->Draw();
	    }
	}
        drawBorders(mdctype,angle,type);
    }
    if(getWriteHists())
    {
	HTool::writeObject(result);
	dirTemp->TDirectory::Cd("..");
    }
    HTool::deleteObject(dummy);
}
void HMdcGarCal2Maker::drawBorders(Int_t mdctype,Int_t angle,Int_t type)
{
    // Draw lines for max value of Celledge / max distance
    Float_t celledge,maxDist;
    calcBounderies(mdctype,angle,&celledge,&maxDist);
    Float_t upperY=0;
    if(type==1||type==3)upperY=100;
    if(type==0||type==2)upperY=1000;

    TLine *edge=new TLine(celledge,0,celledge,upperY);
    edge->SetLineColor(4);
    edge->Draw("same");

    TLine *maxdist=new TLine(maxDist,0,maxDist,upperY);
    maxdist->SetLineColor(2);
    maxdist->Draw("same");
}
void HMdcGarCal2Maker::writeAscii()
{
    // Writes the created parameters form the working arrays to an ascii file.
    if(getWriteAscii())
    {

	cout<<"HMdcGarCal2Maker::writeAscii()    : Write output ascii file"<<endl;

	output=fopen(fNameAsciiOut,"w");

	Char_t tag[60];
	sprintf(tag,"%s","[MdcCal2ParSim]\n");
	fprintf(output,"############################################################################\n");
	fprintf(output,"# HMdcCal2ParSim\n");
	fprintf(output,"%s%i%s%i%s%i%s%i%s","# GARFIELD voltage ",getVoltage(0),", ",getVoltage(1),", ",getVoltage(2),", ",getVoltage(3),"\n");
	fprintf(output,"%s%i%s","# threshold ",getStepSize()*getThreshold()," %\n");
	fprintf(output,"%s%3.1f%s%3.1f%s%3.1f%s%3.1f%s","# cut at ",getCutMax(0),", ",getCutMax(1),", ",getCutMax(2),", ",getCutMax(3)," ns\n");
	fprintf(output,"# format type:0=time1, 1=variation time1, 2=time2, 3=variation time2\n");
	fprintf(output,"# sector module angle type par0 par1 par2 par3 par4 par5 par6 par7 par8 par9 \n");
	fprintf(output,"############################################################################\n");
	fprintf(output,"%s\n",tag);
	for(Int_t sector=0;sector<6;sector++)
	{
	    for(Int_t mdctype=0;mdctype<4;mdctype++)
	    {
		for(Int_t angle=0;angle<18;angle++)  // 0-17 from 0 to 85 impactc angle
		{
		    for(Int_t type=0;type<4;type++)  // time1,variation time1,time2, varitation time2
		    {
			for(Int_t dist=0;dist<10;dist++)  // 10*10 steps=100
			{
			    fprintf(output,"%i %i %2i %2i ",
				    sector,mdctype,angle,type);
			    if(type==0)
			    {
				fprintf(output,"%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f \n",
					mtime1_corr[mdctype][angle][dist*10 + 0],mtime1_corr[mdctype][angle][dist*10 + 1],
					mtime1_corr[mdctype][angle][dist*10 + 2],mtime1_corr[mdctype][angle][dist*10 + 3],
					mtime1_corr[mdctype][angle][dist*10 + 4],mtime1_corr[mdctype][angle][dist*10 + 5],
					mtime1_corr[mdctype][angle][dist*10 + 6],mtime1_corr[mdctype][angle][dist*10 + 7],
					mtime1_corr[mdctype][angle][dist*10 + 8],mtime1_corr[mdctype][angle][dist*10 + 9]);
			    }
			    if(type==1)
			    {
				fprintf(output,"%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f \n",
					mtime1_err_corr[mdctype][angle][dist*10 + 0],mtime1_err_corr[mdctype][angle][dist*10 + 1],
					mtime1_err_corr[mdctype][angle][dist*10 + 2],mtime1_err_corr[mdctype][angle][dist*10 + 3],
					mtime1_err_corr[mdctype][angle][dist*10 + 4],mtime1_err_corr[mdctype][angle][dist*10 + 5],
					mtime1_err_corr[mdctype][angle][dist*10 + 6],mtime1_err_corr[mdctype][angle][dist*10 + 7],
					mtime1_err_corr[mdctype][angle][dist*10 + 8],mtime1_err_corr[mdctype][angle][dist*10 + 9]);
			    }
			    if(type==2)
			    {
				fprintf(output,"%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f \n",
					mtime2_corr[mdctype][angle][dist*10 + 0],mtime2_corr[mdctype][angle][dist*10 + 1],
					mtime2_corr[mdctype][angle][dist*10 + 2],mtime2_corr[mdctype][angle][dist*10 + 3],
					mtime2_corr[mdctype][angle][dist*10 + 4],mtime2_corr[mdctype][angle][dist*10 + 5],
					mtime2_corr[mdctype][angle][dist*10 + 6],mtime2_corr[mdctype][angle][dist*10 + 7],
					mtime2_corr[mdctype][angle][dist*10 + 8],mtime2_corr[mdctype][angle][dist*10 + 9]);
			    }
			    if(type==3)
			    {
				fprintf(output,"%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f \n",
					mtime2_err_corr[mdctype][angle][dist*10 + 0],mtime2_err_corr[mdctype][angle][dist*10 + 1],
					mtime2_err_corr[mdctype][angle][dist*10 + 2],mtime2_err_corr[mdctype][angle][dist*10 + 3],
					mtime2_err_corr[mdctype][angle][dist*10 + 4],mtime2_err_corr[mdctype][angle][dist*10 + 5],
					mtime2_err_corr[mdctype][angle][dist*10 + 6],mtime2_err_corr[mdctype][angle][dist*10 + 7],
					mtime2_err_corr[mdctype][angle][dist*10 + 8],mtime2_err_corr[mdctype][angle][dist*10 + 9]);
			    }
			}
		    }
		}
	    }
	}
	fprintf(output,"############################################################################\n");
	fclose(output);
    }
    else
    {
	cout<<"HMdcGarCal2Maker::writeAscii()    : No ascii file written"<<endl;
    }
}
void HMdcGarCal2Maker::readInput()
{
    // Reads input file to get the hists for time1 and time2 produced by
    // HMdcGarSignalReader.
    gROOT->Reset();

    if(!HTool::open(&inputRoot,fNameRootIn,"READ"))
    {
	exit(1);
    }
    inputRoot->cd();

    Char_t namepath[300];
    Char_t namehist[300];

    for(Int_t mdctype=0;mdctype<4;mdctype++) {
        if(!check(mdctype,-99))continue;
	cout<<"mdc "<<mdctype<<" :"<<flush;
	for(Int_t angle=0;angle<18;angle++) {
	    cout<<" "<<angle*5<<flush;

	    if(getVersion()==1)sprintf(namepath,"%s%02i%s%02i%s","mdc ",mdctype,"/angle ",angle*5,"/");
            if(getVersion()==2)sprintf(namepath,"%s%i%s%02i%s","mdc ",mdctype,"/angle ",angle*5,"/");

	    sprintf(namehist,"%s%s%i%s%02i%s%02i%s",namepath,"hmeandrift_time1[",mdctype,"][",getThreshold(),"][",angle*5,"]");
	    htime1=(TH1F*) (inputRoot->Get(namehist));

	    if(getVersion()==1)sprintf(namepath,"%s%02i%s%02i%s","mdc ",mdctype,"/angle ",angle*5,"/");
            if(getVersion()==2)sprintf(namepath,"%s%i%s%02i%s","mdc ",mdctype,"/angle ",angle*5,"/");

	    sprintf(namehist,"%s%s%i%s%02i%s%02i%s",namepath,"hmeandrift_time2[",mdctype,"][",getThreshold(),"][",angle*5,"]");
	    htime2=(TH1F*) (inputRoot->Get(namehist));

	    fillArrays(htime1,htime2,mdctype,angle);

	    HTool::deleteObject(htime1);
	    HTool::deleteObject(htime2);
	}
        cout<<" "<<endl;
    }
    HTool::close(&inputRoot);
}
