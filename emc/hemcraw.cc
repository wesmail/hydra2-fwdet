//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////////
//
//  HEmcRaw
//
//  Unpacked raw data of the EMC detector using the TRB3 for readout
//  modified by JAM (j.adamczewski@gsi.de) April 2018
//
/////////////////////////////////////////////////////////////////////////////

#include "hemcraw.h"

ClassImp(HEmcHit_t)
ClassImp(HEmcRaw)

Float_t HEmcRaw::getFastTimeLeading(UInt_t n) const 
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fFastHits[n].getLeadingTime();
}

Float_t HEmcRaw::getSlowTimeLeading(UInt_t n) const
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fSlowHits[n].getLeadingTime();
}

Float_t HEmcRaw::getFastTimeTrailing(UInt_t n) const
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fFastHits[n].getTrailingTime();
}

Float_t HEmcRaw::getSlowTimeTrailing(UInt_t n) const
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fSlowHits[n].getTrailingTime();
}

Float_t HEmcRaw::getFastWidth(UInt_t n) const
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fFastHits[n].getToT();

}

Float_t HEmcRaw::getSlowWidth(UInt_t n) const
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fSlowHits[n].getToT();
}

void HEmcRaw::getFastTimeAndWidth(UInt_t n, Float_t& t, Float_t& w) const
{
    t = getFastTimeLeading(n);
    w = getFastWidth(n);
}

void HEmcRaw::getSlowTimeAndWidth(UInt_t n, Float_t& t, Float_t& w) const
{
    t = getSlowTimeLeading(n);
    w = getSlowWidth(n);
}

