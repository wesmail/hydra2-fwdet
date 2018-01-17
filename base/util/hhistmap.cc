//*-- Author : Jochen Markert 18.07.2007
#include "hflexfiller.h"
#include "hades.h"
#include "haddef.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hcategorymanager.h"
#include "htool.h"



#include "TH1.h"
#include "TH1S.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2S.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH3S.h"
#include "TH3I.h"
#include "TH3F.h"
#include "TH3D.h"
#include "TSystem.h"
#include "TPRegexp.h"
#include "TRegexp.h"
#include "TROOT.h"

#include <iostream>
#include <fstream>
#include <string>
using namespace std;


//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HHistMap
// helper class to hold a map of histogram, TCanvas, TCut,
// TF1 and TObject (parameters) pointers.
// The HHistMap is streamed into output file and can be
// read back to obtain all hists/canvas/cuts/fits/parmeters
//
//#include "hhistmap.h"
//#include "harray.h"
//#include "htool.h"
//#include "TH1.h"
//#include "TH1F.h"
//#include "TH2F.h"
//#include "TROOT.h"
//#include "TRandom.h"
//#include "TMath.h"
//#include "TObjArray.h"
//#include "TF1.h"
//#include "TMacro.h"
//
//#include <iostream>
//using namespace std;
//
//Bool_t fillGaus(TH1* h ,TString name,HHistMap* map,TObjArray* params)
//{
//    // fill some random gauss with
//    // randomized mean and sigma
//    // h : hist                                    (set automatically)
//    // array : h is part of this array in HHistMap (set automatically)
//    // map : HHistMap owning this object           (set automatically)
//    // params : user params
//
//    // defaults
//    Double_t meanlow  = 10;
//    Double_t meanup   = 10;
//    Double_t siglow   = 2 ;
//    Double_t sigup    = 2 ;
//
//    if(map){
//        HArrayD* par = (HArrayD*) map->getPar("parsGaus");
//	if(par){
//
//	    TArrayD* pars = par->data;
//
//	    meanlow = pars->At(0);
//	    meanup  = pars->At(1);
//	    siglow  = pars->At(2);
//	    sigup   = pars->At(3);
//	}
//    }
//
//    Double_t mean = meanlow + gRandom->Rndm() * (meanup-meanlow);
//    Double_t sig  = siglow  + gRandom->Rndm() * (sigup -siglow );
//
//    if (h)  for(Int_t i = 0 ; i < 1000; i ++ ) h->Fill(gRandom->Gaus(mean,sig));
//
//    return kTRUE;
//}
//
//Bool_t fillRndm(TH1* h ,TString name,HHistMap* map,TObjArray* params)
//{
//    // fill some random
//    // randomized range
//    // h : hist                                    (set automatically)
//    // array : h is part of this array in HHistMap (set automatically)
//    // map : HHistMap owning this object           (set automatically)
//    // params : user params
//
//    // defaults
//    Double_t meanlow  = 10;
//    Double_t meanup   = 10;
//
//    if(map){
//	HArrayD* par = (HArrayD*) map->getPar("parsGaus");
//	if(par){
//	    TArrayD* pars = par->data;
//
//	    meanlow = pars->At(0);
//	    meanup  = pars->At(1);
//	}
//    }
//
//    if (h)  for(Int_t i = 0 ; i < 10000; i ++ ) h->Fill(meanlow + gRandom->Rndm() * (meanup-meanlow));
//
//    return kTRUE;
//}
//
//Bool_t fillMeanRms(TH1* h ,TString name,HHistMap* map,TObjArray* params)
//{
//    // fill mean and rms to some other histogram
//    // h : hist                                    (set automatically)
//    // array : h is part of this array in HHistMap (set automatically)
//    // map : HHistMap owning this object           (set automatically)
//    // params : user params
//
//    // defaults
//
//    if(map&&h){
//	TH1* mean = map->get("h8_mean");
//	TH1* rms  = map->get("h8_rms");
//
//        Int_t index = map->getIndex(name,h);
//	if(mean) {
//	    mean->SetBinContent(index+1,h->GetMean());
//	    mean->SetBinError  (index+1,h->GetMeanError());
//	}
//	if(rms){
//	    rms ->SetBinContent(index+1,h->GetRMS());
//            rms ->SetBinError  (index+1,h->GetRMSError());
//        }
//    }
//
//    return kTRUE;
//}
//
//Bool_t setColor(TH1* h ,TString name,HHistMap* map,TObjArray* params)
//{
//    // set color of hists in array
//    // h : hist                                   (set automatically)
//    // name : h is part of this array in HHistMap (set automatically)
//    // map : HHistMap owning this hist            (set automatically)
//    // params : user params
//
//    if(map){
//        HArrayI* cols = (HArrayI*) map->getPar("colors");
//	if(cols){
//	    Int_t index   = map->getIndex(name,h);     // linear index of this object in array
//	    h->SetLineColor(cols->data->At(index));
//	}
//    }
//    return kTRUE;
//}
//
//void histmap()
//{
//    gROOT->SetBatch();
//
//    HHistMap hM("test.root");  // all hist will go to test.root
//    hM.setSilentFail(kTRUE);   // (default kFALSE), kTRUE: do not emmit errors when hists do not exist in get(), get2() etc.
//    hM.addHist(new TH1F("h","h",100,0,100),"hists/h");  // single hist, stored in subdir hists/h in output
//
//
//    //------------------------------------------------------------
//    // create hist arrays outside HHistMap and add them to HHistMap
//    TH1F* h1[6];
//    for(Int_t sector=0;sector<6;sector++){
//	h1[sector]=new TH1F(Form("h1_s%i",sector),Form("h1_s%i",sector),100,0,100);
//    }
//
//    // create a linear array for 2dim indexing
//    TH1F* h2[24]; // 6x4 like h2[6][4]
//    for(Int_t sector=0;sector<6;sector++){
//	for(Int_t module=0;module<4;module++){
//	    h2[HTool::getLinearIndex(sector,6,module,4)]= new TH1F(Form("h2_s%i_m%i",sector,module),Form("h2_s%i_m%i",sector,module),100,0,100);
//	}
//    }
//
//
//    hM.addHistArray((TH1**)h1,"h1","h1_s%i"    ,"h1_s%i"    ,"hists/h1",6);    // 1dim array size 6
//    hM.addHistArray((TH1**)h2,"h2","h2_s%i_m%i","h2_s%i_m%i","hists/h2",6,4);  // 2dim array size 6x4
//    //------------------------------------------------------------
//
//
//
//
//    //------------------------------------------------------------
//    // create hist arrays inside HHistMap    ( see hM.addHistN(...) and  hM.addHistArrayN(...) for non const hists
//    hM.addHist     ("TH1F","h3","h3",100,0,100,0,0,0,0,0,0,"time1 [ns]","counts","","hists/h3");
//
//    TString secname= "I,II,III,IV,V,VI";
//    TString modname= "I,II,III,IV";
//
//
//    // create hist array with custom names
//    hM.addHistArray("TH1F","h4","h4t1_s[%i]m[%i]","time1s[%i]m[%i]"
//		    ,100,0.,100.
//		    ,0,0.,0.
//		    ,0,0.,0.
//		    ,"time1 [ns]","counts","","hists/h4",6,4,-1,-1,-1,secname,modname);  // create hists with special names;
//
//    hM.addHistArray("TH2F","h5","h5t1_t12_s[%i]m[%i]","time1_t12s[%i]m[%i]"
//		    ,100,0.,100.
//		    ,100,0.,100.
//		    ,0,0.,0.
//		    ,"time1 [ns]","t2-t1","","hists/h5",6,4);
//
//    hM.addHistArray("TH1F","h6","h6t1_s[%i]m[%i]","time1s[%i]m[%i]"
//		    ,100,0.,100.
//		    ,0,0.,0.
//		    ,0,0.,0.
//		    ,"time1 [ns]","counts","","hists/h6",6,4);
//
//    hM.addHistArray("TH1F","h7","h7t1_s[%i]m[%i]","time1s[%i]m[%i]"
//		    ,100,0.,100.
//		    ,0,0.,0.
//		    ,0,0.,0.
//		    ,"time1 [ns]","counts","","hists/h7",6,4);
//
//    hM.addHist("TH1F","h8_mean","h8_mean",24,0,24,0,0,0,0,0,0,"time1 index [ns]","time1 mean [ns]","","hists/h8");
//    hM.addHist("TH1F","h8_rms" ,"h8_rms" ,24,0,24,0,0,0,0,0,0,"time1 index [ns]","time1 rms [ns]" ,"","hists/h8");
//    //------------------------------------------------------------
//
//
//    //------------------------------------------------------------
//    // get multi dimensional arrays created by HHistMap.
//    // delete of the pointer arrays is done in destructor!
//    TH2F*** h5 = NULL; h5 = hM.getDim2(h5,"h5");
//    for(Int_t sector=0;sector<6;sector++){
//	for(Int_t module=0;module<4;module++){
//	    //cout<<sector<<" "<<module<<" "<<h5[sector][module]->GetName()<<endl;
//	}
//    }
//    //------------------------------------------------------------
//
//
//    //------------------------------------------------------------
//    // create pointer arrays , not managed by HHistMap
//    // take care about delete yourself
//    TH2F*** htest = NULL;
//    htest = HHistMap::newDim2(htest,6,4);
//    // .... put hist pointer to array
//    HHistMap::deleteDim2(htest,6,4,kFALSE); // delete only pointer array, not hists!
//    //------------------------------------------------------------
//
//
//    //########################################################################
//    //########################################################################
//    //########################################################################
//    //########################################################################
//
//
//
//    //------------------------------------------------------------
//    // create canvas arrays outside HHistMap and add them to HHistMap
//    hM.addCanvas(new TCanvas("c","c",500,500),1,0,500,500,"canvas/c");  // canvas outside created
//
//    TCanvas* c1[6];
//    for(Int_t sector=0;sector<6;sector++){
//	c1[sector]=new TCanvas(Form("c1_s%i",sector),Form("c1_s%i",sector),500,500);
//    }
//
//   // create a linear array for 2dim indexing
//    TCanvas* c2[24]; // 6x4 like c2[6][4]
//    for(Int_t sector=0;sector<6;sector++){
//	for(Int_t module=0;module<4;module++){
//	    c2[HTool::getLinearIndex(sector,6,module,4)] = new TCanvas(Form("c2_s%i_m%i",sector,module),Form("c2_s%i_m%i",sector,module),500,500);
//	}
//    }
//
//
//    hM.addCanvasArray((TCanvas**)c1,"c1","c1_s%i"    ,"c1_s%i"    ,1,0,500,500,"canvas/c1",6);    // 1dim array size 6
//    hM.addCanvasArray((TCanvas**)c2,"c2","c2_s%i_m%i","c2_s%i_m%i",1,0,500,500,"canvas/c2",6,4);  // 2dim array size 6x4
//    //------------------------------------------------------------
//
//
//
//
//    //------------------------------------------------------------
//    // create canvas arrays inside HHistMap
//    hM.addCanvas     ("c3","c3",2,1,500,500,"canvas/c3");
//    hM.addCanvasArray("c4","c4t1_s[%i]m[%i]","c4time1s[%i]m[%i]"
//		      ,1,0,500,500
//                      ,"canvas/c4"
//		      ,6,4);
//    //------------------------------------------------------------
//
//
//    //------------------------------------------------------------
//    // get multi dimensional arrays created by HHistMap.
//    // delete of the pointer arrays is done in destructor!
//    TCanvas*** c4 = NULL; c4 = hM.getCanvasDim2(c4,"c4");
//    for(Int_t sector=0;sector<6;sector++){
//	for(Int_t module=0;module<4;module++){
//	    //cout<<sector<<" "<<module<<" "<<c4[sector][module]->GetName()<<endl;
//	}
//    }
//    //------------------------------------------------------------
//
//
//    //------------------------------------------------------------
//    // create pointer arrays , not managed by HHistMap
//    // take care about delete yourself
//    TCanvas*** ctest = NULL;
//    ctest = HHistMap::newDim2(ctest,6,4);
//    // .... put hist pointer to array
//    HHistMap::deleteDim2(ctest,6,4,kFALSE); // delete only pointer array, not hists!
//    //------------------------------------------------------------
//
//
//    //########################################################################
//    //########################################################################
//    //########################################################################
//    //########################################################################
//    // accessing objects
//
//
//    //------------------------------------------------------------
//    // METHOD I:
//    // access the histograms by name : use the lable
//    // retrieve pointer by :
//    // TH1* get()
//    // TH2* get2()  for 2Dim hists  (get will work too, but some funtcion of TH2 can not be used with TH1 pointer)
//    // TH3* get3()  for 3dim hists  (get will work too, but some funtcion of TH3 can not be used with TH1 pointer)
//
//    hM.get("h")     ->FillRandom("gaus",1000);  // single
//    hM.get("h1",1)  ->FillRandom("gaus",1000);  // 1dim index 1
//    hM.get("h2",1,0)->FillRandom("gaus",1000);  // 2dim index [1][0]
//    for(Int_t i=0;i<1000;i++) hM.get ("h5",1,0)->Fill(gRandom->Gaus()*10+50,gRandom->Gaus()*10+50);     // function in TH1
//    for(Int_t i=0;i<1000;i++) hM.get2("h5",1,1)->Fill(gRandom->Gaus()*10+50,gRandom->Gaus()*10+50,2.);  // function in TH2
//
//    //------------------------------------------------------------
//
//    //------------------------------------------------------------
//    // METHOD II:
//    // access the histograms by name : use the lable
//    // retrieve a dynamically created multidim pointer
//
//    TH2F*** h5_access = NULL;
//    h5_access = hM.getDim2(h5,"h5");
//    h5_access[0][0]->Fill( 1000, 100);
//    //------------------------------------------------------------
//
//    //------------------------------------------------------------
//    // METHOD I:
//    // access the canvas by name : use the lable
//    hM.getCanvas("c3")     ->cd(1); // single canvas 2pads
//    hM.get("h5",1,0)->Draw("colz");
//    hM.getCanvas("c3")     ->cd(2); // single
//    hM.get("h5",1,1)->Draw("colz");
//
//    hM.getCanvas("c1",1)  ->cd(); // 1dim index 1
//    hM.getCanvas("c2",1,0)->cd(); // 2dim index [1][0]
//    //------------------------------------------------------------
//
//    //------------------------------------------------------------
//    // METHOD II:
//    // access the histograms by name : use the lable
//    TCanvas*** c4_access = NULL;
//    c4_access = hM.getCanvasDim2(c4_access,"c4");
//    c4_access[0][0]->cd();
//    //------------------------------------------------------------
//
//
//    //------------------------------------------------------------
//    // working with cuts and params
//
//    Double_t xcut[]= {
//	0.191092,
//	0.144397,
//	0.15158,
//	0.318606,
//	0.483836,
//	0.548491,
//	0.51796,
//	0.37967,
//	0.221624,
//	0.201868,
//	0.191092};
//
//    Double_t  ycut[]={
//	83.8983,
//	67.4788,
//	47.3517,
//	32.5212,
//	34.375,
//	55.8263,
//	77.8072,
//	89.4597,
//      87.6059,
//	85.7521,
//	83.8983};
//
//    hM.addCut("myCut",sizeof(xcut)/sizeof(Double_t),xcut,ycut,"cut");
//
//    Double_t vals[4] = {30,50,2,6};
//    HArrayD parsGaus(sizeof(vals)/sizeof(Double_t),vals); // wrapper for TArray
//    parsGaus.print();
//
//    hM.addPar(&parsGaus,"parsGaus","pars");
//
//    Int_t colorsMod[4] = {1,2,4,8};
//    HArrayI colors(24);
//    Int_t* cl = colors.getArray();
//    for(Int_t s = 0; s < 6; s ++) {
//       for(Int_t m = 0; m < 4; m ++) {
//	    cl[HTool::getLinearIndex(s,6,m,4)] = colorsMod[m];
//	}
//    }
//    hM.addPar(&colors,"colors","pars");
//    colors.print();
//
//    //------------------------------------------------------------
//
//    //------------------------------------------------------------
//    // working with functions on arrays
//
//    hM.functionArray("h4", fillGaus   ,NULL); // fill some random  distributions
//    hM.functionArray("h4", fillMeanRms,NULL); // fill hist with means+rms of h4   "h8_mean" "h8_rms"
//    hM.functionArray("h6", fillRndm   ,NULL); // fill some random  distributions
//    hM.functionArray("h4", setColor   ,NULL); // setColors per module
//    hM.functionArray("h6", setColor   ,NULL); // setColors per module
//
//
//    //see also : 2 and 3 operator add,multiply,divide
//    hM.addArray     ("h7","h6",1.,1.);  // make copy of h6 to h7
//    hM.divideArray  ("h6","h4",1.,1., "");
//    //hM.multiplyArray("h6","h4",1.,1., "");
//    // standard actions
//    //hM.resetArray ("h7","");
//    //hM.scaleArray ("h7", 2.);
//    //hM.sumW2Array ("h7");
//    //------------------------------------------------------------
//
//    //------------------------------------------------------------
//    // working with macros
//
//	TF1* fgaus = new TF1("fgaus","gaus",-1,1);
//	fgaus->SetLineColor(2);
//	hM.addFit(fgaus,"fits/gaus");
//
//      hM.addCanvas("c10","c10",1,1,500,500,"canvas/c2");
//
//
//	TMacro* mac = new TMacro("drawHist");
//	//mac->ReadFile("./histmac/hhistmap_drawHist.C");  // read  macro from existing file (file should not be in workdir)
//
//	mac->AddLine("void drawHist(){");
//	mac->AddLine("cout<<\"exec drawHist\"<<endl;");
//	mac->AddLine("gHHistMap->getCanvas(\"c10\")->Draw();");
//	mac->AddLine("gHHistMap->getCanvas(\"c10\")->cd();");
//	mac->AddLine("gHHistMap->get(\"h\")->SetLineColor(4);");
//	mac->AddLine("gHHistMap->get(\"h\")->SetLineStyle(2);");
//	mac->AddLine("gHHistMap->get(\"h\")->SetMarkerStyle(8);");
//	mac->AddLine("gHHistMap->get(\"h\")->SetMarkerColor(8);");
//	mac->AddLine("gHHistMap->getFit(\"fgaus\")->SetRange(-1,1);");
//	mac->AddLine("gHHistMap->get(\"h\")->Fit(gHHistMap->getFit(\"fgaus\"),\"R\");");
//	mac->AddLine("cout<<\"mean =\"<<gHHistMap->getFit(\"fgaus\")->GetParameter(1)<<\" sig =\"<<gHHistMap->getFit(\"fgaus\")->GetParameter(2)<<endl;");
//	mac->AddLine("gHHistMap->get(\"h\")->Draw(\"PL\");");
//	mac->AddLine("gHHistMap->getCanvas(\"c10\")->Update();");
//	mac->AddLine("}");
//      // mac->SaveSource("./histmac/hhistmap_drawHist.C"); // write macro to text file
//
//	hM.addMacro(mac,"drawHist","macros/draw");
//
//      hm->getMacro("drawHist")->Print();  // print source code
//      hm->getMacro("drawHist")->Exec();   // execute the macro
//
//    //------------------------------------------------------------
//
//    // print content of HHistMap
//    hM.print();
//    //hM.printMap();
//    //hM.printCanvasMap();
//    //hM.printCutMap();
//    //hM.printParameterMap();
//    //hM.printMacroMap();
//
//
//    //------------------------------------------------------------
//    // input + output
//    hM.writeHists();                       // write hists + canvas + cuts + fits + params + HHistMap  to root file
//    //------------------------------------------------------------
//
//
//    //------------------------------------------------------------
//    // merging of root files
//    HHistMap::addHistMap("histmap_file_*.root","merge.root"); // merge all HHistMap objects in matching files into merge.root
//    HHistMap::addHistMap("histmap_file_1.root,histmap_file_2.root,histmap_file_3.root","merge.root"); // merge all HHistMap objects in files 1,2,3 into merge.root
//    HHistMap::addHistMap("histmap_file_list.txt,histmap_file_2.root,histmap_file_3.root","merge.root"); // merge all HHistMap objects in files of filelist into merge.root
//    //------------------------------------------------------------
//
//
////////////////////////////////////////////////////////////////////////////////

ClassImp(HHistMap)

HHistMap *gHHistMap=0;

HHistMap::HHistMap(TString outputname) {
    output=NULL;
    if(outputname != "") setOutputFile(outputname);
    gHHistMap = this;
    setSilentFail(kFALSE);
    setDeleteObjects(kFALSE);
}

