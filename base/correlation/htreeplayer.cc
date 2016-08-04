#include "htreeplayer.h"
#include "htreeformula.h"
#include "hmulttreeformula.h"
#include "hcombinedindex.h"
#include "TROOT.h"
#include "Foption.h"
#include "TSystem.h"
#include "TRealData.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TClass.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TEventList.h"
#include "TProofServ.h"


ClassImp(HTreePlayer)

const Int_t kForceRead      = BIT(11); 
/*
Int_t HTreePlayer::MakeCode(Char_t* filename = 0) {
  if (filename) return 1; else return 0;
}
*/

HTreePlayer::HTreePlayer(void) {
  nIsMultTree = 0;
  fMultTreeFormula = new HMultTreeFormula;
}

HTreePlayer::~HTreePlayer(void) {
  if (fMultTreeFormula) delete fMultTreeFormula;
}

void HTreePlayer::SetMultTreeFormula() {
   ((HMultTreeFormula*)fMultTreeFormula)->SetVars(fTree, fVar1, fVar2,
                                                            fVar3, fSelect);
   ((HMultTreeFormula*)fMultTreeFormula)->CreateMultCodes();

   if(fMultiplicity)
      fMultiplicity = fMultTreeFormula;
}
 
void HTreePlayer::CompileVariables(const Text_t *varexp, const Text_t *selection)
{
//*-*-*-*-*-*-*Compile input variables and selection expression*-*-*-*-*-*
//*-*          ================================================
//
//  varexp is an expression of the general form e1:e2:e3
//    where e1,etc is a formula referencing a combination of the columns
//  Example:
//     varexp = x     simplest case: draw a 1-Dim distribution of column named x
//            = sqrt(x)            : draw distribution of sqrt(x)
//            = x*y/z
//            = y:sqrt(x) 2-Dim dsitribution of y versus sqrt(x)
//
//  selection is an expression with a combination of the columns
//  Example:
//      selection = "x<y && sqrt(z)>3.2"
//       in a selection all the C++ operators are authorized
//
//

   printf("Here comes Leszek's Tree Player\n");
/*
   if (!nIsMultTree)
       return TTreePlayer::CompileVariables(varexp, selection);
*/
   const Int_t MAXCOL = 4;
   TString title;
   Int_t i,nch,ncols;
   Int_t index[MAXCOL];
//*-*- Compile selection expression if there is one
   fDimension = 0;
   ClearFormula();
   fMultiplicity = 0;
   Int_t force = 0;
   if (strlen(selection)) {
      fSelect = new HTreeFormula("Selection",selection, fTree);
      if (!fSelect->GetNdim()) {delete fSelect; fSelect = 0; return; }
      if (fSelect->GetMultiplicity() == 1) fMultiplicity = fSelect;
      if (fSelect->GetMultiplicity() == -1) force = 4;
      ((HTreeFormula*)fSelect)->GroupLeaves();
   }
//*-*- if varexp is empty, take first column by default
   nch = strlen(varexp);
   if (nch == 0) {fDimension = 0; SetMultTreeFormula(); return;}
   title = varexp;

//*-*- otherwise select only the specified columns
   ncols  = 1;
   for (i=0;i<nch;i++)  if (title[i] == ':') ncols++;
   if (ncols > 3 ) return;
   MakeIndex(title,index);

   fTree->ResetBit(kForceRead);
   if (ncols >= 1) {
      fVar1 = new HTreeFormula("Var1",GetNameByIndex(title,index,0),fTree);
      if (!fVar1->GetNdim()) { ClearFormula(); return;}
      if (!fMultiplicity && fVar1->GetMultiplicity() == 1) fMultiplicity = fVar1;
      if (!force && fVar1->GetMultiplicity() == -1) force = 1;
      ((HTreeFormula*)fVar1)->GroupLeaves();
   }
   if (ncols >= 2) {
      fVar2 = new HTreeFormula("Var2",GetNameByIndex(title,index,1),fTree);
      if (!fVar2->GetNdim()) { ClearFormula(); return;}
      if (!fMultiplicity && fVar2->GetMultiplicity() == 1) fMultiplicity = fVar2;
      if (!force && fVar2->GetMultiplicity() == -1) force = 2;
      ((HTreeFormula*)fVar2)->GroupLeaves();
   }
   if (ncols >= 3) {
      fVar3 = new HTreeFormula("Var3",GetNameByIndex(title,index,2),fTree);
      if (!fVar3->GetNdim()) { ClearFormula(); return;}
      if (!fMultiplicity && fVar3->GetMultiplicity()  == 1) fMultiplicity = fVar3;
      if (!force && fVar3->GetMultiplicity() == -1) force = 3;
      ((HTreeFormula*)fVar3)->GroupLeaves();
   }
   if (force) SetBit(kForceRead);
   
   fDimension    = ncols;
   SetMultTreeFormula();
}

