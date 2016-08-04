#ifndef HMDCSLOPES__H
#define HMDCSLOPES__H

#include "TObject.h"

class HMdcSlopes : public TObject
{
 protected:
		Int_t sector;         // MdcSlopes container coordinate: sector
		Int_t module;         // MdcSlopes container coordinate: module
		Int_t mbo;            // MdcSlopes container coordinate: mbo
		Int_t tdc;            // MdcSlopes container coordinate: tdc (tdc*8 + channel)

		Float_t slope;        // value of slope
		Float_t error;        // error of the slope
		Int_t   slopeMethode; // method used to calculate slope

 public:
		HMdcSlopes(void);
		~HMdcSlopes(void);
		void getAddress(Int_t *s, Int_t *m, Int_t *mb, Int_t *t);
		Int_t getSector(void){return sector;};
		Int_t getModule(void){return module;};
		Int_t getMbo(void){return mbo;};
		Int_t getTdc(void){return tdc;};
		Float_t getSlope(void){return slope;};
		Float_t getError(void){return error;};
		Int_t getMethode(void){return slopeMethode;};
		void setAddress(Int_t s, Int_t m, Int_t mb, Int_t t);
		void setSector(Int_t s){sector = s;};
		void setModule(Int_t m){module = m;};
		void setMbo(Int_t mb){mbo = mb;};
		void setTdc(Int_t t){tdc = t;};
		void setSlope(Float_t sl){slope = sl;};
		void setError(Float_t er){error = er;};
		void setMethode(Int_t mth){slopeMethode = mth;};

		ClassDef(HMdcSlopes,1) // MDC slope
};

inline void HMdcSlopes::getAddress(Int_t *s, Int_t *m, Int_t *mb, Int_t *t)
{
		*s = sector;
		*m = module;
		*mb = mbo;
		*t = tdc;
}

inline void HMdcSlopes::setAddress(Int_t s, Int_t m, Int_t mb, Int_t t)
{
		sector = s;
		module = m;
		mbo = mb;
		tdc = t;
}


#endif
