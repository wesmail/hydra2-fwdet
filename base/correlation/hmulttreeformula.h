//////////////////////////////////////////////////////////////////////////
//                                                                      //
// HMultTreeFormula                                                     //
//                                                                      //
// The multiplicity formula class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef HMultTreeFormula_H
#define HMultTreeFormula_H

#include "htreeformula.h"
#include "hcombinedindex.h"
#include "TTreeFormula.h"

class HMultTreeFormula : public TTreeFormula {
public:
   HMultTreeFormula();
   HMultTreeFormula(const Char_t *name,const Char_t *formula, TTree *tree);
   HMultTreeFormula(TTree* tree, TTreeFormula* fV1, TTreeFormula* fV2,
                    TTreeFormula* fV3, TTreeFormula* fSel);

   virtual  ULong_t GetLNdata();

   void SetVars(TTree* tree, TTreeFormula* fV1, TTreeFormula* fV2,
                TTreeFormula* fV3, TTreeFormula* fSel);

   void CreateMultCodes();

   HCombinedIndex& CreateCombinedIndex();

   Short_t* GetBranchCodes(){return  fBranchCodes;}
   Short_t* GetCodes(){return  fCodes;}

private:
   void UpdateCodes(HTreeFormula* fVar);
   void UpdateCodesFromCutG(HTreeFormula* fVar);
   void EliminateCutGCode();
   HCombinedIndex index;

   TTreeFormula *fVar1;  
   TTreeFormula *fVar2;  
   TTreeFormula *fVar3;  
   TTreeFormula *fSelect;

   Short_t fBranchCodes[100];
   Short_t fBranchLen[100];  

   Int_t ndata;

   ClassDef(HMultTreeFormula,1)  //The multiplicity Tree formula
};

#endif
