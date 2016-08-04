//*-- AUTHOR : J.Kempter
//*-- Modified :

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcOffset
//
// Defines the offset  parameter for MDC calibration. Uses HMdcCalParRaw container
// as  input/output for calibration data
//
///////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <math.h>
#include <stdlib.h>
#include <fstream> 
#include <iomanip> 
#include <iostream> 

#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TStopwatch.h"
#include "TDirectory.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TSystem.h"
#include "TLatex.h"

#include "heventheader.h"
#include "hmdcoffset.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdcsizescells.h"
#include "hmdcraw.h"
#include "hmdcclus.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hdetector.h"
#include "hevent.h"
#include "hcategory.h"
#include "hlocation.h"
#include "hmdccal1.h"
#include "hmdclookupgeom.h"
#include "hmdclookupraw.h"
#include "hmdccalparraw.h"
#include "hstart2hit.h"
#include "hstartdef.h"
#include "hmdctimecut.h"
#include "htool.h"

ClassImp(HMdcOffset)

const Int_t HMdcOffset::nbin        = 2048;
const Int_t HMdcOffset::nbinp1      = HMdcOffset::nbin+1;
const Int_t HMdcOffset::nbinm1      = HMdcOffset::nbin-1;
const Int_t HMdcOffset::nSubEvents  = (6*192)*3;


HMdcOffset::HMdcOffset(void)
{
    setDefault();
    initVariables();
    //initMemory();
}

HMdcOffset::HMdcOffset(const Text_t* name,const Text_t* title) : HReconstructor(name,title)
{
    setDefault();
    initVariables();
    //initMemory();
}
HMdcOffset::~HMdcOffset(void)
{
    // Destructor.
    if (iter)            { delete iter; }
    if (iter_start)      { delete iter_start; }
    if (iter_clus)       { delete iter_clus; }
    if (fNameAsciiOffset){ delete fNameAsciiOffset; }
    if (fNameRootOffset) { delete fNameRootOffset;  }
    if (hreverse)        { delete[] hreverse; }
    iter       = NULL;
    iter_start = NULL;
    iter_clus  = NULL;

}
void HMdcOffset::initVariables()
{
    // pointer to iterators and categories
    rawCat       = 0;
    hitStartCat  = 0;
    clusCat      = 0;
    iter         = 0;
    iter_start   = 0;
    iter_clus    = 0;
    locraw.set(4,0,0,0,0);

    // switch options
    isPulserFile      = kFALSE;
    noStart           = kFALSE;
    useTimeCuts       = kTRUE;
    useClusters       = kTRUE;
    useWireOffset     = kTRUE;
    useTof            = kFALSE;
    debug             = kFALSE;
    fillHistsOnly     = kFALSE;
    readHists         = kFALSE;
    perMBO            = kFALSE;
    perMBOafterSingle = kFALSE;
    filterwindow1     = 50.;
    filterwindow2     =  5.;
    filenumber        = 0;

    // pointers to fileds and hists
    hreverse     = (MyField*) new MyField;
    hint         = 0;
    hinv         = 0;

    // container pointers
    calparraw    = 0;
    lookupgeom   = 0;
    timecut      = 0;
    sizescells   = 0;

    // constants
    signalspeed  = 0.004; // [ns/mm]

    // varables in finalize
    validRange   = 1000.;
    meanhOffset  = 0;
    myoffset     = 0;
    myerror      = 0;

    // counters
    eventcounter = 0;
    skipcounter  = 0;
    nSkipEvents  = 0;

    // output file names
    fNameAsciiOffset  = 0;
    fNameRootOffset   = 0;
    ferrorlog         = 0;
    offsetTuple       = 0;
    offsetPulserTuple = 0;

    // fit variables
    yequalzero        = 0;
    crosspointX       = 0;
    fitpar0           = 0;
    fitpar0error      = 0;
    fitpar1           = 0;
    fitpar1error      = 0;

    fitparNoise0      = 0;
    fitparNoise0error = 0;
    fitparNoise1      = 0;
    fitparNoise1error = 0;
    totalsigma        = 0;

    fitGaussMean      = 0;
    fitGaussSigma     = 0;

    
}
void HMdcOffset::setDefault()
{
    // Sets the the default values for the fits.
    setNoise     (100, 50);
    setThreshold (0.15, 0.5);
    setRangeGauss(50);
    setCleanThreshold(0);
    setFitNoise(kTRUE);
}

void HMdcOffset::setOutputAscii(const Char_t *c)
{
    // Sets ascii output of HMdcOffset for debugging output.
    if (fNameAsciiOffset) { delete fNameAsciiOffset; }
    fNameAsciiOffset = new Char_t[strlen(c) + 1];
    strcpy(fNameAsciiOffset, c);
}

void HMdcOffset::setOutputRoot(const Char_t *c)
{
    // Sets rootfile output of HMdcOffset where all created histograms were written.
    //
    if (fNameRootOffset) { delete fNameRootOffset; }
    fNameRootOffset = new Char_t[strlen(c) + 1];
    strcpy(fNameRootOffset, c);
}
void HMdcOffset::setUseTof(TString filename)
{
    // Retrieves TF1's for min tof substraction from root file
    // and switches the usesage of tof substraction kTRUE.
    if (filename.CompareTo("") == 0)
    {   // check for empty string
	useTof = kFALSE;
	Warning("setUseTof()","Empty String received for inputfile name of tof parameters!");
        exit(1);
    }
    else
    {   // if string is not empty
	TFile* inp = new TFile(filename,"READ");
	if(inp == 0)
	{   // if file does not exist
	    Warning("setUseTof()","Input file not existent!");
	    exit(1);
	}
	else
	{   // if file exists
	    cout<<"HMdcOffset::setUseTof(): Reading from "<<filename.Data()<<endl;
	    Char_t fitname[300];
	    for(Int_t mdc = 0; mdc < 4; mdc ++){
		for(Int_t lay = 0; lay < 6; lay ++){
		    sprintf(fitname,"fit[%i][%i]",mdc,lay);
                    toffunc[mdc][lay] = (TF1*)inp->Get(fitname);
		    if(toffunc[mdc][lay] == 0)
		    {   // if fit object has not been found in file
			Warning("setUseTof()","Error in reading input file!");
                        exit(1);
		    }
		}
	    }
	    useTof = kTRUE;
	}
    }
}
void HMdcOffset::setReadHists(TString filename,Bool_t print)
{
    // Retrieves TF1's for min tof substraction from root file
    // and switches the usesage of tof substraction kTRUE.
    if (filename.CompareTo("") == 0)
    {   // check for empty string
	readHists = kFALSE;
	Warning("setReadHists()","Empty String received for Hist inputfile !");
        exit(1);
    }
    else
    {   // if string is not empty
	if(gSystem->AccessPathName(filename.Data()))
	{   // if file does not exist
	    Warning("setReadHists()","Input file : %s not existent! Skipping!",filename.Data());
	}
	else
	{   // if file exists
	    filenumber ++;
	    Info("setReadHists()"," Reading from %3i : %s ",filenumber,filename.Data());
            HSpectrometer* spec = gHades->getSetup();
	    if(!spec){

		Error("setReadHists()","HSpectrometer not defined!");
                exit(1);

		
	    }
	    HDetector* mdc = spec->getDetector("Mdc");
	    if(mdc == NULL){

		Error("setReadHists()","HMdcDetector not defined!");
		exit(1);
	    }

	    TFile* inp = new TFile(filename,"READ");

	    for(Int_t s = 0; s < 6; s ++)
	    {  //loop over sectors
		if(print) { cout<<"Sector "<<s<<endl; }
		for(Int_t mo = 0; mo < 4; mo ++)
		{  //loop over modules
		    if(!mdc->getModule(s,mo)){ continue; }
		    if(print) { cout<<"    Module "<<mo<<endl; }
		    for(Int_t mb = 0; mb < 16; mb ++)
		    {  //loop over layer
			for(Int_t t = 0; t < 96; t ++) {  //loop over cells
			    MyHist* h = (MyHist*)inp->Get(Form("sector %i/module %i/mbo %02i/hinv/hinv[%i][%i][%02i][%02i]",s,mo,mb,s,mo,mb,t));
			    if(h){
				Int_t val = 0;
				for(Int_t i = 0; i < 2048; i ++){
				    if(!perMBO){
					val = (Int_t)h->GetBinContent(i + 1);
					(*hreverse)[s][mo][mb][t][i] += val;
				    }
				    else {
					val = (Int_t)h->GetBinContent(i + 1);
					for(Int_t t1 = 0; t1 < 96; t1 ++){
                                            (*hreverse)[s][mo][mb][t1][i] += val;
					}
				    }
				}
                                delete h;
			    }else{
                               break; // next MB...etc
			    }
			} //end tdc
		    } // end mbo
		} // end module
	    } // end sector

	    readHists = kTRUE;
	    inp->Close();
            delete inp;
	}
    }
}
void HMdcOffset::printStatus()
{
    printf("**************** HMdcOffset ***********************\n");
    printf("* ACTUAL SETTINGS                                 *\n");
    if(useTimeCuts)
    {
	printf("*    TIME CUTS are used                           *\n");
    }
    if(!useTimeCuts)
    {
	printf("*    NO TIME CUTS are used                        *\n");
    }
    if(useClusters)
    {
	printf("*    TIMES FILLED FROM CLUSTERS                   *\n");
    }
    if(!useClusters)
    {
	printf("*    TIMES FILLED FROM RAW                        *\n");
    }
    if(useWireOffset && useClusters)
    {
	printf("*    TIME OF SIGNAL ON WIRE SUBTRACTED            *\n");
    }
    if(!useWireOffset)
    {
	printf("*    TIME OF SIGNAL ON WIRE NOT SUBTRACTED        *\n");
    }
    if(useTof)
    {
	printf("*    MINIMUM TOF WILL BE SUBSTRACTED              *\n");
    }
    if(!useTof)
    {
	printf("*    MINIMUM TOF WILL NOT BE SUBSTRACTED          *\n");
    }
    if(isPulserFile)
    {
	printf("*    Mode set to PULSER FILE                      *\n");
    }
    if(!isPulserFile)
    {
	printf("*    Mode set to REAL DATA FILE                   *\n");
    }

    if(!noStart)
    {
	printf("*    START TIMES are used                         *\n");
    }
    if(noStart)
    {
	printf("*    NO START TIMES are used                      *\n");
    }
    if(readHists)
    {
	printf("*    DATA WILL BE READ FROM HISTOGRAMS            *\n");
    }
    if(!readHists)
    {
	printf("*    DATA WILL BE READ FROM HLDFILES              *\n");
    }
    if(fillHistsOnly)
    {
	printf("*    ONLY HISTOGRAMS WILL BE FILLED (BATCH mode)  *\n");
    }
    if(!fillHistsOnly)
    {
	printf("*    HISTOGRAMS/NTUPLES FILLED IN ONE SHOT        *\n");
    }
    if(perMBO)
    {
	printf("*    HISTOGRAMS WILL BE FILLED PER MBO            *\n");
    }
    if(perMBOafterSingle)
    {
	printf("*    HISTOGRAMS WILL BE FILLED PER MBO AFTER CH   *\n");
    }
    if(!perMBOafterSingle && !perMBO)
    {
	printf("*    HISTOGRAMS FILLED FOR SINGLE TDC CHANNEL     *\n");
    }
    printf("*    SIGNALSPEED = %5.3f [ns/mm]                 *\n"   ,signalspeed);
    printf("*    VALID OFFSET RANGE = %5.1f                  *\n"   ,validRange);
    printf("*    NOISE OFFSET = %4i                          *\n"   ,offsetfitNoise);
    printf("*    NOISE WIDTH  = %4i                          *\n"   ,widthfitNoise);
    printf("*    MIN Threshold   = %1.2f MAX Threshold = %1.2f  *\n",minfitthreshold,maxfitthreshold);
    printf("*    Range GAUSS FIT = %4i                       *\n"   ,rangeGauss);
    printf("**************** HMdcOffset ***********************\n");
}
Bool_t HMdcOffset::init(void)
{
    //  Inits HMdcOffset and the needed HMdcCalParRaw, if this container does not exists
    if(!readHists)
    {
	if(!noStart)
	{
	    hitStartCat = gHades->getCurrentEvent()->getCategory(catStart2Hit);       //????
	    if (!hitStartCat)
	    {
		Error("init()","NO START HIT CATEGORY IN INPUT!");
		return kFALSE;
	    } else { iter_start = (HIterator *)hitStartCat->MakeIterator("native"); }
	}
	if(useClusters)
	{
	    clusCat = gHades->getCurrentEvent()->getCategory(catMdcClus);       //????
	    if (!clusCat)
	    {
		Error("init()","NO CLUS CATEGORY IN INPUT!");
		return kFALSE;
	    } else { iter_clus = (HIterator *)clusCat->MakeIterator("native"); }

	    if(useWireOffset)
	    {
		sizescells = HMdcSizesCells::getExObject();
		if(!sizescells)
		{
		    Error("init()","NO HMDCSIZESCELLS CONTAINER IN INPUT!");
		    return kFALSE;
		}
	    }
	}
	rawCat = gHades->getCurrentEvent()->getCategory(catMdcRaw);
	if (!rawCat)
	{
	    Error("init()","NO MDC RAW CATEGORY IN INPUT!");
	    return kFALSE;
	} else { iter = (HIterator *)rawCat->MakeIterator(); }

	if(useTimeCuts){
	    timecut     = (HMdcTimeCut*)   gHades->getRuntimeDb()->getContainer("MdcTimeCut");
	} else {
	    timecut = NULL;
	}
    }

    calparraw  = (HMdcCalParRaw*) gHades->getRuntimeDb()->getContainer("MdcCalParRaw");
    lookupgeom = (HMdcLookupGeom*)gHades->getRuntimeDb()->getContainer("MdcLookupGeom");
    lookupraw  = (HMdcLookupRaw*) gHades->getRuntimeDb()->getContainer("MdcLookupRaw");

    fActive      = kTRUE;
    eventcounter = 0;
    skipcounter  = 0;
    printStatus();
    return kTRUE;
}

