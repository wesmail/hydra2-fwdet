#ifndef HPARTICLESTART2HITF_H
#define HPARTICLESTART2HITF_H

#include "hlocation.h"
#include "hreconstructor.h"

#include "TNtuple.h"
#include "TFile.h"

#include <vector>

using namespace std;

class HCategory;

class HParticleStart2HitF : public HReconstructor {
private:
   HCategory*     fCatTof;      //! pointer to the tof hit data
   HCategory*     fCatTofClu;   //! pointer to the tof cluster data 
   HCategory*     fCatRpc;      //! pointer to the rpc hit data
   HCategory*     fCatRpcClu;   //! pointer to the rpc cluster data
   HCategory*     fCatStartCal; //! pointer to the start cal data
   HCategory*     fCatStartHit; //! pointer to the start hit data

   TNtuple *nt1;                //! Debug ntuple 1 (original tofs)
   TNtuple *nt2;                //! Debug ntuple 2 (start time for both modules)
   TNtuple *nt3;                //! Debug ntuple 3 (recontructed start times)
   TFile *out;                  //! Output file for debug
   
   Bool_t bDebug;               //! Flag to store debug ntuples
   Float_t startTimeOriginal;   //! Start time from original HStart2Hit
   Int_t   startFlag;           //! flag of reconstruction method (-1,0,1,2)

   vector<Int_t> firstCluster;  // all strips inside 0.5 ns arround best strip
   vector<Int_t> secondCluster; // all other strip not in first cluster
   Float_t       fSecondTime;   // closest second time
   Float_t findMinimumTime(void);      // find the minimum time-of-flight
   Float_t findOriginalStartTime(void);// find start time from HStart2Hit category
   Float_t findStartTime(Float_t tof); // find the proper start time
   void correctTof(Float_t corrTime);  // modify the HTofHit tof
   void correctRpc(Float_t corrTime);  // modify the HRpcHit tof
   void correctStart(Float_t corrTime);// modify the HStart2Hit time

   void setDebug(Bool_t mode = kTRUE) { bDebug = mode; };

public:
   HParticleStart2HitF(void);
   HParticleStart2HitF(const Text_t* name, const Text_t* title, Bool_t skip = kFALSE);
   ~HParticleStart2HitF(void) {}

   Bool_t init(void);
   Int_t  execute(void);
   Bool_t finalize(void);

   ClassDef(HParticleStart2HitF, 0) // Hit finder for START2 detector suing META hits
};

#endif /* !HPARTICLESTART2HITF_H */

