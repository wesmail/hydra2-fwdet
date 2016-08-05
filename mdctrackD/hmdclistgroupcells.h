#ifndef HMDCLISTGROUPCELLS_H
#define HMDCLISTGROUPCELLS_H

#include "hmdctbarray.h"
#include "TObject.h"

#include <string.h>

class HMdcList12GroupCells : public TObject {
  private:
    enum {laySz=12};        // size of on layer
    UChar_t arr[144];       // 144= 12bytes * 12layers
    Int_t firstCell[12];    // firstCell[layer] - first cell in layer
  public:
    HMdcList12GroupCells(void)  {clear();}
    ~HMdcList12GroupCells(void) {}
    inline HMdcList12GroupCells& operator = (const HMdcList12GroupCells& v);
    inline void    clear(void);
    inline Int_t   setTime(Int_t lay, Int_t cell, UChar_t time);
    Int_t          add(HMdcList12GroupCells *lst);
    inline void    delTime(Int_t lay, Int_t cell, UChar_t time);
    inline Int_t   getNCells(Int_t lay, Int_t layEnd=-1) const;
    inline Int_t   getNCells(void) const;          // Total num.of hits
    Int_t          getMaxNCellsPerLay(void) const; // Max.number of cells per layer
    inline Int_t   getNDrTimes(void) const;
    inline Int_t   getNDrTimes(Int_t lay) const;
    inline Int_t   getNDrTimes(Int_t lay1,Int_t lay2) const;
    inline Int_t   getNDrTimesMod(Int_t m) const {return getNDrTimes(m*6,m*6+5);}
    inline Int_t   getNLayers(void) const;    // Num. of layers with fired wires
    inline Int_t   getNLayers(Int_t imod) const;
    inline Int_t   getNLayersMod(Int_t mod) const; // -/- in module
    inline UChar_t getListLayers(Int_t imod=-1) const;
    inline Int_t   getCell(Int_t lay, Int_t idx) const;
    inline Int_t   getFirstCell(Int_t lay) const;
    inline Int_t   getLastCell(Int_t lay) const;
    Int_t          getActiveModule(void) const;
    inline Int_t   get4FirstCells(Int_t lay, Int_t* list) const; // for Hit&Seg
    inline UShort_t getOneLayerGroup(Int_t lay) const;
    inline Int_t   next(Int_t lay, Int_t prCell) const;
    inline Int_t   next(Int_t lay, Int_t prCell, UChar_t& tm) const;
    inline Bool_t  getNext(Int_t lay, Int_t& cell) const;
    inline Bool_t  getNextCell(Int_t& lay, Int_t& cell) const;
    inline Int_t   previous(Int_t lay, Int_t prCell) const;
    inline UChar_t getTime(Int_t lay, Int_t cell) const;
    Int_t          compare(const HMdcList12GroupCells* lst, Int_t l1=-1,
                           Int_t l2=-1, HMdcList12GroupCells* lstIdent=0) const;
    Int_t          compareAndUnset(const HMdcList12GroupCells* lst,
                                  HMdcList12GroupCells* lstIdent,Int_t modi=-1);
    Bool_t         isIncluded(const HMdcList12GroupCells& lst, Int_t l1=0,Int_t l2=11) const;
    Int_t          nIdentDrTimes(const HMdcList12GroupCells* lst,Int_t l1=0,Int_t l2=11) const;
    void           print(void) const;
    void           print(Int_t lay) const;
    
    // Next functions NOT for user:
    const  UChar_t*       getArr(void) const             {return arr;}
    const  Int_t*         getFirstCell(void) const       {return firstCell;}
    static Bool_t         findOverlap(Int_t& c1, Int_t& c2);
    void                  copyData(const UChar_t* a,const Int_t* fc);
    
  private:
    inline void           setInBounds(Int_t& l1, Int_t& l2) const;
    inline Bool_t         isLayOk(Int_t l) const;
    inline void           shiftLeft(Int_t lay);
    inline UChar_t*       layAddr(Int_t lay) {return arr+lay*laySz;}
    inline UChar_t*       cellAddr(Int_t lay,Int_t cell) {
                              return arr+lay*laySz+((cell-firstCell[lay])>>2);}
    inline const UChar_t* layAddrC(Int_t lay) const {return arr+lay*laySz;}
    inline const UChar_t* cellAddrC(Int_t lay,Int_t cell) const {
                              return arr+lay*laySz+((cell-firstCell[lay])>>2);}

