//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : B. Kardan  9.07.2018
//*-- VERSION : 0.62
//
// HParticleEvtChara
//
// Purpose: EventCharacterization
// - Centrality from Hit(TOF,RPC,FW) and Track Estimators in Data/Sim
// - QVerctor and Phi (Reaction Plane Estimate) from FW
// - Event-weight for downscaled Events PT2
//
//--------------------------------------------------------------------------
// Usage:
//
//  - input files can be found at : example /cvmfs/hades.gsi.de/param/eventchara/
//
//  - to define the ParameterFile where Classes and Estimators are stored use
//    setParameterFile("/cvmfs/hades.gsi.de/param/eventchara/ParameterFile.root")
//    
//  - to print the definition of estimator & class use
//    printCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k10);
//  
//  - to get the CentralityClass of an event (with estimator and class definition) use 
//    getCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k10);
//
//  - to get the CentralityPercentile of an event (only estimator is needed) use 
//    getCentralityPercentile(HParticleEvtChara::kTOFRPC);
//
//  - to get the EventWeight to re-weight downscaled events use
//    getEventWeight();
//    
//  - to get the EventPlane with ReCentering use
//    getEventPlane(HParticleEvtChara::kReCentering);
//  
//--------------------------------------------------------------------------  
//   Estimators:
//   
//     TOFRPC                   - (default) TOF and RPC hit multiplicity in time-window
//     TOF                      - TOF hit multiplicity in time-window
//     RPC                      - RPC hit multiplicity in time-window
//     TOFRPCtot                - total TOF and RPC hit multiplicity in event-window
//     SelectedParticleCand     - selected Particle multiplicity
//     PrimaryParticleCand      - primary Particle multiplicity
//     SelectedParticleCandCorr - selected Particle multiplicity corrected by the 
//                                running mean and scaled with the reference mean 
//                                (selTrack * referenceMean/<selTrack>)
//     SelectedParticleCandSecCorr
//                               - selected Particle multiplicity corrected by the 
//                                running mean and scaled with the reference mean 
//                                (selTrack * referenceMean/<selTrack>)
//     SelectedParticleCandNorm - selected Particle multiplicity normalized by 
//                                the running mean (selTrack/<selTrack>)
//     FWSumChargeSpec          - sum of charge (dE/dx in a.u.) of Spectator in 
//                                FW acceptance with beta~0.9
//     FWSumChargeZ             - sum of charge (int Z till 6 with fixed cuts in dE/dx)
//                                of Spectator in FW acceptance with beta~0.9
//
//   Classes:
//     2                   - 2% classes
//     5                   - 5% classes
//     10                  - (default) 10% classes
//     13                  - 13% classes
//     15                  - 15% classes
//     20                  - 20% classes
//     FOPI                - FOPI centrality classes
//   
//   
//   EventPlaneCorrection:
//     kNoCorrection            - EP only selection of spectator candidates in FW,
//                                no Correction
//     kSmear                   - smearing of x and y of each FW hit inside cell size
//     kShiftFW                 - global shift x and y with the centre of gravity of
//                                all FW hits in all events (per class/day)
//     kScalingFW               - global scale x and yith the sigma of the distribution
//                                all FW hits in all events (per class/day)
//     kReCentering             - re-centering of QVector with <Qx> and <Qy> (calc. evt-by-evt)
//     kScaling                 - scaling of QVector with the sigma of Qx and Qy (calc. evt-by-evt)
//     kRotation                - rotation of EP via residual Fourier harmonics 
//                                after re-centering and scaling
//   
//--------------------------------------------------------------------------
// quick how-To:
//  - for an example see demo-macro:
//       /scripts/batch/GE/loopDST/loopDST.C
//
//  - include header-file
//      #include "hparticleevtchara.h"
//  
//  - before eventLoop:
//       HParticleEvtChara evtChara;
//           ParameterfileCVMFS = "/lustre/nyx/hades/user/bkardan/param/centrality_epcorr_apr12_gen8_2018_06.root";
//  
//       if(isSimulation)
//           ParameterfileCVMFS = "/lustre/nyx/hades/user/bkardan/param/centrality_sim_au1230au_gen8a_UrQMD_minbias_2018_06.root";
//    
//       if(!evtChara.setParameterFile(ParameterfileCVMFS)){
//           cout << "Parameterfile not found !!! " << endl;return kFALSE;
//       }
//  
//       if(!evtChara.init()) {cout << "HParticleEvtChara not init!!! " << endl;return kFALSE;}
//  
//       evtChara.printCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k5);
//       evtChara.printCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k10);
//  
//  - inside event-loop:
//  
//    Int_t   CentralityClassTOFRPC = evtChara.getCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k10);
//    10% Centrality-Classes:       1(0-10%) - 5(40-50%) ... 0:Overflow max:Underflow
//  
//    Float_t CentralityTOFRPC      = evtChara.getCentralityPercentile(HParticleEvtChara::kTOFRPC);
//    CentralityPercentile:         0 - 100% percentile of the total cross section
//                                      101% Over-,Underflow or Outlier Events
//  
//    Float_t EventPlane            = evtChara.getEventPlane(HParticleEvtChara::kReCentering|HParticleEvtChara::kScaling);
//    EventPlane:                   0 - 2pi (in rad)  re-centered & scaled EP
//                                 -1   no EP could be reconstructed
//
////////////////////////////////////////////////////////////////////////////

#include "hparticleevtchara.h"
ClassImp(HParticleEvtChara)

HParticleEvtChara::HParticleEvtChara(const Text_t* name,const Text_t* title)
   : HReconstructor(name,title) {
        fParticleEvtInfoCat   = 0;
        fParameterFile        = "centrality.root";
        fPostFix              = "";
        isSimulation          = kFALSE;
        fReferenceMeanSelTrack= 36.44;
        fDayOfYear            = 0;
        
        fRandom       = new TRandom2();

        fCentralityPercentileHist.resize(kNumCentralityEstimator);
        fCentralityHist.resize(kNumCentralityEstimator);
        fEstimatorHist.resize(kNumCentralityEstimator);
        for (int centEst = 0; centEst < (int)kNumCentralityEstimator; ++centEst) fCentralityHist[centEst].resize(kNumCentralityClass);
        fEventPlaneCorrectionHist.resize(kNumEventPlaneParameter);
        
        fFWminBeta.resize(3);
        fFWmaxBeta.resize(3);
        fFWminCharge.resize(3);
        fFWminBeta[0]=0.84;   fFWmaxBeta[0]=1.2;    fFWminCharge[0]=80; // small cells  
        fFWminBeta[1]=0.85;   fFWmaxBeta[1]=1.2;    fFWminCharge[1]=84; // medium cells
        fFWminBeta[2]=0.8;    fFWmaxBeta[2]=1.2;    fFWminCharge[2]=88; // large cells
        
        fFWChargeCuts.resize(6);
        fFWChargeCuts[5]=386;
        fFWChargeCuts[4]=339;
        fFWChargeCuts[3]=298;
        fFWChargeCuts[2]=241;
        fFWChargeCuts[1]=175;
        fFWChargeCuts[0]=84 ;
        
    }

HParticleEvtChara::~HParticleEvtChara() {
}

