//////////////////////////////////////////////////////////////////////////////
//
// @(#)hydraTrans/richNew:$Id: $
//*-- Author: Martin Jurkovic   2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichThresholdPar
//
//  Parameter container for threshold data.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef HRICHTHRESHOLDPAR_H
#define HRICHTHRESHOLDPAR_H

#include "hparset.h"

class HRichThresholdPar : public HParSet {

private:
   Int_t   fCalParVers;
   Float_t fSigmaMultiplier[6];

public:
   HRichThresholdPar(const Char_t* name   = "RichThresholdParameters",
                     const Char_t* title  = "Rich Threshold Parameters",
                     const Char_t* context = "");
   ~HRichThresholdPar() {}

   Bool_t  readline(const Char_t* buf);
   Bool_t  init(HParIo* input, Int_t* set);
   Int_t   write(HParIo* output);
   void    clear();
   void    putAsciiHeader(TString& header);
   void    write(std::fstream& fout);
   void    printParams();

   Int_t   getCalParVers();
   Float_t getSigmaMultiplier(Int_t sec);

   void    setCalParVers(Int_t i);
   void    setSigmaMultiplier(Int_t sec, Float_t multiply);

   ClassDef(HRichThresholdPar, 1) //Threshold parameters

};


inline Int_t   HRichThresholdPar::getCalParVers()
{
   return fCalParVers;
}
inline Float_t HRichThresholdPar::getSigmaMultiplier(Int_t sec)
{
   if (sec >= 0 && sec < 6) {
      return fSigmaMultiplier[sec];
   } else {
      Error("getSigmaMultiplier", "Wrong sector number (%d)", sec);
      return -1.;
   }
}

inline void    HRichThresholdPar::setCalParVers(Int_t i)
{
   fCalParVers = i;
}
inline void    HRichThresholdPar::setSigmaMultiplier(Int_t sec, Float_t multiply)
{
   if (sec >= 0 && sec < 6) {
      fSigmaMultiplier[sec] = multiply;
   } else {
      Error("setSigmaMultiplier", "Wrong sector number (%d)", sec);
   }
}

#endif // HRICHTHRESHOLDPAR_H
