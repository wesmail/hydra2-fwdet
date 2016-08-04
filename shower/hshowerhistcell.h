#ifndef HShowerHistCell_H
#define HShowerHistCell_H
#pragma interface

#include "hparset.h"
#include "hlocation.h"

class TH3S;
class TH2F;
class HShowerCalPar;
class HShowerCalParCell;
class HShowerGeometry;

enum enum_HSHC_Methods
{
    HSHC_CAL_TEST = 0,
    HSHC_CAL_ELECT,
    HSHC_RUN_SIGMA,
    HSHC_RUN_THRESHOLD,
    HSHC_CAL_SIGMA,
    HSHC_CAL_THRESHOLD,

    HSHC_RUN_INTEGRATE,
    HSHC_CAL_INTEGRATE,

    HSHC_MAX
};

extern const Char_t* g_cpstr_HSHC_Methods[];

//------------------------------------------------------------------------------

#define MAX_OFFSET          (-255.0f)
#define DEF_CHANNEL_10pC    (500.0f)

//------------------------------------------------------------------------------

class HShowerHistCell : public TObject
{
public:
    HShowerHistCell();
    HShowerHistCell(Int_t nSect, Int_t nMod, Int_t nRow, Int_t nCol);
   ~HShowerHistCell();

    Bool_t  reset();
    Bool_t  fill(Int_t nRow, Int_t nCol, Int_t nVal);
    Bool_t  draw(Int_t nRow, Int_t nCol, Option_t* opt="");
    Bool_t  book();
    Bool_t  book(Int_t nRows, Int_t nCols);
    void    deleteHist();

    void    calculate(Int_t iEvents, HShowerCalPar* pCalPar, Int_t iMethod,
                                Float_t fParam1, Float_t fParam2);

    TH3S*   getHistogram()     { return m_pHist;   }
    void    writeHistogram();

    Char_t  getSector(void)   { return m_nSector; }
    Char_t  getModule(void)   { return m_nModule; }
    Char_t  getRows(void)     { return m_nRows;   }
    Char_t  getCols(void)     { return m_nCols;   }

    void    setSector(Char_t s) { m_nSector = s;    }
    void    setModule(Char_t m) { m_nModule = m;    }
    void    setRows(Char_t r)   { m_nRows   = r;    }
    void    setCols(Char_t c)   { m_nCols   = c;    }


    void    setChannel10pC(Float_t f)   { m_fChannel10pC = f;       }
    Float_t getChannel10pC(void)        { return m_fChannel10pC;    }

    void    setGeometry(HShowerGeometry* pGeom) { m_pGeom = pGeom;}
    HShowerGeometry* getGeometry()              { return m_pGeom;}

private:
    void    getName(Char_t* name, Int_t nInt);
    void    getTitle(Char_t* title, Int_t nInt);

    void    fillData(Int_t nRow, Int_t nCol, Int_t nBins, Int_t * pData);
    Int_t   getMax(void);
    Float_t getMean(Int_t nMin, Int_t nMax, Int_t nThreshold);
    Float_t getSigma(Float_t fMean, Int_t nMin, Int_t nMax, Int_t nThreshold);

    void  makeRunFromCal(HShowerCalParCell* pCell);

    // -------------------------------------------------------------------------

    TH3S   *m_pHist;  //!
    TH2F   *m_pSlopeHist;  //!
    TH2F   *m_pOffsetHist;  //!

    Char_t  m_nSector;
    Char_t  m_nModule;

    Char_t  m_nRows;
    Char_t  m_nCols;

    Int_t   m_nDataSize; //!
    Int_t*  m_pData; //!

    Int_t   m_iEvents; //!

    Float_t m_fChannel10pC;
    HShowerGeometry *m_pGeom; //!Pointer to geometry parameters container

    // -------------------------------------------------------------------------

    ClassDef(HShowerHistCell, 2) //ROOT extension

    void calTest(HLocation &loc, HShowerCalParCell *pCell,
                                 Float_t fParam1, Float_t fParam2);
    void calElect(HLocation &loc,HShowerCalParCell* pCell,
                                 Float_t fParam1, Float_t fParam2);
    void runSigma(HLocation &loc,HShowerCalParCell* pCell,
                                 Float_t fParam1, Float_t fParam2);
    void runThreshold(HLocation &loc,HShowerCalParCell* pCell,
                                 Float_t fParam1, Float_t fParam2);
    void runIntegrate(HLocation &loc,HShowerCalParCell* pCell,
                                 Float_t fParam1, Float_t fParam2);
};

#endif