Bool_t HParticleEvtChara::init()
{
    if(gHades){
        HRecEvent* evt =(HRecEvent*) gHades->getCurrentEvent();
        if(evt){

            HCategory* catKin=HCategoryManager::getCategory(catGeantKine,kFALSE,"catGeantKine");
            if(catKin) {isSimulation=kTRUE; Info("init()","GeantKine found - is Simulation");}
            else       {isSimulation=kFALSE;Info("init()","GeantKine not found - is not Simulation");}

            fParticleEvtInfoCat =  HCategoryManager::getCategory(catParticleEvtInfo,kTRUE,"catParticleEvtInfo, from HParticleEvtChara::init()");
            if(!fParticleEvtInfoCat) { Info("init()","No catParticleEvtInfo in input!"); return kFALSE;}

            fParticleCandCat =  HCategoryManager::getCategory(catParticleCand,kTRUE,"catParticleCand, from HParticleEvtChara::init()");
            if(!fParticleCandCat) { Info("init()","No catParticleCand in input!");}

            fCatWallHit = HCategoryManager::getCategory(catWallHit,kTRUE,"catWallHit, from HParticleEvtChara::init()");
            if(!fCatWallHit) { Info("init()","No catWallHit in input!");}
            
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
    loadDayOfYear();
    
    return kTRUE;
}

Bool_t HParticleEvtChara::reinit()
{
    loadDayOfYear();
    return kTRUE;
}

Int_t HParticleEvtChara::loadDayOfYear()
{
    if(isSimulation){
        fDayOfYear = 0;
    }
    else{
        TString tempFileName = "";
        if(gLoop){
            gLoop->isNewFile(tempFileName);
        }
        else if(gHades){
            if(gHades->getRuntimeDb()){
                tempFileName = gHades->getRuntimeDb()->getCurrentFileName();
            }
            else if(gHades->getDataSource()){
                tempFileName = gHades->getDataSource()->getCurrentFileName();
            }
        }
        if(tempFileName==""){
            //Error("loadDayOfYear()","File/Day not found! using default day108");
            fDayOfYear = 108; // default FIXME
            return fDayOfYear;
        }
        fDayOfYear = HTime::getDayFileName(HTime::stripFileName(tempFileName,kTRUE,kFALSE));
        //Info("loadDayOfYear()",">>> Day of Year: %d",fDayOfYear);
    }
    return fDayOfYear;
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
    // read parameter file
    TString path = gSystem->ExpandPathName(fParameterFile.Data());
    if (gSystem->AccessPathName(path)) {
        Error("loadParameterFile()","File  %s does not exist!",path.Data());
        return kFALSE;
    } else {
        fFile  = new TFile(path,"OPEN");
        TObject* ParameterFileVersion = (TObject*) fFile->Get("HParticleEvtCharaVersion");
        if(ParameterFileVersion){
            TString Version(ParameterFileVersion->GetTitle());
            Float_t fVersion = Version.Atof();
            if(fVersion < getVersion()){
                Error("loadParameterFile()","File  %s is out-dated! Needed Version: %02.1f -- in file: %02.1f",path.Data(), getVersion(), fVersion);
                return kFALSE;
            }
            cout<<"\n--------------------------------------------------------------------------------------"  << endl;
            Info("loadParameterFile()",">>> Parameter input file (ver. %02.1f) : %s",fVersion,path.Data());
            
            cout<<"\n--------------------------------------------------------------------------------------"  << endl;
        }
        else{
            Error("loadParameterFile()","In File  %s no Version information found!",path.Data());
            return kFALSE;
        }
    }
    loadCentralityEstimatorHist();
    loadEventPlaneCorrectionHist();
    return kTRUE;
}

Bool_t HParticleEvtChara::saveParameterFile()
{
    // read parameter file
    TString path = gSystem->ExpandPathName(fParameterFile.Data());
    //if (gSystem->AccessPathName(path)) {
    //    Error("loadParameterFile()","File  %s does not exist!",path.Data());
    //    return kFALSE;
    //} else {
    fFile  = new TFile(path,"CREATE");
    if(!fFile) return kFALSE;
    cout << "Version of HParticleEvtChara:  " << getVersion() << endl;
    TString version=Form("%02.1f",getVersion());
    TNamed OutputVersion("HParticleEvtCharaVersion",version.Data()); 
    OutputVersion.Write();
    saveCentralityEstimatorHist();
    saveEventPlaneCorrectionHist();
    return kTRUE;
}

Bool_t HParticleEvtChara::loadCentralityEstimatorHist()
{

    cout<<"\n--------------------------------------------------------------------------------------"  << endl;
    Info("loadCentralityEstimatorHist()","Calibration for Centrality Estimator and Classes loading:");
    Int_t n=0,m=0;
    for (int centEst = 0; centEst < (int) kNumCentralityEstimator; ++centEst){
       printf("\n>>>> %27s %4s :  ",getStringCentralityEstimator(centEst).Data() ,fPostFix.Data());
       TString temp;
       if(fPostFix.CompareTo("")==0) temp = Form("%s_percentile",getStringCentralityEstimator(centEst).Data() );
       else                       temp = Form("%s_%s_percentile",getStringCentralityEstimator(centEst).Data(), fPostFix.Data() );
       fCentralityPercentileHist[centEst] = (TH1F*) fFile->FindObjectAny( temp.Data() );
       if(fCentralityPercentileHist[centEst]){
            printf("percentile");
            m++;
        }
       
       for (int centC = 0; centC < (int) kNumCentralityClass; ++centC){
           TString temp2;
           if(fPostFix.CompareTo("")==0) temp2 = Form("%s_%s_fixedCuts",getStringCentralityEstimator(centEst).Data(), getStringCentralityClass(centC).Data() );
           else                       temp2 = Form("%s_%s_%s_fixedCuts",getStringCentralityEstimator(centEst).Data(), fPostFix.Data(), getStringCentralityClass(centC).Data() );
           fCentralityHist[centEst][centC] = (TH1F*) fFile->FindObjectAny( temp2.Data() );
           if(fCentralityHist[centEst][centC]){
                printf("  %8s",getStringCentralityClass(centC).Data() );
                n++;
           }
       }
   }
   cout<< "\n\nCentrality Percentile Curves      #"<< m << endl;
   cout<< "Centrality Estimator and Classes  #"<< n << endl;
   cout<< "--------------------------------------------------------------------------------------"  << endl;

   return 0;
}

Bool_t HParticleEvtChara::saveCentralityEstimatorHist()
{
    if(!fFile) return kFALSE;
    fFile->mkdir("EstimatorHist");
    fFile->cd("/EstimatorHist/");

    cout<<"\n--------------------------------------------------------------------------------------"  << endl;
    Info("saveCentralityEstimatorHist()","Estimator Hist saving:");
    Int_t m=0;
    for (int centEst = 0; centEst < (int) kNumCentralityEstimator; ++centEst){
       if(fEstimatorHist[centEst]){
            fEstimatorHist[centEst]->Write();
            m++;
        }
   }
   cout<< "\n\nEstimator Hist      #"<< m << "saved" << endl;
   cout<< "--------------------------------------------------------------------------------------"  << endl;

    fFile->mkdir("Centrality");
    fFile->cd("/Centrality/");
    
    cout<<"\n--------------------------------------------------------------------------------------"  << endl;
    Info("saveCentralityEstimatorHist()","Calibration for Centrality Estimator and Classes saving:");
    Int_t n=0;
    m=0;
    for (int centEst = 0; centEst < (int) kNumCentralityEstimator; ++centEst){
       printf("\n>>>> %27s %4s :  ",getStringCentralityEstimator(centEst).Data() ,fPostFix.Data());
       if(fCentralityPercentileHist[centEst]){
            fCentralityPercentileHist[centEst]->Write();
            printf("percentile");
            m++;
        }
       
       for (int centC = 0; centC < (int) kNumCentralityClass; ++centC){
           if(fCentralityHist[centEst][centC]){
                fCentralityHist[centEst][centC]->Write();
                printf("  %8s",getStringCentralityClass(centC).Data() );
                n++;
           }
       }
   }
   cout<< "\n\nCentrality Percentile Curves      #"<< m << "saved" << endl;
   cout<< "Centrality Estimator and Classes  #"<< n << "saved" << endl;
   cout<< "--------------------------------------------------------------------------------------"  << endl;
   return 0;
}


Bool_t HParticleEvtChara::loadEventPlaneCorrectionHist()
{

    cout<<"\n--------------------------------------------------------------------------------------"  << endl;
    Info("loadEventPlaneCorrectionHist()","Calibration for EventPlane Correction Histogramms loading:");
    Int_t n=0;
    for (int epParam = 0; epParam < (int) kNumEventPlaneParameter; ++epParam){
       printf("\n>>>> %27s :  ",getStringEventPlaneParameter(epParam).Data() );
       TString temp;
       temp = Form("EPcorr_%s_Day_Centrality",getStringEventPlaneParameter(epParam).Data() );
       fEventPlaneCorrectionHist[epParam] = (TProfile2D*) fFile->FindObjectAny( temp.Data() );   //FIXME
       if(fEventPlaneCorrectionHist[epParam]){
            //for (int dim = 0; dim < fEventPlaneCorrectionHist[qAxis]->GetDimension(); ++dim){
                printf("%4s(%2d bins) ",fEventPlaneCorrectionHist[epParam]->GetXaxis()->GetTitle(),
                                  fEventPlaneCorrectionHist[epParam]->GetNbinsX());
                printf("%4s(%2d bins) ",fEventPlaneCorrectionHist[epParam]->GetYaxis()->GetTitle(),
                                  fEventPlaneCorrectionHist[epParam]->GetNbinsY());
                printf("%4s(%2d bins) ",fEventPlaneCorrectionHist[epParam]->GetZaxis()->GetTitle(),
                                  fEventPlaneCorrectionHist[epParam]->GetNbinsZ());
            n++;
        }
   }

   cout<< "\n\nEventPlane Correction Histogramms  #"<< n << endl;
   cout<< "--------------------------------------------------------------------------------------"  << endl;

   return 0;
}
Bool_t HParticleEvtChara::saveEventPlaneCorrectionHist()
{
    if(!fFile) return kFALSE;
    fFile->mkdir("EPcorr");
    fFile->cd("/EPcorr/");
    
    cout<<"\n--------------------------------------------------------------------------------------"  << endl;
    Info("saveEventPlaneCorrectionHist()","Calibration for EventPlane Correction Histogramms saving:");
    Int_t n=0;
    for (int epParam = 0; epParam < (int) kNumEventPlaneParameter; ++epParam){
       printf("\n>>>> %27s :  ",getStringEventPlaneParameter(epParam).Data() );
       if(fEventPlaneCorrectionHist[epParam]){
           fEventPlaneCorrectionHist[epParam]->Write();
           printf("%4s(%2d bins) ",fEventPlaneCorrectionHist[epParam]->GetXaxis()->GetTitle(),
                             fEventPlaneCorrectionHist[epParam]->GetNbinsX());
           printf("%4s(%2d bins) ",fEventPlaneCorrectionHist[epParam]->GetYaxis()->GetTitle(),
                             fEventPlaneCorrectionHist[epParam]->GetNbinsY());
           printf("%4s(%2d bins) ",fEventPlaneCorrectionHist[epParam]->GetZaxis()->GetTitle(),
                             fEventPlaneCorrectionHist[epParam]->GetNbinsZ());
            n++;
        }
   }

   cout<< "\n\nEventPlane Correction Histogramms  #"<< n << "saved" << endl;
   cout<< "--------------------------------------------------------------------------------------"  << endl;

   return 0;
}



Int_t HParticleEvtChara::execute(){
    return 0;
}

Int_t HParticleEvtChara::print(){
    return 0;
}

void HParticleEvtChara::reset()
{
    // Reset.
}
//----------------------------------------------------------------------
Bool_t HParticleEvtChara::isNewEvent()
{
    if(gHades->getCurrentEvent()->getHeader()->getEventSeqNumber() == currentEventSeqNumber){
        return kFALSE;
    } 
    else{
        currentEventSeqNumber = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
        return kTRUE;
    }
}

Float_t HParticleEvtChara::getEventWeight()
{
    if(isSimulation) return 1;
// ---- PT2 Event without PT3 Events ----------
    if(gHades->getCurrentEvent()->getHeader()->getTBit() == 4096) return 8;
// ---- PT3 Event without PT2 Events --------------
    if(gHades->getCurrentEvent()->getHeader()->getTBit() == 8192) return 1;
// ---- PT2 Event with PT3 Events --------------
    if(gHades->getCurrentEvent()->getHeader()->getTBit() == 12288)return 1;
    return 0;
}


//----------------------------------------------------------------------

Int_t HParticleEvtChara::GetFWmoduleSize(HWallHitSim* wall)
{
    Int_t i = wall->getCell();
    
    if ( i < 0 || i > 301 ) return -1;
    else if ( i==65 || i==66 || i==77 || i==78 ) return -1;      //beam hole
    else if ( i<144 ) return 1;    // small modules
    else if ( i<208 ) return 2;    // middle modules
    else if ( i<302 ) return 3;    // large modules
    return -1;
}

Bool_t HParticleEvtChara::PassesCutsFW(HWallHitSim* wall)
{
    if(!wall) return kFALSE;
    Float_t betaFW  =  wall->getDistance() / wall->getTime()/ 299.792458;
    
     if(!isSimulation){
         if( (GetFWmoduleSize(wall) == 1
              &&  wall->getCharge()>fFWminCharge[0]&&betaFW>fFWminBeta[0] && betaFW <fFWmaxBeta[0] ) //small
    	  || (GetFWmoduleSize(wall) == 2
              &&  wall->getCharge()>fFWminCharge[1]&&betaFW>fFWminBeta[1] && betaFW <fFWmaxBeta[1] ) //medium
    	  || (GetFWmoduleSize(wall) == 3
              &&  wall->getCharge()>fFWminCharge[2]&&betaFW>fFWminBeta[2] && betaFW <fFWmaxBeta[2] )  //large
              ) return kTRUE;	  
     }
     else
     {
         if( (GetFWmoduleSize(wall) == 1  
              &&  betaFW>fFWminBeta[0] && betaFW <fFWmaxBeta[0] ) //small
          || (GetFWmoduleSize(wall) == 2  
              &&  betaFW>fFWminBeta[1] && betaFW <fFWmaxBeta[1] ) //medium
          || (GetFWmoduleSize(wall) == 3  
              &&  betaFW>fFWminBeta[2] && betaFW <fFWmaxBeta[2] )  //large
              ) return kTRUE;
     }
     return kFALSE;
}

Int_t HParticleEvtChara::getFWCharge(HWallHitSim* wall)
{
    Float_t Charge = 0;
    if(!wall) return kFALSE;
    if(!isSimulation) Charge = wall->getCharge();
    else              Charge = 93.*pow(wall->getCharge(),0.46-0.006*sqrt(wall->getCharge()));  // parametrization from R.Holzmann

    if(Charge>=fFWChargeCuts[5])       return 6;
    else if(Charge>=fFWChargeCuts[4])  return 5;
    else if(Charge>=fFWChargeCuts[3])  return 4;
    else if(Charge>=fFWChargeCuts[2])  return 3;
    else if(Charge>=fFWChargeCuts[1])  return 2;
    else if(Charge>=fFWChargeCuts[0])  return 1;
    else return 0;
    
}

Float_t HParticleEvtChara::getCorrection(UInt_t flag)
{
    Float_t fCorrection = 0.;
    Double_t cent = getCentralityPercentile();
    if(fEventPlaneCorrectionHist[flag]) 
        fCorrection = fEventPlaneCorrectionHist[flag]->GetBinContent(fEventPlaneCorrectionHist[flag]->FindBin(cent, fDayOfYear) );
    return fCorrection;
}

Float_t HParticleEvtChara::getCorrectionError(UInt_t flag)
{
    Float_t fCorrectionError = 1.;
    Double_t cent = getCentralityPercentile();
    if(fEventPlaneCorrectionHist[flag]) 
        fCorrectionError = fEventPlaneCorrectionHist[flag]->GetBinError(fEventPlaneCorrectionHist[flag]->FindBin(cent, fDayOfYear) );
    return fCorrectionError;
}

Float_t HParticleEvtChara::getSmearValue(Int_t size){
    if(size==1) return ( fRandom->Rndm(1) - 0.5 )*40.;
    else if(size==2) return ( fRandom->Rndm(1) - 0.5 )*80.;
    else if(size==3) return ( fRandom->Rndm(1) - 0.5 )*160.;
    return 0;
}

Float_t HParticleEvtChara::getThetaWeight(HWallHitSim* wall, Float_t min, Float_t max){
     Float_t theta = wall->getTheta();
     if(theta>max) return 0.;    // max Theta 
     if(theta<min) return 1.;    // min Theta
     Float_t wtheta = 1. -( (theta-min)/(max-min) );   
     if(wtheta>0 && wtheta < 1.) return wtheta;
     else return 0.;
}

void HParticleEvtChara::filterFWhits()
{
    //  -----------------------------------------------------------------
    //HWallHit  into QVector
    iFWHitvector.clear();
    if(!fCatWallHit) return;
    HWallHitSim *wall = 0;  // includes HWallHit
    for(Int_t i = 0; i < fCatWallHit->getEntries(); i ++ ){
        wall  = (HWallHitSim*) HCategoryManager::getObject(wall,fCatWallHit,i);
        if(!PassesCutsFW(wall)) continue;
        iFWHitvector.push_back(i);
    } 
    //shuffle
    std::random_shuffle ( iFWHitvector.begin(), iFWHitvector.end() );
}

vector<Int_t> HParticleEvtChara::getFWhits(){
    if(isNewEvent()) filterFWhits();
    return iFWHitvector;
}

TVector2 HParticleEvtChara::getQVector(UInt_t statusflag, UInt_t SubEvent, UInt_t nHarmonic)
{
    //  -----------------------------------------------------------------
    //HWallHit  into QVector
    TVector2 vQ(0.,0.);
    if(isNewEvent()) filterFWhits();
    if(iFWHitvector.size()<1) return vQ;
    if(!fCatWallHit) return vQ;

    Double_t dQX = 0.;
    Double_t dQY = 0.;
    Double_t sumOfWeights=0;

    std::size_t start = 0;
    std::size_t stop  = iFWHitvector.size();
    if(SubEvent==1) stop = iFWHitvector.size()/2; 
    else if(SubEvent==2) start = iFWHitvector.size()/2;

    HWallHitSim *wall = 0;  // includes HWallHit
    Float_t wallX=0,wallY=0,wallZ=0;
    Float_t weight = 1;
    TVector2 vectFW(0.,0.);
    
    for ( std::vector<int>::iterator it=iFWHitvector.begin() + start ; it<iFWHitvector.begin() + stop ; ++it){
          wall  = (HWallHitSim*) HCategoryManager::getObject(wall,fCatWallHit,*it);
          if(!wall) continue;
          weight = 1;
          wall->getXYZLab(wallX,wallY,wallZ);
          if(isFlagSet(kSmear, statusflag)){
                Int_t size = GetFWmoduleSize(wall);
                wallX += getSmearValue(size);
                wallY += getSmearValue(size);
          }
          if(isFlagSet(kShiftFW, statusflag)){
              wallX -= getCorrection(kFWx);
              wallY -= getCorrection(kFWy);
          }
          if(isFlagSet(kScalingFW, statusflag)){
              wallX /= getCorrectionError(kFWx);
              wallY /= getCorrectionError(kFWy);
          }
          if(isFlagSet(kWeightCharge, statusflag)){
              weight *= getFWCharge(wall);
          }
          if(isFlagSet(kWeightTheta, statusflag)){
              weight *= getThetaWeight(wall);
          }

          if(isFlagSet(kUsePostionFWHit, statusflag)){
              dQX += weight*wallX;
              dQY += weight*wallY;
              sumOfWeights +=weight;
          }
          else{  // Use only Angle of FW hit(default)
              vectFW.Set(wallX, wallY);
              // building up the Q-vector with normalized vectors:
              dQX += weight*TMath::Cos(nHarmonic*vectFW.Phi());
              dQY += weight*TMath::Sin(nHarmonic*vectFW.Phi());
              sumOfWeights +=weight;
          }
    } // loop over all selected FW hits

    if(!isFlagSet(kNoNormalize, statusflag) && sumOfWeights>0){
        dQX /= sumOfWeights;
        dQY /= sumOfWeights;
    }
    if(isFlagSet(kReCentering, statusflag)){
        dQX -= getCorrection(kQx);
        dQY -= getCorrection(kQy);
    }
    if(isFlagSet(kScaling, statusflag)){
        dQX /= getCorrectionError(kQx);
        dQY /= getCorrectionError(kQy);
    }
    vQ.Set(dQX,dQY);
    if(isFlagSet(kRotation, statusflag)){
        vQ.Rotate(getCorrectionPhi(kReCentering));
    }
    return vQ;
}

Float_t HParticleEvtChara::getCorrectionPhi(UInt_t flag){
    
    //used Formular in Footnoot1 of Phys.Rev. C58 (1998) 1671-1678
    //also appendix of Phys.Rev. C56 (1997) 3254-3264
    
    TVector2 qVector = getQVector(flag);
    if(qVector.X()==0 && qVector.Y()==0) return 0;
    Float_t Psi = qVector.Phi();
        
    Float_t corrPsi = 0;
    corrPsi =    2.*   (getCorrection(kFourierC1)*TMath::Sin(1*Psi)) 
               + 1.*   (getCorrection(kFourierC2)*TMath::Sin(2*Psi)) 
               + 2./3.*(getCorrection(kFourierC3)*TMath::Sin(3*Psi)) 
               + 1./2.*(getCorrection(kFourierC4)*TMath::Sin(4*Psi))
               - 2.*   (getCorrection(kFourierS1)*TMath::Cos(1*Psi))
               - 1.*   (getCorrection(kFourierS2)*TMath::Cos(2*Psi))
               - 2./3.*(getCorrection(kFourierS3)*TMath::Cos(3*Psi))
               - 1./2.*(getCorrection(kFourierS4)*TMath::Cos(4*Psi));
    return corrPsi;
}

Float_t HParticleEvtChara::getEventPlane(UInt_t statusflag, UInt_t SubEvent, UInt_t nHarmonic)
{
    TVector2 qVector = getQVector(statusflag, SubEvent, nHarmonic);
    if(qVector.X()==0 && qVector.Y()==0) return -1;
    else return qVector.Phi();
}

Float_t HParticleEvtChara::getQVectorMagnitude(UInt_t statusflag, UInt_t SubEvent, UInt_t nHarmonic){

    TVector2 qVector = getQVector(statusflag, SubEvent, nHarmonic);
    if(qVector.X()==0 && qVector.Y()==0) return -1;
    else return qVector.Mod();
}

Float_t HParticleEvtChara::getEventPlaneWeight(UInt_t statusflag, UInt_t SubEvent, UInt_t nHarmonic)
{
    return 1.;
}

Float_t HParticleEvtChara::getEventPlaneParameter(UInt_t e, UInt_t EPcorr)
{
         if(getQVector().X()==0 && getQVector().Y()==0) return -999;

         if(e==kFWx)        return -999;  // can not be filled evt-by-evt
    else if(e==kFWy)        return -999;  
    else if(e==kQx)         return getQVector(EPcorr|kShiftFW|kScalingFW).X();
    else if(e==kQy)         return getQVector(EPcorr|kShiftFW|kScalingFW).Y();
    else if(e==kFourierC1)  return TMath::Cos(1*getQVector(EPcorr|kShiftFW|kScalingFW).Phi());
    else if(e==kFourierC2)  return TMath::Cos(2*getQVector(EPcorr|kShiftFW|kScalingFW).Phi());
    else if(e==kFourierC3)  return TMath::Cos(3*getQVector(EPcorr|kShiftFW|kScalingFW).Phi());
    else if(e==kFourierC4)  return TMath::Cos(4*getQVector(EPcorr|kShiftFW|kScalingFW).Phi());
    else if(e==kFourierS1)  return TMath::Sin(1*getQVector(EPcorr|kShiftFW|kScalingFW).Phi());
    else if(e==kFourierS2)  return TMath::Sin(2*getQVector(EPcorr|kShiftFW|kScalingFW).Phi());
    else if(e==kFourierS3)  return TMath::Sin(3*getQVector(EPcorr|kShiftFW|kScalingFW).Phi());
    else if(e==kFourierS4)  return TMath::Sin(4*getQVector(EPcorr|kShiftFW|kScalingFW).Phi());

    if(e==kResolution || e==kResolution2){
        if(iFWHitvector.size()<3) return -999;
        Int_t nHarmonic = 1;
        if(e==kResolution2) nHarmonic =2;
        TVector2 qVectorA = getQVector(EPcorr|kShiftFW|kScalingFW, 1, nHarmonic).Unit();
        TVector2 qVectorB = getQVector(EPcorr|kShiftFW|kScalingFW, 2, nHarmonic).Unit();
        if(qVectorA.X()==0 && qVectorA.Y()==0 && qVectorB.X()==0 && qVectorB.Y()==0) return -999;
        return qVectorA*qVectorB;
    }
    else return -999;
}


//----------------------------------------------------------------------
Int_t HParticleEvtChara::getCentralityClass(TString estimator)
{
    // legacy code
    // Return centrality class, default in 5% of total cross section with estimator
    // or with preset classes like FOPI {6.3%, 21.0%, 30.9%}

    if (estimator.CompareTo("TOFRPCtimecut")==0 || estimator.CompareTo("TOFRPC5")==0) 
        return getCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k5);
    else if (estimator.CompareTo("TOFRPCtimecutFOPI")==0 || estimator.CompareTo("TOFRPCFOPI")==0) 
        return getCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::kFOPI);
    else if (estimator.CompareTo("TOFRPCtimecut10")==0 || estimator.CompareTo("TOFRPC10")==0) 
        return getCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k10);
    else {
        Error("getCentralityClass()","No CentralityEstimator defined!");
        return 0;
    }
}