//______________________________________________________________________________
void HTreePlayer::EntryLoop(Int_t &action, TObject *obj, Int_t nentries, Int_t firstentry, Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Loop on all entries*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      ==================
//
//  nentries is the number of entries to process (default is all)
//  first is the first entry to process (default is 0)
//
//  action =  1  Fill 1-D histogram obj
//         =  2  Fill 2-D histogram obj
//         =  3  Fill 3-D histogram obj
//         =  4  Fill Profile histogram obj
//         =  5  Fill a TEventlist
//         = 11  Estimate Limits
//         = 12  Fill 2-D PolyMarker obj
//         = 13  Fill 3-D PolyMarker obj
//  action < 0   Evaluate Limits for case abs(action)
//

//   if (!nIsMultTree)
//       return TTreePlayer::EntryLoop(action, obj, nentries, firstentry, option);

   ULong_t i,entry,entryNumber, lastentry,nfill0;
   ULong_t ndata;

   Double_t ww;
   Int_t  npoints;
   lastentry = firstentry + nentries - 1;
   if (lastentry > fTree->GetEntries() - 1) {
      lastentry = (Int_t)fTree->GetEntries() - 1;
      nentries   = lastentry - firstentry + 1;
   }

   TDirectory *cursav = gDirectory;

   if (!gProofServ) fNfill = 0;

   //Create a timer to get control in the entry loop(s)
   TProcessEventTimer *timer = 0;
   //   if (!gROOT->IsBatch() && !gProofServ && fTimerInterval)  // Root 223/09
   //      timer = new TProcessEventTimer(fTimerInterval);
   Int_t interval = fTree->GetTimerInterval();                   // Root 223/12
   if (!gROOT->IsBatch() && !gProofServ && interval)
      timer = new TProcessEventTimer(interval); 
   npoints = 0;
   if (!fV1 && fVar1)   fV1 = new Double_t[fTree->GetEstimate()];
   if (!fV2 && fVar2)   fV2 = new Double_t[fTree->GetEstimate()];
   if (!fV3 && fVar3)   fV3 = new Double_t[fTree->GetEstimate()];
   if (!fW)             fW  = new Double_t[fTree->GetEstimate()];
   Int_t force = TestBit(kForceRead);
   if (!fMultiplicity) {
      for (entry=firstentry;entry<(ULong_t)firstentry+nentries;entry++) {
         entryNumber = fTree->GetEntryNumber(entry);
         if ((Long_t)entryNumber == -1) break;
         if (timer && timer->ProcessEvents()) break;
         if (gROOT->IsInterrupted()) break;
         fTree->LoadTree(entryNumber);
         if (fSelect) {
            if (force) fSelect->GetNdata();
            fW[fNfill] = fSelect->EvalInstance(0);
            if (!fW[fNfill]) continue;
         } else fW[fNfill] = 1;
         if (fVar1) {
            if (force) fVar1->GetNdata();
            fV1[fNfill] = fVar1->EvalInstance(0);
         }
         if (fVar2) {
            if (force) fVar2->GetNdata();
            fV2[fNfill] = fVar2->EvalInstance(0);
            if (fVar3) {
               if (force) fVar3->GetNdata();
               fV3[fNfill] = fVar3->EvalInstance(0);
            }
         }
         fNfill++;
         if (fNfill >= fTree->GetEstimate()) {
            TakeAction(fNfill,npoints,action,obj,option);
            fNfill = 0;
         }
      }

      // nentries == -1 when all entries have been processed by proofserver
      if (gProofServ && nentries != -1) return;

      if (fNfill) {
         TakeAction(fNfill,npoints,action,obj,option);
      }
      fSelectedRows = npoints;
      if (npoints == 0) fDraw = 1; // do not draw
      delete timer;
      return;
   }

   Bool_t Var1Multiple = kFALSE;
   Bool_t Var2Multiple = kFALSE;
   Bool_t Var3Multiple = kFALSE;
   Bool_t SelectMultiple = kFALSE;
   if (fVar1 && fVar1->GetMultiplicity()) Var1Multiple = kTRUE;
   if (fVar2 && fVar2->GetMultiplicity()) Var2Multiple = kTRUE;
   if (fVar3 && fVar3->GetMultiplicity()) Var3Multiple = kTRUE;
   if (fSelect && fSelect->GetMultiplicity()) SelectMultiple = kTRUE;

   for (entry=firstentry;entry<(ULong_t)firstentry+nentries;entry++) {
      entryNumber = fTree->GetEntryNumber(entry);
//      if (entryNumber < 0) break;
      if ((Long_t)entryNumber == -1) break;
      if (timer && timer->ProcessEvents()) break;
      if (gROOT->IsInterrupted()) break;
      fTree->LoadTree(entryNumber);
      nfill0 = fNfill;
      ndata = ((HMultTreeFormula*)fMultiplicity)->GetLNdata();
      if (!ndata) continue;

      HCombinedIndex index = ((HMultTreeFormula*)fMultiplicity)
                                              ->CreateCombinedIndex();
      index = 0;
 
      if (fSelect) {
         fW[fNfill] = ((HTreeFormula*)fSelect)->EvalInstance(&index);
         if (!fW[fNfill]  && !SelectMultiple) continue;
      } else fW[fNfill] = 1;
      if (fVar1)
         fV1[fNfill] = ((HTreeFormula*)fVar1)->EvalInstance(&index);
      if (fVar2) {
         fV2[fNfill] = ((HTreeFormula*)fVar2)->EvalInstance(&index);
         if (fVar3)  fV3[fNfill] = ((HTreeFormula*)fVar3)->EvalInstance(&index);
      }
      if (fW[fNfill]) {
         fNfill++;
         if (fNfill >= fTree->GetEstimate()) {
            TakeAction(fNfill,npoints,action,obj,option);
            fNfill = 0;
         }
      }
      ww = 1;

      ++index;
      for (i=1; i<ndata; ++index, i++) {
         if (SelectMultiple) {
            ww = ((HTreeFormula*)fSelect)->EvalInstance(&index);
            if (ww == 0) continue;
         }

         if (!fDimension) {
            fW[fNfill] = ww;
            fNfill++;
            if (fNfill >= fTree->GetEstimate()) {
               TakeAction(fNfill,npoints,action,obj,option);
               fNfill = 0;
            }
            continue;
         }

         if (Var1Multiple) 
             fV1[fNfill] = ((HTreeFormula*)fVar1)->EvalInstance(&index);
         else              
             fV1[fNfill] = fV1[nfill0];
         if (fVar2) {
            if (Var2Multiple) fV2[fNfill] = ((HTreeFormula*)fVar2)
                                                        ->EvalInstance(&index);
            else              fV2[fNfill] = fV2[nfill0];
            if (fVar3) {
               if (Var3Multiple) fV3[fNfill] = ((HTreeFormula*)fVar3)
                                                        ->EvalInstance(&index);
               else              fV3[fNfill] = fV3[nfill0];
            }
         }
         fW[fNfill] = ww;

         fNfill++;
         if (fNfill >= fTree->GetEstimate()) {
            TakeAction(fNfill,npoints,action,obj,option);
            fNfill = 0;
         }
      }
   }

   delete timer;

   // nentries == -1 when all entries have been processed by proofserver
   if (gProofServ && nentries != -1) return;

   if (fNfill) {
      TakeAction(fNfill,npoints,action,obj,option);
   }

   fSelectedRows = npoints;
   if (npoints == 0) fDraw = 1; // do not draw
   if (cursav) cursav->cd();
}

