#ifndef HRICH700DIGITIZER_H
#define HRICH700DIGITIZER_H

#include "hlocation.h"
#include "hreconstructor.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hgeantrich.h"
#include "hrich700pmt.h"

#include <map>
#include <vector>
using namespace std;

class HGeantKine;
class HRich700DigiPar;

class HRich700Digitizer : public HReconstructor {

private:

   HCategory* fCatKine;       //!
   HCategory* fCatRichPhoton; //!
   HCategory* fCatRichDirect; //!
   HCategory* fCatRichCal;    //!
   HRich700DigiPar* fDigiPar; //!

   static HRich700Digitizer* fDigitizer; //!

   // if true: we store trackIds only for the converted photons
   // This can influence efficiency
   // Must be set to true when ideal ring finder is used
   Bool_t  fStoreOnlyConvertedPhotonTrackIds;

   //------------------------------------------------------
   // delta electron handling
   Bool_t  fUseDeltaElectrons;      //! switch for use/not use delta electron time smearing
   Bool_t  fUseDeltaMomSelection;   //! switch for use/not use momentum below momMaxDeltaElecCut for primary electrons to identify delta electrons
   Int_t   fIonID;                  //! beam ion (au ==109)
   Float_t fMomMaxDeltaElecCut;     //! delta electron smearing : primary electrons below this mom are considdered to be delta electrons  [MeV/c]
   Float_t fMomMinDeltaCut[6];      //! min mom cut per sector (account for different mirror materials) [MeV/c]
   Float_t fProbDeltaAccepted;      //! 0 - 1 probability to accept a delta electron (yield adjustment)
   map<HGeantKine*,Float_t> fmDeltaTrackProb;  //! map delta electron candidates to prob
   map<HGeantKine*,Float_t>::iterator fitDelta;//! map delta electron candidates to prob
   //------------------------------------------------------

   HRich700Pmt fPmt;

   void processEvent();

   void setProbabilityForDeltaElectrons();

   Bool_t workOnDeltaElectrons(HGeantKine* primary, Int_t sector);

   void processRichPhoton(HGeantRichPhoton* photon);

   void processRichDirect(HGeantRichDirect* direct);

   void addRichCal(Int_t sector, Int_t col, Int_t row, Int_t trackId);

   void addAllTrackIds();

   void addTrackId(Int_t sector, Int_t col, Int_t row, Int_t trackId);

   void addCrossTalkHit(Int_t sector, Int_t col, Int_t row, Int_t trackId);

   void addNoiseHits();

public:
   HRich700Digitizer(const Text_t* name ="Rich700Digitizer",  const Text_t* title= "Rich700Digitizer");

   ~HRich700Digitizer();

   Bool_t init();
   Bool_t reinit();
   Int_t  execute();
   Bool_t finalize();
   static HRich700Digitizer* getDigitizer() { return fDigitizer;}

   void setStoreOnlyConvertedPhotonTrackIds(Bool_t s){fStoreOnlyConvertedPhotonTrackIds = s;}

   //----------- using delta electrons -----------------
   void   setDeltaElectronUse(Bool_t use, Bool_t useDeltaMomSel=kFALSE, Int_t ionId=109,Float_t momCut=20.,Float_t probDelta=2.){ fProbDeltaAccepted = probDelta; fUseDeltaElectrons = use;fUseDeltaMomSelection = useDeltaMomSel; fIonID=ionId; fMomMaxDeltaElecCut = momCut; }
   Bool_t getDeltaElectronUse() { return fUseDeltaElectrons;}
   void   setDeltaElectronMinMomCut(Float_t s0=2.,Float_t s1=2.,Float_t s2=4.,Float_t s3=2.,Float_t s4=2.,Float_t s5=4.) { fMomMinDeltaCut[0]=s0; fMomMinDeltaCut[1]=s1; fMomMinDeltaCut[2]=s2;  fMomMinDeltaCut[3]=s3;  fMomMinDeltaCut[4]=s4; fMomMinDeltaCut[5]=s5; }

   ClassDef(HRich700Digitizer, 0)

};

#endif // HRICH700DIGITIZER_H
