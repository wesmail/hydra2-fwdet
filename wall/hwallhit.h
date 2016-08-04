#ifndef HWALLHIT_H
#define HWALLHIT_H

#include "TObject.h"

//F. Krizek 11.8.2005

class HWallHit : public TObject {
protected:
    Float_t timeCal;    // tdc time
    Float_t chargeCal;  // adc charge
    Int_t   cell;       // paddle number
    Float_t theta;      // theta
    Float_t phi;        // phi
    Float_t d;          // d is distance
    Float_t xlab;       // x coordinate in lab system
    Float_t ylab;       // y coordinate in lab system
    Float_t zlab;       // z coordinate in lab system
public:
    HWallHit(void);
    virtual ~HWallHit(void) {;}

    void setTime    (const Float_t v)                 { timeCal    = v;   }
    void setCharge  (const Float_t v)                 { chargeCal  = v;   }
    void setCell    (const Int_t n)                   { cell       = n;   }
    void setAddress (const Int_t c)                   { cell       = c;   }
    void setTheta   (const Float_t t)                 { theta      = t;   }
    void setPhi     (const Float_t p)                 { phi        = p;   }
    void setDistance(const Float_t dis)               { d          = dis; }
    void setXYZLab  (Float_t x, Float_t y, Float_t z) { xlab = x; ylab = y; zlab = z;}

    virtual void clear(void);
    void    fill(const Float_t t, const Float_t a, const Int_t c);

    Float_t getTime    (void) const { return timeCal;   }
    Float_t getCharge  (void) const { return chargeCal; }
    Int_t   getCell    (void) const { return cell;      }
    void    getAddress (Int_t& c)   { c = cell;         }
    Float_t getTheta   (void) const { return theta;     }
    Float_t getPhi     (void) const { return phi;       }
    Float_t getDistance(void) const { return d;         }
    void    getXYZLab(Float_t &x, Float_t &y, Float_t &z) { x = xlab; y = ylab; z = zlab; }


    ClassDef(HWallHit,1)  // WALL detector calibrated data
};

#endif /* ! HWALLHIT_H */
