#ifndef HEMCTRB3LOOKUP_H
#define HEMCTRB3LOOKUP_H

#include "TObjArray.h"
#include "TObject.h"

#include "hparset.h"
#include "htrbnetdef.h"

using namespace Trbnet;
using namespace std;

// define meaning of flag bits here:
#define EMC_CHANNEL_FASTBIT         (0x1 << 0)
#define EMC_CHANNEL_SLOWBIT         (0x1 << 1)
#define EMC_CHANNEL_BROKENBIT       (0x1 << 7)

class HEmcTrb3LookupChan : public TObject {
protected:
    Int_t  sector;    // sector number
    Int_t  cell;      // cell number
    UChar_t flag;     // tdc channel mode flag
public:
    HEmcTrb3LookupChan(void)  {clear();}
    virtual ~HEmcTrb3LookupChan(void) {}
    Int_t   getSector()    { return sector;}
    Int_t   getCell()      { return cell;}
    UChar_t getFlag()	   { return flag;}
    Bool_t isSlowChannel() { return ((flag & EMC_CHANNEL_SLOWBIT) == EMC_CHANNEL_SLOWBIT); }
    Bool_t isFastChannel() { return ((flag & EMC_CHANNEL_FASTBIT) == EMC_CHANNEL_FASTBIT); }
    Bool_t isDefinedChannel() { return ( isFastChannel() || isSlowChannel());  }
    Bool_t isBrokenChannel()  { return ((flag & EMC_CHANNEL_BROKENBIT) == EMC_CHANNEL_BROKENBIT); }
    void   setSlowChannel  (Bool_t on) { on ? (flag |=EMC_CHANNEL_SLOWBIT)   : (flag &= ~EMC_CHANNEL_SLOWBIT); }
    void   setBrokenChannel(Bool_t on) { on ? (flag |=EMC_CHANNEL_BROKENBIT) : (flag &= ~EMC_CHANNEL_BROKENBIT); }
    void   setFastChannel(Bool_t on)   { on ? (flag |=EMC_CHANNEL_FASTBIT)   : (flag &= ~EMC_CHANNEL_FASTBIT); }

    void   getAddress(Int_t& s, Int_t& c) const { s = sector; c = cell; }
    void   fill(Int_t s, Int_t c, UChar_t f) { sector = s; cell = c; flag = f; }
    void   fill(HEmcTrb3LookupChan& r) { sector = r.getSector(); cell = r.getCell(); flag	= r.getFlag(); }
    void   setSector(const Int_t s) {sector = s;}
    void   setCell(const Int_t c)   {cell = c;}
    void   setFlag(const UChar_t f) {flag = f;}
    void   clear(void)              {sector = cell =  -1; flag=0;}
    ClassDef(HEmcTrb3LookupChan, 1) // Channel level of the lookup table for the EMC TRB3 unpacker
};


class HEmcTrb3LookupBoard: public TObject {
    friend class HEmcTrb3Lookup;
protected:
    TObjArray* array;   // pointer array containing HEmcTrb3LookupChan objects
public:
    HEmcTrb3LookupBoard(void);
    virtual ~HEmcTrb3LookupBoard(void);
    Int_t getSize()  { return array ? array->GetLast() + 1 : 0; }
    HEmcTrb3LookupChan* getChannel(Int_t c) { if (c >= 0 && c < getSize()) return &((*this)[c]); else return 0; }
    HEmcTrb3LookupChan& operator[](Int_t i) { return *static_cast<HEmcTrb3LookupChan*>((*array)[i]); }
    void clear(void);
    ClassDef(HEmcTrb3LookupBoard, 1) // Board level of  the lookup table for the EMC TRB3 unpacker
};


class HEmcTrb3Lookup : public HParSet {
protected:
    TObjArray* array;  // array of pointers of type HEmcTrb3LookupBoard
    Int_t arrayOffset; // offset to calculate the index
public:
    HEmcTrb3Lookup(const Char_t* name = "EmcTrb3Lookup",
		   const Char_t* title = "Lookup table for the TRB3 unpacker of the EMC detector",
		   const Char_t* context = "EmcTrb3LookupProduction",
		   Int_t minTrbnetAddress = Trbnet::kEmcTrb3MinTrbnetAddress,
		   Int_t maxTrbnetAddress = Trbnet::kEmcTrb3MaxTrbnetAddress);
    virtual ~HEmcTrb3Lookup(void);
    HEmcTrb3LookupBoard* getBoard(Int_t trbnetAddress) {
	if ((arrayOffset <= trbnetAddress) && (trbnetAddress <= (array->GetSize() + arrayOffset)))
	    return (HEmcTrb3LookupBoard*)(array->At(trbnetAddress - arrayOffset));
	return 0;
    }
    HEmcTrb3LookupBoard* operator[](Int_t i) { return static_cast<HEmcTrb3LookupBoard*>((*array)[i]); }
    Int_t  getSize(void) { return array->GetLast() + 1; }
    Int_t  getArrayOffset(void) { return arrayOffset;  }
    Bool_t init(HParIo* input, Int_t* set);
    Int_t  write(HParIo* output);
    void   clear(void);
    void   printParam(void);
    Bool_t fill(Int_t, Int_t, Int_t, Int_t, Int_t);
    Bool_t readline(const Char_t*);
    void   putAsciiHeader(TString&);
    void   write(fstream&);
    ClassDef(HEmcTrb3Lookup, 1) // Lookup table for the TRB3 unpacker of the EMC detector
};

#endif  /*!HEMCTRB3LOOKUP_H*/
