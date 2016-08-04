//*****************************************************************************
// File: $RCSfile: htrbnetdebuginfo.cc,v $
//
// Author: Joern Wuestenfeld
//
// Version: $Revision $
// Modified by $Author $
//
//
//*****************************************************************************
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
// HTrbNetUnpacker
//
//
//*****************************************************************************

#include "htrbnetdebuginfo.h"

ClassImp(HTrbNetDebugInfo)

HTrbNetDebugInfo::HTrbNetDebugInfo(void)
{
	address = 0;
	statusWord = 0;
}

HTrbNetDebugInfo::~HTrbNetDebugInfo(void)
{
}
