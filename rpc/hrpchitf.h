#ifndef HRPCHITF_H
#define HRPCHITF_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HIterator;
class HRpcGeomPar;
class HRpcGeomCellPar;
class HRpcCellStatusPar;
class HRpcDigiPar;
class HRpcWtoQPar;
class HRpcHitFPar;
class HSpecGeomPar;
class HRpcTimePosPar;
class HRpcSlewingPar;

class HRpcHitF : public HReconstructor {
protected:
  HCategory *pCalCat;                // pointer to the cal data
  HCategory *pCalCatTmp;             // pointer to the calTmp data for embedding
  HCategory *pHitCat;                // pointer to the hit data
  HCategory *pGeantRpcCat;           // pointer to the GeantRpc data
  HCategory *pStartHitCat;           // pointer to the Start Hit data
  HRpcDigiPar *pDigiPar;             // Digitization parameters
  HRpcWtoQPar *pWtoQPar;             // Width to Charge conversion parameters
  HRpcHitFPar *pHitFPar;             // Hit Finder parameters
  HIterator *iter;                   // iterator on cal data
  HSpecGeomPar* pSpecGeomPar;        // pointer to official geometry parameters
  HRpcGeomPar* pRpcGeometry;         // rpc geometry parameters from official geometry
  HRpcGeomCellPar* pGeomCellPar;     // rpc cell geometry parameters
  HRpcCellStatusPar* pCellStatusPar; // cell status parameters
  HRpcTimePosPar* pTimePosPar;       // Time-position dependence correction parameters
  HRpcSlewingPar* pSlewingPar;       // Time-charge dependence (slewing) correction pars
  HLocation loc;                     // location

  Bool_t simulation;                 // Flag to decide whether simulation(1) or analisis(0)

public:
  HRpcHitF();
  HRpcHitF(const Text_t* name,const Text_t* title);
  ~HRpcHitF();
  void initParContainer();
  Bool_t init();
  Bool_t finalize() { return kTRUE; }
  Int_t execute();
public:
  ClassDef(HRpcHitF,0) // Hit Finder cal->hit for RPC data
};

#endif /* !HRPCHITF_H */

