#ifndef __HVIRTUALCANDSIM_H__
#define __HVIRTUALCANDSIM_H__

#include "TObject.h"
#include "hvirtualcand.h"

class HVirtualCandSim
{
protected:

    // sim info
    Int_t   fGeantPID;                // GEANT PID value
    Int_t   fGeantTrack;              // GEANT track number
    UInt_t  fGeantCorrTrackIds;       // Geant number of correlated track ids (which detector has seen this track?)
    Float_t fGeantxMom;               // GEANT x mom [Mev/c]
    Float_t fGeantyMom;               // GEANT y mom [Mev/c]
    Float_t fGeantzMom;               // GEANT z mom [Mev/c]
    Float_t fGeantxVertex;            // GEANT x vertex [mm]
    Float_t fGeantyVertex;            // GEANT y vertex [mm]
    Float_t fGeantzVertex;            // GEANT z vertex [mm]
    Int_t   fGeantParentTrackNum;     // GEANT parent track number
    Int_t   fGeantParentPID;          // GEANT parent track PID
    Int_t   fGeantGrandParentTrackNum;// GEANT grand parent track number
    Int_t   fGeantGrandParentPID;     // GEANT grand parent track PID
    Int_t   fGeantCreationMechanism;  // GEANT creatiom mechanism
    Int_t   fGeantMediumNumber;       // GEANT medium number
    Float_t fGeantgeninfo;            // GEANT info on particle generation
    Float_t fGeantgeninfo1;           // GEANT info on particle generation
    Float_t fGeantgeninfo2;           // GEANT info on particle generation
    Float_t fGeantgenweight;          // GEANT info on particle generation
public:
    HVirtualCandSim() :
    fGeantPID(0),fGeantTrack(-1), fGeantCorrTrackIds(0),
    fGeantxMom(-1), fGeantyMom(-1), fGeantzMom(-1), fGeantxVertex(-1), fGeantyVertex(-1), fGeantzVertex(-1),
    fGeantParentTrackNum(0),fGeantParentPID(-1),fGeantGrandParentTrackNum(0),fGeantGrandParentPID(-1), 
    fGeantCreationMechanism(0), fGeantMediumNumber(0),
    fGeantgeninfo(-1),fGeantgeninfo1(-1),fGeantgeninfo2(-1),fGeantgenweight(0)
    {
    }
    virtual ~HVirtualCandSim();

    void     setGeantPID(Int_t a)                 { fGeantPID = a;                   }
    void     setGeantTrack(Int_t a)               { fGeantTrack = a;                 }
    void     setGeantCorrTrackIds(UInt_t a)       { fGeantCorrTrackIds = a;          }
    void     setGeantxMom(Float_t a)              { fGeantxMom = a;                  }
    void     setGeantyMom(Float_t a)              { fGeantyMom = a;                  }
    void     setGeantzMom(Float_t a)              { fGeantzMom = a;                  }
    void     setGeantxVertex(Float_t a)           { fGeantxVertex = a;               }
    void     setGeantyVertex(Float_t a)           { fGeantyVertex = a;               }
    void     setGeantzVertex(Float_t a)           { fGeantzVertex = a;               }
    void     setGeantParentTrackNum(Int_t a)      { fGeantParentTrackNum = a;        }
    void     setGeantParentPID(Int_t a)           { fGeantParentPID = a;             }
    void     setGeantGrandParentTrackNum(Int_t a) { fGeantGrandParentTrackNum = a;   }
    void     setGeantGrandParentPID(Int_t a)      { fGeantGrandParentPID = a;        }
    void     setGeantCreationMechanism(Int_t a)   { fGeantCreationMechanism = a;     }
    void     setGeantMediumNumber(Int_t a)        { fGeantMediumNumber = a;          }
    void     setGeantGeninfo(Float_t a)           { fGeantgeninfo = a;               }
    void     setGeantGeninfo1(Float_t a)          { fGeantgeninfo1 = a;              }
    void     setGeantGeninfo2(Float_t a)          { fGeantgeninfo2 = a;              }
    void     setGeantGenweight(Float_t a)         { fGeantgenweight = a;             }


    Int_t    getGeantPID()                 const  { return fGeantPID;                }
    Int_t    getGeantTrack()               const  { return fGeantTrack;              }
    UInt_t   getGeantCorrTrackIds()        const  { return fGeantCorrTrackIds;       }
    Float_t  getGeantxMom()                const  { return fGeantxMom;               }
    Float_t  getGeantyMom()                const  { return fGeantyMom;               }
    Float_t  getGeantzMom()                const  { return fGeantzMom;               }
    Float_t  getGeantTotalMom()            const  { return (fGeantxMom ==-1 && fGeantyMom ==-1 && fGeantzMom ==-1 ) ? -1 : sqrt(fGeantxMom*fGeantxMom+fGeantyMom*fGeantyMom+fGeantzMom*fGeantzMom);}
    Float_t  getGeantxVertex()             const  { return fGeantxVertex;            }
    Float_t  getGeantyVertex()             const  { return fGeantyVertex;            }
    Float_t  getGeantzVertex()             const  { return fGeantzVertex;            }
    Int_t    getGeantParentTrackNum()      const  { return fGeantParentTrackNum;     }
    Int_t    getGeantParentPID()           const  { return fGeantParentPID;          }
    Int_t    getGeantGrandParentTrackNum() const  { return fGeantGrandParentTrackNum;}
    Int_t    getGeantGrandParentPID()      const  { return fGeantGrandParentPID;     }
    Int_t    getGeantCreationMechanism()   const  { return fGeantCreationMechanism;  }
    Int_t    getGeantMediumNumber()        const  { return fGeantMediumNumber;       }
    Float_t  getGeantGeninfo()             const  { return fGeantgeninfo;            }
    Float_t  getGeantGeninfo1()            const  { return fGeantgeninfo1;           }
    Float_t  getGeantGeninfo2()            const  { return fGeantgeninfo2;           }
    Float_t  getGeantGenweight()           const  { return fGeantgenweight;          }

    virtual void   print(UInt_t selection=31);
    ClassDef(HVirtualCandSim,1)  // A simple Sim track of a particle
};

#endif // __HVIRTUALCANDSIM_H__
