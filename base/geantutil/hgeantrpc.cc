//*-- Author   : 08/06/2006 D. Gonzalez-Diaz
//*-- Modified : 10/08/2007 D. Gonzalez-Diaz
//*-- Modified : 27/11/2012 A.Mangiarotti Added special get function
//*-- Modified :            getHitDigi optimised for the digitizer.
//*-- Modified : 09/03/2013 A.Mangiarotti Restructured to reorganise all
//*-- Modified :            the four gaps into a single cell wise object
//*-- Modified :            without loss of important information but
//*-- Modified :            with an overall reduction in size. Part of
//*-- Modified :            the structure is dictated by the requirement
//*-- Modified :            of being able to correctly read old
//*-- Modified :            simulation files. For the same reason of
//*-- Modified :            backward compatibility the addressing scheme
//*-- Modified :            sector-cell-column-gap has not been changed.
//*-- Modified :            In the new cell wise object the gap is always
//*-- Modified :            zero. Access methods reorganised to provide
//*-- Modified :            more options to retrieve only the information
//*-- Modified :            needed.
////////////////////////////////////////////////////////////////////////////
//  HGeantRpc
//  GEANT RPC hit data
////////////////////////////////////////////////////////////////////////////

#include "hgeantrpc.h"

#include "TBuffer.h"

ClassImp(HGeantRpc)

HGeantRpc::HGeantRpc(void) {
  trackNumber       = 0;
  trackLength       = 0.0;
  loctrackLength    = 0.0;
  eHit              = 0.0;
  xHit              = 0.0;
  yHit              = 0.0;
  zHit              = 0.0;
  tofHit            = 0.0;
  momHit            = 0.0;
  thetaHit          = 0.0;
  phiHit            = 0.0;
  detectorID        = -999;
  // New after Jan 2013.
  loctrackLength1   = 0.0;
  eHit1             = 0.0;
  xHit1             = 0.0;
  yHit1             = 0.0;
  momHit1           = 0.0;
  loctrackLength2   = 0.0;
  eHit2             = 0.0;
  xHit2             = 0.0;
  yHit2             = 0.0;
  momHit2           = 0.0;
  loctrackLength3   = 0.0;
  eHit3             = 0.0;
  xHit3             = 0.0;
  yHit3             = 0.0;
  momHit3           = 0.0;
  HGeantRpc_version = -999;
}
/*
HGeantRpc::HGeantRpc(HGeantRpc &aRpc) {
  trackNumber       = aRpc.trackNumber;
  trackLength       = aRpc.trackLength;
  loctrackLength    = aRpc.loctrackLength;
  eHit              = aRpc.eHit;
  xHit              = aRpc.xHit;
  yHit              = aRpc.yHit;
  zHit              = aRpc.zHit;
  tofHit            = aRpc.tofHit;
  momHit            = aRpc.momHit;
  thetaHit          = aRpc.thetaHit;
  phiHit            = aRpc.phiHit;
  detectorID        = aRpc.detectorID;
  // New after Jan 2013.
  loctrackLength1   = aRpc.loctrackLength1;
  eHit1             = aRpc.eHit1;
  xHit1             = aRpc.xHit1;
  yHit1             = aRpc.yHit1;
  momHit1           = aRpc.momHit1;
  loctrackLength2   = aRpc.loctrackLength2;
  eHit2             = aRpc.eHit2;
  xHit2             = aRpc.xHit2;
  yHit2             = aRpc.yHit2;
  momHit2           = aRpc.momHit2;
  loctrackLength3   = aRpc.loctrackLength3;
  eHit3             = aRpc.eHit3;
  xHit3             = aRpc.xHit3;
  yHit3             = aRpc.yHit3;
  momHit3           = aRpc.momHit3;
  HGeantRpc_version = aRpc.HGeantRpc_version;
}
*/
HGeantRpc::~HGeantRpc(void) {
}

void HGeantRpc::setIncidence(Float_t athetaHit, Float_t aphiHit)
{
  thetaHit = athetaHit;
  phiHit   = aphiHit;
}

void HGeantRpc:: getIncidence(Float_t& athetaHit, Float_t& aphiHit)
{
  athetaHit = thetaHit;
  aphiHit   = phiHit;
}

