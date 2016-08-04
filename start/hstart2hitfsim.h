#ifndef HSTART2HITFSIM_H
#define HSTART2HITFSIM_H

#include "hreconstructor.h"
#include "hgeomvector.h"
#include <vector>
#include <algorithm>

class HCategory;
class HIterator;
class HCategory;
class HStart2Hit;
class HStart2GeomPar;
class HStart2DigiPar;
class HSpecGeomPar;

class HStart2HitFSim : public HReconstructor {
private:
  //---------------------------------------------------------------
  // define some auxiliary strutures
  typedef struct celltrack { // stores all needed infos per cell
    Float_t gtime;           // GEANT time (used for sorting)
    Int_t   gtrack;          // GEANT track number
    Float_t geloss;          // GEANT energy loss

    // Reset initial values
    void reset() {
      gtime  =  10000.;
      gtrack = -1;
      geloss = 0.f;
    }

    Bool_t static cmpTime(celltrack* a, celltrack* b) {
      // sort by GEANT time in increasing order
      return (a->gtime < b->gtime);
    }
  } celltrack;

  typedef struct {
    // collection of all tracks per cell

    vector<celltrack*> ctracks; // all cell tracks

    void reset() { // Reset initial values. deletes vector
      for (UInt_t i=0;i<ctracks.size();i++) { delete ctracks[i]; }
      ctracks.clear();
    }

    // Sort by increasing GEANT time
    void sortTime(void) {
      std::sort(ctracks.begin(),ctracks.end(),celltrack::cmpTime);
    }

  } celldata;

  vector<celldata* > cellobjects; //! temporary working array for digitized data
  void clearCellobjects();
  //---------------------------------------------------------------

   HCategory*      fCatHit;         //! pointer to the hit data
   Double_t        fresolution;     //! time resolution in ns
   HCategory*      fCatGeantStart;  //! pointer to Start Geant data category
   HIterator*      iterGeantStart;  //! iterator over catStartGeantRaw category
   HCategory*      fCatGeantKine;   //! pointer to Geant kine category
   HStart2GeomPar* fGeomPar;        //! geometry parameters of Start detector
   HStart2DigiPar* fDigiPar;        //! digitization parameters
   HSpecGeomPar*   fSpecGeomPar;    //! pointer to HSpecGeomPar parameter container
   HGeomVector     targetCenter;    //! Lab position of target center
static  HStart2HitFSim* fHitFinder;  //!
static  Bool_t     doGeantHit;       //! switch (default true) : geant hit will be used.
                                     //  set kFALSE if geant input has start cat , but should not be used
public:
   HStart2HitFSim(void);
   HStart2HitFSim(const Text_t* name, const Text_t* title,Double_t resol=0.06);

   ~HStart2HitFSim(void);
   static HStart2HitFSim* getHitFinder(){ return fHitFinder;}
   static void setDoGeantHit(Bool_t doit) { doGeantHit = doit; }
   void   setResolution(Double_t res) { fresolution = res ; }

   Bool_t init(void);
   Bool_t reinit(void);
   Int_t  execute(void);
   Bool_t finalize(){return kTRUE;}

private:
   void   initVar(void);
   Bool_t setParameterContainers(void);
   void   digitize(HStart2Hit*);

public:
   ClassDef(HStart2HitFSim, 0) // Hit finder for START2 detector
};

#endif /* !HSTART2HITFSIM_H */

