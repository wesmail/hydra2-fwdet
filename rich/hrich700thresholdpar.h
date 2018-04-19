//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
// *-- Author  : JAM
// *-- Revised : Joern Adamczewski-Musch <j.adamczewski@gsi.de> 2017
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700ThresholdPar
//    Treestyle parameter container with trb3 time thresholds for rich700
//    Defined after example of hrichcalpar JAM Nov2017
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICH700THRESHOLDPAR_H
#define HRICH700THRESHOLDPAR_H

#include "hlocation.h"
#include "hobjtable.h"
#include "hparset.h"

#include "hrich700pixelthreshold.h"

class HParAsciiFileIo;

class HRich700ThresholdPar : public HParSet {

private:
   HObjTable fParamsTable;
   HLocation fLoc;


public:

   HRich700ThresholdPar(const Char_t* name    = "Rich700ThresholdPar",
               const Char_t* title   = "Rich700 Time Threshold Parameters",
               const Char_t* context = "RichPrimaryCalibration");
   ~HRich700ThresholdPar();

   Bool_t readline(const Char_t* buf);
   Bool_t init(HParIo* input, Int_t* set);
   Int_t  write(HParIo* output);
   void   clear();
   void   putAsciiHeader(TString& header);
   void   write(std::fstream& fout);
   void   printParams();

   HRich700PixelThreshold* getSlot(HLocation &loc);
   HRich700PixelThreshold* getObject(HLocation &loc);

   Bool_t setPixelThreshold(
		   Int_t pmt, Int_t pix,
		   Double_t tmin, Double_t tmax, Double_t totmin, Double_t totmax, Short_t flag);

   ClassDef(HRich700ThresholdPar, 1) //RICH calibration parameters class
};
#endif // HRICH700THRESHOLDPAR_H
