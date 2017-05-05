#ifndef __HPARTICLECAND_H__
#define __HPARTICLECAND_H__

#include "TMath.h"
#include "TObject.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include "hparticledef.h"
using namespace Particle;

class HParticleCand : public TLorentzVector
{
protected:



    Int_t       fFlags;		             // bit flags for cleaning
    //Int_t sumval;
    //  |32|31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|
    //                                                                       |r|tof|c|sys|sec  |
    Char_t      fPID;                        // PID of particle
    Short_t     fSector;                     // sector information from MDC (0..5)                              (3bit) 0-7
    Short_t     fSystem;                     // 0 == TOFino, 1 == TOF                                           (2bit) 0-3

    // track properties
    Short_t     fCharge;                     // particle's charge (+1,-1)                                       (1bit) 0-1
    Short_t     fTofRec;                     // 0 -> time-of-flight reconstruction was not succesfull,          (2bit) 0-3
                                             // 1 -> tof+mdc dEdx, 2 -> only tof dEdx, 3 -> only Mdc dEdx
    UShort_t    fRingCorr;                   // ring - mdc correlation
    Short_t     fIndex;                      // index in category

    SmallFloat  fBeta;                       // particle's beta
    SmallFloat  fMomentum;                   // particle's momentum [MeV]
    SmallFloat  fMass2;                      // particle's mass^2 [MeV^2]
    SmallFloat  fPhi;                        // track's phi in cave coordinates (0..360 deg)
    SmallFloat  fTheta;                      // track's theta in cave coordinates (0..90 deg)
    SmallFloat  fR;                          // distance of closest point to beamline [mm]
    SmallFloat  fZ;                          // z coordinate of closest point to beamline [mm]
    SmallFloat  fChi2;                       // tracking chi^2 (1e6 == tracking failed)
    SmallFloat  fDistanceToMetaHit;          // track's distance to meta hit [mm]

    SmallFloat  fMdcdEdx;                    // Mdc dE/dx for inner and outer segment
    SmallFloat  fTofdEdx;                    // Tof dE/dx


    // MDC related track properties
    SmallFloat  fInnerSegmentChi2;           // MDC segment fitter chi^2 (-1 if not fitted)
    SmallFloat  fOuterSegmentChi2;           // MDC segment fitter chi^2 (-1 if not fitted)
    SmallFloat  fAngleToNearbyFittedInner;   // angle to closest fitted inner segment (negative angles indicate segments which belong to hadrons or neighbouring tracks) [deg]
    SmallFloat  fAngleToNearbyUnfittedInner; // angle to closest unfitted inner segment (negative angles indicate segments which belong to hadrons or neighbouring tracks) [deg]

    // properties of the RICH ring
    Char_t      fRingNumPads;                // number of fired pads typ. (-1 - 100) (OLD) | number of Cal objects (NEW)
    Short_t     fRingAmplitude;              // typ. (-1 - 4000)
    Short_t     fRingHouTra;                 // typ. (-1 - 5000)
    Short_t     fRingPatternMatrix;          // pattern matrix of ring
    SmallFloat  fRingCentroid;               // ring centroid (OLD) | radius (NEW)
    SmallFloat  fRichPhi;
    SmallFloat  fRichTheta;
    Float_t     fRingChi2;                   // chi2 of ring fit (NEW)

    // properties of meta hits
    SmallFloat  fMetaMatchQuality;      // distance of the outer segment to the meta hit
    SmallFloat  fMetaMatchQualityShower;// distance of the outer segment to the shower hit
    SmallFloat  fMetaMatchRadius;       // distance of the outer segment to the meta hit   [mm]
    SmallFloat  fMetaMatchRadiusShower; // distance of the outer segment to the shower hit [mm]
    SmallFloat  fRkMetaDx;              // distance in X of outer segment to used Meta  [mm]
    SmallFloat  fRkMetaDy;              // distance in Y of outer segment to used Meta  [mm]

