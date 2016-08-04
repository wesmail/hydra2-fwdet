//*-- created  : /19/12/2001 Dusan Zovinec
//*-- Modified : /14/02/2002 by D.Zovinec
//*-- Modified : /04/03/2002 by D.Zovinec
//*-- Modified : /05/03/2002 by Ilse Koenig
//*-- Modified : /23/09/2002 by D.Zovinec

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HTofClusterFPar
//
// Clusterfinder parameters for TOF
// (adapted from /u/halo/anal/v5_45/hkicktrackfpar.cc)
//
////////////////////////////////////////////////////////////////////////////

#include "htofclusterfpar.h"
#include "hparamlist.h"

ClassImp(HTofClusterFPar)

HTofClusterFPar::HTofClusterFPar(const Char_t* name,const Char_t* title,
                                 const Char_t* context)
                : HParCond(name,title,context) {
  clear();
}

HTofClusterFPar::~HTofClusterFPar(void) {
}

void HTofClusterFPar::clear(void) {
  diffTmax = 0.0;
  diffXmax = 0.0;
  mipLimit = 0.0;
  mL1all = 0.0;
  sL1all = 0.0;
  mL2all = 0.0;
  sL2all = 0.0;
  ratCall1 = 0.0;
  mL1lep = 0.0;
  sL1lep = 0.0;
  mL2lep = 0.0;
  sL2lep = 0.0;
  ratClep1 = 0.0;
  diffPmax = 0.0;
  lossEmax = 0.0;

  status=kFALSE;
  resetInputVersions();
}

void HTofClusterFPar::putParams(HParamList* l) {
  if (!l) return;
  l->add("diffTmax",diffTmax);
  l->add("diffXmax",diffXmax);
  l->add("mipLimit",mipLimit);
  l->add("mL1all",mL1all);
  l->add("sL1all",sL1all);
  l->add("mL2all",mL2all);
  l->add("sL2all",sL2all);
  l->add("ratCall1",ratCall1);
  l->add("mL1lep",mL1lep);
  l->add("sL1lep",sL1lep);
  l->add("mL2lep",mL2lep);
  l->add("sL2lep",sL2lep);
  l->add("ratClep1",ratClep1);
  l->add("diffPmax",diffPmax);
  l->add("lossEmax",lossEmax);
}

Bool_t HTofClusterFPar::getParams(HParamList* l) {
  if (!l) return kFALSE;
  //  if (!l->fill("iCutOffThresheold",&iCutOffThresheold)) return kFALSE;
  if (!l->fill("diffTmax",&diffTmax)) return kFALSE;
  if (!l->fill("diffXmax",&diffXmax)) return kFALSE;
  if (!l->fill("mipLimit",&mipLimit)) return kFALSE;
  if (!l->fill("mL1all",&mL1all)) return kFALSE;
  if (!l->fill("sL1all",&sL1all)) return kFALSE;
  if (!l->fill("mL2all",&mL2all)) return kFALSE;
  if (!l->fill("sL2all",&sL2all)) return kFALSE;
  if (!l->fill("ratCall1",&ratCall1)) return kFALSE;
  if (!l->fill("mL1lep",&mL1lep)) return kFALSE;
  if (!l->fill("sL1lep",&sL1lep)) return kFALSE;
  if (!l->fill("mL2lep",&mL2lep)) return kFALSE;
  if (!l->fill("sL2lep",&sL2lep)) return kFALSE;
  if (!l->fill("ratClep1",&ratClep1)) return kFALSE;
  if (!l->fill("diffPmax",&diffPmax)) return kFALSE;
  if (!l->fill("lossEmax",&lossEmax)) return kFALSE;
  return kTRUE;
} 
void HTofClusterFPar::readFrom(const HTofClusterFPar &par) {
  diffTmax = par.diffTmax;
  diffXmax = par.diffXmax;
  mipLimit = par.mipLimit;
  mL1all = par.mL1all;
  sL1all = par.sL1all;
  mL2all = par.mL2all;
  sL2all = par.sL2all;
  ratCall1 = par.ratCall1;
  mL1lep = par.mL1lep;
  sL1lep = par.sL1lep;
  mL2lep = par.mL2lep;
  sL2lep = par.sL2lep;
  ratClep1 = par.ratClep1;
  diffPmax = par.diffPmax;
  lossEmax = par.lossEmax;
}

void HTofClusterFPar::Streamer(TBuffer &R__b)
{
  UInt_t R__s, R__c;
  if (R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
    HParSet::Streamer(R__b);
    R__b >> diffTmax;
    R__b >> diffXmax;
    if(R__v <= 1){
      mipLimit = 0.;
      mL1all = 0.;
      sL1all = 0.;
      mL2all = 0.;
      sL2all = 0.;
      ratCall1 = 0.;
      mL1lep = 0.;
      sL1lep = 0.;
      mL2lep = 0.;
      sL2lep = 0.;
      ratClep1 = 0.;
      diffPmax = 0.;
      lossEmax = 0.;
    } else if(R__v == 2){
      R__b >> mL1all;
      R__b >> sL1all;
      R__b >> mL2all;
      R__b >> sL2all;
      R__b >> ratCall1;
      R__b >> mL1lep;
      R__b >> sL1lep;
      R__b >> mL2lep;
      R__b >> sL2lep;
      R__b >> ratClep1;
      mipLimit = 0.;
      diffPmax = 0.;
      lossEmax = 0.;
    } else if(R__v==3){
      R__b >> mipLimit;
      R__b >> mL1all;
      R__b >> sL1all;
      R__b >> mL2all;
      R__b >> sL2all;
      R__b >> ratCall1;
      R__b >> mL1lep;
      R__b >> sL1lep;
      R__b >> mL2lep;
      R__b >> sL2lep;
      R__b >> ratClep1;
      diffPmax = 0.;
      lossEmax = 0.;
    } else if(R__v>=4){
      R__b >> mipLimit;
      R__b >> mL1all;
      R__b >> sL1all;
      R__b >> mL2all;
      R__b >> sL2all;
      R__b >> ratCall1;
      R__b >> mL1lep;
      R__b >> sL1lep;
      R__b >> mL2lep;
      R__b >> sL2lep;
      R__b >> ratClep1;
      R__b >> diffPmax;
      R__b >> lossEmax;
    }

    R__b.CheckByteCount(R__s, R__c, HTofClusterFPar::IsA());
  } else {
    R__c = R__b.WriteVersion(HTofClusterFPar::IsA(), kTRUE);
    HParSet::Streamer(R__b);
    R__b << diffTmax;
    R__b << diffXmax;
    R__b << mipLimit;
    R__b << mL1all;
    R__b << sL1all;
    R__b << mL2all;
    R__b << sL2all;
    R__b << ratCall1;
    R__b << mL1lep;
    R__b << sL1lep;
    R__b << mL2lep;
    R__b << sL2lep;
    R__b << ratClep1;
    R__b << diffPmax;
    R__b << lossEmax;
    R__b.SetByteCount(R__c, kTRUE);
  }
}