Float_t HParticleEvtChara::getCentralityEstimator(UInt_t centE)
{
    HParticleEvtInfo *event_info = (HParticleEvtInfo*)fParticleEvtInfoCat->getObject(0);
    if(!event_info) {
        Error("getCentralityClass()","No HParticleEvtInfo");
    return 0;
    }
    
    if(centE     ==kTOFRPC)                  return event_info->getSumTofMultCut() + event_info->getSumRpcMultHitCut();
    else if(centE==kTOF)                     return event_info->getSumTofMultCut();
    else if(centE==kRPC)                     return event_info->getSumRpcMultHitCut();
    else if(centE==kTOFRPCtot)               return event_info->getSumTofMult() + event_info->getSumRpcMultHit();
    else if(centE==kTOFtot)                  return event_info->getSumTofMult();
    else if(centE==kRPCtot)                  return event_info->getSumRpcMultHit();
    else if(centE==kSelectedParticleCand)    return event_info->getSumSelectedParticleCandMult();
    else if(centE==kSelectedParticleCandCorr) return event_info->getSumSelectedParticleCandMult()*(fReferenceMeanSelTrack/event_info->getMeanMult());    
    else if(centE==kSelectedParticleCandNorm) return event_info->getSumSelectedParticleCandMult()/event_info->getMeanMult();   
    else if(centE==kSelectedParticleCandSecNorm) return getSelectedParticleCandSecNorm();    
    else if(centE==kPrimaryParticleCand)     return event_info->getSumPrimaryParticleCandMult();
    else if(centE==kMdcWires)                return event_info->getMdcWires();
    else if(centE==kFWSumChargeSpec)         return getFWSumChargeSpec();
    else if(centE==kFWSumChargeZ)            return getFWSumZ(6);
    else if(centE==kDirectivity)             return getDirectivity();
    else if(centE==kRatioEtEz)               return getRatioEtEz();
    else if(centE==kEt)                      return getEt();
    else return 0;
}

