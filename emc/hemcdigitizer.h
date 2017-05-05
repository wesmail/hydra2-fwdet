#ifndef HEMCDIGITIZER_H
#define HEMCDIGITIZER_H

using namespace std;
#include "hreconstructor.h"
#include "hgeomtransform.h"
#include "hlocation.h"
#include "emcdef.h"
#include <vector>

class HIterator;
class HCategory;
class HLinearCategory;
class HEmcCal;
class HEmcDetector;
class HEmcGeomPar;
class HEmcDigiPar;
class HEmcSimulPar;
class HGeantEmc;

class HEmcDigitizer : public HReconstructor {
protected:

  //---------------------------------------------------------------
  // define some auxiliary structures
  typedef struct celltrack { // stores all needed infos per cell
    Float_t gtime;           // GEANT time (used for sorting)
    Int_t   gtrack;          // GEANT track number
    Float_t trackEn;         // number of photo electrons

    // Reset initial values
    void reset() {
      gtime      =  10000.;
      gtrack     = -1;
      trackEn    = 0;
    }

    Bool_t static cmpTime(celltrack* a, celltrack* b) {
      // sort by GEANT time in increasing order
      return (a->gtime < b->gtime);
    }
    Bool_t static cmpEnergy(celltrack* a, celltrack* b) {
      // sort by GEANT time in increasing order
      return (a->trackEn > b->trackEn);
    }
  } celltrack;

  typedef struct {  // collects all tracks per cell
    Float_t            energy;         // number of photo electrons
    Bool_t             isEmbeddedReal; // kTRUE - real data cell
    vector<celltrack*> ctracks;        // all tracks in the cell
    vector<Int_t>      inputTracks;    // tracks in the cell ("inputTracks" is not always equal to "ctracks")

    void reset() { // Resets initial values, deletes vector
      energy         = 0.;
      isEmbeddedReal = kFALSE;
      for (UInt_t i=0;i<ctracks.size();i++) { delete ctracks[i]; }
      inputTracks.clear();
      ctracks.clear();
    }

    // Sorts by increasing GEANT time
    void sortTime(void) {
      std::sort(ctracks.begin(),ctracks.end(),celltrack::cmpTime);
    }

    void sortEnergy(void) {
      std::sort(ctracks.begin(),ctracks.end(),celltrack::cmpEnergy);
    }

  } celldata;
  
  vector<celldata* > cellobjects; // temporary working array for digitized data
  void clearCellobjects();

  //---------------------------------------------------------------
  
  Int_t            embeddingmode;    //  switch for keeping geant hits / realistic embedding

  HLocation        fLoc;             // Location for new object
  HCategory*       fGeantEmcCat;     // GeantEmc data category
  HLinearCategory* fGeantKineCat;    // GeantKine data category
  HCategory*       fCalCat;          // Cal data category
  HCategory*       fStartHitCat;     // StartHit data category
  HEmcDetector*    fEmcDet;          // EmcDetector
  HEmcGeomPar*     fGeomPar;         // Geometry parameters
  HEmcDigiPar*     fDigiPar;         // Digitization parameters
  HEmcSimulPar*    fSimulPar;        // Simulation parameter: lookup table for PMT types
  HIterator*       iterGeantEmc;     // Iterator over catEmcGeantRaw category
  HGeomTransform   labTrans[6];      // Transfomation lab. <-> emc_module
  
  Int_t            maxCell;          // maximal number of cells (including spares)
  Float_t          sigmaT;           // time resolution
  Float_t          phot2Energy[3];   // mean energy deposit per photo electron for different types of PMT (1000./1306.) [MeV]
  Float_t          facEnergSmear[3]; // factor for energy smearing for different types of PMT
  Double_t         zVertBorder;      // tracks without HGeantEmc hit but with vertex in region zVertBorder (coor.sys. of emc module) 
                                     //  will be analysed as track with HGeantEmc hit
                                     //  Input plane of EMC module corresponds to zVertBorder=0.
  Float_t          energyDepositCut; // Cut for the minimal energy deposit in the cell. It is used for time1 determination.
  Float_t          signalVelocity;   // Velocity of the signal propagation along cell (default: speed of light in the vacuum)
  Float_t          halfOfCellLength; // It is needed because HGeanEmc hits now are shifted by half of cell relative coor.system of EMC !
  
public:
  HEmcDigitizer(void);
  HEmcDigitizer(const Text_t* name,const Text_t* title);
  ~HEmcDigitizer(void);
  Bool_t     init(void);
  Bool_t     reinit(void);
  Bool_t     finalize(void) {return kTRUE;}
  Int_t      execute(void);
  void       setzVertBorder(Double_t vc)       {zVertBorder      = vc;}
  void       setEnergyDepositCut(Double_t cut) {energyDepositCut = cut;}
  void       setSignalVelocity(Double_t v)     {signalVelocity   = v;}

  void       setEmbeddingMode(Int_t mode)      {embeddingmode    = mode;}
  Int_t      getEmbeddingMode(void) const      {return embeddingmode;}
  
  
protected:
  void       initVariables(void);
  Bool_t     setParameterContainers(void);
  Int_t      cellObjectsSize(void) const             {return 6 * emcMaxComponents;}     // size of "cellobjects" vector
  Int_t      cellObjectIndex(Int_t s,Int_t c) const  {return s * emcMaxComponents +c;}  // sector&cell  ==>  index in "cellobjects" vector 
  Int_t      sectorFromIndex(Int_t ind) const        {return ind / emcMaxComponents;}   // index in "cellobjects" vector  ==>  sector
  Int_t      cellFromIndex(Int_t ind) const          {return ind % emcMaxComponents;}   // index in "cellobjects" vector  ==>  cell
  HGeantEmc* getInputHit(HGeantEmc* pGeantEmc,Int_t &inputTrack) const;

  ClassDef(HEmcDigitizer,0) //Digitizer of EMC data.
};

#endif
