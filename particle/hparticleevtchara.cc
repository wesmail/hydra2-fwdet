//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : B. Kardan  08.12.2015
//*-- VERSION : 0.07
//
// HParticleEvtChara
//
// Purpose: EventCharacterization
// - Centrality
// - QVerctor and Psi (Reaction Plane Estimate) from FW, META and Tracks 
// - Output Table of Glauber&Systematics 
// - QA and Statistics.... 
//
//  input files can be found at : example /cvmfs/hades.gsi.de/param/eventchara/
//
//
//  setBeamTime() and setGeneration() has to be set for the correct
//  data set (default "apr12" and "gen8")
//
//  to choose the preferred Centrality Estimator use the function
//  useEstimator() with following implemented options:
//
//  SelecTrackCorr      - (default) selected Particle multiplicity normalized 
//                        by the runningmean (selTrack * referenceMean/<selTrack>)
//  TOFRPCtimecut       - TOF and RPC hit multiplicity in time-window
//  TOFtimecut          - TOF hit multiplicity in time-window
//  TOFRPC              - total TOF and RPC hit multiplicity in event-window
//  
//  
//
//--------------------------------------------------------------------------
// USAGE:
//
//  // 2 possible ways :
//  a. As Reconstructor task (to do)
//  b. With HLoop :
//
//     before eventLoop :
//       HParticleEvtChara evtChara;
//       evtChara.setBeamTime("apr12");
//       evtChara.setGeneration("gen8");
//       evtChara.useEstimator("SelcTrackCorr");
//       evtChara.init();
//       evtChara.printCentralityClass();
//
//     inside eventloop :
//       evtChara.execute();
//       Int_t   CentralityClassSelcTrackCorr   = evtChara.getCentralityClass5();
//       Int_t   CentralityClassTOFRPCtimecut   = evtChara.getCentralityClass5("TOFRPCtimecut");
//                5% Centrality-Classes:   1(0-5%) - 20(95-100%)     0:Overflow  21:Bad-CCentrality
//
//       Int_t   CentralityClass10SelcTrackCorr = evtChara.getCentralityClass10("SelcTrackCorr");
//       Int_t   CentralityClass10TOFRPCtimecut = evtChara.getCentralityClass10("TOFRPCtimecut");
//                10% Centrality-Classes:   1(0-10%) - 10(90-100%)   0:Overflow  11:Bad-Centrality
//
//       Float_t CentralitySelcTrackCorr        = evtChara.getCentralityPercentile("SelcTrackCorr");
//       Float_t CentralityTOFRPCtimecut        = evtChara.getCentralityPercentile("TOFRPCtimecut");
//       Float_t CentralityTOFtimecut           = evtChara.getCentralityPercentile("TOFtimecut");
//                CentralityPercentile:  0 - 100% in the resolution of the estimator
//                                       101%: Bad-Centrality
//
//
////////////////////////////////////////////////////////////////////////////
#include "hparticleevtchara.h"
ClassImp(HParticleEvtChara)

HParticleEvtChara::HParticleEvtChara(const Text_t* name,const Text_t* title)
   : HReconstructor(name,title) {
	fParticleEvtInfoCat   = 0;
	fParameterFile        = "centrality.root";
	fRun                  = "apr12";
	fGen                  = "gen8";
	fCentralityEstimator = "TOFRPCtimecut10";
	isSimulation          = kFALSE;
	fQuality              = 0;
	
	fFWSumChargeSpec      = 0;
        fDirectivity          = 0;
        fRatioEtEz            = 0;
        fEt                   = 0;

	fReferenceMeanSelTrack= 36.44;
	
        fHtempTOFtimecut      = 0x0;   // histogram with centrality vs multiplicity using TOF
        fHtempTOFRPCtimecut   = 0x0;   // histogram with centrality vs multiplicity using TOFRPCtimecut
        fHtempSelecTrackCorr  = 0x0;   // histogram with centrality vs multiplicity using SelectedTrackCorr
	fHtempSelecTrackNorm  = 0x0;
	fHtempFWSumChargeSpec = 0x0;
	fHtempDirectivity     = 0x0;
	fHtempRatioEtEz       = 0x0;
	fHtempEt              = 0x0;
	
	hMultTOFRPCtimecut_5percent_fixedCuts = 0x0;
	hMultTOFRPCtimecut_10percent_fixedCuts= 0x0;
	hMultTOFRPCtimecut_FOPI_fixedCuts     = 0x0;
	
}

