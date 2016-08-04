//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
// HGeomNode
//
// Class to hold the basic geometry properties of a GEANT volume
//
////////////////////////////////////////////////////////////////

#include "hgeomnode.h"
#include "TList.h"
#include <iostream>
#include <iomanip>

ClassImp(HGeomNode)

HGeomNode::HGeomNode() {
  // Constructor
  labTransform=0;
  clear();
}

HGeomNode::HGeomNode(HGeomNode& r) {
  // Copy constructor
  points=0;
  nPoints = 0;
  fName=r.GetName();
  setVolumePar(r);
  if (r.isActive()) active=kTRUE;
  else active=kFALSE;
  volumeType=r.getVolumeType();
  center=r.getCenterPosition();
  copyNode=r.getCopyNode();
  if (r.isCreated()) created=kTRUE;
  else created=kFALSE; 
  rootVolume=r.getRootVolume();
  labTransform=new HGeomTransform(*(r.getLabTransform()));
}

HGeomNode::~HGeomNode() {
  // Destructor
  if (points) {
    for (Int_t i=0;i<nPoints;i++) points->RemoveAt(i);
    delete points;
    points=0;
  }
  if (labTransform) {
    delete labTransform;
    labTransform=0;
  }
}

void HGeomNode::setVolumePar(HGeomNode& r) {
  // Copies all volume parameters except the name
  setMother(r.getMotherNode());
  medium=r.getMedium();
  setShape(r.getShapePointer());
  Int_t n=r.getNumPoints();
  createPoints(n);
  for (Int_t i=0;i<nPoints;i++) setPoint(i,*(r.getPoint(i)));
  transform=r.getTransform();
}

void HGeomNode::clear() {
  // Clears the volume and deletes the points
  pMother=0;
  medium=0;
  pShape=0;
  volumeType=kHGeomElement;
  active=kFALSE;
  created=kFALSE;
  copyNode=0;
  rootVolume=0;
  if (labTransform) {
    delete labTransform;
    labTransform=0;
  }
  HGeomVolume::clear();
}

Int_t HGeomNode::getCopyNo() {
  // Returns the copy number
  Int_t n=1;
  Int_t l=fName.Length();
  if (l>4) {
    TString s(fName);
    s.Remove(0,4);
    sscanf(s.Data(),"%i",&n);
  }
  return n; 
}

TArrayD* HGeomNode::getParameters() {
  // Returns the parameters to create a GEANT/ROOT volume
  if (pShape) return pShape->calcVoluParam(this);
  return 0;
}

HGeomTransform* HGeomNode::getPosition() {
  // Returns the transformation to position a volume in GEANT/ROOT
  if (pShape&&pMother) {
    pShape->calcVoluPosition(this,pMother->getCenterPosition());
    center.setTransform(*(pShape->getCenterPosition()));
    return pShape->getVoluPosition();
  }
  return 0;
}

void HGeomNode::print() {
  // Prints all parameters of a volume
  cout<<((const Char_t*)fName)<<'\n';
  if (pMother)cout<<((const Char_t*)mother)<<'\n';
  else cout<<"-- unknown mother --\n";
  if (!copyNode) {
    if (pShape) cout<<((const Char_t*)shape)<<'\n';
    else cout<<"-- unknown shape --\n";
    if (medium) cout<<medium->GetName()<<'\n';
    else cout<<"-- unknown medium --\n";
    if (points && pShape) pShape->printPoints(this);
    else cout<<"-- no points --\n";
  }
  transform.getTransVector().print();
  transform.getRotMatrix().print();
  cout<<"//----------------------------------------------------------\n";
}

Bool_t HGeomNode::write(fstream& fout) {
  // Writes all parameters of a volume to file
  fout<<fName.Data()<<'\n';
  if (pMother) fout<<((const Char_t*)mother)<<'\n';
  else {
    Error("write","Unknown mother for %s\n",fName.Data());
    return kFALSE;
  }
  if (!copyNode) {
    if (pShape) fout<<((const Char_t*)shape)<<'\n';
    else {
      Error("write","Unknown shape for %s\n",fName.Data());
      return kFALSE;
    }
    if (medium) fout<<medium->GetName()<<'\n';
    else {
      Error("write","Unknown medium for %s\n",fName.Data());
      return kFALSE;
    }
    if (points && pShape) pShape->writePoints(&fout,this);
    else {
      Error("write","No points for %s\n",fName.Data());
      return kFALSE;
    }
  }
  const HGeomRotation& r=transform.getRotMatrix();
  const HGeomVector& v=transform.getTransVector();
  fout.precision(3);
  fout<<v(0)<<" "<<v(1)<<" "<<v(2)<<'\n';
  fout.precision(7);
  for(Int_t i=0;i<9;i++) fout<<r(i)<<"  ";
  fout<<'\n';
  fout<<"//----------------------------------------------------------\n";
  return kTRUE;
}