Bool_t HMdcOffset::reinit(void)
{
    //
    cout<<"skipcount "<<skipcounter<<endl;
    skipcounter = 0;
    fActive     = kTRUE;
    return kTRUE;
}



void HMdcOffset::createHist(TFile* file,Int_t s, Int_t m, Int_t l, Int_t c, Bool_t read)
{
    // Histograms for inverted Time1 and integrated Time1 per Tdc-Channel are created
    // in a subdirectory structure.
    static Char_t title[50], tmp[30];

    sprintf(title,"%s%i%s%i%s%i%s%i","Sector ",s,"  Module ",m,"  Mbo ", l,"  Tdc ", c);
    file->TDirectory::Cd("hinv");
    sprintf(tmp, "%s%i%s%i%s%02i%s%02i%s", "hinv[",s,"][",m,"][",l,"][",c,"]");
    if(read) { hinv = (MyHist*) gDirectory->Get(tmp); }
    else     { hinv = new MyHist(tmp, title, nbin,-nbin + 0.5,0.5); } //inverted hist.
    file->TDirectory::Cd("../hint");
    sprintf(tmp, "%s%i%s%i%s%02i%s%02i%s", "hint[",s,"][",m,"][",l,"][",c,"]");
    if(read) { hint = (MyHist*) gDirectory->Get(tmp); }
    else     { hint = new MyHist(tmp, title, nbin,-nbin + 0.5,0.5); } // integrated hist.
    file->TDirectory::Cd("..");
}
void HMdcOffset::createHist_2D(Int_t s, Int_t m, Bool_t read)
{
    // Histograms for inverted Time1 and integrated Time1 per Tdc-Channel are created
    // in a subdirectory structure.
    static Char_t title[50], tmp[30];
    for(Int_t mb = 0; mb < 16; mb ++)
    {
	sprintf(title,"%s%i%s%i%s%i","Sector ",s,"  Module ",m,"  Mbo ", mb);
	sprintf(tmp, "%s%i%s%i%s%02i%s", "htime1_mbo[",s,"][",m,"][",mb,"]");
	if(read) {
	    htime1_mbo[mb] = (TH2F*) gDirectory->Get(tmp);
	    htime1_mbo[mb]->SetName(Form("%s_perMBO",tmp));
	    htime1_mbo[mb]->Reset();
	} else { htime1_mbo[mb] = new TH2F(tmp, title,700,-200,500,96,1,97); } //hist time vers tdc.
    }
    for(Int_t lay = 0; lay < 6; lay ++)
    {
	sprintf(title,"%s%i%s%i%s%i","Sector ",s,"  Module ",m,"  Lay ", lay);
	sprintf(tmp, "%s%i%s%i%s%02i%s", "htime1_lay[",s,"][",m,"][",lay,"]");
	if(read) {
	    htime1_lay[lay] = (TH2F*) gDirectory->Get(tmp);
            htime1_lay[lay]->SetName(Form("%s_perMBO",tmp));
	    htime1_lay[lay]->Reset();
	} else { htime1_lay[lay] = new TH2F(tmp, title,700,-200,500,210,1,211); } //hist time vers tdc.

	sprintf(title,"%s%i%s%i%s%i","Sector ",s,"  Module ",m,"  Lay ", lay);
	sprintf(tmp, "%s%i%s%i%s%02i%s", "htime1_lay_inv_norm[",s,"][",m,"][",lay,"]");

	if(read) {
	    htime1_lay_inv_norm[lay] = (TH2F*) gDirectory->Get(tmp);
            htime1_lay_inv_norm[lay]->SetName(Form("%s_perMBO",tmp));
            htime1_lay_inv_norm[lay]->Reset();
	} else { htime1_lay_inv_norm[lay] = new TH2F(tmp, title,700,-200,500,210,1,211); } //hist time vers tdc.

	sprintf(title,"%s%i%s%i%s%i","Sector ",s,"  Module ",m,"  Lay ", lay);
	sprintf(tmp, "%s%i%s%i%s%02i%s", "htime1_lay_int[",s,"][",m,"][",lay,"]");

	if(read) {
	    htime1_lay_int[lay] = (TH2F*) gDirectory->Get(tmp);
	    htime1_lay_int[lay]->SetName(Form("%s_perMBO",tmp));
            htime1_lay_int[lay]->Reset();
	} else { htime1_lay_int[lay] = new TH2F(tmp, title,700,-200,500,210,1,211); } //hist time vers tdc.

	sprintf(title,"%s%i%s%i%s%i","Sector ",s,"  Module ",m,"  Lay ", lay);
	sprintf(tmp, "%s%i%s%i%s%02i%s", "htime1_lay_int_norm[",s,"][",m,"][",lay,"]");
	if(read) {
	    htime1_lay_int_norm[lay] = (TH2F*) gDirectory->Get(tmp);
	    htime1_lay_int_norm[lay]->SetName(Form("%s_perMBO",tmp));
            htime1_lay_int_norm[lay]->Reset();
	} else { htime1_lay_int_norm[lay] = new TH2F(tmp, title,700,-200,500,210,1,211); } //hist time vers tdc.
    }

}
void HMdcOffset::deleteHist()
{
    // Created histograms are deleted
    delete hinv;
    delete hint;
}
void HMdcOffset::deleteHist_2D()
{
    // Created histograms are deleted
    for(Int_t mb = 0; mb < 16; mb ++)
    {
	delete htime1_mbo[mb];
        htime1_mbo[mb] = 0;
    }
    for(Int_t lay = 0; lay < 6; lay ++)
    {
	delete htime1_lay         [lay];
	delete htime1_lay_inv_norm[lay];
	delete htime1_lay_int     [lay];
	delete htime1_lay_int_norm[lay];
	htime1_lay         [lay] = 0;
	htime1_lay_inv_norm[lay] = 0;
	htime1_lay_int     [lay] = 0;
	htime1_lay_int_norm[lay] = 0;
    }



}

