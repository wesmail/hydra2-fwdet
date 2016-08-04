//*-- AUTHOR : Ilse Koenig
//*-- Created : 03/12/2004

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////
//
//  HScsPar
//
//  Base class for Slow Control parameter containers
//  The derived classes must implement the protected default constructor
//  to set the container name and title and to add the channels in the
//  TObjArray pChannels.
//
///////////////////////////////////////////////////////////////////////////

#include "hscspar.h"
#include "hscschannel.h"
#include "hpario.h"
#include "hdetpario.h"

ClassImp(HScsPar)

HScsPar::HScsPar() {
  // Protected default constructor
  pChannels=0;
}

HScsPar::HScsPar(const Char_t* name,const Char_t* title) {
  // Constructor with container name and title
  SetName(name);
  SetTitle(title);
  pChannels=0;
}

HScsPar::~HScsPar() {
  // Destructor
  if (pChannels) {
    pChannels->Delete();
    delete pChannels;
    pChannels=0;
  }
}

Bool_t HScsPar::init(HParIo* io,Int_t* set) {
 // initializes the container from an input using the spectrometer interface class
  HDetParIo* input=io->getDetParIo("HSpecParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HScsPar::write(HParIo* io) {
 // writes the container to an output using the spectrometer interface class
  HDetParIo* output=io->getDetParIo("HSpecParIo");
  if (output) return output->write(this);
  return kFALSE;
}

void HScsPar::clear() {
  // Clears the channel data, but does not delete the channels
  if (pChannels) {
    for (Int_t i=0;i<pChannels->GetSize();i++) {
      HScsChannel* p=(HScsChannel*)(pChannels->At(i));
      if (p) p->clear();
    }
  }
}

void HScsPar::printParams() {
  // Prints the container to stdout
  if (pChannels) {
    for (Int_t i=0;i<pChannels->GetSize();i++) {
      HScsChannel* p=(HScsChannel*)(pChannels->At(i));
      if (p) p->print();
    }
  }
}

void HScsPar::write(fstream& fout) {
  // Writes the container to fstream
  if (pChannels) {
    for (Int_t i=0;i<pChannels->GetSize();i++) {
      HScsChannel* p=(HScsChannel*)(pChannels->At(i));
      if (p) p->write(fout);
    }
  }
}
