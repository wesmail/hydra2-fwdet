#ifndef HRICH700RAW_H
#define HRICH700RAW_H

#include "TObject.h"



/*
 * a single tdc hit. Inspired by testbeam code from C.Pauly
 * JAM (j.adamczewski@gsi.de) 8-Jun-2017
 *
 */
class HRich700hit_t
{

public:
	HRich700hit_t(Double_t leading=0, Double_t trailing=0, Double_t tot=0, UInt_t flag=0):
		fLeadingEdgeTime(leading), fTrailingEdgeTime(trailing), fToT(tot), fFlag(flag){}
 	Double_t fLeadingEdgeTime;
 	Double_t fTrailingEdgeTime;
 	Double_t fToT;
 	UInt_t fFlag; //bit 0: valid rising edge, bit 1: valid falling edge
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
   Int_t   fPixel;           // local pixel id on the PMT
   std::vector <HRich700hit_t> fHitlist; // list of hits in this pixel during the event

public:
   HRich700Raw() : TObject(), fPMT(-1), fPixel(-1)
   	   {fHitlist.clear();}
   virtual ~HRich700Raw() {}

   Int_t   getMultiplicity()       const {return fHitlist.size();}
   Int_t   getPMT()             const {return fPMT;};
   Int_t   getPixel()              const {return fPixel;}
   const HRich700hit_t* getHit(const UInt_t n)      const {  return (n<fHitlist.size() ? &fHitlist[n] : 0 );}

   void    setPMT(Int_t p){fPMT=p;}
   void    setPixel(Int_t p){fPixel=p;}
   void    setAddress(const Int_t pmt, const Int_t pixel)
   {
	   setPMT(pmt);
	   setPixel(pixel);
   }

   void    getAddress(Int_t& pmt, Int_t& pix)
   {
      pmt = getPMT();
      pix = getPixel();
   }

   void addHit(Double_t leading, Double_t trailing, Double_t tot, UInt_t flag=3)
   {
	   fHitlist.push_back(HRich700hit_t(leading,trailing,tot,flag));
   }

   void clearHits()
   {
	   fHitlist.clear();
   }

   ClassDef(HRich700Raw, 1) // raw data of RICH700 detector using TRB3/dirich for readout
};



#endif /* ! HRich700Raw_H */
