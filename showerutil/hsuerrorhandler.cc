// @(#)$Id: hsuerrorhandler.cc,v 1.4 2009-07-03 12:16:14 halo Exp $
//*-- Author : Marcin Jaskula 28/11/2002

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HSUErrorHandler                                                            //
//                                                                            //
// Color error handler                                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "hsuerrorhandler.h"
#include "TSystem.h"
#include <cstdlib>

// -----------------------------------------------------------------------------

Int_t HSUErrorHandler::iColors[kIdxMax] = {
        HSUErrorHandler::kDefaultColor,
        93,
        91,
        HSUErrorHandler::kDefaultColor,
        HSUErrorHandler::kDefaultColor
};

// -----------------------------------------------------------------------------

void HSUErrorHandler::setHandler(Bool_t bHSUErrorHandler)
{
    SetErrorHandler((bHSUErrorHandler)
            ? HSUErrorHandler::printError : DefaultErrorHandler);
}

// -----------------------------------------------------------------------------

void HSUErrorHandler::setColor(Int_t iIndex, Int_t iColor)
{
    if((iIndex < 0) || (iIndex >= kIdxMax))
    {
        ::Error("HSUErrorHandler::setColor", "Index out of bounds: %d", iIndex);
        return;
    }

    if(iColor < 0)
        iColor = kDefaultColor;
    else
    {
        if(iColor >= 100)
        {
            iColor %= 100;

            ::Warning("HSUErrorHandler::setColor", "Color truncated to %d",
                            iColor);
        }
    }

    iColors[iIndex] = iColor;
}

// -----------------------------------------------------------------------------

void HSUErrorHandler::printError(Int_t level, Bool_t abort, const Char_t *location,
                          const Char_t *msg)
{
    if (level < gErrorIgnoreLevel)
        return;

const Char_t *type   = 0;
Int_t       iColor = iColors[level / 1000];

    if (level >= kInfo)
        type = "Info";
    if (level >= kWarning)
        type = "Warning";
    if (level >= kError)
        type = "Error";
    if (level >= kSysError)
        type = "SysError";
    if (level >= kFatal)
        type = "Fatal";

    if(iColor != HSUErrorHandler::kDefaultColor)
        HSUErrorHandler::setTermColor(iColor);

    if (!location || strlen(location) == 0)
        fprintf(stderr, "%s: %s", type, msg);
    else
        fprintf(stderr, "%s in <%s>: %s", type, location, msg);

    if(iColor != HSUErrorHandler::kDefaultColor)
        HSUErrorHandler::resetTermColor();

    fprintf(stderr, "\n");

    fflush(stderr);
    if (abort)
    {
        fprintf(stderr, "aborting\n");
        fflush(stderr);

        if (gSystem)
        {
            gSystem->StackTrace();
            gSystem->Abort();
        }
        else
            ::abort();
    }
}

// -----------------------------------------------------------------------------

void HSUErrorHandler::setTermColor(Int_t iBackground, Int_t iForegorund)
{
    fprintf(stderr, "%s%dm%s%dm", "[3", iForegorund, "[4", iBackground);
    fflush(stderr);
}
