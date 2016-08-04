//////////////////////////////////////////////////////////////////////////
//                                                                      //
// HMultTreeFormula                                                     //
//                                                                      //
// The multiplicity formula class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef HTreeFormula_H
#define HTreeFormula_H

#include "TTreeFormula.h"
#include "hcombinedindex.h"
#include "hcutg.h"

class HTreeFormula : public TTreeFormula {
public:
   HTreeFormula();
   HTreeFormula(const Char_t *name,const Char_t *formula, TTree *tree);

   virtual Double_t EvalInstance(HCombinedIndex* pInstancesIndex);
   virtual void GroupLeaves();

   Short_t* GetBranchCodes(){return  fBranchCodes;}
   Short_t* GetCodes(){return  fCodes;}

   HCutG* GetCutG(int);
private:
   Int_t LookForGroupCode(Char_t *, int);

   Short_t fBranchCodes[100]; //table of codes of the same branch
   Short_t fInstances[100];
   ClassDef(HTreeFormula,1)  //The multiplicity Tree formula
};

#endif
