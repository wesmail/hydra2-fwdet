//*-- AUTHOR Rainer Schicker
//*-- modified: 15/08/2005 by Filip Krizek
//*-- modified: 18/12/2001 by Ilse Koenig
//*-- modified: 06/03/2000 by Ilse Koenig
//*-- created : 18/02/00

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
// HWallCalPar
//
// Container class for Forward Wall calibration parameters
//
/////////////////////////////////////////////////////////////


using namespace std;
#include "hwallcalpar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hwalldetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream>

ClassImp(HWallCalPar)
ClassImp(HWallCalParCell)

HWallCalPar::HWallCalPar(const Char_t* name,const Char_t* title,
                         const Char_t* context)
             : HParSet(name,title,context) {
  // constructor creates an array of pointers of type HWallCalParCell
  strcpy(detName,"Wall");
  HDetector* det= gHades->getSetup()->getDetector("Wall");
  Int_t nComp=det->getMaxComponents(); //302 cells
  array = new TObjArray(nComp);
  for(Int_t i=0; i<nComp; i++) array->AddAt(new HWallCalParCell(),i);
}

HWallCalPar::~HWallCalPar() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HWallCalPar::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HWallParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HWallCalPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HWallParIo");
  if (out) return out->write(this);
  return -1;
}

void HWallCalPar::clear() {
  // clears the container
  for(Int_t i=0;i<getSize();i++) (*this)[i].clear();
  status=kFALSE;
  resetInputVersions();
}

void HWallCalPar::printParam() {
  // prints the calibration parameters
  printf("Calibration parameters for the Wall detector\n");
  printf("cell TDC_K TDC_offset ADC_K ADC_offset\n");
  for(Int_t i=0;i<getSize();i++) {
    HWallCalParCell& cell=(*this)[i];
    printf("%i  %8.3f %9.3f %7.4f %7.4f %8.3f %9.3f \n",i,cell.getTDC_Slope(),cell.getTDC_Offset(),cell.getTDC_WalkCorr1(),cell.getTDC_WalkCorr2(),cell.getADC_Slope(),cell.getADC_Offset());
  }
}

void HWallCalPar::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ascii file I/O
  Int_t  cell;
  Float_t TDC_slope, TDC_offset, TDC_WalkCorr1, TDC_WalkCorr2, ADC_slope, ADC_offset;
  sscanf(buf,"%i%f%f%f%f%f%f", &cell, &TDC_slope, &TDC_offset, &TDC_WalkCorr1, &TDC_WalkCorr2, &ADC_slope, &ADC_offset);
  if (!set[0]) return;
  (*this)[cell].fill(TDC_slope,TDC_offset,TDC_WalkCorr1,TDC_WalkCorr2,ADC_slope,ADC_offset); 
  set[0]=999;
}

void HWallCalPar::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HWallParAsciiFileIo
  header=
    "# Calibration parameters for the WALL detector\n"
    "# Format:\n"
    "# cell  TDC_slope   TDC_offset   TDC_WalkCorr1   TDC_WalkCorr2   ADC_slope   ADC_offset\n";
}

Bool_t HWallCalPar::writeline(Char_t *buf,Int_t cell) {
  // writes one line to the buffer used by ascii file I/O
  HWallCalParCell& c=(*this)[cell];
  sprintf(buf,"%i %8.3f %9.3f %7.4f %7.4f %8.3f %9.3f\n",
          cell, c.getTDC_Slope(),c.getTDC_Offset(),c.getTDC_WalkCorr1(),c.getTDC_WalkCorr2(),c.getADC_Slope(),c.getADC_Offset());
  return kTRUE;
}
  






