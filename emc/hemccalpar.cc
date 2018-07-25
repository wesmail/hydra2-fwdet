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
  twcPar1   = 0.F;
  twcPar2   = 0.F;
  twcPar3   = 0.F;
}

void HEmcCalParCell::fill(Float_t ts, Float_t to, Float_t as, Float_t ao, Float_t t1, Float_t t2, Float_t t3) {
  tdcSlope = ts;
  tdcOffset = to;
  adcSlope = as;
  adcOffset = ao;
  twcPar1 = t1;
  twcPar2 = t2;
  twcPar3 = t3;
}

void HEmcCalParCell::fill(Float_t* data) {
  tdcSlope  = data[0];
  tdcOffset = data[1];
  adcSlope  = data[2];
  adcOffset = data[3];
  twcPar1   = data[4];
  twcPar2   = data[5];
  twcPar3   = data[6];
}

void HEmcCalParCell::fill(HEmcCalParCell& r) {
  tdcSlope  = r.getTdcSlope();
  tdcOffset = r.getTdcOffset();
  adcSlope  = r.getAdcSlope();
  adcOffset = r.getAdcOffset();
  twcPar1   = r.getTwcPar1();
  twcPar2   = r.getTwcPar2();
  twcPar3   = r.getTwcPar3();
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
  if(array)
  {
      array->Delete();
      delete array;
  }
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
  printf(" sector(0...5)   position(0...162)  TdcSlope  TdcOffset  AdcSlope  AdcOffset  Twc0  Twc1  Twc2\n");
  Float_t data[7];
  Int_t mountpos=-1;
  for (Int_t i = 0; i < getSize(); i++) {
    HEmcCalParSec& sec = (*this)[i];
    for (Int_t j = 0; j < sec.getSize(); j++) {
      HEmcCalParCell& cell = sec[j];
      cell.getData(data);
      mountpos=HEmcDetector::getPositionFromCell(j);
      if(mountpos<=0) continue;
      printf("%4i %4i %10.5f %10.3f %10.5f %10.3f %10.3f %10.3f %10.3f\n",
             i+1, j, data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
    }
  }
}

void HEmcCalPar::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ASCII file I/O
  Int_t sector, cell, position;
  Float_t data[7] = {0.F, 0.F, 0.F, 0.F, 0.F, 0.F, 0.F};
  Int_t n = sscanf(buf, "%i%i%f%f%f%f%f%f%f",
         &sector, &position, &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6]);
  if (n<9)
  {
      Error("readline", "Not enough values in line %s\n", buf);
  }
  else if (n>9)
  {
      Error("readline", "Too many values in line %s\n", buf);
  }
  else
  {

    if(sector<0 || sector>5) return;
    if (!set[sector]) return;
    cell=HEmcDetector::getCellFromPosition(position);
    if(cell<0)
    {
      Error("readline", "EMC cell not defined for mounting position  %d\n", position);
      return;
    }
    (*this)[sector][cell].fill(data);
    set[sector] = 999;
  }
}

void HEmcCalPar::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HEmcParAsciiFileIo
  header = "# Calibration parameters for the Emc\n"
           "# Format:\n"
           "# sector(0...5)   position(0...162)   TdcSlope  TdcOffset AdcSlope  AdcOffset  Twc0  Twc1  Twc2\n";
}

void HEmcCalPar::write(fstream& fout) {
  Text_t buf[155];
  Float_t data[7];
  Int_t mountpos=-1;
  for (Int_t i = 0; i < getSize(); i++) {
    HEmcCalParSec& sector = (*this)[i];
    for (Int_t j = 0; j < sector.getSize(); j++) {
      HEmcCalParCell& cell = sector[j];
      cell.getData(data);
      mountpos=HEmcDetector::getPositionFromCell(j);
      if(mountpos<=0) continue;
      sprintf(buf, "%4i %4i %10.5f %10.3f %10.5f %10.3f %10.3f %10.3f %10.3f\n",
              i+1, mountpos, data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
      fout<<buf;
    }
  }  
}
