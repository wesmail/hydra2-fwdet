// Last update: 21.04.06 --- B.Spruck
// Modified for Wall by M.Golubeva 01.11.2006

#ifndef HWALLCAL_H
#define HWALLCAL_H

#include "TObject.h"
#include "hwallraw.h"

class HWallCal : public TObject {
 protected:
  Int_t nHits;  // number of hits
  Int_t cell;  // fiber number
  Float_t time1;  // tdc time of 1st hit
  Float_t adc1;   // adc of 1st hit
  Float_t time2;  // tdc time of 2nd hit
  Float_t adc2;   // adc of 2nd hit
  Float_t time3;  // tdc time of 3rd hit
  Float_t adc3;   // adc of 3rd hit
  Float_t time4;  // tdc time of 4th hit
  Float_t adc4;   // adc of 4th hit
  
 public:
  HWallCal(void) { clear(); }
  ~HWallCal(void) {;}
 
 void clear(void);
 
 Int_t getNHits(void) const { return nHits; }
 Int_t getCell(void) const { return cell; }

 Float_t getTime(const Int_t n) const;
 Float_t getAdc(const Int_t n) const;
 void getTimeAndAdc(const Int_t n, Float_t& time, Float_t& adc );
 inline Int_t getMaxMult(void){ return(4);}// return number of multiplicity supported (constant)
 
 void setCell(const Int_t n)    { cell=n; } 
 Bool_t setTimeAdc(const Float_t t,const Float_t a);
 
 
 ClassDef(HWallCal,1)  // WALL detector cal data
};
   
#endif /* ! HWALLCAL_H */