TH1F* HParticleEvtChara::getCentralityClassHist(UInt_t centE, UInt_t centC) const
{
    if(centE<kNumCentralityEstimator && centC<kNumCentralityClass) return (TH1F*)fCentralityHist[centE][centC];
    else return 0;
}

TH1F* HParticleEvtChara::getCentralityPercentileHist(UInt_t centE) const
{
    if(centE<kNumCentralityEstimator) return (TH1F*)fCentralityPercentileHist[centE];
    else return 0;
}

Int_t HParticleEvtChara::getCentralityClass(UInt_t centE, UInt_t centC)
{
    // Return centrality class, default in 5% of total cross section with estimator
    // or with preset classes like FOPI {6.3%, 21.0%, 30.9%}
    if(centE>=kNumCentralityEstimator){
        return 101.;
    }
    else if(!getCentralityClassHist(centE, centC)){
        //Error("getCentralityClass()","No CentralityEstimator defined!");
        return 0;
    }
    else {
        Int_t buffer = getCentralityEstimator(centE);
        Int_t bin = getCentralityClassHist(centE, centC)->FindBin(buffer);
        return getCentralityClassHist(centE, centC)->GetBinContent(bin);
    }
    return 0;
    
}

Bool_t HParticleEvtChara::printCentralityClass(TString estimator)
{
    // legacy code
    // print all CentralityClasses in the Estimator

    if (estimator.CompareTo("TOFRPCtimecut")==0 || estimator.CompareTo("TOFRPC5")==0) 
        return printCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k5);
    else if (estimator.CompareTo("TOFRPCtimecutFOPI")==0 || estimator.CompareTo("TOFRPCFOPI")==0) 
        return printCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::kFOPI);
    else if (estimator.CompareTo("TOFRPCtimecut10")==0 || estimator.CompareTo("TOFRPC10")==0) 
        return printCentralityClass(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k10);
    else {
         Error("printCentralityClass()","Sorry. printCentralityClass() for %s not implemented yet!",estimator.Data());
         return kFALSE;
     }

    return kFALSE;
}

