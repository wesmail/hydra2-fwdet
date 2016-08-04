//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////
//
//  HPionTrackerTrackF:
//
//  Track finder of the PionTracker
//
////////////////////////////////////////////////////////////////

#include "hpiontrackertrackf.h"
#include "hpiontrackerdef.h"
#include "hpiontrackerhit.h"
#include "hpiontrackertrack.h"
#include "hpiontrackerdetector.h"
#include "hpiontrackertrackfpar.h"
#include "hpiontrackermompar.h"
#include "hpiontrackermomrunpar.h"
#include "hpiontrackerbeampar.h"
#include "HBeam.h"

#include "hades.h"
#include "hcategory.h"
#include "hdebug.h"
#include "hevent.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hrun.h"
#include "hspectrometer.h"

#include <iomanip>
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";
#define PRh(x) std::cout << "++DEBUG: " << #x << " = hex |" << std::hex << x << std::dec << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

//typedef std::vector<int> test;

ClassImp (HPionTrackerTrackF)


    //#define USE_FILE 1 // uncomment for using file input
    //#define USE_FILE_SCAN 1 // uncomment for using file input



    //-------------------transport coefficent to the HADES target-------for angular variables-----------------/////////



    HPionTrackerTrackF::HPionTrackerTrackF (void)
{
    initVars();
    correction.clear();
    memset(corrs, 0, 2* sizeof(Double_t));
}

HPionTrackerTrackF::HPionTrackerTrackF (const Text_t * name, const Text_t * title, Bool_t skip)
: HReconstructor (name, title)
{
    // constructor
    // the parameter "skip" (default; kFALSE) defines, if the event is skipted in case no hit can be reconstructed
    initVars();
    correction.clear();
    memset(corrs, 0, 2* sizeof(Double_t));
}

HPionTrackerTrackF::~HPionTrackerTrackF (void)
{
    //destructor deletes the iterator on the raw category
    if (NULL != iter)
    {
	delete iter;
	iter = NULL;
    }
}

void HPionTrackerTrackF::initVars(void)
{
    pHitCat		   	= NULL;
    pTrackCat		= NULL;
    iter		   	= NULL;
    pTrackfpar		= NULL;
    pTrackBeampar	 	= NULL;
    idet1 = 0;
    idet2 = 1;

    id_det1 =16;  // element detector_1
    id_det2 =25;  // element detector_2
    id_outQ9=29;  // element out_Q9
    id_targ =32;  // element target

    // Setting transport matrix elements for det1 position
    // element ID 16  detector_1
    T12[idet1] = -0.03919;
    T14[idet1] = -0.00111;
    T16[idet1] = -0.81235;
    T32[idet1] = -0.01312;
    T33[idet1] = -18.281;
    T34[idet1] = -0.00298;
    T36[idet1] = 0.3955;
    T126[idet1] = 0.03095;
    T146[idet1] = -0.0003049;
    T166[idet1] = 0.005611;
    T336[idet1] = 0.216;
    T346[idet1] =  0.0263;
    T366[idet1] = -0.003661;

    // Setting transport matrix elements for det2 position
    // element ID 25  detector_2
    T12[idet2] = -0.47448;
    T14[idet2] =  0.00006;
    T16[idet2] = -0.03413;
    T32[idet2] = -0.06929;
    T33[idet2] = -67.86523;
    T34[idet2] = -0.08382;
    T36[idet2] =  1.42422;
    T126[idet2] = -0.0308;
    T146[idet2] = 0.0008834;
    T166[idet2] = -0.02265;
    T336[idet2] = 0.8774;
    T346[idet2] =  0.09678;
    T366[idet2] = -0.01531;


    //-------------------transport coefficent to the HADES target-------for angular variables-----------------/////////
    // element ID 29   out_Q9
    T12_t = -0.12828;
    T14_t = 0.00014;
    T16_t = 0.00700;
    T126_t = -0.04456;
    T146_t = 0.001482;
    T166_t = -0.007186;
    T32_t = -0.05304;
    T33_t = -49.73210;
    T34_t = -0.06448;
    T36_t = 0.99967 ;
    T326_t = -0.0009558;
    T336_t = 0.08271;
    T346_t = 0.07017;
    T366_t = 0.0005012;
    // element ID 32 target
    T21_t = 3.76702;
    T22_t = 0.57575;
    T23_t = -1.16249;
    T24_t = 0.00019;
    T26_t = 0.07783;
    T226_t = -0.04209;
    T246_t = 0.001718;
    T266_t = 0.02538;
    T41_t = 1.00162;
    T42_t = 0.26582;
    T43_t = 263.23;
    T44_t =  0.32118; // wrong from 25.4.2014 xls 0.32059;
    T46_t = -5.58106;
    T426_t = - 0.003304;
    T436_t = - 8.291; //- 8.383;   // xls changed 6.5.2014
    T446_t = - 0.3796; //- 0.3817; // xls changed 6.5.2014
    T466_t = 0.1623;

    nev = 0;

#ifdef USE_FILE
    inputpos.open("trackdetpos-g05x-g05y.txt");
#endif




}

