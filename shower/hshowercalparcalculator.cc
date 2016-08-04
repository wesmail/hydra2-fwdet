#include "TROOT.h"
#include "hshowercalparcalculator.h"

#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "hcategory.h"
#include "hmatrixcatiter.h"
#include "hlocation.h"
#include "hshowerraw.h"
#include "hshowercalpar.h"
#include "hshowerhist.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "showerdef.h"

#include "hshowerhistcell.h"

//------------------------------------------------------------------------------

ClassImp(HShowerCalParCalculator)

//------------------------------------------------------------------------------

HShowerCalParCalculator::HShowerCalParCalculator(const Text_t *name,const Text_t *title,
                          Int_t iMethod, Float_t fParam1, Float_t fParam2)
                          : HReconstructor(name,title)
{
    clearAtBegin(iMethod, fParam1, fParam2);
}

//------------------------------------------------------------------------------

HShowerCalParCalculator::HShowerCalParCalculator(Int_t iMethod,
                                        Float_t fParam1, Float_t fParam2)
{
    clearAtBegin(iMethod, fParam1, fParam2);
}

//------------------------------------------------------------------------------

HShowerCalParCalculator::HShowerCalParCalculator(const Text_t *name,const Text_t *title,
                          const Text_t *nameOfMethod, Float_t fParam1,
                          Float_t fParam2)
                          : HReconstructor(name,title)
{
    clearAtBegin(0, fParam1, fParam2);
    setMethod(nameOfMethod);
}

//------------------------------------------------------------------------------

HShowerCalParCalculator::~HShowerCalParCalculator(void)
{
    if(fIter)
        delete fIter;
}

//------------------------------------------------------------------------------

void HShowerCalParCalculator::clearAtBegin(Int_t iMethod,
                                        Float_t fParam1, Float_t fParam2)
{
    m_pCalPar     = NULL;
    m_pCalParHist = NULL;
    fIter         = NULL;
    m_loc.set(4, 0, 0, 0, 0);
    m_zeroLoc.set(0);

    m_fParam1 = fParam1;
    m_fParam2 = fParam2;

    setMethod(iMethod);
}

//------------------------------------------------------------------------------

void HShowerCalParCalculator::setMethod(Int_t iMethod)
{
    if(((m_iMethod = iMethod) < 0) || (m_iMethod >= HSHC_MAX))
    {
        gROOT->Error("HShowerCalParCalculator", "No such method: %d\n",
                                            iMethod);
    }
}

//------------------------------------------------------------------------------

void HShowerCalParCalculator::setMethod(const Char_t* pName)
{
Int_t i;

    for(i = 0; i < HSHC_MAX; i++)
    {
        if((g_cpstr_HSHC_Methods[i] == NULL)
            || (strcasecmp(pName, g_cpstr_HSHC_Methods[i]) != 0))
        {
            continue;
        }

        setMethod(i);

        return;
    }

    gROOT->Error("HShowerCalParCalculator", "Unknown method: %s\n", pName);
}

//------------------------------------------------------------------------------

const Char_t* HShowerCalParCalculator::getMethodName(void)
{
    return g_cpstr_HSHC_Methods[m_iMethod];
}

//------------------------------------------------------------------------------

void HShowerCalParCalculator::initCalPar()
{
    HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                                  ->getDetector("Shower");
    HRuntimeDb* rtdb=gHades->getRuntimeDb();

    m_pCalPar = rtdb->getContainer("ShowerCalPar");
    if (m_pCalPar) {
      ((HShowerCalPar*)m_pCalPar)->
             setSetup(pShowerDet->getShowerSectors(),
                      pShowerDet->getShowerModules(),
                      pShowerDet->getRows(),
                      pShowerDet->getColumns());
    }

    m_pCalParHist = rtdb->getContainer("ShowerHist");
    if (m_pCalParHist) {
      ((HShowerHist*)m_pCalParHist)->
             setSetup(pShowerDet->getShowerSectors(),
                      pShowerDet->getShowerModules(),
                      pShowerDet->getRows(),
                      pShowerDet->getColumns());
    }
}

