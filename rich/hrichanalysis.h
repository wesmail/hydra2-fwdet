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
//  HRichAnalysis
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHANALYSIS_H
#define HRICHANALYSIS_H

#include "TArrayI.h"
#include "TObject.h"

#include "hreconstructor.h"
#include "hrichhit.h"
#include "hrichlabel.h"
#include "hrichpadsignal.h"

class HCategory;
class HIterator;
class HParSet;
class HRichAnalysisPar;
class HRichGeometryPar;
class HRichHit;
class HRichHitHeader;
class HRichPadClean;
class HRichPadLabel;
class HRichRingFind;


class HRichAnalysis: public HReconstructor {

private:
   HRichAnalysis(const HRichAnalysis& source);
   HRichAnalysis& operator=(const HRichAnalysis& source);

public:

   Int_t iActiveSector;

   Short_t* pLeftBorder;  // left border of active pad area
   Short_t* pRightBorder; // dito right side (largest active col in row)

   HRichPadSignal **pPads; //!
   HRichLabel *pLabelArea;
   HRichHit *pRings;
   HIterator* fIter; //!
   HIterator* fIterHitHeader; //!

   Int_t iPadFiredNr;
   Int_t fPadFired[6];
   Int_t iPadCleanedNr;
   Int_t iClusterCleanedNr;
   TArrayI iClustersCleaned;
   Int_t iLabelNr;
   Int_t iRingNr;
   Int_t iRingNrTot;
   Int_t sectorPairNrTot;
   Int_t allPairNrTot; //2 rings in different sectors

   Int_t iFakePad;
   Int_t iFakeLocalMax4;
   Int_t iFakeLocalMax8;

   Int_t maxFiredTotalPads;

protected:

   HRichPadClean *pPadClean;
   HRichPadLabel *pPadLabel;
   HRichRingFind *pRingFind;

   HRichPadSignal* pSectorPads; //points to pads of active sector
   Int_t maxCols;
   Int_t maxRows;
   Int_t maxPads;
   Int_t secWithCurrent;

   HCategory *m_pCalCat; //Pointer to the cal data category
   HCategory *m_pHitCat; //Pointer to the hit data category
   HCategory *m_pHitHdrCat; //Pointer to the hit header data category

   HRichAnalysisPar* fpAnalysisPar;
   HRichGeometryPar* fpGeomPar;

   TArrayI iPadActive;
   HRichHitHeader *hithdrLoop;//!

public:

   HRichAnalysis();
   HRichAnalysis(const Text_t *name, const Text_t *title, Bool_t kSkip = kFALSE);
   virtual ~HRichAnalysis();


   Bool_t init();
   Bool_t reinit();
   Bool_t finalize();
   Int_t execute();
   Bool_t initParameters();

   void Reset();

   Int_t GetPadsXNr() {
      return maxCols;
   }
   Int_t GetPadsYNr() {
      return maxRows;
   }

   void SetActiveSector(Int_t sectornr);
   Int_t GetActiveSector() {
      return iActiveSector;
   }
   Int_t SetNextSector();

   Int_t GetPadNr() {
      return maxPads;
   }

   HRichPadSignal* GetPad(Int_t padnr) {
      return &pSectorPads[padnr];
   }
   HRichPadSignal* GetPad(Int_t padx, Int_t pady) {
      return &pSectorPads[padx + pady*maxCols];
   }

   Int_t GetLabelNr() {
      return iLabelNr;
   }
   HRichLabel* GetLabel(Int_t labelnr) {
      return &pLabelArea[labelnr];
   }

   Int_t GetRingNr() {
      return iRingNr;
   }
   HRichHit* GetRing(Int_t ringnr) {
      return &pRings[ringnr];
   }


   Bool_t IsOut(Int_t x, Int_t y) {
      return (!(y >= 0 && y < maxRows &&
                x >= pLeftBorder[y] && x <= pRightBorder[y]));
   }

   Bool_t IsOut(Int_t nowPad, Int_t dx, Int_t dy);
   Bool_t IsOut(Int_t x, Int_t y, Int_t dx, Int_t dy);

   HCategory* getCalCat() {
      return m_pCalCat;
   }
   HCategory* getHitCat() {
      return m_pHitCat;
   }
   HCategory* getHitHdrCat() {
      return m_pHitHdrCat;
   }
   void setCalCat(HCategory* pCalCat) {
      m_pCalCat = pCalCat;
   }
   void setHitCat(HCategory* pHitCat) {
      m_pHitCat = pHitCat;
   }
   void setHitHdrCat(HCategory* pHitHdrCat) {
      m_pHitHdrCat = pHitHdrCat;
   }

   HRichAnalysisPar* getAnalysisPar() {
      return fpAnalysisPar ;
   }
   HRichGeometryPar* getGeometryPar() {
      return fpGeomPar     ;
   }
   void setAnalysisPar(HRichAnalysisPar* pPar) {
      fpAnalysisPar = pPar;
   }
   void setGeometryPar(HRichGeometryPar* pPar) {
      fpGeomPar     = pPar;
   }

protected:
   Bool_t  kSkipEvtIfNoRing;
   void clear();
   void updateHeaders(const Int_t nSec, Int_t nEvNr);
   void updateHits(Int_t nSec);

   Int_t getPadsIndex(Int_t nSec, Int_t nRow, Int_t nCol);
   Int_t* getPadsPointer(Int_t* pPads, Int_t nSec);


public:
   ClassDef(HRichAnalysis, 0)
};

//============================================================================

#endif // HRICHANALYSIS_H
