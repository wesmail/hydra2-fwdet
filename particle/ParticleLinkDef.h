#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
//-------------------------------------
// enums
#pragma link C++ namespace Particle;
#pragma link C++ enum eDetBits;
#pragma link C++ enum eSelMeta;
#pragma link C++ enum eMatching;
#pragma link C++ enum eVertex;
#pragma link C++ enum eMomReco;
#pragma link C++ enum ePair;
#pragma link C++ enum eFlagBits;
#pragma link C++ enum eClosePairSelect;
#pragma link C++ enum ePairCase;
//-------------------------------------
// globals
#pragma link C++ global catParticleCand;
#pragma link C++ global catParticleEvtInfo;
#pragma link C++ global catParticleDebug;
#pragma link C++ global catParticleCal;
#pragma link C++ global catParticleMdc;
#pragma link C++ global catParticleBtRing;
#pragma link C++ global catParticleBtRingInfo;

//-------------------------------------
// data objects
#pragma link C++ class HParticleCand-;
#pragma link C++ class HParticleCandSim-;
#pragma link C++ class HParticleWallHit;
#pragma link C++ class HParticleWallHitSim;
#pragma link C++ class HParticleCal;
#pragma link C++ class HParticleMdc;
#pragma link C++ class HParticleEvtInfo-;
#pragma link C++ class HParticleEvtChara;
#pragma link C++ class HParticleBtRing-;
#pragma link C++ class HParticleBtRingInfo;
#pragma link C++ class HParticleBtRingF;
#pragma link C++ class HParticleBtClusterF;
#pragma link C++ class HParticleBtAngleTrafo;
#pragma link C++ class HParticleBtPar;
#pragma link C++ class HParticleBt;
//-------------------------------------
// reconstructors
#pragma link C++ class HParticleCandFiller;
#pragma link C++ class HParticleTrackSorter;
#pragma link C++ class HParticleTrackCleaner;
#pragma link C++ class HParticleRunningMeanI;
#pragma link C++ class HParticleEvtInfoFiller;
#pragma link C++ class HParticleStart2HitF;
#pragma link C++ class HParticleVertexFind;
#pragma link C++ class HParticlePathLengthCorr;
#pragma link C++ class HParticleT0Reco;

//-------------------------------------
// parameters
#pragma link C++ class HParticleCandFillerPar;
#pragma link C++ class HParticleContFact;

//-------------------------------------
// tools
#pragma link C++ class HParticleTool;
#pragma link C++ class HParticleTree;
#pragma link C++ class HParticleAngleCor;
#pragma link C++ class HParticleGeant;
#pragma link C++ class HParticleGeantDecay;
#pragma link C++ class HParticleGeantEvent;
#pragma link C++ class HParticleGeantPair;
#pragma link C++ class HParticlePair;
#pragma link C++ class HParticleDraw;
#pragma link C++ class HParticlePairDraw;
#pragma link C++ class HParticlePairMaker;
#pragma link C++ class HParticleBooker;
#pragma link C++ class HParticleCutRange;
#pragma link C++ class HParticleCut<HParticleCand>;
#pragma link C++ class HParticleCut<HParticleCandSim>;
#pragma link C++ class HParticleCut<HParticlePair>;
#pragma link C++ class HParticleCut<HParticleGeantPair>;
#pragma link C++ class HParticleCut<HParticleGeant>;
#pragma link C++ class HParticleCut<HParticleGeantDecay>;
#pragma link C++ class HParticleCut<HParticleEvtInfo>;
#pragma link C++ class HParticleCut<HRichHit>;
#pragma link C++ class HParticleCut<HRichHitSim>;
#pragma link C++ class HParticleCut<HEventHeader>;
#pragma link C++ class HParticleCut<HVertex>;
#pragma link C++ class HParticleCut<HGeantKine>;

//-------------------------------------
// helper classes
#pragma link C++ class pointers;
#pragma link C++ class closetrack;
#pragma link C++ class closeVec;
#pragma link C++ class trackinfo;
#pragma link C++ class tracksVec;
#pragma link C++ class mdc_trk;
#pragma link C++ class mdc_seg;
#pragma link C++ class rich_hit;
#pragma link C++ class tof_hit;
#pragma link C++ class shower_hit;
#pragma link C++ class emc_clst;
#pragma link C++ class rpc_clst;
#pragma link C++ class spline_track;
#pragma link C++ class rk_track;
#pragma link C++ class kal_track;
#pragma link C++ class candidate;


#pragma link C++ global gParticleBooker;

#endif
