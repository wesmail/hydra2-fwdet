#ifndef HMDCLISTCELLS_H
#define HMDCLISTCELLS_H

#include "hmdctbarray.h"
#include "TObject.h"

#include <string.h>

class HMdcList12GroupCells;
class HMdcCellGroup12;

class HMdcLayListCells : public TObject {
  protected:
    enum { arrSize=56,     // array size 
           maxCells=224};  // max.number of cells in layer (= arrSize*4)
    UChar_t  arr[arrSize]; // cells lis (two bits per wire)
    UChar_t* arrEnd;       // pointer of last element in "arr"
    Float_t* drTime1;      // array of drift times (time1!)
    Float_t* drTime2;      // array of drift times (time2!)
    Float_t* tof1;         // array of TOFs (time1!)
    Float_t* tof2;         // array of TOFs (time2!)
    UChar_t* nClust;       // array of cluster counters
    UChar_t* nRClust;      // array of "real" cluster counters
    UChar_t* nFitted;      // array of fitted wires counters
    UChar_t* nRFitted;     // array of "real" fitted wires counters
    Int_t    nFittedTot;   // total amount of fitted wires in layer
    Int_t*   gnTrackNum1;  // array of geant track num. (time1!)
    Int_t*   gnTrackNum2;  // array of geant track num. (time2!)
  public:
    HMdcLayListCells() {init();}
    ~HMdcLayListCells(void);
    void           copyListOfCells(HMdcLayListCells& lst);
    void           clear(void);
    void           clearClustCounter(void);
    void           clearFitWiresCounter(void);
    inline Bool_t  setTime(Int_t cell,UChar_t time);
    inline void    delTime(Int_t cell,UChar_t time);
    inline Int_t   getNCells(void) const;
    inline Int_t   getCell(Int_t idx) const;
    inline Int_t   getFirstCell(void) const;
    inline Int_t   getLastCell(void) const;
    inline Int_t   getGeantTrack(Int_t cell,UChar_t tm) const;
    inline Int_t   next(Int_t prCell) const;
    inline Bool_t  nextCell(Int_t& cell) const;
    Bool_t         nextNonFittedCell(Int_t& cell) const;
    Bool_t         previousNonFittedCell(Int_t& cell) const;
    inline Int_t   previous(Int_t prCell) const;
    inline Bool_t  previousCell(Int_t &cell) const;
    inline UChar_t getTime(Int_t cell) const;
    inline Bool_t  isCell(Int_t cell) const;
    void           print(void) const;
    void           printCellInfo(Int_t cell) const;
    
