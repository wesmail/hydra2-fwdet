#ifndef __HPARTICLEWALLHIT_H__
#define __HPARTICLEWALLHIT_H__

#include "TObject.h"
#include "TLorentzVector.h"


class HParticleWallHit : public TLorentzVector
{
private:

    // Forward wall related properties
    Int_t   fWallCell;    // cell number
    Float_t fWallCharge;  // callibrated charge
    Float_t fWallTime;    // callibrated time-of-flight
    Float_t fWallXlab;    // x position in lab frame
    Float_t fWallYlab;    // y position in lab frame
    Float_t fWallZlab;    // z position in lab frame


public:
    HParticleWallHit() :
        fWallCell(-1), fWallCharge(0), fWallTime(0), fWallXlab(0), fWallYlab(0), fWallZlab(0)
    {
    }
        ~HParticleWallHit() {}

        void     setWallCell(Int_t a)             { fWallCell = a;              }
        void     setWallCharge(Float_t a)         { fWallCharge = a;            }
        void     setWallTime(Float_t a)           { fWallTime = a;              }
        void     setWallXlab(Float_t a)           { fWallXlab = a;              }
        void     setWallYlab(Float_t a)           { fWallYlab = a;              }
        void     setWallZlab(Float_t a)           { fWallZlab = a;              }

	Int_t    getWallCell()   const            { return fWallCell;           }
        Float_t  getWallCharge() const            { return fWallCharge;         }
        Float_t  getWallTime()   const            { return fWallTime;           }
        Float_t  getWallXlab()   const            { return fWallXlab;           }
        Float_t  getWallYlab()   const            { return fWallYlab;           }
        Float_t  getWallZlab()   const            { return fWallZlab;           }

        ClassDef(HParticleWallHit,1)  // A simple wall hit of a particle
};


#endif // __HPARTICLEWALLHIT_H__
