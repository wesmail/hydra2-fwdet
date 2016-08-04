#ifndef  HTOFCLUSTERF_H
#define  HTOFCLUSTERF_H

using namespace std;
#include "htofhitf.h"
#include <iostream> 
#include <iomanip>
#include "hlocation.h"
#include "TMath.h"

class HCategory;
class HIterator;
class HTofGeomPar;
class HSpecGeomPar;
class HTofClusterFPar;
class HSpectrometer;
class HRuntimeDb;
class HTofCluster;

class HTofClusterF : public HReconstructor {
private:
  HTofHit *hit;
  HTofCluster *cluster;
  Float_t tof_ph, xposMod_ph;
  Float_t tof_h, xposMod_h, edep_h, xposModAdc_h, lAmp_h, rAmp_h, xlab_h, ylab_h, zlab_h;
  Int_t   flagAdc_h;
  Float_t absTd_h, absXd_h;
  Float_t tof_c, xposMod_c, edep_c, xposModAdc_c, lAmp_c, rAmp_c, xlab_c, ylab_c, zlab_c;
  Int_t   flagAdc_c, size_c;

protected:
  HCategory* fHitCat;  // Pointer to the hit category
  HCategory* fClusterCat;  // Pointer to the cluster category
  HLocation fLoc;       // Location of hit
  HLocation fpLoc;      // Location of previous hit
  HLocation fCLoc;      // Location of cluster
  HIterator* iterh;     // Iterator through hit category.
  HIterator* iterc;     // Iterator through cluster category.
  HTofGeomPar *fTofGeometry; // Container for TOF geometry.
  HSpecGeomPar *fSpecGeometry; // Container for Spectrometer geometry.
  HTofClusterFPar *fClusterFPar; // Clusterfinder parameter container.
  Int_t        indexHit1;        //! linear index of first tofhit
  Int_t        indexHit2;        //! linear index of second tofhit

  void writeProb(void);
  Float_t calcProb(Float_t edep,const Char_t* aset="all");
  void fillPreviousData(void);
  void fillClusterData(void);
  void readHit(void);
  void writeCluster(Int_t mode);
  void calcWeightedMean();

public:
  HTofClusterF(void);
  HTofClusterF(const Text_t *name,const Text_t *title);
  ~HTofClusterF(void);
  Bool_t init(void);
  void initParContainer(HSpectrometer *,HRuntimeDb *);
  Bool_t finalize(void) {return kTRUE;}
  Int_t execute(void);
  ClassDef(HTofClusterF,0) //Finds TOF clusters.
};

#endif /* !HTOFCLUSTERF_H */





