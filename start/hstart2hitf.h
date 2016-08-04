#ifndef HSTART2HITF_H
#define HSTART2HITF_H

#include "hlocation.h"
#include "hreconstructor.h"
#include <vector>


using namespace std;

class HCategory;
class HStart2HitFPar;

class HStart2HitF : public HReconstructor {
private:
   Bool_t          fSkipEvent; //! Flag to suppress event w/o found start time. Default is kFALSE
   HCategory*      fCatCal;    //! pointer to the cal data
   HCategory*      fCatHit;    //! pointer to the hit data
   HLocation       loc;        //! location for new hit object
   HStart2HitFPar* fPar;       //! pointer to hit finder parameter container

   vector<Int_t> firstCluster;  //! all strips inside 0.5 ns arround best strip
   vector<Int_t> secondCluster; //! all other strip not in first cluster
   Float_t       fSecondTime;   //! closest second time


public:
   HStart2HitF(void);
   HStart2HitF(const Text_t* name, const Text_t* title, Bool_t skip = kFALSE);
   ~HStart2HitF(void) {}

   Bool_t init(void);
   Int_t  execute(void);
   Bool_t finalize(void);

   ClassDef(HStart2HitF, 0) // Hit finder for START2 detector
};

inline Bool_t HStart2HitF::finalize(void)
{
   return kTRUE;
}

#endif /* !HSTART2HITF_H */

