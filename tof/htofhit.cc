#include "htofhit.h"

//*-- Modified 14/11/2001 by D.Zovinec

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////
//HTofHit 
//
// Class with a TOF Hit's data
//
// adapted from /u/halo/packages/abase/new by Manuel Sanchez (17/06/98)
/////////////////////////////

void HTofHit::Streamer(TBuffer &R__b)
{
  // Stream an object of class HTofHit.
  if (R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion();
    TObject::Streamer(R__b);
    R__b >> index;
    R__b >> tof;
    R__b >> xpos;
    R__b >> xlab;
    R__b >> ylab;
    R__b >> zlab;
    R__b >> dist;
    R__b >> theta;
    R__b >> phi;
    R__b >> sector;
    R__b >> module;
    R__b >> cell;
    if(R__v <= 1){
      xposAdc = 0.;
      edep = 0.;
      lAmp = 0.;
      rAmp = 0.;
      flagAdc = 0;
    } else {
      R__b >> xposAdc;
      R__b >> edep;
      R__b >> lAmp;
      R__b >> rAmp;
      R__b >> flagAdc;
    }
  } else {
    R__b.WriteVersion(HTofHit::IsA());
    TObject::Streamer(R__b);
    R__b << index;
    R__b << tof;
    R__b << xpos;
    R__b << xposAdc;
    R__b << edep;
    R__b << lAmp;
    R__b << rAmp;
    R__b << xlab;
    R__b << ylab;
    R__b << zlab;
    R__b << dist;
    R__b << theta;
    R__b << phi;
    R__b << sector;
    R__b << module;
    R__b << cell;
    R__b << flagAdc;
  }
}

ClassImp(HTofHit)