Bool_t HParticleEvtChara::printCentralityClass(UInt_t centE, UInt_t centC)
{
    // print all CentralityClasses in the Estimator
    cout<<"\n--------------------------------------------------------------------------------------"  << endl;
    cout<<"Centrality Classes for "<< getStringCentralityEstimator(centE).Data() << "  with " << getStringCentralityClass(centC).Data()  <<" bins" << endl;
    return printCentralityClass(getCentralityClassHist(centE, centC)); 
}

Bool_t HParticleEvtChara::printCentralityClass(TH1* htemp)
{
    // print all CentralityClasses in the Estimator
    if(!htemp){
        Error("printCentralityClass()","Sorry. printCentralityClass() for ... not implemented yet!");
        return kFALSE;
    }
    else{
        cout<<"\n--------------------------------------------------------------------------------------"  << endl;
        cout<< "# of Classes: "<< htemp->GetNbinsX()-2 << endl;
        printf(" Class \t  lowEdge -   upEdge \t Centrality[%%] \t BinWidth[%%] \treal CentralityBin[%%] \tBinCenter[%%]\n");
        Float_t pcent = 0;
        for(Int_t i = htemp->GetNbinsX(); i>0 ; --i) {
            printf("  %2.f :\t %8.2f - %8.2f \t %13s \t %11.3f \t %8.2f - %8.2f \t %8.2f\n",
                      htemp->GetBinContent(i),
                      (htemp->GetXaxis())->GetBinLowEdge(i),
                      (htemp->GetXaxis())->GetBinUpEdge(i),
                      (htemp->GetXaxis())->GetBinLabel(i),
                      htemp->GetBinError(i),
                      pcent,
                      pcent+htemp->GetBinError(i),
                      pcent+0.5*htemp->GetBinError(i) );
            pcent += htemp->GetBinError(i);
        }
        cout<<"--------------------------------------------------------------------------------------"  << endl;
        return kTRUE;
    }
    return kFALSE;
}

Bool_t HParticleEvtChara::addEventPlaneCorrectionHist(TProfile2D *hist, UInt_t epParam)
{
    if(!hist)return kFALSE;
    if(epParam>=kNumEventPlaneParameter) return kFALSE;
    if(epParam==kChi)  return kTRUE;
    
    TString temp;
    temp = Form("EPcorr_%s_Day_Centrality",getStringEventPlaneParameter(epParam).Data() );
    hist->SetNameTitle(temp.Data(),temp.Data());

    if(epParam==kResolution){
         fEventPlaneCorrectionHist[kResolution] = (TH2D*) makeEPresolution(hist);

         temp = Form("EPcorr_%s_Day_Centrality",getStringEventPlaneParameter(kChi).Data() );
         hist->SetNameTitle(temp.Data(),temp.Data());
         fEventPlaneCorrectionHist[kChi] = (TH2D*) makeEPresolution(hist, kTRUE);
    }
    else{
         fEventPlaneCorrectionHist[epParam] = (TProfile2D*) hist;
    }
    return kTRUE;
}

