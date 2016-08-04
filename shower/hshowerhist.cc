//#
//#
//#                      FRAMEWORK
//#
//#       Authors:                           W.Koenig
//#       adoption to framework & ROOT       W.Schoen
//#       new framework for different metods M.Jaskula
//#
//#       last mod. M.Jaskula Mon Aug 27 19:20:32 CEST 2001
// ###################################################
using namespace std;
#include "TH1.h"
#include "TH3.h"

#pragma implementation
#include "TROOT.h"
#include <iostream> 
#include <iomanip>
#include "hades.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hlocation.h"
#include "hspectrometer.h"
#include "hshowercalpar.h"
#include "hshowerdetector.h"
#include "hshowerhist.h"
#include "hshowerhistcell.h"

//------------------------------------------------------------------------------

ClassImp(HShowerHist)

//------------------------------------------------------------------------------

////////// **********ShowerHist defintion ****************/////////////
HShowerHist::HShowerHist(const Char_t* name,const Char_t* title,
                                       const Char_t* context)
                   : HParSet(name,title,context) {
    strcpy(detName,"Shower");
    setSetup(6, 3, 32, 32); //default setup
    setCellClassName("HShowerHistCell");
}

//------------------------------------------------------------------------------

HShowerHist::~HShowerHist()
{
    m_ParamsTable.deleteTab();
}

//------------------------------------------------------------------------------

void HShowerHist::setSetup(Int_t nSectors, Int_t nModules,
                           Int_t  nRows, Int_t nColumns)
{
    m_nSectors = nSectors;
    m_nModules = nModules;
    m_nRows = nRows;
    m_nColumns = nColumns;
}

//------------------------------------------------------------------------------

void HShowerHist::setCellClassName(const Char_t* pszName)
{
    strncpy(m_szClassName, pszName, sizeof(m_szClassName));
}

//------------------------------------------------------------------------------

Char_t* HShowerHist::getCellClassName()
{
    return m_szClassName;
}

//------------------------------------------------------------------------------

HShowerHistCell* HShowerHist::getSlot(HLocation &loc)
{
HLocation l;

    l.setNIndex(2);
    l[0] = loc[0];
    l[1] = loc[1];

    return (HShowerHistCell*) m_ParamsTable.getSlot(loc);
}

//------------------------------------------------------------------------------

HShowerHistCell* HShowerHist::getObject(HLocation &loc)
{
HLocation l;

    l.setNIndex(2);
    l[0] = loc[0];
    l[1] = loc[1];

    return (HShowerHistCell*) m_ParamsTable.getObject(l);
}

//------------------------------------------------------------------------------

Bool_t HShowerHist::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HShowerParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

//------------------------------------------------------------------------------

Int_t HShowerHist::write(HParIo* output) {
  // writes the shower container to the output
  HDetParIo* out=output->getDetParIo("HShowerParIo");
  if (out) return out->write(this);
  return kFALSE;
}

//------------------------------------------------------------------------------

Bool_t HShowerHist::defaultInit()
{
Int_t              ret = kFALSE;
HShowerDetector *pShowerDet;

    if(gHades)
    {
        pShowerDet = (HShowerDetector*)gHades->getSetup()
                                              ->getDetector("Shower");

        if(pShowerDet)
        {
            m_nSectors = pShowerDet->getShowerSectors();
            m_nModules = pShowerDet->getShowerModules();
            m_nRows    = pShowerDet->getRows();
            m_nColumns = pShowerDet->getColumns();
            ret        = kTRUE;
        }
    }

    setSetup(m_nSectors, m_nModules, m_nRows, m_nColumns);

    printf("%d - %d - %d - %d\n", m_nSectors, m_nModules, m_nRows, m_nColumns);
    allocateHist();

    return ret;
}

//------------------------------------------------------------------------------

void HShowerHist::allocateHist()
{
Int_t              sect, mod;
HShowerHistCell *pHistCell;
HLocation        loc;

    loc.setNIndex(2);

    m_ParamsTable.set(2, m_nSectors, m_nModules);

    m_ParamsTable.setCellClassName(getCellClassName());
    m_ParamsTable.makeObjTable();

    for(sect = 0; sect < m_nSectors; sect++)
        for(mod = 0; mod < m_nModules; mod++)
        {
            loc[0] = sect;
            loc[1] = mod;
            pHistCell = (HShowerHistCell*) getSlot(loc);
            if (pHistCell)
            {
                pHistCell = new(pHistCell) HShowerHistCell(sect, mod,
                                                       m_nRows, m_nColumns);
                pHistCell->book();
            }
        }

    setStatic();
}

