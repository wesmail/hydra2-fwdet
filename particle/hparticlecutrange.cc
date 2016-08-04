#include "hparticlecutrange.h"
#include <iostream>
#include <iomanip>

#include "TDirectory.h"
#include "TROOT.h"

using namespace std;

ClassImp(HParticleCutRange)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleCutRange
//
// Helper class for cuts.
// A cut has a name (should be unique), a cut number (better unique)
// and a condition as argument of creation. The cut object owns counters
// for the number of calls and the number of failed evaluations. The
// The counters cand be used to monitor the efficiency of the cut
// If the TCutG object belonging to the cut is set  evalG(var,var,version)
// will evaluate the graph. cut. As option TF1 objects cann be used to
// cut (see the exapmple macro below)
// The cuts can be inversed by setInverse(Bool_t). In fact
// cuts will than select outside the band low-up range and
// for the graph cut isInside(var,var)==kFALSE will be used for selection.
//
//#########################################################
// USAGE:
//  // outside eventloop:
//  HParticleCutRange cut1("BetaCut",1,0.9,1.1);
//  // will create a cut object selecting beta range and
//  ....
//  //  inside eventloop:
//  if(cut1->eval(beta,0)) { // true if condition is fullfilled, stat counted for version 0
//
//  }
//  if(cut1->eval(beta,1)) { // true if condition is fullfilled, stat counted for version 1
//
//  }
//
//  ....
//  // after eventloop
//  cut1.print(); // show cut name,number,cut statistics and range
//
//void testCutRange()
//{
//    // example macro for HParticleCutRange demonstrating the
//    // different features. The cut will be applied on the
//    // mass of an particle in this example.
//    //
//    // Cuts can be applied in 3 ways:
//    //
//    // CUTMODE:
//    // 1. 1-dim cuts (upper+lower limit)
//    // 2. 2-dim cut using TCutG
//    // 3. 2-dim cut using TF1
//    //
//    // In TCutG or TF1 mode the example cuts in mass depends on the momentum.
//    // The TCutG is owned by the cut and the user just has to set the
//    // points. TF1 have to be contructed outside the clas due to the
//    // available contructors of TF1. The user ahs to set the pointers
//    // to the TF1. Remark: if the TF1 is contructed from a user function
//    // the cut will not work when read back from root file.
//    //
//    // TF1MODE:
//    // There are 4 options :
//    // 1. low+up  (1 seperate TF1 for lower and upper cut)
//    // 2. low     (1 TF1 for lower cut)
//    // 3. up      (1 TF1 for upper cut)
//    // 3. mean+width (1 TF1 for mean of the cut, 1 TF1 for the width of the cut)
//
//
//    Bool_t   invert   = kFALSE;  // invert cut : default: kFALSE
//    Int_t    cutmode  = 2; //0=range,1=cutG,2=TF1
//    TString  tf1mode  = "low+up"; // TF1 mode: low+up, low,up,mean+width
//
//    Double_t slope    = .10;
//
//    Double_t mass_pion   = HPhysicsConstants::mass(8);
//    Double_t mass_proton = HPhysicsConstants::mass(14);
//
//
//    HParticleCutRange cut1("massCut",1,mass_proton-50,mass_proton+50);
//    cut1.setInverse(invert);
//
//
//    //--------------------------------------------------
//    // TF1 mode
//    // Cuts in mass depend on mommentum
//    // low+up , low , up mode
//    TF1* flow = new TF1("flow","pol1",0,1000);
//    flow->SetParameter(0,mass_pion);
//    flow->SetParameter(1,-slope);
//
//    TF1* fup = new TF1("fup","pol1"  ,0,1000);
//    fup->SetParameter(0,mass_pion);
//    fup->SetParameter(1,slope);
//    fup->SetLineColor(4);
//
//    // mean+width mode
//    TF1* fmean = new TF1("fmean","pol1",0,1000);
//    fmean->SetParameter(0,mass_pion);
//    fmean->SetParameter(1,0);
//
//    TF1* fwidth = new TF1("fwidth","pol1",0,1000);
//    fwidth->SetParameter(0,0);
//    fwidth->SetParameter(1,slope);
//    fwidth->SetLineColor(4);
//
//    if(tf1mode=="low+up")    cut1.setTF1(flow,fup,tf1mode);
//    if(tf1mode=="low")       cut1.setTF1(flow,  0,tf1mode);
//    if(tf1mode=="up")        cut1.setTF1(   0,fup,tf1mode);
//    if(tf1mode=="mean+width")cut1.setTF1(flow,fup,tf1mode);
//    //--------------------------------------------------
//
//
//    //--------------------------------------------------
//    // TCutG mode
//    cut1.getTCutG()->SetPoint(0,   0,mass_proton);
//    cut1.getTCutG()->SetPoint(1,1000,mass_proton - slope*1000);
//    cut1.getTCutG()->SetPoint(2,1000,mass_proton + slope*1000);
//    cut1.getTCutG()->SetPoint(3,   0,mass_proton);
//    //--------------------------------------------------
//
//
//
//    TH2F* hmomvsmass = new TH2F("hmassvsmom","mom vs mass",200,0,2000,200,0,1200);
//    hmomvsmass->SetXTitle("mom [MeV/c]");
//    hmomvsmass->SetYTitle("mass [MeV/c^{2}]");
//
//    //--------------------------------------------------
//    // create some dummy spectrum
//    Double_t masses [] = {mass_pion,mass_proton};
//
//    for(Int_t i = 0 ; i <10000; i ++){
//        Double_t mom     =  gRandom->Rndm()*2000;
//	Double_t massres =  mom*0.1;
//        Int_t    index   = (Int_t) (gRandom->Rndm()*2);
//        Double_t mass    = gRandom->Gaus(masses[index],massres);
//
//	if(cutmode == 0 && cut1.eval(mass)) {
//	    hmomvsmass->Fill(mom,mass);
//	}
//
//	if(cutmode == 1 && cut1.evalG(mom,mass)) {
//	    hmomvsmass->Fill(mom,mass);
//	}
//
//	if(cutmode == 2 && cut1.evalF(mass,mom)) {
//	    hmomvsmass->Fill(mom,mass);
//	}
//    }
//    //--------------------------------------------------
//
//
//    cut1.print();
//
//    TCanvas* result = new TCanvas("result","result",1000,800);
//
//    hmomvsmass->Draw("colz");
//
//
//    if(cutmode == 1){
//        cut1.getTCutG()->Draw("same");
//    }
//
//    if(cutmode == 2){
//	if(cut1.getTF1Low())cut1.getTF1Low()->DrawCopy("same");
//	if(cut1.getTF1Up() )cut1.getTF1Up() ->DrawCopy("same");
//    }
//
//    TFile* cutsfile = new TFile("myCuts.root","RECREATE");
//    cut1.Write();
//    cutsfile->Save();
//    cutsfile->Close();
//
//
//}
////////////////////////////////////////////////////////////////////////////////

