//*-- AUTHOR Ilse Koenig
//*-- created : 08/12/2000 by Ilse Koenig
//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////
// HTofLookup
//
// Container class for mapping Crate/TdcAdc/Channel to Module/Paddle
//   needed by the TOF unpacker
//
////////////////////////////////////////////////////////////////////
using namespace std;
#include "htoflookup.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HTofLookupChan)
ClassImp(HTofLookupSlot)
ClassImp(HTofLookupCrate)
ClassImp(HTofLookup)

HTofLookupSlot::HTofLookupSlot(Int_t nChan) {
  // constructor creates an array of nChan pointers of type HTofLookupChan
  array = new TObjArray(nChan);
  for(Int_t i=0;i<nChan;i++) array->AddAt(new HTofLookupChan(),i);
  nChannels=32;
  maxChannel=nChan-1;
  clear();
}

HTofLookupSlot::~HTofLookupSlot() {
  // destructor
  array->Delete();
  delete array;
}

void HTofLookupSlot::clear() {
  // sets the module type to 'U' (unknown) and clears the array
  modType='U';
  for(Int_t i=0;i<=maxChannel;i++) (*this)[i].clear();
}

void HTofLookupSlot::fill(Char_t dcType, Int_t channel, 
        Int_t sector, Int_t module, Int_t cell, Char_t side) {
  // Sets the module type and fills the HLookupChan objects with index channel
  // The array is automatically expanded to 32 channels. 
  modType=dcType;
  if (channel>maxChannel) {
    array->Expand(nChannels);
    for(Int_t i=maxChannel+1;i<nChannels;i++) array->AddAt(new HTofLookupChan(),i);
    maxChannel=nChannels-1;
  }
  (*this)[channel].fill(sector,module,cell,side);
}

HTofLookupCrate::HTofLookupCrate(Int_t numSlots) {
  // constructor creates an array of pointers of type HTofLookupSlot
  nSlots=21;
  maxSlot=numSlots-1;
  array = new TObjArray(numSlots);
  for(Int_t i=0;i<numSlots;i++) array->AddAt(new HTofLookupSlot(),i);
}

HTofLookupCrate::~HTofLookupCrate() {
  // destructor
  array->Delete();
  delete array;
}

void HTofLookupCrate::fill(Int_t slot, Char_t modType,
        Int_t channel, Int_t sector, Int_t module, Int_t cell, Char_t side) {
  // Expands the array, if necessary, creates the objects of type HTofLookupSlot,
  // and then calls the fill function of the objects with index slot. 
  if (slot>maxSlot) {
    array->Expand(slot+1);
    for(Int_t i=maxSlot+1;i<=slot;i++) array->AddAt(new HTofLookupSlot(),i);
    maxSlot=slot;
  }
  (*this)[slot].fill(modType,channel,sector,module,cell,side);
}

HTofLookup::HTofLookup(const Char_t* name,const Char_t* title,
                       const Char_t* context,
                       Int_t nCrates, Int_t nSlots)
           : HParSet(name,title,context) {
  // The constructor creates an array of pointers of type HTofLookupCrate of
  // size nSlots
  // The container name is set to "TofLookup".
  strcpy(detName,"Tof");
  maxCrate=nCrates-1;
  array = new TObjArray(nCrates);
  for (Int_t i=0;i<nCrates;i++) array->AddAt(new HTofLookupCrate(nSlots),i);
}

HTofLookup::~HTofLookup() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HTofLookup::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HTofParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HTofLookup::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HTofParIo");
  if (out) return out->write(this);
  return -1;
}

void HTofLookup::clear() {
  // clears the container
  for(Int_t c=0;c<=maxCrate;c++) {
    for(Int_t s=0;s<(*this)[c].getSize();s++) (*this)[c][s].clear();
  }
  status=kFALSE;
  resetInputVersions();
}

void HTofLookup::printParam() {
  // prints the lookup table
  printf("Lookup table for the TOF unpacker\n");
  printf("crate  slot  type  channel  sector  module  cell  side\n");
  for(Int_t c=0;c<getSize();c++) {
    for(Int_t s=0;s<(*this)[c].getSize();s++) {
      HTofLookupSlot& slot=(*this)[c][s];
      for(Int_t r=0;r<slot.getSize();r++) {
        HTofLookupChan& chan=slot[r];
        if (chan.getSector()>=0) printf("%4i%4i   %c%4i%4i%4i%4i   %c\n",
               c,s,slot.getType(),r,chan.getSector(),chan.getModule(),
               chan.getCell(),chan.getSide());
      }
    }
  }
}

void HTofLookup::fill(Int_t crate, Int_t slot, Char_t modType,
        Int_t channel, Int_t sector, Int_t module, Int_t cell, Char_t side) {
  // Expands the array, if necessary, creates the objects of type HTofLookupCrate,
  // and then calls the fill function of the objects with index slot. 
  if (crate>maxCrate) {
    array->Expand(crate+1);
    for(Int_t i=maxCrate+1;i<=crate;i++) array->AddAt(new HTofLookupCrate(),i);
    maxCrate=crate;
  }
  (*this)[crate].fill(slot,modType,channel,sector,module,cell,side);
}

void HTofLookup::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ascii file I/O
  Int_t crate, slot, chan, sec, mod, cell;
  Char_t dcType[2], side[2];
  sscanf(buf,"%i%i%s%i%i%i%i%s",&crate,&slot,dcType,&chan,&sec,&mod,&cell,side);
  Int_t pos=sec*22+mod;
  if (set[pos]) {
    fill(crate,slot,dcType[0],chan,sec,mod,cell,side[0]);
    set[pos]=999;
  }
}

void HTofLookup::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HTofParAsciiFileIo
  header=
    "# Lookup table for the TOF unpacker\n"
    "# Format:\n"
    "# crate  slot  type  channel  sector  module  rod  side\n";
}

Bool_t HTofLookup::writeline(Char_t *buf, Int_t crate, Int_t slot, Int_t chan) {
  // writes one line to the buffer used by ascii file I/O
  HTofLookupSlot& s=(*this)[crate][slot];
  HTofLookupChan& c=s[chan];
  if (c.getSector()>=0) {
    sprintf(buf,"%4i%4i   %c%4i%4i%4i%4i   %c\n",
          crate,slot,s.getType(),chan,c.getSector(),c.getModule(),c.getCell(),
          c.getSide());
    return kTRUE;
  }
  return kFALSE;
}
