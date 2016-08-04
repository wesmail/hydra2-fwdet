#ifndef HSTART2CAL_H
#define HSTART2CAL_H

#include "TObject.h"

class HStart2Cal : public TObject {
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
   HStart2Cal(void)  : fMultiplicity(0), fModule(-1), fStrip(-1),
      fTime1(-1000000.), fWidth1(-1000000.),
      fTime2(-1000000.), fWidth2(-1000000.),
      fTime3(-1000000.), fWidth3(-1000000.),
      fTime4(-1000000.), fWidth4(-1000000.)   {}
   ~HStart2Cal(void) {}

   Int_t   getMaxMultiplicity(void);
   Int_t   getMultiplicity(void)     const;
   Int_t   getModule(void)           const;
   Int_t   getStrip(void)            const;
   Float_t getTime(const Int_t n)    const;
   Float_t getWidth(const Int_t n)     const;
   void    getAddress(Int_t& m, Int_t& s);
   void    getTimeAndWidth(const Int_t n, Float_t& time, Float_t& width);

   void    setModule(const Int_t m);
   void    setStrip(const Int_t s);
   void    setAddress(const Int_t m, const Int_t s);
   Bool_t  setTimeAndWidth(const Float_t time, const Float_t width);

   ClassDef(HStart2Cal, 4) // START2 detector cal data
};

inline Int_t HStart2Cal::getMaxMultiplicity(void)
{
   return(4);
}
inline Int_t HStart2Cal::getMultiplicity(void) const
{
   return fMultiplicity;
}
inline Int_t HStart2Cal::getModule(void)       const
{
   return fModule;
}
inline Int_t HStart2Cal::getStrip(void)        const
{
   return fStrip;
}
inline void  HStart2Cal::getAddress(Int_t& m, Int_t& s)
{
   m = fModule;
   s = fStrip;
}

inline void  HStart2Cal::setModule(const Int_t m)
{
   fModule = m;
}
inline void  HStart2Cal::setStrip(const Int_t s)
{
   fStrip = s;
}
inline void  HStart2Cal::setAddress(const Int_t m, const Int_t s)
{
   fModule = m;
   fStrip = s;
}

#endif /* ! HSTART2CAL_H */
