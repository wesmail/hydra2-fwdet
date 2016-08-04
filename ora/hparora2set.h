#ifndef HPARORA2SET_H
#define HPARORA2SET_H

#include "TNamed.h"

class HParOra2Set : public TNamed {
public:
  Int_t contextId;         // Id of context
  Double_t versDate[2];    // Actual date range of parameters
  HParOra2Set(const Char_t* pName);
  ~HParOra2Set() {}
  void clearVersDate();      
  ClassDef(HParOra2Set,0) // Oracle interface storage class for parameter sets
};

#endif  /* !HPARORA2SET_H */
