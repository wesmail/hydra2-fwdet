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
//  HRichFrameTab
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHFRAMETAB_H
#define HRICHFRAMETAB_H

#include "TObject.h"

class HParFrame;

class HRichFrameTab : public TObject {

private:

   Int_t       fNrFrames;    // Number of frames
   HRichFrame* fpFrame;      //

public:

   HRichFrameTab();
   ~HRichFrameTab();

   virtual Bool_t init();

   void createFrames(Int_t n);
   void deleteFrames();

   Int_t isOut(Int_t nFrame, Float_t x, Float_t y);

///////////////////////////////////////////////////////////////////
// GETTERS
   Int_t getNFrames();
   HRichFrame* getFrame(Int_t n);

   ClassDef(HRichFrameTab, 1)
};

///////////////////////////////////////////////////////////////////
// GETTERS
inline Int_t HRichFrameTab::getNFrames()
{
   return fNrFrames;
}

#endif // HRICHFRAMETAB_H