    // backup before recalc
    SmallFloat  fBetaOrg;               // particle's beta  before recalc
    SmallFloat  fMomentumOrg;           // particle's momentum [MeV]  before correction
    SmallFloat  fDistanceToMetaHitOrg;  // track's distance to meta hit [mm]  before correction

    //properties of shower hits | EMC cluster
    SmallFloat  fShowerSum0;          // charge sum of PreShower cluster (0)  | EMC:energy
    SmallFloat  fShowerSum1;          // charge sum of PreShower cluster (1)  | EMC:time
    SmallFloat  fShowerSum2;          // charge sum of PreShower cluster (2)
    UShort_t    fEmcFlags;            // |16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|
                                      //                        |s|m| ncells  |u|    s=rpc ind is same, m=cluster is matched in time with rpc, u=isEMC

    Char_t      fSelectedMeta;        // which Metahit has been used  kNoUse,kTofClst,kTofHit1,kTofHit2,kRpcClst,kShowerHit,kEmcClst
    Short_t     fMetaInd;             // index of Metamatch
    Short_t     fRichInd;             // index of RICH hit
    Short_t     fRichBTInd;           // index of RICH BT hit
    Short_t     fInnerSegInd;         // index of inner MDC Segment
    Short_t     fOuterSegInd;         // index of outer MDC Segment
    Short_t     fRpcInd;              // index of RPC Hit
    Short_t     fShowerInd;           // index of SHOWER hit | EMC cluster
    Short_t     fTofHitInd;           // index of TOF hit
    Short_t     fTofClstInd;          // index of TOF cluster
    UInt_t      fLayers;              // bit array for fired MDC layers  (1-24 bit layers + 31-29 bit fake reject + 28,27,26 bit clusOffvertex,vertex,primary)
    UShort_t    fWires;               // lower 10bits for wires per segment , 11,12,13,14 at edge
    UInt_t      fmetaAddress;         // store adress (mod+cell / col+cell for TOF+RPC hit 0 and 1)
                                      //  |32|31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|
                                      //                                |             cell2  |    mod2   |  cell1        | mod1  |

public:
    HParticleCand() :
	 fFlags(0)
        ,fPID(-1)
	,fSector(-1)
	,fSystem(-1)
	,fCharge(0)
	,fTofRec(0)
	,fRingCorr(0)
	,fIndex(-1)
	,fBeta(-1)
	,fMomentum(-1)
	,fMass2(-1)
	,fPhi(-1)
	,fTheta(-1)
	,fR(-1)
	,fZ(-1)
	,fChi2(-1)
	,fDistanceToMetaHit(-1)
	,fMdcdEdx(-1)
	,fTofdEdx(-1)
	,fInnerSegmentChi2(-1)
	,fOuterSegmentChi2(-1)
	,fAngleToNearbyFittedInner(0)
	,fAngleToNearbyUnfittedInner(0)
	,fRingNumPads(0)
	,fRingAmplitude(0)
	,fRingHouTra(0)
	,fRingPatternMatrix(-1)
	,fRingCentroid(-1)
	,fRichPhi(-1)
	,fRichTheta(-1)
	,fRingChi2(-1000)
	,fMetaMatchQuality(-1)
	,fMetaMatchQualityShower(-1)
	,fMetaMatchRadius(-1)
	,fMetaMatchRadiusShower(-1)
	,fRkMetaDx(-1000)
	,fRkMetaDy(-1000)
	,fBetaOrg(-1)
	,fMomentumOrg(-1)
	,fDistanceToMetaHitOrg(-1)
 	,fShowerSum0 (-1)
	,fShowerSum1(-1)
	,fShowerSum2(-1)
	,fEmcFlags(0)
	,fSelectedMeta(-1)
	,fMetaInd(-1)
	,fRichInd(-1)
	,fRichBTInd(-1)
	,fInnerSegInd(-1)
	,fOuterSegInd(-1)
	,fRpcInd(-1)
	,fShowerInd(-1)
	,fTofHitInd(-1)
	,fTofClstInd(-1)
	,fLayers(0)
	,fWires(0)
	,fmetaAddress(0)
			