  ClassDef(HMdcList12GroupCells,1) //List of all cells in layer
};

inline HMdcList12GroupCells& HMdcList12GroupCells::operator =
    (const HMdcList12GroupCells& lst) {
  memcpy(arr,lst.arr,144);
  memcpy(firstCell,lst.firstCell,48); // 12*sizeof(Int_t)
  return *this;
}

inline void HMdcList12GroupCells::clear(void) {
  memset(arr,0,144);
  for(Int_t l=0; l<12; l++) firstCell[l]=0;
}

inline Bool_t HMdcList12GroupCells::isLayOk(Int_t l) const {
  return l>=0 && l<12;
}
    
inline Int_t HMdcList12GroupCells::setTime(Int_t lay, Int_t cell, UChar_t time) {
  // return: 0 - ok.!;  n>0 - num.cells excl.from cl.
  if( cell<0 || (time&3)==0 || !isLayOk(lay)) return 0;
  Int_t nCellsDel=0;
  UChar_t *iLayer=arr+lay*laySz;
  UChar_t *pEnd=iLayer+11;
  if(*iLayer == 0) firstCell[lay]=cell & 0xFFFFFFFC;  // =(cell/4)*4
  else if(cell < firstCell[lay]) {
    if(firstCell[lay]-cell >= 48) {
      nCellsDel = getNCells(lay);
      memset(iLayer,0,laySz);
      firstCell[lay] = cell;
    } else {
      Int_t sh=(firstCell[lay]>>2)-(cell>>2);
      nCellsDel=HMdcTBArray::getNSet(pEnd-sh+1,pEnd);
      HMdcTBArray::shiftRight(iLayer,pEnd,pEnd-sh);
      firstCell[lay] = cell & 0xFFFFFFFC;
    }
  }
  else if( cell-firstCell[lay] >= 48 ) return 1; // out of data array
  HMdcTBArray::set(iLayer, cell-firstCell[lay], time);
  return nCellsDel;
}

inline void HMdcList12GroupCells::delTime(Int_t lay, Int_t cell, UChar_t time) {
  Int_t pos=cell-firstCell[lay];
  if(pos < 0 || pos >=  48) return;
  UChar_t* iLayer=arr+lay*laySz;
  HMdcTBArray::unset(iLayer, pos, time);
  if(*iLayer == 0) shiftLeft(lay);
}

inline void HMdcList12GroupCells::shiftLeft(Int_t lay) {
  UChar_t* iLayer=arr+lay*laySz;
  UChar_t* pEnd=iLayer+11;
  Int_t sh=HMdcTBArray::first(iLayer,pEnd);
  if(sh < 0) firstCell[lay]=0;
  else {
    sh = sh>>2;
    HMdcTBArray::shiftLeft(iLayer,pEnd,iLayer+sh);
    firstCell[lay] += (sh<<2);
  }
}

inline Int_t HMdcList12GroupCells::getNCells(Int_t lay, Int_t layEnd) const {
  if(layEnd==-1) layEnd=lay;
  if(!isLayOk(lay) || !isLayOk(layEnd) || lay>layEnd) return -1;
  return HMdcTBArray::getNSet(arr+lay*laySz,arr+layEnd*laySz+11);
}

inline Int_t HMdcList12GroupCells::getNCells(void) const {
  return HMdcTBArray::getNSet(arr,arr+143);
}

inline Int_t HMdcList12GroupCells::getNDrTimes(void) const {
  return HMdcBArray::getNSet(arr,arr+143);
}

inline Int_t HMdcList12GroupCells::getNDrTimes(Int_t lay) const {
  if(!isLayOk(lay)) return 0;
  const UChar_t* layArr = arr+lay*laySz;
  if( *layArr ) return HMdcBArray::getNSet(layArr,layArr+11);
  return 0;
}

inline Int_t HMdcList12GroupCells::getNDrTimes(Int_t lay1,Int_t lay2) const {
  if(lay1<0 || lay2>=12) return -1;
  return HMdcBArray::getNSet(arr+lay1*laySz,arr+lay2*laySz+11);
}

inline Int_t HMdcList12GroupCells::getCell(Int_t lay, Int_t idx) const {
  // return: -1 - idx <0 or >num.cells
  if(!isLayOk(lay)) return -1;
  const UChar_t* layArr = arr+lay*laySz;
  Int_t pos=HMdcTBArray::position(layArr,layArr+11,idx);
  if(pos >= 0) return pos+firstCell[lay];
  return -1;
}

