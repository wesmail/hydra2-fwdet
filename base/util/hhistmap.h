//*-- Author : Jochen Markert 18.07.2007

#ifndef  __HHISTMAP_H__
#define  __HHISTMAP_H__

#include "htool.h"

#include "TString.h"
#include "TObjArray.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "TCutG.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TObject.h"
#include "TArrayI.h"
#include "TFile.h"
#include "TMacro.h"

#include <map>
#include <iostream>
#include <iomanip>

using namespace std;

class HHistMap;

R__EXTERN HHistMap *gHHistMap;

class HHistMap : public TNamed {

private:

    map < TString , TString    > hDir;   // directories per histogram
    map < TString , TObjArray  > hM;     // TObjArray for histogram pointers
    map < TString , TString    > hMask;  // mask string for names
    map < TString , TString    > hTitle; // mask string for titles
    map < TString , TArrayI    > hD;     // Array for dimensions
    map < TString , TH1**      > hDim1;  //! dynamically created Array of hist pointers dimension = 1
    map < TString , TH1***     > hDim2;  //! dynamically created Array of hist pointers dimension = 2
    map < TString , TH1****    > hDim3;  //! dynamically created Array of hist pointers dimension = 3
    map < TString , TH1*****   > hDim4;  //! dynamically created Array of hist pointers dimension = 4
    map < TString , TH1******  > hDim5;  //! dynamically created Array of hist pointers dimension = 5

    map < TString , TString    >  hDirC;   // directories per canvas
    map < TString , TObjArray  >  hMC;     // TObjArray for canvas pointers
    map < TString , TString    >  hMaskC;  // mask string for names canvas
    map < TString , TString    >  hTitleC; // mask string for titles canvas
    map < TString , TArrayI    >  hDC;     // Array for dimensions canvas
    map < TString , TArrayI    >  hNpC;    // canvas properties : number of pads x,y xpix,ypix canvas
    map < TString , TCanvas**      > hDim1C;  //! dynamically created Array of canvas pointers dimension = 1
    map < TString , TCanvas***     > hDim2C;  //! dynamically created Array of canvas pointers dimension = 2
    map < TString , TCanvas****    > hDim3C;  //! dynamically created Array of canvas pointers dimension = 3
    map < TString , TCanvas*****   > hDim4C;  //! dynamically created Array of canvas pointers dimension = 4
    map < TString , TCanvas******  > hDim5C;  //! dynamically created Array of canvas pointers dimension = 5


    map < TString , TString    >  hDirCut;  // directories per TCutG
    map < TString , TCutG*     >  hMCut;    // TCutG pointers

    map < TString , TString    >  hDirFit;  // directories per TF1
    map < TString , TF1*       >  hMFit;    // TF1 pointers

    map < TString , TString    >  hDirPar;  //  map of parameter objects  (not streamed to root file)
    map < TString , TObject*   >  hMPar;    //  map of parameter objects  (not streamed to root file)

    map < TString , TString    >  hDirMacro;  //  map of macro objects
    map < TString , TMacro*   >  hMMacro;    //  map of macro objects



    TFile* output;                         //! store hists later
    Bool_t  fDeleteObjects;                //! delete hists+canvas etc in destructor
    Bool_t  fSilentFail;                   //! switch off error+warnings in get() etc (own risk, default is kFALSE)

    TString getLabels(const TString name,vector<TString>&labels);


public:
    HHistMap(TString outputname="") ;
    ~HHistMap();
    void   setDeleteObjects(Bool_t del) { fDeleteObjects = del;}
    Bool_t getDeleteObjects()           { return fDeleteObjects;}

    void setCurrentMap() { gHHistMap=this;}
    void setSilentFail(Bool_t doit)             {fSilentFail = doit;}


    const map < TString , TObjArray >* getMap() { return &hM;   }
    TFile*                             getFile(){ return output;}
    void   print();
    void   printMap();
    void   printCanvasMap();
    void   printCutMap();
    void   printFitMap();
    void   printParameterMap();
    void   printMacroMap();



    void   addHistContent(HHistMap& hM2);
    Bool_t addFile     (TString infile);
    Bool_t addFiles    (TString expression);
    Bool_t addFilesList(TString filelist);
    Bool_t addMultFiles(TString commaSeparatedList);