    {
    }
	~HParticleCand() {}
	// -------------------------------------------------------------------------
	//  flag manipulations
	void   setFlagBit   (eFlagBits bit)  { fFlags |=  ( 0x01 << bit ); }
	void   unsetFlagBit (eFlagBits bit)  { fFlags &= ~( 0x01 << bit ); }
	Bool_t isFlagBit    (eFlagBits bit)  { return (fFlags >> bit ) & 0x01 ; }
	void   setFlagBit   (Int_t bit);
	void   unsetFlagBit (Int_t bit);
	Bool_t isFlagBit    (Int_t bit);
	void   setFlagBitByValue(eFlagBits bit, Bool_t val)  { val ? setFlagBit(bit) : unsetFlagBit(bit); }
	void   setFlagBitByValue(Int_t     bit, Bool_t val)  { val ? setFlagBit(bit) : unsetFlagBit(bit); }
	Bool_t isFlagDoubleHit ()            { return (fFlags & 0xF) == 0x0 ? kFALSE : kTRUE; }
	Bool_t isFlagAllBestHit()            { return ((fFlags >> 4 ) & 0xF) == 0xF ? kTRUE : kFALSE; }
	Bool_t isFlagNoBestHit ()            { return ((fFlags >> 4 ) & 0xF) == 0x0 ? kTRUE : kFALSE; }
	Int_t  getFlagField()                { return fFlags;  }
	void   setFlagField(Int_t field)     { fFlags = field; }
	Bool_t isFlagAND(Int_t num, ...);
	Bool_t isFlagOR (Int_t num, ...);
	Int_t  getDoubleHitsLeptons()        { return (fFlags & 0xF);}
	Int_t  getDoubleHitsHadrons()        { return ((fFlags >> 1) & 0x7);}
	void   printFlags(TString comment="");
	virtual void   print(UInt_t selection=31);
	Bool_t select(Bool_t (*function)(HParticleCand* )) { return  (*function)(this); }
	// -------------------------------------------------------------------------


