#pragma implementation
#include "hshowertrack.h"
ClassImp(HShowerTrack)
//*-- Author : Leszek Kidon
//*-- Modified : 22/02/2000 L.Fabbietti

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////
//______________________________________________________
//  HShowerTrack
//  This class stores the track numbers of the particle
//  that hit the PreShower. It is sortable by the
//  fired pad address.
//
//////////////////////////////////////////////////////


Int_t HShowerTrack::clear()
{
   m_nAddress = -1;
   m_nTrack = -1;
 
   return 1;
}

