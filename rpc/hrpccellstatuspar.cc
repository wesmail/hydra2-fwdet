//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : I. Koenig
////////////////////////////////////////////////////////////////////////////
// HRpcCellStatusPar
//
// Container class for RPC cell status parameters
// (Condition Style)
//
// cellStatus: Lookup table for the cell status
//             0: cell is OFF (delivers no signal on one or both sides)
//             1: cell is ON  (delivers signals on both sides)
//             By default all cells are ON.
//             Used in RPC hit finder
//
////////////////////////////////////////////////////////////////////////////

#include "hrpccellstatuspar.h"
#include "hparamlist.h"

using namespace std;
#include <iostream>

ClassImp(HRpcCellStatusPar)

HRpcCellStatusPar::HRpcCellStatusPar(const Char_t* name,const Char_t* title,
                                     const Char_t* context)
                 : HParCond(name,title,context) {
  cellStatus.Set(1152);
  clear();
}

void HRpcCellStatusPar::clear() {
  // Sets all cells ON
  cellStatus.Reset(1);
  status=kFALSE;
  resetInputVersions();
  changed=kFALSE;
}

void  HRpcCellStatusPar::setCellStatus(Int_t sec, Int_t col, Int_t cell, Int_t a) {
  // Sets the cell status
  if (sec<0||sec>5||col<0||col>5||cell<0||cell>31) {
    Error("setCellStatus","Address out of bounds");
  } else {
    cellStatus[192*sec+32*col+cell]=a;
  }
}

void HRpcCellStatusPar::putParams(HParamList* l) {
  // Puts all parameters to the list used by the io;
  if (!l) return;
  l->add("cellStatus", cellStatus);
}

Bool_t HRpcCellStatusPar::getParams(HParamList* l) {
  // Retrieves from the list all parameters needed
  if (!l) return kFALSE;
  if (!(l->fill("cellStatus", &cellStatus))) return kFALSE;
  return kTRUE;
}
 
void HRpcCellStatusPar::printCellParams() {
  // Shows the status of all cells
  cout<<"---- Format: sector  column  status of cells ----"<<endl;  
  for(Int_t sec=0;sec<6;sec++) {
    for(Int_t col=0;col<6;col++) {
      cout<<sec<<" "<<col<<"   ";
      for(Int_t cell=0;cell<32;cell++) {
        cout<<" "<<getCellStatus(sec,col,cell);
      }
      cout<<endl;
    }
  }
}