	void    setIndex(Int_t i)                         { fIndex = i;                }
	void    setPID(Int_t id)                          { fPID = (Char_t)id;         }
	void    setSector(Int_t s)                        { fSector = s;               }
	void    setSystem(Int_t s)                        { fSystem = s;               }
	void    setRingCorr(UShort_t rc)                  { fRingCorr = rc;            }
	void    setTofRec(Short_t tr)                     { fTofRec = tr;              }
	void    setMdcdEdx(Float_t d)                     { fMdcdEdx = d;              }
	void    setTofdEdx(Float_t d)                     { fTofdEdx = d;              }
	void    setCharge(Short_t c)                      { fCharge = c;               }
	void    setBeta(Float_t b)                        { fBeta = b;                 }
	void    setMomentum(Float_t m)                    { fMomentum = m;             }
	void    setMass2(Float_t m)                       { fMass2 = m;                }
	void    setPhi(Float_t p)                         { fPhi = p;                  }
	void    setTheta(Float_t t)                       { fTheta = t;                }
	void    setR(Float_t r)                           { fR = r;                    }
	void    setZ(Float_t z)                           { fZ = z;                    }
	void    setChi2(Float_t c)                        { fChi2 = c;                 }
	void    setDistanceToMetaHit(Float_t d)           { fDistanceToMetaHit = d;    }
	void    setInnerSegmentChi2(Float_t c)            { fInnerSegmentChi2 = c;    }
	void    setOuterSegmentChi2(Float_t c)            { fOuterSegmentChi2 = c;    }
	void    setAngleToNearbyFittedInner(Float_t a)    { fAngleToNearbyFittedInner = a;       }
	void    setAngleToNearbyUnfittedInner(Float_t a)  { fAngleToNearbyUnfittedInner = a;     }
	void    setRingNumCals(Int_t n)                   { fRingNumPads = n <= 127 ? n: 127;    }  // RICH700
	void    setRingNumPads(Int_t n)                   { fRingNumPads = n <= 127 ? n: 127;    }
	void    setRingAmplitude(Int_t amp)               { fRingAmplitude = (Short_t) amp;      }
	void    setRingHouTra(Int_t a)                    { fRingHouTra = (Short_t)a;            }
	void    setRingPatternMatrix(Int_t p)             { fRingPatternMatrix = (Short_t) p;    }
	void    setRingRadius(Float_t c)                  { fRingCentroid = c;          }     // RICH700
	void    setRingChi2(Float_t c)                    { fRingChi2 = c;              }     // RICH700
	void    setRingCentroid(Float_t c)                { fRingCentroid = c;          }
	void    setRichPhi(Float_t p)                     { fRichPhi = p;               }
	void    setRichTheta(Float_t p)                   { fRichTheta = p;             }
	void    setMetaMatchQuality(Float_t q)            { fMetaMatchQuality = q;      }
	void    setMetaMatchQualityShower(Float_t q)      { fMetaMatchQualityShower = q;}
	void    setMetaMatchQualityEmc(Float_t q)         { fMetaMatchQualityShower = q;} // share with shower
	void    setMetaMatchRadius(Float_t q)             { fMetaMatchRadius = q;       }
	void    setMetaMatchRadiusShower(Float_t q)       { fMetaMatchRadiusShower = q; }
	void    setMetaMatchRadiusEmc(Float_t q)          { fMetaMatchRadiusShower = q; } // share with shower
	void    setRkMetaDx(Float_t q)                    { fRkMetaDx   = q;            }
	void    setRkMetaDy(Float_t q)                    { fRkMetaDy   = q;            }
	void    setBetaOrg(Float_t b)                     { fBetaOrg = b;               }
	void    setDistanceToMetaHitOrg(Float_t d)        { fDistanceToMetaHitOrg = d;  }
	void    setMomentumOrg(Float_t m)                 { fMomentumOrg = m;           }
	void    setShowerSum0(Float_t q)                  { fShowerSum0 = q;            }
	void    setShowerSum1(Float_t q)                  { fShowerSum1 = q;            }
	void    setShowerSum2(Float_t q)                  { fShowerSum2 = q;            }
	void    setEmcEnergy(Float_t e)                   { fShowerSum0 = e;            } // share with shower
	void    setEmcTime(Float_t t)                     { fShowerSum1 = t;            } // share with shower
        void    setIsEmc()                                { fEmcFlags |= 0x01; }
        void    setIsEmcMatchedToRpc()                    { fEmcFlags |= (0x01 << 6); }
        void    setIsEmcMatchedToSameRpc()                { fEmcFlags |= (0x01 << 7); }
        void    setEmcNCells(UChar_t n)                   { if( n>31) n=31; fEmcFlags |= (((n&(0x1F))<<1)); }
	void    unsetIsEmc()                              { fEmcFlags &= ~( 0x01 << 0 ); }
	void    unsetIsEmcMatchedToRpc()                  { fEmcFlags &= ~( 0x01 << 6 ); }
	void    unsetIsEmcMatchedToSameRpc()              { fEmcFlags &= ~( 0x01 << 7 ); }
	void    unsetEmcNCells()                          { fEmcFlags &= ~( 0x1F << 1 ); }

	void    setSelectedMeta(Int_t flag)               { fSelectedMeta = (Char_t)flag;}
	void    setMetaInd(Int_t ind)                     { fMetaInd    = ind;          }
	void    setRichInd(Int_t ind)                     { fRichInd    = ind;          }
	void    setRichBTInd(Int_t ind)                   { fRichBTInd  = ind;          }
	void    setInnerSegInd(Int_t ind)                 { fInnerSegInd= ind;          }
	void    setOuterSegInd(Int_t ind)                 { fOuterSegInd= ind;          }
        void    setRpcInd(Int_t ind)                      { fRpcInd     = ind;          }
	void    setShowerInd(Int_t ind)                   { fShowerInd  = ind;          }
	void    setEmcInd(Int_t ind)                      { fShowerInd  = ind;          }  // share with shower
	void    setTofHitInd(Int_t ind)                   { fTofHitInd  = ind;          }
	void    setTofClstInd(Int_t ind)                  { fTofClstInd = ind;          }



