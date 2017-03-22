
#ifndef __HSelectEmbedding___


#include "hreconstructor.h"
#include "hcategorymanager.h"
#include "hcategory.h"
#include "hrecevent.h"

//--------category definitions---------
#include "hgeantdef.h"
//-------------------------------------

//-------objects-----------------------
#include "hgeantkine.h"
//-------------------------------------


#include <iostream>
#include <cstdlib>
using namespace std;


class HSelectEmbedding : public HReconstructor
{

   Int_t totalEvents;
   Int_t acceptedEvents;
public:

    HSelectEmbedding(const Text_t *name = "",const Text_t *title ="") {
	totalEvents=0;
	acceptedEvents=0;
    }
    virtual ~HSelectEmbedding() {;}

    Bool_t init()
    {
        return kTRUE;
    }

    Int_t execute()
    {
        totalEvents++;
	// -------------------------------------------------------------------------
	HCategory* fCatKine = HCategoryManager::getCategory(catGeantKine,kTRUE,"catGeantKine");
	if(!fCatKine) { cout<<"NO catGeantKine!"<<endl; exit(1);}
	// -------------------------------------------------------------------------
        Int_t seqNr = (Int_t)gHades->getCurrentEvent()->getHeader()->getEventSeqNumber() ;

	// -------------------------------------------------------------------------
	// check if there is a fitting seqNumer in Kine userVal
	// since the events can be merge with delta electrons events (not produce for the them seqnumber)
        // we have to check if any object fullfills the condition
	HGeantKine* kine;
        Bool_t found = kFALSE;
	Int_t n = fCatKine->getEntries();
	Int_t nr = 0;
	for(Int_t i = 0; i < n; i ++){
	    kine = HCategoryManager::getObject(kine,fCatKine,i);
	    if(kine){
		nr = (Int_t) kine->getUserVal();

		if(nr==seqNr) {
                    found = kTRUE;
                    break;
		}
	    }
	} // loop kine

	if(!found){
	    //cout<<"skip  event seqNr in "<<seqNr<<" "<<nr<<endl;
	    return kSkipEvent;
	}
	// -------------------------------------------------------------------------

        acceptedEvents++;
	return 0;
    }

    Bool_t finalize()
    {
	cout<<"HSelectEmbedding : accepted "<<acceptedEvents<<" out of "<<totalEvents<<" events ("<< (totalEvents >0 ? ((Float_t)acceptedEvents/totalEvents)*100 : 0)<<"%)"<<endl;

	return kTRUE;
    }

};
#endif
