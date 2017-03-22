//////////////////////////////////////////////////////////////////////////////
//
// @(#)hydraTrans/richNew:$Id: $
//
//*-- Author  : RICH team member
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichMappingPar
//
// This class provides mapping between the HW addresses (ADC module,
// APV board, CHANNEL number in the electronics) to the SW address
// (COLUMN and ROW number on the padplane).
//
// HW_ADDRESS (calculated the same way as in the electronics):
//     (ADC << 11) + (APV << 7) + CHANNEL
//
// SW_ADDRESS:
//     (COL * 100) + ROW
//
//////////////////////////////////////////////////////////////////////////////


#include "hdetpario.h"
#include "hpario.h"
#include "hrichmappingpar.h"

#include <fstream>

using namespace std;

ClassImp(HRichMappingPar)

HRichMappingPar::HRichMappingPar(const Char_t* name,
                                 const Char_t* title,
                                 const Char_t* context)
   : HParSet(name, title, context)
{
   clear();
}

void HRichMappingPar::clear()
{
// Claar the address arrays

   for (Int_t i = 0; i < RICH_SIZE_HW; ++i) {
      fAddrArrHW[i] = -1;
   }
   for (Int_t i = 0; i < RICH_SIZE_SW; ++i) {
      fAddrArrSW[i] = -1;
   }

   status = kFALSE;
   resetInputVersions();
}

Bool_t HRichMappingPar::init(HParIo* inp, Int_t* set)
{
// Initializes the container from an input

   HDetParIo* input = inp->getDetParIo("HRichParIo");
   if (NULL != input) {
      Bool_t returnValue = input->init(this, set);
#if DEBUG_LEVEL > 3
      printParams();
#endif
      return returnValue;
   }
   return kFALSE;
}

Int_t HRichMappingPar::write(HParIo* output)
{
// Writes the container to an output

   HDetParIo* out = output->getDetParIo("HRichParIo");
   if (NULL != out) {
      return out->write(this);
   }
   return -1;
}

Bool_t HRichMappingPar::readline(const Char_t* buf)
{
// Decodes one line read from ASCII file I/O and fills the channel

   UInt_t col = 0;
   UInt_t row = 0;
   UInt_t adc = 0;
   UInt_t apv = 0;
   UInt_t ch  = 0;

   sscanf(buf, "%u %u %u %u %u", &col, &row, &adc, &apv, &ch);
   return setAddress(row, col, adc, apv, ch);
}

void HRichMappingPar::write(fstream& fout)
{
// Writes the Rich Mapping Parameters to an ASCII file.

   Int_t addrHW = -1;

   for (Int_t i = 0; i < RICH_SIZE_SW; ++i) {
      if (-1 != (addrHW = fAddrArrSW[i])) {
         fout.width(4); // col
         fout << left << (i / 100);
         fout.width(4); // row
         fout << left << (i % 100);
         fout.width(4); // ADC
         fout << left << ((addrHW >> 11) & RICH_ADC_MASK);
         fout.width(4); // APV
         fout << left << ((addrHW >> 7)  & RICH_APV_MASK);
         fout.width(4); // CHANNEL
         fout << left << ((addrHW >> 0)  & RICH_CHANNEL_MASK);
         fout << endl;
      }
   }
}

Bool_t HRichMappingPar::setAddress(UInt_t row, UInt_t col, UInt_t adc, UInt_t apv, UInt_t ch)
{
// Checks the adresses and fills the array

   if (row >= RICH700_MAX_ROWS || col >= RICH700_MAX_COLS) {
      Error("setAddress", "Wrong SW pad coordinates: row %u out of (0, %i) \n\t\t\t col %u out of (0,%i)",
            row, RICH700_MAX_ROWS, col, RICH700_MAX_COLS);
      return kFALSE;
   }
   if (adc >= RICH_MAX_ADCS || apv >= RICH_MAX_APVS ||
       ch >= RICH_MAX_CHANNELS || 1 == (ch % 2)) {
      Error("setAddress",
            "Wrong HW pad coordinates: adc %u out of (0,%i) \n\t\t\t apv %u out of (0,%i) \n\t\t\t channel %u out of (0,%i)",
            adc, RICH_MAX_ADCS, apv, RICH_MAX_APVS, ch, RICH_MAX_CHANNELS);
      return kFALSE;
   }

   Int_t addrHW = static_cast<Int_t>((adc << 11) + (apv << 7) + ch);
   Int_t addrSW = static_cast<Int_t>(col * 100 + row);

   if (0 <= addrHW && addrHW < RICH_SIZE_HW) {
      fAddrArrHW[addrHW] = addrSW;
   } else {
      Error("setAdress", "Wrong HW address %i (max %i)", addrHW, RICH_SIZE_HW);
      Error("setAdress", "adc: %i, apv %i, ch: %i", adc, apv, ch);
      return kFALSE;
   }

   if (0 <= addrSW && addrSW < RICH_SIZE_SW) {
      fAddrArrSW[addrSW] = addrHW;
   } else {
      Error("setAdress", "Wrong SW address %i (max %i)", addrSW, RICH_SIZE_SW);
      return kFALSE;
   }

   return kTRUE;
}