inline Int_t HMdcList12GroupCells::getFirstCell(Int_t lay) const {
  // return: -1 - num.cells=0
  if(!isLayOk(lay)) return -1;
  const UChar_t* layArr = arr+lay*laySz;
  if(*layArr == 0) return -1;
  Int_t pos=HMdcTBArray::first(layArr,layArr+11);
  if(pos >= 0) return pos+firstCell[lay];
  return -1;
}

inline Int_t HMdcList12GroupCells::get4FirstCells(Int_t lay, Int_t* list) const {
  // return: first cell (frCell) or -1 (num.cells=0)
  // fill list[0]-time of frCell, time frCell+1, ..., time frCell+3
  if(!isLayOk(lay)) return -1;
  const UChar_t* addr=arr+lay*laySz;
  Int_t pos=HMdcTBArray::first(addr,addr+11);
  if(pos < 0) return -1;
  Int_t fcell=firstCell[lay]+pos;
  Int_t posLast=pos+4;
  for(Int_t ps=pos; ps<posLast; ps++) {
    *list=HMdcTBArray::get(addr,ps);
    list++;
  }
  return fcell;
}

inline UShort_t HMdcList12GroupCells::getOneLayerGroup(Int_t lay) const {
  // Return list of cells of one layer in format of HMdcSeg
  if(!isLayOk(lay)) return 0;
  const UChar_t* addr=arr+lay*laySz;
  Int_t pos = HMdcTBArray::first(addr,addr+11);
  Int_t bin = pos >> 2;  // >>2 eq. /4
  Int_t sh  = (pos & 3) << 1;
  UChar_t  l4wr = (addr[bin]>>sh) | (addr[bin+1]<<(8-sh));
  return (UShort_t(firstCell[lay]+pos) << 8) | UShort_t(l4wr);
}

inline Int_t HMdcList12GroupCells::getNLayers(Int_t imod) const {
  if(imod<0||imod>1) return getNLayers();
  return getNLayersMod(imod);
}

inline Int_t HMdcList12GroupCells::getNLayers(void) const {
  Int_t nLayers=0;
  for(Int_t lay=0; lay<12; lay++) if(arr[lay*laySz] > 0) nLayers++;
  return nLayers;
}

inline UChar_t HMdcList12GroupCells::getListLayers(Int_t imod) const {
  // imod - module number in segment (0 or 1)
  // imod== -1 - List_imod0|List_imod1
  // return 6 bits, one bit per layer
  if(imod<-1 || imod>1) return 0;
  Int_t im1 = imod==-1 ? 0:imod;
  Int_t im2 = imod==-1 ? 1:imod;
  UChar_t layList = 0;
  for(Int_t im=im1;im<=im2;im++) {
    Int_t lsh = im*6;
    for(Int_t l=0;l<6;l++) if(arr[(l+lsh)*laySz] > 0) layList |= 1<<l;
  }
  return layList;
}

inline Int_t HMdcList12GroupCells::getNLayersMod(Int_t mod) const {
  if(mod<0 || mod>1) return 0;
  Int_t nLayers=0;
  for(Int_t lay=mod*6; lay<(mod+1)*6; lay++) if(arr[lay*laySz] > 0) nLayers++;
  return nLayers;
}
    
inline Int_t HMdcList12GroupCells::getLastCell(Int_t lay) const {
  // return: -2 - num.cells=0
  if(!isLayOk(lay)) return -2;
  const UChar_t* layArr = arr+lay*laySz;
  if(*layArr == 0) return -2;
  Int_t pos=HMdcTBArray::last(layArr,layArr+11);
  if(pos >= 0) return pos+firstCell[lay];
  return -2;
}

inline Int_t HMdcList12GroupCells::next(Int_t lay, Int_t prCell) const {
  if(!isLayOk(lay)) return -1;
  const UChar_t* layArr = arr+lay*laySz;
  if(*layArr == 0) return -1;          // no wires in layer
  Int_t cell = HMdcTBArray::next(layArr,layArr+11,prCell-firstCell[lay]);
  if(cell>=0) return cell+firstCell[lay];
  return -1;
}

