#ifndef HRATREE_H
#define HRATREE_H

#include "TObject.h"
#include "TObjArray.h"
#include "hlocation.h"

class HCategory;
class HLocatedDataObject;
class HIterator;
class TArrayI;

class HRaNode : public TObject {
protected:
  TObjArray *fSubNodes;
public:
  HRaNode(void) { fSubNodes = 0; }
  HRaNode(Int_t size) {fSubNodes=new TObjArray(size);}
  ~HRaNode(void); 
  HRaNode *getSubNode(Int_t idx) {return (HRaNode *)
				    fSubNodes->UncheckedAt(idx);} 
  void addSubNode(HRaNode *n) {fSubNodes->Add(n);}
  virtual void clear(void);
  ClassDef(HRaNode,0) //node for a HRaTree
};

class HRaIndexNode : public HRaNode {
protected:
  TObjArray fCells;
  HRaIndexNode(void) : fCells(100) {}
public:
  HRaIndexNode(Int_t size) : fCells(size) {}
  ~HRaIndexNode(void) {}
  void clear(void);
  TObject *getCell(Int_t i) {return fCells.UncheckedAt(i);}
  void setCell(TObject *obj,Int_t i) {fCells.AddAt(obj,i);}
  ClassDef(HRaIndexNode,0) //Object table for a HRaTree
};

class HRaTree : public TObject {
 protected:
  HRaNode *fRoot; //index tree's root node
  HCategory *fSourceCategory; //! Category actually holding data
  Int_t fDepth; //Tree's depth
  HLocation fLoc; //Location for this tree.
  HIterator *fIter;//! Iterator on data objects in the category
  Int_t fLowerLevel;//

  Bool_t addObject(HLocatedDataObject *obj);
  Bool_t buildTree(TArrayI *sizes);
  HRaNode *buildNode(TArrayI *sizes,Int_t lvl);
 public:
  HRaTree(void);
  HRaTree(HCategory *cat,TArrayI *sizes);
  HRaTree(HCategory *cat,HLocation &aLoc,TArrayI *sizes);
  ~HRaTree(void);
  Bool_t update(void);
  HRaNode *getRoot(void) {return(fRoot);}
  virtual TObject *getObject(HLocation &aLoc);
  virtual TObject *getObject(Int_t i1=-1,Int_t i2=-1,Int_t i3=-1,Int_t i4=-1,
		     Int_t i5=-1,Int_t i6=-1,Int_t i7=-1,Int_t i8=-1,
		     Int_t i9=-1);
  ClassDef(HRaTree,1) //Random acces in a tree fashion.
};

#endif /* !HRATREE_H */
