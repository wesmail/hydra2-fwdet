
#ifndef EMCDEF_H
#define EMCDEF_H

#include "haddef.h"

const Cat_t catEmcRaw     = EMC_OFFSET;
const Cat_t catEmcCal     = EMC_OFFSET+1;
const Cat_t catEmcCluster = EMC_OFFSET+2;

const Int_t emcMaxRows       = 15;   // maximum number of rows
const Int_t emcMaxColumns    = 17;   // maximum number of columns
                                     // cell = row*maxColumns + column
const Int_t emcMaxComponents = 255;  // =15*17

#endif /* !EMCDEF_H */
