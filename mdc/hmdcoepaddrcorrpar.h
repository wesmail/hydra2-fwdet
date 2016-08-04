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
#ifndef HMDCOEPADDCORRPAR__H
#define HMDCOEPADDCORRPAR__H

#include "hparcond.h"
#include <map>

using namespace std;

class HMdcOepAddrCorrPar : public HParCond
{
protected:
    map<Int_t, Int_t> address;	       //! Map wrong addresses of OEP's to correct ones e.g. 2001 -> 2003
    TArrayI AddressLookupMap;	       //  Map wrong addresses of OEP's to correct ones e.g. 2001 -> 2003
public:
    HMdcOepAddrCorrPar(const char* name = "MdcOepAddrCorrPar", const char* title = "Mdc unpacker lookuptable for data recovery", const char* context = "MdcOepAddrCorrParProduction");
    //HMdcOepAddrCorrPar(HMdcOepAddrCorrPar &par);
    ~HMdcOepAddrCorrPar(void);

    Int_t  getSize(void){ return address.size(); }
    Bool_t checkAddress(Int_t *addr);
    Bool_t init(HParIo* inp,Int_t* set);

    void   setMapping(Int_t adress, Int_t mapTo); // set mapping pairs to map
    void   putToArray(void);                      // put Values from map To TArrayI

    void   putParams(HParamList* l);
    Bool_t getParams(HParamList* l);
    void   printParam(void);
    //void   Streamer(TBuffer &R__b);

    ClassDef(HMdcOepAddrCorrPar,1)
};

#endif