void HGeantRpc:: setTLength(Float_t atrackLength, Float_t aloctrackLength)
{
  trackLength    = atrackLength;
  loctrackLength = aloctrackLength;
}

void HGeantRpc:: getTLength(Float_t& atrackLength, Float_t& aloctrackLength)
{
  atrackLength    = trackLength;
  aloctrackLength = loctrackLength;
}

void HGeantRpc::setHit(Float_t axHit, Float_t ayHit, Float_t azHit,
                       Float_t atofHit, Float_t amomHit, Float_t aeHit,
                       Float_t aloctrackLength)
{
  eHit           = aeHit;
  xHit           = axHit;
  yHit           = ayHit;
  zHit           = azHit;
  tofHit         = atofHit;
  momHit         = amomHit;
  loctrackLength = aloctrackLength;
}

void HGeantRpc::setHit(Float_t axHit, Float_t ayHit, Float_t azHit,
                       Float_t atofHit, Float_t amomHit, Float_t aeHit)
{
  eHit   = aeHit;
  xHit   = axHit;
  yHit   = ayHit;
  zHit   = azHit;
  tofHit = atofHit;
  momHit = amomHit;
}

void HGeantRpc:: getHit(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                        Float_t& atofHit, Float_t& amomHit, Float_t& aeHit)
{
  aeHit   = eHit;
  axHit   = xHit;
  ayHit   = yHit;
  azHit   = zHit;
  atofHit = tofHit;
  amomHit = momHit;
}

Float_t HGeantRpc::getlocTLengthGap(Int_t nGap)
{
  if(nGap==0) {
    return loctrackLength;
  } else if(nGap==1) {
    return loctrackLength1;
  } else if(nGap==2) {
    return loctrackLength2;
  } else if(nGap==3) {
    return loctrackLength3;
  }
  return 0.0;
}

void HGeantRpc::setGap(Int_t nGap, Float_t axHit, Float_t ayHit, Float_t amomHit,
                       Float_t aeHit, Float_t aloctrackLength)
{
  if(nGap==0) {
    loctrackLength  = aloctrackLength;
    eHit            = aeHit;
    xHit            = axHit;
    yHit            = ayHit;
    momHit          = amomHit;
  } else if(nGap==1) {
    xHit1           = axHit;
    yHit1           = ayHit;
    momHit1         = amomHit;
    eHit1           = aeHit;
    loctrackLength1 = aloctrackLength;
  } else if(nGap==2) {
    loctrackLength2 = aloctrackLength;
    eHit2           = aeHit;
    xHit2           = axHit;
    yHit2           = ayHit;
    momHit2         = amomHit;
  } else if(nGap==3) {
    loctrackLength3 = aloctrackLength;
    eHit3           = aeHit;
    xHit3           = axHit;
    yHit3           = ayHit;
    momHit3         = amomHit;
  }
}

void HGeantRpc::getGap(Int_t nGap, Float_t& axHit, Float_t& ayHit, Float_t& amomHit,
                       Float_t& aeHit, Float_t& aloctrackLength)
{
  if(nGap==0) {
    aloctrackLength = loctrackLength;
    aeHit           = eHit;
    axHit           = xHit;
    ayHit           = yHit;
    amomHit         = momHit;
  } else if(nGap==1) {
    aloctrackLength = loctrackLength1;
    aeHit           = eHit1;
    axHit           = xHit1;
    ayHit           = yHit1;
    amomHit         = momHit1;
  } else if(nGap==2) {
    aloctrackLength = loctrackLength2;
    aeHit           = eHit2;
    axHit           = xHit2;
    ayHit           = yHit2;
    amomHit         = momHit2;
  } else if(nGap==3) {
    aloctrackLength = loctrackLength3;
    aeHit           = eHit3;
    axHit           = xHit3;
    ayHit           = yHit3;
    amomHit         = momHit3;
  }
}

void HGeantRpc::getGap(Int_t nGap, Float_t& axHit, Float_t& ayHit, Float_t& amomHit,
                       Float_t& aeHit)
{
  if(nGap==0) {
    aeHit   = eHit;
    axHit   = xHit;
    ayHit   = yHit;
    amomHit = momHit;
  } else if(nGap==1) {
    aeHit   = eHit1;
    axHit   = xHit1;
    ayHit   = yHit1;
    amomHit = momHit1;
  } else if(nGap==2) {
    aeHit   = eHit2;
    axHit   = xHit2;
    ayHit   = yHit2;
    amomHit = momHit2;
  } else if(nGap==3) {
    aeHit   = eHit3;
    axHit   = xHit3;
    ayHit   = yHit3;
    amomHit = momHit3;
  }
}

