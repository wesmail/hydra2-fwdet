#ifndef FWDETDEF_H
#define FWDETDEF_H

#include "Rtypes.h"
#include "haddef.h"

const Cat_t catFwDetStrawCal        = FWDET_OFFSET+1;
const Cat_t catFwDetScinCal         = FWDET_OFFSET+2;
const Cat_t catFwDetRpcCal          = FWDET_OFFSET+3;
const Cat_t catFwDetRpcHit          = FWDET_OFFSET+4;
const Cat_t catVectorCand           = FWDET_OFFSET+5;

const int FWDET_STRAW_MAX_MODULES   = 2;    // max number of modules (stations)
const int FWDET_STRAW_MAX_LAYERS    = 4;    // max number of layers per module
// physical geometry
const int FWDET_STRAW_MAX_PANELS    = 7;    // max number of panels in a layer
const int FWDET_STRAW_MAX_BLOCKS    = 4;    // max number of blocks in a panel
const int FWDET_STRAW_MAX_STRAWS    = 8;    // max number of straws in a block
// virtual geometry
const int FWDET_STRAW_MAX_PLANES    = 2;    // max number of straws in a block
const int FWDET_STRAW_MAX_CELLS     = 120;  // max number of straws in a block

const int FWDET_STRAW_MAX_VPLANES   =   // max number of virtual planes
    FWDET_STRAW_MAX_MODULES*FWDET_STRAW_MAX_LAYERS*FWDET_STRAW_MAX_PLANES;

const int FWDET_SCIN_MAX_MODULES    = 1;
const int FWDET_SCIN_MAX_CELLS      = 100;

const int FWDET_RPC_MODULE_MIN      = 6;
const int FWDET_RPC_MODULE_MAX      = 7;
const int FWDET_RPC_MAX_MODULES     = 2;
const int FWDET_RPC_MAX_LAYERS      = 2;
const int FWDET_RPC_MAX_CELLS       = 32;

const int FWDET_RPC_MAX_STRIPS      = 32;
const int FWDET_RPC_MAX_HITS        = 2;

const int FWDET_MAX_SECTORS         = -1;
const int FWDET_MAX_MODULES         = 9;
const int FWDET_MAX_COMPONENTS      = 9;

#endif /* !FWDETDEF_H */
