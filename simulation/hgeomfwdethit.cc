//*-- AUTHOR : Ilse Koenig
//*-- Created : 27/11/2015

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////
// HGeomFwDetHit
//
// Class for GEANT hit definition of the new forward detectors
//
//////////////////////////////////////////////////////////////

#include "hgeomfwdethit.h"
#include "hgeomfwdet.h"
#include "hgeomnode.h"

ClassImp(HGeomFwDetHit)

HGeomFwDetHit::HGeomFwDetHit(HGeomFwDet* p) : HGeomHit(p)
{
    // Constructor
}

Int_t HGeomFwDetHit::getIdType()
{
    // Returns the idType for the current node
    if (!currentNode) return 0;
    const Char_t* volumeName=currentNode->GetName();
    Int_t l1=(Int_t)(volumeName[1]-'0');
    return 760 + l1;
}