void HGeantRpc::getGap(Int_t nGap, Float_t& axHit, Float_t& ayHit, Float_t& amomHit)
{
  if(nGap==0) {
    axHit   = xHit;
    ayHit   = yHit;
    amomHit = momHit;
  } else if(nGap==1) {
    axHit   = xHit1;
    ayHit   = yHit1;
    amomHit = momHit1;
  } else if(nGap==2) {
    axHit   = xHit2;
    ayHit   = yHit2;
    amomHit = momHit2;
  } else if(nGap==3) {
    axHit   = xHit3;
    ayHit   = yHit3;
    amomHit = momHit3;
  }
}

void HGeantRpc::getHit(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                       Float_t& atofHit, Float_t& amomHit, Float_t& aeHit,
                       Float_t& aloctrackLength)
{
  aloctrackLength = loctrackLength;
  aeHit           = eHit;
  axHit           = xHit;
  ayHit           = yHit;
  azHit           = zHit;
  atofHit         = tofHit;
  amomHit         = momHit;
}

void HGeantRpc::getHit(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                       Float_t& atofHit, Float_t& amomHit)
{
  axHit   = xHit;
  ayHit   = yHit;
  azHit   = zHit;
  atofHit = tofHit;
  amomHit = momHit;
}

void HGeantRpc::getHit(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                       Float_t& atofHit)
{
  axHit   = xHit;
  ayHit   = yHit;
  azHit   = zHit;
  atofHit = tofHit;
}

void HGeantRpc::getHitDigi(Float_t& axHit, Float_t& atofHit,
                           Float_t& amomHit, Float_t& aloctrackLength)
{
  aloctrackLength = loctrackLength;
  axHit           = xHit;
  atofHit         = tofHit;
  amomHit         = momHit;
}

void HGeantRpc::getCellAverage(Float_t gap, Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                               Float_t& atofHit, Float_t& amomHit, Float_t& aeHit,
                               Float_t& aloctrackLength)
{
  aloctrackLength = 0.0;
  aeHit           = 0.0;
  axHit           = 0.0;
  ayHit           = 0.0;
  azHit           = zHit;
  atofHit         = tofHit;
  amomHit         = 0.0;
  UInt_t ii=0;
  if((momHit>0.)||(eHit!=0.)) {
    if(loctrackLength>=0.) {
      aloctrackLength += loctrackLength;
    } else {
      aloctrackLength += gap;
    }
    aeHit             += eHit;
    axHit             += xHit;
    ayHit             += yHit;
    amomHit           += momHit;
    ii++;
  }
  if((momHit1>0.)||(eHit1!=0.)) {
    if(loctrackLength1>=0.) {
      aloctrackLength += loctrackLength1;
    } else {
      aloctrackLength += gap;
    }
    aeHit             += eHit1;
    axHit             += xHit1;
    ayHit             += yHit1;
    amomHit           += momHit1;
    ii++;
  }
  if((momHit2>0.)||(eHit2!=0.)) {
    if(loctrackLength2>=0.) {
      aloctrackLength += loctrackLength2;
    } else {
      aloctrackLength += gap;
    }
    aeHit             += eHit2;
    axHit             += xHit2;
    ayHit             += yHit2;
    amomHit           += momHit2;
    ii++;
  }
  if((momHit3>0.)||(eHit3!=0.)) {
    if(loctrackLength3>=0.) {
      aloctrackLength += loctrackLength3;
    } else {
      aloctrackLength += gap;
    }
    aeHit             += eHit3;
    axHit             += xHit3;
    ayHit             += yHit3;
    amomHit           += momHit3;
    ii++;
  }
  if(ii>0) {
    Float_t aii=(Float_t)ii;
    axHit           /= aii;
    ayHit           /= aii;
    amomHit         /= aii;
    aeHit           /= aii;
    aloctrackLength /= aii;
  }
}

