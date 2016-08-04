//*-- Author  : Pablo Cabanelas
//*-- Created : 23/08/2007

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////
// HRpcCalPar
//
// Container class for RPC calibration parameters
//
////////////////////////////////////////////////////////////////////


using namespace std;
#include "hrpccalpar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hrpcdetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>
#include <cstdlib>

ClassImp(HRpcCalPar)
  ClassImp(HRpcCalParSec)
  ClassImp(HRpcCalParCol)
  ClassImp(HRpcCalParCell)
  

  void HRpcCalParCell::clear() {
  rightTimeOffset  = 0.0;
  leftTimeOffset   = 0.0;
  rightTotOffset   = 0.0;
  leftTotOffset    = 0.0;
  tdc2time         = 1.0;
  tot2charge       = 1.0;
  t0               = 0.0;
}

HRpcCalParCol::HRpcCalParCol() {
  // Constructor creates an array of pointers of type HRpcCalParCell.
  MaxCells = 32;
  array = new TObjArray(MaxCells);
  for(Int_t i=0 ; i<MaxCells ; i++) array->AddAt(new HRpcCalParCell(),i);
}

HRpcCalParCol::~HRpcCalParCol(void) {
  // Destructor.
  if(array)
    {
      array->Delete();
      delete array;
    }
}

void HRpcCalParCol::fill(Int_t cell,Float_t rightTimeOffset,Float_t leftTimeOffset,Float_t rightTotOffset,Float_t leftTotOffset,
			 Float_t tdc2time,Float_t tot2charge,Float_t t0) {

  if (cell<MaxCells && cell>=0) {
    (*this)[cell].fill(rightTimeOffset,leftTimeOffset,rightTotOffset,leftTotOffset,tdc2time,tot2charge,t0);  
    // calls fill() method of HRpcCalParCell
  } else {
    Error("fill","Invalid address cell: %i",cell);
  }
  
}

HRpcCalParSec::HRpcCalParSec() {
  // Constructor creates an array of pointers of type HRpcCalParCol.
  MaxColumns = 6;  
  array = new TObjArray(MaxColumns);
  for(Int_t i=0 ; i<MaxColumns ; i++) array->AddAt(new HRpcCalParCol(),i);
}

HRpcCalParSec::~HRpcCalParSec(void) {
  // Destructor.
  if (array) {
    array->Delete();
    delete array;
  }
}

void HRpcCalParSec::fill(Int_t col,Int_t cell,Float_t rightTimeOffset,Float_t leftTimeOffset,Float_t rightTotOffset,Float_t leftTotOffset,
			 Float_t tdc2time,Float_t tot2charge,Float_t t0) {

  if (col<MaxColumns && col>=0) {
    (*this)[col].fill(cell,rightTimeOffset,leftTimeOffset,rightTotOffset,leftTotOffset,tdc2time,tot2charge,t0);  // calls fill() method of HRpcCalParCol
  } else {
    Error("fill","Invalid address column: %i",col);
  }
	
}

HRpcCalPar::HRpcCalPar(const Char_t* name,const Char_t* title,
		       const Char_t* context)
  : HParSet(name,title,context) {
  // Constructor creates an array of pointers of type HRpcCalParSec.
  // The container name is set to "RpcCalPar".
  strcpy(detName,"Rpc");

  MaxSectors = 6;
  array = new TObjArray(MaxSectors);

  for (Int_t i=0; i<MaxSectors; i++) array->AddAt(new HRpcCalParSec(),i);

}

HRpcCalPar::~HRpcCalPar(void) {
  // Destructor.
  array->Delete();
  delete array;
}

