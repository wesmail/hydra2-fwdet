//*-- AUTHOR Ilse Koenig
//*-- Created : 17/01/2005

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HWallLookup
//
//  Container class for mapping Crate/TdcAdc/Channel to Cell
//  needed by the unpacker for the Forward Wall
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hwalllookup.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HWallLookupChan)
ClassImp(HWallLookupSlot)
ClassImp(HWallLookupCrate)
ClassImp(HWallLookup)

HWallLookupSlot::HWallLookupSlot(Int_t nChan) {
  // constructor creates an array of nChan pointers of type HWallLookupChan
  array = new TObjArray(nChan);
  for(Int_t i=0;i<nChan;i++) array->AddAt(new HWallLookupChan(),i);
  nChannels=32;
  maxChannel=nChan-1;
  clear();
}

HWallLookupSlot::~HWallLookupSlot(void) {
  // destructor
  array->Delete();
  delete array;
}

void HWallLookupSlot::clear(void) {
  // sets the module type to 'U' (unknown) and clears the array
  modType='U';
  for(Int_t i=0;i<=maxChannel;i++) (*this)[i].clear();
}

void HWallLookupSlot::fill(Char_t dcType, Int_t channel, Int_t cell) {
  // Sets the module type and fills the HLookupChan objects with index channel
  // The array is automatically expanded to 32 channels. 
  modType=dcType;
  if (channel>maxChannel) {
    array->Expand(nChannels);
    for(Int_t i=maxChannel+1;i<nChannels;i++) array->AddAt(new HWallLookupChan(),i);
    maxChannel=nChannels-1;
  }
  (*this)[channel].setCell(cell);
}

HWallLookupCrate::HWallLookupCrate(Int_t numSlots) {
  // constructor creates an array of pointers of type HWallLookupSlot
  nSlots=21;
  maxSlot=numSlots-1;
  array = new TObjArray(numSlots);
  for(Int_t i=0;i<numSlots;i++) array->AddAt(new HWallLookupSlot(),i);
}

HWallLookupCrate::~HWallLookupCrate(void) {
  // destructor
  array->Delete();
  delete array;
}

void HWallLookupCrate::fill(Int_t slot, Char_t modType, Int_t channel, Int_t cell) {
  // Expands the array, if necessary, creates the objects of type HWallLookupSlot,
  // and then calls the fill function of the objects with index slot. 
  if (slot>maxSlot) {
    array->Expand(slot+1);
    for(Int_t i=maxSlot+1;i<=slot;i++) array->AddAt(new HWallLookupSlot(),i);
    maxSlot=slot;
  }
  (*this)[slot].fill(modType,channel,cell);
}

HWallLookup::HWallLookup(const Char_t* name,const Char_t* title,
                       const Char_t* context,
                       Int_t nCrates, Int_t nSlots)
           : HParSet(name,title,context) {
  // The constructor creates an array of pointers of type HWallLookupCrate of
  // size nSlots
  // The container name is set to "WallLookup".
  strcpy(detName,"Wall");
  maxCrate=nCrates-1;
  array = new TObjArray(nCrates);
  for (Int_t i=0;i<nCrates;i++) array->AddAt(new HWallLookupCrate(nSlots),i);
}

HWallLookup::~HWallLookup(void) {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HWallLookup::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HWallParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HWallLookup::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HWallParIo");
  if (out) return out->write(this);
  return -1;
}

void HWallLookup::clear(void) {
  // clears the container
  for(Int_t c=0;c<=maxCrate;c++) {
    for(Int_t s=0;s<(*this)[c].getSize();s++) (*this)[c][s].clear();
  }
  status=kFALSE;
  resetInputVersions();
}

void HWallLookup::printParam(void) {
  // prints the lookup table
  printf("Lookup table for the Forward Wall unpacker\n");
  printf("crate  slot  type  channel  cell\n");
  for(Int_t c=0;c<getSize();c++) {
    for(Int_t s=0;s<(*this)[c].getSize();s++) {
      HWallLookupSlot& slot=(*this)[c][s];
      for(Int_t r=0;r<slot.getSize();r++) {
        HWallLookupChan& chan=slot[r];
        if (chan.getCell()>=0) printf("%4i%4i   %c%4i%5i\n",
            c,s,slot.getType(),r,chan.getCell());
      }
    }
  }
}

void HWallLookup::fill(Int_t crate, Int_t slot, Char_t modType,
                       Int_t channel, Int_t cell) {
  // Expands the array, if necessary, creates the objects of type HWallLookupCrate,
  // and then calls the fill function of the objects with index slot. 
  if (crate>maxCrate) {
    array->Expand(crate+1);
    for(Int_t i=maxCrate+1;i<=crate;i++) array->AddAt(new HWallLookupCrate(),i);
    maxCrate=crate;
  }
  (*this)[crate].fill(slot,modType,channel,cell);
}

void HWallLookup::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ascii file I/O
  Int_t crate, slot, chan, cell;
  Char_t dcType[2];
  sscanf(buf,"%i%i%s%i%i",&crate,&slot,dcType,&chan,&cell);
  fill(crate,slot,dcType[0],chan,cell);
  if (set[0]) set[0]=999;
}

void HWallLookup::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HWallParAsciiFileIo
  header=
    "# Lookup table for the Forward Wall unpacker\n"
    "# Format:\n"
    "# crate  slot  type  channel  cell\n";
}

Bool_t HWallLookup::writeline(Char_t *buf, Int_t crate, Int_t slot, Int_t chan) {
  // writes one line to the buffer used by ascii file I/O
  HWallLookupSlot& s=(*this)[crate][slot];
  HWallLookupChan& c=s[chan];
  if (c.getCell()>=0) {
    sprintf(buf,"%4i%4i   %c%4i%5i\n",
	    crate,slot,s.getType(),chan,c.getCell());
    return kTRUE;
  }
  return kFALSE;
}
