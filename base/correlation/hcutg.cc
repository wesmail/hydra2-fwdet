//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// HCutG                                                                //
//                                                                      //
// The multiplicity formula class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "htreeformula.h"
#include "hcutg.h"
#include "TGraph.h"
#include "TCutG.h"

ClassImp(HCutG)


HCutG::HCutG() {
}

HCutG::HCutG(const Text_t *name, Int_t n, Float_t *x, Float_t *y) 
    :TCutG(name, n, x, y) {
}

HCutG::HCutG(TCutG *pCut, const Text_t *name) {
  Convert(pCut);
  SetName(name);
}

Double_t HCutG::EvalInstance(HCombinedIndex* pInstancesIndex) {
//  Evaluate graphical cut value for current event instance
/*
   if (!fTree) return 0;
   if (!fFormulaX) {
      fFormulaX = new HTreeFormula("f_x",fVarX.Data(),fTree);
      if (!fFormulaX) return 0;
      ((HTreeFormula*)fFormulaX)->GroupLeaves();
   }
   if (!fFormulaY) {
      fFormulaY = new HTreeFormula("f_y",fVarY.Data(),fTree);
      if (!fFormulaY) return 0;
      ((HTreeFormula*)fFormulaY)->GroupLeaves();
   }
*/
#warning do poprawienia

   if (!GetFormulaX()) return 0;
   if (!GetFormulaY()) return 0;
   Float_t xcut = ((HTreeFormula*)GetFormulaX())
                          ->HTreeFormula::EvalInstance(pInstancesIndex);
   Float_t ycut = ((HTreeFormula*)GetFormulaY())
                          ->HTreeFormula::EvalInstance(pInstancesIndex);
   return IsInside(xcut,ycut);
}

void HCutG::GroupLeaves() {
#warning do poprawienia
/*
   if (!fTree) return;
   if (!fFormulaX) {
      fFormulaX = new HTreeFormula("f_x",fVarX.Data(),fTree);
      if (!fFormulaX) return;
      ((HTreeFormula*)fFormulaX)->GroupLeaves();
   }

   if (!GetFormulaY()) {
      fFormulaY = new HTreeFormula("f_y",fVarY.Data(),fTree);
      if (!fFormulaY) return;
      ((HTreeFormula*)fFormulaY)->GroupLeaves();
   }
*/
      if (!GetFormulaX()) return;
      if (!GetFormulaY()) return;
      ((HTreeFormula*)GetFormulaX())->HTreeFormula::GroupLeaves();
      ((HTreeFormula*)GetFormulaY())->HTreeFormula::GroupLeaves();
}

void HCutG::Convert(TCutG *pCut) {
  TCutG &t = *this;

  t = *pCut; 
}