	Short_t getIndex()                      const     { return fIndex;             }
	Int_t   getPID()                        const     { return fPID;               }
	Short_t getSector()                     const     { return fSector;            }
	Short_t getSystem()                     const     { return fSystem;            }
	Short_t getSystemUsed()                 const     { return ( isRpcClstUsed() || isShowerUsed() ) ?  0 :  ( ( isTofHitUsed() || isTofClstUsed() ) ? 1 : -1 ); }
	UShort_t getRingCorr()                  const     { return fRingCorr;          }
	Bool_t  isRichMatch(eMatching match)    const     { return (fRingCorr&match) == 0 ? kFALSE:kTRUE; }
	Short_t getTofRec()                     const     { return fTofRec;            }
	Float_t getMdcdEdx()                    const     { return fMdcdEdx;           }
	Float_t getTofdEdx()                    const     { return fTofdEdx;           }
	Short_t getCharge()                     const     { return fCharge;            }
	Float_t getBeta()                       const     { return fBeta;              }
	Float_t getMomentum()                   const     { return fMomentum;          }
	Float_t getMass2()                      const     { return fMass2;             }
        Float_t getMass ()                      const     { return fMass2 == -1 ?  -1 : ( fMass2 < 0 ? TMath::Sqrt(-fMass2) : TMath::Sqrt(fMass2) ) ; }
	Float_t getPhi()                        const     { return fPhi;               }
	Float_t getTheta()                      const     { return fTheta;             }
	Float_t getR()                          const     { return fR;                 }
	Float_t getZ()                          const     { return fZ;                 }
	Float_t getChi2()                       const     { return fChi2;              }
	Float_t getDistanceToMetaHit()          const     { return fDistanceToMetaHit; }
        Float_t getTof()                        const     { return fBeta>0 ? fDistanceToMetaHit/(fBeta*300.) : -1; }
        Float_t getTofNorm(Float_t dist=2100)   const     { return fBeta>0 ? (fDistanceToMetaHit/(fBeta*300.)) * dist/fDistanceToMetaHit : -1; }
	Float_t getInnerSegmentChi2()           const     { return fInnerSegmentChi2; }
	Float_t getOuterSegmentChi2()           const     { return fOuterSegmentChi2; }
	Float_t getAngleToNearbyFittedInner()   const     { return fAngleToNearbyFittedInner;    }
	Float_t getAngleToNearbyUnfittedInner() const     { return fAngleToNearbyUnfittedInner;  }
	Int_t   getRingNumCals()                const     { return (Int_t)fRingNumPads;          }
	Int_t   getRingNumPads()                const     { return (Int_t)fRingNumPads;          }
	Int_t   getRingAmplitude()              const     { return (Int_t) fRingAmplitude;       }
        Float_t getAverageRingCharge()          const     { return fRingNumPads == 0 ?  0 : fRingAmplitude/(Float_t)fRingNumPads;}
        Int_t   getRingHouTra()                 const     { return (Int_t) fRingHouTra;          }
	Int_t   getRingPatternMatrix()          const     { return (Int_t)fRingPatternMatrix;    }
	Float_t getRingRadius()                 const     { return fRingCentroid;       }     // RICH700
	Float_t getRingChi2()                   const     { return fRingChi2;           }     // RICH700
	Float_t getRingCentroid()               const     { return fRingCentroid;       }
	Float_t getRichPhi()                    const     { return fRichPhi;            }
	Float_t getRichTheta()                  const     { return fRichTheta;          }
	Float_t getMetaMatchQuality()           const     { return fMetaMatchQuality;   }
	Float_t getMetaMatchQualityShower()     const     { return fMetaMatchQualityShower;}
	Float_t getMetaMatchQualityEmc()        const     { return fMetaMatchQualityShower;} // share with shower
	Float_t getMetaMatchRadius()            const     { return fMetaMatchRadius;   }
	Float_t getMetaMatchRadiusShower()      const     { return fMetaMatchRadiusShower;}
	Float_t getMetaMatchRadiusEmc()         const     { return fMetaMatchRadiusShower;}  // share with shower
	Float_t getRkMetaDx()                   const     { return fRkMetaDx;           }
	Float_t getRkMetaDy()                   const     { return fRkMetaDy;           }
	Float_t getBetaOrg()                    const     { return fBetaOrg;            }
	Float_t getDistanceToMetaHitOrg()       const     { return fDistanceToMetaHitOrg; }
	Float_t getMomentumOrg()                const     { return fMomentumOrg;        }
        Float_t calcRkMetaMatch(Float_t weightX=1.,Float_t weightY=1.) { return (fRkMetaDx==-1000&&fRkMetaDy==-1000)? -1 : sqrt( (weightX*fRkMetaDx)*(weightX*fRkMetaDx) + (weightY*fRkMetaDy)*(weightY*fRkMetaDy)  );}