void HRichMappingPar::printParams()
{
// Prints the lookup table

   Int_t addrHW = -1;

   cout << endl;
   cout << "#########################################################################" << endl;
   cout << "[RichMappingParameters]" << endl;
   cout << "/ Format:" << endl;
   cout << "/ Col Row ADC APV Channel" << endl << endl;
   for (Int_t i = 0; i < RICH_SIZE_SW; ++i) {
      if (-1 != (addrHW = fAddrArrSW[i])) {
         cout.width(4); // col
         cout << left << (i / 100);
         cout.width(4); // row
         cout << left << (i % 100);
         cout.width(4); // ADC
         cout << left << ((addrHW >> 11) & RICH_ADC_MASK);
         cout.width(4); // APV
         cout << left << ((addrHW >> 7)  & RICH_APV_MASK);
         cout.width(4); // CHANNEL
         cout << left << ((addrHW >> 0)  & RICH_CHANNEL_MASK);
         cout << endl;
      }
   }
   cout << "#########################################################################" << endl << endl;
}

void HRichMappingPar::putAsciiHeader(TString& header)
{
   // puts the ASCII header to the string used in HDetParAsciiFileIo
   header =
      "# Rich mapping parameters for unpacker\n"
      "# Format:\n"
      "# Col Row ADC APV Channel\n";
}

Bool_t HRichMappingPar::getHWAddress(Int_t row, Int_t col,
                              UInt_t& adc, UInt_t& apv, UInt_t& ch)
{
   Int_t addrSW = col * 100 + row;
   Int_t addrHW = -1;

   if (addrSW < 0 || addrSW >= RICH_SIZE_SW) {
      Error("getHWAddress", "Address %i out of bounds (0,%i)", addrSW, RICH_SIZE_SW);
      return kFALSE;
   }
   if (-1 != (addrHW = fAddrArrSW[addrSW])) {
      adc = static_cast<UInt_t>((addrHW >> 11) & RICH_ADC_MASK);
      apv = static_cast<UInt_t>((addrHW >> 7)  & RICH_APV_MASK);
      ch  = static_cast<UInt_t>((addrHW >> 0)  & RICH_CHANNEL_MASK);
      return kTRUE;
   } else {
      return kFALSE;
   }
}

Bool_t HRichMappingPar::getSWAddress(Int_t addrHW,
                              Int_t& row, Int_t& col)
{
   Int_t addrSW = -1;

   row = -1;
   col = -1;

   if (addrHW < 0 || addrHW >= RICH_SIZE_HW) {
      Error("getSWAddress", "Address %i out of bounds (0,%i)", addrHW, RICH_SIZE_HW);
      return kFALSE;
   }
   if (-1 != (addrSW = fAddrArrHW[addrHW])) {
      col = (addrSW / 100);
      row = (addrSW % 100);
      return kTRUE;
   } else {
      return kFALSE;
   }
}

Bool_t HRichMappingPar::isValidSWAddress(Int_t row, Int_t col)
{
   if (row < 0 || col < 0 || (col * 100 + row) >= RICH_SIZE_SW) {
      Error("isValidSWAddress", "Address (row,col) = (%i,%i) out of bounds (0,%i)", row, col, RICH_SIZE_SW);
      return kFALSE;
   }
   return (-1 != fAddrArrSW[(col * 100 + row)]) ? kTRUE : kFALSE;
}

Bool_t HRichMappingPar::isValidHWAddress(Int_t addrHW)
{
   if (addrHW < 0 || addrHW >= RICH_SIZE_HW) {
      Error("isValidHWAddress", "Address %i out of bounds (0,%i)", addrHW, RICH_SIZE_HW);
      return kFALSE;
   }
   return (-1 != fAddrArrHW[addrHW]) ? kTRUE : kFALSE;
}


