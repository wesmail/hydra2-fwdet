#ifndef HWALLEVENTPLANE_H
#define HWALLEVENTPLANE_H

#include "TObject.h"


class HWallEventPlane : public TObject {
protected:
    Float_t fPhi;       // eventplane       angle [deg]
    Float_t fPhiA;      // subevent-A-plane angle [deg]
    Float_t fPhiB;      // subevent-B-plane angle [deg]
    Float_t fPhiAB;     // angle between two (1/2)-subevent estimates A and B [deg]
    Int_t NA;           // Number of cells randomly selected for subevent A
    Int_t NB;           // Number of cells randomly selected for subevent B
public:
    HWallEventPlane(void);
    virtual ~HWallEventPlane(void) {;}

    void    setPhi     (Float_t phi){ fPhi   = phi;  }
    void    setPhiA    (Float_t phi){ fPhiA  = phi;  }
    void    setPhiB    (Float_t phi){ fPhiB  = phi;  }
    void    setPhiAB   (Float_t phi){ fPhiAB = phi;  }
    void    setNA      (Float_t n  ){ NA = n;        }
    void    setNB      (Float_t n  ){ NB = n;        }

    Float_t getPhi     (void) const { return fPhi;   }
    Float_t getPhiA    (void) const { return fPhiA;  }
    Float_t getPhiB    (void) const { return fPhiB;  }
    Float_t getPhiAB   (void) const { return fPhiAB; }
    Float_t getNA      (void) const { return NA; }
    Float_t getNB      (void) const { return NB; }

    ClassDef(HWallEventPlane,1)  // WALL eventplane
};

#endif /* ! HWALLEVENTPLANE_H */
