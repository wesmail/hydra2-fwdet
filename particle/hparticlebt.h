#ifndef __HPARTICLEBT_H__
#define __HPARTICLEBT_H__

#include "TObject.h"
#include "hparticlebtringf.h"
#include "hparticlebtclusterf.h"
#include "hparticlebtring.h"
#include "hparticletracksorter.h"
#include "hreconstructor.h"
#include "hparticleanglecor.h"
#include "hparticlebtpar.h"


class HParticleBt : public HReconstructor {

protected:

    HCategory*             fParticleBtOutCat;
    HCategory*             fParticleBtRingInfoCat;
  
    HParticleBtRingF*      fRing;
    HParticleBtClusterF*   fClus;
    HCategory*             fCandCat;
    HCategory*             fRichCal;

    HParticleTrackSorter   fSorter;
    HParticleAngleCor      fAngleCor;
    HParticleBtPar*        fBtPar;
    Bool_t                 fDebugInfo;
    Bool_t                 fdoAngleCorr;

    static Float_t         fBetaRPCLimit;
    static Float_t         fBetaTOFLimit;
    static Float_t         fMdcdEdxLimit;
    static Float_t         foAngleLimit;

public:

    HParticleBt(void);
    HParticleBt(const Text_t* name,const Text_t* title,const TString beamtime);

    ~HParticleBt(void);

    // -----------------------------------------------------

    void setDebugInfo(Bool_t debugInfo);
    Bool_t init(void);
    Bool_t reinit(void);
    Bool_t finalize(void) { return kTRUE; }
    Int_t execute(void);

    // -----------------------------------------------------
protected:

    void fillOutput(vector <HParticleCand> &candidates);
    
    static Bool_t selectLeptonsBeta(HParticleCand* pcand);
    static Bool_t isGoodLepton(HParticleCand* cand);


   ClassDef(HParticleBt, 0)
};


#endif  // __HPARTICLEBT_H__

