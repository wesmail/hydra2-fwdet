#ifndef HRICH700TRB3LOOKUP_H
#define HRICH700TRB3LOOKUP_H

#include "TObjArray.h"
#include "TObject.h"

#include "hparset.h"
#include "htrbnetdef.h"

using namespace Trbnet;
using namespace std;


// number of channels per dirich tdc. Note that channel 0 is special? so we need range 0 up to 32 inclusive
#define HRICH700_MAXTDCCHANNELS 33

/**
 * trb3 lookup classes for rich700. Adopted from lookup of start2
 * JAM (j.adamczewski@gsi.de) 8-Jun-2017 revised 24-Oct-2017
 * */


class HRich700Trb3LookupChan : public TObject {
protected:
   Int_t  fPMT;    	        //  Global ID of the RIÖCH700 PMT
   Int_t  fPixel;          // 	Local pixel number
public:
   HRich700Trb3LookupChan()     {
      clear();
   }
   virtual ~HRich700Trb3LookupChan()    {
      ;
   }
   Int_t getPMT()    {
      return fPMT;
   }
   Int_t getPixel()    {
        return fPixel;
     }


   void getAddress(Int_t& pmt, Int_t& pix) {
      pmt = fPMT;
      pix = fPixel;
   }
   void fill(Int_t pmt, Int_t pix) {
	   fPMT = pmt;
	   fPixel = pix;
   }
   void fill(HRich700Trb3LookupChan& r) {
	   fPMT = r.getPMT();
	   fPixel = r.getPixel();
   }
   void setPMT(const Int_t pmt)    {
	   fPMT = pmt;
   }
   void setPixel(const Int_t pix)    {
   	   fPixel = pix;
      }

   void clear() {
      fPMT = -1;
      fPixel = -1; //
   }
   ClassDef(HRich700Trb3LookupChan, 1) // Channel level of the lookup table for the RICH700 TRB3 unpacker
};


class HRich700Trb3LookupTdc: public TObject {
   friend class HRich700Trb3Lookup;
protected:
   TObjArray* array;     // pointer array containing HRich700Trb3LookupChan objects
public:
   HRich700Trb3LookupTdc();
   virtual ~HRich700Trb3LookupTdc();
   Int_t getSize()  {
      return array ? array->GetLast()+1 : 0;
   }
   HRich700Trb3LookupChan* getChannel(Int_t c) {
      if (c >= 0 && c < getSize()) return &((*this)[c]);
      else return 0;
   }
   HRich700Trb3LookupChan& operator[](Int_t i) {
      return *static_cast<HRich700Trb3LookupChan*>((*array)[i]);
   }
   void clear();
   ClassDef(HRich700Trb3LookupTdc, 1) // Tdc level of  the lookup table for the RICH700 TRB3 unpacker
};


class HRich700Trb3Lookup : public HParSet {
protected:
   TObjArray* array;  // array of pointers of type HRich700Trb3LookupTdc
   Int_t arrayOffset; // offset to calculate the index
   Int_t arrayCursor; // scan exisiting subevent ids with this
public:
   HRich700Trb3Lookup(const Char_t* name = "Rich700Trb3Lookup",
                     const Char_t* title = "Lookup table for the TRB3 unpacker of the RICH700 detector",
                     const Char_t* context = "Rich700Trb3LookupProduction",
                     Int_t minTrbnetAddress = Trbnet::kRICHTrb3MinTrbnetAddress,
                     Int_t maxTrbnetAddress = Trbnet::kRICHTrb3MaxTrbnetAddress);
   virtual ~HRich700Trb3Lookup();
   HRich700Trb3LookupTdc* getTdc(Int_t trbnetAddress) {
	   if(trbnetAddress ==0) return 0;
      return (HRich700Trb3LookupTdc*)(array->At(trbnetAddress - arrayOffset));
   }
   HRich700Trb3LookupTdc* operator[](Int_t i) {
      return static_cast<HRich700Trb3LookupTdc*>((*array)[i]);
   }
   Int_t getSize() {
      return array->GetLast() + 1;
   }
   Int_t getArrayOffset() {
      return arrayOffset;
   }

  void resetTdcIterator();
  Int_t getNextTdcAddress();


   Bool_t init(HParIo* input, Int_t* set);
   Int_t write(HParIo* output);
   void clear();
   void printParam();
   Bool_t fill(Int_t, Int_t, Int_t, Int_t);
   Bool_t readline(const Char_t*);
   void putAsciiHeader(TString&);
   void write(fstream&);
   ClassDef(HRich700Trb3Lookup, 1) // Lookup table for the TRB3unpacker of the RICH700 detector
};

#endif  /*!HRich700Trb3Lookup_H*/
