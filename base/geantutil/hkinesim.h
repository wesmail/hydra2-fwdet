// File: hkinesim.h
//
// Author: Laura Fabbietti <L.Fabbietti@physik.tu-muenchen.de>
// Last update by Laura Fabbietti: 99/12/17 14:37:23
//
#ifndef HKINESIM_H
#define HKINESIM_H

#include "hlinearcategory.h"
#include "hcategory.h"
#include "hiterator.h"

class HIterator;
class HGeantKine;
class HKineSim {
private:
    HIterator* iter;                  // Iterator over HGeant kine category
    HLinearCategory* fGeantKineCat;   //! HGeant kine date

public:
    HKineSim(void);
    virtual ~HKineSim(void);
    Bool_t checkChargedPart(Int_t nTrack);
    HGeantKine* getObj(Int_t nTrack);
public:
    ClassDef(HKineSim,0)  // check HGeant kine info
};
#endif /* !HKINESIM_H */



