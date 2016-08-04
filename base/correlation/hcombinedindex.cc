#include "hcombinedindex.h"
#include "hmulttreeformula.h"

ClassImp(HCombinedIndex);

HCombinedIndex::HCombinedIndex() {
  m_nSize = 0;
  m_nIndexes = NULL;
  m_nRanges = NULL;
  m_nIsOverflow = 0;
  m_pMult = NULL; 
}

HCombinedIndex::HCombinedIndex(Short_t nSize, TTreeFormula* pMult) {
  m_pMult = pMult;
  m_nIndexes = NULL;
  m_nRanges = NULL;
  m_nIsOverflow = 0;

  setSize(nSize);
}

HCombinedIndex::HCombinedIndex(Short_t nSize, Short_t *pRanges, 
                                               TTreeFormula* pMult) {

  Int_t i;

  m_pMult = pMult;
  m_nIndexes = NULL;
  m_nRanges = NULL;
  m_nIsOverflow = 0;

  setSize(nSize);
  for(i = 0; i < nSize; i++)
      m_nRanges[i] = pRanges[i];
  calcMaxValue();
}



HCombinedIndex::HCombinedIndex(HCombinedIndex& ind) {
  Short_t* pInd;
  Short_t* pRange;
  Int_t i;

  m_nSize = ind.getSize();
  m_pMult = ind.getMultFormula();
  m_nIndexes = new Short_t[m_nSize];
  m_nRanges = new Short_t[m_nSize];
  m_nIsOverflow = ind.IsOverflow();

  pInd = ind.getIndexesTable();
  pRange = ind.getRangesTable();
  for(i = 0; i < m_nSize; i++) {
     m_nIndexes[i] = pInd[i];
     m_nRanges[i] = pRange[i];
  }

  calcMaxValue();
}

HCombinedIndex::~HCombinedIndex() {
  deleteTables();
}

void HCombinedIndex::set(Short_t nSize, Short_t *pRanges,
                                               TTreeFormula* pMult) {
  Int_t i;

  m_pMult = pMult;
  m_nIndexes = NULL;
  m_nRanges = NULL;
  m_nIsOverflow = 0;

  setSize(nSize);
  for(i = 0; i < nSize; i++)
      m_nRanges[i] = pRanges[i];
  calcMaxValue();

}


Short_t HCombinedIndex::getIndexByCode(Short_t code) {
  Int_t i;
  Short_t *pBranchCodes;

  if (!m_pMult) return -1;

  pBranchCodes = ((HMultTreeFormula*)m_pMult)->GetBranchCodes();
  for(i = 0; i < m_nSize; i++)
     if (pBranchCodes[i] == code) return m_nIndexes[i];

  return -1;
}


HCombinedIndex& HCombinedIndex::operator=(HCombinedIndex& ind) {
  Short_t* pInd;
  Short_t* pRange;
  Int_t i;

  m_nSize = ind.getSize();
  m_pMult = ind.getMultFormula();
  m_nIndexes = new Short_t[m_nSize];
  m_nRanges = new Short_t[m_nSize];
  m_nIsOverflow = ind.IsOverflow();

  pInd = ind.getIndexesTable();
  pRange = ind.getRangesTable();
  for(i = 0; i < m_nSize; i++) {
     m_nIndexes[i] = pInd[i];
     m_nRanges[i] = pRange[i];
  }

  calcMaxValue();

  return (*this);
}

HCombinedIndex& HCombinedIndex::operator=(Int_t val) {
  setCombinedIndex(val);

  return *this;
}

HCombinedIndex& HCombinedIndex::operator+=(HCombinedIndex& ind) {
  Int_t val1, val2;

  val1 = getCombinedIndex();
  val2 = (int)ind; 
  setCombinedIndex(val1 + val2);

  return (*this);
}

HCombinedIndex& HCombinedIndex::operator+=(Int_t val2) {
  Int_t val1;
  
  val1 = getCombinedIndex();
  setCombinedIndex(val1 + val2);

  return (*this);
}

HCombinedIndex& HCombinedIndex::operator-=(HCombinedIndex& ind) {
  Int_t val1, val2;

  val1 = getCombinedIndex();
  val2 = (int)ind;

  m_nIsOverflow = 0;
  if (val1 < val2) {
     val1 += getMaxValue();
     m_nIsOverflow = 1;
  }
  
  setCombinedIndex(val1 - val2);

  return (*this);
}

HCombinedIndex& HCombinedIndex::operator-=(Int_t val2) {
  Int_t val1;

  val1 = getCombinedIndex();

  m_nIsOverflow = 0;
  if (val1 < val2) {
     val1 += getMaxValue();
     m_nIsOverflow = 1;
  }

  setCombinedIndex(val1 - val2);

  return (*this);
} 

