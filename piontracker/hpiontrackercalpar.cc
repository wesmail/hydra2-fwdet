//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
// HPionTrackerCalPar
//
// Container class for PionTracker calibration parameters
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hades.h"
#include "hdetpario.h"
#include "hpario.h"
#include "hspectrometer.h"
#include "hpiontrackercalpar.h"
#include "hpiontrackerdetector.h"
#include <iomanip>
#include <iostream>
#include <fstream>

ClassImp(HPionTrackerCalPar)
ClassImp(HPionTrackerCalParMod)
ClassImp(HPionTrackerCalParCell)

void HPionTrackerCalParCell::clear() {
  tdcSlope  = 1.F;
  tdcOffset = 0.F;
  adcSlope  = 1.F;
  adcOffset = 0.F;
}

void HPionTrackerCalParCell::fill(Float_t ts, Float_t to, Float_t as, Float_t ao) {
  tdcSlope = ts;
  tdcOffset = to;
  adcSlope = as;
  adcOffset = ao;
}

void HPionTrackerCalParCell::fill(Float_t* data) {
  tdcSlope = data[0];
  tdcOffset = data[1];
  adcSlope = data[2];
  adcOffset = data[3];
}

void HPionTrackerCalParCell::fill(HPionTrackerCalParCell& r) {
  tdcSlope  = r.getTdcSlope();
  tdcOffset = r.getTdcOffset();
  adcSlope  = r.getAdcSlope();
  adcOffset = r.getAdcOffset();
}


HPionTrackerCalParMod::HPionTrackerCalParMod(Int_t n) {
  // constructor creates an array of pointers of type HPionTrackerCalParCell
  array = new TObjArray(n);
  for (Int_t i = 0 ; i < n ; i++) {
    array->AddAt(new HPionTrackerCalParCell(), i);
  }
}

HPionTrackerCalParMod::~HPionTrackerCalParMod() {
  // destructor
  array->Delete();
  delete array;
}


HPionTrackerCalPar::HPionTrackerCalPar(const Char_t* name, const Char_t* title,
                             const Char_t* context)
                   : HParSet(name, title, context) {
  // constructor creates an array of pointers of type HPionTrackerCalParMod
  strcpy(detName, "PionTracker");
  HPionTrackerDetector* det = (HPionTrackerDetector*)(gHades->getSetup()->getDetector("PionTracker"));
  Int_t nMod = det->getMaxModInSetup();
  Int_t nComp = det->getMaxComponents();
  array = new TObjArray(nMod);
  for (Int_t i = 0; i < nMod; i++) {
    if (det->getModule(-1, i) > 0) {
      array->AddAt(new HPionTrackerCalParMod(nComp), i);
    } else {
      array->AddAt(new HPionTrackerCalParMod(0), i);
    }
  }
}

HPionTrackerCalPar::~HPionTrackerCalPar() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HPionTrackerCalPar::init(HParIo* inp, Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input = inp->getDetParIo("HPionTrackerParIo");
  if (input) return (input->init(this, set));
  return kFALSE;
}

Int_t HPionTrackerCalPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out = output->getDetParIo("HPionTrackerParIo");
  if (out) return out->write(this);
  return -1;
}

void HPionTrackerCalPar::clear() {
  // clears the container
  for (Int_t i = 0; i < getSize(); i++) {
    HPionTrackerCalParMod& mod = (*this)[i];
    for (Int_t j = 0; j < mod.getSize(); j++) mod[j].clear();
  }
  status = kFALSE;
  resetInputVersions();
}

void HPionTrackerCalPar::printParams() {
  // prints the calibration parameters
  printf("Calibration parameters for the PionTracker\n");
  printf("module  strip  TdcSlope  TdcOffset  AdcSlope  AdcOffset\n");
  Float_t data[4];
  for (Int_t i = 0; i < getSize(); i++) {
    HPionTrackerCalParMod& mod = (*this)[i];
    for (Int_t j = 0; j < mod.getSize(); j++) {
      HPionTrackerCalParCell& cell = mod[j];
      cell.getData(data);
      printf("%4i %4i %10.5f %10.3f %10.5f %10.3f\n",
             i, j, data[0], data[1], data[2], data[3]);
    }
  }
}

void HPionTrackerCalPar::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ASCII file I/O
  Int_t mod, strip;
  Float_t data[4] = {0.F, 0.F, 0.F, 0.F};
  sscanf(buf, "%i%i%f%f%f%f", &mod, &strip, &data[0], &data[1], &data[2], &data[3]);
  if (!set[mod]) return;
  (*this)[mod][strip].fill(data);
  set[mod] = 999;
}

void HPionTrackerCalPar::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HPionTrackerParAsciiFileIo
  header = "# Calibration parameters for the PionTracker\n"
           "# Format:\n"
           "# module   strip   TdcSlope  TdcOffset AdcSlope  AdcOffset\n";
}

void HPionTrackerCalPar::write(fstream& fout) {
  Text_t buf[155];
  Float_t data[4];
  for (Int_t i = 0; i < getSize(); i++) {
    HPionTrackerCalParMod& mod = (*this)[i];
    for (Int_t j = 0; j < mod.getSize(); j++) {
      HPionTrackerCalParCell& cell = mod[j];
      cell.getData(data);
      sprintf(buf, "%4i %4i %10.5f %10.1f %10.5f %10.1f\n",
              i, j, data[0], data[1], data[2], data[3]);
      fout<<buf;
    }
  }
  
}
