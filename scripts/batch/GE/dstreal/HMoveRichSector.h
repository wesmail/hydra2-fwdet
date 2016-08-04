
#ifndef __HMoveRichSector___


#include "hreconstructor.h"
#include "hcategorymanager.h"
#include "hcategory.h"

//--------category definitions---------
#include "richdef.h"
//-------------------------------------

//-------objects-----------------------
#include "hrichhit.h"
//-------------------------------------


#include <iostream>
#include <cstdlib>
using namespace std;


class HMoveRichSector : public HReconstructor
{
public:

    HMoveRichSector(const Text_t *name = "",const Text_t *title ="") {
    }
    virtual ~HMoveRichSector() {;}

    Bool_t init()
    {
        return kTRUE;
    }

    Int_t execute()
    {
	// -------------------------------------------------------------------------
	HCategory* fCatRichHit = HCategoryManager::getCategory(catRichHit,kTRUE,"catRichHit");
	if(!fCatRichHit) { cout<<"NO catRichHit!"<<endl; exit(1);}
	// -------------------------------------------------------------------------


	// -------------------------------------------------------------------------
	HRichHit* richHit;

	Int_t n = fCatRichHit->getEntries();
	for(Int_t i = 0; i < n; i ++){
	    richHit = HCategoryManager::getObject(richHit,fCatRichHit,i);
	    if(richHit){

		Int_t s = richHit->getSector();
		if(s != 4) richHit->setPhi(richHit->getPhi()+60 );
                else      richHit->setPhi(richHit->getPhi()-300);

		if(s != 4) richHit->setMeanPhi(richHit->getMeanPhi()+60 );
                else       richHit->setMeanPhi(richHit->getMeanPhi()-300);


		// rotate sector by one
		if(s < 5) s+= 1;
                else      s = 0;
		richHit->setSector(s);


	    }
	} // loop richit
	// -------------------------------------------------------------------------


	return 0;
    }

    Bool_t finalize()
    {
	return kTRUE;
    }

};
#endif
