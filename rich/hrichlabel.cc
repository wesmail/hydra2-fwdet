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
//  HRichLabel
//
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichlabel.h"

ClassImp(HRichLabel)


Int_t HRichLabel::iLabelNr = 0;

HRichLabel::HRichLabel()
{
   iLeftX = 127;
   iRightX = -1;
   iLowerY = 127;
   iUpperY = -1;
   iSignature = 0;
   iFiredPadsNr = 0;
   iLabeledPadsNr = 0;
}

HRichLabel::HRichLabel(const HRichLabel& source) :TObject(source)
{
   iLeftX = source.iLeftX;
   iRightX = source.iRightX;
   iLowerY = source.iLowerY;
   iUpperY = source.iUpperY;
   iSignature = source.iSignature;
   iFiredPadsNr = source.iFiredPadsNr;
   iLabeledPadsNr = source.iLabeledPadsNr;
}


HRichLabel& HRichLabel::operator=(const HRichLabel& source)
{
   if (this != &source) {
      iLeftX = source.iLeftX;
      iRightX = source.iRightX;
      iLowerY = source.iLowerY;
      iUpperY = source.iUpperY;
      iSignature = source.iSignature;
      iFiredPadsNr = source.iFiredPadsNr;
      iLabeledPadsNr = source.iLabeledPadsNr;
   }

   return *this;
}


void HRichLabel::Reset()
{
   iSignature = 0;
   iFiredPadsNr = 0;
   iLabeledPadsNr = 0;
   if (iLabelNr) iLabelNr--;
}


