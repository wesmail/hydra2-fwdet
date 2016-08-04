using namespace std;
#include "honlinemonstack.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"
#include <iostream> 
#include <iomanip>
#include "TObjString.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TList.h"

ClassImp(HOnlineMonStack)


HOnlineMonStack::HOnlineMonStack(const Char_t* name, const Char_t* title )
    :  HOnlineMonHistAddon(name,title,1,1)
{
    stack = new TObjArray();
    firstName = "";
}

HOnlineMonStack::HOnlineMonStack(HOnlineMonStack& h)
    :  HOnlineMonHistAddon(h.GetName(),h.GetTitle(),1,1)
{

    cout<<"copy Object"<<endl;

    stack = new TObjArray();

    TIterator* iter = stack->MakeIterator();
    TObject* obj = NULL;
    while ( (obj = iter->Next()) !=NULL)
    {
	TString clname = obj->ClassName();
	if(clname.CompareTo("HOnlineMonHist") == 0) {
	    stack->Add(new HOnlineMonHist(*((HOnlineMonHist*)obj)));

	} else if (clname.CompareTo("HOnlineMonHist2")==0 ) {
	    stack->Add(new HOnlineMonHist2(*((HOnlineMonHist2*)obj)));

	} else if (clname.CompareTo("HOnlineHistArray")==0 ) {
	    stack->Add(new HOnlineHistArray(*((HOnlineHistArray*)obj)));

	} else if (clname.CompareTo("HOnlineHistArray2")==0 ) {
	    stack->Add(new HOnlineHistArray2(*((HOnlineHistArray2*)obj)));

	} else if (clname.CompareTo("HOnlineTrendArray")==0 ) {
	    stack->Add(new HOnlineTrendArray(*((HOnlineTrendArray*)obj)));

	} else if (clname.CompareTo("TLegend")==0 ) {
	    stack->Add(new TLegend(*((TLegend*)obj)));

	} else if (clname.CompareTo("TGraph")==0 ) {
	    stack->Add(new TGraph(*((TGraph*)obj)));

	} else {
	    Error("HOnlineMonStack()","Unsupported Object type =%s!",clname.Data());
	}

    }
    delete iter;

    firstName = "";
}

void HOnlineMonStack::reset(Int_t level,Int_t count)
{

    if(level < 10 ) return; // online on client side!
    TIterator* iter = stack->MakeIterator();
    TObject* obj = NULL;
    while ( (obj = iter->Next()) !=NULL)
    {

	TString clname = obj->ClassName();
	if(clname.CompareTo("HOnlineMonHist") == 0 ||
           clname.CompareTo("HOnlineMonHist2") == 0 ||
           clname.CompareTo("HOnlineHistArray") == 0 ||
           clname.CompareTo("HOnlineHistArray2") == 0 ||
           clname.CompareTo("HOnlineTrendArray") == 0

	  ) {

	    ((HOnlineMonHistAddon*)obj)->reset(0,0);

	} else if (clname.CompareTo("TGraph")==0 ) {


	}
	if(obj->InheritsFrom("TH1")) {
	    ((TH1*)obj)->Reset();
	}

    }
    delete iter;

}

void HOnlineMonStack::add(HOnlineMonHistAddon* h)
{
    HOnlineMonStack* st = (HOnlineMonStack*) h;

    TIterator* iter = stack->MakeIterator();
    TObject* obj = NULL;
    while ( (obj = iter->Next()) !=NULL)
    {

	TString clname = obj->ClassName();
	if(clname.CompareTo("HOnlineMonHist") == 0 ||
           clname.CompareTo("HOnlineMonHist2") == 0 ||
           clname.CompareTo("HOnlineHistArray") == 0 ||
           clname.CompareTo("HOnlineHistArray2") == 0 ||
           clname.CompareTo("HOnlineTrendArray") == 0

	  ) {
	    HOnlineMonHistAddon* addon  = (HOnlineMonHistAddon*)obj;
	    HOnlineMonHistAddon* addon2 = (HOnlineMonHistAddon*) st->getStack()->FindObject(addon->GetName());

	    addon->add(addon2);

	} else if (clname.CompareTo("TGraph")==0 ) {


	}
	if(obj->InheritsFrom("TH1")) {
	    TH1* h  = (TH1*)obj;
	    TH1* h2 = (TH1*) st->getStack()->FindObject(h->GetName());

	    h->Add(h2);
	}


    }
    delete iter;

}

