//*-- AUTHOR : J.Wuestenfeld
//*-- Modified : 02/18/2005 by J. Wuestenfeld

////////////////////////////////////////////////////////////////////////////
// HMdcTdcChannel
//
// Container class for the TDC channle masks of the MDC
//
////////////////////////////////////////////////////////////////////////////
#ifndef HMDCTDCCHANNEL__H
#define HMDCTDCCHANNEL__H

#include "TObject.h"
#include "TObjArray.h"
#include "TString.h"
#include "hparset.h"

class HMdcRawStruct;
class HMdcLookupGeom;

class HMdcTdcChannelTdc : public TObject {
protected:
    Int_t channelMask[2];   // TDC channel mask (index 1: channel mask, 0: callibration mask)
public:
    HMdcTdcChannelTdc(){channelMask[0] = 0x0;channelMask[1] = 0x0;}
    ~HMdcTdcChannelTdc() {;}
    Int_t getChannelMask(Int_t type)  {return channelMask[type];}
    void fill(Int_t mask,Int_t type){
      // fill mask for given type
      channelMask[type]=mask;
    }
    void fill(HMdcTdcChannelTdc&);
    void setChannelMask(Int_t mask,Int_t type)  {channelMask[type]=mask;}
    void clear() {
      // resets contents
      channelMask[0] = -1;
      channelMask[1] = -1;
    }
    ClassDef(HMdcTdcChannelTdc,1) // Tdc level of the MDC TDC channel mask
};


class HMdcTdcChannelMbo : public TNamed {
protected:
    TObjArray *array; // array of pointers of type HMdcTdcChannelTdc
public:
    HMdcTdcChannelMbo(Int_t tdc = 12, const Text_t* name="");
    ~HMdcTdcChannelMbo();
    HMdcTdcChannelTdc& operator[](Int_t i) {
			// access to array
      return *static_cast<HMdcTdcChannelTdc*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcTdcChannelMbo,1) // Mbo level of the MDC TDC channel mask
};

class HMdcTdcChannelMod : public TObject {
protected:
    TObjArray *array;   // array of pointers of type HMdcTdcChanneldMbo
public:
    HMdcTdcChannelMod(Int_t mbo = 16);
    ~HMdcTdcChannelMod();
    HMdcTdcChannelMbo& operator[](Int_t i) {
			//access to array
      return *static_cast<HMdcTdcChannelMbo*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    void createMbo(Int_t, Int_t, const Char_t*);
    ClassDef(HMdcTdcChannelMod,1) // Module level of the MDC TDC channel mask
};

class HMdcTdcChannelSec : public TObject {
protected:
    TObjArray* array;   // array of pointers of type HMdcTdcChannel Mod

public:
    HMdcTdcChannelSec(Int_t mod = 4);
    ~HMdcTdcChannelSec();
    HMdcTdcChannelMod& operator[](Int_t i) {
			// access to array
      return *static_cast<HMdcTdcChannelMod*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcTdcChannelSec,1) // Sector level of the MDC TDC channel mask
};

class HMdcTdcChannel : public HParSet {
protected:
   TObjArray* array;                // array of pointers of type HMdcTdcChannelSec
   Int_t oraVersion;                // parameter version in Oracle
   Int_t status;                    // status of the parameters
   HMdcRawStruct* pRawStruct;       //! Internaly needed to setup the container structure
   HMdcLookupGeom* pLookupGeom;     //! Internally needed to map out not connected channels
   Bool_t mapNotConnectedChannels;  // Map out channels that have no wire connected

public:
   HMdcTdcChannel(const Char_t* name="MdcTdcChannel",
                  const Char_t* title="channel masks for Mdc TDC",
                  const Char_t* context="MdcTdcChannelProduction",
                  Int_t n=6);
   ~HMdcTdcChannel();
   HMdcTdcChannelSec& operator[](Int_t i) {
      return *static_cast<HMdcTdcChannelSec*>((*array)[i]);
   }
   void fill(Int_t sector, Int_t module, Int_t mbo, Int_t dbo,Int_t th) {
      // fills mask into specified HMdcTdcChannelTdc object
      ((HMdcTdcChannelSec *)((*array)[0]))[sector][module][mbo][dbo].fill(th,0);
   }
   Int_t getSize() {return array->GetEntries();}
   Int_t getOraVersion() { return oraVersion; }
   Int_t getStatus(void){return status;};
   Bool_t getMapNotConnetedChannels(void) {return mapNotConnectedChannels;};
   void setOraVersion(Int_t v) { oraVersion=v;}
   void setStatus(Int_t v){status = v;};
   void setMapNotConnectedChannels(Bool_t s=kFALSE) {mapNotConnectedChannels=s;};
   void clear();
   Bool_t init(HParIo*, Int_t*);
   void putAsciiHeader(TString&);
   void readline(const Char_t*, Int_t*);
   void readLineFromFile(const Char_t*);
   Int_t write(HParIo*);
   Bool_t writeline(Char_t*, Int_t, Int_t, Int_t, Int_t);
   void mapOutUnusedChannels(void);
   void printParam();
   ClassDef(HMdcTdcChannel,2) // Container for the MDC TDC channel masks
};

#endif  /*!HMDCTDCCHANNEL__H*/

