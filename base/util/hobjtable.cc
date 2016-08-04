using namespace std;
#include "TROOT.h"
#include "TClass.h"
#include <fstream> 
#include "hobjtable.h"

ClassImp(HObjTable)

HObjTable::HObjTable()
{
  m_nNIndexes = 0;
  m_pIndexTab = NULL;
  m_fData = NULL;
  m_pNullObject = NULL;
}

HObjTable::~HObjTable() {
    if (m_pNullObject) delete m_pNullObject;
    clearTab();
}

void HObjTable::setSizes(Int_t nNIndexes, Int_t *pSizes)
{
  clearTab();
  m_nNIndexes = nNIndexes;
  m_pIndexTab = new Int_t[m_nNIndexes];

  for(Int_t i = 0; i < nNIndexes; i++)
     m_pIndexTab[i] = pSizes[i];
}

void HObjTable::set(Int_t nNIndexes,...)
{
  Int_t *pTab = new Int_t[nNIndexes];

  va_list ap;
  va_start (ap, nNIndexes);

  for(Int_t i = 0; i < nNIndexes; i++)
      pTab[i]=va_arg(ap,Int_t);

  setSizes(nNIndexes, pTab);

  va_end(ap);

  delete [] pTab;
}

void HObjTable::setCellClassName(const Char_t* pszName)
{
  strncpy(m_szClassName, pszName, sizeof(m_szClassName));
  if (m_pNullObject) delete m_pNullObject;
  m_pNullObject = (TObject*)gROOT->GetClass(m_szClassName)->New();
}

Char_t* HObjTable::getCellClassName()
{
  return m_szClassName;
}

void HObjTable::clearTab()
{
  if (m_pIndexTab)
  {
    delete [] m_pIndexTab;
    m_pIndexTab = NULL;
    m_nNIndexes = 0;
  }

  if (m_fData)
  {
    deleteTab();
    delete m_fData;
    m_fData = NULL;
  }
}  

void HObjTable::deleteTab()
{
  if (m_nNIndexes > 1)
    for(Int_t i = 0; i < m_pIndexTab[0]; i++)
    {
      HObjTable* objtab = (HObjTable*)getObject(i);
      if (objtab) objtab->deleteTab();       
    }
  
  if(m_nNIndexes)
    m_fData->Delete();
}

Int_t HObjTable::makeObjTable()
{
  Int_t i;
  HLocation loc;

  if (!m_pIndexTab)
    return 0;

  loc.set(1, m_pIndexTab[0]);

  if(m_nNIndexes == 1)
      m_fData = new TClonesArray(m_szClassName, m_pIndexTab[0]);
  else {
      m_fData = new TClonesArray("HObjTable", m_pIndexTab[0]);
      for(i = 0; i < m_pIndexTab[0]; i++)
      {
        
        loc[0] = i;
        HObjTable *pObjTabSlot = (HObjTable*)getSlot(loc);
        makeSubObjTable(m_nNIndexes - 1, m_pIndexTab + 1, pObjTabSlot);
      }
  }

  return m_fData ? 1 : 0;
}
 
HObjTable* HObjTable::makeSubObjTable(Int_t nNIndexes, Int_t *pSizes, TObject* pObj)
{
  HObjTable *fObjTab;

  fObjTab = new(pObj) HObjTable;
  fObjTab->setSizes(nNIndexes, pSizes);
  fObjTab->setCellClassName(m_szClassName);
  fObjTab->makeObjTable();

  return fObjTab;
}  
     

TObject *&HObjTable::getSlot(Int_t nIndx)
{
  return (m_fData->operator[](nIndx));
}

TObject *&HObjTable::getSlot(HLocation &aLoc) {
 HLocation aLoc1;

  if (aLoc.getNIndex() == 1)
     return getSlot(aLoc[0]); 

 aLoc1.setNIndex(aLoc.getNIndex() - 1);
 for (Int_t i = 0; i < aLoc1.getNIndex(); i++)
   aLoc1[i] = aLoc[i + 1];

 HObjTable* pTab = (HObjTable*)getObject(aLoc[0]);

 return pTab->getSlot(aLoc1);
}

TObject* HObjTable::getObject(Int_t nIndx)
{
/*
  if (nIndx < m_nNIndexes) {
     cerr << "ERROR: Table SIZE = " << m_nNIndexes << " Index = " << nIndx << endl;
     return m_pNullObject;
  }
*/
//  TObject* pObj = (TObject *)m_fData->At(nIndx);
  TObject* pObj = (TObject *)(*m_fData)[nIndx];
  return (pObj) ? pObj : m_pNullObject;
}

TObject* HObjTable::getObject(HLocation &aLoc) {
 HLocation aLoc1;

 if (aLoc.getNIndex() == 1)
   return getObject(aLoc[0]); 

 aLoc1.setNIndex(aLoc.getNIndex() - 1);
 for (Int_t i = 0; i < aLoc1.getNIndex(); i++)
   aLoc1[i] = aLoc[i + 1];

 HObjTable* pTab = (HObjTable*)getObject(aLoc[0]);

 return (TObject *)pTab->getObject(aLoc1);
}

TClonesArray *HObjTable::getClones(HLocation &aLoc) {
 return m_fData;
}

//______________________________________________________________________________
void HObjTable::Streamer(TBuffer &R__b)
{
   // Stream an object of class HObjTable.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      clearTab();
      TObject::Streamer(R__b);
      R__b >> m_nNIndexes;
      m_pIndexTab = new Int_t[m_nNIndexes];
      R__b.ReadFastArray(m_pIndexTab, m_nNIndexes);
      R__b >> m_pNullObject;
      R__b.ReadStaticArray(m_szClassName);
      if(m_nNIndexes == 1)
          m_fData = new TClonesArray(m_szClassName, m_pIndexTab[0]);
      else 
          m_fData = new TClonesArray("HObjTable", m_pIndexTab[0]);
      m_fData->Streamer(R__b);
   } else {
      R__b.WriteVersion(HObjTable::IsA());
      TObject::Streamer(R__b);
      R__b << m_nNIndexes;
      R__b.WriteFastArray(m_pIndexTab, m_nNIndexes);
      R__b << m_pNullObject;
      R__b.WriteArray(m_szClassName, 40);
      m_fData->Streamer(R__b);
   }
}

