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
//  HRichTrack
//
//  This class stores the track numbers of the particles
//  that hit the RICH.
//  For each particle it contains the corresponding track number,
//  the address of the fired pad and a flag that distinguishes
//  photons and charged particles.
//  In the categories catRichGeantRaw and catRichGeantRaw+1
//  the particle track number is stored in the case that a direct
//  hit has occurred; if a photon hits the RICH, its parent track number
//  is stored, therefore we need a flag to distinguish the two cases.
//  The class is sortable by the pad address.
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichtrack.h"

ClassImp(HRichTrack)

HRichTrack::HRichTrack()
{
   fEventNr = -1;
   fTrackNr = -1;
   fAddress = -1;
   fFlag    = -1;
}
