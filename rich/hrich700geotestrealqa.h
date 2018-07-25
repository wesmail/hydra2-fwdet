#ifndef HRICH700GEOTESTREALQA_H
#define HRICH700GEOTESTREALQA_H

#include "hlocation.h"
#include "hreconstructor.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hrich700histmanager.h"
#include "hgeantkine.h"

#include "TCanvas.h"

#include <vector>
#include <string>

using namespace std;

class HRich700DigiPar;

class HRich700GeoTestRealQa : public HReconstructor {

private:

   // Input / output categories
   HCategory* fCatRichCal; //!
   HCategory* fCatRichHit; //!
   HCategory* fCatRichRaw; //!
   HRich700DigiPar* fDigiPar;     //!

   Int_t fEventNum;                  //!

   HRich700HistManager* fHM;
   string fOutputDir;


   void initHist();
   void fillHistCals();
   void fillHistRaw();
   void drawHist();

public:
   HRich700GeoTestRealQa();
   ~HRich700GeoTestRealQa();

   Bool_t init();
   Bool_t reinit();
   Int_t  execute();
   Bool_t finalize();

   void setOutputDir(const string& dir) {fOutputDir = dir;}


   ClassDef(HRich700GeoTestRealQa, 0)

};

#endif // HRICH700GEOTESTQA_H
