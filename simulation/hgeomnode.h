#ifndef HGEOMNODE_H
#define HGEOMNODE_H

using namespace std;
#include <fstream> 
#include <iomanip>
#include "hgeomvolume.h"
#include "hgeombasicshape.h"
#include "hgeommedium.h"
#include "TArrayD.h"

enum EHGeomNodeType {
  kHGeomElement = 0,
  kHGeomKeepin  = 1,
  kHGeomModule  = 2,
  kHGeomTopNode = 3,
  kHGeomRefNode = 4 };

class TGeoVolume;

class HGeomNode : public HGeomVolume {
protected:
  HGeomBasicShape* pShape;      // pointer to the shape
  HGeomNode* pMother;           // pointer to the mother volume
  HGeomMedium* medium;          // pointer to medium
  EHGeomNodeType volumeType;    // type of volume
  Bool_t active;                // in/out flag for creation
  HGeomTransform center;        // position of the GEANT/ROOT center
  HGeomTransform* labTransform; // Transformation to the LAB system  
  HGeomNode* copyNode;          // pointer to the reference volume for a copy
  Bool_t created;               // Flag set kTRUE after creation of volume
  TGeoVolume* rootVolume;       // pointer to the ROOT node
public:
  HGeomNode();
  HGeomNode(HGeomNode&);
  ~HGeomNode();
  EHGeomNodeType getVolumeType() { return volumeType; }
  Bool_t isTopNode() { return (Bool_t)(volumeType == kHGeomTopNode);}
  Bool_t isRefNode() { return (Bool_t)(volumeType == kHGeomRefNode);}
  Bool_t isKeepin()  { return (Bool_t)(volumeType == kHGeomKeepin);}
  Bool_t isModule()  { return (Bool_t)(volumeType == kHGeomModule);}
  Int_t getCopyNo();
  HGeomBasicShape* getShapePointer() { return pShape; }
  HGeomNode* getMotherNode() {return pMother;}
  HGeomMedium* getMedium() {return medium;}
  Bool_t isActive() {return active;}
  HGeomTransform& getCenterPosition() {return center;}
  TArrayD* getParameters();
  HGeomTransform* getPosition();
  HGeomTransform* getLabTransform();
  HGeomNode* getCopyNode() {return copyNode;}    
  TGeoVolume* getRootVolume() {return rootVolume;}
  TList* getTree();
  Bool_t isSensitive();
  Bool_t isCreated() {return created;}
  void setCreated() {created=kTRUE;}
  void setName(const Text_t* s);
  void setVolumeType(EHGeomNodeType t) {volumeType=t;}
  void setVolumePar(HGeomNode&);
  void setShape(HGeomBasicShape* s);
  void setMother(HGeomNode* s);
  void setMedium(HGeomMedium* m) {medium=m; }
  void setActive(Bool_t a=kTRUE) {active=a;}
  void setCenterPosition(const HGeomTransform& t) {center=t;}
  void setCopyNode(HGeomNode* p) {copyNode=p;}
  void setRootVolume(TGeoVolume* p) {rootVolume=p;}
  HGeomTransform* calcLabTransform();
  void setLabTransform(HGeomTransform&);
  Bool_t calcModuleTransform(HGeomTransform&);
  Bool_t calcRefPos(HGeomVector&);
  void clear();
  void print();
  Bool_t write(fstream&);
  Int_t compare(HGeomNode&);
  ClassDef(HGeomNode,0) // basic geometry parameters of a volume
};

// -------------------- inlines --------------------------

inline void HGeomNode::setName(const Text_t* s) {
  // Sets the name of the volume
  fName=s;
  fName.ToUpper();
}

inline void HGeomNode::setShape(HGeomBasicShape* s) {
  // Sets the shape of the volume
  if (s) {
    pShape=s;
    shape=pShape->GetName();
  }
}

inline void HGeomNode::setMother(HGeomNode* m) {
  // Sets the mother of the volume
  if (m) {
    pMother=m;
    mother=pMother->GetName();
  }
}

inline Bool_t HGeomNode::isSensitive() {
  if (medium) return medium->isSensitive();
  else return kFALSE;
}

#endif /* !HGEOMNODE_H */