HParticleCutRange::HParticleCutRange(TString name,
				     Int_t num,Double_t l,Double_t u)
{
    SetName(name);
    TDirectory* saveDir = gDirectory;
    gROOT->cd();
    fCut = new TCutG();
    fCut->SetName(name);
    fLowF1  = NULL;
    fUpF1   = NULL;
    fF1Mode = -1;
    flow        = l;
    fup         = u;
    fCutNumber  = num;
    fmaxVersion = 0;
    fbInverseCut= kFALSE;
    setMaxCut(4);
    saveDir->cd();
}

HParticleCutRange::~HParticleCutRange()
{
    ;
}


void HParticleCutRange::setTF1(TF1* flowF, TF1* fupF,TString mode)
{
    // Set the TF1 objects for low and up cut. The TF1
    // will be used by evalF(var,var2) for cutting where
    // the cuts in var will be a function of var2.
    // There are the following modes implemented:
    //
    // mode = "low+up" : lower and upper cur are defined
    //                   by independent TF1 (band pass)
    //      = "low"    : low TF1 is used to defined a cut
    //                   for var < lowcut(var2) ( high pass)
    //      = "up"     : up TF1 is used to defined a cut
    //                   for var > upcut(var2) ( low pass)
    //      = "mean+width" : is used to define a cut
    //                       by mean(var2)(low TF1) + width(var2)(up TF1)
    //                       for var > mean + width && var < mean - width  ( band pass)
    // the cuts can be inverted as the other cuts


    fF1Mode= -1;
    fLowF1 = NULL;
    fUpF1  = NULL;

    if(mode.CompareTo("low+up") == 0){
	if(flowF == 0 || fupF == 0){
	    Error("setTF1()","Both TF1 needs to be !=0 in mode \"low+up\"");
	    return;
	} else {
	    fF1Mode = 0;
	    fLowF1  = flowF;
	    fUpF1   = fupF;
	}
    } else if(mode.CompareTo("low") == 0){
	if(flowF == 0 || fupF != 0){
	    Error("setTF1()","Lower TF1 needs to be set and upper TF1 == 0  in mode \"low\"");
	    return;

	} else {
            fF1Mode=1;
	    fLowF1 = flowF;
	}
    } else if(mode.CompareTo("up") == 0){
	if(flowF != 0 || fupF == 0){
	    Error("setTF1()","Upper TF1 needs to be set and lower TF1 == 0  in mode \"up\"");
	    return;

	} else {
            fF1Mode=2;
	    fUpF1 = fupF;
	}
    } else if(mode.CompareTo("mean+width") == 0){
	if(flowF == 0 || fupF == 0){
	    Error("setTF1()","Both TF1 needs to be !=0 in mode \"mean+width\"");
	    return;
	} else {
            fF1Mode=3;
	    fLowF1 = flowF;
	    fUpF1 = fupF;
	}
    } else {
	Error("setTF1()","Unknown mode =%s",mode.Data());
    }
}