Bool_t HCombinedIndex::operator==(HCombinedIndex& ind) {
  Int_t val1, val2;

  val1 = getCombinedIndex();
  val2 = (int)ind;

  return val1==val2;
}

Bool_t HCombinedIndex::operator==(Int_t val2) {
  Int_t val1;
  
  val1 = getCombinedIndex();
  
  return val1==val2;
} 

Bool_t HCombinedIndex::operator<(HCombinedIndex& ind) {
  Int_t val1, val2;
  
  val1 = getCombinedIndex();
  val2 = (int)ind;
  
  return val1<val2;
} 

Bool_t HCombinedIndex::operator<(Int_t val2) {
  Int_t val1;
  
  val1 = getCombinedIndex();
  
  return val1<val2;
} 

Bool_t HCombinedIndex::operator>(HCombinedIndex& ind) {
  Int_t val1, val2;
  
  val1 = getCombinedIndex();
  val2 = (int)ind;
  
  return val1>val2;
} 

Bool_t HCombinedIndex::operator>(Int_t val2) {
  Int_t val1;
  
  val1 = getCombinedIndex();
  
  return val1>val2;
} 

HCombinedIndex& HCombinedIndex::operator++() {
  Int_t i;

  for(i = m_nSize; i--;) {
    m_nIndexes[i]++;
    if (m_nIndexes[i] < m_nRanges[i])  return (*this);

    m_nIndexes[i] = 0;
  }

  m_nIsOverflow = 1;
  return  (*this); 
}

HCombinedIndex& HCombinedIndex::operator--() {
  Int_t i;

  for(i = m_nSize; i--;) {
    m_nIndexes[i]--;
    if (m_nIndexes[i] >= 0)  return (*this);

    m_nIndexes[i] = m_nRanges[i] - 1;;
  }

  m_nIsOverflow = 1;
  return  (*this);
}


HCombinedIndex::operator int() {
  return getCombinedIndex();
}


void  HCombinedIndex::setSize(Short_t nSize) {
  Int_t i;

  deleteTables();

  m_nSize = nSize;

  m_nIndexes = new Short_t[nSize];
  m_nRanges = new Short_t[nSize];
  for(i = 0; i < m_nSize; i++) {
     m_nIndexes[i] = 0;
     m_nRanges[i] = 0; 
  }

  calcMaxValue();
}

void HCombinedIndex::setIndex(Int_t ind, Short_t val) {
  if ((ind < 0) || (ind >= m_nSize)) return;
  if ((val < 0) || (val >= m_nRanges[ind])) return;

  m_nIndexes[ind] = val;
}

void HCombinedIndex::setRange(Int_t ind, Short_t val) {
  if ((ind < 0) && (ind >= m_nSize)) return;

  m_nRanges[ind] = val;
  calcMaxValue();
}

void HCombinedIndex::setCombinedIndex(Int_t val) {
   Int_t i;
  
   if (!getMaxValue()) return;
 
   for(i = 0; i < m_nSize; i++)
      if (m_nRanges[i] == 0) return;

   m_nIsOverflow = 0;
   if (val >= getMaxValue()) {
     m_nIsOverflow = 1;
     val %= getMaxValue();
   }

   calcIndex(val, getMaxValue(), 0);    
}

Short_t HCombinedIndex::getIndex(Int_t ind) {
  if ((ind < 0) && (ind >= m_nSize)) return -1;

  return m_nIndexes[ind];
}

Short_t HCombinedIndex::getRange(Int_t ind) {
  if ((ind < 0) && (ind >= m_nSize)) return -1;

  return m_nRanges[ind];
}

Int_t HCombinedIndex::getCombinedIndex() {
  Int_t i, val;

  val = m_nIndexes[0];
  for(i = 1; i < m_nSize; i++) {
     val *= m_nRanges[i - 1]; 
     val += m_nIndexes[i];
  }
  
  return val;    
}


void HCombinedIndex::deleteTables() {
  if (m_nIndexes) delete[] m_nIndexes;
  if (m_nRanges) delete[] m_nRanges;
  
  m_nSize = 0;
  m_nIndexes = NULL;
  m_nRanges = NULL;
}

Int_t HCombinedIndex::calcMaxValue() {
  Int_t val = 1;
  Int_t i;

  for(i = 0; i < m_nSize; i++)
      val *= m_nRanges[i];

  m_nMaxValue = val;
  return val;
}

void HCombinedIndex::calcIndex(Int_t val, Int_t range, Int_t index) {
  if (index == m_nSize) return;

  m_nIndexes[index] = val/range;
  val %= range;
  range /= m_nRanges[index];
  calcIndex(val, range, ++index);
}