    static Bool_t addHistMap(TString input="",TString output="");
    //------------------------------------------------------------------------
    // hists
    Bool_t addHist(TH1* h ,TString dir="");
    Bool_t addHist(TString Type = "",TString name="",TString title="",
		   Int_t nbinx=0,Double_t x1=0,Double_t x2=0,
		   Int_t nbiny=0,Double_t y1=0,Double_t y2=0,
		   Int_t nbinz=0,Double_t z1=0,Double_t z2=0,
		   TString xtitle="",TString ytitle="",TString ztitle="",
		   TString dir="");
    Bool_t addHistN(TString Type = "",TString name="",TString title="",
		   Int_t nbinx=0,Double_t* x=0,
		   Int_t nbiny=0,Double_t* y=0,
		   Int_t nbinz=0,Double_t* z=0,
		   TString xtitle="",TString ytitle="",TString ztitle="",
		   TString dir="");
    Bool_t removeHist(TString name);

    TH1* createHist(TString Type="",TString name="",TString title="",
		    Int_t nbinx=0,Double_t x1=0.,Double_t x2=0.,
		    Int_t nbiny=0,Double_t y1=0.,Double_t y2=0.,
		    Int_t nbinz=0,Double_t z1=0.,Double_t z2=0.,
		    TString xtitle="",TString ytitle="",TString ztitle="");
    TH1* createHistN(TString Type="",TString name="",TString title="",
		    Int_t nbinx=0,Double_t* x=0,
		    Int_t nbiny=0,Double_t* y=0,
		    Int_t nbinz=0,Double_t* z=0,
		    TString xtitle="",TString ytitle="",TString ztitle="");

    Bool_t addHistArray(TH1* h[],TString name,TString mask="",TString title="",TString dir="",Int_t i1max=-1,Int_t i2max=-1,Int_t i3max=-1,Int_t i4max=-1,Int_t i5max=-1);
    Bool_t addHistArray(TString Type="",TString name="",TString mask="",TString title="",
			Int_t nbinx=0,Double_t x1=0.,Double_t x2=0.,
			Int_t nbiny=0,Double_t y1=0.,Double_t y2=0.,
			Int_t nbinz=0,Double_t z1=0.,Double_t z2=0.,
			TString xtitle="",TString ytitle="",TString ztitle="",
			TString dir="",
			Int_t i1max=-1,Int_t i2max=-1,Int_t i3max=-1,Int_t i4max=-1,Int_t i5max=-1,
                        TString n1="",TString n2="",TString n3="",TString n4="",TString n5=""
		       );

    Bool_t addHistArrayN(TString Type="",TString name="",TString mask="",TString title="",
			 Int_t nbinx=0,Double_t* x=0,
			 Int_t nbiny=0,Double_t* y=0,
			 Int_t nbinz=0,Double_t* z=0,
			 TString xtitle="",TString ytitle="",TString ztitle="",
			 TString dir="",
			 Int_t i1max=-1,Int_t i2max=-1,Int_t i3max=-1,Int_t i4max=-1,Int_t i5max=-1,
			 TString n1="",TString n2="",TString n3="",TString n4="",TString n5=""
			);

    TH1*         get   (TString name,Int_t i1=0,Int_t i2=-1,Int_t i3=-1,Int_t i4=-1,Int_t i5=-1);
    TH2*         get2  (TString name,Int_t i1=0,Int_t i2=-1,Int_t i3=-1,Int_t i4=-1,Int_t i5=-1);
    TH3*         get3  (TString name,Int_t i1=0,Int_t i2=-1,Int_t i3=-1,Int_t i4=-1,Int_t i5=-1);
    TProfile*    getP  (TString name,Int_t i1=0,Int_t i2=-1,Int_t i3=-1,Int_t i4=-1,Int_t i5=-1);
    TProfile2D*  get2P (TString name,Int_t i1=0,Int_t i2=-1,Int_t i3=-1,Int_t i4=-1,Int_t i5=-1);
    TProfile3D*  get3P (TString name,Int_t i1=0,Int_t i2=-1,Int_t i3=-1,Int_t i4=-1,Int_t i5=-1);

    vector<TString> getMatchingHistLabels(TString expression,TString type="",Int_t dim=-1);
    vector<TH1*   > getMatchingHists     (TString expression,TString type="",Int_t dim=-1);

    void resetAllHists(TString opt="ICES");
    void disconnectAllHists();

