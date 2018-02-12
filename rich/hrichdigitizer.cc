//////////////////////////////////////////////////////////////////////////////
//
// $Id: $

//
//*-- Author  : Laura Fabbietti <Laura.Fabbietti@ph.tum.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichDigitizer
//
// For each digitized pad the corresponding track numbers of the
// direct hits and photons are stored in a linear category
// (catRichTrack<-HRichTrack) that is made sortable by the pad address.
// Each pad object of the HRichCalSim class has 2 members, nTrack1, nTrack2,
// that give the indexes of the first and the last track numbers in the linear
// category (catRichTrack) corresponding to a pad.
// This method allows to store as many particle track numbers as the pad
// " sees ".
//  here are listed the values of some useful variables
//  # means non costant value.
//  Each value refers to the 1 fired pad.
//
//  realID=gHades->getEmbeddingRealTrackId()
//
//                        track Number      Flag     energy
//  Cheren. Phot.            #               0        #
//  Feedback Phot.          -5               0        0
//  Direct Hits              #               1        0
//  Noise Hits              -1               0        0
//  REAL Hits (embedding)    realID          0        0
//---------------------------------------------------------
//
//
//  -----------------------                                -------------
//  |     HRichUnpacker    |                               | HGeantRich |
//  |   (embedding mode)   | \                             -------------
//  |                      |  \                                 ||
//  -----------------------    \                                || input to digitizer
//                              \                               \/
//                          -------------    read real    ------------------
//                         | HRichCalSim | ------------>  |                 |
//                          -------------  <-----------   |                 |
//                                                        | HRichDigitizer  |
//                          -------------                 |                 |
//                         | HRichTrack  | <-----------   |                 |
//                          -------------                 ------------------
//                                         write output
//
//  In the case of TRACK EMBEDDING of simulated tracks into
//  experimental data the real data are written by the HRichUnpacker into
//  HRichCalSim category. The real hits are taken into
//  account by the digitizer (adding of charges, adding track numbers to HRichTrack).
//  The embedding mode is recognized automatically by analyzing the
//  gHades->getEmbeddingMode() flag.
//            Mode ==0 means no embedding
//                 ==1 realistic embedding (first real or sim hit makes the game)
//                 ==2 keep GEANT tracks   (as 1, but GEANT track numbers will always
//                     win against real data. besides the tracknumber the output will
//                     be the same as in 1)
//
// Needed parameter containers:
//   - HRichCalPar (used for noise/threshold calculation)
//   - HRichGeometryPar
//   - HRichDigitisationPar
////////////////////////////////////////////////////////////////////////////
//
// SIMULATION OD DELTA ELECTRONS
//
//  void   setDeltaElectronUse(Bool_t use, Bool_t useDeltaMomSel=kFALSE, Int_t ionId=109,Float_t t1min=-950.,Float_t t1max=400.,Float_t momCut=20. Float_t prob)
//  void   setDeltaElectronMinMomCut(Float_t s0=0.,Float_t s1=0.,Float_t s2=0.,Float_t s3=0.,Float_t s4=0.,Float_t s5=0.)
//
//  Delta electrons can simulated by 3 different ways
//
// 1. Shooting electrons by kine generator ( primary electrons, momentun < momMaxDeltaElecCut) useDeltaMomSel=kTRUE
// 2. Shooting beam ions by kine generator of evt file input (primary ionID)
// 3. Shooting delta electrons by evt file (primary electron, generator info -3)               useDeltaMomSel=kFALSE
//
// The delta electrons source are identified in the input by the methodes above.
// To take care for the different material budgets an additional low momentum
// cut off per sector (default 0 MeV) can be applied. The accepted deltas can be adjusted by
// probability (default 2 : do not use prob). By default delta electrons in
// the input are suppressed (use=kFALSE).
//
////////////////////////////////////////////////////////////////////////////

#include "TF1.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector.h"

#include "hades.h"
#include "hcategory.h"
#include "hdebug.h"
#include "hevent.h"
#include "heventheader.h"
#include "hgeantrich.h"
#include "hlinearcatiter.h"
#include "hmatrixcatiter.h"
#include "hparset.h"
#include "hrichcalpar.h"
#include "hrichcalparcell.h"
#include "hrichcalsim.h"
#include "hrichdetector.h"
#include "hrichdigitisationpar.h"
#include "hrichdigitizer.h"
#include "hrichgeometrypar.h"
#include "hrichpad.h"
#include "hrichpadcorner.h"
#include "hrichpadfilter.h"
#include "hrichtrack.h"
#include "hgeantkine.h"
#include "hrichwiresignal.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "richdef.h"

#include <iomanip>
#include <iostream>
#include <stdlib.h>

using namespace std;

ClassImp(HRichDigitizer)
 HRichDigitizer* HRichDigitizer::fDigitizer = 0;
//----------------------------------------------------------------------------
HRichDigitizer::HRichDigitizer(const Text_t* name,
                               const Text_t* title,
                               Bool_t  kNoise,
                               Float_t slope,
                               Bool_t  oem)    : HReconstructor(name, title)
{

   setDefaults();

   isOEM             = oem;
   isActiveNoise     = kNoise;
   fSlopeCorrection  = slope;
   fDigitizer =this;
}
//============================================================================

//----------------------------------------------------------------------------
HRichDigitizer::HRichDigitizer()
{

   setDefaults();
   fDigitizer =this;

}
//============================================================================

//----------------------------------------------------------------------------
void
HRichDigitizer::setDefaults()
{
   catRichPhoton    = NULL;
   catRichDirect    = NULL;
   catTrack         = NULL;
   catCal           = NULL;

   iterRichPhoton   = NULL;
   iterRichDirect   = NULL;
   iterRichTrack    = NULL;
   iterRichCal      = NULL;

   pDigitisationPar = NULL;
   pGeometryPar     = NULL;
   pCalPar          = NULL;

   ga               = NULL;

   isOEM             = kFALSE;
   isActiveNoise     = kFALSE;

   countFBphot       = 0;
   countNoisePad     = 0;

   fWiresNr          = 0;
   fWiresDist        = 0.0;
   distWirePads      = 0.0;
   fYShift           = 0.0;

   binsQE            = 0;
   fChargePerChannel = 0.0;
   fChargeScaling    = 1.0;
   fElectronsNr      = 0.0;
   fFactor1          = 0.0;
   fFactor1Sig       = 0.0;
   fFactor2          = 0.0;
   fFactor2Sig       = 0.0;
   fIncreaseNoise    = 1.0;
   fParam1           = 0.0;
   fParam2           = 0.0;
   fQupper           = 0.0;
   fSigmaValue       = 0.0;
   fExpSlope         .Set(6);
   fExpSlope         .Reset(0.0);
   fSlopeCorrection  = 1.0;
   photlength        .Set(18);
   photlength        .Reset(0.0);
   photeffic         .Set(18);
   photeffic         .Reset(0.0);
   for (Int_t sec = 0; sec < 6; ++sec) {
      correction[sec].Set(18);
      correction[sec].Reset(0.0);
   }

   noiseProb         = 0.0;
   for (Int_t i = 0; i < 9; ++i)
      fDigitPadMatrix[i] = 0.0;


   for (Int_t i = 0; i < 9; ++i)
      fDigitPadMatrix[i] = 0.0;

   loc1 .set(1, 0);
   locat.set(3, 0, 0, 0);
   loc  .set(3, 0, 0, 0);

   setDeltaElectronUse(kFALSE,kFALSE,109,20.,2.);
   setDeltaElectronMinMomCut(0.,0.,0.,0.,0.,0.);


}
//============================================================================

