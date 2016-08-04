#ifndef HRPCCHARGEOFFSETPAR_H
#define HRPCCHARGEOFFSETPAR_H
#include "TArrayF.h"
#include "hparcond.h"

class HParamList;

class HRpcChargeOffsetPar : public HParCond {

protected:

    TArrayI fRunIDs;          // Array with Beggining-End of RunId slots. Last stores the last RunID
    TArrayF fChargeCorrPar;   // Array with Correction pars.
    //Int_t   fFirstRunID;      // First RunId ...I can reduced to a get Function
    //Int_t   fLastRunID;       // Value of the last run ID in the set
    Int_t   fCurrent;         // Current set of correction parameters; Starts from 0 up to fNRuns-1
    Int_t   fNRuns;           // number of sets of parameters


public:
    HRpcChargeOffsetPar(const Char_t* name ="RpcChargeOffsetPar",
	      const Char_t* title  ="Correction of RPC charge offsets",
	      const Char_t* context="RpcChargeOffsetParProduction");
    ~HRpcChargeOffsetPar();

    void     addNewSet(Float_t* pars, Int_t startRunID);// {fChargeCorrPar.Set((fCurrent+2)*6*64); fRunIDs.Set(fCurrent+2); fCurrent = +1;  for(Int_t i = 0; i < 64*6 ; i++) fChargeCorrPar.AddAt(pars[i], fCurrent*64*6); fRunIDs.AddAt(startRunID,fCurrent);fNRuns++;  }
    void     addFirstSet(Float_t* pars, Int_t startRunID);// {fChargeCorrPar.Set((fCurrent+2)*6*64); fRunIDs.Set(fCurrent+2); fCurrent = +1;  for(Int_t i = 0; i < 64*6 ; i++) fChargeCorrPar.AddAt(pars[i], fCurrent*64*6); fRunIDs.AddAt(startRunID,fCurrent);fNRuns++;  }
    void     addLastSet(Float_t* pars, Int_t startRunID,Int_t endRunID);// 
    void     addLastRunId(Int_t runId);
    Float_t  getPar(Int_t sec, Int_t TRB, Int_t MBO, Int_t DBO) const { return fChargeCorrPar[fCurrent*64*6 + sec*64 + MBO*16 + TRB*4 + DBO/2]; }
    Float_t  getPar(Int_t regNum) const { return fChargeCorrPar[fCurrent*64*6 + regNum]; }
    //Float_t* getPars(Int_t runID); //return the array with parameters for a certain runID
    Int_t    getFirstRunID() const { return fRunIDs[0]; }
    Int_t    getLastRunID()  const { return fRunIDs[fNRuns]; }
    Int_t    getNextRunID() const { return fRunIDs[fCurrent+1]; }
    Int_t    getCurrentSet() { return fCurrent; }
    Int_t    getNSets()      { return fNRuns; }

    //void    setPar(Int_t sec, Int_t col, Int_t cell, Int_t npar, Float_t par)  { fChargeCorrPar[ sec*2880 + col*480 + cell*15 + npar ] = par; }
    void     setPars(Int_t nrun, Float_t* pars, Int_t startRunID)  { for(Int_t i = 0; i < 64*6 ; i++) fChargeCorrPar.AddAt(pars[i], nrun*64*6); fRunIDs.AddAt(startRunID, nrun);  }
    Bool_t   init(HParIo* inp, Int_t* set);
    //Bool_t   init(HParIo* inp);
    void     putParams(HParamList*);
    Bool_t   getParams(HParamList*);
    Int_t    getSlot(Int_t runId);
    void     clear();

    ClassDef(HRpcChargeOffsetPar,1) // Container for the RPC hit finder parameters
};
#endif  /*!HRPCCHARGEOFFSETPAR_H*/
