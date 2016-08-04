///////////////////////////////////////////////////////////
// File: $RCSfile: $
//
// Author : J.Wuestenfeld
// Version: $Revision: $
///////////////////////////////////////////////////////////
#ifndef HMDCOEPSTATUSDATA__H
#define HMDCOEPSTATUSDATA__H

#include "TObject.h"

/*
		HMdcOepStatusData,
		
		Stores each of the MDC OEP status words in one 32 bit integer.
		No decoding mapping etc. done.
*/
class HMdcOepStatusData : public TObject
{
	protected:
		Int_t address;		// Address of OEP (module, sector, mbo)
		Int_t data[32];		// 32 words of status data
	public:
		HMdcOepStatusData(void);
		~HMdcOepStatusData(void){;};

		Int_t  getStatusWord(Int_t index);
		UInt_t getAddress(void){return address;};
		UInt_t getSector(void){return (address >> 4) & 0xf;};
		UInt_t getModule(void){return (address >> 8) & 0xf;};
		UInt_t getMbo(void){return address & 0xf;};

		Bool_t setStatusWord(Int_t index, UInt_t addr, UInt_t val);

	ClassDef(HMdcOepStatusData,1)
};

#endif
