//*-- AUTHOR : Ilse Koenig
//*-- Modified : 18/06/99 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcCal2
//
// Data object for a Mdc cell containing calibrated distances to the wire
//
// In measurement mode the TDC accepts two hits per channel. The TDC can hereby
// trigger either on two leading edges (hit multiplicity nHits: -1 or -2) or
// on a leading and the trailing edge of a single pulse (nHits: +1 or +2).
//
// The addresses of the cell can be accessed via the inline functions
//     void setSector(const Int_t s)
//     void setModule(const Int_t m)
//     void setLayer(const Int_t l)
//     void setCell(const Int_t c)
//     void setAddress(const Int_t sector,const Int_t module,
//                     const Int_t layer,const Int_t cell)
//     Int_t getSector(void) const
//     Int_t getModule(void) const
//     Int_t getLayer(void) const
//     Int_t getCell(void) const
//     void getAddress(Int_t& sector,Int_t& module,Int_t& layer,Int_t& cell)
// the number of hits with the inline functions
//     void setNHits(const Int_t n)
//     Int_t getNHits(void) const
// and the distances and its errors with the inline functions
//     void setDist1(const Float_t dist1,const Float_t errDist1)
//     void setDist2(const Float_t dist2,const Float_t errDist2)
//     Float_t getDist1(void)
//     Float_t getDist2(void)
//     Float_t getErrDist1(void)
//     Float_t getErrDist2(void)
//
// The inline function clear() sets the data data members to the following
// values:
//         nHits=0;
//         sector=module=layer=cell=-1;
//         dist1=dist2=errDist1=errDist2=-99.F;
//
///////////////////////////////////////////////////////////////////////////////

#include "hmdccal2.h"

ClassImp(HMdcCal2)


