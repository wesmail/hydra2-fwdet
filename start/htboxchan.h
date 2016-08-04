// File: htboxchan.h
//
// Author: Rainer Schicker
// created: 2/3/00
// Modified: 21/11/2001 D.Zovinec
// Modified: 03/09/2004 T.Wojcik
//
#ifndef HTBOXCHAN_H
#define HTBOXCHAN_H

#include "TObject.h"

class HTBoxChan : public TObject {

private:

   Int_t fChannel;     // channel number
   UInt_t fScaler;      // scalers data

public:
   HTBoxChan(void) : fChannel(-1), fScaler(0) {}
   ~HTBoxChan(void) {}
   void setScalerData(const Int_t s, const UInt_t i);
   void getScalerData(Int_t& ch, UInt_t& s);
   Int_t getChannel(void)   { return fChannel; }

   ClassDef(HTBoxChan, 1) // Trigger Box data
};


inline void HTBoxChan::setScalerData(const Int_t ch, const UInt_t s)
{
   fChannel = ch;
   fScaler = s;
}
inline void HTBoxChan::getScalerData(Int_t& ch, UInt_t& s)
{
   ch = fChannel;
   s  = fScaler;
}

#endif /* ! HTBoxChan_H */






