//*-- AUTHOR Ilse Koenig
//*-- created : 03/12/2009 by Ilse Koenig
//*-- modified for TRB3 : 10/03/2014 by Sergey Linev

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
// HStart2Trb3Lookup
//
// Lookup table for the TRB3 unpacker of the START2 detector to map the
// Trbnet address (range defined in htrbnetdef.h) and TDC channel
// to the detector address module, cell.
///////////////////////////////////////////////////////////////////////

#include "hdetpario.h"
#include "hpario.h"
#include "hstart2trb3lookup.h"

#include <fstream>

using namespace std;

ClassImp(HStart2Trb3LookupChan)
ClassImp(HStart2Trb3LookupTdc)
ClassImp(HStart2Trb3Lookup)

HStart2Trb3LookupTdc::HStart2Trb3LookupTdc()
{
   // constructor creates an array of 65 channels
   array = new TObjArray(65);
   for (Int_t i = 0; i < 65; i++) array->AddAt(new HStart2Trb3LookupChan(), i);
}

HStart2Trb3LookupTdc::~HStart2Trb3LookupTdc()
{
   // destructor deletes the array of channels
   array->Delete();
   delete array;
}

void HStart2Trb3LookupTdc::clear()
{
   // calls the clear function for each channel
   for (Int_t i = 0; i < getSize(); i++)(*this)[i].clear();
}

//---------------------------------------------------------------

HStart2Trb3Lookup::HStart2Trb3Lookup(const Char_t* name,
                                     const Char_t* title,
                                     const Char_t* context,
                                     Int_t minTrbnetAddress,
                                     Int_t maxTrbnetAddress)
   : HParSet(name, title, context)
{
   // constructor creates an empty array of size nTdcs
   arrayOffset = minTrbnetAddress;
   array = new TObjArray(maxTrbnetAddress - minTrbnetAddress + 1);
}

HStart2Trb3Lookup::~HStart2Trb3Lookup()
{
   // destructor
   array->Delete();
   delete array;
}

Bool_t HStart2Trb3Lookup::init(HParIo* inp, Int_t* set)
{
   // initializes the container from an input
   HDetParIo* input = inp->getDetParIo("HStartParIo");
   if (input) return (input->init(this, set));
   return kFALSE;
}

Int_t HStart2Trb3Lookup::write(HParIo* output)
{
   // writes the container to an output
   HDetParIo* out = output->getDetParIo("HStartParIo");
   if (out) return out->write(this);
   return -1;
}

void HStart2Trb3Lookup::clear()
{
   // deletes all HStart2Trb3LookupTdc objects from the array and resets the input versions
   array->Delete();
   status = kFALSE;
   resetInputVersions();
}

void HStart2Trb3Lookup::printParam()
{
   // prints the lookup table
   printf("Lookup table for the TRB3 unpacker of the START2 detector\n");
   printf("trbnet-address  channel  module  cell\n");
   for (Int_t i = 0; i <= array->GetLast(); i++) {
      HStart2Trb3LookupTdc* b = (*this)[i];
      if (b) {
         for (Int_t j = 0; j < b->getSize(); j++) {
            HStart2Trb3LookupChan& chan = (*b)[j];
            Int_t module = chan.getModule();
            if (module >= 0) {
               printf("0x%x %4i %5i%5i\n",
                      arrayOffset + i, j, module, chan.getCell());
            }
         }
      }
   }
}

Bool_t HStart2Trb3Lookup::fill(Int_t id, Int_t chan, Int_t mod, Int_t cell)
{
   // creates the HStart2Trb3LookupTdc objects, if not existing, and fills the channel
   Bool_t rc = kFALSE;
   HStart2Trb3LookupTdc* p = getTdc(id);
   if (!p) {
      p = new HStart2Trb3LookupTdc();
      array->AddAt(p, id - arrayOffset);
   }
   HStart2Trb3LookupChan* c = p->getChannel(chan);
   if (c) {
      c->fill(mod, cell);
      rc = kTRUE;
   } else {
      Error("fill", "Invalid channel number %i", chan);
   }
   return rc;
}

Bool_t HStart2Trb3Lookup::readline(const Char_t *buf)
{
   // decodes one line read from ASCII file I/O and fills the channel
   Bool_t rc = kFALSE;
   Int_t id, chan, mod, cell;
   Int_t n = sscanf(buf, " 0x%x %i %i%i", &id, &chan, &mod, &cell);
   if (4 == n) {
      rc = fill(id, chan, mod, cell);
   } else {
      if (n < 4) Error("readline", "Not enough values in line %s\n", buf);
      else     Error("readline", "Too many values in line %s\n", buf);
   }
   return rc;
}

void HStart2Trb3Lookup::putAsciiHeader(TString& header)
{
   // puts the ASCII header to the string used in HStartParAsciiFileIo
   header =
      "# Lookup table for the TRB3 unpacker of the START2 detector\n"
      "# Format:\n"
      "# trbnet-address  channel  module  cell\n";
}

void HStart2Trb3Lookup::write(fstream& fout)
{
   // writes the information of all non-zero HStart2Trb3LookupTdc objects to the ASCII file
   for (Int_t i = 0; i <= array->GetLast(); i++) {
      HStart2Trb3LookupTdc* b = (*this)[i];
      if (b) {
         for (Int_t j = 0; j < b->getSize(); j++) {
            HStart2Trb3LookupChan& chan = (*b)[j];
            Int_t module = chan.getModule();
            if (module >= 0) {
               fout << "0x" << hex << (arrayOffset + i) << dec << setw(5) << j
                    << setw(5) << module << setw(5) << chan.getCell() << '\n';
            }
         }
      }
   }
}