void HTreePlayer::TakeAction(Int_t nfill, Int_t &npoints, Int_t &action, TObject *obj, Option_t *option) {
  
  if (action != 5)
    TTreePlayer::TakeAction(nfill, npoints, action, obj, option);
 else {
     TEventList *elist = (TEventList*)obj;
     Int_t enumb = fTree->GetChainOffset() + fTree->GetReadEntry();
     Int_t lastIndex = elist->GetN() - 1;
     if (elist->GetEntry(lastIndex)!=enumb) elist->Enter(enumb);

    //*-* Do we need to update screen?
     npoints += nfill;
     if (!fTree->GetUpdate()) return;
     if (npoints > fDraw+fTree->GetUpdate()) {
/*       if (fDraw) gPad->Modified();
         else     obj->Draw(option);
       gPad->Update();
*/
       if (!fDraw) obj->Draw(option);
       fDraw = npoints;
     } 
  } 
}

/*
 void HTreePlayer::DrawSelect(const Text_t* varexp, const Text_t* selection, Option_t* option, Int_t nentries, Int_t firstentry) {
     Char_t* op = strstr(option, "old");
     nIsMultTree = 1;
     if (op) {
        strcpy(op, op + 3);
        nIsMultTree = 0;
     }
     TTreePlayer::DrawSelect(varexp, selection, option, nentries, firstentry);
     nIsMultTree = 0;
 }

void HTreePlayer::Draw(TCut varexp, TCut selection, Option_t *option=""
                       ,Int_t nentries=1000000000, Int_t firstentry=0) {
     Char_t* op = strstr(option, "old");
     nIsMultTree = 1;
     if (op) {
        strcpy(op, op + 3);
        nIsMultTree = 0;
     }  
     TTreePlayer::Draw(varexp, selection, option, nentries, firstentry);
     nIsMultTree = 0;
 }

                   
void HTreePlayer::Draw(Option_t *opt){TTreePlayer::Draw(opt);}
*/ 
