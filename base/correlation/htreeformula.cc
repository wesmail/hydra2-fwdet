//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// HMultTreeFormula                                                     //
//                                                                      //
// The multiplicity formula class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "htreeformula.h"
#include "htree.h"
#include "hcutg.h"
#include "TObjArray.h"
#include "TCutG.h"
#include "TLeafC.h"
#include "TLeafObject.h"

ClassImp(HTreeFormula)


HTreeFormula::HTreeFormula() {
}

HTreeFormula::HTreeFormula(
              const Char_t *name,const Char_t *formula, TTree *tree) :
              TTreeFormula(name, formula, tree) {

}

void HTreeFormula::GroupLeaves() {
  Int_t i;
  TLeaf* leaf;
  static Char_t lname[64];
  TString name;
  Int_t code;

  for (i = 0; i < GetNcodes(); i++) {
     if (fCodes[i] < 0) {
       fBranchCodes[i] = -1;
       HCutG *pCutG = (HCutG*)fMethods.At(i);
       pCutG->GroupLeaves();

       if (fMultiplicity != 1)
          fMultiplicity = pCutG->GetFormulaX()->GetMultiplicity();
       if (fMultiplicity != 1)
          fMultiplicity = pCutG->GetFormulaY()->GetMultiplicity();

       continue;
     }

     leaf =  GetLeaf(i);
     if (leaf) {
        name = leaf->GetBranch()->GetName();
        strcpy(lname, name.Data());
        code = -1;

        Int_t len = strlen(lname);
        for(Int_t j = len; j--;) {
           if (lname[j] == '.') {
              lname[j]='_';
              lname[j+1] = 0;
              code = LookForGroupCode(lname, i);
              break;
           }       
        }  

        if (code < 0)
           fBranchCodes[i] = fCodes[i];
        else
           fBranchCodes[i] = code;
     }
  }
}

HCutG* HTreeFormula::GetCutG(Int_t i) {
  if ((i < 0) ||  (i > fNcodes)) return 0;

  if (fCodes[i] < 0)
     return (HCutG*)fMethods.At(i);
  else
     return 0;  
}

Int_t HTreeFormula::LookForGroupCode(Char_t* lname, Int_t indcode) {
  Int_t i;
  Char_t branchname[128];

  TObjArray *lleaves = fTree->GetListOfLeaves();
  Int_t nleaves = lleaves->GetEntriesFast();

  if ((fCodes[indcode] < 0)  || (fCodes[indcode] >= nleaves)) return -1;
  
//          Look for a data member
  for (i=fCodes[indcode]; i--;) {
     TLeaf *leaf = (TLeaf*)lleaves->UncheckedAt(i);
     if (!strcmp(lname,leaf->GetBranch()->GetName() ) ) {
        fBranchCodes[indcode] = i;
        return i;
     }
  }

//          Look for branchname.leafname
  for (i=fCodes[indcode]; i--;) {
     TLeaf *leaf = (TLeaf*)lleaves->UncheckedAt(i);
     sprintf(branchname,"%s.%s",leaf->GetBranch()->GetName(),leaf->GetName());
     if (!strcmp(lname,branchname)) {
        fBranchCodes[indcode] = i;
        return i;
     }
  }
//          Look for a member function
  for (i=fCodes[indcode]; i--;) {
     TLeaf *leaf = (TLeaf*)lleaves->UncheckedAt(i);
     if (!strcmp(lname,leaf->GetBranch()->GetName())) {
        fBranchCodes[indcode] = i;
        return i;
     }
  }

  return -1;
}


