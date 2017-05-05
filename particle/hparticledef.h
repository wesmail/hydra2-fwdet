#ifndef __HPARTICLEDEF_H__
#define __HPARTICLEDEF_H__

#include "Rtypes.h"
#include "haddef.h"

typedef Float16_t SmallFloat;

const Cat_t catParticleCand   = PARTICLE_OFFSET + 1;
const Cat_t catParticleEvtInfo= PARTICLE_OFFSET + 2;
const Cat_t catParticleDebug  = PARTICLE_OFFSET + 3;
const Cat_t catParticleCal    = PARTICLE_OFFSET + 4;
const Cat_t catParticlePair   = PARTICLE_OFFSET + 5;
const Cat_t catParticleMdc    = PARTICLE_OFFSET + 6;
const Cat_t catParticleBtRing = PARTICLE_OFFSET + 7;
const Cat_t catParticleBtRingInfo = PARTICLE_OFFSET + 8;

namespace Particle {

    // Enumerated variables indicating which detector
    // has seen a HGeant particle
    // and combinations thereof (to be used as shorthand-notation)

enum eSelMeta {
	kNoUse     =-1,
	kTofClst   = 0,
	kTofHit1   = 1,
	kTofHit2   = 2,
	kRpcClst   = 3,
	kShowerHit = 4,
	kEmcClst   = 5
};

enum eDetBits
{
    //individual detectors
	kIsNotSet          = 0x00,  // This object does not contain valid data
	kIsInRICHIPU       = 0x01,  //    1 RICH IPU has seen this trackid
    
	kIsInOuterMDC      = 0x02,  //    2 Outer Mdc has seen this trackid
	kIsInRICH          = 0x04,  //    4 RICH has seen this trackid
	kIsInInnerMDC      = 0x08,  //    8 Inner Mdc has seen this trackid
    
	kIsInSHOWER        = 0x10,  //   16 Shower has seen this trackid
	kIsInEMC           = 0x10,  //   16 EMC has seen this trackid (SHOWER and EMC will not be used together)
	kIsInTOF           = 0x20,  //   32 TOF has seen this trackid
	kIsInRPC           = 0x40,  //   64 Rpc has seen this trackid
	kIsInMETA          = 0x80,  //  128 One of the Meta dets has seen this trackid
        kIsGhost           = 0x100, //  256 Tracking Ghost
	kIsInnerGhost      = 0x200, //  512 Tracking Ghost in inner MDC
	kIsOuterGhost      = 0x400  // 1024 Tracking Ghost in outer MDC
};

enum eMatching
{
    //individual detectors
	kIsNoMatch     = 0x00,  // This object does not contain valid data
	kIsRICHMDC     = 0x01,  // 1 RICH MDC matching
	kIsRICHRK      = 0x02   // 2 RICH RK matching
};

enum eVertex
{
	kVertexCluster     = 1,  // vertex from cluster finder
	kVertexSegment     = 2,  // vertex from fitted inner segments
	kVertexParticle    = 3,  // vertex from reconstructed particle candidates
	kVertexUser        = 4   // vertex provided by user
};

enum eMomReco
{
    //individual detectors
	kMomSpline     = 1,  // from spline
	kMomRK         = 2,  // from runge kutta
	kMomKalman     = 3   // from kalman
};

enum ePair
{
    //individual detectors
	kIsUS          =    0,  // This pair is unlike sign
	kIsLSPos       =    1,  // This pair is like sign, both positive
	kIsLSNeg       =    2   // This pair is like sign, both negative
};

// -------------------------------------------------------------------------
// Track Cleaner flags
enum eFlagBits {
    kIsDoubleHitRICH       =  0,    //
    kIsDoubleHitInnerMDC   =  1,    //
    kIsDoubleHitOuterMDC   =  2,    //
    kIsDoubleHitMETA       =  3,    //