void HMdcOffset::fillHist(Int_t s, Int_t m, Int_t l, Int_t c)
{
    // Histograms for inverted Time1 and integrated Time1 per Tdc-Channel are filled

    hint->SetBinContent(1,(Double_t)(*hreverse)[s][m][l][c][0]);
    hinv->SetBinContent(1,(Double_t)(*hreverse)[s][m][l][c][0]);

    for(Int_t k = 2; k < nbinp1; k ++)
    {
	if(((Double_t)(*hreverse)[s][m][l][c][k - 1]) > cleanThreshold ){
	    hinv->SetBinContent(k,(Double_t)(*hreverse)[s][m][l][c][k - 1]);
	}
	hint->SetBinContent(k,(hint->GetBinContent(k - 1)+hinv->GetBinContent(k)));
    }
    integral[s][m][l][c] = (Int_t)hint->GetBinContent(nbin - 2);
}
void HMdcOffset::fillHist_2D(Int_t s, Int_t m, Int_t l, Int_t c)
{
    // Histograms for Time1 vers Tdc-Channel and cell are filled
    HMdcLookupChan& chan = (*lookupgeom)[s][m][l][c];
    Int_t layer          = chan.getNLayer();
    Int_t cell           = chan.getNCell();
    Int_t myoffbin       = (Int_t)(2048 - offsets[s][m][l][c]); // convert offset x to offset bin
    Float_t temp;





    for(Int_t k = 1; k < nbinp1; k ++) // loop over all bins of hinv  (x from -2048 to 0)
    {
	if(k - myoffbin < 500 && k - myoffbin > - 200) // check if bin in range
	{
	    Int_t mybin = 200 + k - myoffbin;

	    temp = htime1_mbo[l]->GetCellContent(mybin,c + 1);
	    htime1_mbo       [l]->SetCellContent(mybin,c + 1   ,hinv->GetBinContent(k) + temp);

	    temp = htime1_lay         [layer]->GetCellContent(mybin,cell + 1);
	    htime1_lay                [layer]->SetCellContent(mybin,cell + 1,hinv->GetBinContent(k) + temp);

	    temp = htime1_lay_int     [layer]->GetCellContent(mybin,cell + 1);
	    htime1_lay_int            [layer]->SetCellContent(mybin,cell + 1,hint->GetBinContent(k) + temp);

	    temp = htime1_lay_inv_norm[layer]->GetCellContent(mybin,cell + 1);
	    htime1_lay_inv_norm       [layer]->SetCellContent(mybin,cell + 1,hinv->GetBinContent(k) + temp);

	    temp = htime1_lay_int_norm[layer]->GetCellContent(mybin,cell + 1);
	    htime1_lay_int_norm       [layer]->SetCellContent(mybin,cell + 1,hint->GetBinContent(k) +temp);

	}
    }

    Float_t max_inv = hinv->GetMaximum();
    Float_t max_int = hint->GetMaximum();

    for(Int_t k = 1; k < nbinp1; k ++) // loop over all bins of hinv  (x from -2048 to 0)
    {
	if(max_inv != 0)
	{
	    temp = htime1_lay_inv_norm[layer]->GetCellContent(k,cell + 1);
	    htime1_lay_inv_norm       [layer]->SetCellContent(k,cell + 1,temp / max_inv);
	}
	if(max_int != 0)
	{
	    temp = htime1_lay_int_norm[layer]->GetCellContent(k,cell + 1);
	    htime1_lay_int_norm       [layer]->SetCellContent(k,cell + 1,temp / max_int);
	}
    }
}

