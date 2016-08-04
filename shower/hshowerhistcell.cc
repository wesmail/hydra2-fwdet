#pragma implementation

#include "TROOT.h"

#include "TH2.h"
#include "TH3.h"
#include "hshowerhistcell.h"
#include "hshowercalpar.h"
#include "hshowergeometry.h"
#include "hshowerpad.h"
#include "hades.h"
#include "hruntimedb.h"

//------------------------------------------------------------------------------

ClassImp(HShowerHistCell)

//------------------------------------------------------------------------------

const Char_t* g_cpstr_HSHC_Methods[HSHC_MAX] =
        { "Test", "CalElect", "RunSigma", "RunThreshold",
          "CalSigma", "CalThreshold", "RunIntegrate", "CalIntegrate" };

//------------------------------------------------------------------------------

#define ROUND_INT(A) {if((Int_t)(A) != (A)) \
                            A = (Float_t)((Int_t)(A) - 1);}

//------------------------------------------------------------------------------

HShowerHistCell::HShowerHistCell()
{
    m_pHist = NULL;
    m_pSlopeHist = NULL;
    m_pOffsetHist = NULL;

    setSector(-1);
    setModule(-1);
    setRows(0);
    setCols(0);

    m_nDataSize = 0;
    m_pData = NULL;
    m_pGeom = NULL;

    m_fChannel10pC = DEF_CHANNEL_10pC;
}

//------------------------------------------------------------------------------

HShowerHistCell::HShowerHistCell(Int_t nSect, Int_t nMod,
                 Int_t nRows, Int_t nCols)
{
    m_pHist       = NULL;
    m_pSlopeHist  = NULL;
    m_pOffsetHist = NULL;

    setSector(nSect);
    setModule(nMod);
    setRows(nRows);
    setCols(nCols);

    m_nDataSize = 0;
    m_pData = NULL;
    m_pGeom = NULL;

    m_fChannel10pC = 500.0;
}

//------------------------------------------------------------------------------

HShowerHistCell::~HShowerHistCell()
{
    deleteHist();
}

//------------------------------------------------------------------------------

Bool_t HShowerHistCell::reset()
{
    m_pHist->Reset();
    m_pSlopeHist->Reset();
    m_pOffsetHist->Reset();

    return kTRUE;
}

//------------------------------------------------------------------------------

Bool_t HShowerHistCell::book()
{
    if(m_nRows * m_nCols==0)
        return kFALSE;

Char_t name[60];
Char_t title[60];
Char_t name2[60];
Char_t title2[60];

    deleteHist();

    getName(name, sizeof(name));
    getTitle(title, sizeof(title));

    m_pHist = new TH3S(name, title,
                           m_nRows, 0, m_nRows,
                           m_nCols, 0, m_nCols,
                           1024, 0, 1024);

    m_nDataSize = m_pHist->GetNbinsZ();
    m_pData = new Int_t[m_nDataSize];

    sprintf(name2, "%sSlope", name);
    sprintf(title2, "%s - Slope", title);

    m_pSlopeHist = new TH2F(name2, title,
                           m_nRows, 0, m_nRows,
                           m_nCols, 0, m_nCols);

    m_pSlopeHist->GetXaxis()->SetTitle("columns");
    m_pSlopeHist->GetYaxis()->SetTitle("rows");

    sprintf(name2, "%sOffset", name);
    sprintf(title2, "%s - Offset", title);

    m_pOffsetHist = new TH2F(name2, title,
                           m_nRows, 0, m_nRows,
                           m_nCols, 0, m_nCols);

    m_pOffsetHist->GetXaxis()->SetTitle("columns");
    m_pOffsetHist->GetYaxis()->SetTitle("rows");

    return kTRUE;
}

//------------------------------------------------------------------------------

Bool_t HShowerHistCell::book(Int_t nRows, Int_t nCols)
{
    if(nRows * nCols == 0)
        return kFALSE;

    setRows(nRows);
    setCols(nCols);

    return book();
}

//------------------------------------------------------------------------------

