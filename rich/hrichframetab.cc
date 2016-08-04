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


#include "hrichframe.h"
#include "hrichframetab.h"

using namespace std;

ClassImp(HRichFrameTab)

HRichFrameTab::HRichFrameTab()
   : TObject()
{
   fNrFrames = 0;
   fpFrame   = NULL;
}

HRichFrameTab::~HRichFrameTab()
{
   deleteFrames();
}

void
HRichFrameTab::deleteFrames()
{
   if (fpFrame) {
      delete[] fpFrame;
   }
   fpFrame = NULL;
   fNrFrames = 0;
}

Bool_t
HRichFrameTab::init()
{
   createFrames(1);

   //HRichFrame *fpFrame = getFrame(0);
   //fpFrame->initAscii(NULL);

   return kTRUE;
}

void
HRichFrameTab::createFrames(Int_t n)
{
   deleteFrames();
   fNrFrames = n;
   fpFrame = new HRichFrame[fNrFrames];
}

HRichFrame*
HRichFrameTab::getFrame(Int_t nFrame)
{
   if (nFrame < 0 || nFrame >= fNrFrames) return NULL;

   return &fpFrame[nFrame];
}

Int_t
HRichFrameTab::isOut(Int_t nFrame, Float_t x, Float_t y)
{
   if (nFrame < 0 || nFrame >= fNrFrames) return 0;

   return fpFrame[nFrame].isOut(x, y);
}
