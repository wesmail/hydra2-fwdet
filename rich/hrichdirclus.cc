//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Laura Fabbietti <Laura.Fabbietti@ph.tum.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichDirClus
//
// This class stores information about the clusters in the RICH
// detector like cluster size, total charge accumulated in the
// cluster and number of fired pads in the cluster.
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichdirclus.h"

using namespace std;

ClassImp(HRichDirClus)

HRichDirClus::HRichDirClus()
{
   fSector      = 0;
   fNrPads      = 0;
   fDimX        = 0.0;;
   fDimY        = 0.0;
   fTotalCharge = 0.0;
   fPhiDiff     = 0.0;
   fTheta       = 0.0;
   fMeanX       = 0.0;
   fMeanY       = 0.0;
}
