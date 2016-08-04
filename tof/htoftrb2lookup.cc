//*-- AUTHOR Ilse Koenig
//*-- created : 15/12/2009 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////
// HTofTrb2Lookup
//
// Lookup table for the TRB2 unpacker of the TOF detector to map the
// Trbnet address (range defined in htrbnetdef.h) and channel (0..127)
// to the detector address
// sector, module, cell, side.
//////////////////////////////////////////////////////////////////////

using namespace std;
#include "htoftrb2lookup.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <stdlib.h>

ClassImp(HTofTrb2LookupChan)
ClassImp(HTofTrb2LookupBoard)
ClassImp(HTofTrb2Lookup)

HTofTrb2LookupBoard::HTofTrb2LookupBoard() {
  // constructor creates an array of 128 channels
  array = new TObjArray(128);
  for(Int_t i=0;i<128;i++) array->AddAt(new HTofTrb2LookupChan(),i);
}

HTofTrb2LookupBoard::~HTofTrb2LookupBoard() {
  // destructor deletes the array of channels
  array->Delete();
  delete array;
}

void HTofTrb2LookupBoard::clear() {
  // calls the clear function for each channel
  for(Int_t i=0;i<128;i++) (*this)[i].clear();
}

//---------------------------------------------------------------

HTofTrb2Lookup::HTofTrb2Lookup(const Char_t* name,
                       const Char_t* title,
		       const Char_t* context,
                       Int_t minTrbnetAddress,
                       Int_t maxTrbnetAddress)
           : HParSet(name,title,context) {
  // constructor creates an empty array of size nBoards 
  arrayOffset=minTrbnetAddress;
  array = new TObjArray(maxTrbnetAddress-minTrbnetAddress+1);
}

HTofTrb2Lookup::~HTofTrb2Lookup() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HTofTrb2Lookup::init(HParIo* inp,Int_t* set) {
  // initializes the container from an input
  HDetParIo* input=inp->getDetParIo("HTofParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HTofTrb2Lookup::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HTofParIo");
  if (out) return out->write(this);
  return -1;
}

void HTofTrb2Lookup::clear() {
  // deletes all HTofTrb2LookupBoard objects from the array and resets the input versions
  array->Delete();
  status=kFALSE;
  resetInputVersions();
}

void HTofTrb2Lookup::printParam() {
  // prints the lookup table
  printf("Lookup table for the TRB2 unpacker of the TOF detector\n");
  printf("trbnet-address  channel  sector  module  cell  side\n");
  printf("side: l-left, r-right\n");
  for(Int_t i=0;i<=array->GetLast();i++) {
    HTofTrb2LookupBoard* b=(*this)[i];
    if (b) {
      for(Int_t j=0;j<b->getSize();j++) {
        HTofTrb2LookupChan& chan=(*b)[j];
        Int_t sector=chan.getSector();
        if (sector>=0) {
          printf("0x%x %4i %3i%4i%4i  %c\n",
                 arrayOffset+i,j,sector,
                 chan.getModule(),chan.getCell(),chan.getSide());
        }
      }
    }
  }
}

Bool_t HTofTrb2Lookup::fill(Int_t id,Int_t chan,
                            Int_t sec,Int_t mod,Int_t cell,Char_t side) {
  // creates the HTofTrb2LookupBoard objects, if not existing, and fills the channel
  Bool_t rc=kFALSE;
  HTofTrb2LookupBoard* p=getBoard(id);
  if (!p) {
    p=new HTofTrb2LookupBoard();
    array->AddAt(p,id-arrayOffset);
  }
  HTofTrb2LookupChan* c=p->getChannel(chan);
  if (c) {
    c->fill(sec,mod,cell,side);
    rc=kTRUE;
  } else {
    Error("fill","Invalid channel number %i",chan);
  }
  return rc; 
}

Bool_t HTofTrb2Lookup::readline(const Char_t *buf) {
  // decodes one line read from ASCII file I/O and fills the channel
  Bool_t rc=kFALSE;
  Int_t id, chan, sec, mod, cell;
  Char_t side='\0';
  Int_t n=sscanf(buf," 0x%x %i %i%i%i %c",&id,&chan,&sec,&mod,&cell,&side);
  if (6==n) {
    rc=fill(id,chan,sec,mod,cell,side);
  } else {
      if (n<6) Error("readline","Not enough values in line %s\n",buf);
      else     Error("readline","Too many values in line %s\n",buf); 
  }
  return rc;
}

void HTofTrb2Lookup::putAsciiHeader(TString& header) {
  // puts the ASCII header to the string used in HTofParAsciiFileIo
  header=
    "# Lookup table for the TRB2 unpacker of the TOF detector\n"
    "# Format:\n"
    "# trbnet-address  channel  sector  module  cell  side\n";
}

void HTofTrb2Lookup::write(fstream& fout) {
  // writes the information of all non-zero HTofTrb2LookupBoard objects to the ASCII file
  for(Int_t i=0;i<=array->GetLast();i++) {
    HTofTrb2LookupBoard* b=(*this)[i];
    if (b) {
      for(Int_t j=0;j<b->getSize();j++) {
        HTofTrb2LookupChan& chan=(*b)[j];
        Int_t sector=chan.getSector();
        if (sector>=0) {
          fout<<"0x"<<hex<<(arrayOffset+i)<<dec<<setw(5)<<j
              <<"  "<<sector<<"  "<<chan.getModule()
              <<"  "<<chan.getCell()<<"  "<<chan.getSide()<<'\n';
        }
      }
    }
  }
}