        Float_t getShowerSum0()                 const     { return fShowerSum0;         }
	Float_t getShowerSum1()                 const     { return fShowerSum1;         }
	Float_t getShowerSum2()                 const     { return fShowerSum2;         }
        Float_t getShowerDeltaSum()             const     { return fShowerSum0 ==-1 && fShowerSum1 == -1 ? -1 : fShowerSum1+fShowerSum2-fShowerSum0; }
        Float_t getEmcEnergy()                  const     { return fShowerSum0;         }  // share with shower
	Float_t getEmcTime()                    const     { return fShowerSum1;         }  // share with shower
        Bool_t  isEmc()                                   { return fEmcFlags & 0x01;}
        Bool_t  isEmcMatchedToRpc()                       { return (fEmcFlags>>6) & 0x01;}
        Bool_t  isEmcMatchedToSameRpc()                   { return (fEmcFlags>>7) & 0x01;}
        UChar_t getEmcNCells()                            { return (fEmcFlags>>1) & 0x1F;}

        Int_t   getSelectedMeta()               const     { return (Int_t)fSelectedMeta;}  // find the defined enum in hparticledef.h
	Bool_t  isTofHitUsed()                  const     { return (fSelectedMeta == Particle::kTofHit1 || fSelectedMeta == Particle::kTofHit2) ? kTRUE : kFALSE; }
        Bool_t  isTofClstUsed()                 const     { return (fSelectedMeta == Particle::kTofClst)   ? kTRUE : kFALSE; }
        Bool_t  isRpcClstUsed()                 const     { return (fSelectedMeta == Particle::kRpcClst)   ? kTRUE : kFALSE; }
        Bool_t  isShowerUsed()                  const     { return (fSelectedMeta == Particle::kShowerHit) ? kTRUE : kFALSE; }
        Bool_t  isEmcUsed()                     const     { return (fSelectedMeta == Particle::kEmcClst)   ? kTRUE : kFALSE; }
        Bool_t  isMetaUsed()                    const     { return (fSelectedMeta != Particle::kNoUse)     ? kTRUE : kFALSE; }

