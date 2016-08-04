#ifndef HRPCCLUSFPAR_H
#define HRPCCLUSFPAR_H

#include "hparcond.h"

class HRpcClusFPar : public HParCond {

	protected:
		Float_t fNsigmasX;    // Number of sigmas for position searching window
		Float_t fNsigmasT;    // Number of sigmas for time searching window

	public:
		HRpcClusFPar(const char* name="RpcClusFPar",
				const char* title="Cluster finder parameters for the Rpc Detector",
				const char* context="RpcClusFProduction");
		~HRpcClusFPar() {;}

		Float_t getNsigmasX()  { return fNsigmasX; }
		Float_t getNsigmasT()  { return fNsigmasT; }

		void fill(Float_t aNsigX,Float_t aNsigT) {

			fNsigmasX  = aNsigX;
			fNsigmasT  = aNsigT;

		}

		void   clear();
		void   putParams(HParamList*);
		Bool_t getParams(HParamList*);

		ClassDef(HRpcClusFPar,1) // Container for the RPC cluster finding parameters

};

#endif  /*!HRPCCLUSFPAR_H*/
