#ifndef HTREE_H
#define HTREE_H

#include "TTreeFormula.h"
#include "TTreePlayer.h"
#include "TBranch.h"

class HTreePlayer : public TTreePlayer {
public:
  HTreePlayer(void);
  ~HTreePlayer(void);
//  virtual Int_t MakeCode(Char_t* filename = 0);         

  virtual void CompileVariables(const Text_t *varexp="", const Text_t *selection="");  
  virtual void EntryLoop(Int_t &action, TObject *obj, Int_t nevents=1000000000, Int_t firstevent=0, Option_t *option="");
  virtual void TakeAction(Int_t nfill, Int_t &npoints, Int_t &action, TObject *obj, Option_t *option);

/*
  virtual  void Draw(const Text_t* varexp, const Text_t* selection, Option_t* option="", Int_t nentries = 1000000000, Int_t firstentry = 0);
  virtual void Draw(Option_t* opt);                 
  virtual void Draw(TCut varexp, TCut selection, Option_t *option=""
                       ,Int_t nevents=1000000000, Int_t firstevent=0);
*/ 
private:
  void SetMultTreeFormula(void);
  Int_t nIsMultTree;  //!
  TTreeFormula *fMultTreeFormula; //!

  ClassDef(HTreePlayer,1) //TTree implementing a modified split algorithm
};

#endif /* !HTREE_H */




