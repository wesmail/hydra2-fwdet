#ifndef __HADDEF_H
#define __HADDEF_H

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

typedef Short_t Cat_t;
typedef Short_t IdOfs_t;
typedef UInt_t Loc_t;


// Hydra Global Variables
const Int_t kBitCategorySize=5;
const Int_t kCategorySize=32;

const Int_t rlRaw=0;
const Int_t rlHit=-1;
const Int_t rlUndefined=-2;

const Int_t ctUnknown=-1;
const Int_t ctMatrix=0;
const Int_t ctSplit=1;
const Int_t ctLinear=2;

const UInt_t kStartEvent=0xd;
const UInt_t kLastEvent=0xe;

//Task control variable
const Int_t kSkipEvent=-9;
const Int_t MAX_PART_EVENTS=22;

// Main Partial Event  identifiers

const Int_t INCREMENT=32;
const Int_t STARTVALUE=0; 
const Int_t MDC_OFFSET=STARTVALUE;                    // 0
const Int_t RICH_OFFSET= STARTVALUE + 1*INCREMENT;    // 32
const Int_t SHOWER_OFFSET= STARTVALUE + 2*INCREMENT;  // 64
const Int_t TOF_OFFSET= STARTVALUE + 3*INCREMENT;     // 96
const Int_t START_OFFSET= STARTVALUE + 4*INCREMENT;   // 128
const Int_t TOFINO_OFFSET= STARTVALUE + 5*INCREMENT;  // 160
const Int_t SIMUL_OFFSET= STARTVALUE + 6*INCREMENT;   // 192
const Int_t RPC_OFFSET= STARTVALUE + 7*INCREMENT;     // 224
const Int_t MATCHU_OFFSET= STARTVALUE + 8*INCREMENT;  // 256
const Int_t TRACK_OFFSET= STARTVALUE + 9*INCREMENT;   // 288
const Int_t PHYANA_OFFSET= STARTVALUE + 10*INCREMENT; // 320
const Int_t PID_OFFSET = STARTVALUE + 11*INCREMENT;   // 352
const Int_t HYP_OFFSET = STARTVALUE + 12*INCREMENT;   // 384
const Int_t WALL_OFFSET = STARTVALUE + 13*INCREMENT;  // 416
const Int_t PAIRS_OFFSET = STARTVALUE + 14*INCREMENT; // 448 
const Int_t HODO_OFFSET = STARTVALUE + 15*INCREMENT;  // 480 
const Int_t PARTICLE_OFFSET = STARTVALUE + 16*INCREMENT;  // 512
const Int_t TRBNET_OFFSET= STARTVALUE + 17 * INCREMENT;   // 544
const Int_t EMC_OFFSET = STARTVALUE + 18*INCREMENT;       // 576
const Int_t PIONTRACKER_OFFSET = STARTVALUE + 19*INCREMENT;  // 608
const Int_t FWDET_OFFSET = STARTVALUE + 20*INCREMENT;     // 640

//define offset for the partial events
const Cat_t catMdc=     MDC_OFFSET;   
const Cat_t catRich=    RICH_OFFSET;
const Cat_t catShower=  SHOWER_OFFSET;
const Cat_t catTof=     TOF_OFFSET;
const Cat_t catStart=   START_OFFSET;
const Cat_t catTofino=  TOFINO_OFFSET;
const Cat_t catWall=    WALL_OFFSET;
const Cat_t catHodo =   HODO_OFFSET;
const Cat_t catSimul=   SIMUL_OFFSET;
const Cat_t catRpc=     RPC_OFFSET;
const Cat_t catEmc=     EMC_OFFSET;
const Cat_t catPionTracker= PIONTRACKER_OFFSET;
const Cat_t catFwDet=   FWDET_OFFSET;
const Cat_t catMatchU=  MATCHU_OFFSET;
const Cat_t catTracks=  TRACK_OFFSET;
const Cat_t catPhyAna=  PHYANA_OFFSET;
const Cat_t catPairs =  PAIRS_OFFSET;
const Cat_t catParticle =  PARTICLE_OFFSET;
const Cat_t catTrbNet = TRBNET_OFFSET;

const Cat_t catFlex=126;
const Cat_t catTrack=127;
const Cat_t catInvalid=-1;

#endif






