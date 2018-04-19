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
//  HRich700PixelThreshold
//    Contains trb3 time thresholds for single pixel of rich700
//    Defined after example of hrichcalparcell JAM Nov2017
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICH700PIXELTHRESHOLD_H
#define HRICH700PIXELTHRESHOLD_H

#include "hlocateddataobject.h"

class HRich700PixelThreshold : public HLocatedDataObject {

private:

   Short_t fPMT;
   Short_t fPixel;

   Double_t fTMin;
   Double_t fTMax;
   Double_t fTotMin;
   Double_t fTotMax;
   Short_t fFlag;


public:

   HRich700PixelThreshold();
   HRich700PixelThreshold(Double_t tmin, Double_t tmax, Double_t totmin, Double_t totmax, Short_t flag);
   virtual ~HRich700PixelThreshold() {}




///////////////////////////////////////////////////////////////////
// GETTERS
	Double_t getT_Min() const {
		return fTMin;
	}
	Double_t getT_Max() const {
		return fTMax;
	}
	Double_t getTot_Min() const {
		return fTotMin;
	}
	Double_t getTot_Max() const {
		return fTotMax;
	}
	Short_t getFlag() const {
		return fFlag;
	}

	Short_t getPMT() const {
		return fPMT;
	}
	Short_t getPixel() const {
		return fPixel;
	}

	Int_t getNLocationIndex(void);
	Int_t getLocationIndex(Int_t i);


///////////////////////////////////////////////////////////////////
// SETTERS

    void setT_Min(Double_t t) { fTMin=t;}
     void setT_Max(Double_t t)  { fTMax=t;}
    void setTot_Min(Double_t t) { fTotMin=t;}
    void setTot_Max(Double_t t)  { fTotMax=t;}
    void setFlag(Short_t f)  { fFlag=f;}

    void setPMT(Short_t p) { fPMT=p;}
    void setPixel(Short_t p) { fPixel=p;}

    void setParams(Double_t tmin=0, Double_t tmax=1.0e5, Double_t totmin=0, Double_t totmax=1.0e4, Short_t flag=-1)
    {
    	setT_Min(tmin);
    	setT_Max(tmax);
    	setTot_Min(totmin);
    	setTot_Max(totmax);
    	setFlag(flag);
    }

    void reset(){setParams();}

   ClassDef(HRich700PixelThreshold, 1) // Time threshold data of a single pixel
};







#endif // HRICH700PIXELTHRESHOLD_H