Bool_t HShowerHistCell::fill(Int_t nRow, Int_t nCol, Int_t nVal)
{
    if( ! m_pHist)
        return kFALSE;

    m_pHist->Fill(nRow, nCol, nVal);

    return kTRUE;
}

//------------------------------------------------------------------------------

Bool_t HShowerHistCell::draw(Int_t nRow, Int_t nCol, Option_t* opt)
{
    if( ! m_pHist)
        return kFALSE;

//   m_pHist->Draw(opt);
    return kTRUE;
}

//------------------------------------------------------------------------------

void HShowerHistCell::deleteHist()
{
    if(m_pHist)
        m_pHist = NULL;

    if(m_pSlopeHist)
        m_pSlopeHist = NULL;

    if(m_pOffsetHist)
        m_pOffsetHist = NULL;

    if(m_pData)
    {
        delete [] m_pData;
        m_pData = NULL;
        m_nDataSize = 0;
    }
}

//------------------------------------------------------------------------------

void HShowerHistCell::getName(Char_t* name, Int_t nSize)
{
Char_t buf[40];

    sprintf(buf, "S%d%d", m_nSector, m_nModule);
    strncpy(name, buf, nSize);
}

//------------------------------------------------------------------------------

void HShowerHistCell::getTitle(Char_t* title, Int_t nSize)
{
Char_t buf[60];
Char_t mods[3][6] = {"pre", "post", "post2"};

    sprintf(buf, "Shower Histogram: Sector %d %s",
                        m_nSector, mods[(Int_t)m_nModule]);
    strncpy(title, buf, nSize);
}

//------------------------------------------------------------------------------

void HShowerHistCell::writeHistogram()
{
//    m_pHist->Write();
    m_pSlopeHist->Write();
    m_pOffsetHist->Write();
}

//------------------------------------------------------------------------------

void HShowerHistCell::fillData(Int_t nRow, Int_t nCol,
                                             Int_t nBins, Int_t *pData)
{
Int_t i;
Int_t bin;
Int_t binx, biny, binz;

    for(i = 0; i < nBins; i++)
    {
        binx = m_pHist->GetXaxis()->FindBin(nRow);
        biny = m_pHist->GetYaxis()->FindBin(nCol);
        binz = m_pHist->GetZaxis()->FindBin(i);
        bin  = m_pHist->GetBin(binx, biny, binz);
        pData[i] = (Int_t)m_pHist->GetBinContent(bin);
    }
}

//------------------------------------------------------------------------------

Int_t HShowerHistCell::getMax(void)
{
Int_t   i    = 0;
Int_t nMax = 0;

    for (i = 0; i < m_nDataSize; i++)
        if (nMax < m_pData[i])
            nMax = m_pData[i];

    return nMax;
}

//------------------------------------------------------------------------------

Float_t HShowerHistCell::getMean(Int_t nMin, Int_t nMax, Int_t nThreshold)
{
Int_t nSum = 0;
Int_t nE   = 0;
Int_t nStart = (nMin < 0)           ? 0           : nMin;
Int_t nStop  = (nMax > m_nDataSize) ? m_nDataSize : nMax;
Int_t   i;

    for(i = nStart; i < nStop; i++)
        if(m_pData[i] >= nThreshold)
        {
            nSum += m_pData[i];
            nE   += i * m_pData[i];
        }

    return (nSum == 0) ? -1.0 : ((float) nE) / nSum;
}

//------------------------------------------------------------------------------

Float_t HShowerHistCell::getSigma(Float_t fMean, Int_t nMin, Int_t nMax,
                                                Int_t nThreshold)
{
Float_t fSum = 0;
Float_t fE2 = 0;
Int_t nStart = (nMin < 0)           ? 0           : nMin;
Int_t nStop  = (nMax > m_nDataSize) ? m_nDataSize : nMax;
Int_t   i;
Float_t fRet;

    for(i = nStart; i < nStop; i++)
    {
        if(m_pData[i] >= nThreshold)
        {
            fE2  += m_pData[i] * (i - fMean) * (i - fMean);
            fSum += m_pData[i];
        }
    }

    if(fSum <= 0.0)
        return 0.0f;

    fRet = (Float_t) TMath::Abs(sqrt(fE2 / fSum));

    return fRet;
}