    //------------------------------------------------------------------------
    // array Operations
    Bool_t        cloneArray (TString source,TString name="",TString mask="",TString title="",TString dir="",Bool_t reset=kFALSE);
    Bool_t        resetArray (TString name    , TString opt="");
    static Bool_t resetArray (TObjArray* array, TString opt="");
    Bool_t        scaleArray (TString name    , Double_t fact);
    static Bool_t scaleArray (TObjArray* array, Double_t fact);
    Bool_t        sumW2Array (TString name    );
    static Bool_t sumW2Array (TObjArray* array);
    Bool_t        divideArray  (TString name1    ,TString name2    , Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    static Bool_t divideArray  (TObjArray* array1,TObjArray* array2, Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    Bool_t        divideArray  (TString name0    ,TString name1    ,TString name2    , Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    static Bool_t divideArray  (TObjArray* array0,TObjArray* array1,TObjArray* array2, Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    Bool_t        multiplyArray(TString name1    ,TString name2    , Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    static Bool_t multiplyArray(TObjArray* array1,TObjArray* array2, Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    Bool_t        multiplyArray(TString name0    ,TString name1    ,TString name2    , Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    static Bool_t multiplyArray(TObjArray* array0,TObjArray* array1,TObjArray* array2, Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    Bool_t        addArray     (TString name1    ,TString name2    , Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    static Bool_t addArray     (TObjArray* array1,TObjArray* array2, Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    Bool_t        addArray     (TString name0    ,TString name1    ,TString name2    , Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    static Bool_t addArray     (TObjArray* array0,TObjArray* array1,TObjArray* array2, Double_t fact1= 1., Double_t fact2=1.,TString opt="");
    Bool_t        functionArray(TString name    , Bool_t (*function)(TH1* ,TString name,HHistMap* map, TObjArray* params),TObjArray* params);
    static Bool_t functionArray(TObjArray* array, Bool_t (*function)(TH1* ,TObjArray* array,TObjArray* params),TObjArray* params);
    //------------------------------------------------------------------------




    TObjArray*   getHistArray (TString name);
    TArrayI*     getDimArray  (TString name);
    TArrayI*     getDimAddress(TString name,TH1*h);
    TArrayI*     getDimAddress(TString name,TString histname);
    Int_t        getIndex     (TString name,TH1*h);
    Int_t        getIndex     (TString name,TString histname);
    TString      getDir       (TString name);
    TString      getTitle     (TString name);
    TString      getMask      (TString name);


    //------------------------------------------------------------------------
    // canvas
    Bool_t addCanvas(TCanvas* h ,
		     Int_t nx=1,Int_t ny=0,
		     Int_t xpix=500,Int_t ypix=500,TString dir="");
    Bool_t addCanvas(TString name="",TString title="",
		     Int_t nx=1,Int_t ny=0,
		     Int_t xpix=500,Int_t ypix=500,
		     TString dir="");
    Bool_t removeCanvas(TString name);

    Bool_t addCanvasArray(TCanvas* c[],TString name,TString mask="",TString title="",
			  Int_t nx=1,Int_t ny=0,
			  Int_t xpix=500,Int_t ypix=500,
			  TString dir="",
			  Int_t i1max=-1,Int_t i2max=-1,Int_t i3max=-1,Int_t i4max=-1,Int_t i5max=-1);
    Bool_t addCanvasArray(TString name="",TString mask="",TString title="",
                          Int_t nx=1,Int_t ny=0,
			  Int_t xpix=500,Int_t ypix=500,TString dir="",
			  Int_t i1max=-1,Int_t i2max=-1,Int_t i3max=-1,Int_t i4max=-1,Int_t i5max=-1,
			  TString n1="",TString n2="",TString n3="",TString n4="",TString n5=""
			 );

    TCanvas*     getCanvas        (TString name,Int_t i1=0,Int_t i2=-1,Int_t i3=-1,Int_t i4=-1,Int_t i5=-1);
    TObjArray*   getCanvasArray   (TString name);
    TArrayI*     getCanvasDimArray(TString name);
    TString      getCanvasDir     (TString name);
    TString      getCanvasTitle   (TString name);
    TString      getCanvasMask    (TString name);
    TArrayI*     getCanvasPropertiesArray(TString name);

    vector<TString>   getMatchingCanvasLabels(TString expression,Int_t dim=-1);
    vector<TCanvas* > getMatchingCanvas      (TString expression,Int_t dim=-1);

    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // cuts
    Bool_t  addCut     (TString name,Int_t n=0,Double_t* x=NULL,Double_t* y=NULL,TString dir = "");
    Bool_t  addCut     (TCutG* cut,TString dir = "");
    Bool_t  removeCut  (TString name,Bool_t removeObject=kTRUE);
    TCutG*  getCut     (TString name);
    TString getCutDir  (TString name);
    Bool_t  isInsideCut(TString name,Double_t x, Double_t y);

    vector<TString>   getMatchingCutLabels(TString expression);
    vector<TCutG* >   getMatchingCuts     (TString expression);

    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // fits
    Bool_t  addFit     (TF1* cut,TString dir = "");
    Bool_t  removeFit  (TString name,Bool_t removeObject=kTRUE);
    TF1*    getFit     (TString name);
    TString getFitDir  (TString name);
    vector<TString>   getMatchingFitLabels(TString expression);
    vector<TF1* >     getMatchingFits     (TString expression);

    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // params
    Bool_t    addPar     (TObject* par,TString name,TString dir="");
    Bool_t    removePar  (TString name,Bool_t removeObject=kTRUE);
    TObject*  getPar     (TString name);
    TString   getParDir  (TString name);
    vector<TString>   getMatchingParLabels(TString expression);
    vector<TObject* > getMatchingPars     (TString expression);
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // macros
    Bool_t    addMacro   (TMacro* par,TString name,TString dir="");
    Bool_t    removeMacro(TString name,Bool_t removeObject=kTRUE);
    TMacro*   getMacro   (TString name);
    TString   getMacroDir(TString name);
    vector<TString>   getMatchingMacroLabels(TString expression);
    vector<TMacro* >  getMatchingMacros     (TString expression);
    //------------------------------------------------------------------------


    Bool_t setOutputFile(TString name="");
    Bool_t writeHists(TString opt="");
    //Bool_t writeConfig(TString name="histmap_config.txt");
    //Bool_t readConfig (TString name="histmap_config.txt");


    //------------------------------------------------------------------------
    // hists
    template<class T> T**     getDim1(T**   h,TString name){
	//  use :  T** h = hM->getDim1(h, "name")
	//  Do not delete the created array of pointers when retrieving
	//  them for hist arrays managed by HHistMap, they will
        //  be deleted in the destructor!
	h = NULL;
	map<TString,TObjArray>::iterator iter = hM.find(name.Data());
	if( iter != hM.end() ) {
	    TArrayI& ar = hD[(*iter).first];
	    Int_t s = ar.GetSize();
	    if(s != 1) {
		Error("getDim1()","dimension of array does not match!");
                return h;
	    }

	    map<TString,TH1**>::iterator iter1 = hDim1.find(name.Data());
	    if (iter1 == hDim1.end()){
		h = newDim1(h,ar.At(0));
                hDim1[name] = (TH1**) h;
	    } else {
		h = (T**) iter1->second;
	    }

	    TObjArray& a = iter->second;
	    for(Int_t i1 = 0; i1 < ar.At(0); i1 ++) {
                   h[i1] = (T*)a.At(i1);
	    }
	}
	return h;
    }

    template<class T> T***    getDim2(T***   h,TString name){
	//  use :  T*** h = hM->getDim2(h, "name")
	//  Do not delete the created array of pointers when retrieving
	//  them for hist arrays managed by HHistMap, they will
        //  be deleted in the destructor!

	h = NULL;
	map<TString,TObjArray>::iterator iter = hM.find(name.Data());
	if( iter != hM.end() ) {
	    TArrayI& ar = hD[(*iter).first];
	    Int_t s = ar.GetSize();
	    if(s != 2) {
		Error("getDim2()","dimension of array does not match!");
                return h;
	    }

	    map<TString,TH1***>::iterator iter1 = hDim2.find(name.Data());
	    if (iter1 == hDim2.end()){
		h = newDim2(h,ar.At(0),ar.At(1));
                hDim2[name] = (TH1***) h;
	    } else {
		h = (T***) iter1->second;
	    }

	    TObjArray& a = iter->second;
            Int_t ind = 0;
	    for(Int_t i1 = 0; i1 < ar.At(0); i1 ++) {
		for(Int_t i2 = 0; i2 < ar.At(1); i2 ++) {
		    h[i1][i2] = (T*)a.At(ind);
                    ind++;
		}
	    }
	}
	return h;
    }

    template<class T> T****   getDim3(T****  h,TString name){
	//  use :  T**** h = hM->getDim3(h, "name")
	//  Do not delete the created array of pointers when retrieving
	//  them for hist arrays managed by HHistMap, they will
        //  be deleted in the destructor!

	h = NULL;
	map<TString,TObjArray>::iterator iter = hM.find(name.Data());
	if( iter != hM.end() ) {
	    TArrayI& ar = hD[(*iter).first];
	    Int_t s = ar.GetSize();
	    if(s != 3) {
		Error("getDim3()","dimension of array does not match!");
		return h;
	    }

	    map<TString,TH1****>::iterator iter1 = hDim3.find(name.Data());
	    if (iter1 == hDim3.end()){
		h = newDim3(h,ar.At(0),ar.At(1),ar.At(2));
                hDim3[name] = (TH1****) h;
	    } else {
		h = (T****) iter1->second;
	    }

	    TObjArray& a = iter->second;
	    Int_t ind = 0;
	    for(Int_t i1 = 0; i1 < ar.At(0); i1 ++) {
		for(Int_t i2 = 0; i2 < ar.At(1); i2 ++) {
		    for(Int_t i3 = 0; i3 < ar.At(2); i3 ++) {
			h[i1][i2][i3] = (T*)a.At(ind);
			ind++;
		    }
		}
	    }
	}
	return h;
    }

    template<class T> T*****  getDim4(T***** h,TString name){
	//  use :  T***** h = hM->getDim4(h, "name")
	//  Do not delete the created array of pointers when retrieving
	//  them for hist arrays managed by HHistMap, they will
        //  be deleted in the destructor!
	h = NULL;
	map<TString,TObjArray>::iterator iter = hM.find(name.Data());
	if( iter != hM.end() ) {
	    TArrayI& ar = hD[(*iter).first];
	    Int_t s = ar.GetSize();
	    if(s != 4) {
		Error("getDim4()","dimension of array does not match!");
		return h;
	    }

	    map<TString,TH1*****>::iterator iter1 = hDim4.find(name.Data());
	    if (iter1 == hDim4.end()){
		h = newDim4(h,ar.At(0),ar.At(1),ar.At(2),ar.At(3));
                hDim4[name] = (TH1*****) h;
	    } else {
		h = (T*****) iter1->second;
	    }

	    TObjArray& a = iter->second;
	    Int_t ind = 0;
	    for(Int_t i1 = 0; i1 < ar.At(0); i1 ++) {
		for(Int_t i2 = 0; i2 < ar.At(1); i2 ++) {
		    for(Int_t i3 = 0; i3 < ar.At(2); i3 ++) {
			for(Int_t i4 = 0; i4 < ar.At(3); i4 ++) {
			    h[i1][i2][i3][i4] = (T*)a.At(ind);
			    ind++;
			}
		    }
		}
	    }
	}
	return h;
    }

    template<class T> T****** getDim5(T******h,TString name){
	//  use :  T****** h = hM->getDim5(h, "name")
	//  Do not delete the created array of pointers when retrieving
	//  them for hist arrays managed by HHistMap, they will
        //  be deleted in the destructor!
	h = NULL;
	map<TString,TObjArray>::iterator iter = hM.find(name.Data());
	if( iter != hM.end() ) {
	    TArrayI& ar = hD[(*iter).first];
	    Int_t s = ar.GetSize();
	    if(s != 5) {
		Error("getDim5()","dimension of array does not match!");
		return h;
	    }

	    map<TString,TH1******>::iterator iter1 = hDim5.find(name.Data());
	    if (iter1 == hDim5.end()){
		h = newDim5(h,ar.At(0),ar.At(1),ar.At(2),ar.At(3),ar.At(4));
                hDim5[name] = (TH1******) h;
	    } else {
		h = (T******) iter1->second;
	    }

	    TObjArray& a = iter->second;
	    Int_t ind = 0;
	    for(Int_t i1 = 0; i1 < ar.At(0); i1 ++) {
		for(Int_t i2 = 0; i2 < ar.At(1); i2 ++) {
		    for(Int_t i3 = 0; i3 < ar.At(2); i3 ++) {
			for(Int_t i4 = 0; i4 < ar.At(3); i4 ++) {
			    for(Int_t i5 = 0; i5 < ar.At(4); i5 ++) {
			    h[i1][i2][i3][i4][i5] = (T*)a.At(ind);
			    ind++;
			    }
			}
		    }
		}
	    }
	}
	return h;
    }

    //------------------------------------------------------------------------
    // canvas
    template<class T> T**     getCanvasDim1(T**   c,TString name){
	//  use :  T** c = hM->getCanvasDim1(c, "name")
	//  Do not delete the created array of pointers when retrieving
	//  them for canvas arrays managed by HHistMap, they will
        //  be deleted in the destructor!
	c = NULL;
	map<TString,TObjArray>::iterator iter = hMC.find(name.Data());
	if( iter != hMC.end() ) {
	    TArrayI& ar = hDC[(*iter).first];
	    Int_t s = ar.GetSize();
	    if(s != 1) {
		Error("getCanvasDim1()","dimension of array does not match!");
                return c;
	    }

	    map<TString,TCanvas**>::iterator iter1 = hDim1C.find(name.Data());
	    if (iter1 == hDim1C.end()){
		c = newDim1(c,ar.At(0));
                hDim1C[name] = (TCanvas**) c;
	    } else {
		c = (T**) iter1->second;
	    }

	    TObjArray& a = iter->second;
	    for(Int_t i1 = 0; i1 < ar.At(0); i1 ++) {
                   c[i1] = (T*)a.At(i1);
	    }
	}
	return c;
    }

    template<class T> T***    getCanvasDim2(T***   c,TString name){
	//  use :  T*** c = hM->getCanvasDim2(c, "name")
	//  Do not delete the created array of pointers when retrieving
	//  them for canvas arrays managed by HHistMap, they will
        //  be deleted in the destructor!

	c = NULL;
	map<TString,TObjArray>::iterator iter = hMC.find(name.Data());
	if( iter != hMC.end() ) {
	    TArrayI& ar = hDC[(*iter).first];
	    Int_t s = ar.GetSize();
	    if(s != 2) {
		Error("getCanvasDim2()","dimension of array does not match!");
                return c;
	    }

	    map<TString,TCanvas***>::iterator iter1 = hDim2C.find(name.Data());
	    if (iter1 == hDim2C.end()){
		c = newDim2(c,ar.At(0),ar.At(1));
                hDim2C[name] = (TCanvas***) c;
	    } else {
		c = (T***) iter1->second;
	    }

	    TObjArray& a = iter->second;
            Int_t ind = 0;
	    for(Int_t i1 = 0; i1 < ar.At(0); i1 ++) {
		for(Int_t i2 = 0; i2 < ar.At(1); i2 ++) {
		    c[i1][i2] = (T*)a.At(ind);
                    ind++;
		}
	    }
	}
	return c;
    }

    template<class T> T****   getCanvasDim3(T****  c,TString name){
	//  use :  T**** c = hM->getCanvasDim3(c, "name")
	//  Do not delete the created array of pointers when retrieving
	//  them for canvas arrays managed by HHistMap, they will
        //  be deleted in the destructor!

	c = NULL;
	map<TString,TObjArray>::iterator iter = hMC.find(name.Data());
	if( iter != hMC.end() ) {
	    TArrayI& ar = hDC[(*iter).first];
	    Int_t s = ar.GetSize();
	    if(s != 3) {
		Error("getCanvasDim3()","dimension of array does not match!");
		return c;
	    }

	    map<TString,TCanvas****>::iterator iter1 = hDim3C.find(name.Data());
	    if (iter1 == hDim3C.end()){
		c = newDim3(c,ar.At(0),ar.At(1),ar.At(2));
                hDim3C[name] = (TCanvas****) c;
	    } else {
		c = (T****) iter1->second;
	    }

	    TObjArray& a = iter->second;
	    Int_t ind = 0;
	    for(Int_t i1 = 0; i1 < ar.At(0); i1 ++) {
		for(Int_t i2 = 0; i2 < ar.At(1); i2 ++) {
		    for(Int_t i3 = 0; i3 < ar.At(2); i3 ++) {
			c[i1][i2][i3] = (T*)a.At(ind);
			ind++;
		    }
		}
	    }
	}
	return c;
    }

    template<class T> T*****  getCanvasDim4(T***** c,TString name){
	//  use :  T***** c = hM->getCanvasDim4(c, "name")
	//  Do not delete the created array of pointers when retrieving
	//  them for canvas arrays managed by HHistMap, they will
        //  be deleted in the destructor!
	c = NULL;
	map<TString,TObjArray>::iterator iter = hMC.find(name.Data());
	if( iter != hMC.end() ) {
	    TArrayI& ar = hDC[(*iter).first];
	    Int_t s = ar.GetSize();
	    if(s != 4) {
		Error("getCanvasDim4()","dimension of array does not match!");
		return c;
	    }

	    map<TString,TCanvas*****>::iterator iter1 = hDim4C.find(name.Data());
	    if (iter1 == hDim4C.end()){
		c = newDim4(c,ar.At(0),ar.At(1),ar.At(2),ar.At(3));
                hDim4C[name] = (TCanvas*****) c;
	    } else {
		c = (T*****) iter1->second;
	    }

	    TObjArray& a = iter->second;
	    Int_t ind = 0;
	    for(Int_t i1 = 0; i1 < ar.At(0); i1 ++) {
		for(Int_t i2 = 0; i2 < ar.At(1); i2 ++) {
		    for(Int_t i3 = 0; i3 < ar.At(2); i3 ++) {
			for(Int_t i4 = 0; i4 < ar.At(3); i4 ++) {
			    c[i1][i2][i3][i4] = (T*)a.At(ind);
			    ind++;
			}
		    }
		}
	    }
	}
	return c;
    }

    template<class T> T****** getCanvasDim5(T******c,TString name){
	//  use :  T****** c = hM->getCanvasDim5(c, "name")
	//  Do not delete the created array of pointers when retrieving
	//  them for canvas arrays managed by HHistMap, they will
        //  be deleted in the destructor!
	c = NULL;
	map<TString,TObjArray>::iterator iter = hMC.find(name.Data());
	if( iter != hMC.end() ) {
	    TArrayI& ar = hDC[(*iter).first];
	    Int_t s = ar.GetSize();
	    if(s != 5) {
		Error("getCanvasDim5()","dimension of array does not match!");
		return c;
	    }

	    map<TString,TCanvas******>::iterator iter1 = hDim5C.find(name.Data());
	    if (iter1 == hDim5C.end()){
		c = newDim5(c,ar.At(0),ar.At(1),ar.At(2),ar.At(3),ar.At(4));
                hDim5C[name] = (TCanvas******) c;
	    } else {
		c = (T******) iter1->second;
	    }

	    TObjArray& a = iter->second;
	    Int_t ind = 0;
	    for(Int_t i1 = 0; i1 < ar.At(0); i1 ++) {
		for(Int_t i2 = 0; i2 < ar.At(1); i2 ++) {
		    for(Int_t i3 = 0; i3 < ar.At(2); i3 ++) {
			for(Int_t i4 = 0; i4 < ar.At(3); i4 ++) {
			    for(Int_t i5 = 0; i5 < ar.At(4); i5 ++) {
				c[i1][i2][i3][i4][i5] = (T*)a.At(ind);
				ind++;
			    }
			}
		    }
		}
	    }
	}
	return c;
    }

    //------------------------------------------------------------------------
    // generic

    template<class T> static T**     newDim1(T** h,Int_t imax1){
	//  User has to take care of deleting pointer arrays (see deleteDim1() )!

	h = NULL;
	h = new  T* [imax1];
	return h;
    };

    template<class T> static T***    newDim2(T*** h,Int_t imax1,Int_t imax2){
	//  User has to take care of deleting pointer arrays (see deleteDim2() )!
	h = NULL;
	h = new  T** [imax1];
	for(Int_t i1 = 0; i1 < imax1; i1 ++){
	    h[i1] = new T* [imax2];
	}
	return h;
    };

    template<class T> static T****   newDim3(T****h ,Int_t imax1,Int_t imax2,Int_t imax3){
	//  User has to take care of deleting pointer arrays (see deleteDim3() )!
	h = NULL;
	h = new  T*** [imax1];
	for(Int_t i1 = 0; i1 < imax1; i1 ++){
	    h[i1] = new T** [imax2];
	    for(Int_t i2 = 0; i2 < imax2; i2 ++){
		h[i1][i2] = new T* [imax3];
	    }
	}
	return h;
    }

    template<class T> static T*****  newDim4(T*****h,Int_t imax1,Int_t imax2,Int_t imax3,Int_t imax4){
	//  User has to take care of deleting pointer arrays (see deleteDim4() )!
	h = NULL;
	h = new T**** [imax1];
	for(Int_t i1 = 0; i1 < imax1; i1 ++){
	    h[i1] = new T*** [imax2];
	    for(Int_t i2 = 0; i2 < imax2; i2 ++){
		h[i1][i2] = new T** [imax3];
		for(Int_t i3 = 0; i3 < imax3; i3 ++){
		    h[i1][i2][i3] = new T* [imax4];
		}
	    }
	}
	return h;
    }

    template<class T> static T****** newDim5(T******h,Int_t imax1,Int_t imax2,Int_t imax3,Int_t imax4,Int_t imax5){
	//  User has to take care of deleting pointer arrays (see deleteDim5() )!
	h = NULL;
	h = new T***** [imax1];
	for(Int_t i1 = 0; i1 < imax1; i1 ++){
	    h[i1] = new T**** [imax2];
	    for(Int_t i2 = 0; i2 < imax2; i2 ++){
		h[i1][i2] = new T*** [imax3];
		for(Int_t i3 = 0; i3 < imax3; i3 ++){
		    h[i1][i2][i3] = new T** [imax4];
		    for(Int_t i4 = 0; i4 < imax4; i4 ++){
			h[i1][i2][i3][i4] = new T* [imax5];
		    }
		}
	    }
	}
	return h;
    }

    template<class T> static void deleteDim1(T**    h,Int_t imax1,Bool_t deleteObjects = kFALSE){
	// use this function to dlete dynamically created arrays of hist pointers
	// Do not apply this to histograms managed by HHistMap, this pointer arrays
        // will be deleleted in the destructor
	for(Int_t i1 = 0; i1 < imax1; i1 ++){
	    if (deleteObjects) delete h[i1];
	}
	delete [] h;
	h = NULL;
    }

    template<class T> static void deleteDim2(T***    h,Int_t imax1,Int_t imax2,Bool_t deleteObjects = kFALSE){
	// use this function to dlete dynamically created arrays of hist pointers
	// Do not apply this to histograms managed by HHistMap, this pointer arrays
        // will be deleleted in the destructor
	for(Int_t i1 = 0; i1 < imax1; i1 ++){
	    for(Int_t i2 = 0; i2 < imax2; i2 ++){
		if (deleteObjects) delete h[i1][i2];
	    }
	    delete [] h[i1];
	}
	delete [] h;
	h = NULL;
    }

    template<class T> static void deleteDim3(T****   h,Int_t imax1,Int_t imax2,Int_t imax3,Bool_t deleteObjects = kFALSE){
	// use this function to dlete dynamically created arrays of hist pointers
	// Do not apply this to histograms managed by HHistMap, this pointer arrays
        // will be deleleted in the destructor
	for(Int_t i1 = 0; i1 < imax1; i1 ++){
	    for(Int_t i2 = 0; i2 < imax2; i2 ++){
		for(Int_t i3 = 0; i3 < imax3; i3 ++){
		    if (deleteObjects) delete h[i1][i2][i3];
		}
		delete [] h[i1][i2];
	    }
	    delete [] h[i1];
	}
	delete [] h;
	h = NULL;
    }

    template<class T> static void deleteDim4(T*****  h,Int_t imax1,Int_t imax2,Int_t imax3,Int_t imax4,Bool_t deleteObjects = kFALSE){
	// use this function to dlete dynamically created arrays of hist pointers
	// Do not apply this to histograms managed by HHistMap, this pointer arrays
        // will be deleleted in the destructor
	for(Int_t i1 = 0; i1 < imax1; i1 ++){
	    for(Int_t i2 = 0; i2 < imax2; i2 ++){
		for(Int_t i3 = 0; i3 < imax3; i3 ++){
		    for(Int_t i4 = 0; i4 < imax4; i4 ++){
			if (deleteObjects) delete h[i1][i2][i3][i4];
		    }
		    delete [] h[i1][i2][i3];
		}
		delete [] h[i1][i2];
	    }
	    delete [] h[i1];
	}
	delete [] h;
	h = NULL;
    }

    template<class T> static void deleteDim5(T****** h,Int_t imax1,Int_t imax2,Int_t imax3,Int_t imax4,Int_t imax5,Bool_t deleteObjects = kFALSE){
	// use this function to dlete dynamically created arrays of hist pointers
	// Do not apply this to histograms managed by HHistMap, this pointer arrays
        // will be deleleted in the destructor
	for(Int_t i1 = 0; i1 < imax1; i1 ++){
	    for(Int_t i2 = 0; i2 < imax2; i2 ++){
		for(Int_t i3 = 0; i3 < imax3; i3 ++){
		    for(Int_t i4 = 0; i4 < imax4; i4 ++){
			for(Int_t i5 = 0; i5 < imax5; i5 ++){
			    if (deleteObjects) delete h[i1][i2][i3][i4][i5];
			}
			delete [] h[i1][i2][i3][i4];
		    }
		    delete [] h[i1][i2][i3];
		}
		delete [] h[i1][i2];
	    }
	    delete [] h[i1];
	}
	delete [] h;
	h = NULL;
    }
    //------------------------------------------------------------------------


    ClassDef(HHistMap,1)
};

#endif /* !__HHISTMAP_H__ */