Bool_t HRpcCalPar::init(HParIo* inp,Int_t* set) {
  // Intitializes the container from an input.
  // If input not available, default initialization.
  HDetParIo* input=inp->getDetParIo("HRpcParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HRpcCalPar::write(HParIo* output) {
  // Writes the container to an output.
  HDetParIo* out=output->getDetParIo("HRpcParIo");
  if (out) return out->write(this);
  return -1;
}

void HRpcCalPar::clear() {
  // Clears the container 
  for(Int_t sec=0;sec<getSize();sec++) {  //size of sectors
    for(Int_t col=0;col<(*this)[sec].getSize();col++) {  //size of columns
      for(Int_t cell=0;cell<(*this)[sec][col].getSize();cell++) {  //size of cells
	(*this)[sec][col][cell].clear();
      }
    }
  }
  status=kFALSE;
  resetInputVersions();
}

void HRpcCalPar::fill(Int_t sec, Int_t col, Int_t cell, Float_t rightTimeOffset, Float_t leftTimeOffset, Float_t rightTotOffset, Float_t leftTotOffset,
		      Float_t tdc2time, Float_t tot2charge, Float_t t0 ) {

  if (sec<MaxSectors && sec>=0) {
    (*this)[sec].fill(col,cell,rightTimeOffset,leftTimeOffset,rightTotOffset,leftTotOffset,tdc2time,tot2charge,t0);  // calls fill() method of HRpcCalParSec
  } else {
    Error("fill","Invalid address sector: %i",sec);
  }

}

void HRpcCalPar::readline(const Char_t *buf, Int_t *set) {
  // Decodes one line read from ascii file I/O.
  Int_t sec,col,cell;
  Float_t rightTimeOffset,leftTimeOffset,rightTotOffset,leftTotOffset,tdc2time,tot2charge,t0;

  sscanf(buf,"%i %i %i %f %f %f %f %f %f %f", &sec, &col, &cell, &rightTimeOffset, &leftTimeOffset, &rightTotOffset, &leftTotOffset,
	 &tdc2time, &tot2charge, &t0);
  fill(sec,col,cell,rightTimeOffset,leftTimeOffset,rightTotOffset,leftTotOffset,tdc2time,tot2charge,t0);
  set[sec]=999;
}

void HRpcCalPar::putAsciiHeader(TString& header) {
  // Puts the ascii header to the string used in HRpcParAsciiFileIo.
  header=
    "########################################################################################################################"
    "# Calibration parameters for the RPC detector\n"
    "# Format:\n"
    "# Sector Column Cell Right.Time.Offset Left.Time.Offset Right.TOT.Offset Left.TOT.Offset TDC.to.TIME TOT.to.CHARGE T0\n"
    "########################################################################################################################";
}

Bool_t HRpcCalPar::writeline(Char_t *buf, Int_t sec, Int_t col, Int_t cell) {
  // Writes one line to the buffer used by ascii file I/O.
  HRpcCalParCell& pCell=(*this)[sec][col][cell];
  sprintf(buf,"%4i %4i %4i %12.4f %12.4f %12.4f %12.4f %12.4f %12.4f %12.4f\n",
	  sec, col, cell,
	  pCell.getRightTimeOffset(), pCell.getLeftTimeOffset(), pCell.getRightTOTOffset(), pCell.getLeftTOTOffset(),
	  pCell.getTDC2Time(), pCell.getTOT2Charge(), pCell.getT0());
  return kTRUE;  
}

void HRpcCalPar::printPause(void) {
  // Pauses printing.
  printf("Press any key to continue");
  getchar();
  printf("\n\n");
}

void HRpcCalPar::printParam(void) {
  // Prints the parameters.
  printf(" Calibration parameters for the RPC detector\n");
  printf(" Format:\n");
  printf(" Sector Column Cell Right.Time.Offset Left.Time.Offset Right.TOT.Offset Left.TOT.Offset TDC.to.TIME TOT.to.CHARGE T0\n");
  Text_t buf[155];
  Int_t n0 = getSize();
  for(Int_t i0=0; i0<n0; i0++) {
    Int_t n1 = (*this)[i0].getSize();
    for(Int_t i1=0; i1<n1; i1++) {
      Int_t n2 = (*this)[i0][i1].getSize();
      for(Int_t i2=0; i2<n2; i2++) {
	if (writeline(buf, i0, i1, i2)) printf(buf,strlen(buf));
      }
    }
  }
}