    inline Bool_t  addTime1(Int_t cell, Float_t timeValue);
    inline Bool_t  nextTime(Int_t& cell, Float_t& timeValue) const;
    inline Float_t getTimeValue(Int_t cell) const;
    Bool_t         addCell(Int_t c,UChar_t tm,Float_t dt1,Float_t dt2);
    Bool_t         addCellSim(Int_t c,UChar_t tm,Float_t dt1,Float_t dt2,
                              Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2);
    UChar_t        nextCell(Int_t& c,Float_t& dt1,Float_t& dt2) const;
    UChar_t        nextCellSim(Int_t& c,Float_t& dt1,Float_t& dt2,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    UChar_t        nextCellSim(Int_t& c,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    Bool_t         increaseClusCount(Int_t cell);
    Bool_t         increaseRClusCount(Int_t cell);
    Bool_t         increaseFittedCount(Int_t cell);
    Bool_t         increaseRFittedCount(Int_t cell);
    Bool_t         increaseFittedAndRealCount(Int_t cell);
    Bool_t         decreaseClusCount(Int_t cell);
    Bool_t         decreaseRClusCount(Int_t cell);
    Bool_t         decreaseFittedCount(Int_t cell);
    Bool_t         decreaseRFittedCount(Int_t cell);
    Bool_t         decreaseFittedAndRealCount(Int_t cell);
    inline UChar_t getNClusters(Int_t cell) const;
    inline UChar_t getNRClusters(Int_t cell) const;
    inline UChar_t getNFitted(Int_t cell) const;
    inline UChar_t getNRFitted(Int_t cell) const;
    Int_t          getNumNotFitted(void) const;            // Return number of not fitted wires

  protected:
    void init(void);
    inline Bool_t boundsOk(Int_t cell) const;

  ClassDef(HMdcLayListCells,0) //List of all cells in layer
};
  
inline Bool_t HMdcLayListCells::boundsOk(Int_t cell) const {
  return cell>=0 && cell < maxCells;
}

inline Bool_t HMdcLayListCells::setTime(Int_t cell,UChar_t time) {
  if(!boundsOk(cell)) return kFALSE;
  HMdcTBArray::set(arr,cell,time);
  return kTRUE;
}

inline void HMdcLayListCells::delTime(Int_t cell,UChar_t time) {
  if(boundsOk(cell)) HMdcTBArray::unset(arr,cell,time);
}

inline Int_t HMdcLayListCells::getNCells(void) const {
  return HMdcTBArray::getNSet(arr,arrEnd);
}

inline Int_t HMdcLayListCells::getCell(Int_t idx) const {
  return HMdcTBArray::position(arr,arrEnd,idx);
}

inline Int_t HMdcLayListCells::getFirstCell(void) const {
  return HMdcTBArray::first(arr,arrEnd);
}

inline Int_t HMdcLayListCells::getLastCell(void) const {
  return HMdcTBArray::last(arr,arrEnd);
}

inline Int_t HMdcLayListCells::next(Int_t prCell) const {
  return HMdcTBArray::next(arr,arrEnd,prCell);
}

inline Bool_t HMdcLayListCells::nextCell(Int_t& cell) const {
  cell = HMdcTBArray::next(arr,arrEnd,cell);
  return cell >= 0;
}

inline Int_t HMdcLayListCells::previous(Int_t prCell) const {
  return HMdcTBArray::previous(arr,arrEnd,prCell);
}

inline Bool_t HMdcLayListCells::previousCell(Int_t &cell) const {
  cell = HMdcTBArray::previous(arr,arrEnd,cell);
  return cell >= 0;
}


inline UChar_t HMdcLayListCells::getTime(Int_t cell) const {
  if(boundsOk(cell)) return HMdcTBArray::get(arr,cell);
  return 0;
}

inline Bool_t HMdcLayListCells::isCell(Int_t cell) const {
  return getTime(cell) != 0;
}

inline Bool_t HMdcLayListCells::addTime1(Int_t cell, Float_t timeValue) {
  // use this function for for storing time1 only
  if(!boundsOk(cell)) return kFALSE;
  if(drTime1 == NULL) drTime1 = new Float_t [maxCells];
  HMdcTBArray::set(arr,cell,1);  // time1 only !!!
  drTime1[cell] = timeValue;
  return kTRUE;
}

inline Bool_t HMdcLayListCells::nextTime(Int_t& cell, Float_t& timeValue) 
    const {
  if(!nextCell(cell) || drTime1 == NULL) return kFALSE;
  timeValue = drTime1[cell];
  return kTRUE;
}

inline Float_t HMdcLayListCells::getTimeValue(Int_t cell) const {
  return (drTime1 && getTime(cell)) ? drTime1[cell]:-999.F;
}

inline Int_t HMdcLayListCells::getGeantTrack(Int_t cell,UChar_t tm) const {
  if(boundsOk(cell)) {
    if(tm == 1 && gnTrackNum1 != NULL) return gnTrackNum1[cell];
    if(tm == 2 && gnTrackNum2 != NULL) return gnTrackNum2[cell];
  }
  return 0;
}

inline UChar_t HMdcLayListCells::getNClusters(Int_t cell) const {
  if(!boundsOk(cell) || nClust == NULL) return 0;
  return nClust[cell];
};

inline UChar_t HMdcLayListCells::getNRClusters(Int_t cell) const {
  if(!boundsOk(cell) || nRClust == NULL) return 0;
  return nRClust[cell];
};

inline UChar_t HMdcLayListCells::getNFitted(Int_t cell) const {
  if(!boundsOk(cell) || nFitted == NULL) return 0;
  return nFitted[cell];
};

inline UChar_t HMdcLayListCells::getNRFitted(Int_t cell) const {
  if(!boundsOk(cell) || nRFitted == NULL) return 0;
  return nRFitted[cell];
};
    
// - - - - - - - - - - - - - - - - - - - - - - - - - -

class HMdcModListCells : public TObject {
  protected:
    HMdcLayListCells list[6];
  public:
    HMdcModListCells(void) {clear();}
    ~HMdcModListCells(void) {}
    inline HMdcLayListCells& operator[](Int_t l);
    inline const HMdcLayListCells& operator[](Int_t l) const;
    
    inline void    clear(void);
    inline void    clearClustCounter(void);
    inline void    clearFitWiresCounter(void);
    inline Bool_t  setTime(Int_t lay, Int_t cell, UChar_t time);
    inline void    delTime(Int_t lay, Int_t cell, UChar_t time);
    inline Int_t   getNCells(void) const;
    inline Bool_t  nextCell(Int_t& lay, Int_t& cell) const;
    inline UChar_t getTime(Int_t lay, Int_t cell) const;
    inline Bool_t  isCell(Int_t lay, Int_t cell) const;
    inline Int_t   getNLayers(void) const;
    void           print(void) const;
    
    inline Bool_t  addTime1(Int_t l, Int_t c, Float_t tm);
    inline Bool_t  nextTime(Int_t& l, Int_t& c, Float_t& tm) const;
    inline Float_t getTimeValue(Int_t l, Int_t c) const;
    inline Bool_t  addCell(Int_t l,Int_t c,UChar_t tm,Float_t dt1,Float_t dt2);
    inline Bool_t  addCellSim(Int_t l,Int_t c,UChar_t tm,
                              Float_t dt1,Float_t dt2,
                              Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2);
    UChar_t        nextCell(Int_t& l,Int_t& c,Float_t& dt1,Float_t& dt2) const;
    UChar_t        nextCellSim(Int_t& l,Int_t& c,Float_t& dt1,Float_t& dt2,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    UChar_t        nextCellSim(Int_t& l,Int_t& c,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    inline Bool_t  increaseClusCount(Int_t l,Int_t cell);
    inline Bool_t  increaseRClusCount(Int_t l,Int_t cell);
    inline Bool_t  increaseFittedCount(Int_t l,Int_t cell);
    inline Bool_t  increaseRFittedCount(Int_t l,Int_t cell);
    inline Bool_t  increaseFittedAndRealCount(Int_t l,Int_t cell);
    inline Bool_t  decreaseClusCount(Int_t l,Int_t cell);
    inline Bool_t  decreaseRClusCount(Int_t l,Int_t cell);
    inline Bool_t  decreaseFittedCount(Int_t l,Int_t cell);
    inline Bool_t  decreaseRFittedCount(Int_t l,Int_t cell);
    inline Bool_t  decreaseFittedAndRealCount(Int_t l,Int_t cell);
    inline UChar_t getNClusters(Int_t l,Int_t cell) const;
    inline UChar_t getNRClusters(Int_t l,Int_t cell) const;
    inline UChar_t getNFitted(Int_t l,Int_t cell) const;
    inline UChar_t getNRFitted(Int_t l,Int_t cell) const;
    Int_t          genNumNotFittedLayers(void) const;
    Int_t          genNumNotFittedLayers(Int_t &nNFCells) const;

  protected:
    inline Bool_t  nLayOk(Int_t l) const {return l>=0 && l<6;}

  ClassDef(HMdcModListCells,0) //List of all cells in layer
};

inline HMdcLayListCells& HMdcModListCells::operator[](Int_t l) {
  if(nLayOk(l)) return list[l];
  HMdcLayListCells* null=0;
  return *null;
}

inline const HMdcLayListCells& HMdcModListCells::operator[](Int_t l) const {
  if(nLayOk(l)) return list[l];
  HMdcLayListCells* null=0;
  return *null;
}
    
inline void HMdcModListCells::clear(void) {
  for(Int_t l=0;l<6;l++) list[l].clear();
}

inline void HMdcModListCells::clearClustCounter(void) {
  for(Int_t l=0;l<6;l++) list[l].clearClustCounter();
}

inline void HMdcModListCells::clearFitWiresCounter(void) {
  for(Int_t l=0;l<6;l++) list[l].clearFitWiresCounter();
}

inline Bool_t HMdcModListCells::setTime(Int_t lay, Int_t cell, UChar_t time) {
  if(nLayOk(lay)) return list[lay].setTime(cell,time);
  return kFALSE;
}

inline Bool_t HMdcModListCells::nextCell(Int_t& lay, Int_t& cell) const {
  if(lay<0) {
    lay=0;
    cell=-1;
  }
  for(;lay<6;lay++) if(list[lay].nextCell(cell)) return kTRUE;
  lay=-1;
  return kFALSE;
}

inline void HMdcModListCells::delTime(Int_t lay, Int_t cell, UChar_t time) {
  if(nLayOk(lay)) list[lay].delTime(cell,time);
}

inline Int_t HMdcModListCells::getNCells(void) const {
  Int_t nCells=0;
  for(Int_t l=0;l<6;l++) nCells+=list[l].getNCells();
  return nCells;
}

inline UChar_t HMdcModListCells::getTime(Int_t lay, Int_t cell) const {
  if(nLayOk(lay)) return list[lay].getTime(cell);
  return 0;
}

inline Int_t HMdcModListCells::getNLayers(void) const {
  Int_t nLay=0;
  for(Int_t l=0;l<6;l++) if(list[l].getFirstCell()>=0) nLay++;
  return nLay;
}

inline Bool_t HMdcModListCells::isCell(Int_t lay, Int_t cell) const {
  if(nLayOk(lay)) return list[lay].isCell(cell);
  return kFALSE;
}


inline Bool_t HMdcModListCells::addTime1(Int_t l, Int_t c, Float_t tm) {
  if(nLayOk(l)) return list[l].addTime1(c,tm);
  return kFALSE;
}

inline Bool_t HMdcModListCells::nextTime(Int_t& l, Int_t& c, Float_t& tm) 
    const {
  if(!nextCell(l,c)) return kFALSE;
  tm = getTimeValue(l,c);
  return kTRUE;
}

inline Float_t HMdcModListCells::getTimeValue(Int_t l,Int_t c) 
    const {
  if(nLayOk(l)) return list[l].getTimeValue(c);
  return -999.F;
}

inline Bool_t HMdcModListCells::addCell(Int_t l,Int_t c,UChar_t tm,
    Float_t dt1,Float_t dt2) {
  if(nLayOk(l)) return list[l].addCell(c,tm,dt1,dt2);
  return kFALSE; 
}

inline Bool_t HMdcModListCells::addCellSim(Int_t l,Int_t c,
    UChar_t tm,Float_t dt1,Float_t dt2,
    Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2) {
  // tm can be eq. 1(time1 only), 2(time2 only) or 3(both)
  // dt1 and dt2 - driftTime1 and driftTime2 from HMdcCal1
  // tr1 and tr2 - geant track numbers for driftTime1 and driftTime2
  if(nLayOk(l)) return list[l].addCellSim(c,tm,dt1,dt2,tr1,tr2,tf1,tf2);
  return kFALSE; 
}

inline Bool_t HMdcModListCells::increaseClusCount(Int_t l,Int_t cell) {
  if(nLayOk(l)) return list[l].increaseClusCount(cell);
  return kFALSE;
}

inline Bool_t HMdcModListCells::increaseRClusCount(Int_t l,Int_t cell) {
  if(nLayOk(l)) return list[l].increaseRClusCount(cell);
  return kFALSE;
}

inline Bool_t HMdcModListCells::increaseFittedCount(Int_t l,Int_t cell) {
  if(nLayOk(l)) return list[l].increaseFittedCount(cell);
  return kFALSE;
}

inline Bool_t HMdcModListCells::increaseRFittedCount(Int_t l,Int_t cell) {
  if(nLayOk(l)) return list[l].increaseRFittedCount(cell);
  return kFALSE;
}

inline Bool_t HMdcModListCells::increaseFittedAndRealCount(Int_t l,Int_t cell) {
  if(nLayOk(l)) return list[l].increaseFittedAndRealCount(cell);
  return kFALSE;
}

inline Bool_t HMdcModListCells::decreaseClusCount(Int_t l,Int_t cell) {
  if(nLayOk(l)) return list[l].decreaseClusCount(cell);
  return kFALSE;
}

inline Bool_t HMdcModListCells::decreaseRClusCount(Int_t l,Int_t cell) {
  if(nLayOk(l)) return list[l].decreaseRClusCount(cell);
  return kFALSE;
}

inline Bool_t HMdcModListCells::decreaseFittedCount(Int_t l,Int_t cell) {
  if(nLayOk(l)) return list[l].decreaseFittedCount(cell);
  return kFALSE;
}

inline Bool_t HMdcModListCells::decreaseFittedAndRealCount(Int_t l,Int_t cell) {
  if(nLayOk(l)) return list[l].decreaseFittedAndRealCount(cell);
  return kFALSE;
}

inline Bool_t HMdcModListCells::decreaseRFittedCount(Int_t l,Int_t cell) {
  if(nLayOk(l)) return list[l].decreaseRFittedCount(cell);
  return kFALSE;
}

inline UChar_t HMdcModListCells::getNClusters(Int_t l,Int_t cell) const {
  if(nLayOk(l)) return list[l].getNClusters(cell);
  return kFALSE;
}

inline UChar_t HMdcModListCells::getNRClusters(Int_t l,Int_t cell) const {
  if(nLayOk(l)) return list[l].getNRClusters(cell);
  return kFALSE;
}

inline UChar_t HMdcModListCells::getNFitted(Int_t l,Int_t cell) const {
  if(nLayOk(l)) return list[l].getNFitted(cell);
  return kFALSE;
}

inline UChar_t HMdcModListCells::getNRFitted(Int_t l,Int_t cell) const {
  if(nLayOk(l)) return list[l].getNRFitted(cell);
  return kFALSE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - -

class HMdcSecListCells : public TObject {
  protected:
    HMdcModListCells list[4];
  public:
    HMdcSecListCells(void) {clear();}
    ~HMdcSecListCells(void) {}
    inline HMdcModListCells& operator[](Int_t m);
    inline const HMdcModListCells& operator[](Int_t m) const;
    
    inline void    clear(void);
    void           clearClustCounter(Int_t seg);
    void           clearFitWiresCounter(Int_t seg);
    inline Bool_t  setTime(Int_t m, Int_t l, Int_t c, UChar_t t);
    inline void    delTime(Int_t m, Int_t l, Int_t c, UChar_t t);
    inline Int_t   getNCells(void) const;
    inline Bool_t  nextCell(Int_t& m, Int_t& l, Int_t& c) const;
    inline UChar_t getTime(Int_t m, Int_t l, Int_t c) const;
    inline Int_t   getNLayers(void) const;
    void           print(void) const;

    inline Bool_t  addTime1(Int_t m, Int_t l, Int_t c, Float_t tm);
    inline Bool_t  nextTime(Int_t& m, Int_t& l, Int_t& c, Float_t& tm) const;
    inline Float_t getTimeValue(Int_t m, Int_t l, Int_t c) const;
    inline Bool_t  addCell(Int_t m,Int_t l,Int_t c,UChar_t tm,Float_t dt1,Float_t dt2);
    inline Bool_t  addCellSim(Int_t m,Int_t l,Int_t c,UChar_t tm,Float_t dt1,Float_t dt2,
                              Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2);
    UChar_t        nextCell(Int_t& m,Int_t& l,Int_t& c,Float_t& dt1,Float_t& dt2) const;
    UChar_t        nextCellSim(Int_t& m,Int_t& l,Int_t& c,Float_t& dt1,Float_t& dt2,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    UChar_t        nextCellSim(Int_t& m,Int_t& l,Int_t& c,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    inline Bool_t  increaseClusCount(Int_t m,Int_t l,Int_t cell);
    inline Bool_t  increaseRClusCount(Int_t m,Int_t l,Int_t cell);
    inline Bool_t  increaseFittedCount(Int_t m,Int_t l,Int_t cell);
    inline Bool_t  increaseRFittedCount(Int_t m,Int_t l,Int_t cell);
    inline Bool_t  increaseFittedAndRealCount(Int_t m,Int_t l,Int_t cell);
    inline Bool_t  decreaseClusCount(Int_t m,Int_t l,Int_t cell);
    inline Bool_t  decreaseRClusCount(Int_t m,Int_t l,Int_t cell);
    inline Bool_t  decreaseFittedCount(Int_t m,Int_t l,Int_t cell);
    inline Bool_t  decreaseRFittedCount(Int_t m,Int_t l,Int_t cell);
    inline Bool_t  decreaseFittedAndRealCount(Int_t m,Int_t l,Int_t cell);
    inline UChar_t getNClusters(Int_t m,Int_t l,Int_t cell) const;
    inline UChar_t getNRClusters(Int_t m,Int_t l,Int_t cell) const;
    inline UChar_t getNFitted(Int_t m,Int_t l,Int_t cell) const;
    inline UChar_t getNRFitted(Int_t m,Int_t l,Int_t cell) const;
    Bool_t         addToClusCounters(Int_t seg,HMdcList12GroupCells& listCells);
    Bool_t         addToRClusCounters(Int_t seg,HMdcList12GroupCells& listCells);
    Bool_t         subFrClusCounters(Int_t seg,HMdcList12GroupCells& listCells);
    Bool_t         subFrRClusCounters(Int_t seg,HMdcList12GroupCells& listCells);
    Bool_t         subFittedCounters(Int_t seg, HMdcCellGroup12 *listCells);
    Bool_t         subRFittedCounters(Int_t seg,HMdcCellGroup12 *listCells);
    Bool_t         subFittedAndRealCounters(Int_t seg, HMdcCellGroup12 *listCells);
    Int_t          getNUniqueWires(Int_t seg,HMdcList12GroupCells& listCells) const;
    Int_t          getNUniqueWires2(Int_t seg,HMdcList12GroupCells& listCells) const;
    Int_t          getNUniqueWires(Int_t seg,HMdcList12GroupCells& listCells,Int_t& nLays) const;
    Int_t          getNUniqueWires2(Int_t seg,HMdcList12GroupCells& listCells,Int_t& nLays) const;
    Int_t          getNUniqueAndRWires(Int_t seg,HMdcList12GroupCells& listCells,
                                       Int_t& nWInReal) const;
    Int_t          getNUniqueAndRWires2(Int_t seg,HMdcList12GroupCells& listCells,
                                        Int_t& nWInReal) const;
    Bool_t         addToNFittedCounters(Int_t seg,HMdcCellGroup12 *listCells);
    Bool_t         addToNRFittedCounters(Int_t seg,HMdcCellGroup12 *listCells);
    Bool_t         addToFittedAndRealCounters(Int_t seg,HMdcCellGroup12 *listCells);
    inline Int_t   getNFittedUnWires(Int_t seg,HMdcCellGroup12 *listCells) const;
    inline Int_t   getNFittedUnWires2(Int_t seg,HMdcCellGroup12 *listCells) const;
    inline Int_t   getNFittedUnLayers(Int_t seg,HMdcCellGroup12 *listCells) const;
    inline Int_t   getNFittedUnAndRWires(Int_t seg,HMdcCellGroup12 *listCells,Int_t& nWInReal) const;
    inline Int_t   getNFittedUnAndRWires2(Int_t seg,HMdcCellGroup12 *listCells,Int_t& nWInReal) const;
    
    Float_t        getSegmentWeight(Int_t seg,HMdcCellGroup12 *listCells,Int_t& nWInReal,
                                    Bool_t isAdded=kTRUE) const;
  protected:
    inline Bool_t  nModOk(Int_t m) const {return m>=0 && m<4;}
    Int_t          getNFittedUnWires(Int_t seg,HMdcCellGroup12 *listCells,UChar_t ns) const;
    Int_t          getNFittedUnAndRWires(Int_t seg,HMdcCellGroup12 *listCells,Int_t& nWInReal,UChar_t ns) const;
    Int_t          getNFittedUnLayers(Int_t seg,HMdcCellGroup12 *listCells,UChar_t ns) const;
    
  ClassDef(HMdcSecListCells,0) //List of all cells in layer
};

inline HMdcModListCells& HMdcSecListCells::operator[](Int_t m) {
  if(nModOk(m)) return list[m];
  HMdcModListCells* null=0;
  return *null;
}

inline const HMdcModListCells& HMdcSecListCells::operator[](Int_t m) const {
  if(nModOk(m)) return list[m];
  HMdcModListCells* null=0;
  return *null;
}
  
inline void HMdcSecListCells::clear(void) {
  for(Int_t m=0;m<4;m++) list[m].clear();
}

inline Bool_t HMdcSecListCells::setTime(Int_t m, Int_t l, Int_t c, UChar_t t) {
  if(nModOk(m)) return list[m].setTime(l,c,t);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::nextCell(Int_t& m, Int_t& l, Int_t& c) const {
  if(m<0) {
    m = l = 0;
    c = -1;
  }
  for(;m<4;m++) if(list[m].nextCell(l,c)) return kTRUE;
  m = -1;
  return kFALSE;
}

inline void HMdcSecListCells::delTime(Int_t m, Int_t l, Int_t c, UChar_t t) {
  if(nModOk(m)) list[m].delTime(l,c,t);
}

inline Int_t HMdcSecListCells::getNCells(void) const {
  Int_t nCells=0;
  for(Int_t m=0;m<4;m++) nCells+=list[m].getNCells();
  return nCells;
}

inline UChar_t HMdcSecListCells::getTime(Int_t m, Int_t l, Int_t c) const {
  if(nModOk(m)) return list[m].getTime(l,c);
  return 0;
}

inline Int_t HMdcSecListCells::getNLayers(void) const {
  Int_t nLay=0;
  for(Int_t m=0;m<4;m++) nLay+=list[m].getNLayers();
  return nLay;
}

inline Bool_t HMdcSecListCells::addTime1(Int_t m, Int_t l, Int_t c, Float_t tm) {
  if(nModOk(m)) return list[m].addTime1(l,c,tm);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::nextTime(Int_t& m, Int_t& l, Int_t& c, 
    Float_t& tm) const {
  if(!nextCell(m,l,c)) return kFALSE;
  tm=getTimeValue(m,l,c);
  return kTRUE;
}

inline Float_t HMdcSecListCells::getTimeValue(Int_t m, Int_t l, Int_t c)
    const {
  if(nModOk(m)) return list[m].getTimeValue(l,c);
  return -999.F;
}

inline Bool_t HMdcSecListCells::addCell(Int_t m, Int_t l,Int_t c,UChar_t tm,
    Float_t dt1,Float_t dt2) {
  if(nModOk(m)) return list[m].addCell(l,c,tm,dt1,dt2);
  return kFALSE; 
}

inline Bool_t HMdcSecListCells::addCellSim(Int_t m, Int_t l,Int_t c,
    UChar_t tm,Float_t dt1,Float_t dt2,
    Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2) {
  // tm can be eq. 1(time1 only), 2(time2 only) or 3(both)
  // dt1 and dt2 - driftTime1 and driftTime2 from HMdcCal1
  // tr1 and tr2 - geant track numbers for driftTime1 and driftTime2
  if(nModOk(m)) return list[m].addCellSim(l,c,tm,dt1,dt2,tr1,tr2,tf1,tf2);
  return kFALSE; 
}

inline Bool_t HMdcSecListCells::increaseClusCount(Int_t m,Int_t l,Int_t cell) {
  if(nModOk(m)) return list[m].increaseClusCount(l,cell);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::increaseRClusCount(Int_t m,Int_t l,Int_t cell) {
  if(nModOk(m)) return list[m].increaseRClusCount(l,cell);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::increaseFittedCount(Int_t m,Int_t l,Int_t cell) {
  if(nModOk(m)) return list[m].increaseFittedCount(l,cell);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::increaseRFittedCount(Int_t m,Int_t l,Int_t cell) {
  if(nModOk(m)) return list[m].increaseRFittedCount(l,cell);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::increaseFittedAndRealCount(Int_t m,Int_t l,Int_t cell) {
  if(nModOk(m)) return list[m].increaseFittedAndRealCount(l,cell);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::decreaseClusCount(Int_t m,Int_t l,Int_t cell) {
  if(nModOk(m)) return list[m].decreaseClusCount(l,cell);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::decreaseRClusCount(Int_t m,Int_t l,Int_t cell) {
  if(nModOk(m)) return list[m].decreaseRClusCount(l,cell);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::decreaseFittedCount(Int_t m,Int_t l,Int_t cell) {
  if(nModOk(m)) return list[m].decreaseFittedCount(l,cell);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::decreaseFittedAndRealCount(Int_t m,Int_t l,Int_t cell) {
  if(nModOk(m)) return list[m].decreaseFittedAndRealCount(l,cell);
  return kFALSE;
}

inline Bool_t HMdcSecListCells::decreaseRFittedCount(Int_t m,Int_t l,Int_t cell) {
  if(nModOk(m)) return list[m].decreaseRFittedCount(l,cell);
  return kFALSE;
}

inline UChar_t HMdcSecListCells::getNClusters(Int_t m,Int_t l,Int_t cell) const {
  if(nModOk(m)) return list[m].getNClusters(l,cell);
  return kFALSE;
}

inline UChar_t HMdcSecListCells::getNRClusters(Int_t m,Int_t l,Int_t cell) const {
  if(nModOk(m)) return list[m].getNRClusters(l,cell);
  return kFALSE;
}

inline UChar_t HMdcSecListCells::getNFitted(Int_t m,Int_t l,Int_t cell) const {
  if(nModOk(m)) return list[m].getNFitted(l,cell);
  return kFALSE;
}

inline UChar_t HMdcSecListCells::getNRFitted(Int_t m,Int_t l,Int_t cell) const {
  if(nModOk(m)) return list[m].getNRFitted(l,cell);
  return kFALSE;
}

inline Int_t HMdcSecListCells::getNFittedUnWires(Int_t seg,HMdcCellGroup12 *listCells) const {
  // call it if "listCells" was added to the wire counters of HMdcSecListCells
  return getNFittedUnWires(seg,listCells,1);
}

inline Int_t HMdcSecListCells::getNFittedUnWires2(Int_t seg,HMdcCellGroup12 *listCells) const {
  // call it if "listCells" was NOT added to the wire counters of HMdcSecListCells
  return getNFittedUnWires(seg,listCells,0);
}

inline Int_t HMdcSecListCells::getNFittedUnLayers(Int_t seg,HMdcCellGroup12 *listCells) const {
  // call it if "listCells" was added to the wire counters of HMdcSecListCells
  return getNFittedUnLayers(seg,listCells,1);
}

inline Int_t HMdcSecListCells::getNFittedUnAndRWires(Int_t seg,HMdcCellGroup12 *listCells,
                                                  Int_t& nWInReal) const {
  // call it if "listCells" was added to the wire counters of HMdcSecListCells
  return getNFittedUnAndRWires(seg,listCells,nWInReal,1);
}

inline Int_t HMdcSecListCells::getNFittedUnAndRWires2(Int_t seg,HMdcCellGroup12 *listCells,
                                                   Int_t& nWInReal) const {
  // call it if "listCells" was NOT added to the cluster conters of HMdcSecListCells
  return getNFittedUnAndRWires(seg,listCells,nWInReal,0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - -
class HCategory;
class HIterator;
class HMdcTimeCut;

class HMdcEvntListCells : public TObject {
  protected:
    HMdcSecListCells list[6];
    HCategory*                pMdcCal1Cat;   // category MdcCal1
    HIterator*                iterMdcCal1;   // iterator for MdcCal1 category
    Bool_t                    isGeant;       // =kTRUE for GEANT data.
    HMdcTimeCut*              pDrTimeCut;    // drift time cuts 
    static HMdcEvntListCells* pGlobalLCells; // pointer to the global object
  public:
    HMdcEvntListCells(void);
    ~HMdcEvntListCells(void);
    static HMdcEvntListCells* getExObject(void) {return pGlobalLCells;}
    static HMdcEvntListCells* getObject(Bool_t& isCreated);
    static void               deleteCont(void);
    inline HMdcSecListCells& operator[](Int_t s);
    inline const HMdcSecListCells& operator[](Int_t s) const;
    Bool_t setMdcCal1Cat(void);
    Int_t  collectWires(Int_t sec=-1);
    
    inline void    clear(void);
    inline Bool_t  setTime(Int_t s, Int_t m, Int_t l, Int_t c, UChar_t t);
    inline void    delTime(Int_t s, Int_t m, Int_t l, Int_t c, UChar_t t);
    inline Int_t   getNCells(void) const;
    inline Bool_t  nextCell(Int_t& s, Int_t& m, Int_t& l, Int_t& c) const;
    inline Bool_t  nextCell(Int_t& s,Int_t& m,Int_t& l,Int_t& c,UChar_t& tm) const;
    inline UChar_t getTime(Int_t s, Int_t m, Int_t l, Int_t c) const;
    void print(void) const;

    inline Bool_t  addTime1(Int_t s, Int_t m, Int_t l, Int_t c, Float_t tm);
    inline Bool_t  nextTime(Int_t& s, Int_t& m, Int_t& l, Int_t& c, Float_t& tm) const;
    inline Float_t getTimeValue(Int_t s, Int_t m, Int_t l, Int_t c) const;
    inline Bool_t  addCell(Int_t s,Int_t m,Int_t l,Int_t c,UChar_t tm,
                           Float_t dt1,Float_t dt2);
    inline Bool_t  addCellSim(Int_t s,Int_t m,Int_t l,Int_t c,UChar_t tm,
                           Float_t dt1,Float_t dt2,
                           Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2);
    UChar_t        nextCell(Int_t& s,Int_t& m,Int_t& l,Int_t& c,
                            Float_t& dt1,Float_t& dt2) const;
    UChar_t        nextCellSim(Int_t& s,Int_t& m,Int_t& l,Int_t& c,
                       Float_t& dt1,Float_t& dt2,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    UChar_t        nextCellSim(Int_t& s,Int_t& m,Int_t& l,Int_t& c,
                       Int_t& tr1,Int_t& tr2,Float_t& tf1,Float_t& tf2) const;
    void           clearClustCounter(Int_t seg);
    void           clearFitWiresCounter(Int_t seg);
    inline Bool_t  increaseFittedCount(Int_t s,Int_t m,Int_t l,Int_t cell);
    inline Bool_t  increaseRFittedCount(Int_t s,Int_t m,Int_t l,Int_t cell);
    inline Bool_t  increaseFittedAndRealCount(Int_t s,Int_t m,Int_t l,Int_t cell);
  protected:
    inline Bool_t nSecOk(Int_t s) const {return s>=0 && s<6;}

  ClassDef(HMdcEvntListCells,0) //List of all cells in layer
};

inline HMdcSecListCells& HMdcEvntListCells::operator[](Int_t s) {
  if(nSecOk(s)) return list[s];
  HMdcSecListCells* null=0;
  return *null;
}

inline const HMdcSecListCells& HMdcEvntListCells::operator[](Int_t s) const {
  if(nSecOk(s)) return list[s];
  HMdcSecListCells* null=0;
  return *null;
}
  
inline void HMdcEvntListCells::clear(void) {
  for(Int_t s=0;s<6;s++) list[s].clear();
}

inline Bool_t HMdcEvntListCells::setTime(Int_t s, Int_t m, Int_t l, Int_t c, 
    UChar_t t) {
  if(nSecOk(s)) return list[s].setTime(m,l,c,t);
  return kFALSE;
}

inline Bool_t HMdcEvntListCells::nextCell(Int_t& s,Int_t& m,Int_t& l,Int_t& c) 
    const {
  if(s<0) {
    s=m=l=0;
    c=-1;
  }
  for(;s<6;s++) if(list[s].nextCell(m,l,c)) return kTRUE;
  s=-1;
  return kFALSE;
}

inline Bool_t HMdcEvntListCells::nextCell(Int_t& s,Int_t& m,Int_t& l,Int_t& c,
    UChar_t& tm) const {
  if(nextCell(s,m,l,c)) {
    tm=getTime(s,m,l,c);
    if(tm) return kTRUE;
  }
  return kFALSE;
}

inline void HMdcEvntListCells::delTime(Int_t s, Int_t m, Int_t l, Int_t c, 
    UChar_t t) {
  if(nSecOk(s)) list[s].delTime(m,l,c,t);
}

inline Int_t HMdcEvntListCells::getNCells(void) const {
  Int_t nCells=0;
  for(Int_t s=0;s<6;s++) nCells+=list[s].getNCells();
  return nCells;
}

inline UChar_t HMdcEvntListCells::getTime(Int_t s, Int_t m, Int_t l, Int_t c) 
    const {
  if(nSecOk(s)) return list[s].getTime(m,l,c);
  return 0;
}

inline Bool_t HMdcEvntListCells::addTime1(Int_t s, Int_t m, Int_t l, Int_t c, 
    Float_t tm) {
  if(nSecOk(s)) return list[s].addTime1(m,l,c,tm);
  return kFALSE;
}

inline Bool_t HMdcEvntListCells::nextTime(Int_t& s, Int_t& m, Int_t& l, Int_t& c, 
    Float_t& tm) const {
  if(!nextCell(s,m,l,c)) return kFALSE;
  tm=getTimeValue(s,m,l,c);
  return kTRUE;
}

inline Float_t HMdcEvntListCells::getTimeValue(Int_t s, Int_t m, Int_t l, 
    Int_t c) const {
  if(nSecOk(s)) return list[s].getTimeValue(m,l,c);
  return -999.F;
}

inline Bool_t HMdcEvntListCells::addCell(Int_t s, Int_t m, Int_t l,Int_t c,
    UChar_t tm,Float_t dt1,Float_t dt2) {
  // tm can be eq. 1(time1 only), 2(time2 only) or 3(both)
  // dt1 and dt2 - driftTime1 and driftTime2 from HMdcCal1
  if(nSecOk(m)) return list[s].addCell(m,l,c,tm,dt1,dt2);
  return kFALSE; 
}

inline Bool_t HMdcEvntListCells::addCellSim(Int_t s, Int_t m, Int_t l,Int_t c,
    UChar_t tm,Float_t dt1,Float_t dt2,
    Int_t tr1,Int_t tr2,Float_t tf1,Float_t tf2) {
  // tm can be eq. 1(time1 only), 2(time2 only) or 3(both)
  // dt1 and dt2 - driftTime1 and driftTime2 from HMdcCal1
  // tr1 and tr2 - geant track numbers for driftTime1 and driftTime2
  if(nSecOk(m)) return list[s].addCellSim(m,l,c,tm,dt1,dt2,tr1,tr2,tf1,tf2);
  return kFALSE; 
}

inline Bool_t HMdcEvntListCells::increaseFittedCount(Int_t s,Int_t m,Int_t l,Int_t cell) {
  if(nSecOk(s)) return list[s].increaseFittedCount(m,l,cell);
  return kFALSE;
}

inline Bool_t HMdcEvntListCells::increaseRFittedCount(Int_t s,Int_t m,Int_t l,Int_t cell) {
  if(nSecOk(s)) return list[s].increaseRFittedCount(m,l,cell);
  return kFALSE;
}

inline Bool_t HMdcEvntListCells::increaseFittedAndRealCount(Int_t s,Int_t m,Int_t l,Int_t cell) {
  if(nSecOk(s)) return list[s].increaseFittedAndRealCount(m,l,cell);
  return kFALSE;
}

#endif