Double_t HTreeFormula::EvalInstance(HCombinedIndex* pInstancesIndex) {
//*-*-*-*-*-*-*-*-*-*-*Evaluate this treeformula*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  =========================
//

  Int_t i,pos,pos2,int1,int2;
  Float_t aresult;
  Double_t tab[255];
  Float_t param[50];
  Double_t dexp;
  Char_t *tab2[20];

  for(i = 0; i < fNcodes; i++)
    fInstances[i] = pInstancesIndex->getIndexByCode(fBranchCodes[i]);

  if (fNoper == 1) {
     if (fCodes[0] < 0) {
        HCutG *gcut = (HCutG*)fMethods.At(0);
        gcut->EvalInstance(pInstancesIndex);
        return gcut->EvalInstance(pInstancesIndex);
     }
     TLeaf *leaf = GetLeaf(0);
     if (fInstances[0]) {
         if (fInstances[0] < leaf->GetNdata()) 
                        return leaf->GetValue(fInstances[0]);
         else          
                        return leaf->GetValue(0);
     } else {
         leaf->GetBranch()->GetEntry(fTree->GetReadEntry());
         if (!(leaf->IsA() == TLeafObject::Class())) return leaf->GetValue(fIndex[0]);
         return GetValueLeafObject(fIndex[0],(TLeafObject *)leaf);
     }
  }

  for(i=0;i<fNval;i++) {
     if (fCodes[i] < 0) {
        HCutG *gcut = (HCutG*)fMethods.At(i);
        param[i] = gcut->EvalInstance(pInstancesIndex);
     } else {
        TLeaf *leaf = GetLeaf(i);
        if (fInstances[i]) {
            if (fInstances[i] < leaf->GetNdata()) 
                param[i] = leaf->GetValue(fInstances[i]);
            else                             
                param[i] = leaf->GetValue(0);
        } else {
           leaf->GetBranch()->GetEntry(fTree->GetReadEntry());
           if (!(leaf->IsA() == TLeafObject::Class())) param[i] = leaf->GetValue(fIndex[i]);
           else param[i] = GetValueLeafObject(fIndex[i],(TLeafObject *)leaf);
        }
     }
  }

  pos  = 0;
  pos2 = 0;
  for (i=0; i<fNoper; i++) {
    Int_t action = fOper[i];
//*-*- a tree string
    if (action >= 105000) {
//       if (!precalculated_str) {
//          precalculated_str=1;
//          for (i=0;i<fNstring;i++) string_calc[i]=DefinedString(i);
//       }
       TLeafC *leafc = (TLeafC*)GetLeaf(action-105000);
       leafc->GetBranch()->GetEntry(fTree->GetReadEntry());
       pos2++; tab2[pos2-1] = leafc->GetValueString();
//       pos2++; tab2[pos2-1] = string_calc[action-105000];
       continue;
    }
//*-*- a tree variable
    if (action >= 100000) {
       pos++; tab[pos-1] = param[action-100000];
       continue;
    }
//*-*- String
    if (action == 80000) {
       pos2++; tab2[pos2-1] = (Char_t*)fExpr[i].Data();
       continue;
    }
//*-*- numerical value
    if (action >= 50000) {
       pos++; tab[pos-1] = fConst[action-50000];
       continue;
    }
    if (action == 0) {
      pos++;
      sscanf((const Char_t*)fExpr[i],"%g",&aresult);
      tab[pos-1] = aresult;
//*-*- basic operators and mathematical library
    } else if (action < 100) {
        switch(action) {
          case   1 : pos--; tab[pos-1] += tab[pos]; break;
          case   2 : pos--; tab[pos-1] -= tab[pos]; break;
          case   3 : pos--; tab[pos-1] *= tab[pos]; break;
          case   4 : if (tab[pos-1] == 0) {tab[pos-1] = 0;} //  division by 0
                     else { pos--; tab[pos-1] /= tab[pos]; }
                     break;
          case   5 : {pos--; int1=Int_t(tab[pos-1]); int2=Int_t(tab[pos]); tab[pos-1] = Double_t(int1%int2); break;}
          case  10 : tab[pos-1] = TMath::Cos(tab[pos-1]); break;
          case  11 : tab[pos-1] = TMath::Sin(tab[pos-1]); break;
          case  12 : if (TMath::Cos(tab[pos-1]) == 0) {tab[pos-1] = 0;} // { tangente indeterminee }
                     else tab[pos-1] = TMath::Tan(tab[pos-1]);
                     break;
          case  13 : if (TMath::Abs(tab[pos-1]) > 1) {tab[pos-1] = 0;} //  indetermination
                     else tab[pos-1] = TMath::ACos(tab[pos-1]);
                     break;
          case  14 : if (TMath::Abs(tab[pos-1]) > 1) {tab[pos-1] = 0;} //  indetermination
                     else tab[pos-1] = TMath::ASin(tab[pos-1]);
                     break;
          case  15 : tab[pos-1] = TMath::ATan(tab[pos-1]); break;
          case  70 : tab[pos-1] = TMath::CosH(tab[pos-1]); break;
          case  71 : tab[pos-1] = TMath::SinH(tab[pos-1]); break;
          case  72 : if (TMath::CosH(tab[pos-1]) == 0) {tab[pos-1] = 0;} // { tangente indeterminee }
                     else tab[pos-1] = TMath::TanH(tab[pos-1]);
                     break;
          case  73 : if (tab[pos-1] < 1) {tab[pos-1] = 0;} //  indetermination
                     else tab[pos-1] = TMath::ACosH(tab[pos-1]);
                     break;
          case  74 : tab[pos-1] = TMath::ASinH(tab[pos-1]); break;
          case  75 : if (TMath::Abs(tab[pos-1]) > 1) {tab[pos-1] = 0;} // indetermination
                     else tab[pos-1] = TMath::ATanH(tab[pos-1]); break;
          case  16 : pos--; tab[pos-1] = TMath::ATan2(tab[pos-1],tab[pos]); break;
          case  20 : pos--; tab[pos-1] = TMath::Power(tab[pos-1],tab[pos]); break;
          case  21 : tab[pos-1] = tab[pos-1]*tab[pos-1]; break;
          case  22 : tab[pos-1] = TMath::Sqrt(TMath::Abs(tab[pos-1])); break;
          case  23 : pos2 -= 2; pos++;if (strstr(tab2[pos2],tab2[pos2+1])) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  30 : if (tab[pos-1] > 0) tab[pos-1] = TMath::Log(tab[pos-1]);
                     else {tab[pos-1] = 0;} //{indetermination }
                     break;
          case  31 : dexp = tab[pos-1];
                     if (dexp < -70) {tab[pos-1] = 0; break;}
                     if (dexp >  70) {tab[pos-1] = TMath::Exp(70); break;}
                     tab[pos-1] = TMath::Exp(dexp); break;
          case  32 : if (tab[pos-1] > 0) tab[pos-1] = TMath::Log10(tab[pos-1]);
                     else {tab[pos-1] = 0;} //{indetermination }
                     break;
          case  40 : pos++; tab[pos-1] = TMath::ACos(-1); break;
          case  41 : tab[pos-1] = TMath::Abs(tab[pos-1]); break;
          case  42 : if (tab[pos-1] < 0) tab[pos-1] = -1; else tab[pos-1] = 1; break;
      //  case  50 : tab[pos-1] = gRandom->Rndm(1); break;
          case  50 : tab[pos-1] = 0.5; break;
          case  60 : pos--; if (tab[pos-1]!=0 && tab[pos]!=0) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  61 : pos--; if (tab[pos-1]!=0 || tab[pos]!=0) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  62 : pos--; if (tab[pos-1] == tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  63 : pos--; if (tab[pos-1] != tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  64 : pos--; if (tab[pos-1] < tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  65 : pos--; if (tab[pos-1] > tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  66 : pos--; if (tab[pos-1]<=tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  67 : pos--; if (tab[pos-1]>=tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  68 : if (tab[pos-1]!=0) tab[pos-1] = 0; else tab[pos-1] = 1; break;
          case  76 : pos2 -= 2; pos++; if (!strcmp(tab2[pos2+1],tab2[pos2])) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  77 : pos2 -= 2; pos++;if (strcmp(tab2[pos2+1],tab2[pos2])) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  78 : pos--; tab[pos-1]= ((Int_t) tab[pos-1]) & ((Int_t) tab[pos]); break;
          case  79 : pos--; tab[pos-1]= ((Int_t) tab[pos-1]) | ((Int_t) tab[pos]); break;
       }
    }
  }
  Double_t result = tab[0];
  return result;
}

