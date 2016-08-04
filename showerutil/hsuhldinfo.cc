#pragma implementation

// -----------------------------------------------------------------------------

#include "hsuhldinfo.h"
#include "hldfilevt.h"
#include "hevent.h"
#include "hrecevent.h"

// -----------------------------------------------------------------------------

ClassImp(HSUHldFileInfo);

// -----------------------------------------------------------------------------

HSUHldFileInfo::HSUHldFileInfo(const Text_t *psName, Bool_t bAutoProceed)
                : HldFileSource()
{
Char_t *p;

    if((p = (Char_t *)strrchr(psName,'/')) == NULL)
    {
        setDirectory(".");
        addFile(psName);
    }
    else
    {
        *p = '\0';
        setDirectory(psName);
        addFile(p + 1);
        *p = '/';
    }

    m_ppEvent = NULL;
    setEventAddress(&m_ppEvent);

    init();

    if(bAutoProceed)
        proceed();
}

// -----------------------------------------------------------------------------

void HSUHldFileInfo::print(void)
{
    printf("File  : %s\nRunId : %d\nEvents: %d\n",
            getCurrentFileName(), getCurrentRunId(), fEventNr);
}

// -----------------------------------------------------------------------------

void HSUHldFileInfo::proceed(void)
{
    for( ; ; )
    {
        if((isDumped) && (dumpEvt() == kDsError))
            break;

        if((isScanned) && (scanEvt() == kDsError))
            break;

        if( ! ((HldFilEvt*)fReadEvent)->execute())
            break;

        fEventNr++;
        decodeHeader((*fEventAddr)->getHeader());
    }
}
