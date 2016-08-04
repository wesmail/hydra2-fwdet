using namespace std;
#include <iostream> 
#include <iomanip>

#include "hshowerpad.h"
#include "hparamlist.h"

ClassImp(HShowerPad)
ClassImp(HShowerPadTab)

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HShowerPad
// Class describe coordinates one pad
//
// HShowerPadTab
// Table of local coordinates of pads.  
// Thera are pads informations in one dimensional table
// Two-dimensional structure of pads in shower detector
// is calculated to one index of table of pads
//
/////////////////////////////////////////////////////////////


HShowerPad::HShowerPad() {
//creating default pad
     nPadsX=32;
     nPadNr = 0;
     nPadFlag = 0;
     fXld = fYld = fXlu = fYlu = 0.f;
     fXrd = fYrd = fXru = fYru = 0.f;
}

HShowerPad::HShowerPad(const HShowerPad& srcPad) :TObject(srcPad)
{
  //copy ctor
  nPadsX=32;
  nPadNr = srcPad.nPadNr;
  nPadFlag =  srcPad.nPadFlag;
  fXld = srcPad.fXld;
  fYld = srcPad.fYld;
  fXlu = srcPad.fXlu;
  fYlu = srcPad.fYlu;
  fXrd = srcPad.fXrd;
  fYrd = srcPad.fYrd;
  fXru = srcPad.fXru;
  fYru = srcPad.fYru;
}

HShowerPad& HShowerPad::operator=(const HShowerPad& srcPad) {
  nPadNr = srcPad.nPadNr;
  nPadFlag = srcPad.nPadFlag;
  fXld = srcPad.fXld;
  fYld = srcPad.fYld;
  fXlu = srcPad.fXlu;
  fYlu = srcPad.fYlu;
  fXrd = srcPad.fXrd;
  fYrd = srcPad.fYrd;
  fXru = srcPad.fXru;
  fYru = srcPad.fYru;
  return *this;
}

void HShowerPad::reset() {
//clearing data
   nPadNr = 0;
   nPadFlag = 0;
   fXld = fYld = fXlu = fYlu = 0.f;
   fXrd = fYrd = fXru = fYru = 0.f;
}

void HShowerPad::getPadCenter(Float_t *pfX, Float_t *pfY) {
//calculate position of geometrical center of pad
  *pfX = 0.25 * (fXld + fXlu + fXrd + fXru); 
  *pfY = 0.25 * (fYld + fYlu + fYrd + fYru); 
}

void HShowerPad::getPadPos(Int_t *pnRow, Int_t *pnCol) {
//return number of row and col 
  *pnCol = nPadNr % nPadsX;
  *pnRow = nPadNr / nPadsX;
}

Int_t HShowerPad::isOut(HShowerGeantWire *qhit) {
//?????????????????
  Float_t fX, fY;
  qhit->getXY(&fX, &fY);
  return isOut(fX, fY);  
}

Int_t HShowerPad::isOut(Float_t x, Float_t y) {
//returns 1 if point (x,y) is outside of pad, otherwise 0
  return (isOutY(x, y) || isOutX(x, y));
}


Int_t HShowerPad::isOutX(Float_t x, Float_t y) {
//returns 1 if  x coordinate of point is outside of pad, otherwise 0
  Float_t a,b;

  if (fXld==fXlu && x<fXld) return 1;
  if (fYld==fYlu && y<fYld) return 1;

  a = (fYlu-fYld)/(fXlu-fXld);
  b = (fXlu*fYld-fXld*fYlu)/(fXlu-fXld);
  if(a>0.){
    if (y>(a*x+b)) return 1;
  } else {
    if (y<(a*x+b)) return 1;
  }

  if (fXru==fXrd && x>fXru) return 1;
  if (fYru==fYrd && y>fYru) return 1;

  a = (fYrd-fYru)/(fXrd-fXru);
  b = (fXrd*fYru-fXru*fYrd)/(fXrd-fXru);
  if(a>0.){
    if (y<(a*x+b)) return 1;
  } else {
    if (y>(a*x+b)) return 1;
  }

  return 0;
}


Int_t HShowerPad::isOutY(Float_t x, Float_t y) {
//returns 1 if  y coordinate of point is outside of pad, otherwise 0

  Float_t a,b;

  if (fYlu==fYru && y>fYlu) return 1;
  if (fYrd==fYld && y<fYrd) return 1;

  if (fXrd==fXld && x<fXrd) return 1;
  if (fXlu==fXru && x>fXlu) return 1;


  a = (fYru-fYlu)/(fXru-fXlu);
  b = (fXru*fYlu-fXlu*fYru)/(fXru-fXlu);
  if (y>(a*x+b)) return 1;

  a = (fYld-fYrd)/(fXld-fXrd);
  b = (fXld*fYrd-fXrd*fYld)/(fXld-fXrd);
  if (y<(a*x+b)) return 1;

  return 0;
}

//-------------------------------------------------------------

HShowerPadTab::HShowerPadTab() {
//default ctor
  nPads = 1024;
  nPadsX = 32;
  nPadsY = 32;
  m_pPadArr = new TObjArray(1024);
}