Bool_t HPionTrackerTrackF::momrec (Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t momref, track & tr)
{
    // Input: 	x and y position at det1 and det2 (in cm)
    //			reference beam momentum (GeV/c)
    // Output: 	reconstructed momentum vector (GeV/c)
    // 		theta and phi angle at hades point (rad)

    // Solve equation in delta: a_0+a_1*delta+a_2*delta^2+a_3*delta^3
    // numerically starting from delta =3

    //--------MOMENTUM SELECTION PARAMETERS -----------------
    Int_t yy_weight = pTrackerMompar->getYYWeight();
    Float_t yy_slope = pTrackerMompar->getYYSlope();
    Float_t yy_offset = pTrackerMompar->getYYOffset();
    Int_t mom_weight = pTrackerMompar->getMomWeight();
    //cout << "mom_weight: " << mom_weight << endl;
    Float_t mom_maxt = corrs[0] - corrs[1];
    Float_t mom_offset = - corrs[1];
    //cout << "corr0: " << corrs[0] << endl;
    //-------------------------------------------------------

    tr.fX1 = x1;
    tr.fY1 = y1;
    tr.fX2 = x2;
    tr.fY2 = y2;

    Double_t a_3 = T126[idet2] * T166[idet1] - T126[idet1] * T166[idet2];
    Double_t a_2 = T12[idet2] * T166[idet1] - T12[idet1]*T166[idet2] +
	T16[idet1] * T126[idet2] - T16[idet2] * T126[idet1];
    Double_t a_1 = T12[idet2] * T16[idet1] - T12[idet1] * T16[idet2] -
	x1 * T126[idet2] + x2 * T126[idet1];
    Double_t a_0 = x2 * T12[idet1] - x1 * T12[idet2];

    Double_t sol = 0.;
    Double_t yyy1 = 0.;
    Double_t yyp1 = 0.;

    for (Int_t isol = 1; isol < 4; ++isol)
    {
	yyy1 = a_3 *sol * sol * sol + a_2 * sol *sol + a_1 * sol + a_0;
	yyp1 = 3.* a_3 * sol * sol + 2. * a_2 * sol + a_1;
	sol = sol - yyy1 / yyp1;
    }

    Double_t alfa = 0, beta = 0, gamma = 0, deter = 0, xdetc1 = 0, xdetc2 = 0,alfap = 0,betap = 0, gammap = 0, sol1 = 0,theta1 = 0, yci1 = 0, phi1 = 0, x_0 = 0;
    Double_t theta,/* yci,*/ phi;

    // calculate the theta value from the theta(delta) system

    theta = (x1-T16[idet1]*sol-T166[idet1]*sol*sol);
    theta = theta/(T12[idet1]+T126[idet1]*sol);

    // Solving the linear sytem :

    alfa = T33[idet1]+T336[idet1]*sol;
    beta =T34[idet1]+T346[idet1]*sol;
    gamma = y1-T32[idet1]*theta -T36[idet1]*sol-T366[idet1]*sol*sol;
    alfap = T33[idet2]+T336[idet2]*sol;
    betap = T34[idet2]+T346[idet2]*sol;
    gammap = y2 -T32[idet2] * theta - T36[idet2] * sol - T366[idet2] * sol * sol;
    deter = alfa*betap-alfap*beta;
    //yci = (gamma*betap-beta*gammap)/deter;
    phi = (alfa*gammap-gamma*alfap)/deter;

    xdetc1 = x1 - T14[idet1]*phi - T146[idet1]*phi*sol;
    xdetc2 = x2 - T14[idet2]*phi - T146[idet2]*phi*sol;

    // Solve again equation in delta: a_0+a_1*delta+a_2*delta^2+a_3*delta^3
    // using xdetc1 and xdetc2 and starting from last found delta value

    a_3 = T126[idet2] * T166[idet1] - T126[idet1] * T166[idet2];
    a_2 = T12[idet2] * T166[idet1] - T12[idet1]*T166[idet2] +
	T16[idet1] * T126[idet2] - T16[idet2] * T126[idet1];
    a_1 = T12[idet2] * T16[idet1] - T12[idet1] * T16[idet2] -
	xdetc1 * T126[idet2] + xdetc2 * T126[idet1];
    a_0 = xdetc2 * T12[idet1] - xdetc1 * T12[idet2];

    sol1=sol;

    for (Int_t isol = 1; isol < 4; ++isol)
    {
	yyy1 = a_3 * sol1 * sol1 * sol1 + a_2 * sol1 * sol1 + a_1 * sol1 + a_0;
	yyp1 = 3.* a_3 * sol1 * sol1 + 2. * a_2 * sol1 + a_1;
	sol1 = sol1-yyy1/yyp1;
	//	cout << " step " << isol << "delta = " << sol1 << endl;
	//std::cout<<"print 2"<<endl;
    }
    //cout << "parameters = " << a_3 << " " << a_2 << " " << a_1 << " " << a_0 << endl;
    //cout << "positions (cm) = " << x1 << " " << y1 << " " << x2 << " " << y2 << endl;

    // calculate the theta value from the theta(delta) system for new delta value

    theta1 = (xdetc1-T16[idet1]*sol1-T166[idet1]*sol1*sol1) / (T12[idet1] + T126[idet1]*sol1);

    // Solving again the linear system with the new delta value:

    alfa = T33[idet1]+T336[idet1]*sol1;
    beta = T34[idet1]+T346[idet1]*sol1;
    gamma = y1-T32[idet1]*theta1 -T36[idet1]*sol1-T366[idet1]*sol1*sol1;
    alfap = T33[idet2]+T336[idet2]*sol1;
    betap = T34[idet2]+T346[idet2]*sol1;
    gammap = y2-T32[idet2]*theta1 - T36[idet2]*sol1 - T366[idet2]*sol1*sol1;
    deter = alfa * betap -alfap * beta;
    yci1 = (gamma * betap - beta * gammap)/deter;
    phi1 = (alfa * gammap - gamma * alfap)/deter;

    // at the production target
    tr.fPhi0 = phi1/1000.;
    tr.fTheta0 = theta1/1000.;
    tr.fY0 = yci1;
    //cout << "y0" << tr.fY0 << "" << yci1 <<endl;
    // calculating the final theta and phi at hades position and converting them in rad (from mrad)

    Float_t Theta_hades = T21_t*x_0+T22_t*theta1+T23_t*yci1+T24_t*phi1+T26_t*sol1+T226_t*theta1*sol1+T246_t*phi1*sol1+T266_t*sol1*sol1;
    Float_t Phi_hades = T41_t*x_0+T42_t*theta1+T43_t*yci1+T44_t*phi1+T46_t*sol1+T426_t*theta1*sol1+T436_t*yci1*sol1+T446_t*phi1*sol1+T466_t*sol1*sol1;

    Float_t x_hades = T12_t*theta1+T14_t*phi1+T16_t*sol1+T126_t*theta1*sol1+T146_t*phi1*sol1+T166_t*sol1*sol1;
    Float_t y_hades = T32_t*theta1+T33_t*yci1+T34_t*phi1+T36_t*sol1+T336_t*yci1*sol1+T346_t*phi1*sol1+T366_t*sol1*sol1;

    // at the hades target
    tr.fTheta = Theta_hades/1000.;
    tr.fPhi = Phi_hades/1000.;
    tr.fXh = x_hades;
    tr.fYh = y_hades;

    // calculating the reconstructed momentum and its 3 components
    tr.fP = momref * (1. + sol1/100. )+mom_offset;
    /* 	cout << "mom: " << momref * (1. + sol1/100. ) << endl;
     cout << "mom_offset: " << mom_offset << endl;
     cout << "mom after corr: " << momref * (1. + sol1/100. )+mom_offset << endl;*/

    //--------------------------------------------
    // DIST FOR MOMENTUM SELECTION
    Float_t RecMom = momref * (1. + sol1/100. )+mom_offset;
    Float_t MaxMom =  mom_maxt;
    //cout << "max transmission: " << mom_maxt << endl;
    tr.fDist = sqrt(yy_weight*pow((-yy_slope * y1 + y2 - yy_offset)/sqrt(pow(yy_slope,2) + 1.0),2) + mom_weight * pow((MaxMom - RecMom),2));
    //-------------------------------------------


    //	cout << "momentum = " << tr.fP << " for a nominal of " << momref << "  (" << (momref-tr.fP)/tr.fP << ")" << endl;

    // this part is not needed, we get px, py, pz from p, theta, phi by TVector3
    // 	Float_t denom = sqrt(1. + pow( TMath::Tan(theta_h),2.) + pow( TMath::Tan(phi_h),2.) );
    // 	tr.fPx = ( tr.fP * TMath::Tan(theta_h))/denom;
    // 	tr.fPy = ( tr.fP * TMath::Tan(phi_h))/denom;
    // 	tr.fPz = ( tr.fP )/denom;

    return kTRUE;
}

