#ifndef HGEOMHIT_H
#define HGEOMHIT_H

using namespace std;
#include "TNamed.h"
#include "TArrayI.h"
#include "hgeomtransform.h"
#include <fstream>
#include <iomanip>

class HGeomSet;
class HGeomNode;

class  HGeomHit : public TNamed {
private:
  class HGeomCopyNodeNum : public TNamed {
    public:
      Int_t maxCopyNum;
      HGeomCopyNodeNum(TString& name,Int_t n) {
        SetName(name);
        maxCopyNum=n;
      }
      ~HGeomCopyNodeNum() {}
  };
  TList* copies;          // List of copy nodes
protected:
  TString hitFile;        // name of hit file
  HGeomSet* pSet;         // pointer to detector part
  HGeomNode* currentNode; // pointer to current node
  Char_t compName[5];       // name of hit component
  Int_t nh;      // number of hit components
  Char_t* chnamh;  // names of the hit components
  Int_t* nbitsh; // array for the number of bits in which to pack the components of a hit
  Float_t* orig; // array for the scale factors applied before packing the hit values
  Float_t* fact; // array for the scale factors applied before packing the hit values
  Int_t nvmax;   // maximum number of levels in tree of module
  Int_t nv;      // number of volume descriptors (derived from tree)
  Char_t* chnmsv;  // array of nv names to decribe the volume (derived from tree)   
  Int_t* nbitsv; // array of bits in which to pack the copy number of the volume chnmsv

  HGeomHit(HGeomSet* p=0);
public:
  virtual ~HGeomHit();
  void setHitFile(const Char_t* filename) {hitFile=filename;}
  const Char_t* getHitFile() {return hitFile.Data();}
  void read(fstream&);
  void write(fstream&);
  void print();
  Bool_t setCurrentNode(HGeomNode*);
  const Char_t* getDetectorName();
  const Char_t* getComponentName() {return compName;}
  Bool_t calcRefPos(HGeomVector&,TString&);
  virtual Int_t getIdType()=0;
  Int_t getNh() {return nh;} 
  Char_t* getChnamh() {return chnamh;}
  Int_t* getNbitsh() {return nbitsh;}
  Float_t* getOrig() {return orig;}
  Float_t* getFact() {return fact;}
  Int_t getNv() {return nv;}
  Char_t* getChnmsv() {return chnmsv;}  
  Int_t* getNbitsv() {return nbitsv;}
  void setNh(Int_t);
  void fill(Int_t,const Char_t*,Int_t,Float_t,Float_t);
  ClassDef(HGeomHit,0) // Base class for hit definition in GEANT
};

#endif  /* !HGEOMHIT_H */
