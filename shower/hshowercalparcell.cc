#pragma implementation
#include "hshowercalparcell.h"
#include "TClass.h"

ClassImp(HShowerCalParCell)

//*-- Author : Leszek Kidon
//*-- Created: 1998

//*-- Modified: Marcin Jaskula
//*-- Sat Sep  1 18:23:44 CEST 2001

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
// HShowerCalParCell
//
// HShowerCalParCell contains slope and offset parameters
// for calibration of one pad
//
//////////////////////////////////////////////////////////////////////

HShowerCalParCell::HShowerCalParCell() {
    reset();
}

// -----------------------------------------------------------------------------

HShowerCalParCell::HShowerCalParCell(Float_t fSlope, Float_t fOffset) {
    reset();
    setParams(fSlope, fOffset, 0.0, 1.0);
}

// -----------------------------------------------------------------------------

HShowerCalParCell::HShowerCalParCell(Float_t fSlope, Float_t fOffset,
                                    Float_t fThreshold, Float_t fGain)
{
    reset();
    setParams(fSlope, fOffset, fThreshold, fGain);
}

// -----------------------------------------------------------------------------

Int_t HShowerCalParCell::reset()
{
   setParams(1.0, 0.0, 0.0, 1.0);
   return 1;
}

// -----------------------------------------------------------------------------

void HShowerCalParCell::setParams(Float_t fSlope, Float_t fOffset) {
//set slope and offset
    setSlope(fSlope);
    setOffset(fOffset);
}

// -----------------------------------------------------------------------------

void HShowerCalParCell::setParams(Float_t fSlope, Float_t fOffset,
                        Float_t fThreshold, Float_t fGain)
{
    setSlope(fSlope);
    setOffset(fOffset);
    setThreshold(fThreshold);
    setGain(fGain);
}

// -----------------------------------------------------------------------------

void HShowerCalParCell::print(void)
{
    printf("S: %f  O: %f  T: %f  G: %f\n", getSlope(), getOffset(),
                    getThreshold(), getGain());
}
