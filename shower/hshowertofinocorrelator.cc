//*-- AUTHOR : Leszek Kidon
//*-- Modified: 31.03.2003 by J.Otwinowski
//    Energy losses in Tofino were added 
//*-- Modified : 19/04/05 by Jacek Otwinowski    
//*-- Modified : 22/05/06 by Jacek Otwinowski    
//	  m_bLowShowerEfficiency flag added
// 
//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
//HShowerTofinoCorrelator
//
// HShowerTofinoCorrelator reconstructor searches data describing
// the same pad in HShowerHit and HTofinoCal categories
// and joins them into one object. 
// Results are stored in HShowerHitTof category.
//
//////////////////////////////////////////////////////////////////////

#include "hshowertofinocorrelator.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "htofinodetector.h"
#include "hcategory.h"
#include "hmatrixcatiter.h"
#include "hlocation.h"
#include "hshowerhit.h"
#include "htofinocal.h"
#include "htofinocalsim.h"
#include "hshowerhittof.h"
#include "hshowerhittoftrack.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "showerdef.h"
#include "htofinocalpar.h"
#include "htofinodigitpar.h"
#include "htofinoshowermap.h"
#include "hlinearcategory.h"
#include "tofinodef.h"

#include "TMath.h"

#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(HShowerTofinoCorrelator)

HShowerTofinoCorrelator::HShowerTofinoCorrelator(const Text_t *name,const Text_t *title) :
  HReconstructor(name,title)
{
   fHitIter=NULL;
   fTofinoIter=NULL;
   fHitTofinoIter=NULL;
   m_pTofinoCalPar = NULL;
   m_pTofShowerMap = NULL;
   m_zeroLoc.set(0);
   m_bIsSimulation = kFALSE;
   m_bLowShowerEfficiency = kFALSE;
}

HShowerTofinoCorrelator::HShowerTofinoCorrelator()
{
   fHitIter=NULL;
   fTofinoIter=NULL;
   fHitTofinoIter=NULL;
   m_pTofinoCalPar = NULL;
   m_pTofShowerMap = NULL;
   m_zeroLoc.set(0);
   m_bIsSimulation = kFALSE;
   m_bLowShowerEfficiency = kFALSE;
}


HShowerTofinoCorrelator::~HShowerTofinoCorrelator(void) {
     if (fHitIter)       delete fHitIter;
     if (fTofinoIter)    delete fTofinoIter;
     if (fHitTofinoIter) delete fHitTofinoIter;
}

Bool_t HShowerTofinoCorrelator::init() {
  // creates the ShowerHit, ShowerTof and HTofinoCal categories 
  // and adds them to the current event,
  // creates an iterator which loops over all local maxima
  // and an iterator which loops over Tofino information.

	//init Tofino calibration parameters

    printf("initialization of Shower/Tofino correlator\n");
    
    HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                                  ->getDetector("Shower");
    if(!pShowerDet){
	Error("HShowerTofinoCorrelator::init()","No Shower Detector!");
	return kFALSE;
    }

    HTofinoDetector *pTofinoDet = (HTofinoDetector*)gHades->getSetup()
                                                  ->getDetector("Tofino");
    if(!pTofinoDet){
	Error("HShowerTofinoCorrelator::init()","No Tofino Detector!");
	return kFALSE;
    }

    HRuntimeDb* rtdb=gHades->getRuntimeDb();

    m_pTofinoCat=gHades->getCurrentEvent()->getCategory(catTofinoCal);
    if (!m_pTofinoCat) {
	Error("HShowerTofinoCorrelator::init()","No category catTofinoCal in input!");
        return kFALSE;
    }

    m_pHitCat=gHades->getCurrentEvent()->getCategory(catShowerHit);
    if (!m_pHitCat) {
	Error("HShowerTofinoCorrelator::init()","No category catShowerHit in input!");
        return kFALSE;
    }

    m_pHitTofCat=gHades->getCurrentEvent()->getCategory(catShowerHitTof);
    if (!m_pHitTofCat) {
      m_pHitTofCat = new HLinearCategory("HShowerHitTof", 1000);
      if (!m_pHitTofCat) return kFALSE;
      else gHades->getCurrentEvent()
                 ->addCategory(catShowerHitTof, m_pHitTofCat, "Tofino");
    }

    m_pTofinoCalPar  =NULL;
    m_pTofinoDigitPar=NULL; 


    // Simulation or event embedding the
    // category catGeantKine has to be present.
    // Use this to decide automatically if it is
    // simulation or not
    if(gHades->getCurrentEvent()->getCategory(catGeantKine))
    {
	m_bIsSimulation=kTRUE;
    }
    else {m_bIsSimulation=kFALSE;}

    if(!m_bIsSimulation && gHades->getEmbeddingMode()>0)
    {

	Error("HShowerTofinoCorrelator::init()",
	      "Real data with embedding Mode not allowed! Fix your settings!");
	return kFALSE;
    }

    if(m_bIsSimulation)
    {
        // Simulation or embedding

        // sim container
	m_pTofinoDigitPar = (HParCond*)rtdb->getContainer("TofinoDigitPar");
	if (!m_pTofinoDigitPar){
	    Error("HShowerTofinoCorrelator::init()","Zero pointer retrieved for TofinoDigitPar!");
	    return kFALSE;
	}
	if(gHades->getEmbeddingMode()>0)
	{
            // real container
	    m_pTofinoCalPar = rtdb->getContainer("TofinoCalPar");
	    if (!m_pTofinoCalPar){
		Error("HShowerTofinoCorrelator::init()","Zero pointer retrieved for TofinoCalPar!");
		return kFALSE;
	    }
	}

    } else {
	// real data
	m_pTofinoCalPar = rtdb->getContainer("TofinoCalPar");
	if (!m_pTofinoCalPar){
	    Error("HShowerTofinoCorrelator::init()","Zero pointer retrieved for TofinoCalPar!");
	    return kFALSE;
	}
    }

    m_pTofShowerMap = rtdb->getContainer("TofinoShowerMap");
    if (!m_pTofShowerMap) {
	Error("HShowerTofinoCorrelator::init()","Zero pointer retrieved for TofinoShowerMap!");
	return kFALSE;
    }

    fHitIter      =(HIterator*)m_pHitCat   ->MakeIterator();
    fTofinoIter   =(HIterator*)m_pTofinoCat->MakeIterator();
    fHitTofinoIter=(HIterator*)m_pHitTofCat->MakeIterator();

    return kTRUE;
}

