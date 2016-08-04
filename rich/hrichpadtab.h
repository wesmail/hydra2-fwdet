//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichPadTab
//
// fPadParams: array with size (numberOfPads * 11) in sector 1
//               11 numbers for each pad:
//                 1:    pad_pos_id = colNumber*100 + rowNumber
//                 2:    theta
//                 3:    phi (in sector 1)
//                 4..11 x and y of the 4 pad corners
//               theta and the pad corners are identical in each sector
//               for phi the sector rotation (relative to sector 1) must be added
//
//////////////////////////////////////////////////////////////////////////////


#ifndef RICHPADTAB_H
#define RICHPADTAB_H

#include "TArrayF.h"
#include "TClonesArray.h"
#include "TObject.h"

#include "richdef.h"

class HParamList;
class HRichPad;

class HRichPadTab : public TObject {

private:

   TClonesArray* fPadsArray;            // Pad array
   Int_t   fActivePadsNr;               // Number of active pads
   UInt_t  fPadsLongestRow;             // Number of the row with the most active pads
   UInt_t  fMiddlePad;                  // Pad number of the middle pad in the longest row
   Float_t fPadsLongestRowMiddle;       // X-position of the middle pad in the longest row

   TArrayF fPadParams;                  // Array of pad parameters


private:

   TObject*& getSlot(Int_t nIndx);
   void      deletePads();
   Int_t     calcAddr(UInt_t col, UInt_t row);
   Bool_t    calculatePadParameters();
   Bool_t    dummyInitialisation();
   Bool_t    initialisePads();

public:

   HRichPadTab();
   ~HRichPadTab();

   void   clear();
   void   incActivePadsNr();
   void   printParams();
   void   printParamsFull();
   void   putParams(HParamList* l);
   Bool_t getParams(HParamList* l);
   Bool_t isOut(UInt_t X, UInt_t Y);
   Int_t  createPads();

// Getters
   HRichPad* getPad(UInt_t padNr);
   HRichPad* getPad(UInt_t col, UInt_t row);
   HRichPad* getPad(Float_t Xpos, Float_t Ypos);
   Int_t getActivePadsNr();

// Setters
   void setPad(HRichPad* pPad, UInt_t col, UInt_t row);
   void setPad(HRichPad* pPad, UInt_t padNr);

   ClassDef(HRichPadTab, 1)
};

inline Int_t HRichPadTab::calcAddr(UInt_t col, UInt_t row)
{
   return col + RICH_MAX_COLS * row;
}
inline void HRichPadTab::incActivePadsNr()
{
   fActivePadsNr++;
}

inline HRichPad* HRichPadTab::getPad(UInt_t padNr)
{
   return static_cast<HRichPad*>(fPadsArray->At(padNr));
}
inline Int_t HRichPadTab::getActivePadsNr()
{
   return fActivePadsNr;
}

#endif // RICHPADTAB_H
