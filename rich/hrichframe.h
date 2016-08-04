//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : RICH team member
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichFrame
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHFRAME_H
#define HRICHFRAME_H

#include "TArrayF.h"
#include "TObjArray.h"

class HParamList;
class HRichFrameCorner;

class HRichFrame : public TObject {

private:

   Int_t     fCornerNr;             // Frame corner number counter
   TObjArray fFrameArr;             // Array of frame corners
   TArrayF   fFrameCorners;         // Array of frame corners coordinates


private:
   void clear();


public:

   HRichFrame();
   virtual ~HRichFrame();

   void   printParams();
   void   putParams(HParamList* l);
   Bool_t getParams(HParamList* l);

   Bool_t calculateFlagArea();
   Int_t  isOut(Float_t x, Float_t y);
   Int_t  addCorner(HRichFrameCorner*);

///////////////////////////////////////////////////////////////////
// GETTERS
   HRichFrameCorner* getCorner(Int_t n);
   Int_t getCornerNr();

///////////////////////////////////////////////////////////////////
// SETTERS
   Int_t setCorner(HRichFrameCorner* pCorner, Int_t n);

   ClassDef(HRichFrame, 1)

};

///////////////////////////////////////////////////////////////////
// GETTERS
inline Int_t HRichFrame::getCornerNr()
{
   return fCornerNr;
}


#endif // HRICHFRAME_H