TH2D* HParticleEvtChara::makeEPresolution(TProfile2D *hist, Bool_t calcChi)
{
    if(!hist) return 0;
    TString sName=hist->GetName();   
    sName.Append("_res");
    
    Int_t nxBins = hist->GetNbinsX();
    Int_t nyBins = hist->GetNbinsY();
    
    TH2D* hresolution  = (TH2D*) new TH2D(sName.Data(), sName.Data(), nxBins, hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax(),
                                                                      nyBins, hist->GetYaxis()->GetXmin(), hist->GetYaxis()->GetXmax());
    (hresolution->GetXaxis())->SetTitle(hist->GetXaxis()->GetTitle());
    (hresolution->GetYaxis())->SetTitle(hist->GetYaxis()->GetTitle());
    (hresolution->GetZaxis())->SetTitle(hist->GetZaxis()->GetTitle());

    Int_t bin,binx,biny;
    for (binx =0;binx<=nxBins+1;binx++) {
       for (biny =0;biny<=nyBins+1;biny++) {
           bin   = biny*(nxBins+2) + binx;
           
           Double_t dQaQb        = hist->GetBinContent(bin);
           Double_t dEntriesQaQb = hist->GetBinEntries(bin);
           if( dQaQb <= 0 || dEntriesQaQb < 1 ){
               hresolution->SetBinContent(bin, 0. );
               hresolution->SetBinError(  bin, 0. );
                continue;
           }
           Double_t dSpreadQaQb  = hist->GetBinError(bin);
           Double_t dV = TMath::Sqrt(dQaQb);
           printf("\nbin=%d binX=%d binY=%d QaQb = %f  +- %f    V = %f\n",bin, binx, biny, dQaQb, dSpreadQaQb , dV);
           Double_t dChi = FindXi(dV,1e-6);
           dV = ComputeResolution( TMath::Sqrt2()*dChi );
           printf("An estimate of the event plane resolution is: %f\n", dV );
           Double_t dVerr = 0.;
           if(dQaQb > 0.) dVerr = (1./(2.*pow(dQaQb,0.5)))*dSpreadQaQb;
           Double_t dChiErr = FindXi(dVerr,1e-6);
           printf("An estimate chi of the event plane is: %f +- %f\n", dChi, dChiErr );
           printf("R:(subevents) = %f +- %f\n",dV,dVerr);
           if(calcChi){
               hresolution->SetBinContent(binx, biny, dChi );
               hresolution->SetBinError(  binx, biny, dChiErr);
           }
           else{
               hresolution->SetBinContent(binx, biny, dV );
               hresolution->SetBinError(  binx, biny, dVerr );
           }
       }
    }
    TString name=hist->GetName();   
    hresolution->SetNameTitle(name.Data(),name.Data());
    hresolution->SetEntries(hist->GetEntries());
    return hresolution;

}
TH1* HParticleEvtChara::makeEPresolution(TH3 *hist, Bool_t calcChi)
{
    if(!hist) return 0;
    TString sName=hist->GetName();   
    sName.Append("_res");
    
    TAxis* axis = hist->GetZaxis();
    axis->SetRange(1, axis->GetNbins());
    TH1* temp  = hist->Project3D("xy_1");

    axis->SetRange(axis->FindBin(TMath::PiOver2()), axis->GetNbins());
    TH1* temp2  = hist->Project3D("xy_2");    
    temp2->Divide(temp);

    Int_t nxBins = temp2->GetNbinsX();
    Int_t nyBins = temp2->GetNbinsY();
    
    TH2D* hresolution  = (TH2D*) new TH2D(sName.Data(), sName.Data(), nxBins, temp2->GetXaxis()->GetXmin(), temp2->GetXaxis()->GetXmax(),
                                                                      nyBins, temp2->GetYaxis()->GetXmin(), temp2->GetYaxis()->GetXmax());
    (hresolution->GetXaxis())->SetTitle(hist->GetXaxis()->GetTitle());
    (hresolution->GetYaxis())->SetTitle(hist->GetYaxis()->GetTitle());
    (hresolution->GetZaxis())->SetTitle(hist->GetZaxis()->GetTitle());
    
    Int_t bin,binx,biny;
    
    for (binx =0;binx<=nxBins+1;binx++) {
       for (biny =0;biny<=nyBins+1;biny++) {
           bin   = biny*(nxBins+2) + binx;
           
           Double_t ratio        = temp2->GetBinContent(binx,biny);
           printf("\nbin=%d binX=%d binY=%d  Ratio=%f\n",bin, binx, biny, ratio);
           if( ratio <= 0 ){
               continue;
           }
           Double_t chisq = -2.*TMath::Log(2.*ratio);
           Double_t dChi = sqrt(chisq);
           printf("An estimate chi of the event plane is: %f\n", dChi );
           Double_t dV = ComputeResolution( TMath::Sqrt2()*dChi );
           printf("An estimate of the event plane resolution is: %f\n", dV );
           Double_t dVerr = 0.;
           printf("R:(subevents) = %f +- %f\n",dV,dVerr);

           if(calcChi){
               hresolution->SetBinContent(binx, biny, dChi );
               //hresolution->SetBinError(  binx, biny, dChiErr);
           }
           else{

               hresolution->SetBinContent(binx, biny, dV );
               hresolution->SetBinError(  binx, biny, dVerr );
           }
       }
    }
    
    TString name=hist->GetName();   
    hresolution->SetNameTitle(name.Data(),name.Data());
    hresolution->SetEntries(hist->GetEntries());
    return hresolution;
}

//--------------------------------------------------------------------
Double_t HParticleEvtChara::ModifiedBesselI(Int_t n, Double_t x) const
{
  // compute half-integer modified Bessel functions
  // order: n>0, for n>5/2, interpolation is used (improve this by using recurrence!!!)
  const Double_t FACTOR = 0.797884561;  //FIXME
  if (n<0) return 0;
  if (x<1e-7) return 0;

  if (n==0)       return FACTOR*sqrt(x)*TMath::SinH(x)/x;   // 1/2
  else if (n==1)  return FACTOR*sqrt(x)*( -TMath::SinH(x)/(x*x) + TMath::CosH(x)/x );   // 3/2
  else if (n==2)  return FACTOR*sqrt(x)*( (3./(x*x)+1.)*TMath::SinH(x)/x - 3.*TMath::CosH(x)/(x*x) );   // 5/2
  return 0.5*(TMath::BesselI(n,x)+TMath::BesselI(n+1,x));   // use average of integer-order Bessel
}

//--------------------------------------------------------------------
Double_t HParticleEvtChara::ComputeResolution( Double_t x, Int_t n ) const
{
  // Computes resolution for Event Plane method
  if(x > 51.3) {
    printf("Warning: Estimation of total resolution might be WRONG. Please check!");
    return 0.99981;
  }
  if (n<1) return 0;
  Int_t n1= (n-1)/2;      
  Int_t n2 = (n+1)/2;
  
  Double_t a = x*x/2;  //   in formula (8) of Ollitrault arXiv:nucl-ex/9711003v2
                       //   it is x*x/2 for  full EP resolution here sub-event resolution
  Double_t b = TMath::Exp(-a);
  if (n==1)        b *= TMath::BesselI0(a)+TMath::BesselI1(a);
  else if(n%2==1)  b *= TMath::BesselI(n1,a)+TMath::BesselI(n2,a);
  else             b *= ModifiedBesselI(n1, a) + ModifiedBesselI(n2, a);
  return TMath::Sqrt(TMath::Pi())/2*x*b; 
}


//--------------------------------------------------------------------
Double_t HParticleEvtChara::FindXi( Double_t res, Double_t prec, Int_t n ) const
{
  // Computes x(res) for Event Plane method
  if(res > 0.99981) {
    printf("Warning: Resolution for subEvent is high. You reached the precision limit.");
    return 51.3;
  }
  int nSteps =0;
  Double_t xtmp=0, xmin=0, xmax=51.3, rtmp=0, delta=2*prec;
  while( delta > prec ) {
    xtmp = 0.5*(xmin+xmax);
    rtmp = ComputeResolution(xtmp, n);
    delta = TMath::Abs( res-rtmp );
    if(rtmp>res) xmax = xtmp;
    if(rtmp<res) xmin = xtmp;
    nSteps++;
  }
  return xtmp;
}
//--------------------------------------------------------------------
Bool_t HParticleEvtChara::addEstimatorHist(TH1F *hist, Float_t fractionXsection, UInt_t centE, Int_t direction)
{
    if(!hist)return kFALSE;
    if(fractionXsection<0.1) return kFALSE;
    if(centE>=kNumCentralityEstimator) return kFALSE;
    
    TString temp;
    if(fPostFix.CompareTo("")==0) temp = Form("%s",getStringCentralityEstimator(centE).Data() );
    else                       temp = Form("%s_%s",getStringCentralityEstimator(centE).Data(), fPostFix.Data() );
    
    hist->SetName(temp.Data());
    hist->SetTitle(temp.Data());
    fEstimatorHist[centE]= (TH1F*) hist;
    fCentralityPercentileHist[centE] = (TH1F*) makePercentiles(hist, fractionXsection, direction);
    for (int centC = 0; centC < (int) kNumCentralityClass; ++centC){
        fCentralityHist[centE][centC] = (TH1F*) makeClasses(hist, fractionXsection, centC, direction);
        if(fCentralityHist[centE][centC]) printCentralityClass(centE, centC);
    }
    return kTRUE;
}

TH1F* HParticleEvtChara::makePercentiles(TH1F *htemp, Float_t fractionXsection, Int_t direction)
{
  if(!htemp) return 0;  

  TH1F *hpercent  = (TH1F*) htemp->Clone("hpercent");
  TString name=htemp->GetName();   
  name.Append("_percentile");
  hpercent->SetNameTitle(name.Data(),name.Data());
  hpercent->Reset();
  Float_t totIntegral = fractionXsection / htemp->Integral(1,htemp->GetNbinsX()); 
  if(direction<0){
      for (int ibin=1; ibin<=htemp->GetNbinsX(); ibin++){
          hpercent->SetBinContent(ibin, totIntegral * htemp->Integral(ibin,htemp->GetNbinsX()) );
      } 
  }
  else if(direction>0){
      for (int ibin=1; ibin<=htemp->GetNbinsX(); ibin++){
          hpercent->SetBinContent(ibin, totIntegral * htemp->Integral(1,ibin) );
      } 
  }
  return hpercent;
}

