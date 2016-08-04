//*-- AUTHOR Ilse Koenig
//*-- created : 15/12/2009 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
// HWallTrb2Lookup
//
// Lookup table of the Forward Wall for the TRB2 unpacker to map the
// Trbnet address (range defined in htrbnetdef.h) and channel (0..127)
// to the detector cell.
///////////////////////////////////////////////////////////////////////

using namespace std;
#include "hwalltrb2lookup.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <stdlib.h>

ClassImp(HWallTrb2LookupChan)
ClassImp(HWallTrb2LookupBoard)
ClassImp(HWallTrb2Lookup)

HWallTrb2LookupBoard::HWallTrb2LookupBoard() {
  // constructor creates an array of 128 channels
  array = new TObjArray(128);
  for(Int_t i=0;i<128;i++) array->AddAt(new HWallTrb2LookupChan(),i);
}

HWallTrb2LookupBoard::~HWallTrb2LookupBoard() {
  // destructor deletes the array of channels
  array->Delete();
  delete array;
}

void HWallTrb2LookupBoard::clear() {
  // calls the clear function for each channel
  for(Int_t i=0;i<128;i++) (*this)[i].clear();
}

//---------------------------------------------------------------

HWallTrb2Lookup::HWallTrb2Lookup(const Char_t* name,
                       const Char_t* title,
		       const Char_t* context,
                       Int_t minTrbnetAddress,
                       Int_t maxTrbnetAddress)
           : HParSet(name,title,context) {
  // constructor creates an empty array of size nBoards 
  arrayOffset=minTrbnetAddress;
  array = new TObjArray(maxTrbnetAddress-minTrbnetAddress+1);
}

HWallTrb2Lookup::~HWallTrb2Lookup() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HWallTrb2Lookup::init(HParIo* inp,Int_t* set) {
  // initializes the container from an input
  HDetParIo* input=inp->getDetParIo("HWallParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HWallTrb2Lookup::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HWallParIo");
  if (out) return out->write(this);
  return -1;
}

void HWallTrb2Lookup::clear() {
  // deletes all HWallTrb2LookupBoard objects from the array and resets the input versions
  array->Delete();
  status=kFALSE;
  resetInputVersions();
}

void HWallTrb2Lookup::printParam() {
  // prints the lookup table
  printf("Lookup table for the TRB2 unpacker of the Forward Wall\n");
  printf("trbnet-address  channel  cell\n");
  for(Int_t i=0;i<=array->GetLast();i++) {
    HWallTrb2LookupBoard* b=(*this)[i];
    if (b) {
      for(Int_t j=0;j<b->getSize();j++) {
        HWallTrb2LookupChan& chan=(*b)[j];
        Int_t cell=chan.getCell();
        if (cell>=0) {
          printf("0x%x %4i %6i\n",arrayOffset+i,j,cell);
        }
      }
    }
  }
}

Bool_t HWallTrb2Lookup::fill(Int_t id,Int_t chan,Int_t cell) {
  // creates the HWallTrb2LookupBoard objects, if not existing, and fills the channel
  Bool_t rc=kFALSE;
  HWallTrb2LookupBoard* p=getBoard(id);
  if (!p) {
    p=new HWallTrb2LookupBoard();
    array->AddAt(p,id-arrayOffset);
  }
  HWallTrb2LookupChan* c=p->getChannel(chan);
  if (c) {
    c->setCell(cell);
    rc=kTRUE;
  } else {
    Error("fill","Invalid channel number %i",chan);
  }
  return rc; 
}

Bool_t HWallTrb2Lookup::readline(const Char_t *buf) {
  // decodes one line read from ASCII file I/O and fills the channel
  Bool_t rc=kFALSE;
  Int_t id, chan, cell;
  Int_t n=sscanf(buf," 0x%x %i %i",&id,&chan,&cell);
  if (3==n) {
    rc=fill(id,chan,cell);
  } else {
    if (n<3) Error("readline","Not enough values in line %s\n",buf);
    else     Error("readline","Too many values in line %s\n",buf); 
  }
  return rc;
}

void HWallTrb2Lookup::putAsciiHeader(TString& header) {
  // puts the ASCII header to the string used in HWallParAsciiFileIo
  header=
    "# Lookup table for the TRB unpacker of the Forward Wall\n"
    "# Format:\n"
    "# trbnet-address  channel  cell\n";
}

void HWallTrb2Lookup::write(fstream& fout) {
  // writes the information of all non-zero HWallTrb2LookupBoard objects to the ASCII file
  for(Int_t i=0;i<=array->GetLast();i++) {
    HWallTrb2LookupBoard* b=(*this)[i];
    if (b) {
      for(Int_t j=0;j<b->getSize();j++) {
        HWallTrb2LookupChan& chan=(*b)[j];
        Int_t cell=chan.getCell();
        if (cell>=0) {
          fout<<"0x"<<hex<<(arrayOffset+i)<<dec<<setw(5)<<j
              <<setw(6)<<cell<<'\n';
        }
      }
    }
  }
}