//------------------------------------------------------------------------------

void HShowerHistCell::makeRunFromCal(HShowerCalParCell* pCell)
{
// For data from the calibration mode cut out left half of the first peak
// and the second peak. The data should be similar to the data from
// the run mode without HV.

Int_t i;
Int_t iMax;

    if((iMax = (int)(-pCell->getOffset())) >= m_nDataSize)
        iMax = m_nDataSize - 1;

    for(i = 0; i <= iMax; i++)
        m_pData[i] = 0;

    if(pCell->getSlope() <= 0.0f)
        return;

        // zero all data starting from the bin in the middle of the peaks
    iMax = (int)(0.5
        * (m_fChannel10pC / pCell->getSlope() - 2 * pCell->getOffset()));
    for(i = iMax; i < m_nDataSize; i++)
        m_pData[i] = 0;
}


//------------------------------------------------------------------------------

void HShowerHistCell::calculate(Int_t iEvents, HShowerCalPar* pCalPar,
                                Int_t iMethod, Float_t fParam1, Float_t fParam2)
{
Int_t                nRow, nCol;
HLocation          loc;
HShowerCalParCell *pCell;
HShowerPadTab     *pPadTable;
HShowerPad        *pPad;

    printf("HShowerHistCell %d %d calculate. Method: %s\n",
            m_nSector, m_nModule, g_cpstr_HSHC_Methods[iMethod]);

    m_iEvents = iEvents;

    if(m_pGeom == NULL)
    {
    HRuntimeDb        *rtdb  = gHades->getRuntimeDb();
    HShowerGeometry   *pGeom = (HShowerGeometry*)rtdb->
                                           getContainer("ShowerGeometry");

        if( ! pGeom)
            Error("calculate", "Unknown container : ShowerGeometry");

        setGeometry(pGeom);
    }

    loc.setNIndex(4);
    loc[0] = m_nSector;
    loc[1] = m_nModule;

    for(nRow = 0; nRow < m_nRows; nRow++)
    {
        loc[2] = nRow;

        for(nCol = 0; nCol < m_nCols; nCol++)
        {
            loc[3] = nCol;

            if((pCell = pCalPar->getObject(loc)) == NULL)
                continue;
#if 0
            printf("A Cell %d %d %2d %2d: ",
                    m_nSector, m_nModule, nRow, nCol);

            pCell->print();
#endif
                // throw away all pads outside detector
            if(((pPadTable = m_pGeom->getPadParam(m_nModule)) == NULL)
                || ((pPad = pPadTable->getPad(nRow, nCol)) == NULL)
                || (pPad->getPadFlag() == 0))
            {
                if(pPadTable == NULL)
                {
                    Error("calculate",
                            "HShowerGeometry->getPadParam(%d) == NULL",
                            m_nModule);
                }
                else
                {
                    if(pPad == NULL)
                    {
                        Error("calculate", "No pad M: %d  R: %d  C: %d",
                                m_nModule, nRow, nCol);
                    }
                }

                pCell->setParams(0.0f, MAX_OFFSET, MAX_OFFSET, 1.0f);

                continue;
            }

            fillData(nRow, nCol, m_nDataSize, m_pData);

            switch(iMethod)
            {
                case HSHC_CAL_TEST:
                    calTest(loc, pCell, fParam1, fParam2);
                    break;

                case HSHC_CAL_ELECT:
                    calElect(loc, pCell, fParam1, fParam2);
                    break;

                case HSHC_RUN_SIGMA:
                    runSigma(loc, pCell, fParam1, fParam2);
                    break;

                case HSHC_RUN_THRESHOLD:
                    runThreshold(loc, pCell, fParam1, fParam2);
                    break;

                case HSHC_CAL_SIGMA:
                    calElect(loc, pCell, fParam1, fParam2);
                    makeRunFromCal(pCell);
                    runSigma(loc, pCell, fParam1, fParam2);
                    break;

                case HSHC_CAL_THRESHOLD:
                    calElect(loc, pCell, fParam1, fParam2);
                    makeRunFromCal(pCell);
                    runThreshold(loc, pCell, fParam1, fParam2);
                    break;

                case HSHC_RUN_INTEGRATE:
                    runIntegrate(loc, pCell, fParam1, fParam2);
                    break;

                case HSHC_CAL_INTEGRATE:
                    calElect(loc, pCell, fParam1, fParam2);
                    makeRunFromCal(pCell);
                    runIntegrate(loc, pCell, fParam1, fParam2);
                    break;

                default:
                    gROOT->Error("HShowerHistCell::calculate",
                            "Unknown method: %d\n", iMethod);
                    return;
            }

#if 0
            printf("A Cell %d %d %2d %2d: ",
                    m_nSector, m_nModule, nRow, nCol);
            pCell->print();
#endif
        }
    }
}

