#ifndef HRICH700RAW_H
#define HRICH700RAW_H

#include "TObject.h"

#define  NMAXRAWRICH 10

/*
 * a single tdc hit. Inspired by testbeam code from C.Pauly
 * JAM (j.adamczewski@gsi.de) 8-Jun-2017
 *
 */
class HRich700hit_t : public TObject
{

public:
	HRich700hit_t(Double_t leading=0, Double_t trailing=0, Double_t tot=0, UInt_t flag=0):
		fLeadingEdgeTime(leading), fTrailingEdgeTime(trailing), fToT(tot), fFlag(flag){}
 	Double_t fLeadingEdgeTime;
 	Double_t fTrailingEdgeTime;
 	Double_t fToT;
 	UInt_t fFlag; //bit 0: valid rising edge, bit 1: valid falling edge
	void set(Double_t leading,Double_t trailing,Double_t tot,UInt_t flag) {
	    fLeadingEdgeTime  = leading;
	    fTrailingEdgeTime = trailing;
	    fToT              = tot;
	    fFlag             = flag;
	}
	void clear(){
	    fLeadingEdgeTime  = 0;
	    fTrailingEdgeTime = 0;
	    fToT              = 0;
	    fFlag             = 0;
	}
      ClassDef(HRich700hit_t, 1)

};


/*
 * raw data of a single rich700 pixel. Is kept in rich700raw category.
 * contains the list of tdc hits during the event.
 * JAM (j.adamczewski@gsi.de) 8-Jun-2017
 *
 */

class HRich700Raw : public TObject {
private:
   Int_t   fPMT;        // global RICH700 PMT id
   Int_t   fPixel;      // local pixel id on the PMT
   Int_t   fSector;
   Int_t   fCol;
   Int_t   fRow;
   HRich700hit_t fHitlist[NMAXRAWRICH]; // list of hits in this pixel during the event
   UInt_t   fnHits;
public:
   HRich700Raw() : TObject(), fPMT(-1), fPixel(-1) ,fSector(-1),fCol(-1),fRow(-1)
    {
	clearHits();
    }
   virtual ~HRich700Raw() {}

   Int_t   getMultiplicity()    const {return fnHits;}
   Int_t   getPMT()             const {return fPMT;};
   Int_t   getPixel()           const {return fPixel;}
   const HRich700hit_t* getHit(const UInt_t n)  const {  return (n<fnHits ? &fHitlist[n] : 0 );}

   void    setPMT(Int_t p)   {fPMT=p;}
   void    setPixel(Int_t p) {fPixel=p;}
   void    setSector(Int_t s){fSector = s;}
   void    setCol(Int_t c){fCol = c;}
   void    setRow(Int_t r){fRow = r;}
   void    setAddress(const Int_t pmt, const Int_t pixel, const Int_t sec, const Int_t col, const Int_t row)
   {
	   fPMT   = pmt;
	   fPixel = pixel;
	   fSector= sec;
	   fCol   = col;
	   fRow   = row;
   }

   void    getAddress(Int_t& pmt, Int_t& pix, Int_t& sec,Int_t& col,Int_t& row)
   {
      pmt = fPMT;
      pix = fPixel;
      sec = fSector;
      col = fCol;
      row = fRow;
   }

   void addHit(Double_t leading, Double_t trailing, Double_t tot, UInt_t flag=3)
   {
       if(fnHits<NMAXRAWRICH-1) {
	   fHitlist[fnHits].set(leading,trailing,tot,flag);
           fnHits++;
       }
   }

   void clearHits()
   {
       for(Int_t i=0;i<NMAXRAWRICH; i++) fHitlist [i].clear();
       fnHits = 0 ;
   }

   ClassDef(HRich700Raw, 1) // raw data of RICH700 detector using TRB3/dirich for readout
};



#endif /* ! HRich700Raw_H */
