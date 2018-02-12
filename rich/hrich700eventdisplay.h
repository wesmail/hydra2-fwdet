#ifndef HRICH700EVENTDISPLAY_H
#define HRICH700EVENTDISPLAY_H

#include "hlocation.h"
#include "hreconstructor.h"
#include "hcategory.h"
#include "hiterator.h"
#include "TCanvas.h"
#include "hrich700histmanager.h"

#include <vector>
#include <string>

using namespace std;

class  HRich700DigiPar;

class HRich700EventDisplay : public HReconstructor {

private:

   // Input / output categories
   HCategory* fCatRichPhoton;        //! Pointer to the geant rich photon category
   HCategory* fCatRichDirect;        //! Pointer to the geant rich direct hits category
   HCategory* fCatTrack;             //! Pointer to the cal data category
   HCategory* fCatCal;               //! Pointer to the richtrack category
   HCategory* fCatRichHit;           //!
   HCategory* fCatRichCal;           //!
   HCategory* fCatKine;              //!
   HRich700DigiPar* fDigiPar;     //!

   Int_t fEventNum;                  //!
   Bool_t fDrawRichPhotons;
   Bool_t fDrawRichDirects;
   Bool_t fDrawRichCals;
   Bool_t fDrawRichHits;
   Int_t fNofEventsToDraw;
   Int_t fNofDrawnEvents;

   string fOutputDir;

   HRich700HistManager* fHM;


   void drawOneEvent();
   void drawOneRing();
   void drawPmts(Double_t offsetX, Double_t offsetY, Bool_t drawSens);

public:
   HRich700EventDisplay();
   ~HRich700EventDisplay();

   Bool_t init();
   Bool_t reinit();
   Int_t  execute();
   Bool_t finalize();

   void setOutputDir(const string& dir){fOutputDir = dir;}
   void setNofEventsToDraw(Int_t nofEvents){fNofEventsToDraw = nofEvents;}


   ClassDef(HRich700EventDisplay, 0)

};

#endif // HRICH700EVENTDISPLAY_H
