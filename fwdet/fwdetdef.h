#ifndef FWDETDEF_H
#define FWDETDEF_H

#include "haddef.h"

const Cat_t catFwDetStrawCal        = FWDET_OFFSET+1;
const Cat_t catFwDetScinCal         = FWDET_OFFSET+2;
const Cat_t catFwDetRpcCal          = FWDET_OFFSET+3;
const Cat_t catFwDetStrawVector     = FWDET_OFFSET+4;

// FIXME preliminary!!!!!!!!!!!!!!!
const int FWDET_STRAW_MAX_MODULES    = 2;
const int FWDET_STRAW_MAX_DOUBLELAYS = 4;
const int FWDET_STRAW_MAX_CELLS      = 120;  // max number of straws in one layer
const int FWDET_SCIN_MAX_MODULES     = 1;
const int FWDET_SCIN_MAX_CELLS       = 100;
const int FWDET_RPC_MAX_MODULES      = 1;
const int FWDET_RPC_MAX_CELLS        = 100;
const int FWDET_MAX_SECTORS          = -1;
const int FWDET_MAX_MODULES          = 9;
const int FWDET_MAX_COMPONENTS       = 9;

#endif /* !FWDETDEF_H */
