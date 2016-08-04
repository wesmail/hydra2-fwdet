#ifndef __HPARTICLEWALLHITSIM_H__
#define __HPARTICLEWALLHITSIM_H__


#include "hparticlewallhit.h"



class HParticleWallHitSim : public HParticleWallHit
{
private:
    // sim info
    Int_t   fGeantPID;               // GEANT PID value
    Float_t fGeantxMom;              // GEANT x momentum [MeV/c]
    Float_t fGeantyMom;              // GEANT y momentum [MeV/c]
    Float_t fGeantzMom;              // GEANT z momentum [MeV/c]
    Float_t fGeantxVertex;           // GEANT x vertex   [mm]
    Float_t fGeantyVertex;           // GEANT y vertex   [mm]
    Float_t fGeantzVertex;           // GEANT z vertex   [mm]
    Int_t   fGeantPartentTrackNum;   // GEANT parent track number
    Int_t   fGeantCreationMechanism; // GEANT creation mechanism
    Int_t   fGeantMediumNumber;      // GEANT medium number
    Float_t fGeantgeninfo;          // GEANT info on particle generation
    Float_t fGeantgeninfo1;         // GEANT info on particle generation
    Float_t fGeantgeninfo2;         // GEANT info on particle generation
    Float_t fGeantgenweight;        // GEANT info on particle generation

public:
    HParticleWallHitSim() :
        fGeantPID(0), 
        fGeantxMom(-1), fGeantyMom(-1), fGeantzMom(-1), fGeantxVertex(-1), fGeantyVertex(-1), fGeantzVertex(-1),
	fGeantPartentTrackNum(0), fGeantCreationMechanism(0), fGeantMediumNumber(0),
        fGeantgeninfo(-1),fGeantgeninfo1(-1),fGeantgeninfo2(-1),fGeantgenweight(0)
    {
    }
        ~HParticleWallHitSim() {}

        void     setGeantPID(Int_t a)               { fGeantPID = a;                  }
        void     setGeantxMom(Float_t a)            { fGeantxMom = a;                 }
        void     setGeantyMom(Float_t a)            { fGeantyMom = a;                 }
        void     setGeantzMom(Float_t a)            { fGeantzMom = a;                 }
        void     setGeantxVertex(Float_t a)         { fGeantxVertex = a;              }
        void     setGeantyVertex(Float_t a)         { fGeantyVertex = a;              }
        void     setGeantzVertex(Float_t a)         { fGeantzVertex = a;              }
        void     setGeantPartentTrackNum(Int_t a)   { fGeantPartentTrackNum = a;      }
        void     setGeantCreationMechanism(Int_t a) { fGeantCreationMechanism = a;    }
        void     setGeantMediumNumber(Int_t a)      { fGeantMediumNumber = a;         }
	void     setGeantGeninfo(Float_t a)         { fGeantgeninfo = a;              }
	void     setGeantGeninfo1(Float_t a)        { fGeantgeninfo1 = a;             }
	void     setGeantGeninfo2(Float_t a)        { fGeantgeninfo2 = a;             }
	void     setGeantGenweight(Float_t a)       { fGeantgenweight = a;            }

	Int_t    getGeantPID()               const  { return fGeantPID;               }
        Float_t  getGeantxMom()              const  { return fGeantxMom;              }
        Float_t  getGeantyMom()              const  { return fGeantyMom;              }
        Float_t  getGeantzMom()              const  { return fGeantzMom;              }
        Float_t  getGeantxVertex()           const  { return fGeantxVertex;           }
        Float_t  getGeantyVertex()           const  { return fGeantyVertex;           }
        Float_t  getGeantzVertex()           const  { return fGeantzVertex;           }
        Int_t    getGeantPartentTrackNum()   const  { return fGeantPartentTrackNum;   }
        Int_t    getGeantCreationMechanism() const  { return fGeantCreationMechanism; }
        Int_t    getGeantMediumNumber()      const  { return fGeantMediumNumber;      }
	Float_t  getGeantGeninfo()           const  { return fGeantgeninfo;           }
	Float_t  getGeantGeninfo1()          const  { return fGeantgeninfo1;          }
	Float_t  getGeantGeninfo2()          const  { return fGeantgeninfo2;          }
	Float_t  getGeantGenweight()         const  { return fGeantgenweight;         }

        ClassDef(HParticleWallHitSim,1)  // A simple wall sim hit of a particle
};


#endif // __HPARTICLEWALLHITSIM_H__
