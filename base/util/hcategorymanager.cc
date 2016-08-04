#include "hcategorymanager.h"
#include "hlinearcategory.h"

// ROOT's IO and RTTI stuff is added here
ClassImp(HCategoryManager)

    //_HADES_CLASS_DESCRIPTION
    ////////////////////////////////////////////////////////////////////////////////
    //
    //
    // HCategoryManager
    //
    // Helper object providing static template functions to create/retrieve objects from
    // LiniearCategories in an defined way. Functions to retieve/create LinearCategories
    // are provieded too.
    ////////////////////////////////////////////////////////////////////////////////

    HLocation HCategoryManager::loc;

HCategory * HCategoryManager::getCategory(Short_t num,Int_t warn,TString name)
{
    // retrieve Category with num num from current event. name is the name of
    // the category used to display in Error messages. If warn is KTRUE and
    // category can not be retrieved only an warning is emmitted.

    HCategory *cat = 0;
    if (name.CompareTo ("") == 0) { name += num; }
    HRecEvent *ev = (HRecEvent*) gHades->getCurrentEvent ();
    if (ev) {
	cat = ev->getCategory(num);
	if (!cat) {
	    if(warn == 0)      {::Warning ("getCategory()", "Cannot get category %s", name.Data());}
	    else if (warn == 1){::Error   ("getCategory()", "Cannot get category %s", name.Data());}
	}
    }
    return cat;

}

HCategory * HCategoryManager::addCategory(Short_t num,TString name,Int_t size,TString partevt, Bool_t dynamicsize)
{
    // add LinearCategory with number num and size size for object type name
    // to partial event partevt. If dynamicsize == KTRUE, objects with variable
    // size can be stored and the category will be cleared in each event.

    HCategory *cat = 0;
    if (name.CompareTo ("") == 0) {
	::Error ("addCategory()", "Category name is empty !");
	return cat;
    }
    HRecEvent *ev = (HRecEvent*) gHades->getCurrentEvent ();
    if (ev) {
	cat = ev->getCategory(num);
	if (!cat) {
	    cat = new HLinearCategory (name.Data(),size);
	    if(dynamicsize) ((HLinearCategory*)cat)->setDynamicObjects(kTRUE);
	    ev->addCategory(num,cat,partevt.Data());
	} else {
	    ::Error ("addCategory()", "cat for %s exists already! Fix this!",name.Data());
	}
    } else {
	::Error ("addCategory()", "ZERO pointer retrieved for event !");
    }
    return cat;

}
