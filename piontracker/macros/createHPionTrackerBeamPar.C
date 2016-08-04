#include "HBeam.h"

#include "hades.h"
#include "hdst.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "hparora2io.h"
#include "hparasciifileio.h"
#include "hparrootfileio.h"
#include "hpiontrackerbeampar.h"


#include "TString.h"

#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>

using namespace std;


Int_t createHPionTrackerBeamPar()
{


    TString inputfile="pibeam_250414_02072014.data";
    Int_t targetNum  =32;
    Bool_t create    =kTRUE; // kTRUE : setup beam in createAscii, kFALSE : init from existing par file
    Int_t runID      = 13001; // pion13 med field sim

    //--------------------------------------------------
    // output setting
    Int_t output     =  1;    // 1: ascii 2:root 3:ora
    if(create) output= 0;
    TString outfile  = "test"; // for root/ascii  .root or .txt will be apended
    //--------------------------------------------------

    //--------------------------------------------------
    // input setting
    TString rootParFile  ="";
    //TString asciiParFile ="dummy.txt";
    TString asciiParFile ="HPionTrackerBeamPar_mod.txt";
    TString paramSource = "ascii"; // root, ascii, oracle
    //--------------------------------------------------

    map<Int_t,TString> elementNames;

    elementNames[0]  = "in_Q1";
    elementNames[1]  = "out_Q1";
    elementNames[2]  = "in_Q2";
    elementNames[3]  = "out_Q2";
    elementNames[4]  = "out_Q2_+_0.6_m";
    elementNames[5]  = "in_FOPI_1";
    elementNames[6]  = "in_FOPI_2";
    elementNames[7]  = "out_FOPI_1";
    elementNames[8]  = "out_FOPI_2";
    elementNames[9]  = "in_dip_1";
    elementNames[10] = "out_dip_1";
    elementNames[11] = "in_Q3";
    elementNames[12] = "out_Q3";
    elementNames[13] = "in_Q4";
    elementNames[14] = "out_Q4";
    elementNames[15] = "inter_focal_plane";
    elementNames[16] = "detector_1";
    elementNames[17] = "in_Q5";
    elementNames[18] = "out_Q5";
    elementNames[19] = "in_Q6";
    elementNames[20] = "out_Q6";
    elementNames[21] = "in_dip_2";
    elementNames[22] = "out_dip_2";
    elementNames[23] = "in_Q7";
    elementNames[24] = "out_Q7";
    elementNames[25] = "detector_2";
    elementNames[26] = "in_Q8";
    elementNames[27] = "out_Q8";
    elementNames[28] = "in_Q9";
    elementNames[29] = "out_Q9";
    elementNames[30] = "inter_point";
    elementNames[31] = "HADES_target";
    elementNames[32] = "target";

    Int_t cuttup[] = {      // 0 = no, 1=radial, 2=box
	1,1,1,1,2, //5
	2,2,2,2,2, //10
	2,1,1,1,1, //15
	1,2,1,1,1, //20
	1,2,2,1,1, //25
	1,1,1,1,1, //30
	1,1,1
    };

    Double_t xcut[] =  {   // [mm]

	60.,60.,60.,60.,70.,   //5
	70.,70.,70.,70.,90.,   //10
	90.,60.,60.,60.,60.,   //15
	60.,50.,60.,60.,60.,   //20
	60.,90.,90.,60.,60.,   //25
	60.,60.,60.,60.,60.,   //30
	60.,60.,60
    };

    Double_t ycut[] =        // [mm]
    {
	60.,60.,60.,60.,35., //5
	35.,35.,35.,35.,35., //10
	35.,60.,60.,60.,60., //15
	60.,50.,60.,60.,60., //20
	60.,35.,35.,60.,60., //25
	60.,60.,60.,60.,60., //30
	60.,60.,60
    };





    Hades* myHades=new Hades;
    HSpectrometer* spec=gHades->getSetup();
    HRuntimeDb* rtdb=gHades->getRuntimeDb();

    Int_t mdcMods[6][4]=
    { {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1} };

    // recommendations from Vladimir+Olga
    // according to params from 28.04.2011
    Int_t nLayers[6][4] = {
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6} };


    HDst::setupSpectrometer("may14",mdcMods,"piontracker");
    // beamtime mdcMods_apr12, mdcMods_full
    // Int_t mdcset[6][4] setup mdc. If not used put NULL (default).
    // if not NULL it will overwrite settings given by beamtime
    // detectors (default)= rich,mdc,tof,rpc,shower,wall,tbox,start

    HDst::setupParameterSources(paramSource,asciiParFile,rootParFile,"now");
    //HDst::setupParameterSources("oracle",asciiParFile,rootParFile,"now"); // use to create param file
    // parsource = oracle,ascii,root (order matters)
    // if source is "ascii" a ascii param file has to provided
    // if source is "root" a root param file has to provided
    // The histDate paramter (default "now") is used wit the oracle source

    //HParRootFileIo* inp1=new HParRootFileIo;
    //TString inputFile="/misc/kempter/Calibration/Nov2001/params_final/out/mdcNov01_All_160902.root";


    HPionTrackerBeamPar* par = (HPionTrackerBeamPar*) rtdb->getContainer("PionTrackerBeamPar");


    if(!create){
	rtdb->initContainers(runID);
	rtdb->setContainersStatic();
    }

    HBeam& pionbeam = par->getBeam();

    if(create){
	if(!par->initBeamLine  (inputfile,targetNum)) return 1;  // transform input file and target element number
	vector<HBeamElement>& elements  = pionbeam.getElements();

	if(elementNames.size() != elements.size()) {
	    cout<<"Number of elements differs from name map! nelements = "<<elements.size()<<", map size = "<<elementNames.size()<<endl;
	    return 1;
	}

	for(UInt_t i = 0 ; i < elements.size(); i++){
	    elements[i].setElement(elementNames[i],cuttup[i],xcut[i],ycut[i]);
	}

	HBeamElement* etarg = par->getBeamElement(32);
         /*
	   T44  target  code  0.32059 file   0.32118
	   T436 target  code -8.291   file  -8.383
	   T446 target  code -0.3796  file  -0.3817

	etarg->Tij[3][3]    =  0.32059;
        etarg->Tij[3][2][4] = -8.291;
        etarg->Tij[3][3][4] = -0.3796;
         */
	par->createAsciiFile("HPionTrackerBeamPar.txt");

    }
     pionbeam.printBeamLine(kTRUE);   // kTRUE : print transform matrices in addition to name and distance


    if(output==3){
	HParOra2Io* ora=new HParOra2Io;
	// ora->open("mdc_oper");
	ora->open("halo");
	rtdb->setOutput(ora);
	rtdb->setSecondInput(ora);
    } else if(output==2){

	HParRootFileIo* ora=new HParRootFileIo;
	ora->open(Form("%s.root",outfile.Data()),"RECREATE");
	rtdb->setOutput(ora);

    } else if (output==1){
	HParAsciiFileIo* ora=new HParAsciiFileIo;
	ora->open(Form("%s.txt",outfile.Data()),"out");
	rtdb->setOutput(ora);
    }

    delete gHades;
    return 0;

}