//------------------------------------------------------------------------------

void HShowerHistCell::calTest(HLocation &loc, HShowerCalParCell *pCell,
                                 Float_t fParam1, Float_t fParam2)
{
static Int_t iCount = 0;

    iCount++;

    pCell->setParams(loc[0], loc[1], loc[2], loc[3]);
}

//------------------------------------------------------------------------------

void HShowerHistCell::calElect(HLocation &loc, HShowerCalParCell* pCell,
                                 Float_t fParam1, Float_t fParam2)
{
Float_t fOffset;
Float_t fSlope;
Float_t fThreshold; // threshold for electronics
Float_t fMean, fMean1, fMean2;
Float_t fMeanThreshold = (fParam1 <= 0.0f) ? 0.05 : fParam1;
Float_t fMinePeakDist  = (fParam2 <= 0.0f) ? (m_fChannel10pC / 4) : fParam2;

    fMeanThreshold *= getMax();
    if(((fMean = getMean(0, m_nDataSize, (Int_t)fMeanThreshold)) < 0)
        || ((fMean1 = getMean(0, (Int_t)fMean, (Int_t)fMeanThreshold)) < 0)
        || ((fMean2 = getMean((Int_t)fMean, m_nDataSize, (Int_t)fMeanThreshold)) < 0)
        || ((fMean2 - fMean1) < fMinePeakDist))
    {
        pCell->setParams(0.0f, MAX_OFFSET, MAX_OFFSET, 1.0f);
        return;
    }

    fOffset = -fMean1;

    fSlope = m_fChannel10pC / (fMean2 - fMean1);
    fThreshold = fOffset;
    ROUND_INT(fThreshold);

    pCell->setParams(fSlope, fOffset, fThreshold, 1.0f);

    if((loc[0] == 5) && (loc[1] == 2) && (loc[2] == 29) && (loc[3] == 9))
    {
        printf("**** ");
        pCell->print();
        printf("fMean %f  fMean1 %f  fMean2 %f  fMinePeakDist %f  "
                "m_fChannel10pC %f\n",
                fMean, fMean1, fMean2, fMinePeakDist, m_fChannel10pC);
    }
}

//------------------------------------------------------------------------------

void HShowerHistCell::runSigma(HLocation &loc, HShowerCalParCell* pCell,
                                 Float_t fParam1, Float_t fParam2)
{
Float_t fThreshold; // threshold for electronics
Float_t fMean;
Float_t fSigma;
Float_t fMeanThreshold = (fParam1 <= 0.0f) ? 0.05 : fParam1;
Float_t fSigmaMod      = (fParam2 <= 0.0f) ? 3.0  : fParam2;

    if(pCell->getOffset() == MAX_OFFSET)
    {
        pCell->setThreshold(MAX_OFFSET);
        return;
    }

    fMeanThreshold *= getMax();

    if((fMean = getMean(0, m_nDataSize, (Int_t)fMeanThreshold)) < 0)
        return;

    fSigma = getSigma(fMean, 0, m_nDataSize, (Int_t)fMeanThreshold);

    fThreshold = -(fMean + fSigmaMod * fSigma);

    if(fThreshold > pCell->getOffset())
    {
        gROOT->Warning("ShowerHistCell::runSigma",
                "\n\tThreshold < Offset %f / %f [%d,%d,%d,%d]\n",
                fThreshold, pCell->getOffset(), loc[0], loc[1], loc[2], loc[3]);

        fThreshold = pCell->getOffset();
    }

    ROUND_INT(fThreshold);

    pCell->setThreshold(fThreshold);
}

