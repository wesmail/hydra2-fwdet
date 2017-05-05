#include "hedgui.h"
#include "hedhelpers.h"

#include "TObject.h"

#include "TRootBrowser.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveScene.h"
#include "TEveViewer.h"

#include "TColor.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TEveGeoNode.h"
#include "TColor.h"


#include <iostream>
using namespace std;

ClassImp(HEDColorDef)
ClassImp(HEDNumberLabeled)
ClassImp(HEDSetup)



//----------------------------------------------------------------
HEDColorDef*     HEDColorDef::gEDColorDef = 0;
TEveGeoTopNode*  HEDColorDef::gGeoTopNode = 0;
Int_t HEDColorDef::colMDC[4] = {kRed,kRed,kRed,kRed};
Int_t HEDColorDef::colTOF    = kGreen;
Int_t HEDColorDef::colRPC    = kBlue;
Int_t HEDColorDef::colSHOWER = kMagenta;
Int_t HEDColorDef::colRICH   = kGray;
Int_t HEDColorDef::colFRAMES = kYellow;
Int_t HEDColorDef::colCOILS  = kGray;
Int_t HEDColorDef::colWALL   = kGray;

Int_t HEDColorDef::transMDC[4] = {60,60,60,60};
Int_t HEDColorDef::transTOF    = 60;
Int_t HEDColorDef::transRPC    = 85;
Int_t HEDColorDef::transSHOWER = 60;
Int_t HEDColorDef::transRICH   = 60;
Int_t HEDColorDef::transFRAMES = 60;
Int_t HEDColorDef::transCOILS  = 60;
Int_t HEDColorDef::transWALL   = 60;

void HEDColorDef::showSec(Bool_t show, Int_t sec){
    TString name = Form("SEC%i",sec+1);
    TString volname;
    for(Int_t i=0; i<volSEC.GetEntries(); i++){
        TGeoVolume* vol = (TGeoVolume*) volSEC[i];
        volname = vol->GetName();
	if(volname.CompareTo(name.Data())==0) {
	    vol->SetVisibility(show);
	    vol->SetVisDaughters(show);
	}
    }
}

void HEDColorDef::showCoils(Bool_t show){
    for(Int_t i=0; i<volCOILS.GetEntries(); i++){
        TGeoVolume* vol = (TGeoVolume*) volCOILS[i];
	vol->SetVisibility(show);
    }
}

void HEDColorDef::showFrames(Bool_t show){
    for(Int_t i=0; i<volFRAMES.GetEntries(); i++){
        TGeoVolume* vol = (TGeoVolume*) volFRAMES[i];
	vol->SetVisibility(show);
    }
}

void HEDColorDef::showWall(Bool_t show){
    for(Int_t i=0; i<volWALL.GetEntries(); i++){
        TGeoVolume* vol = (TGeoVolume*) volWALL[i];
	vol->SetVisibility(show);
    }
}

void HEDColorDef::showRich(Bool_t show){
    for(Int_t i=0; i<volRICH.GetEntries(); i++){
        TGeoVolume* vol = (TGeoVolume*) volRICH[i];
	vol->SetVisibility(show);
    }
}

void HEDColorDef::showMdc(Bool_t show, Int_t sec, Int_t mod){
    for(Int_t i=0; i<nodesMDC.GetEntries(); i++){
	TGeoNode* vol = (TGeoNode*) nodesMDC[i];
        TString name = vol->GetName();
	if(sec < 0 || name.CompareTo(Form("DR%iM_%i",mod+1,sec+1)) == 0){
	    vol->SetVisibility(show);
	    vol->SetVisDaughters(show);
	}

    }
}

void HEDColorDef::showTof(Bool_t show, Int_t sec){
    for(Int_t i=0; i<nodesTOF.GetEntries(); i++){
        TGeoNode* vol = (TGeoNode*) nodesTOF[i];
        TString name = vol->GetName();
	if(sec < 0 || name.Contains(Form("_%i",sec+1))){
	    vol->SetVisibility(show);
	    vol->SetVisDaughters(show);
	}
    }
}