HHistMap::~HHistMap()
{
    //---------------------------------------------------------------------------
    // hists


    //---------------------------------------------------------------------------
    for(map< TString, TH1**>::iterator iter = hDim1.begin(); iter != hDim1.end(); ++iter ) {
        TArrayI& ar = hD[iter->first];
        deleteDim1(iter->second,ar.At(0),kFALSE);
    }
    hDim1.clear();

    for(map< TString, TH1***>::iterator iter = hDim2.begin(); iter != hDim2.end(); ++iter ) {
        TArrayI& ar = hD[iter->first];
        deleteDim2(iter->second,ar.At(0),ar.At(1),kFALSE);
    }
    hDim2.clear();

    for(map< TString, TH1****>::iterator iter = hDim3.begin(); iter != hDim3.end(); ++iter ) {
        TArrayI& ar = hD[iter->first];
        deleteDim3(iter->second,ar.At(0),ar.At(1),ar.At(2),kFALSE);
    }
    hDim3.clear();

    for(map< TString, TH1*****>::iterator iter = hDim4.begin(); iter != hDim4.end(); ++iter ) {
        TArrayI& ar = hD[iter->first];
        deleteDim4(iter->second,ar.At(0),ar.At(1),ar.At(2),ar.At(3),kFALSE);
    }
    hDim4.clear();

    for(map< TString, TH1******>::iterator iter = hDim5.begin(); iter != hDim5.end(); ++iter ) {
        TArrayI& ar = hD[iter->first];
        deleteDim5(iter->second,ar.At(0),ar.At(1),ar.At(2),ar.At(3),ar.At(4),kFALSE);
    }
    hDim5.clear();
    //---------------------------------------------------------------------------

    if(getDeleteObjects()){
	for(map< TString, TObjArray>::iterator iter = hM.begin(); iter != hM.end(); ++iter ) {
	    TString classname = "unused";
	    classname = (*iter).second.At(0)->ClassName();
	    Int_t size = (*iter).second.GetEntries();

            Int_t type = -1;
	    if(classname.Contains("TH1")){
		type=10;
		if(classname.CompareTo("TH1S")==0)  type = 10 ;
		if(classname.CompareTo("TH1I")==0)  type = 11 ;
		if(classname.CompareTo("TH1F")==0)  type = 12 ;
		if(classname.CompareTo("TH1D")==0)  type = 13 ;
	    } else if(classname.Contains("TH2")){
		type=20;
		if(classname.CompareTo("TH2S")==0)  type = 20 ;
		if(classname.CompareTo("TH2I")==0)  type = 21 ;
		if(classname.CompareTo("TH2F")==0)  type = 22 ;
		if(classname.CompareTo("TH2D")==0)  type = 23 ;

	    } else if(classname.Contains("TH3")){
		type=30;
		if(classname.CompareTo("TH3S")==0)  type = 30 ;
		if(classname.CompareTo("TH3I")==0)  type = 31 ;
		if(classname.CompareTo("TH3F")==0)  type = 32 ;
		if(classname.CompareTo("TH3D")==0)  type = 33 ;
	    }
	    for(Int_t i = 0; i< size; i++){
		switch (type) {
		case 10:  delete (TH1S*)  ((*iter).second.At(i));
		break;
		case 11:  delete (TH1I*)  ((*iter).second.At(i));
		break;
		case 12:  delete (TH1F*)  ((*iter).second.At(i));
		break;
		case 13:  delete (TH1D*)  ((*iter).second.At(i));
		break;
		case 20:  delete (TH2S*)  ((*iter).second.At(i));
		break;
		case 21:  delete (TH2I*)  ((*iter).second.At(i));
		break;
		case 22:  delete (TH2F*)  ((*iter).second.At(i));
		break;
		case 23:  delete (TH2D*)  ((*iter).second.At(i));
		break;
		case 30:  delete (TH2S*)  ((*iter).second.At(i));
		break;
		case 31:  delete (TH2I*)  ((*iter).second.At(i));
		break;
		case 32:  delete (TH2F*)  ((*iter).second.At(i));
		break;
		case 33:  delete (TH2D*)  ((*iter).second.At(i));
		break;
		default:
		    break;

		}
	    }
            (*iter).second.Clear();
	}
    }
    hM.clear();



    hD.clear();
    hTitle.clear();
    hMask.clear();
    hDir.clear();


    //---------------------------------------------------------------------------
    // canvas



    //---------------------------------------------------------------------------
    for(map< TString, TCanvas**>::iterator iter = hDim1C.begin(); iter != hDim1C.end(); ++iter ) {
        TArrayI& ar = hDC[iter->first];
        deleteDim1(iter->second,ar.At(0),kFALSE);
    }
    hDim1C.clear();

    for(map< TString, TCanvas***>::iterator iter = hDim2C.begin(); iter != hDim2C.end(); ++iter ) {
        TArrayI& ar = hDC[iter->first];
        deleteDim2(iter->second,ar.At(0),ar.At(1),kFALSE);
    }
    hDim2C.clear();

    for(map< TString, TCanvas****>::iterator iter = hDim3C.begin(); iter != hDim3C.end(); ++iter ) {
        TArrayI& ar = hDC[iter->first];
        deleteDim3(iter->second,ar.At(0),ar.At(1),ar.At(2),kFALSE);
    }
    hDim3C.clear();

    for(map< TString, TCanvas*****>::iterator iter = hDim4C.begin(); iter != hDim4C.end(); ++iter ) {
        TArrayI& ar = hDC[iter->first];
        deleteDim4(iter->second,ar.At(0),ar.At(1),ar.At(2),ar.At(3),kFALSE);
    }
    hDim4C.clear();

    for(map< TString, TCanvas******>::iterator iter = hDim5C.begin(); iter != hDim5C.end(); ++iter ) {
        TArrayI& ar = hDC[iter->first];
        deleteDim5(iter->second,ar.At(0),ar.At(1),ar.At(2),ar.At(3),ar.At(4),kFALSE);
    }
    hDim5C.clear();
    //---------------------------------------------------------------------------

    if(getDeleteObjects()){
   	for(map< TString, TObjArray>::iterator iter = hMC.begin(); iter != hMC.end(); ++iter ) {
	    Int_t size = (*iter).second.GetEntries();
	    for(Int_t i = 0; i< size; i++){
		if( (*iter).second.At(i) ) delete (TCanvas*)((*iter).second.At(i));
	    }
	    (*iter).second.Clear();
	}
    }
    hMC.clear();
    hDC.clear();
    hNpC.clear();
    hTitleC.clear();
    hMaskC.clear();
    hDirC.clear();

    //---------------------------------------------------------------------------
    // cuts
    if(getDeleteObjects()){
	for(map< TString, TCutG*>::iterator iter = hMCut.begin(); iter != hMCut.end(); ++iter ) {
	    if((*iter).second ) delete (*iter).second;
	}
    }
    hMCut.clear();
    hDirCut.clear();

    //---------------------------------------------------------------------------
    // fits
    if(getDeleteObjects()){
	for(map< TString, TF1*>::iterator iter = hMFit.begin(); iter != hMFit.end(); ++iter ) {
	    if((*iter).second ) delete (*iter).second;
	}
    }
    hMFit.clear();
    hDirFit.clear();

    //---------------------------------------------------------------------------
    // parameters
    if(getDeleteObjects()){
	for(map< TString, TObject*>::iterator iter = hMPar.begin(); iter != hMPar.end(); ++iter ) {
	    if((*iter).second ) delete (*iter).second;
	}
    }
    hMPar.clear();
    hDirPar.clear();

    //---------------------------------------------------------------------------
    // macros
    if(getDeleteObjects()){
	for(map< TString, TMacro*>::iterator iter = hMMacro.begin(); iter != hMMacro.end(); ++iter ) {
	    if((*iter).second ) delete (*iter).second;
	}
    }
    hMMacro.clear();
    hDirMacro.clear();


}

TString HHistMap::getLabels(const TString name,vector<TString>&labels)
{

    labels.clear();
    TRegexp regexp("\\[[a-zA-ZA0-9]+\\]");
    Ssiz_t max = name.Length();

    Ssiz_t start = 0;
    Ssiz_t len   = 0;
    Ssiz_t pos   = 0;
    TString out;
    TString cp;
    while(start<max && pos > -1){
	pos = regexp.Index(name,&len,start);
	if(pos<0)continue;
	cp = name;
	cp.Replace(0,pos+1,"");
        cp.Replace(len-2,cp.Length()-len+2,"");
        labels.push_back(cp);
	out += cp;
	out += ",";
	start = pos+len;

    }

    if(out.EndsWith(",")) out.Remove(out.Length()-1,1);
    return out;
}


Bool_t HHistMap::setOutputFile(TString name)
{
    // set the output file for the histograms
    // already existing file will be overwritten

    if(name!=""){
	if(!output){
	    output = new TFile(name.Data(),"RECREATE");
	} else {
            Error("setOutputFile()","Already other file opened! will be ignored...");
	    return kFALSE;
	}
    } else {
	Warning("setOutputFile()","Name of file not specified! Use setOutputFile() if the histograms should be stored to the out put.");
        return kFALSE;
    }
    return kTRUE;
}
void HHistMap::print()
{
    printMap();
    printCanvasMap();
    printCutMap();
    printFitMap();
    printParameterMap();
    printMacroMap();
}

void HHistMap::printMap()
{
    // print he list of available histograms
    Int_t ct = 0;
    cout<<"------------------------------------------------------"<<endl;
    cout<<"Available histograms :"<<endl;
    for(map< TString, TObjArray>::iterator iter = hM.begin(); iter != hM.end(); ++iter ) {
	ct++;
	TString classname = "unused";
	classname = (*iter).second.At(0)->ClassName();
        Int_t size = (*iter).second.GetEntries();
        Int_t dim = hD[(*iter).first].GetSize();
	cout <<setw(3)<<ct<<" "<<setw(40)<< (*iter).first.Data() << " is a " << classname.Data()<<" , size = "<<setw(5)<<size<<" dim = "<<dim<<" , Dir = "<<hDir[(*iter).first]<<endl;
    }
    cout<<"------------------------------------------------------"<<endl;
}

void HHistMap::printCanvasMap()
{
    // print the list of available canvas
    Int_t ct = 0;
    cout<<"------------------------------------------------------"<<endl;
    cout<<"Available canvas :"<<endl;
    for(map< TString, TObjArray>::iterator iter = hMC.begin(); iter != hMC.end(); ++iter ) {
	ct++;
	TString classname = "unused";
	classname = (*iter).second.At(0)->ClassName();
        Int_t size = (*iter).second.GetEntries();
        Int_t dim = hDC[(*iter).first].GetSize();
	cout <<setw(3)<<ct<<" "<<setw(40)<< (*iter).first.Data() << " is a " << classname.Data()<<" , size = "<<setw(5)<<size<<" dim = "<<dim<<" , Dir = "<<hDirC[(*iter).first]<<endl;
    }
    cout<<"------------------------------------------------------"<<endl;
}

void HHistMap::printCutMap()
{
    // print the list of available cuts
    Int_t ct = 0;
    cout<<"------------------------------------------------------"<<endl;
    cout<<"Available cuts :"<<endl;
    for(map< TString, TCutG*>::iterator iter = hMCut.begin(); iter != hMCut.end(); ++iter ) {
	ct++;
	TString classname = "unused";
	classname = (*iter).second->ClassName();
	cout <<setw(3)<<ct<<" "<<setw(40)<< (*iter).first.Data() << " is a " << classname.Data()<<" , Dir = "<<hDirCut[(*iter).first]<<endl;
    }
    cout<<"------------------------------------------------------"<<endl;
}

void HHistMap::printFitMap()
{
    // print the list of available fits
    Int_t ct = 0;
    cout<<"------------------------------------------------------"<<endl;
    cout<<"Available fits :"<<endl;
    for(map< TString, TF1*>::iterator iter = hMFit.begin(); iter != hMFit.end(); ++iter ) {
	ct++;
	TString classname = "unused";
	classname = (*iter).second->ClassName();
	cout <<setw(3)<<ct<<" "<<setw(40)<< (*iter).first.Data() << " is a " << classname.Data()<<" , Dir = "<<hDirFit[(*iter).first]<<endl;
    }
    cout<<"------------------------------------------------------"<<endl;
}

void HHistMap::printParameterMap()
{
    // print the list of available parameters
    Int_t ct = 0;
    cout<<"------------------------------------------------------"<<endl;
    cout<<"Available parameter :"<<endl;
    for(map< TString, TObject*>::iterator iter = hMPar.begin(); iter != hMPar.end(); ++iter ) {
	ct++;
	TString classname = "unused";
	if((*iter).second ) classname = (*iter).second->ClassName();
	cout <<setw(3)<<ct<<" "<<setw(40)<< (*iter).first.Data() << " is a " << classname.Data()<<" , Dir = "<<hDirPar[(*iter).first]<<endl;
    }
    cout<<"------------------------------------------------------"<<endl;
}

void HHistMap::printMacroMap()
{
    // print the list of available macros
    Int_t ct = 0;
    cout<<"------------------------------------------------------"<<endl;
    cout<<"Available macro :"<<endl;
    for(map< TString, TMacro*>::iterator iter = hMMacro.begin(); iter != hMMacro.end(); ++iter ) {
	ct++;
	TString classname = "unused";
	if((*iter).second ) classname = (*iter).second->ClassName();
	cout <<setw(3)<<ct<<" "<<setw(40)<< (*iter).first.Data() << " is a " << classname.Data()<<" , Dir = "<<hDirMacro[(*iter).first]<<endl;
    }
    cout<<"------------------------------------------------------"<<endl;
}

void HHistMap::addHistContent(HHistMap& hM2)
{
    // add content of all hists of hM2 to this (TH1::Add)
    for(map< TString, TObjArray>::iterator iter = hM.begin(); iter != hM.end(); ++iter ) {

	TObjArray* ar1 = getHistArray((*iter).first);
	TObjArray* ar2 = hM2.getHistArray((*iter).first);

	if(ar1 && ar2) {
	    addArray(ar1,ar2,1,1);
	} else {
	    Error("addHistContent()","Histogram: %s not found! skipped.",(*iter).first.Data());
	}

    }
}


Bool_t HHistMap::addFile (TString infile)
{
    // add HHIstMap from a single root file.

    if(gSystem->AccessPathName(infile.Data()) == 0) {
        Bool_t stat =  TDirectory::AddDirectoryStatus();
        TDirectory::AddDirectory(kFALSE);
	TFile* in = new TFile(infile.Data(),"READ");

	HHistMap* hM2 = (HHistMap*)in->Get("HHistMap");

	if(hM2) {
	    cout<<"add file : "<<infile.Data()<<endl;
	    addHistContent(*hM2);
	} else {
	    Error("addFile()","File = %s : HHistMap objekt not found! Will be skipped .... ",infile.Data());
	    in->Close();
	    delete in;
	    gROOT->cd();
	    return kFALSE;
	}

	hM2->setDeleteObjects(kTRUE);
	delete hM2;

	in->Close();
        delete in;
	TDirectory::AddDirectory(stat);

        return kTRUE;
    } else {
        Error("addFile()","File = %s not found! Will be skipped .... ",infile.Data());
        return kFALSE;
    }

}

Bool_t HHistMap::addFiles(TString expression){

    // add all files matching the expression to the chain

    TObjArray* arr = HTool::glob(expression);
    Int_t n = arr->GetEntries();
    if(n == 0) {
	Warning("addFiles()","No file matching expression '%s' found !",expression.Data());
    } else {
	for(Int_t i = 0; i < n; i ++){
	    TString name = ((TObjString*)arr->At(i))->GetString();
	    addFile(name);
	}
    }
    delete arr;
    return kTRUE;

}

Bool_t HHistMap::addFilesList(TString filelist)
{
    // add all files in list filelist. The list should contain
    // 1 root file per line (including path). Return kFALSE if
    // list files does not exist or and error occured while
    // reading the file

    if(gSystem->AccessPathName(filelist.Data()) == 0 )
    {
	ifstream in;
	in.open(filelist.Data());
        TString name;
	while(!in.eof()){
	    in>>name;
	    if(!in.fail()) {
		addFile(name);
	    }
	    else {
               Error("addFilesList()","Error detected during reading the files list ! Will skipped all other files if any.... ");
	       in.close();
               return kFALSE;
	    }
	}
        in.close();

       return kTRUE;
    } else {
       Error("addFilesList()","File = %s not found! Will be skipped .... ",filelist.Data());
       return kFALSE;
    }
}


Bool_t HHistMap::addMultFiles(TString commaSeparatedList)
{
    // add multiple files as comma separated file list : "file1,file2,file3"

    TObjArray* arr = commaSeparatedList.Tokenize(",");
    Int_t n = arr->GetEntries();
    if(n == 0) {
	Warning("addMultFiles()","No file in input string '%s' found !",commaSeparatedList.Data());
    } else {
	for(Int_t i = 0; i < n; i ++){
	    TString name = ((TObjString*)arr->At(i))->GetString();
	    if(!addFile(name)){
		delete arr;
		return kFALSE;
	    }
	}
    }
    delete arr;
    return kTRUE;
}



Bool_t HHistMap::addHistMap(TString input,TString output)
{
    // static function to add histogram contents of same type
    // HHistMap stored in different root files. The HHistMap
    // object of the first input file is copied, reset all hists
    // and then the histogramm content of all files is added.
    //
    //  input : a.  comma separated filelist  ("file1,root,file2.root")
    //          b.  reg expression matching root files (file_*.root" , has to end .root)
    //          c.  list file with 1 file per line
    //  output: root file name to store the merge HHistMap to

    if(input == "" ) {
        cout<<"addHistMap() : No input defined !"<<endl;
	return kFALSE;
    }
    if(output == "" ){
	cout<<"addHistMap() : No output defined !"<<endl;
	return kFALSE;
    }


    Int_t methode = 0;

    TString   firstname="";

    if(input.Contains(",") == 1)  {
        // comma separated file list
	methode = 3;

	TObjArray* arr = input.Tokenize(",");
	Int_t n = arr->GetEntries();
	if(n == 0) {
	    cout<<"addHistMap() : No file in input string found !"<<endl;
	} else {
		firstname = ((TObjString*)arr->At(0))->GetString();
	}
        delete arr;

    } else if(input.EndsWith(".root")==1){
        // single file or regexp
	methode = 1;

	TObjArray* arr = HTool::glob(input);
	Int_t n = arr->GetEntries();
	if(n == 0) {
	    cout<<"addHistMap() : No file matching expression found !"<<endl;
	} else {
	    firstname = ((TObjString*)arr->At(0))->GetString();
	}
        delete arr;

    } else {
	methode = 2;  // list file
	if(gSystem->AccessPathName(input.Data()) == 0 )
	{
	    ifstream in;
	    in.open(input.Data());
            Int_t ct=0;
	    while(!in.eof() && ct<1){
		in>>firstname;
		if(!in.fail()) {
                    ct++;
		}
		else {
		    cout<<"addHistMap() : Error detected during reading the files list ! Will skipped all other files if any.... "<<endl;
		}
	    }
	    in.close();
	}
    }

    HHistMap* hMOut=0;

    if(firstname != "" &&  gSystem->AccessPathName(firstname.Data()) == 0 ){

	TFile in(firstname.Data(),"READ");
	HHistMap* hM2=(HHistMap*)in.Get("HHistMap");

	if(hM2) {
	    hMOut = hM2;
	    hMOut->resetAllHists();
	    hMOut->setOutputFile(output);
	}
	in.Close();
        gROOT->cd();
    } else {
	cout<<"addHistMap() : Error in input. no merge file will be produce!"<<endl;
        return kFALSE;
    }

    if(hMOut){
	if(methode == 3) hMOut->addMultFiles(input);
	if(methode == 2) hMOut->addFilesList(input);
	if(methode == 1) hMOut->addFiles(input);

        hMOut->writeHists();

    }

    return kTRUE;
}