Int_t HMdcOffset::fitHist(Int_t s, Int_t m, Int_t l, Int_t c)
{
    // The offset is calculated from two linear fits. Some pictures of the fitted histograms
    // can be found on Begin_Html <a href="http://www-hades.gsi.de/docs/mdc/mdcana/#calibration">MDC calibration page</a>End_Html .
    // The first linear fit (red) is done to find the rising edge of the integrated spectra.
    // The second linear fit (blue) is done to substract the noise.
    // The ranges for both fits can be set through the functions setNoise()(x-range) for the
    // second fit (default values: 100,50) and setThreshold()(y-range) for the first fit
    // (default values :0.15,0.50). The y-range is calculated by the percentage of the maximum
    // height of the spectra.
    // The offset ist calculated from the intersection point of both linear fits.
    // The sigma of the offset is calculated from the sigmas of the two linear fits.
    //
    // If the analyzed file is a pulser file for external calibration a gaussian fit is done.
    // To set this fit the function setPulserFile() has to be used in the macro. The range of
    // the fit is set through the function setRangeGauss()(default value: 50) around the maximum.
    // The mean and sigma of the fit is written to the ascii debugging output.
    // The hist is checked for multiple peaks.The peaks which have been found are written to an array.

    fitGaussMean      = 0;
    fitGaussSigma     = 0;
    fitpar0           = 0;
    fitpar0error      = 0;
    fitpar1           = 0;
    fitpar1error      = 0;
    fitparNoise0      = 0;
    fitparNoise1      = 0;
    fitparNoise0error = 0;
    fitparNoise1error = 0;
    totalsigma        = 0;
    yequalzero        = 0;
    crosspointX       = 0;
    //-----------------------------------------------------------
    // if hist is empty do nothing
    if(hint->Integral() == 0 )
    {   
	if(debug){ cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: Integral==0 "<<endl; }
	yequalzero  = 1;
	crosspointX = 1;
	return 1;

    }
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // Check for overflows. In case of an overflow bins
    // might be negative
    if(hint->GetMinimum() < 0)
    {
        cout<<" "<<endl;
	cout<<"WARNING: IN HINT "<<s<<" "<<m<<" "<<l<<" "<<c<<" NEGATIV VALUES DETECTED!"<<endl;
        cout<<" "<<endl;
	fprintf(ferrorlog,"%i %i %i %2i %s\n",s,m,l,c,"fit: in hint negative values detected!");
    }
    //-----------------------------------------------------------


    //-----------------------------------------------------------
    // finding upper fitting range for signal fit
    Int_t   xmaxfitbin = nbinm1;

    Float_t yminfit    = minfitthreshold * hint->GetBinContent(nbinm1 - 1);     // setting y thresholds for fitting
    Float_t ymaxfit    = maxfitthreshold * hint->GetBinContent(nbinm1 - 1);     // relative to maximum y-value
    while (hint->GetBinContent(-- xmaxfitbin) > ymaxfit && xmaxfitbin);         // find to ymaxfit corresponding bin
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // if no match for the upper fit range has been found in
    // the desired Y range
    if (!xmaxfitbin)
    {
	if(debug){ cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: xmaxfitbin=0, integral "<<hint->Integral()<<endl; }
	yequalzero  = 1;
	crosspointX = 1;
	return 1;
    }
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // finding lower fitting range for signal fit
    Int_t xminfitbin = xmaxfitbin;
    while (hint->GetBinContent(-- xminfitbin) > yminfit && xminfitbin); // find to yminfit corresponding bin
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // if no match for the lower fit range has been found in
    // the desired Y range
    if (!xminfitbin)
    {
	if(debug){ cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: xminfitbin=0, integral "<<hint->Integral()<<endl; }
	yequalzero  = 2;
	crosspointX = 2;
	return 2;
    }
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // if fit range is just 1 bin the fit will give an error message.
    // this is just to suppress the error message.
    if(xmaxfitbin-xminfitbin == 1)
    {   // fit range is just 1 bin!
	if(debug){ cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: fit range 1 bin, integral "<<hint->Integral()<<endl; }
	yequalzero  = 2;
	crosspointX = 2;
	return 2;

    }
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // create pol1 fit function in desired range and fit the hist.
    TF1 *f = new TF1("fitsignal","pol1",hint->GetBinCenter(xminfitbin),hint->GetBinCenter(xmaxfitbin)); // call fit function poynom 1.order in x-range
    hint->Fit("fitsignal","RNQ");
    fitpar0      = 0;
    fitpar0error = 0;
    fitpar1      = 0;
    fitpar1error = 0;
    fitpar0      = f->GetParameter(0);  // get fitparameters
    fitpar0error = f->GetParError (0);
    fitpar1      = f->GetParameter(1);  //(f1->GetParameter(1)<0.000001)?-1:(f1->GetParameter(1));
    fitpar1error = f->GetParError (1);  // y=fitpar[1]* x +  fitpar[0]
    //-----------------------------------------------------------
    

    //-----------------------------------------------------------
    // if result of fit is nan or inf.
    if(TMath::Finite(f->GetParameter(0)) == 0 ||
       TMath::Finite(f->GetParameter(1)) == 0
      )
    {   // bullshit !!
	if(debug){
	    cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: fit result strange, integral "
		<<hint->Integral()
		<<" xminbin "<<xminfitbin
		<<" xmaxbin "<<xmaxfitbin
		<<" fitpar0 "<<f->GetParameter(0)
		<<" fitpar1 "<<f->GetParameter(1)
		<<endl;
	}
	yequalzero  = 3;
	crosspointX = 3;
	delete f;
	return 3;
    }
    //-----------------------------------------------------------


    //-----------------------------------------------------------
    // remove fit function
    delete f;
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // if fitpar1 is 0 we can not do anything usefull
    if (fitpar1 == 0)
    {
	if(debug) {
	    cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: fitpar1==0 , integral "
		<<hint->Integral()
		<<" xminbin "<<xminfitbin
		<<" xmaxbin "<<xmaxfitbin
		<<" fitpar0 "<<fitpar0
		<<" fitpar1 "<<fitpar1
		<<endl;
	}
	yequalzero  = 3;
	crosspointX = 3;
	return 3;
    }
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // calculate cross point of fit with x-axis
    // and check for good range
    yequalzero =  0;
    yequalzero = -fitpar0 / fitpar1;

    if (-yequalzero < (offsetfitNoise + widthfitNoise) || -yequalzero > nbin)
    {
	if(debug){
	    cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: yequalzero out of range, integral "
		<<hint->Integral()
		<<" -yequalzero "<<-yequalzero
		<<" xminbin "    <<xminfitbin
		<<" xmaxbin "    <<xmaxfitbin
		<<" fitpar0 "    <<fitpar0
		<<" fitpar1 "    <<fitpar1
		<<endl;
	}
	crosspointX = 4;
	return 4;
    }
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // check weather the range for the noise fit contains any
    // counts. This is just to suppress error messages from the
    // fit. If the is no signal yequalzero will be taken as offset.
    if(hint->Integral(hint->FindBin(yequalzero - (offsetfitNoise + widthfitNoise)),
		      hint->FindBin(yequalzero - offsetfitNoise)) < 10    ||
       !fitNoise
      )
    {  // almost no counts in range of fit!

	if(debug){
	    cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: no counts in noise region , integral "
		<<hint->Integral()
		<<" -yequalzero "<<-yequalzero
		<<" minbinNoise "<<hint->FindBin(yequalzero - (offsetfitNoise + widthfitNoise))
		<<" maxbinNoise "<<hint->FindBin(yequalzero - offsetfitNoise)
		<<endl;
	}
	crosspointX       = -yequalzero;
	fitparNoise0      =  0;
	fitparNoise1      =  0;
	fitparNoise0error =  0;
	fitparNoise1error =  0;
    }
    else
    {
	//-----------------------------------------------------------
	// create pol1 fit function for the noise fit in
        // the desired range and fit the hist
	f = new TF1("fitnoise","pol1",
		    yequalzero - (offsetfitNoise + widthfitNoise),
		    yequalzero - offsetfitNoise);
	hint->Fit("fitnoise","RNQ");
	fitparNoise0      = 0;
	fitparNoise1      = 0;
	fitparNoise0error = 0;
	fitparNoise1error = 0;
	fitparNoise0      = f->GetParameter(0); // get fitparameters
	fitparNoise1      = f->GetParameter(1);
	fitparNoise0error = f->GetParError (0); // get fitparameters
	fitparNoise1error = f->GetParError (1);
	//-----------------------------------------------------------

	//-----------------------------------------------------------
	// if result of fit is nan or inf.
        // if so yequalzero is taken as offset
	if(TMath::Finite(f->GetParameter(0)) == 0 ||
	   TMath::Finite(f->GetParameter(1)) == 0
	  )
	{   // bullshit !!
	    if(debug){
		cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: noise fit result strange , integral "
	             <<hint->Integral()
		     <<" -yequalzero " <<-yequalzero
		     <<" minbinNoise " <<hint->FindBin(yequalzero - (offsetfitNoise + widthfitNoise))
		     <<" maxbinNoise " <<hint->FindBin(yequalzero - offsetfitNoise)
		     <<" fitparNoise0 "<<f->GetParameter(0)
		     <<" fitparNoise1 "<<f->GetParameter(1)
		    <<endl;
	    }
	    crosspointX       = -yequalzero;
	    fitparNoise0      =  0;
	    fitparNoise1      =  0;
	    fitparNoise0error =  0;
	    fitparNoise1error =  0;
       }

	//-----------------------------------------------------------
	// if fitpar1==fitparNoise1 we can not calculate
        // any cross point. if so yequalzero is taken as offset
	if (fitpar1 == fitparNoise1)
	{
	    if(debug){
		cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: no intersection for fit functions , integral "
	             <<hint->Integral()
		     <<" -yequalzero " <<-yequalzero
		     <<" minbinNoise " <<hint->FindBin(yequalzero - (offsetfitNoise + widthfitNoise))
		     <<" maxbinNoise " <<hint->FindBin(yequalzero - offsetfitNoise)
                     <<" fitpar0     " <<fitpar0
                     <<" fitpar1     " <<fitpar1
                     <<" fitparNoise0 "<<f->GetParameter(0)
		     <<" fitparNoise1 "<<f->GetParameter(1)
		    <<endl;
	    }
	    crosspointX       = -yequalzero;
	    fitparNoise0      =  0;
	    fitparNoise1      =  0;
	    fitparNoise0error =  0;
	    fitparNoise1error =  0;
	}

	
	//-----------------------------------------------------------
	// finally calculate cross point between noise and signal
	// fit to define the offset
	crosspointX = -(fitparNoise0 - fitpar0)/(fitpar1 - fitparNoise1);
	//-----------------------------------------------------------

	//-----------------------------------------------------------
	// check if value is ok and in meaningfull range
	if (TMath::Finite(crosspointX) == 0 || crosspointX < 0 || crosspointX > nbinm1)
	{
	    if(debug){
		cout<<s<<" "<<m<<" "<<l<<" "<<c<<" skip: crospointX out of range or strange , integral "
		    <<hint->Integral()
		    <<" -yequalzero " <<-yequalzero
		    <<" crosspaintX " <<crosspointX
		    <<" xminbin "     <<xminfitbin
		    <<" xmaxbin "     <<xmaxfitbin
		    <<" minbinNoise " <<hint->FindBin(yequalzero - (offsetfitNoise + widthfitNoise))
		    <<" maxbinNoise " <<hint->FindBin(yequalzero - offsetfitNoise)
		    <<" fitpar0     " <<fitpar0
		    <<" fitpar1     " <<fitpar1
		    <<" fitparNoise0 "<<f->GetParameter(0)
		    <<" fitparNoise1 "<<f->GetParameter(1)
		    <<endl;
	    }
	    crosspointX       = -yequalzero;
	    fitparNoise0      =  0;
	    fitparNoise1      =  0;
	    fitparNoise0error =  0;
	    fitparNoise1error =  0;
	}
 	//-----------------------------------------------------------

	//-----------------------------------------------------------
	// remove fit function
	delete f;
	//-----------------------------------------------------------
    }

    //-----------------------------------------------------------
    // calculating total sigma of the found offset
    totalsigma = (Float_t)sqrt(  (pow( fitparNoise0error    ,2) / pow( fitparNoise1-fitpar1 ,2))
			       + (pow( fitpar0error         ,2) / pow( fitparNoise1-fitpar1 ,2))
			       + (pow( fitparNoise0-fitpar0 ,2) * pow( fitpar1error         ,2) / pow( fitparNoise1 - fitpar1 ,4))
			       + (pow( fitparNoise0-fitpar0 ,2) * pow( fitparNoise1error    ,2) / pow( fitparNoise1 - fitpar1 ,4))
			      );
    //-----------------------------------------------------------

    // check if fitpars are meaningful
    if(fitpar1      < 100000  && fitpar1 > -100000) { fitslope1[s][m][l][c] = fitpar1; }
    else                                            { fitslope1[s][m][l][c] = 0; }
    if(fitparNoise1 < 100000  && fitpar1 > -100000) { fitslope2[s][m][l][c] = fitparNoise1; }
    else                                            { fitslope2[s][m][l][c] = 0; }

    if(useTof){
	// (2048 - traw1 * slope) + starttime - wireoff - tof;
	HMdcLookupChan& chan = (*lookupgeom)[s][m][l][c];
	Int_t lay            = chan.getNLayer();
	Int_t cell           = chan.getNCell();
	Float_t tof          = toffunc[m][lay]->Eval(cell);
	crosspointX += tof;
	yequalzero  -= tof;
    }

    if (!isPulserFile) { return 0; }


    // If there are more than 1 peak only the most prominant is fitted
    Float_t localmax = (2048 - (hinv->GetMaximumBin()));
    f = new TF1("f3","gaus",-localmax - rangeGauss, -crosspointX + rangeGauss);

    hinv->Fit("f3","RNQ");
    fitGaussMean  = f->GetParameter(1);  //mean
    fitGaussSigma = f->GetParameter(2);  //sigma

    findMultiplePeaks(s,m,l,c);

    delete f;

    // FIXME: Error handling for gauss fit ???

    return 0;
}
void HMdcOffset::findMultiplePeaks(Int_t s,Int_t m,Int_t l,Int_t c)
{
    for(Int_t j = 0;j < 2047;j ++){ htime1temp->SetBinContent(j,hinv->GetBinContent(j)); } // copy hinv to temp hist

    htime1temp->Smooth(10);// make it smooth

    Char_t address    [50];
    Float_t binlowold [5];
    Float_t binhighold[5];
    Float_t binlow    [5];
    Float_t binhigh   [5];
    Int_t binmax      [5];

    Float_t max      = 0;
    Float_t min      = 0;
    Float_t threshold= 0.05;

    for(Int_t j=0;j<5;j++)// reset helper arrays
    {
	binlowold [j] = 0;
	binhighold[j] = 0;
	binlow    [j] = 0;
	binhigh   [j] = 0;
	binmax    [j] = 0;
    }
    max         = min = 0;
    max         = htime1temp->GetMaximum(); // get max height
    min         = threshold * max;
    Int_t count = 0;
    for(Int_t bin = 0; bin < 2047; bin ++)
    {
	if(count < 5)
	{
	    binlow [count] = htime1temp->GetBinContent(bin);
	    binhigh[count] = htime1temp->GetBinContent(bin + 1);

	    if(bin > 40 && binlow[count] > 20                    && // skip the first 40 tdc bins and take into account only if > 20 counts
	       fabs(binhigh      [count]-binlow   [count]) > 1   &&
	           (binhighold   [count]-binlowold[count]  > 0)  &&
	           (binhigh      [count]-binlow   [count]  < 0) ) // if zero was crossed
	    {
		binmax[count] = bin;
		count ++;
	    }
	    if(count < 5)
	    {
		binlowold [count] = binlow [count];
		binhighold[count] = binhigh[count];
	    } else { cout<<"MORE THAN 5 PEAKS!"<<endl; }
	}
    }
    if(count > 1)
    {
        cout<<" "<<endl;
	cout<<"sector "<<s<<" module "<<m<<" mbo "<<l<<" tdc "<<c<<endl;
	for(Int_t i = 0; i < count; i ++)
	{
	    if(i < 5)
	    {
		cout<<"peak found: "<<i + 1
		    <<" bin="       <<binmax        [i]
		    <<" ,x="        <<(2048 - binmax[i])
		    <<" max="       <<binlow        [i]
		    <<endl;
                sprintf(address,"%i %i %i %2i",s,m,l,c);
		fprintf(ferrorlog,"%s %i %s %4i %s %4i %s %8.0f\n",address,i + 1,"multiple peaks: bin=",binmax[i]," ,x=",(2048 - binmax[i])," max=",binlow[i]);
	    } else { cout<<"MORE THAN 5 PEAKS!"<<endl; }
	}
	cout<<" "<<endl;
    }

    for(Int_t j = 0; j < 5; j ++)// coppy to global array
    {
	if(binmax[j] > 0){ offsetpulser[s][m][l][c][j] = (2048 - binmax[j]); }
	else             { offsetpulser[s][m][l][c][j] = 0; }
    }
}
void HMdcOffset::writeAscii(ofstream &fout, Int_t s, Int_t m, Int_t l, Int_t c)
{
    // The adresses of the channel(sector,module,mbo,tdc),offset, two fitparameters for the
    // two linear fits, the sigma of the offset ,offset and the integral(number of entries for this channel)
    // are written to the ascii debugging output.In the case of an pulser file (external calibration)
    // the mean and the sigma of the gaussian fit are also written to the output.
    if (isPulserFile)
    {
	fout
            << setw(3)  << s                 << " "
            << setw(4)  << m                 << " "
            << setw(4)  << l                 << " "
            << setw(5)  << c                 << " "
	    << setw(10) << -yequalzero       << " "
            << setw(10) << yequalzero        << " "
	    << setw(10) << crosspointX       << " "
            << setw(15) << -fitGaussMean     << " "
            << setw(15) << fitGaussSigma     << " "
            << setw(15) << fitpar0           << " "
            << setw(15) << fitpar1           << " "
            << setw(15) << fitparNoise0      << " "
            << setw(15) << fitparNoise1      << " "
            << setw(15) << totalsigma        << " "
            << setw(10) << integral[s][m][l][c]

            << endl;
    }
    else
    {
        fout
            << setw(3)  << s                  << " "
            << setw(4)  << m                  << " "
            << setw(4)  << l                  << " "
            << setw(5)  << c                  << " "
	    << setw(10) << -yequalzero        << " "
	    << setw(10) << crosspointX        << " "
            << setw(15) << fitpar0            << " "
            << setw(15) << fitpar1            << " "
            << setw(15) << fitparNoise0       << " "
            << setw(15) << fitparNoise1       << " "
            << setw(15) << totalsigma         << " "
            << setw(10) << integral[s][m][l][c]
	    << endl;
    }
}

void HMdcOffset::writeHist(TFile* file)
{
    // All created histograms are written to a rootfile.
    // The file is structured for sector,module,mbo.
    
    file->TDirectory::Cd("hinv");
    hinv->Write();
    file->TDirectory::Cd("../hint");
    hint->Write();
    file->TDirectory::Cd("..");
}
void HMdcOffset::writeHist_2D(Int_t s,Int_t m)
{
    TLatex* tex;


    // All created histograms are written to a rootfile.
    for(Int_t mb = 0; mb < 16; mb ++)
    {
	HMdcLookupMoth& moth = (*lookupgeom)[s][m][mb];
	for(Int_t i = 0; i < moth.getSize(); i ++){
	    HMdcLookupChan& t = (*lookupgeom)[s][m][mb][i];
	    Int_t l           = t.getNLayer();
	    Int_t c           = t.getNCell();
	    tex = new TLatex(-150,i+1,Form("s%i m%i l%i c%i",s,m,l,c));
	    tex->SetTextSize(0.02);
	    htime1_mbo[mb]->GetListOfFunctions()->Add(tex);
	}
	htime1_mbo[mb]->SetEntries(1);
        htime1_mbo[mb]->SetOption("colz");
        htime1_mbo[mb]->Write();
    }

    for(Int_t lay = 0;lay < 6; lay ++)
    {
	HMdcLookupLayer& layer = (*lookupraw)[s][m][lay];
	for(Int_t i=0;i<layer.getSize();i++){
	    HMdcLookupCell& cell = (*lookupraw)[s][m][lay][i];
	    Int_t t  = cell.getNChan();
	    Int_t mb = cell.getNMoth();

	    tex = new TLatex(-150,i+1,Form("s%i m%i mb%i t%i",s,m,mb,t));
	    tex->SetTextSize(0.02);
	    htime1_lay[lay]         ->GetListOfFunctions()->Add(tex);
	    htime1_lay_inv_norm[lay]->GetListOfFunctions()->Add(tex);
	    htime1_lay_int     [lay]->GetListOfFunctions()->Add(tex);
	    htime1_lay_int_norm[lay]->GetListOfFunctions()->Add(tex);
	}

	htime1_lay         [lay]->SetEntries(1);
        htime1_lay         [lay]->SetOption("colz");
        htime1_lay         [lay]->Write();

	htime1_lay_inv_norm[lay]->SetEntries(1);
        htime1_lay_inv_norm[lay]->SetOption("colz");
        htime1_lay_inv_norm[lay]->Write();

	htime1_lay_int     [lay]->SetEntries(1);
        htime1_lay_int     [lay]->SetOption("colz");
        htime1_lay_int     [lay]->Write();

	htime1_lay_int_norm[lay]->SetEntries(1);
        htime1_lay_int_norm[lay]->SetOption("colz");
        htime1_lay_int_norm[lay]->Write();
    }
}

ofstream *HMdcOffset::openAsciiFile()
{
    // Opens the ascii debugging output and writes the headerline.
    ofstream *fout = NULL;
    if (fNameAsciiOffset) { fout = new ofstream(fNameAsciiOffset, ios::out); }
    if (fout)
    {            // changed *fout
        if (isPulserFile)
        {
            *fout //<< "Layer     Cell      crosspoint" << endl;
                << setw(3)  << "s"              << " "
                << setw(4)  << "mo"             << " "
                << setw(4)  << "mb"             << " "
                << setw(5)  << "t"              << " "
		<< setw(10) << "yequalzero"     << " "
		<< setw(10) << "Offset"         << " "
                << setw(15) << "GaussMean"      << " "
                << setw(15) << "GaussSigma"     << " "
                << setw(15) << "fitpar0"        << " "
                << setw(15) << "fitpar1"        << " "
                << setw(15) << "fitparNoise0"   << " "
                << setw(15) << "fitparNoise1"   << " "
                << setw(15) << "totalsigma"     << " "
                << setw(10) << "Integral"       << " "<< endl;
          *fout << setw(14) << "[MdcCalParRaw]" <<endl;
        }
        else
        {
            *fout //<< "Layer     Cell      crosspoint" << endl;
                << setw(3)  << "s"              << " "
                << setw(4)  << "mo"             << " "
                << setw(4)  << "mb"             << " "
                << setw(5)  << "t"              << " "
		<< setw(10) << "yequalzero"     << " "
		<< setw(10) << "Offset"         << " "
                << setw(15) << "fitpar0"        << " "
                << setw(15) << "fitpar1"        << " "
                << setw(15) << "fitparNoise0"   << " "
                << setw(15) << "fitparNoise1"   << " "
                << setw(15) << "totalsigma"     << " "
                << setw(10) << "Integral"       << " "<< endl;
          *fout << setw(14) << "[MdcCalParRaw]" <<endl;
	}
    }
    return fout;
}
void HMdcOffset::fillNTuples(Int_t s,Int_t mo,Int_t mb,Int_t t)
{
    // fill NTuples for offsets and multiple peaks
    HMdcLookupChan& chan = (*lookupgeom)[s][mo][mb][t];
    offsetTuple->Fill(s,mo,mb,t,
		      chan.getNLayer(),
		      chan.getNCell(),
		      (*calparraw)[s][mo][mb][t].getSlope(),
		      (*calparraw)[s][mo][mb][t].getSlopeErr(),
		      offsets     [s][mo][mb][t],
		      offsetErr   [s][mo][mb][t],
		      integral    [s][mo][mb][t],
		      offset1     [s][mo][mb][t],
		      fitslope1   [s][mo][mb][t],
		      fitslope2   [s][mo][mb][t]);

    offsetPulserTuple->Fill(s,mo,mb,t,
			    chan.getNLayer(),
			    chan.getNCell(),
			    offsets     [s][mo][mb][t],
			    offsetErr   [s][mo][mb][t],
			    integral    [s][mo][mb][t],
			    offsetpulser[s][mo][mb][t][0],
			    offsetpulser[s][mo][mb][t][1],
			    offsetpulser[s][mo][mb][t][2],
			    offsetpulser[s][mo][mb][t][3],
			    offsetpulser[s][mo][mb][t][4]);

}
void HMdcOffset::fillArrays(TH1F *hOffset, Int_t s,Int_t mo,Int_t mb,Int_t t)
{
    if(!isPulserFile)
    {
	hOffset->Fill(crosspointX);
	offsets  [s][mo][mb][t] = crosspointX;
	offsetErr[s][mo][mb][t] = totalsigma;
	offset1  [s][mo][mb][t] = -yequalzero;
    }
    else
    {
	hOffset->Fill(fitGaussMean);
	offsets  [s][mo][mb][t] = -fitGaussMean;
	offsetErr[s][mo][mb][t] =  fitGaussSigma;
	offset1  [s][mo][mb][t] = -yequalzero;
    }
}
void HMdcOffset::fillCalParRaw(TH1F *hOffsetcorr, Int_t s,Int_t mo,Int_t mb,Int_t t)
{
    // fills CalParRaw with offsets, offseterrors and Methods
    // Checks if offset is in valid range arround mean value of offsets.
    // If it is out of range the value is replaced by the mean value.
    // Not connected channels are marked with -7.
    myoffset = offsets  [s][mo][mb][t];
    myerror  = offsetErr[s][mo][mb][t];
    HMdcLookupChan& chan   = (*lookupgeom)[s][mo][mb][t];
    Int_t connected        = 0;
    connected              = chan.getNCell();
    if(!myoffset){ myoffset = 0;}

    if(myoffset >= 0)   // all offsets where no error showed up
    {
	if((myoffset < meanhOffset-validRange) ||
	   (myoffset > meanhOffset+validRange))
	{   // if offset out of alowed range replace it by mean

	    (*calparraw)[s][mo][mb][t].setOffset(meanhOffset);
	    (*calparraw)[s][mo][mb][t].setOffsetMethod(0);
	    (*calparraw)[s][mo][mb][t].setOffsetErr(100);
	    offsets     [s][mo][mb][t] = meanhOffset;

	    hOffsetcorr->Fill(meanhOffset);
	}
	else  // if offsets in allowed range and no error flag
	{
	    hOffsetcorr->Fill(myoffset);
	    (*calparraw)[s][mo][mb][t].setOffset(myoffset);
	    (*calparraw)[s][mo][mb][t].setOffsetMethod(2);
	    if(myerror > 99) // if some nonsense value shows up in Err
	    {
		(*calparraw)[s][mo][mb][t].setOffsetErr(100);
		offsetErr   [s][mo][mb][t] = 100;
	    }
	    else // if Err gives nomal values
	    {
		(*calparraw)[s][mo][mb][t].setOffsetErr(myerror);
	    }
	}
    }
    else
    {  // if offsets contain error flags
	if(connected == -1) // if no wire is connected
	{
	    (*calparraw)[s][mo][mb][t].setOffset(-7);
	    offsets     [s][mo][mb][t] = -7;
	    hOffsetcorr->Fill(-7);
	}
	else // if wire is connected but error showed up
	{
	    (*calparraw)[s][mo][mb][t].setOffset(myoffset);
	    hOffsetcorr->Fill(myoffset);
	}
	(*calparraw)[s][mo][mb][t].setOffsetMethod(2);
	(*calparraw)[s][mo][mb][t].setOffsetErr(100);
	offsetErr   [s][mo][mb][t] = 100;
    }
}

Bool_t HMdcOffset::finalize(void)
{
    // This function is called after the execute function is finished. At this point
    // the arrays for the drift-time are filled. Froms this arrays the histograms for
    // the drift-time and the integrated drift-time are filled. The fits for the calcutation
    // of the offsets are done and the offsets and the sigma of the offsets are
    // calculated .All histograms are written to a rootfile output and the information
    // of the fits to an ascii debugging output. The offset and the sigma of the offset
    // are filled into the container (HMdcCalParRaw) for the calibration parameters.
    // To characterize the method of calculation of the offsets the function setOffsetMethod()
    // of HMdcCalParRaw is used. As default value for the method 2 is used for  automatic
    // calibration. If no calibration is done this value is set to 0.
    cout << "From HMdcOffset::finalize" << endl;

    // Start Stop Watch
    TStopwatch timer;
    timer.Start();

    // open Root file for writing
    htime1temp = new MyHist("time1temp","time1temp", nbin,-nbin+0.5,0.5); //temp time1 hist.

    TFile *file       = NULL;
    offsetTuple       = NULL;
    offsetPulserTuple = NULL;
    ofstream *fout    = NULL;
    ferrorlog         = fopen("offset_error_log.txt","w"); // open file for log of errors/warnings

    if( fNameRootOffset )
    {
	TH1F *hOffset     = 0;
        TH1F *hOffsetcorr = 0;

	file = new TFile(fNameRootOffset,"RECREATE");
	file->cd();

	if(!fillHistsOnly)
	{
	    offsetTuple       = new TNtuple("offset"    ,"offset"    ,"s:m:mb:tdc:lay:cell:slope:slopeErr:off:offErr:integral:off1:fit1slope:fit2slope");
	    offsetPulserTuple = new TNtuple("multipeaks","multipeaks","s:m:mb:tdc:lay:cell:off:offErr:integral:p1:p2:p3:p4:p5");

	    hOffset     = new TH1F("Offset", "Offset", 512, 0, 2048);
	    hOffsetcorr = new TH1F("Offsetcorr", "Offsetcorr", 512, 0, 2048);


	    // open ascii file for writing
	    if (fNameAsciiOffset) { fout = openAsciiFile(); }
	}


	Int_t err   = -3;
	meanhOffset =  0;

	initArrays();

	HDetector *mdcDet = gHades->getSetup()->getDetector("Mdc");
	if (!mdcDet){
	    cout << "Error in HMdcOffset::finalize: Mdc-Detector setup (gHades->getSetup()->getDetector(\"Mdc\")) missing." << endl;
	}
	else
	{

	    for(Int_t s = 0; s < 6; s ++)
	    {  //loop over sectors
		cout<<"Sector "<<s<<endl;
		TDirectory* dirSec = NULL;
		if (file) { dirSec = Mkdir(file, "sector", s); }
		for(Int_t mo = 0; mo < 4; mo ++)
		{  //loop over modules
		    cout<<"Module "<<mo<<endl;
		    if (!mdcDet->getModule(s, mo)) { continue; }
		    TDirectory*   dirMod = NULL;
		    if (dirSec) { dirMod = Mkdir(dirSec, "module", mo); }
		    if( !fillHistsOnly ) { createHist_2D(s,mo); }
		    Int_t nMb = (*calparraw)[s][mo].getSize();
		    for(Int_t mb = 0; mb < nMb; mb ++)
		    {  //loop over layer
			TDirectory* dirMbo = NULL;
			if (dirMod)
			{
			    dirMbo = Mkdir(dirMod, "mbo", mb, 2);
			    dirMbo->mkdir("hinv");
			    dirMbo->mkdir("hint");
			}
			Int_t nTdc = (*calparraw)[s][mo][mb].getSize();
			for(Int_t t = 0; t < nTdc; t ++)
			{  //loop over cells
			    createHist(file, s, mo, mb, t);
			    fillHist  (s, mo, mb, t);
			    
			    if( !fillHistsOnly )
			    {
				if ((err = fitHist(s, mo, mb, t))) { crosspointX = fitpar1 = -err; }
				fillArrays (hOffset,s,mo,mb,t);
				fillHist_2D(s,mo,mb,t);
			    }
			    if (file){ writeHist (file); }
			    if (fNameAsciiOffset && !fillHistsOnly ){ writeAscii(*fout, s, mo, mb, t); }
			    deleteHist();
			}
			if (dirMbo){ dirMbo->TDirectory::Cd(".."); }
			cout << "." << flush;
		    }
		    if( !fillHistsOnly )
		    {
			writeHist_2D(s,mo);
			deleteHist_2D();
		    }
		    cout<<" "<<endl;
		    if (dirMod){ dirMod->TDirectory::Cd(".."); }
		}
		cout<<" "<<endl;
		if (dirSec){ dirSec->TDirectory::Cd(".."); }
	    }
	    cout << endl;
	}

        //----------------------------------------------------
	// calculate offsets per MBO from single
	// offsets . A 2 step filtering of outliers
	// is applied to reduce influence of noisy and low
	// stat channels
	if(perMBOafterSingle && !fillHistsOnly)
	{
            cout<<"Fill per MBO after Single"<<endl;
            Float_t offtemp[96];
	    for(Int_t s = 0; s < 6; s ++){
		for(Int_t mo = 0; mo < 4; mo ++){
		    for(Int_t mb = 0; mb < 16; mb ++){

			Int_t count = 0;
			memset(offtemp,0,sizeof(Float_t) * 96);

			//----------------------------------------------------
                        // calculate first mean offset of mbo
			for(Int_t t = 0; t < 96; t ++){
			    Float_t off = offsets[s][mo][mb][t];
			    if(off > 0){ // filter invalid offsets
                                offtemp[count] = off;
				count++;
			    }
			}

			Float_t mean      = TMath::Mean(count,offtemp);
			//----------------------------------------------------

			//----------------------------------------------------
			// now filter in large in arround mean to throw out
                        // outliers. Caclulate truncated mean (10% highest/lowest cut)
			memset(offtemp,0,sizeof(Float_t) * 96);
                        count = 0;
			for(Int_t t = 0; t < 96; t ++){
			    Float_t off = offsets[s][mo][mb][t];
			    if(off > 0 && fabs(off - mean) < filterwindow1){ // filter invalid offsets arround mean
				offtemp[count] = off;
				count++;
			    }
			}
			if(count > 0){
			    mean = HTool::truncatedMean(count,offtemp,0.1);
			}
			//----------------------------------------------------

			//----------------------------------------------------
			// now filter arround narrow window arround
                        // improved mean and calulate final mean
			memset(offtemp,0,sizeof(Float_t) * 96);
                        count = 0;
			for(Int_t t = 0; t < 96; t ++){
			    Float_t off = offsets[s][mo][mb][t];
			    if(off > 0 && fabs(off - mean) < filterwindow2){ // filter invalid offsets arround trunc mean
				offtemp[count] = off;
				count++;
			    }
			}

                        mean = TMath::Mean(count,offtemp);
			for(Int_t t = 0; t < 96; t ++){
                            Float_t off = offsets[s][mo][mb][t];
			    if(off > 0) { offsets[s][mo][mb][t] = mean; }   // keep negative or 0
			}
			//----------------------------------------------------
		    }
		}
	    }



	    for(Int_t s = 0; s < 6; s ++)
	    {  //loop over sectors

		cout<<"Sector "<<s<<endl;
		for(Int_t mo = 0; mo < 4; mo ++)
		{  //loop over modules
		    if (!mdcDet->getModule(s, mo)) { continue; }
		    file->cd();
		    file->TDirectory::Cd(Form("sector %i/module %i",s,mo));
		    cout<<"Module "<<mo<<endl;

		    createHist_2D(s,mo,kTRUE);

		    Int_t nMb = (*calparraw)[s][mo].getSize();
		    for(Int_t mb = 0; mb < nMb; mb ++)
		    {  //loop over layer

			Int_t nTdc = (*calparraw)[s][mo][mb].getSize();

			file->cd();
			file->cd(Form("sector %i/module %i/mbo %02i",s,mo,mb));

			for(Int_t t = 0; t < nTdc; t ++)
			{  //loop over cells
			    createHist(file, s, mo, mb, t, kTRUE);
			    fillHist_2D(s,mo,mb,t);
			    deleteHist();
			}
			cout << "." << flush;
		    }
		    file->cd();
		    file->TDirectory::Cd(Form("sector %i/module %i",s,mo));
		    writeHist_2D(s,mo);
		    deleteHist_2D();
		    cout<<" "<<endl;
		}
		cout<<" "<<endl;
	    }
	    cout << endl;
	    file->cd();
	}

        //------------------------------------------------------------------

	if( !fillHistsOnly )
	{
	    // write offsets to calparraw
	    myoffset    = 0;
	    myerror     = 0;
	    meanhOffset = (Float_t)((Int_t)(hOffset->GetMean()));

	    for(Int_t s = 0; s < 6; s ++)
	    {  //loop over sectors
		for(Int_t mo = 0; mo < 4; mo ++)
		{  //loop over modules
		    if (!mdcDet->getModule(s, mo)){ continue; }
		    Int_t nMb = (*calparraw)[s][mo].getSize();
		    for(Int_t mb = 0; mb < nMb; mb++)
		    {  //loop over layer
			Int_t nTdc = (*calparraw)[s][mo][mb].getSize();
			for(Int_t t = 0; t < nTdc; t ++)
			{  //loop over cells
			    fillCalParRaw(hOffsetcorr,s,mo,mb,t);
			    fillNTuples(s,mo,mb,t);
			}
		    }
		}
	    }


	    hOffset          ->Write();
	    hOffsetcorr      ->Write();
	    offsetTuple      ->Write();
	    offsetPulserTuple->Write();

	    delete hOffset;
	    delete hOffsetcorr;



	    //------------------------------------------------------------------
	    // Fill some control hists for mean/rms per mbo, good channels per mbo
            // and rms of deviation from truncated mean per mbo
	    const Char_t* mdcs[4]      = {"MDCI","MDCII","MDCIII","MDCIV"};


	    TCanvas* result2 = new TCanvas("mean_values"         ,"mean values"        ,1000,800);
	    TCanvas* result3 = new TCanvas("rms_offsets"         ,"rms offsets"        ,1000,800);
	    TCanvas* result4 = new TCanvas("good_channels"       ,"good channels"      ,1000,800);
	    TCanvas* result5 = new TCanvas("deviation_offsets"   ,"deviation_offsets"  ,1000,800);


	    TH2F* hmean     = new TH2F("hmean"      ,"mean offset per MB"         ,24,0,24,16  ,0,16);
	    TH2F* hrms      = new TH2F("hrms"       ,"rms offset per MB"          ,24,0,24,16  ,0,16);
	    TH2F* hcts      = new TH2F("hcts"       ,"good channels per MB"       ,24,0,24,16  ,0,16);
	    TH2F* hdev      = new TH2F("hdev"       ,"deviation from mean per MB" ,24,0,24,16  ,0,16);

	    for(Int_t s = 0; s < 6; s++){
		for(Int_t m = 0; m < 4; m++){
		    hmean      ->GetXaxis()->SetBinLabel(m*6+s+1,Form("%s%i",mdcs[m],s));
		    hrms       ->GetXaxis()->SetBinLabel(m*6+s+1,Form("%s%i",mdcs[m],s));
		    hcts       ->GetXaxis()->SetBinLabel(m*6+s+1,Form("%s%i",mdcs[m],s));
		    hdev       ->GetXaxis()->SetBinLabel(m*6+s+1,Form("%s%i",mdcs[m],s));
		}
	    }
	    hmean      ->GetXaxis()->LabelsOption("v");
	    hrms       ->GetXaxis()->LabelsOption("v");
	    hcts       ->GetXaxis()->LabelsOption("v");
	    hdev       ->GetXaxis()->LabelsOption("v");

	    hmean      ->SetYTitle("mbo number");
	    hrms       ->SetYTitle("mbo number");
	    hcts       ->SetYTitle("mbo number");
	    hdev       ->SetYTitle("mbo number");

	    // temporary working histograms
	    TH1F* htmprms = new TH1F("htmprms","htmprms",2000,    0,2000);
	    TH1F* htmpdev = new TH1F("htmpdev","htmpdev",4000,-2000,2000);

	    Float_t offsetMB[96];
	    Float_t offsetWindow = 5.;  // window arround truncated mean for second iteration
	    Float_t trunc        = 0.1; // fraction of lowest/highest values to truncate for the mean calulation


	    for(Int_t s = 0; s < 6; s++){
		for(Int_t m = 0; m < 4; m++){
		    for(Int_t mb = 0; mb < 16; mb++){

			Int_t    ctall         = 0;
			Int_t    ctsuppressed  = 0;
			Float_t  sum           = 0;
			Float_t  off           = 0;
			Float_t  mean          = 0;
			Double_t truncMean     = 0;
			htmprms->Reset();
			htmpdev->Reset();

			memset(&offsetMB[0],0,96 * sizeof(Float_t)); // reset array to 0

			//-------------------------------------------------
			// fill temporary array of offset values
			// to calulate the truncated mean of offset per MB
			for(Int_t t = 0; t < 96; t++){
			    off = offsets[s][m][mb][t];

			    if(off <= 0) { continue; }

			    htmprms->Fill(off);
			    offsetMB[t] = off;
			    ctall ++;
			}

			HTool::sort(96,offsetMB,0,kTRUE,kTRUE);  // sort the array descending , overwrite
			truncMean = HTool::truncatedMean(ctall,offsetMB, trunc, kTRUE, kFALSE); // 10% , descending, do not overwrite

			//-------------------------------------------------
			// filter arround the truncated mean and
			// calculate the improved mean
			for(Int_t t = 0; t < 96; t++){
			    off = offsets[s][m][mb][t];
			    if(off <= 0) { continue; }
			    if(fabs(off - truncMean) > offsetWindow) { continue; }

			    sum += off;
			    ctsuppressed ++;
			}

			if(ctsuppressed != 0){
			    // second iteration of mean
			    mean = sum / ctsuppressed;
			} else { mean = truncMean; }
			//-------------------------------------------------

			//-------------------------------------------------
			// calulate the deviation of each channel
			// from the mean of the MB .
			for(Int_t t = 0; t < 96; t++){
			    off = offsets[s][m][mb][t];
			    if(off <= 0) { continue; }
			    htmpdev->Fill(off - mean);
			}
			//-------------------------------------------------


			//-------------------------------------------------
			// fill results
			if(ctsuppressed != 0){
			    hdev ->Fill(m * 6 + s,mb,htmpdev->GetRMS());
			    hmean->Fill(m * 6 + s,mb,mean);
			    hrms ->Fill(m * 6 + s,mb,htmprms->GetRMS());
			    hcts ->Fill(m * 6 + s,mb,ctall);
			}
			//-------------------------------------------------

		    }
		}
	    }


	    //-------------------------------------------------
	    // draw results

	    result2->cd();
	    HTool::roundHist(hmean,0);
	    hmean->DrawCopy("colz text");

	    result3->cd();
	    HTool::roundHist(hrms,1);
	    hrms->DrawCopy("colz text");

	    result4->cd();
	    hcts->DrawCopy("colz text");

	    result5->cd();
	    HTool::roundHist(hdev,1);
	    hdev->DrawCopy("colz text");

	    delete htmprms ;
	    delete htmpdev ;

	    file->cd();

            result2->Write();
            result3->Write();
            result4->Write();
            result5->Write();
	    //------------------------------------------------------------------

	}

	// stop watch
	cout << "Time of cell loop (offset calculation time):" << endl;
	timer.Print();
	timer.Stop();

	// close root file
	file->Save();
	file->Close();
	delete file;

	if (fNameAsciiOffset && !fillHistsOnly )
	{ // close ascii file
	    fout->close();
	    delete fout;
	}
	else
	{
	    cout<<"WARNING: NO ASCII OUTPUT SET, NO FILE WRITTEN TO DISK!"<<endl;
            fprintf(ferrorlog,"%s\n","WARNING: NO ASCII OUTPUT SET, NO FILE WRITTEN TO DISK!");
	}
    }
    else
    {
	cout<<"WARNING: NO ROOT OUTPUT SET, NO FILE WRITTEN TO DISK!"<<endl;
        fprintf(ferrorlog,"%s\n","WARNING: NO ROOT OUTPUT SET, NO FILE WRITTEN TO DISK!");
    }
    
    fclose(ferrorlog);
    return kTRUE;
}

TDirectory *HMdcOffset::Mkdir(TDirectory *dirOld,const Char_t *c, Int_t i, Int_t p)    //! Makes new Dir, changes to Dir, returns pointer to new Dir
{
    // Function to create subdirectories
    static Char_t sDir[255];// = new Char_t[strlen(c)+3];
    static Char_t sFmt[10];
    sprintf(sFmt, "%%s %%0%1ii", p);
    sprintf(sDir, sFmt, c, i);
    TDirectory *dirNew = dirOld->mkdir(sDir);
    dirOld->cd(sDir);
    //  free (sDir);
    return dirNew;
}

Float_t HMdcOffset::getstarttime(){
    // Need some work for multiple hists in start detector
    // Better select multiplicity 1 in start.
    static Int_t ntimes0         = 0;
    static Int_t ntimesGreater1  = 0;

    Int_t i             =  0;
    Int_t startmod      = -1;
    HStart2Hit* starthit =  0;
    Float_t starttime   =  0;
    iter_start->Reset();
    while ((starthit = (HStart2Hit *)iter_start->Next()) != 0) {
	startmod = starthit->getModule();
	if(startmod == 0)
	{
	    i ++;
	    if(starthit->getFlag()){ starttime = -starthit->getTime(); }  // changed
	}
    }

    if(i==0) ntimes0++;
    if(i>1 ) ntimesGreater1++;

    //if(i != 1){ Error("getstarttime(int)","Multiplicity in Start > 1 or 0"); }

    if(gHades->getEventCounter()%1000==0){
	Info("getstarttime()","Start time summary for 1000 events : %i == 0, %i > 1 hit multiplicity",ntimes0,ntimesGreater1);
    }
    if(TMath::Finite(starttime) == 0){ starttime = 0; }
    if(TMath::Abs(starttime) > 100)  { starttime = 0; }

    return starttime;
}

Int_t HMdcOffset::execute()
{
    Int_t result = 0;
    if(readHists)     { return 0; } // nothing to fill
    if(useClusters)   { result = executeClus(); }
    else              { result = executeRaw();  }
    return result;
}
Int_t HMdcOffset::executeClus()
{
    // Fired cells are take from the list of Cells inside one HMdcClus.
    // Raw data of Time1  multiplied by the slope of the channel taken from the
    // container of the calibration parameters are filled into the array for the
    // drift-time . This array contains 2048 (corresponding to the resulution of
    // the tdc chip) entries for each tdc channel.
    static HMdcRaw *raw   = NULL;
    static HMdcClus *clus = NULL;

    Float_t testTime1     = 0;
    Float_t testTime2     = 0;

    Double_t wireoff      = 0.;
    Float_t starttime     = 0;

    Double_t xp1,yp1,zp1,xp2,yp2,zp2,signalpath;
    Int_t s, mo, mb, t;
    Int_t l,c;

    iter_clus->Reset();
    if(skipcounter > nSkipEvents)
    {
	if(!noStart) // Start Time is used
	{
	    starttime = getstarttime();
	}
	while ((clus = (HMdcClus*)iter_clus->Next()))
	{
	    s = mo = mb = t = -99;
	    l = c = -99;
	    xp1 = clus->getX();
	    yp1 = clus->getY();
	    zp1 = clus->getZ();
	    xp2 = clus->getXTarg();
	    yp2 = clus->getYTarg();
	    zp2 = clus->getZTarg();

	    s           = clus->getSec();
	    locraw[0]   = s;

	    Int_t ioseg = clus->getIOSeg();
	    //Int_t mod  = clus->getMod();


	    for(Int_t lay = 0; lay < 12; lay ++)
	    {  // loop over layers
		//---------------finding module-----------
		//if(mod < 0)
		//{ 	// combined clusters
		    if(ioseg == 0){
			(lay < 6)? locraw[1] = 0 : locraw[1] = 1;
		    } else if(ioseg == 1){
			(lay < 6)? locraw[1] = 2 : locraw[1] = 3;
		    }
		//}
		//else
		//{  // chamber clusters
		  //  locraw[1] = mod;
		//}
		mo = locraw[1];
		//---------------finding layer------------
		(lay < 6)? l = lay : l = lay - 6;
		//----------------------------------------

		Int_t nCells = clus->getNCells(lay);
		for(Int_t cell = 0; cell < nCells; cell ++)
		{  // loop over cells in layer
		    c = clus->getCell(lay,cell);

		    HMdcLookupCell& mycell = (*lookupraw)[s][mo][l][c];
		    mb        = mycell.getNMoth();
		    t         = mycell.getNChan();
		    locraw[2] = mb;
		    locraw[3] = t;

		    raw       = (HMdcRaw*)rawCat->getObject(locraw);
		    if(raw)
		    {
			if(useWireOffset)
			{
			    signalpath = (*sizescells)[s][mo][l].getSignPath(xp1,yp1,zp1,xp2,yp2,zp2,c);
			    if(signalpath > 0) { wireoff = signalpath*signalspeed; }
			    else               { wireoff = 0.; }
			}

			Float_t slope = ((*calparraw)[s][mo][mb][t].getSlope()) ;
			Float_t t1    = raw->getTime(1);
			Float_t t2    = raw->getTime(2);
			Int_t bin     =  (Int_t)(2048 - (t1 * slope + starttime - wireoff));

			if( slope != 0) {
			    testTime1 = t1 - (starttime/slope);
			    testTime2 = t2 - (starttime/slope);
			} else {
			    testTime1 = t1;
			    testTime2 = t2;
			}

			if(useTimeCuts)  // if Time Cuts and Start Time
			{
			    if(testTimeCuts(s,mo,testTime1,testTime2))
			    {
				// we have to use Float_ts here

				if(!perMBO) {
				    (*hreverse)[s][mo][mb][t][bin] ++;
				} else {
				    for(Int_t ti = 0; ti < 96; ti ++){
					(*hreverse)[s][mo][mb][ti][bin] ++;
				    }
				}
			    }
			} else {   // No Time Cuts and Start Time
			    if(!perMBO) {
				(*hreverse)[s][mo][mb][t][bin] ++;
			    } else {
				for(Int_t ti = 0; ti < 96; ti ++){
				    (*hreverse)[s][mo][mb][ti][bin] ++;
				}
			    }
			}
		    } else { Error("executeClus()","Error: NULL pointer for HMdcRaw retrieved!!"); }
		}
	    }
	}
	if(eventcounter % nStep == 0){ cout<<eventcounter<<" events analyzed"<<endl; }
    }
    skipcounter  ++;
    eventcounter ++;
    return 0;
}
Int_t HMdcOffset::executeRaw()
{
    // Raw data of Time1  multiplied by the slope of the channel taken from the
    // container of the calibration parameters are filled into the array for the
    // drift-time . This array contains 2048 (corresponding to the resulution of
    // the tdc chip) entries for each tdc channel.
    static HMdcRaw *raw = NULL;
    Float_t testTime1   = 0;
    Float_t testTime2   = 0;
    Float_t starttime   = 0.;

    Int_t s, mo, mb, t;

    iter->Reset();
    if(skipcounter > nSkipEvents)
    {
	if(!noStart) // Start Time is used
	{
	    starttime = getstarttime();
	}
	while ((raw = (HMdcRaw*)iter->Next()))
	{
	    s = mo = mb = t = -99;
	    raw->getAddress(s, mo, mb, t);

	    Float_t slope = ((*calparraw)[s][mo][mb][t].getSlope()) ;
	    Float_t t1    = raw->getTime(1);
	    Float_t t2    = raw->getTime(2);
	    Int_t bin     =  (Int_t)(2048 - (t1 * slope + starttime));

	    if(slope != 0) {
		testTime1 = t1-(starttime/slope);
		testTime2 = t2-(starttime/slope);
	    } else {
		testTime1 = t1;
		testTime2 = t2;
	    }

	    if(useTimeCuts)  // if Time Cuts and Start Time
	    {
		if(testTimeCuts(s,mo,testTime1,testTime2))
		{
		    // we have to use Float_ts here
		    if(!perMBO) {
			(*hreverse)[s][mo][mb][t][bin] ++;
		    } else {
			for(Int_t ti = 0; ti < 96; ti ++){
			    (*hreverse)[s][mo][mb][ti][bin] ++;
			}
		    }
		}
	    } else {   // No Time Cuts and Start Time
		if(!perMBO) {
		    (*hreverse)[s][mo][mb][t][bin] ++;
		} else {
		    for(Int_t ti = 0; ti < 96; ti ++){
			(*hreverse)[s][mo][mb][ti][bin] ++;
		    }
		}
	    }
	}
	if(eventcounter % nStep == 0){ cout<<eventcounter<<" events analyzed"<<endl; }
    }
    skipcounter  ++;
    eventcounter ++;
    return 0;
}