    kIsBestHitRICH         =  4,    // (by number of pads)
    kIsBestHitInnerMDC     =  5,    // (by chi2,  chi2 >= 0 )
    kIsBestHitOuterMDC     =  6,    // (by chi2 , chi2 >= 0 )
    kIsBestHitMETA         =  7,    // (by RKMETA match quality)
    kIsBestRK              =  8,    // (by RK chi2, none fitted outer segments with lower priority)
    kIsBestRKRKMETA        =  9,    // (by RK chi2 * RK META match quality)
    kIsBestRKRKMETARadius  = 10,    // (by RK chi2 * RK META match radius)
    kIsBestUser            = 11,    // (user function)

    kIsAcceptedHitRICH     = 12,    // remember if the hit was been taken into account
    kIsAcceptedHitRICHMDC  = 13,    // RICH hit + ring correlation segment
    kIsAcceptedHitInnerMDC = 14,    // needs inner MDC chi2 >= 0
    kIsAcceptedHitOuterMDC = 15,    // needs outer MDC chi2 >= 0
    kIsAcceptedHitMETA     = 16,    // needs Meta Hit
    kIsAcceptedRKMETA      = 17,    // needs RK chi2 >= 0 + Meta hit
    kIsAcceptedRKRICH      = 18,    // needs RK chi2 >= 0 + RICH hit + ring correlation after RK
    kIsAcceptedRK          = 19,    // needs RK chi2 >= 0
    kIsAcceptedRKRKMETA    = 20,    // needs RK chi2 >= 0 + Meta hit
    kIsAcceptedRKRKMETARadius= 21,  // needs RK chi2 >= 0 + Meta hit
    kIsAcceptedUser          = 22,  // needs RK chi2 >= 0 + Meta hit
    
    kIsLepton              = 29,    // mark the selected leptons
    kIsUsed                = 30,    // mark the object as arleady used
    kIsRejected            = 31     // mark the object as rejected

};
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
//  flags for HParticleTool::getCloseCand() , HParticleTool::evalCloseCandFlags()
enum eClosePairSelect {

    kSameRICH              = 0x01,        // 1: candidates share same RICH hit
    kSameInnerMDC          = 0x02,        // 2: candidates share innerMDC seg
    kSameOuterMDC          = 0x04,        // 3: candidates share outerMDC seg
    kSameMETA              = 0x08,        // 4: candidates share META hit (used META)
    kSamePosPolarity       = 0x10,        // 5: candidates share have same positive polarity (polarity has to be defined)
    kSameNegPolarity       = 0x20,        // 6: candidates share have same negative polarity (polarity has to be defined)
    kSamePolarity          = 0x40,        // 7: candidates share have same polarity (polarity has to be defined)

    kRICH2                 =  0x80,       // 8: candidate2 has RICH
    kFittedInnerMDC2       =  0x100,      // 9: candidate2 has fitted inner MDC seg
    kFittedOuterMDC2       =  0x200,      //10: candidate2 has fitted outer MDC seg
    kOuterMDC2             =  0x400,      //11: candidate2 has outer MDC seg
    kRK2                   =  0x800,      //12: candidate2 has fitted momentum
    kMETA2                 =  0x1000,     //13: candidate2 has META hit
    kIsLepton2             =  0x2000,     //14: candidate2 has kIsLepton
    kIsUsed2               =  0x4000,     //15: candidate2 has kIsUsed

    kNoSameRICH            =  0x8000,     //16:
    kNoSameInnerMDC        =  0x10000,    //17:
    kNoSameOuterMDC        =  0x20000,    //18:
    kNoSameMETA            =  0x40000,    //19:
    kNoSamePosPolarity     =  0x80000,    //20:
    kNoSameNegPolarity     =  0x100000,   //21:
    kNoSamePolarity        =  0x200000,   //22:

