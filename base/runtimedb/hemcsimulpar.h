#ifndef HEMCSIMULPAR_H
#define HEMCSIMULPAR_H

#include "hparcond.h"
#include "TArrayI.h"

class HEmcSimulPar : public HParCond {
protected:
  Int_t   numRows;       //! number of rows per module
  Int_t   numCols;       //! number of columns per module
  Int_t   numPmtTypes;   // number of photomultiplier types in lookup table
  TArrayI lookupPmtType; // lookup table for PMT types (0: no PMT, 1: 1.5inch, 2: 3inch)

public:
  HEmcSimulPar(const char* name="EmcSimulPar",
               const char* title="Emc simulation parameters",
	       const char* context="EmcSimulProduction");

  ~HEmcSimulPar() {;}

  Int_t pmIndex(Int_t sec,Int_t cell)          { return sec*numRows*numCols + cell; }
  Int_t pmIndex(Int_t sec,Int_t row,Int_t col) { return sec*numRows*numCols + row*numCols + col; }

  void setNumPmtTypes(const Int_t n)                 { numPmtTypes=n;}
  void setPmtType(Int_t sec,Int_t cell,Int_t pmtype) { lookupPmtType.SetAt(pmtype ,pmIndex(sec,cell)); }

  Int_t  getNumPmtTypes()                    const { return numPmtTypes; }
  Int_t  getPmtType(Int_t sec,Int_t cell)          { return lookupPmtType[pmIndex(sec,cell)]; }
  Int_t  getPmtType(Int_t sec,Int_t row,Int_t col) { return lookupPmtType[pmIndex(sec,row,col)]; }
  Bool_t getLookupArray(Int_t, Int_t*);
  
  void   clear(void);
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);
  void   printLookupPmtType(void);
  
  ClassDef(HEmcSimulPar,1) // Container for the EMC parameters in HGeant simulation
};

#endif /* !HEMCSIMULPAR_H */
