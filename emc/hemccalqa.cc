//*-- Author  : R. Lalik
//*-- Created : 07.05.2018
//*-- Modified: R. Lalik

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////
//
//  HEmcCalQA
//
//  Class for the calibrated EMC QA data
//
////////////////////////////////////////////////////

#include "hemccalqa.h"

ClassImp(HEmcCalQA)

void HEmcCalQA::clear(void) {
  cell      = 0;
  sector    = -1;
  row       = -1;
  column    = -1;
  nFast     = 0;
  nSlow     = 0;
  nMatched  = 0;
}

void HEmcCalQA::addFastHit(Float_t time, Float_t width)
{
  HEmcHitQA_t h;
  h.time = time;
  h.width = width;
  addFastHit(h);
}

void HEmcCalQA::addFastHit(const HEmcHitQA_t& h)
{
  if (nFast < NTIMESEMCRAW)
  {
    fast[nFast] = h;
    ++nFast;
  }
}

HEmcHitQA_t HEmcCalQA::getFastHit(UInt_t n) const
{
  if (n < nFast)
    return fast[n];
  else
    return HEmcHitQA_t();
}

void HEmcCalQA::addSlowHit(Float_t time, Float_t width)
{
  HEmcHitQA_t h;
  h.time = time;
  h.width = width;
  addSlowHit(h);
}

void HEmcCalQA::addSlowHit(const HEmcHitQA_t& h)
{
  if (nSlow < NTIMESEMCRAW)
  {
    slow[nSlow] = h;
    ++nSlow;
  }
}

HEmcHitQA_t HEmcCalQA::getSlowHit(UInt_t n) const
{
  if (n < nSlow)
    return slow[n];
  else
    return HEmcHitQA_t();
}

void HEmcCalQA::addMatchedHit(Float_t time, Float_t energy)
{
  HEmcHitMatchQA_t h;
  h.time = time;
  h.energy = energy;
  addMatchedHit(h);
}

void HEmcCalQA::addMatchedHit(const HEmcHitMatchQA_t& h)
{
  if (nMatched < NTIMESEMCRAW)
  {
    matched[nMatched] = h;
    ++nMatched;
  }
}

HEmcHitMatchQA_t HEmcCalQA::getMatchedHit(UInt_t n) const
{
  if (n < nMatched)
    return matched[n];
  else
    return HEmcHitMatchQA_t();
}