    kNoRICH2               =  0x400000,   //23: candidate2 has no RICH
    kNoFittedInnerMDC2     =  0x800000,   //24: candidate2 has no fitted inner MDC seg
    kNoFittedOuterMDC2     =  0x1000000,  //25: candidate2 has no fitted outer MDC seg
    kNoOuterMDC2           =  0x2000000,  //26: candidate2 has no outer MDC seg
    kNoRK2                 =  0x4000000,  //27: candidate2 has no fitted momentum
    kNoMETA2               =  0x8000000,  //28: candidate2 has no META hit
    kNoIsLepton2           =  0x10000000, //29: candidate2 has no kIsLepton
    kNoIsUsed2             =  0x20000000, //30: candidate2 has no kIsUsed
    kNoUseRICH             =  0x40000000  //31: do not take RICH into account

};

// predefined pair cases
enum ePairCase {
    // LEPTON CASES
    kPairCase1            = kNoSameRICH|kNoSameInnerMDC|kNoSameOuterMDC|kNoSameMETA|kRICH2|kOuterMDC2|kMETA2,
    kPairCase2            = kNoSameRICH|kNoSameInnerMDC|kNoSameOuterMDC|kSameMETA  |kRICH2|kOuterMDC2|kMETA2,
    kPairCase3            = kNoSameRICH|kNoSameInnerMDC|kSameOuterMDC  |kSameMETA  |kRICH2|kOuterMDC2|kMETA2,
    kPairCase4            = kNoSameRICH|kNoSameInnerMDC|kSameOuterMDC  |kNoSameMETA|kRICH2|kOuterMDC2|kMETA2,
    kPairCase5            = kNoSameRICH|kSameInnerMDC  |kNoSameOuterMDC|kNoSameMETA|kRICH2|kOuterMDC2|kMETA2,
    kPairCase6            = kSameRICH  |kSameInnerMDC  |kNoSameOuterMDC|kNoSameMETA|kRICH2|kOuterMDC2|kMETA2,
    kPairCase7            = kSameRICH  |kNoSameInnerMDC|kNoSameOuterMDC|kNoSameMETA|kRICH2|kOuterMDC2|kMETA2,
    kPairCase8            = kSameRICH  |kNoSameInnerMDC|kNoSameOuterMDC|kSameMETA  |kRICH2|kOuterMDC2|kMETA2,
    kPairCase9            = kSameRICH  |kSameInnerMDC  |kNoSameOuterMDC|kSameMETA  |kRICH2|kOuterMDC2|kMETA2,
    kPairCase10           = kSameRICH  |kSameInnerMDC  |kSameOuterMDC  |kNoSameMETA|kRICH2|kOuterMDC2|kMETA2,
    kPairCase11           = kNoSameRICH|kNoSameInnerMDC|kNoSameOuterMDC            |kRICH2|kOuterMDC2|kNoMETA2,
    kPairCase12           = kSameRICH  |kNoSameInnerMDC|kNoSameOuterMDC            |kRICH2|kOuterMDC2|kNoMETA2,
    kPairCase13           = kSameRICH  |kSameInnerMDC  |kNoSameOuterMDC            |kRICH2|kOuterMDC2|kNoMETA2,
    kPairCase14           = kNoSameRICH|kNoSameInnerMDC                            |kRICH2|kNoOuterMDC2|kNoMETA2,
    kPairCase15           = kSameRICH  |kNoSameInnerMDC                            |kRICH2|kNoOuterMDC2|kNoMETA2,
    // HADRON CASES
    kPairCase16           = kNoUseRICH |kNoSameInnerMDC|kNoSameOuterMDC|kNoSameMETA|kOuterMDC2|kMETA2,
    kPairCase17           = kNoUseRICH |kNoSameInnerMDC|kNoSameOuterMDC|kSameMETA  |kOuterMDC2|kMETA2,
    kPairCase18           = kNoUseRICH |kNoSameInnerMDC|kSameOuterMDC  |kSameMETA  |kOuterMDC2|kMETA2,
    kPairCase19           = kNoUseRICH |kNoSameInnerMDC|kSameOuterMDC  |kNoSameMETA|kOuterMDC2|kMETA2,
    kPairCase20           = kNoUseRICH |kSameInnerMDC  |kNoSameOuterMDC|kSameMETA  |kOuterMDC2|kMETA2,
    kPairCase21           = kNoUseRICH |kSameInnerMDC  |kSameOuterMDC  |kNoSameMETA|kOuterMDC2|kMETA2,
    kPairCase22           = kNoUseRICH |kSameInnerMDC  |kNoSameOuterMDC|kNoSameMETA|kOuterMDC2|kMETA2,
    kPairCase23           = kNoUseRICH |kNoSameInnerMDC|kNoSameOuterMDC            |kOuterMDC2|kNoMETA2,
    kPairCase24           = kNoUseRICH |kSameInnerMDC  |kNoSameOuterMDC            |kOuterMDC2|kNoMETA2,
    kPairCase25           = kNoUseRICH |kNoSameInnerMDC                            |kNoOuterMDC2|kNoMETA2,
    // LEPTON HADRON MIXED CASES
    kPairCase26           = kNoSameRICH|kNoSameInnerMDC|kNoSameOuterMDC|kNoSameMETA|kNoRICH2|kOuterMDC2|kMETA2,
    kPairCase27           = kNoSameRICH|kNoSameInnerMDC|kNoSameOuterMDC|kSameMETA  |kNoRICH2|kOuterMDC2|kMETA2,
    kPairCase28           = kNoSameRICH|kNoSameInnerMDC|kSameOuterMDC  |kSameMETA  |kNoRICH2|kOuterMDC2|kMETA2,
    kPairCase29           = kNoSameRICH|kNoSameInnerMDC|kSameOuterMDC  |kNoSameMETA|kNoRICH2|kOuterMDC2|kMETA2,
    kPairCase30           = kNoSameRICH|kSameInnerMDC  |kNoSameOuterMDC|kNoSameMETA|kNoRICH2|kOuterMDC2|kMETA2,
    kPairCase31           = kNoSameRICH|kNoSameInnerMDC|kNoSameOuterMDC            |kNoRICH2|kOuterMDC2|kNoMETA2,
    kPairCase32           = kNoSameRICH|kNoSameInnerMDC                            |kNoRICH2|kNoOuterMDC2|kNoMETA2

};
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
//  flags for HParticleEvtInfo , event selectors
enum eEventSelect {  // all bits evaluate good

