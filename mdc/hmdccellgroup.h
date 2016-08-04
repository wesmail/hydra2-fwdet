#ifndef HMDCCELLGROUP_H
#define HMDCCELLGROUP_H

#include "TObject.h"
#include <string.h>

class HMdcCellGroup : public TObject {
 protected:
  static UChar_t nCellsLookUp[256];         //!
  static UChar_t cellPosLookUp[4][256];     //!
  static Bool_t lookUpsInitialized;         //!
  UShort_t *pLayer; //!pointer to layer information.
  
  void initLookups(void);  
  HMdcCellGroup(UShort_t *layerBuffer) {
    if (!lookUpsInitialized) initLookups();
    pLayer=layerBuffer;
  }
  void printCells(Int_t nlays);
 public:
  ~HMdcCellGroup(void) { }
  inline Int_t getNCells(Int_t layer);
  inline Int_t getCell(Int_t layer,Int_t idx);
  inline Int_t getSignId(Int_t layer,Int_t idx);
  inline void  setSignId(Int_t layer,Int_t cell,Int_t t1,Int_t t2=0,Int_t t3=0,Int_t t4=0);
  inline void  setLayerGroup(Int_t layer,UShort_t cl) {pLayer[layer] = cl;}
  Int_t        getLayerListCells(Int_t layer,Int_t *list);
  
  ClassDef(HMdcCellGroup,1) //A group of cells in layers
};

class HMdcCellGroup6 : public HMdcCellGroup {
 protected:
  UShort_t iLayer[6];
 public:
  HMdcCellGroup6(void) : HMdcCellGroup(iLayer) 
    { memset(iLayer,0,6*sizeof(UShort_t)); }
  ~HMdcCellGroup6(void) {}
  void clear(void) { memset(iLayer,0,6*sizeof(UShort_t)); }
  Int_t getSumWires()
  {
      Int_t sum=0;
      for(Int_t i=0;i<6;i++){sum+=getNCells(i);}
      return sum;
  }
  Int_t getNLayers()
  {
      Int_t sum=0;
      for(Int_t i=0;i<6;i++){if(getNCells(i)>0)sum++;}
      return sum;
  }
  void printCellGroup(void) {printCells(6);}
  ClassDef(HMdcCellGroup6,1) //Group of cells in 6 layers
};

class HMdcCellGroup12 : public HMdcCellGroup {
 protected:
  UShort_t iLayer[12];
 public:
  HMdcCellGroup12(void) : HMdcCellGroup(iLayer)
    { memset(iLayer,0,12*sizeof(UShort_t));}
  ~HMdcCellGroup12(void) {}
  void clear(void) { memset(iLayer,0,12*sizeof(UShort_t)); }
  Int_t getSumWires()
  {
      Int_t sum=0;
      for(Int_t i=0;i<12;i++){sum+=getNCells(i);}
      return sum;
  }
  Int_t getSumWires(UInt_t mod)
  {
      Int_t sum=0;
      if(mod==0)for(Int_t i=0;i<6;i++) {sum+=getNCells(i);}
      if(mod==1)for(Int_t i=6;i<12;i++){sum+=getNCells(i);}
      return sum;
  }
  Int_t getNLayers()
  {
      Int_t sum=0;
      for(Int_t i=0;i<12;i++){if(getNCells(i)>0)sum++;}
      return sum;
  }
  Int_t getNLayers(UInt_t mod)
  {
      Int_t sum=0;
      if(mod==0)for(Int_t i=0;i<6;i++){if(getNCells(i)>0)sum++;}
      if(mod==1)for(Int_t i=6;i<12;i++){if(getNCells(i)>0)sum++;}
      return sum;
  }
  void printCellGroup(void) {printCells(12);}
  
  UChar_t getNSharedCells(HMdcCellGroup12 *gr,Int_t& nLayers,Int_t& nLayOr) {        
      Int_t   sum   = 0;
      UChar_t layor = 0;
      nLayOr  = 0;
      nLayers = 0;
      for(Int_t layer=0; layer<12; layer++) {
        UChar_t nsc = getNSharedCells(pLayer[layer],gr->pLayer[layer]);
        if(nsc == 0) continue;
        sum += nsc;
        nLayers++;
        
        Int_t l = layer < 6 ? layer : layer-6;
        if(l > 2) l--;
        Int_t layInd = 1<<l;
        if((layor & layInd) == 0) {
          nLayOr++;
          layor |= layInd;
        }
      }
      return sum; 
  }
  
  UChar_t getNSharedCells(HMdcCellGroup12 *gr) {   
      Int_t   sum   = 0;
      for(Int_t layer=0; layer<12; layer++) if(pLayer[layer]!=0 && gr->pLayer[layer]!=0) {
        sum += getNSharedCells(pLayer[layer],gr->pLayer[layer]);
      }
      return sum; 
  }
  
  private:

  UChar_t getNSharedCells(UShort_t l1, UShort_t l2) {
    UShort_t c1 = l1 >> 8;
    UShort_t c2 = l2 >> 8;
    UShort_t dc = c1>c2 ? c1-c2 : c2-c1;
    if(dc > 4) return 0;
    if     (c1 > c2) l1 <<= dc<<1;
    else if(c2 > c1) l2 <<= dc<<1;
    return nCellsLookUp[l1 & l2 & 0xFF];
  }

  ClassDef(HMdcCellGroup12,1) //group of cells in 12 layers.
};


//----------------------- Inlines ------------------------------
inline Int_t HMdcCellGroup::getNCells(Int_t layer) {
  return nCellsLookUp[pLayer[layer] & 0xFF];
}

inline Int_t HMdcCellGroup::getCell(Int_t layer,Int_t idx) {
  return ((pLayer[layer]>>8) & 0xFF)+cellPosLookUp[idx][pLayer[layer] & 0xFF];
}

inline Int_t HMdcCellGroup::getSignId(Int_t layer,Int_t idx) {
  //Returns 1 for time1 and 2 for time2 in cell basecell+idx; starting with 
  //idx=0
  if (idx < getNCells(layer) ) {
    Int_t shift=cellPosLookUp[idx][pLayer[layer] & 0xFF]*2;
    return (pLayer[layer] >> shift) & 0x3;
  }
  return 0;
}

inline void HMdcCellGroup::setSignId(Int_t layer,Int_t cell,
			      Int_t t1,Int_t t2,Int_t t3,Int_t t4) {
  //Sets the signal ids and base cell number for layer "layer"
  //Input:
  //  layer --> Number of layer to set
  //  cell  --> Number of the first cell fired in this layer
  //  t1 - t4 --> SignalId for cells: "cell"+0, "cell"+1 ... "cell"+4
  //              Possible values are:
  //                0 --> This cell has not fired
  //                1 --> First signal (time/distance) was used
  //                2 --> Second signal (time/distance) was used
  pLayer[layer]=((cell & 0xFF) << 8) | ((t4 & 0x3)<<6) | 
    ((t3 & 0x3)<<4) | ((t2 & 0x3)<<2) | ((t1 & 0x3));
}

#endif
