#ifndef FWDETDEF_H
#define FWDETDEF_H

#include "Rtypes.h"
#include "haddef.h"

const Cat_t catFwDetStrawCal        = FWDET_OFFSET+1;
const Cat_t catFwDetScinCal         = FWDET_OFFSET+2;
const Cat_t catFwDetRpcCal          = FWDET_OFFSET+3;
const Cat_t catFwDetRpcHit          = FWDET_OFFSET+4;
const Cat_t catVectorCand           = FWDET_OFFSET+5;

const Int_t FWDET_STRAW_MAX_MODULES   = 2;    // max number of modules (stations)
const Int_t FWDET_STRAW_MAX_LAYERS    = 4;    // max number of layers per module
const Int_t FWDET_STRAW_MAX_PLANES    = 2;    // max number of straws in a block
const Int_t FWDET_STRAW_MAX_CELLS     = 112;  // max number of straws in a block
const Int_t FWDET_STRAW_MAX_SUBCELLS  = 4;    // max number of straws in a block

const Int_t FWDET_STRAW_MAX_VPLANES   =   // max number of virtual planes
    FWDET_STRAW_MAX_MODULES*FWDET_STRAW_MAX_LAYERS*FWDET_STRAW_MAX_PLANES;

const Int_t FWDET_SCIN_MAX_MODULES    = 1;
const Int_t FWDET_SCIN_MAX_CELLS      = 100;

const Int_t FWDET_RPC_MODULE_MIN      = 6;
const Int_t FWDET_RPC_MODULE_MAX      = 6;
const Int_t FWDET_RPC_MAX_MODULES     = 2;
const Int_t FWDET_RPC_MAX_LAYERS      = 2;
const Int_t FWDET_RPC_MAX_CELLS       = 2;
const Int_t FWDET_RPC_MAX_SUBCELLS    = 6;

const Int_t FWDET_RPC_MAX_STRIPS      = 32;
const Int_t FWDET_RPC_MAX_HITS        = 2;

const Int_t FWDET_MAX_SECTORS         = -1;
const Int_t FWDET_MAX_MODULES         = 9;
const Int_t FWDET_MAX_COMPONENTS      = 9;

#endif /* !FWDETDEF_H */
