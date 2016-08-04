// File: showerunpacker.h
//
// Author: Leszek Kidon
// Last update: 25/5/2000
//

#ifndef HShowerUnpacker_H
#define HShowerUnpacker_H

#include "haddef.h"
#include "hldsubevt.h"
#include "hldunpack.h"
#include "hlocation.h"
#include "showerdef.h"

//default unpacker 

class HShowerRawHist;

class HShowerUnpacker: public HldUnpack {



public:

  HShowerUnpacker(Int_t nSubId);
  ~HShowerUnpacker();

  virtual Int_t getSubEvtId() const { return m_nSubId; }

  Int_t execute();
  Bool_t init(void);
  Bool_t reinit(void);
  Bool_t finalize(void);

private:

  Int_t fillData(class HLocation &loc, Int_t nCharge, Int_t nEvtId);

  Int_t dump();

  HLocation m_loc; //!Location of object to be unpacked.
  HLocation m_zeroLoc; //!


   class SHeader {

      Bool_t fEmpty;
      UInt_t fSize;
      UInt_t fSource;

      public:

      SHeader() { reset(); }
      ~SHeader() {}

      void reset() { fSize   = 0; fSource = 0; }
      void getEvent(UInt_t word) { fSize   = (word >> 16) & 0xFFFF; fSource = word & 0xFFFF; }

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

   struct DataWord {

      UInt_t sector;
      UInt_t module;
      UInt_t column;
      UInt_t row;
      UInt_t charge;

   } fDataWord;       

  Int_t m_nSubId;

  UInt_t  fEventNr;              //! current event #

  HShowerRawHist* m_pHist;
  ClassDef(HShowerUnpacker,0)		// unpack Shower data
};



#endif /* !HShowerUnpacker_H */



