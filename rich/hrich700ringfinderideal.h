#ifndef HRICH700RINGFINDERIDEAL_H
#define HRICH700RINGFINDERIDEAL_H

#include "hlocation.h"
#include "hreconstructor.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hgeantrich.h"
#include "hrichcalsim.h"

#include <vector>

using namespace std;

class HRich700DigiPar;

class HRich700RingFinderIdeal : public HReconstructor {

private:

   HCategory* fCatKine;    //!
   HCategory* fCatRichCal; //!
   HCategory* fCatRichHit; //!
   HRich700DigiPar* fDigiPar; //!

   void processEvent();

   void addRichHit(Int_t sector, Int_t trackId, const vector<HRichCalSim*>& cals);

public:
   HRich700RingFinderIdeal();

   ~HRich700RingFinderIdeal();

   Bool_t init();
   Bool_t reinit();
   Int_t  execute();
   Bool_t finalize();


public:
   ClassDef(HRich700RingFinderIdeal, 0)

};

#endif // HRICH700RINGFINDERIDEAL_H