HShowerPadTab::~HShowerPadTab() {
  if (m_pPadArr) {
    m_pPadArr->Delete();
    delete m_pPadArr;
    m_pPadArr = NULL;
  }
}

void HShowerPadTab::reset() {
//clearing data
  m_pPadArr->Delete();
}

void HShowerPadTab::putParams(HParamList* l) {
  if (!l) return;
  Int_t nCorners=8;
  TArrayF arr(nPads*nCorners);
  HShowerPad* p=0;
  Int_t k=0;
  for(Int_t i=0;i<nPads;i++) {
    p=(HShowerPad*)m_pPadArr->At(i);
    if (p) {
      k=i*nCorners;
      arr.AddAt(p->fXld,k);
      arr.AddAt(p->fYld,++k);
      arr.AddAt(p->fXlu,++k);
      arr.AddAt(p->fYlu,++k);
      arr.AddAt(p->fXrd,++k);
      arr.AddAt(p->fYrd,++k);
      arr.AddAt(p->fXru,++k);
      arr.AddAt(p->fYru,++k);
    }
  }
  l->add(Form("pPadCorners_%i",m_nModuleID),arr);
}

Bool_t HShowerPadTab::getParams(HParamList* l) {
  if (!l) return kFALSE;
  TArrayF arr;
  Int_t nCorners=8;
  if (!l->fill(Form("pPadCorners_%i",m_nModuleID),&arr)) return kFALSE;
  if (arr.GetSize()!= nPads*nCorners) {
    Error("getParams(HParamList*)",
          "Only pad corners for %i pads read",arr.GetSize()/nCorners);
    return kFALSE;
  }
  HShowerFrame* phFrame = getFrame();
  HShowerPad* p=0;
  Int_t k=0;
  for (Int_t i=0;i<nPads;i++) {
    k=i*nCorners;
    p = new HShowerPad;
    p->setPadNr(i);
    p->fXld=arr.At(k);
    p->fYld=arr.At(++k);
    p->fXlu=arr.At(++k);
    p->fYlu=arr.At(++k);
    p->fXrd=arr.At(++k);
    p->fYrd=arr.At(++k);
    p->fXru=arr.At(++k);
    p->fYru=arr.At(++k);
    if (!phFrame->isOut(p->fXld, p->fYld) &&
        !phFrame->isOut(p->fXlu, p->fYlu) &&
        !phFrame->isOut(p->fXru, p->fYru) &&
        !phFrame->isOut(p->fXrd, p->fYrd)) {
      p->setPadFlag(1);
    } else if (!phFrame->isOut(p->fXld, p->fYld) ||
        !phFrame->isOut(p->fXlu, p->fYlu) ||
        !phFrame->isOut(p->fXru, p->fYru) ||
        !phFrame->isOut(p->fXrd, p->fYrd)) {
      p->setPadFlag(2);
    } else {
      p->setPadFlag(0);
    }
    setPad(p,i);
  }
  return kTRUE;
}

void HShowerPadTab::setPad(HShowerPad* pPad, Int_t nRow, Int_t nCol) {
//set pad information at position defined by nRow and nCol
  Int_t nAddr = calcAddr(nRow, nCol);  //calculating index
  setPad(pPad, nAddr);
}

void HShowerPadTab::setPad(HShowerPad* pPad, Int_t nAddr) {
//set pad information at position defined by nAddr
  if( m_pPadArr->At(nAddr)) delete m_pPadArr->At(nAddr);
  m_pPadArr->AddAt(pPad, nAddr);
}

HShowerPad* HShowerPadTab::getPad(Int_t nPadNr) {
//return pad information
  return (HShowerPad*)m_pPadArr->At(nPadNr);
}

HShowerPad* HShowerPadTab::getPad(Int_t nRow, Int_t nCol) {
//return pad information
//nRow and nCol is used to calculate index
  if ((nRow < 0) || (nRow >= nPadsY)) return NULL;
  if ((nCol < 0) || (nCol >= nPadsX)) return NULL;
  Int_t nAddr = calcAddr(nRow, nCol);
  return getPad(nAddr);
}

HShowerPad* HShowerPadTab::getPad(Float_t  fXpos, Float_t fYpos) {
//looking for pad which contains point fXpos, fYpos
  Int_t nPadAddr = 0;
  Int_t nHit = 0;
  for(Int_t j = 0; j < nPadsY; j++) {
    if (!getPad(nPadAddr)->isOutY(fXpos, fYpos)) {
      nHit=1;
      break;
    } else nPadAddr += nPadsX;
  }
  if (!nHit) return NULL;

  nHit = 0;
  for(Int_t j = 0; j < nPadsX; j++, nPadAddr++) {
    if (!getPad(nPadAddr)->isOutX(fXpos, fYpos)) {                    
      nHit = 1;
      break;
    }
  }
  if (!nHit) return NULL;

  return getPad(nPadAddr);
}

Bool_t HShowerPadTab::isOut(Int_t nX, Int_t nY) {
//testing if pad in nX, nY position is inside table
  if ((nX < 0) || (nX >= nPadsX)) return 1;
  if ((nY < 0) || (nY >= nPadsY)) return 1;
  return 0;
}
