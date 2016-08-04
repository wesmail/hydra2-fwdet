#pragma implementation
#include "htofraw.h"
#include "TDirectory.h"
#include "TTree.h"

//*-- Author : W.Koenig / W. Schoen / D. Vassiliev
//*-- Modified : 05/06/98 by Manuel Sanchez
//*-- Modified : 12/12/97 by Walter Karig


//_HADES_CLASS_DESCRIPTION 
////////////////////////////
//HTofRaw
//
// This class contains TOF raw data
//
// adapted from /u/halo/packages/abase/new by Manuel Sanchez (17/06/98)
////////////////////////////

ClassImp(HTofRaw)


  void  HTofRaw::doit(void) {
    TTree *tree=(TTree *)gDirectory->Get("T");
    tree->Draw("HTofRaw.fData.fLeftT>>newhist");
}

HTofRaw::HTofRaw(void) {
    clear();
}

void HTofRaw::clear(){
     nHit=0;
     rightT= leftT=0.0;
     rightQ= leftQ=0.0;
}

HTofRaw::~HTofRaw(void) {
}