    kGoodVertexClust       = 0x01,        // 1: 1     good cluster vertex   (chi2>0, z > -65 (apr12), -160 (Jul14+aug14)) (requires 1 track)
    kGoodVertexCand        = 0x02,        // 2: 2     good candidate vertex (chi2>0, z > -65 (apr12), -160 (Jul14+aug14)) (requires at least 2 reconstructed particles)
    kGoodSTART             = 0x04,        // 3: 4     good StartHit         (object exists (+ corrflag > -1 if HParticleStart2HitF was used)
    kNoPileUpSTART         = 0x08,        // 4: 8     no pileup in START (object exists + no second cluster)
    kNoPileUpMETA          = 0x10,        // 5: 16    no pileup in META  (no objects tof<0 or tof>60, threshold 0 (pionbeam)  5 apr12
    kNoPileUpMDC           = 0x20,        // 6: 32    no pileup in MDC   (objects t1>200 (inner or t1>400 (outer)), threshold 7 (pionbeam) 35 (apr12))
    kNoFlashMDC            = 0x40,        // 7: 64    flash event in MDC
    kGoodMDCMult           = 0x80,        // 8: 128   good sectors inside normal mult
    kGoodMDCMIPSMult       = 0x100,       // 9: 256   good sectors inside normal mult for MIPS
    kGoodLepMult           = 0x200,       //10: 512   good sectors inside normal mult
    kGoodTRIGGER           = 0x400,       //11: 1024  phys trigger (pt1 jul14/aug14, pt3 apr12)
    kGoodSTART2            = 0x800,       //12: 2048  good StartHit         (object exists (+ corrflag ==2 if HParticleStart2HitF was used)
    kNoVETO                = 0x1000,      //13: 4096  no VETO hit +- 10ns arround START
    kGoodSTARTVETO         = 0x2000,      //14: 8192  no starthit >15ns <350ns uncorrelated with veto
    kGoodSTARTMETA         = 0x4000       //15: 16384 no starthit >80ns <350ns  correlated with metahits
};

// -------------------------------------------------------------------------
//  flags for beam times
enum eBeamTime {
    kUnknownBeam = 0,
    kApr12       = 1,
    kJul14       = 2,
    kAug14       = 3

};

}

#endif // __HPARTICLEDEF_H__
