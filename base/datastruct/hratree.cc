#include "hratree.h"
#include "hcategory.h"
#include "hlocation.h"
#include "hlocateddataobject.h"
#include "hiterator.h"
#include "hdebug.h"
#include "TIterator.h"
#include "TArrayI.h"
#include "TClass.h"

//*-- Author : Manuel Sanchez
//*-- Modified : 6/10/98 by Manuel Sanchez

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////
//HRaTree, HRaNode, HRaIndexNode
//
// HRaTree stands for "Hades Random Access Tree". This class implements 
//random access to the objects in any category using a tree; the only condition
//is that the data objects in the category inherit from HLocatedDataObject, so
//the random access implemented by the HRaTree corresponds to the indexes 
//in the data objects rather than the natural indexing in the category.
//
// As said, the HRaTree is a tree of HRaNode objects. The lower level nodes
//are HRaIndexNode and store pointers to the actual data objects being accesed
//with the HRaTree.
////////////////////////////////////////

void HRaNode::clear(void) {
  //Calls "Clear" for each of the subnodes
  TIter next(fSubNodes);
  HRaNode *node;
  while ( (node=(HRaNode *)next())!=NULL) {
    node->clear();
  }
}

HRaNode::~HRaNode(void) {
    delete fSubNodes;
}

void HRaIndexNode::clear(void) {
  //Clears the pointers to the data objects being accessed throught the HRaTree
  fCells.Clear();
}

HRaTree::HRaTree(void) {
  //Constructor
 fRoot=NULL;
 fSourceCategory=NULL;
 fDepth=0;
 fLoc.setNIndex(0);
 fLowerLevel=0;
 fIter = NULL;
}

HRaTree::HRaTree(HCategory *cat,TArrayI *sizes) {
  //Constructor for a tree accesing the data in "cat". "sizes" gives the 
  //max size of each level in the tree.
  if (!cat->getClass()->InheritsFrom("HLocatedDataObject")) {
    Warning("HRaTree::HRaTree",
	    "Data class not inheriting from HlocatedDataObject");
    fSourceCategory=NULL;
    fDepth=0;
  } else {
    HLocatedDataObject *obj;
    fSourceCategory=cat;
    obj=(HLocatedDataObject *)cat->getClass()->New();
    fDepth=obj->getNLocationIndex();
    delete obj;
    fIter=(HIterator *)cat->MakeIterator();
  }
  buildTree(sizes);
  fLoc.setNIndex(0);
  fLowerLevel=0;
}

HRaTree::HRaTree(HCategory *cat,HLocation &loc,TArrayI *sizes) {
  //Constructor like the one before but accessing only those objects in the
  //category which correspond to the location "loc"
  if (!cat->getClass()->InheritsFrom("HLocatedDataObject")) {
    Warning("HRaTree::HRaTree",
	    "Data class not inheriting from HlocatedDataObject");
    fSourceCategory=NULL;
    fDepth=0;
  } else {
    HLocatedDataObject *obj;
    fSourceCategory=cat;
    obj=(HLocatedDataObject *)cat->getClass()->New();
    fDepth=obj->getNLocationIndex()-loc.getNIndex();
    delete obj;
    fIter=(HIterator *)cat->MakeIterator();
    fIter->gotoLocation(loc);
  }
  buildTree(sizes);
  fLoc=loc; // Cuidado con este...
  fLowerLevel=loc.getNIndex();
}

HRaNode *HRaTree::buildNode(TArrayI *sizes,Int_t lvl) {
  //Recursive function used to instantiate the tree.
  Int_t i;
  HRaNode *r=new HRaNode(sizes->At(lvl));
  if (sizes->GetSize()-2>lvl) {
    for (i=0;i<sizes->At(lvl);i++) {
      r->addSubNode(buildNode(sizes,lvl+1));
    }
  } else {
    for (i=0;i<sizes->At(lvl);i++) {
      r->addSubNode(new HRaIndexNode(sizes->At(lvl+1)));
    } 
  }
  return r;
}

Bool_t HRaTree::buildTree(TArrayI *sizes) {
  //Instantiates the tree using buildNode()
  Bool_t r=kTRUE;
  if (fDepth==sizes->GetSize()) {
    if (fDepth==1) {
      fRoot=new HRaIndexNode(sizes->At(0));
    } else {
      fRoot=buildNode(sizes,0);
    }
  } else {
    r=kFALSE;
    Error("buildTree","Sizes vector dimension and tree depth don't match");
  }
  return r;
}

