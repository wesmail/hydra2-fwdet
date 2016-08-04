//*-- AUTHOR : Ilse Koenig
//*-- Created : 05/11/2004
//*-- Modified : 24/11/2004 by V. Pechenov

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////
//
//  HMetaMatchPar
//
//  Parameter container for HMetaMatchF
//
// Rich:
//   Tr - angle theta of ring from HRichHit (in degree) in lab.coor.sys.
//   Pr - angle phi of ring from HRichHit (in degree) in lab.coor.sys.
//   Tm - angle theta of ring from HRichHit (in degree) in lab.coor.sys.
//   Pm - angle phi of ring from HRichHit (in degree) in lab.coor.sys.
//   SigmaTheta= (richThetaMaxCut - richThetaMinCut)/2.
//   Toffset   = (richThetaMinCut + richThetaMaxCut)/2.
//   qualityRich = 
//     sqrt[((Pr - Pm - richSigmaPhiOffset)*sin(Tm)/richSigmaPhi)^2 +
//          ((Tr - Tm - Toffset)/SigmaTheta)^2]
//
//   Condition of RICH MDC matching:  qualityRich < richQualityCut
//
//   Parameters preparation (for each sector):
//     From distribution of (Tr-Tm) to get 
//     window "richThetaMinCut" - "richThetaMaxCut".
//     From distribution of (Pr-Pm) to get (by Gaus approximation for example)
//     "richSigmaPhiOffset" (mean of approximation).
//     From distribution of (Pr-Pm-richSigmaPhiOffset)*sin(Tm) to get 
//     "richSigmaPhi" (sigma of this distr.).
//     Select "richQualityCut", for example 1.0.
//
//  RichIPU:
//   Tr - angle theta of ring from HRichHitIPU (in degree) in lab.coor.sys.
//   Pr - angle phi of ring from HRichHitIPU (in degree) in lab.coor.sys.
//   Tm - angle theta of ring from HRichHitIPU (in degree) in lab.coor.sys.
//   Pm - angle phi of ring from HRichHitIPU (in degree) in lab.coor.sys.
//   SigmaTheta= (richIPUThetaMaxCut - richIPUThetaMinCut)/2.
//   Toffset   = (richIPUThetaMinCut + richIPUThetaMaxCut)/2.
//   qualityRichIPU = 
//     sqrt[((Pr - Pm - richIPUSigmaPhiOffset)*sin(Tm)/richIPUSigmaPhi)^2 +
//          ((Tr - Tm - Toffset)/SigmaTheta)^2]
//
//   Condition of RICHIPU MDC matching:  qualityIPURich < richIPUQualityCut
//
//   Parameters preparation (for each sector):
//     From distribution of (Tr-Tm) to get 
//     window "richIPUThetaMinCut" - "richIPUThetaMaxCut".
//     From distribution of (Pr-Pm) to get (by Gaus approximation for
//     example) "richIPUSigmaPhiOffset" (mean of this distr.).
//     From distribution of (Pr-Pm-richIPUSigmaPhiOffset)*sin(Tm) to get
//     "richIPUSigmaPhi" (sigma of this distr.).
//     Select "richIPUQualityCut", for example 1.0
//
// Rpc:
//   Xr,Yr - rpc hit position in coordinate system of rpc module
//   dXr,dYr - hit position errors (== HRpcHit::getXRMS(), getYRMS())
//   Xm,Ym - mdc segment cross point with rpc module in coordinate system 
//           of corresponding rpc module 
//   qualityRpc = 
//     sqrt[((Xr - Xm - rpcSigmaXOffset)/sqrt(dXr^2 + rpcSigmaXMdc^2))^2 +
//          ((Yr - Ym - rpcSigmaYOffset)/sqrt(dYr^2 + rpcSigmaYMdc^2))^2]
//
//   Condition of SHOWER MDC matching:  qualityRpc < rpcQualityCut
//
// Shower:
//   Xs,Ys - shower hit position in coordinate system of shower module
//   dXs,dYs - hit position errors (== HShowerHit::getSigmaX(), getSigmaY())
//   Xm,Ym - mdc segment cross point with shower module in coordinate system 
//           of corresponding shower module 
//   qualityShower = 
//     sqrt[((Xs - Xm - showerSigmaXOffset)/sqrt(dXs^2 + showerSigmaXMdc^2))^2 +
//          ((Ys - Ym - showerSigmaYOffset)/sqrt(dYs^2 + showerSigmaYMdc^2))^2]
//
//   Condition of SHOWER MDC matching:  qualityShower < showerQualityCut
//
//   Parameters preparation (for each sector):
//     From distribution of (Xs-Xm) to get "showerSigmaXOffset" 
//     (mean of this distr.).
//     From distribution of (Xs-Xm-showerSigmaXOffset)/dXs (!) to get 
//     sigma of this distribution.
//     If sigma > 1. to calculate "showerSigmaXMdc" = sqrt(sigma^2 - 1.),
//     otherwise set "showerSigmaXMdc" = 0.
//     From distribution of (Ys-Ym) to get "showerSigmaYOffset".
//     From distribution of (Ys-Ym-showerSigmaYOffset)/dYs (!) to get 
//     sigma of this distribution.
//     If sigma > 1. to calculate "showerSigmaYMdc" = sqrt(sigma^2 - 1.),
//     otherwise set "showerSigmaYMdc" = 0.
//     Select "showerQualityCut", for example 5.0 (five sigmas cut).
//
//
// Tof:
//   Xt,Yt - toh hit (or tof cluster position in coordinate system of tof module
//   Xm,Ym - mdc segment cross point with tof module in coordinate system 
//           of corresponding tof module
//   qualityTof = sqrt[ ((Xt - Xm - tofSigmaXOffset)/tofSigmaX)^2 +
//                      ((Yt - Ym - tofSigmaYOffset)/tofSigmaY)^2 ]
//
//   Condition of TOF MDC matching:  qualityTof < tofQualityCut
//
//   Parameters preparation (for each sector):
//     From distribution of (Xt-Xm) to get "tofSigmaX" (sigma of this distr.) 
//     and "tofSigmaXOffset" (mean of this distr).
//     From distribution of (Yt-Ym) to get "tofSigmaY" (sigma of this distr.) 
//     and "tofSigmaYOffset" (mean of this distr).
//     Select "tofQualityCut", for example 5.0 (five sigmas cut).
//
///////////////////////////////////////////////////////////////////////////

