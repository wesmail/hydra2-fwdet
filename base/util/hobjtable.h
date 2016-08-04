#ifndef __HObjTable_H
#define __HObjTable_H

#include "TObject.h"
#include "TClonesArray.h"

#include "hlocation.h"

class HObjTable : public TObject {
protected:
  TClonesArray *m_fData; // Container for the data in the Matrix category;
  Int_t m_nNIndexes;
  Int_t *m_pIndexTab; //[m_nNIndexes]
 
public:
  HObjTable(void);
  virtual ~HObjTable(void);

  void setSizes(Int_t nIndexes, Int_t *pSizes);
  void set(Int_t nIndexes, ...);

  void setCellClassName(const Char_t* pszName);
  Char_t* getCellClassName();

  virtual TObject *getObject(Int_t nIndx);
  virtual TObject *getObject(HLocation &loc);
  virtual TObject *&getSlot(HLocation &loc);
  virtual TObject *&getSlot(Int_t nIndx);

  virtual TClonesArray *getClones(HLocation &aLoc);

  Int_t makeObjTable();
  void deleteTab();

private:
  virtual HObjTable *makeSubObjTable(Int_t nIndexes, Int_t *pSizes, TObject *pObj);
  void clearTab();
  TObject *m_pNullObject;

  Char_t m_szClassName[40];

  ClassDef(HObjTable,1)
};
		     

#endif
