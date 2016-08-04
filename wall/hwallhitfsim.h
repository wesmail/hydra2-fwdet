#ifndef HWALLHITFSIM_H
#define HWALLHITFSIM_H
using namespace std;
#include "hreconstructor.h"
#include <iostream> 
#include <iomanip>
#include "hlocation.h"
#include "hwallhit.h"  
#include "hwallhitf.h"  
#include "hwallhitsim.h"  
#include "hwalldigipar.h"  
#include "hwallgeompar.h"
#include "hspecgeompar.h"

class HIterator;
class HCategory;
class HWallDigiPar;
class HWallGeomPar;
class HSpecGeomPar;
class HRuntimeDb;
class HWallHitF;
class HWallHitSim;
class HWallRawSim;


class HWallHitFSim : public HWallHitF {
//class HWallHitFSim : public HReconstructor {
protected:
  //private://FK//
  HLocation fLoc;             //! Location for new object
  HCategory* fRawCat;         //! Pointer to raw data category
  HCategory* fHitCat;         //FK//???????????????????????????
  HIterator* iter;            //! Iterator over Raw category
  HWallDigiPar *pWallDigiPar; //! Calibration parameters 
  HWallGeomPar *pWallGeometry; // Geom parameters 
  HSpecGeomPar *pSpecGeometry;
  Bool_t fWallSimulation;      //FK//

  void fillHitTrack(HWallHitSim *, HWallRawSim *);

  virtual void fillHit(HWallHitSim *, HWallRawSim *);
  void fillGeometry(HWallHitSim *);
public:
  HWallHitFSim(void);
  HWallHitFSim(const Text_t* name,const Text_t* title);
  ~HWallHitFSim(void);
  void initParContainer();
  Bool_t init(void);
  Bool_t finalize(void) {return kTRUE;}
  Int_t execute(void);
 
  ClassDef(HWallHitFSim,0) // Calibrater for Wall hit data
};

#endif
