#ifndef __HVIRTULCAND_H__
#define __HVIRTULCAND_H__

#include "TLorentzVector.h"

typedef Float16_t SmallFloat;

class HVirtualCand : public TLorentzVector
{
protected:
    SmallFloat  fBeta;                       // particle's beta
    SmallFloat  fMomentum;                   // particle's momentum [MeV]
    SmallFloat  fMass2;                      // particle's mass^2 [MeV^2]

    // track properties
    Short_t     fCharge;                     // particle's charge (+1,-1)                                       (1bit) 0-1
    Short_t     fTofRec;                     // 0 -> time-of-flight reconstruction was not succesfull,          (2bit) 0-3
                                             // 1 -> tof+mdc dEdx, 2 -> only tof dEdx, 3 -> only Mdc dEdx

    SmallFloat  fPhi;                        // track's phi in cave coordinates (0..360 deg)
    SmallFloat  fTheta;                      // track's theta in cave coordinates (0..90 deg)
    SmallFloat  fR;                          // distance of closest point to beamline [mm]
    SmallFloat  fZ;                          // z coordinate of closest point to beamline [mm]
    SmallFloat  fChi2;                       // tracking chi^2 (1e6 == tracking failed)
public:
    HVirtualCand() :
    fBeta(-1), fMomentum(-1), fMass2(-1), fCharge(0), fTofRec(0),
    fPhi(-1), fTheta(-1), fR(-1), fZ(-1),fChi2(-1)
    {
    }
    virtual ~HVirtualCand();

    void    setBeta(Float_t b)                        { fBeta = b;                 }
    void    setMomentum(Float_t m)                    { fMomentum = m;             }
    void    setMass2(Float_t m)                       { fMass2 = m;                }
    void    setCharge(Short_t c)                      { fCharge = c;               }
    void    setTofRec(Short_t tr)                     { fTofRec = tr;              }
    void    setPhi(Float_t p)                         { fPhi = p;                  }
    void    setTheta(Float_t t)                       { fTheta = t;                }
    void    setR(Float_t r)                           { fR = r;                    }
    void    setZ(Float_t z)                           { fZ = z;                    }
    void    setChi2(Float_t c)                        { fChi2 = c;                 }

    Float_t getBeta()                       const     { return fBeta;              }
    Float_t getMomentum()                   const     { return fMomentum;          }
    Float_t getMass2()                      const     { return fMass2;             }
    Float_t getMass ()                      const     { return fMass2 == -1 ?  -1 : ( fMass2 < 0 ? TMath::Sqrt(-fMass2) : TMath::Sqrt(fMass2) ) ; }
    Short_t getCharge()                     const     { return fCharge;            }
    Short_t getTofRec()                     const     { return fTofRec;            }
    Float_t getPhi()                        const     { return fPhi;               }
    Float_t getTheta()                      const     { return fTheta;             }
    Float_t getR()                          const     { return fR;                 }
    Float_t getZ()                          const     { return fZ;                 }
    Float_t getChi2()                       const     { return fChi2;              }

    Float_t getMomentumPID          (Int_t pid);

    void calc4vectorProperties(Double_t mass = 0.51099892)
    {
        // first, transform from spherical to cartesian coordinates
        SetXYZM( TMath::Abs(fMomentum) * TMath::Sin( TMath::DegToRad() * fTheta )
            * TMath::Cos( TMath::DegToRad() * fPhi ),
            TMath::Abs(fMomentum) * TMath::Sin( TMath::DegToRad() * fTheta )
            * TMath::Sin( TMath::DegToRad() * fPhi ),
            TMath::Abs(fMomentum) * TMath::Cos( TMath::DegToRad() * fTheta ),
            mass );
    }

    virtual void   print(UInt_t selection=31);

    ClassDef(HVirtualCand,1)  // A simple track of a particle
};

#endif // __HVIRTUALCAND_H__