void HGeantRpc::getCellAverage(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                               Float_t& atofHit, Float_t& amomHit, Float_t& aeHit)
{
  aeHit      = 0.0;
  axHit      = 0.0;
  ayHit      = 0.0;
  azHit      = zHit;
  atofHit    = tofHit;
  amomHit    = 0.0;
  UInt_t ii=0;
  if((momHit>0.)||(eHit!=0.)) {
    aeHit   += eHit;
    axHit   += xHit;
    ayHit   += yHit;
    amomHit += momHit;
    ii++;
  }
  if((momHit1>0.)||(eHit1!=0.)) {
    aeHit   += eHit1;
    axHit   += xHit1;
    ayHit   += yHit1;
    amomHit += momHit1;
    ii++;
  }
  if((momHit2>0.)||(eHit2!=0.)) {
    aeHit   += eHit2;
    axHit   += xHit2;
    ayHit   += yHit2;
    amomHit += momHit2;
    ii++;
  }
  if((momHit3>0.)||(eHit3!=0.)) {
    aeHit   += eHit3;
    axHit   += xHit3;
    ayHit   += yHit3;
    amomHit += momHit3;
    ii++;
  }
  if(ii>0) {
    Float_t aii=(Float_t)ii;
    aeHit   /= aii;
    axHit   /= aii;
    ayHit   /= aii;
    amomHit /= aii;
  }
}

void HGeantRpc::getCellAverage(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                               Float_t& atofHit, Float_t& amomHit)
{
  axHit   = 0.0;
  ayHit   = 0.0;
  azHit   = zHit;
  atofHit = tofHit;
  amomHit = 0.0;
  UInt_t ii=0;
  if((momHit>0.)||(eHit!=0.)) {
    axHit   += xHit;
    ayHit   += yHit;
    amomHit += momHit;
    ii++;
  }
  if((momHit1>0.)||(eHit1!=0.)) {
    axHit   += xHit1;
    ayHit   += yHit1;
    amomHit += momHit1;
    ii++;
  }
  if((momHit2>0.)||(eHit2!=0.)) {
    axHit   += xHit2;
    ayHit   += yHit2;
    amomHit += momHit2;
    ii++;
  }
  if((momHit3>0.)||(eHit3!=0.)) {
    axHit   += xHit3;
    ayHit   += yHit3;
    amomHit += momHit3;
    ii++;
  }
  if(ii>0) {
    Float_t aii=(Float_t)ii;
    axHit   /= aii;
    ayHit   /= aii;
    amomHit /= aii;
  }
}

void HGeantRpc::getCellAverage(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                               Float_t& atofHit)
{
  axHit      = 0.0;
  ayHit      = 0.0;
  azHit      = zHit;
  atofHit    = tofHit;
  UInt_t ii=0;
  if((momHit>0.)||(eHit>0.)) {
    axHit   += xHit;
    ayHit   += yHit;
    ii++;
  }
  if((momHit1>0.)||(eHit1!=0.)) {
    axHit   += xHit1;
    ayHit   += yHit1;
    ii++;
  }
  if((momHit2>0.)||(eHit2!=0.)) {
    axHit   += xHit2;
    ayHit   += yHit2;
    ii++;
  }
  if((momHit3>0.)||(eHit3!=0.)) {
    axHit   += xHit3;
    ayHit   += yHit3;
    ii++;
  }
  if(ii>0) {
    Float_t aii=(Float_t)ii;
    axHit             /= aii;
    ayHit             /= aii;
  }
}

