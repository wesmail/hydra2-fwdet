#ifndef HRPCLOOKUPGEOM_H
#define HRPCLOOKUPGEOM_H

#include "TNamed.h"
#include "TObjArray.h"
#include "hparset.h"

class HParIo;

class HRpcLookupGCell: public TObject {
protected:
    Int_t fLeftFeeAddress;        //left fee addr from lookuptable
    Int_t fRightFeeAddress;       //right fee addr from lookuptable
public:
    HRpcLookupGCell() {
	fLeftFeeAddress  = -1;
	fRightFeeAddress = -1;
    }
    ~HRpcLookupGCell() {}

    void fill(Int_t lFee=-1,Int_t rFee=-1) {
	fLeftFeeAddress  = lFee;
	fRightFeeAddress = rFee;
    }
    Int_t getLeftMBOnum();
    Int_t getLeftDBOnum();
    Int_t getLeftTRBnum();
    Int_t getRightMBOnum();
    Int_t getRightDBOnum();
    Int_t getRightTRBnum();
    void  getTrbMboDboLeftNum(Int_t& trbnum,Int_t& mbonum,Int_t& dbonum);
    void  getTrbMboDboRightNum(Int_t& trbnum,Int_t& mbonum,Int_t& dbonum);
    Int_t getLeftFEEAddress( )  { return fLeftFeeAddress; }
    Int_t getRightFEEAddress( ) { return fRightFeeAddress; }
    void  getLeftRightRegAdd(Int_t sec, Int_t& leftRegNum, Int_t& rightRegNum);
    void  setLeftFEEAddress(Int_t ladd = -1)  { fLeftFeeAddress  = ladd; }
    void  setRightFEEAddress(Int_t radd = -1) { fRightFeeAddress = radd; }

    void clear() {
	fLeftFeeAddress  = -1;
	fRightFeeAddress = -1;
    }
    ClassDef(HRpcLookupGCell,1) // RPC cell geom->FEE
};


class HRpcLookupGCol: public TObject {
protected:
    TObjArray* array;
public:
    HRpcLookupGCol(Int_t n = 0);
    ~HRpcLookupGCol();
    HRpcLookupGCell& operator[](Int_t i) {
	return *static_cast<HRpcLookupGCell*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HRpcLookupGCol,1) // Layer level of RPC lookup table (cell->channel)
};

class HRpcLookupGSec: public TObject {
protected:
  TObjArray* array;
public:
    HRpcLookupGSec(Int_t n = 0);
    ~HRpcLookupGSec();
    HRpcLookupGCol& operator[](Int_t i) {
	return *static_cast<HRpcLookupGCol*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HRpcLookupGSec,1) // Sector level of RPC lookup table (cell->channel)
};


class HRpcLookupGeom : public HParSet {
protected:
  TObjArray* array;     // array of pointers
public:
    HRpcLookupGeom(const Char_t* name="RpcLookupGeom",
				   const Char_t* title="Rpc lookup table from geom to channel",
				   const Char_t* context="",
				   Int_t n = 6);
    ~HRpcLookupGeom();
    HRpcLookupGSec& operator[](Int_t i) {
	return *static_cast<HRpcLookupGSec*>((*array)[i]);
    }
    Bool_t init(HParIo* input);
    Int_t  write(HParIo* output);
    Bool_t writeline(Char_t *, Int_t, Int_t, Int_t);
    void   putAsciiHeader(TString&);
    Int_t  getSize() {return array->GetEntries();}
    void   clear();
    ClassDef(HRpcLookupGeom,1) //  RPC lookup table (cell->channel)
};


















#endif /* !HRPCLOOKUPGEOM_H */

