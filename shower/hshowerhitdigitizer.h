#ifndef HShowerHitDigitizer_H
#define HShowerHitDigitizer_H

#include "hshowerdigitizer.h"

class HGeantShower;

class HShowerHitDigitizer : public HShowerDigitizer {
public:
    HShowerHitDigitizer();
    HShowerHitDigitizer(const Text_t *name,const Text_t *title, Float_t unit=10.);
   ~HShowerHitDigitizer();

    Bool_t digitize(TObject *pHit);
    HShowerHitDigitizer &operator=(HShowerHitDigitizer &c);

    Bool_t init(void);
    Bool_t finalize(void) {return kTRUE;}

private:
    virtual Bool_t digiHits(HGeantShower *simhit);
    //virtual Float_t calcHit(Float_t fBeta,
    //        const Float_t *pfMeanParams, const Float_t *pfSigmaParams);

    Float_t fUnit;        // Length unit conversion for HGeant
    Double_t  piconst;


    ClassDef(HShowerHitDigitizer,0) // Shower hit digitizer
};

#endif
