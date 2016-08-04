#ifndef HSTART2RAW_H
#define HSTART2RAW_H

#include "TObject.h"

class HStart2Raw : public TObject {
private:
   Int_t   fMultiplicity;    // number of hits
   Int_t   fModule;          // module number
   Int_t   fStrip;           // strip number
   Float_t fTime1;           // tdc time of 1st hit
   Float_t fWidth1;          // width of 1st hit
   Float_t fTime2;           // tdc time of 2nd hit
   Float_t fWidth2;          // width of 2nd hit
   Float_t fTime3;           // tdc time of 3rd hit
   Float_t fWidth3;          // width of 3rd hit
   Float_t fTime4;           // tdc time of 4th hit
   Float_t fWidth4;          // width of 4th hit

public:
   HStart2Raw(void) : fMultiplicity(0), fModule(-1), fStrip(-1),
      fTime1(-1000000.), fWidth1(-1000000.),
      fTime2(-1000000.), fWidth2(-1000000.),
      fTime3(-1000000.), fWidth3(-1000000.),
      fTime4(-1000000.), fWidth4(-1000000.)   {}
   ~HStart2Raw(void) {}

   Int_t   getMaxMultiplicity(void);
   Int_t   getMultiplicity(void)       const;
   Int_t   getModule(void)             const;
   Int_t   getStrip(void)              const;
   Float_t getTime(const Int_t n)      const;
   Float_t getWidth(const Int_t n)       const;
   void    getTimeAndWidth(const Int_t n, Float_t& time, Float_t& width);
   void    getAddress(Int_t& m, Int_t& s);

   void    setModule(Int_t m);
   void    setStrip(Int_t s);
   void    setAddress(const Int_t m, const Int_t s);
   Bool_t  setTimeAndWidth(const Float_t time, const Float_t width);

   ClassDef(HStart2Raw, 4) // raw data of START detector using TRB for readout
};


inline Int_t   HStart2Raw::getMaxMultiplicity(void)
{
   return 4;
}
inline Int_t   HStart2Raw::getMultiplicity(void) const
{
   return fMultiplicity;
}
inline Int_t   HStart2Raw::getModule(void) const
{
   return fModule;
}
inline Int_t   HStart2Raw::getStrip(void) const
{
   return fStrip;
}
inline void    HStart2Raw::getAddress(Int_t& m, Int_t& s)
{
   m = fModule;
   s = fStrip;
}

inline void    HStart2Raw::setModule(const Int_t m)
{
   fModule = m;
}
inline void    HStart2Raw::setStrip(const Int_t s)
{
   fStrip = s;
}
inline void    HStart2Raw::setAddress(const Int_t m, const Int_t s)
{
   fModule = m;
   fStrip = s;
}

#endif /* ! HSTART2RAW_H */
