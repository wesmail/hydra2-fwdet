//*****************************************************************************
// File: $RCSfile: $
//
// Author: Joern Wuestenfeld
//
// Version: $Revision $
// Modified by $Author $
//
//*****************************************************************************
#include "hmdcoepstatusdata.h"

ClassImp(HMdcOepStatusData)

    HMdcOepStatusData::HMdcOepStatusData(void)
{
    for(Int_t i = 0;i < 32; i++)
    {
	data[i] = 0;
    }
}

Int_t HMdcOepStatusData::getStatusWord(Int_t index)
{
    if((index < 0) || (index > 31))
	return -99;
    else
	return data[index];
}

Bool_t HMdcOepStatusData::setStatusWord(Int_t index,  UInt_t addr, UInt_t val)
{
    if((index < 0) || (index > 31))
	return kFALSE;
    else
    {
	address = addr;
	data[index] = val;
    }
    return kTRUE;
}