//------------------------------------------------------------------------------

Bool_t HShowerCalParCalculator::init()
{
    printf("initialization of shower calpar calculator\n");
    m_nExecutes = 0;
    HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                                  ->getDetector("Shower");

    m_pRawCat=gHades->getCurrentEvent()->getCategory(catShowerRaw);
    if (!m_pRawCat) {
      m_pRawCat=pShowerDet->buildCategory(catShowerRaw);

      if (!m_pRawCat) return kFALSE;
      else gHades->getCurrentEvent()
                         ->addCategory(catShowerRaw, m_pRawCat, "Shower");
    }

    initCalPar();
    fIter = (HIterator*)m_pRawCat->MakeIterator("native");

    return kTRUE;
}

//------------------------------------------------------------------------------

Bool_t HShowerCalParCalculator::reinit()
{
/*
    HRuntimeDb* rtdb=gHades->getRuntimeDb();

    HShowerCalAsic *pCalAsic = (HShowerCalAsic*)rtdb->
                                               getContainer("ShowerCalAsic");

    HShowerLookupAsic *pLookupAsic = (HShowerLookupAsic*)rtdb->
                                               getContainer("ShowerLookupAsic");


    if (pCalAsic && pLookupAsic) {
       ((HShowerCalPar*)getCalPar())->setAsicTables(pLookupAsic, pCalAsic);
    }
*/

    return kTRUE;
}

//------------------------------------------------------------------------------

void HShowerCalParCalculator::setCalPar(HParSet *pCalPar)
{
    if (m_pCalPar)
        delete m_pCalPar;

    m_pCalPar = pCalPar;
}

//------------------------------------------------------------------------------

void HShowerCalParCalculator::setCalParHist(HParSet *pCalParHist)
{
    if (m_pCalParHist)
        delete m_pCalParHist;

    m_pCalParHist = pCalParHist;
}

//------------------------------------------------------------------------------

Bool_t HShowerCalParCalculator::finalize(void)
{
    ((HShowerHist*)getCalParHist())
                ->calculate(m_nExecutes, (HShowerCalPar*)getCalPar(),
                        m_iMethod, m_fParam1, m_fParam2);

    m_pCalPar->setChanged();

    if(m_pCalPar->getInputVersion(1) == -1)
    {
        if(m_pCalPar->getInputVersion(2) == -1)
        {
            m_pCalPar->setInputVersion(1, 1);
            printf("No version. Set 1 to 1 -> %d",
                        m_pCalPar->getInputVersion(1));
        }
        else
        {
            printf("Change Input Version 2: %d ->",
                        m_pCalPar->getInputVersion(2));
            m_pCalPar->setInputVersion(m_pCalPar->getInputVersion(2) + 1, 2);
            printf(" %d\n", m_pCalPar->getInputVersion(2));
        }
    }
    else
    {
        printf("Change Input Version 1: %d ->", m_pCalPar->getInputVersion(1));
        m_pCalPar->setInputVersion(m_pCalPar->getInputVersion(1) + 1, 1);
        printf(" %d\n", m_pCalPar->getInputVersion(1));
    }

    return kTRUE;
}

//------------------------------------------------------------------------------

Int_t HShowerCalParCalculator::execute()
{
HShowerRaw *pRaw;

    fIter->Reset();
    while((pRaw = (HShowerRaw*)fIter->Next()))
    {
        m_loc[0]=pRaw->getSector();
        m_loc[1]=pRaw->getModule();
        m_loc[2]=pRaw->getRow();
        m_loc[3]=pRaw->getCol();
        ((HShowerHist*)getCalParHist())->fill(m_loc, (Int_t)pRaw->getCharge());
    }

    m_nExecutes++;

    return 0;
}
