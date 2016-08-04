#ifndef HWALLONEHIT_H
#define HWALLONEHIT_H

#include "TObject.h"

class HWallOneHit : public TObject {
protected:
    Float_t timeHit;    // tdc time
    Float_t chargeHit;  // adc charge  
    Int_t cell;      // paddle number
public:
    HWallOneHit(void);
    ~HWallOneHit(void) {;}

    void setTime(const Float_t v)   { timeHit=v; }
    void setCharge(const Float_t v) { chargeHit=v; }
    void setCell(const Int_t n)     { cell=n; }
    void setAddress(const Int_t c);

    void clear(void);
    void fill(const Float_t t, const Float_t a, const Int_t c);

    Float_t getTime(void) const { return timeHit; }
    Float_t getCharge(void) const { return chargeHit; }
    Int_t getCell(void) const { return cell; }
    void getAddress(Int_t& c);

    ClassDef(HWallOneHit,1)  // WALL detector calibrated data
};

#endif /* ! HWALLONEHIT_H */
