#include "hflexfiller.h"
#include "hades.h"
#include "haddef.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hcategorymanager.h"

#include <iostream>

using namespace std;



//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HFlexFiller
//
// This HReconstructor allows the user to plugin a private fill function
// defined in a macro into the Hades eventloop with full access to all data levels.
// The user can fill the category of HFlex objects.
//
// The user can provide his own selection fill via
// setUserFill(Int_t (*function)(HHistMap*,TObjArray* ),HHistMap* hM=0,TObjArray* params=0,Bool_t createMap=kTRUE)
// Parameters for the function can be passed via the TObjArray from the macro.
// Histogram pointers can be passed by HHistMap
// If the pointers are NULL they are simply ignored.
//
//----------------------------------------------
// example:
//
//
//Int_t fillFlex(HHistMap* hM=0,TObjArray* pars=0){
//
//
//    HCategory* candCat  = (HCategory*)HCategoryManager::getCategory(catParticleCand ,kTRUE ,"catParticleCand");
//    HCategory* flexCat  = (HCategory*)HCategoryManager::getCategory(catFlex         ,kTRUE ,"catFlex");
//
//    if(!flexCat) { cout<<"NO FlexCat!"<<endl; }
//    if(!candCat) { cout<<"NO candCat!"<<endl; }
//
//    if(candCat&&flexCat)
//    {
//      HLocation loc; // dummy loc object
//	HFlex fl;
//	fl.addVars("candInd=-1","chi2=-1");  // some variables  + init values
//
//	Int_t n = candCat->getEntries();
//	for(Int_t i=0; i < n; i++){
//
//	    HParticleCand* cand = HCategoryManager::getObject(cand,candCat,i);
//	    if(cand){
//
//		Int_t   ind  = cand->getIndex();
//                Float_t chi2 = cand->getChi2();
//                HFlex* flex = 0;
//                Int_t index;
//
//		flex = (HFlex*) flexCat->getNewSlot(loc,&index);
//       	if(flex){
//		    flex = new (flex) HFlex(fl);
//
//		    flex->setI("candInd",ind);
//                    flex->setD("chi2"   ,chi2);
//
//		}
//
//		if(hM){
//                  if(hM->get("chi2")) hM->get("chi2")->Fill(chi2);
//		}
//
//	    }
//
//	} // end loop candidates
//
//
//   }
//    return 0;
//}
//
//
// int dst(){
//
//    .... hades dst macro....
//
//
//
//    HHistMap hM("testHists.root");                              // set output file for hists
//    hM.addHist(new TH1F("chi2","chi2",1000,0,1000),"tracking"); // hist will be written to directory "tracking"
//
//
//    HFlexFiller* flexfiller = new HFlexFiller("FlexFiller","FlexFiller");
//    flexfiller->setUserFill(fillFlex,&hM);                      // connect your private function
//
//
//    .......
//
//
//    // add recontructor to the end of the tasklist
//    masterTaskSet->add(flexfiller);                             // connect the filler
//
//
//    gHades->init();                                             // init params etc
//    gHades->eventLoop(nEvents,startEvt);                        // loop over events. the function will called for each event
//
//    hM.writeHists();                                            // store the hists
//
//    delete gHades;
// }
//
////////////////////////////////////////////////////////////////////////////////

ClassImp(HFlexFiller)


HFlexFiller::HFlexFiller(void)
    : HReconstructor("FlexFiller", "User fill of HFlex category")
{
    clear();
}

HFlexFiller::HFlexFiller(const Text_t* name,const Text_t* title)
: HReconstructor(name, title)
{
    clear();
}

HFlexFiller::~HFlexFiller(void)
{

}

void HFlexFiller::clear()
{
    createCat = kFALSE;
    pUserFill = NULL;
    parameters= NULL;
    hM        = NULL;
}

Bool_t HFlexFiller::init(void)
{
    // create the catFlex category if needed

    if(createCat){
	if(gHades->getCurrentEvent()->getCategory(catFlex)){
	    Error("HFlexFiller::init()"," HFlexCategory exists already!");
	    exit(1);
	}


	HCategory* cat = HCategoryManager::addCategory(catFlex,"HFlex",5000,"",kTRUE);
	if (!cat) {
	    Error("HFlexFiller::init()"," Could not create HFlex category!");
	    return kFALSE;
	}
    }
    return kTRUE;
}

Int_t HFlexFiller::execute(void)
{
    // call the user funcion once per event
    if(!pUserFill) return 0; // nothing to do
    return (*pUserFill)(hM,parameters);

}

Bool_t HFlexFiller::finalize(void)
{
    return kTRUE;
}
