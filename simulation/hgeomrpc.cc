//*-- AUTHOR : Ilse Koenig
//*-- Created : 09/05/2006

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
// HGeomRpc
//
// Class for geometry of RPC
/////////////////////////////////////////////////////////////

#include "hgeomrpc.h"
#include "hgeomnode.h"
#include "hgeommedia.h"
#include "hgeomrpchit.h"

ClassImp(HGeomRpc)

HGeomRpc::HGeomRpc() {
  // Constructor
  fName="rpc";
  maxSectors=6;
  maxModules=1;
  strcpy(modName,"EBOX");
  strcpy(eleName,"E");
  pHit=new HGeomRpcHit(this);
}