Bool_t HHistMap::addHist(TH1* h,TString dir)
{
    // add a single histogram
    if(!h) {
	Error("addHist()","hist pointer = NULL!");
	return kFALSE;
    }
    TString name = h->GetName();

    h->SetDirectory(0);

    map<TString,TObjArray>::iterator iter = hM.find(name);
    if( iter == hM.end() ){ // new variable
	TObjArray a;
        a.Add(h);
	TArrayI dim(1);
        dim.Reset(0);  // no array
	hM    [name] = a;
	hMask [name] = name;
	hTitle[name] = h->GetTitle();
	hD    [name] = dim;
        hDir  [name] = dir;

    } else { // exists already
	Error("addHist()","hist with name \"%s\" has been used already!",name.Data());
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HHistMap::removeHist(TString name)
{
    // remove a histogram/histogram array

    map<TString,TObjArray>::iterator iter = hM.find(name);
    if( iter == hM.end() ){ // new variable
	Error("removeHist()","hist with name \"%s\" not found!",name.Data());
        return kFALSE;
    } else { // exists
        // histograms
	iter->second.Delete();
	hM.erase( iter );
        // mask,title,dir
	map<TString,TString>::iterator iter2 = hMask.find(name);
	hMask.erase( iter2 );
        iter2 = hTitle.find(name);
	hTitle.erase( iter2 );
	iter2 = hDir.find(name);
	hDir.erase( iter2 );
        // dim array
	map<TString,TArrayI>::iterator iter3 = hD.find(name);
        Int_t dim = iter3->second.GetSize();
        TArrayI& ar = iter3->second;
        // dynamic pointers
	if(dim == 1){
            map<TString,TH1**>::iterator iter4 = hDim1.find(name);
	    if(iter4 != hDim1.end()) deleteDim1(iter4->second,ar.At(0),kFALSE);
	} else if(dim == 2){
            map<TString,TH1***>::iterator iter4 = hDim2.find(name);
	    if(iter4 != hDim2.end()) deleteDim2(iter4->second,ar.At(0),ar.At(1),kFALSE);
	} else if(dim == 3){
            map<TString,TH1****>::iterator iter4 = hDim3.find(name);
	    if(iter4 != hDim3.end()) deleteDim3(iter4->second,ar.At(0),ar.At(1),ar.At(2),kFALSE);
	} else if(dim == 4){
            map<TString,TH1*****>::iterator iter4 = hDim4.find(name);
	    if(iter4 != hDim4.end()) deleteDim4(iter4->second,ar.At(0),ar.At(1),ar.At(2),ar.At(3),kFALSE);
	} else if(dim == 5){
            map<TString,TH1******>::iterator iter4 = hDim5.find(name);
	    if(iter4 != hDim5.end()) deleteDim5(iter4->second,ar.At(0),ar.At(1),ar.At(2),ar.At(3),ar.At(4),kFALSE);
	}
	hD.erase( iter3 );
    }
    return kTRUE;
}

TH1* HHistMap::createHist(TString Type,
			  TString name,TString title,
			  Int_t nbinx,Double_t x1,Double_t x2,
			  Int_t nbiny,Double_t y1,Double_t y2,
			  Int_t nbinz,Double_t z1,Double_t z2,
			  TString xtitle,TString ytitle,TString ztitle)
{
    // create an histograms (const binning).
    // Type : TH1S,TH1I,TH1F,TH1D,TH2S,TH2I,TH2F,TH2D,TH3S,TH3I,TH3F,TH3D,TProfile,TProfile2D,TProfile3D
    // name : hist name
    // title : hist title
    // hist axis : 1d  Int_t nbinx,Double_t x1,Double_t x2,
    //    	   2d  Int_t nbiny,Double_t y1,Double_t y2,
    //    	   3d  Int_t nbinz,Double_t z1,Double_t z2,
    // axis title  xtitle,ytitle,ztitle
    // dir : output dir

    if(name == "") {
	Error("createHist()","hist name empty!");
	return NULL;
    }

    TH1* h = NULL;
    if(Type == "TH1S" ||
       Type == "TH1I" ||
       Type == "TH1F" ||
       Type == "TH1D"
      ){
	if(Type == "TH1S")  {
	    h = new TH1S(name.Data(),title.Data(),nbinx,x1,x2);
	} else if(Type == "TH1I")  {
	    h = new TH1I(name.Data(),title.Data(),nbinx,x1,x2);
	} else if(Type == "TH1F")  {
	    h = new TH1F(name.Data(),title.Data(),nbinx,x1,x2);
	} else if(Type == "TH1D")  {
	    h = new TH1D(name.Data(),title.Data(),nbinx,x1,x2);
	}
	h->SetXTitle(xtitle.Data());
	h->SetYTitle(ytitle.Data());


    } else if (
	       Type == "TH2S" ||
	       Type == "TH2I" ||
	       Type == "TH2F" ||
	       Type == "TH2D"
	      )
    {
	if(Type == "TH2S")  {
	    h = new TH2S(name.Data(),title.Data(),nbinx,x1,x2,nbiny,y1,y2);
	} else if(Type == "TH2I")  {
	    h = new TH2I(name.Data(),title.Data(),nbinx,x1,x2,nbiny,y1,y2);
	} else if(Type == "TH2F")  {
	    h = new TH2F(name.Data(),title.Data(),nbinx,x1,x2,nbiny,y1,y2);
	} else if(Type == "TH2D")  {
	    h = new TH2D(name.Data(),title.Data(),nbinx,x1,x2,nbiny,y1,y2);
	}
	h->SetXTitle(xtitle.Data());
	h->SetYTitle(ytitle.Data());
        h->SetZTitle(ztitle.Data());

    } else if (
	       Type == "TH3S" ||
	       Type == "TH3I" ||
	       Type == "TH3F" ||
	       Type == "TH3D"
	      ){
	if(Type == "TH3S")  {
	    h = new TH3S(name.Data(),title.Data(),nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2);
	} else if(Type == "TH3I")  {
	    h = new TH3I(name.Data(),title.Data(),nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2);
	} else if(Type == "TH3F")  {
	    h = new TH3F(name.Data(),title.Data(),nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2);
	} else if(Type == "TH3D")  {
	    h = new TH3D(name.Data(),title.Data(),nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2);
	}
	h->SetXTitle(xtitle.Data());
	h->SetYTitle(ytitle.Data());
	h->SetZTitle(ztitle.Data());

    } else if (
	       Type == "TProfile"   ||
	       Type == "TProfile2D" ||
	       Type == "TProfile3D"
	      ){
	if(Type == "TProfile")  {
	    h = new TProfile(name.Data(),title.Data(),nbinx,x1,x2);
	} else if(Type == "TProfile2D")  {
	    h = new TProfile2D(name.Data(),title.Data(),nbinx,x1,x2,nbiny,y1,y2);
	} else if(Type == "TProfile3D")  {
	    h = new TProfile3D(name.Data(),title.Data(),nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2);
	}
	h->SetXTitle(xtitle.Data());
	h->SetYTitle(ytitle.Data());
	h->SetZTitle(ztitle.Data());
    } else {
	Error("createHist()","Unknow hist type %s. Supported are S,I,F,D for 1,2 and 3 dim hists and profiles.",Type.Data());
        return NULL;
    }
    h->SetDirectory(0);
    return h;
}

TH1* HHistMap::createHistN(TString Type,
			  TString name,TString title,
			  Int_t nbinx,Double_t* x,
			  Int_t nbiny,Double_t* y,
			  Int_t nbinz,Double_t* z,
			  TString xtitle,TString ytitle,TString ztitle)
{
    // create an histograms (non const binning).
    // Type : TH1S,TH1I,TH1F,TH1D,TH2S,TH2I,TH2F,TH2D,TH3S,TH3I,TH3F,TH3D,TProfile,TProfile2D,TProfile3D
    // name : hist name
    // title : hist title
    // hist axis : 1d  Int_t nbinx, pointer to Double_t array x for bins,
    //    	   2d  Int_t nbiny, pointer to Double_t array y for bins,
    //    	   3d  Int_t nbinz, pointer to Double_t array z for bins,
    // axis title  xtitle,ytitle,ztitle
    // dir : output dir

    if(name == "") {
	Error("createHistN()","hist name empty!");
	return NULL;
    }

    TH1* h = NULL;
    if(Type == "TH1S" ||
       Type == "TH1I" ||
       Type == "TH1F" ||
       Type == "TH1D"
      ){
	if(Type == "TH1S")  {
	    h = new TH1S(name.Data(),title.Data(),nbinx,x);
	} else if(Type == "TH1I")  {
	    h = new TH1I(name.Data(),title.Data(),nbinx,x);
	} else if(Type == "TH1F")  {
	    h = new TH1F(name.Data(),title.Data(),nbinx,x);
	} else if(Type == "TH1D")  {
	    h = new TH1D(name.Data(),title.Data(),nbinx,x);
	}
	h->SetXTitle(xtitle.Data());
	h->SetYTitle(ytitle.Data());


    } else if (
	       Type == "TH2S" ||
	       Type == "TH2I" ||
	       Type == "TH2F" ||
	       Type == "TH2D"
	      )
    {
	if(Type == "TH2S")  {
	    h = new TH2S(name.Data(),title.Data(),nbinx,x,nbiny,y);
	} else if(Type == "TH2I")  {
	    h = new TH2I(name.Data(),title.Data(),nbinx,x,nbiny,y);
	} else if(Type == "TH2F")  {
	    h = new TH2F(name.Data(),title.Data(),nbinx,x,nbiny,y);
	} else if(Type == "TH2D")  {
	    h = new TH2D(name.Data(),title.Data(),nbinx,x,nbiny,y);
	}
	h->SetXTitle(xtitle.Data());
	h->SetYTitle(ytitle.Data());
        h->SetZTitle(ztitle.Data());

    } else if (
	       Type == "TH3S" ||
	       Type == "TH3I" ||
	       Type == "TH3F" ||
	       Type == "TH3D"
	      ){
	if(Type == "TH3S")  {
	    h = new TH3S(name.Data(),title.Data(),nbinx,x,nbiny,y,nbinz,z);
	} else if(Type == "TH3I")  {
	    h = new TH3I(name.Data(),title.Data(),nbinx,x,nbiny,y,nbinz,z);
	} else if(Type == "TH3F")  {
	    h = new TH3F(name.Data(),title.Data(),nbinx,x,nbiny,y,nbinz,z);
	} else if(Type == "TH3D")  {
	    h = new TH3D(name.Data(),title.Data(),nbinx,x,nbiny,y,nbinz,z);
	}
	h->SetXTitle(xtitle.Data());
	h->SetYTitle(ytitle.Data());
	h->SetZTitle(ztitle.Data());

    } else if (
	       Type == "TProfile" ||
	       Type == "TProfile2D" ||
	       Type == "TProfile3D"
	      ){
	if(Type == "TProfile")  {
	    h = new TProfile(name.Data(),title.Data(),nbinx,x);
	} else if(Type == "TProfile2D")  {
	    h = new TProfile2D(name.Data(),title.Data(),nbinx,x,nbiny,y);
	} else if(Type == "TProfile3D")  {
	    h = new TProfile3D(name.Data(),title.Data(),nbinx,x,nbiny,y,nbinz,z);
	}
	h->SetXTitle(xtitle.Data());
	h->SetYTitle(ytitle.Data());
	h->SetZTitle(ztitle.Data());

    } else {
	Error("createHistN()","Unknow hist type %s. Supported are S,I,F,D for 1,2 and 3 dim hists and profiles.",Type.Data());
        return NULL;
    }
    h->SetDirectory(0);
    return h;
}

Bool_t HHistMap::addHist(TString Type,
			 TString name,TString title,
			 Int_t nbinx,Double_t x1,Double_t x2,
			 Int_t nbiny,Double_t y1,Double_t y2,
			 Int_t nbinz,Double_t z1,Double_t z2,
			 TString xtitle,TString ytitle,TString ztitle,
			 TString dir)
{
    // add an histograms (const binning).
    // Type : TH1S,TH1I,TH1F,TH1D,TH2S,TH2I,TH2F,TH2D,TH3S,TH3I,TH3F,TH3D,TProfile,TProfile2D,TProfile3D
    // name : hist name
    // title : hist title
    // hist axis : 1d  Int_t nbinx,Double_t x1,Double_t x2,
    //    	   2d  Int_t nbiny,Double_t y1,Double_t y2,
    //    	   3d  Int_t nbinz,Double_t z1,Double_t z2,
    // axis title  xtitle,ytitle,ztitle
    // dir : output dir

    TH1* h = createHist(Type,name,title,nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2,xtitle,ytitle,ztitle);

    if(h == NULL) {
	return kFALSE;
    }

    h->SetDirectory(0);

    map<TString,TObjArray>::iterator iter = hM.find(name);
    if( iter == hM.end() ){ // new variable
	TObjArray a;
        a.Add(h);
	TArrayI dim(1);
        dim.Reset(0);  // no array
	hM    [name] = a;
	hMask [name] = name;
	hTitle[name] = title;
	hD    [name] = dim;
        hDir  [name] = dir;

    } else { // exists already
	Error("addHist()","hist with name \"%s\" has been used already!",name.Data());
        delete h;
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HHistMap::addHistN(TString Type,
			 TString name,TString title,
			 Int_t nbinx,Double_t* x,
			 Int_t nbiny,Double_t* y,
			 Int_t nbinz,Double_t* z,
			 TString xtitle,TString ytitle,TString ztitle,
			 TString dir)
{
    // add an histograms (non const binning).
    // Type : TH1S,TH1I,TH1F,TH1D,TH2S,TH2I,TH2F,TH2D,TH3S,TH3I,TH3F,TH3D,TProfile,TProfile2D,TProfile3D
    // name : hist name
    // title : hist title
    // hist axis : 1d  Int_t nbinx, pointer to Double_t array x for bins,
    //    	   2d  Int_t nbiny, pointer to Double_t array y for bins,
    //    	   3d  Int_t nbinz, pointer to Double_t array z for bins,
    // axis title  xtitle,ytitle,ztitle
    // dir : output dir

    TH1* h = createHistN(Type,name,title,nbinx,x,nbiny,y,nbinz,z,xtitle,ytitle,ztitle);

    if(h == NULL) {
	return kFALSE;
    }

    h->SetDirectory(0);

    map<TString,TObjArray>::iterator iter = hM.find(name);
    if( iter == hM.end() ){ // new variable
	TObjArray a;
        a.Add(h);
	TArrayI dim(1);
        dim.Reset(0);  // no array
	hM    [name] = a;
	hMask [name] = name;
	hTitle[name] = title;
	hD    [name] = dim;
        hDir  [name] = dir;

    } else { // exists already
	Error("addHistN()","hist with name \"%s\" has been used already!",name.Data());
        delete h;
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HHistMap::addHistArray(TH1* h[],TString name,
			      TString mask,TString title,TString dir,
			      Int_t i1max,Int_t i2max,Int_t i3max,Int_t i4max,Int_t i5max)
{
    // add and array of histograms. the size of the array and
    // a lable "name" to access the array later has to be specified
    // mask  : printf format string for hist names (needed for config file)
    // title : printf format string for hist titles (needed for config file)
    // dir   : dir in otput file
    // Only 1-dim arrays are supported. For more dimensional histogram
    // arrays (up to 5 indices) one should use linearized versions
    // using index made by HTool::getLinearIndex(....)
    // i1max to i5max give the size in the dimension
    // the histograms can later be retrieved by get("name",i1,i2....)


    if(!h[0]) {
	Error("addHistArray()","hist pointer = NULL!");
	return kFALSE;
    }
    if(i1max < 0) {
	Error("addHistArray()","array size < 0!");
	return kFALSE;
    }
    if(name == "") {
	Error("addHistArray()","name is empty < 0!");
	return kFALSE;
    }

    Int_t dim = -1;

    if     (i1max >= 0 && i2max <  0 && i3max <  0 && i4max <  0 && i5max <  0  ) dim = 1;
    else if(i1max >= 0 && i2max >= 0 && i3max <  0 && i4max <  0 && i5max <  0  ) dim = 2;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max <  0 && i5max <  0  ) dim = 3;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max >= 0 && i5max <  0  ) dim = 4;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max >= 0 && i5max >= 0  ) dim = 5;

    if(dim<0) {
	Error("addHistArray()","problem with dimension for hist %s : ind %i %i %i %i %i !",name.Data(),i1max,i2max,i3max,i4max,i5max);
	return kFALSE;
    }

    Int_t size = 0;
    if     (dim == 1) size = i1max;
    else if(dim == 2) size = i1max*i2max;
    else if(dim == 3) size = i1max*i2max*i3max;
    else if(dim == 4) size = i1max*i2max*i3max*i4max;
    else if(dim == 5) size = i1max*i2max*i3max*i4max*i5max;

    map<TString,TObjArray>::iterator iter = hM.find(name);
    if( iter == hM.end() ){ // new variable
	TObjArray a(size);

	for(Int_t i=0;i<size;i++) {
	    if(h[i]) h[i]->SetDirectory(0);
	    else {
		Error("addHistArray()","hist pointer = NULL for index = %i!",i);
	    }
	    a.Add(h[i]);
	}
	TArrayI d(dim);
        if(dim >= 1) d[0] = i1max;
        if(dim >= 2) d[1] = i2max;
        if(dim >= 3) d[2] = i3max;
        if(dim >= 4) d[3] = i4max;
        if(dim >= 5) d[4] = i5max;

	hM    [name] = a;
	hMask [name] = mask;
	hTitle[name] = title;
	hD    [name] = d;
        hDir  [name] = dir;

    } else { // exists already
	Error("addHistArray()","hist with name \"%s\" has been used already!",name.Data());
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HHistMap::addHistArray(TString Type,TString name,TString mask,TString title,
			      Int_t nbinx,Double_t x1,Double_t x2,
			      Int_t nbiny,Double_t y1,Double_t y2,
			      Int_t nbinz,Double_t z1,Double_t z2,
			      TString xtitle,TString ytitle,TString ztitle,
			      TString dir,
			      Int_t i1max,Int_t i2max,Int_t i3max,Int_t i4max,Int_t i5max,
                              TString n1,TString n2,TString n3,TString n4,TString n5
			     )
{


    // add an array of histograms (const binning). the size of the array and
    // a lable "name" to access the array later has to be specified
    // Type : TH1S,TH1I,TH1F,TH1D,TH2S,TH2I,TH2F,TH2D,TH3S,TH3I,TH3F,TH3D
    // name : lable for the histarray
    // mask : histname format like printf for the give numbers of dimensions : "htest[%i][%i]" (2dim array)
    // title : hist title format like printf for the give numbers of dimensions : "htest[%i][%i]" (2dim array)
    //         or same title for all hists (if %i is not contained in format)
    // hist axis : 1d  Int_t nbinx,Double_t x1,Double_t x2,
    //    	   2d  Int_t nbiny,Double_t y1,Double_t y2,
    //    	   3d  Int_t nbinz,Double_t z1,Double_t z2,
    // axis title  xtitle,ytitle,ztitle
    // dir : output dir
    // i1max to i5max give the size in the dimension
    // the histograms can later be retrieved by get("name",i1,i2....)
    // -1 (default) mean not used
    // TString n1,n2,n3,n4,n5 (default empty) can contain a comma separated
    // list of labels for each dimension (example: i1max=4 ,"MDCI,MDCII,MDCIII,MDCIV" )
    // imax and number of labels have to be be equal. The lable
    // will replace the dimension index (example:  htime1_%i => htime1_0 to htime1_3 will
    // become htime1_MDCI to htime1_MDCIV) in the name and title


    if(i1max < 0) {
	Error("addHistArray()","array size < 0!");
	return kFALSE;
    }
    if(name == "") {
	Error("addHistArray()","name is empty < 0!");
	return kFALSE;
    }

    Int_t dim = -1;

    if     (i1max >= 0 && i2max <  0 && i3max <  0 && i4max <  0 && i5max <  0  ) dim = 1;
    else if(i1max >= 0 && i2max >= 0 && i3max <  0 && i4max <  0 && i5max <  0  ) dim = 2;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max <  0 && i5max <  0  ) dim = 3;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max >= 0 && i5max <  0  ) dim = 4;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max >= 0 && i5max >= 0  ) dim = 5;

    if(dim<0) {
	Error("addHistArray()","problem with dimension for hist %s : ind %i %i %i %i %i !",name.Data(),i1max,i2max,i3max,i4max,i5max);
	return kFALSE;
    }

    Int_t size = 0;
    if     (dim == 1) size = i1max;
    else if(dim == 2) size = i1max*i2max;
    else if(dim == 3) size = i1max*i2max*i3max;
    else if(dim == 4) size = i1max*i2max*i3max*i4max;
    else if(dim == 5) size = i1max*i2max*i3max*i4max*i5max;


    Int_t is1,is2,is3,is4,is5;
    is1=is2=is3=is4=is5=0;

    if(n1!="" || n2!="" || n3!="" || n4!="" || n5!=""){
	// if the string option for one or more is used
        // we have to create dummy strings for the rest
	if(n1==""){
	    for(Int_t i = 0; i < i1max; i ++){
		if(i < i1max-1) n1 += Form("%i,",i);
                else            n1 += Form("%i" ,i);
	    }
	}
        if(n2=="" && dim > 1){
	    for(Int_t i = 0; i < i2max; i ++){
		if(i < i2max-1) n2 += Form("%i,",i);
                else            n2 += Form("%i" ,i);
	    }
	}
        if(n3=="" && dim > 2){
	    for(Int_t i = 0; i < i3max; i ++){
		if(i < i3max-1) n3 += Form("%i,",i);
                else            n3 += Form("%i" ,i);
	    }
	}
        if(n4=="" && dim > 3){
	    for(Int_t i = 0; i < i4max; i ++){
		if(i < i4max-1) n4 += Form("%i,",i);
                else            n4 += Form("%i" ,i);
	    }
	}
        if(n5=="" && dim > 4){
	    for(Int_t i = 0; i < i5max; i ++){
		if(i < i5max-1) n5 += Form("%i,",i);
                else            n5 += Form("%i" ,i);
	    }
	}
    }



    TString* ns1 = HTool::parseString(n1,is1,",",kFALSE);
    TString* ns2 = HTool::parseString(n2,is2,",",kFALSE);
    TString* ns3 = HTool::parseString(n3,is3,",",kFALSE);
    TString* ns4 = HTool::parseString(n4,is4,",",kFALSE);
    TString* ns5 = HTool::parseString(n5,is5,",",kFALSE);


    Bool_t useNameArray = kFALSE;

    if(n1 != "" && is1 != i1max)             Error("addHistArray()","Named index String =\"%s\" has not the same size as array index 1 = %i ! Named string will be ignored.",n1.Data(),i1max);
    if(dim > 1 && n2 != "" && is2 != i2max)  Error("addHistArray()","Named index String =\"%s\" has not the same size as array index 2 = %i ! Named string will be ignored.",n2.Data(),i2max);
    if(dim > 2 && n3 != "" && is3 != i3max)  Error("addHistArray()","Named index String =\"%s\" has not the same size as array index 3 = %i ! Named string will be ignored.",n3.Data(),i3max);
    if(dim > 3 && n4 != "" && is4 != i4max)  Error("addHistArray()","Named index String =\"%s\" has not the same size as array index 4 = %i ! Named string will be ignored.",n4.Data(),i4max);
    if(dim > 4 && n5 != "" && is5 != i5max)  Error("addHistArray()","Named index String =\"%s\" has not the same size as array index 5 = %i ! Named string will be ignored.",n5.Data(),i5max);


    if(dim == 1 && is1 != 0
       && is1 == i1max
      ) useNameArray = kTRUE;
    if(dim == 2 && is1 != 0 && is2 != 0
       && is1 == i1max
       && is2 == i2max
      ) useNameArray = kTRUE;
    if(dim == 3 && is1 != 0 && is2 != 0 && is3 != 0
       && is1 == i1max
       && is2 == i2max
       && is3 == i3max
      ) useNameArray = kTRUE;
    if(dim == 4 && is1 != 0 && is2 != 0 && is3 != 0 && is4 != 0
       && is1 == i1max
       && is2 == i2max
       && is3 == i3max
       && is4 == i4max
      ) useNameArray = kTRUE;
    if(dim == 5 && is1 != 0 && is2 != 0 && is3 != 0 && is4 != 0 && is5 != 0
       && is1 == i1max
       && is2 == i2max
       && is3 == i3max
       && is4 == i4max
       && is5 == i5max
      ) useNameArray = kTRUE;



    map<TString,TObjArray>::iterator iter = hM.find(name);
    if( iter == hM.end() ){ // new variable
	TObjArray a(size);

	TH1* h;
	TString current ;
        TString titlenew = title;
        Bool_t doTitle = kFALSE;
        if(title.Contains("%i")) doTitle = kTRUE;

	TString maskS = mask;
        maskS.ReplaceAll("%i","%s");
        TString titleS = title;
        titleS.ReplaceAll("%i","%s");

	for(Int_t i1=0; i1<i1max;i1++){
	    if(dim == 1) {

		if(useNameArray){
		    current = Form(maskS.Data(),ns1[i1].Data());
		    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data());
		} else {
		    current = Form(mask.Data(),i1);
		    if(doTitle) titlenew = Form(title.Data(),i1);
		}

		h = createHist(Type,current,titlenew,nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2,xtitle,ytitle,ztitle);

		if(h == NULL){
		    Error("addHistArray()","hist pointer = NULL for index = %i!",i1);
		    a.Delete();
		    return kFALSE;
		}
		a.Add(h);
	    } else {
		for(Int_t i2=0; i2<i2max;i2++){
		    if(dim == 2) {

			if(useNameArray){
			    current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data());
			    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data());
			} else {
			    current = Form(mask.Data(),i1,i2);
			    if(doTitle) titlenew = Form(title.Data(),i1,i2);
			}

			h = createHist(Type,current,titlenew,nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2,xtitle,ytitle,ztitle);
			if(h == NULL){
			    Error("addHistArray()","hist pointer = NULL for index = %i!",i2);
			    a.Delete();
			    return kFALSE;
			}
			a.Add(h);

		    } else {
			for(Int_t i3=0; i3<i3max;i3++){
			    if(dim == 3) {

				if(useNameArray){
				    current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data());
				    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data());
				} else {
				    current = Form(mask.Data(),i1,i2,i3);
				    if(doTitle) titlenew = Form(title.Data(),i1,i2,i3);
				}

				h = createHist(Type,current,titlenew,nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2,xtitle,ytitle,ztitle);
				if(h == NULL){
				    Error("addHistArray()","hist pointer = NULL for index = %i!",i3);
				    a.Delete();
				    return kFALSE;
				}
				a.Add(h);

			    } else {
				for(Int_t i4=0; i4<i4max;i4++){
				    if(dim == 4) {

					if(useNameArray){
					    current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data());
					    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data());
					} else {
					    current = Form(mask.Data(),i1,i2,i3,i4);
					    if(doTitle) titlenew = Form(title.Data(),i1,i2,i3,i4);
					}

					h = createHist(Type,current,titlenew,nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2,xtitle,ytitle,ztitle);
					if(h == NULL){
					    Error("addHistArray()","hist pointer = NULL for index = %i!",i4);
					    a.Delete();
					    return kFALSE;
					}
					a.Add(h);
				    } else {
					for(Int_t i5=0; i5<i5max;i5++){

					    if(useNameArray){
						current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data(),ns5[i5].Data());
						if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data(),ns5[i5].Data());
					    } else {
						current = Form(mask.Data(),i1,i2,i3,i4,i5);
						if(doTitle) titlenew = Form(title.Data(),i1,i2,i3,i4,i5);
					    }

					    h = createHist(Type,current,titlenew,nbinx,x1,x2,nbiny,y1,y2,nbinz,z1,z2,xtitle,ytitle,ztitle);
					    if(h == NULL){
						Error("addHistArray()","hist pointer = NULL for index = %i!",i5);
						a.Delete();
						return kFALSE;
					    }
					    a.Add(h);
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}

	TArrayI d(dim);
	if(dim >= 1) d[0] = i1max;
	if(dim >= 2) d[1] = i2max;
	if(dim >= 3) d[2] = i3max;
        if(dim >= 4) d[3] = i4max;
        if(dim >= 5) d[4] = i5max;

	hM    [name] = a;
	hMask [name] = mask;
	hTitle[name] = title;
	hD    [name] = d;
        hDir  [name] = dir;


    } else { // exists already
	Error("addHistArray()","hist with name \"%s\" has been used already!",name.Data());

        if(ns1) delete [] ns1;
        if(ns2) delete [] ns2;
	if(ns3) delete [] ns3;
	if(ns4) delete [] ns4;
	if(ns5) delete [] ns5;

	return kFALSE;
    }

    if(ns1) delete [] ns1;
    if(ns2) delete [] ns2;
    if(ns3) delete [] ns3;
    if(ns4) delete [] ns4;
    if(ns5) delete [] ns5;

    return kTRUE;
}

Bool_t HHistMap::addHistArrayN(TString Type,TString name,TString mask,TString title,
			       Int_t nbinx,Double_t* x,
			       Int_t nbiny,Double_t* y,
			       Int_t nbinz,Double_t* z,
			       TString xtitle,TString ytitle,TString ztitle,
			       TString dir,
			       Int_t i1max,Int_t i2max,Int_t i3max,Int_t i4max,Int_t i5max,
			       TString n1,TString n2,TString n3,TString n4,TString n5
			      )
{


    // add an array of histograms (non const binning). the size of the array and
    // a lable "name" to access the array later has to be specified
    // Type : TH1S,TH1I,TH1F,TH1D,TH2S,TH2I,TH2F,TH2D,TH3S,TH3I,TH3F,TH3D
    // name : lable for the histarray
    // mask : histname format like printf for the give numbers of dimensions : "htest[%i][%i]" (2dim array)
    // title : hist title format like printf for the give numbers of dimensions : "htest[%i][%i]" (2dim array)
    //         or same title for all hists (if %i is not contained in format)
    // hist axis : 1d  Int_t nbinx, pointer to Double_t array x for bins,
    //    	   2d  Int_t nbiny, pointer to Double_t array y for bins,
    //    	   3d  Int_t nbinz, pointer to Double_t array z for bins,
    // axis title  xtitle,ytitle,ztitle
    // dir : output dir
    // i1max to i5max give the size in the dimension
    // the histograms can later be retrieved by get("name",i1,i2....)
    // -1 (default) mean not used
    // TString n1,n2,n3,n4,n5 (default empty) can contain a comma separated
    // list of labels for each dimension (example: i1max=4 ,"MDCI,MDCII,MDCIII,MDCIV" )
    // imax and number of labels have to be be equal. The lable
    // will replace the dimension index (example:  htime1_%i => htime1_0 to htime1_3 will
    // become htime1_MDCI to htime1_MDCIV) in the name and title


    if(i1max < 0) {
	Error("addHistArrayN()","array size < 0!");
	return kFALSE;
    }
    if(name == "") {
	Error("addHistArrayN()","name is empty < 0!");
	return kFALSE;
    }

    Int_t dim = -1;

    if     (i1max >= 0 && i2max <  0 && i3max <  0 && i4max <  0 && i5max <  0  ) dim = 1;
    else if(i1max >= 0 && i2max >= 0 && i3max <  0 && i4max <  0 && i5max <  0  ) dim = 2;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max <  0 && i5max <  0  ) dim = 3;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max >= 0 && i5max <  0  ) dim = 4;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max >= 0 && i5max >= 0  ) dim = 5;

    if(dim<0) {
	Error("addHistArrayN()","problem with dimension for hist %s : ind %i %i %i %i %i !",name.Data(),i1max,i2max,i3max,i4max,i5max);
	return kFALSE;
    }

    Int_t size = 0;
    if     (dim == 1) size = i1max;
    else if(dim == 2) size = i1max*i2max;
    else if(dim == 3) size = i1max*i2max*i3max;
    else if(dim == 4) size = i1max*i2max*i3max*i4max;
    else if(dim == 5) size = i1max*i2max*i3max*i4max*i5max;


    Int_t is1,is2,is3,is4,is5;
    is1=is2=is3=is4=is5=0;

    if(n1!="" || n2!="" || n3!="" || n4!="" || n5!=""){
	// if the string option for one or more is used
        // we have to create dummy strings for the rest
	if(n1==""){
	    for(Int_t i = 0; i < i1max; i ++){
		if(i < i1max-1) n1 += Form("%i,",i);
                else            n1 += Form("%i" ,i);
	    }
	}
        if(n2=="" && dim > 1){
	    for(Int_t i = 0; i < i2max; i ++){
		if(i < i2max-1) n2 += Form("%i,",i);
                else            n2 += Form("%i" ,i);
	    }
	}
        if(n3=="" && dim > 2){
	    for(Int_t i = 0; i < i3max; i ++){
		if(i < i3max-1) n3 += Form("%i,",i);
                else            n3 += Form("%i" ,i);
	    }
	}
        if(n4=="" && dim > 3){
	    for(Int_t i = 0; i < i4max; i ++){
		if(i < i4max-1) n4 += Form("%i,",i);
                else            n4 += Form("%i" ,i);
	    }
	}
        if(n5=="" && dim > 4){
	    for(Int_t i = 0; i < i5max; i ++){
		if(i < i5max-1) n5 += Form("%i,",i);
                else            n5 += Form("%i" ,i);
	    }
	}
    }



    TString* ns1 = HTool::parseString(n1,is1,",",kFALSE);
    TString* ns2 = HTool::parseString(n2,is2,",",kFALSE);
    TString* ns3 = HTool::parseString(n3,is3,",",kFALSE);
    TString* ns4 = HTool::parseString(n4,is4,",",kFALSE);
    TString* ns5 = HTool::parseString(n5,is5,",",kFALSE);


    Bool_t useNameArray = kFALSE;

    if(n1 != "" && is1 != i1max)             Error("addHistArrayN()","Named index String =\"%s\" has not the same size as array index 1 = %i ! Named string will be ignored.",n1.Data(),i1max);
    if(dim > 1 && n2 != "" && is2 != i2max)  Error("addHistArrayN()","Named index String =\"%s\" has not the same size as array index 2 = %i ! Named string will be ignored.",n2.Data(),i2max);
    if(dim > 2 && n3 != "" && is3 != i3max)  Error("addHistArrayN()","Named index String =\"%s\" has not the same size as array index 3 = %i ! Named string will be ignored.",n3.Data(),i3max);
    if(dim > 3 && n4 != "" && is4 != i4max)  Error("addHistArrayN()","Named index String =\"%s\" has not the same size as array index 4 = %i ! Named string will be ignored.",n4.Data(),i4max);
    if(dim > 4 && n5 != "" && is5 != i5max)  Error("addHistArrayN()","Named index String =\"%s\" has not the same size as array index 5 = %i ! Named string will be ignored.",n5.Data(),i5max);


    if(dim == 1 && is1 != 0
       && is1 == i1max
      ) useNameArray = kTRUE;
    if(dim == 2 && is1 != 0 && is2 != 0
       && is1 == i1max
       && is2 == i2max
      ) useNameArray = kTRUE;
    if(dim == 3 && is1 != 0 && is2 != 0 && is3 != 0
       && is1 == i1max
       && is2 == i2max
       && is3 == i3max
      ) useNameArray = kTRUE;
    if(dim == 4 && is1 != 0 && is2 != 0 && is3 != 0 && is4 != 0
       && is1 == i1max
       && is2 == i2max
       && is3 == i3max
       && is4 == i4max
      ) useNameArray = kTRUE;
    if(dim == 5 && is1 != 0 && is2 != 0 && is3 != 0 && is4 != 0 && is5 != 0
       && is1 == i1max
       && is2 == i2max
       && is3 == i3max
       && is4 == i4max
       && is5 == i5max
      ) useNameArray = kTRUE;

    map<TString,TObjArray>::iterator iter = hM.find(name);
    if( iter == hM.end() ){ // new variable
	TObjArray a(size);

	TH1* h;
	TString current ;
        TString titlenew = title;
        Bool_t doTitle = kFALSE;
        if(title.Contains("%i")) doTitle = kTRUE;

	TString maskS = mask;
        maskS.ReplaceAll("%i","%s");
        TString titleS = title;
        titleS.ReplaceAll("%i","%s");

	for(Int_t i1=0; i1<i1max;i1++){
	    if(dim == 1) {

		if(useNameArray){
		    current = Form(maskS.Data(),ns1[i1].Data());
		    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data());
		} else {
		    current = Form(mask.Data(),i1);
		    if(doTitle) titlenew = Form(title.Data(),i1);
		}

		h = createHistN(Type,current,titlenew,nbinx,x,nbiny,y,nbinz,z,xtitle,ytitle,ztitle);

		if(h == NULL){
		    Error("addHistArrayN()","hist pointer = NULL for index = %i!",i1);
		    a.Delete();
		    return kFALSE;
		}
		a.Add(h);
	    } else {
		for(Int_t i2=0; i2<i2max;i2++){
		    if(dim == 2) {

			if(useNameArray){
			    current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data());
			    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data());
			} else {
			    current = Form(mask.Data(),i1,i2);
			    if(doTitle) titlenew = Form(title.Data(),i1,i2);
			}

			h = createHistN(Type,current,titlenew,nbinx,x,nbiny,y,nbinz,z,xtitle,ytitle,ztitle);
			if(h == NULL){
			    Error("addHistArrayN()","hist pointer = NULL for index = %i!",i2);
			    a.Delete();
			    return kFALSE;
			}
			a.Add(h);

		    } else {
			for(Int_t i3=0; i3<i3max;i3++){
			    if(dim == 3) {
		     
				if(useNameArray){
				    current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data());
				    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data());
				} else {
				    current = Form(mask.Data(),i1,i2,i3);
				    if(doTitle) titlenew = Form(title.Data(),i1,i2,i3);
				}

				h = createHistN(Type,current,titlenew,nbinx,x,nbiny,y,nbinz,z,xtitle,ytitle,ztitle);
				if(h == NULL){
				    Error("addHistArrayN()","hist pointer = NULL for index = %i!",i3);
				    a.Delete();
				    return kFALSE;
				}
				a.Add(h);

			    } else {
				for(Int_t i4=0; i4<i4max;i4++){
				    if(dim == 4) {

					if(useNameArray){
					    current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data());
					    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data());
					} else {
					    current = Form(mask.Data(),i1,i2,i3,i4);
					    if(doTitle) titlenew = Form(title.Data(),i1,i2,i3,i4);
					}

					h = createHistN(Type,current,titlenew,nbinx,x,nbiny,y,nbinz,z,xtitle,ytitle,ztitle);
					if(h == NULL){
					    Error("addHistArrayN()","hist pointer = NULL for index = %i!",i4);
					    a.Delete();
					    return kFALSE;
					}
					a.Add(h);
				    } else {
					for(Int_t i5=0; i5<i5max;i5++){

					    if(useNameArray){
						current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data(),ns5[i5].Data());
						if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data(),ns5[i5].Data());
					    } else {
						current = Form(mask.Data(),i1,i2,i3,i4,i5);
						if(doTitle) titlenew = Form(title.Data(),i1,i2,i3,i4,i5);
					    }

					    h = createHistN(Type,current,titlenew,nbinx,x,nbiny,y,nbinz,z,xtitle,ytitle,ztitle);
					    if(h == NULL){
						Error("addHistArrayN()","hist pointer = NULL for index = %i!",i5);
						a.Delete();
						return kFALSE;
					    }
					    a.Add(h);
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}

	TArrayI d(dim);
	if(dim >= 1) d[0] = i1max;
	if(dim >= 2) d[1] = i2max;
	if(dim >= 3) d[2] = i3max;
        if(dim >= 4) d[3] = i4max;
        if(dim >= 5) d[4] = i5max;

	hM    [name] = a;
	hMask [name] = mask;
	hTitle[name] = title;
	hD    [name] = d;
        hDir  [name] = dir;

    } else { // exists already
	Error("addHistArrayN()","hist with name \"%s\" has been used already!",name.Data());

        if(ns1) delete [] ns1;
        if(ns2) delete [] ns2;
	if(ns3) delete [] ns3;
	if(ns4) delete [] ns4;
	if(ns5) delete [] ns5;

	return kFALSE;
    }

    if(ns1) delete [] ns1;
    if(ns2) delete [] ns2;
    if(ns3) delete [] ns3;
    if(ns4) delete [] ns4;
    if(ns5) delete [] ns5;

    return kTRUE;
}