Bool_t HPionTrackerTrackF::init (void)
{
    // gets the parameter containers
    // gets the PionTrackerCal category and creates the PionTrackerTrack category
    // creates an iterator which loops over all fired strips in PionTrackerCal

    HPionTrackerDetector * det = (HPionTrackerDetector *) gHades->getSetup()->getDetector ("PionTracker");

    if (!det)
    {
	Error ("init", "No PionTracker found.");
	return kFALSE;
    }

    pTrackfpar = (HPionTrackerTrackFPar *) gHades->getRuntimeDb()->getContainer ("PionTrackerTrackFPar");
    if (!pTrackfpar) return kFALSE;

    pTrackBeampar = (HPionTrackerBeamPar *) gHades->getRuntimeDb()->getContainer ("PionTrackerBeamPar");
    if (!pTrackBeampar) {
	Error ("init", "Could not retrieve HPionTrackerBeamPar!");
	return kFALSE;
    }

    pHitCat = gHades->getCurrentEvent()->getCategory (catPionTrackerHit);

    if (!pHitCat)
    {
	Error ("init()", "HPionTrackerhit category not available!");
	return kFALSE;
    }

    //----------------------------------------------
    // MOMENTUM SELECTION & CALIBRATION
    pTrackerMompar = (HPionTrackerMomPar *) gHades->getRuntimeDb()->getContainer ("PionTrackerMomPar");
    if (!pTrackerMompar)
    {
	Error ("init", "Could not retrieve HPionTrackerMomPar!");
	return kFALSE;
    }
    pTrackerMomRunpar = (HPionTrackerMomRunPar *) gHades->getRuntimeDb()->getContainer ("PionTrackerMomRunPar");
    if (!pTrackerMomRunpar)
    {
	Error ("init", "Could not retrieve HPionTrackerBeamRunPar!");
	return kFALSE;
    }
    //-----------------------------------------------

    iter = (HIterator *) pHitCat->MakeIterator();
    lochit.set(2, 0, 0);

    pTrackCat = det->buildCategory (catPionTrackerTrack);
    if (!pTrackCat) return kFALSE;

    loc.set(0);

    fActive = kTRUE;

    return kTRUE;
}
Bool_t HPionTrackerTrackF::reinit (void)
{
    if(pTrackBeampar) {

	HBeamElement* edet1 = pTrackBeampar->getBeamElement(id_det1);
	HBeamElement* edet2 = pTrackBeampar->getBeamElement(id_det2);
	HBeamElement* eoutQ9= pTrackBeampar->getBeamElement(id_outQ9);
	HBeamElement* etarg = pTrackBeampar->getBeamElement(id_targ);

	if(edet1&&edet2&&eoutQ9&&etarg){

	    //------------------------------------------------------------
	    // Setting transport matrix elements for det1 position
	    // element ID 16  detector_1
	    T12[idet1]  = edet1->Tij[0][1];
	    T14[idet1]  = edet1->Tij[0][3];
	    T16[idet1]  = edet1->Tij[0][4];

	    T32[idet1]  = edet1->Tij[2][1];
	    T33[idet1]  = edet1->Tij[2][2];
	    T34[idet1]  = edet1->Tij[2][3];
	    T36[idet1]  = edet1->Tij[2][4];

	    T126[idet1] = edet1->Tijk[0][1][4];
	    T146[idet1] = edet1->Tijk[0][3][4];
	    T166[idet1] = edet1->Tijk[0][4][4];

	    T336[idet1] = edet1->Tijk[2][2][4];
	    T346[idet1] = edet1->Tijk[2][3][4];
	    T366[idet1] = edet1->Tijk[2][4][4];

	    // Setting transport matrix elements for det2 position
	    // element ID 25  detector_2
	    T12[idet2]  = edet2->Tij[0][1];
	    T14[idet2]  = edet2->Tij[0][3];
	    T16[idet2]  = edet2->Tij[0][4];

	    T32[idet2]  = edet2->Tij[2][1];
	    T33[idet2]  = edet2->Tij[2][2];
	    T34[idet2]  = edet2->Tij[2][3];
	    T36[idet2]  = edet2->Tij[2][4];

	    T126[idet2] = edet2->Tijk[0][1][4];
	    T146[idet2] = edet2->Tijk[0][3][4];
	    T166[idet2] = edet2->Tijk[0][4][4];

	    T336[idet2] = edet2->Tijk[2][2][4];
	    T346[idet2] = edet2->Tijk[2][3][4];
	    T366[idet2] = edet2->Tijk[2][4][4];

	    //-------------------transport coefficent to the HADES target-------for angular variables-----------------/////////
	    // element ID 29   out_Q9
	    T12_t  = eoutQ9->Tij[0][1];
	    T14_t  = eoutQ9->Tij[0][3];
	    T16_t  = eoutQ9->Tij[0][4];
	    T32_t  = eoutQ9->Tij[2][1];
	    T33_t  = eoutQ9->Tij[2][2];
	    T34_t  = eoutQ9->Tij[2][3];
	    T36_t  = eoutQ9->Tij[2][4];
	    T126_t = eoutQ9->Tijk[0][1][4];
	    T146_t = eoutQ9->Tijk[0][3][4];
	    T166_t = eoutQ9->Tijk[0][4][4];
	    T326_t = eoutQ9->Tijk[2][1][4];
	    T336_t = eoutQ9->Tijk[2][2][4];
	    T346_t = eoutQ9->Tijk[2][3][4];
	    T366_t = eoutQ9->Tijk[2][4][4];
	    // element ID 32  target
	    T21_t  = etarg->Tij[1][0];
	    T22_t  = etarg->Tij[1][1];
	    T23_t  = etarg->Tij[1][2];
	    T24_t  = etarg->Tij[1][3];
	    T26_t  = etarg->Tij[1][4];
	    T41_t  = etarg->Tij[3][0];
	    T42_t  = etarg->Tij[3][1];
	    T43_t  = etarg->Tij[3][2];
	    T44_t  = etarg->Tij[3][3];
	    T46_t  = etarg->Tij[3][4];
	    T226_t = etarg->Tijk[1][1][4];
	    T246_t = etarg->Tijk[1][3][4];
	    T266_t = etarg->Tijk[1][4][4];
	    T426_t = etarg->Tijk[3][1][4];
	    T436_t = etarg->Tijk[3][2][4];
	    T446_t = etarg->Tijk[3][3][4];
	    T466_t = etarg->Tijk[3][4][4];


	} else {
	    Error ("reinit", "Could not retrieve beam elements from HPionTrackerBeamPar!");
	    return kFALSE;
	}

    } else return kFALSE;

    {//	return kTRUE;
	// ------------------------------------------------------------------------
	// MOMENTUM CORRECTION PER RUN
	//	{
	memset(corrs, 0, 2*sizeof(Double_t));

	if(!pTrackerMomRunpar)
	{
	    Error ("reinit", "Could not retrieve params from HPionTrackerMomRunPar!");
	    return kFALSE;
	}

	Int_t runID = gHades->getRuntimeDb()->getCurrentRun()->getRunId();
	correction.clear();
	pTrackerMomRunpar->getRun(runID, &correction);
	for (Int_t i =0; i < correction.getNVals(); ++i)
	{
	    corrs[i] = correction.getVal(i);
	}
    }

    return kTRUE;
    // ------------------------------------------------------------------------
}
Int_t HPionTrackerTrackF::execute (void)
{
    Float_t x1, x2, y1, y2, z1, z2; //, x_l,x_u,y_l,y_u;
    // make hits
    HPionTrackerHit *pHit0 = NULL, *pHit1 = NULL;

    //------------
    // MOMENTUM SELECTION
    std::vector<track> trackcan;
    trackcan.clear();
    //------------
    track tr;

    Int_t cnt0 = 0;
    while (true)	// plane 0, det1
    {

	Int_t cnt1 = 0;
	lochit[0] = 0;
	lochit[1] = cnt0++;

	pHit0 = (HPionTrackerHit*)pHitCat->getObject(lochit);
	//PR(pHit0);

	if (!pHit0)
	    break;

	while (true)	// plane 1, det2
	{
	    lochit[0] = 1;
	    lochit[1] = cnt1++;

	    pHit1 = (HPionTrackerHit*)pHitCat->getObject(lochit);
	    // 			PR(pHit1);
	    if (!pHit1)
		break;

	    // 			PR(cnt0);
	    //			PR(cnt1);

	    if (pTrackfpar->getTrackingFlag() == 0)	// no tracking
	    {
		tr.fP = pTrackfpar->getRefMom();
		tr.fTheta = 0.;
		tr.fPhi = 0.;
	    }
	    else
	    {
#ifdef USE_FILE
		nev = nev + 1;
		inputpos >> x1 >> y1 >> x2 >> y2 ;
#else
		pHit0->getLabPos(x1,y1,z1);
		pHit1->getLabPos(x2,y2,z2);
#endif


#ifdef USE_FILE_SCAN
		scanpos.open("poscan.txt");
		scanpos >> x_l >> x_u >> y_l >> y_u ;
		//				cout << x_l << " " <<  y_l << " " << x_u << " " << y_u <<endl ;
		//				cout << x1 << " " << y1 << " Present pos" << endl;
		scanpos.close();
		if(x1<x_l||x1>x_u||y1<y_l||y1>y_u) continue;

		//				cout << x1 << " " << y1 << " Portion ACCEPTED" << endl;
#endif


		Bool_t success = momrec(x1, y1, x2, y2, pTrackfpar->getRefMom() * 1e-3, tr);
#ifdef USE_FILE
		if (nev > 10000) success = false;
		cout << "Positions from file : " << endl;
		cout << x1 << " " << x2 << " " << nev << endl;
#endif
		if (!success) return 0;	// set condition for input txt file number of events
	    }

	    trackcan.push_back(tr);//	insertTrack(tr);
	}
    }

    if (trackcan.size())
    {
	// Sorting minimal distance (Momentum selection)
	std::sort(trackcan.begin(), trackcan.end(), HPionTrackerTrackF::sortfunction);

	Float_t eloss_corr = pTrackerMompar->getElossCorr();

	trackcan[0].fP = trackcan[0].fP + eloss_corr;

	for (UInt_t t = 0; t < trackcan.size(); ++t)
	{
	    insertTrack(trackcan[t]);
	}
    }

    //------------------------------------------------------------END-----------------------------------------------------------------------------------

    // insertTrack(vector);
    return 0;
}

void HPionTrackerTrackF::insertTrack(const track& tr)
{
    HPionTrackerTrack * pTrack = NULL;

    pTrack = static_cast<HPionTrackerTrack*>(pTrackCat->getNewSlot(loc));
    if (pTrack != NULL)
    {
	pTrack = new(pTrack) HPionTrackerTrack;
	pTrack->setPThetaPhi(tr.fP, tr.fTheta, tr.fPhi, tr.fMatch);
	pTrack->setPosAll(tr.fX1, tr.fY1, tr.fX2, tr.fY2, tr.fXh, tr.fYh);
	pTrack->setProdAngles(tr.fTheta0, tr.fPhi0);
	pTrack->setProdY(tr.fY0);
    }
}
