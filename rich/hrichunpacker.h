//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Martin Jurkovic <martin.jurkovic@ph.tum.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichUnpacker
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRichUnpacker_H
#define HRichUnpacker_H

#include "hldunpack.h"

class HCategory;
class HRichMappingPar;


class HRichUnpacker : public HldUnpack {

protected:

   static const UInt_t kHubDebugId;  //! Identification of debug "subsubevent"
   static const UInt_t kADCZero;     //! ADC module ZERO offset

   UInt_t  fRichId;               //! current event #
   UInt_t  fEventNr;              //! current event #
   UInt_t  fStartEvt;             //! start unpacking with event #
   UInt_t  fSecMisMatchCntr;      //!

   struct DataWord {
      UInt_t sector;
      UInt_t adc;
      UInt_t apv;
      UInt_t channel;
      UInt_t charge;
   } fDataWord;                    //! structure for channel address

   HCategory*        fpCalCat;               //!
   HRichMappingPar*  fpMapPar;               //!


private:

   void printDataWord(const DataWord& addr);
   void printMapping(const DataWord& addr);

   inline Int_t getAddress(const DataWord& addr) const;

public:
   HRichUnpacker(Int_t  richId,
                 Int_t  strtEvt = 0);
   ~HRichUnpacker() {}

   Bool_t init(void);
   Int_t  execute(void);
   Bool_t finalize();

   inline Int_t getSubEvtId() const;

   ClassDef(HRichUnpacker, 0)       // Unpack RICH data
};


inline Int_t
HRichUnpacker::getAddress(const DataWord& addr) const
{
   return static_cast<Int_t>((addr.adc << 11) + (addr.apv << 7) + addr.channel);
}

inline Int_t
HRichUnpacker::getSubEvtId() const
{
   return fRichId;
}

class DHeader {

private:
   Bool_t fEmpty;
   UInt_t fSize;
   UInt_t fSource;

public:
   DHeader();
   ~DHeader() {}

   void reset();
   void getEvent(UInt_t word);
   void dump();

   Bool_t  empty()  const {
      return fEmpty;
   }
   UInt_t getSize()   const {
      return fSize;
   }
   UInt_t getSource() const {
      return fSource;
   }

};

#endif /* !HRichUnpacker */

