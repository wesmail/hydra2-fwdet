//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
//
//  HPionTrackerTrb3Lookup
//
//  Lookup table for the TRB3 unpacker of the PionTracker to map the
//  Trbnet address (range defined in htrbnetdef.h) and channel (0..255)
//  to the detector address module, cell.
//
///////////////////////////////////////////////////////////////////////

#include "hpiontrackertrb3lookup.h"
#include "hdetpario.h"
#include "hpario.h"
#include <fstream>

using namespace std;

ClassImp(HPionTrackerTrb3LookupChan)
ClassImp(HPionTrackerTrb3LookupBoard)
ClassImp(HPionTrackerTrb3Lookup)

HPionTrackerTrb3LookupBoard::HPionTrackerTrb3LookupBoard() {
  // constructor creates an array of 256 channels
  array = new TObjArray(256);
  for (Int_t i = 0; i < 256; i++) array->AddAt(new HPionTrackerTrb3LookupChan(), i);
}

HPionTrackerTrb3LookupBoard::~HPionTrackerTrb3LookupBoard() {
  // destructor deletes the array of channels
  array->Delete();
  delete array;
}

void HPionTrackerTrb3LookupBoard::clear() {
  // calls the clear function for each channel
  for (Int_t i = 0; i < 256; i++)(*this)[i].clear();
}

//---------------------------------------------------------------

HPionTrackerTrb3Lookup::HPionTrackerTrb3Lookup(const Char_t* name,
                                     const Char_t* title,
                                     const Char_t* context,
                                     Int_t minTrbnetAddress,
                                     Int_t maxTrbnetAddress)
                      : HParSet(name, title, context) {
  // constructor creates an empty array of size nBoards
  arrayOffset = minTrbnetAddress;
  array = new TObjArray(maxTrbnetAddress - minTrbnetAddress + 1);
}

HPionTrackerTrb3Lookup::~HPionTrackerTrb3Lookup() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HPionTrackerTrb3Lookup::init(HParIo* inp, Int_t* set) {
  // initializes the container from an input
  HDetParIo* input = inp->getDetParIo("HPionTrackerParIo");
  if (input) return (input->init(this, set));
  return kFALSE;
}

Int_t HPionTrackerTrb3Lookup::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out = output->getDetParIo("HPionTrackerParIo");
  if (out) return out->write(this);
  return -1;
}

void HPionTrackerTrb3Lookup::clear() {
  // deletes all HPionTrackerTrb3LookupBoard objects from the array and resets the input versions
  array->Delete();
  status = kFALSE;
  resetInputVersions();
}

void HPionTrackerTrb3Lookup::printParams() {
  // prints the lookup table
  printf("Lookup table for the TRB3 unpacker of the PionTracker\n");
  printf("trbnet-address  channel  module  cell\n");
  for (Int_t i = 0; i <= array->GetLast(); i++) {
    HPionTrackerTrb3LookupBoard* b = (*this)[i];
    if (b) {
      for (Int_t j = 0; j < b->getSize(); j++) {
        HPionTrackerTrb3LookupChan& chan = (*b)[j];
        Int_t module = chan.getModule();
        if (module >= 0) {
          printf("0x%x %4i %5i%5i\n",
                 arrayOffset + i, j, module, chan.getCell());
        }
      }
    }
  }
}

Bool_t HPionTrackerTrb3Lookup::fill(Int_t id, Int_t chan, Int_t mod, Int_t cell) {
  // creates the HPionTrackerTrb3LookupBoard objects, if not existing, and fills the channel
  Bool_t rc = kFALSE;
  HPionTrackerTrb3LookupBoard* p = getBoard(id);
  if (!p) {
    p = new HPionTrackerTrb3LookupBoard();
    array->AddAt(p, id - arrayOffset);
  }
  HPionTrackerTrb3LookupChan* c = p->getChannel(chan);
  if (c) {
    c->fill(mod, cell);
    rc = kTRUE;
  } else {
    Error("fill", "Invalid channel number %i", chan);
  }
  return rc;
}

Bool_t HPionTrackerTrb3Lookup::readline(const Char_t *buf) {
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

void HPionTrackerTrb3Lookup::putAsciiHeader(TString& header) {
  // puts the ASCII header to the string used in HPionTrackerParAsciiFileIo
  header = "# Lookup table for the TRB3 unpacker of the PionTracker\n"
           "# Format:\n"
           "# trbnet-address  channel  module  cell\n";
}

void HPionTrackerTrb3Lookup::write(fstream& fout) {
  // writes the information of all non-zero HPionTrackerTrb3LookupBoard objects to the ASCII file
  for (Int_t i = 0; i <= array->GetLast(); i++) {
    HPionTrackerTrb3LookupBoard* b = (*this)[i];
    if (b) {
      for (Int_t j = 0; j < b->getSize(); j++) {
        HPionTrackerTrb3LookupChan& chan = (*b)[j];
        Int_t module = chan.getModule();
        if (module >= 0) {
          fout << "0x" << hex << (arrayOffset + i) << dec << setw(5) << j
               << setw(5) << module << setw(5) << chan.getCell() << '\n';
        }
      }
    }
  }
}