void   HParticleCutRange::setMaxCut(UInt_t max)
{
    // set the max number of versions of stst counters
    fmaxCut     = max;
    if(fmaxCut == 0) fmaxCut = 1;
    resetCounter();
}

Bool_t HParticleCutRange::eval(Double_t var,UInt_t version){

    // returns kTRUE if the object fullfills the condition.
    // counters for calls and failed conditions are filled.
    // version is used to fill the stat for a given version
    // (0 up to 9 incl )
    if(version<fmaxCut){
	fctCall[version]++;
        if(version>fmaxVersion) fmaxVersion = version;
    }
    if( (!fbInverseCut &&  (var > fup || var < flow)) ||
        ( fbInverseCut && !(var > fup || var < flow))
      )
       {
	if(version<fmaxCut)fctFail[version]++;
	return kFALSE;
    } else return kTRUE;
}

Bool_t HParticleCutRange::evalG(Double_t var,Double_t var2,UInt_t version){

    // returns kTRUE if the object fullfills the condition.
    // counters for calls and failed conditions are filled.
    // version is used to fill the stat for a given version

    if(fCut->GetN() == 0) {
	Warning("evalG(var,var)","Cut %s has no set TCutG !",GetName());
        return kFALSE;
    }
    if(version<fmaxCut){
	fctCall[version]++;
        if(version>fmaxVersion) fmaxVersion = version;
    }
    if( (!fbInverseCut && !fCut->IsInside(var,var2)) ||
        ( fbInverseCut &&  fCut->IsInside(var,var2))
      )
       {
	if(version<fmaxCut)fctFail[version]++;
	return kFALSE;
    } else return kTRUE;
}

Bool_t HParticleCutRange::evalF(Double_t var,Double_t var2,UInt_t version){

    // returns kTRUE if the object fullfills the condition.
    // counters for calls and failed conditions are filled.
    // version is used to fill the stat for a given version

    if(fF1Mode == -1) {
	Warning("evalF(var,var)","TF1 Cut %s has no set !",GetName());
        return kFALSE;
    }

    if(version<fmaxCut){
	fctCall[version]++;
        if(version>fmaxVersion) fmaxVersion = version;
    }

    Double_t lowCut = -1e30;
    Double_t upCut  =  1e30;

    if(fF1Mode == 0) {    // low+up
	lowCut = fLowF1->Eval(var2);
	upCut  = fUpF1 ->Eval(var2);
    } else if (fF1Mode == 1 ) { // low
	lowCut = fLowF1->Eval(var2);
    } else if (fF1Mode == 2) { // up
	upCut = fUpF1->Eval(var2);
    } else {  // mean+width
	Double_t width = fUpF1->Eval(var2);
	Double_t mean  = fLowF1->Eval(var2);
	lowCut = mean - width;
	upCut  = mean + width;
    }


    if( (!fbInverseCut && !( var<upCut && var>lowCut) )  ||
        ( fbInverseCut &&  ( var<upCut && var>lowCut))
      )
       {
	if(version<fmaxCut)fctFail[version]++;
	return kFALSE;
    } else return kTRUE;
}

void HParticleCutRange::print()
{
    Int_t p = cout.precision();
    std::ios_base::fmtflags fl =cout.flags();
    cout<<"CutNumber : "<<setw(3)<<fCutNumber
	<<" name : "<<setw(20)<<GetName()
	<<", lowCut  : "<<setw(10)<<flow
	<<", UpCut  : "<<setw(10)<<fup<<", cut [%] : "<<flush;
    for(UInt_t i = 0; i < fmaxCut ; i++){ cout<<setw(5)<<fixed<<right<<setprecision(1)<<getCutRate(i)<<" "<<flush;}
    cout<<setprecision(p)<<endl;
    cout.flags(fl);
}

void HParticleCutRange::resetCounter()
{
    fctFail.clear();
    fctCall.clear();
    fctFail.assign(fmaxCut,0);
    fctCall.assign(fmaxCut,0);
}

Float_t HParticleCutRange::getCutRate(UInt_t version)
{
    return (version <fmaxCut && fctCall[version] > 0) ? (fctFail[version]/(Float_t)fctCall[version])*100.:0;
}

UInt_t HParticleCutRange::getNCall(UInt_t version)
{
    return (version <fmaxCut ) ? fctCall[version]:0;
}

UInt_t HParticleCutRange::getNFail(UInt_t version)
{
    return (version <fmaxCut ) ? fctFail[version]:0;
}