inline Int_t HMdcList12GroupCells::next(Int_t lay, Int_t prCell, UChar_t& tm) 
    const {
  // return newxt cell and drift time indexis ( 1 - time1 in HMdcCal1,
  // 2 - time2 in HMdcCal2 and 3 for time1+time2
  if(!isLayOk(lay)) return -1;
  const UChar_t* layArr = arr+lay*laySz;
  if(*layArr == 0) return -1;
  Int_t cell = HMdcTBArray::next(layArr,layArr+11,prCell-firstCell[lay]);
  if(cell<0) return cell;
  tm = HMdcTBArray::get(layArr,cell);
  return cell+firstCell[lay];
}

inline Bool_t HMdcList12GroupCells::getNext(Int_t lay, Int_t& cell) const {
  cell=next(lay,cell);
  if(cell>=0) return kTRUE;
  return kFALSE;
}

inline Bool_t HMdcList12GroupCells::getNextCell(Int_t& lay, Int_t& cell) const {
  if(lay<0) {
    lay=0;
    cell=-1;
  }
  for(;lay<12;lay++) if(getNext(lay,cell)) return kTRUE;
  return kFALSE;
}

inline Int_t HMdcList12GroupCells::previous(Int_t lay, Int_t prCell) const {
  if(!isLayOk(lay)) return kFALSE;
  const UChar_t* layArr = arr+lay*laySz;
  if(*layArr == 0) return -1;          // no wires in layer
  Int_t cell = HMdcTBArray::previous(layArr,layArr+11,prCell-firstCell[lay]);
  if(cell>=0) return cell+firstCell[lay];
  return -1;
}

inline UChar_t HMdcList12GroupCells::getTime(Int_t lay, Int_t cell) const {
  Int_t pos=cell-firstCell[lay];
  if(pos<0 || pos>=48) return 0;
  return HMdcTBArray::get(arr+lay*laySz,pos);
}

inline void HMdcList12GroupCells::setInBounds(Int_t& l1, Int_t& l2) const {
  if(l1<0) {
    l1=0;
    l2=11;
  } else  {
    if(l2<0) l2=l1;
    else if(l2>=12) l2=11;
  }
}

class HMdcList24GroupCells : public TObject {
  private:
    enum {laySz=12};
    UChar_t arr[288];       // 288= 12bytes * 24layers 
    Int_t firstCell[24];    // firstCell[layer] - first cell in layer
  public:
    HMdcList24GroupCells() {clear();}
    ~HMdcList24GroupCells(void) {}
    inline HMdcList24GroupCells& operator = (const HMdcList24GroupCells& v);
    inline void    clear(Int_t seg=-1);
    inline void    setSeg1(const HMdcList12GroupCells* seg);
    inline void    setSeg2(const HMdcList12GroupCells* seg);
    inline void    set(const HMdcList12GroupCells* seg1,
                       const HMdcList12GroupCells* seg2);
    inline Int_t   setTime(Int_t lay, Int_t cell, UChar_t time);
    Int_t          add(HMdcList24GroupCells *lst);
    inline void    delTime(Int_t lay, Int_t cell, UChar_t time);
    
//    inline Int_t   getNCells(Int_t lay, Int_t layEnd=-1) const;
    inline Int_t   getNCells(Int_t lay) const;
    inline Int_t   getNCellsMod(Int_t mod) const;
    inline Int_t   getNCells(void) const;          // Total num.of cells
    Int_t          getMaxNCellsPerLay(void) const; // Max. numb.of cells/layer
    inline Int_t   getNDrTimes(void) const;
    inline Int_t   getNDrTimes(Int_t lay) const;
    inline Int_t   getNDrTimes(Int_t lay1,Int_t lay2) const;
    inline Int_t   getNDrTimesMod(Int_t m) const {return getNDrTimes(m*6,m*6+5);}
    inline Int_t   getNLayers(void) const;          // Num. fired layers
    inline Int_t   getNLayersMod(Int_t mod) const;  // -/- in module
    inline UChar_t getListLayers(Int_t imod=-1) const;
    inline Int_t   getCell(Int_t lay, Int_t idx) const;
    inline Int_t   getFirstCell(Int_t lay) const;
    inline Int_t   getLastCell(Int_t lay) const;
    inline Int_t   get4FirstCells(Int_t lay, Int_t* list) const; // for Hit&Seg
    inline UShort_t getOneLayerGroup(Int_t lay) const;
    inline Int_t   next(Int_t lay, Int_t prCell) const;
    inline Bool_t  getNext(Int_t lay, Int_t& cell) const;
    inline Bool_t  getNextCell(Int_t& lay, Int_t& cell) const;
    inline Int_t   previous(Int_t lay, Int_t prCell) const;
    inline UChar_t getTime(Int_t lay, Int_t cell) const;
    Bool_t         getSeg(HMdcList12GroupCells& fSeg,Int_t seg) const;
    