void HEDColorDef::showRpc(Bool_t show, Int_t sec){
    for(Int_t i=0; i<nodesRPC.GetEntries(); i++){
        TGeoNode* vol = (TGeoNode*) nodesRPC[i];
        TString name = vol->GetName();
	if(sec < 0 || name.CompareTo(Form("EBOX_%i",sec+1)) == 0){
	    vol->SetVisibility(show);
	    vol->SetVisDaughters(show);
	}
    }
}

void HEDColorDef::showShower(Bool_t show, Int_t sec){
    for(Int_t i=0; i<nodesSHOWER.GetEntries(); i++){
	TGeoNode* vol = (TGeoNode*) nodesSHOWER[i];
	TString name = vol->GetName();
	if(sec < 0 ||
	   name.CompareTo(Form("SHK%i_1",sec+1)) == 0 ||
	   name.CompareTo(Form("GMOM_%i",sec+1)) == 0
	  ){
	    vol->SetVisibility(show);
	    vol->SetVisDaughters(show);
	}
    }
}


void HEDColorDef::colRich(Pixel_t pixel, Int_t trans){
    for(Int_t i=0; i<volRICH.GetEntries(); i++){
	TGeoVolume* vol = (TGeoVolume*) volRICH[i];
	vol->SetLineColor(TColor::GetColor(pixel));
        if(trans != -1) vol->SetTransparency(trans);
    }
}
void HEDColorDef::colMdc(Pixel_t pixel, Int_t trans,Int_t mod){

    Int_t istart,imax;
    if(mod == -1) { istart = 0; imax = 4;}
    else          { istart = mod; imax = mod + 1; }
    for(Int_t i = istart; i<imax&&i<volMDC.GetEntries(); i++){
	TGeoVolume* vol = (TGeoVolume*) volMDC[i];
	vol->SetLineColor(TColor::GetColor(pixel));
	if(trans != -1) vol->SetTransparency(trans);
    }
}
void HEDColorDef::colTof(Pixel_t pixel, Int_t trans){
    for(Int_t i=0; i<volTOF.GetEntries(); i++){
	TGeoVolume* vol = (TGeoVolume*) volTOF[i];
	vol->SetLineColor(TColor::GetColor(pixel));
	if(trans != -1) vol->SetTransparency(trans);
    }
}
void HEDColorDef::colRpc(Pixel_t pixel, Int_t trans){
    for(Int_t i=0; i<volRPC.GetEntries(); i++){
	TGeoVolume* vol = (TGeoVolume*) volRPC[i];
	vol->SetLineColor(TColor::GetColor(pixel));
	if(trans != -1) vol->SetTransparency(trans);
    }
}
void HEDColorDef::colShower(Pixel_t pixel, Int_t trans){
    for(Int_t i=0; i<volSHOWER.GetEntries(); i++){
	TGeoVolume* vol = (TGeoVolume*) volSHOWER[i];
	vol->SetLineColor(TColor::GetColor(pixel));
	if(trans != -1) vol->SetTransparency(trans);
    }
}
void HEDColorDef::colWall(Pixel_t pixel, Int_t trans){
    for(Int_t i=0; i<volWALL.GetEntries(); i++){
	TGeoVolume* vol = (TGeoVolume*) volWALL[i];
	vol->SetLineColor(TColor::GetColor(pixel));
	if(trans != -1) vol->SetTransparency(trans);
    }
}
void HEDColorDef::colFrames(Pixel_t pixel, Int_t trans){
    for(Int_t i=0; i<volFRAMES.GetEntries(); i++){
	TGeoVolume* vol = (TGeoVolume*) volFRAMES[i];
	vol->SetLineColor(TColor::GetColor(pixel));
	if(trans != -1) vol->SetTransparency(trans);
    }
}
void HEDColorDef::colCoils(Pixel_t pixel, Int_t trans){
    for(Int_t i=0; i<volCOILS.GetEntries(); i++){
	TGeoVolume* vol = (TGeoVolume*) volCOILS[i];
	vol->SetLineColor(TColor::GetColor(pixel));
	if(trans != -1) vol->SetTransparency(trans);
    }
}
//----------------------------------------------------------------






