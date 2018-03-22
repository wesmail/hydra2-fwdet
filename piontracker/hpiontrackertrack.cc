//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////////
//
//  HPionTrackerTrack
//
//  Hits of a PionTracker detector
//
/////////////////////////////////////////////////////////////////////////////

#include "hpiontrackertrack.h"

#include "TVector3.h"
#include "TBuffer.h"

ClassImp(HPionTrackerTrack)


void HPionTrackerTrack::setPxyz (Float_t px, Float_t py, Float_t pz, Float_t match)
{
	fPx = px;
	fPy = py;
	fPz = pz;
	fMatch = match;

	TVector3 v(px, py, pz);
	fP = v.Mag();
	fTheta = v.Theta();
	fPhi = v.Phi();
	fCosTheta = v.CosTheta();
}

void HPionTrackerTrack::setPThetaPhi (Float_t p, Float_t theta, Float_t phi, Float_t match)
{
	fP = p;
	fTheta = theta;
	fPhi = phi;
	fMatch = match;

// 	TVector3 v(1);printf("p = %f\n", p);
// 	v.SetMag(p);
// 	v.SetTheta(theta);
// 	v.SetPhi(phi);
	TVector3 v;
	v.SetMagThetaPhi(p, theta, phi);

	fCosTheta = v.CosTheta();
	fPx = v.X();
	fPy = v.Y();
	fPz = v.Z();
}

void HPionTrackerTrack::getPos1(Float_t& hx, Float_t& hy) const
{
	hx = fX1;
	hy = fY1;
}

void HPionTrackerTrack::getPos2(Float_t& hx, Float_t& hy) const
{
	hx = fX2;
	hy = fY2;
}

void HPionTrackerTrack::getPosH(Float_t& hx, Float_t& hy) const
{
	hx = fXh;
	hy = fYh;
}

void HPionTrackerTrack::getProdAngles(Float_t & theta, Float_t & phi) const
{
	theta = fTheta0;
	phi = fPhi0;
}

void HPionTrackerTrack::getProdY(Float_t & y0) const
{
	y0 = fY0;
}

void HPionTrackerTrack::setPos1(Float_t hx, Float_t hy)
{
	fX1 = hx;
	fY1 = hy;
}

void HPionTrackerTrack::setPos2(Float_t hx, Float_t hy)
{
	fX2 = hx;
	fY2 = hy;
}

void HPionTrackerTrack::setPosH(Float_t hx, Float_t hy)
{
	fXh = hx;
	fYh = hy;
}

void HPionTrackerTrack::setPosAll(Float_t hx1, Float_t hy1, Float_t hx2, Float_t hy2, Float_t hxh, Float_t hyh)
{
	fX1 = hx1;
	fY1 = hy1;
	fX2 = hx2;
	fY2 = hy2;
	fXh = hxh;
	fYh = hyh;
}

void HPionTrackerTrack::setProdAngles(Float_t theta, Float_t phi)
{
	fTheta0 = theta;
	fPhi0 = phi;
}

void HPionTrackerTrack::setProdY(Float_t y0)
{
	fY0 = y0;
}


void HPionTrackerTrack::Streamer(TBuffer &R__b)

{

   // Stream an object of class HPionTrackerTrack.

   UInt_t R__s, R__c;

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> fP;
      R__b >> fTheta;
      R__b >> fPhi;
      R__b >> fCosTheta;
      R__b >> fX1;
      R__b >> fY1;
      R__b >> fX2;
      R__b >> fY2;
      R__b >> fXh;
      R__b >> fYh;
      R__b >> fPx;
      R__b >> fPy;
      R__b >> fPz;
      if(R__v > 1) {
	  R__b >> fPhi0;
	  R__b >> fTheta0;
      } else {
            fPhi0   = -1000.;
            fTheta0 = -1000.;
      }
      if(R__v > 2) {
	  R__b >> fY0;
      } else {
            fY0 = -1000.;
      }
      R__b >> fMatch;
      if(R__v > 2) {
	  R__b >> fDist;
      } else {
            fDist = -1000.;
      }

      R__b.CheckByteCount(R__s, R__c, HPionTrackerTrack::IsA());
   } else {
      R__c = R__b.WriteVersion(HPionTrackerTrack::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << fP;
      R__b << fTheta;
      R__b << fPhi;
      R__b << fCosTheta;
      R__b << fX1;
      R__b << fY1;
      R__b << fX2;
      R__b << fY2;
      R__b << fXh;
      R__b << fYh;
      R__b << fPx;
      R__b << fPy;
      R__b << fPz;
      R__b << fPhi0;
      R__b << fTheta0;
      R__b << fY0;
      R__b << fMatch;
      R__b << fDist;
      R__b.SetByteCount(R__c, kTRUE);
   }

}

