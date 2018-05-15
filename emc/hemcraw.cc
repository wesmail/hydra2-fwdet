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

Double_t HEmcRaw::getFastTimeLeading(UInt_t n)
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fFastHits[n].getLeadingTime();
}

Double_t HEmcRaw::getSlowTimeLeading(UInt_t n)
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fSlowHits[n].getLeadingTime();
}

Double_t HEmcRaw::getFastTimeTrailing(UInt_t n)
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fFastHits[n].getTrailingTime();
}

Double_t HEmcRaw::getSlowTimeTrailing(UInt_t n)
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fSlowHits[n].getTrailingTime();
}

Double_t HEmcRaw::getFastWidth(UInt_t n)
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fFastHits[n].getToT();

}

Double_t HEmcRaw::getSlowWidth(UInt_t n)
{
    if (n > NTIMESEMCRAW -1)
        return -1.;
    return fSlowHits[n].getToT();
}

void HEmcRaw::getFastTimeAndWidth(UInt_t n, Double_t& t, Double_t& w)
{
    t = getFastTimeLeading(n);
    w = getFastWidth(n);
}

void HEmcRaw::getSlowTimeAndWidth(UInt_t n, Double_t& t, Double_t& w)
{
    t = getSlowTimeLeading(n);
    w = getSlowWidth(n);
}