TH1* HHistMap::get(TString name,Int_t i1,Int_t i2,Int_t i3,Int_t i4,Int_t i5)
{
    // get histogram from map. print error and map
    // if not existing. the map has to be set before
    // If the requested object is an array of histograms
    // the index has to specified.


    map<TString,TObjArray>::iterator iter = hM.find(name.Data());
    if( iter != hM.end() ) {
	TArrayI& ar = hD[(*iter).first];
	Int_t s = ar.GetSize();
	Int_t index = 0 ;
	if     (s == 1 && ar.At(0) > 0 ) index = i1;
	else if(s == 2 )                 index = HTool::getLinearIndex(i1,ar.At(0),i2,ar.At(1));
        else if(s == 3 )                 index = HTool::getLinearIndex(i1,ar.At(0),i2,ar.At(1),i3,ar.At(2));
        else if(s == 4 )                 index = HTool::getLinearIndex(i1,ar.At(0),i2,ar.At(1),i3,ar.At(2),i4,ar.At(3));
        else if(s == 5 )                 index = HTool::getLinearIndex(i1,ar.At(0),i2,ar.At(1),i3,ar.At(2),i4,ar.At(3),i5,ar.At(4));


	if(index<0) {
	    if(!fSilentFail)Error("get()","Retrieved NULL pointer for %s , index = ind %i %i %i %i %i !",name.Data(),i1,i2,i3,i4,i5);
            return 0;
	}
	return (TH1*)(*iter).second.At(index) ;
    }
    else {
	if(!fSilentFail) {
	    printMap();
	    Error("get()", "No matching histogram found for name = %s",name.Data());
	}
    }
    return 0;
}

TProfile* HHistMap::getP(TString name,Int_t i1,Int_t i2,Int_t i3,Int_t i4,Int_t i5)
{
    // get Tprofile from map. print error and map
    // if not existing. the map has to be set before
    // If the requested object is an array of histograms
    // the index has to specified.
     return (TProfile*)get(name,i1,i2,i3,i4,i5);
}

TH2* HHistMap::get2(TString name,Int_t i1,Int_t i2,Int_t i3,Int_t i4,Int_t i5)
{
    // get TH2 histogram from map (do not call it on other hist types!).
    // print error and map if not existing. the map has to be set before
    // If the requested object is an array of histograms
    // the index has to specified.
    TH1* h = get(name,i1,i2,i3,i4,i5);
    return (TH2*) h;
}

TProfile2D* HHistMap::get2P(TString name,Int_t i1,Int_t i2,Int_t i3,Int_t i4,Int_t i5)
{
    // get TProfile2D from map (do not call it on other hist types!).
    // print error and map if not existing. the map has to be set before
    // If the requested object is an array of histograms
    // the index has to specified.
    TH1* h = get(name,i1,i2,i3,i4,i5);
    return (TProfile2D*) h;
}

TH3* HHistMap::get3(TString name,Int_t i1,Int_t i2,Int_t i3,Int_t i4,Int_t i5)
{
    // get TH3 histogram from map (do not call it on other hist types!).
    // print error and map if not existing. the map has to be set before
    // If the requested object is an array of histograms
    // the index has to specified.
    TH1* h = get(name,i1,i2,i3,i4,i5);
    return (TH3*) h;
}

TProfile3D* HHistMap::get3P(TString name,Int_t i1,Int_t i2,Int_t i3,Int_t i4,Int_t i5)
{
    // get TProfile3D histogram from map (do not call it on other hist types!).
    // print error and map if not existing. the map has to be set before
    // If the requested object is an array of histograms
    // the index has to specified.
    TH1* h = get(name,i1,i2,i3,i4,i5);
    return (TProfile3D*) h;
}

vector < TString> HHistMap::getMatchingHistLabels(TString expression,TString type,Int_t dim)
{
    // return stl vector of label name matching the expression.
    // if type !="" the hist type is evaluated too.
    // if dim  !=-1 the dimension of the array is evalated in addition.

    TPRegexp filter(expression);
    TString label ;
    TString classname;
    vector <TString> out;
    for(map< TString, TObjArray>::iterator iter = hM.begin(); iter != hM.end(); ++iter ) {
         label =  (*iter).first;
	 if(filter.Match(label)){

	     classname = "unused";
	     classname = (*iter).second.At(0)->ClassName();
	     Int_t dim1 = hD[(*iter).first].GetSize();
	     if(type.CompareTo("") != 0 && classname.CompareTo(type.Data())!=0) continue;
             if(dim != -1 && dim1 != dim)                                       continue;
             out.push_back(label);
	 }
    }
    return out;
}

vector < TH1*> HHistMap::getMatchingHists(TString expression,TString type,Int_t dim)
{
    // return stl vector of TH1*  matching the expression (by histname).
    // if type !="" the hist type is evaluated too.
    // if dim  !=-1 the dimension of the array is evalated in addition.

    TPRegexp filter(expression);
    TString label ;
    TString classname;
    vector <TH1*> out;
    for(map< TString, TObjArray>::iterator iter = hM.begin(); iter != hM.end(); ++iter ) {
	classname = "unused";
	classname = (*iter).second.At(0)->ClassName();
	Int_t dim1 = hD[(*iter).first].GetSize();
	if(type.CompareTo("") != 0 && classname.CompareTo(type.Data())!=0) continue;
	if(dim != -1 && dim1 != dim)                                       continue;

	Int_t size = (*iter).second.GetEntries();
	for(Int_t i=0;i<size;i++){
            TH1* h = ((TH1*)((*iter).second.At(i)));
	    label  = h->GetName();
	    if(filter.Match(label)){
		out.push_back(h);
	    }
	}
    }
    return out;
}


