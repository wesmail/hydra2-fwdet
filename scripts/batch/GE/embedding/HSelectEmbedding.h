
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
public:

    HSelectEmbedding(const Text_t *name = "",const Text_t *title ="") {
    }
    virtual ~HSelectEmbedding() {;}

    Bool_t init()
    {
        return kTRUE;
    }

    Int_t execute()
    {
	// -------------------------------------------------------------------------
	HCategory* fCatKine = HCategoryManager::getCategory(catGeantKine,kTRUE,"catGeantKine");
	if(!fCatKine) { cout<<"NO catGeantKine!"<<endl; exit(1);}
	// -------------------------------------------------------------------------
        UInt_t seqNr = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber() ;

	// -------------------------------------------------------------------------
	HGeantKine* kine;

	Int_t n = fCatKine->getEntries();
	for(Int_t i = 0; i < n; i ++){
	    kine = HCategoryManager::getObject(kine,fCatKine,i);
	    if(kine){
		UInt_t nr = (UInt_t) kine->getUserVal();
		if(nr!=seqNr) {
                    cout<<"skip  event seqNr in "<<seqNr<<" "<<nr<<endl;
		    return kSkipEvent;
		}
	    }
	} // loop kine
	//cout<<"accept  event seqNr in "<<seqNr<<endl;
	// -------------------------------------------------------------------------


	return 0;
    }

    Bool_t finalize()
    {
	return kTRUE;
    }

};
#endif
