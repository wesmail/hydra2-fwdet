//*-- Author : A. Nekhaev
//*-- Modified: 05/08/99 by R. Holzmann
//*-- Modified: 07/06/99 by Ilse Koenig


//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////
//HMdcGeantCell
//
// Container to store temporally cell data for
// MdcDigitizer. This data are not written to
// hades tree.For HMdcDigitizer internal use only.
// Up to 15 hits per cell are stored.
////////////////////////////////////////

#include "hmdcgeantcell.h"


void HMdcGeantCell::clear() {
    // resets all data members to the default value

  sector = module = layer = cell = fNumHits = 0;
  for(Int_t i=0;i<NMAXHITS;i++) {
    fMinDist[i]=0.F;
    fTimeFlight[i]=0.F;
    nTrack[i]=0;
    fImpactAngle[i]=0.F;
    flagCutEdge[i]=kFALSE;
    wireOffset[i]=0.F;
    efficiency[i]=0.F;
    theta[i]=0.;
  }
}

ClassImp(HMdcGeantCell)
