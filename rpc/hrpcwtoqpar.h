#ifndef HRPCWTOQPAR_H
#define HRPCWTOQPAR_H

#include "hparcond.h"

class HRpcWtoQPar : public HParCond {

	protected:
		Float_t fPar0;      
		Float_t fPar1;
		Float_t fPar2;
		Float_t fPar3;
		Float_t fPar4;
		Float_t fPar5;
		Float_t ftdc2time;   // TDC to Time conversion [ns/bin]

	public:
		HRpcWtoQPar(const char* name="RpcWtoQPar",
				const char* title="Width to Charge conversion parameters for the RPC detector",
				const char* context="RpcWtoQProduction");
		~HRpcWtoQPar() {;}

		Float_t getPar0()   { return fPar0; }
		Float_t getPar1()   { return fPar1; }
		Float_t getPar2()   { return fPar2; }
		Float_t getPar3()   { return fPar3; }
		Float_t getPar4()   { return fPar4; }
		Float_t getPar5()   { return fPar5; }
		Float_t getTdc2Time()   { return ftdc2time; }

		void fill(Float_t P0,Float_t P1,Float_t P2,Float_t P3,Float_t P4,Float_t P5,Float_t t2t) {

			fPar0 = P0;
			fPar1 = P1;
			fPar2 = P2;
			fPar3 = P3;
			fPar4 = P4;
			fPar5 = P5;
			ftdc2time = t2t;

		}

		void   clear();
		void   putParams(HParamList*);
		Bool_t getParams(HParamList*);

		ClassDef(HRpcWtoQPar,1) // Container for the RPC width to charge conversion parameters

};

#endif  /*!HRPCWTOQPAR_H*/
