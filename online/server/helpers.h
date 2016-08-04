#ifndef ___HELPERS____
#define ___HELPERS____

#include "honlinemonhist.h"
#include "honlinemonstack.h"
#include "TString.h"
#include "TList.h"
#include "TIterator.h"

#include <map>
#include <iostream>
#include <stdlib.h>

#include <cstdlib>

using namespace  std;


class mapHolder {
private:
    static map < TString , HOnlineMonHistAddon* >* currentMap;
    static mapHolder* holder;
    mapHolder() {
        holder = this;
    };
public:
    static mapHolder* getMapHolder(){
	if(holder == 0) return new mapHolder;
        else            return holder;
    }
    static void setMap(map < TString , HOnlineMonHistAddon* >& myMap){
         currentMap = &myMap;
    }
    static map < TString , HOnlineMonHistAddon* >* getMap() { return currentMap; }

    static void resetHists(Int_t evtCt){
	// do reset if needed
	for( map<TString , HOnlineMonHistAddon*>::iterator iter = currentMap->begin(); iter != currentMap->end(); ++iter ) {
	    iter->second->reset(2,evtCt); // reset hists if needed  (2 = if resetable and count%getRefreshRate() == 0)
	}
    }
    static void createHists(Int_t nHists,const Char_t* hists[],TList& histpool){

        map<TString , HOnlineMonHistAddon*>& mapL = *currentMap;
	HOnlineMonHistAddon* addon;
	// create hists and add them to the pool
	for(Int_t i=0; i < nHists; i++ ){
	    addon = new HOnlineMonHistAddon((Text_t*)hists[i]);
	    if(addon->getActive() == 1){
		HOnlineMonHistAddon* addon2 = addon->createHist();
		if (!addon2) { cout<<"error : could not create hist : "<<addon->GetName()<<"!"<<endl; exit(2);}
		histpool.Add(addon2);
		mapL[addon->GetName()] = (HOnlineMonHistAddon*) histpool.FindObject(addon->GetName());
		delete addon;
	    } else {
		mapL[addon->GetName()] = 0;
	    }
	}
    }

    static void addStack(HOnlineMonStack* stack,TList& histpool){

        map<TString , HOnlineMonHistAddon*>& mapL = *currentMap;

	if(stack->getActive() == 1){
	    if(mapL.find(stack->GetName())== mapL.end()){
		histpool.Add(stack);
		mapL[stack->GetName()] = (HOnlineMonHistAddon*) histpool.FindObject(stack->GetName());
	    }
	}
    }

};

mapHolder*                              mapHolder::holder     = 0;
map < TString , HOnlineMonHistAddon* >* mapHolder::currentMap = 0;



void printMap(map< TString, HOnlineMonHistAddon* >& mapL){
    Int_t ct = 0;
    cout<<"------------------------------------------------------"<<endl;
    cout<<"Available histograms :"<<endl;
    for( map< TString, HOnlineMonHistAddon*>::iterator iter = mapL.begin(); iter != mapL.end(); ++iter ) {
	ct++;
	TString classname = "unused";
	if((*iter).second ) classname = (*iter).second->ClassName();
	cout <<ct<<" "<< (*iter).first.Data() << " is a " << classname.Data()<<endl;
    }
    cout<<"------------------------------------------------------"<<endl;
}

void printList(TList& list)
{
    TIterator* it  = list.MakeIterator();

    cout<<"------------------------------------------------------"<<endl;
    cout<<"Available histograms :"<<endl;
    HOnlineMonHistAddon* addon;
    it  = list.MakeIterator();
    while( ((addon = (HOnlineMonHistAddon*) it->Next()) != 0) ) {
	cout<<" active "   <<addon->getActive()
	    <<" resetable "<<addon->getResetable()
	    <<" refresh "  <<addon->getRefreshRate()
	    <<" name : "   <<addon->GetName()
	    <<" type : "   <<addon->ClassName()
	    <<endl;
	}
    delete it;
    cout<<"------------------------------------------------------"<<endl;
}


HOnlineMonHistAddon* get(TString name)
{
    // get histogram from map. print error and map
    // if not existing.
    // mapHolder::getMapHolder()
    // mapHolder::setMap(map< TString, HOnlineMonHistAddon* >& mapL)
    // have to be called before

    map< TString, HOnlineMonHistAddon* >& mapL = *mapHolder::getMap();

    map<TString,HOnlineMonHistAddon*>::iterator iter = mapL.find(name.Data());
    if( iter != mapL.end() ) { return (*iter).second ;}
    else {
	printMap(mapL);
	cout<<"Error::get(), No matching histogram found for name = "<<name.Data()<<endl;
        exit(1);
    }
    return 0;
}

#endif