void HHistMap::resetAllHists(TString opt)
{
    // reset available histograms
    for(map< TString, TObjArray>::iterator iter = hM.begin(); iter != hM.end(); ++iter ) {
         resetArray(iter->first, opt);
    }
}
void HHistMap::disconnectAllHists()
{
    // set Directory of available histograms  0
    for(map< TString, TObjArray>::iterator iter = hM.begin(); iter != hM.end(); ++iter ) {
	TObjArray* array = getHistArray(iter->first) ;
	if(array){
	    Int_t n = array->GetSize();  // full size, check for NULL in loop!
	    for(Int_t i = 0 ;i < n; i++) {
		TH1* h = (TH1*)array->At(i);
		if(h) {
		    h->SetDirectory(0);
		}
	    }
	}
    }
}
//--------------------------------------------------------------------
// array operations
Bool_t HHistMap::cloneArray (TString source,TString name,TString mask,TString title,TString dir,Bool_t reset)
{
    // clones the source array to new array "name" with histograms following
    // the naming mask "mask" , title and dir (see addHistArray())
    // the procedure used THXX::Copy() . All features of the source hist
    // and the content will be copied (Not the list of functions!).
    // if reset is kTRUE the new hists will be reset.


    TObjArray* src = HHistMap::getHistArray(source);
    if(src){

	TArrayI* dim = getDimArray(source);
        Int_t nArray =dim->GetSize();
	Int_t max  [5] = {-1,-1,-1,-1,-1};
	Int_t nbins[3] = {0,0,0};

	for(Int_t j = 0 ;j < dim->GetSize(); j++){
	    max[j] = dim->At(j);
	}

        TH1* h = (TH1*)src->At(0);
        TString Type = h->ClassName();

        Int_t ndim = h->GetDimension();

	nbins[0] = h->GetNbinsX();
	if(h->GetNbinsY()>1)  nbins[1] = h->GetNbinsY() ;
        if(h->GetNbinsZ()>1)  nbins[2] = h->GetNbinsZ() ;

	TString xtitle = h->GetXaxis()->GetTitle();
	TString ytitle = "";
	TString ztitle = "";
	if(h->GetYaxis()) ytitle = h->GetYaxis()->GetTitle();
	if(h->GetZaxis()) ztitle = h->GetZaxis()->GetTitle();

	Double_t x1 = 0, x2 = 0;
	Double_t y1 = 0, y2 = 0;
	Double_t z1 = 0, z2 = 0;

        x1 = h->GetXaxis()->GetXmin();
        x2 = h->GetXaxis()->GetXmax();
	if(ndim>1){
	    y1 = h->GetYaxis()->GetXmin();
	    y2 = h->GetYaxis()->GetXmax();
	}
	if(ndim>2){
	    z1 = h->GetZaxis()->GetXmin();
	    z2 = h->GetZaxis()->GetXmax();
	}

	vector<TString> labels;
        TString labelsdim[5];

	if(nArray>=1){

            Bool_t isIndex = kTRUE;
	    for(Int_t i1 = 0; i1 < max[0]; i1++){
                TH1* h = (TH1*)src->At(i1);
                const TString name = h->GetName();
		getLabels(name,labels);
                if(!labels[0].IsDigit()  && labels[0].Atoi()!=i1) isIndex = kFALSE;
		labelsdim[0] += labels[0];
                if(i1!=max[0]-1)labelsdim[0] += ",";
	    }

            if(isIndex)  labelsdim[0] = "";
	}

	if(nArray>=2){
            Bool_t isIndex = kTRUE;
	    for(Int_t i2 = 0; i2 < max[1]; i2++){
		TH1* h = (TH1*)src->At(i2);
		const TString name = h->GetName();
		getLabels(name,labels);
		if(!labels[1].IsDigit()  && labels[1].Atoi()!=i2) isIndex = kFALSE;
		labelsdim[1] += labels[1];
		if(i2!=max[1]-1)labelsdim[1] += ",";
	    }
	    if(isIndex)  labelsdim[1] = "";
	}

	if(nArray>=3){
	    Bool_t isIndex = kTRUE;
	    for(Int_t i3 = 0; i3 < max[2]; i3++){
		TH1* h = (TH1*)src->At(i3);
		const TString name = h->GetName();
		getLabels(name,labels);
		if(!labels[2].IsDigit()  && labels[2].Atoi()!=i3) isIndex = kFALSE;
		labelsdim[2] += labels[2];
		if(i3!=max[2]-1)labelsdim[2] += ",";
	    }
	    if(isIndex)  labelsdim[2] = "";
	}

	if(nArray>=4){
	    Bool_t isIndex = kTRUE;
	    for(Int_t i4 = 0; i4 < max[3]; i4++){
		TH1* h = (TH1*)src->At(i4);
		const TString name = h->GetName();
		getLabels(name,labels);
		if(!labels[3].IsDigit()  && labels[3].Atoi()!=i4) isIndex = kFALSE;
		labelsdim[3] += labels[3];
		if(i4!=max[3]-1)labelsdim[3] += ",";
	    }
            if(isIndex)  labelsdim[3] = "";
	}

	if(nArray==5) {
	    Bool_t isIndex = kTRUE;
	    for(Int_t i5 = 0; i5 < max[4]; i5++){
		TH1* h = (TH1*)src->At(i5);
		const TString name = h->GetName();
		getLabels(name,labels);
		if(!labels[4].IsDigit()  && labels[4].Atoi()!=i5) isIndex = kFALSE;
		labelsdim[4] += labels[4];
		if(i5!=max[4]-1)labelsdim[4] += ",";
	    }
            if(isIndex)  labelsdim[4] = "";
	}

	Bool_t isVariable = h->GetXaxis()->IsVariableBinSize();
	if(!isVariable && ndim > 1){
             isVariable = h->GetYaxis()->IsVariableBinSize();
	     if(!isVariable && ndim > 2){
		 isVariable = h->GetZaxis()->IsVariableBinSize();
	     }
	}

	if(!isVariable){
	    addHistArray(Type,name,mask,title,
			 nbins[0], x1,x2,
			 nbins[1], y1,y2,
			 nbins[2], z1,z2,
			 xtitle,ytitle,ztitle,
			 dir,
			 max[0],max[1],max[2],max[3],max[4],
			 labelsdim[0],labelsdim[1],labelsdim[2],labelsdim[3],labelsdim[4]
			);

	} else {

	    TArrayD* xA = (TArrayD*)h->GetXaxis() -> GetXbins();
            TArrayD* yA = 0;
            TArrayD* zA = 0;
	    if(ndim==2){
		yA =(TArrayD*) h->GetYaxis() -> GetXbins();
	    }
	    if(ndim==3){
		yA = (TArrayD*)h->GetYaxis() -> GetXbins();
		zA = (TArrayD*)h->GetZaxis() -> GetXbins();
	    }

	    addHistArrayN(Type,name,mask,title,
			  nbins[0], xA->GetArray(),
			  nbins[1], yA->GetArray(),
			  nbins[2], zA->GetArray(),
			  xtitle,ytitle,ztitle,
			  dir,
			  max[0],max[1],max[2],max[3],max[4],
			  labelsdim[0],labelsdim[1],labelsdim[2],labelsdim[3],labelsdim[4]
			 );
 
	}

        TObjArray* clone = HHistMap::getHistArray(name);

	if(clone&&src && clone->GetEntries()==src->GetEntries()){
	    // exchange clone hist by src hist copy
	    // (content+graphhic features. list of functions is not copied!)

	    for(Int_t i=0;i<src->GetEntries();i++){

                   TH1* hsrc     = (TH1*)src->At(i);
		   TH1* hclone   = (TH1*)clone->At(i);

		   TString name  = hclone->GetName();
		   TString title = hclone->GetTitle();

 		   if     (Type == "TH1S")  ((TH1S*)hsrc)->Copy(*hclone);
		   else if(Type == "TH1I")  ((TH1I*)hsrc)->Copy(*hclone);
		   else if(Type == "TH1F")  ((TH1F*)hsrc)->Copy(*hclone);
		   else if(Type == "TH1D")  ((TH1D*)hsrc)->Copy(*hclone);
		   else if(Type == "TH2S")  ((TH2S*)hsrc)->Copy(*hclone);
		   else if(Type == "TH2I")  ((TH2I*)hsrc)->Copy(*hclone);
		   else if(Type == "TH2F")  ((TH2F*)hsrc)->Copy(*hclone);
		   else if(Type == "TH2D")  ((TH2D*)hsrc)->Copy(*hclone);
		   else if(Type == "TH3S")  ((TH3S*)hsrc)->Copy(*hclone);
		   else if(Type == "TH3I")  ((TH3I*)hsrc)->Copy(*hclone);
		   else if(Type == "TH3F")  ((TH3F*)hsrc)->Copy(*hclone);
		   else if(Type == "TH3D")  ((TH3D*)hsrc)->Copy(*hclone);
		   else {
                      Error("cloneArray()","Unknown type %s!",Type.Data());
		   }

                   hclone->SetName(name.Data());
		   hclone->SetTitle(title.Data());

                   if(reset)hclone->Reset();
	    }

	} else {
           Error("cloneArray()","clone array and source array do not have the same size!");
           return kFALSE;
	}

    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::resetArray(TString name    , TString opt)
{
    // reset array of histograms
    TObjArray* ar = getHistArray(name);
    return resetArray(ar,opt);
}


Bool_t HHistMap::resetArray(TObjArray* array, TString opt)
{
    // reset array of histograms
    if(array){
	Int_t n = array->GetSize();  // full size, check for NULL in loop!
	for(Int_t i = 0 ;i < n; i++) {
	    TH1* h = (TH1*)array->At(i);
	    if(h) {
		h->Reset(opt);
	    }
	}
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::scaleArray(TString name    , Double_t fact)
{
    // scale array of histograms by factor fact
    TObjArray* ar = getHistArray(name);
    return scaleArray(ar,fact);
}


Bool_t HHistMap::scaleArray(TObjArray* array, Double_t fact)
{
    // scale array of histograms by factor fact
    if(array){
	Int_t n = array->GetSize();  // full size, check for NULL in loop!
	for(Int_t i = 0 ;i < n; i++) {
	    TH1* h = (TH1*)array->At(i);
	    if(h) h->Scale(fact);
	}
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::sumW2Array(TString name )
{
    // call sumW2 for an array of histograms
    TObjArray* ar = getHistArray(name);
    return sumW2Array(ar);
}


Bool_t HHistMap::sumW2Array(TObjArray* array)
{
    // call sumW2 for an array of histograms
    if(array){
	Int_t n = array->GetSize();  // full size, check for NULL in loop!
	for(Int_t i = 0 ;i < n; i++) {
	    TH1* h = (TH1*)array->At(i);
	    if(h) h->Sumw2();
	}
    } else {
	return kFALSE;
    }

    return kTRUE;
}


Bool_t HHistMap::divideArray(TString name1,TString name2, Double_t fact1, Double_t fact2,TString opt)
{
    // divide two arrays of histograms (synthax see
    // TH1::Divide(TH1*,TH2* ,Double _t fact1,Double _t fact2,TString opt))
    // hist1 will be modified

    TObjArray* ar1 = getHistArray(name1);
    TObjArray* ar2 = getHistArray(name2);

    if(ar1 && ar2) {
       return  divideArray(ar1,ar2,fact1,fact2,opt);
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::divideArray(TString name0, TString name1,TString name2, Double_t fact1, Double_t fact2,TString opt)
{
    // divide two arrays of histograms (syntax see
    // TH1::Divide(TH1*,TH2* ,Double _t fact1,Double _t fact2,TString opt))
    // and store the result in a new histogram
    // hist0 will be modified

    TObjArray* ar0 = getHistArray(name0);
    TObjArray* ar1 = getHistArray(name1);
    TObjArray* ar2 = getHistArray(name2);

    if(ar0 && ar1 && ar2) {
       return  divideArray(ar0,ar1,ar2,fact1,fact2,opt);
    } else {
	return kFALSE;
    }

    return kTRUE;
}
Bool_t HHistMap::divideArray(TObjArray* array1,TObjArray* array2, Double_t fact1, Double_t fact2,TString opt)
{
    // divide two arrays of histograms (syntax see
    // TH1::Divide(TH1*,TH2* ,Double _t fact1,Double _t fact2,TString opt))

    if(array1 && array2){
	Int_t n1 = array1->GetSize();  // full size, check for NULL in loop!
	Int_t n2 = array2->GetSize();  // full size, check for NULL in loop!

        if(n1 != n2) return kFALSE;

	for(Int_t i = 0 ;i < n1; i++) {
	    TH1* h1 = (TH1*)array1->At(i);
            TH1* h2 = (TH1*)array2->At(i);

	    if(h1 && h2) h1->Divide(h1,h2,fact1,fact2,opt);
	}
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::divideArray(TObjArray* array0,TObjArray* array1,TObjArray* array2, Double_t fact1, Double_t fact2,TString opt)
{
    // divide two arrays of histograms (syntax see
    // TH1::Divide(TH1*,TH2* ,Double _t fact1,Double _t fact2,TString opt))
    // and store the result in a new hist (hist0)
    if(array0 && array1 && array2){

	Int_t n0 = array0->GetSize();  // full size, check for NULL in loop!
	Int_t n1 = array1->GetSize();  // full size, check for NULL in loop!
	Int_t n2 = array2->GetSize();  // full size, check for NULL in loop!

        if(n0 != n1 || n1 != n2) return kFALSE;

	for(Int_t i = 0 ;i < n1; i++) {
	    TH1* h0 = (TH1*)array0->At(i);
 	    TH1* h1 = (TH1*)array1->At(i);
            TH1* h2 = (TH1*)array2->At(i);
	    if(h0 && h1 && h2) h0->Divide(h1,h2,fact1,fact2,opt);
	}
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::multiplyArray(TString name1    ,TString name2    , Double_t fact1, Double_t fact2,TString opt)
{
    // multiply two arrays of histograms (synthax see
    // TH1::Multiply(TH1*,TH2* ,Double _t fact1,Double _t fact2,TString opt))

    TObjArray* ar1 = getHistArray(name1);
    TObjArray* ar2 = getHistArray(name2);

    if(ar1 && ar2) {
       return  multiplyArray(ar1,ar2,fact1,fact2,opt);
    } else {
	return kFALSE;
    }

    return kTRUE;
}
Bool_t HHistMap::multiplyArray(TString name0,TString name1,TString name2, Double_t fact1, Double_t fact2,TString opt)
{
    // multiply two arrays of histograms (synthax see
    // TH1::Multiply(TH1*,TH2* ,Double _t fact1,Double _t fact2,TString opt))
    // and store reult in a new histogram (hist0)
    TObjArray* ar0 = getHistArray(name0);
    TObjArray* ar1 = getHistArray(name1);
    TObjArray* ar2 = getHistArray(name2);

    if(ar0 && ar1 && ar2) {
       return  multiplyArray(ar0,ar1,ar2,fact1,fact2,opt);
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::multiplyArray(TObjArray* array1,TObjArray* array2, Double_t fact1, Double_t fact2,TString opt)
{
    // divide two arrays of histograms (synthax see
    // TH1::Multiply(TH1*,TH2* ,Double _t fact1,Double _t fact2,TString opt))

    if(array1 && array2){
	Int_t n1 = array1->GetSize();  // full size, check for NULL in loop!
	Int_t n2 = array2->GetSize();  // full size, check for NULL in loop!

        if(n1 != n2) return kFALSE;

	for(Int_t i = 0 ;i < n1; i++) {
	    TH1* h1 = (TH1*)array1->At(i);
            TH1* h2 = (TH1*)array2->At(i);

	    if(h1 && h2) h1->Multiply(h1,h2,fact1,fact2,opt);
	}
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::multiplyArray(TObjArray* array0,TObjArray* array1,TObjArray* array2, Double_t fact1, Double_t fact2,TString opt)
{
    // divide two arrays of histograms (synthax see
    // TH1::Multiply(TH1*,TH1* ,Double _t fact1,Double _t fact2,TString opt))
    // and stiore result in a new histogram (hist0)
    if(array0 && array1 && array2){
	Int_t n0 = array0->GetSize();  // full size, check for NULL in loop!
	Int_t n1 = array1->GetSize();  // full size, check for NULL in loop!
	Int_t n2 = array2->GetSize();  // full size, check for NULL in loop!

        if(n0 != n1 || n1 != n2) return kFALSE;

	for(Int_t i = 0 ;i < n1; i++) {
	    TH1* h0 = (TH1*)array0->At(i);
	    TH1* h1 = (TH1*)array1->At(i);
            TH1* h2 = (TH1*)array2->At(i);
	    if(h0 && h1 && h2) h0->Multiply(h1,h2,fact1,fact2,opt);
	}
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::addArray(TString name1,TString name2, Double_t fact1, Double_t fact2,TString opt)
{
    // add two arrays of histograms (syntax see
    // TH1::Add(TH1*,TH1* ,Double _t fact1,Double _t fact2,TString opt))

    TObjArray* ar1 = getHistArray(name1);
    TObjArray* ar2 = getHistArray(name2);

    if(ar1 && ar2) {
       return  addArray(ar1,ar2,fact1,fact2);
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::addArray(TString name0,TString name1,TString name2, Double_t fact1, Double_t fact2,TString opt)
{
    // add two arrays of histograms (syntax see
    // TH1::Add(TH1*,TH1* ,Double _t fact1,Double _t fact2,TString opt))
    // and store the result in a new histogram (hist0)
    TObjArray* ar0 = getHistArray(name0);
    TObjArray* ar1 = getHistArray(name1);
    TObjArray* ar2 = getHistArray(name2);

    if(ar0 && ar1 && ar2) {
       return  addArray(ar0,ar1,ar2,fact1,fact2);
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::addArray(TObjArray* array1,TObjArray* array2, Double_t fact1, Double_t fact2,TString opt)
{
    // add two arrays of histograms (syntax see
    // TH1::Add(TH1*,TH1* ,Double _t fact1,Double _t fact2,TString opt))

    if(array1 && array2){
	Int_t n1 = array1->GetSize();  // full size, check for NULL in loop!
	Int_t n2 = array2->GetSize();  // full size, check for NULL in loop!

        if(n1 != n2) return kFALSE;

	for(Int_t i = 0 ;i < n1; i++) {
	    TH1* h1 = (TH1*)array1->At(i);
            TH1* h2 = (TH1*)array2->At(i);

	    if(h1 && h2) h1->Add(h1,h2,fact1,fact2);
	}
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::addArray(TObjArray* array0,TObjArray* array1,TObjArray* array2, Double_t fact1, Double_t fact2,TString opt)
{
    // add two arrays of histograms (syntax see
    // TH1::Add(TH1*,TH1* ,Double _t fact1,Double _t fact2,TString opt))
    // and store the reult in a new histogram (hist0)
    if(array0 && array1 && array2){
	Int_t n0 = array0->GetSize();  // full size, check for NULL in loop!
	Int_t n1 = array1->GetSize();  // full size, check for NULL in loop!
	Int_t n2 = array2->GetSize();  // full size, check for NULL in loop!

        if(n0 != n1 || n1 != n2) return kFALSE;

	for(Int_t i = 0 ;i < n1; i++) {
	    TH1* h0 = (TH1*)array0->At(i);
	    TH1* h1 = (TH1*)array1->At(i);
            TH1* h2 = (TH1*)array2->At(i);

	    if(h0 && h1 && h2) h0->Add(h1,h2,fact1,fact2);
	}
    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::functionArray(TString name    ,
			       Bool_t (*function)(TH1* ,TString name,HHistMap* map,TObjArray* params),
			       TObjArray* params)
{
    // execute function for each histogram of the array. TObjArray* params can be used
    // to pass parameters to the funcion

    TObjArray* ar = getHistArray(name);

    if(ar) {
	Int_t n = ar->GetSize();  // full size, check for NULL in loop!

	for(Int_t i = 0 ;i < n; i++) {
	    TH1* h = (TH1*)ar->At(i);
	    if(h) {
		function(h,name,this,params);
	    }
	}

    } else {
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HHistMap::functionArray(TObjArray* array,
			       Bool_t (*function)(TH1* ,TObjArray* array,TObjArray* params),
			       TObjArray* params)
{
    // execute function for each histogram of the array. TObjArray* params can be used
    // to pass parameters to the funcion

    if(array){
	Int_t n = array->GetSize();  // full size, check for NULL in loop!

	for(Int_t i = 0 ;i < n; i++) {
	    TH1* h = (TH1*)array->At(i);
	    if(h)  function(h,array,params);
	}

    } else {
      return kFALSE;
    }
    return kTRUE;
}
//--------------------------------------------------------------------




TObjArray* HHistMap::getHistArray(TString name)
{
    // get histogram array from map. print error and map
    // if not existing. the map has to be set before


    map<TString,TObjArray>::iterator iter = hM.find(name.Data());
    if( iter != hM.end() ) {
	return &(*iter).second ;
    }
    else {
	if(!fSilentFail) {
	    printMap();
	    Error("getHistArray()", "No matching histogram found for name = %s",name.Data());
	}
    }
    return 0;
}

TArrayI* HHistMap::getDimArray(TString name)
{
    // get histogram dim array from map. print error and map
    // if not existing. the map has to be set before


    map<TString,TArrayI>::iterator iter = hD.find(name.Data());
    if( iter != hD.end() ) {
	return &(*iter).second ;
    }
    else {
	if(!fSilentFail){
	    printMap();
	    Error("getDimArray()", "No matching histogram found for name = %s",name.Data());
	}
    }
    return 0;
}

TString HHistMap::getDir(TString name)
{
    // get histogram dir from map. print error and map
    // if not existing. the map has to be set before


    map<TString,TString>::iterator iter = hDir.find(name.Data());
    if( iter != hDir.end() ) {
	return (*iter).second ;
    }
    else {
	if(!fSilentFail) {
	    printMap();
	    Error("getDir()", "No matching histogram found for name = %s",name.Data());
	}
    }
    TString dummy="" ;
    return dummy;
}

TString HHistMap::getMask(TString name)
{
    // get histogram mask from map. print error and map
    // if not existing. the map has to be set before


    map<TString,TString>::iterator iter = hMask.find(name.Data());
    if( iter != hMask.end() ) {
	return (*iter).second ;
    }
    else {
	if(!fSilentFail){
	    printMap();
	    Error("getMask()", "No matching histogram found for name = %s",name.Data());
	}
    }

    TString dummy="" ;
    return dummy;
}

TString HHistMap::getTitle(TString name)
{
    // get histogram title from map. print error and map
    // if not existing. the map has to be set before


    map<TString,TString>::iterator iter = hTitle.find(name.Data());
    if( iter != hTitle.end() ) {
	return (*iter).second ;
    }
    else {
	if(!fSilentFail) {
	    printMap();
	    Error("getTitle()", "No matching histogram found for name = %s",name.Data());
	}
    }
    TString dummy="" ;
    return dummy;
}


TArrayI* HHistMap::getDimAddress(TString name,TH1*h)
{
    // return multi dim coordinates for histogram
    // user has to delete the TArrayI

    if(!h) return NULL;

    TObjArray* ar = getHistArray(name);

    if(ar ) {

	TArrayI* dim = getDimArray(name);
	Int_t max[5] = {0,0,0,0,0};

	for(Int_t j = 0 ;j < dim->GetSize(); j++){
	    max[j] = dim->At(j);
	}
	Int_t index = ar->IndexOf((TObject*)h);

	Int_t x[5] = {-1,-1,-1,-1,-1};

	HTool::getDimIndex(index,x[0],x[1],x[2],x[3],x[4],max[0],max[1],max[2],max[3],max[4]);

	return  new TArrayI(dim->GetSize(),&x[0]);

    } else {
	return NULL;
    }
}

TArrayI* HHistMap::getDimAddress(TString name,TString histname)
{
    // return multi dim coordinates for histogram
    // user has to delete the TArrayI

    TObjArray* ar = getHistArray(name);

    if(ar ) {

	TArrayI* dim = getDimArray(name);
	Int_t max[5] = {0,0,0,0,0};

	for(Int_t j = 0 ;j < dim->GetSize(); j++){
	    max[j] = dim->At(j);
	}

	TObject* h = ar->FindObject(name.Data());

	if(h) {
	    Int_t index = ar->IndexOf(h);

	    Int_t x[5] = {-1,-1,-1,-1,-1};

	    HTool::getDimIndex(index,x[0],x[1],x[2],x[3],x[4],max[0],max[1],max[2],max[3],max[4]);

	    return  new TArrayI(dim->GetSize(),&x[0]);
	} else {
	    return NULL;
	}

    } else {
	return NULL;
    }
}

Int_t    HHistMap::getIndex     (TString name,TH1*h)
{
    // return index in linear array for histogram
    // -1 if not found

    TObjArray* ar = getHistArray(name);

    if(ar ) {
	return ar->IndexOf((TObject*)h);
    } else {
	return -1;
    }
}

Int_t    HHistMap::getIndex     (TString name,TString histname)
{
    // return index in linear array for histogram
    // -1 if not found

    TObjArray* ar = getHistArray(name);

    if(ar ) {
        TObject* h = ar->FindObject(name.Data());
	return ar->IndexOf(h);
    } else {
	return -1;
    }
}


//------------------------------------------------------------------------
// canvas
Bool_t HHistMap::addCanvas(TCanvas* c ,
			   Int_t nx,Int_t ny,
			   Int_t xpix,Int_t ypix,
			   TString dir){

    // add a single canvas
    // nx,ny pads in x and y
    if(!c) {
	Error("addCanvas()","canvas pointer = NULL!");
	return kFALSE;
    }
    TString name = c->GetName();

    map<TString,TObjArray>::iterator iter = hMC.find(name);
    if( iter == hMC.end() ){ // new variable
	TObjArray a;
	a.Add(c);
	TArrayI dim(1);

	if ((nx >  1 && ny <= 0) ||
	    (nx < 1) || (ny < 0)
	   ){
	    Error("addCanvas()","canvas npads inconsistent : nx = %i, ny = %i for canvas %s! Will not make subdivisions."
		  ,nx,ny,name.Data());
	}

	TArrayI  np(4);
	np.SetAt(nx  ,0);
        np.SetAt(ny  ,1);
        np.SetAt(xpix,2);
        np.SetAt(ypix,3);

	dim.Reset(0);  // no array
	hMC    [name] = a;
	hMaskC [name] = name;
	hTitleC[name] = c->GetTitle();;
	hDC    [name] = dim;
	hNpC   [name] = np;
	hDirC  [name] = dir;

    } else { // exists already
	Error("addCanvas()","canvas with name \"%s\" has been used already!",name.Data());
	return kFALSE;
    }
    return kTRUE;
}


Bool_t HHistMap::addCanvas(TString name,TString title,
			   Int_t nx,Int_t ny,
			   Int_t xpix,Int_t ypix,
			   TString dir)
{
    // add an canvas.
    // name : canvas name
    // title : canvas title
    // xpix,ypix :x and y size of canvas in pixels (default 500)
    // dir : output dir

    TCanvas* c = new TCanvas(name,title,xpix,ypix);

    if(c == NULL) {
	return kFALSE;
    }

    map<TString,TObjArray>::iterator iter = hMC.find(name);
    if( iter == hMC.end() ){ // new variable
	TObjArray a;
	a.Add(c);
	TArrayI dim(1);
	dim.Reset(0);  // no array

	TArrayI  np(4);
	np.SetAt(nx  ,0);
        np.SetAt(ny  ,1);
        np.SetAt(xpix,2);
        np.SetAt(ypix,3);

        if       (nx >  1 && ny >  0) c->Divide(nx,ny);
	else if ((nx >  1 && ny <= 0) ||
                 (nx < 1) || (ny < 0)
		){
	    Error("addCanvas()","canvas npads inconsistent : nx = %i, ny = %i for canvas %s! Will not make subdivisions."
		  ,nx,ny,name.Data());
	}

	hMC    [name] = a;
	hMaskC [name] = name;
	hTitleC[name] = title;
	hDC    [name] = dim;
	hNpC   [name] = np;
	hDirC  [name] = dir;

    } else { // exists already
	Error("addCanvas()","canvas with name \"%s\" has been used already!",name.Data());
	delete c;
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HHistMap::removeCanvas(TString name)
{
    // remove a canvas/canvas array

    map<TString,TObjArray>::iterator iter = hMC.find(name);
    if( iter == hMC.end() ){ // new variable
	Error("removeCanvas()","canvas with name \"%s\" not found!",name.Data());
        return kFALSE;
    } else { // exists
        // canvas
	iter->second.Delete();
	hMC.erase( iter );
        // mask,title,dir
	map<TString,TString>::iterator iter2 = hMaskC.find(name);
	hMaskC.erase( iter2 );
        iter2 = hTitle.find(name);
	hTitleC.erase( iter2 );
	iter2 = hDirC.find(name);
	hDirC.erase( iter2 );
        // dim array
	map<TString,TArrayI>::iterator iter3 = hDC.find(name);
        Int_t dim = iter3->second.GetSize();
        TArrayI& ar = iter3->second;
        // dynamic pointers
	if(dim == 1){
            map<TString,TCanvas**>::iterator iter4 = hDim1C.find(name);
	    if(iter4 != hDim1C.end()) deleteDim1(iter4->second,ar.At(0),kFALSE);
	} else if(dim == 2){
            map<TString,TCanvas***>::iterator iter4 = hDim2C.find(name);
	    if(iter4 != hDim2C.end()) deleteDim2(iter4->second,ar.At(0),ar.At(1),kFALSE);
	} else if(dim == 3){
            map<TString,TCanvas****>::iterator iter4 = hDim3C.find(name);
	    if(iter4 != hDim3C.end()) deleteDim3(iter4->second,ar.At(0),ar.At(1),ar.At(2),kFALSE);
	} else if(dim == 4){
            map<TString,TCanvas*****>::iterator iter4 = hDim4C.find(name);
	    if(iter4 != hDim4C.end()) deleteDim4(iter4->second,ar.At(0),ar.At(1),ar.At(2),ar.At(3),kFALSE);
	} else if(dim == 5){
            map<TString,TCanvas******>::iterator iter4 = hDim5C.find(name);
	    if(iter4 != hDim5C.end()) deleteDim5(iter4->second,ar.At(0),ar.At(1),ar.At(2),ar.At(3),ar.At(4),kFALSE);
	}
	hDC.erase( iter3 );
    }
    return kTRUE;
}

Bool_t HHistMap::addCanvasArray(TCanvas* c[],TString name,TString mask,TString title,
				Int_t nx,Int_t ny,
				Int_t xpix,Int_t ypix,
				TString dir,
				Int_t i1max,Int_t i2max,Int_t i3max,Int_t i4max,Int_t i5max)
{
    // add an array of canvas. the size of the array and
    // a lable "name" to access the array later has to be specified
    // mask  : printf format string for canvas names (needed for config file)
    // title : printf format string for canvas titles (needed for config file)
    // dir   : dir in otput file
    // Only 1-dim arrays are supported. For more dimensional histogram
    // arrays (up to 5 indices) one should use linearized versions
    // using index made by HTool::getLinearIndex(....)
    // i1max to i5max give the size in the dimension
    // the histograms can later be retrieved by get("name",i1,i2....)


    if(!c[0]) {
	Error("addCanvasArray()","canvas pointer = NULL!");
	return kFALSE;
    }
    if(i1max < 0) {
	Error("addCanvasArray()","array size < 0!");
	return kFALSE;
    }
    if(name == "") {
	Error("addCanvasArray()","name is empty < 0!");
	return kFALSE;
    }

    Int_t dim = -1;

    if     (i1max >= 0 && i2max <  0 && i3max <  0 && i4max <  0 && i5max <  0  ) dim = 1;
    else if(i1max >= 0 && i2max >= 0 && i3max <  0 && i4max <  0 && i5max <  0  ) dim = 2;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max <  0 && i5max <  0  ) dim = 3;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max >= 0 && i5max <  0  ) dim = 4;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max >= 0 && i5max >= 0  ) dim = 5;

    if(dim<0) {
	Error("addCanvasArray()","problem with dimension for hist %s : ind %i %i %i %i %i !",name.Data(),i1max,i2max,i3max,i4max,i5max);
	return kFALSE;
    }

    Int_t size = 0;
    if     (dim == 1) size = i1max;
    else if(dim == 2) size = i1max*i2max;
    else if(dim == 3) size = i1max*i2max*i3max;
    else if(dim == 4) size = i1max*i2max*i3max*i4max;
    else if(dim == 5) size = i1max*i2max*i3max*i4max*i5max;

    map<TString,TObjArray>::iterator iter = hMC.find(name);
    if( iter == hMC.end() ){ // new variable
	TObjArray a(size);

	for(Int_t i=0;i<size;i++) {
	    if(!c[i]){
		Error("addCanvasArray()","canvas pointer = NULL for index = %i!",i);
	    }
	    a.Add(c[i]);
	}
	TArrayI d(dim);
        if(dim >= 1) d[0] = i1max;
        if(dim >= 2) d[1] = i2max;
        if(dim >= 3) d[2] = i3max;
        if(dim >= 4) d[3] = i4max;
        if(dim >= 5) d[4] = i5max;

	if ((nx >  1 && ny <= 0) ||
	    (nx < 1) || (ny < 0)
	   ){
	    Error("addCanvasArray()","canvas npads inconsistent : nx = %i, ny = %i for canvas %s! Will not make subdivisions."
		  ,nx,ny,name.Data());
	}

	TArrayI  np(4);
	np.SetAt(nx  ,0);
        np.SetAt(ny  ,1);
        np.SetAt(xpix,2);
        np.SetAt(ypix,3);

	hMC    [name] = a;
	hMaskC [name] = mask;
	hTitleC[name] = title;
	hDC    [name] = d;
	hNpC   [name] = np;
        hDirC  [name] = dir;

    } else { // exists already
	Error("addCanvasArray()","canvas with name \"%s\" has been used already!",name.Data());
	return kFALSE;
    }
    return kTRUE;
}
Bool_t HHistMap::addCanvasArray(TString name,TString mask,TString title,
                                Int_t nx,Int_t ny,
				Int_t xpix,Int_t ypix,
				TString dir,
				Int_t i1max,Int_t i2max,Int_t i3max,Int_t i4max,Int_t i5max,
				TString n1,TString n2,TString n3,TString n4,TString n5
			       )
{
    // add an array of canvas. the size of the array and
    // a lable "name" to access the array later has to be specified
    // name : lable for the canvas array
    // mask : canvas name format like printf for the give numbers of dimensions : "ctest[%i][%i]" (2dim array)
    // title : canvas title format like printf for the give numbers of dimensions : "ctest[%i][%i]" (2dim array)
    //         or same title for all canvas (if %i is not contained in format)
    // xpix,ypix : canvas size in pixels (default 500)
    // dir : output dir
    // i1max to i5max give the size in the dimension
    // the canvases can later be retrieved by getCanvas("name",i1,i2....)
    // -1 (default) mean not used
    // TString n1,n2,n3,n4,n5 (default empty) can contain a comma separated
    // list of labels for each dimension (example: i1max=4 ,"MDCI,MDCII,MDCIII,MDCIV" )
    // imax and number of labels have to be be equal. The lable
    // will replace the dimension index (example:  ctime1_%i => ctime1_0 to ctime1_3 will
    // become ctime1_MDCI to ctime1_MDCIV) in the name and title


    if(i1max < 0) {
	Error("addCanvasArray()","array size < 0!");
	return kFALSE;
    }
    if(name == "") {
	Error("addCanvasArray()","name is empty < 0!");
	return kFALSE;
    }

    Int_t dim = -1;

    if     (i1max >= 0 && i2max <  0 && i3max <  0 && i4max <  0 && i5max <  0  ) dim = 1;
    else if(i1max >= 0 && i2max >= 0 && i3max <  0 && i4max <  0 && i5max <  0  ) dim = 2;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max <  0 && i5max <  0  ) dim = 3;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max >= 0 && i5max <  0  ) dim = 4;
    else if(i1max >= 0 && i2max >= 0 && i3max >= 0 && i4max >= 0 && i5max >= 0  ) dim = 5;

    if(dim<0) {
	Error("addCanvasArray()","problem with dimension for canvas %s : ind %i %i %i %i %i !",name.Data(),i1max,i2max,i3max,i4max,i5max);
	return kFALSE;
    }

    Int_t size = 0;
    if     (dim == 1) size = i1max;
    else if(dim == 2) size = i1max*i2max;
    else if(dim == 3) size = i1max*i2max*i3max;
    else if(dim == 4) size = i1max*i2max*i3max*i4max;
    else if(dim == 5) size = i1max*i2max*i3max*i4max*i5max;

    Int_t is1,is2,is3,is4,is5;
    is1=is2=is3=is4=is5=0;

    if(n1!="" || n2!="" || n3!="" || n4!="" || n5!=""){
	// if the string option for one or more is used
        // we have to create dummy strings for the rest
	if(n1==""){
	    for(Int_t i = 0; i < i1max; i ++){
		if(i < i1max-1) n1 += Form("%i,",i);
                else            n1 += Form("%i" ,i);
	    }
	}
        if(n2=="" && dim > 1){
	    for(Int_t i = 0; i < i2max; i ++){
		if(i < i2max-1) n2 += Form("%i,",i);
                else            n2 += Form("%i" ,i);
	    }
	}
        if(n3=="" && dim > 2){
	    for(Int_t i = 0; i < i3max; i ++){
		if(i < i3max-1) n3 += Form("%i,",i);
                else            n3 += Form("%i" ,i);
	    }
	}
        if(n4=="" && dim > 3){
	    for(Int_t i = 0; i < i4max; i ++){
		if(i < i4max-1) n4 += Form("%i,",i);
                else            n4 += Form("%i" ,i);
	    }
	}
        if(n5=="" && dim > 4){
	    for(Int_t i = 0; i < i5max; i ++){
		if(i < i5max-1) n5 += Form("%i,",i);
                else            n5 += Form("%i" ,i);
	    }
	}
    }



    TString* ns1 = HTool::parseString(n1,is1,",",kFALSE);
    TString* ns2 = HTool::parseString(n2,is2,",",kFALSE);
    TString* ns3 = HTool::parseString(n3,is3,",",kFALSE);
    TString* ns4 = HTool::parseString(n4,is4,",",kFALSE);
    TString* ns5 = HTool::parseString(n5,is5,",",kFALSE);


    Bool_t useNameArray = kFALSE;

    if(n1 != "" && is1 != i1max)             Error("addCanvasArray()","Named index String =\"%s\" has not the same size as array index 1 = %i ! Named string will be ignored.",n1.Data(),i1max);
    if(dim > 1 && n2 != "" && is2 != i2max)  Error("addCanvasArray()","Named index String =\"%s\" has not the same size as array index 2 = %i ! Named string will be ignored.",n2.Data(),i2max);
    if(dim > 2 && n3 != "" && is3 != i3max)  Error("addCanvasArray()","Named index String =\"%s\" has not the same size as array index 3 = %i ! Named string will be ignored.",n3.Data(),i3max);
    if(dim > 3 && n4 != "" && is4 != i4max)  Error("addCanvasArray()","Named index String =\"%s\" has not the same size as array index 4 = %i ! Named string will be ignored.",n4.Data(),i4max);
    if(dim > 4 && n5 != "" && is5 != i5max)  Error("addCanvasArray()","Named index String =\"%s\" has not the same size as array index 5 = %i ! Named string will be ignored.",n5.Data(),i5max);

    if(dim == 1 && is1 != 0
       && is1 == i1max
      ) useNameArray = kTRUE;
    if(dim == 2 && is1 != 0 && is2 != 0
       && is1 == i1max
       && is2 == i2max
      ) useNameArray = kTRUE;
    if(dim == 3 && is1 != 0 && is2 != 0 && is3 != 0
       && is1 == i1max
       && is2 == i2max
       && is3 == i3max
      ) useNameArray = kTRUE;
    if(dim == 4 && is1 != 0 && is2 != 0 && is3 != 0 && is4 != 0
       && is1 == i1max
       && is2 == i2max
       && is3 == i3max
       && is4 == i4max
      ) useNameArray = kTRUE;
    if(dim == 5 && is1 != 0 && is2 != 0 && is3 != 0 && is4 != 0 && is5 != 0
       && is1 == i1max
       && is2 == i2max
       && is3 == i3max
       && is4 == i4max
       && is5 == i5max
      ) useNameArray = kTRUE;

    map<TString,TObjArray>::iterator iter = hMC.find(name);
    if( iter == hMC.end() ){ // new variable
	TObjArray a(size);

	TCanvas* c;
	TString current ;
	TString titlenew = title;
	Bool_t doTitle = kFALSE;
	if(title.Contains("%i")) doTitle = kTRUE;

	TString maskS = mask;
        maskS.ReplaceAll("%i","%s");
        TString titleS = title;
        titleS.ReplaceAll("%i","%s");

	for(Int_t i1=0; i1<i1max;i1++){
	    if(dim == 1) {

		if(useNameArray){
		    current = Form(maskS.Data(),ns1[i1].Data());
		    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data());
		} else {
		    current = Form(mask.Data(),i1);
		    if(doTitle) titlenew = Form(title.Data(),i1);
		}

		c = new TCanvas(current,titlenew,xpix,ypix);

		if       (nx >  1 && ny >  0) c->Divide(nx,ny);
		else if ((nx >  1 && ny <= 0) ||
			 (nx < 1) || (ny < 0)
			){
		    Error("addCanvasArray()","canvas npads inconsistent : nx = %i, ny = %i for canvas %s! Will not make subdivisions."
			  ,nx,ny,name.Data());
		}

		if(c == NULL){
		    Error("addCanvasArray()","canvas pointer = NULL for index = %i!",i1);
		    a.Delete();
		    return kFALSE;
		}
		a.Add(c);
	    } else {
		for(Int_t i2=0; i2<i2max;i2++){
		    if(dim == 2) {

			if(useNameArray){
			    current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data());
			    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data());
			} else {
			    current = Form(mask.Data(),i1,i2);
			    if(doTitle) titlenew = Form(title.Data(),i1,i2);
			}

			c = new TCanvas(current,titlenew,xpix,ypix);

			if       (nx >  1 && ny >  0) c->Divide(nx,ny);
			else if ((nx >  1 && ny <= 0) ||
				 (nx < 1) || (ny < 0)
				){
			    Error("addCanvasArray()","canvas npads inconsistent : nx = %i, ny = %i for canvas %s! Will not make subdivisions."
				  ,nx,ny,name.Data());
			}

			if(c == NULL){
			    Error("addCanvasArray()","canvas pointer = NULL for index = %i!",i2);
			    a.Delete();
			    return kFALSE;
			}
		       a.Add(c);

		    } else {
			for(Int_t i3=0; i3<i3max;i3++){
			    if(dim == 3) {

				if(useNameArray){
				    current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data());
				    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data());
				} else {
				    current = Form(mask.Data(),i1,i2,i3);
				    if(doTitle) titlenew = Form(title.Data(),i1,i2,i3);
				}

				c = new TCanvas(current,titlenew,xpix,ypix);

				if       (nx >  1 && ny >  0) c->Divide(nx,ny);
				else if ((nx >  1 && ny <= 0) ||
					 (nx < 1) || (ny < 0)
					){
				    Error("addCanvasArray()","canvas npads inconsistent : nx = %i, ny = %i for canvas %s! Will not make subdivisions."
					  ,nx,ny,name.Data());
				}

				if(c == NULL){
				    Error("addCanvasArray()","canvas pointer = NULL for index = %i!",i3);
				    a.Delete();
				    return kFALSE;
				}
				a.Add(c);

			    } else {
				for(Int_t i4=0; i4<i4max;i4++){
				    if(dim == 4) {

					if(useNameArray){
					    current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data());
					    if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data());
					} else {
					    current = Form(mask.Data(),i1,i2,i3,i4);
					    if(doTitle) titlenew = Form(title.Data(),i1,i2,i3,i4);
					}

                                        c = new TCanvas(current,titlenew,xpix,ypix);

					if       (nx >  1 && ny >  0) c->Divide(nx,ny);
					else if ((nx >  1 && ny <= 0) ||
						 (nx < 1) || (ny < 0)
						){
					    Error("addCanvasArray()","canvas npads inconsistent : nx = %i, ny = %i for canvas %s! Will not make subdivisions."
						  ,nx,ny,name.Data());
					}

					if(c == NULL){
					    Error("addCanvasArray()","canvas pointer = NULL for index = %i!",i4);
					    a.Delete();
					    return kFALSE;
					}
					a.Add(c);
				    } else {
					for(Int_t i5=0; i5<i5max;i5++){

					    if(useNameArray){
						current = Form(maskS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data(),ns5[i5].Data());
						if(doTitle) titlenew = Form(titleS.Data(),ns1[i1].Data(),ns2[i2].Data(),ns3[i3].Data(),ns4[i4].Data(),ns5[i5].Data());
					    } else {
						current = Form(mask.Data(),i1,i2,i3,i4,i5);
						if(doTitle) titlenew = Form(title.Data(),i1,i2,i3,i4,i5);
					    }

					    c = new TCanvas(current,titlenew,xpix,ypix);

					    if(c == NULL){
						Error("addCanvasArray()","canvas pointer = NULL for index = %i!",i5);
						a.Delete();
						return kFALSE;
					    }
					    a.Add(c);
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}

	TArrayI d(dim);
	if(dim >= 1) d[0] = i1max;
	if(dim >= 2) d[1] = i2max;
	if(dim >= 3) d[2] = i3max;
	if(dim >= 4) d[3] = i4max;
	if(dim >= 5) d[4] = i5max;

	TArrayI  np(4);
	np.SetAt(nx  ,0);
        np.SetAt(ny  ,1);
        np.SetAt(xpix,2);
        np.SetAt(ypix,3);

	hMC    [name] = a;
	hMaskC [name] = mask;
	hTitleC[name] = title;
	hDC    [name] = d;
	hNpC   [name] = np;
	hDirC  [name] = dir;

    } else { // exists already
	Error("addCanvasArray()","canvas with name \"%s\" has been used already!",name.Data());

	if(ns1) delete [] ns1;
	if(ns2) delete [] ns2;
	if(ns3) delete [] ns3;
	if(ns4) delete [] ns4;
	if(ns5) delete [] ns5;


	return kFALSE;
    }

    if(ns1) delete [] ns1;
    if(ns2) delete [] ns2;
    if(ns3) delete [] ns3;
    if(ns4) delete [] ns4;
    if(ns5) delete [] ns5;

    return kTRUE;

}

TCanvas*     HHistMap::getCanvas        (TString name,Int_t i1,Int_t i2,Int_t i3,Int_t i4,Int_t i5)
{
    // get canvas from map. print error and map
    // if not existing. the map has to be set before
    // If the requested object is an array of canvas
    // the index has to specified.


    map<TString,TObjArray>::iterator iter = hMC.find(name.Data());
    if( iter != hMC.end() ) {
	TArrayI& ar = hDC[(*iter).first];
	Int_t s = ar.GetSize();
	Int_t index = 0 ;
	if     (s == 1 && ar.At(0) > 0 ) index = i1;
	else if(s == 2 )                 index = HTool::getLinearIndex(i1,ar.At(0),i2,ar.At(1));
        else if(s == 3 )                 index = HTool::getLinearIndex(i1,ar.At(0),i2,ar.At(1),i3,ar.At(2));
        else if(s == 4 )                 index = HTool::getLinearIndex(i1,ar.At(0),i2,ar.At(1),i3,ar.At(2),i4,ar.At(3));
        else if(s == 5 )                 index = HTool::getLinearIndex(i1,ar.At(0),i2,ar.At(1),i3,ar.At(2),i4,ar.At(3),i5,ar.At(4));


	if(index<0) {
	    if(!fSilentFail) Error("getCanvas()","Retrieved NULL pointer for %s , index = ind %i %i %i %i %i !",name.Data(),i1,i2,i3,i4,i5);
            return 0;
	}
	return (TCanvas*)(*iter).second.At(index) ;
    }
    else {
	if(!fSilentFail) {
	    printCanvasMap();
	    Error("getCanvas()", "No matching canvas found for name = %s",name.Data());
	}
    }
    return 0;
}

TObjArray*   HHistMap::getCanvasArray   (TString name)
{
    // get canvas array from map. print error and map
    // if not existing. the map has to be set before

    map<TString,TObjArray>::iterator iter = hMC.find(name.Data());
    if( iter != hMC.end() ) {
	return &(*iter).second ;
    }
    else {
	if(!fSilentFail) {
	    printCanvasMap();
	    Error("getCanvasArray()", "No matching canvas found for name = %s",name.Data());
	}
    }
    return 0;
}

TArrayI*     HHistMap::getCanvasDimArray(TString name)
{
    // get canvas dim array from map. print error and map
    // if not existing. the map has to be set before


    map<TString,TArrayI>::iterator iter = hDC.find(name.Data());
    if( iter != hDC.end() ) {
	return &(*iter).second ;
    }
    else {
	if(!fSilentFail) {
	    printCanvasMap();
	    Error("getCanvasDimArray()", "No matching canvas found for name = %s",name.Data());
	}
    }
    return 0;
}

TString      HHistMap::getCanvasDir     (TString name)
{
    // get canvas dir from map. print error and map
    // if not existing. the map has to be set before


    map<TString,TString>::iterator iter = hDirC.find(name.Data());
    if( iter != hDirC.end() ) {
	return (*iter).second ;
    }
    else {
	if(!fSilentFail) {
	    printCanvasMap();
	    Error("getCanvasDir()", "No matching canvas found for name = %s",name.Data());
	}
    }
    TString dummy="" ;
    return dummy;
}

TString      HHistMap::getCanvasTitle   (TString name)
{
    // get canvas title from map. print error and map
    // if not existing. the map has to be set before


    map<TString,TString>::iterator iter = hTitleC.find(name.Data());
    if( iter != hTitleC.end() ) {
	return (*iter).second ;
    }
    else {
	if(!fSilentFail) {
	    printCanvasMap();
	    Error("getCanvasTitle()", "No matching canvas found for name = %s",name.Data());
	}
    }
    TString dummy="" ;
    return dummy;
}

TString      HHistMap::getCanvasMask    (TString name)
{
    // get canvas mask from map. print error and map
    // if not existing. the map has to be set before


    map<TString,TString>::iterator iter = hMaskC.find(name.Data());
    if( iter != hMaskC.end() ) {
	return (*iter).second ;
    }
    else {
	if(!fSilentFail) {
	    printCanvasMap();
	    Error("getCanvasMask()", "No matching canvas found for name = %s",name.Data());
	}
    }

    TString dummy="" ;
    return dummy;
}

TArrayI*     HHistMap::getCanvasPropertiesArray(TString name)
{
    // get canvas properties array from map. print error and map
    // if not existing. the map has to be set before


    map<TString,TArrayI>::iterator iter = hNpC.find(name.Data());
    if( iter != hNpC.end() ) {
	return &(*iter).second ;
    }
    else {
	if(!fSilentFail) {
	    printCanvasMap();
	    Error("getCanvasPropertiesArray()", "No matching canvas found for name = %s",name.Data());
	}
    }
    return 0;
}

vector < TString> HHistMap::getMatchingCanvasLabels(TString expression,Int_t dim)
{
    // return stl vector of label name matching the expression.
    // if dim  !=-1 the dimension of the array is evalated in addition.

    TPRegexp filter(expression);
    TString label ;
    vector <TString> out;
    for(map< TString, TObjArray>::iterator iter = hMC.begin(); iter != hMC.end(); ++iter ) {
         label =  (*iter).first;
	 if(filter.Match(label)){

	     Int_t dim1 = hDC[(*iter).first].GetSize();
             if(dim != -1 && dim1 != dim) continue;
             out.push_back(label);
	 }
    }
    return out;
}

vector < TCanvas*> HHistMap::getMatchingCanvas(TString expression,Int_t dim)
{
    // return stl vector of TCanvas*  matching the expression (by canvas name).
    // if dim  !=-1 the dimension of the array is evalated in addition.

    TPRegexp filter(expression);
    TString label ;
    vector <TCanvas*> out;
    for(map< TString, TObjArray>::iterator iter = hMC.begin(); iter != hMC.end(); ++iter ) {
	Int_t dim1 = hDC[(*iter).first].GetSize();
	if(dim != -1 && dim1 != dim)  continue;

	Int_t size = (*iter).second.GetEntries();
	for(Int_t i=0;i<size;i++){
            TCanvas* c = ((TCanvas*)((*iter).second.At(i)));
	    label  = c->GetName();
	    if(filter.Match(label)){
		out.push_back(c);
	    }
	}
    }
    return out;
}


//------------------------------------------------------------------------


//------------------------------------------------------------------------
// cuts

Bool_t HHistMap::addCut(TCutG* cut,TString dir)
{

    // add a TCutG

    if(!cut) {
	Error("addCut()","cut pointer = NULL!");
	return kFALSE;
    }

    TString name = cut->GetName();

    map<TString,TCutG*>::iterator iter = hMCut.find(name);
    if( iter == hMCut.end() ){ // new variable
	hMCut  [name] = cut;
        hDirCut[name] = dir;

    } else { // exists already
	Error("addCut()","cut with name \"%s\" has been used already!",name.Data());
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HHistMap::addCut(TString name,Int_t n,Double_t* x,Double_t* y,TString dir)
{
    // add a TCutG

    if(name == "") {
	Error("addCut()","cut name empty!");
	return kFALSE;
    }
    if(n < 3) {
	Error("addCut()","not enough points for cut!");
	return kFALSE;
    }
    if(x==NULL || y==NULL ) {
	Error("addCut()","array pointer of values is NULL !");
	return kFALSE;
    }

    map<TString,TCutG*>::iterator iter = hMCut.find(name);
    if( iter == hMCut.end() ){ // new variable

	hMCut  [name] = new TCutG(name.Data(),n,x,y);
        hDirCut[name] = dir;

    } else { // exists already
	Error("addCut()","cut with name \"%s\" has been used already!",name.Data());
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HHistMap::removeCut(TString name, Bool_t removeObject)
{
    // remove a cut

    map<TString,TCutG*>::iterator iter = hMCut.find(name);
    if( iter == hMCut.end() ){ // new variable
	Error("removeCut()","cut with name \"%s\" not found!",name.Data());
        return kFALSE;
    } else { // exists
        // cut
	if(removeObject)delete iter->second;
	hMCut.erase( iter );
        // mask,title,dir
	map<TString,TString>::iterator iter2 = hDirCut.find(name);
	hDirCut.erase( iter2 );
    }
    return kTRUE;
}

TCutG* HHistMap::getCut(TString name)
{
    // get TCutG by name

    map<TString,TCutG*>::iterator iter = hMCut.find(name);
    if( iter == hMCut.end() ){ // new variable
	if(!fSilentFail) {
	    Error("getCut()","cut with name \"%s\" not found!",name.Data());
	    printCutMap();
	}
    } else { // exists already
	return iter->second;
    }
    return NULL;
}

TString HHistMap::getCutDir(TString name)
{
    // get TCutG by name

    map<TString,TCutG*>::iterator iter = hMCut.find(name);
    if( iter == hMCut.end() ){ // new variable
	if(!fSilentFail){
	    Error("getCutDir()","cut with name \"%s\" not found!",name.Data());
	    printCutMap();
	}
    } else { // exists already
	return hDirCut[name];
    }
    TString dummy = "";
    return dummy;
}

Bool_t HHistMap::isInsideCut(TString name,Double_t x, Double_t y)
{
    // returns kTRUE if point is inside the cut

    TCutG* cut = getCut(name);
    if(cut){
	return cut->IsInside(x,y);
    }
    return kFALSE;
}

vector < TString> HHistMap::getMatchingCutLabels(TString expression)
{
    // return stl vector of label name matching the expression.

    TPRegexp filter(expression);
    TString label ;
    vector <TString> out;
    for(map< TString, TCutG*>::iterator iter = hMCut.begin(); iter != hMCut.end(); ++iter ) {
         label =  (*iter).first;
	 if(filter.Match(label)){
             out.push_back(label);
	 }
    }
    return out;
}

vector < TCutG*> HHistMap::getMatchingCuts(TString expression)
{
    // return stl vector of TCutG*  matching the expression.

    TPRegexp filter(expression);
    TString label ;
    vector <TCutG*> out;
    for(map< TString, TCutG*>::iterator iter = hMCut.begin(); iter != hMCut.end(); ++iter ) {
	label  = iter->first;
	if(filter.Match(label)){
	    out.push_back(iter->second);
	}
    }
    return out;
}


//------------------------------------------------------------------------

//------------------------------------------------------------------------
// fits

Bool_t HHistMap::addFit(TF1* fit,TString dir)
{

    // add a TF1

    if(!fit) {
	Error("addFit()","fit pointer = NULL!");
	return kFALSE;
    }

    TString name = fit->GetName();

    map<TString,TF1*>::iterator iter = hMFit.find(name);
    if( iter == hMFit.end() ){ // new variable
	hMFit  [name] = fit;
        hDirFit[name] = dir;

    } else { // exists already
	Error("addFit()","fit with name \"%s\" has been used already!",name.Data());
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HHistMap::removeFit(TString name, Bool_t removeObject)
{
    // remove a fit

    map<TString,TF1*>::iterator iter = hMFit.find(name);
    if( iter == hMFit.end() ){ // new variable
	Error("removeFit()","fit with name \"%s\" not found!",name.Data());
        return kFALSE;
    } else { // exists
        // fit
	if(removeObject)delete iter->second;
	hMFit.erase( iter );
        // mask,title,dir
	map<TString,TString>::iterator iter2 = hDirFit.find(name);
	hDirFit.erase( iter2 );
    }
    return kTRUE;
}

TF1* HHistMap::getFit(TString name)
{
    // get TF1 by name

    map<TString,TF1*>::iterator iter = hMFit.find(name);
    if( iter == hMFit.end() ){ // new variable

	Error("getFit()","fit with name \"%s\" not found!",name.Data());
        printFitMap();
    } else { // exists already
	return iter->second;
    }
    return NULL;
}

TString HHistMap::getFitDir(TString name)
{
    // get Fit dir by name

    map<TString,TF1*>::iterator iter = hMFit.find(name);
    if( iter == hMFit.end() ){ // new variable
	if(!fSilentFail){
	    Error("getFitDir()","fit with name \"%s\" not found!",name.Data());
	    printFitMap();
	}
    } else { // exists already
	return hDirFit[name];
    }
    TString dummy = "";
    return dummy;
}

vector < TString> HHistMap::getMatchingFitLabels(TString expression)
{
    // return stl vector of label name matching the expression.

    TPRegexp filter(expression);
    TString label ;
    vector <TString> out;
    for(map< TString, TF1*>::iterator iter = hMFit.begin(); iter != hMFit.end(); ++iter ) {
         label =  (*iter).first;
	 if(filter.Match(label)){
             out.push_back(label);
	 }
    }
    return out;
}

vector < TF1*> HHistMap::getMatchingFits(TString expression)
{
    // return stl vector of TF1*  matching the expression.

    TPRegexp filter(expression);
    TString label ;
    vector <TF1*> out;
    for(map< TString, TF1*>::iterator iter = hMFit.begin(); iter != hMFit.end(); ++iter ) {
	label  = iter->first;
	if(filter.Match(label)){
	    out.push_back(iter->second);
	}
    }
    return out;
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// parameters
Bool_t HHistMap::addPar(TObject* par,TString name, TString dir)
{

    // add a Parmeter object

    if(!par) {
	Error("addPar()","par pointer = NULL!");
	return kFALSE;
    }

    map<TString,TObject*>::iterator iter = hMPar.find(name);
    if( iter == hMPar.end() ){ // new variable

 	hMPar  [name] = par;
        hDirPar[name] = dir;

    } else { // exists already
	Error("addPar()","par with name \"%s\" has been used already!",name.Data());
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HHistMap::removePar(TString name, Bool_t removeObject)
{
    // remove a parameter

    map<TString,TObject*>::iterator iter = hMPar.find(name);
    if( iter == hMPar.end() ){ // new variable
	Error("removePar()","par with name \"%s\" not found!",name.Data());
        return kFALSE;
    } else { // exists
        // par
	if(removeObject) delete iter->second;
	hMPar.erase( iter );
        // mask,title,dir
	map<TString,TString>::iterator iter2 = hDirPar.find(name);
	hDirPar.erase( iter2 );
    }
    return kTRUE;
}

TObject* HHistMap::getPar(TString name)
{
    // get Parameter by name

    map<TString,TObject*>::iterator iter = hMPar.find(name);
    if( iter == hMPar.end() ){ // new variable
	if(!fSilentFail) {
	    Error("getPar()","par with name \"%s\" not found!",name.Data());
	    printParameterMap();
	}
    } else { // exists already
	return iter->second;
    }
    return NULL;
}

TString HHistMap::getParDir(TString name)
{
    // get TCutG by name

    map<TString,TObject*>::iterator iter = hMPar.find(name);
    if( iter == hMPar.end() ){ // new variable
	if(!fSilentFail){
	    Error("getParDir()","par with name \"%s\" not found!",name.Data());
	    printParameterMap();
	}
    } else { // exists already
	return hDirPar[name];
    }
    TString dummy = "";
    return dummy;
}

vector < TString> HHistMap::getMatchingParLabels(TString expression)
{
    // return stl vector of label name matching the expression.

    TPRegexp filter(expression);
    TString label ;
    vector <TString> out;
    for(map< TString, TObject*>::iterator iter = hMPar.begin(); iter != hMPar.end(); ++iter ) {
         label =  (*iter).first;
	 if(filter.Match(label)){
             out.push_back(label);
	 }
    }
    return out;
}

vector < TObject*> HHistMap::getMatchingPars(TString expression)
{
    // return stl vector of TObject*  matching the expression.

    TPRegexp filter(expression);
    TString label ;
    vector <TObject*> out;
    for(map< TString, TObject*>::iterator iter = hMPar.begin(); iter != hMPar.end(); ++iter ) {
	label  = iter->first;
	if(filter.Match(label)){
	    out.push_back(iter->second);
	}
    }
    return out;
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// macros

Bool_t HHistMap::addMacro(TMacro* mac,TString name,TString dir)
{

    // add a TCutG

    if(!mac) {
	Error("addMacro()","macro pointer = NULL!");
	return kFALSE;
    }

    TString name1 = mac->GetName();
    if(name != "") name1 = name;
    map<TString,TMacro*>::iterator iter = hMMacro.find(name1);
    if( iter == hMMacro.end() ){ // new variable
	hMMacro  [name1] = mac;
        hDirMacro[name1] = dir;

    } else { // exists already
	Error("addMacro()","macro with name \"%s\" has been used already!",name.Data());
	return kFALSE;
    }
    return kTRUE;
}


Bool_t HHistMap::removeMacro(TString name, Bool_t removeObject)
{
    // remove a cut

    map<TString,TMacro*>::iterator iter = hMMacro.find(name);
    if( iter == hMMacro.end() ){ // new variable
	Error("removeMacro()","mac with name \"%s\" not found!",name.Data());
        return kFALSE;
    } else { // exists
        // cut
	if(removeObject)delete iter->second;
	hMMacro.erase( iter );
        // mask,title,dir
	map<TString,TString>::iterator iter2 = hDirMacro.find(name);
	hDirMacro.erase( iter2 );
    }
    return kTRUE;
}

TMacro* HHistMap::getMacro(TString name)
{
    // get TMacro by name

    map<TString,TMacro*>::iterator iter = hMMacro.find(name);
    if( iter == hMMacro.end() ){ // new variable
	if(!fSilentFail){
	    Error("getMacro()","macro with name \"%s\" not found!",name.Data());
	    printMacroMap();
	}
    } else { // exists already
	return iter->second;
    }
    return NULL;
}

TString HHistMap::getMacroDir(TString name)
{
    // get TMacro dir by name

    map<TString,TMacro*>::iterator iter = hMMacro.find(name);
    if( iter == hMMacro.end() ){ // new variable
	if(!fSilentFail){
	    Error("getMacroDir()","macro with name \"%s\" not found!",name.Data());
	    printMacroMap();
	}
    } else { // exists already
	return hDirMacro[name];
    }
    TString dummy = "";
    return dummy;
}

vector < TString> HHistMap::getMatchingMacroLabels(TString expression)
{
    // return stl vector of label name matching the expression.

    TPRegexp filter(expression);
    TString label ;
    vector <TString> out;
    for(map< TString, TMacro*>::iterator iter = hMMacro.begin(); iter != hMMacro.end(); ++iter ) {
         label =  (*iter).first;
	 if(filter.Match(label)){
             out.push_back(label);
	 }
    }
    return out;
}

vector < TMacro*> HHistMap::getMatchingMacros(TString expression)
{
    // return stl vector of TMacros*  matching the expression.

    TPRegexp filter(expression);
    TString label ;
    vector <TMacro*> out;
    for(map< TString, TMacro*>::iterator iter = hMMacro.begin(); iter != hMMacro.end(); ++iter ) {
	label  = iter->first;
	if(filter.Match(label)){
	    out.push_back(iter->second);
	}
    }
    return out;
}

//------------------------------------------------------------------------


Bool_t HHistMap::writeHists(TString opt)
{
    // write he histograms and canvas to the output file
    // the histograms will be stored in subdirs as
    // specified in addHist(....) or in the main
    // directory if no dir is specified
    // opt = "nohists" will write only the HHistMap object
    // opt = "nomap"   will write only the hists into subbdirs
    // opt = ""  default will write both

    opt.ToLower();



    if(output) output->cd();
    else      {
	Error("writeHists()","No file opened! will be ignored...");
        return kFALSE;
    }

    if(opt.CompareTo("nohists")!=0)
    {

	for(map< TString, TObjArray>::iterator iter = hM.begin(); iter != hM.end(); ++iter ) {
	    output->cd();
	    TString dir = hDir[(*iter).first];
	    if(hDir[(*iter).first] != "") HTool::changeToDir(dir);
	    (*iter).second.Write();
	}

	for(map< TString, TObjArray>::iterator iter = hMC.begin(); iter != hMC.end(); ++iter ) {
	    output->cd();
	    TString dir = hDirC[(*iter).first];
	    if(hDirC[(*iter).first] != "") HTool::changeToDir(dir);
	    (*iter).second.Write();
	}

	for(map< TString, TCutG*>::iterator iter = hMCut.begin(); iter != hMCut.end(); ++iter ) {
	    output->cd();
	    TString dir = hDirCut[(*iter).first];
	    if(hDirCut[(*iter).first] != "") HTool::changeToDir(dir);
	    TCutG* cut = (*iter).second ;
	    if(strlen(cut->GetName())==0){
		cut->SetName((*iter).first.Data());
	    }
	    (*iter).second->Write();
	}

	for(map< TString, TF1*>::iterator iter = hMFit.begin(); iter != hMFit.end(); ++iter ) {
	    output->cd();
	    TString dir = hDirFit[(*iter).first];
	    if(hDirFit[(*iter).first] != "") HTool::changeToDir(dir);
	    TF1* fit = (*iter).second ;
	    if(strlen(fit->GetName()) == 0){
		fit->SetName((*iter).first.Data());
	    }
	    (*iter).second->Write();
	}

	for(map< TString, TObject*>::iterator iter = hMPar.begin(); iter != hMPar.end(); ++iter ) {
	    output->cd();
	    TString dir = hDirPar[(*iter).first];
	    if(hDirPar[(*iter).first] != "") HTool::changeToDir(dir);

	    if((*iter).second->InheritsFrom("TNamed") ){

		TNamed* par = (TNamed*)(*iter).second;
		TString name = par->GetName();
		if(name == ""){
		    ((TNamed*)(*iter).second)->SetName((*iter).first.Data());
		}
		par->Write();
	    } else (*iter).second->Write();
	}

	for(map< TString, TMacro*>::iterator iter = hMMacro.begin(); iter != hMMacro.end(); ++iter ) {
	    output->cd();
	    TString dir = hDirMacro[(*iter).first];
	    if(hDirMacro[(*iter).first] != "") HTool::changeToDir(dir);
	    TMacro* cut = (*iter).second ;
	    if(strlen(cut->GetName())==0){
		cut->SetName((*iter).first.Data());
	    }
	    (*iter).second->Write();
	}
    }
    output->cd();

    if(opt.CompareTo("nomap")!=0)
    {
	TString name = GetName();
	if(name == "") SetName("HHistMap");
	this ->Write();
    }
    output->Save();
    output->Close();

    return kTRUE;
}
/*
Bool_t HHistMap::writeConfig(TString filename)
{
    // write the histograms config to the output file
    // this file can be read in to create all hists from
    // config file. " " is used as token delimiter,
    // @ separates token from asigned values. " " inside
    // axis titles or title will be replaced by "!".
    // The readConfig() routine takes care to change back.
    // empty titles will be writte as "no" and changed
    // back accordingly in readConfig(). Special graphical
    // options are not supported. parameters and fits
    // are not supported too.


    ofstream output;
    output.open(filename.Data());

    if(output.good())
    {

	//---------------------------------------------------------------------------
        // hist
	for(map< TString, TObjArray>::iterator iter = hM.begin(); iter != hM.end(); ++iter )
	{
	    TString out    = "";
	    TString lable  = (*iter).first;
            TString mask   = hMask [(*iter).first];
            TString title  = hTitle[(*iter).first];
            TString dir    = hDir  [(*iter).first];
            Int_t dim      = getDimArray(lable)->GetSize();
	    TString type   = "";
	    TString xtitle = "no";
            TString ytitle = "no";
	    TString ztitle = "no";

	    Int_t i1max,i2max,i3max,i4max,i5max;
            i1max=i2max=i3max=i4max=i5max=-1;
            Int_t nbinx,nbiny,nbinz;
	    Double_t x1,x2,y1,y2,z1,z2;
            nbinx=nbiny=nbinz=0;
            x1=x2=y1=y2=z1=z2=0.;


	    if  (dim == 1) {
                i1max  = (*getDimArray(lable))[0];
		type   = get(lable,0)->ClassName();
                nbinx  = get(lable,0)->GetNbinsX();
		xtitle = get(lable,0)->GetXaxis()->GetTitle();
                x1     = get(lable,0)->GetXaxis()->GetXmin();
                x2     = get(lable,0)->GetXaxis()->GetXmax();

		if(type.Contains("2")) {
		    nbiny  = get(lable,0)->GetNbinsY();
		    ytitle = get(lable,0)->GetYaxis()->GetTitle();
		    ztitle = get(lable,0)->GetZaxis()->GetTitle();
		    y1     = get(lable,0)->GetYaxis()->GetXmin();
		    y2     = get(lable,0)->GetYaxis()->GetXmax();
 		}
		if(type.Contains("3")) {
		    nbiny  = get(lable,0)->GetNbinsY();
		    nbinz  = get(lable,0)->GetNbinsZ();
		    ytitle = get(lable,0)->GetYaxis()->GetTitle();
		    ztitle = get(lable,0)->GetZaxis()->GetTitle();
		    y1     = get(lable,0)->GetYaxis()->GetXmin();
		    y2     = get(lable,0)->GetYaxis()->GetXmax();
		    z1     = get(lable,0)->GetZaxis()->GetXmin();
		    z2     = get(lable,0)->GetZaxis()->GetXmax();
		}

	    } else if(dim == 2) {
		i1max = (*getDimArray(lable))[0];
		i2max = (*getDimArray(lable))[1];
		type   = get(lable,0,0)->ClassName();
                nbinx  = get(lable,0,0)->GetNbinsX();
		xtitle = get(lable,0,0)->GetXaxis()->GetTitle();
                x1     = get(lable,0,0)->GetXaxis()->GetXmin();
                x2     = get(lable,0,0)->GetXaxis()->GetXmax();
		if(type.Contains("2")){
		    nbiny  = get(lable,0,0)->GetNbinsY();
		    ytitle = get(lable,0,0)->GetYaxis()->GetTitle();
		    ztitle = get(lable,0,0)->GetZaxis()->GetTitle();
		    y1     = get(lable,0,0)->GetYaxis()->GetXmin();
		    y2     = get(lable,0,0)->GetYaxis()->GetXmax();
		}
		if(type.Contains("3")){
		    nbiny  = get(lable,0,0)->GetNbinsY();
		    nbinz  = get(lable,0,0)->GetNbinsZ();
		    ytitle = get(lable,0,0)->GetYaxis()->GetTitle();
		    ztitle = get(lable,0,0)->GetZaxis()->GetTitle();
		    y1     = get(lable,0,0)->GetYaxis()->GetXmin();
		    y2     = get(lable,0,0)->GetYaxis()->GetXmax();
		    z1     = get(lable,0,0)->GetZaxis()->GetXmin();
		    z2     = get(lable,0,0)->GetZaxis()->GetXmax();
		}
	    } else if(dim == 3) {
		i1max  = (*getDimArray(lable))[0];
		i2max  = (*getDimArray(lable))[1];
		i3max  = (*getDimArray(lable))[2];
		type   = get(lable,0,0,0)->ClassName();
                nbinx  = get(lable,0,0,0)->GetNbinsX();
		xtitle = get(lable,0,0,0)->GetXaxis()->GetTitle();
                x1     = get(lable,0,0,0)->GetXaxis()->GetXmin();
                x2     = get(lable,0,0,0)->GetXaxis()->GetXmax();
		if(type.Contains("2")){
		    nbiny  = get(lable,0,0,0)->GetNbinsY();
		    ytitle = get(lable,0,0,0)->GetYaxis()->GetTitle();
		    ztitle = get(lable,0,0,0)->GetZaxis()->GetTitle();
		    y1     = get(lable,0,0,0)->GetYaxis()->GetXmin();
		    y2     = get(lable,0,0,0)->GetYaxis()->GetXmax();
		}
		if(type.Contains("3")){
		    nbiny  = get(lable,0,0,0)->GetNbinsY();
		    nbinz  = get(lable,0,0,0)->GetNbinsZ();
		    ytitle = get(lable,0,0,0)->GetYaxis()->GetTitle();
		    ztitle = get(lable,0,0,0)->GetZaxis()->GetTitle();
		    y1     = get(lable,0,0,0)->GetYaxis()->GetXmin();
		    y2     = get(lable,0,0,0)->GetYaxis()->GetXmax();
		    z1     = get(lable,0,0,0)->GetZaxis()->GetXmin();
		    z2     = get(lable,0,0,0)->GetZaxis()->GetXmax();
		}
	    } else if(dim == 4) {
		i1max  = (*getDimArray(lable))[0];
		i2max  = (*getDimArray(lable))[1];
		i3max  = (*getDimArray(lable))[2];
		i4max  = (*getDimArray(lable))[3];
		type   = get(lable,0,0,0,0)->ClassName();
                nbinx  = get(lable,0,0,0,0)->GetNbinsX();
		xtitle = get(lable,0,0,0,0)->GetXaxis()->GetTitle();
                x1     = get(lable,0,0,0,0)->GetXaxis()->GetXmin();
                x2     = get(lable,0,0,0,0)->GetXaxis()->GetXmax();
		if(type.Contains("2")){
		    nbiny  = get(lable,0,0,0,0)->GetNbinsY();
		    ytitle = get(lable,0,0,0,0)->GetYaxis()->GetTitle();
		    ztitle = get(lable,0,0,0,0)->GetZaxis()->GetTitle();
		    y1     = get(lable,0,0,0,0)->GetYaxis()->GetXmin();
		    y2     = get(lable,0,0,0,0)->GetYaxis()->GetXmax();
		}
		if(type.Contains("3")){
		    nbiny  = get(lable,0,0,0,0)->GetNbinsY();
		    nbinz  = get(lable,0,0,0,0)->GetNbinsZ();
		    ytitle = get(lable,0,0,0,0)->GetYaxis()->GetTitle();
		    ztitle = get(lable,0,0,0,0)->GetZaxis()->GetTitle();
		    y1     = get(lable,0,0,0,0)->GetYaxis()->GetXmin();
		    y2     = get(lable,0,0,0,0)->GetYaxis()->GetXmax();
		    z1     = get(lable,0,0,0,0)->GetZaxis()->GetXmin();
		    z2     = get(lable,0,0,0,0)->GetZaxis()->GetXmax();
		}
	    } else if(dim == 5) {
		i1max  = (*getDimArray(lable))[0];
		i2max  = (*getDimArray(lable))[1];
		i3max  = (*getDimArray(lable))[2];
		i4max  = (*getDimArray(lable))[3];
		i5max  = (*getDimArray(lable))[4];
		type   = get(lable,0,0,0,0,0)->ClassName();
                nbinx  = get(lable,0,0,0,0,0)->GetNbinsX();
		xtitle = get(lable,0,0,0,0,0)->GetXaxis()->GetTitle();
                x1     = get(lable,0,0,0,0,0)->GetXaxis()->GetXmin();
                x2     = get(lable,0,0,0,0,0)->GetXaxis()->GetXmax();
		if(type.Contains("2")){
		    nbiny  = get(lable,0,0,0,0,0)->GetNbinsY();
		    ytitle = get(lable,0,0,0,0,0)->GetYaxis()->GetTitle();
		    ztitle = get(lable,0,0,0,0,0)->GetZaxis()->GetTitle();
		    y1     = get(lable,0,0,0,0,0)->GetYaxis()->GetXmin();
		    y2     = get(lable,0,0,0,0,0)->GetYaxis()->GetXmax();
		}
		if(type.Contains("3")){
		    nbiny  = get(lable,0,0,0,0,0)->GetNbinsY();
		    nbinz  = get(lable,0,0,0,0,0)->GetNbinsZ();
		    ytitle = get(lable,0,0,0,0,0)->GetYaxis()->GetTitle();
		    ztitle = get(lable,0,0,0,0,0)->GetZaxis()->GetTitle();
		    y1     = get(lable,0,0,0,0,0)->GetYaxis()->GetXmin();
		    y2     = get(lable,0,0,0,0,0)->GetYaxis()->GetXmax();
		    z1     = get(lable,0,0,0,0,0)->GetZaxis()->GetXmin();
		    z2     = get(lable,0,0,0,0,0)->GetZaxis()->GetXmax();
		}
	    }

	    if(xtitle == "") xtitle = "no";
            if(ytitle == "") ytitle = "no";
            if(ztitle == "") ztitle = "no";
            if(title  == "")  title = "no";
	    xtitle.ReplaceAll(" ","!");
            ytitle.ReplaceAll(" ","!");
            ztitle.ReplaceAll(" ","!");
            title.ReplaceAll(" ","!");

	    out += Form("type@%s dim@%i",type.Data(),dim);
	    out += Form(" maxind@%i:%i:%i:%i:%i",i1max,i2max,i3max,i4max,i5max);
	    out += Form(" lable@%s name@%s title@%s dir@%s",lable.Data(),mask.Data(),title.Data(),dir.Data());
	    out += Form(" x@%i:%f:%f",nbinx,x1,x2);
	    out += Form(" y@%i:%f:%f",nbiny,y1,y2);
	    out += Form(" z@%i:%f:%f",nbinz,z1,z2);
	    out += Form(" axistitle@%s:%s:%s",xtitle.Data(),ytitle.Data(),ztitle.Data());

           
            output<<out.Data()<<endl;
	}


	//---------------------------------------------------------------------------
        // canvas

	for(map< TString, TObjArray>::iterator iter = hMC.begin(); iter != hMC.end(); ++iter )
	{
	    TString out    = "";
	    TString lable  = (*iter).first;
            TString mask   = hMaskC [(*iter).first];
            TString title  = hTitleC[(*iter).first];
            TString dir    = hDirC  [(*iter).first];
            Int_t   dim    = getCanvasDimArray(lable)->GetSize();
	    TString type   = "TCanvas";

	    Int_t i1max,i2max,i3max,i4max,i5max;
            i1max=i2max=i3max=i4max=i5max=-1;
            Int_t nx,ny,xpix,ypix;
            nx=ny=xpix=ypix=0;

	    nx   = getCanvasPropertiesArray(lable)->At(0);
	    ny   = getCanvasPropertiesArray(lable)->At(1);
	    xpix = getCanvasPropertiesArray(lable)->At(2);
	    ypix = getCanvasPropertiesArray(lable)->At(3);

	    if  (dim == 1) {
                i1max = (*getCanvasDimArray(lable))[0];

	    } else if(dim == 2) {
		i1max = (*getCanvasDimArray(lable))[0];
		i2max = (*getCanvasDimArray(lable))[1];
	    } else if(dim == 3) {
		i1max = (*getCanvasDimArray(lable))[0];
		i2max = (*getCanvasDimArray(lable))[1];
		i3max = (*getCanvasDimArray(lable))[2];
	    } else if(dim == 4) {
		i1max = (*getCanvasDimArray(lable))[0];
		i2max = (*getCanvasDimArray(lable))[1];
		i3max = (*getCanvasDimArray(lable))[2];
		i4max = (*getCanvasDimArray(lable))[3];
	    } else if(dim == 5) {
		i1max = (*getCanvasDimArray(lable))[0];
		i2max = (*getCanvasDimArray(lable))[1];
		i3max = (*getCanvasDimArray(lable))[2];
		i4max = (*getCanvasDimArray(lable))[3];
		i5max = (*getCanvasDimArray(lable))[4];
	    }

	    title.ReplaceAll(" ","!");


	    out += Form("type@%s dim@%i",type.Data(),dim);
	    out += Form(" maxind@%i:%i:%i:%i:%i",i1max,i2max,i3max,i4max,i5max);
	    out += Form(" lable@%s name@%s title@%s dir@%s",lable.Data(),mask.Data(),title.Data(),dir.Data());
	    out += Form(" pads@%i:%i",nx,ny);
	    out += Form(" window@%i:%i",xpix,ypix);


            output<<out.Data()<<endl;
	}


	//---------------------------------------------------------------------------
        // cuts

	for(map< TString, TCutG*>::iterator iter = hMCut.begin(); iter != hMCut.end(); ++iter )
	{
	    TString out    = "";
	    TString lable  = (*iter).first;
            TString dir    = hDirCut  [(*iter).first];
	    TString type   = "TCutG";
	    TCutG* cut = (*iter).second ;

	    out += Form("type@%s name@%s dir@%s ",type.Data(),lable.Data(),dir.Data());
	    output<<out.Data();

	    Double_t* x = cut->GetX();
            Double_t* y = cut->GetY();

	    output<<"xcut@";
	    for(Int_t i = 0; i < cut->GetN(); i++ ){
		output<<x[i];
		if(i < cut->GetN() - 1) output<<",";
	    }
            output<<" ycut@";
	    for(Int_t i = 0; i < cut->GetN(); i++ ){
		output<<y[i];
		if(i < cut->GetN() - 1) output<<",";
	    }

	    output<<endl;
	}


    } else {
         Error("writeConfig()","Could not create config file!");
         return kFALSE;
    }
    output.close();
    return kTRUE;
}

Bool_t HHistMap::readConfig(TString filename)
{
    // read the histograms config from the input file
    // the format is defined by writeConfig() with 1 line
    // per histogram definition
    // info's about the file format see in writeConfig()


    ifstream input;
    input.open(filename.Data());


    Char_t line[1000];
    if(input.good())
    {
	while(input.good())
	{
	    input.getline(line,1000);

	    if(!input.fail())
	    {
		TString lable;
		TString mask;
		TString title;
		TString dir;
		Int_t dim=-1;
		TString type;


		Int_t i1max,i2max,i3max,i4max,i5max;
		i1max=i2max=i3max=i4max=i5max=-1;

		// hists
		TString xtitle;
		TString ytitle;
		TString ztitle;

		Int_t nbinx,nbiny,nbinz;
		Double_t x1,x2,y1,y2,z1,z2;
		nbinx=nbiny=nbinz=0;
		x1=x2=y1=y2=z1=z2=0.;

                // canvas
		Int_t nx,ny,xpix,ypix;
                nx=ny=xpix=ypix=0;

		// cuts
                Double_t* xcut = NULL;
                Double_t* ycut = NULL;

		TString l = line;
		TObjArray* ar= l.Tokenize(" ");

		for(Int_t i=0;i<ar->GetEntries();i++){
		    TString token = ((TObjString*)ar->At(i))->GetString();
		    if(token != " " ){
			if(token.BeginsWith("type@")){
			    token.ReplaceAll("type@","");
			    type = token;
			    continue;
			} else if (token.BeginsWith("dim@")){
			    token.ReplaceAll("dim@","");
			    sscanf(token.Data(),"%d",&dim);
			    continue;
			} else if (token.BeginsWith("maxind@")){
			    token.ReplaceAll("maxind@","");
			    token.ReplaceAll(":"," ");
			    sscanf(token.Data(),"%d %d %d %d %d",&i1max,&i2max,&i3max,&i4max,&i5max);
			    continue;
			} else if (token.BeginsWith("lable@")){
			    token.ReplaceAll("lable@","");
			    lable = token;
			    continue;
			} else if (token.BeginsWith("name@")){
			    token.ReplaceAll("name@","");
			    mask = token;
			    continue;
			} else if (token.BeginsWith("title@")){
			    token.ReplaceAll("title@","");
			    title = token;
			    continue;
			} else if (token.BeginsWith("dir@")){
			    token.ReplaceAll("dir@","");
			    dir = token;
			    continue;
			} else if (token.BeginsWith("x@")){
			    token.ReplaceAll("x@","");
			    token.ReplaceAll(":"," ");
			    sscanf(token.Data(),"%d %f %f",&nbinx,&x1,&x2);
			    continue;
			} else if (token.BeginsWith("y@")){
			    token.ReplaceAll("y@","");
			    token.ReplaceAll(":"," ");
			    sscanf(token.Data(),"%d %f %f",&nbiny,&y1,&y2);
			    continue;
			} else if (token.BeginsWith("z@")){
			    token.ReplaceAll("z@","");
			    token.ReplaceAll(":"," ");
			    sscanf(token.Data(),"%d %f %f",&nbinz,&z1,&z2);
			    continue;
			} else if (token.BeginsWith("axistitle@")){
			    token.ReplaceAll("axistitle@","");
			    token.ReplaceAll(":"," ");

			    Char_t xt[1000],yt[1000],zt[1000];
			    sscanf(token.Data(),"%s %s %s",xt,yt,zt);
			    xtitle = xt;
			    ytitle = yt;
			    ztitle = zt;
			    continue;
			} else if (token.BeginsWith("pads@")){ // canvas
			    token.ReplaceAll("pads@","");
			    token.ReplaceAll(":"," ");
			    sscanf(token.Data(),"%d %d ",&nx,&ny);
			    continue;
			} else if (token.BeginsWith("window@")){ // canvas
			    token.ReplaceAll("window@","");
			    token.ReplaceAll(":"," ");
			    sscanf(token.Data(),"%d %d ",&xpix,&ypix);
			    continue;
			} else if (token.BeginsWith("xcut@")){ // cut xvals
			    token.ReplaceAll("xcut@","");

			    TObjArray* values = token.Tokenize(",");
			    TIterator* myiter = values->MakeIterator();
                            TObjString* stemp;
			    if(values && values->GetEntries() >0){
				xcut = new Double_t [values->GetEntries()];
				// iterate over the list of arguments
                                Int_t ct = 0;
				while ((stemp=(TObjString*)myiter->Next())!= 0) {
				    TString valstr=stemp->GetString();
                                    sscanf(valstr.Data(),"%lf",&xcut[ct]);
                                    ct++;
				}
			    }
			    values->Delete();
			    delete values;
			    delete myiter;


			    continue;
			} else if (token.BeginsWith("ycut@")){ // cut yvals
			    token.ReplaceAll("ycut@","");

			    TObjArray* values = token.Tokenize(",");
			    TIterator* myiter = values->MakeIterator();
                            TObjString* stemp;
			    if(values && values->GetEntries() >0){
				ycut = new Double_t [values->GetEntries()];
				// iterate over the list of arguments
                                Int_t ct = 0;
				while ((stemp=(TObjString*)myiter->Next())!= 0) {
				    TString valstr=stemp->GetString();
                                    sscanf(valstr.Data(),"%lf",&ycut[ct]);
                                    ct++;
				}
			    }
			    values->Delete();
			    delete values;
			    delete myiter;


			    continue;
			} else {
			    Error("readConfig()","Unknown token = %s!",token.Data());
			}
		    }
		}

		if(xtitle == "no") xtitle="";
		if(ytitle == "no") ytitle="";
		if(ztitle == "no") ztitle="";
		if(title  == "no") title ="";
		xtitle.ReplaceAll("!"," ");
		ytitle.ReplaceAll("!"," ");
		ztitle.ReplaceAll("!"," ");
		title.ReplaceAll("!"," ");

		if(type == "TCanvas"){
		    if(i1max > 0) {

			addCanvasArray(lable,mask,title,nx,ny,xpix,ypix,dir,
				       i1max,i2max,i3max,i4max,i5max);
		    } else {
			addCanvas(mask,title,nx,ny,xpix,ypix,dir);
		    }

		} else {
		    if(i1max > 0) {
			addHistArray(type,lable,mask,title,
				     nbinx,x1,x2,
				     nbiny,y1,y2,
				     nbinz,z1,z2,
				     xtitle,ytitle,ztitle,dir,
				     i1max,i2max,i3max,i4max,i5max);
		    } else {
			addHist(type,mask,title,
				nbinx,x1,x2,
				nbiny,y1,y2,
				nbinz,z1,z2,
				xtitle,ytitle,ztitle,dir);
		    }
		}

		if(type == "TCutG"){
		    if(xcut && ycut){
                        addCut(lable.Data(),sizeof(xcut),xcut,ycut,dir);

		    } else {
			Error("readConfig()","x- or y-valus of cut = %s could not be read correctly!",lable.Data());
		    }
                    if(xcut) delete [] xcut;
		    if(ycut) delete [] ycut;
                    xcut = NULL;
                    ycut = NULL;

		}
		ar->Delete();
		delete ar;
	    }
	}
    } else {
	Error("readConfig()","Could not create config file!");
	return kFALSE;
    }
    input.close();
    return kTRUE;
}
*/


