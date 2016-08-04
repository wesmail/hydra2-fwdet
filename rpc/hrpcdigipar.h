#ifndef HRPCDIGIPAR_H
#define HRPCDIGIPAR_H

#include "hparcond.h"

class HRpcDigiPar : public HParCond {

	protected:
		Float_t fVprop;    // signal propagation velocity [mm/ns]
		Float_t fS_x;      // position resolution [mm]
		Float_t fS0_time;  // time resolution par0 [ps]
		Float_t fS1_time;  // time resolution par1 [ps]
		Float_t fS2_time;  // time resolution par2 (pure number)
		Float_t fS3_time;  // time resolution par3 (pure number)
		Float_t fT_off;    // time offset [ps]
		Float_t fQmean0;   // 4-gap average charge [pC]
		Float_t fQmean1;   // 4-gap average charge [pC]
		Float_t fQmean2;   // 4-gap average charge [pC]
		Float_t fQwid0;    // 4-gap width charge [pC]
		Float_t fQwid1;    // 4-gap width charge [pC]
		Float_t fQwid2;    // 4-gap width charge [pC]
		Float_t fEff0;     // 4-gap efficiency par0 
		Float_t fEff1;     // 4-gap efficiency par1
		Float_t fEff2;     // 4-gap efficiency par2 (pure number)
		Float_t fEff3;     // 4-gap efficiency par3 (pure number)
		Float_t fEff4;     // 4-gap efficiency par4 (pure number)
		Float_t fEff5;     // 4-gap efficiency par5 (pure number)
		Float_t fTime2Tdc; // Time (ns) to TDC bin conversion (1/0.098)
		Float_t fPedestal; // Mean Charge Pedestal [ns]
		Float_t fQtoW0;    // First parameter for charge to width conversion
		Float_t fQtoW1;    // Second parameter for charge to width conversion
		Float_t fQtoW2;    // Third parameter for charge to width conversion
		Float_t fQtoW3;    // Fourth parameter for charge to width conversion
		Float_t fGap;      // Gas gap size [mm]
		Int_t   fMode;     // Digitizing Mode type
		                   // see http://hades-wiki.gsi.de/cgi-bin/view/RPC/RpcDigiPar
                                   // for the old version.

	public:
		HRpcDigiPar(const char* name="RpcDigiPar",
				const char* title="RPC digitization parameters",
				const char* context="RpcDigiProduction");
		~HRpcDigiPar() {;}

		Float_t getVprop()    { return fVprop; }
		Float_t getSigmaX()   { return fS_x; }
		Float_t getSigmaT()   { return fS0_time; }
		Float_t getSigmaT1()  { return fS1_time; }
		Float_t getSigmaT2()  { return fS2_time; }
		Float_t getSigmaT3()  { return fS3_time; }
		Float_t getToff()     { return fT_off; }
		Float_t getQmean()    { return fQmean0; }
		Float_t getQmean1()   { return fQmean1; }
		Float_t getQmean2()   { return fQmean2; }
		Float_t getQwid()     { return fQwid0; }
		Float_t getQwid1()    { return fQwid1; }
		Float_t getQwid2()    { return fQwid2; }
		Float_t getEff()      { return fEff0; }
		Float_t getEff1()     { return fEff1; }
		Float_t getEff2()     { return fEff2; }
		Float_t getEff3()     { return fEff3; }
		Float_t getEff4()     { return fEff4; }
		Float_t getEff5()     { return fEff5; }
		Float_t getTime2Tdc() { return fTime2Tdc; }
		Float_t getPedestal() { return fPedestal; }
		Float_t getQtoW0()    { return fQtoW0; }
		Float_t getQtoW1()    { return fQtoW1; }
		Float_t getQtoW2()    { return fQtoW2; }
		Float_t getQtoW3()    { return fQtoW3; }
		Float_t getGap()      { return fGap; }
		Int_t   getMode()     { return fMode; }

		void fill(Float_t aVprop ,Float_t aSx    ,Float_t aSt0   ,Float_t aSt1   ,
                          Float_t aSt2   ,Float_t aSt3   ,Float_t aToff  ,Float_t aQmean0,
                          Float_t aQmean1,Float_t aQmean2,Float_t aQwid0 ,Float_t aQwid1 ,
                          Float_t aQwid2 ,Float_t aEff0  ,Float_t aEff1  ,Float_t aEff2  ,
			  Float_t aEff3  ,Float_t aEff4  ,Float_t aEff5  ,Float_t aT2T   ,
			  Float_t aPes   ,Float_t a0     ,Float_t a1     ,Float_t a2     ,
			  Float_t a3     ,Float_t gap    ,Int_t   aMode) {

			fVprop    = aVprop;
			fS_x      = aSx;
			fS0_time  = aSt0;
			fS1_time  = aSt1;
			fS2_time  = aSt2;
			fS3_time  = aSt3;
			fT_off    = aToff;
			fQmean0   = aQmean0;
			fQmean1   = aQmean1;
			fQmean2   = aQmean2;
			fQwid0    = aQwid0;
			fQwid1    = aQwid1;
			fQwid2    = aQwid2;
			fEff0     = aEff0;
			fEff1     = aEff1;
			fEff2     = aEff2;
			fEff3     = aEff3;
			fEff4     = aEff4;
			fEff5     = aEff5;
			fTime2Tdc = aT2T;
			fPedestal = aPes;
			fQtoW0    = a0;
			fQtoW1    = a1;
			fQtoW2    = a2;
			fQtoW3    = a3;
			fGap      = gap;
                        fMode     = aMode;
		}

		void   clear();
		void   putParams(HParamList*);
		Bool_t getParams(HParamList*);

		ClassDef(HRpcDigiPar,3) // Container for the RPC digitization parameters

};

#endif  /*!HRPCDIGIPAR_H*/
