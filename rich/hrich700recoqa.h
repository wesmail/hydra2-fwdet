#ifndef HRICH700RECOQA_H
#define HRICH700RECOQA_H

#include "hreconstructor.h"

#include "TCanvas.h"

#include <map>
#include <string>
#include <vector>

using namespace std;


class HCategory;
class HGeantKine;
class HRich700HistManager;

class HRich700RecoQa : public HReconstructor {

private:

   HCategory* fCatKine;           //!
   HCategory* fCatRichHit;        //!
   HCategory* fCatRichCal;        //!

   Int_t fEventNum;


   HRich700HistManager* fHM;
   map<Int_t, Int_t> fHitMap;
   Int_t fMinNofRichCalsAcc;
   Int_t fTrueQuota;
   vector<TCanvas*> fCanvas;
   string fOutputDir;

   void processEvent();
   void initHist();
   void drawHist();
   void fillAccRecHist();
   void drawAccRecEff(const string& canvasNameAccRec, const string& canvasNameEff, const string& histNameAcc, const string& histNameRec);
   void initRichHitMap();
   Bool_t isPrimaryElectron(HGeantKine* kine);
   Bool_t isRichAcc(Int_t trackId);

public:
   HRich700RecoQa();
   ~HRich700RecoQa();

   Bool_t init();
   Bool_t reinit();
   Int_t  execute();
   Bool_t finalize();

   void setOutputDir(const string& dir) {fOutputDir = dir;}
   ClassDef(HRich700RecoQa, 0)
};

#endif
