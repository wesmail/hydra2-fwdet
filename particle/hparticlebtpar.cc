//*-- Author   : Patrick Sellheim, Georgy Kornakov
//*-- Created  : 09/01/2014

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
// HParticleBtPar
//
// Parametercontainer for RICH backtracking algorithm.
//
//
////////////////////////////////////////////////////////////////////////////
#include "hparticlebtpar.h"
#include "hparamlist.h"

ClassImp(HParticleBtPar)

HParticleBtPar::HParticleBtPar(const Char_t* name,const Char_t* title,
			       const Char_t* context)
    : HParCond(name,title,context)
{

    fRingFitPar.Set(4);
    fChargeThres.Set(12);
    fGeomConst.Set(4);
    fRingSeg.Set(2);
    fSharedCharge.Set(9);
    fSigmaGaus.Set(4);
    fParThetaAngle.Set(2);
    fPhiOffset.Set(12);

    fSigmaRange.Set(4);
    fClusterLimits.Set(2);
    fVertexPos.Set(3);
    fRichSeg.Set(3);
    fNeighbourPad.Set(8);
    fTF2ParMean.Set(5400);// 45*15*8;
    fTF2ParSigma.Set(8775);// 45*15*13
    fLeptonSelection.Set(3);

    clear();
}
HParticleBtPar::~HParticleBtPar()
{
  // destructor
}
void HParticleBtPar::clear()
{
    fNSigma          = 3;
    fSizeMatrix      = 16;
    fPhiOffsetPar    = 30;
    fMinimumSigmaVal = 0.5;
    fMaximumType     = 7;

    fRingFitPar.Reset(0);
    fChargeThres.Reset(0.);
    fGeomConst.Reset(0.);
    fRingSeg.Reset(0);
    fSharedCharge.Reset(0.);
    fSigmaGaus.Reset(0.);
    fParThetaAngle.Reset(0.);
    fPhiOffset.Reset(0.);

    fSigmaRange.Reset(0.);
    fClusterLimits.Reset(0.);
    fVertexPos.Reset(0.);
    fRichSeg.Reset(0.);
    fNeighbourPad.Reset(0);
    fTF2ParMean.Reset(0.);
    fTF2ParSigma.Reset(0.);
    fLeptonSelection.Reset(0.);


    status = kFALSE;
    resetInputVersions();
    changed = kFALSE;

}

void HParticleBtPar::putParams(HParamList* l)
{
    // Puts all params of HRpcSlewingPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("fNSigma",         fNSigma         );
    l->add("fSizeMatrix",     fSizeMatrix     );
    l->add("fPhiOffsetPar",   fPhiOffsetPar   );
    l->add("fMinimumSigmaVal",fMinimumSigmaVal);
    l->add("fMaximumType",    fMaximumType    );

    l->add("fRingFitPar",     fRingFitPar     );
    l->add("fChargeThres",    fChargeThres    );
    l->add("fGeomConst",      fGeomConst      );
    l->add("fRingSeg",        fRingSeg        );
    l->add("fSharedCharge",   fSharedCharge   );
    l->add("fSigmaGaus",      fSigmaGaus      );
    l->add("fParThetaAngle",  fParThetaAngle  );
    l->add("fPhiOffset",      fPhiOffset      );

    l->add("fSigmaRange",     fSigmaRange     );
    l->add("fClusterLimits",  fClusterLimits  );
    l->add("fVertexPos",      fVertexPos      );
    l->add("fRichSeg",        fRichSeg        );
    l->add("fNeighbourPad",   fNeighbourPad   );
    l->add("fTF2ParMean",     fTF2ParMean     );
    l->add("fTF2ParSigma",    fTF2ParSigma    );
    l->add("fLeptonSelection",fLeptonSelection);


}
Bool_t HParticleBtPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("fNSigma",          &fNSigma         ))) { cout<<"fNSigma"<<endl; return kFALSE; }
    if(!( l->fill("fSizeMatrix",      &fSizeMatrix     ))) { cout<<"fSizeMatrix"<<endl;return kFALSE;  }
    if(!( l->fill("fPhiOffsetPar",    &fPhiOffsetPar   ))) { cout<<"fPhiOffsetPar"<<endl;return kFALSE;  }
    if(!( l->fill("fMinimumSigmaVal", &fMinimumSigmaVal ))){ cout<<"fMinimumSigmaVal"<<endl; return kFALSE; }
    if(!( l->fill("fMaximumType",     &fMaximumType    ))) { cout<<"fMaximumType"<<endl;return kFALSE;  }
   
    if(!( l->fill("fRingFitPar",      &fRingFitPar     ))) { cout<<"fRingFitPar"<<endl;return kFALSE;  }
    if(!( l->fill("fChargeThres",     &fChargeThres    ))) { cout<<"fChargeThres"<<endl;return kFALSE;  }
    if(!( l->fill("fGeomConst",       &fGeomConst      ))) { cout<<"fGeomConst"<<endl;return kFALSE;  }
    if(!( l->fill("fRingSeg",         &fRingSeg        ))) { cout<<"fRingSeg"<<endl;return kFALSE;  }
    if(!( l->fill("fSharedCharge",    &fSharedCharge   ))) { cout<<"fSharedCharge"<<endl;return kFALSE;  }
    if(!( l->fill("fSigmaGaus",       &fSigmaGaus      ))) { cout<<"fSigmaGaus"<<endl;return kFALSE;  }
    if(!( l->fill("fParThetaAngle",   &fParThetaAngle  ))) { cout<<"fParThetaAngle"<<endl;return kFALSE;  }
    if(!( l->fill("fPhiOffset",       &fPhiOffset      ))) { cout<<"fPhiOffset"<<endl;return kFALSE;  }
   
    if(!( l->fill("fSigmaRange",      &fSigmaRange      ))){ cout<<"fSigmaRange"<<endl; return kFALSE; }
    if(!( l->fill("fClusterLimits",   &fClusterLimits   ))){ cout<<"fClusterLimits"<<endl; return kFALSE; }
    if(!( l->fill("fVertexPos",       &fVertexPos       ))){ cout<<"fVertexPos"<<endl; return kFALSE; }
    if(!( l->fill("fRichSeg",         &fRichSeg         ))){ cout<<"fRichSeg"<<endl; return kFALSE; }
    if(!( l->fill("fNeighbourPad",    &fNeighbourPad    ))){ cout<<"fNeighbourPad"<<endl; return kFALSE; }
    if(!( l->fill("fTF2ParMean",      &fTF2ParMean      ))) { cout<<"fTF2ParMean"<<endl;return kFALSE; }
    if(!( l->fill("fTF2ParSigma",     &fTF2ParSigma     ))){ cout<<"fTF2ParSigma"<<endl; return kFALSE; }
    if(!( l->fill("fLeptonSelection", &fLeptonSelection ))){ cout<<"fLeptonSelection"<<endl; return kFALSE; }

    return kTRUE;
}