//------------------------------------------------------------------------------

void HShowerHistCell::runThreshold(HLocation &loc, HShowerCalParCell* pCell,
                                 Float_t fParam1, Float_t fParam2)
{
Float_t fThreshold; // threshold for electronics
Float_t fMean;
Float_t fPeakThreshold = (fParam1 <= 0.0f) ? 0.05 : fParam1;
Int_t   iNeibourhood   = (fParam2 <= 0.0f) ? 5 : (Int_t) fParam2;
Int_t     iCurBin;
Int_t   iMax = getMax();

    if(pCell->getOffset() == MAX_OFFSET)
    {
        pCell->setThreshold(MAX_OFFSET);
        return;
    }

    if(iMax <= 1)
        return;

    if(fParam1 >= 0.0f)
    {
        fPeakThreshold = (fParam1 == 0.0f) ? 0.05 : fParam1;
        fPeakThreshold *= iMax;
    }
    else
    {
        fPeakThreshold = -fParam1;
        if(iMax < fPeakThreshold)
            return;
    }

    if(((fMean = pCell->getOffset()) >= 0.0f)
        && ((fMean = -getMean(0, m_nDataSize, 0)) > 0))
    {
        return;
    }

    ROUND_INT(fMean);
    fMean = -fMean;

    for(iCurBin = (int)(fMean + iNeibourhood);
        (iCurBin > fMean) && (m_pData[iCurBin] <= fPeakThreshold); iCurBin--);

    fThreshold = -iCurBin;
    ROUND_INT(fThreshold);

    if(fThreshold > pCell->getOffset())
    {
        gROOT->Warning("ShowerHistCell::runThreshold",
                "\n\tThreshold < Offset %f / %f [%d,%d,%d,%d]\n",
                fThreshold, pCell->getOffset(), loc[0], loc[1], loc[2], loc[3]);

        fThreshold = pCell->getOffset();
    }

    if(fThreshold < pCell->getThreshold())
        pCell->setThreshold(fThreshold);
}

//------------------------------------------------------------------------------

void HShowerHistCell::runIntegrate(HLocation &loc, HShowerCalParCell* pCell,
                                 Float_t fParam1, Float_t fParam2)
{
Float_t fThreshold; // threshold for electronics
Float_t fMean;
Int_t   iNeibourhood   = (fParam2 <= 0.0f) ? 5 : (Int_t) fParam2;
Int_t     iCurBin;
Int_t   iMax;
Int_t   iSum;

    if(pCell->getOffset() == MAX_OFFSET)
    {
        pCell->setThreshold(MAX_OFFSET);
        return;
    }

    if(fParam1 >= 0.0f)
        iMax = (int)(((fParam1 == 0.0f) ? 0.05 : fParam1) * m_iEvents);
    else
        iMax = (int)(-fParam1);

    if(((fMean = pCell->getOffset()) >= 0.0f)
        && ((fMean = -getMean(0, m_nDataSize, 0)) > 0))
    {
        return;
    }

    ROUND_INT(fMean);
    fMean = -fMean;

    for(iCurBin = (int)(fMean + iNeibourhood), iSum = 0;
        (iCurBin > fMean) && ((iSum += m_pData[iCurBin]) <= iMax); iCurBin--);

    fThreshold = -iCurBin;
    if(iSum > iMax)
        fThreshold -= 1.0f;

    ROUND_INT(fThreshold);

    if(fThreshold > pCell->getOffset())
    {
        gROOT->Warning("ShowerHistCell::runIntegrate",
                "\n\tThreshold < Offset %f / %f [%d,%d,%d,%d]\n",
                fThreshold, pCell->getOffset(), loc[0], loc[1], loc[2], loc[3]);

        fThreshold = pCell->getOffset();
    }

    if(fThreshold < pCell->getThreshold())
        pCell->setThreshold(fThreshold);
}
