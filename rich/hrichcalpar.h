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
//  HRichCalPar
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHCALPAR_H
#define HRICHCALPAR_H

#include "hlocation.h"
#include "hobjtable.h"
#include "hparset.h"

class HParAsciiFileIo;
class HRichCalParCell;

class HRichCalPar : public HParSet {

private:
   HObjTable fParamsTable;
   HLocation fLoc;


public:

   HRichCalPar(const Char_t* name    = "RichCalPar",
               const Char_t* title   = "Rich Calibration Parameters",
               const Char_t* context = "RichPrimaryCalibration");
   ~HRichCalPar();

   Bool_t readline(const Char_t* buf);
   Bool_t init(HParIo* input, Int_t* set);
   Int_t  write(HParIo* output);
   void   clear();
   void   putAsciiHeader(TString& header);
   void   write(std::fstream& fout);
   void   printParams();

   HRichCalParCell* getSlot(HLocation &loc);
   HRichCalParCell* getObject(HLocation &loc);
   Bool_t setCell(Int_t sec, Int_t row, Int_t col,
                  Float_t offset, Float_t sigma);

   ClassDef(HRichCalPar, 1) //RICH calibration parameters class
};
#endif // HRICHCALPAR_H
