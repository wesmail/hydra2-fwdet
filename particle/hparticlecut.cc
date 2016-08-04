#include "hparticlecut.h"

templateClassImp(HParticleCut)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleCut
//
// Helper class for cuts. The synthax for conditions is working like
// T->Draw. Internaly a TTreeFormular is used to map the conditions
// to the leaves and methods of the class. Since the class is using
// template pointers the cuts works for all objects which are added
// as templates in HParticleLinkDef.h (like #pragma link C++ class HParticleCut<HParticleCand>;)
// A cut has a name (should be unique), a cut number (better unique)
// and a condition as argument of creation. The cut object owns counters
// for the number of calls and the number of failed evaluations. The
// The counters cand be used to monitor the efficiency of the cut.
// The cut can be inversed by setInverse(Bool_t).
//
//#########################################################
// USAGE:
//  // outside eventloop:
//  HParticleCut<HParticleCand> cut1("BetaCut",1,"fBeta<1.2&&fBeta>0.9&&isFlagBit(30)==1");
//  // will create a cut object on HParticleCand, selecting beta range and
//  // full reconstrued particles
//  ....
//  //  inside eventloop:
//  if(cut1->eval(cand,0)) { // true if condition is fullfilled , count stat for version 0
//
//  }
//  if(cut1->eval(cand,1)) { // true if condition is fullfilled , count stat for version 1
//
//  }
//
//  ....
//  // after eventloop
//  cut1.print(); // show cut name,number,cut statistics and condition
//
////////////////////////////////////////////////////////////////////////////////
