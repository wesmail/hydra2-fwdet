//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichRingFind
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHRINGFIND_H
#define HRICHRINGFIND_H

#include "TArrayI.h"
#include "TList.h"
#include "TObject.h"

class HRichAnalysis;
class HRichAnalysisPar;
class HRichGeometryPar;
class HRichHit;


class HRichRingFind: public TObject {
private:

   HRichRingFind(const HRichRingFind& source);
   HRichRingFind& operator=(const HRichRingFind& source);


protected:

   HRichAnalysisPar *pAnalysisParams;
   HRichGeometryPar *pGeometryParams;

   Int_t maxRings;   // max number of rich hits in pRings array

   Int_t iInnerCount;
   Int_t iInnerPhot4;
   Int_t iInnerPhot8;
   Float_t fClusterSize;
   Float_t fClusterLMax4;
   Float_t fClusterLMax8;

   Int_t iRingImageSize;
   TArrayI iRingTempImage;
   Double_t HomogenDistr(Double_t left, Double_t right);

   Float_t xMeanMax;
   Float_t yMeanMax;
   Float_t xPadMeanMax;
   Float_t yPadMeanMax;
   Float_t thetaMeanMax;
   Float_t phiMeanMax;
   Int_t fMaxClusterSize;
   Int_t fMaxClusterSum;
   Int_t fMaxThrClusterSize;
   Int_t iCount;
   Int_t iHitCount;
   Int_t maxCols;
   Int_t maxRows;

   Int_t iMatrixSize;
   Int_t iMatrixHalfSize;

   Int_t ly_from;
   Int_t ly_to;
   Int_t lx_from;
   Int_t lx_to;
   Int_t d_col_ij;
   Int_t d_row_ij;
   Int_t d_col_jk;
   Int_t d_row_jk;
   Int_t d2_colrow_jk;
   Int_t d2_colrow_ij;

   TArrayI iPadActive;

public:

   TArrayI iPadPlane;
   TArrayI iPadPlaneCopy;

   TArrayI iPadCol;
   TArrayI iPadRow;

   Int_t iRingNr;
   HRichHit *pRings;

   TList fHitList1;
   TList fHitList2;
   TList fHitCandidate;

   HRichRingFind();
   virtual ~HRichRingFind();


   Bool_t init(HRichAnalysis*);


   Int_t GetAlgorithmNr(HRichAnalysis *showMe);
   Float_t CalcDistance(const HRichHit& ring1, const HRichHit& ring2); // uses integer pad as ring-center
   Float_t CalcDistanceMean(const HRichHit& ring1, const HRichHit& ring2); // uses fraction of pad as ring-center
   Float_t CalcDistance(Int_t x, Int_t y, const HRichHit& ring);
   Float_t CalcDistance(Int_t x1, Int_t y1, Int_t x2, Int_t y2);

   Bool_t TestDensity(HRichAnalysis *showYou, HRichHit *pHit);
   Bool_t TestBorder(HRichAnalysis *showYou, HRichHit *pHit, Int_t amplit);
   Bool_t TestDynamic(HRichAnalysis *showYou, HRichHit *pHit, Int_t amplit);
   Bool_t TestRatio(HRichAnalysis *showYou, HRichHit *pHit);
   Bool_t TestAsymmetry(HRichAnalysis *showYou, HRichHit *pHit, Int_t amplit);
   Bool_t TestRingCharge(HRichAnalysis *showYou, HRichHit *hit);
   Int_t  TestRing(HRichAnalysis *showYou, HRichHit *hit, Int_t amplit);

   Int_t CleanIdenticalPairs(HRichAnalysis* showMe);


   void MaxFinding(HRichAnalysis *showYou, TList *hitList, TArrayI *in, TArrayI *out,
                   Int_t ringnr, Float_t distance);

   void MaxAnalysis(HRichAnalysis *showMe, TList *hitList, TArrayI *in, TArrayI *out,
                    Int_t minAmpl);
   void MaxCluster(HRichAnalysis *showYou, TArrayI *in, TArrayI *out,
                   Int_t nowPad, Int_t maxCode, Int_t minAmpl);

   Int_t MaxLabAmpl(TList *hitList, Int_t maxCode);
   void MaxSelector(HRichAnalysis *showMe, TList *hitList, TArrayI *in, TArrayI *out);
   void MaxMarker(HRichAnalysis *showYou, TArrayI *in, TArrayI *out,
                  Int_t nowPad, Int_t maxCode);
   void CloseMaxRejection(TList *hitList);


   virtual void CalcRingParameters(HRichAnalysis *showMe, HRichHit *pHit);
   virtual void CalcFakeContribution(HRichAnalysis *showMe);
   void CalcRingClusters(HRichAnalysis *showYou, Int_t *dumpArr, HRichHit* pHit,
			 Int_t nowX, Int_t nowY);

   void RingFindFitMatrix(HRichAnalysis *showMe, Int_t minampl, Int_t distance, Int_t howmanyrings = 5);
   void RingFindHoughTransf(HRichAnalysis *showMe, Int_t minampl, Int_t distance, Int_t howmanyrings = 5);

   Int_t MatchRings(HRichAnalysis *showMe, TList *hitList1, TList *hitList2);

   Int_t Execute(HRichAnalysis *giveMe);


   ClassDef(HRichRingFind, 0)
};


#endif // HRICHRINGFIND_H