TH1F* HParticleEvtChara::makeClasses(TH1F *htemp, Float_t fractionXsection, UInt_t centC, Int_t direction)
{

    if(!htemp) return 0;
    if(fractionXsection<0.1) return 0;
    Int_t nClasses = getCentralityClassNbins(centC);
    Double_t integral = htemp->Integral();
    Double_t norm = integral / fractionXsection;
    Float_t* PercentileArray = getCentralityClassArray(centC);
    std::vector<Double_t> binEdge;
    std::vector<Double_t> xSection;
    std::vector<TString>  fLabels; 


    Int_t start = 1;
    Int_t stop = htemp->GetNbinsX();
    if(direction < 0){
        direction = -1;
        start = htemp->GetNbinsX()-1;
        stop = 0;
        binEdge.push_back(htemp->GetBinLowEdge(htemp->GetNbinsX()));
    }
    else{
        direction = 1;
        binEdge.push_back(htemp->GetBinLowEdge(1));
    }
    Int_t bin = start;
    Float_t lxs = 0;
    Float_t txs = 0;


    // Find edge and starting point at begining
    while(1){
        lxs += htemp->GetBinContent(bin);
        Double_t pxs = lxs/norm;
        if( pxs>0.001 ){
          if (direction>0)   binEdge.push_back(htemp->GetBinLowEdge(bin+1));
          else               binEdge.push_back(htemp->GetBinLowEdge(bin));
          xSection.push_back(pxs);
          bin += direction;
          break;
        }
        bin += direction;
    }

    Float_t totxs = 0;
    for (Int_t i = 0; i < nClasses; ++i) {
        lxs = 0 ;
        totxs += PercentileArray[i];
        while(1){
            lxs += htemp->GetBinContent(bin);
            txs += htemp->GetBinContent(bin);
            Double_t pxs = txs/norm;
            Double_t tdiff = (txs+htemp->GetBinContent(bin+direction))/norm;
            if( ( pxs>totxs )
                || (TMath::Abs(pxs-totxs)<=TMath::Abs(tdiff-totxs)) )
            {
              if (direction>0)  binEdge.push_back(htemp->GetBinLowEdge(bin+1));
              else              binEdge.push_back(htemp->GetBinLowEdge(bin));
              xSection.push_back(lxs/norm);
              bin += direction;
              break;
            }
            bin += direction;
            if ( (direction>0 && bin>=stop) || (direction<0 && bin<=stop ) || (txs>=integral) ) break;
        }
        if(totxs>fractionXsection || (direction>0 && bin>=stop) || (direction<0 && bin<=stop ) || (txs>=integral)) break;
    }

      fLabels.push_back("overflow");
      Double_t totXsection =0;
      for(std::vector<double>::size_type index = 1; index < xSection.size()-1; ++index){
          fLabels.push_back(Form("%02.0f-%02.0f",round(totXsection),round(totXsection+xSection[index])) );
          totXsection += xSection[index];
      }
      fLabels.push_back("underflow");
 
      std::reverse(fLabels.begin(),fLabels.end());
      std::reverse(binEdge.begin(), binEdge.end());
      std::reverse(xSection.begin(), xSection.end());

      Double_t xlowbins[binEdge.size()];
      std::copy(binEdge.begin(), binEdge.end(), xlowbins);

      TString name = htemp->GetTitle();
      name = Form("%s_%s_fixedCuts", name.Data(), getStringCentralityClass(centC).Data());
      TH1F *hfixedCuts = new TH1F(name.Data(), name.Data(), binEdge.size()-1, xlowbins); 

      for(std::vector<double>::size_type bin = 0; bin < fLabels.size(); ++bin){
          (hfixedCuts->GetXaxis())->SetBinLabel(bin+1,fLabels[bin]);
          hfixedCuts->SetBinContent(bin+1, fLabels.size()-bin-1);
          hfixedCuts->SetBinError(bin+1, xSection[bin]);
      }
  return hfixedCuts;
}



Int_t HParticleEvtChara::getNbins(TString estimator)
{
    // legacy code
    // Number of Bins (CentralityClasses and Over- and Underflow) in the Estimator

    if (estimator.CompareTo("TOFRPCtimecut")==0 || estimator.CompareTo("TOFRPC5")==0) 
        return getNbins(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k5);
    else if (estimator.CompareTo("TOFRPCtimecutFOPI")==0 || estimator.CompareTo("TOFRPCFOPI")==0) 
        return getNbins(HParticleEvtChara::kTOFRPC, HParticleEvtChara::kFOPI);
    else if (estimator.CompareTo("TOFRPCtimecut10")==0 || estimator.CompareTo("TOFRPC10")==0) 
        return getNbins(HParticleEvtChara::kTOFRPC, HParticleEvtChara::k10);
    else { Error("getNbins()","Sorry. getNbins() for %s not implemented yet!",estimator.Data()); return 0;}
}
Int_t HParticleEvtChara::getNbins(UInt_t centE, UInt_t centC)
{
    // Number of Bins (CentralityClasses and Over- and Underflow) in the Estimator
    TH1F *htemp = getCentralityClassHist(centE, centC); 
    if(htemp) return htemp->GetNbinsX();
    else return 0;
}

Int_t HParticleEvtChara::getCentralityClassNbins(UInt_t centC)
{
    if(centC==k1040)      return 6;
    else if(centC==kFOPI) return 6;
    Float_t binSize = getCentralityClassBinSize(centC);
    if(binSize>0) return round((100./binSize));
    //    cout << "binSize: "<< binSize  <<"  nBins: " <<nBins << " " <<endl;
    return 0;
}

Float_t* HParticleEvtChara::getCentralityClassArray(UInt_t centC)
{
    // 
    Float_t binSize = getCentralityClassBinSize(centC);
    Int_t nBins = getCentralityClassNbins(centC);
    if (nBins<1) return 0;
    
    Float_t* arr = new Float_t[nBins];
    if(binSize>0){
        Float_t xs = 0.;
        for(Int_t i =0;  i < nBins ; i++){
            xs+=binSize;
            if(xs < 100.){
                arr[i] = (Float_t) binSize;
            }
            else{
                arr[i] = 100. - (binSize*i);
            }
        }
    }
    else if(centC==k1040) {
        Double_t fxs[6] = {10.,30.,30, 10.,10.,10.};
        for(Int_t i =0;  i < nBins ; i++) arr[i] = fxs[i];
    }
    else if(centC==kFOPI) {
        Double_t fxs[6] = {6.3,14.7,9.9,10.,10.,10.};
        for(Int_t i =0;  i < nBins ; i++) arr[i] = fxs[i];
    }
    
    return arr;
}

Float_t* HParticleEvtChara::getUpEdgeArray(UInt_t centE, UInt_t centC)
{
    // 
    TH1F *htemp = getCentralityClassHist(centE, centC);
    
    if(htemp){
        Int_t nBins = htemp->GetNbinsX();
        Float_t* arr = new Float_t[nBins];
        for(Int_t i =0;  i < nBins ; i++) {
            //printf(" %8.2f\n",(htemp->GetXaxis())->GetBinLowEdge(nBins-i));
            arr[i] = (Float_t) (htemp->GetXaxis())->GetBinLowEdge(nBins-i);
        }
         return arr;
    }
    else return 0;
}

Float_t* HParticleEvtChara::getBinCenterArray(UInt_t centE, UInt_t centC)
{
    // 
    TH1F *htemp = getCentralityClassHist(centE, centC);
    
    if(htemp){
        Int_t nBins = htemp->GetNbinsX();
        Float_t* arr = new Float_t[nBins];
        Float_t pcent = 0;
        for(Int_t i =0;  i < nBins ; i++) {
                   pcent += 0.5*htemp->GetBinError(i);
            //       printf(" %8.2f\n",pcent);
            arr[nBins-i] = (Float_t) pcent;
        }
         return arr;
    }
    else return 0;
}

vector<TString> HParticleEvtChara::getLabelArray(UInt_t centE, UInt_t centC)
{
    // 
    TH1F *htemp = getCentralityClassHist(centE, centC); 
    vector<TString> ar;
    if(htemp){
        Int_t nBins = htemp->GetNbinsX();
        for(Int_t i =1;  i < nBins-1 ; i++) {
            //printf(" %s\n",(htemp->GetXaxis())->GetBinLabel(nBins-i));
            TString prv = TString( (htemp->GetXaxis())->GetBinLabel(nBins-i) );
            ar.push_back(prv);
        }
    }
    return ar;
}

Int_t HParticleEvtChara::getCentralityClass5(TString estimator)
{
    // Return centrality class 5% of total cross section
    return 1+(Int_t)getCentralityPercentile(estimator)/5.;
}
Int_t HParticleEvtChara::getCentralityClass10(TString estimator)
{
    // Return centrality class 5% of total cross section
    return 1+(Int_t)getCentralityPercentile(estimator)/10.;
}

Float_t HParticleEvtChara::getCentralityPercentile(TString estimator)
{
    // legacy code
    return getCentralityPercentile(getCentralityEstimatorFromString(estimator));
}