#include "hmetamatchpar.h"
#include "hparamlist.h"

ClassImp(HMetaMatchPar)

HMetaMatchPar::HMetaMatchPar(const Char_t* name,
                       const Char_t* title,
                       const Char_t* context)
           : HParCond(name,title,context) {
  // constructor
  Float_t a[6]={0.,0.,0.,0.,0.,0.};
  richThetaMinCut.Set(6,a);
  richThetaMaxCut.Set(6,a);
  richSigmaPhi.Set(6,a);
  richSigmaPhiOffset.Set(6,a);
  richQualityCut.Set(6,a);
  richIPUThetaMinCut.Set(6,a);
  richIPUThetaMaxCut.Set(6,a);
  richIPUSigmaPhi.Set(6,a);
  richIPUSigmaPhiOffset.Set(6,a);
  richIPUQualityCut.Set(6,a);
  showerSigmaXMdc.Set(6,a);
  showerSigmaYMdc.Set(6,a);
  showerSigmaXOffset.Set(6,a);
  showerSigmaYOffset.Set(6,a);
  showerQualityCut.Set(6,a);
  tofSigmaX.Set(6,a);
  tofSigmaY.Set(6,a);
  tofSigmaXOffset.Set(6,a);
  tofSigmaYOffset.Set(6,a);
  tofQualityCut.Set(6,a);
  rpcSigmaXMdc.Set(6,a);
  rpcSigmaYMdc.Set(6,a);
  rpcSigmaXOffset.Set(6,a);
  rpcSigmaYOffset.Set(6,a);
  rpcQualityCut.Set(6,a); 
}