    Int_t  compare(HMdcList24GroupCells* lst, Int_t l1=-1, Int_t l2=-1) const;
    Int_t  compareAndUnset(const HMdcList24GroupCells* lst,HMdcList24GroupCells* lstIdent);
    Bool_t isIncluded(HMdcList24GroupCells& lst, Int_t l1=0,Int_t l2=23) const;
    Int_t  nIdentDrTimes(HMdcList24GroupCells* lst,Int_t l1=0,Int_t l2=23) const;
    
    void print(void) const;
    void print(Int_t lay) const;
  private:
    inline void           setInBounds(Int_t& l1, Int_t& l2) const;
    inline Bool_t         isLayOk(Int_t l) const;
    inline Bool_t         isModOk(Int_t m) const;
    inline void           shiftLeft(Int_t lay);
    inline const UChar_t* layAddrC(Int_t lay) const {return arr+lay*laySz;}
    inline const UChar_t* cellAddrC(Int_t lay,Int_t cell) const {
                              return arr+lay*laySz+((cell-firstCell[lay])>>2);}
    inline UChar_t*       layAddr(Int_t lay)        {return arr+lay*laySz;}
    inline UChar_t*       cellAddr(Int_t lay,Int_t cell) {
                              return arr+lay*laySz+((cell-firstCell[lay])>>2);}

  ClassDef(HMdcList24GroupCells,1) //List of all cells in layer
};

inline HMdcList24GroupCells& HMdcList24GroupCells::operator =
    (const HMdcList24GroupCells& lst) {
  memcpy(arr,lst.arr,288);
  memcpy(firstCell,lst.firstCell,96); // 24*sizeof(Int_t)
  return *this;
}

inline void HMdcList24GroupCells::setSeg1(const HMdcList12GroupCells* seg) {
  if(seg) {
    memcpy(arr,seg->getArr(),144);
    memcpy(firstCell,seg->getFirstCell(),48); // 12*sizeof(Int_t)
  }
}

inline void HMdcList24GroupCells::setSeg2(const HMdcList12GroupCells* seg) {
  if(seg) {
    memcpy(arr+144,seg->getArr(),144);
    memcpy(firstCell+12,seg->getFirstCell(),48); // 12*sizeof(Int_t)
  }
}

inline void HMdcList24GroupCells::set(const HMdcList12GroupCells* seg1,
                    const HMdcList12GroupCells* seg2) {
  if(seg1) setSeg1(seg1);
  else clear(0);
  if(seg2) setSeg2(seg2);
  else clear(1);
}
 
inline void HMdcList24GroupCells::clear(Int_t seg) {
  if(seg<0 || seg>1) {
    memset(arr,0,288);
    for(Int_t l=0; l<24; l++) firstCell[l]=0;
  } else {
    Int_t l0=seg*12;
    memset(arr+l0*laySz,0,12*laySz);
    for(Int_t l=0; l<12; l++) firstCell[l+l0]=0;
  }
}

inline Bool_t HMdcList24GroupCells::isLayOk(Int_t l) const {
  return l>=0 && l<24;
}

inline Bool_t HMdcList24GroupCells::isModOk(Int_t m) const {
  return m>=0 && m<4;
}

inline Int_t HMdcList24GroupCells::setTime(Int_t lay, Int_t cell, UChar_t time) {
  // return: 0 - ok.!;  n>0 - num.cells excl.from cl.
  if( cell<0 || (time&3)==0 || !isLayOk(lay)) return 0;
  Int_t nCellsDel=0;
  UChar_t *iLayer=arr+lay*laySz;
  UChar_t *pEnd=iLayer+11;
  if(*iLayer == 0) firstCell[lay]=cell & 0xFFFFFFFC;  // =(cell/4)*4
  else if(cell < firstCell[lay]) {
    Int_t sh=(firstCell[lay]>>2)-(cell>>2);
    nCellsDel=HMdcTBArray::getNSet(pEnd-sh+1,pEnd);
    HMdcTBArray::shiftRight(iLayer,pEnd,pEnd-sh);
    firstCell[lay]=cell & 0xFFFFFFFC;
  }
  else if( cell-firstCell[lay] >= 48 ) return 1; // out of data array
  HMdcTBArray::set(iLayer, cell-firstCell[lay], time);
  return nCellsDel;
}

