//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : RICH team member
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichMappingPar
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHMAPPINGPAR_H
#define HRICHMAPPINGPAR_H

#include "hparset.h"
#include "richdef.h"

class HRichMappingPar : public HParSet {

private:

   Int_t fAddrArrHW[RICH_SIZE_HW];  // Array of HW addresses
   Int_t fAddrArrSW[RICH_SIZE_SW];  // Array of SW addresses


public:

   HRichMappingPar(const Char_t* name    = "RichMappingParameters",
                   const Char_t* title   = "Rich Mapping Parameters",
                   const Char_t* context = "RichMappingParProduction");
   ~HRichMappingPar() {}

   Bool_t setAddress(UInt_t row, UInt_t col, UInt_t adc, UInt_t apv, UInt_t ch);
   Bool_t readline(const Char_t* buf);
   Bool_t init(HParIo* input, Int_t* set);
   Int_t  write(HParIo* output);
   void   clear();
   void   putAsciiHeader(TString& header);
   void   write(std::fstream& fout);
   void   printParams();

   Bool_t isValidSWAddress(Int_t row, Int_t col);// is x,y of a pad (SW_address) connected with a valid channel
   Bool_t isValidHWAddress(Int_t addrHW);       // is a valid HW_address?
   Bool_t getHWAddress(Int_t row, Int_t col, UInt_t& adc, UInt_t& apv, UInt_t& ch);
   Bool_t getSWAddress(Int_t address, Int_t& row, Int_t& col);

   ClassDef(HRichMappingPar, 1) // Mapping parameters between the HW and SW address of a pad

};

#endif // HRICHMAPPINGPAR_H


