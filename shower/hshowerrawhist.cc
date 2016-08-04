#include "hshowerrawhist.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "hcategory.h"
#include "hmatrixcatiter.h"
#include "hlocation.h"
#include "hshowerraw.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "showerdef.h"

#include "TH1.h"
#include "TH2.h"

ClassImp(HShowerRawHist)

HShowerRawHist::~HShowerRawHist(void) {
     if (fIter) delete fIter;
}

Bool_t HShowerRawHist::init() {
    printf("initialization of shower calibrater\n");
    m_nEvents = 0;
    HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                                  ->getDetector("Shower");

    m_pRawCat=gHades->getCurrentEvent()->getCategory(catShowerRaw);
    if (!m_pRawCat) {
      m_pRawCat=pShowerDet->buildCategory(catShowerRaw);

      if (!m_pRawCat) return kFALSE;
      else gHades->getCurrentEvent()
                         ->addCategory(catShowerRaw, m_pRawCat, "Shower");
    }

    fIter=(HIterator*)m_pRawCat->MakeIterator();

    bookHist();
    return kTRUE;
}

Bool_t HShowerRawHist::finalize(void) {
   finalizeHist();
   return kTRUE;
}

Int_t HShowerRawHist::execute()
{
  HShowerRaw *pRaw;

  Int_t n = 0;
 
  fIter->Reset();
  while((pRaw = (HShowerRaw *)fIter->Next()))
  {
    fillHist(pRaw);
    n++;
  }

  m_nEvents++;
  return 0;
}

/***********************************************************/
/* definition of private function                          */
/***********************************************************/

Bool_t HShowerRawHist::bookHist() {
  Char_t name[80];
  Char_t title[80];

  m_pChargeHist = new TH1F("ShowerRawCharge", "Shower Raw Level - Charge",
                   128, 0, 256); 

  for(Int_t i = 0; i < 3; i++) {
     sprintf(name, "ShowerRawCharge%d", i);
     sprintf(title, "Shower Raw Level - Charge in Module %d", i);
     m_pChargeModHist[i] = new TH1F(name, title, 128, 0, 256);   

     sprintf(name, "ShowerRawAvg%d", i);
     sprintf(title, "Shower Raw Level - Average Charge in Module %d", i);
     m_pChargeAvgHist[i] =new TH2F(name, title, 32, 0, 32, 32, 0, 32);
      
     sprintf(name, "ShowerRawFreq%d", i);
     sprintf(title, "Shower Raw Level - Freq in Module %d", i);
     m_pChargeFreqHist[i] =new TH2F(name, title, 32, 0, 32, 32, 0, 32);
  }
  return kTRUE;
}


Bool_t HShowerRawHist::fillHist(Int_t nModule, Int_t nRow, 
                                         Int_t nColumn, Float_t fCharge) {

  m_pChargeHist->Fill(fCharge);
  m_pChargeModHist[nModule]->Fill(fCharge);

  m_pChargeFreqHist[nModule]->Fill(nColumn, nRow, 1 );
  m_pChargeAvgHist[nModule]->Fill(nColumn, nRow, fCharge);

  return kTRUE; 
}

Bool_t HShowerRawHist::fillHist(HShowerRaw* pRaw) {
  Int_t mod = pRaw->getModule();
  Int_t row = pRaw->getRow();
  Int_t col = pRaw->getCol();
  Float_t charge = pRaw->getCharge();

  fillHist(mod, row, col, charge);

  return kTRUE;
}


Bool_t HShowerRawHist::finalizeHist() {
  for(Int_t i = 0; i < 3; i++) {
    m_pChargeAvgHist[i]->Divide(m_pChargeFreqHist[i]);
    m_pChargeFreqHist[i]->Scale(1.0/m_nEvents);
  }  
  writeHist();
  return kTRUE;
}

Bool_t HShowerRawHist::writeHist() {
  printf("writing histograms ...\n");
  m_pChargeHist->Write();
  for(Int_t i = 0; i < 3; i++) {
       m_pChargeModHist[i]->Write();  
       m_pChargeFreqHist[i]->Write();  
       m_pChargeAvgHist[i]->Write();  
  }

  return kTRUE;
}

