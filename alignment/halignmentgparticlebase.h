#ifndef HALIGNMENTGPARTICLEBASE_H
#define HALIGNMENTGPARTICLEBASE_H
#include "TObject.h"
#include "hgeomvector.h"

class HAlignmentGParticleBase:public TObject  
{
 public:
  HAlignmentGParticleBase();
  virtual ~HAlignmentGParticleBase() {;}
  virtual void ResetAll() = 0;
 
  void SetIsGood(Bool_t _isGood) {isGood=_isGood;}
  Bool_t GetIsGood() const {return isGood;}
  
 protected:
 Bool_t isGood;
  
  ClassDef(HAlignmentGParticleBase,0)
};
    
#endif