void HMetaMatchPar::clear(void) {
  // all parameters are initialized with 0.
  for(Int_t i=0;i<6;i++) {
    richThetaMinCut[i]       = 0.f;
    richThetaMaxCut[i]       = 0.f;
    richSigmaPhi[i]          = 0.f;
    richSigmaPhiOffset[i]    = 0.f;
    richQualityCut[i]        = 0.f;
    richIPUThetaMinCut[i]    = 0.f;
    richIPUThetaMaxCut[i]    = 0.f;
    richIPUSigmaPhi[i]       = 0.f;
    richIPUSigmaPhiOffset[i] = 0.f;
    richIPUQualityCut[i]     = 0.f;
    showerSigmaXMdc[i]       = 0.f;
    showerSigmaYMdc[i]       = 0.f;
    showerSigmaXOffset[i]    = 0.f;
    showerSigmaYOffset[i]    = 0.f;
    showerQualityCut[i]      = 0.f;
    tofSigmaX[i]             = 0.f;
    tofSigmaY[i]             = 0.f;
    tofSigmaXOffset[i]       = 0.f;
    tofSigmaYOffset[i]       = 0.f;
    tofQualityCut[i]         = 0.f;
    
    rpcSigmaXMdc[i]          = 0.f;
    rpcSigmaYMdc[i]          = 0.f;
    rpcSigmaXOffset[i]       = 0.f;
    rpcSigmaYOffset[i]       = 0.f;
    rpcQualityCut[i]         = 0.f;
  }
  status=kFALSE;
  resetInputVersions();
  changed=kFALSE;
}

void HMetaMatchPar::putParams(HParamList* l) {
  // puts all parameters to the parameter list, which is used by the io
  if (!l) return;
  l->add("richThetaMinCut",      richThetaMinCut);
  l->add("richThetaMaxCut",      richThetaMaxCut);
  l->add("richSigmaPhi",         richSigmaPhi);
  l->add("richSigmaPhiOffset",   richSigmaPhiOffset);
  l->add("richQualityCut",       richQualityCut);
  l->add("richIPUThetaMinCut",   richIPUThetaMinCut);
  l->add("richIPUThetaMaxCut",   richIPUThetaMaxCut);
  l->add("richIPUSigmaPhi",      richIPUSigmaPhi);
  l->add("richIPUSigmaPhiOffset",richIPUSigmaPhiOffset);
  l->add("richIPUQualityCut",    richIPUQualityCut);
  l->add("showerSigmaXMdc",      showerSigmaXMdc);
  l->add("showerSigmaYMdc",      showerSigmaYMdc);
  l->add("showerSigmaXOffset",   showerSigmaXOffset);
  l->add("showerSigmaYOffset",   showerSigmaYOffset);
  l->add("showerQualityCut",     showerQualityCut);
  l->add("tofSigmaX",            tofSigmaX);
  l->add("tofSigmaY",            tofSigmaY);
  l->add("tofSigmaXOffset",      tofSigmaXOffset);
  l->add("tofSigmaYOffset",      tofSigmaYOffset);
  l->add("tofQualityCut",        tofQualityCut);
    
  l->add("rpcSigmaXMdc",         rpcSigmaXMdc);
  l->add("rpcSigmaYMdc",         rpcSigmaYMdc);
  l->add("rpcSigmaXOffset",      rpcSigmaXOffset);
  l->add("rpcSigmaYOffset",      rpcSigmaYOffset);
  l->add("rpcQualityCut",        rpcQualityCut);
}

Bool_t HMetaMatchPar::getParams(HParamList* l) {
  // gets all parameters from the parameter list, which is used by the io
  if (!l) return kFALSE;
  if (!(l->fill("richThetaMinCut",      &richThetaMinCut)))       return kFALSE;
  if (!(l->fill("richThetaMaxCut",      &richThetaMaxCut)))       return kFALSE;
  if (!(l->fill("richSigmaPhi",         &richSigmaPhi)))          return kFALSE;
  if (!(l->fill("richSigmaPhiOffset",   &richSigmaPhiOffset)))    return kFALSE;
  if (!(l->fill("richQualityCut",       &richQualityCut)))        return kFALSE;
  if (!(l->fill("richIPUThetaMinCut",   &richIPUThetaMinCut)))    return kFALSE;
  if (!(l->fill("richIPUThetaMaxCut",   &richIPUThetaMaxCut)))    return kFALSE;
  if (!(l->fill("richIPUSigmaPhi",      &richIPUSigmaPhi)))       return kFALSE;
  if (!(l->fill("richIPUSigmaPhiOffset",&richIPUSigmaPhiOffset))) return kFALSE;
  if (!(l->fill("richIPUQualityCut",    &richIPUQualityCut)))     return kFALSE;  
  if (!(l->fill("showerSigmaXMdc",      &showerSigmaXMdc)))       return kFALSE;
  if (!(l->fill("showerSigmaYMdc",      &showerSigmaYMdc)))       return kFALSE;
  if (!(l->fill("showerSigmaXOffset",   &showerSigmaXOffset)))    return kFALSE;
  if (!(l->fill("showerSigmaYOffset",   &showerSigmaYOffset)))    return kFALSE;
  if (!(l->fill("showerQualityCut",     &showerQualityCut)))      return kFALSE;
  if (!(l->fill("tofSigmaX",            &tofSigmaX)))             return kFALSE;
  if (!(l->fill("tofSigmaY",            &tofSigmaY)))             return kFALSE;
  if (!(l->fill("tofSigmaXOffset",      &tofSigmaXOffset)))       return kFALSE;
  if (!(l->fill("tofSigmaYOffset",      &tofSigmaYOffset)))       return kFALSE;
  if (!(l->fill("tofQualityCut",        &tofQualityCut)))         return kFALSE;
  
  if (!(l->fill("rpcSigmaXMdc",         &rpcSigmaXMdc)))          return kFALSE;
  if (!(l->fill("rpcSigmaYMdc",         &rpcSigmaYMdc)))          return kFALSE;
  if (!(l->fill("rpcSigmaXOffset",      &rpcSigmaXOffset)))       return kFALSE;
  if (!(l->fill("rpcSigmaYOffset",      &rpcSigmaYOffset)))       return kFALSE;
  if (!(l->fill("rpcQualityCut",        &rpcQualityCut)))         return kFALSE;

  return kTRUE;
}

