#ifndef HRICH700DIGIPARCREATOR_H
#define HRICH700DIGIPARCREATOR_H

#include "hlocation.h"
#include "hreconstructor.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hrich700histmanager.h"
#include "hgeantkine.h"
#include "hrich700histmanager.h"
#include "hrich700ringfinderpar.h"

#include "TCanvas.h"

#include <vector>
#include <string>
#include "TH1F.h"

using namespace std;

class HRich700DigiPar;

class HRich700DigiParCreator : public HReconstructor {

private:

   HCategory* fCatKine;
   HCategory* fCatRichHit;
   HRich700DigiPar* fDigiPar;
   HRich700RingFinderPar* fRingFinderPar;

   Int_t fEventNum;

   static const Int_t knPmt = 600;
   TH1F* fhPhi[knPmt];
   TH1F* fhTheta[knPmt];

   static const Int_t knXY = 220;    // -660,660  [6mm]
   TH1F* fhPhiXY[knXY][knXY];
   TH1F* fhThetaXY[knXY][knXY];

   TH2F* fhXYCounter;
   TH2F* fhXYPhiMean;
   TH2F* fhXYThetaMean;
   TH2F* fhXYPhiRms;
   TH2F* fhXYThetaRms;

   TH2F* fhXYIndCounter;
   TH2F* fhXYIndPhiMean;
   TH2F* fhXYIndThetaMean;
   TH2F* fhXYIndPhiRms;
   TH2F* fhXYIndThetaRms;

   TH2F* fhXYPhiInterpolation;
   TH2F* fhXYThetaInterpolation;

   string fOutputFilePath;
   string fOutputHistFilePath;

   HRich700HistManager* fHM;

   void processEvent();
   string createStringPhiThetaPmtId();
   string createStringPhiThetaXY();
   void createOutputFile();

   void fillInterpolatedHist();
   void writeHist();
   void initHist();
   void drawHist();

   Double_t calculateHistMean(TH1* hist, Double_t minVal, Double_t maxVal);

   Bool_t isPrimaryElectron(HGeantKine* kine);

public:
   HRich700DigiParCreator();
   ~HRich700DigiParCreator();

   Bool_t init();
   Bool_t reinit();
   Int_t  execute();
   Bool_t finalize();

   void setOutputFilePath(const string& filePath) {fOutputFilePath = filePath;}
   void setOutputHistFilePath(const string& filePath) {fOutputHistFilePath = filePath;}

   void drawFromFile(const string& fileName, const string& outputDir);

   ClassDef(HRich700DigiParCreator, 0)

};

#endif // HRICH700DIGIPARCREATOR_H
