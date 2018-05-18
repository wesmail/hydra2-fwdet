#ifndef HEMCCALIBRATERPAR_H
#define HEMCCALIBRATERPAR_H

#include "hparcond.h"

class HEmcCalibraterPar : public HParCond {
protected:
  Float_t fMatchWindowMin;  // match window Slow-Fast in [ns]
  Float_t fMatchWindowMax;  // match window Slow-Fast in [ns]

public:
  HEmcCalibraterPar(const char* name="EmcCalibraterPar",
              const char* title="Emc calibrater parameters",
              const char* context="EmcCalibraterProduction");
  ~HEmcCalibraterPar() {;}
  Float_t getMatchWindowMin() const { return fMatchWindowMin; }
  Float_t getMatchWindowMax() const { return fMatchWindowMax; }

  void   clear(void);
  void   setMatchWindow(Float_t min, Float_t max);
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);

  ClassDef(HEmcCalibraterPar,1) // Container for the EMC digitization parameters
};

#endif /* !HEMCCALIBRATERPAR_H */
  