inline void HMdcList24GroupCells::delTime(Int_t lay, Int_t cell, UChar_t time) {
  Int_t pos=cell-firstCell[lay];
  if(pos < 0 || pos >=  48) return;
  UChar_t *iLayer=arr+lay*laySz;
  HMdcTBArray::unset(iLayer, pos, time);
  if(*iLayer == 0) shiftLeft(lay);
}

inline void HMdcList24GroupCells::shiftLeft(Int_t lay) {
  UChar_t* iLayer=arr+lay*laySz;
  UChar_t* pEnd=iLayer+11;
  Int_t sh=HMdcTBArray::first(iLayer,pEnd);
  if(sh < 0) firstCell[lay]=0;
  else {
    sh = sh>>2;
    HMdcTBArray::shiftLeft(iLayer,pEnd,iLayer+sh);
    firstCell[lay] += (sh<<2);
  }
}

// inline Int_t HMdcList24GroupCells::getNCells(Int_t lay, Int_t layEnd) const {
//   if(layEnd==-1) layEnd=lay;
//   if(!isLayOk(lay) || !isLayOk(layEnd) || lay>layEnd) return -1;
//   return HMdcTBArray::getNSet(arr+lay*laySz,arr+layEnd*laySz+11);
// }

inline Int_t HMdcList24GroupCells::getNCells(Int_t lay) const {
  if(!isLayOk(lay)) return -1;
  const UChar_t *arrLay = arr+lay*laySz;
  return HMdcTBArray::getNSet(arrLay,arrLay+11);
}

inline Int_t HMdcList24GroupCells::getNCellsMod(Int_t mod) const {
  if( !isModOk(mod) ) return -1;
  const UChar_t *arrLay = arr+mod*6*laySz;
  return HMdcTBArray::getNSet(arrLay,arrLay+71); // 71 = 5*laySz+11
}

inline Int_t HMdcList24GroupCells::getNCells(void) const {
  return HMdcTBArray::getNSet(arr,arr+287);   // 287 = 23*laySz+11
}

inline Int_t HMdcList24GroupCells::getNDrTimes(void) const {
  return HMdcBArray::getNSet(arr,arr+287);
}

inline Int_t HMdcList24GroupCells::getNDrTimes(Int_t lay) const {
  if(!isLayOk(lay)) return 0;
  const UChar_t* layArr = arr+lay*laySz;
  if( *layArr ) return HMdcBArray::getNSet(layArr,layArr+11);
  return 0;
}

inline Int_t HMdcList24GroupCells::getNDrTimes(Int_t lay1,Int_t lay2) const {
  if(lay1<0 || lay2>=24) return -1;
  return HMdcBArray::getNSet(arr+lay1*laySz,arr+lay2*laySz+11);
}

inline Int_t HMdcList24GroupCells::getCell(Int_t lay, Int_t idx) const {
  // return: -1 - idx <0 or >num.cells
  if(!isLayOk(lay)) return -1;
  const UChar_t* layArr = arr+lay*laySz;
  Int_t pos=HMdcTBArray::position(layArr,layArr+11,idx);
  if(pos >= 0) return pos+firstCell[lay];
  return -1;
}

inline Int_t HMdcList24GroupCells::getFirstCell(Int_t lay) const {
  // return: -1 - num.cells=0
  if(!isLayOk(lay)) return -1;
  const UChar_t* layArr = arr+lay*laySz;
  if(*layArr == 0) return -1;
  Int_t pos=HMdcTBArray::first(layArr,layArr+11);
  if(pos >= 0) return pos+firstCell[lay];
  return -1;
}

inline Int_t HMdcList24GroupCells::get4FirstCells(Int_t lay, Int_t* list) const {
  // return: first cell (frCell) or -1 (num.cells=0)
  // fill list[0]-time of frCell, time frCell+1, ..., time frCell+3
  if(!isLayOk(lay)) return -1;
  const UChar_t* addr=arr+lay*laySz;
  Int_t pos=HMdcTBArray::first(addr,addr+11);
  if(pos < 0) return -1;
  Int_t fcell=firstCell[lay]+pos;
  Int_t posLast=pos+4;
  for(Int_t ps=pos; ps<posLast; ps++,list++) *list = HMdcTBArray::get(addr,ps);
  return fcell;
}

