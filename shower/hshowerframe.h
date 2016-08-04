#ifndef HSHOWERFRAME_H
#define HSHOWERFRAME_H
#pragma interface

#include "TObject.h"
#include "TObjArray.h"

class HShowerFrameCorner: public TObject {
private:
  Float_t fXcor;   // X coordinate of frame 
  Float_t fYcor;   // Y coordinate of frame
  Int_t nFlagArea; // sensitive area flag
 
public:
  HShowerFrameCorner();

  Float_t getX(){return fXcor;}
  Float_t getY(){return fYcor;}
  Int_t getFlagArea(){return nFlagArea;}

  void setX(Float_t fX){fXcor = fX;}
  void setY(Float_t fY){fYcor = fY;}
  void setFlagArea(Int_t nFlag){nFlagArea = nFlag;}

  ClassDef(HShowerFrameCorner, 0)
};

class HShowerFrame: public TObject {
private:
  Int_t nCorners;       // number of corners
  Int_t m_nModuleID;    // module identification
  TObjArray m_FrameArr; // table of corners coordinates

public:
  HShowerFrame();
  virtual ~HShowerFrame();

  void reset();
  void printParams();
  Int_t isOut(Float_t x, Float_t y);

  HShowerFrameCorner* getCorner(Int_t n);
  Int_t addCorner(HShowerFrameCorner*);
  Int_t setCorner(HShowerFrameCorner* pCorner, Int_t n); 

  void setNCorners(Int_t n){nCorners = n;}
  Int_t getNCorners(){return nCorners;}

  void setModuleID(Int_t id){m_nModuleID = id;}
  Int_t getModuleID(){return m_nModuleID;}

  ClassDef(HShowerFrame, 0)
};

#endif /* !HSHOWERFRAME_H */