	Int_t   getMetaInd()                    const     { return fMetaInd;            }
        Int_t   getRichInd()                    const     { return fRichInd;            }
        Int_t   getRichBTInd()                  const     { return fRichBTInd;          }
	Int_t   getInnerSegInd()                const     { return fInnerSegInd;        }
	Int_t   getOuterSegInd()                const     { return fOuterSegInd;        }
	Int_t   getRpcInd()                     const     { return fRpcInd;             }
	Int_t   getShowerInd()                  const     { return fShowerInd;          }
	Int_t   getEmcInd()                     const     { return fShowerInd;          } // share with shower
	Int_t   getTofHitInd()                  const     { return fTofHitInd;          }
	Int_t   getTofClstInd()                 const     { return fTofClstInd;         }
        Int_t   getMetaHitInd()                 const     {
	    if      (fSelectedMeta == Particle::kNoUse)     return -1 ;
            else if (fSelectedMeta == Particle::kTofHit1 || fSelectedMeta == Particle::kTofHit2)   return fTofHitInd;
            else if (fSelectedMeta == Particle::kTofClst)   return fTofClstInd;
	    else if (fSelectedMeta == Particle::kRpcClst)   return fRpcInd;
            else if (fSelectedMeta == Particle::kShowerHit) return fShowerInd;
            else if (fSelectedMeta == Particle::kEmcClst)   return fShowerInd;   // share with shower
            return -1;
	}


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
        Float_t getDeltaTheta(){ return  (fRichInd ==-1) ? -1000 : fRichTheta - fTheta; }                                         // delta theta Rich-RK
	Float_t getDeltaPhi()  { return  (fRichInd ==-1) ? -1000 : ( fRichPhi - fPhi ) * TMath::Sin(TMath::DegToRad() * fTheta);} // delta phi Rich-RK
        Float_t getRichMatchingQuality() {
            if(fRichInd ==-1) return -1;
	    else {
		Float_t dPhi   = getDeltaPhi();
		Float_t dTheta = getDeltaTheta();
		return sqrt(dPhi*dPhi + dTheta*dTheta);
	    }
	}
        Float_t getRichMatchingQualityNorm();
	Bool_t  isNewRich()              { return fRingChi2 !=1000 ? kTRUE : kFALSE;}

	Float_t getZprime(Float_t xBeam,Float_t yBeam,Float_t& rPrime);
	Float_t getZprime(Float_t xBeam,Float_t yBeam);
	Float_t getRprime(Float_t xBeam,Float_t yBeam);

        Float_t getMomentumPID          (Int_t pid);
        Float_t getMomentumCorrectionPID(Int_t pid);
        Float_t getCorrectedMomentumPID (Int_t pid);
        Float_t getMass2PID(Int_t pid);
        Float_t getMass2CorrectionPID(Int_t pid);
        Float_t getCorrectedMass2PID(Int_t pid);
	void    setLayers(UInt_t io,UInt_t layers) { fLayers|=(layers&(0xFFF<<(io*12)));}
	void    setLayer(UInt_t io,UInt_t lay) { fLayers |=  ( 0x01 << (io*12+lay) );          }
	void    unsetAllLayers()               { fLayers &= ~0xFFFFFF; }
	static void setLayer(UInt_t io,UInt_t lay,UInt_t& layers) { layers |=  ( 0x01 << (io*12+lay) ); }
	Bool_t  getLayer(UInt_t io,UInt_t lay) { return ( fLayers &  ( 0x01 << (io*12+lay) )); }
	Int_t   getNLayer(UInt_t io);
	Int_t   getNLayerMod(UInt_t mod);
	Bool_t  hasLayers(UInt_t io,UInt_t layerstest) { return (((fLayers>>(io*12))&0xFFF)==((layerstest>>(io*12))&0xFFF));}
	Bool_t  isFakeRejected(Int_t io=-1) {
            if     (io==-1) return ( fLayers &  ( 0x01 << 31 ));
            else if(io==0 ) return ( fLayers &  ( 0x01 << 30 ));
            else if(io==1 ) return ( fLayers &  ( 0x01 << 29 ));
            else return kFALSE;
	}
        void  setFakeRejected(Int_t io=-1) {
            if     (io==-1) fLayers |=  ( 0x01 << 31 );
            else if(io==0 ) fLayers |=  ( 0x01 << 30 );
            else if(io==1 ) fLayers |=  ( 0x01 << 29 );
	}
        void  unsetFakeRejected(Int_t io=-1) {
            if     (io==-1) fLayers &= ~( 0x01 << 31 );
            else if(io==0 ) fLayers &= ~( 0x01 << 30 );
            else if(io==1 ) fLayers &= ~( 0x01 << 29 );
	}
        void  unsetAllFakeRejected() {
            fLayers &= ~( 0x07 << 29 );
	}
	void   setOffVertexClust()   { fLayers |=  ( 0x01 << 28 );}
	void   unsetOffVertexClust() { fLayers &= ~( 0x01 << 28 );}
	Bool_t isOffVertexClust()    { return ( fLayers &  ( 0x01 << 28 ));}