//----------------------------------------------------------------
HEDSetup* HEDSetup::gEDSetup = 0;

HEDSetup::HEDSetup(const TGWindow *p, UInt_t w, UInt_t h)
    : TGMainFrame(p, w, h)
{

    Int_t width  = 300;
    Int_t height = 100;

    SetCleanup(kDeepCleanup);
    DontCallClose();

    HEDColorDef* colorDef = HEDColorDef::getEDColorDef();

    TGVerticalFrame *fHLMaster = new TGVerticalFrame(this,width,height,kLHintsTop|kLHintsLeft);


    //-------------------------------------------------------------
    // sector check boxes
    TGHorizontalFrame *fHL2 = new TGHorizontalFrame(fHLMaster, 1, 1,kLHintsTop);
    TGGroupFrame* fButtonGroupSec = new TGGroupFrame(fHL2, "Sector setup",kVerticalFrame);

    fCheckSector[0] = new TGCheckButton(fButtonGroupSec, new TGHotString("S 0"), IDs.GetUnID());
    fButtonGroupSec->AddFrame(fCheckSector[0], new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));

    fCheckSector[1] = new TGCheckButton(fButtonGroupSec, new TGHotString("S 1"), IDs.GetUnID());
    fButtonGroupSec->AddFrame(fCheckSector[1], new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));

    fCheckSector[2] = new TGCheckButton(fButtonGroupSec, new TGHotString("S 2"), IDs.GetUnID());
    fButtonGroupSec->AddFrame(fCheckSector[2], new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));

    fCheckSector[3] = new TGCheckButton(fButtonGroupSec, new TGHotString("S 3"), IDs.GetUnID());
    fButtonGroupSec->AddFrame(fCheckSector[3], new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));

    fCheckSector[4] = new TGCheckButton(fButtonGroupSec, new TGHotString("S 4"), IDs.GetUnID());
    fButtonGroupSec->AddFrame(fCheckSector[4], new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));

    fCheckSector[5] = new TGCheckButton(fButtonGroupSec, new TGHotString("S 5"), IDs.GetUnID());
    fButtonGroupSec->AddFrame(fCheckSector[5], new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 2, 1, 1));

    fHL2->AddFrame(fButtonGroupSec, new TGLayoutHints(kLHintsLeft | kLHintsTop, 1, 1, 1, 1));
    //-------------------------------------------------------------

    //-------------------------------------------------------------
    // volume check boxes
    TGGroupFrame* fButtonGroupVol = new TGGroupFrame(fHL2, "Volumes setup",kVerticalFrame);
    TColor* col;

    TGHorizontalFrame *fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colRICH);
    colRICH = new TGColorSelect(fH,col->GetPixel(), 0);
    fH->AddFrame(colRICH, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transRICH = new TGNumberEntry(fH, colorDef->transRICH, 3, -1, TGNumberFormat::kNESInteger);
    transRICH->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transRICH, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    fCheckRICH   = new TGCheckButton(fH, new TGHotString("RICH"),   IDs.GetUnID());
    fH->AddFrame(fCheckRICH, new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));


    //---------------------MDC-------------------------------------
    // MDC0
    fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colMDC[0]);
    colMDC[0] = new TGColorSelect(fH,col->GetPixel(), 0);
    fH->AddFrame(colMDC[0], new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transMDC[0] = new TGNumberEntry(fH, colorDef->transMDC[0], 3, -1, TGNumberFormat::kNESInteger);
    transMDC[0]->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transMDC[0], new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    fCheckMDC[0]    = new TGCheckButton(fH, new TGHotString("MDC0"),    IDs.GetUnID());
    fH->AddFrame(fCheckMDC[0], new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));
    // MDC1
    fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colMDC[1]);
    colMDC[1] = new TGColorSelect(fH,col->GetPixel(), 0);
    fH->AddFrame(colMDC[1], new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transMDC[1] = new TGNumberEntry(fH, colorDef->transMDC[1], 3, -1, TGNumberFormat::kNESInteger);
    transMDC[1]->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transMDC[1], new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    fCheckMDC[1]    = new TGCheckButton(fH, new TGHotString("MDC1"),    IDs.GetUnID());
    fH->AddFrame(fCheckMDC[1], new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));
    // MDC2
    fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colMDC[2]);
    colMDC[2] = new TGColorSelect(fH,col->GetPixel(), 0);
    fH->AddFrame(colMDC[2], new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transMDC[2] = new TGNumberEntry(fH, colorDef->transMDC[2], 3, -1, TGNumberFormat::kNESInteger);
    transMDC[2]->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transMDC[2], new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    fCheckMDC[2]    = new TGCheckButton(fH, new TGHotString("MDC2"),    IDs.GetUnID());
    fH->AddFrame(fCheckMDC[2], new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));
    // MDC3
    fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colMDC[3]);
    colMDC[3] = new TGColorSelect(fH,col->GetPixel(), 0);
    fH->AddFrame(colMDC[3], new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transMDC[3] = new TGNumberEntry(fH, colorDef->transMDC[3], 3, -1, TGNumberFormat::kNESInteger);
    transMDC[3]->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transMDC[3], new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    fCheckMDC[3]    = new TGCheckButton(fH, new TGHotString("MDC3"),    IDs.GetUnID());
    fH->AddFrame(fCheckMDC[3], new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));
    //--------------------------------------------------------------




    fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colTOF);
    colTOF = new TGColorSelect(fH, col->GetPixel(), 0);
    fH->AddFrame(colTOF, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transTOF = new TGNumberEntry(fH, colorDef->transTOF, 3, -1, TGNumberFormat::kNESInteger);
    transTOF->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transTOF, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    fCheckTOF    = new TGCheckButton(fH, new TGHotString("TOF"),    IDs.GetUnID());
    fH->AddFrame(fCheckTOF, new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));


    fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colRPC);
    colRPC = new TGColorSelect(fH, col->GetPixel(), 0);
    fH->AddFrame(colRPC, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transRPC = new TGNumberEntry(fH, colorDef->transRPC, 3, -1, TGNumberFormat::kNESInteger);
    transRPC->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transRPC, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    fCheckRPC    = new TGCheckButton(fH, new TGHotString("RPC"),    IDs.GetUnID());
    fH->AddFrame(fCheckRPC, new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));

    fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colWALL);
    colWALL = new TGColorSelect(fH, col->GetPixel(), 0);
    fH->AddFrame(colWALL, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transWALL = new TGNumberEntry(fH, colorDef->transWALL, 3, -1, TGNumberFormat::kNESInteger);
    transWALL->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transWALL, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    fCheckWALL    = new TGCheckButton(fH, new TGHotString("WALL"),    IDs.GetUnID());
    fH->AddFrame(fCheckWALL, new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));


    fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colSHOWER);
    colSHOWER = new TGColorSelect(fH, col->GetPixel(), 0);
    fH->AddFrame(colSHOWER, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transSHOWER = new TGNumberEntry(fH, colorDef->transSHOWER, 3, -1, TGNumberFormat::kNESInteger);
    transSHOWER->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transSHOWER, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    if(!HEDTransform::isEmc()) fCheckSHOWER = new TGCheckButton(fH, new TGHotString("SHOWER"), IDs.GetUnID());
    else                       fCheckSHOWER = new TGCheckButton(fH, new TGHotString("ECAL"), IDs.GetUnID());
    fH->AddFrame(fCheckSHOWER, new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));


    fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colFRAMES);
    colFRAMES = new TGColorSelect(fH, col->GetPixel(), 0);
    fH->AddFrame(colFRAMES, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transFRAMES = new TGNumberEntry(fH, colorDef->transFRAMES, 3, -1, TGNumberFormat::kNESInteger);
    transFRAMES->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transFRAMES, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    fCheckFRAMES = new TGCheckButton(fH, new TGHotString("FRAMES"), IDs.GetUnID());
    fH->AddFrame(fCheckFRAMES, new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));


    fH = new TGHorizontalFrame(fButtonGroupVol, 1, 1,kLHintsTop);
    col = gROOT->GetColor(colorDef->colCOILS);
    colCOILS = new TGColorSelect(fH, col->GetPixel(), 0);
    fH->AddFrame(colCOILS, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    transCOILS = new TGNumberEntry(fH, colorDef->transCOILS, 3, -1, TGNumberFormat::kNESInteger);
    transCOILS->SetLimits(TGNumberFormat::kNELLimitMinMax,0,100);
    fH->AddFrame(transCOILS, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 0, 2, 2));
    fCheckCOILS  = new TGCheckButton(fH, new TGHotString("COILS"),  IDs.GetUnID());
    fH->AddFrame(fCheckCOILS, new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 2, 1, 1));
    fButtonGroupVol->AddFrame(fH, new TGLayoutHints(kLHintsExpandX, 0, 2, 1, 1));


    fHL2->AddFrame(fButtonGroupVol, new TGLayoutHints(kLHintsRight | kLHintsTop, 1, 1, 1, 1));
    //-------------------------------------------------------------

    fHLMaster->AddFrame(fHL2);
    //-------------------------------------------------------------


    //-------------------------------------------------------------
    // timed check boxes
    TGHorizontalFrame *fHL3 = new TGHorizontalFrame(fHLMaster, width, height);
    TGGroupFrame *grouploop = new TGGroupFrame(fHL3, "run loop",kHorizontalFrame);
    grouploop->SetTitlePos(TGGroupFrame::kCenter);

    fCheckTimed = new TGCheckButton(grouploop, new TGHotString("run loop"),   IDs.GetUnID());
    grouploop->AddFrame(fCheckTimed, new TGLayoutHints(kLHintsLeft, 0, 2, 1, 1));
    fNumberLoopTime = new HEDNumberLabeled(grouploop, "seconds");
    grouploop->AddFrame(fNumberLoopTime, new TGLayoutHints(kLHintsRight, 0, 2, 1, 1));

    fHL3->AddFrame(grouploop, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    fHLMaster->AddFrame(fHL3);
    //-------------------------------------------------------------


    //-------------------------------------------------------------
    TGHorizontalFrame *fHL5 = new TGHorizontalFrame(fHLMaster, width, height);
    TGGroupFrame *groupMDC  = new TGGroupFrame(fHL5, "MDC modules");
    groupMDC->SetTitlePos(TGGroupFrame::kCenter);

    TGMatrixLayout* matrix = new TGMatrixLayout(groupMDC,6,4);
    groupMDC->SetLayoutManager(matrix);
    for(Int_t s = 0; s < 6; s ++){
	for(Int_t m = 0; m < 4; m ++){
	    fCheckMDCMod[s][m] = new TGCheckButton(groupMDC, new TGHotString(Form("S%iM%i",s,m)),IDs.GetUnID());
	    groupMDC->AddFrame(fCheckMDCMod[s][m]);
	}
    }
    fHL5->AddFrame(groupMDC, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));
    fHLMaster->AddFrame(fHL5);
    //-------------------------------------------------------------

    //-------------------------------------------------------------
    TGHorizontalFrame *fHL6 = new TGHorizontalFrame(fHLMaster, width, height);
    TGGroupFrame *groupTOF  = new TGGroupFrame(fHL6, "TOF sectors",kHorizontalFrame);
    groupTOF->SetTitlePos(TGGroupFrame::kCenter);

    for(Int_t s = 0; s < 6; s ++){
	fCheckTOFSec[s] = new TGCheckButton(groupTOF, new TGHotString(Form("S%i",s)),IDs.GetUnID());
	groupTOF->AddFrame(fCheckTOFSec[s]);
    }
    fHL6->AddFrame(groupTOF, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));
    fHLMaster->AddFrame(fHL6);
    //-------------------------------------------------------------

    //-------------------------------------------------------------
    TGHorizontalFrame *fHL7 = new TGHorizontalFrame(fHLMaster, width, height);
    TGGroupFrame *groupRPC  = new TGGroupFrame(fHL7, "RPC sectors",kHorizontalFrame);
    groupRPC->SetTitlePos(TGGroupFrame::kCenter);

    for(Int_t s = 0; s < 6; s ++){
	fCheckRPCSec[s] = new TGCheckButton(groupRPC, new TGHotString(Form("S%i",s)),IDs.GetUnID());
	groupRPC->AddFrame(fCheckRPCSec[s]);
    }
    fHL7->AddFrame(groupRPC, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));
    fHLMaster->AddFrame(fHL7);
    //-------------------------------------------------------------

    //-------------------------------------------------------------
    TString namegroup = "SHOWER sectors";
    if(HEDTransform::isEmc()) namegroup ="ECAL sectors";

    TGHorizontalFrame *fHL8 = new TGHorizontalFrame(fHLMaster, width, height);
    TGGroupFrame *groupSHOWER  = new TGGroupFrame(fHL8, namegroup.Data(),kHorizontalFrame);
    groupSHOWER->SetTitlePos(TGGroupFrame::kCenter);

    for(Int_t s = 0; s < 6; s ++){
	fCheckSHOWERSec[s] = new TGCheckButton(groupSHOWER, new TGHotString(Form("S%i",s)),IDs.GetUnID());
	groupSHOWER->AddFrame(fCheckSHOWERSec[s]);
    }
    fHL8->AddFrame(groupSHOWER, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));
    fHLMaster->AddFrame(fHL8);
    //-------------------------------------------------------------


    //-------------------------------------------------------------
    // redraw event button
    TGHorizontalFrame *fHL4 = new TGHorizontalFrame(fHLMaster, width, height, kFixedWidth);


    fRedraw = new TGTextButton   (fHL4,"Redraw", IDs.GetUnID());
    fRedraw->SetToolTipText("Click here to redraw display after option change");
    fRedraw->Connect("Clicked()", "HEDSetup", this , "reDraw()");
    fHL4->AddFrame(fRedraw, new TGLayoutHints(kLHintsExpandX));
    fHLMaster->AddFrame(fHL4, new TGLayoutHints(kLHintsCenterX, 1, 1, 1, 1));
    //-------------------------------------------------------------


    AddFrame(fHLMaster);


    //-------------------------------------------------------------
    //Default state

    for(Int_t i=0;i<6;i++){  fCheckSector[i]->SetOn(); } // default on
    for(Int_t s=0;s<6;s++){
	fCheckTOFSec[s]   ->SetOn();
	fCheckRPCSec[s]   ->SetOn();
	fCheckSHOWERSec[s]->SetOn();
	for(Int_t m=0;m<4;m++){
	    fCheckMDCMod[s][m]->SetOn();
	}
    } // default on
    fCheckRICH  ->SetOn();
    for(Int_t i=0;i<4;i++){ fCheckMDC[i]->SetOn();}
    fCheckTOF   ->SetOn();
    fCheckRPC   ->SetOn();
    fCheckSHOWER->SetOn();
    fCheckWALL  ->SetOn();
    fCheckFRAMES->SetOn();
    fCheckCOILS ->SetOn();

    fCheckTimed    ->SetOn(kFALSE);
    fNumberLoopTime->GetTGNumberEntry()->SetIntNumber(5);
    fNumberLoopTime->GetTGNumberEntry()->SetLimits(TGNumberFormat::kNELLimitMinMax,1,100);

    SetWindowName("Display Setup");
    MapSubwindows();
    Resize(GetDefaultSize()); //
    MapWindow();
    //-------------------------------------------------------------

}

