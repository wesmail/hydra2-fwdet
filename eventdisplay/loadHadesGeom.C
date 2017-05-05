#ifndef __LOADHADESGEOM__
#define __LOADHADESGEOM__

#include "TFile.h"
#include "TSystem.h"
#include "TString.h"

#include "TEveManager.h"
#include "TEveElement.h"
#include "TEveScene.h"
#include "TEveTrans.h"
#include "TEveArrow.h"
#include "TEveText.h"

#include "TEveGeoNode.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoVolume.h"
#include "TGeoSphere.h"


#include "TRegexp.h"
#include "TGListTree.h"


#include "hedgui.h"
#include "hedhelpers.h"
#include "hgeomvector.h"

#include <iostream>
#include <list>


using namespace std;

//---------------------------------------------------------
//             LOAD HADES GEOM
// Reads HADES geometry from root file containing TGeoManager.
// All volumes are set invisible by default and only some
// selected volumes are set visible again with the desired
// color and transparency. If the TEveManager does not
// exist it will be created. The Geometry will be added
// to the global scene (persistent). The pointer to the used
// TGeoVolumes and TGeoNodes are stored in HEDColorDef. This
// object is used by the GUI for changing the properties later.
//---------------------------------------------------------

void addAllNodes(TObjArray* mother, TObjArray* allnodes){
    // add rekursive all daughters to array allnodes

    if(mother){
	allnodes->AddAll(mother);
	for(Int_t i=0; i<mother->GetEntries(); i++){
	    TGeoNode* d1 = (TGeoNode*)mother->At(i);
	    TObjArray* d2 = d1->GetNodes();
	    addAllNodes(d2,allnodes);
	}
    }
}



