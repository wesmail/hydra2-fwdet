#ifndef HFWDETRPCCALSIM_H
#define HFWDETRPCCALSIM_H

#include "hfwdetrpccal.h"

#include "TObject.h"

class HFwDetRpcCalSim : public HFwDetRpcCal
{
public:
    HFwDetRpcCalSim();
    virtual ~HFwDetRpcCalSim();

    ClassDef(HFwDetRpcCalSim, 1);
};

#endif /* !HFWDETRPCCALSIM_H */
