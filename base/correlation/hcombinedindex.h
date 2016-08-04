#ifndef HCOMBINEDINDEX_H
#define HCOMBINEDINDEX_H

#include "TObject.h"
#include "TTreeFormula.h"

class HCombinedIndex : public TObject {
public:
   HCombinedIndex();
   HCombinedIndex(Short_t nSize, TTreeFormula* pMult);
   HCombinedIndex(Short_t nSize, Short_t* pRanges, TTreeFormula* pMult);
   HCombinedIndex(HCombinedIndex&);
   ~HCombinedIndex();

   Short_t getIndexByCode(Short_t code);

   HCombinedIndex& operator=(HCombinedIndex&);
   HCombinedIndex& operator=(int);
   HCombinedIndex& operator+=(HCombinedIndex&);
   HCombinedIndex& operator+=(int);
   HCombinedIndex& operator-=(HCombinedIndex&);
   HCombinedIndex& operator-=(int);
   Bool_t operator==(HCombinedIndex&);
   Bool_t operator==(int);
   Bool_t operator>(HCombinedIndex&);
   Bool_t operator>(int);
   Bool_t operator<(HCombinedIndex&);
   Bool_t operator<(int);
   HCombinedIndex& operator++();
   HCombinedIndex& operator--();
   operator int();

   void setSize(Short_t nSize);
   void setMultFormula(TTreeFormula* pMult){m_pMult = pMult;}
   void setIndex(Int_t ind, Short_t val);
   void setRange(Int_t ind, Short_t val);
   void setCombinedIndex(Int_t val);
   void set(Short_t nSize, Short_t* pRanges, TTreeFormula* pMult);

   Short_t getSize(){return m_nSize;}
   Short_t* getIndexesTable(){return m_nIndexes;}
   Short_t* getRangesTable(){return m_nRanges;}
   TTreeFormula* getMultFormula(){return m_pMult;}
   Int_t getMaxValue(){return m_nMaxValue;}

   Short_t getIndex(Int_t ind);
   Short_t getRange(Int_t ind);
   Int_t getCombinedIndex();
   Short_t IsOverflow(){return m_nIsOverflow;}
private:
   void deleteTables();
   Int_t calcMaxValue();
   void calcIndex(Int_t val, Int_t range, Int_t index);
 
   Short_t m_nSize;
   Short_t *m_nIndexes;
   Short_t *m_nRanges;
   Int_t m_nMaxValue;
   Short_t m_nIsOverflow;
  
   TTreeFormula* m_pMult;

   ClassDef(HCombinedIndex,0)
};

#endif
