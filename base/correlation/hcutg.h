//////////////////////////////////////////////////////////////////////////
//                                                                      //
// HMultTreeFormula                                                     //
//                                                                      //
// The multiplicity formula class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef HCutG_H
#define HCutG_H

#include "TCutG.h"
#include "TTreeFormula.h"
#include "hcombinedindex.h"

class TTreeFormula;
class HCutG : public TCutG {
public:
   HCutG();
   HCutG(const Text_t *name, Int_t n, Float_t *x, Float_t *y);
   HCutG(TCutG* pCut, const Text_t *name);

   TTreeFormula* GetFormulaX(){return (TTreeFormula*)TCutG::GetObjectX();}
   TTreeFormula* GetFormulaY(){return (TTreeFormula*)TCutG::GetObjectY();}

   virtual void GroupLeaves();
   virtual Double_t EvalInstance(HCombinedIndex* pInstancesIndex);
   void Convert(TCutG* pCut);

private:
   ClassDef(HCutG,1)  //The multiplicity Tree formula
};

#endif
