#ifndef HShowerHist_H
#define HShowerHist_H
#pragma interface

#include "hparset.h"
#include "hlocation.h"
#include "hobjtable.h"

class HShowerHistCell;
class HShowerCalPar;

//------------------------------------------------------------------------------

class HShowerHist : public HParSet
{
public:
    HShowerHist(const Char_t* name="ShowerHist",
                const Char_t* title="Histograms of offsets and slopes for Shower",
                const Char_t* context="");
   ~HShowerHist();

    void setSetup(Int_t nSectors, Int_t nModules, Int_t nRows, Int_t nCol);

    void setCellClassName(const Char_t* pszName);
    Char_t* getCellClassName();

    HShowerHistCell* getSlot(HLocation &loc);
    HShowerHistCell* getObject(HLocation &loc);

    Bool_t init(HParIo* inp,Int_t* set);
    Int_t  write(HParIo*);
    Bool_t defaultInit();

    Int_t Write(const Text_t* name = 0, Int_t option = 0, Int_t bufsize = 0);

    void bookAll();
    void book(HLocation& loc);
    void book(Int_t nSector, Int_t nModule);
    void resetAll();
    void reset(HLocation& loc);
    void reset(Int_t nSector, Int_t nModule);
    void fill(HLocation& loc, Int_t nVal);
    void fill(Int_t nSector, Int_t nModule, Int_t nRow,
                                                Int_t nCol, Int_t nVal);
    void draw(HLocation& loc, Option_t *opt="");
    void draw(Int_t nSector, Int_t nModule, Int_t nRow, Int_t nCol,
                                                Option_t *opt="");

    void calculate(Int_t iEvents, HShowerCalPar* pCalPar, Int_t iMethod,
                            Float_t fParam1, Float_t fParam2);

private:
    void allocateHist();

    // -------------------------------------------------------------------------

private:
    Int_t     m_nSectors;
    Int_t     m_nModules;
    Int_t     m_nRows;
    Int_t     m_nColumns;

    Float_t   m_fChannel10pC;

    Char_t    m_szClassName[40];

    HObjTable m_ParamsTable;

    ClassDef(HShowerHist, 1) //ROOT extension
};

#endif