HRaTree::~HRaTree(void) {
  //Destructor
  if (fRoot) delete fRoot;
  fSourceCategory=NULL;

  if(fIter) {
      delete fIter;
      fIter = NULL;
  }

}

Bool_t HRaTree::addObject(HLocatedDataObject *obj) {
  //Adds the pointer "obj" to the right place in the tree so it can be accesed
  //after.
  Int_t level=fLowerLevel;
  HRaNode *node=fRoot;
  HRaIndexNode *inode=NULL;
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HRaTree::addObject");
#endif

  if (fDepth==1) {
    inode=(HRaIndexNode *)fRoot;
  } else {
    for (level=fLowerLevel;level<fLowerLevel+fDepth-2;level++) { 
      //counts fDepth-2 times
      node=node->getSubNode(obj->getLocationIndex(level));
      if (node==NULL) return kFALSE;
    }

    inode=(HRaIndexNode *)node->getSubNode(obj->getLocationIndex(level));
    if (!node) return kFALSE;
  }
  level++;
#if DEBUG_LEVEL>2
  gDebuger->leaveFunc("HRaTree::addObject");
#endif
  inode->setCell(obj,obj->getLocationIndex(level));
  return kTRUE;
}

Bool_t HRaTree::update(void) {
  //Updates the contents of the tree with the data in the accessed category.
  HLocatedDataObject *obj;

  if (fDepth==0 || fRoot==NULL || fSourceCategory==NULL || fIter==NULL) {
    Error("update","Tree not properly constructed");
    return kFALSE;
  } else {
    fRoot->clear();
    fIter->gotoLocation(fLoc);
    while ( (obj=(HLocatedDataObject *)fIter->Next())!=NULL) {
      if (!addObject(obj)) {
	return kFALSE;
      }
    }
  }
  return kTRUE;
}

TObject *HRaTree::getObject(HLocation &aLoc) {
  //Returns the object corresponding to the location "loc"
  //Warning: for the sake of speed no index checking is done here.
  HRaNode *node=fRoot;
  Int_t level=0;

  //  if (!fRoot || aLoc.getNIndex()<fDepth) return NULL;
  for (level=0;level<fDepth-1;level++) {
    node=node->getSubNode(aLoc.getUncheckedIndex(level));
    if (!node) return NULL;
  }
  return ((HRaIndexNode *)node)->getCell(aLoc.getUncheckedIndex(level));
}

TObject *HRaTree::getObject(Int_t i1,Int_t i2,Int_t i3,Int_t i4,
					  Int_t i5,Int_t i6,Int_t i7,Int_t i8,
					  Int_t i9) {
  //Returns the object corresponding to the location with indexes:
  //"i1","i2" up to "i9"
  HRaNode *node=NULL;
  TObject *r=NULL;

  if (i1>-1 && fDepth>0) {
    node=fRoot;
    if(i2>-1 && fDepth>1) {
      node=node->getSubNode(i1);
      if (i3>-1&& fDepth>2) {
	node=node->getSubNode(i2);
	if (i4>-1&& fDepth>3) {
	  node=node->getSubNode(i3);
	  if (i5>-1&& fDepth>4) {
	    node=node->getSubNode(i4);
	    if (i6>-1&& fDepth>5) {
	      node=node->getSubNode(i5);
	      if (i7>-1&& fDepth>6) {
		node=node->getSubNode(i6);
		if (i8>-1&& fDepth>7) {
		  node=node->getSubNode(i7);
		  if (i9>-1&& fDepth>8) {
		    node=node->getSubNode(i8);
		    r=((HRaIndexNode *)node)->getCell(i9);
		  } else r=((HRaIndexNode *)node)->getCell(i8);
		} else r=((HRaIndexNode *)node)->getCell(i7);
	      } else r=((HRaIndexNode *)node)->getCell(i6);
	    } else r=((HRaIndexNode *)node)->getCell(i5);
	  } else r=((HRaIndexNode *)node)->getCell(i4);
	} else r=((HRaIndexNode *)node)->getCell(i3);
      } else r=((HRaIndexNode *)node)->getCell(i2);
    } else r=((HRaIndexNode *)node)->getCell(i1);
  } else r=NULL;
  return (r);
}

ClassImp(HRaNode)
ClassImp(HRaIndexNode)
ClassImp(HRaTree)
