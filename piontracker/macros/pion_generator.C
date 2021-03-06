#include "hphysicsconstants.h"
#include "HBeam.h"
#include "hhistmap.h"

#include "TRandom.h"
#include "TH1F.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TCanvas.h"
#include "TString.h"
#include "TF1.h"

#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>

using namespace std;


Int_t pion_generator(Int_t nEvents=100000,TString outfile="test_pion_0.01_0.05_0.6.evt")
{
    map<Int_t,TString> elementNames;

    elementNames[0]  = "in_Q1";
    elementNames[1]  = "out_Q1";
    elementNames[2]  = "in_Q2";
    elementNames[3]  = "out_Q2";
    elementNames[4]  = "out_Q2_+_0.6_m";
    elementNames[5]  = "in_FOPI";
    elementNames[6]  = "in_FOPI";
    elementNames[7]  = "out_FOPI";
    elementNames[8]  = "out_FOPI";
    elementNames[9]  = "in_dip_1";
    elementNames[10] = "out_dip_1";
    elementNames[11] = "in_Q3";
    elementNames[12] = "out_Q3";
    elementNames[13] = "in_Q4";
    elementNames[14] = "out_Q4";
    elementNames[15] = "inter_focal_plane";
    elementNames[16] = "detector_I";
    elementNames[17] = "in_Q5";
    elementNames[18] = "out_Q5";
    elementNames[19] = "in_Q6";
    elementNames[20] = "out_Q6";
    elementNames[21] = "in_dip_2";
    elementNames[22] = "out_dip_2";
    elementNames[23] = "in_Q7";
    elementNames[24] = "out_Q7";
    elementNames[25] = "inter_point_det_2";
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

    HBeam pionbeam;
    pionbeam.setBeam           (HPhysicsConstants::pid("pi-"),1.3,60,60,0.0,0.0); // id, totl mom [GeV], beamtube x and y size, xoff,yoff
    pionbeam.setBeamProfile    (.05,0.0);                                         // sigma [mm], flatradius [mm]
    pionbeam.setBeamResolution (0.01,0.05,0.06);                                  // dpx [rad],dpy [rad] ,dpz [relative]  [+-]
    if(!pionbeam.initBeamLine  ("pibeam_250414.data",32)) return 1;               // transform input file and target element number
    pionbeam.addDetector("det1", -17092.6,2,50.,50.);                             // [mm] relative to HADES 0,0,0
    pionbeam.addDetector("det2", -5400.0,2,50.,50.);                              // [mm] relative to HADES 0,0,0
    pionbeam.addDetector("plane",-1300.0,1,60.,60.);                              // [mm] relative to HADES 0,0,0


    vector<HBeamElement>& elements  = pionbeam.getElements();
    vector<HBeamElement>& detectors = pionbeam.getDetectors();

    if(elementNames.size() != elements.size()) {
	cout<<"Number of elements differs from name map! nelements = "<<elements.size()<<", map size = "<<elementNames.size()<<endl;
        return 1;
    }

    for(UInt_t i = 0 ; i < elements.size(); i++){
        elements[i].setElement(elementNames[i],cuttup[i],xcut[i],ycut[i]);
    }

    pionbeam.printBeamLine(kTRUE);   // kTRUE : print transform matrices in addition to name and distance
    pionbeam.printDetectors();
    pionbeam.printBeamProfile();


    //-----------------------------------------------------------
    // build comma separated string for hist arrays
    TString detnames;
    map <TString,Int_t> nameToInd;
    vector<HBeamParticle>& vhistory = pionbeam.newParticle();
    for(UInt_t i = 0; i < vhistory.size(); i++) {
	if(i<vhistory.size() -1) detnames += vhistory[i].fName + ",";
        else                     detnames += vhistory[i].fName;
	nameToInd[vhistory[i].fName] = i;
    }
    //-----------------------------------------------------------

    Int_t  indexOut        = nameToInd["plane"];
    Int_t  indexDet[]      = { nameToInd["det1"],nameToInd["det2"] };
    Int_t  nDetectors      = vhistory.size();
    Bool_t bWriteDetectors = kFALSE; // true : write det1,det2 as beam particles

    //-----------------------------------------------------------
    Double_t range  = 300;
    TString histfile = outfile;
    histfile.ReplaceAll(".evt","");
    histfile.ReplaceAll(".root","");
    histfile += "_histmap.root";
    HHistMap hM(histfile);


    hM.addHistArray("TH2F","hxy","hxy_[%i]","hxy[%i]"
			 ,2000,-range ,range,2000,-range,range,0,0.,0.
			 ,"x [mm]","y [mm]","counts","xy/",nDetectors,-1,-1,-1,-1,detnames);
    hM.addHistArray("TH1F","hmom","hmom[%i]","hmom[%i]"
			 ,4000,0 ,4, 0,0.,0., 0,0.,0.
			 ,"p [GeV/c]","counts","","mom/",nDetectors,-1,-1,-1,-1,detnames);
    hM.addHistArray("TH1F","hmom_x","hmom_x[%i]","hmom_x[%i]"
			 ,500,-0.10 , 0.10, 0,0.,0., 0,0.,0.
			 ,"p_x [GeV/c]","counts","","mom_x/",nDetectors,-1,-1,-1,-1,detnames);
    hM.addHistArray("TH1F","hmom_y","hmom_y[%i]","hmom_y[%i]"
			 ,500,-0.10 , 0.10, 0,0.,0., 0,0.,0.
			 ,"p_y [GeV/c]","counts","","mom_y/",nDetectors,-1,-1,-1,-1,detnames);
    hM.addHistArray("TH1F","hmom_z","hmom_z[%i]","hmom_z[%i]"
			 ,4000,0 ,4, 0,0.,0., 0,0.,0.
			 ,"p_z [GeV/c]","counts","","mom_Z/",nDetectors,-1,-1,-1,-1,detnames);
    hM.addHistArray("TH2F","hxyAcc","hxyAcc_[%i]","hxyAcc[%i]"
			 ,2000,-range ,range,2000,-range,range,0,0.,0.
			 ,"x [mm]","y [mm]","counts","xy/",nDetectors,-1,-1,-1,-1,detnames);
    hM.addHistArray("TH1F","hmomAcc","hmomAcc[%i]","hmomAcc[%i]"
			 ,4000,0 ,4, 0,0.,0., 0,0.,0.
			 ,"p [GeV/c]","counts","","mom/",nDetectors,-1,-1,-1,-1,detnames);
    hM.addHistArray("TH1F","hmomAcc_x","hmomAcc_x[%i]","hmomAcc_x[%i]"
			 ,500,-0.040 , 0.040, 0,0.,0., 0,0.,0.
			 ,"p_x [GeV/c]","counts","","mom_x/",nDetectors,-1,-1,-1,-1,detnames);
    hM.addHistArray("TH1F","hmomAcc_y","hmomAcc_y[%i]","hmomAcc_y[%i]"
			 ,500,-0.040 , 0.040, 0,0.,0., 0,0.,0.
			 ,"p_y [GeV/c]","counts","","mom_y/",nDetectors,-1,-1,-1,-1,detnames);
    hM.addHistArray("TH1F","hmomAcc_z","hmomAcc_z[%i]","hmomAcc_z[%i]"
			 ,4000,0 ,4, 0,0.,0., 0,0.,0.
			 ,"p_z [GeV/c]","counts","","mom_Z/",nDetectors,-1,-1,-1,-1,detnames);
    hM.addHist     ("TH1F","hAcceptanceElement"    ,"hAcceptanceElement"    ,elements.size(), 0, elements.size(), 0, 0., 0., 0, 0., 0.,"","Acceptance","", "acceptance/");
    hM.addHist     ("TH1F","hAcceptanceAccumulated","hAcceptanceAccumulated",elements.size(), 0, elements.size(), 0, 0., 0., 0, 0., 0.,"","Acceptance","", "acceptance/");
    hM.addHist     ("TH2F","hxElement","hxElement",elements.size(), 0, elements.size(), 200, -range,range, 0, 0., 0.,""," x pos [mm]","", "acceptance/");
    hM.addHist     ("TH2F","hyElement","hyElement",elements.size(), 0, elements.size(), 200, -range,range, 0, 0., 0.,""," y pos [mm]","", "acceptance/");
    hM.addHist     ("TH2F","hxElementTotalAcc","hxElementTotalAcc",elements.size(), 0, elements.size(), 200, -range,range, 0, 0., 0.,""," x pos [mm]","", "acceptance/");
    hM.addHist     ("TH2F","hyElementTotalAcc","hyElementTotalAcc",elements.size(), 0, elements.size(), 200, -range,range, 0, 0., 0.,""," y pos [mm]","", "acceptance/");
    hM.addHist     ("TH1F","hxDir"    ,"hxDir"    ,100, -100, 100, 0,0.,0., 0, 0., 0.," x dir [mrad]"," counts","", "dir/");
    hM.addHist     ("TH1F","hyDir"    ,"hyDir"    ,100, -100, 100, 0,0.,0., 0, 0., 0.," y dir [mrad]"," counts","", "dir/");

    //-------------------------------------------------------
    // special settings
    hM.get("hAcceptanceAccumulated")->SetLineColor(kRed);
    for(Int_t i = 0 ; i < hM.get("hAcceptanceAccumulated")->GetNbinsX(); i ++ ){
          hM.get("hAcceptanceAccumulated")->GetXaxis()->SetBinLabel(i+1,elementNames[i].Data());
          hM.get("hAcceptanceElement")    ->GetXaxis()->SetBinLabel(i+1,elementNames[i].Data());
          hM.get("hxElement")             ->GetXaxis()->SetBinLabel(i+1,elementNames[i].Data());
          hM.get("hyElement")             ->GetXaxis()->SetBinLabel(i+1,elementNames[i].Data());
          hM.get("hxElementTotalAcc")     ->GetXaxis()->SetBinLabel(i+1,elementNames[i].Data());
          hM.get("hyElementTotalAcc")     ->GetXaxis()->SetBinLabel(i+1,elementNames[i].Data());
    }

    hM.get("hAcceptanceAccumulated")->GetXaxis()->LabelsOption("v");
    hM.get("hAcceptanceElement"    )->GetXaxis()->LabelsOption("v");
    hM.get("hxElement"             )->GetXaxis()->LabelsOption("v");
    hM.get("hyElement"             )->GetXaxis()->LabelsOption("v");
    hM.get("hxElementTotalAcc"     )->GetXaxis()->LabelsOption("v");
    hM.get("hyElementTotalAcc"     )->GetXaxis()->LabelsOption("v");
    //-------------------------------------------------------

    hM.addCanvas("cBeam"      ,"Beam profile"            , 4, 2, 1500, 800,"");
    hM.addCanvas("cXY"        ,"xy distributions"        , 4, 2, 1500, 800,"");
    hM.addCanvas("cAcceptance","acceptance distributions", 3, 2, 1500, 800,"");


    Int_t    flag               =  4 ; // if( getVERTEX && writeINDEX == 0)
    Double_t blast              =  0 ; // not available
    Int_t    nParticle          =  1 ;
    if(bWriteDetectors) nParticle = nDetectors-2; // no beam, no target
    Float_t  event_impact_param = -1.; // not available
    Double_t weight             =  1.; // no gen weights
    Int_t    parentID           = -1 ; // no parent
    Int_t    sourceID           = -1 ; // no source

    FILE* asciiFile = 0;
    asciiFile = fopen(outfile.Data(),"w");


    Int_t ctEvents   = 0;
    Int_t ctTotalTry = 0;
    Int_t maxTry     = 100; // max number of allowed try to prevent endless loop
    Bool_t Accepted  = kTRUE;
    //-----------------------------------------------------------
    for(Int_t ctEvt = 1; ctEvt <= nEvents; ctEvt++)
    {

	if(asciiFile) fprintf(asciiFile," %i %i %f %f %i\n",ctEvt,nParticle,blast,event_impact_param,flag);
	ctEvents ++;

	Int_t n             = 0;
	Int_t ctTryParticle = 0;
	while (n < nParticle && ctTryParticle <= maxTry)
	{
            vector<TLorentzVector> vPion;
            
	    //-----------------------------------------------------------
	    // create particles
	    Bool_t reDo = kTRUE;
            Int_t ctTry = 0;
	    while(reDo && ctTry <= maxTry){  // try as long a particle is accepted by the beam line
		ctTotalTry ++;
                ctTry++;

		vector<HBeamParticle>& vhistory = pionbeam.newParticle();
		if(vhistory[vhistory.size()-1].fAccepted){
		    reDo = kFALSE;
		}

		vPion.clear();
		for(UInt_t i = 0; i < vhistory.size(); i++){
		    TLorentzVector vpion;
		    vpion.SetXYZM(vhistory[i].fP.X(),vhistory[i].fP.Y(),vhistory[i].fP.Z(), HPhysicsConstants::mass(vhistory[i].fPid)*0.001);
		    vPion.push_back(vpion);
		}

		//-----------------------------------------------------------
		// qa plots
		hM.get("hxDir")->Fill(TMath::ATan2(vPion[0].Px(),vPion[0].Pz())*1000);
                hM.get("hyDir")->Fill(TMath::ATan2(vPion[0].Py(),vPion[0].Pz())*1000);

		for(UInt_t i=0; i< vhistory.size();i++){
		    hM.get2("hxy"   ,i)->Fill(vhistory[i].fPos.X(),vhistory[i].fPos.Y());
		    hM.get ("hmom"  ,i)->Fill(vPion   [i].P());
		    hM.get ("hmom_x",i)->Fill(vPion   [i].Px());
		    hM.get ("hmom_y",i)->Fill(vPion   [i].Py());
		    hM.get ("hmom_z",i)->Fill(vPion   [i].Pz());
		}

		Accepted = kTRUE;
		for(UInt_t i = 0 ; i < elements.size(); i++){
		    hM.get2("hxElement")->Fill(i,elements[i].fout[0]);
		    hM.get2("hyElement")->Fill(i,elements[i].fout[2]);

		    if(!elements[i].fAccepted) Accepted = kFALSE;

		    if(Accepted) hM.get("hAcceptanceAccumulated")->Fill(i);
		}

		//-----------------------------------------------------------

	    } // end while redo

	    if(ctTry == maxTry){ //
		ctTryParticle++;
		cout<<"evt "<<ctEvt<<" no accepted beam particle after "<<ctTry<<" attempts! redo one particle "<<ctTryParticle<<endl;

		if(ctTryParticle == maxTry)  {
		    cout<<"ERROR : Too many attempts. Outputfile will be corrupted (empty or to small event)"<<endl;
		    if(asciiFile) fclose(asciiFile);

                    return 1;
		}

		continue;
	    }

	    if(vhistory.size() < 3) {
		cout<<"no history available!"<<endl;
		continue;
	    }


	    n++; // succesfull

            
	    //-----------------------------------------------------------

	    //-----------------------------------------------------------
            // new particle
	    if(asciiFile )
	    {
		//target first
                UInt_t itarget = indexOut ;
		fprintf(asciiFile," %e %e %e %e %e %e %e %e %i %i %i %e\n",                 // 12 vars
			vPion[itarget].E(),vPion[itarget].Px(),vPion[itarget].Py(),vPion[itarget].Pz(),
			0.       ,vhistory[itarget].fPos.X(),vhistory[itarget].fPos.Y(),vhistory[itarget].fPos.Z(),
			vhistory[itarget].fPid , sourceID , parentID, weight);
		if(bWriteDetectors){

                    UInt_t ndet = sizeof(indexDet)/sizeof(Int_t);
		    for(UInt_t j = 0; j < ndet; j ++){   // write out wanted beam detector hits
                        Int_t i = j;
			fprintf(asciiFile," %e %e %e %e %e %e %e %e %i %i %i %e\n",                 // 12 vars
				vPion[i].E(),vPion[i].Px(),vPion[i].Py(),vPion[i].Pz(),
				0.       ,vhistory[i].fPos.X(),vhistory[i].fPos.Y(),vhistory[i].fPos.Z(),
				vhistory[i].fPid , sourceID , parentID, weight);
		    }
		}
	    }
	    //-----------------------------------------------------------

	    //-----------------------------------------------------------
	    // qa plots
	    if(Accepted){
		for(UInt_t i=0; i< vhistory.size();i++){
		    hM.get2("hxyAcc"   ,i)->Fill(vhistory[i].fPos.X(),vhistory[i].fPos.Y());
		    hM.get ("hmomAcc"  ,i)->Fill(vPion   [i].P());
		    hM.get ("hmomAcc_x",i)->Fill(vPion   [i].Px());
		    hM.get ("hmomAcc_y",i)->Fill(vPion   [i].Py());
		    hM.get ("hmomAcc_z",i)->Fill(vPion   [i].Pz());
		}
		for(UInt_t i = 0 ; i < elements.size(); i++){
		    hM.get2("hxElementTotalAcc")->Fill(i,elements[i].fout[0]);
		    hM.get2("hyElementTotalAcc")->Fill(i,elements[i].fout[2]);
		}
	    }
	    //-----------------------------------------------------------


	} // end particle loop

    } // end event loop
    //-----------------------------------------------------------

    for(UInt_t i = 0 ; i < elements.size(); i++){
	hM.get("hAcceptanceElement")->SetBinContent(i+1,elements[i].fCtAll > 0 ? 100 - (elements[i].fCtFail/(Double_t)elements[i].fCtAll)*100 : 100);
    }
    hM.get("hAcceptanceAccumulated")->Scale(1./(elements[0].fCtAll/100.));



    if(asciiFile) fclose(asciiFile);

    pionbeam.printBeamLine(kFALSE);   // print acceptance statistic for detecors and elements
    pionbeam.printDetectors();



    hM.getCanvas("cBeam")->cd(1);
    gPad->SetLogz();
    hM.get2("hxy",nameToInd["beam"])->GetXaxis()->SetRangeUser(-5,5);
    hM.get2("hxy",nameToInd["beam"])->GetYaxis()->SetRangeUser(-5,5);
    hM.get2("hxy",nameToInd["beam"]) ->Draw("colz");  // beam

    hM.getCanvas("cBeam")->cd(2);
    hM.get("hmom",nameToInd["beam"])->Draw();    // beam
    hM.getCanvas("cBeam")->cd(3);
    hM.get("hmom_x",nameToInd["beam"])->Draw();  // beam
    hM.getCanvas("cBeam")->cd(4);
    hM.get("hmom_y",nameToInd["beam"])->Draw();  // beam
    hM.getCanvas("cBeam")->cd(5);
    hM.get("hmom_z",nameToInd["beam"])->Draw();  // beam
    hM.getCanvas("cBeam")->cd(6);
    hM.get("hxDir")->Draw();  // beam
    hM.getCanvas("cBeam")->cd(7);
    hM.get("hyDir")->Draw();  // beam


    hM.getCanvas("cXY")->cd(1);
    gPad->SetLogz();
    hM.get2("hxy",nameToInd["det1"])->GetXaxis()->SetRangeUser(-60,60);
    hM.get2("hxy",nameToInd["det1"])->GetYaxis()->SetRangeUser(-200,200);
    hM.get2("hxy",nameToInd["det1"]) ->Draw("colz");  // det1
    hM.getCanvas("cXY")->cd(2);
    gPad->SetLogz();
    hM.get2("hxy",nameToInd["det2"])->GetXaxis()->SetRangeUser(-60,60);
    hM.get2("hxy",nameToInd["det2"])->GetYaxis()->SetRangeUser(-300,300);
    hM.get2("hxy",nameToInd["det2"]) ->Draw("colz");  // det2
    hM.getCanvas("cXY")->cd(3);
    gPad->SetLogz();
    hM.get2("hxy",nameToInd["plane"])->GetXaxis()->SetRangeUser(-30,30);
    hM.get2("hxy",nameToInd["plane"])->GetYaxis()->SetRangeUser(-200,200);
    hM.get2("hxy",nameToInd["plane"]) ->Draw("colz");  // plane
    hM.getCanvas("cXY")->cd(4);
    gPad->SetLogz();
    hM.get2("hxy",nameToInd["target"])->GetXaxis()->SetRangeUser(-20,20);
    hM.get2("hxy",nameToInd["target"])->GetYaxis()->SetRangeUser(-20,20);
    hM.get2("hxy",nameToInd["target"]) ->Draw("colz");  // target
    hM.getCanvas("cXY")->cd(5);
    gPad->SetLogz();
    hM.get2("hxyAcc",nameToInd["det1"])->GetXaxis()->SetRangeUser(-60,60);
    hM.get2("hxyAcc",nameToInd["det1"])->GetYaxis()->SetRangeUser(-200,200);
    hM.get2("hxyAcc",nameToInd["det1"]) ->Draw("colz");  // det1
    hM.getCanvas("cXY")->cd(6);
    gPad->SetLogz();
    hM.get2("hxyAcc",nameToInd["det2"])->GetXaxis()->SetRangeUser(-60,60);
    hM.get2("hxyAcc",nameToInd["det2"])->GetYaxis()->SetRangeUser(-300,300);
    hM.get2("hxyAcc",nameToInd["det2"]) ->Draw("colz");  // det2
    hM.getCanvas("cXY")->cd(7);
    gPad->SetLogz();
    hM.get2("hxyAcc",nameToInd["plane"])->GetXaxis()->SetRangeUser(-30,30);
    hM.get2("hxyAcc",nameToInd["plane"])->GetYaxis()->SetRangeUser(-200,200);
    hM.get2("hxyAcc",nameToInd["plane"]) ->Draw("colz");  // plane
    hM.getCanvas("cXY")->cd(8);
    gPad->SetLogz();
    hM.get2("hxyAcc",nameToInd["target"])->GetXaxis()->SetRangeUser(-20,20);
    hM.get2("hxyAcc",nameToInd["target"])->GetYaxis()->SetRangeUser(-20,20);
    hM.get2("hxyAcc",nameToInd["target"]) ->Draw("colz");  // target


    hM.getCanvas("cAcceptance")->cd(1);
    hM.get("hAcceptanceElement")->Draw();
    hM.get("hAcceptanceAccumulated")->Draw("same");

    hM.getCanvas("cAcceptance")->cd(2);
    gPad->SetLogz();
    hM.get2("hxElement")->Draw("colz");

    hM.getCanvas("cAcceptance")->cd(3);
    gPad->SetLogz();
    hM.get2("hyElement")->Draw("colz");

    hM.getCanvas("cAcceptance")->cd(4);
    gPad->SetLogz();
    hM.get2("hxElementTotalAcc")->Draw("colz");

    hM.getCanvas("cAcceptance")->cd(5);
    gPad->SetLogz();
    hM.get2("hyElementTotalAcc")->Draw("colz");

    hM.writeHists();


    cout<<"Total Acceptance : "<< (ctTotalTry > 0 ?  (ctEvents/(Double_t)ctTotalTry)*100 : 100)<<" %"<<endl;

    return 0;
}
