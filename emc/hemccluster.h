#ifndef HEMCCLUSTER_H
#define HEMCCLUSTER_H

#include "TObject.h"
#include "hemccalsim.h"

class HEmcCluster : public TObject {
protected:
  Char_t  sector;         // sector number (0...5)
  UChar_t cell;           // cell which has maximal energy deposit (0...254)
  Short_t index;          // cluster index in category catEmcCluster
  UChar_t ncells;         // number of cells in the cluster
  Float_t energy;         // energy deposited in the cluster
  Float_t cellEnergy;     // energy deposited in the tower (cell) which has maximal energy deposit
  Float_t time;           // mean time weighted by energy
  Float_t sigmaEnergy;    // error of "energy"
  Float_t sigmaTime;      // error of "time"  
  Float_t xmod;           // X coordinate in Module system [mm]
  Float_t ymod;           // Y coordinate in Module system [mm]
  Float_t sigmaXYmod;     // error of Xmod (Ymod)  (these values are the same)
  Float_t xlab;           // X coordinate in Lab system [mm]
  Float_t ylab;           // Y coordinate in Lab system [mm]
  Float_t zlab;           // Z coordinate in Lab system [mm]
  Float_t theta;          // theta angle [degrees]
  Float_t phi;            // phi angle [degrees]
  UInt_t flags;           // user's bit flag                  ... |6|5|4|3|2|1|
                          //                                               s a    a=active  s=sorter flag kIsUsed in HParticleCand
  UChar_t cellList[24];   // list of cells in cluster
  
  Short_t rpcIndex;       // index of correlated HRpcCluster
  UChar_t nMatchedCells;  // number of EMC cells matched with RPC
  UChar_t nMatchedTracks; // number of matched tracks (HParticleCand)
  Float_t qualityDThDPh;  // quality of anglular matching with RPC: it is two dimensional taking into account
                          // the difference between two angles divided by anglular error
  Float_t qualityDTime;   // quality of time matching with RPC: the difference between times divided by time error
  
public:
  HEmcCluster(void) :
    sector(-1),
    cell (0),
    index(-1),
    ncells(0),
    energy(0.),
    cellEnergy(0.),
    time(0.),
    sigmaEnergy(0.),
    sigmaTime(0.),
    xmod(0.),
    ymod(0.),
    sigmaXYmod(0.),
    xlab(0.),
    ylab(0.),
    zlab(0.),
    theta(0.),
    phi(0.),
    flags(1),
    rpcIndex(-1),
    nMatchedCells(0),
    nMatchedTracks(0),
    qualityDThDPh(-100.),
    qualityDTime (-100.)
  {
    memset(cellList,0,sizeof(cellList));
  }
  ~HEmcCluster(void){}

  // Functions getVariable  
  Char_t getSector(void)          const         {return sector;}
  UChar_t getCell(void)           const         {return cell;}
  Short_t getIndex(void)          const         {return index;}
  Float_t getEnergy(void)         const         {return energy;}
  Float_t getTime(void)           const         {return time;}
  Float_t getSigmaEnergy(void)    const         {return sigmaEnergy ;}
  Float_t getSigmaTime(void)      const         {return sigmaTime;}
  Float_t getMaxEnergy(void)      const         {return cellEnergy;}
  Float_t getXMod(void)           const         {return xmod;}
  Float_t getYMod(void)           const         {return ymod;}
  Float_t getSigmaXMod(void)      const         {return sigmaXYmod;}
  Float_t getSigmaYMod(void)      const         {return sigmaXYmod;}
  Float_t getXLab(void)           const         {return xlab;}
  Float_t getYLab(void)           const         {return ylab;}
  Float_t getZLab(void)           const         {return zlab;}
  void    getXYZLab(Float_t& x,Float_t& y,Float_t& z) {x=xlab; y=ylab; z=zlab;}
  Float_t getTheta()              const         {return  theta;}
  Float_t getPhi()                const         {return  phi;}
  UChar_t getCell(Int_t ind)      const;
  UChar_t getNCells(void)         const         {return ncells<=25 ? ncells : 25;}
  UChar_t getTotalNCells(void)    const         {return ncells;}
  Bool_t  ifActive(void)          const         {return flags & 0x01;     }
  Bool_t  isUsedInParticleCand(void) const      {return (flags>>1) & 0x01;}
  
  // Matching with rpc
  Short_t getRpcIndex(void)       const         {return rpcIndex;}
  Float_t getQualDThDPh(void)     const         {return qualityDThDPh;}
  Float_t getQualDTime(void)      const         {return qualityDTime;}
  UChar_t getNMatchedCells(void)  const         {return nMatchedCells;}
  UChar_t getNMatchedTracks(void) const         {return nMatchedTracks;}
  
  void    setRpcIndex(Short_t i)                {rpcIndex = i;}
  void    setQualDThDPh(Float_t q)              {qualityDThDPh = q;}
  void    setQualDTime(Float_t q)               {qualityDTime  = q;}
  void    setNMatchedCells(UChar_t n)           {nMatchedCells = n;}
  
  void    addMatchedTrack(void)                 {if(nMatchedTracks < 255) nMatchedTracks++;}
 
  // Functions setVariable
  void    setSector(Char_t s)                   {sector      = s;}
  void    setIndex(Short_t ind)                 {index       = ind;}
  void    setEnergy(Float_t e)                  {energy      = e;}
  void    setTime(Float_t t)                    {time        = t;}
  void    setSigmaEnergy(Float_t e)             {sigmaEnergy = e;}
  void    setSigmaTime(Float_t t)               {sigmaTime   = t;}
  void    setMaxEnergy(Float_t e)               {cellEnergy  = e;}
  void    setXYMod(Float_t x,Float_t y)         {xmod=x; ymod=y;}
  void    setSigmaXYMod(Float_t sigma)          {sigmaXYmod=sigma;}
  void    setXYZLab(Float_t x,Float_t y,Float_t z) {xlab=x; ylab=y; zlab=z;}
  void    setTheta(Float_t a)                   {theta       = a;}
  void    setPhi(Float_t a)                     {phi         = a;}
  void    setCellList(Int_t ncs,UChar_t* list);
  void    setInactive()                         { flags &= ~( 0x01 << 0 ); }
  void    setIsUsedInParticleCand(void)         { flags |=  ( 0x01 << 1);  }
  void    unsetIsUsedInParticleCand(void)       { flags &= ~( 0x01 << 1 ); }

  ClassDef(HEmcCluster,1) //EMC cal data class
};

#endif /* !HEMCCLUSTER_H */