void HOnlineMonStack::getMinMax(Double_t& min,Double_t& max)
{

    max = -1e200;
    min =  1e200;

    Double_t minL = min;
    Double_t maxL = max;

    TIterator* iter = stack->MakeIterator();
    TObject* obj = NULL;
    while ( (obj = iter->Next()) !=NULL)
    {
	TString clname = obj->ClassName();
	if(obj->InheritsFrom("HOnlineMonHistAddon")){
	    ((HOnlineMonHistAddon*)obj)->getMinMax(minL,maxL);
            if(minL < min) min = minL;
            if(maxL > max) max = maxL;
	} else if (clname.CompareTo("TGraph")==0 ) {


	} else if (obj->InheritsFrom("TH1") ) {

	    if(clname.Contains("TH1")) {

                TH1* h = (TH1*)obj;

		for(Int_t i = 0; i < h->GetNbinsX(); i++ ) {
		    Double_t val = h->GetBinContent(i+1);
		    if(val < min) min = val;
		    if(val > max) max = val;

		}
	    }

            if(clname.Contains("TH2")) {

                TH2* h = (TH2*)obj;

		for(Int_t i = 0; i < h->GetNbinsX(); i++ ) {
		    for(Int_t j = 0; j < h->GetNbinsY(); j++ ) {
			Double_t val = h->GetBinContent(i+1,j+1);
			if(val < min) min = val;
			if(val > max) max = val;
		    }
		}
	    }
	}
    }
    delete iter;


}


