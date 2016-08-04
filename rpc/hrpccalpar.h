#ifndef HRPCCALPAR_H
#define HRPCCALPAR_H

#include "TObject.h"
#include "TObjArray.h"
#include "hlocation.h"
#include "hparset.h"

class HRpcCalParCell: public TObject {

 private:
  Float_t rightTimeOffset;   //Time offset of the right channel    [TDC units]
  Float_t leftTimeOffset;    //Time offset of the left channel     [TDC units]
  Float_t rightTotOffset;    //ToT  offset of the right channel    [TDC units]
  Float_t leftTotOffset;     //ToT  offset of the left channel     [TDC units]
  Float_t tdc2time;          //Conversion from TDC units to time   [ns]
  Float_t tot2charge;        //Conversion from TOT units to charge [pC]
  Float_t t0;                //Overall offset                      [TDC units]

 public:
  HRpcCalParCell(void) {clear();}
  ~HRpcCalParCell(void) {}
  
  Float_t getRightTimeOffset() { return rightTimeOffset; }
  Float_t getLeftTimeOffset()  { return leftTimeOffset; }
  Float_t getRightTOTOffset()  { return rightTotOffset; }
  Float_t getLeftTOTOffset()   { return leftTotOffset; }
  Float_t getTDC2Time()        { return tdc2time; }
  Float_t getTOT2Charge()      { return tot2charge; }
  Float_t getT0()              { return t0;}

  void getAddress(Float_t& rtimoff, Float_t& ltimoff, Float_t& rtotoff, Float_t& ltotoff, Float_t& t2t,
		  Float_t& t2c,Float_t& tzero) {
    rtimoff = rightTimeOffset;
    ltimoff = leftTimeOffset;
    rtotoff = rightTotOffset;
    ltotoff = leftTotOffset;
    t2t     = tdc2time;
    t2c     = tot2charge;
    tzero   = t0;
  }

  void fill(const Float_t rtimoff, const Float_t ltimoff, const Float_t rtotoff, const Float_t ltotoff,
	    const Float_t t2t, const Float_t t2c, const Float_t tzero) {
    rightTimeOffset  =  rtimoff;
    leftTimeOffset   =  ltimoff;
    rightTotOffset   =  rtotoff;
    leftTotOffset    =  ltotoff;
    tdc2time         =  t2t;
    tot2charge       =  t2c;
    t0               =  tzero;
  }
  
  void fill(HRpcCalParCell& cell) {
    rightTimeOffset  =  cell.getRightTimeOffset();
    leftTimeOffset   =  cell.getLeftTimeOffset();
    rightTotOffset   =  cell.getRightTOTOffset();
    leftTotOffset    =  cell.getLeftTOTOffset();
    tdc2time         =  cell.getTDC2Time();
    tot2charge       =  cell.getTOT2Charge();
    t0               =  cell.getT0();
  }
  
  void setRightTimeOffset(Float_t rtimoff ) { rightTimeOffset = rtimoff; }
  void setLeftTimeOffset(Float_t ltimoff)   { leftTimeOffset = ltimoff;  }
  void setRightTOTOffset(Float_t rtotoff)   { rightTotOffset = rtotoff;  }
  void setLeftTOTOffset(Float_t ltotoff)    { leftTotOffset = ltotoff;   }
  void setTDC2Time(Float_t t2t)             { tdc2time = t2t;            } 
  void setTOT2Charge(Float_t t2c)           { tot2charge = t2c;          }
  void setT0(Float_t tzero)                 { t0 = tzero;                }

  void clear();
  ClassDef(HRpcCalParCell,1)  // Cell level of the RPC calibration parameters
};


class HRpcCalParCol: public TObject {
 private:
  TObjArray* array;    // pointer array containing HRpcCalParCell objects
  Int_t MaxCells;      //Maximum number of cells

 public:
  HRpcCalParCol();
  ~HRpcCalParCol();
  HRpcCalParCell& operator[](Int_t i) {
    return *static_cast<HRpcCalParCell*>((*array)[i]);
  }
  HRpcCalParCell* getCell(Int_t cell) {
    if (cell>=0 && cell<MaxCells) return &((*this)[cell]);
    else return 0;
  }
  Int_t getSize() { return array->GetEntries(); }
  void fill(Int_t,Float_t,Float_t,Float_t,Float_t,Float_t,Float_t,Float_t);
  ClassDef(HRpcCalParCol,1)   // Column level of the RPC calibration parameters
};


class HRpcCalParSec: public TObject {
 private:
  TObjArray* array;     // pointer array containing HRpcCalParCol objects
  Int_t MaxColumns;     //Maximum number of sectors

 public:
  HRpcCalParSec();
  ~HRpcCalParSec();

  HRpcCalParCol& operator[](Int_t i) {
    return *static_cast<HRpcCalParCol*>((*array)[i]);
  }
  HRpcCalParCol* getCol(Int_t col) {
    if (col>=0 && col<MaxColumns) return &((*this)[col]);
    else return 0;
  }
  Int_t getSize() { return array->GetEntries(); }
  void fill(Int_t,Int_t,Float_t,Float_t,Float_t,Float_t,Float_t,Float_t,Float_t);
  ClassDef(HRpcCalParSec,1) // Sector level of the RPC calibration parameters
};


class HRpcCalPar : public HParSet {
 private:
  TObjArray* array;       // array of pointers of type HRpcCalParSec
  Int_t MaxSectors;       //Maximum number of columns

  void printPause(void);
 public:
  HRpcCalPar(const Char_t* name="RpcCalPar",
	     const Char_t* title="Calibration parameters for the Rpc Detector",
	     const Char_t* context="RpcCalProduction");
  ~HRpcCalPar(void);
  HRpcCalParSec& operator[](Int_t i) {
    return *static_cast<HRpcCalParSec*>((*array)[i]);
  }
  HRpcCalParCol* getCol(Int_t sec, Int_t col) {
    if (sec>=0 && sec<MaxSectors) return (*this)[sec].getCol(col);
    else return 0;
  }
  Int_t getSize() { return array->GetEntries(); }
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  void clear();
  void printParam();
  void fill(Int_t,Int_t,Int_t,Float_t,Float_t,Float_t,Float_t,Float_t,Float_t,Float_t);
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  Bool_t writeline(Char_t*, Int_t, Int_t, Int_t);
  ClassDef(HRpcCalPar,1)  // Container for the RPC calibration parameters
};

#endif /* !HRPCCALPAR_H */
