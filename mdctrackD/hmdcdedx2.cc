//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : J. Markert
////////////////////////////////////////////////////////////////////////////
// HMdcDeDx
//
// This transformation class calculates the "pseudo dEdx" from t2-t1 (time above threshold)
// of all fired drift cells included in one HMdcSeg. The transformation is performed doing a
// normalization of the measured t2-t1 with impact angle and minimum distance to wire
// (MDCCAL2 parametrization) and the algorithm to normalize the single measurements and
// average over all cells included in the segment.
//-------------------------------------------------------------------------
// Calculation of dEdx:
// Float_t calcDeDx(HMdcSeg*,Float_t*,Float_t*,UChar_t*,Float_t*,UChar_t*,
//		    Int_t vers=2,Int_t mod=2, Bool_t useTruncMean=kTRUE, Float_t truncMeanWindow=-99.,Int_t inputselect)
// calculates dedx of a MDC (or 2) segment by doing normalization for
// impact angle and distance from wire for the fired cells of
// the segment. The measurements are sorted in decreasing order,
// the mean and sigma is calculated. Afterwards the truncated mean
// is calculated and returned as dedx. Mean,sigma,number of wires before
// truncating,truncated mean,truncated mean sigma and number of wires
// after truncating can be obtained by the returned pointers.
// Different methods can be selected:
// vers==0 : Input filling from inner HMdcSeg
// vers==1 : Input filling from outer HMdcSeg
// vers==2 : Input filling from inner+outer HMdcSeg (default)
// mod==0  : calc dedx from 1st module in segment
// mod==1  : calc dedx from 2nd module in segment
// mod==2  : calc dedx from whole segment (default)
// useTruncMean: kTRUE (default) apply truncated Mean
// truncMeanWindow (unit: SIGMA RMS of mean TOT): -99 (default) use standard window
// inputselect==0 : fill from segment (default,wires rejected by fit are missing)
// inputselect==1 : fill from cluster
// returns -99 if nothing was calculated
//-------------------------------------------------------------------------
// Settings of the truncated Mean method:
// The truncated mean is calculated according to the set window (in sigma units)
// (setWindow(Float_t window)) arround the mean. For the calculation of the truncated
// mean only drift cells with a dEdx inside the window arround the mean (calculated from
// all drift cells of the segment) will be taken into account.
// With setMinimumWires(Int_t minwires) the algorithm can be forced to keep
// a minimum number of wires. The method will stop removing drift cells from the active
// sample if the minimum number is reached.
////////////////////////////////////////////////////////////////////////////
#include "hmdcdedx2.h"
#include "hmessagemgr.h"
#include "hparamlist.h"
#include "hmatrixcategory.h"
#include "hlinearcategory.h"
#include "hrecevent.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdccal1.h"
#include "hmdcseg.h"
#include "hmdchit.h"
#include "hmdcclusinf.h"
#include "hmdcclusfit.h"
#include "hmdcclus.h"
#include "hmdcwirefit.h"
#include "hmdcsizescells.h"
#include "hphysicsconstants.h"
#include "hgeantkine.h"

#include "TStyle.h"
#include "TMath.h"
#include "TRandom.h"
#include "TH1.h"
#include <stdlib.h>

ClassImp(HMdcDeDx2)

Float_t HMdcDeDx2::MaxMdcMinDist[4] = {4.,4.,8.,9.};
Bool_t  HMdcDeDx2::debug=kFALSE;