inline UShort_t HMdcList24GroupCells::getOneLayerGroup(Int_t lay) const {
  // For comparing with list in HMdcCellGroup!
  // return cell list in format of HMdcCellGroup
  if(!isLayOk(lay)) return 0;
  const UChar_t* addr=arr+lay*laySz;
  Int_t pos=HMdcTBArray::first(addr,addr+11);
  if(pos < 0) return 0;
  Int_t    posLast = pos+4;
  UShort_t list    = UShort_t(firstCell[lay]+pos) << 8;
  Int_t    sh      = 0;
  for(Int_t ps=pos; ps<posLast; ps++,sh += 2) {
    UChar_t t = HMdcTBArray::get(addr,ps);
    if(t != 0) list |= t<<sh;
  }
  return list;
}

inline Int_t HMdcList24GroupCells::getNLayers(void) const {
  Int_t nLayers=0;
  for(Int_t lay=0; lay<24; lay++) if(getFirstCell(lay)>=0) nLayers++;
  return nLayers;
}

inline Int_t HMdcList24GroupCells::getNLayersMod(Int_t mod) const {
  if(mod<0 || mod>3) return 0;
  Int_t nLayers=0;
  for(Int_t lay=mod*6; lay<(mod+1)*6; lay++) if(getFirstCell(lay)>=0) nLayers++;
  return nLayers;
}

inline UChar_t HMdcList24GroupCells::getListLayers(Int_t imod) const {
  // imod - module number in sector (0 - 4)
  // imod== -1 - List_imod0 | List_imod1 | List_imod2 | List_imod3
  // return 6 bits, one bit per layer
  if(imod<-1 || imod>3) return 0;
  Int_t im1 = imod==-1 ? 0:imod;
  Int_t im2 = imod==-1 ? 3:imod;
  UChar_t layList = 0;
  for(Int_t im=im1;im<=im2;im++) {
    Int_t lsh       = im*6;
    for(Int_t l=0;l<6;l++) if(arr[(l+lsh)*laySz] > 0) layList |= 1<<l;
  }
  return layList;
}

inline Int_t HMdcList24GroupCells::getLastCell(Int_t lay) const {
  // return: -2 - num.cells=0
  if(!isLayOk(lay)) return -2;
  const UChar_t* layArr = arr+lay*laySz;
  if(*layArr == 0) return -2;
  Int_t pos=HMdcTBArray::last(layArr,layArr+11);
  if(pos >= 0) return pos+firstCell[lay];
  return -2;
}

inline Int_t HMdcList24GroupCells::next(Int_t lay, Int_t prCell) const {
  if(!isLayOk(lay)) return -1;
  const UChar_t* layArr = arr+lay*laySz;
  Int_t cell=HMdcTBArray::next(layArr,layArr+11,prCell-firstCell[lay]);
  if(cell>=0) return cell+firstCell[lay];
  return -1;
}

inline Bool_t HMdcList24GroupCells::getNext(Int_t lay, Int_t& cell) const {
  cell=next(lay,cell);
  if(cell>=0) return kTRUE;
  return kFALSE;
}

inline Bool_t HMdcList24GroupCells::getNextCell(Int_t& lay, Int_t& cell) const {
  if(lay<0) {
    lay=0;
    cell=-1;
  }
  for(;lay<24;lay++) if(getNext(lay,cell)) return kTRUE;
  return kFALSE;
}

inline Int_t HMdcList24GroupCells::previous(Int_t lay, Int_t prCell) const {
  if(!isLayOk(lay)) return kFALSE;
  const UChar_t* layArr = arr+lay*laySz;
  if(*layArr == 0) return -1;          // no wires in layer
  Int_t cell = HMdcTBArray::previous(layArr,layArr+11,prCell-firstCell[lay]);
  if(cell>=0) return cell+firstCell[lay];
  return -1;
}

inline UChar_t HMdcList24GroupCells::getTime(Int_t lay, Int_t cell) const {
  Int_t pos=cell-firstCell[lay];
  if(pos<0 || pos>=48) return 0;
  return HMdcTBArray::get(arr+lay*laySz,pos);
}

inline void HMdcList24GroupCells::setInBounds(Int_t& l1, Int_t& l2) const {
  if(l1<0) {
    l1=0;
    l2=23;
  } else  {
    if(l2<0) l2=l1;
    else if(l2>23) l2=23;
  }
}


#endif
