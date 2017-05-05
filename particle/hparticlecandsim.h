#ifndef __HPARTICLECANDSIM_H__
#define __HPARTICLECANDSIM_H__

#include "hparticlecand.h"


class HParticleCandSim : public HParticleCand
{
private:

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
    Int_t   fGeantTrackRich  [3];     // GEANT tracks in rich
    Int_t   fGeantTrackMeta  [4];     // GEANT tracks in tof or rpc
    Int_t   fGeantTrackShower[4];     // GEANT tracks in Shower | ECAL
    Int_t   fGeantTrackInnerMdc[2];   // GEANT tracks in inner MDC seg
    Int_t   fGeantTrackOuterMdc[2];   // GEANT tracks in outer MDC seg
    Char_t  fGeantTrackInnerMdcN[2];  // GEANT number of wires contributing for tracks in inner MDC seg
    Char_t  fGeantTrackOuterMdcN[2];  // GEANT number of wires contributing for tracks in outer MDC seg
public:
    HParticleCandSim() :
	fGeantPID(0),fGeantTrack(-1), fGeantCorrTrackIds(0),
	fGeantxMom(-1), fGeantyMom(-1), fGeantzMom(-1), fGeantxVertex(-1), fGeantyVertex(-1), fGeantzVertex(-1),
	fGeantParentTrackNum(0),fGeantParentPID(-1),fGeantGrandParentTrackNum(0),fGeantGrandParentPID(-1), 
        fGeantCreationMechanism(0), fGeantMediumNumber(0),
	fGeantgeninfo(-1),fGeantgeninfo1(-1),fGeantgeninfo2(-1),fGeantgenweight(0)
    {
	for(Int_t i=0;i<4;i++){
	    fGeantTrackMeta[i]=-1;
	    fGeantTrackShower[i]=-1;
	}
	for(Int_t i=0;i<3;i++){
	    fGeantTrackRich[i]=-1;
	}
	for(Int_t i=0;i<2;i++){
	    fGeantTrackInnerMdc[i]=-1;
	    fGeantTrackOuterMdc[i]=-1;
	    fGeantTrackInnerMdcN[i]=0;
	    fGeantTrackOuterMdcN[i]=0;
	}

    }
	~HParticleCandSim() {}

	void     setGeantPID(Int_t a)                 { fGeantPID = a;                   }
	void     setGeantTrack(Int_t a)               { fGeantTrack = a;                 }
	void     setGeantTrackRich(Int_t a,Int_t i)   { if(i<3 && i>=0) fGeantTrackRich  [i] = a; }
	void     setGeantTrackMeta(Int_t a,Int_t i)   { if(i<4 && i>=0) fGeantTrackMeta  [i] = a; }
	void     setGeantTrackShower(Int_t a,Int_t i) { if(i<4 && i>=0) fGeantTrackShower[i] = a; }
	void     setGeantTrackEmc   (Int_t a,Int_t i) { if(i<4 && i>=0) fGeantTrackShower[i] = a; }  // shared with shower
	void     setGeantTrackInnerMdc(Int_t a,Int_t i,Int_t n) { if(i<2 && i>=0) { fGeantTrackInnerMdc[i] = a; fGeantTrackInnerMdcN[i] = n; }}
	void     setGeantTrackOuterMdc(Int_t a,Int_t i,Int_t n) { if(i<2 && i>=0) { fGeantTrackOuterMdc[i] = a; fGeantTrackOuterMdcN[i] = n; }}
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
	Int_t    getGeantTrackRich(Int_t i)           { return (i<3 && i>=0)?  fGeantTrackRich  [i] :-1; }
	Int_t    getGeantTrackMeta(Int_t i)           { return (i<4 && i>=0)?  fGeantTrackMeta  [i] :-1; }
	Int_t    getGeantTrackShower(Int_t i)         { return (i<4 && i>=0)?  fGeantTrackShower[i] :-1; }
	Int_t    getGeantTrackEmc   (Int_t i)         { return (i<4 && i>=0)?  fGeantTrackShower[i] :-1; }  // shared with shower
	Int_t    getGeantTrackInnerMdc(Int_t i)       { return (i<2 && i>=0)?  fGeantTrackInnerMdc[i] :-1; }
        Int_t    getGeantTrackOuterMdc(Int_t i)       { return (i<2 && i>=0)?  fGeantTrackOuterMdc[i] :-1; }
	Int_t    getGeantTrackInnerMdcN(Int_t i)      { return (i<2 && i>=0)?  fGeantTrackInnerMdcN[i] : 0; }
        Int_t    getGeantTrackOuterMdcN(Int_t i)      { return (i<2 && i>=0)?  fGeantTrackOuterMdcN[i] : 0; }
	Float_t  getGeantTrackInnerMdcWeight(Int_t i) { return (i<2 && i>=0 && getNWSeg(0) !=0 )?  Float_t(fGeantTrackInnerMdcN[i])/getNWSeg(0) : 0; }
        Float_t  getGeantTrackOuterMdcWeight(Int_t i) { return (i<2 && i>=0 && getNWSeg(0) !=0 )?  Float_t(fGeantTrackOuterMdcN[i])/getNWSeg(1) : 0; }

	UInt_t   getGeantCorrTrackIds()        const  { return fGeantCorrTrackIds;       }
        Bool_t   isGhostTrack()                const  { return (fGeantCorrTrackIds&kIsGhost) == 0 ? kFALSE:kTRUE; }
        Bool_t   isInDetector (eDetBits det)        const  { return (fGeantCorrTrackIds&det) == 0 ? kFALSE:kTRUE; }
	Bool_t   isInDetectors(UInt_t flagdetbits)  const  { return (fGeantCorrTrackIds&flagdetbits) == flagdetbits ? kTRUE:kFALSE; }
        Int_t    getNDetector()                     const  {
             Int_t n=0;
             if(fGeantCorrTrackIds&kIsInRICH)      n+=1;
             if(fGeantCorrTrackIds&kIsInInnerMDC)  n+=1;
             if(fGeantCorrTrackIds&kIsInOuterMDC)  n+=1;
             if(fGeantCorrTrackIds&kIsInTOF)       n+=1;
             if(fGeantCorrTrackIds&kIsInRPC)       n+=1;
             if(fGeantCorrTrackIds&kIsInSHOWER||fGeantCorrTrackIds&kIsInEMC)    n+=1;
             return n;
        }
	Bool_t   isGoodMDCWires (Int_t tr,Int_t   minInner=7  ,Int_t   minOuter=7  ,                                    Bool_t checkall=kFALSE,Bool_t requireOuter=kTRUE);
        Bool_t   isGoodMDCWeight(Int_t tr,Float_t minInner=0.5,Float_t minOuter=0.5,                                    Bool_t checkall=kFALSE,Bool_t requireOuter=kTRUE);
        Bool_t   isGoodMDC      (Int_t tr,Float_t minInner=0.5,Float_t minOuter=0.5,Int_t minInnerW=5,Int_t minOuterW=5,Bool_t checkall=kFALSE,Bool_t requireOuter=kTRUE);


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
	ClassDef(HParticleCandSim,3)  // A simple Sim track of a particle
};


#endif // __HPARTICLECANDSIM_H__