HParticleEvtChara::~HParticleEvtChara() {
}

Bool_t HParticleEvtChara::init()
{
    // if you need to get parameter containers ... put it here
    if(gHades){
	HRecEvent* evt =(HRecEvent*) gHades->getCurrentEvent();
	if(evt){

	    HCategory* catKin=HCategoryManager::getCategory(catGeantKine,kFALSE,"catGeantKine");
	    if(catKin) {isSimulation=kTRUE; Info("init()","GeantKine found - is Simulation");}
	    else       {isSimulation=kFALSE;Info("init()","GeantKine not found - is not Simulation");}

	    fParticleEvtInfoCat =  HCategoryManager::getCategory(catParticleEvtInfo,kTRUE,"catParticleEvtInfo");
	    if(!fParticleEvtInfoCat) { Info("init()","No catParticleEvtInfo in input!"); return kFALSE;}

	    fCatWallHit = HCategoryManager::getCategory(catWallHit,kTRUE,"catWallHit");
	    if(!fCatWallHit) { Info("init()","No catWallHit in input!");return kFALSE;}
	} else {
	    Error("init()","NO EventStructure found!");
	    return kFALSE;
	}

    } else {
	Error("init()","NO HADES found!");
	return kFALSE;
    }
    // read parameter file
    if(fParameterFile){
	if(!loadParameterFile()) return kFALSE;
    } else {
	Error("init()","NO Parameterfile found!");
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HParticleEvtChara::setParameterFile(TString ParameterFile)
{
    fParameterFile = ParameterFile;
    TString path = gSystem->ExpandPathName(fParameterFile.Data());
    if (gSystem->AccessPathName(path)) {
	Error("init()","File  %s does not exist!",path.Data());
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HParticleEvtChara::loadParameterFile()
{
    // if you need to get parameter containers ... put it here

    // read parameter file
    TString path = gSystem->ExpandPathName(fParameterFile.Data());
    if (gSystem->AccessPathName(path)) {
	Error("loadParameterFile()","File  %s does not exist!",path.Data());
	return kFALSE;
    } else {
	Info("loadParameterFile()","---------------------------------------------------------");
	Info("loadParameterFile()",">>> Parameter input file   : %s",path.Data());
	Info("loadParameterFile()","---------------------------------------------------------");
	fFile  = new TFile(path,"OPEN");
	fFile->ls();

    }


    fHtempTOFtimecut = (TH1F*) (fFile->Get("hTOFtimecut_percentile"));
    if (!fHtempTOFtimecut) printf(Form("Calibration for TOF does not exist in %s \n", path.Data()));

    fHtempTOFRPCtimecut = (TH1F*) (fFile->Get("hTOFRPCtimecut_percentile"));
    if (!fHtempTOFRPCtimecut) printf(Form("Calibration for TOFRPCtimecut does not exist in %s \n", path.Data()));

    fHtempSelecTrackCorr = (TH1F*) (fFile->Get("hSelecTrackCorr_percentile"));
    if (!fHtempSelecTrackCorr) printf(Form("Calibration for SelecTrackCorr does not exist in %s \n", path.Data()));

    fHtempSelecTrackNorm = (TH1F*) (fFile->Get("hSelecTrackNorm_percentile"));
    if (!fHtempSelecTrackNorm) printf(Form("Calibration for SelecTrackNorm does not exist in %s \n", path.Data()));

    fHtempFWSumChargeSpec = (TH1F*) (fFile->Get("hFWSumChargeSpec_percentile"));
    if (!fHtempFWSumChargeSpec) printf(Form("Calibration for FWSumChargeSpec does not exist in %s \n", path.Data()));

    fHtempDirectivity = (TH1F*) (fFile->Get("hDirectivity_percentile"));
    if (!fHtempDirectivity) printf(Form("Calibration for Directivity does not exist in %s \n", path.Data()));

    fHtempRatioEtEz = (TH1F*) (fFile->Get("hRatioEtEz_percentile"));
    if (!fHtempRatioEtEz) printf(Form("Calibration for RatioEtEz does not exist in %s \n", path.Data()));

    fHtempEt = (TH1F*) (fFile->Get("hEt_percentile"));
    if (!fHtempEt) printf(Form("Calibration for Et does not exist in %s \n", path.Data()));

    hMultTOFRPCtimecut_5percent_fixedCuts = (TH1F*) (fFile->Get("hTOFRPCtimecut_5percent_fixedCuts"));
    if (!hMultTOFRPCtimecut_5percent_fixedCuts) printf(Form("Calibration for TOFRPCtimecut FixedCuts not exist in %s \n", path.Data()));

    hMultTOFRPCtimecut_10percent_fixedCuts = (TH1F*) (fFile->Get("hTOFRPCtimecut_10percent_fixedCuts"));
    if (!hMultTOFRPCtimecut_10percent_fixedCuts) printf(Form("Calibration for TOFRPCtimecut10 FixedCuts not exist in %s \n", path.Data()));

    hMultTOFRPCtimecut_FOPI_fixedCuts = (TH1F*) (fFile->Get("hTOFRPCtimecut_FOPI_fixedCuts"));
    if (!hMultTOFRPCtimecut_FOPI_fixedCuts) printf(Form("Calibration for TOFRPCtimecutFOPI FixedCuts not exist in %s \n", path.Data()));

    return kTRUE;
}

void HParticleEvtChara::setBeamTime(TString beamTime)
{
    beamTime.ToLower();
    if(beamTime.CompareTo("apr12")==0) {
	fRun = beamTime;
    } else {
	fRun = "";
	Error("setBeamTime()","unknown beam time %s!",beamTime.Data());
    }
}

void HParticleEvtChara::setGeneration(TString generation)
{
    generation.ToLower();
    if(generation.CompareTo("gen8")==0) {
	fGen = generation;
    } else {
	fRun = "";
	Error("setGeneration()","unknown generation Data set %s!",generation.Data());
    }
}


Int_t HParticleEvtChara::execute(){
    // get EventNumber ,
    calcFWSumChargeSpec();
    return 0;
}

Int_t HParticleEvtChara::print(){
    return 0;
}

void HParticleEvtChara::Reset()
{
    // Reset.
}


//----------------------------------------------------------------------
Int_t HParticleEvtChara::getCentralityClass() const
{
    return getCentralityClass(fCentralityEstimator);
}

Int_t HParticleEvtChara::getCentralityClass(TString estimator) const
{
    // Return centrality class, default in 5% of total cross section with estimator
    // or with preset classes like FOPI {6.3%, 21.0%, 30.9%}

    HParticleEvtInfo *event_info = (HParticleEvtInfo*)fParticleEvtInfoCat->getObject(0);
    if(!event_info) {
	Error("getCentralityClass()","No HParticleEvtInfo");
	return 0;
    }
    if (estimator.CompareTo("")==0) estimator = fCentralityEstimator;
    UInt_t buffer=0;


    if (estimator.CompareTo("SelcTrackCorr")==0) {
	buffer = event_info->getSumSelectedParticleCandMult()*(fReferenceMeanSelTrack/event_info->getMeanMult());
	// FIXME
    }
    else if (estimator.CompareTo("SelcTrackCorr10")==0) {
	buffer = event_info->getSumSelectedParticleCandMult()*(fReferenceMeanSelTrack/event_info->getMeanMult());
	// FIXME
    }
    else if (estimator.CompareTo("SelcTrackCorrFOPI")==0) {
	buffer = event_info->getSumSelectedParticleCandMult()*(fReferenceMeanSelTrack/event_info->getMeanMult());
	// FIXME
    }
    else if (estimator.CompareTo("TOFRPCtimecut")==0) {
	buffer =  event_info->getSumTofMultCut() + event_info->getSumRpcMultHitCut();
	if(hMultTOFRPCtimecut_5percent_fixedCuts) return (hMultTOFRPCtimecut_5percent_fixedCuts->GetBinContent(hMultTOFRPCtimecut_5percent_fixedCuts->FindBin((buffer))));
	else return 0;
    }
    else if (estimator.CompareTo("TOFRPCtimecut10")==0) {
	buffer =  event_info->getSumTofMultCut() + event_info->getSumRpcMultHitCut();
	if(hMultTOFRPCtimecut_10percent_fixedCuts) return (hMultTOFRPCtimecut_10percent_fixedCuts->GetBinContent(hMultTOFRPCtimecut_10percent_fixedCuts->FindBin((buffer))));
	else return 0;
    }
    else if (estimator.CompareTo("TOFRPCtimecutFOPI")==0) {
	buffer =  event_info->getSumTofMultCut() + event_info->getSumRpcMultHitCut();
	if(hMultTOFRPCtimecut_FOPI_fixedCuts) return (hMultTOFRPCtimecut_FOPI_fixedCuts->GetBinContent(hMultTOFRPCtimecut_FOPI_fixedCuts->FindBin((buffer))));
	else return 0;
    }
    else if (estimator.CompareTo("TOFtimecut")==0) {
	buffer =  event_info->getSumTofMultCut();
	// FIXME
    }
    else {
	Error("getCentralityClass()","No CentralityEstimator defined!");
	return 0;
    }

    return 0;
}
Int_t HParticleEvtChara::getCentralityClass(UInt_t buffer, TString estimator) const
{
    // Return centrality class, default in 5% of total cross section with estimator and provides Value
    // or with preset classes like FOPI {6.3%, 21.0%, 30.9%}

    if (estimator.CompareTo("Et")==0) {
	// FIXME
    }
    else {
	Error("getCentralityClass()","No CentralityEstimator defined!");
	return 0;
    }

    return 0;
}

void HParticleEvtChara::printCentralityClass(){
    return printCentralityClass(fCentralityEstimator);
}

void HParticleEvtChara::printCentralityClass(TString estimator)
{
    // print all CentralityClasses in the Estimator
    TH1F *htemp;
    if (estimator.CompareTo("TOFRPCtimecut")==0) htemp = (TH1F*) hMultTOFRPCtimecut_5percent_fixedCuts;
    else if (estimator.CompareTo("TOFRPCtimecutFOPI")==0) htemp = (TH1F*) hMultTOFRPCtimecut_FOPI_fixedCuts;
    else if (estimator.CompareTo("TOFRPCtimecut10")==0) htemp = (TH1F*) hMultTOFRPCtimecut_10percent_fixedCuts;
    else { Error("printCentralityClass()","Sorry. printCentralityClass() for %s not implemented yet!",estimator.Data()); return;}


    if(htemp){
	cout<<"\n--------------------------------------"  << endl;
	cout<<"Centrality Classes for "<< estimator <<"  # of Classes: "<< htemp->GetNbinsX()-2 <<  endl;
	printf(" Class \t  lowEdge -   upEdge \t Centrality[%%] \t BinWidth[%%]\n");
	for(Int_t i = htemp->GetNbinsX(); i>0 ; --i) {

	    printf("  %2.f :\t %8.2f - %8.2f \t %13s \t %11.3f \n",
		   htemp->GetBinContent(i),
		   (htemp->GetXaxis())->GetBinLowEdge(i),
		   (htemp->GetXaxis())->GetBinUpEdge(i),
		   (htemp->GetXaxis())->GetBinLabel(i),
		   htemp->GetBinError(i) );
	}
	cout<<"--------------------------------------"<<endl;
    }

}
Int_t HParticleEvtChara::getNbins()
{
    // Number of Bins (CentralityClasses and Over- and Underflow) in the Estimator
    return getNbins(fCentralityEstimator);
}
Int_t HParticleEvtChara::getNbins(TString estimator)
{
    // Number of Bins (CentralityClasses and Over- and Underflow) in the Estimator
    TH1F *htemp;
    if (estimator.CompareTo("TOFRPCtimecut")==0) htemp = (TH1F*) hMultTOFRPCtimecut_5percent_fixedCuts;
    else if (estimator.CompareTo("TOFRPCtimecutFOPI")==0) htemp = (TH1F*) hMultTOFRPCtimecut_FOPI_fixedCuts;
    else if (estimator.CompareTo("TOFRPCtimecut10")==0) htemp = (TH1F*) hMultTOFRPCtimecut_10percent_fixedCuts;
    else { Error("getNbins()","Sorry. getNumberCentralityClasses() for %s not implemented yet!",estimator.Data()); return 0;}

    if(htemp) return htemp->GetNbinsX();
    else return 0;
}
Int_t HParticleEvtChara::getCentralityClass5()
{
    // Return centrality class 5% of total cross section
    return getCentralityClass5(fCentralityEstimator);
}
Int_t HParticleEvtChara::getCentralityClass10()
{
    // Return centrality class 5% of total cross section
    return getCentralityClass10(fCentralityEstimator);
}
Int_t HParticleEvtChara::getCentralityClass5(TString estimator)
{
    // Return centrality class 5% of total cross section
    if (estimator.CompareTo("")==0) estimator = fCentralityEstimator;
    return 1+(Int_t)getCentralityPercentile(estimator)/5.;
}
Int_t HParticleEvtChara::getCentralityClass10(TString estimator)
{
    // Return centrality class 5% of total cross section
    if (estimator.CompareTo("")==0) estimator = fCentralityEstimator;
    return 1+(Int_t)getCentralityPercentile(estimator)/10.;
}

Float_t  HParticleEvtChara::getCentralityPercentile() const
{
    return getCentralityPercentile(fCentralityEstimator);
}

Float_t  HParticleEvtChara::getCentralityPercentile(TString estimator) const
{
    HParticleEvtInfo *event_info = (HParticleEvtInfo*)fParticleEvtInfoCat->getObject(0);
    if(!event_info) {
	Error("getCentralityPercentile()","No HParticleEvtInfo");
	return 0;
    }
    if (estimator.CompareTo("")==0) estimator = fCentralityEstimator;


    if (estimator.CompareTo("SelcTrackCorr")==0) {
	UInt_t mult = event_info->getSumSelectedParticleCandMult()*(fReferenceMeanSelTrack/event_info->getMeanMult());
	if(fHtempSelecTrackCorr) return (fHtempSelecTrackCorr->GetBinContent(fHtempSelecTrackCorr->FindBin((mult))));
	else return 101.;
    }
    else if (estimator.CompareTo("SelcTrackNorm")==0) {
	Float_t mult = event_info->getSumSelectedParticleCandMult()/event_info->getMeanMult();
	if(fHtempSelecTrackNorm) return (fHtempSelecTrackNorm->GetBinContent(fHtempSelecTrackNorm->FindBin((mult))));
	else return 101.;
    }
    else if (estimator.CompareTo("TOFtimecut")==0) {
	UInt_t mult  = event_info->getSumTofMultCut();
	if(fHtempTOFtimecut) return (fHtempTOFtimecut->GetBinContent(fHtempTOFtimecut->FindBin((mult))));
	else return 101.;
    }
    else if (estimator.CompareTo("TOFRPCtimecut")==0) {
	UInt_t mult  = event_info->getSumTofMultCut() + event_info->getSumRpcMultHitCut();
	if(fHtempTOFRPCtimecut) return (fHtempTOFRPCtimecut->GetBinContent(fHtempTOFRPCtimecut->FindBin((mult))));
	else return 101.;
    }
    else if (estimator.CompareTo("FWSumChargeSpec")==0) {
	UInt_t mult  = 10000 - fFWSumChargeSpec;
	if(fHtempFWSumChargeSpec) return (fHtempFWSumChargeSpec->GetBinContent(fHtempFWSumChargeSpec->FindBin((mult))));
	else return 101.;
    }
    else if (estimator.CompareTo("RatioEtEz")==0) {
	Float_t mult  = 2. - fRatioEtEz;
	if(fHtempRatioEtEz) return (fHtempRatioEtEz->GetBinContent(fHtempRatioEtEz->FindBin((mult))));
	else return 101.;
    }
    else if (estimator.CompareTo("Directivity")==0) {
	Float_t mult  = 1. - fDirectivity;
	if(fHtempDirectivity) return (fHtempDirectivity->GetBinContent(fHtempDirectivity->FindBin((mult))));
	else return 101.;
    }
    else if (estimator.CompareTo("Et")==0) {
	Float_t mult  = fEt;
	if(fHtempEt) return (fHtempEt->GetBinContent(fHtempEt->FindBin((mult))));
	else return 101.;
    }


    else if (estimator.CompareTo("STCTOFRPCtc")==0) {
	UInt_t mult = event_info->getSumSelectedParticleCandMult()*(fReferenceMeanSelTrack/event_info->getMeanMult());
	UInt_t mult2  = event_info->getSumTofMultCut() + event_info->getSumRpcMultHitCut();
	if(fHtempSelecTrackCorr && fHtempTOFRPCtimecut)
	    return ((fHtempSelecTrackCorr->GetBinContent(fHtempSelecTrackCorr->FindBin((mult)))) +
		    (fHtempTOFRPCtimecut->GetBinContent(fHtempTOFRPCtimecut->FindBin((mult2)))) )/2;
	else return 101.;
    }
    else {
	return 101.;
    }


}

Bool_t HParticleEvtChara::calcFWSumChargeSpec(){

    Float_t MultFWSumChargeSpec=0;
    Float_t betaFW=0;

    if(!isSimulation){
	//HWallHit
	if(!fCatWallHit) {Error("calcFWSumChargeSpec()","No fCatWallHit");return kFALSE;}
	HWallHit *wall = 0;

	for(Int_t i = 0; i < fCatWallHit->getEntries(); i ++ ){
	    wall  = (HWallHit*) HCategoryManager::getObject(wall,fCatWallHit,i);
	    if(!wall) {Error("calcFWSumChargeSpec()","No HWallHit");return kFALSE;}
	    betaFW =  wall->getDistance() / wall->getTime()/ 299.792458;

	    if(wall->getCell()<144&&wall->getCharge()>80){                               //small
		if(betaFW>0.84 && betaFW <1.0) MultFWSumChargeSpec += wall->getCharge();
	    }
	    else if(wall->getCell()>=144&&wall->getCell()<208&&wall->getCharge()>85){    //medium
		if(betaFW>0.85 && betaFW <1.0) MultFWSumChargeSpec += wall->getCharge();
	    }
	    else if(wall->getCell()>=208&&wall->getCharge()>86){                         //large
		if(betaFW>0.8 && betaFW <1.0) MultFWSumChargeSpec += wall->getCharge();
	    }
	}
	fFWSumChargeSpec = MultFWSumChargeSpec;
	return kTRUE;
    }
    else {
	return kFALSE;
    }
}
