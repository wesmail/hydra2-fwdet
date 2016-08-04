#ifndef __HCATEGORYMANAGER_H__
#define __HCATEGORYMANAGER_H__



#include "hades.h"
#include "hrecevent.h"
#include "hcategory.h"
#include "hlocation.h"

#include "TObject.h"
#include "TString.h"


class HCategoryManager: public TObject
{
private:
    static HLocation loc; //!

public:
    HCategoryManager ()  {;}
    ~HCategoryManager () {;}

    static HCategory *getCategory(Short_t num,Int_t warn = 0, TString name = "");
    static HCategory *addCategory(Short_t num,TString name = "",Int_t size = 1000,TString partev = "", Bool_t dynamicsize = kFALSE);

    template<class T> static T* getObject(T* pout,Short_t num = -1, Int_t index = -1,Bool_t silent=kFALSE)
    {
	// returns object with index index from category with number num.
        // In error case return 0.

	HCategory *cat = 0;
	pout = 0;
	if (index < 0 || num < 0) {  return pout; }
	HRecEvent *ev = (HRecEvent*) gHades->getCurrentEvent ();
	if (ev) {
	    cat = ev->getCategory(num);
	    if (!cat) {
		if(!silent) ::Error("getObject()", "Cannot get category cat num = %i", num);
		return pout;
	    } else {
		pout = (T*) cat->getObject(index);
		if (!pout) {
		    if(!silent) ::Error("getObject()", "ZERO pointer retrieved for object!");
		    return pout;
		}
	    }
	} else {
	    ::Error("getObject()", "ZERO pointer retrieved for event!");
	    return pout;
	}
	return pout;
    }

    template<class T> static T* getObject(T* pout,HCategory* cat = 0, Int_t index = -1,Bool_t silent=kFALSE)
    {
	// returns object with index index from category cat.
        // In error case return 0.
	pout = 0;
	if (index < 0 || cat == 0) {  return pout; }
	pout = (T*) cat->getObject(index);
	if (!pout) {
	    if(!silent) ::Error("getObject()", "ZERO pointer retrieved for object!");
	    return pout;
	}
	return pout;
    }

    template<class T> static T* newObject(T* pout,Short_t num , Int_t& index)
    {
	// returns new object with index index from category with number num.
        // In error case return 0.
	HCategory *cat = 0;
	pout = 0;
	if (num < 0) {  return pout; }
	HRecEvent *ev = (HRecEvent*) gHades->getCurrentEvent ();
	if (ev) {
	    cat = ev->getCategory(num);
	    if (!cat) {
		::Error("newObject()", "Cannot get category cat num = %i", num);
		return pout;
	    } else {
		pout = (T*) cat->getNewSlot(loc,&index);
		if (!pout) {
		    ::Error("newObject()", "ZERO pointer retrieved for object!");
		    return pout;
		}
		pout = new (pout) T;
	    }
	} else {
	    ::Error("newObject()", "ZERO pointer retrieved for event!");
	    return pout;
	}
	return pout;
    }

    template<class T> static T* newObject(T* pout,HCategory* cat, Int_t& index)
    {
	// returns new object with index index from category cat.
        // In error case return 0.
	pout = 0;
	if (cat == 0) {  return pout; }

	pout = (T*) cat->getNewSlot(loc,&index);
	if (!pout) {
	    ::Error("newObject()", "ZERO pointer retrieved for object!");
	    return pout;
	}
	pout = new (pout) T;

	return pout;
    }

    template<class T> static T* newObject(T* pout,const T* pin,Short_t num , Int_t& index)
    {
	// returns new object with index index from category with number num.
        // the new object is created via copy constructor
	// In error case return 0.
	HCategory *cat = 0;
	pout = 0;
	if (num < 0) {  return pout; }
	HRecEvent *ev = (HRecEvent*) gHades->getCurrentEvent ();
	if (ev) {
	    cat = ev->getCategory(num);
	    if (!cat) {
		::Error("newObject()", "Cannot get category cat num = %i", num);
		return pout;
	    } else {
		pout = (T*) cat->getNewSlot(loc,&index);
		if (!pout) {
		    ::Error("newObject()", "ZERO pointer retrieved for object!");
		    return pout;
		}
		pout = new (pout) T(*pin);
	    }
	} else {
	    ::Error("newObject()", "ZERO pointer retrieved for event!");
	    return pout;
	}
	return pout;
    }

    template<class T> static T* newObjectCopy(T* pout,T* pin,HCategory* cat, Int_t& index)
    {
	// returns new object with index index from category cat.
        // created via copy contructor!
	// In error case return 0.
	pout = 0;
	if (cat == 0) {  return pout; }

	pout = (T*) cat->getNewSlot(loc,&index);
	if (!pout) {
	    ::Error("newObject()", "ZERO pointer retrieved for object!");
	    return pout;
	}
	pout = new (pout) T(*pin);

	return pout;
    }


    template<class T> static T* newSlot(T* pout,Short_t num , Int_t& index)
    {
	// returns new object pointer (no object yet) with index index from category with number num.
        // In error case return 0.
	HCategory *cat = 0;
	pout = 0;
	if (num < 0) {  return pout; }
	HRecEvent *ev = (HRecEvent*) gHades->getCurrentEvent ();
	if (ev) {
	    cat = ev->getCategory(num);
	    if (!cat) {
		::Error("getObject()", "Cannot get category cat num = %i", num);
		return pout;
	    } else {
		pout = (T*) cat->getNewSlot(loc,&index);
		if (!pout) {
		    ::Error("getObject()", "ZERO pointer retrieved for object!");
		    return pout;
		}
		return pout;
	    }
	}
    }

    template<class T> static T* newSlot(T* pout,HCategory* cat , Int_t& index)
    {
	// returns new object pointer (no object yet) with index index from category cat.
        // In error case return 0.
	pout = 0;
	if (cat == 0) {  return pout; }
	pout = (T*) cat->getNewSlot(loc,&index);
	if (!pout) {
	    ::Error("newSlot()", "ZERO pointer retrieved for object!");
	    return pout;
	}
	return pout;
    }

    ClassDef(HCategoryManager, 0)	// A simple helper object
};


#endif // __HCATEGORYMANAGER_H__
