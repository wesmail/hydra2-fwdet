//*-- AUTHOR : J. Markert
//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HMdcTrackFitPar
//
// Container class for Dubna track fitter parameters
//
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "hmdctrackfitpar.h"
#include "hparamlist.h"
#include <iostream> 
#include <iomanip>
#include <stdlib.h>

ClassImp(HMdcTrackFitPar)

HMdcTrackFitPar::HMdcTrackFitPar(const Char_t* name,const Char_t* title,
                       const Char_t* context)
    : HParCond(name,title,context)
{
    //
    clear();
}
HMdcTrackFitPar::~HMdcTrackFitPar()
{
  // destructor
}
void HMdcTrackFitPar::clear()
{
    cutWeight     = -99;
    tofFlag       = -99;
    doTargScan    = -99;

    minTimeOffset = -99;
    maxTimeOffset = -99;
    minCellsNum   = -99;
    chi2CutFlag   = -99;
    totalChi2Cut  = -99;
    chi2PerNdfCut = -99;

    useTukeyFlag  = -99;
    cnWs          = -99;
    cn2s          = -99;
    cn4s          = -99;
    minSig2       = -99;
    maxNFilterIter= -99;
    minWeight     = -99;
    maxChi2       = -99;

    minTOffsetIter= -99;

    funCt1        = -99;
    stepD1        = -99;
    funCt2        = -99;
    stepD2        = -99;
    stepD3        = -99;


    status=kFALSE;
    resetInputVersions();
    changed=kFALSE;
}
void HMdcTrackFitPar::printParam(void)
{
    // prints the parameters of HMdcTrackFitPar to the screen.
    printf("############################################################################################\n");
    printf("HMdcTrackFitPar:\n");
    printf("cutWeight      = %f \n" ,cutWeight     );
    printf("tofFlag        = %i \n" ,tofFlag       );
    printf("doTargScan     = %i \n" ,doTargScan    );

    printf("minTimeOffset  = %f \n" ,minTimeOffset );
    printf("maxTimeOffset  = %f \n" ,maxTimeOffset );
    printf("minCellsNum    = %i \n" ,minCellsNum   );
    printf("chi2CutFlag    = %i \n" ,chi2CutFlag   );
    printf("totalChi2Cut   = %f \n" ,totalChi2Cut  );
    printf("chi2PerNdfCut  = %f \n" ,chi2PerNdfCut );

    printf("useTukeyFlag   = %i \n" ,useTukeyFlag  );
    printf("cnWs           = %f \n" ,cnWs          );
    printf("cn2s           = %f \n" ,cn2s          );
    printf("cn4s           = %f \n" ,cn4s          );
    printf("minSig2        = %f \n" ,minSig2       );
    printf("maxNFilterIter = %i \n" ,maxNFilterIter);
    printf("minWeight      = %f \n" ,minWeight     );
    printf("maxChi2        = %f \n" ,maxChi2       );

    printf("minTOffsetIter = %f \n" ,minTOffsetIter);

    printf("funCt1         = %f \n" ,funCt1        );
    printf("stepD1         = %f \n" ,stepD1        );
    printf("funCt2         = %f \n" ,funCt2        );
    printf("stepD2         = %f \n" ,stepD2        );
    printf("stepD3         = %f \n" ,stepD3        );
    printf("############################################################################################\n");
}
void HMdcTrackFitPar::putParams(HParamList* l)
{
    // Puts all params of HMdcTrackFitPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("cutWeight"     ,cutWeight     );
    l->add("tofFlag"       ,tofFlag       );
    l->add("doTargScan"    ,doTargScan    );

    l->add("minTimeOffset" ,minTimeOffset );
    l->add("maxTimeOffset" ,maxTimeOffset );
    l->add("minCellsNum"   ,minCellsNum   );
    l->add("chi2CutFlag"   ,chi2CutFlag   );
    l->add("totalChi2Cut"  ,totalChi2Cut  );
    l->add("chi2PerNdfCut" ,chi2PerNdfCut );

    l->add("useTukeyFlag"  ,useTukeyFlag  );
    l->add("cnWs"          ,cnWs          );
    l->add("cn2s"          ,cn2s          );
    l->add("cn4s"          ,cn4s          );
    l->add("minSig2"       ,minSig2       );
    l->add("maxNFilterIter",maxNFilterIter);
    l->add("minWeight"     ,minWeight     );
    l->add("maxChi2"       ,maxChi2       );

    l->add("minTOffsetIter",minTOffsetIter);

    l->add("funCt1"        ,funCt1        );
    l->add("stepD1"        ,stepD1        );
    l->add("funCt2"        ,funCt2        );
    l->add("stepD2"        ,stepD2        );
    l->add("stepD3"        ,stepD3        );

}
Bool_t HMdcTrackFitPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;

    if(!(l->fill("cutWeight"     ,&cutWeight     ))) return kFALSE;
    if(!(l->fill("tofFlag"       ,&tofFlag       ))) return kFALSE;
    if(!(l->fill("doTargScan"    ,&doTargScan    ))) return kFALSE;

    if(!(l->fill("minTimeOffset" ,&minTimeOffset ))) return kFALSE;
    if(!(l->fill("maxTimeOffset" ,&maxTimeOffset ))) return kFALSE;
    if(!(l->fill("minCellsNum"   ,&minCellsNum   ))) return kFALSE;
    if(!(l->fill("chi2CutFlag"   ,&chi2CutFlag   ))) return kFALSE;
    if(!(l->fill("totalChi2Cut"  ,&totalChi2Cut  ))) return kFALSE;
    if(!(l->fill("chi2PerNdfCut" ,&chi2PerNdfCut ))) return kFALSE;

    if(!(l->fill("useTukeyFlag"  ,&useTukeyFlag  ))) return kFALSE;
    if(!(l->fill("cnWs"          ,&cnWs          ))) return kFALSE;
    if(!(l->fill("cn2s"          ,&cn2s          ))) return kFALSE;
    if(!(l->fill("cn4s"          ,&cn4s          ))) return kFALSE;
    cn2s = cn4s*cn4s/cnWs;
    if(!(l->fill("minSig2"       ,&minSig2       ))) return kFALSE;
    if(!(l->fill("maxNFilterIter",&maxNFilterIter))) return kFALSE;
    if(!(l->fill("minWeight"     ,&minWeight     ))) return kFALSE;
    if(!(l->fill("maxChi2"       ,&maxChi2       ))) return kFALSE;

    if(!(l->fill("minTOffsetIter",&minTOffsetIter))) return kFALSE;

    if(!(l->fill("funCt1"        ,&funCt1        ))) return kFALSE;
    if(!(l->fill("stepD1"        ,&stepD1        ))) return kFALSE;
    if(!(l->fill("funCt2"        ,&funCt2        ))) return kFALSE;
    if(!(l->fill("stepD2"        ,&stepD2        ))) return kFALSE;
    if(!(l->fill("stepD3"        ,&stepD3        ))) return kFALSE;


    return kTRUE;
}
