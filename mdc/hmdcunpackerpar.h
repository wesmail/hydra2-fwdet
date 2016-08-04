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
#ifndef HMDCUNPACKERPAR__H
#define HMDCUNPACKERPAR__H

#include "hparcond.h"

class HParIo;
class HParamList;
class TArrayI;

class HMdcUnpackerPar : public HParCond
{
protected:
    Int_t  decodeVersion; // Version of decodeding to be used
    Int_t  tdcMode;	  // Mode of the tdc triggering
    Int_t  doAddrCorr;	  // Use address correction tables
    Int_t  mapOutUnusedChannels;  // Enable mapping of unused channels

public:
    HMdcUnpackerPar(const char* name = "MdcUnpackerPar", const char* title = "Mdc unpacker lookuptable for data recovery", const char* context = "MdcUnpackerParProduction");
    ~HMdcUnpackerPar(void);

    Int_t  getDecodeVersion(void);
    Bool_t getParams(HParamList *l);
    Int_t  getTdcMode(void);
    Int_t  getDoAddrCorr(void);
	Int_t  getMapUnusedChannels(void);

    void   setDecodeVersion(Int_t val) {decodeVersion = val;};
    void   setTdcMode      (Int_t val) {tdcMode = val;};
    void   setDoAddrCorr   (Int_t val) {doAddrCorr = val;};
	void   setMapUnusedChannels(Int_t val){mapOutUnusedChannels = val;};

    Bool_t init(HParIo* inp,Int_t* set);
    void   printParam(void);
    void   putParams(HParamList *l);
    void   setParamContext(const char *context);
    Int_t  write(HParIo* output);

    ClassDef(HMdcUnpackerPar,1)
};

#endif //HMDCUNPACKERPAR__H