	void   setUsedVertex()       { fLayers |=  ( 0x01 << 27 );}
	void   unsetUsedVertex()     { fLayers &= ~( 0x01 << 27 );}
	Bool_t isUsedVertex()        { return ( fLayers &  ( 0x01 << 27 ));}

	void   setPrimaryVertex()       { fLayers |=  ( 0x01 << 26 );}
	void   unsetPrimaryVertex()     { fLayers &= ~( 0x01 << 26 );}
	Bool_t isPrimaryVertex()        { return ( fLayers &  ( 0x01 << 26 ));}

	void    printLayers();

	void  setNWSeg(UInt_t io,UInt_t nw) {
	    if(io<2){
		if(nw>31) nw = 31;
		fWires&= ~(0x1F<<(io*5));
		fWires|=((nw&0x1F)<<(io*5));
	    }
	}
	Int_t getNWSeg(UInt_t io){ if(io<2){ return  (fWires>>(io*5))&0x1F; } else { Int_t n = (fWires>>(0*5))&0x1F; n+= (fWires>>(1*5))&0x1F; return n;} }



	void setAtMdcEdge(UInt_t i){
	    if(i<4) {
                fWires|=((0x1)<<(i+10));
	    }
	}
	void unsetAtMdcEdge(UInt_t i){
	    if(i<4) {
                fWires&=~((0x1)<<(i+10));
	    }
	}
	Bool_t isAtMdcEdge(UInt_t i){
	    if(i<4){
		return ( fWires &  ( 0x01 << (10+i) ));
	    } else return kFALSE;
	}
        Bool_t isAtAnyMdcEdge(UInt_t io=2){
	    if(io<3){
		if(io==0||io==2){
		    if( fWires &  ( 0x01 << (10+0) )) return kTRUE;
		    if( fWires &  ( 0x01 << (10+1) )) return kTRUE;
		}
		if(io>=1){
		    if( fWires &  ( 0x01 << (10+2) )) return kTRUE;
		    if( fWires &  ( 0x01 << (10+3) )) return kTRUE;
		}
                return kFALSE;
	    } else return kFALSE;
	}

	void setMetaModule(UInt_t hit,UInt_t mod){
	    if(hit<2 && mod < 8){
		mod++;
		fmetaAddress&= ~(0xF<<(hit*11+7));
		fmetaAddress|=(mod&0xF)<<(hit*11+7);
	    }
	}
	void unsetMetaModule(UInt_t hit) { if(hit<2) fmetaAddress&= ~(0xF<<(hit*11+7)); }
	void setMetaCell(UInt_t hit,UInt_t cell) {
	    if(hit<2 && cell < 33){
		cell++;
		fmetaAddress&= ~(0x7F<<(hit*11));
		fmetaAddress|=(cell&0x7F)<<(hit*11);
	    }
	}
	void  unsetMetaCell(UInt_t hit) { if(hit<2) fmetaAddress&= ~(0x7F<<(hit*11)); }

	Int_t getMetaModule(UInt_t hit) {  if(hit<2){ return (fmetaAddress>>(hit*11+7)&(0xF))-1; } else return -1; }
	Int_t getMetaCell  (UInt_t hit) {  if(hit<2){ return (fmetaAddress>>(hit*11)&(0x7F))-1;  } else return -1; }

	ClassDef(HParticleCand,10)  // A simple track of a particle
};


#endif // __HPARTICLECAND_H__
