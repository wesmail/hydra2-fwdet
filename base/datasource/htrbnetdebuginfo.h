///////////////////////////////////////////////////////////
// File: $RCSfile: htrbnetdebuginfo.h,v $
//
// Author : J.Wuestenfeld
// Version: $Revision: 1.1.1.1 $
///////////////////////////////////////////////////////////

#ifndef HTRBNETDEBUGINFO__H
#define HTRBNETDEBUGINFO__H

#include "TObject.h"

class HTrbNetDebugInfo : public TObject
{
protected:
    UInt_t address;	      	//< Address of TrbNet entity
    UInt_t statusWord;		//< Status of TrbNet entity

public:
   HTrbNetDebugInfo(void);
   ~HTrbNetDebugInfo(void);

   UInt_t getAddress   (void)    {return address;};
   UInt_t getStatusWord(void)    {return statusWord;};
   UInt_t getStatusBit (char bit){return (statusWord & (0x1 << bit)) >> bit;};

   void setAddress(UInt_t addr){address = addr;};
   void setStatus (UInt_t val) {statusWord = val;};

   ClassDef(HTrbNetDebugInfo,1)
};

#endif
