//*****************************************************************************
// File: $RCSfile: $
//
// Author: Joern Wuestenfeld
//
// Version: $Revision $
// Modified by $Author $
//
//
//*****************************************************************************
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
// HMdcOepAddrCorrPar
//
// Parameter container for the MDC unapcker.
// This container keeps all parameters that are needed for the unpacker of
// the MDC data to correct wrong OEP addresses.
//
//*****************************************************************************

#include "hmdcoepaddrcorrpar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"

#include <stdlib.h>
#include <iostream>
#include <iomanip>

HMdcOepAddrCorrPar::HMdcOepAddrCorrPar(const char* name, const char* title, const char* context)
    : HParCond(name,title,context)
{
    // default constructor
}
/*
HMdcOepAddrCorrPar::HMdcOepAddrCorrPar(HMdcOepAddrCorrPar &par)
{
    // copy constructor
}
*/
HMdcOepAddrCorrPar::~HMdcOepAddrCorrPar(void)
{
    //destructor
    address.clear();
}

Bool_t HMdcOepAddrCorrPar::checkAddress(Int_t *addr)
{
    // check if the passeda ddress is in the map, and return the corrected address then.
    // If address has been mapped, return true, else false.
    map<Int_t,Int_t>::iterator it = address.find(*addr);
    if (it != address.end())
    {
	*addr = (*it).second;
	return kTRUE;
    }
    else
	return kFALSE;
}

Bool_t HMdcOepAddrCorrPar::init(HParIo* inp,Int_t* set)
{
    // intitializes the container from an input
    HDetParIo* input=inp->getDetParIo("HCondParIo");
    if (input) {
	Bool_t rc = input->init(this,set);
	if (rc){
	    if(changed){
		address.clear();
		for(Int_t i = 0; i < AddressLookupMap.GetSize()/2; i++)
		{
		    address[AddressLookupMap[i*2+0]] = AddressLookupMap[i*2+1]; // set addFrom,addTo
		}
	    }
	    return rc;
	}
    }
    return kFALSE;
}


void HMdcOepAddrCorrPar::putParams(HParamList* l)
{
    // put parameters to param list
    putToArray();
    l->add("AddressLookupMap",AddressLookupMap);

}

Bool_t HMdcOepAddrCorrPar::getParams(HParamList* l)
{
    // Retrieve parameters from HParamList,
    if (!l) return kFALSE;
    
    if(! (l->fill("AddressLookupMap", &AddressLookupMap)   ))
    {
	return kFALSE;
    }
    address.clear();
    for(Int_t i = 0; i<AddressLookupMap.GetSize()/2; i++)
    {
	address.insert(make_pair(AddressLookupMap[i*2+0],AddressLookupMap[i*2+1])); // set addFrom,addTo
    }
    return kTRUE;
}

void HMdcOepAddrCorrPar::setMapping(Int_t mapFrom, Int_t mapTo)
{
    // add a pair of addresses, that should be mapped.
    map<Int_t,Int_t>::iterator it;

    it = address.find(mapFrom);
    if(it == address.end())
    {
	// new address
	address.insert(make_pair(mapFrom,mapTo));
    }
    else
    {
	// already existing address modified
	(*it).second = mapTo;
    }
}
void HMdcOepAddrCorrPar::putToArray(void)
{   // put all mapping pairs from map address
    // and put them into the TArrayI. This methode
    // should be used only if values should
    // be set from macro to create the container
    // content after the values have been added
    // to the map using setMapping()

    map<Int_t,Int_t>::iterator it;
    Int_t s,i = 0;
    s = address.size();

    AddressLookupMap.Reset();
    AddressLookupMap.Set(s*2);
    for(it= address.begin(); it!= address.end(); it++)
    {
	AddressLookupMap[i*2+0] = (*it).first;
	AddressLookupMap[i*2+1] = (*it).second;
	i++;
    }
}
void HMdcOepAddrCorrPar::printParam(void)
{
    print();
    cout<<"size="<<AddressLookupMap.GetSize()<<endl;

    for(Int_t i = 0; i < AddressLookupMap.GetSize()/2; i++)
    {
	cout<<setw(4)<<i<<" correction pair: 0x"<<hex<<AddressLookupMap[i*2+0]<<" ==> 0x"<<hex<<AddressLookupMap[i*2+1]<<dec<<endl;
    }
}

void HMdcOepAddrCorrPar::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcOepAddrCorrPar.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HParCond::Streamer(R__b);
      AddressLookupMap.Streamer(R__b);

      address.clear();
      for(Int_t i = 0; i<AddressLookupMap.GetSize()/2; i++)
      {
	  address.insert(make_pair(AddressLookupMap[i*2+0],AddressLookupMap[i*2+1])); // set addFrom,addTo
      }

      R__b.CheckByteCount(R__s, R__c, HMdcOepAddrCorrPar::IsA());
   } else {
      R__c = R__b.WriteVersion(HMdcOepAddrCorrPar::IsA(), kTRUE);
      HParCond::Streamer(R__b);
      AddressLookupMap.Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}



