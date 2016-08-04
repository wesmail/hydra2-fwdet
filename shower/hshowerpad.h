#ifndef HSHOWERPAD_H
#define HSHOWERPAD_H

#include "TObject.h"
#include "TObjArray.h"
#include "hshowerframe.h"
#include "hshowergeantwire.h"

class HParamList;

class HShowerPad  : public TObject{
private:
   friend class HShowerPadTab;
   Int_t nPadNr;   //pad identification
   Int_t nPadFlag; // 0-total outside, 1-inside, 2-partial inside
   Int_t nPadsX;   //number of pads in one raw

   //coordinates of corners of pad
   Float_t fXld,fYld,fXlu,fYlu; // d - down, u - up
   Float_t fXrd,fYrd,fXru,fYru; // l - left, r - right
public:
   HShowerPad();
   virtual ~HShowerPad() {}
   HShowerPad(const HShowerPad& srcPad);

   void reset();

   Int_t isOut(HShowerGeantWire *qhit);
   Int_t isOut(Float_t x, Float_t y);
   Int_t isOutX(Float_t x, Float_t y);
   Int_t isOutY(Float_t x, Float_t y);

   void getPadCenter(Float_t *pfX, Float_t *pfY);

   Int_t getPadNr(){return nPadNr;}
   void getPadPos(Int_t *pRow, Int_t *pCol);
   Int_t getPadFlag(){return nPadFlag;}
   void setPadFlag(Int_t nFlag){nPadFlag = nFlag;}
   void setPadNr(Int_t nPad){nPadNr = nPad;}

   Float_t getXld(void) const { return fXld; }
   Float_t getYld(void) const { return fYld; }
   Float_t getXlu(void) const { return fXlu; }
   Float_t getYlu(void) const { return fYlu; }
   Float_t getXrd(void) const { return fXrd; }
   Float_t getYrd(void) const { return fYrd; }
   Float_t getXru(void) const { return fXru; }
   Float_t getYru(void) const { return fYru; }

   Float_t getXld_mm(void) const { return 10.0f * fXld; }
   Float_t getYld_mm(void) const { return 10.0f * fYld; }
   Float_t getXlu_mm(void) const { return 10.0f * fXlu; }
   Float_t getYlu_mm(void) const { return 10.0f * fYlu; }
   Float_t getXrd_mm(void) const { return 10.0f * fXrd; }
   Float_t getYrd_mm(void) const { return 10.0f * fYrd; }
   Float_t getXru_mm(void) const { return 10.0f * fXru; }
   Float_t getYru_mm(void) const { return 10.0f * fYru; }

   HShowerPad& operator=(const HShowerPad&); 

   ClassDef(HShowerPad, 2)
};

class HShowerPadTab  : public TObject {
private:

   Int_t nPads;            //total number of pads
   Int_t nPadsX;           //number of rows
   Int_t nPadsY;           //number of columns
   Int_t m_nModuleID;      //module identification
   TObjArray* m_pPadArr;   //table of pads
   HShowerFrame* m_pFrame; //reference to frame for this module

public:
   HShowerPadTab();
   ~HShowerPadTab();

   void reset();
   void   putParams(HParamList*);
   Bool_t getParams(HParamList*);

   void setPad(HShowerPad* pPad, Int_t nRow, Int_t nCol);
   void setPad(HShowerPad* pPad, Int_t nPadNr);

   HShowerPad* getPad(Int_t nPadNr);
   HShowerPad* getPad(Int_t nRow, Int_t nCol);
   HShowerPad* getPad(Float_t fXpos, Float_t fYpos);

   Int_t getNPad() {return nPads;}
   Int_t getNPadX() {return nPadsX;}
   Int_t getNPadY() {return nPadsY;}

   Bool_t isOut(Int_t nX, Int_t nY);

   void setFrame(HShowerFrame *pFrame) {m_pFrame = pFrame;}
   HShowerFrame* getFrame() {return m_pFrame;}
   void setModuleID(Int_t id) {m_nModuleID = id;}
   Int_t getModuleID() {return m_nModuleID;}

private:
   Int_t calcAddr(Int_t nRow, Int_t nCol) {return nCol + nPadsX * nRow;}
  
   ClassDef(HShowerPadTab, 2)
};
#endif /* !HSHOWERPAD_H */
