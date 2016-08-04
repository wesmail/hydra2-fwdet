#ifndef HWALLDIGITIZER_H
#define HWALLDIGITIZER_H
using namespace std;
#include "hreconstructor.h"
#include <iostream> 
#include <iomanip>
#include "hlocation.h"

//Last modified 7.4.2005 by F.Krizek
// Modified by M.Golubeva 01.11.2006
// Modified by M.Golubeva 22.12.2006

class HIterator;
class HCategory;
class HWallDigiPar;
class HWallGeomPar;

class HWallDigitizer : public HReconstructor {

private:
  HLocation fLoc;                       //! Location for new object
  HCategory* fGeantCat;                 //! Pointer to Geant data category
  HCategory* fRawCat;                   //! Pointer to raw data category
  HWallDigiPar *pWallDigiPar;            //! Digitization parameters
  HWallGeomPar *pWallGeomPar;         //!Container for WALL geometry 
  HIterator* iterGeant;                 //! Iterator over GeantCell category
  HIterator* iterWallRaw;                // Iterator over HWallRawSim category
public:
  HWallDigitizer(void);
  HWallDigitizer(const Text_t* name,const Text_t* title);
  ~HWallDigitizer(void);
  void initParContainer();
  Bool_t init(void);
  Bool_t finalize(void) {return kTRUE;}
  Int_t execute(void);

public:
  ClassDef(HWallDigitizer,0) //Digitizer of WALL data.
};

#endif