HMdcDeDx2::HMdcDeDx2(const Char_t* name,const Char_t* title,
		     const Char_t* context)
    : HParCond(name,title,context)
{
    //
    clear();
    loccal.set(4,0,0,0,0);
    minimumWires  = 5;
    isInitialized = kFALSE;
    setWindow(1.);
    measurements.Set(MAX_WIRES);
    measurements.Reset(-99);
    module = 2;
    method = 2;
    useCalibration = kTRUE;
    for(Int_t i = 0; i < 4; i ++){
	MinDistStep[i] = MaxMdcMinDist[i] / (Float_t) N_DIST;
        parmindistcut[i] = MaxMdcMinDist[i];
    }
    AngleStep = 5.;

    memset(&parMax[0][0][0][0],0,sizeof(Double_t) * 6 * 4 * N_ANGLE * N_DIST);
    memset(&pargaincorr[0][0][0][0],0,sizeof(Double_t) * 6 * 4 * 6 * 220);
}
HMdcDeDx2::~HMdcDeDx2()
{
    // destructor
}
void HMdcDeDx2::clear()
{
    hefr    = 0;
    status  = kFALSE;
    resetInputVersions();
    changed = kFALSE;
}
void HMdcDeDx2::printParam(TString opt)
{
    // prints the parameters of HMdcDeDx2 to the screen.

    cout<<"HMdcDeDx2:"<<endl;
    if(opt.CompareTo("par") == 0 || opt.CompareTo("all") == 0)
    {
	cout<<"par:"<<endl;

	for(Int_t s = 0; s < 6; s ++){
	    for(Int_t m = 0; m < 4; m ++){
		for(Int_t a = 0; a < N_ANGLE; a ++){
		    for(Int_t d = 0; d < N_DIST; d ++){
			    printf("s %i m %i angle %2i dist %2i %1.15e %1.15e %1.15e %1.15e\n",
				   s,m,a,d,
				   par[s][m][a][d][0],par[s][m][a][d][1],par[s][m][a][d][2],par[s][m][a][d][3]);
		    }
		}
	    }
	}
    }
    if(opt.CompareTo("parMax") == 0 || opt.CompareTo("all") == 0)
    {
	cout<<"parMax:"<<endl;

	for(Int_t s = 0; s < 6; s ++){
	    for(Int_t m = 0; m < 4; m ++){
		for(Int_t a = 0; a < N_ANGLE; a ++){
		    for(Int_t d = 0; d < N_DIST; d ++){
			printf("s %i m %i angle %2i dist %2i %7.3f\n",
			       s,m,a,d,
			       parMax[s][m][a][d]);
		    }
		}
	    }
	}
    }
    if(opt.CompareTo("shiftpar") == 0 || opt.CompareTo("all") == 0)
    {
	cout<<"shift par:"<<endl;
	for(Int_t s = 0; s < 6; s ++){
	    for(Int_t m = 0; m < 4; m ++){
		for(Int_t a = 0; a < N_ANGLE; a ++){
		    for(Int_t d = 0; d < N_DIST; d ++){
			    printf("s %i m %i angle %2i dist %2i %1.15e %1.15e\n",
				   s,m,a,d,
				   shiftpar[s][m][a][d][0],shiftpar[s][m][a][d][1]);
		    }
		}
	    }
	}
    }

    if(opt.CompareTo("pargaincorr") == 0 || opt.CompareTo("all") == 0)
    {
	cout<<"pargaincorr:"<<endl;

	for(Int_t s = 0; s < 6; s ++){
	    for(Int_t m = 0; m < 4; m ++){
		for(Int_t l = 0; l < 6; l ++){
		    for(Int_t c = 0; c < 220; c ++){
			printf("s %i m %i l %i c %i %7.3f\n",
			       s,m,l,c,
			       pargaincorr[s][m][l][c]);
		    }
		}
	    }
	}
    }

    if(opt.CompareTo("parmindistcut") == 0 || opt.CompareTo("all") == 0)
    {
	cout<<"parmindistcut:"<<endl;

	for(Int_t m = 0; m < 4; m ++){
	    printf("m %i %7.3f\n",
		   m,
		   parmindistcut[m]);
	}
    }

    cout<<"window:"<<endl;
    printf("window %7.3f \n",window);
    cout<<"hefr:"<<endl;
    printf("hefr %7.3f \n",hefr);

}
Bool_t HMdcDeDx2::createMaxPar(Bool_t print)
{
    // create the maximum allowed value of ToT to keep
    // the numerical calulations inside the range of double.
    // The procedure loops over all s,m,abin,dbin to set
    // the boundary automatically. This function needs
    // the parameter array for the fit functions to be initialized
    // before. The boundary value are stored inside the container.
    Double_t dEdX;
    Double_t ToT;
    for(Int_t s = 0; s < 6; s ++){
	for(Int_t m = 0; m < 4; m ++){
	    for(Int_t a = 0; a < N_ANGLE; a ++){
		for(Int_t d = 0; d <N_DIST; d ++){
		    Double_t* p = &par[s][m][a][d][0];

		    ToT  = 1.;
		    dEdX = TMath::Power(10.,(TMath::Power((ToT - p[0]) / p[1],(1. / p[2])))) - p[3];
		    while(TMath::Finite(dEdX))
		    {
                        ToT += 1.;
			dEdX = TMath::Power(10.,(TMath::Power((ToT - p[0]) / p[1],(1. / p[2])))) - p[3];
			if(!TMath::Finite(dEdX)) {
			    parMax[s][m][a][d] = ToT - 1;
			    break;
			}
		    }
		    if(print)
		    {
			cout<<"s " <<s
			    <<" m "<<m
			    <<" a "<<a
			    <<" d "<<d
			    <<" ToTmax " <<parMax[s][m][a][d]
			    <<endl;
		    }
		}
	    }
	}
    }

  return kTRUE;
}
Bool_t HMdcDeDx2::initContainer()
{
    if(!isInitialized)
    {

	catcal     = (HCategory*)((HRecEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcCal1);
	if(!catcal) { Error("initContainer()","HMdcCal1 Category not found in current Event!");}

	cathit     = (HCategory*)((HRecEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcHit);
	if(!cathit) { Error("initContainer()","HMdcHit Category not found in current Event!");}

	catclusinf = (HCategory*)((HRecEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcClusInf);
	if(!catclusinf) { Error("initContainer()","HMdcClusInf Category not found in current Event!");}

	catclus     = (HCategory*)((HRecEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcClus);
	if(!catclus) { Error("initContainer()","HMdcClus Category not found in current Event!");}

   	sizescells = HMdcSizesCells::getExObject();
	if(!sizescells)
	{
	    Error("init()","NO HMDCSIZESCELLS CONTAINER IN INPUT!");
	    return kFALSE;
	}
	//if(!sizescells->hasChanged()) sizescells->initContainer();
	isInitialized = kTRUE;
    }
    return kTRUE;
}
void HMdcDeDx2::putParams(HParamList* l)
{
    // Puts all params of HMdcDeDx2 to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("par"        ,&par     [0][0][0][0][0],6 * 4 * N_ANGLE * N_DIST * N_PARAM      );
    l->add("parMax"     ,&parMax  [0][0][0][0]   ,6 * 4 * N_ANGLE * N_DIST                );
    l->add("shiftpar"   ,&shiftpar[0][0][0][0][0],6 * 4 * N_ANGLE * N_DIST * N_SHIFT_PARAM);

    l->add("pargaincorr",&pargaincorr[0][0][0][0],6 * 4 * 6 * 220);
    l->add("parmindistcut",&parmindistcut[0],4);

    l->add("window"   ,window);
    l->add("hefr"     ,hefr);


}
Bool_t HMdcDeDx2::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if (!( l->fill("par"      ,&par     [0][0][0][0][0],6 * 4 * N_ANGLE * N_DIST * N_PARAM      ))) return kFALSE;
    if (!( l->fill("parMax"   ,&parMax  [0][0][0][0]   ,6 * 4 * N_ANGLE * N_DIST                ))) return kFALSE;
    if (!( l->fill("shiftpar" ,&shiftpar[0][0][0][0][0],6 * 4 * N_ANGLE * N_DIST * N_SHIFT_PARAM))) return kFALSE;

    if (!( l->fill("pargaincorr"  ,&pargaincorr[0][0][0][0],6 * 4 * 6 * 220                     ))) return kFALSE;
    if (!( l->fill("parmindistcut",&parmindistcut[0]       ,4                                   ))) return kFALSE;

    if (!( l->fill("window"   ,&window                                                          ))) return kFALSE;
    if (!( l->fill("hefr"     ,&hefr                                                            ))) return kFALSE;

    return kTRUE;
}

void HMdcDeDx2::sort(Int_t nHits)
{
    // Puts the measurement values into decreasing order.

    Int_t nhit=nHits;
    if(nHits>MAX_WIRES) nhit=MAX_WIRES;

    register Int_t a,b,c;
    Int_t exchange;
    Double_t val;

    for(a = 0; a < nhit - 1; ++ a)
    {
	exchange = 0;
	c        = a;
	val      = measurements[a];

	for(b = a + 1; b < nhit; ++ b)
	{
	    if(measurements[b] > val)   // : < increasing  :> decreasing
	    {
		c = b;
		val = measurements[b];
		exchange = 1;
	    }
	}
	if(exchange)
	{
	    measurements[c] = measurements[a];
	    measurements[a] = val;
	}
    }
}
Float_t HMdcDeDx2::calcDeDx(HMdcSeg* seg[2],
			    Float_t* meanold,Float_t* sigmaold,UChar_t* nwire,
			    Float_t* sigmanew,UChar_t* nwiretrunc,
			    Int_t vers,Int_t mod,
			    Bool_t useTruncMean,
			    Float_t truncMeanWindow,
			    Int_t inputselect)
{
    // calculates dedx of a MDC segment by doing normalization for
    // impact angle and distance from wire for the fired cells of
    // the segment. The measurements are sorted in decreasing order,
    // the mean and sigma is calculated. Afterwards the truncated mean
    // according to the set window (in sigma units) arround the mean
    // is calculated and returned as dedx. Mean,sigma,number of wires before
    // truncating,truncated mean,truncated mean sigma and number of wires
    // after truncating can be obtained by the returned pointers.
    // Different methods can be selected:
    // vers==0 : Input filling from inner HMdcSeg
    // vers==1 : Input filling from outer HMdcSeg
    // vers==2 : Input filling from inner+outer HMdcSeg (default)
    // mod==0  : calc dedx from 1st module in segment
    // mod==1  : calc dedx from 2nd module in segment
    // mod==2  : calc dedx from whole segment (default)
    // useTruncMean: kTRUE (default) apply truncated Mean
    // truncMeanWindow (unit: SIGMA RMS of mean TOT): -99 (default) use standard window
    // inputselect==0 : fill from segment (default, wires rejected by fit are missing)
    // inputselect==1 : fill from cluster
    // returns -99 if nothing was calculated

    //to be sure initialized values are returned
    *meanold   = -1.;
    *sigmaold  = -1.;
    *nwire     =  0;
    *sigmanew  = -1.;
    *nwiretrunc=  0;
    Float_t dedx       = -99.;
    Float_t mean       = -99.;
    Float_t sigma      = -99.;
    UChar_t nWire      =   0;
    UChar_t nWireTrunc =   0;

    if(!catcal ) return mean; // no cal1 data in input !

    if(!isInitialized) {
	Error("HMdcDeDx2::calcDeDx()","Container HMdcDeDx2 has not been initialized!");
	return mean;

    }
    if(vers >= 0 && vers <= 2) method = vers;

    if(seg[0] == 0 && (method == 0 || method == 2))
    {
	Error("HMdcDeDx2::calcDeDx()","ZERO POINTER FOR inner HMdcSeg RECEIVED!");
	return mean;
    }
    if(seg[1] == 0 && (method == 1))
    {
	return mean;
    }

    if(mod >= 0 && mod < 3)
    {
	module = mod;
    }
    else
    {
	Warning("calcDeDx()","Unknown module type! Will use both modules of Segment!");
        module = 2;
    }

    //---------------------------------------------------
    // getting input
    measurements.Reset(-99);

    nWire = fillingInput(seg,inputselect);
    if(debug) {
	cout<<"calcDeDx() from fillingInput(): "
	    <<"nw "      <<((Int_t)nWire)
	    <<" methode "<<method
            <<" module " <<module
	    <<endl;
    }
    if(nWire == 0) return mean;

    *nwire = nWire;

    if(nWire > 0) mean = TMath::Mean(nWire,measurements.GetArray(), NULL);
    *meanold = mean;
    //---------------------------------------------------
    // calculating sigma
    if(nWire >= 1)
    {
        sigma       = TMath::RMS(nWire,measurements.GetArray());
        *sigmaold   = sigma;
        //--------------------truncating measurements outside +- x*sigma
	if (useTruncMean) {
	    nWireTrunc  = select(mean,sigma,nWire,truncMeanWindow);
	} else {
	    sort(nWire);
            nWireTrunc=nWire;
	}
        *nwiretrunc = nWireTrunc;
        //--------------------calculating truncated mean
        dedx        = TMath::Mean(nWireTrunc,measurements.GetArray(), NULL);
        //--------------------calculating new sigma
        sigma       = TMath::RMS(nWireTrunc,measurements.GetArray());
        *sigmanew   = sigma;

    }else{
	Warning("calcDeDx()","nWire <=1 : skipped %i and %i with t2<=-998 !",
		ctskipmod1,ctskipmod1);
    }

    //---------------------------------------------------
    // now calibrate mean value of ToT to dEdX
    Int_t s = 0;
    if(seg[0]){
	s = seg[0]->getSec();
    }
    dedx = toTTodEdX(s,ref_MOD,-1,-1,ref_ANGLE,ref_DIST,dedx);
    if(dedx > 100000) {
	if(debug){cout<<"calcDeDx() : dEdX>1000 -->"<<dedx<<endl;}
	dedx = 100000.;
    }
    //---------------------------------------------------

    return dedx;
}
UChar_t HMdcDeDx2::fillingInput(HMdcSeg* seg[2],Int_t inputselect)
{
    // Fills array of measurements from HMdcSeg and returns the number of fired wires in Segment
    // The single measurements are normalized by impact angle and distance from wire. Only the first
    // wires < MAX_WIRES are accepted.
    // inputselect==0 : fill from segment (default,wires rejected by fit are missing)
    // inputselect==1 : fill from cluster

    ctskipmod0 = 0;
    ctskipmod1 = 0;

    Float_t t1,t2;
    Double_t alpha,mindist;
    Double_t x1,y1,z1,x2,y2,z2;

    UChar_t nWire = 0;

    if(!catcal ) return nWire; // no cal1 data in input !

    if(inputselect == 1 &&
       (!cathit || !catclusinf || !catclus)
      ) return nWire; // no cluster data in input !

    HMdcClus* clus[2] = {0};
    //----------------------------------------getting input--------------------------------------------------
    Int_t low = 0;
    Int_t up  = 2;

    if     (method == 0) {low = 0;up = 1;}   // inner seg
    else if(method == 1) {low = 1;up = 2;}   // outer seg
    else if(method == 2) {low = 0;up = 2;}   // both  seg

    for(Int_t io = low; io < up; io ++)
    {
        if(seg[io] == 0) continue;

        //---------------------------------------------------------------------------
	if(inputselect == 1)
	{
	    // we have to get the cluster for looping
	    // the drift cells
	    for(Int_t ihit = 0; ihit < 2; ihit ++)
	    {
		Int_t hitind = seg[io]->getHitInd(ihit);
		if(hitind != -1)
		{   // hit and clusinf have the same index!
		    HMdcClusInf* clusinf = (HMdcClusInf*) catclusinf->getObject(hitind);
		    if(clusinf)
		    {
			clus[io] = (HMdcClus*) catclus->getObject(clusinf->getClusIndex());
			if(clus[io] == 0){
			    Error("fillingInput()","Zero pointer for HMdcClus object retrieved!");
			    return nWire;
			}
		    }
		    else Error("fillingInput()","Zero pointer for HMdcClusInd object retrieved!");

		    break;  // found cluster -> exit the loop
		}

	    }
	}
        //---------------------------------------------------------------------------


	for(Int_t l = 0; l < 12; l ++)
	{
	    if(module == 0 && l > 5) continue;  // fill for 1st module only
	    if(module == 1 && l < 6) continue;  // fill for 2st module only

	    Int_t nCell = 0;
            // input selection
	    if(inputselect == 1){nCell = clus[io]->getNCells(l);}
	    else                {nCell = seg [io]->getNCells(l);}

	    loccal[0]   = seg[io]->getSec();
	    Int_t ioseg = seg[io]->getIOSeg();

	    for(Int_t i = 0; i < nCell; i ++)
	    {
		if(ioseg == 0){
		    (l < 6)? loccal[1] = 0 : loccal[1] = 1;
		}else if(ioseg == 1){
		    (l < 6)? loccal[1] = 2 : loccal[1] = 3;
		}
		(l < 6)? loccal[2] = l : loccal[2] = l - 6;

		// input selection
		if(inputselect == 1){loccal[3] = clus[io]->getCell(l,i);}
                else                {loccal[3] = seg [io]->getCell(l,i);}

		calcSegPoints(seg[io],x1,y1,z1,x2,y2,z2);
		(*sizescells)[loccal[0]][loccal[1]][loccal[2]].getImpact(x1,y1,z1,x2,y2,z2,loccal[3],alpha,mindist);

		HMdcCal1* cal1 = (HMdcCal1*)catcal->getObject(loccal);
		if(cal1 != 0)
		{
		    t1  = cal1->getTime1();
		    t2  = cal1->getTime2();

		    if(t2 != -998 && t2 != -999 &&
                       t1 != -998 && t1 != -999 &&
		       TMath::Finite(t1)        &&
                       TMath::Finite(t2)        &&

                       mindist<=parmindistcut[loccal[1]]
		      )
		    {   // some times t1 or t2 can be missing (-999,-998)
			// calculating mean value

			if(nWire < MAX_WIRES - 1)
			{
			    if(useCalibration)measurements[nWire] = normalize(loccal[0],loccal[1],loccal[2],loccal[3],alpha,mindist,t2-t1);
			    else              measurements[nWire] = t2-t1;
			    nWire ++;
			}else{
			    if(debug) Warning("fillingInput()","Number of wires in Segment=%i > MAX_WIRES = %i! Skipped!",nWire,MAX_WIRES);
			}
		    } else {
			if(loccal[1] == 0 || loccal[1] == 2) ctskipmod0 ++;
			if(loccal[1] == 1 || loccal[1] == 3) ctskipmod1 ++;}
		}else{
		    Warning("calcDeDx()","ZERO pointer recieved for cal1 object!");
		}
	    }
	}
    }
    return nWire;
}

UChar_t HMdcDeDx2::select(Float_t mean,Float_t sigma,UChar_t nWire,Float_t wind)
{
    // loops over the measurement array with nWire values and puts values to
    // -99 if the measurements are outside the wanted window arround the mean value mean
    // (set with setWindow() (sigma units)).
    // The procedure keeps at least a number of minimum wires set with setMinimumWires().

    UChar_t nWTrunc = nWire;
    UChar_t count   = 0;
    UChar_t minW    = (UChar_t)getMinimumWires();

    if(nWTrunc < minW)
    {
	if(debug){cout<<"select : skipp because nWT<minW "<<endl;}
	return nWTrunc;
    }

    //---------------------------------------------------
    // decide about the window for truncated Mean method
    Float_t tempWindow = getWindow();
    if(wind > 0) tempWindow = wind;
    //---------------------------------------------------

    //--------------------sorting measurements in decreasing order
    sort(nWire);
    if(debug){
	cout<<"---------------------------------------------------"<<endl;
	cout<<"measurements before truncation :"<<endl;
	for(Int_t i = 0; i < nWire; i ++){
	    cout<<i<<" "<<measurements[i]<<endl;
	}
    }

    Bool_t fail_high = kFALSE;
    Bool_t fail_low  = kFALSE;

    while(nWTrunc > minW &&
	  count < nWire )
    {
	//------------------------------------------------------------
	// starting from highest val
        if(!fail_high && fabs(measurements[count] - mean) > (tempWindow * sigma))
        { // if measurement outside the window
	    measurements[count] = -99;
	    nWTrunc --;
        } else  fail_high = kTRUE;
	//------------------------------------------------------------

	//------------------------------------------------------------
	// taking lowest val next
	if(nWTrunc > minW)
	{
	    if(!fail_low && fabs(measurements[nWire - 1 - count] - mean) > (tempWindow * sigma))
	    { // if measurement outside the window
		measurements[nWire - 1 - count] = -99;
		nWTrunc --;
	    }
	    else  fail_low = kTRUE;
	} else  fail_low = kTRUE;
	//------------------------------------------------------------
	count ++;
        if(fail_low && fail_high) break;
    }
    sort(nWire);

    if(debug){
	cout<<"---------------------------------------------------"<<endl;
	cout<<"measurements after truncation :"<<endl;
	for(Int_t i = 0; i < nWTrunc; i ++){
	    cout<<i<<" "<<measurements[i]<<endl;
	}
	cout<<"method "  <<method
	    <<" window " <<tempWindow
	    <<" mean "   <<mean
	    <<" meanN "  <<TMath::Mean(nWTrunc,measurements.GetArray())
	    <<" sig "    <<sigma
	    <<" sigN "   <<TMath::RMS(nWTrunc,measurements.GetArray())
	    <<" w "      <<((Int_t)nWire)
	    <<" trunc w "<<((Int_t)nWTrunc)
	    <<endl;
    }

    return  nWTrunc;

}
void HMdcDeDx2::setFuncPar(Int_t s,Int_t m,Int_t abin,Int_t dbin,Double_t* p,Int_t size)
{
    // set the values for the dedx functions by s, m, angle bin, dist bin. Needs pointer
    // to parameter array[N_PARAM]
    if(size == N_PARAM             &&
       abin >= 0 && abin < N_ANGLE &&
       dbin >= 0 && dbin < N_DIST  &&
       s >= 0 && s < 6             &&
       m >= 0 && m < 4){
	for(Int_t i = 0; i < size; i ++) par[s][m][abin][dbin][i] = p[i];
    } else Error("SetFuncPar(...)","array indices out of range!");
}
void HMdcDeDx2::setFuncPar(Double_t* p)
{
    // set all values for the dedx functions. Needs pointer
    // to parameter array[6][4][N_ANGLE][N_DIST][N_PARAM]
   for(Int_t s = 0; s < 6;s ++) {
	for(Int_t m = 0; m < 4; m ++) {
	    for(Int_t a = 0; a < N_ANGLE; a ++) {
		for(Int_t d = 0; d < N_DIST; d ++) {
		    for(Int_t i = 0; i < N_PARAM; i ++) {

			Int_t maxS = 4 * N_ANGLE * N_DIST * N_PARAM;
                        Int_t maxM = N_ANGLE * N_DIST * N_PARAM;
                        Int_t maxA = N_DIST * N_PARAM;

			par[s][m][a][d][i] = p[s * maxS + m * maxM + a * maxA + d * N_PARAM + i];
		    }
		}
	    }
	}
    }
}
void HMdcDeDx2::getFuncPar(Double_t* p)
{
    // set all values for the dedx functions. Needs pointer
    // to parameter array[6][4][N_ANGLE][N_DIST][N_PARAM]
   for(Int_t s = 0;s < 6; s ++) {
	for(Int_t m = 0; m < 4; m ++) {
	    for(Int_t a = 0; a < N_ANGLE; a ++) {
		for(Int_t d = 0; d < N_DIST; d ++) {
		    for(Int_t i = 0; i < N_PARAM; i ++) {

			Int_t maxS = 4 * N_ANGLE * N_DIST * N_PARAM;
                        Int_t maxM = N_ANGLE * N_DIST * N_PARAM;
                        Int_t maxA = N_DIST * N_PARAM;

			p[s * maxS + m * maxM + a * maxA + d * N_PARAM + i] = par[s][m][a][d][i];
		    }
		}
	    }
	}
    }
}
void HMdcDeDx2::setFuncMaxPar(Int_t s,Int_t m,Int_t abin,Int_t dbin,Double_t p)
{
    // set the values for the dedx functions by s, m, angle bin, dist bin.
    if(abin >= 0 && abin < N_ANGLE &&
       dbin >= 0 && dbin < N_DIST  &&
       s >= 0 && s < 6             &&
       m >= 0 && m < 4){
	parMax[s][m][abin][dbin] = p;
    } else Error("SetFuncPar(...)","array indices out of range!");
}
void HMdcDeDx2::setFuncMaxPar(Double_t* p)
{
    // set all values for the dedx functions. Needs pointer
    // to parameter array[6][4][N_ANGLE][N_DIST]
    for(Int_t s = 0; s < 6; s ++) {
	for(Int_t m = 0; m < 4; m ++) {
	    for(Int_t a = 0; a < N_ANGLE; a ++) {
		for(Int_t d = 0; d < N_DIST;d ++) {
		    Int_t maxS = 4 * N_ANGLE * N_DIST;
		    Int_t maxM = N_ANGLE * N_DIST;
		    Int_t maxA = N_DIST;

		    parMax[s][m][a][d] = p[s * maxS + m * maxM + a * maxA + d];
		}
	    }
	}
    }
}
void HMdcDeDx2::getFuncMaxPar(Double_t* p)
{
    // set all values for the dedx functions. Needs pointer
    // to parameter array[6][4][N_ANGLE][N_DIST]
   for(Int_t s = 0; s < 6; s ++) {
       for(Int_t m = 0; m < 4; m ++) {
	   for(Int_t a = 0; a < N_ANGLE; a ++) {
	       for(Int_t d = 0; d <N_DIST; d ++) {
		   Int_t maxS = 4 * N_ANGLE * N_DIST;
		   Int_t maxM = N_ANGLE * N_DIST;
		   Int_t maxA = N_DIST;

		   p[s * maxS + m * maxM + a * maxA + d] = parMax[s][m][a][d];
	       }
	   }
       }
   }
}

void HMdcDeDx2::setFuncWidthPar(Int_t s,Int_t m,Int_t abin,Int_t dbin,Double_t* p,Int_t size)
{
    // set the values fpr the dedx width functions by s, m, angle bin, dist bin. Needs pointer
    // to parameter array[N_SHIFT_PARAM]

    if(size == N_SHIFT_PARAM      &&
       abin >= 0 && abin < N_ANGLE&&
       dbin >= 0 && dbin < N_DIST &&
       s >= 0 && s < 6            &&
       m >= 0 && m < 4){
	for(Int_t i = 0; i < size; i ++) shiftpar[s][m][abin][dbin][i] = p[i];
    } else Error("SetFuncPar(...)","array indices out of range!");
}
void HMdcDeDx2::setFuncWidthPar(Double_t* p)
{
    // set all values for the dedx width functions. Needs pointer
    // to parameter array[6][4][N_ANGLE][N_DIST][N_SHIFT_PARAM]
    for(Int_t s = 0;s < 6; s ++) {
	for(Int_t m = 0; m < 4; m ++) {
	    for(Int_t a = 0; a < N_ANGLE; a ++) {
		for(Int_t d = 0; d < N_DIST; d ++) {
		    for(Int_t i = 0; i < N_SHIFT_PARAM; i ++) {

			Int_t maxS = 4 * N_ANGLE * N_DIST * N_SHIFT_PARAM;
                        Int_t maxM = N_ANGLE * N_DIST * N_SHIFT_PARAM;
                        Int_t maxA = N_DIST * N_SHIFT_PARAM;

			shiftpar[s][m][a][d][i] = p[s * maxS + m * maxM + a * maxA + d * N_SHIFT_PARAM + i];
		    }
		}
	    }
	}
    }
}
void HMdcDeDx2::getFuncWidthPar(Double_t* p)
{
    // set all values for the dedx width functions. Needs pointer
    // to parameter array[6][4][N_ANGLE][N_DIST][N_SHIFT_PARAM]
    for(Int_t s = 0; s < 6; s ++) {
	for(Int_t m = 0; m < 4; m ++) {
	    for(Int_t a = 0; a < N_ANGLE; a ++) {
		for(Int_t d = 0; d < N_DIST; d ++) {
		    for(Int_t i = 0; i < N_SHIFT_PARAM; i ++) {

			Int_t maxS = 4 * N_ANGLE * N_DIST * N_SHIFT_PARAM;
                        Int_t maxM = N_ANGLE * N_DIST * N_SHIFT_PARAM;
                        Int_t maxA = N_DIST * N_SHIFT_PARAM;

			p[s * maxS + m * maxM + a * maxA + d * N_SHIFT_PARAM + i] = shiftpar[s][m][a][d][i];
		    }
		}
	    }
	}
    }
}

void HMdcDeDx2::setFuncGainPar(Int_t s,Int_t m,Int_t l,Int_t c,Double_t p)
{
    // set the values for the gain corrections by s, m, l, c.
    if(c >= 0 && c < 220 &&
       l >= 0 && l < 6   &&
       s >= 0 && s < 6   &&
       m >= 0 && m < 4){
	pargaincorr[s][m][l][c] = p;
    } else Error("SetGainPar(...)","array indices out of range!");
}
void HMdcDeDx2::setFuncGainPar(Double_t* p)
{
    // set all values for the gain correction. Needs pointer
    // to parameter array[6][4][6][220]
    for(Int_t s = 0; s < 6; s ++) {
	for(Int_t m = 0; m < 4; m ++) {
	    for(Int_t l = 0; l < 6; l ++) {
		for(Int_t c = 0; c < 220;c ++) {
		    Int_t maxS = 4 * 6 * 220;
		    Int_t maxM = 6 * 220;
		    Int_t maxL = 220;

		    pargaincorr[s][m][l][c] = p[s * maxS + m * maxM + l * maxL + c];
		}
	    }
	}
    }
}
void HMdcDeDx2::getFuncGainPar(Double_t* p)
{
    // set all values for the gain correction. Needs pointer
    // to parameter array[6][4][6][220]
   for(Int_t s = 0; s < 6; s ++) {
       for(Int_t m = 0; m < 4; m ++) {
	   for(Int_t l = 0; l < 6; l ++) {
	       for(Int_t c = 0; c < 220;c ++) {
		   Int_t maxS = 4 * 6 * 220;
		   Int_t maxM = 6 * 220;
		   Int_t maxL = 220;

		   p[s * maxS + m * maxM + l * maxL + c] = pargaincorr[s][m][l][c];
	       }
	   }
       }
   }
}

void HMdcDeDx2::setMinDistCutPar(Int_t m,Double_t p)
{
    // set the values for the cut on minDist by m.
    if(m >= 0 && m < 4){
	parmindistcut[m] = p;
    } else Error("SetMinDistCutPar(...)","array indices out of range!");
}
void HMdcDeDx2::setMinDistCutPar(Double_t* p)
{
    // set all values for the minDistcut. Needs pointer
    // to parameter array[4]
    for(Int_t m = 0; m < 4; m ++) {

	parmindistcut[m] = p[m];
    }
}
void HMdcDeDx2::getMinDistCutPar(Double_t* p)
{
    // set all values for the minDistcut. Needs pointer
    // to parameter array[4]
    for(Int_t m = 0; m < 4; m ++) {

	p[m] = parmindistcut[m];
   }
}

Double_t HMdcDeDx2::calcLength(Int_t m,Double_t angle,Double_t dist)
{
    // calc max length of track in cell for given module
    // m (0-3), impact angle "angle" [Deg] and distance
    // from wire "dist" [mm]

    Double_t maxLength;
    Double_t yU,xU,yL,xL;
    Double_t alphaRad,tanAlpha,b;
    Double_t cellX[4];
    Double_t cellY[4];
     
    Double_t cX[4] = {2.5,3.0,6.0,7.0};
    Double_t cY[4] = {2.5,2.5,4.0,5.0};
    for(Int_t i=0;i<4;i++){
	cellX[i] = cX[i];
	cellY[i] = cY[i];
    }

    alphaRad = angle * TMath::DegToRad();// angle of Curve
    b        = dist / cos(alphaRad);
    tanAlpha = tan(alphaRad);
    if(angle > 0) {
	xU = (cellY[m] - b) / tanAlpha;
	if(xU <= cellX[m]) {
	    yU = cellY[m];
	} else {
	    yU = tanAlpha * cellX[m] + b;
	    xU = cellX[m];
	}

	xL = (-cellY[m] - b) / tanAlpha;
	if(xL >= -cellX[m]) {
	    yL = -cellY[m];
	}  else {
	    yL = tanAlpha * -cellX[m] + b;
	    xL = -cellX[m];
	}

	maxLength = sqrt( (xU - xL) * (xU - xL) + (yU - yL) * (yU - yL) );
    } else {
	yU = 0;
	xU = 0;
	yL = 0;
	xL = 0;
	maxLength = 2 * cellX[m];
    }

    return maxLength ;
}


void  HMdcDeDx2::calcSegPoints(HMdcSeg * seg,Double_t& x1, Double_t& y1, Double_t& z1, Double_t& x2, Double_t& y2, Double_t& z2)
{
    // calculates 2 coordinate points from segment

    Double_t teta = seg->getTheta();
    Double_t phi  = seg->getPhi();
    Double_t z    = seg->getZ();
    Double_t r    = seg->getR();
    Double_t pi2  = acos(-1.)/2.;

    Double_t X = r * cos(phi + pi2);
    Double_t Y = r * sin(phi + pi2);
    Double_t Z = z;

    x1 = X;
    y1 = Y;
    z1 = Z;
    x2 = X + cos(phi) * sin(teta);
    y2 = Y + sin(phi) * sin(teta);
    z2 = Z + cos(teta);

}

void  HMdcDeDx2::findBin(Int_t m,Double_t* angle,Double_t* mindist,Int_t* abin,Int_t* dbin)
{
    Double_t a =* angle;
    Double_t d =* mindist;

    if(d>MaxMdcMinDist[m]) d = MaxMdcMinDist[m];
    if(a > 90) a = 90.;
    if(a < 0)  a =  0.;

    (*abin) = (Int_t)((90. - a) / AngleStep);
    (*dbin) = (Int_t)(d / MinDistStep[m]);

    if((*abin) == 18)     (*abin) = 17;
    if((*dbin) == N_DIST) (*dbin) = N_DIST - 1;

}

Double_t HMdcDeDx2::toTSigma(Int_t s,Int_t m,Double_t angle,Double_t mindist,Int_t shift)
{
    // returns asymmetric width of ToT for single drift cell
    // shift == 0 (default) : returns 0
    //       ==-1           : low   bound width of ToT distribution (sigma)
    //       == 1           : upper bound width of ToT distribution (sigma)

    Int_t abin = 0;
    Int_t dbin = 0;

    findBin(m,&angle,&mindist,&abin,&dbin);

    // shift is half FWHM = 2.355/2=1.1775 sigma
    // we have to recalulate sigma here

    if      (shift < 0) return (shiftpar[s][m][abin][dbin][0] / 1.1775);
    else if (shift > 0) return (shiftpar[s][m][abin][dbin][1] / 1.1775);
    else return 0.;
}

Double_t HMdcDeDx2::toTTodEdX(Int_t s,Int_t m,Int_t l,Int_t c,Double_t angle,Double_t mindist,Double_t ToT)
{
    // calculation of ToT -> dEdX for single drift cell

    Int_t abin = 0;
    Int_t dbin = 0;

    findBin(m,&angle,&mindist,&abin,&dbin);

    Double_t* p= &par[s][m][abin][dbin][0];

    //-----------------------------------------------------
    Double_t ToTgaincorr = ToT;
    Double_t ToTcorr     = ToT;

    if(l > -1 && c > -1){

	ToTgaincorr = ToT + pargaincorr[s][m][l][c];
        ToTcorr     = ToTgaincorr/calcLength(m,angle,mindist);
    }

    if(ToTcorr > parMax[s][m][abin][dbin]) ToTcorr = parMax[s][m][abin][dbin];

    Double_t dEdX = TMath::Power(10.,(TMath::Power((ToTcorr - p[0]) / p[1],(1. / p[2])))) - p[3];
    if(!TMath::Finite(dEdX)){
	Warning("totTodEdX()","dEdX out of range: s %i m %i abin %i dbin %i ToT %1.15e dEdX %1.15e !",s,m,abin,dbin,ToTcorr,dEdX);
	dEdX = -1;
    }
    return dEdX;
}
Double_t HMdcDeDx2::dEdXToToT(Int_t s,Int_t m,Int_t l,Int_t c,Double_t angle,Double_t mindist,Double_t dEdX)
{
    // calculation of dEdX -> ToT for single drift cell

    Int_t abin = 0;
    Int_t dbin = 0;

    findBin(m,&angle,&mindist,&abin,&dbin);

    Double_t* p = &par[s][m][abin][dbin][0];

    Double_t ToT       = p[0] + p[1] * TMath::Power((TMath::Log10(dEdX +  p[3])),p[2]);
    Double_t ToTcorr   = ToT;

    if(l > -1 && c > -1){
        ToTcorr = ToT*calcLength(m,angle,mindist);
        ToTcorr = ToTcorr - pargaincorr[s][m][l][c];
    }

    if(!TMath::Finite(ToTcorr)){
	Warning("dEdXToToT()","ToT out of range: s %i m %i abin %i dbin %i ToT %1.15e dEdX %1.15e !",s,m,abin,dbin,ToTcorr,dEdX);
	ToTcorr = -1;
    }

    return ToTcorr;
}
Double_t HMdcDeDx2::normalize(Int_t s,Int_t m,Int_t l,Int_t c,Double_t angle,Double_t mindist,Double_t ToT)
{
    // calibrate ToT :
    // dEdX=TotTodEdX(t2-t1) ----> dEdXToToT(dEdX) for reference module,angle,distbin
    if(ToT < 0)  return ToT;
    Double_t     dEdX = toTTodEdX(s,m,l,c,angle,mindist,ToT);
    if(dEdX >= 0) ToT = dEdXToToT(s,ref_MOD,-1,-1,ref_ANGLE,ref_DIST,dEdX);

    return ToT;
}



//----------------------dEdx-----------------------------------------------
Double_t HMdcDeDx2::beta(Int_t id,Double_t p)
{
    Double_t mass   = HPhysicsConstants::mass(id);
    if(mass == 0) return -1;
    return  sqrt(1. / (((mass*mass)/(p*p)) + 1.));
}
TGraph* HMdcDeDx2::betaGraph(Int_t id,Int_t opt,Int_t markerstyle,Int_t markercolor,Float_t markersize)
{
    // creates a TGraph for particle with GEANT ID id and
    // momentum p
    // opt     = 1  : no exchange x-axis and y-axis
    //          -1  : take -p
    //           2  : exchange axis
    //          -2  : -p + exchange axis

    if(opt<-2||opt>2||opt==0){
	::Error("betaGraph()","Unknown Option opt=%i!. Use default opt=1",opt);
	opt=1;
    }

    Double_t pmin = 50;
    Double_t pmax = 100000.;
    Int_t    np   = 100;

    Double_t ptot = 0;
    Double_t xarray[np];
    Double_t yarray[np];


    for(Int_t i = 1; i <= np; i ++)
    {
      ptot = pmin * pow((pmax / pmin),(i - 1) / (Double_t)(np - 1.));
      xarray[i - 1] = (opt == -1 || opt == -2)? -ptot : ptot;
      yarray[i - 1] = HMdcDeDx2::beta(id,ptot);
    }
    Char_t name[300];
    sprintf(name,"beta_%s",HPhysicsConstants::pid(id));
    TGraph* g = 0;
    if(opt==1||opt==-1)g = new TGraph(np,xarray,yarray);
    else         g = new TGraph(np,yarray,xarray);
    g->SetName(name);
    g->SetMarkerStyle(markerstyle);
    g->SetMarkerSize (markersize);
    g->SetMarkerColor(markercolor);

    return g;
}

Double_t HMdcDeDx2::gamma(Int_t id,Double_t p)
{
    Double_t mass   = HPhysicsConstants::mass(id);
    if(mass == 0) return -1;
    Double_t beta2 = 1. / (((mass*mass)/(p*p)) + 1.);
    return  sqrt(1./ (1 - beta2));
}

Double_t HMdcDeDx2::energyLoss(Int_t id,Double_t p,Double_t hefr)
{
    // Calculates the dEdx (MeV 1/g cm2) of an particle with GEANT ID id
    // and momentum p (MeV) for He/i-butan gas mixture with He fraction hefr
    // (He (hefr) / i-butan (1-hefr)). The fomular is taken from PDG and doesn't
    // include the density correction term. The values for the mean excitation
    // energy are taken from Sauli.

    if(p == 0)                 return -1;
    if(hefr < 0. || hefr > 1.) return -1;
    Double_t mass   = HPhysicsConstants::mass(id);
    if(mass == 0) return -1;

    // keep function value inside boundary
    if      (mass <  150                 && p < 5 )   p = 5 ;
    else if (mass >= 150  && mass < 1000 && p < 20)   p = 20;
    else if (mass >= 1000 && mass < 2000 && p < 35)   p = 35;
    else if (mass >= 2000                && p < 55)   p = 55;

    // Z and A
    Double_t Z_gas  = 2. * hefr + (1. - hefr) * 34.;
    Double_t A_gas  = 4. * hefr + (1. - hefr) * 58.;
    // I_0 and I
    Double_t I_0_gas = 24.6 * hefr + (1. - hefr) * 10.8;
    Double_t I2      = pow(I_0_gas * Z_gas * (1.e-6),2); // sauli
    //Double_t I2     =pow(16. * pow(Z_gas,0.9),2); //C.Lippmann thesis


    Double_t K      = 0.307075; // MeV cm2 PDG, 4 * pi * N_{A} * r_{e}2 * m_{e}2 * c2
    Double_t mass2  = pow(mass,2);
    Double_t m_ec2  = HPhysicsConstants::mass(3);
    Double_t z2     = pow((Float_t)HPhysicsConstants::charge(id),2);
    Double_t p2     = pow(p,2);
    Double_t beta2  = 1. / ((mass2/p2) + 1.);
    Double_t gamma2 = 1./ (1 - beta2);
    Double_t gamma  = sqrt(gamma2);

    Double_t Tmax   = (2. * m_ec2 * beta2 * gamma2) / (1. + 2.* gamma * (m_ec2 / mass) + pow((m_ec2 / mass),2));
    Double_t term1  = K * z2 * (Z_gas / A_gas) * (1. / beta2);
    Double_t term2  = ((2. * m_ec2 * beta2 * gamma2 * Tmax) / I2);
    Double_t dedx   = term1 * (0.5 * log(term2) - beta2);

    return dedx;
}
TGraph* HMdcDeDx2::energyLossGraph(Int_t id,Double_t hefr,TString opt,Bool_t exchange,Int_t markerstyle,Int_t markercolor,Float_t markersize)
{
    // creates a TGraph for particle with GEANT ID id and
    // and He/i-butan gas mixture with He fraction hefr
    // (He (hefr) / i-butan (1-hefr) dEdx vs p,beta,1/beta2 or betagamma
    // depending on opt (p,beta,1/beta2,betagamma). exchange=kTRUE
    // will exchange x-axis and y-axis.


    Double_t pmin = 50;
    Double_t pmax = 100000.;
    Int_t    np   = 100;

    Double_t ptot = 0;
    Double_t xarray[np];
    Double_t yarray[np];

    Int_t    vers = 0;
    Double_t mass = HPhysicsConstants::mass(id);


    if(opt.CompareTo("p"             ) == 0) vers = 0;
    else if(opt.CompareTo("beta"     ) == 0) vers = 1;
    else if(opt.CompareTo("1/beta2"  ) == 0) vers = 2;
    else if(opt.CompareTo("betagamma") == 0) vers = 3;
    else {cout<<"HMdcDedx::calcDeDxGraph():unknow option!"<<endl;}

    for(Int_t i = 1; i <= np; i ++)
    {
      ptot = pmin * pow((pmax / pmin),(i - 1) / (Double_t)(np - 1.));
      if(vers == 0){xarray[i - 1] = ptot;}
      if(vers == 1){xarray[i - 1] = sqrt(1. / ((pow(mass,2) / pow(ptot,2)) + 1.));}
      if(vers == 2){xarray[i-1] = ((pow(mass,2) / pow(ptot,2)) + 1.);}
      if(vers == 3){xarray[i-1] = (ptot / mass);}
      yarray[i - 1] = HMdcDeDx2::energyLoss(id,ptot,hefr);
    }
    Char_t name[300];
    sprintf(name,"dedx_%s_He_%5.1f_ibutane_%5.1f",HPhysicsConstants::pid(id),hefr * 100,(1 - hefr) * 100);
    TGraph* g = 0;
    if(!exchange)g = new TGraph(np,xarray,yarray);
    else         g = new TGraph(np,yarray,xarray);
    g->SetName(name);
    g->SetMarkerStyle(markerstyle);
    g->SetMarkerSize (markersize);
    g->SetMarkerColor(markercolor);

    return g;
}
Double_t HMdcDeDx2::scaledTimeAboveThreshold(HGeantKine* kine,
					     Double_t p,
                                             Float_t t1, Float_t t2, Float_t* t2err,
					     Int_t s,Int_t m,Int_t l,Int_t c,
					     Double_t alpha,Double_t mindist)
{
    // calculated the t2 of drift cell measurent
    // from dedx of a given particle with momentum p.
    // The assumption is that all drift cell measurements
    // belonging to one track can be treated independent.
    // return t2 and smeared error to pointer t2err for an
    // single measurement. If the result t2-t1 would
    // be < 0 a random value for t2 0-20 ns larger than
    // t1(already including error) will be created.
    // The smeared error will be returned to the pointer t2err.


    //------------------------------------------------
    // check input
    if(!kine) {
        Error("caledTimeAboveThreshold()","retrieved ZERO pointer for kine object!");
	return -99;
    }
    if(p < 0) {
        Error("caledTimeAboveThreshold()","momentum = %5.3f is wrong!",p);
	return -99;
    }
    if(t1 < -997) {
        Error("caledTimeAboveThreshold()","t1 = %5.3f is wrong!",t1);
	return -99;
    }
    if(t2 < -997) {
        Error("caledTimeAboveThreshold()","t2 = %5.3f is wrong!",t1);
	return -99;
    }
    //------------------------------------------------

    //------------------------------------------------
    // get particle ID from kine object
    Int_t pTr = -1,pID = 0;
    kine->getParticle(pTr,pID);
    //------------------------------------------------


    //------------------------------------------------
    // get mass and charge of particle for dedx
    // calculation
    Double_t mass   = HPhysicsConstants::mass  (pID);
    Int_t    charge = HPhysicsConstants::charge(pID);


    if(charge == 0 || mass == 0)
    {
        Warning("HMdcDeDx2::scaledEnergyLoss()","id %i %s mass %7.5f charge %i p %7.5f skipped!"
                ,pID,HPhysicsConstants::pid(pID),mass,charge,p);
        return t2;
    }
    //------------------------------------------------

    //------------------------------------------------
    // calulate analytical dedx of particle

    Double_t dedx   = energyLoss(pID,p,getHeFraction());

    if(!TMath::Finite(dedx)){
	Error("scaledTimeAboveThreshold()","dedx either NaN or Inf!");
        return t2;
    }
    //------------------------------------------------

    //------------------------------------------------
    // recalculate dedx to mean value of Time over
    // threshold for the given drift chamber
    Double_t ToTmean = dEdXToToT(s,m,l,c,alpha,mindist,dedx);

    if(!TMath::Finite(ToTmean) || dedx < 0){
	Error("caledTimeAboveThreshold()","ToT either NaN or Inf!");
	cout<<"s "      <<s
            <<" m "     <<m
	    <<" angle " <<alpha
	    <<" dist "  <<mindist
	    <<" dedx "  <<dedx
	    <<" pid "   <<pID
	    <<" mass "  <<mass
	    <<" charge "<<charge
	    <<" mom "   <<p
	    <<" t1 "    <<t1
	    <<" t2 "    <<t2
	    <<endl;

	return t2;
    }
    //------------------------------------------------

    //------------------------------------------------
    // generate some realistic smearing arround
    // the ToT mean value for single measurement
    // The distribution of ToT is asymmetric.
    // the smearing has to be done for left and right half
    // different.

    Double_t length = calcLength(m,alpha,mindist);
    Double_t sigL   = toTSigma (s,m,alpha,mindist,-1)*length;
    Double_t sigR   = toTSigma (s,m,alpha,mindist, 1)*length;

    Double_t smear;
    if(gRandom->Rndm() > 0.5){
	smear =  TMath::Abs(gRandom->Gaus(0., sigR));
    } else{
	smear = -TMath::Abs(gRandom->Gaus(0., sigL));
    }
    if(t2err) * t2err = smear;
    //------------------------------------------------


    //------------------------------------------------
    // return t2 mean of an  single measurement. If the
    // result t2-t1 would  be < 0 a random value for
    // t2 0-20 ns larger than t1 will be created.
    t2 = ToTmean + t1;
    if(t2 <= t1){
	t2 = t1 + gRandom->Rndm() * 20.;
    }
    if(debug){
	cout<<"scaledEnergyLoss() "
	    <<" s "     <<s
            <<" m "     <<m
	    <<" a "     <<alpha
            <<" d "     <<mindist
	    <<" id "    <<pID
	    <<" mass "  <<mass
	    <<" charge "<<charge
	    <<" mom "   <<p
            <<" dedx "  <<dedx
	    <<" t1 "    <<t1
            <<" t2 "    <<t2
	    <<" t2err " <<smear
            <<" tot "   <<ToTmean
	    <<" sigL "  <<sigL
	    <<" sigR "  <<sigR
            <<endl;
    }

    return t2;
}

