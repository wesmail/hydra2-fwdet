//*-- AUTHOR Ilse Koenig
//*-- created : 17/12/2009 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////
// HRpcTrb2Lookup
//
// Lookup table for the TRB2 unpacker of the RPC detector to map the
// Trbnet address (range defined in htrbnetdef.h) and channel (0..127)
// to the detector address
// sector, column, cell, side, front-end address.
//////////////////////////////////////////////////////////////////////

using namespace std;
#include "hrpctrb2lookup.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <stdlib.h>

ClassImp(HRpcTrb2LookupChan)
ClassImp(HRpcTrb2LookupBoard)
ClassImp(HRpcTrb2Lookup)

HRpcTrb2LookupBoard::HRpcTrb2LookupBoard() {
  // constructor creates an array of 128 channels
  array = new TObjArray(128);
  for(Int_t i=0;i<128;i++) array->AddAt(new HRpcTrb2LookupChan(),i);
}

HRpcTrb2LookupBoard::~HRpcTrb2LookupBoard() {
  // destructor deletes the array of channels
  array->Delete();
  delete array;
}

void HRpcTrb2LookupBoard::clear() {
  // calls the clear function for each channel
  for(Int_t i=0;i<128;i++) (*this)[i].clear();
}

//---------------------------------------------------------------

HRpcTrb2Lookup::HRpcTrb2Lookup(const Char_t* name,
                       const Char_t* title,
		       const Char_t* context,
                       Int_t minTrbnetAddress,
                       Int_t maxTrbnetAddress)
           : HParSet(name,title,context) {
  // constructor creates an empty array of size nBoards 
  arrayOffset=minTrbnetAddress;
  array = new TObjArray(maxTrbnetAddress-minTrbnetAddress+1);
}

HRpcTrb2Lookup::~HRpcTrb2Lookup() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HRpcTrb2Lookup::init(HParIo* inp,Int_t* set) {
  // initializes the container from an input
  HDetParIo* input=inp->getDetParIo("HRpcParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HRpcTrb2Lookup::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HRpcParIo");
  if (out) return out->write(this);
  return -1;
}

void HRpcTrb2Lookup::clear() {
  // deletes all HRpcTrb2LookupBoard objects from the array and resets the input versions
  array->Delete();
  status=kFALSE;
  resetInputVersions();
}

void HRpcTrb2Lookup::printParam() {
  // prints the lookup table
  printf("Lookup table for the TRB2 unpacker of the RPC detector\n");
  printf("trbnet-address  channel  sector  column  cell  side  front-end address\n");
  printf("side: l-left, r-right\n");
  for(Int_t i=0;i<=array->GetLast();i++) {
    HRpcTrb2LookupBoard* b=(*this)[i];
    if (b) {
      for(Int_t j=0;j<b->getSize();j++) {
        HRpcTrb2LookupChan& chan=(*b)[j];
        Int_t sector=chan.getSector();
        if (sector>=0) {
          printf("0x%x %4i %3i%4i%4i  %c  %i\n",
                 arrayOffset+i,j,sector,
                 chan.getColumn(),chan.getCell(),chan.getSide(),chan.getFeAddress());
        }
      }
    }
  }
}

Bool_t HRpcTrb2Lookup::fill(Int_t id,Int_t chan,
                            Int_t sec,Int_t mod,Int_t cell,Char_t side,Int_t fea) {
  // creates the HRpcTrb2LookupBoard objects, if not existing, and fills the channel
  Bool_t rc=kFALSE;
  HRpcTrb2LookupBoard* p=getBoard(id);
  if (!p) {
    p=new HRpcTrb2LookupBoard();
    array->AddAt(p,id-arrayOffset);
  }
  HRpcTrb2LookupChan* c=p->getChannel(chan);
  if (c) {
    c->fill(sec,mod,cell,side,fea);
    rc=kTRUE;
  } else {
    Error("fill","Invalid channel number %i",chan);
  }
  return rc; 
}

Bool_t HRpcTrb2Lookup::readline(const Char_t *buf) {
  // decodes one line read from ASCII file I/O and fills the channel
  Bool_t rc=kFALSE;
  Int_t id, chan, sec, mod, cell, feAddress;
  Char_t side='\0';
  Int_t n=sscanf(buf," 0x%x %i %i%i%i %c %i",&id,&chan,&sec,&mod,&cell,&side,&feAddress);
  if (7==n) {
    rc=fill(id,chan,sec,mod,cell,side,feAddress);
  } else {
      if (n<7) Error("readline","Not enough values in line %s\n",buf);
      else     Error("readline","Too many values in line %s\n",buf); 
  }
  return rc;
}

void HRpcTrb2Lookup::putAsciiHeader(TString& header) {
  // puts the ASCII header to the string used in HRpcParAsciiFileIo
  header=
    "# Lookup table for the TRB2 unpacker of the RPC detector\n"
    "# Format:\n"
    "# trbnet-address  channel  sector  column  cell  side  front-end-address\n";
}

void HRpcTrb2Lookup::write(fstream& fout) {
  // writes the information of all non-zero HRpcTrb2LookupBoard objects to the ASCII file
  for(Int_t i=0;i<=array->GetLast();i++) {
    HRpcTrb2LookupBoard* b=(*this)[i];
    if (b) {
      for(Int_t j=0;j<b->getSize();j++) {
        HRpcTrb2LookupChan& chan=(*b)[j];
        Int_t sector=chan.getSector();
        if (sector>=0) {
          fout<<"0x"<<hex<<(arrayOffset+i)<<dec<<setw(5)<<j
              <<"  "<<sector<<"  "<<chan.getColumn()
              <<setw(5)<<chan.getCell()<<"  "<<chan.getSide()
              <<setw(9)<<chan.getFeAddress()<<'\n';
        }
      }
    }
  }
}