void HMetaMatchPar::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMetaMatchPar.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HParCond::Streamer(R__b);
      richThetaMinCut.Streamer(R__b);
      richThetaMaxCut.Streamer(R__b);
      richSigmaPhi.Streamer(R__b);
      richSigmaPhiOffset.Streamer(R__b);
      richQualityCut.Streamer(R__b);
      richIPUThetaMinCut.Streamer(R__b);
      richIPUThetaMaxCut.Streamer(R__b);
      richIPUSigmaPhi.Streamer(R__b);
      richIPUSigmaPhiOffset.Streamer(R__b);
      richIPUQualityCut.Streamer(R__b);
      showerSigmaXMdc.Streamer(R__b);
      showerSigmaYMdc.Streamer(R__b);
      showerSigmaXOffset.Streamer(R__b);
      showerSigmaYOffset.Streamer(R__b);
      showerQualityCut.Streamer(R__b);
      tofSigmaX.Streamer(R__b);
      tofSigmaY.Streamer(R__b);
      tofSigmaXOffset.Streamer(R__b);
      tofSigmaYOffset.Streamer(R__b);
      tofQualityCut.Streamer(R__b);
      
      if(R__v > 1) {
        rpcSigmaXMdc.Streamer(R__b);   
        rpcSigmaYMdc.Streamer(R__b);   
        rpcSigmaXOffset.Streamer(R__b);
        rpcSigmaYOffset.Streamer(R__b);
        rpcQualityCut.Streamer(R__b);
      }
      
      R__b.CheckByteCount(R__s, R__c, HMetaMatchPar::IsA());
   } else {
      R__c = R__b.WriteVersion(HMetaMatchPar::IsA(), kTRUE);
      HParCond::Streamer(R__b);
      richThetaMinCut.Streamer(R__b);
      richThetaMaxCut.Streamer(R__b);
      richSigmaPhi.Streamer(R__b);
      richSigmaPhiOffset.Streamer(R__b);
      richQualityCut.Streamer(R__b);
      richIPUThetaMinCut.Streamer(R__b);
      richIPUThetaMaxCut.Streamer(R__b);
      richIPUSigmaPhi.Streamer(R__b);
      richIPUSigmaPhiOffset.Streamer(R__b);
      richIPUQualityCut.Streamer(R__b);
      showerSigmaXMdc.Streamer(R__b);
      showerSigmaYMdc.Streamer(R__b);
      showerSigmaXOffset.Streamer(R__b);
      showerSigmaYOffset.Streamer(R__b);
      showerQualityCut.Streamer(R__b);
      tofSigmaX.Streamer(R__b);
      tofSigmaY.Streamer(R__b);
      tofSigmaXOffset.Streamer(R__b);
      tofSigmaYOffset.Streamer(R__b);
      tofQualityCut.Streamer(R__b);
      
      rpcSigmaXMdc.Streamer(R__b);   
      rpcSigmaYMdc.Streamer(R__b);   
      rpcSigmaXOffset.Streamer(R__b);
      rpcSigmaYOffset.Streamer(R__b);
      rpcQualityCut.Streamer(R__b); 
      
      R__b.SetByteCount(R__c, kTRUE);
   }
}
