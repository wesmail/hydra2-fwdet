#include "hntuplemap.h"
#include "TObjArray.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TSystem.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <string>
#include <cstdlib>

using namespace std;

ClassImp(HNtupleMap)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
// HNtupleMap
//
// This Class is designed to make the use of existing TNuples
// more comfortable. TFile, and TNtuple are puplic to allow
// to access the full functionality of this classes. In createMap(...)
// the specified root file will be opened and the pointer to the ntuple
// created. A STL map c is created which allows to access the ntuple members
// by name. The map will be used later to index the data pointer
// of Float_t* data = ntuple->GetArgs();
// data[c["x"]] will give back the value of variable "x" of the ntuple
// with c["x"] the by this function created map.
// CAUTION: DUE TO THE USE OF STL MAP YOU HAVE TO PAY A PERFORMANCE PENALTY!
//
// example :
//
// void testNtupleMap()
// {
//    HNtupleMap a;
//    a.createMap("myfile.root", "myntuple");
//    a.printMap();
//
//    Int_t n = a.ntuple->GetEntries();
//    for(Int_t i = 0; i < n ; i ++){
//	a.ntuple->GetEntry(i);
//	cout<<a["some_member"]<<" "<<a["some_other_member"]<<" "<<endl;
//    }
// }
////////////////////////////////////////////////////////////////////////////

HNtupleMap::HNtupleMap()
{
    ntuple = NULL;
    file   = NULL;
    a      = NULL;
}

HNtupleMap::~HNtupleMap()
{

    if(ntuple) {
	delete ntuple;
	ntuple = NULL;
    }
    if(file) {
	file ->Close();
	delete file;
	file   = NULL;
    }
    a = NULL;
}

void HNtupleMap::createMap(TString in, TString ntupleName)
{
    // create a STL map with the key  == leaf name of the ntuple
    // the mapped value stores the index of the leave in the list
    // of leaves.
    file = new TFile(in.Data(),"READ");
    if(!file)  { Error("createMap()","File %s not found!",in.Data()); exit(1);}

    ntuple = (TNtuple*) file->Get(ntupleName.Data());
    if(!ntuple) { Error("createMap()","Ntuple %s not found!",ntupleName.Data()); exit(1);}

    TObjArray* li = ntuple->GetListOfLeaves();
    if(li){
	for(Int_t i = 0; i < li->GetLast() + 1; i ++){
	    TLeaf* leaf = (TLeaf*) li->At(i);
	    c.insert(make_pair(leaf->GetName(),i));
	}
	a = ntuple->GetArgs();
	if(!a) { Error("createMap()","Could not get Argument pointer!"); exit(1);}
    } else { Error("createMap()","Could not get list of leaves!"); exit(1);}
}

Float_t& HNtupleMap::operator[](const std::string& val) {
    if(!a) { Error("operator[]","map not initialized!"); exit(1);}

    map<string,int>::iterator iter = c.find(val);
    if( iter == c.end() ) { Error("operator[]","key not found!"); exit(1);}
    else    return a[iter->second];
}

void HNtupleMap::printMap(){
    if(ntuple){
	cout<<"-------------------------------------------------"<<endl;
	cout<<"Mapped index of ntuple : "<<ntuple->GetName()<<endl;
	map<string,Int_t>::iterator it;
	Int_t ct=0;
	for(it = c.begin(); it != c.end(); it++){
	    cout<<right<<setw(5)<<ct<<" leaf : "<<setw(20)<< it->first <<", index: "<<setw(5)<<it->second<<endl;
	    ct++;
	}
	cout<<"-------------------------------------------------"<<endl;
    } else {
	Error("printMap()","Ntuple pointer is NULL!"); exit(1);}
}
