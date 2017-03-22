#ifndef HRICH700GEOTESTQA_H
#define HRICH700GEOTESTQA_H

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

class HRich700DigiMapPar;

class HRich700GeoTestQa : public HReconstructor {

private:

   // Input / output categories
   HCategory* fCatKine;
   HCategory* fCatRichPhoton;        //! Pointer to the geant rich photon category
   HCategory* fCatRichDirect;        //! Pointer to the geant rich direct hits category
   HCategory* fCatRichMirror;        //!
   HCategory* fCatTrack;             //! Pointer to the cal data category
   HCategory* fCatCal;               //! Pointer to the richtrack category
   HCategory* fCatRichHit;           //!
   HRich700DigiMapPar* fDigiMap;     //!

   Int_t fEventNum;                  //!

   HRich700HistManager* fHM;

   vector<TCanvas*> fCanvas;
   string fOutputDir;


   void initHist();
   void fillMcHist();
   void drawHist();

   Bool_t isPrimaryElectron(HGeantKine* kine);
   void   createH2MeanRms  (TH2D* hist,TH1D** meanHist,TH1D** rmsHist);
   void   drawH2MeanRms    (TH2* hist, const string& canvasName);


   TCanvas* createCanvas(const string& name, const string& title, Int_t width,Int_t height);
   void     saveCanvasToImage();

public:
   HRich700GeoTestQa();
   ~HRich700GeoTestQa();

   Bool_t init();
   Bool_t reinit();
   Int_t  execute();
   Bool_t finalize();

   void setOutputDir(const string& dir) {fOutputDir = dir;}


   ClassDef(HRich700GeoTestQa, 0)

};

#endif // HRICH700GEOTESTQA_H
