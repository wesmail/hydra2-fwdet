//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
// HEmcCalPar
//
// Container class for Emc calibration parameters
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hades.h"
#include "hdetpario.h"
#include "hpario.h"
#include "hspectrometer.h"
#include "hemccalpar.h"
#include "hemcdetector.h"
#include <iomanip>
#include <iostream>
#include <fstream>

ClassImp(HEmcCalPar)
ClassImp(HEmcCalParSec)
ClassImp(HEmcCalParCell)

void HEmcCalParCell::clear() {
  tdcSlope  = 1.F;
  tdcOffset = 0.F;
  adcSlope  = 1.F;
  adcOffset = 0.F;
}

void HEmcCalParCell::fill(Float_t ts, Float_t to, Float_t as, Float_t ao) {
  tdcSlope = ts;
  tdcOffset = to;
  adcSlope = as;
  adcOffset = ao;
}

void HEmcCalParCell::fill(Float_t* data) {
  tdcSlope  = data[0];
  tdcOffset = data[1];
  adcSlope  = data[2];
  adcOffset = data[3];
}

void HEmcCalParCell::fill(HEmcCalParCell& r) {
  tdcSlope  = r.getTdcSlope();
  tdcOffset = r.getTdcOffset();
  adcSlope  = r.getAdcSlope();
  adcOffset = r.getAdcOffset();
}


HEmcCalParSec::HEmcCalParSec(Int_t n) {
  // constructor creates an array of pointers of type HEmcCalParCell
  array = new TObjArray(n);
  for (Int_t i = 0 ; i < n ; i++) {
    array->AddAt(new HEmcCalParCell(), i);
  }
}

HEmcCalParSec::~HEmcCalParSec() {
  // destructor
  array->Delete();
  delete array;
}


HEmcCalPar::HEmcCalPar(const Char_t* name, const Char_t* title,
                             const Char_t* context)
                   : HParSet(name, title, context) {
  // constructor creates an array of pointers of type HEmcCalParSec
  strcpy(detName, "Emc");
  HEmcDetector* det = (HEmcDetector*)(gHades->getSetup()->getDetector("Emc"));
  Int_t nSec = det->getMaxSecInSetup();
  Int_t nComp = det->getMaxComponents();
  array = new TObjArray(nSec);
  for (Int_t i = 0; i < nSec; i++) {
    if (det->getModule(i, 0) > 0) {
      array->AddAt(new HEmcCalParSec(nComp), i);
    } else {
      array->AddAt(new HEmcCalParSec(0), i);
    }
  }
}

HEmcCalPar::~HEmcCalPar() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HEmcCalPar::init(HParIo* inp, Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input = inp->getDetParIo("HEmcParIo");
  if (input) return (input->init(this, set));
  return kFALSE;
}

Int_t HEmcCalPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out = output->getDetParIo("HEmcParIo");
  if (out) return out->write(this);
  return -1;
}

void HEmcCalPar::clear() {
  // clears the container
  for (Int_t i = 0; i < getSize(); i++) {
    HEmcCalParSec& sec = (*this)[i];
    for (Int_t j = 0; j < sec.getSize(); j++) sec[j].clear();
  }
  status = kFALSE;
  resetInputVersions();
}

void HEmcCalPar::printParams() {
  // prints the calibration parameters
  printf("Calibration parameters for the Emc\n");
  printf("sector  cell  TdcSlope  TdcOffset  AdcSlope  AdcOffset\n");
  Float_t data[4];
  for (Int_t i = 0; i < getSize(); i++) {
    HEmcCalParSec& sec = (*this)[i];
    for (Int_t j = 0; j < sec.getSize(); j++) {
      HEmcCalParCell& cell = sec[j];
      cell.getData(data);
      printf("%4i %4i %10.5f %10.3f %10.5f %10.3f\n",
             i, j, data[0], data[1], data[2], data[3]);
    }
  }
}

void HEmcCalPar::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ASCII file I/O
  Int_t sector, cell;
  Float_t data[4] = {0.F, 0.F, 0.F, 0.F};
  sscanf(buf, "%i%i%f%f%f%f", &sector, &cell, &data[0], &data[1], &data[2], &data[3]);
  if (!set[sector]) return;
  (*this)[sector][cell].fill(data);
  set[sector] = 999;
}

void HEmcCalPar::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HEmcParAsciiFileIo
  header = "# Calibration parameters for the Emc\n"
           "# Format:\n"
           "# sector   cell   TdcSlope  TdcOffset AdcSlope  AdcOffset\n";
}

void HEmcCalPar::write(fstream& fout) {
  Text_t buf[155];
  Float_t data[4];
  for (Int_t i = 0; i < getSize(); i++) {
    HEmcCalParSec& sector = (*this)[i];
    for (Int_t j = 0; j < sector.getSize(); j++) {
      HEmcCalParCell& cell = sector[j];
      cell.getData(data);
      sprintf(buf, "%4i %4i %10.5f %10.3f %10.5f %10.3f\n",
              i, j, data[0], data[1], data[2], data[3]);
      fout<<buf;
    }
  }  
}
