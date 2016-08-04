#include "TString.h"
#include "hrpcdigipar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"
using namespace std;

//*-- Author   : Pablo Cabanelas
//*-- Created  : 08/06/2010
//*-- Modified : 10/11/2012 A.Mangiarotti Added parameters for the
//*-- Modified : new digitizer: fS1_time, fS2_time, fS3_time,
//*-- Modified : fQmean1, fQmean2, fQwid, fQwid1, fQwid2, fEff1
//*-- Modified : fEff2, fEff3 and fMode. Costume streamer added to
//*-- Modified : initialise all the new parameters to zero if the
//*-- Modified : old container is read. The same behaviour is
//*-- Modified : defined for condition style.
//
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////
//
//  HRpcDigiPar:
//  Container for the RPC digitization parameters
//
//  (Condition Style)
//
/////////////////////////////////////////////////////////////////////////

ClassImp(HRpcDigiPar)

// constructor
HRpcDigiPar::HRpcDigiPar(const char* name,const char* title,
		const char* context) : HParCond(name,title,context) {
	clear();
}

// clears the container
void HRpcDigiPar::clear() {
	fVprop    = 0.;
	fS_x      = 0.;
	fS0_time  = 0.;
	fS1_time  = 0.;
	fS2_time  = 0.;
	fS3_time  = 0.;
	fT_off    = 0.;
	fQmean0   = 0.;
	fQmean1   = 0.;
	fQmean2   = 0.;
	fQwid0    = 0.;
	fQwid1    = 0.;
	fQwid2    = 0.;
	fEff0     = 0.;
	fEff1     = 0.;
	fEff2     = 0.;
	fEff3     = 0.;
	fEff4     = 0.;
	fEff5     = 0.;
	fTime2Tdc = 0.;
	fPedestal = 0.;
	fQtoW0    = 0.;
	fQtoW1    = 0.;
	fQtoW2    = 0.;
	fQtoW3    = 0.;
	fGap      = 0.;
        fMode     = 0;
	status=kFALSE;
	resetInputVersions();
}

// puts all parameters to the parameter list, which is used by the io
void HRpcDigiPar::putParams(HParamList* l) {
	if (!l) return;
	l->add("fVprop"   ,fVprop);
	l->add("fS_x"     ,fS_x);
	l->add("fS_time"  ,fS0_time);
	l->add("fS1_time" ,fS1_time);
	l->add("fS2_time" ,fS2_time);
	l->add("fS3_time" ,fS3_time);
	l->add("fT_off"   ,fT_off);
	l->add("fQmean"   ,fQmean0);
	l->add("fQmean1"  ,fQmean1);
	l->add("fQmean2"  ,fQmean2);
	l->add("fQwid"    ,fQwid0);
	l->add("fQwid1"   ,fQwid1);
	l->add("fQwid2"   ,fQwid2);
	l->add("fEff"     ,fEff0);
	l->add("fEff1"    ,fEff1);
	l->add("fEff2"    ,fEff2);
	l->add("fEff3"    ,fEff3);
	l->add("fEff4"    ,fEff4);
	l->add("fEff5"    ,fEff5);
	l->add("fTime2Tdc",fTime2Tdc);
	l->add("fPedestal",fPedestal);
	l->add("fQtoW0"   ,fQtoW0);
	l->add("fQtoW1"   ,fQtoW1);
	l->add("fQtoW2"   ,fQtoW2);
	l->add("fQtoW3"   ,fQtoW3);
	l->add("fGap"     ,fGap);
	l->add("fMode"    ,fMode);
}

