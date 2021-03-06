//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
// HEmcTrb3Lookup
//
// Lookup table for the TRB3 unpacker of the EMC detector to map the
// Trbnet address (range defined in htrbnetdef.h) and channel (1..255)
// to the detector address sector, cell.
///////////////////////////////////////////////////////////////////////

#include "hdetpario.h"
#include "hpario.h"
#include "hemctrb3lookup.h"
#include "hemcdetector.h"

#include <fstream>

using namespace std;

ClassImp(HEmcTrb3LookupChan)
ClassImp(HEmcTrb3LookupBoard)
ClassImp(HEmcTrb3Lookup)

HEmcTrb3LookupBoard::HEmcTrb3LookupBoard(void) {
  // constructor creates an array of 65 channels
  array = new TObjArray(65);// channels 1-64 (index 0 empty)
  for (Int_t i = 0; i < 65; i++) array->AddAt(new HEmcTrb3LookupChan(), i);
}

HEmcTrb3LookupBoard::~HEmcTrb3LookupBoard(void) {
  // destructor deletes the array of channels
  array->Delete();
  delete array;
}

void HEmcTrb3LookupBoard::clear(void) {
  // calls the clear function for each channel
  for (Int_t i = 0; i < getSize(); i++)(*this)[i].clear();
}

//---------------------------------------------------------------

HEmcTrb3Lookup::HEmcTrb3Lookup(const Char_t* name,
                                     const Char_t* title,
                                     const Char_t* context,
                                     Int_t minTrbnetAddress,
                                     Int_t maxTrbnetAddress)
              : HParSet(name, title, context) {
  // constructor creates an empty array of size nBoards
  arrayOffset = minTrbnetAddress;
  array = new TObjArray(maxTrbnetAddress - minTrbnetAddress + 1);
}

HEmcTrb3Lookup::~HEmcTrb3Lookup(void) {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HEmcTrb3Lookup::init(HParIo* inp, Int_t* set) {
  // initializes the container from an input
  HDetParIo* input = inp->getDetParIo("HEmcParIo");
  if (input) return (input->init(this, set));
  return kFALSE;
}

Int_t HEmcTrb3Lookup::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out = output->getDetParIo("HEmcParIo");
  if (out) return out->write(this);
  return -1;
}

void HEmcTrb3Lookup::clear(void) {
  // deletes all HEmcTrb3LookupBoard objects from the array and resets the input versions
  array->Delete();
  status = kFALSE;
  resetInputVersions();
}

void HEmcTrb3Lookup::printParam(void) {
  // prints the lookup table
  printf("Lookup table for the TRB3 unpacker of the EMC detector\n");
  printf("trbnet-address  channel  sector  cell flag\n");
  for (Int_t i = 0; i <= array->GetLast(); i++) {
    HEmcTrb3LookupBoard* b = (*this)[i];
    if (b) {
      for (Int_t j = 0; j < b->getSize(); j++) {
         HEmcTrb3LookupChan& chan = (*b)[j];
         Int_t sector = chan.getSector();
         if (sector >= 0) {
             printf("0x%x %4i %5i %5i %2i\n",
		    arrayOffset + i, j, sector, chan.getCell(), chan.getFlag());
            }
         }
      }
   }
}

Bool_t HEmcTrb3Lookup::fill(Int_t id, Int_t chan, Int_t sec, Int_t cell, Int_t flag) {
  // creates the HEmcTrb3LookupBoard objects, if not existing, and fills the channel
  Bool_t rc = kFALSE;
  HEmcTrb3LookupBoard* p = getBoard(id);
  if (!p) {
    p = new HEmcTrb3LookupBoard();
    array->AddAt(p, id - arrayOffset);
  }
  HEmcTrb3LookupChan* c = p->getChannel(chan);
  if (c) {
    c->fill(sec, cell, flag);
    rc = kTRUE;
  } else {
    Error("fill", "Invalid channel number %i", chan);
  }
  return rc;
}

Bool_t HEmcTrb3Lookup::readline(const Char_t *buf) {
  // decodes one line read from ASCII file I/O and fills the channel
  Bool_t rc = kFALSE;
  Int_t id, chan, sec, flag, cell,pos;
  Int_t n = sscanf(buf, " 0x%x %i %d %d %d", &id, &chan, &sec, &pos, &flag);
  if (5 == n) {
    cell=HEmcDetector::getCellFromPosition(pos);
    rc = fill(id, chan, sec, cell, flag); // JAM2018 - sector numbers in database/ascii start with 1, in hydra arrays with 0
    //Warning("readline", "JAM - read id0x%x chan:%d sector:%d position:%d cell:%d flag:%d\n", id, chan, sec, pos, cell, flag);
  } else {
    if (n < 5) Error("readline", "Not enough values in line %s\n", buf);
    else     Error("readline", "Too many values in line %s\n", buf);
  }
  return rc;
}

void HEmcTrb3Lookup::putAsciiHeader(TString& header) {
  // puts the ASCII header to the string used in HEmcParAsciiFileIo
  header =
    "# Lookup table for the TRB3 unpacker of the EMC detector\n"
    "# Format:\n"
    "# trbnet-address  channel  sector(0...5) position(0...162)  tdcflag(1:fast, 2:slow)\n";
}

void HEmcTrb3Lookup::write(fstream& fout) {
  // writes the information of all non-zero HEmcTrb3LookupBoard objects to the ASCII file
  for (Int_t i = 0; i <= array->GetLast(); i++) {
    HEmcTrb3LookupBoard* b = (*this)[i];
    if (b) {
      for (Int_t j = 0; j < b->getSize(); j++) {
         HEmcTrb3LookupChan& chan = (*b)[j];
         Int_t sector = chan.getSector();
         if (sector >= 0) {
             Int_t pos=HEmcDetector::getPositionFromCell(chan.getCell());
             if(pos<0) continue;
             fout << "0x" << hex << (arrayOffset + i) << dec << setw(5) << j
                << setw(5) << sector+1 << setw(5) << pos << setw(5) << static_cast<Int_t>(chan.getFlag()) <<'\n';
           }
        }
     }
  }
}
