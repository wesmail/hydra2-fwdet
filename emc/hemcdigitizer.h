#ifndef HEMCDIGITIZER_H
#define HEMCDIGITIZER_H

using namespace std;
#include "hreconstructor.h"
#include <iostream> 
#include <iomanip>
#include "hlocation.h"
#include "TRandom2.h"
#include <vector>
#include <algorithm>

class HIterator;
class HCategory;
class HEmcCal;
class HEmcDetector;
class HEmcGeomPar;
class HEmcDigiPar;

class HEmcDigitizer : public HReconstructor {
protected:

  //---------------------------------------------------------------
  // define some auxiliary strutures
  typedef struct celltrack { // stores all needed infos per cell
    Float_t gtime;           // GEANT time (used for sorting)
    Int_t   gtrack;          // GEANT track number

    // Reset initial values
    void reset() {
      gtime     =  10000.;
      gtrack    = -1;
    }

    Bool_t static cmpTime(celltrack* a, celltrack* b) {
      // sort by GEANT time in increasing order
      return (a->gtime < b->gtime);
    }
  } celltrack;

  typedef struct { // collection of all tracks per cell
    Char_t  sec;   // sector
    UChar_t cell;  // cell
    Int_t   npe;   // number of photo electrons

    vector<celltrack*> ctracks; // all cell tracks

    void reset() { // Reset initial values. deletes vector
      sec  = -1;
      cell = 0;
      npe  = 0;
      for (UInt_t i=0;i<ctracks.size();i++) { delete ctracks[i]; }
      ctracks.clear();
    }

    // Sort by increasing GEANT time
    void sortTime(void) {
      std::sort(ctracks.begin(),ctracks.end(),celltrack::cmpTime);
    }

    // Get the smallest TOF of all tracks
    Float_t getSmallestTof(void) {
      Float_t time = 10000.F;
      for(UInt_t i=0;i<ctracks.size();i++) {
        if (ctracks[i]->gtime < time) time = ctracks[i]->gtime;
      } 
      return time;
    }
  } celldata;

  vector<celldata* > cellobjects; //! temporary working array for digitized data
  void clearCellobjects();

  //---------------------------------------------------------------

  HLocation fLoc;            // Location for new object
  HCategory* fGeantEmcCat;   // Pointer to Emc Geant data category
  HCategory* fCalCat;        // Pointer to Cal  data category
  HEmcDetector* fEmcDet;     // Pointer to EmcDetector
  HEmcGeomPar* fGeomPar;     // Geometry parameters 
  HEmcDigiPar* fDigiPar;     // Digitization parameters
  HIterator* iterGeantEmc;   // Iterator over catEmcGeantRaw category
  TRandom2* randGen;         // Random generator for time and energy smearing
  
  Int_t   maxCell;           // maximum number of cells (including spares)
  Float_t sigmaT;            // time resolution
  Float_t phot2E;            // mean energy deposition per photon electron (1000./1306.) in MeV
  Float_t facESmear;         // factor for energy smearing
  
public:
  HEmcDigitizer(void);
  HEmcDigitizer(const Text_t* name,const Text_t* title);
  ~HEmcDigitizer(void);
  Bool_t init(void);
  Bool_t reinit(void);
  Bool_t finalize(void) {return kTRUE;}
  Int_t  execute(void);

protected:
  void   initVariables(void);
  void   npe2energy(Float_t npe, HEmcCal* cal);
  Bool_t setParameterContainers(void);
  
  ClassDef(HEmcDigitizer,0) //Digitizer of EMC data.
};

#endif
