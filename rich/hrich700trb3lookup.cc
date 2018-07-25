//*-- AUTHOR Ilse Koenig
//*-- created : 03/12/2009 by Ilse Koenig
//*-- modified for TRB3 : 10/03/2014 by Sergey Linev
//*-- adapted from START to RICH700 : 08/06/2017 by JAM (j.adamczewski@gsi.de)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
// HRich700Trb3Lookup
//
// Lookup table for the TRB3 unpacker of the RICH  detector to map the
// Trbnet address (range defined in htrbnetdef.h) and TDC channel
// to the detector address module, cell. JAM 6/2017
///////////////////////////////////////////////////////////////////////

#include "hdetpario.h"
#include "hpario.h"
#include "hrich700trb3lookup.h"

#include <fstream>

using namespace std;

ClassImp(HRich700Trb3LookupChan)
ClassImp(HRich700Trb3LookupTdc)
ClassImp(HRich700Trb3Lookup)

HRich700Trb3LookupTdc::HRich700Trb3LookupTdc()
{
   // constructor creates an array of 33 channels
   array = new TObjArray(HRICH700_MAXTDCCHANNELS);
   for (Int_t i = 0; i < HRICH700_MAXTDCCHANNELS; i++) array->AddAt(new HRich700Trb3LookupChan(), i);


}

HRich700Trb3LookupTdc::~HRich700Trb3LookupTdc()
{
   // destructor deletes the array of channels

   array->Delete();
   delete array;
}

void HRich700Trb3LookupTdc::clear()
{
   // calls the clear function for each channel
   for (Int_t i = 0; i < getSize(); i++)(*this)[i].clear();
}

//---------------------------------------------------------------

HRich700Trb3Lookup::HRich700Trb3Lookup(const Char_t* name,
                                     const Char_t* title,
                                     const Char_t* context,
                                     Int_t minTrbnetAddress,
                                     Int_t maxTrbnetAddress)
   : HParSet(name, title, context)
{
   // constructor creates an empty array of size nTdcs
   arrayOffset = minTrbnetAddress;
   array = new TObjArray(maxTrbnetAddress - minTrbnetAddress + 1);
   arrayCursor=0;
}

HRich700Trb3Lookup::~HRich700Trb3Lookup()
{
   // destructor
   array->Delete();
   delete array;
}

Bool_t HRich700Trb3Lookup::init(HParIo* inp, Int_t* set)
{
   // initializes the container from an input
   HDetParIo* input = inp->getDetParIo("HRichParIo");
   if (input) return (input->init(this, set));
   return kFALSE;
}

Int_t HRich700Trb3Lookup::write(HParIo* output)
{
   // writes the container to an output
   HDetParIo* out = output->getDetParIo("HRichParIo");
   if (out) return out->write(this);
   return -1;
}

void HRich700Trb3Lookup::clear()
{
   // deletes all HRich700Trb3LookupTdc objects from the array and resets the input versions

   array->Delete();
   status = kFALSE;
   resetInputVersions();
}


void HRich700Trb3Lookup::resetTdcIterator()
{
	arrayCursor=0;
}

Int_t HRich700Trb3Lookup::getNextTdcAddress()
{
    Int_t address=0;
    HRich700Trb3LookupTdc* slot=0;
    if(arrayCursor>=getSize()) return 0; // supress warnings when exceeding array at first


    for(Int_t n=arrayCursor;n<getSize();n++){
	slot= dynamic_cast<HRich700Trb3LookupTdc*> (array->At(n));
	if(slot){  // first non empty slot
	    address     = n+arrayOffset;
	    arrayCursor = n+1; // set cursor to next slot for next call
	    return address;
	}
	arrayCursor = n+1;
    }
    return address;
}


void HRich700Trb3Lookup::printParam()
{
   // prints the lookup table
   printf("Lookup table for the TRB3 unpacker of the RICH700 detector\n");
   printf("trbnet-address  channel  PMT  pixel\n");
   for (Int_t i = 0; i <= array->GetLast(); i++) {
      HRich700Trb3LookupTdc* b = (*this)[i];
      if (b) {
         for (Int_t j = 0; j < b->getSize(); j++) {
            HRich700Trb3LookupChan& chan = (*b)[j];
            Int_t pmt = chan.getPMT();
            if (pmt >= 0) {

               printf("0x%x %4i %5i %5i\n",
                      arrayOffset + i, j, pmt, chan.getPixel());
            }
         }
      }
   }
}

Bool_t HRich700Trb3Lookup::fill(Int_t id, Int_t chan, Int_t pmt, Int_t pix)
{
   // creates the HRich700Trb3LookupTdc objects, if not existing, and fills the channel
   Bool_t rc = kFALSE;
   HRich700Trb3LookupTdc* p = getTdc(id);
   if (!p) {
      p = new HRich700Trb3LookupTdc();
      array->AddAt(p, id - arrayOffset);
   }
   HRich700Trb3LookupChan* c = p->getChannel(chan);
   if (c) {
      c->fill(pmt, pix);
      rc = kTRUE;
   } else {
      Error("fill", "Invalid channel number %i", chan);
   }
   return rc;
}

Bool_t HRich700Trb3Lookup::readline(const Char_t *buf)
{
   // decodes one line read from ASCII file I/O and fills the channel
	//printf("HRich700Trb3Lookup::readline: %s \n",buf); // JAM2017
	Bool_t rc = kFALSE;
   Int_t id, chan, pmt, pix;
   Int_t n = sscanf(buf, " 0x%x %d %d %d", &id, &chan, &pmt, &pix);
   //printf(" - id:%x chan:%d pmt:%d pix:%d \n",id,chan,pmt,pix); // JAM2017
   if (4 == n) {
      rc = fill(id, chan, pmt, pix);
   } else {
      if (n < 4) Error("readline", "Not enough values in line %s\n", buf);
      else     Error("readline", "Too many values in line %s\n", buf);
   }
   return rc;
}

void HRich700Trb3Lookup::putAsciiHeader(TString& header)
{
   // puts the ASCII header to the string used in HRichParAsciiFileIo
   header =
      "# Lookup table for the TRB3 unpacker of the RICH700 detector\n"
      "# Format:\n"
      "# trbnet-address  channel  pmt  pixel\n";
}

void HRich700Trb3Lookup::write(fstream& fout)
{
   // writes the information of all non-zero HRich700Trb3LookupTdc objects to the ASCII file
   for (Int_t i = 0; i <= array->GetLast(); i++) {
      HRich700Trb3LookupTdc* b = (*this)[i];
      if (b) {
         for (Int_t j = 0; j < b->getSize(); j++) {
            HRich700Trb3LookupChan& chan = (*b)[j];
            Int_t pmt = chan.getPMT();
            if (pmt >= 0) {
               fout << "0x" << hex << (arrayOffset + i) << dec << setw(5) << j
                    << setw(5) << pmt << setw(5) << chan.getPixel() << '\n';
            }
         }
      }
   }
}