TList* HGeomNode::getTree() {
  // Returns the tree of the mother volumes
  TList* tree=new TList();  
  HGeomNode* v=this;
  HGeomNode* mother;
  do {
    mother=v->getMotherNode();
    if (mother) tree->Add(mother);
    v=mother;
  } while (v&&!v->isTopNode());
  return tree; 
}

HGeomTransform* HGeomNode::getLabTransform() {
  // Returns the lab transformation
  if (labTransform) return labTransform;
  else return calcLabTransform();
}

HGeomTransform* HGeomNode::calcLabTransform() {
  // Calculates the lab transformation
  labTransform=new HGeomTransform(transform);
  if (!isTopNode()) {
    if (!pMother) {
      Error("calcLabTransform()","Mother volume of %s not found!",GetName());
      delete labTransform;
      return 0;
    }
    HGeomTransform* tm=pMother->getLabTransform();
    if (!tm) {; 
      Error("calcLabTransform()",
            "Lab Transformation of mother volume of %s not found!",GetName());
      delete labTransform;
      return 0;
    } 
    labTransform->transFrom(*tm);
  }
  return labTransform;
}

void HGeomNode::setLabTransform(HGeomTransform& t) {
  // Sets the lab transformation
  if (!isTopNode()) {
    if (labTransform) labTransform->setTransform(t); 
    else labTransform=new HGeomTransform(t);
    if (!pMother) {
      Error("calcLabTransform()","Mother volume of %s not found!",GetName());
      delete labTransform;
      return;
    }
    HGeomTransform* tm=pMother->getLabTransform();
    if (!tm) {; 
      Error("calcLabTransform()",
            "Lab Transformation of mother volume of %s not found!",GetName());
      delete labTransform;
      return;
    } 
    transform=t;
    transform.transTo(*tm);
  }
}

Bool_t HGeomNode::calcModuleTransform(HGeomTransform& modTransform) {
  // Calculates the transformation relative to the detectors coordinate system
  HGeomNode* node=this;
  modTransform=transform;
  while (node&&!node->isModule()) {
    HGeomNode* pm=node->getMotherNode();
    if (!pm) {
      Error("calcModuleTransform","Mother volume of %s not found!",node->GetName());
      return kFALSE;
    }
    modTransform.transFrom(pm->getTransform());
    node=pm;
  }
  if (isTopNode()) return kFALSE;
  else return kTRUE;
}

Bool_t HGeomNode::calcRefPos(HGeomVector& refPos) {
  // Calculates the position in the detectors coordinate system
  HGeomNode* node=this;
  refPos=center.getTransVector();
  do {
    refPos=node->getTransform().transFrom(refPos);
    node=node->getMotherNode();
    if (!node) {
      Error("calcModuleTransform","Mother volume %s not found!",node->GetName());
      return kFALSE;
    }
  } while (node&&!node->isModule());
  if (isTopNode()) return kFALSE;
  else return kTRUE;
}

Int_t HGeomNode::compare(HGeomNode& rn) {
  // Compares the volume with the volume rn and prints the diagnose
  // Returns kTRUE if the volume parameters are the same
  Int_t diff[]={0,0,0,0,0,0};
  Int_t n=0;
  cout<<fName<<'\t';
  if (mother.CompareTo(rn.getMother())!=0) {
    diff[0]=1;
    n++;
  }
  if (medium&&rn.getMedium()) {
    TString med=medium->GetName();
     if (med.CompareTo(rn.getMedium()->GetName())!=0) {
       diff[1]=1;
       n++;
     }
  } else {
    diff[1]=1;
    n++;
  }
  if (shape.CompareTo(rn.getShape())!=0) {
    diff[2]=1;
    n++;
  }
  Int_t np=rn.getNumPoints();
  if (points&&nPoints==np) {
    for (Int_t i=0;i<np;i++) {
      HGeomVector v1=*(getPoint(i));
      HGeomVector v2=*(rn.getPoint(i));
      if (fabs(v1(0)-v2(0))>=0.001||fabs(v1(1)-v2(1))>=0.001||fabs(v1(2)-v2(2))>=0.001) {
        diff[3]=1;
        n++;
        break;
      }
    }
  } else {
    diff[3]=1;
    n++;
  }
  HGeomVector v1=transform.getTransVector();
  HGeomVector v2=rn.getTransform().getTransVector();
  if (fabs(v1(0)-v2(0))>=0.001||fabs(v1(1)-v2(1))>=0.001||fabs(v1(2)-v2(2))>=0.001) {
    diff[4]=1;
    n++;
  }
  HGeomRotation r1=transform.getRotMatrix();
  HGeomRotation r2=rn.getTransform().getRotMatrix();
  for (Int_t i=0;i<9;i++) {
    if (fabs(r1(i)-r2(i))>=1.e-7) {
      diff[5]=1;
      n++;
      break;
    }
  }
  if (n>0) {
    for (Int_t i=0;i<6;i++) cout<<"    "<<diff[i]<<"  ";
    cout<<'\n';
  } else cout<<" same\n";
  return n;
}
