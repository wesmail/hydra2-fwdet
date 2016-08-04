//*-- AUTHOR Ilse Koenig
//*-- created : 03/12/2009 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
// HStart2Trb2Lookup
//
// Lookup table for the TRB2 unpacker of the START2 detector to map the
// Trbnet address (range defined in htrbnetdef.h) and channel (0..127)
// to the detector address module, cell.
///////////////////////////////////////////////////////////////////////

#include "hdetpario.h"
#include "hpario.h"
#include "hstart2trb2lookup.h"

#include <fstream>

using namespace std;

ClassImp(HStart2Trb2LookupChan)
ClassImp(HStart2Trb2LookupBoard)
ClassImp(HStart2Trb2Lookup)

HStart2Trb2LookupBoard::HStart2Trb2LookupBoard()
{
   // constructor creates an array of 128 channels
   array = new TObjArray(128);
   for (Int_t i = 0; i < 128; i++) array->AddAt(new HStart2Trb2LookupChan(), i);
}

HStart2Trb2LookupBoard::~HStart2Trb2LookupBoard()
{
   // destructor deletes the array of channels
   array->Delete();
   delete array;
}

void HStart2Trb2LookupBoard::clear()
{
   // calls the clear function for each channel
   for (Int_t i = 0; i < 128; i++)(*this)[i].clear();
}

//---------------------------------------------------------------

HStart2Trb2Lookup::HStart2Trb2Lookup(const Char_t* name,
                                     const Char_t* title,
                                     const Char_t* context,
                                     Int_t minTrbnetAddress,
                                     Int_t maxTrbnetAddress)
   : HParSet(name, title, context)
{
   // constructor creates an empty array of size nBoards
   arrayOffset = minTrbnetAddress;
   array = new TObjArray(maxTrbnetAddress - minTrbnetAddress + 1);
}

HStart2Trb2Lookup::~HStart2Trb2Lookup()
{
   // destructor
   array->Delete();
   delete array;
}

Bool_t HStart2Trb2Lookup::init(HParIo* inp, Int_t* set)
{
   // initializes the container from an input
   HDetParIo* input = inp->getDetParIo("HStartParIo");
   if (input) return (input->init(this, set));
   return kFALSE;
}

Int_t HStart2Trb2Lookup::write(HParIo* output)
{
   // writes the container to an output
   HDetParIo* out = output->getDetParIo("HStartParIo");
   if (out) return out->write(this);
   return -1;
}

void HStart2Trb2Lookup::clear()
{
   // deletes all HStart2Trb2LookupBoard objects from the array and resets the input versions
   array->Delete();
   status = kFALSE;
   resetInputVersions();
}

void HStart2Trb2Lookup::printParam()
{
   // prints the lookup table
   printf("Lookup table for the TRB2 unpacker of the START2 detector\n");
   printf("trbne-address  channel  module  cell\n");
   for (Int_t i = 0; i <= array->GetLast(); i++) {
      HStart2Trb2LookupBoard* b = (*this)[i];
      if (b) {
         for (Int_t j = 0; j < b->getSize(); j++) {
            HStart2Trb2LookupChan& chan = (*b)[j];
            Int_t module = chan.getModule();
            if (module >= 0) {
               printf("0x%x %4i %5i%5i\n",
                      arrayOffset + i, j, module, chan.getCell());
            }
         }
      }
   }
}

Bool_t HStart2Trb2Lookup::fill(Int_t id, Int_t chan, Int_t mod, Int_t cell)
{
   // creates the HStart2Trb2LookupBoard objects, if not existing, and fills the channel
   Bool_t rc = kFALSE;
   HStart2Trb2LookupBoard* p = getBoard(id);
   if (!p) {
      p = new HStart2Trb2LookupBoard();
      array->AddAt(p, id - arrayOffset);
   }
   HStart2Trb2LookupChan* c = p->getChannel(chan);
   if (c) {
      c->fill(mod, cell);
      rc = kTRUE;
   } else {
      Error("fill", "Invalid channel number %i", chan);
   }
   return rc;
}

Bool_t HStart2Trb2Lookup::readline(const Char_t *buf)
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

void HStart2Trb2Lookup::putAsciiHeader(TString& header)
{
   // puts the ASCII header to the string used in HStartParAsciiFileIo
   header =
      "# Lookup table for the TRB2 unpacker of the START2 detector\n"
      "# Format:\n"
      "# trbnet-address  channel  module  cell\n";
}

void HStart2Trb2Lookup::write(fstream& fout)
{
   // writes the information of all non-zero HStart2Trb2LookupBoard objects to the ASCII file
   for (Int_t i = 0; i <= array->GetLast(); i++) {
      HStart2Trb2LookupBoard* b = (*this)[i];
      if (b) {
         for (Int_t j = 0; j < b->getSize(); j++) {
            HStart2Trb2LookupChan& chan = (*b)[j];
            Int_t module = chan.getModule();
            if (module >= 0) {
               fout << "0x" << hex << (arrayOffset + i) << dec << setw(5) << j
                    << setw(5) << module << setw(5) << chan.getCell() << '\n';
            }
         }
      }
   }
}
