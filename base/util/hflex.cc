#include "hflex.h"
#include "TObjString.h"
#include "TObjArray.h"

#include <iostream>
#include <iomanip>

using namespace std;

ClassImp(HFlex)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
// HFlex
//
//
// example :
//
// void testFlex()
// {
//    HFlex a;
//    a.addVars("n=0:mult=0", "x=-1000:y=-1000:z=-1000");
//    a.print();
//    a.setI("n",5);
//    a.setD("x",1);
//    a.setD("y",2);
//    a.setD("z",3);
//    a.print();
//    cout<<a.getI("n")<<" "<<a.getD("x")<<endl;
// }
////////////////////////////////////////////////////////////////////////////


void HFlex::addVars(TString varsI,TString varsD)
{
    // add list of inter (first argument) and double
    // variables (second argument) to the object.
    // example : addVars("n=0:mult=0", "x=-1000:y=-1000:z=-1000")
    // The variable names are separated by ":". They might
    // be obtional be initialized using "=value". This function
    // has to be called first (only once) before variables can be used.


    varsI.ReplaceAll(" ","");
    varsD.ReplaceAll(" ","");

    //-----------Integer vars-------------------------
    if(varsI!="")
    {
	TObjArray* ar = varsI.Tokenize(":"); // split string by ":"

	for(Int_t i = 0; i < ar->GetEntries(); i ++)
	{
	    TObjString* str=(TObjString*)ar->At(i);

	    TString v=str->GetString();

	    //-------------------------------------------
	    if(v.Contains("=") != 0){   // asigned value for var
		Ssiz_t first = v.First("=");
		if(first>0){
		    TString v1 = v;
		    v1.Replace(first,v1.Length()-first,"");
		    v.Replace(0,first+1,"");

		    map<TString,Int_t>::iterator iter = mInt.find(v1);
		    if( iter == mInt.end() ){ // new variable
			if(v!="") mInt[v1] = v.Atoi();
			else  {
			    Error("addVars()","var name \"%s\" has no assigned value but \"=\" ! assign 0.",v1.Data());
			    mInt[v1] = 0;
			}
		    } else { // exists already
			Error("addVars()","var name \"%s\" has been used already!",v1.Data());
		    }

		} else {
		    Error("addVars()","var name empty but assigned value = %s ! skipped!",v.Data());
		}
		//-------------------------------------------
	    } else { // only var , no asigned val
		map<TString,Int_t>::iterator iter = mInt.find(v);
		if( iter == mInt.end() ){ // new variable
		    mInt[v] = 0;
		} else { // exists already
		    Error("addVars()","var name \"%s\" has been used already!",v.Data());
		}
	    }
	    //-------------------------------------------

	}

	ar->Delete();
	delete ar;
    }
    //-------------------------------------------

    //-----------Double_t vars-------------------------
    if(varsD!="")
    {
	TObjArray* ar = varsD.Tokenize(":"); // split string by ":"

	for(Int_t i = 0; i < ar->GetEntries(); i ++){
	    TObjString* str=(TObjString*)ar->At(i);

	    TString v=str->GetString();

	    //-------------------------------------------
	    if(v.Contains("=") != 0){   // asigned value for var
		Ssiz_t first = v.First("=");
		if(first>0){
		    TString v1 = v;
		    v1.Replace(first,v1.Length()-first,"");
		    v.Replace(0,first+1,"");

		    map<TString,Double_t>::iterator iter = mDouble.find(v1);
		    if( iter == mDouble.end() ){ // new variable
			if(v!="") mDouble[v1] = v.Atof();
			else  {
			    Error("addVars()","var name \"%s\" has no assigned value but \"=\" ! assign 0.",v1.Data());
			    mDouble[v1] = 0;
			}
		    } else { // exists already
			Error("addVars()","var name \"%s\" has been used already!",v1.Data());
		    }

		} else {
		    Error("addVars()","var name empty but assigned value = %s ! skipped!",v.Data());
		}
		//-------------------------------------------
	    } else { // only var , no asigned val
		map<TString,Double_t>::iterator iter = mDouble.find(v);
		if( iter == mDouble.end() ){ // new variable
		    mDouble[v] = 0;
		} else { // exists already
		    Error("addVars()","var name \"%s\" has been used already!",v.Data());
		}
	    }
	    //-------------------------------------------

	}

	ar->Delete();
	delete ar;

    }
    //-------------------------------------------




}


Int_t HFlex::getI(TString var){
    // retrieve integer variable var. returns 0 if
    // var is not known

    map<TString,Int_t>::iterator iter = mInt.find(var);
    if( iter == mInt.end() ){
	Error("getInt()","var \"%s\" not found!",var.Data());
	return 0;
    }
    else                       return iter->second;
}

Double_t HFlex::getD(TString var){
    // retrieve double variable var. returns 0 if
    // var is not known

    map<TString,Double_t>::iterator iter = mDouble.find(var);
    if( iter == mDouble.end() ){
	Error("getDouble()","var \"%s\" not found!",var.Data());
	return 0;
    }
    else                       return iter->second;
}

void HFlex::setI(TString var,Int_t val){
    // set integer variable var to val.

    map<TString,Int_t>::iterator iter = mInt.find(var);
    if( iter == mInt.end() ){
	Error("getInt()","var \"%s\" not found!",var.Data());
    }
    else  iter->second=val;
}

void HFlex::setD(TString var,Double_t val){
    // set double variable var to val.

    map<TString,Double_t>::iterator iter = mDouble.find(var);
    if( iter == mDouble.end() ){
	Error("getInt()","var \"%s\" not found!",var.Data());
    }
    else  iter->second=val;
}


void HFlex::print(TString varlist,Int_t w)
{

    // print all variable : type name value
    // the width of the name field can be changed
    // by argument w
    // varlist : "var1:var2" (print all if empty)
    cout<<"------------------"<<endl;

    TObjArray* ar = varlist.Tokenize(":"); // split string by ":"


    map<TString,Int_t>::iterator iter;
    for( iter = mInt.begin(); iter != mInt.end(); ++iter ) {
	if(varlist=="" ) cout <<"Int_t    "<<setw(w)<< iter->first.Data() << " = " << iter->second << endl;
	else {
	    for(Int_t i = 0; i < ar->GetEntries(); i ++){
		TObjString* str=(TObjString*)ar->At(i);
		TString v=str->GetString();
		if(v==iter->first)
		{
		    cout <<"Int_t    "<<setw(w)<< iter->first.Data() << " = " << iter->second << endl;
                    break;
		}
	    }
	}
    }
    map<TString,Double_t>::iterator iter2;
    for( iter2 = mDouble.begin(); iter2 != mDouble.end(); ++iter2 ) {
	if(varlist=="" ) cout <<"Double_t "<<setw(w)<< iter2->first.Data() << " = " << iter2->second << endl;
	else {
	    for(Int_t i = 0; i < ar->GetEntries(); i ++){
		TObjString* str=(TObjString*)ar->At(i);
		TString v=str->GetString();
		if(v==iter2->first)
		{
		    cout <<"Int_t    "<<setw(w)<< iter2->first.Data() << " = " << iter2->second << endl;
                    break;
		}
	    }
	}
    }
    ar->Delete();
    delete ar;

    cout<<"------------------"<<endl;
}