Bool_t HShowerTofinoCorrelator::finalize(void) {
   return kTRUE;
}


HShowerHitTof* HShowerTofinoCorrelator::addHitTof(HShowerHit* pHit) {
// create one object, which contains data
// from HShowerHit and connect with it data from Tofino
  
    HShowerHitTof *pHitTof;

    pHitTof=(HShowerHitTof *)m_pHitTofCat->getNewSlot(m_zeroLoc);
    if (pHitTof!=NULL) {

	pHitTof=new(pHitTof) HShowerHitTofTrack;
	*pHitTof = *pHit;
	pHitTof->setTof(-200.);
	pHitTof->setADC(-200.);
	pHitTof->setDriftTime(-200.);
	pHitTof->setTofinoCell(9);
	pHitTof->setTofinoMult(0);
    }
  
    return pHitTof;
}

Int_t HShowerTofinoCorrelator::execute()
{
    HShowerHit    *pHit;
    HShowerHitTof *pHitTof;
    HTofinoCal    *pTCal;
    HTofinoCalSim *pTCalSim;
    HTofinoCal    *pTofCalTab[24]={NULL};
    Int_t nMult[24]           ={0}; //multiplicity in shower detector covered by one Tofino cell
    Int_t nRightMax           = 31; // max correlated Shower col with Tofino cell
    Int_t nLeftMin            = 0;  // min corelated Shower col with Tofino cell
    Int_t nSector, nRow, nTofCell;
    Int_t nTofIndex, iPed     = 0;
    Int_t nTofCellLeft, nTofCellRight, nNC;
    Float_t pEloss[4]         ={0};
    Float_t eloss             =0;
    Float_t distPMT           =0;

    HLocation ffLoc;
    ffLoc.set(2,-1,-1);

    Float_t fTime      = 0.0;
    Float_t fDriftTime = 0.0;
    Float_t fTof       = 0.0;
    Float_t fAdc       = 0.0;

    fHitIter   ->Reset();
    fTofinoIter->Reset();

    //----------------------------------------------------------------
    // make logic variable to
    // decide about the mode (real,sim,embedding)
    Int_t mode=-1; // 0:real,1:sim,2:embedding
    if     ( m_bIsSimulation  && gHades->getEmbeddingMode() >0) { mode=2;}
    else if( m_bIsSimulation  && gHades->getEmbeddingMode()==0) { mode=1;}
    else if(!m_bIsSimulation  && gHades->getEmbeddingMode()==0) { mode=0;}
    else   { Error("execute()","Unknown mode!"); exit(1);}
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    //mapping data to table indexed by sector,cell combination
    while((pTCal = (HTofinoCal*)fTofinoIter->Next())) {
	pTofCalTab[pTCal->getSector()*4+pTCal->getCell()] = pTCal;
    }
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    // loop over HShowerHit objects
    while((pHit = (HShowerHit*)fHitIter->Next()))
    {
	//----------------------------------------------------------------
	// high shower efficiency case
	if( isLowShowerEfficiency() == kFALSE && pHit->getModule() !=0 ) continue;
	//----------------------------------------------------------------

	if ( pHit->getModule()==0 || (pHit->getSum(0)==0 && pHit->getModule()==1) )
	{
	    //----------------------------------------------------------------
	    //looking for Tofino cell covering pad row, col
	    nTofCell = ((HTofinoShowerMap*)m_pTofShowerMap)->getCellNumber(pHit->getRow(), pHit->getCol());
	    if (nTofCell==9) continue; //empty entry in map
	    //----------------------------------------------------------------

	    nNC           = 1;
	    nTofCellRight = nTofCellLeft = nTofCell;
	    nSector       = pHit->getSector();


	    if (!pTofCalTab[nSector*4 + nTofCell]) {
		//try in cells on the right
		if(pHit->getCol() < nRightMax){
		    nTofCellRight = ((HTofinoShowerMap*)m_pTofShowerMap)->
			getCellNumber(pHit->getRow(), pHit->getCol()+1);
		}
		//try in cells on the left
		if(pHit->getCol() > nLeftMin){
		    nTofCellLeft = ((HTofinoShowerMap*)m_pTofShowerMap)->
			getCellNumber(pHit->getRow(), pHit->getCol()-1);
		}
	    }

	    if ((nTofCellRight != 9) && (nTofCellRight != nTofCell)) {
		if (pTofCalTab[nSector*4 + nTofCellRight]) {
		    nTofCell = nTofCellRight;
		    nNC = -1;
		}
	    }

	    if ((nTofCellLeft != 9) && (nTofCellLeft != nTofCell)) {
		if (pTofCalTab[nSector*4 + nTofCellLeft]) {
		    nTofCell = nTofCellLeft;
		    nNC = -1;
		}
	    }

	    nSector   = pHit->getSector();
	    nRow      = pHit->getRow();
	    nTofIndex = nSector*4 + nTofCell;
	    ffLoc[0]  = nSector;
	    ffLoc[1]  = nTofCell;

	    //----------------------------------------------------------------
	    // all calculations follows here. First we have to decide
	    // wether we should take care about embedding mode. For sim and real
	    // everything is clear.

	    Int_t useContainer     =mode; // make copy, decide later
	    Int_t nTofinoHitsSim   =-1;
	    Int_t track_sim_or_real= 0;  // flag : - (real), + (sim), 0 (no track or same number od real/sim)

	    if (pTofCalTab[nTofIndex])
	    {

		if( mode==2 )
		{
		    // embedding mode only
		    pTCalSim=(HTofinoCalSim*) pTofCalTab[nTofIndex];
		    nTofinoHitsSim=pTCalSim->getNHit();
		    for(Int_t i=0;i<nTofinoHitsSim;i++)
		    {
			if      (pTCalSim->getTrack(i)>0)                                   { track_sim_or_real++; }
			else if (pTCalSim->getTrack(i)==gHades->getEmbeddingRealTrackId() ) { track_sim_or_real--; }
			else                                                                { ; }
		    }

		    //----------------------------------------------------------------
		    // now we decide if we use real or sim parameters for
		    // energy loss calculation. In case of 1 hit in tofino paddle
		    // it works exact. If the there are more than 1 hit then
		    // we decide by majority sim/real tracks. In case of equal
		    // number of sim/real hits we prefer sim as default.
		    if     (track_sim_or_real>0) { useContainer=1; } // sim
		    else if(track_sim_or_real<0) { useContainer=0; } // real
		    else                         { useContainer=1; } // default is sim
		    //----------------------------------------------------------------

		}
		//----------------------------------------------------------------

		fDriftTime = calcDriftTime (useContainer,nSector,nTofCell,(Float_t)nRow);
		iPed       = getADCPedestal(useContainer,ffLoc);
		getElossParams(useContainer,ffLoc,pEloss);

		fTime   = pTofCalTab[nTofIndex]->getTime();
		fAdc    = pTofCalTab[nTofIndex]->getCharge();
		distPMT = calcDistance(useContainer,nSector,nTofCell,(Float_t)nRow);

		//----------------------------------------------------------------
		// calculate energyloss. Done different for
		// real and simulation data. For embedding one needs to
		// check wether the hit has been created by sim or real data.
		if( useContainer==1 )
		{
		    // sim
		    eloss = 1.8*fAdc*TMath::Exp(distPMT/2400.)/512.;
		    eloss -= 0.00015*distPMT;
		    eloss /= 1.8;
		}
		else
		{
		    // real
		    eloss = 2.0*(fAdc-iPed)/(pEloss[1]+pEloss[2]*nRow+pEloss[3]*nRow*nRow);
		    eloss /= 2.0;
		}
		//----------------------------------------------------------------

		//----------------------------------------------------------------
		// create new HShowerHitTof object. Done different for
		// real and simulation data. In case of embedding the
		// simulation object is created
		HShowerHitTof *pHitTof;
		pHitTof=(HShowerHitTof *)m_pHitTofCat->getNewSlot(m_zeroLoc);

		if (pHitTof!=NULL)
		{
		    pHitTof=new(pHitTof) HShowerHitTof;
		    *pHitTof = *pHit;

		    // low Shower efficiency case
		    if(isLowShowerEfficiency() == kTRUE && pHit->getModule() == 1){
			pHitTof->setModule(0);
		    }

		    pHitTof->setTof(fTime - fDriftTime);
		    pHitTof->setDriftTime(fDriftTime);
		    pHitTof->setADC(eloss);
		    pHitTof->setSector(pHit->getSector());
		    pHitTof->setTofinoCell(nTofCell);
		    nMult[nTofIndex]++;
		}
		//----------------------------------------------------------------
	    }
	} // end while
	//----------------------------------------------------------------
    }

    //----------------------------------------------------------------
    //updating multiplicity information
    fHitTofinoIter->Reset();
    while((pHitTof = (HShowerHitTof *)fHitTofinoIter->Next())) {
	fTof      = pHitTof->getTof();
	nTofIndex = pHitTof->getSector()*4 +  pHitTof->getTofinoCell();
	pHitTof->setTofinoMult(nMult[nTofIndex]);
    }
    //----------------------------------------------------------------

    return 0;
}
Float_t HShowerTofinoCorrelator::calcDriftTime(Int_t mode,Int_t nSector,Int_t nTofCell,Float_t nRow)
{
    // mode switches netween simulation/real containers
    // mode : 0 -> real
    //      : 1 -> sim

    if      ( mode==0 ) return ((HTofinoCalPar*  )m_pTofinoCalPar  )->calcDriftTime(nSector,nTofCell,nRow);
    else if ( mode==1 ) return ((HTofinoDigitPar*)m_pTofinoDigitPar)->calcDriftTime(nSector,nTofCell,nRow);
    else {Error("calcDriftTime()","Unknown mode=%i, should be 0 or 1", mode); return -999;}
}
Int_t HShowerTofinoCorrelator::getADCPedestal(Int_t mode,HLocation& loc1)
{
    // mode switches netween simulation/real containers
    // mode : 0 -> real
    //      : 1 -> sim

    if      ( mode==0 ) return (Int_t)((HTofinoCalPar*  )m_pTofinoCalPar  )->getADCPedestal(loc1);
    else if ( mode==1 ) return (Int_t)((HTofinoDigitPar*)m_pTofinoDigitPar)->getADCPedestal(loc1);
    else {Error("getADCPedestal()","Unknown mode=%i, should be 0 or 1", mode);return -999;}
}
void HShowerTofinoCorrelator::getElossParams(Int_t mode,HLocation& loc1,Float_t* pEloss)
{
    // mode switches netween simulation/real containers
    // mode : 0 -> real
    //      : 1 -> sim

    if      ( mode==0 ) {((HTofinoCalPar*  )m_pTofinoCalPar  )->getElossParams(loc1,pEloss); return;}
    else if ( mode==1 ) {((HTofinoDigitPar*)m_pTofinoDigitPar)->getElossParams(loc1,pEloss); return;}
    else {Error("getElossParams()","Unknown mode=%i, should be 0 or 1", mode); return ; }
}
Float_t HShowerTofinoCorrelator::calcDistance(Int_t mode,Int_t nSector,Int_t nTofCell,Float_t nRow)
{
    // mode switches netween simulation/real containers
    // mode : 0 -> real
    //      : 1 -> sim

    HLocation loc1;
    loc1.set(2,nSector,nTofCell);
    if      ( mode==0 ) return ((HTofinoCalPar*  )m_pTofinoCalPar  )->getObject(loc1)->calcDistance(nRow);
    else if ( mode==1 ) return ((HTofinoDigitPar*)m_pTofinoDigitPar)->calcDistance(nSector,nTofCell,nRow);
    else {Error("calcDistance()","Unknown mode=%i, should be 0 or 1", mode); return -999;}
}