// gets all parameters from the parameter list, which is used by the io
Bool_t HRpcDigiPar::getParams(HParamList* l) {
	if (!l) return kFALSE;
	if (!(l->fill("fVprop"   ,&fVprop)))    return kFALSE;
	if (!(l->fill("fS_x"     ,&fS_x)))      return kFALSE;
	if (!(l->fill("fS_time"  ,&fS0_time)))  return kFALSE;
	if (!(l->fill("fS1_time" ,&fS1_time)))  {Info("getParams","old database without S1_time"); fS1_time=0.;}
	if (!(l->fill("fS2_time" ,&fS2_time)))  {Info("getParams", "old database without S2_time"); fS2_time=0.;}
	if (!(l->fill("fS3_time" ,&fS3_time)))  {Info("getParams", "old database without S3_time"); fS3_time=0.;}
	if (!(l->fill("fT_off"   ,&fT_off)))    return kFALSE;
	if (!(l->fill("fQmean"   ,&fQmean0)))   return kFALSE;
	if (!(l->fill("fQmean1"  ,&fQmean1)))   {Info("getParams", "old database without Qmean1"); fQmean1=0.;}
	if (!(l->fill("fQmean2"  ,&fQmean2)))   {Info("getParams", "old database without Qmean2"); fQmean2=0.;}
	if (!(l->fill("fQwid"    ,&fQwid0)))    {Info("getParams", "old database without Qwid1"); fQwid0=0.;}
	if (!(l->fill("fQwid1"   ,&fQwid1)))    {Info("getParams", "old database without Qwid2"); fQwid1=0.;}
	if (!(l->fill("fQwid2"   ,&fQwid2)))    {Info("getParams", "old database without Qwid3"); fQwid2=0.;}
	if (!(l->fill("fEff"     ,&fEff0)))     return kFALSE;
	if (!(l->fill("fEff1"    ,&fEff1)))     {Info("getParams", "old database without Eff1"); fEff1=0.;}
	if (!(l->fill("fEff2"    ,&fEff2)))     {Info("getParams", "old database without Eff2"); fEff2=0.;}
	if (!(l->fill("fEff3"    ,&fEff3)))     {Info("getParams", "old database without Eff3"); fEff3=0.;}
	if (!(l->fill("fEff4"    ,&fEff4)))     {Info("getParams", "old database without Eff3"); fEff4=0.;}
	if (!(l->fill("fEff5"    ,&fEff5)))     {Info("getParams", "old database without Eff3"); fEff5=0.;}
	if (!(l->fill("fTime2Tdc",&fTime2Tdc))) return kFALSE;
	if (!(l->fill("fPedestal",&fPedestal))) return kFALSE;
	if (!(l->fill("fQtoW0"   ,&fQtoW0)))    return kFALSE;
	if (!(l->fill("fQtoW1"   ,&fQtoW1)))    return kFALSE;
	if (!(l->fill("fQtoW2"   ,&fQtoW2)))    return kFALSE;
	if (!(l->fill("fQtoW3"   ,&fQtoW3)))    return kFALSE;
	if (!(l->fill("fGap"     ,&fGap)))      return kFALSE;
	if (!(l->fill("fMode"    ,&fMode)))     {Info("getParams", "old database without fMode"); fMode=0;}
	return kTRUE;
}

// Stream an object of class HRpcDigiPar.
void HRpcDigiPar::Streamer(TBuffer &R__b)
{
   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HParCond::Streamer(R__b);
      R__b >> fVprop;
      R__b >> fS_x;
      R__b >> fS0_time;
      if(R__v>1) {
        R__b >> fS1_time;
	R__b >> fS2_time;
	R__b >> fS3_time;
      } else {
        fS1_time=0.;
	fS2_time=0.;
	fS3_time=0.;
      }
      R__b >> fT_off;
      R__b >> fQmean0;
      if(R__v>1) {
        R__b >> fQmean1;
	R__b >> fQmean2;
        R__b >> fQwid0;
        R__b >> fQwid1;
	R__b >> fQwid2;
      } else {
        fQmean1=0.;
	fQmean2=0.;
        fQwid0=0.;
        fQwid1=0.;
	fQwid2=0.;
      }
      R__b >> fEff0;
      if(R__v>1) {
        R__b >> fEff1;
	R__b >> fEff2;
	R__b >> fEff3;
      } else {
        fEff1=0.;
	fEff2=0.;
	fEff3=0.;
      }
      if(R__v>2) {
        R__b >> fEff4;
	R__b >> fEff5;
      } else {
        fEff4=0.;
	fEff5=0.;
      }
      R__b >> fTime2Tdc;
      R__b >> fPedestal;
      R__b >> fQtoW0;
      R__b >> fQtoW1;
      R__b >> fQtoW2;
      R__b >> fQtoW3;
      R__b >> fGap;
      if(R__v>1) {
        R__b >> fMode;
      } else {
        fMode=0;
      }
      R__b.CheckByteCount(R__s, R__c, HRpcDigiPar::IsA());
   } else {
      R__c = R__b.WriteVersion(HRpcDigiPar::IsA(), kTRUE);
      HParCond::Streamer(R__b);
      R__b << fVprop;
      R__b << fS_x;
      R__b << fS0_time;
      R__b << fS1_time;
      R__b << fS2_time;
      R__b << fS3_time;
      R__b << fT_off;
      R__b << fQmean0;
      R__b << fQmean1;
      R__b << fQmean2;
      R__b << fQwid0;
      R__b << fQwid1;
      R__b << fQwid2;
      R__b << fEff0;
      R__b << fEff1;
      R__b << fEff2;
      R__b << fEff3;
      R__b << fEff4;
      R__b << fEff5;
      R__b << fTime2Tdc;
      R__b << fPedestal;
      R__b << fQtoW0;
      R__b << fQtoW1;
      R__b << fQtoW2;
      R__b << fQtoW3;
      R__b << fGap;
      R__b << fMode;
      R__b.SetByteCount(R__c, kTRUE);
   }
}
