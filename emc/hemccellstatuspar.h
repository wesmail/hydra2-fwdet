#ifndef HEMCCELLSTATUSPAR_H
#define HEMCCELLSTATUSPAR_H

#include "TArrayI.h"
#include "hparcond.h"

class HParamList;

class HEmcCellStatusPar : public HParCond {
protected:
  TArrayI cellStatus; // Status of cell: 0=dead/off, 1=on
public:
  HEmcCellStatusPar(const Char_t* name   ="EmcCellStatusPar",
                    const Char_t* title  ="Cell status parameters for the Emc Detector",
                    const Char_t* context="EmcCellStatusProduction");
  ~HEmcCellStatusPar() {;}

  Int_t getCellStatus(Int_t sec, Int_t cell) {return cellStatus[163*sec+cell];}
  void  setCellStatus(Int_t sec, Int_t cell, Int_t a);
  void  printCellParams();

  void   clear();
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);

  ClassDef(HEmcCellStatusPar,1) // Container for Emc cell status parameters 
};

#endif /* !HEMCCELLSTATUSPAR_H */
