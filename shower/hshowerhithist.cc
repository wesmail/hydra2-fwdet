#include "hshowerrawhist.h"
#include "hshowercalhist.h"
#include "hshowerhithist.h"
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

ClassImp(HShowerHitHist)

    HShowerHitHist::~HShowerHitHist(void) {
	if (fIter) delete fIter;
    }

Bool_t HShowerHitHist::init() {
    printf("initialization of shower hist\n");
    m_nEvents = 0;

    m_pHitCat=gHades->getCurrentEvent()->getCategory(catShowerHit);
    if (m_pHitCat) {
	fIter = (HIterator*)m_pHitCat->MakeIterator();
    }
    bookHist();
    return kTRUE;
}

Bool_t HShowerHitHist::finalize(void) {
    finalizeHist();
    return kTRUE;
}

Int_t HShowerHitHist::execute()
{
    if(fIter == 0) return 0; // noting todo
    HShowerHit *pHit;

    Int_t n = 0;

    fIter->Reset();
    while((pHit = (HShowerHit *)fIter->Next()))
    {
	fillHist(pHit);
	n++;
    }

    m_nEvents++;
    return 0;
}

/***********************************************************/
/* definition of private function                          */
/***********************************************************/

Bool_t HShowerHitHist::bookHist() {
    Char_t name[80];
    Char_t title[80];

    m_pChargeHitHist = new TH1F("ShowerHitCharge", "Shower Hit Level - Charge",
				128, 0, 256);

    for(Int_t i = 0; i < 3; i++) {
	sprintf(name, "ShowerHitCharge%d", i);
	sprintf(title, "Shower Hit Level - Charge in Module %d", i);
	m_pChargeHitModHist[i] = new TH1F(name, title, 128, 0, 256);

	sprintf(name, "ShowerHitAvg%d", i);
	sprintf(title, "Shower Hit Level - Average Charge in Module %d", i);
	m_pChargeHitAvgHist[i] =new TH2F(name, title, 32, 0, 32, 32, 0, 32);
	//  m_pChargeHitAvgHist[i]->SetDrawOption(colz);
	sprintf(name, "ShowerHitFreq%d", i);
	sprintf(title, "Shower Hit Level - Freq in Module %d", i);
	m_pChargeHitFreqHist[i] =new TH2F(name, title, 32, 0, 32, 32, 0, 32);
    }
    return kTRUE;
}


Bool_t HShowerHitHist::fillHist(Int_t nModule, Int_t nRow,
				Int_t nColumn, Float_t fCharge) {

    m_pChargeHitHist->Fill(fCharge);
    m_pChargeHitModHist[nModule]->Fill(fCharge);

    m_pChargeHitFreqHist[nModule]->Fill(nColumn, nRow, 1 );
    m_pChargeHitAvgHist[nModule]->Fill(nColumn, nRow, fCharge);

    return 1;
}

Bool_t HShowerHitHist::fillHist(HShowerHit* pHit) {
    Int_t mod = pHit->getModule();
    Int_t row = pHit->getRow();
    Int_t col = pHit->getCol();
    Float_t charge = pHit->getCharge();

    fillHist(mod, row, col, charge);

    return kTRUE;
}


Bool_t HShowerHitHist::finalizeHist() {
    for(Int_t i = 0; i < 3; i++) {
	m_pChargeHitAvgHist[i]->Divide(m_pChargeHitFreqHist[i]);
	m_pChargeHitFreqHist[i]->Scale(1.0/m_nEvents);
    }
    writeHist();
    return kTRUE;
}

Bool_t HShowerHitHist::writeHist() {
    printf("writing histograms ...\n");
    m_pChargeHitHist->Write();
    for(Int_t i = 0; i < 3; i++) {
	m_pChargeHitModHist[i]->Write();
	m_pChargeHitFreqHist[i]->Write();
	m_pChargeHitAvgHist[i]->Write();
    }

    return kTRUE;
}