void HGeantRpc::getCellAverageDigi(Float_t gap, Float_t& axHit, Float_t& atofHit,
                                   Float_t& amomHit, Float_t& aloctrackLength)
{
  aloctrackLength      = 0.0;
  axHit                = 0.0;
  atofHit              = tofHit;
  amomHit              = 0.0;
  UInt_t ii=0;
  if((momHit>0.)||(eHit!=0.)) {
    if(loctrackLength>=0.) {
      aloctrackLength += loctrackLength;
    } else {
      aloctrackLength += gap;
    }
    axHit             += xHit;
    amomHit           += momHit;
    ii++;
  }
  if((momHit1>0.)||(eHit1!=0.)) {
    if(loctrackLength1>=0.) {
      aloctrackLength += loctrackLength1;
    } else {
      aloctrackLength += gap;
    }
    axHit             += xHit1;
    amomHit           += momHit1;
    ii++;
  }
  if((momHit2>0.)||(eHit2!=0.)) {
    if(loctrackLength2>=0.) {
      aloctrackLength += loctrackLength2;
    } else {
      aloctrackLength += gap;
    }
    axHit             += xHit2;
    amomHit           += momHit2;
    ii++;
  }
  if((momHit3>0.)||(eHit3!=0.)) {
    if(loctrackLength3>=0.) {
      aloctrackLength += loctrackLength3;
    } else {
      aloctrackLength += gap;
    }
    axHit             += xHit3;
    amomHit           += momHit3;
    ii++;
  }
  if(ii>0) {
    Float_t aii=(Float_t)ii;
    aloctrackLength   /= aii;
    axHit             /= aii;
    amomHit           /= aii;
  }
}

// Stream an object of class HGeantRpc.
void HGeantRpc::Streamer(TBuffer &R__b)
{
   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HLinkedDataObject::Streamer(R__b);
      //
      // Handling of backward compatibility before Jan 2013
      // for READING.
      //
      if(R__v==4) {
        //
        // Version 4 before Jan 2013.
        //
        R__b >> trackNumber;
        R__b >> trackLength;
        R__b >> loctrackLength;
        R__b >> eHit;
        R__b >> xHit;
        R__b >> yHit;
        R__b >> zHit;
        R__b >> tofHit;
        R__b >> momHit;
        R__b >> thetaHit;
        R__b >> phiHit;
        R__b >> detectorID;
        // Set version.
        HGeantRpc_version = 4;
        // Zero all other unused variables.
        loctrackLength1 = 0.0;
        eHit1           = 0.0;
        xHit1           = 0.0;
        yHit1           = 0.0;
        momHit1         = 0.0;
        loctrackLength2 = 0.0;
        eHit2           = 0.0;
        xHit2           = 0.0;
        yHit2           = 0.0;
        momHit2         = 0.0;
        loctrackLength3 = 0.0;
        eHit3           = 0.0;
        xHit3           = 0.0;
        yHit3           = 0.0;
        momHit3         = 0.0;
      } else {
        //
        // Version 5 after Jan 2013.
        //
        R__b >> trackNumber;
        R__b >> trackLength;
        R__b >> loctrackLength;
        R__b >> eHit;
        R__b >> xHit;
        R__b >> yHit;
        R__b >> zHit;
        R__b >> tofHit;
        R__b >> momHit;
        R__b >> thetaHit;
        R__b >> phiHit;
        R__b >> detectorID;
        R__b >> loctrackLength1;
        R__b >> eHit1;
        R__b >> xHit1;
        R__b >> yHit1;
        R__b >> momHit1;
        R__b >> loctrackLength2;
        R__b >> eHit2;
        R__b >> xHit2;
        R__b >> yHit2;
        R__b >> momHit2;
        R__b >> loctrackLength3;
        R__b >> eHit3;
        R__b >> xHit3;
        R__b >> yHit3;
        R__b >> momHit3;
        // Set version.
        HGeantRpc_version = 5;
      }
      R__b.CheckByteCount(R__s, R__c, HGeantRpc::IsA());
   } else {
      R__c = R__b.WriteVersion(HGeantRpc::IsA(), kTRUE);
      HLinkedDataObject::Streamer(R__b);
      R__b << trackNumber;
      R__b << trackLength;
      R__b << loctrackLength;
      R__b << eHit;
      R__b << xHit;
      R__b << yHit;
      R__b << zHit;
      R__b << tofHit;
      R__b << momHit;
      R__b << thetaHit;
      R__b << phiHit;
      R__b << detectorID;
      R__b << loctrackLength1;
      R__b << eHit1;
      R__b << xHit1;
      R__b << yHit1;
      R__b << momHit1;
      R__b << loctrackLength2;
      R__b << eHit2;
      R__b << xHit2;
      R__b << yHit2;
      R__b << momHit2;
      R__b << loctrackLength3;
      R__b << eHit3;
      R__b << xHit3;
      R__b << yHit3;
      R__b << momHit3;
      R__b.SetByteCount(R__c, kTRUE);
   }
}