//------------------------------------------------------------------------------

void HShowerHist::bookAll()
{
HLocation loc;

    loc.setNIndex(2);
    for(loc[0] = 0; loc[0] < m_nSectors; loc[0]++)
        for(loc[1] = 0; loc[1] < m_nModules; loc[1]++)
            book(loc);
}

//------------------------------------------------------------------------------

void HShowerHist::book(HLocation& loc)
{
HShowerHistCell *pHistCell;
HLocation        l;

    l.setNIndex(2);
    l[0] = loc[0];
    l[1] = loc[1];

    pHistCell = getObject(l);
    if (pHistCell)
        pHistCell->book();
}

//------------------------------------------------------------------------------

void HShowerHist::book(Int_t nSector, Int_t nModule)
{
HLocation loc;

    loc.setNIndex(2);
    loc[0] = nSector;
    loc[1] = nModule;

    book(loc);
}

//------------------------------------------------------------------------------

void HShowerHist::resetAll()
{
HLocation loc;

    loc.setNIndex(2);
    for(loc[0] = 0; loc[0] < m_nSectors; loc[0]++)
        for(loc[1] = 0; loc[1] < m_nModules; loc[1]++)
            reset(loc);
}

//------------------------------------------------------------------------------

void HShowerHist::reset(HLocation& loc)
{
HShowerHistCell *pHistCell;
HLocation        l;

    l.setNIndex(2);
    l[0] = loc[0];
    l[1] = loc[1];

    pHistCell = getObject(l);
    if (pHistCell)
        pHistCell->reset();
}

//------------------------------------------------------------------------------

void HShowerHist::reset(Int_t nSector, Int_t nModule)
{
HLocation loc;

    loc.setNIndex(2);
    loc[0] = nSector;
    loc[1] = nModule;

    reset(loc);
}

//------------------------------------------------------------------------------

void HShowerHist::fill(HLocation& loc, Int_t nVal)
{
HShowerHistCell *pHistCell;
Int_t            nRow = loc[2];
Int_t            nCol = loc[3];

    pHistCell = getObject(loc);
    if (pHistCell)
        pHistCell->fill(nRow, nCol, nVal);
}

//------------------------------------------------------------------------------

void HShowerHist::fill(Int_t nSector, Int_t nModule,
                                 Int_t nRow, Int_t nCol, Int_t nVal)
{
HLocation loc;

    loc.setNIndex(4);
    loc[0] = nSector;
    loc[1] = nModule;
    loc[2] = nRow;
    loc[3] = nCol;

    fill(loc, nVal);
}

//------------------------------------------------------------------------------

void HShowerHist::draw(HLocation& loc, Option_t *opt)
{
HShowerHistCell *pHistCell;
Int_t            nRow = loc[2];
Int_t            nCol = loc[3];

    pHistCell = getObject(loc);
    if (pHistCell)
        pHistCell->draw(nRow, nCol, opt);
}

//------------------------------------------------------------------------------

void HShowerHist::draw(Int_t nSector, Int_t nModule,
                                    Int_t nRow, Int_t nCol, Option_t* opt )
{
HLocation loc;

    loc.setNIndex(4);
    loc[0] = nSector;
    loc[1] = nModule;
    loc[2] = nRow;
    loc[3] = nCol;

    draw(loc, opt);
}

//------------------------------------------------------------------------------

Int_t HShowerHist::Write(const Text_t* name, Int_t option, Int_t bufsize)
{
    HParSet::Write(name, option, bufsize);

HLocation loc;

    loc.setNIndex(2);
    for(loc[0] = 0; loc[0] < m_nSectors; loc[0]++)
        for(loc[1] = 0; loc[1] < m_nModules; loc[1]++)
            ((HShowerHistCell*)getObject(loc))->writeHistogram();

    return 0;
}

//------------------------------------------------------------------------------

void HShowerHist::calculate(Int_t iEvents, HShowerCalPar* pCalPar,
                            Int_t iMethod, Float_t fParam1, Float_t fParam2)
{
HLocation        loc;
HShowerHistCell *pHistCell;

    printf("\ncalculating calibration parameters ...\n");

    loc.setNIndex(4);

    for(loc[0] = 0; loc[0] < m_nSectors; loc[0]++)
    {
        for(loc[1] = 0; loc[1] < m_nModules; loc[1]++)
        {
            if((pHistCell = getObject(loc)) == NULL)
                continue;

            pHistCell->calculate(iEvents, pCalPar, iMethod, fParam1, fParam2);
        }
    }
}