Bool_t loadHadesGeom(TString geomFile="GeomManager_Sep08_rpc.root")
{

    if(!gEve) TEveManager::Create(kTRUE);

    HEDColorDef* colorDef = HEDColorDef::getEDColorDef();
    colorDef->setDefault();


    if(gSystem->AccessPathName(geomFile.Data())){
	cerr<<"EVENT DISPLAY : ROOT GeomManager FILE NOT FOUND ! #######################"<<endl;
	return kFALSE;
    }

    TFile geomInput(geomFile.Data());
    geomInput.GetListOfKeys()->Print();
    TGeoManager *geom = (TGeoManager*)geomInput.Get("HadesGeom");
    if(!geom){
	cerr<<"EVENT DISPLAY : HadesGeom OBJECT NOT FOUND IN FILE ! ####################"<<endl;
	return kFALSE;
    }

    geom->CloseGeometry();

    Bool_t drawMDC    = kTRUE;
    Bool_t drawRICH   = kTRUE;
    Bool_t drawTOF    = kTRUE;
    Bool_t drawSHOWER = kTRUE;
    Bool_t drawRPC    = kTRUE;
    Bool_t drawWALL   = kTRUE;
    Bool_t drawCOILS  = kTRUE;
    Bool_t drawFRAMES = kTRUE;


    TRegexp regMDC         ("D[1234]C4");
    TRegexp regMDCMOTHER   ("DR[1234]M");
    TRegexp regTOF         ("T[123456789].S");
    TRegexp regTOFMOTHER   ("T[12345678].F_");
    TRegexp regSHOWER      ("SH[123]M");
    TRegexp regSHOWERMOTHER("SHK[123456]");
    TRegexp regEMC         ("GLEA");
    TRegexp regEMCMOTHER   ("GMOM_[123456]");

    TString patternrich;
    if(!HEDTransform::isNewRich()) patternrich="RPAD";
    else                           patternrich="RDET";

    TRegexp regRICH        (patternrich.Data());
    TRegexp regRICHMIR     ("RMIR");
    TRegexp regRPC         ("EG[123456789].");
    TRegexp regRPCMOTHER   ("EBOX_[123456]");

    cout<<"volumes  "  << gGeoManager->GetListOfVolumes()->GetEntries()<<endl;

    TGeoNode *node = gGeoManager->GetTopNode();
    TObjArray* allnodes = node->GetNodes();
    TString sector="";
    TString result="";


    //------------------------------------------------------------
    // loop volumes to define color + visability
    // sector,RICH,FRAMES and COILS are switched
    // in the GUI by the pointers to the volumes
    TObjArray* allvolumes = gGeoManager->GetListOfVolumes();
    for(Int_t i=0; i<allvolumes->GetEntries(); i++){
	TGeoVolume* vol     = (TGeoVolume*)allvolumes->At(i);
	vol->SetInvisible();
	TString name = vol->GetName();

	result = name(regMDC);
	if(result != "") {
            Int_t m = -1;
	    if(name = "D1C4")      m = 0;
            else if(name = "D2C4") m = 1;
	    else if(name = "D3C4") m = 2;
	    else if(name = "D4C4") m = 3;
	    else {
		cout<<"Volume name = "<<name.Data()<<" not found!"<<endl;
		exit(1);
	    }
	    vol->SetLineColor(colorDef->colMDC[m]);
	    if(drawMDC)vol->SetVisibility(kTRUE);
	    vol->SetTransparency(colorDef->transMDC[0]);
	    colorDef->volMDC.AddLast(vol);
	}

	result = name(regTOF);
	if(result != ""){
	    vol->SetLineColor(colorDef->colTOF);
	    if(drawTOF)vol->SetVisibility(kTRUE);
	    vol->SetTransparency(colorDef->transTOF);
	    colorDef->volTOF.AddLast(vol);
	}
	result = name(regRPC);
	if(result != ""){
	    vol->SetLineColor(colorDef->colRPC);
	    if(drawRPC)vol->SetVisibility(kTRUE);
	    vol->SetTransparency(colorDef->transRPC);
	    colorDef->volRPC.AddLast(vol);
	}

	result = name(regSHOWER);
	if(result != ""){
	    vol->SetLineColor(colorDef->colSHOWER);
	    if(drawSHOWER)vol->SetVisibility(kTRUE);
	    vol->SetVisDaughters(kFALSE);
	    vol->SetTransparency(colorDef->transSHOWER);
	    colorDef->volSHOWER.AddLast(vol);
	}

	result = name(regEMC);
	if(result != ""){
	    vol->SetLineColor(colorDef->colSHOWER);
	    if(drawSHOWER)vol->SetVisibility(kTRUE);
	    vol->SetVisDaughters(kFALSE);
	    vol->SetTransparency(colorDef->transSHOWER);
	    colorDef->volSHOWER.AddLast(vol);
	}

	result = name(regRICH);
	if(result != ""){
	    vol->SetLineColor(colorDef->colRICH);
	    if(drawRICH)vol->SetVisibility(kTRUE);
	    vol->SetTransparency(colorDef->transRICH);
	    colorDef->volRICH.AddLast(vol);
	}

	result = name(regRICHMIR);
        if(result != ""){
	    vol->SetLineColor(colorDef->colRICH);
	    if(drawRICH)vol->SetVisibility(kTRUE);
	    vol->SetTransparency(colorDef->transRICH);
	    colorDef->volRICH.AddLast(vol);
	}

	if(name == "WALL"){
	    cout<<"wall volume!#####################################3"<<endl;
	    vol->SetLineColor(colorDef->colWALL);
	    if(drawWALL)vol->SetVisibility(kTRUE);
	    vol->SetVisDaughters(kFALSE);
	    vol->SetTransparency(colorDef->transWALL);
	    colorDef->volWALL.AddLast(vol);
	}


	if(name == "FBOT"){
	    vol->SetLineColor(kGray);
	    vol->SetVisibility(kTRUE);
	    vol->SetTransparency(60);
	}
	if(name.Contains("FRA") != 0 ||
	   name.Contains("FMKU")!= 0 ||
	   name.Contains("FMKC")!= 0
	  ){
	    vol->SetLineColor(colorDef->colFRAMES);
	    if(drawFRAMES)vol->SetVisibility(kTRUE);
	    vol->SetVisRaytrace(kFALSE);
	    vol->SetVisDaughters(kFALSE);
	    vol->SetTransparency(colorDef->transFRAMES);
	    colorDef->volFRAMES.AddLast(vol);
	}

	if(name.Contains("CRVC")!= 0 ||
	   name.Contains("CVFC")!= 0 ||
	   name.Contains("CMTT")!= 0 ||
	   name.Contains("CMBT")!= 0 ||
	   name.Contains("CFVC")!= 0
	  ){
	    vol->SetLineColor(colorDef->colCOILS);
	    if(drawCOILS)vol->SetVisibility(kTRUE);
	    vol->SetVisDaughters(kFALSE);
	    vol->SetTransparency(colorDef->transCOILS);
	    colorDef->volCOILS.AddLast(vol);
	}
    }
    //------------------------------------------------------------


    //------------------------------------------------------------
    // loop nodes to collect pointers to TGeoNode
    // for switching of detectors in th GUI where
    // sector is needed
    for(Int_t i=0; i<allnodes->GetEntries(); i++){
	TGeoNode* n     = (TGeoNode*)allnodes->At(i);
	TGeoVolume* vol = n->GetVolume();

	TString name = n->GetName();


	if(name.Contains("SEC")) {

	    if(name.Contains("SEC")) {
		colorDef->volSEC.AddLast(vol);
	    }

	    sector = name;
	    TObjArray* secnodes = new TObjArray();
	    addAllNodes(n->GetNodes(),secnodes);

	    for(Int_t j = 0; j<secnodes->GetEntries(); j++){
		TGeoNode* nsec  = (TGeoNode*)secnodes->At(j);

		TString namesec = nsec->GetName();

		result = namesec(regMDCMOTHER);
		if(result != "") {
		    colorDef->nodesMDC.AddLast(nsec);
		}
                result = namesec(regTOFMOTHER);
		if(result != "") {
		    colorDef->nodesTOF.AddLast(nsec);
		}
                result = namesec(regRPCMOTHER);
		if(result != "") {
		    colorDef->nodesRPC.AddLast(nsec);
		}
                result = namesec(regSHOWERMOTHER);
		if(result != "") {
		    colorDef->nodesSHOWER.AddLast(nsec);
		}
                result = namesec(regEMCMOTHER);
		if(result != "") {
		    colorDef->nodesSHOWER.AddLast(nsec);
		}
	    }

	}
	else {
	    sector = "";

	}
    }
    //------------------------------------------------------------




    //------------------------------------------------------------
    // exploded view
    if(0)
    {

        Double_t richshift    = -230;
        Double_t imdcshift    = -150;
        Double_t omdcshift    =   80;
        Double_t rpcshift     =  180;
        Double_t tofshift     =  180;
        Double_t showershift  =  230;
        TGeoNode* node;
	TGeoMatrix* matrix;

	{   // RICH
	    gGeoManager->cd("CAVE_1/RICH_1");
	    node = gGeoManager->GetCurrentNode();
	    matrix = node->GetMatrix();
	    matrix->SetDz(richshift);

	    gGeoManager->cd("CAVE_1/RICH_1/RMET_1");
	    node = gGeoManager->GetCurrentNode();
	    node->GetVolume()->SetVisibility(kTRUE);
	    node->GetVolume()->SetVisDaughters(kFALSE);
            node->GetVolume()->SetLineColor(colorDef->colRICH);

	    gGeoManager->cd("CAVE_1/RICH_1/RSPH_1");
	    node = gGeoManager->GetCurrentNode();
	    node->GetVolume()->SetVisibility(kTRUE);
	    node->GetVolume()->SetVisDaughters(kFALSE);
            node->GetVolume()->SetLineColor(colorDef->colRICH);
	}

	{
	    // MAGNET
	    gGeoManager->cd("CAVE_1/FCFS_1");
	    node = gGeoManager->GetCurrentNode();
	    node->GetVolume()->SetVisibility(kTRUE);
	    node->GetVolume()->SetVisDaughters(kFALSE);
	    node->GetVolume()->SetLineColor(colorDef->colCOILS);
	}

	for(Int_t s = 1; s <= 6; s++){

            // TOF
	    for(Int_t i = 15; i <= 22; i++){
		gGeoManager->cd(Form("CAVE_1/SEC%i_1/T%iF_%i",s,i,s));
                node = gGeoManager->GetCurrentNode();
		matrix = node->GetMatrix();
		const Double_t* shift = matrix->GetTranslation();
		matrix->SetDz(shift[2] + tofshift);
	    }

	    {
		// RPC
		gGeoManager->cd(Form("CAVE_1/SEC%i_1/EBOX_%i",s,s));
		node = gGeoManager->GetCurrentNode();
		matrix = node->GetMatrix();
		const Double_t* shift = matrix->GetTranslation();
		matrix->SetDz(shift[2]+rpcshift);
	    }

            if(!HEDTransform::isEmc())
	    {
		// SHOWER
		gGeoManager->cd(Form("CAVE_1/SEC%i_1/SHK%i_1",s,s));
		node = gGeoManager->GetCurrentNode();
		matrix = node->GetMatrix();
		const Double_t* shift = matrix->GetTranslation();
		matrix->SetDz(shift[2]+showershift);
	    } else {
		// EMC
		gGeoManager->cd(Form("CAVE_1/SEC%i_1/GMOM_%i",s,s));
		node = gGeoManager->GetCurrentNode();
		matrix = node->GetMatrix();
		const Double_t* shift = matrix->GetTranslation();
		matrix->SetDz(shift[2]+showershift);
	    }

	    {
		// Inner MDC
		{
		    gGeoManager->cd(Form("CAVE_1/SEC%i_1/DR1M_%i",s,s));
		    node = gGeoManager->GetCurrentNode();
		    matrix = node->GetMatrix();
		    const Double_t* shift = matrix->GetTranslation();
		    matrix->SetDz(shift[2]+imdcshift);
		}
		{
		    gGeoManager->cd(Form("CAVE_1/SEC%i_1/DR2M_%i",s,s));
		    node = gGeoManager->GetCurrentNode();
		    matrix = node->GetMatrix();
		    const Double_t* shift = matrix->GetTranslation();
		    matrix->SetDz(shift[2]+imdcshift+30);
		}
		// Outer MDC
		{
		    gGeoManager->cd(Form("CAVE_1/SEC%i_1/DR3M_%i",s,s));
		    node = gGeoManager->GetCurrentNode();
		    matrix = node->GetMatrix();
		    const Double_t* shift = matrix->GetTranslation();
		    matrix->SetDz(shift[2]+omdcshift);
		}
		{
		    gGeoManager->cd(Form("CAVE_1/SEC%i_1/DR4M_%i",s,s));
		    node = gGeoManager->GetCurrentNode();
		    matrix = node->GetMatrix();
		    const Double_t* shift = matrix->GetTranslation();
		    matrix->SetDz(shift[2]+omdcshift+30);
		}
	    }

	}

    }
    //------------------------------------------------------------



    //------------------------------------------------------------
    // some transformations for RichPadPlane + RichMirror we have
    // to get from GeoManager

    {
	gGeoManager->cd("CAVE_1/RICH_1/RSPH_1/RMIR_1"); // make GeoManager update transformations
	Double_t local [3]={0,0,0};
	Double_t master[3]={0,0,0};
	gGeoManager->LocalToMaster(local,master);
	TGeoNode* nrich = gGeoManager->GetCurrentNode();
	TGeoVolume* volrich = nrich->GetVolume();
	TGeoSphere* sphere = (TGeoSphere*) volrich->GetShape();
	Float_t phi1,phi2,theta1,theta2,r1,r2;
	phi1   = sphere->GetPhi1();
	phi2   = sphere->GetPhi2();
	r2     = sphere->GetRmax();
	r1     = sphere->GetRmin();
	theta1 = sphere->GetTheta1();
	theta2 = sphere->GetTheta2();

	cout<<"RICH MIRROR LAB="<<master[0]<<","<<master[1]<<","<<master[2]
	    <<" Radius min "<<r1
	    <<" Radius max "<<r2
	    <<endl;
	HEDTransform::setRichMirrorTrans(master[0],master[1],master[2],0,0,0);
        HEDTransform::setRichMirror(sphere);
    }

    {

	if(!HEDTransform::isNewRich()) {
	    gGeoManager->cd("CAVE_1/RICH_1/RMET_1/RDET_1/RPAD_1"); // make GeoManager update transformations
	    Double_t local [3]={0,0,0};
	    Double_t master[3]={0,0,0};
	    gGeoManager->LocalToMaster(local,master);
	    cout<<"RICH PAD LAB="<<master[0]<<","<<master[1]<<","<<master[2] <<endl;
	    HEDTransform::setRichSecTrans(master[0],master[1],master[2],-20,0,0);
	}
    }
    //------------------------------------------------------------

    //----------------------------------------------------------
    // geometry scene
    TGeoNode *node1 = gGeoManager->GetTopNode();
    TEveGeoTopNode* en = new TEveGeoTopNode(gGeoManager, node1);
    HEDColorDef::gGeoTopNode = en;
    en->SetVisLevel(10);
    en->GetNode()->GetVolume()->SetVisibility(kFALSE);
    gEve->AddGlobalElement(en);
    //----------------------------------------------------------


    //----------------------------------------------------------
    // Add coordinate axis to  EveManager.
    TString axisName = TString("Coordinate system");
    Float_t axisArrowLength  = 50.;
    Float_t axisArrowTubeR   = 0.02;
    Color_t axisColor        = kMagenta;
    Int_t axisLabelFontSize  = 15;
    Int_t axisLabelOffset    = 5;

    TEveElementList *axis = new TEveElementList(axisName, axisName);
    TEveArrow *xAxis = new TEveArrow(axisArrowLength, 0., 0.);
    xAxis->SetMainColor(axisColor);
    xAxis->SetTubeR(axisArrowTubeR);
    xAxis->SetConeR(axisArrowTubeR);
    xAxis->SetElementNameTitle("X Axis", "X");
    axis->AddElement(xAxis);
    TEveText *xAxisLabel = new TEveText("X");
    xAxisLabel->SetMainColor(axisColor);
    xAxisLabel->SetFontSize(axisLabelFontSize);
    xAxisLabel->SetLighting(kTRUE);
    Double_t trans[3];
    trans[0] = axisArrowLength + axisLabelOffset;
    trans[1] = 0;
    trans[2] = 0;
    xAxisLabel->PtrMainTrans()->SetPos(trans);
    axis->AddElement(xAxisLabel);

    TEveArrow *yAxis = new TEveArrow(0., axisArrowLength, 0.);
    yAxis->SetMainColor(axisColor);
    yAxis->SetTubeR(axisArrowTubeR);
    yAxis->SetConeR(axisArrowTubeR);
    yAxis->SetElementNameTitle("Y Axis", "Y");
    axis->AddElement(yAxis);
    TEveText *yAxisLabel = new TEveText("Y");
    yAxisLabel->SetMainColor(axisColor);
    yAxisLabel->SetFontSize(axisLabelFontSize);
    yAxisLabel->SetLighting(kTRUE);
    trans[0] = 0;
    trans[1] = axisArrowLength + axisLabelOffset;
    trans[2] = 0;
    yAxisLabel->PtrMainTrans()->SetPos(trans);
    axis->AddElement(yAxisLabel);

    TEveArrow *zAxis = new TEveArrow(0., 0., axisArrowLength);
    zAxis->SetMainColor(axisColor);
    zAxis->SetTubeR(axisArrowTubeR);
    zAxis->SetConeR(axisArrowTubeR);
    zAxis->SetElementNameTitle("Z Axis", "Z");
    axis->AddElement(zAxis);
    TEveText *zAxisLabel = new TEveText("Z");
    zAxisLabel->SetMainColor(axisColor);
    zAxisLabel->SetFontSize(axisLabelFontSize);
    zAxisLabel->SetLighting(kTRUE);
    trans[0] = 0;
    trans[1] = 0;
    trans[2] = axisArrowLength + axisLabelOffset;
    zAxisLabel->PtrMainTrans()->SetPos(trans);
    axis->AddElement(zAxisLabel);

    gEve->AddGlobalElement(axis);
    //----------------------------------------------------------

    return kTRUE;
}

#endif
