#ifndef HRPCCELLSTATUSPAR_H
#define HRPCCELLSTATUSPAR_H

#include "TArrayI.h"
#include "hparcond.h"

class HParamList;

class HRpcCellStatusPar : public HParCond {
protected:
  TArrayI cellStatus; // Status of cell: 0=dead/off, 1=on
public:
  HRpcCellStatusPar(const Char_t* name   ="RpcCellStatusPar",
                    const Char_t* title  ="Cell status parameters for the Rpc Detector",
                    const Char_t* context="RpcCellStatusProduction");
  ~HRpcCellStatusPar() {;}

  Int_t getCellStatus(Int_t sec, Int_t col, Int_t cell) {return cellStatus[192*sec+32*col+cell];}
  void  setCellStatus(Int_t sec, Int_t col, Int_t cell, Int_t a);
  void  printCellParams();

  void   clear();
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);

  ClassDef(HRpcCellStatusPar,1) // Container for Rpc cell status parameters 
};

#endif /* !HRPCCELLSTATUSPAR_H */
