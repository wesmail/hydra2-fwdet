//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
// HGeomRpcHit
//
// Class for RPC hit definition in GEANT
//
/////////////////////////////////////////////////////////////

#include "hgeomrpchit.h"
#include "hgeomrpc.h"
#include "hgeomnode.h"
#include "TString.h"

ClassImp(HGeomRpcHit)

HGeomRpcHit::HGeomRpcHit(HGeomRpc* p) : HGeomHit(p) {
  // Constructor
}

Int_t HGeomRpcHit::getIdType() {
  // Returns the idType for the current node
  if (!currentNode) return 0;
  const Char_t* volumeName=currentNode->GetName();

  // DetectorID corresponding to the virtual volume that surrounds RPC
  if (volumeName[0]=='E'&&volumeName[1]=='B'&&volumeName[2]=='O'&&volumeName[3]=='X') {
    return 500-1;        
  }

  Int_t l1=(Int_t)(volumeName[2]-'0');          // -'0' is mandatory for getting the number
  Int_t l2=(Int_t)(volumeName[3]-'0');         
 
  if (l2>=17) l2=l2-7;                         // This is how A(17) , b(18), c(19), d(20), is encoded ->> 10,11,12

  return 500 + 35*(l1-1) + (l2-1);
}
