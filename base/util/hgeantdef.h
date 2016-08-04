#ifndef HGEANTDEF_H
#define HGEANTDEF_H

#include "haddef.h"

// GEANT identifiers
const Cat_t catGeantHeader=SIMUL_OFFSET;
const Cat_t catGeantKine=SIMUL_OFFSET+1;
const Cat_t catMdcGeantRaw=SIMUL_OFFSET+2;
const Cat_t catRichGeantRaw=SIMUL_OFFSET+3;
const Cat_t catShowerGeantRaw=SIMUL_OFFSET+6;  // RICH needs 3 categories, skip 2 !
const Cat_t catTofGeantRaw=SIMUL_OFFSET+7;
const Cat_t catStartGeantRaw=SIMUL_OFFSET+8;
const Cat_t catWallGeantRaw=SIMUL_OFFSET+9;
const Cat_t catRpcGeantRaw=SIMUL_OFFSET+10;
const Cat_t catEmcGeantRaw=SIMUL_OFFSET+11;
const Cat_t catFwDetGeantRaw=SIMUL_OFFSET+12;

#endif
