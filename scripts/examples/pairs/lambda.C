
#include "hades.h"
#include "hloop.h"
#include "htool.h"
#include "hcategorymanager.h"
#include "hparticleanglecor.h"
#include "hparticlepairmaker.h"
#include "hparticletool.h"
#include "hphysicsconstants.h"
#include "hhistmap.h"
#include "hparticletracksorter.h"
#include "henergylosscorrpar.h"


#include "hcategory.h"
#include "hlinearcategory.h"
#include "hrichhit.h"
#include "hrichhitsim.h"
#include "hparticlecand.h"
#include "hparticlecandsim.h"
#include "hparticlepair.h"
#include "hparticlegeantpair.h"

#include "hgeantkine.h"




#include "hparticledef.h"
#include "hstartdef.h"
#include "richdef.h"


#include "hparticlegeant.h"
#include "hparticlegeantdecay.h"
#include "hparticlegeantevent.h"
#include "hparticlecutrange.h"




#include "TTree.h"


#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <iomanip>

using namespace std;
using namespace Particle;


Bool_t selectHadrons(HParticleCand* pcand)
{
    // build in selection function for hadron candidates.
    // Requires besides an RK + META and fitted
    // inner+outer segment.

    Bool_t test = kFALSE;
    if( pcand->isFlagAND(4,
			 Particle::kIsAcceptedHitInnerMDC,
			 Particle::kIsAcceptedHitOuterMDC,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getInnerSegmentChi2() > 0
       &&
       pcand->getChi2()             < 25      // RK
      ) test = kTRUE;

    if(!test) return kFALSE;

    if(test) test = pcand->getMetaMatchQuality() < 3 ? kTRUE : kFALSE ;

    return test;
}

Bool_t selectPos(HParticleCand* cand)
{
    // standard function for positrons
    // require richmatch (broad window,  frequireRich =kTRUE)
    // Select positive polatity

    if( cand->getCharge() > 0 ) return kTRUE;
    else                        return kFALSE;

    return kTRUE;
}

Bool_t selectNeg(HParticleCand* cand)
{
    // standard function for electrons
    // require richmatch (broad window,  frequireRich =kTRUE)
    // Select negative polatity

    if( cand->getCharge() < 0 ) return kTRUE;
    else                        return kFALSE;

    return kTRUE;
}



Int_t lambda(TString infileList="/hera/hades/dstsim/apr12/dst_fit/Au_Au_1230MeV_omega_lambda_1000evts_1_1_dst_apr12.root",
	     TString outfile="lambdasim.root",Int_t nEvents=1000)
{
    //
    //  infileList : comma seprated file list "file1.root,file2.root"
    //  outfile    : used to store hists in root file
    //  nEvents    : number of events to processed. if  nEvents < entries or < 0 the chain will be processed
    //
    //
    //  Example analysis for lambda decays in simulation using HParticleGeantEvent and HParticlePairmaker
    //
    //  1. All lambda pairs from simulation are build by
    //     requesting  HParticleGeantDecays from HParticleGeantEvent with motherID 18 (lambda)
    //  2. All reconstructed Candidates for the geant tracks in the decay are retrieved
    //  3. HParticleGeantPair is filled and treated like pairs from reconstruction
    //  4. All HParticlePairs are filtered HParticlePairMaker where both legs are marked
    //     kIsUsed and the pair unlikesign polarity.
    //  5. Vertex and off-vertex cuts are applied to the pairs
    //  6. histgrams for different observables are filled in simulation (yield,acceptance,true reco)
    //     and reco pairs (before cut, after cut, true pairs)




    HLoop loop(kTRUE);

   // Bool_t ret = loop.addMultFiles(infileList);
    Bool_t ret = loop.addFiles(infileList);
    if(ret == 0) {
	cout<<"READBACK: ERROR : cannot find inputfiles : "<<infileList.Data()<<endl;
	return 1;
    }

//    if(!loop.setInput("-*,+HParticleCand")) {
    if(!loop.setInput("")) {
	cout<<"READBACK: ERROR : cannot read input !"<<endl;
	exit(1);
    } // read all categories
    loop.printCategories();
    loop.printChain();


    HLinearCategory* kineCat = (HLinearCategory*) HCategoryManager::getCategory(catGeantKine);
    HCategory* particleCat   = HCategoryManager::getCategory(catParticleCand);
    HCategory* evtInfoCat    = HCategoryManager::getCategory(catParticleEvtInfo);

    if(!particleCat) { cout<<"No particleCat in input!"<<endl; return 1;}
    if(!evtInfoCat)  { cout<<"No evenInfoCat in input!"<<endl; return 1;}

    Int_t entries = loop.getEntries();
    if(nEvents < entries && nEvents >= 0 ) entries = nEvents;


    //---------------------------HISTOGRAMS-----------------------------------------------------
    HHistMap hM(outfile);

    TString particles     = "proton,pion,lambda";  // has to match particles in eType
    TString dist          = "lambda,track";
    TString version       = "noCut,Cut,True";  // has to match eMonitor
    TString geantversion  = "yield,acc,reco";  // has to match eEff
    TString geantmult     = "4pi,acc,reco";    //


    enum eType {
     kProton     = 0,
     kPion       = 1,
     kLambda     = 2,
     kTrack      = 3,
     kDecay      = 4,
     kMassProton = 5,
     kMassPion   = 6,
     kMomPion    = 7,
     koAngle     = 8
    };

    enum eMonitor {
	kNoCut = 0,
	kCut   = 1,
        kReal  = 2
    };

    enum eEff {
	kYield = 0,
	kAcc   = 1,
        kReco  = 2
    };


    Float_t massPionWidth        =  50;
    Float_t massProtonWidth      = 100;


    vector<HParticleCutRange> cuts;
    HParticleCutRange cutProton("cutProton",1,    4, 1e30);
    HParticleCutRange cutPion  ("cutPion"  ,2,   21, 1e30); //21
    HParticleCutRange cutLambda("cutLambda",3,-1e30, 5);
    HParticleCutRange cutTrack ("cutTrack" ,4,-1e30, 9);
    HParticleCutRange cutDecay ("cutDecay" ,5,   50, 1e30);
    HParticleCutRange cutMassProton ("cutMassProton",6,HPhysicsConstants::mass(14)-massProtonWidth,HPhysicsConstants::mass(14)+massProtonWidth);
    HParticleCutRange cutMassPion   ("cutMassPion"  ,7,HPhysicsConstants::mass(9) -massPionWidth  ,HPhysicsConstants::mass(9) +massPionWidth);
    HParticleCutRange cutMomPion    ("cutMomPion"   ,8,170,550);
    HParticleCutRange cutOAngle     ("cutOAngle"    ,9,15 ,1e30);

    cuts.push_back(cutProton);
    cuts.push_back(cutPion);
    cuts.push_back(cutLambda);
    cuts.push_back(cutTrack);
    cuts.push_back(cutDecay);

    cuts.push_back(cutMassProton);
    cuts.push_back(cutMassPion);
    cuts.push_back(cutMomPion);
    cuts.push_back(cutOAngle);


    hM.addHistArray("TH1F","hminVerDist"    ,"hminVerDist[%i][%i]"  ,"hminVerDist[%i][%i]"    ,300,0.,90.,0,0.,0.,0,0.,0.,"min distance [mm]","counts","","hists/vertex",3, 3,-1,-1,-1,particles,version);  // create hists with special names;
    hM.addHistArray("TH1F","hVerDist_lambda","hVerDist_[lambda][%i]","hVerDist_[lambda][%i]"  ,300,    0,  90,0,0,0,0,0,0,"distance [mm]"    ,"counts","","hists/vertex",3,-1,-1,-1,-1,version);
    hM.addHistArray("TH1F","hTrackDist"     ,"hTrackDist[%i]"       ,"hTrackDist[%i]"         ,300,    0,  90,0,0,0,0,0,0,"distance [mm]"    ,"counts","","hists/vertex",3,-1,-1,-1,-1,version);
    hM.addHistArray("TH1F","hmass"          ,"hmass[%i]"            ,"hmass[%i]"              ,400,-2000,2000,0,0,0,0,0,0,"mass [MeV/c2]"    ,"counts","","hists/mass"  ,3,-1,-1,-1,-1,version);
    hM.addHistArray("TH1F","hmom_pion"      ,"hmom_[pion][%i]"      ,"hmom_[pion][%i]"        ,400,    0,2000,0,0,0,0,0,0,"momentum [MeV/c]" ,"counts","","hists/mom"   ,3,-1,-1,-1,-1,version);
    hM.addHistArray("TH1F","hinvmass"       ,"hinvmass[%i]"         ,"hinvmass[%i]"           ,200,    0,2000,0,0,0,0,0,0,"mass [MeV/c2]"    ,"counts","","hists/mass"  ,3,-1,-1,-1,-1,version);

    hM.addHistArray("TH1F","hdistVertex"    ,"hdistVertex[%i][%i]"   ,"hdistVertex[%i][%i]"    ,90,0.,90.,0,0.,0.,0,0.,0.,"distance from vertex[mm]","counts"    ,"","hists/distEff",3, 3,-1,-1,-1,particles,geantversion);  // create hists with special names;
    hM.addHistArray("TH1F","hdistVertexEff" ,"hdistVertexEff[%i]"   ,"hdistVertexEff[%i]"      ,90,0.,90.,0,0.,0.,0,0.,0.,"distance from vertex[mm]","efficiency","","hists/distEff",3,-1,-1,-1,-1,particles);  // create hists with special names;



    hM.addHistArray("TH1F","hgeant_minVerDist"    ,"hgeant_minVerDist[%i][%i]"  ,"hgeant_minVerDist[%i][%i]"    ,300,0.,90.,0,0.,0.,0,0.,0.,"min distance [mm]","counts","","hists/vertex",3, 3,-1,-1,-1,particles,geantversion);  // create hists with special names;
    hM.addHistArray("TH1F","hgeant_VerDist_lambda","hgeant_VerDist_[lambda][%i]","hgeant_VerDist_[lambda][%i]"  ,300,    0,  90,0,0,0,0,0,0,"distance [mm]"    ,"counts","","hists/vertex",3,-1,-1,-1,-1,geantversion);
    hM.addHistArray("TH1F","hgeant_TrackDist"     ,"hgeant_TrackDist[%i]"       ,"hgeant_TrackDist[%i]"         ,300,    0,  90,0,0,0,0,0,0,"distance [mm]"    ,"counts","","hists/vertex",3,-1,-1,-1,-1,geantversion);
    hM.addHistArray("TH1F","hgeant_mass"          ,"hgeant_mass[%i]"            ,"hgeant_mass[%i]"              ,400,-2000,2000,0,0,0,0,0,0,"mass [MeV/c2]"    ,"counts","","hists/mass"  ,3,-1,-1,-1,-1,geantversion);
    hM.addHistArray("TH1F","hgeant_mom_pion"      ,"hgeant_mom_[pion][%i]"      ,"hgeant_mom_[pion][%i]"        ,400,    0,2000,0,0,0,0,0,0,"momentum [MeV/c]" ,"counts","","hists/mom"   ,3,-1,-1,-1,-1,geantversion);
    hM.addHistArray("TH1F","hgeant_invmass"       ,"hgeant_invmass[%i]"         ,"hgeant_invmass[%i]"           ,200,    0,2000,0,0,0,0,0,0,"mass [MeV/c2]"    ,"counts","","hists/mass"  ,3,-1,-1,-1,-1,geantversion);


    hM.addHistArray("TH1F","hgeant_mult"          ,"hgeant_mult[%i]"            ,"hgeant_mult[%i]"              ,300,    0,300 ,0,0,0,0,0,0,"mult"             ,"counts","","hists/mult"  ,3,-1,-1,-1,-1,geantmult);
    hM.addHist     ("TH1F","hgeant_impact"        ,"hgeant_impact" ,200, 0,15  ,0,0,0,0,0,0,"impact parameter [fm]","counts","","hists/imapct");


    //--------------------------CONFIGURATION---------------------------------------------------
    //At begin of the program (outside the event loop)

    HParticleTrackSorter sorter;
    //sorter.setDebug();                                            // for debug
    //sorter.setPrintLevel(3);                                      // max prints
    //sorter.setRICHMatching(HParticleTrackSorter::kUseRKRICHWindow,4.); // select matching RICH-MDC for selectLeptons() function
    //sorter.setIgnoreInnerMDC();                                   // do not reject Double_t inner MDC hits
    //sorter.setIgnoreOuterMDC();                                   // do not reject Double_t outer MDC hits
    //sorter.setIgnoreMETA();                                       // do not reject Double_t META hits
    //sorter.setIgnorePreviousIndex();                              // do not reject indices from previous selctions
    sorter.init();                                                  // get catgegory pointers etc...

    HEnergyLossCorrPar enLossCorr;
    enLossCorr.setDefaultPar("apr12"); //  "jan04","nov02","aug04","apr12"

    HParticlePairMaker pairmaker;
    pairmaker.setPIDs(14,9,18);                         // lambda     default : leptons (2,3,51)
    pairmaker.setPIDsSelection(selectPos,selectNeg);    // set your own selection function for pid1+pid2 (signature :  Bool_t myfunction(HParticleCand*) )
    pairmaker.setUseLeptons(kFALSE);                    // kTRUE : use kIsLepton, kFALSE: kIsUsed for selection of reference  (default kTRUE)
    // pairmake.setRequireRich(kFALSE);                 // do not ask for rich Hit in selection functions ( default = kTRUE)
    pairmaker.setVertexCase(Particle::kVertexParticle); // select which event vertex to use (default = kVertexParticle)
    HParticlePair::setDoMomentumCorrection(kTRUE);      // default : kTRUE (do energyloss correction

    HParticleGeantEvent geantevent;

    vector<HParticlePair*> pairs;




    if(entries < 0 || entries > loop.getEntries()) entries = loop.getEntries();


    for (Int_t i=0; i < entries; i++) {
	Int_t nbytes =  loop.nextEvent(i);             // get next event. categories will be cleared before
	if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last event reached

	if(i%1000 == 0) cout<<"event "<<i<<endl;

        //--------------------------------------------------------------------------
        //  LOOSE SELECTION
	sorter.cleanUp();
        sorter.resetFlags(kTRUE,kTRUE,kTRUE,kTRUE);      // reset all flags for flags (0-28) ,reject,used,lepton
        Int_t nCandHad     = sorter.fill(selectHadrons); // fill only good hadrons (already marked good leptons will be skipped)
        Int_t nCandHadBest = sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsHadron);

	pairmaker .nextEvent();   // build all pairs
	geantevent.nextEvent();

	hM.get("hgeant_mult",0)->Fill(geantevent.getMultiplicity(-1,0,0,0,0) );                                     // 4pi primaries   :id,generation,charge,type,detbits
        hM.get("hgeant_mult",1)->Fill(geantevent.getMultiplicity(-1,0,0,1,0) );                                     // primaries in acceptance
	hM.get("hgeant_mult",2)->Fill(geantevent.getMultiplicity(-1,0,0,2,kIsInInnerMDC|kIsInOuterMDC|kIsInMETA) ); // primaries reco : kIsUsed + !isGostTrack() + kIsInInnerMDC|kIsInOuterMDC|kIsInMETA
        hM.get("hgeant_impact")->Fill(geantevent.getImpactParam());

 	vector<HParticleGeantDecay*> decays;

	//--------------------------------------------------------------------------
        // build all lambda pairs from Geant
	geantevent.isDecay(decays,18,-2);          // all lambda decays (PLUTO+UrQmd) :  motherid,generation,med,dist
	vector<Int_t> pids;
	pids.push_back(14);
	pids.push_back(9);
        vector <HParticleGeantPair> vgPair;

	for(UInt_t j = 0; j < decays.size(); j++) {
	    HParticleGeantDecay* lambda = decays[j];
	    if(lambda->isDaughterPID(pids)){  // proton and piminus found
		HGeantKine* d1   = lambda->getDaughter(0);
                HGeantKine* d2   = lambda->getDaughter(1);
		HGeantKine* moth = lambda->getMother();

		HParticleGeantPair gpair;  // set proton as first daughter
		if(d1->getID()==14) gpair.setPair(d1,d2,moth,18,geantevent.getEventVertex());
                else                gpair.setPair(d2,d1,moth,18,geantevent.getEventVertex());
		vgPair.push_back(gpair);
	    }
	}
	//--------------------------------------------------------------------------


	//--------------------------------------------------------------------------
	// Geant pairs ana

        vector<HParticleCandSim*> vreco;
	for(UInt_t j = 0; j < vgPair.size(); j++){
	    HParticleGeantPair& pair = vgPair[j];
	    Int_t indProton = 0;
	    Int_t indPion   = 1;


            HParticleGeant* pProton  = geantevent.getParticle(pair.getCand(indProton));
            HParticleGeant* pPion    = geantevent.getParticle(pair.getCand(indPion));
            Bool_t inAccPion   = pPion  ->isInAcceptance();
            Bool_t inAccProton = pProton->isInAcceptance();

	    //--------------------------------------------------------------------------
	    // find the true reconstructed candidates for this
            // geant tracks
	    HParticleCandSim* recoProton = 0;
            HParticleCandSim* recoPion   = 0;

	    for(UInt_t l=0; l < pProton->getNRecoCand(); l++){
		HParticleCandSim* p = pProton->getRecoCand(l);
		if(!p->isGhostTrack() && p->isInDetectors(kIsInInnerMDC|kIsInOuterMDC|kIsInMETA)) {
		    recoProton = p;
		    break;
		}
	    }
	    for(UInt_t l=0; l < pPion->getNRecoCand(); l++){
		HParticleCandSim* p = pPion->getRecoCand(l);
		if(!p->isGhostTrack() && p->isInDetectors(kIsInInnerMDC|kIsInOuterMDC|kIsInMETA)) {
		    recoPion = p;
		    break;
		}
	    }

	    Float_t distFromVertexProton = pProton ->getDistFromVertex(&geantevent.getEventVertex());;
            Float_t distFromVertexPion   = pPion   ->getDistFromVertex(&geantevent.getEventVertex());;

            hM.get("hdistVertex",kProton,kYield)->Fill(distFromVertexProton);
            hM.get("hdistVertex",kPion  ,kYield)->Fill(distFromVertexPion);
            hM.get("hdistVertex",kLambda,kYield)->Fill(distFromVertexPion);

	    if(inAccProton)           hM.get("hdistVertex",kProton,kAcc)->Fill(distFromVertexProton);
            if(inAccPion)             hM.get("hdistVertex",kPion  ,kAcc)->Fill(distFromVertexPion);
            if(inAccProton&&inAccPion)hM.get("hdistVertex",kLambda,kAcc)->Fill(distFromVertexPion);

	    if(recoProton)             hM.get("hdistVertex",kProton,kReco)->Fill(distFromVertexProton);
	    if(recoPion  )             hM.get("hdistVertex",kPion  ,kReco)->Fill(distFromVertexPion);
            if(recoProton&&recoPion  ) hM.get("hdistVertex",kLambda,kReco)->Fill(distFromVertexPion);
	    //--------------------------------------------------------------------------







	    // yield
	    hM.get("hgeant_mass",kYield)          ->Fill( pair.getCandVect(indPion  ).M() * HPhysicsConstants::charge(pair.getCandPID(indPion  )) );
	    hM.get("hgeant_mass",kYield)          ->Fill( pair.getCandVect(indProton).M() * HPhysicsConstants::charge(pair.getCandPID(indProton)) );
	    hM.get("hgeant_mom_pion",kYield)      ->Fill( pair.getCand(indPion)->getTotalMomentum());
	    hM.get("hgeant_invmass" ,kYield)      ->Fill( pair.M());
	    hM.get("hgeant_minVerDist",kProton,kYield)->Fill( pair.getVerMinDistCand(indProton));
	    hM.get("hgeant_minVerDist",kPion  ,kYield)->Fill( pair.getVerMinDistCand(indPion)  );
	    hM.get("hgeant_minVerDist",kLambda,kYield)->Fill( pair.getVerMinDistMother());
	    hM.get("hgeant_VerDist_lambda",kYield)->Fill( pair.getVerDistMother());
	    hM.get("hgeant_TrackDist",kYield)     ->Fill( pair.getMinDistCandidates());

	    // acceptance
	    if(inAccProton && inAccPion){
		hM.get("hgeant_mass",kAcc)          ->Fill( pair.getCandVect(indPion  ).M() * HPhysicsConstants::charge(pair.getCandPID(indPion  )) );
		hM.get("hgeant_mass",kAcc)          ->Fill( pair.getCandVect(indProton).M() * HPhysicsConstants::charge(pair.getCandPID(indProton)) );
		hM.get("hgeant_mom_pion",kAcc)      ->Fill( pair.getCand(indPion)->getTotalMomentum());
		hM.get("hgeant_invmass" ,kAcc)      ->Fill( pair.M());
		hM.get("hgeant_minVerDist",kProton,kAcc)->Fill( pair.getVerMinDistCand(indProton));
		hM.get("hgeant_minVerDist",kPion  ,kAcc)->Fill( pair.getVerMinDistCand(indPion)  );
		hM.get("hgeant_minVerDist",kLambda,kAcc)->Fill( pair.getVerMinDistMother());
		hM.get("hgeant_VerDist_lambda",kAcc)->Fill( pair.getVerDistMother());
		hM.get("hgeant_TrackDist",kAcc)     ->Fill( pair.getMinDistCandidates());
	    }


	    vector<Double_t> vars;
	    vars.assign(cuts.size(),0);
	    vars[kMassPion] = pair.getCandVect(indPion  ).M();
	    vars[kMassProton] = pair.getCandVect(indProton).M();
	    vars[koAngle    ] = pair.getOpeningAngle();
	    vars[kMomPion]    = pair.getCand(indPion)->getTotalMomentum();
	    vars[kProton ]    = pair.getVerMinDistCand(indProton);
	    vars[kPion   ]    = pair.getVerMinDistCand(indPion);
	    vars[kLambda ]    = pair.getVerMinDistMother();
	    vars[kDecay  ]    = pair.getVerDistMother();
	    vars[kTrack  ]    = pair.getMinDistCandidates();

            UInt_t good = 0;
	    for(UInt_t l = 0; l < cuts.size(); l ++ ){
                  if(cuts[l].eval(vars[l],0)) good++;
	    }

	    // reco
	    if( good == cuts.size() ) // all cuts sucessfull
	    {
		hM.get("hgeant_mass",kReco)          ->Fill( pair.getCandVect(indPion  ).M() * HPhysicsConstants::charge(pair.getCandPID(indPion  )) );
		hM.get("hgeant_mass",kReco)          ->Fill( pair.getCandVect(indProton).M() * HPhysicsConstants::charge(pair.getCandPID(indProton)) );
		hM.get("hgeant_mom_pion",kReco)      ->Fill( pair.getCand(indPion)->getTotalMomentum());
		hM.get("hgeant_invmass" ,kReco)      ->Fill( pair.M());
		hM.get("hgeant_minVerDist",kProton,kReco)->Fill( pair.getVerMinDistCand(indProton));
		hM.get("hgeant_minVerDist",kPion  ,kReco)->Fill( pair.getVerMinDistCand(indPion)  );
		hM.get("hgeant_minVerDist",kLambda,kReco)->Fill( pair.getVerMinDistMother());
		hM.get("hgeant_VerDist_lambda",kReco)->Fill( pair.getVerDistMother());
		hM.get("hgeant_TrackDist",kReco)     ->Fill( pair.getMinDistCandidates());
	    }
	}
	//--------------------------------------------------------------------------


	UInt_t flag = kPairCase16 |kIsUsed2|kNoSamePolarity;       // both candidates full reconstructed + opposite charge
	pairmaker.filterPairsVector(pairs,flag);



	for(UInt_t l = 0; l < pairs.size(); l++ ){
	    HParticlePair* pair = pairs[l];

	    if(pair->isFakePair())     continue;

	    Int_t indProton,indPion;

            //------------------------------------------------------------
	    // find out which candidate is which particle
	    if(pair->getCandPID(0) == 14) {
		indProton = 0;
		indPion   = 1;
                if(pair->getCandPID(1) != 9) cout<<"proton but no pion "<<endl;

	    } else if (pair->getCandPID(0) == 9) {
		indProton = 1;
		indPion   = 0;
		if(pair->getCandPID(1) != 14) cout<<"pion but no proton "<<endl;
	    } else {
		cout<<"error : wrong pid case detected"<<endl;
                continue;
	    }
            //------------------------------------------------------------


	    Float_t masspion   = pair->getCand(indPion  )->getMass();
            Float_t massproton = pair->getCand(indProton)->getMass();


            // before cut
	    hM.get("hmass",kNoCut)              ->Fill( masspion   * pair->getCand(indPion  )->getCharge() );
            hM.get("hmass",kNoCut)              ->Fill( massproton * pair->getCand(indProton)->getCharge() );
 	    hM.get("hmom_pion",kNoCut)          ->Fill( pair->getCand(indPion)->getMomentum());
            hM.get("hinvmass" ,kNoCut)          ->Fill( pair->M());
	    hM.get("hminVerDist",kProton,kNoCut)->Fill( pair->getVerMinDistCand(indProton));
	    hM.get("hminVerDist",kPion  ,kNoCut)->Fill( pair->getVerMinDistCand(indPion)  );
	    hM.get("hminVerDist",kLambda,kNoCut)->Fill( pair->getVerMinDistMother());
 	    hM.get("hVerDist_lambda",kNoCut)    ->Fill( pair->getVerDistMother());
            hM.get("hTrackDist",kNoCut)         ->Fill( pair->getMinDistCandidates());


	    vector<Double_t> vars;
	    vars.assign(cuts.size(),0);
	    vars[kMassPion]   = masspion;
	    vars[kMassProton] = massproton;
	    vars[koAngle    ] = pair->getOpeningAngle();
	    vars[kMomPion]    = pair->getCand(indPion)->getMomentum();
	    vars[kProton ]    = pair->getVerMinDistCand(indProton);
	    vars[kPion   ]    = pair->getVerMinDistCand(indPion);
	    vars[kLambda ]    = pair->getVerMinDistMother();
	    vars[kDecay  ]    = pair->getVerDistMother();
	    vars[kTrack  ]    = pair->getMinDistCandidates();

            UInt_t good = 0;
	    for(UInt_t m = 0; m < cuts.size(); m ++ ){
                  if(cuts[m].eval(vars[m],1)) good++;
	    }


            // apply cut
	    if( good == cuts.size() )
	    {

		hM.get("hmom_pion",kCut)          ->Fill( pair->getCand(indPion)->getMomentum());
		hM.get("hmass",kCut)              ->Fill( masspion   * pair->getCand(indPion)->getCharge() );
		hM.get("hmass",kCut)              ->Fill( massproton * pair->getCand(indProton)->getCharge() );
		hM.get("hminVerDist",kProton,kCut)->Fill( pair->getVerMinDistCand(indProton));
		hM.get("hminVerDist",kPion  ,kCut)->Fill( pair->getVerMinDistCand(indPion)  );
		hM.get("hminVerDist",kLambda,kCut)->Fill( pair->getVerMinDistMother());
		hM.get("hVerDist_lambda",kCut)    ->Fill( pair->getVerDistMother());
		hM.get("hTrackDist",kCut)         ->Fill( pair->getMinDistCandidates());

		hM.get("hinvmass",kCut)->Fill( pair->M());


		// true pairs
		if(pair->getIsSimulation() && pair->isTruePair()){  // both

		    hM.get("hmom_pion",kReal)          ->Fill( pair->getCand(indPion)->getMomentum());
		    hM.get("hmass",kReal)              ->Fill( masspion   * pair->getCand(indPion)->getCharge() );
		    hM.get("hmass",kReal)              ->Fill( massproton * pair->getCand(indProton)->getCharge() );
		    hM.get("hminVerDist",kProton,kReal)->Fill( pair->getVerMinDistCand(indProton));
		    hM.get("hminVerDist",kPion  ,kReal)->Fill( pair->getVerMinDistCand(indPion)  );
		    hM.get("hminVerDist",kLambda,kReal)->Fill( pair->getVerMinDistMother());
		    hM.get("hVerDist_lambda",kReal)    ->Fill( pair->getVerDistMother());
		    hM.get("hTrackDist",kReal)         ->Fill( pair->getMinDistCandidates());

		    hM.get("hinvmass",kReal)->Fill( pair->M());
		}

	    }

	}

    } // end eventloop


    sorter.finalize(); // clean up stuff


    //------------------------------------------------------------
    // calculate efficiency
    hM.get("hdistVertexEff",kProton)->Divide(hM.get("hdistVertex",kProton,kReco),hM.get("hdistVertex",kProton,kAcc));
    hM.get("hdistVertexEff",kPion)  ->Divide(hM.get("hdistVertex",kPion  ,kReco),hM.get("hdistVertex",kPion  ,kAcc));
    hM.get("hdistVertexEff",kLambda)->Divide(hM.get("hdistVertex",kLambda,kReco),hM.get("hdistVertex",kLambda,kAcc));
    //------------------------------------------------------------
    for(UInt_t i = 0; i < cuts.size(); i ++) cuts[i].print();

    hM.writeHists();


    delete gHades;
    return 0;

}
