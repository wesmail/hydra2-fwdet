//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// HMultTreeFormula                                                     //
//                                                                      //
// The multiplicity formula class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TTree.h"
#include "hcutg.h"
#include "htreeformula.h"
#include "hmulttreeformula.h"

ClassImp(HMultTreeFormula)


HMultTreeFormula::HMultTreeFormula() {
   fVar1 = 0;
   fVar2 = 0;
   fVar3 = 0;
   fSelect = 0;
}

HMultTreeFormula::HMultTreeFormula(TTree* tree, TTreeFormula* fV1, 
                                   TTreeFormula* fV2, TTreeFormula* fV3, 
                                   TTreeFormula* fSel) {
   SetVars(tree, fV1, fV2, fV3, fSel);
}

HMultTreeFormula::HMultTreeFormula(
              const Char_t *name,const Char_t *formula, TTree *tree) :
              TTreeFormula(name, formula, tree) {
   fVar1 = 0;
   fVar2 = 0;
   fVar3 = 0;
   fSelect = 0;
}

void HMultTreeFormula::SetVars(TTree* tree, TTreeFormula* fV1, 
                               TTreeFormula* fV2, TTreeFormula* fV3,
                               TTreeFormula* fSel) {
   fTree = tree;
   fVar1 = fV1;
   fVar2 = fV2;
   fVar3 = fV3;
   fSelect = fSel;
}

void HMultTreeFormula::UpdateCodes(HTreeFormula* fVar) {
  Int_t j, k, isCutG;
  Int_t isNewCode;

  if (!fVar) return;

  Short_t* brCodes = fVar->GetBranchCodes();
  Short_t* lCodes = fVar->GetCodes();

  isCutG = 0;
  if (fVar) {
     for(k = 0; k < fVar->GetNcodes(); k++) {
        if (lCodes[k] < 0) {
           isCutG = 1;
           continue;
        }
        isNewCode = kTRUE;
        for(j = 0; j < fNcodes; j++)  
          if (brCodes[k] == fBranchCodes[j]) { 
             isNewCode = kFALSE;
             break;
          }

        if (isNewCode) {
          fBranchCodes[fNcodes] = brCodes[k];
          fCodes[fNcodes] =  lCodes[k];
          fNcodes++;
        }
     }
  }
  
  if (isCutG) UpdateCodesFromCutG(fVar);  
}

void HMultTreeFormula::UpdateCodesFromCutG(HTreeFormula* fVar) {
  Int_t i;
  HTreeFormula* pFormula;

  if (!fVar) return;
  
  Short_t* lCodes = fVar->GetCodes();

  for(i = 0; i < fVar->GetNcodes(); i++) {
    if(lCodes[i] < 0) {
         HCutG *pCutG = (HCutG*)fVar->GetCutG(i);
         if (!pCutG) break;

         pFormula = (HTreeFormula*)pCutG->GetFormulaX();
         UpdateCodes(pFormula);
         pFormula = (HTreeFormula*)pCutG->GetFormulaY();
         UpdateCodes(pFormula);
    } 
  }
}

void HMultTreeFormula::EliminateCutGCode() {
  Int_t i, j;

  for(i = 0, j = 0; i < fNcodes; i++) {
    if (fCodes[i] < 0) continue;
    if (j < i) {
       fCodes[j] = fCodes[i];
       fBranchCodes[j] = fCodes[i];
    }
    j++;
  }
   
  fNcodes = j; 
}

void HMultTreeFormula::CreateMultCodes() {
  fNcodes = 0;
  UpdateCodes((HTreeFormula*)fSelect);
  UpdateCodes((HTreeFormula*)fVar1);
  UpdateCodes((HTreeFormula*)fVar2);
  UpdateCodes((HTreeFormula*)fVar3);

  EliminateCutGCode();
}

ULong_t HMultTreeFormula::GetLNdata() {
/*
  if (fSelect) fSelect->GetNdata();
  if (fVar1) fVar1->GetNdata();
  if (fVar2) fVar2->GetNdata();
  if (fVar3) fVar3->GetNdata();
*/ 
  TObjArray *lleaves = fTree->GetListOfLeaves();

  Int_t i;
  Long_t ndata;
  for(i = 0; i < fNcodes; i++) {
     if (fCodes[i] < 0) continue;
     TLeaf *leaf = (TLeaf*)lleaves->UncheckedAt(fCodes[i]);
     if (leaf->GetLeafCount()) {
         TBranch *branch = leaf->GetLeafCount()->GetBranch();
         branch->GetEvent(fTree->GetReadEvent());
         fBranchLen[i] = leaf->GetLen();
     }
     else
       fBranchLen[i] = 1;     
  }

  ndata = 1;
  for(i = 0; i < fNcodes; i++)
     ndata *= fBranchLen[i];

  return ndata;
}

HCombinedIndex& HMultTreeFormula::CreateCombinedIndex() {
  index.set(fNcodes, fBranchLen, this);
  index = 0;

  return index;
}