Float_t HParticleEvtChara::getCentralityPercentile(UInt_t centE)
{

    if(centE>=kNumCentralityEstimator){
        return 101.;
    }
    else if(!getCentralityPercentileHist(centE)){
      //  Error("getCentralityPercentile()","No CentralityEstimator defined!");
        return 101.;
    }
    else {
        Int_t buffer = getCentralityEstimator(centE);
        Int_t bin = getCentralityPercentileHist(centE)->FindBin(buffer);
        return getCentralityPercentileHist(centE)->GetBinContent(bin);
    }

    return 101.;
}

Float_t HParticleEvtChara::getSelectedParticleCandSecNorm(){
    
    HParticleEvtInfo *event_info = (HParticleEvtInfo*)fParticleEvtInfoCat->getObject(0);
    if(!event_info) {
        Error("getCentralityClass()","No HParticleEvtInfo");
        return 0;
    }
    
    Float_t SelectedParticleCandSecCorr=0;
    Int_t nGoodSectors=0;
    for (Int_t s = 0; s < 6; ++s ){
        if(gLoop->goodSector(s)){
            SelectedParticleCandSecCorr += event_info->getSelectedParticleCandMult(s)/event_info->getMeanMult(s);
            nGoodSectors++;
        }
    }
    if(nGoodSectors>0) SelectedParticleCandSecCorr /= nGoodSectors;
    return SelectedParticleCandSecCorr;
}

Float_t HParticleEvtChara::getFWSumChargeSpec(){

    Float_t MultFWSumChargeSpec=0;

    //HWallHit
    if(!fCatWallHit) return -1;
    HWallHitSim *wall = 0;
    for(Int_t i = 0; i < fCatWallHit->getEntries(); i ++ ){
        wall  = (HWallHitSim*) HCategoryManager::getObject(wall,fCatWallHit,i);
        if(!wall) continue;
        if(!PassesCutsFW(wall)) continue;
        if(!isSimulation) MultFWSumChargeSpec += wall->getCharge();
        else              MultFWSumChargeSpec += 93.*pow(wall->getCharge(),0.46-0.006*sqrt(wall->getCharge()));  // parametrization from R.Holzmann
     }

    return MultFWSumChargeSpec;
}

Float_t HParticleEvtChara::getFWSumZ(Int_t maxZ, Float_t maxTheta, UInt_t SubEvent){

    if(isNewEvent()) filterFWhits();
    Int_t acceptedHits = 0;
    Int_t multFWcells  = iFWHitvector.size();
    if(multFWcells<1) return 0;
    Float_t MultFWSumChargeSpec=0;
    //HWallHit
    if(!fCatWallHit) return -1;

    std::size_t start = 0;
    std::size_t stop  = iFWHitvector.size();
    if(SubEvent==1) stop = iFWHitvector.size()/2; 
    else if(SubEvent==2) start = iFWHitvector.size()/2;
    
    HWallHitSim *wall = 0;
    for ( std::vector<int>::iterator it=iFWHitvector.begin() + start ; it<iFWHitvector.begin() + stop ; ++it){
        wall  = (HWallHitSim*) HCategoryManager::getObject(wall,fCatWallHit,*it);
        if(!wall) continue;
        //printf("%d %d - ", *it, acceptedHits);
        //if(!PassesCutsFW(wall)) continue;
        acceptedHits++;
        Int_t Z = getFWCharge(wall);
        if(Z>maxZ) Z=maxZ;
        if(maxTheta>0){
            if(wall->getTheta()>maxTheta) continue;
        }
        else{
            Float_t weight = 1.-((wall->getTheta()-2)/7.);
            if(weight>0 && weight < 1.) Z *= weight;
        }
        MultFWSumChargeSpec += Z;
     }
    return MultFWSumChargeSpec;
}

Float_t HParticleEvtChara::getEt(){

    Float_t Et=0;
    if(!fParticleCandCat) return -1;
    //if(!fParticleCandCat) {Error("calcFWSumChargeSpec()","No calcEt");return kFALSE;}
    HParticleCand* particle_cand = 0;

    for(Int_t i = 0; i < fParticleCandCat->getEntries(); i ++ ){
        particle_cand  = (HParticleCand*) HCategoryManager::getObject(particle_cand,fParticleCandCat,i);
        if(!particle_cand->isFlagBit(Particle::kIsUsed)) continue;
        if(particle_cand->isFakeRejected() )            continue;
        if(particle_cand->getSystemUsed() == -1)        continue;
        if(!particle_cand->isFlagAND(4,                               
                Particle::kIsAcceptedHitInnerMDC,
                Particle::kIsAcceptedHitOuterMDC,
                Particle::kIsAcceptedHitMETA,
                Particle::kIsAcceptedRK)  )       continue;
//        if(!particle_cand->getPID()==14)       continue;
//        if(!particle_cand->getPID()==8)       continue;
//        if(!particle_cand->getPID()==9)       continue;
        if( particle_cand->getMomentumOrg() == particle_cand->getMomentum() ) continue;
        if( TMath::Abs(particle_cand->Rapidity()-0.74)>0.5 && particle_cand->Pt()>300 ){  //
            Et              += particle_cand->Et();
        }
    //-----------------------------------------------------------------
    } // end particle loop

    return Et;

}

Float_t HParticleEvtChara::getRatioEtEz(){
    
    Float_t RatioEtEz=0;
    Float_t Et=0;
    Float_t Ez=0;
    if(!fParticleCandCat) return -1;
    //if(!fParticleCandCat) {Error("calcRatioEtEz()","No fParticleCandCat");return kFALSE;}
    HParticleCand* particle_cand = 0;

    for(Int_t i = 0; i < fParticleCandCat->getEntries(); i ++ ){
        particle_cand  = (HParticleCand*) HCategoryManager::getObject(particle_cand,fParticleCandCat,i);
        if(!particle_cand->isFlagBit(Particle::kIsUsed)) continue;
        if(particle_cand->isFakeRejected() )            continue;
        if(particle_cand->getSystemUsed() == -1)        continue;
        if(!particle_cand->isFlagAND(4,                               
                Particle::kIsAcceptedHitInnerMDC,
                Particle::kIsAcceptedHitOuterMDC,
                Particle::kIsAcceptedHitMETA,
                Particle::kIsAcceptedRK)  )       continue;
        //if(!particle_cand->getPID()==14)       continue;
        //if(!particle_cand->getPID()==8)       continue;
        //if(!particle_cand->getPID()==9)       continue;
        if( particle_cand->getMomentumOrg() == particle_cand->getMomentum() ) continue;
//      if( TMath::Abs(particle_cand->Rapidity()-0.74)>0.5 && particle_cand->Pt()>300 ){  //
            Ez              += particle_cand->E()*particle_cand->CosTheta();   // pt2 == 0 ? 0 : E()*E() * pt2/(pt2+Z()*Z());
            Et              += particle_cand->Et();
            //cout<<"  Sum Et:  "<<Et<<"  Et:  "<<tlv1.Et()<<endl;
//      }
    //-----------------------------------------------------------------
    } // end particle loop
    if(Ez){
        RatioEtEz = Et/Ez;
    }
    else {
        RatioEtEz=0;
    }
    return RatioEtEz;


}


Float_t HParticleEvtChara::getDirectivity(){

    Float_t Directivity=0;
    TLorentzVector QVector = TLorentzVector();
    Int_t QVectorT = 0;
        if(!fParticleCandCat) return -1;
    //if(!fParticleCandCat) {Error("calcDirectivity()","No fParticleCandCat");return kFALSE;}
    HParticleCand* particle_cand = 0;

    for(Int_t i = 0; i < fParticleCandCat->getEntries(); i ++ ){
        particle_cand  = (HParticleCand*) HCategoryManager::getObject(particle_cand,fParticleCandCat,i);
        if(!particle_cand->isFlagBit(Particle::kIsUsed)) continue;
        if(particle_cand->isFakeRejected() )            continue;
        if(particle_cand->getSystemUsed() == -1)        continue;
        if(!particle_cand->isFlagAND(4,                               
                Particle::kIsAcceptedHitInnerMDC,
                Particle::kIsAcceptedHitOuterMDC,
                Particle::kIsAcceptedHitMETA,
                Particle::kIsAcceptedRK)  )       continue;
        
        if( particle_cand->Rapidity() > 0.74 ){  //
            QVector  += (TLorentzVector)* particle_cand;
            QVectorT +=  particle_cand->Pt();
        }
        else {  //
            QVector  -= (TLorentzVector)* particle_cand;
            QVectorT += particle_cand->Pt();
        }



    //-----------------------------------------------------------------
    } // end particle loop

    if(QVectorT>0)Directivity = QVector.Pt() / QVectorT;      //  = |Sum(vector_p_{t,i})| / Sum(|p_{t,i}|)   for 0>Y_cm
    else Directivity = 0;

    return Directivity;

}