void HOnlineMonStack::draw(Bool_t allowHiddenZero)
{


    Double_t min,max;
    getMinMax(min,max);

    TIterator* iter = stack->MakeIterator();
    TObject* obj = NULL;
    Bool_t first =kTRUE;

    while ( (obj = iter->Next()) !=NULL)
    {

	TString clname = obj->ClassName();

	Int_t linecol     = -1;
        Int_t linestyle   = -1;
        Int_t fillcol     = -1;
        Int_t fillstyle   = -1;
        Int_t markercol   = -1;
        Int_t markerstyle = -1;
        Float_t markersize  = -1;
        TString Option    = "no";

        //---------------------------------------------------
        // options support
	if(clname.CompareTo("HOnlineMonHist")   == 0 ||
           clname.CompareTo("HOnlineMonHist2")  == 0 ||
           clname.CompareTo("HOnlineTrendHist") == 0 ||
           obj->InheritsFrom("TH1")                  ||
           clname.CompareTo("TGraph")==0
	  ){
	    TString name = ((TNamed*)obj)->GetName();

	    if(options.find(name) != options.end()){

		TString option = options[name];

		TObjArray* myarguments = option.Tokenize(" ");
		TIterator* myiter = myarguments->MakeIterator();
                TObjString* stemp;
	        TString argument;
	        // iterate over the list of arguments
		while ((stemp=(TObjString*)myiter->Next())!= 0)
		{   //LINE#0:0 FILL#0:0 MARKER#0:0:0 OPT#
		    argument=stemp->GetString();
		    if(argument.Contains("LINE#")){
			argument.ReplaceAll(":"," ");
			sscanf(argument.Data(),"%i%i",&linecol,&linestyle);

		    } else if (argument.Contains("FILL#")) {
			argument.ReplaceAll("FILL#","");
			argument.ReplaceAll(":"," ");
			sscanf(argument.Data(),"%i%i",&fillcol,&fillstyle);

		    } else if (argument.Contains("MARKER#")) {
			argument.ReplaceAll("MARKER#","");
			argument.ReplaceAll(":"," ");
			sscanf(argument.Data(),"%i%i%f",&markercol,&markerstyle,&markersize);

                    } else if (argument.Contains("OPT#")) {
			argument.ReplaceAll("OPT#","");
			Option=argument;
		    }

		}

		delete myiter;
		myarguments->Delete();
		delete myarguments;

	    }



	}
        //---------------------------------------------------



	if(obj->InheritsFrom("HOnlineMonHistAddon")){

            if(!first) ((HOnlineMonHistAddon*)obj)->setDrawSame(kTRUE);

	    if(clname.CompareTo("HOnlineHistArray") == 0 ){
		((HOnlineHistArray*)obj)->drawAll( kTRUE ,min,max);
	    } else if(clname.CompareTo("HOnlineHistArray2") == 0 ){
		((HOnlineHistArray2*)obj)->drawAll( kTRUE ,min,max);
            } else if(clname.CompareTo("HOnlineTrendArray") == 0 ){
		((HOnlineTrendArray*)obj)->drawAll( kTRUE ,min,max);
	    } else ((HOnlineMonHistAddon*)obj)->draw();


	    first = kFALSE;

	} else if (clname.CompareTo("TGraph")==0 && !first) {


            if(linecol     > 0) ((TGraph*)obj)->SetLineColor(linecol);
            if(linestyle   > 0) ((TGraph*)obj)->SetLineStyle(linestyle);
            if(markerstyle > 0) ((TGraph*)obj)->SetMarkerStyle(markercol);
            if(markercol   > 0) ((TGraph*)obj)->SetMarkerColor(markercol);
            if(markersize  > 0) ((TGraph*)obj)->SetMarkerSize(markersize);

	    if(!first) {
		if(Option == "no") ((TGraph*)obj)->Draw("Psame");
        	else               ((TGraph*)obj)->Draw(Form("%ssame",Option.Data()));
            }
	} else if (obj->InheritsFrom("TH1") ) {

	    if(clname.Contains("TH1")){
		((TH1*)obj)->SetMinimum(min*1.1);
		((TH1*)obj)->SetMaximum(max*1.1);
		if(linecol     > 0) ((TH1*)obj)->SetLineColor(linecol);
		if(linestyle   > 0) ((TH1*)obj)->SetLineStyle(linestyle);
		if(markerstyle > 0) ((TH1*)obj)->SetMarkerStyle(markercol);
		if(markercol   > 0) ((TH1*)obj)->SetMarkerColor(markercol);
		if(markersize  > 0) ((TH1*)obj)->SetMarkerSize(markersize);

	    }

	    if(clname.Contains("TH2")){
		// nothing to do
		if(markerstyle > 0) ((TH1*)obj)->SetMarkerStyle(markercol);
		if(markercol   > 0) ((TH1*)obj)->SetMarkerColor(markercol);
		if(markersize  > 0) ((TH1*)obj)->SetMarkerSize(markersize);
	    }


	    if(first){
		if(Option == "no")((TH1*)obj)->Draw();
		else              ((TH1*)obj)->Draw(Option.Data());
	    } else {
		if(Option == "no")((TH1*)obj)->Draw("same");
		else              ((TH1*)obj)->Draw(Form("%ssame",Option.Data()));
	    }
	    first = kFALSE;

	} else if (clname.CompareTo("TLegend")==0 ) {

	    ((TLegend*)obj)->Draw();
	}



    }
    delete iter;

}


HOnlineMonStack::~HOnlineMonStack()
{
    if(stack) delete stack;
    stack = 0;
}
void HOnlineMonStack::addTimeStamp( TString timestamp)
{
    if(stack->GetEntries() > 0){
	TObject* obj = stack->At(0);

	if(obj && firstName == "" && obj->InheritsFrom("TNamed")){
	    firstName = ((TNamed*)obj)->GetTitle();
	}

	if(obj) ((TNamed*)obj) ->SetTitle(Form("%s%s",firstName.Data(),timestamp.Data()));
    }
}
void  HOnlineMonStack::addToStack(TObject* obj,TString option)
{
    if(option.CompareTo("") != 0 && obj->InheritsFrom("TNamed")){
	TString name = ((TNamed*)(obj))->GetName();
	if(name.CompareTo("") != 0){
	    if(options.find(name) == options.end()) {
		options[name] = option;
	    }
	}
    }
    stack->Add(obj);
}

