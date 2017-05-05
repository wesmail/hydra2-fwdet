#ifndef HEMCCLUSTERF_H
#define HEMCCLUSTERF_H
using namespace std;
#include "hreconstructor.h"
#include <iostream> 
#include "hlocation.h"
#include "emcdef.h" 

class HIterator;
class HCategory;
class HGeomVector;
class HEmcCal;
class HRpcCluster;

class HEmcClusterF : public HReconstructor {

private:
  HLocation  fLoc;                               // Location of a new object
  HCategory* fEmcCalCat;                         // Pointer to EmcCal data category
  HCategory* fClusterCat;                        // Pointer to Cluster data category 
  HCategory* fRpcCat;                            // Pointer to Rpc data category
  
  Float_t      cellXmod[emcMaxComponents];       // X coordinate of cells in Module system [mm]
  Float_t      cellYmod[emcMaxComponents];       // Y coordinate of cells in Module system [mm]
  HGeomVector* emcCellsLab[6][emcMaxComponents]; // Centre of input plane module (cell) [mm]
  Float_t      thetaEmcLab[6][emcMaxComponents]; // Theta of "emcCellsLab" point in lab.sys. [deg]
  Float_t      sigmaTheta[6][emcMaxComponents];  // Sigma of the theta  [deg]
  Float_t      phiEmcLab[6][emcMaxComponents];   // Phi of "emcCellsLab" point in lab.sys. [deg]
  Float_t      sigmaPhi[6][emcMaxComponents];    // Sigma of the phi    [deg]
  Float_t      sigmaXYmod;                       // =92./sqrt(12.)

  // For all cells in sector:
  Float_t      energy[emcMaxComponents];         // [cell] For all hits in the sector
  Char_t       flagUsed[emcMaxComponents];       // [cell] For all hits in the sector
  HEmcCal*     pSecECells[emcMaxComponents];     // [cell] All hits in the sector
  
  // For one cluster:
  UChar_t      listClustCell[emcMaxComponents];  // [index] For one cluster
  HEmcCal*     pClustCells[emcMaxComponents];    // [index] For one cluster
 
  // Matching with RPC parameters
  Float_t      dThetaSigOffset;                  // (thetaEmc-thetaRpc)/sigmaDTheta+dThSigOffset
  Float_t      dThetaScale;                      // dTheta = dThetaSigOffset/dThetaScale
  Float_t      dTimeOffset;                      // [ns] (time1 - timeRpcN + dTimeOffset)/sigmaDTof;
  Float_t      dTimeCut;                         // Nsigma cut for matching by time
  Float_t      dThdPhCut;                        // Nsigma cut for matching by angles
  // Cluster finder parameters
  Float_t      cellToCellSpeed;                  // [ns/cell] speed of signal distribution from one cell to another
  Float_t      distOffset;                       //
  Float_t      timeCutMin;                       // 
  Float_t      timeCutMax;                       //
  
  Bool_t       isSimulation;                     //
public:
  HEmcClusterF(void);
  HEmcClusterF(const Text_t* name,const Text_t* title);
  ~HEmcClusterF(void);
  //void initParContainer();
  Bool_t init(void);
  Bool_t reinit(void);
  Bool_t finalize(void) {return kTRUE;}
  Int_t execute(void);
 
protected:
  void         initData(void);
  HRpcCluster* rpcMatch(HEmcCal* cal,Float_t &qualityDThDPh,Float_t &qualityDTime);
  Int_t        maxEnergyCell(void) const;
  Float_t      calcDist(HEmcCal *cal1,HEmcCal *cal2) const;
  Int_t        getNearbyCell(Int_t cell,Int_t i) const;
  static bool  cmpEnergy(pair<Int_t,Float_t> p1,pair<Int_t,Float_t> p2) {return p1.second > p2.second;}
  
public:
  ClassDef(HEmcClusterF,0)   // ClusterF of EMC data
};

#endif