//----------------------------------------------------------------------------
HRichDigitizer::~HRichDigitizer()
{

   if (iterRichPhoton) {
      delete iterRichPhoton;
      iterRichPhoton = NULL;
   }
   if (iterRichDirect) {
      delete iterRichDirect;
      iterRichDirect = NULL;
   }
   if (iterRichTrack) {
      delete iterRichTrack;
      iterRichTrack = NULL;
   }
   if (iterRichCal) {
      delete iterRichCal;
      iterRichCal = NULL;
   }

   fChargeOnWireList.Delete();
   fTrackNumberList.Delete();

   if (ga) {
      delete ga;
      ga = NULL;
   }

}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichDigitizer::init()
{

   HRichDetector* pRichDet = static_cast<HRichDetector*>(gHades->getSetup()
                                                         ->getDetector("Rich"));
   HRuntimeDb* rtdb = gHades->getRuntimeDb();


   if (kTRUE == isActiveNoise) {
      Info("init", "------------------------------------------------------");
      Info("init", "RICH digitizer NOISE MODE is ON");
      Info("init", "------------------------------------------------------");
   } else {
      Info("init", "------------------------------------------------------");
      Info("init", "RICH digitizer NOISE is OFF");
      Info("init", "------------------------------------------------------");
   }


   // Set pointer to RICH calibration parameters
   pCalPar = static_cast<HRichCalPar*>(rtdb->getContainer("RichCalPar"));
   if (NULL == pCalPar) {
      Error("init", "Initialization of calibration parameters failed, returning...");
      return kFALSE;
   }

   // Set pointer to RICH geometry parameters
   pGeometryPar = static_cast<HRichGeometryPar*>(rtdb->getContainer("RichGeometryParameters"));
   if (NULL == pGeometryPar) {
      pGeometryPar = new HRichGeometryPar;
      rtdb->addContainer(pGeometryPar);
   }
   if (NULL == pGeometryPar) {
      Error("init", "Initialization of geometry parameters failed, returning...");
      return kFALSE;
   }


   // Set pointer to RICH digitization parameters
   pDigitisationPar = static_cast<HRichDigitisationPar*>(rtdb->getContainer("RichDigitisationParameters"));
   if (NULL == pDigitisationPar) {
      pDigitisationPar = new HRichDigitisationPar;
      rtdb->addContainer(pDigitisationPar);
   }
   if (NULL == pDigitisationPar) {
      Error("init", "Initialization of digitization parameters failed, returning...");
      return kFALSE;
   }
   catKine = gHades->getCurrentEvent()->getCategory(catGeantKine);
   if (NULL == catKine) {
      Error("init", "Initializatin of kine category failed, returning...");
      return kFALSE;
   }

   // Initialize geant rich cherenkov photon category and set appropriate iterator
   catRichPhoton = gHades->getCurrentEvent()->getCategory(catRichGeantRaw);
   if (NULL == catRichPhoton) {
      Error("init", "Initializatin of Cherenkov photon category failed, returning...");
      return kFALSE;
   }
   iterRichPhoton = static_cast<HIterator*>(catRichPhoton->MakeIterator());
   if (NULL == iterRichPhoton) {
      Error("init", "Can not create catRichPhoton iterator, returning...");
      return kFALSE;
   }


   // Initialize geant category for direct hits and set appropriate iterator
   catRichDirect = gHades->getCurrentEvent()->getCategory(catRichGeantRaw + 1);
   if (NULL == catRichDirect) {
      Error("init", "Initialization of geant category for direct hits failed, returning...");
      return kFALSE;
   }
   iterRichDirect = static_cast<HIterator*>(catRichDirect->MakeIterator());
   if (NULL == iterRichDirect) {
      Error("init", "Can not create catRichDirect iterator, returning...");
      return kFALSE;
   }


   // Initialize output category for RICH HGeant tracks and its appropriate iterator
   catTrack = gHades->getCurrentEvent()->getCategory(catRichTrack);
   if (NULL == catTrack) {
      catTrack = pRichDet->buildCategory(catRichTrack);
      if (NULL == catTrack) {
         Error("init", "Can not build output category catRichTrack, returning...");
         return kFALSE;
      } else
         gHades->getCurrentEvent()->addCategory(catRichTrack, catTrack, "Rich");
   }
   iterRichTrack = static_cast<HIterator*>(catTrack->MakeIterator());
   if (NULL == iterRichTrack) {
      Error("init", "Can not create catRichTrack iterator, returning...");
      return kFALSE;
   }


   // Initialize output category catRichCalSim and its appropriate iterator
   catCal = gHades->getCurrentEvent()->getCategory(catRichCal);
   if (NULL == catCal) {
      catCal = pRichDet->buildMatrixCat("HRichCalSim", 1);
      if (NULL == catCal) {
         Error("init", "Can not build output category catRichCalSim, returning...");
         return kFALSE;
      } else gHades->getCurrentEvent()->addCategory(catRichCal, catCal, "Rich");
   }
   iterRichCal = static_cast<HIterator*>(catCal->MakeIterator());
   if (NULL == iterRichCal) {
      Error("init", "Can not create catRichCalSim iterator, returning...");
      return kFALSE;
   }

   ga = new TF1("ga", "[0]*exp([1]*(x-[2])*(x-[2]))", -10, 10);

   return kTRUE;

}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichDigitizer::reinit()
{

   if (NULL == pDigitisationPar || NULL == pGeometryPar) {
      Error("reinit", "One of the needed pointers is NULL, returning...");
      return kFALSE;
   }

   // Get geometry parameters
   fWiresNr     = pGeometryPar->getWiresPar()->getNrWires();
   fWiresDist   = pGeometryPar->getWiresPar()->getDistWire();
   distWirePads = pGeometryPar->getDistanceWiresPads();
   fYShift      = pGeometryPar->getSectorShift();
   fYShift      = fYShift / cos(20. * TMath::DegToRad());

   // Get digitization parameters
   fChargePerChannel = pDigitisationPar->getChargePerChannel();
   fChargeScaling    = pDigitisationPar->getChargeScaling();
   fElectronsNr      = pDigitisationPar->getElectronsNr();
   fFactor1          = pDigitisationPar->getFactor1();
   fFactor1Sig       = pDigitisationPar->getFactor1Sig();
   fFactor2          = pDigitisationPar->getFactor2();
   fFactor2Sig       = pDigitisationPar->getFactor2Sig();
   fIncreaseNoise    = pDigitisationPar->getIncreaseNoise();
   fParam1           = pDigitisationPar->getParameter1();
   fParam2           = pDigitisationPar->getParameter2();
   fQupper           = pDigitisationPar->getQupper();
   fSigmaValue       = pDigitisationPar->getSigmaValue();
   binsQE            = pDigitisationPar->getQEBinsNr();

   photlength.Set(binsQE, pDigitisationPar->getPhotonLenArray());
   photeffic .Set(binsQE, pDigitisationPar->getPhotonEffArray());
   fExpSlope .Set(6,      pDigitisationPar->getExpSlope());

   for (Int_t sec = 0; sec < 6; ++sec)
      correction[sec].Set(binsQE, pDigitisationPar->getCorrectionParams(sec));


   // this variable is the probability that a given pad will have
   // a signal induced by the electronic noise above threshold
   ga->SetRange(-10.0, 10.0);
   ga->SetParameters(1.0 / ((TMath::Sqrt(2. * TMath::Pi())) * fIncreaseNoise),
                     -1.0 / (2. * fIncreaseNoise * fIncreaseNoise));
   noiseProb = 0.5 -  ga->Integral(0.0, fSigmaValue);


   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichDigitizer::execute()
{
   // At the end of the execute() the TClonesArray that contains
   // the track numbers is sorted by the pad addreses. Then
   // every CalSim object is retrieved from the catRichCal
   // container to assign to each pad the corresponding nTrack1, nTrack2.
   // These 2 numbers are the indexes of the track nbs. in the TClonesArray
   // HRichTrack. Finally the noise is calculated.

   vector<Int_t> index;
   mapTracks.clear();

   HGeantRichPhoton* pCherenkovHit = NULL;
   HGeantRichDirect* pDirectHit    = NULL;
   HRichCalSim*      calsimobj     = NULL;
   HRichPadFilter    padFilter;


   Bool_t wasReal    = kFALSE;
   Int_t fCerNr      = 0;
   Int_t fDirNr      = 0;

   countNoisePad = 0;
   fChargeOnWireList.Delete();
   fTrackNumberList.Delete();


   if (NULL == pCalPar) {
      Error("execute", "Pointer to HRichCalPar is NULL, needed for THR, noise,... simulation!!!");
      return kSkipEvent;
   }


   //---------------------------------------------------------------------
   // In embedding mode the trackNumber of the
   // real data has to be set

   // remember the address of pads fired by real data
   // for later
   vector < Int_t > addr_Real(200, 0); // alocate for 200
   addr_Real.clear();                 // actual size = 0

   if (gHades->getEmbeddingMode() > 0 && gHades->getEmbeddingDebug()==1)  catCal->Clear();

   if (gHades->getEmbeddingMode() > 0) {

      HRichCalSim* cal = NULL;
      iterRichCal->Reset();
      while ((cal = static_cast<HRichCalSim*>(iterRichCal->Next()))) {
         addr_Real.push_back(cal->getAddress());  // remember which objects came from real data
         cal->setEventNr(gHades->getCurrentEvent()->getHeader()->getEventSeqNumber());
         cal->setEnergy(0); // realID
         Float_t dummy[2];
         dummy[0] = gHades->getEmbeddingRealTrackId();
         dummy[1] = 0; // flag: realID==0
         TVector rTrack(1, 2, dummy);
         loc[0] = cal->getSector();
         loc[1] = cal->getRow();
         loc[2] = cal->getCol();
         updateTrack(cal, loc, &rTrack);
      }
   }
   //---------------------------------------------------------------------

   //-------------------------------------------------------------------
   // digitization of GEANT data starts here

   // if one wants to have pure simulation in embedding mode, HRichCalSim
   // category has to be cleared (filled with real data in unpacker task)
   // catCal->Clear();

   //---------------------------------------------------------------------
   // prob selection for delta electrons
   if(useDeltaElectrons && probDeltaAccepted<1)
   {
       mDeltaTrackProb.clear();

       if(catKine){
	   Int_t nKine = catKine->getEntries();
	   for(Int_t i=0;i<nKine;i++){
	       HGeantKine* kine =  (HGeantKine*)catKine-> getObject(i);
	       Float_t mom = kine->getTotalMomentum() ;
	       Int_t generator = kine->getGeneratorInfo();
	       if(kine->getParentTrack() == 0 &&
		  (kine->getID() == ionID ||                                                    // beam ions simulated
		   ( useDeltaMomSelection && kine->getID() == 3 && mom < momMaxDeltaElecCut) || // any electron < momCut (shooting with kine generator)
		   (!useDeltaMomSelection && kine->getID() == 3 && generator ==-3 )             // delta electrons input file source id ==-3
		 )
		 )
	      {
		  mDeltaTrackProb[kine] = gRandom->Rndm();
	      }
	   }
       }
   }
   //---------------------------------------------------------------------

   iterRichPhoton->Reset();
   while ((pCherenkovHit = static_cast<HGeantRichPhoton*>(iterRichPhoton->Next()))) {

       //-------------------------------------------------------------------
       // work on delta electrons
       Int_t trkNum        = pCherenkovHit->getTrack();
       HGeantKine* primary = HGeantKine::getPrimary(trkNum,(HLinearCategory*)catKine);

       //---------------------------------------------------------------------
       // identify delta electrons
       Bool_t isDelta      = kFALSE;
       Float_t mom         = 0;
       Int_t   generator   = 0;
       if(primary) { // primary
	   mom       = primary->getTotalMomentum() ;
	   generator = primary->getGeneratorInfo();
	   if( primary->getID() == ionID ||                                                    // beam ions simulated
	      ( useDeltaMomSelection && primary->getID() == 3 && mom <momMaxDeltaElecCut)  ||  // any electron < momCut (shooting with kine generator)
	      (!useDeltaMomSelection && primary->getID() == 3 && generator ==-3 )              // delta electrons input file source id ==-3
	     ) isDelta = kTRUE;
       }
       //---------------------------------------------------------------------

       if(useDeltaElectrons)
       {
	   if(isDelta)
	   {
	       Int_t s = pCherenkovHit->getSector();
	       if(mom < momMinDeltaCut[s]) continue;  // skipp all delta lower than cutmom

	       //-------------------------------------------------------------------
	       if(probDeltaAccepted<1)
	       {
		   Float_t prob = 2;
		   itDelta = mDeltaTrackProb.find(primary);
		   if(itDelta != mDeltaTrackProb.end()) prob = itDelta->second;
		   else {
		       Error("execute()","No primary in delta map for trk = %i found! Should not happen!",trkNum);
		       primary->print();
		   }
		   if(prob < probDeltaAccepted ) continue;
	       }
	       //-------------------------------------------------------------------
	   }
       } else { // skipp all deltaelectrons (generatorinfo ==-3)
	   if(isDelta) continue;
       }
       //-------------------------------------------------------------------


      ++fCerNr;
      // for the oem measurement I have to increase the track number
      // of each produced photon by myself (fCerNr). This number
      // will be the track number od the photon.
      if (kTRUE == isOEM)
         digitiseCherenkovHits(pCherenkovHit, fCerNr);
      else digitiseCherenkovHits(pCherenkovHit, 0);
   }

   iterRichDirect->Reset();
   while ((pDirectHit = static_cast<HGeantRichDirect*>(iterRichDirect->Next()))) {

       //-------------------------------------------------------------------
       // work on delta electrons
       Int_t trkNum        = pDirectHit->getTrack();
       HGeantKine* primary = HGeantKine::getPrimary(trkNum,(HLinearCategory*)catKine);
       if(primary) { // primary
	   Float_t mom     = primary->getTotalMomentum() ;
	   Int_t generator = primary->getGeneratorInfo();
	   if( primary->getID() == ionID ||                                                    // beam ions simulated
	      ( useDeltaMomSelection && primary->getID() == 3 && mom <momMaxDeltaElecCut)  ||  // any electron < momCut (shooting with kine generator)
	      (!useDeltaMomSelection && primary->getID() == 3 && generator ==-3 )              // delta electrons input file source id ==-3
	     )
	   {
	       if(useDeltaElectrons)
	       {
		   Int_t s = pDirectHit->getSector();
		   if(mom < momMinDeltaCut[s]) continue;  // skipp all delta lower than cutmom

		   //-------------------------------------------------------------------
		   if(probDeltaAccepted<1)
		   {
		       Float_t prob = 2;
		       itDelta = mDeltaTrackProb.find(primary);
		       if(itDelta != mDeltaTrackProb.end()) prob = itDelta->second;
		       else {
			   Error("execute()","No primary in delta map for trk = %i found! Should not happen!",trkNum);
			   primary->print();
		       }
		       if(prob < probDeltaAccepted ) continue;
		   }
		   //-------------------------------------------------------------------

	       } else {
		   continue; // skipp all delta electrons and daughter hits
	       }
	   }
       }
       //-------------------------------------------------------------------

       digitiseDirectHits(pDirectHit);
      fDirNr++;
   }

   digitisePads();
   //-------------------------------------------------------------------



   //-------------------------------------------------------------------
   // generate noise and add the charge to the already fired pads
   iterRichCal->Reset();
   while ((calsimobj = static_cast<HRichCalSim*>(iterRichCal->Next()))) {
      //loop on cal sim objects. First the calsim container
      // contains only those pads fired by the photons.
      // if the electronic noise is switched a charge fluctuation
      // dues to the noise is added to the pad charge
      // and then all those pads that are below threshold
      // get a 0 charge

      wasReal = kFALSE;
      if (find(addr_Real.begin(), addr_Real.end(), calsimobj->getAddress()) != addr_Real.end()) {
         wasReal = kTRUE;
      }

      //do not add correlated noise to real data
      if (kTRUE == isActiveNoise && kFALSE == wasReal && NULL != pCalPar) {
         addNoiseToCharge(calsimobj);
      }

      if (NULL != pCalPar) {
         if (0 == checkPad(calsimobj) && kFALSE == wasReal) {
            // if charge is below threshold and
            // pad does not contain real data
            calsimobj->setCharge(0.);//LF
         }
      }

      // in the updateCharge member for each fired pad the energy of
      // the "hitting" photon is added. At the same time the number
      // of the photons hits is stored, so that after having digizited
      // all pads the average photon energy for each pad can be calculated
      // as follows.

      if (0 != calsimobj->getNHits())
         calsimobj->setEnergy(calsimobj->getEnergy() / calsimobj->getNHits());
      //-------------------------------------------------------------------
      // set index range

   } // end of loop on calsim object, pads fired by photons
   //-------------------------------------------------------------------


   //-------------------------------------------------------------------
   // after the charge propagation due tot he photons is completed
   // the electronic noise is produced. This part is skipped in embedding.
   if (kTRUE == isActiveNoise && gHades->getEmbeddingMode() == 0 && NULL != pCalPar) {
       makeNoiseOnPads();
   }
   //-------------------------------------------------------------------





   //-------------------------------------------------------------------
   // last action: sort track category and fill
   // the index range to the cal object

   catTrack->sort();   // sort track objects


   iterRichCal->Reset();
   while ((calsimobj = static_cast<HRichCalSim*>(iterRichCal->Next()))) {

       Int_t addPad = calsimobj->getAddress();
       //calsimobj->setNTrack1(0);
       map<Int_t, vector <HRichTrack*> >::iterator iter = mapTracks.find(addPad);  // pad addr -> list of HRichTrack
       if (iter != mapTracks.end()) { // found address
	   vector <HRichTrack*> & list = iter->second;
	   UInt_t n = list.size();
	   if (n > 1) {  // more than 1 track hit the pad
	       index.clear();
	       for (UInt_t i = 0; i < n; i ++) {
		   // store the indices in HRichTrack cat for each HRichTrack
		   index.push_back(catTrack->getIndex(list[i]));
	       }
	       std::sort(index.begin(), index.end()); // sort indices in ascending order
	       calsimobj->addTrackId(index[0]);
	       calsimobj->addTrackId(index[n - 1]);
	   } else { // only 1 track hit the pad
	       calsimobj->addTrackId(catTrack->getIndex(list[0]));   // both values are the same
	       calsimobj->addTrackId(catTrack->getIndex(list[0]));
	   }
       } else {
	   Error("execute()", "address of pad =%i not found in map!",addPad);
       }
   }
   //-------------------------------------------------------------------




   //-------------------------------------------------------------------
   // all the pads with charge 0 are removed from the calsim container
   catCal->filter(padFilter);
   //-------------------------------------------------------------------

   return 0;
}
//============================================================================

//----------------------------------------------------------------------------
Float_t
HRichDigitizer::GaussFun(const Float_t mean,
                         const Float_t sigma)
{

   Double_t x = 0.0;
   Double_t y = 0.0;
   Double_t z = 0.0;

   do {
      y = gRandom->Rndm();
   } while (!y);
   z = gRandom->Rndm();

   x = z * 2 * TMath::Pi();
   return mean + sigma * static_cast<Float_t>(sin(x) * sqrt(-2.0 * log(y)));
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichDigitizer::calcQE(const Float_t photlen,
                       const Int_t sec)
{

   Int_t   i     = 0;
   Float_t fDraw = 0.0;

   if (100 == photlen)
      return kTRUE;

   fDraw = gRandom->Rndm();

   for (i = 0; i < binsQE; ++i) {
      if (i == 0) {
         if (photlen < ((photlength[i] + photlength[i+1]) / 2)) {
            if (fSlopeCorrection * fDraw > photeffic[i] * correction[sec][i]) {
               return kFALSE;
            } else {
               return kTRUE;
            }
         }
      } else if (i == binsQE - 1) {
         if (photlen >= ((photlength[i-1] + photlength[i]) / 2)) {
            if (fSlopeCorrection * fDraw > photeffic[i] * correction[sec][i]) {
               return kFALSE;
            } else {
               return kTRUE;
            }
         }
      } else if (((photlength[i-1] + photlength[i]) / 2) <= photlen &&
                 ((photlength[i] + photlength[i+1]) / 2) > photlen) {
         if (fSlopeCorrection * fDraw > photeffic[i] * correction[sec][i]) {
            return kFALSE;
         } else {
            return kTRUE;
         }
      }
   }
   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichDigitizer::getWireNr(const Float_t xhit)
{

   Int_t   i       = 0;
   Int_t   nWireNr = -1;
   Float_t Xwir    = 0.0;

   for (i = 0; i < fWiresNr; ++i) {
      Xwir = pGeometryPar->getWiresPar()->getWire(i)->getWireX();
      if ((xhit >= Xwir - fWiresDist) && (xhit < Xwir + fWiresDist)) {
         nWireNr = i;
         break;
      }
   }

   return nWireNr;

}
//============================================================================

//----------------------------------------------------------------------------
Float_t
HRichDigitizer::calcChargeOnWire(const Int_t   sector,
                                 const Float_t xhit,
                                 const Float_t yhit,
                                 const Float_t nTrack,
                                 const Float_t nFlag,
                                 Float_t ene)
{
// this function calculates the charge on each wire and saves it in a TList
// moreover, it saves the track number and the corresponding flag
// (cf HRichTrack) in a second TList.
// The charge is calculated according to one exponential function
// obtained fitting the total charge distribution of the photon candidates
// belonging to class one ( OEM data).
// That means that the so calculated charge isn't in reality the
// original charge deposited on the wire but the total amount of the charge
// coupled to the pads. Therefore it isn't any longer necessary to
// use an additional coupling factor. ( 75%)


   Bool_t      endLoop      = kFALSE;
   Int_t       fHitedWireNr = 0;
   Float_t     fX           = 0.0;
   Float_t     fY           = 0.0;
   Float_t     fQ           = 0.0;
   Float_t     charge       = 0.0;
   HRichFrame* pFrame       = NULL;
   TVector*    gTrack1      = NULL;
   Float_t     dummy[2];

   if (NULL == (pFrame = pGeometryPar->getFramePar())) {
      Error("calcChargeOnWire", "Can not get RICH frame parameters");
      return -1;
   }

   if (-1 == (fHitedWireNr = getWireNr(xhit))) return -1;

   dummy[0] = nTrack;
   dummy[1] = nFlag;
   gTrack1  = new  TVector(1, 2, dummy);

   fX = pGeometryPar->getWiresPar()->getWire(fHitedWireNr)->getWireX();
   fY  = yhit;

   while (kFALSE == endLoop) {
      charge = 1.0 / fExpSlope[sector] * log(gRandom->Rndm()  * (exp(fExpSlope[sector] * fQupper) - 1.0) + 1.0);
      if (charge <= fQupper) endLoop = kTRUE;
   }

   fQ = charge * fChargePerChannel;

   if (ene == 100) {
      ene = 0.;
   }

   if (fQ > 0. && kFALSE == pFrame->isOut(fX, fY)) {
      fChargeOnWireList.Add(new HRichWireSignal(sector, fHitedWireNr,
                                                fQ, fX, fY, ene));
      fTrackNumberList.Add(gTrack1);
   } else {
      delete gTrack1;
   }

   return fQ;

}
//============================================================================

//----------------------------------------------------------------------------
void
HRichDigitizer::digitiseCherenkovHits(HGeantRichPhoton *pCerHit,
                                      const Int_t count)
{
// for every photon hit on a pad the resulting charge on a wire
// is calculated and the track number of the photon parent is stored.
// (cf. calcChargeOnWire).

   Float_t xHitcm = (pCerHit->getX() / 10.0);           //HGeant output is in mm!!!
   Float_t yHitcm = (pCerHit->getY() / 10.0 + fYShift); //HGeant output is in mm!!!

   countFBphot = 0;

   if (0 == count)
      processPhoton(pCerHit->getEnergy(), xHitcm, yHitcm,
                    pCerHit->getTrack(), pCerHit->getSector());
   else processPhoton(pCerHit->getEnergy(), xHitcm, yHitcm,
                         count, pCerHit->getSector());

}
//============================================================================

//----------------------------------------------------------------------------
void
HRichDigitizer::processPhoton(const Float_t ene,
                              const Float_t xPos,
                              const Float_t yPos,
                              const Int_t   track,
                              const Int_t   sector)
{

   Float_t eneFeedBack  = 0.0;
   Float_t xHitcmFB     = 0.0;
   Float_t yHitcmFB     = 0.0;
   Float_t chargeOnWire = 0.0;
   HRichFrame* pFrame   = NULL;

   if (NULL == (pFrame = pGeometryPar->getFramePar())) {
      Error("processPhoton", "Can not get RICH frame parameters");
      return;
   }

   if (kTRUE  == calcQE(ene, sector) &&
       kFALSE == pFrame->isOut(xPos, yPos)) {
      chargeOnWire = calcChargeOnWire(sector, xPos, yPos, track, 0, ene);
      if (calcFeedBack(sector, xPos, yPos, eneFeedBack,
                       xHitcmFB, yHitcmFB, chargeOnWire) && countFBphot < 7) {
         // -5 is the tracknumber assigned to all the feedback photons.
         processPhoton(eneFeedBack, xHitcmFB, yHitcmFB, -5, sector);
      }
   }
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichDigitizer::calcFeedBack(const Int_t   sec,
                             const Float_t xhit,
                             const Float_t yhit,
                             Float_t &ene,
                             Float_t &xhittFB,
                             Float_t &yhittFB,
                             const Float_t charge)
{
// We assume that the feed back photon is produced on the anodic wire,
// the number of the feed back photon is proportional to the value A0.

   countFBphot++;

   Float_t thetaDir = 0.0;
   Float_t phiDir   = 0.0;
   Float_t r        = 0.0;
   Float_t fDraw1   = 0.0;
   Float_t fDraw2   = 0.0;

   if (gRandom->Rndm() < (charge * 7.8e-6)) {
      fDraw1   = gRandom->Rndm();
      fDraw2   = gRandom->Rndm();

      thetaDir = fDraw1 * 90;
      phiDir   = fDraw1 * 360;
      r        = distWirePads * TMath::Tan(thetaDir * TMath::DegToRad());
      xhittFB  = xhit + r * TMath::Cos(phiDir * TMath::DegToRad());
      yhittFB  = yhit + r * TMath::Sin(phiDir * TMath::DegToRad());

      // the Alice TRD gives as VUV photon energies the following
      // values : 156nm,166nm,193nm with weights 30%,57%,13%
      // they correspond to :7.94 eV,7.46 eV,6.42eV.

      if (fDraw2 < 0.13) ene = 6.42;
      else if (fDraw2 < 0.43) ene = 7.94;
      else                      ene = 7.46;

      return kTRUE;
   } else {
      ene     = 0;
      xhittFB = 0;
      yhittFB = 0;
      return kFALSE;

   }
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichDigitizer::digitiseDirectHits(HGeantRichDirect *pDirHit)
{
// for every direct hit (charge particle hitting the RICH or ionizing
// the gas near the surface of the photon detector)  the
// resulting charge on the wires is calculated and the track number
// of the charged particle hitting the pad is stored.
// (cf. calcChargeOnWire)

   Int_t       i           = 0;
   Int_t       fStepsNr    = 0;
   Int_t       dTrack      = -1;
   Int_t       dId         = -1;
   Float_t     fStepLength = 0.0;
   Float_t     fNewX       = 0.0;
   Float_t     fNewY       = 0.0;
   HRichFrame* pFrame      = NULL;

   Float_t     xHitcm      = pDirHit->getX() / 10.0F;             //HGeant output is in mm!!!
   Float_t     yHitcm      = pDirHit->getY() / 10.0F + fYShift ;  //HGeant output is in mm!!!

   if (NULL == (pFrame = pGeometryPar->getFramePar())) {
      Error("digitiseDirectHits", "Can not get RICH frame parameters");
      return;
   }

   if (pDirHit->getEnergyLoss() > 0.0F &&
       // one day it will be additional condition if it is mirror or detector
       kFALSE == pFrame->isOut(xHitcm, yHitcm)) {
      // Number of electrons is given in keV while the energy loss is given
      // in MeV, therefore a factor 1000 is needed to calculate the correct
      // number of steps

      fStepsNr = static_cast<Int_t>(1 + fElectronsNr * 1000 * pDirHit->getEnergyLoss());
      if (fStepsNr > 5000) {
#ifdef HRICH_DEBUGMODE
         Warning("digitiseDirectHits",
                 "Evt %d: number of maximum steps (5000) exceeded by %d!",
                 gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(), fStepsNr);
#endif
         fStepsNr = 5000;
      }

      // units: trackLength is in mm and coordinates in cm!
      fStepLength = 0.1 * pDirHit->getTrackLengthInPhotDet() / static_cast<Float_t>(fStepsNr);

      for (i = 0; i < fStepsNr; ++i) {
         fNewX = xHitcm + (i + 1) * fStepLength *
                 sin(TMath::DegToRad() * pDirHit->getTheta()) * cos(TMath::DegToRad() * pDirHit->getPhi());
         fNewY = yHitcm + (i + 1) * fStepLength *
                 sin(TMath::DegToRad() * pDirHit->getTheta()) * sin(TMath::DegToRad() * pDirHit->getPhi());
         pDirHit->getTrack(dTrack, dId);
         // the energy of the direct hits is set to 0
         // not to modify the average energy deposited by one or more
         // photons on one pad. (this is very important for the OEM
         // analysis).
         calcChargeOnWire(pDirHit->getSector(), fNewX, fNewY, dTrack, 1, 0);
      }
   }
}
//============================================================================

//----------------------------------------------------------------------------
HRichPad*
HRichDigitizer::translateCorners(HRichPad *pPad,
                                 const Float_t dx,
                                 const Float_t dy)
{
// check what you want to translate - probably real corners

   Int_t     i;
   HRichPad* fpTranslatedPad = new HRichPad(*pPad);

   if (NULL == fpTranslatedPad) {
      Error("translateCorners", "Can not get pointer to HRichPad");
      return NULL;
   }

   for (i = 0; i < fpTranslatedPad->getCornersNr(); ++i) {
      fpTranslatedPad->getCorner(i)->setX(fpTranslatedPad->getCorner(i)->getX() - dx);
      fpTranslatedPad->getCorner(i)->setY(fpTranslatedPad->getCorner(i)->getY() - dy);
   }
   fpTranslatedPad->setXmin(fpTranslatedPad->getXmin() - dx);
   fpTranslatedPad->setYmin(fpTranslatedPad->getYmin() - dy);
   fpTranslatedPad->setXmax(fpTranslatedPad->getXmax() - dx);
   fpTranslatedPad->setYmax(fpTranslatedPad->getYmax() - dy);

   // the ownership of this pointer is passed to outer variable-pointer
   return fpTranslatedPad;
}
//===========================================================================

//----------------------------------------------------------------------------
void
HRichDigitizer::makeNoiseOnPads()
{
// nbNoisePads is the number of pads on which the electronic noise
// will produce a non zero signal. This number is extimated taken
// into account the threshold taht has been used for the pedestals.

   HRichCalSim*     pCalSim   = NULL;
   HRichCalParCell* pCell     = NULL;
   const Int_t    maxCols     = pGeometryPar->getColumns();
   const Int_t    maxPads     = pGeometryPar->getPadsNr();
   Int_t          nbNoisePads = 0;
   Int_t          padNr       = 0;
   Int_t          iCol        = 0;
   Int_t          iRow        = 0;
   Float_t        fCharge     = 0.0;
   Float_t        fSigmaPad   = 0.0;
   Float_t        fMeanPad    = 0.0;
   Float_t        fFloatMean  = 0.0;

   // here the number of pads that will have a noise induced signal above
   // threshold is calculated. The number countNoisePad/6 represents
   // the number of pads that had already a noise signal above threshold
   // in each sector.

   nbNoisePads = static_cast<Int_t>(TMath::Floor(4800 * noiseProb));
   nbNoisePads = gRandom->Poisson(nbNoisePads) - static_cast<Int_t>(countNoisePad / 6.);

   if (nbNoisePads < 0) {
      nbNoisePads = 0;
   }

   // a Gauss distributed signal is induced on nbNoisePads pads
   // for each sector.
   for (Int_t sec = 0; sec < 6; ++sec) {
      if (pGeometryPar->getSectorActive(sec) > 0) {
         for (Int_t j = 0; j < nbNoisePads; ++j) {
            do {
               padNr     = static_cast<Int_t>(TMath::Floor(maxPads * gRandom->Rndm()));
               iRow      = pGeometryPar->calcRow(padNr);
               iCol      = pGeometryPar->calcCol(padNr);

               // the following condition is necessary to test that the pad
               // exists and is connected to the electronic

               if (padNr >= maxPads) {
                  Error("makeNoiseOnPads", "Index %d out of bounds...", maxPads);
                  continue;
               }

            } while (!pGeometryPar->getPadsPar()->getPad(padNr)->getPadActive() && iCol < maxCols);

            loc[0] = sec;
            loc[1] = iRow;
            loc[2] = iCol;

            pCell = static_cast<HRichCalParCell*>(pCalPar->getObject(loc));
            if (NULL != pCell) {
               fSigmaPad = pCell->getSigma();
            } else {
               Error("makeNoiseOnPads", "Did not get any valid HRichCalParCell...");
               fSigmaPad = 0.0;
            }

            if (fSigmaPad > 0.4) {
               fMeanPad   = pCell->getOffset();
               fFloatMean = fMeanPad - (TMath::Floor(fMeanPad));
               fCharge    = calcNoiseOnPad(fSigmaPad, fFloatMean);

               // the noise contribution is calculated with a gauss dist.
               // centred in 0, hence we have to add the float part of the
               // mean. Hence we cast the charge to an integer, then we
               // subtract the float part of the mean value and finally
               // we add a random number between 0 and 1 to have a float.

               pCalSim = static_cast<HRichCalSim*>(catCal->getObject(loc));
               // if the pad has already been fired no additional
               // charge is added and a new pad is drawn.

               if (NULL == pCalSim && fCharge > 0) {
                  pCalSim = static_cast<HRichCalSim*>(catCal->getSlot(loc));
                  if (NULL != pCalSim) {
                     pCalSim = new(pCalSim) HRichCalSim;
                     pCalSim->setEventNr(gHades->getCurrentEvent()->getHeader()->getEventSeqNumber());
                     pCalSim->setSector(loc[0]);
                     pCalSim->setCol(loc[2]);
                     pCalSim->setRow(loc[1]);
                     pCalSim->setCharge(fCharge);

		     // Now update HRichTrack
                     Float_t dummy[2];
                     dummy[0] = -1; // noise pads have tracknumber == -1
                     dummy[1] = 0;  // flag == 0
                     TVector rTrack(1, 2, dummy);
                     updateTrack(pCalSim, loc, &rTrack);
                  }
               } else j--;
            } else j--;
         }
      }
   }
}
//============================================================================

//----------------------------------------------------------------------------
Float_t
HRichDigitizer::calcNoiseOnPad(const Float_t fSigmaPad,
                               const Float_t fFloatMean)
{
// the charge induced on the pad by the electronic noise
// is calculated according to a gauss function with
// sigma = fSigmaPad*fIncreaseNoise and mean 0.
// Anyway only values above threshold
// (fSigmaValue * fSigmaPad)
// are drawn

   const Int_t    cutLoop     = 10;
   const Int_t    iThreshold  = static_cast<Int_t>(fFloatMean + fSigmaValue * fSigmaPad);
   Int_t          count       = 0;
   Int_t          chan        = 0;
   Float_t        fCharge     = 0.0;



   if (fSigmaValue < 2.0)
      return static_cast<Int_t>(TMath::Sqrt(2.) * fSigmaPad * fIncreaseNoise
                                * noiseCharge[0] + fFloatMean) - fFloatMean + gRandom->Rndm();

   do {
      // the InverseErf function is defined in the array noiseCharge
      // with 1000 bin in the range (0.95,1). This range should be enough
      // to calculate the noise contribution up to a 2 sigma cut if the
      // variable fIncreaseNoise is 1. If the noise is increased ( e.g.
      // fSigmaNoise=1.9) the values are consistent with a cut of 3.9 sigma.

      chan = static_cast<Int_t>(((1. - 2. * noiseProb * gRandom->Rndm()) - 0.95) / (0.05 / 1000.));
      if (chan >= 0 && chan < 1000) {
         fCharge = TMath::Sqrt(2.) * fSigmaPad * fIncreaseNoise * noiseCharge[chan] + fFloatMean;
      } else fCharge = 0.;
      count++;
   } while ((static_cast<Int_t>(fCharge) <= iThreshold || fCharge > 1000.) && count < cutLoop);
   // the value of sigma can be either too big or too small
   // we try to avoid extreme cases.
   // If the sigma of the pad is too big (>10) or too small(<0.4)
   // then the array used to calculate the value of the charge
   // has not the required granularity and the previous loop
   // is neverending. Therefore we allow only 10 tries, that
   // should be sufficient for a reasonable sigma.

   if ((static_cast<Int_t>(fCharge) <= iThreshold || fCharge > 1000.) && count == cutLoop)
      return 0.;

   return static_cast<Int_t>(fCharge) - fFloatMean + gRandom->Rndm();

}
//===========================================================================

//----------------------------------------------------------------------------
void
HRichDigitizer::addNoiseToCharge(HRichCalSim* calSim)
{
// An additional charge is added on pads that have already
// been "fired" by a photon.
// This charge is drawn according to a Gauss distribution
// with sigma = fSigmaPad*fIncreaseNoise and mean = 0.

   HRichCalParCell* pCell     = NULL;
   Float_t          fCharge   = 0.0;
   Float_t          fSigmaPad = 0.0;


   loc[0] = calSim->getSector();
   loc[1] = calSim->getRow();
   loc[2] = calSim->getCol();

   pCell = static_cast<HRichCalParCell*>(pCalPar->getObject(loc));
   if (NULL != pCell) {
      fSigmaPad = pCell->getSigma();
   } else {
      Error("addNoiseToCharge", "Did not get any valid HRichCalParCell...");
      fSigmaPad = 1.0;
   }

   fCharge = GaussFun(0, fSigmaPad * fIncreaseNoise);
   if (fCharge > fSigmaPad * fSigmaValue) {
      countNoisePad++;
   }

   calSim->addCharge(fCharge);
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichDigitizer::checkPad(HRichCalSim *calSim)
{
// this function checks if the pads fired by the photons
// are above threshold.
// The procedure used to calculate the threshols is the
// same used for real data.  The charge on the pad is composed
// of the signal induced by the photon plus the noise fluctuation.
// The float part of the mean value of the noise distribution
// is added to the charge then
// the charge value is casted to an integer and is
// compared to the threshold.
// If the charge is above threshold, the float part of the mean is
// subtracted from the total charge and a random number between 0 and 1
// is added to the integer produce a float.

   HRichCalParCell* pCell = NULL;

   Float_t fCharge    = calSim->getCharge();
   Int_t   iRow       = calSim->getRow();
   Int_t   iCol       = calSim->getCol();
   Int_t   iSec       = calSim->getSector();
   Int_t   padNr      = iCol + iRow * pGeometryPar->getColumns();
   Int_t   iCharge    = 0;
   Int_t   iThreshold = 0;
   Float_t fSigmaPad  = 0.0;
   Float_t fMeanPad   = 0.0;
   Float_t fFloatMean = 0.0;

   loc[0] = iSec;
   loc[1] = iRow;
   loc[2] = iCol;

   pCell = static_cast<HRichCalParCell*>(pCalPar->getObject(loc));
   if (NULL != pCell) {
      fSigmaPad = pCell->getSigma();
      fMeanPad  = pCell->getOffset();
   } else {
      Error("checkPad", "Did not get any valid HRichCalParCell...");
      Error("checkPad", "Is pad active??? %d", pGeometryPar->getPadsPar()->getPad(padNr)->getPadActive());
      fSigmaPad = 1.0;
   }

   // check, if current pad has valid calibration parameters
   if (0 == fSigmaPad && 0 == fMeanPad) {
      return 0;
   }

   fFloatMean = fMeanPad - static_cast<Int_t>(fMeanPad);
   fCharge    += fFloatMean;
   iCharge    = static_cast<Int_t>(fCharge);
   iThreshold = static_cast<Int_t>(fFloatMean + fSigmaValue * fSigmaPad);

   if (iCharge > iThreshold) {
      fCharge = iCharge - fFloatMean + gRandom->Rndm();
      calSim->setCharge(fCharge);
      return iCharge;
   } else return 0;
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichDigitizer::digitisePads()
{
// for each wire on which some charge has been deposited
// the corresponding coupled pads are calculated.
// The function updateCharge creates a HRichCalSim obj if
// the charge on the pad is greater than zero.
// A cut off threshold is applied to each pad in the execute func.
// after the digitilasation of all the hits.
// If the pad is hit twice in the same event, the charges
// corresponding to the 2 different hits are added.
// The particle track number is passed to the function
// updateCharge, too.

   UInt_t            xcenter    = 0;
   UInt_t            ycenter    = 0;
   Int_t             sector     = -1;
   Int_t             iPadInd    = 0;
   Float_t           energyPhot = 0.0;
   Float_t           yRel       = 0.0;
   Float_t           charge4    = 0.0;
   Float_t           param11    = 0.0;
   Float_t           param21    = 0.0;
   HRichPadTab*      pPadsPar   = NULL;
   HRichPad*         pPad       = NULL;
   HRichPad*         pTmpPad    = NULL;
   TVector*          nTrackTemp = NULL;
   HRichWireSignal*  pCharge    = NULL;

   if (NULL == (pPadsPar = pGeometryPar->getPadsPar())) {
      Error("digitisePads", "Can not get RICH pad parameters");
      return;
   }

   for (Int_t i = 0; i < fChargeOnWireList.GetSize(); ++i) {
      for (Int_t j = 0; j < 9; ++j)
         fDigitPadMatrix[j] = 0.;

      pCharge    = static_cast<HRichWireSignal*>(fChargeOnWireList.At(i));
      nTrackTemp = static_cast<TVector*>(fTrackNumberList.At(i));
      energyPhot = pCharge->getEnergy();
      sector     = pCharge->getSector();
      pPad       = pPadsPar->getPad(pCharge->getX(), pCharge->getY());
      xcenter    = pPad->getPadX();
      ycenter    = pPad->getPadY();

      // the position of the photon hit on the central pad is calculated.
      yRel    = TMath::Abs((pCharge->getY() - pPad->getYmin()) / (pPad->getYmax() - pPad->getYmin()));
      pTmpPad = translateCorners(pPad, pCharge->getX(), pCharge->getY());


      // do not forget to delete it at the end of loop
      // ------------------------ central pad (0,0)

      charge4 = 0.;
      if (2 == pTmpPad->getPadFlag()) {
         // First we calculate the charge on the central pad taking as
         // contributions on the side pads, 0 for the weak coupling and t
         // he mean for the strong one.
         // Then we recalculate the coupling parameter for the
         // strong coupling drawing a number on the landau
         // distribution centered in the mean value.

         param11 = gRandom->Landau(fFactor1, fFactor1Sig);
         param21 = GaussFun(fFactor2, fFactor2Sig);
         if (param21 < 0.)
            param21 = 0.;
         charge4 = q4Calc(pCharge->getCharge(), yRel, param11, param21);
         if (charge4 < 0.) fDigitPadMatrix[4] = 0.;
         else {
            updateCharge(sector, pPad, charge4, nTrackTemp, energyPhot);
            fDigitPadMatrix[4] = charge4;
         }
      }

      //------------------------ loop on other pads
      iPadInd = 0;
      for (Int_t n = -1; n < 2; ++n) {
         for (Int_t m = -1; m < 2; ++m) {

            if ((m != 0 || n != 0) &&
                (static_cast<Int_t>(xcenter) + m) >= 0 &&
                (static_cast<Int_t>(ycenter) + n) >= 0) {
               if (pPadsPar->getPad(xcenter + m, ycenter + n) &&
                   pPadsPar->getPad(xcenter + m, ycenter + n)->getPadActive()) {
                  delete pTmpPad;
                  pTmpPad = translateCorners(pPadsPar->getPad(xcenter + m, ycenter + n),
                                             pCharge->getX(), pCharge->getY());
                  if (2 == pTmpPad->getPadFlag()) {
                     fDigitPadMatrix[iPadInd] = calcIndCharge(yRel, fDigitPadMatrix[4],
                                                              iPadInd, pCharge->getWireNr(),
                                                              param11, param21);
                     if (fDigitPadMatrix[iPadInd] <= 0.)
                        fDigitPadMatrix[iPadInd] = 0.;
                     else
                        updateCharge(sector, pTmpPad, fDigitPadMatrix[iPadInd], nTrackTemp, energyPhot);
                  }
               }
            }
            iPadInd ++;
         }
      }
      delete pTmpPad;
   }
}
//============================================================================

//----------------------------------------------------------------------------
Float_t
HRichDigitizer::calcIndCharge(const Float_t yCharge,
                              const Float_t q4,
                              const Int_t   iPdaIndex,
                              const Int_t   iWireNr,
                              const Float_t param11,
                              const Float_t param21)
{
// iWireNr is used to determine whether the wire is
// the left or the right one.
// Even wire numbers correspond to right.
// for the side pads a coupling costant is multiplied
// by the charge on the central pad, the upper an lower
// pad get an amount of charge that is position dependent.

   switch (iPdaIndex) {
      case 1:
         return (qX(1 - yCharge) * q4);
      case 7:
         return (qX(yCharge) * q4);
      case 3: {
         if (iWireNr % 2 == 0) return (param21 * q4);
         else return (param11 * q4);
      }
      case 5: {
         if (iWireNr % 2 == 0) return (param11 * q4);
         else return (param21 * q4);
      }
      case 2: {
         if (iWireNr % 2 == 0) return (1.37 * param11 * q4 * qX(1 - yCharge));
         else return (1.37 * param21 * q4 * qX(1 - yCharge));
      }
      case 8: {
         if (iWireNr % 2 == 0) return (1.37 * param11 * q4 * qX(yCharge));
         else return (1.37 * param21 * q4 * qX(yCharge));
      }
      case 6: {
         if (iWireNr % 2 == 0) return (1.37 * param21 * q4 * qX(yCharge));
         else return (1.37 * param11 * q4 * qX(yCharge));
      }
      case 0: {
         if (iWireNr % 2 == 0) return (1.37 * param21 * q4 * qX(1 - yCharge));
         else return (1.37 * param11 * q4 * qX(1 - yCharge));
      }
   }
   return 0;
}
//============================================================================

//----------------------------------------------------------------------------
Float_t
HRichDigitizer::qX(const Float_t pos)
{
// This function calculate the charge ratio between the upper/lower
// pad and the central one

   Float_t charge   =  0.0;
   Float_t q0       = -0.03;
   Float_t integral =  0.0;

   integral =  1.0 / (fParam1 * fParam1 * q0 + fParam1 * fParam2) -
               1.0 / (fParam1 * fParam1 + fParam1 * fParam2);
   charge   = ((q0 + fParam2 / fParam1) / (1.0 - (q0 + fParam2 / fParam1)
                                           * fParam1 * fParam1 * integral * pos))
              - fParam2 / fParam1;

   if (charge < 0.) charge = 0.;
   return charge;
}
//============================================================================

//----------------------------------------------------------------------------
Float_t
HRichDigitizer::q4Calc(const Float_t charge,
                       const Float_t pos,
                       const Float_t par1,
                       const Float_t par2)
{
// This function calculate the charge induced on the central pad.
// the coupling factor (75%) from the wire to the pad has
// been discarded since the charge distribution on the wire
// has been calculated fitting pads responses.

   Float_t gX  = 1 + qX(pos) + qX(1 - pos);
   Float_t gX1 = 1 + 1.37 * qX(pos) + 1.37 * qX(1 - pos);
   if (0 != gX)
      return  charge / ((par1 + par2) * gX1 + gX);
   else return 0.0;
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichDigitizer::updateCharge(const Int_t     sector,
                             HRichPad* pPad,
                             const Float_t   charge,
                             TVector*  rTrack,
                             const Float_t   ene)
{
// This function creates an HRichCalSim obj that corresponds
// to a fired pad, it calls the function updateTrack,
// that stores the corresponding track numbers.


   Int_t        fCol    = 0;
   Int_t        fRow    = 0;
   HRichCalSim* pCalSim = NULL;

   pPad->getPadXY(&fCol, &fRow);

   locat[0] = sector;
   locat[1] = fRow;
   locat[2] = fCol;

   pCalSim = static_cast<HRichCalSim*>(catCal->getObject(locat));
   if (NULL == pCalSim) {
      pCalSim = static_cast<HRichCalSim*>(catCal->getSlot(locat));
      if (NULL != pCalSim) {
         pCalSim = new(pCalSim) HRichCalSim;
         pCalSim->setEventNr(gHades->getCurrentEvent()->getHeader()->getEventSeqNumber());
         pCalSim->setSector(locat[0]);
         pCalSim->setCol(locat[2]);
         pCalSim->setRow(locat[1]);
         updateTrack(pCalSim, locat, rTrack);
      }
   }
   if (NULL != pCalSim) {
      pCalSim->addCharge(charge * fChargeScaling / fChargePerChannel);
      // the feed back photon and the direct hits are assigned energy
      // equal to 0. Infact the energy of the other photons emitted in the
      // oem experiment depends on the polar emission angle theta
      // (dispersion in the solid radiators) and this is not true
      // for the feed back photons and for the direct hits.
      // There fore their energy isnt added to the pad they fire
      // and isnt taken into account in the calculation theta->Energy
      // because it would falsify the distribution.

      // cut-off in ADC
      if (pCalSim->getCharge() > fQupper)
         pCalSim->setCharge(8192);

      if (ene > 0) {
         pCalSim->addHit();
         pCalSim->addEnergy(ene);
      }
      // the track object is created for all the photon and direct hits
      // the feedback photons have track number -5 and flag 0.
      // Only the pad fired just because of electronic noise
      // dont correspond to any track objects.
      updateTrack(pCalSim, locat, rTrack);
   }
}
//============================================================================

//---------------------------------------------------------------------------
void
HRichDigitizer::updateTrack(HRichCalSim* pCal,
                            HLocation&   loc,
                            TVector*     rTrack)
{
// this function stores the track numbers of parent
// particles of photons and of direct hits in a linear
// category (HRichTrack). This category is set sortable.

   HRichTrack *pRichTrack = NULL;
   Int_t Ad = pCal->getAddress();

   map<Int_t, vector <HRichTrack*> >::iterator iter = mapTracks.find(Ad);
   if (iter != mapTracks.end()) { // found address
      vector <HRichTrack*> & list = iter->second;
      UInt_t n = list.size();
      for (UInt_t i = 0; i < n; i++) {
         pRichTrack = list[i];
         if (pRichTrack->getTrack()   == (*rTrack)(1) &&
             pRichTrack->getFlag()    == (*rTrack)(2)
            ) return; // already registered
      }
   }

   pRichTrack = static_cast<HRichTrack*>(catTrack->getNewSlot(loc1));
   if (NULL != pRichTrack) {
      pRichTrack = new(pRichTrack) HRichTrack;
      pRichTrack->setEventNr(pCal->getEventNr());
      //usage of TVector because of used in TList, explicit cast to integer is needed since TVector converts all arguments into double.
      pRichTrack->setTrack((Int_t)((*rTrack)(1)));
      pRichTrack->setFlag((Int_t)((*rTrack)(2)));
      pRichTrack->setAddress(Ad);

      if (iter != mapTracks.end()) { // old address
         vector <HRichTrack*> & list = iter->second;
         list.push_back(pRichTrack);
      } else {                    // new address
         vector<HRichTrack*> list;
         list.push_back(pRichTrack);
         mapTracks[Ad] = list;
      }
   }
}
//============================================================================

//---------------------------------------------------------------------------
Bool_t
HRichDigitizer::finalize()
{
   return kTRUE;
}
//============================================================================

//---------------------------------------------------------------------------
// This table contains values of the inverse error function in the range
// [0.95;1.] needed to create a set of random numbers following this
// funcion, used in makeNoiseOnPads
const Float_t HRichDigitizer::noiseCharge[] = {1.3859, 1.38621, 1.38651, 1.38681, 1.38712, 1.38742,
                                               1.38772, 1.38803, 1.38833, 1.38864, 1.38894, 1.38925, 1.38955, 1.38986, 1.39016,
                                               1.39047, 1.39078, 1.39108, 1.39139, 1.3917, 1.392, 1.39231, 1.39262, 1.39293,
                                               1.39324, 1.39355, 1.39386, 1.39416, 1.39447, 1.39478, 1.39509, 1.3954, 1.39572,
                                               1.39603, 1.39634, 1.39665, 1.39696, 1.39727, 1.39759, 1.3979, 1.39821, 1.39852,
                                               1.39884, 1.39915, 1.39946, 1.39978, 1.40009, 1.40041, 1.40072, 1.40104, 1.40135,
                                               1.40167, 1.40199, 1.4023, 1.40262, 1.40294, 1.40325, 1.40357, 1.40389, 1.40421,
                                               1.40453, 1.40485, 1.40516, 1.40548, 1.4058, 1.40612, 1.40644, 1.40676, 1.40708,
                                               1.40741, 1.40773, 1.40805, 1.40837, 1.40869, 1.40901, 1.40934, 1.40966, 1.40998,
                                               1.41031, 1.41063, 1.41096, 1.41128, 1.41161, 1.41193, 1.41226, 1.41258, 1.41291,
                                               1.41323, 1.41356, 1.41389, 1.41422, 1.41454, 1.41487, 1.4152, 1.41553, 1.41586,
                                               1.41619, 1.41651, 1.41684, 1.41717, 1.4175, 1.41784, 1.41817, 1.4185, 1.41883,
                                               1.41916, 1.41949, 1.41983, 1.42016, 1.42049, 1.42083, 1.42116, 1.42149, 1.42183,
                                               1.42216, 1.4225, 1.42283, 1.42317, 1.4235, 1.42384, 1.42418, 1.42451, 1.42485,
                                               1.42519, 1.42553, 1.42587, 1.4262, 1.42654, 1.42688, 1.42722, 1.42756, 1.4279,
                                               1.42824, 1.42858, 1.42892, 1.42927, 1.42961, 1.42995, 1.43029, 1.43064, 1.43098,
                                               1.43132, 1.43167, 1.43201, 1.43236, 1.4327, 1.43305, 1.43339, 1.43374, 1.43408,
                                               1.43443, 1.43478, 1.43512, 1.43547, 1.43582, 1.43617, 1.43652, 1.43687, 1.43722,
                                               1.43757, 1.43792, 1.43827, 1.43862, 1.43897, 1.43932, 1.43967, 1.44002, 1.44038,
                                               1.44073, 1.44108, 1.44144, 1.44179, 1.44215, 1.4425, 1.44286, 1.44321, 1.44357,
                                               1.44392, 1.44428, 1.44464, 1.44499, 1.44535, 1.44571, 1.44607, 1.44643, 1.44679,
                                               1.44715, 1.44751, 1.44787, 1.44823, 1.44859, 1.44895, 1.44931, 1.44967, 1.45004,
                                               1.4504, 1.45076, 1.45113, 1.45149, 1.45185, 1.45222, 1.45259, 1.45295, 1.45332,
                                               1.45368, 1.45405, 1.45442, 1.45479, 1.45515, 1.45552, 1.45589, 1.45626, 1.45663,
                                               1.457, 1.45737, 1.45774, 1.45811, 1.45848, 1.45886, 1.45923, 1.4596, 1.45997,
                                               1.46035, 1.46072, 1.4611, 1.46147, 1.46185, 1.46222, 1.4626, 1.46297, 1.46335,
                                               1.46373, 1.46411, 1.46448, 1.46486, 1.46524, 1.46562, 1.466, 1.46638, 1.46676,
                                               1.46714, 1.46753, 1.46791, 1.46829, 1.46867, 1.46906, 1.46944, 1.46982, 1.47021,
                                               1.47059, 1.47098, 1.47136, 1.47175, 1.47214, 1.47253, 1.47291, 1.4733, 1.47369,
                                               1.47408, 1.47447, 1.47486, 1.47525, 1.47564, 1.47603, 1.47642, 1.47681, 1.47721,
                                               1.4776, 1.47799, 1.47839, 1.47878, 1.47918, 1.47957, 1.47997, 1.48036, 1.48076,
                                               1.48116, 1.48156, 1.48195, 1.48235, 1.48275, 1.48315, 1.48355, 1.48395, 1.48435,
                                               1.48475, 1.48516, 1.48556, 1.48596, 1.48636, 1.48677, 1.48717, 1.48758, 1.48798,
                                               1.48839, 1.4888, 1.4892, 1.48961, 1.49002, 1.49043, 1.49083, 1.49124, 1.49165,
                                               1.49206, 1.49247, 1.49289, 1.4933, 1.49371, 1.49412, 1.49454, 1.49495, 1.49536,
                                               1.49578, 1.49619, 1.49661, 1.49703, 1.49744, 1.49786, 1.49828, 1.4987, 1.49912,
                                               1.49954, 1.49996, 1.50038, 1.5008, 1.50122, 1.50164, 1.50207, 1.50249, 1.50291,
                                               1.50334, 1.50376, 1.50419, 1.50461, 1.50504, 1.50547, 1.50589, 1.50632, 1.50675,
                                               1.50718, 1.50761, 1.50804, 1.50847, 1.5089, 1.50934, 1.50977, 1.5102, 1.51064,
                                               1.51107, 1.5115, 1.51194, 1.51238, 1.51281, 1.51325, 1.51369, 1.51413, 1.51457,
                                               1.51501, 1.51545, 1.51589, 1.51633, 1.51677, 1.51721, 1.51765, 1.5181, 1.51854,
                                               1.51899, 1.51943, 1.51988, 1.52033, 1.52077, 1.52122, 1.52167, 1.52212, 1.52257,
                                               1.52302, 1.52347, 1.52392, 1.52437, 1.52483, 1.52528, 1.52573, 1.52619, 1.52665,
                                               1.5271, 1.52756, 1.52802, 1.52847, 1.52893, 1.52939, 1.52985, 1.53031, 1.53077,
                                               1.53123, 1.5317, 1.53216, 1.53262, 1.53309, 1.53355, 1.53402, 1.53449, 1.53495,
                                               1.53542, 1.53589, 1.53636, 1.53683, 1.5373, 1.53777, 1.53824, 1.53871, 1.53919,
                                               1.53966, 1.54014, 1.54061, 1.54109, 1.54156, 1.54204, 1.54252, 1.543, 1.54348,
                                               1.54396, 1.54444, 1.54492, 1.5454, 1.54589, 1.54637, 1.54685, 1.54734, 1.54783,
                                               1.54831, 1.5488, 1.54929, 1.54978, 1.55027, 1.55076, 1.55125, 1.55174, 1.55223,
                                               1.55273, 1.55322, 1.55372, 1.55421, 1.55471, 1.55521, 1.5557, 1.5562, 1.5567,
                                               1.5572, 1.5577, 1.55821, 1.55871, 1.55921, 1.55972, 1.56022, 1.56073, 1.56123,
                                               1.56174, 1.56225, 1.56276, 1.56327, 1.56378, 1.56429, 1.5648, 1.56532, 1.56583,
                                               1.56635, 1.56686, 1.56738, 1.56789, 1.56841, 1.56893, 1.56945, 1.56997, 1.57049,
                                               1.57102, 1.57154, 1.57206, 1.57259, 1.57312, 1.57364, 1.57417, 1.5747, 1.57523,
                                               1.57576, 1.57629, 1.57682, 1.57735, 1.57789, 1.57842, 1.57896, 1.57949, 1.58003,
                                               1.58057, 1.58111, 1.58165, 1.58219, 1.58273, 1.58328, 1.58382, 1.58437, 1.58491,
                                               1.58546, 1.58601, 1.58655, 1.5871, 1.58765, 1.58821, 1.58876, 1.58931, 1.58987,
                                               1.59042, 1.59098, 1.59154, 1.59209, 1.59265, 1.59321, 1.59378, 1.59434, 1.5949,
                                               1.59547, 1.59603, 1.5966, 1.59717, 1.59773, 1.5983, 1.59887, 1.59945, 1.60002,
                                               1.60059, 1.60117, 1.60174, 1.60232, 1.6029, 1.60348, 1.60406, 1.60464, 1.60522,
                                               1.6058, 1.60639, 1.60697, 1.60756, 1.60815, 1.60874, 1.60933, 1.60992, 1.61051,
                                               1.6111, 1.6117, 1.6123, 1.61289, 1.61349, 1.61409, 1.61469, 1.61529, 1.61589,
                                               1.6165, 1.6171, 1.61771, 1.61832, 1.61892, 1.61953, 1.62015, 1.62076, 1.62137,
                                               1.62199, 1.6226, 1.62322, 1.62384, 1.62446, 1.62508, 1.6257, 1.62632, 1.62695,
                                               1.62757, 1.6282, 1.62883, 1.62946, 1.63009, 1.63072, 1.63136, 1.63199, 1.63263,
                                               1.63327, 1.6339, 1.63454, 1.63519, 1.63583, 1.63647, 1.63712, 1.63777, 1.63841,
                                               1.63906, 1.63972, 1.64037, 1.64102, 1.64168, 1.64233, 1.64299, 1.64365, 1.64431,
                                               1.64498, 1.64564, 1.64631, 1.64697, 1.64764, 1.64831, 1.64898, 1.64966, 1.65033,
                                               1.65101, 1.65168, 1.65236, 1.65304, 1.65372, 1.65441, 1.65509, 1.65578, 1.65647,
                                               1.65716, 1.65785, 1.65854, 1.65924, 1.65993, 1.66063, 1.66133, 1.66203, 1.66273,
                                               1.66344, 1.66414, 1.66485, 1.66556, 1.66627, 1.66698, 1.6677, 1.66841, 1.66913,
                                               1.66985, 1.67057, 1.67129, 1.67202, 1.67274, 1.67347, 1.6742, 1.67493, 1.67567,
                                               1.6764, 1.67714, 1.67788, 1.67862, 1.67936, 1.68011, 1.68085, 1.6816, 1.68235,
                                               1.6831, 1.68386, 1.68461, 1.68537, 1.68613, 1.68689, 1.68766, 1.68842, 1.68919,
                                               1.68996, 1.69073, 1.69151, 1.69228, 1.69306, 1.69384, 1.69462, 1.6954, 1.69619,
                                               1.69698, 1.69777, 1.69856, 1.69936, 1.70015, 1.70095, 1.70175, 1.70256, 1.70336,
                                               1.70417, 1.70498, 1.70579, 1.7066, 1.70742, 1.70824, 1.70906, 1.70988, 1.71071,
                                               1.71154, 1.71237, 1.7132, 1.71404, 1.71487, 1.71571, 1.71656, 1.7174, 1.71825,
                                               1.7191, 1.71995, 1.72081, 1.72166, 1.72252, 1.72339, 1.72425, 1.72512, 1.72599,
                                               1.72686, 1.72774, 1.72862, 1.7295, 1.73038, 1.73127, 1.73216, 1.73305, 1.73394,
                                               1.73484, 1.73574, 1.73664, 1.73755, 1.73846, 1.73937, 1.74028, 1.7412, 1.74212,
                                               1.74304, 1.74397, 1.7449, 1.74583, 1.74677, 1.7477, 1.74865, 1.74959, 1.75054,
                                               1.75149, 1.75244, 1.7534, 1.75436, 1.75532, 1.75629, 1.75726, 1.75823, 1.75921,
                                               1.76019, 1.76117, 1.76216, 1.76315, 1.76415, 1.76514, 1.76614, 1.76715, 1.76816,
                                               1.76917, 1.77018, 1.7712, 1.77223, 1.77325, 1.77428, 1.77532, 1.77635, 1.7774,
                                               1.77844, 1.77949, 1.78054, 1.7816, 1.78266, 1.78373, 1.7848, 1.78587, 1.78695,
                                               1.78803, 1.78912, 1.79021, 1.7913, 1.7924, 1.7935, 1.79461, 1.79572, 1.79684,
                                               1.79796, 1.79909, 1.80022, 1.80135, 1.80249, 1.80363, 1.80478, 1.80594, 1.80709,
                                               1.80826, 1.80942, 1.8106, 1.81178, 1.81296, 1.81415, 1.81534, 1.81654, 1.81774,
                                               1.81895, 1.82017, 1.82139, 1.82261, 1.82384, 1.82508, 1.82632, 1.82757, 1.82882,
                                               1.83008, 1.83135, 1.83262, 1.83389, 1.83518, 1.83646, 1.83776, 1.83906, 1.84037,
                                               1.84168, 1.843, 1.84433, 1.84566, 1.847, 1.84835, 1.8497, 1.85106, 1.85243,
                                               1.8538, 1.85518, 1.85657, 1.85796, 1.85937, 1.86078, 1.86219, 1.86362, 1.86505,
                                               1.86649, 1.86794, 1.86939, 1.87086, 1.87233, 1.87381, 1.8753, 1.87679, 1.8783,
                                               1.87981, 1.88133, 1.88286, 1.8844, 1.88595, 1.88751, 1.88908, 1.89065, 1.89224,
                                               1.89383, 1.89544, 1.89705, 1.89868, 1.90031, 1.90196, 1.90361, 1.90528, 1.90696,
                                               1.90864, 1.91034, 1.91205, 1.91377, 1.9155, 1.91725, 1.919, 1.92077, 1.92255,
                                               1.92434, 1.92615, 1.92796, 1.92979, 1.93163, 1.93349, 1.93536, 1.93724, 1.93914,
                                               1.94105, 1.94297, 1.94491, 1.94687, 1.94884, 1.95082, 1.95282, 1.95484, 1.95687,
                                               1.95892, 1.96098, 1.96306, 1.96516, 1.96728, 1.96941, 1.97156, 1.97373, 1.97592,
                                               1.97813, 1.98036, 1.98261, 1.98487, 1.98716, 1.98947, 1.9918, 1.99415, 1.99653,
                                               1.99892, 2.00135, 2.00379, 2.00626, 2.00875, 2.01127, 2.01381, 2.01638, 2.01898,
                                               2.02161, 2.02426, 2.02694, 2.02965, 2.0324, 2.03517, 2.03797, 2.04081, 2.04368,
                                               2.04658, 2.04952, 2.0525, 2.05551, 2.05856, 2.06164, 2.06477, 2.06794, 2.07115,
                                               2.0744, 2.0777, 2.08105, 2.08444, 2.08788, 2.09137, 2.09491, 2.09851, 2.10216,
                                               2.10587, 2.10963, 2.11346, 2.11735, 2.1213, 2.12533, 2.12942, 2.13358, 2.13783,
                                               2.14214, 2.14654, 2.15103, 2.1556, 2.16027, 2.16503, 2.16989, 2.17486, 2.17993,
                                               2.18512, 2.19044, 2.19587, 2.20144, 2.20716, 2.21301, 2.21903, 2.22521, 2.23156,
                                               2.2381, 2.24484, 2.25179, 2.25896, 2.26637, 2.27404, 2.28199, 2.29023, 2.2988,
                                               2.30773, 2.31703, 2.32675, 2.33694, 2.34763, 2.35889, 2.37078, 2.38339, 2.39679,
                                               2.41112, 2.42652, 2.44316, 2.46127, 2.48115, 2.50322, 2.52803, 2.5564, 2.58961,
                                               2.62974, 2.68069, 2.75106, 2.86776
                                              };

















