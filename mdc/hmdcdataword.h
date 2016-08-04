///////////////////////////////////////////////////////////
// File: $RCSfile: $
//
// Author : J.Wuestenfeld
// Version: $Revision: $
///////////////////////////////////////////////////////////
#ifndef HMDCDATAWORD__H
#define HMDCDATAWORD__H

#include "TObject.h"

class HMdcDataword : public TObject
{
protected:
    enum {m1=0x1, m3=0x7, m4=0xf, m5 = 0x1f, m11=0x7ff, m16 = 0xffff, m24 = 0xffffff, m26 = 0x3ffffff, m29=0xFFFFFFF, m32 = 0xffffffff};

    // Event header information
    Int_t decodeType;			// Type of decoding
    Int_t eventNumber;	 // Number of event
    Int_t triggerType;	 // Type of trigger (1 = normal, 9 = calibration)
    Int_t errorFlag;	 // Status information from subevent
    Int_t subEventSize;	 // Number of datawords in subevent
    
    Int_t statusCode;			// Code of Statusword, see doc.
    Int_t statusData;			// See doc.

    // MDC - data - word
    Int_t mboAddress;	 // Adress information of motherboard
    Int_t tdcNumber;	 // Number of TDC chip on MBO
    Int_t channel;	 // Number of channel in TDC
    Int_t hit;		 // Number of hit in TDC (0 or 1)
    Int_t time;		 // Measured time in channels (~500ps)

    // Address
    Int_t sector;	 // MdcRaw container coordinate: sector
    Int_t module;	 // MdcRaw container coordinate: module
    Int_t mbo;		 // MdcRaw container coordinate: mbo
    Int_t tdc;		 // MdcRaw container coordinate: tdc (tdc * 8 + channel)

    Int_t entry;	 // Number of entry
    Bool_t noComment;	 //! Switches of all comments

    Int_t time1;	 // second hit of channel
    Bool_t debug;        // Enable debugging of dataword

public:
    HMdcDataword(Bool_t comment);
    HMdcDataword(void);
    ~HMdcDataword(void){;};

    Bool_t checkConsistency(UInt_t version = 0, UInt_t data = 0);

    void   clearData   (void);
    void   clearAll    (void){clearData();clearHeader();clearAddress();};
    void   clearAddress(void){sector = module = mbo = tdc = 0;};
    void   clearHeader (void){decodeType=eventNumber=triggerType=errorFlag=subEventSize=-1;};

    Bool_t decode(const UInt_t data, Bool_t consistencyCheck = kFALSE);
    void   dump(void);

    // Get functions for member variables
    Int_t  getDecodeType  (void){return decodeType;};
    Int_t  getEventNumber (void){return eventNumber;};
    Int_t  getTriggerType (void){return triggerType;};
    Int_t  getErrorFlag   (void){return errorFlag;};
    Int_t  getModule      (void){return module;}
    Int_t  getSector      (void){return sector;};
    Int_t  getSubEventSize(void){return subEventSize;};

    Int_t  getAddress   (void);
    Int_t  getMboAddress(void){return mboAddress;};
    Int_t  getTdcNumber (void){return tdcNumber;};
    Int_t  getChannel   (void){return channel;};
    Int_t  getHit       (void){return hit;};
    Int_t  getTime      (void){return time;};
		Int_t  getTime1     (void){return time1;};

    Int_t getStatusCode(void){return statusCode;};
    Int_t getStatusData(void){return statusData;};
    
    UInt_t getCodedDataword(UInt_t version = 0);

    void   print(UInt_t version);

    // Set functions for member variables
    void   setAddress(Int_t s, Int_t m, Int_t mb, Int_t t, Int_t e){sector=s;module=m;mbo=mb,tdc=t;entry=e;};

    void   setEventNumber (Int_t val){eventNumber = val;};
    void   setTriggerType (Int_t val){triggerType = val;};
    void   setErrorFlag   (Int_t val){errorFlag = val;};
    void   setSubEventSize(Int_t val){subEventSize = val;};

    void   setAddress   (Int_t addr);
    void   setMboAddress(Int_t val){mboAddress = val;};
    void   setTdcNumber (Int_t val){tdcNumber = val;};
    void   setChannel   (Int_t val){channel = val;};
    void   setHit       (Int_t val){hit = val;};
    void   setTime      (Int_t val){time = val;};
		void   setTime1     (Int_t val){time1 = val;};

    void   setQuietMode(Bool_t b=kTRUE){noComment = b;};

    Bool_t subHeader(const UInt_t *data, UInt_t version, Bool_t consistencyCheck = kFALSE);

    ClassDef(HMdcDataword,1)				// MDC dataford for OEP (Optical End Point)
};

#endif
