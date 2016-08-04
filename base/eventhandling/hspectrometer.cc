//*-- AUTHOR : Ilse Koenig
//*-- Modified : 30/12/02 by Ilse Koenig
//*-- Modified : 11/12/01 by Ilse Koenig
//*-- Modified : 29/07/99 by Ilse Koenig
//*-- Modified : 10/05/01 by Dan Magestro

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HSpectrometer
//
//  class for the actual setup of the HADES spectrometer used
//  in the analysis.
//  It contains the list of defined detectors.
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hspectrometer.h"
#include "hdetector.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hpario.h"
#include "hparrootfileio.h"
#include "hdetpario.h"
#include "hcondparrootfileio.h"
#include "hspecparrootfileio.h"
#include "hparasciifileio.h"
#include "hcondparasciifileio.h"
#include "hspecparasciifileio.h"
#include "hgeomshapes.h"
#include "TClass.h"
#include <iostream> 
#include <iomanip>
#include <stdlib.h>

ClassImp(HSpectrometer)

HSpectrometer::HSpectrometer(void) : TNamed("Hades","The Hades spectrometer") {
  // constructor creates an empty list of detectors
  detectors=new TList();
  changed=kFALSE;
  shapes = 0;
}

HSpectrometer::~HSpectrometer(void) {
  // destructor
  detectors->Delete();
  delete detectors;
  if (shapes) { delete shapes; shapes = 0; }
}

void HSpectrometer::addDetector(HDetector* det) {
  // adds a detector to the list of detectors
  detectors->Add(det);
  changed=kTRUE;
}

HDetector* HSpectrometer::getDetector(const Char_t* name) {
  // returns a pointer to the detector called by name
  return ((HDetector*)detectors->FindObject(name));
}

Bool_t HSpectrometer::init(void) {
  // Calls the init function of each detector in the spectrometer.
  // The string level indicates the start level of the analysis
  // Each detector (as e.g. MDC) which needs special initialization
  // before creating the tasks must have an init(...)-function
  TIter next(detectors);
  HDetector *det=0;
  while ((det=(HDetector *)next())) {
    if (!det->init()) return kFALSE;
  }
  return kTRUE;
}

void HSpectrometer::activateParIo(HParIo* io) {
  // activates the spectrometer I/O
  // loops over the list of detectors and activates the
  // corresponding detector I/Os
  const Char_t* ioName=io->IsA()->GetName();
  if (strcmp(ioName,"HParOraIo")==0) {
    io->setDetParIo("HCondParIo");
    io->setDetParIo("HSpecParIo");
  } else {
    if (strcmp(ioName,"HParRootFileIo")==0) {
      HDetParRootFileIo* p=
          new HCondParRootFileIo(((HParRootFileIo*)io)->getParRootFile());
      io->setDetParIo(p);
      p=new HSpecParRootFileIo(((HParRootFileIo*)io)->getParRootFile());
      io->setDetParIo(p);
    } else {
      if (strcmp(ioName,"HParAsciiFileIo")==0) {
        HDetParAsciiFileIo* p=
            new HCondParAsciiFileIo(((HParAsciiFileIo*)io)->getFile());
        io->setDetParIo(p);
        p=new HSpecParAsciiFileIo(((HParAsciiFileIo*)io)->getFile());
        io->setDetParIo(p);
      }
    }
  }
  TIter next(detectors);
  HDetector* det;
  while ((det=(HDetector*)next())) {
    det->activateParIo(io);
  }
}

Bool_t HSpectrometer::write() {
  // writes the actual setup to the output defined in the
  // runtime database
  HParIo* io=gHades->getRuntimeDb()->getOutput();
  if (io) return write(io);
  cerr<<"actual setup couldn't be written to output"<<endl;
  return kFALSE;
}

Bool_t HSpectrometer::write(HParIo* io) {
  // writes the actual setup to the output
  TIter next(detectors);
  HDetector* det=0;
  Bool_t rc=kTRUE;
  while ((det=(HDetector*)next())) {
    if (det->write(io)==kFALSE) rc=kFALSE;
  }
  if (rc==kTRUE) changed=kFALSE;
  else cerr<<"actual setup couldn't be written to output"<<endl;
  return rc;
}

HGeomShapes* HSpectrometer::getShapes() {
  if (shapes==0) shapes=new HGeomShapes();
  return shapes;
}

void HSpectrometer::print() {
  // prints the actual setup
  cout<<"Actual setup for Hades Spectrometer:"<<"\n";
  TIter next(detectors);
  HDetector* det=0;
  while ((det=(HDetector*)next())) {
    det->print();
  }
}

void HSpectrometer::Streamer(TBuffer &R__b)
{
   // Stream an object of class HSpectrometer

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> detectors;
      R__b >> shapes;
      if (detectors->GetSize() == 0) {
                  printf(" detectors list empty ! \n");
                  exit(0);
                    }
   } else {
      R__b.WriteVersion(HSpectrometer::IsA());
      TObject::Streamer(R__b);
      R__b << detectors;
      R__b << shapes;

   }
}

