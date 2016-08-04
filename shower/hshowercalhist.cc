#include "hshowerrawhist.h"
#include "hshowercalhist.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "hcategory.h"
#include "hmatrixcatiter.h"
#include "hlocation.h"
#include "hshowerraw.h"
#include "hshowercal.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "showerdef.h"

#include "TH1.h"
#include "TH2.h"

ClassImp(HShowerCalHist)

HShowerCalHist::~HShowerCalHist(void) {
     if (fIter) delete fIter;
}

Bool_t HShowerCalHist::init() {
    printf("initialization of shower hist\n");
    m_nEvents = 0;
    HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                                  ->getDetector("Shower");

    m_pCalCat=gHades->getCurrentEvent()->getCategory(catShowerCal);
    if (!m_pCalCat) {
      m_pCalCat=pShowerDet->buildCategory(catShowerCal);

      if (!m_pCalCat) return kFALSE;
      else gHades->getCurrentEvent()
                         ->addCategory(catShowerCal, m_pCalCat, "Shower");
    }

    fIter=(HIterator*)m_pCalCat->MakeIterator();

    bookHist();
    return kTRUE;
}

Bool_t HShowerCalHist::finalize(void) {
   finalizeHist();
   return kTRUE;
}

Int_t HShowerCalHist::execute()
{
  HShowerCal *pCal;

  Int_t n = 0;
 
  fIter->Reset();
  while((pCal = (HShowerCal *)fIter->Next()))
  {
    fillHist(pCal);
    n++;
  }

  m_nEvents++;
  return 0;
}

/***********************************************************/
/* definition of private function                          */
/***********************************************************/

Bool_t HShowerCalHist::bookHist() {
  Char_t name[80];
  Char_t title[80];
   
  m_pChargeCalHist = new TH1F("ShowerCalCharge", "Shower Cal Level - Charge",
                   128, 0, 256); 

  for(Int_t i = 0; i < 3; i++) {
     sprintf(name, "ShowerCalCharge%d", i);
     sprintf(title, "Shower Cal Level - Charge in Module %d", i);
     m_pChargeCalModHist[i] = new TH1F(name, title, 128, 0, 256);   

     sprintf(name, "ShowerCalAvg%d", i);
     sprintf(title, "Shower Cal Level - Average Charge in Module %d", i);
     m_pChargeCalAvgHist[i] =new TH2F(name, title, 32, 0, 32, 32, 0, 32);
   //  m_pChargeCalAvgHist[i]->SetDrawOption(colz); 
     sprintf(name, "ShowerCalFreq%d", i);
     sprintf(title, "Shower Cal Level - Freq in Module %d", i);
     m_pChargeCalFreqHist[i] =new TH2F(name, title, 32, 0, 32, 32, 0, 32);
  }
  return kTRUE;
}


Bool_t HShowerCalHist::fillHist(Int_t nModule, Int_t nRow, 
                                         Int_t nColumn, Float_t fCharge) {

  m_pChargeCalHist->Fill(fCharge);
  m_pChargeCalModHist[nModule]->Fill(fCharge);

  m_pChargeCalFreqHist[nModule]->Fill(nColumn, nRow, 1 );
  m_pChargeCalAvgHist[nModule]->Fill(nColumn, nRow, fCharge);

  return kTRUE; 
}

Bool_t HShowerCalHist::fillHist(HShowerCal* pCal) {
  Int_t mod = pCal->getModule();
  Int_t row = pCal->getRow();
  Int_t col = pCal->getCol();
  Float_t charge = pCal->getCharge();

  fillHist(mod, row, col, charge);

  return kTRUE;
}


Bool_t HShowerCalHist::finalizeHist() {
  for(Int_t i = 0; i < 3; i++) {
    m_pChargeCalAvgHist[i]->Divide(m_pChargeCalFreqHist[i]);
    m_pChargeCalFreqHist[i]->Scale(1.0/m_nEvents);
  }  
  writeHist();
  return kTRUE;
}

Bool_t HShowerCalHist::writeHist() {
  printf("writing histograms ...\n");
  m_pChargeCalHist->Write();
  for(Int_t i = 0; i < 3; i++) {
       m_pChargeCalModHist[i]->Write();  
       m_pChargeCalFreqHist[i]->Write();  
       m_pChargeCalAvgHist[i]->Write();  
  }

  return kTRUE;
}