HEDSetup::~HEDSetup()
{
    // Destructor.
    cout<<"CLEANUP"<<endl;
    Cleanup();
}

void HEDSetup::reDraw(){
    HEDColorDef* colorDef = HEDColorDef::getEDColorDef();

    Bool_t isOn = kFALSE;

    isOn = fCheckRICH->IsOn();
    colorDef->showRich(isOn);

    isOn = fCheckWALL->IsOn();
    colorDef->showWall(isOn);

    for(Int_t i=0;i<4;i++){
	isOn = fCheckMDC[i]->IsOn();
	for(Int_t s = 0; s < 6; s++){
	    colorDef->showMdc(isOn,s,i);
	}
    }

    isOn = fCheckTOF->IsOn();
    colorDef->showTof(isOn,-1);

    isOn = fCheckRPC->IsOn();
    colorDef->showRpc(isOn,-1);

    isOn = fCheckSHOWER->IsOn();
    colorDef->showShower(isOn,-1);

    isOn = fCheckCOILS->IsOn();
    colorDef->showCoils(isOn);

    isOn = fCheckFRAMES->IsOn();
    colorDef->showFrames(isOn);


    for(Int_t s = 0; s < 6; s++){
	isOn = fCheckSector[s]->IsOn();
        colorDef->showSec(isOn,s);
    }

    for(Int_t s = 0; s < 6; s++){
	for(Int_t m = 0; m < 4; m++){

	    if(fCheckMDC[m]->IsOn()       &&  // MDC    has priority
	       fCheckSector[s]->IsOn()        // sector has priority
	      ) {
		isOn = fCheckMDCMod[s][m]->IsOn();
		colorDef->showMdc(isOn,s,m);
	    }
	}
    }

    for(Int_t s = 0; s < 6; s++){
	if(fCheckTOF->IsOn()       &&  // TOF    has priority
	   fCheckSector[s]->IsOn()     // sector has priority
	  ) {
	    isOn = fCheckTOFSec[s]->IsOn();
	    colorDef->showTof(isOn,s);
	}
    }

    for(Int_t s = 0; s < 6; s++){
	if(fCheckRPC->IsOn()       &&  // RPC    has priority
	   fCheckSector[s]->IsOn()     // sector has priority
	  ) {
	    isOn = fCheckRPCSec[s]->IsOn();
	    colorDef->showRpc(isOn,s);
	}
    }

    for(Int_t s = 0; s < 6; s++){
	if(fCheckSHOWER->IsOn()       &&  // SHOWER has priority
	   fCheckSector[s]->IsOn()        // sector has priority
	  ) {
	    isOn = fCheckSHOWERSec[s]->IsOn();
	    colorDef->showShower(isOn,s);
	}
    }
    colorDef->colRich  (colRICH  ->GetColor(),transRICH  ->GetIntNumber());
    for(Int_t m = 0; m < 4; m++){
	colorDef->colMdc(colMDC[m]->GetColor(),transMDC[m]->GetIntNumber(),m);
    }
    colorDef->colTof   (colTOF   ->GetColor(),transTOF   ->GetIntNumber());
    colorDef->colRpc   (colRPC   ->GetColor(),transRPC   ->GetIntNumber());
    colorDef->colShower(colSHOWER->GetColor(),transSHOWER->GetIntNumber());
    colorDef->colWall  (colWALL  ->GetColor(),transWALL  ->GetIntNumber());
    colorDef->colFrames(colFRAMES->GetColor(),transFRAMES->GetIntNumber());
    colorDef->colCoils (colCOILS ->GetColor(),transCOILS ->GetIntNumber());


    gEve->GetGlobalScene()->Changed();
    gEve->DoRedraw3D();
}
//----------------------------------------------------------------



