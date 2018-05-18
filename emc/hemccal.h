#ifndef HEMCCAL_H
#define HEMCCAL_H

using namespace std;

#include "TObject.h"

class HEmcCal : public TObject {
protected:
  UChar_t nHits;        // number of hits in this cell
  Float_t time;         // time of hit
  Float_t energy;       // energy deposited in one crystal
  Char_t  sector;       // sector number (0..5)
  UChar_t cell;         // cell number (0..254)
  Char_t  row;          // row number (0..14)
  Char_t  column;       // column number (0..16)
  Bool_t  isRpcMatch;   // kTRUE Emc cell has matching with RPC
  Short_t clusterIndex; // index of EmcCluster object
  Short_t statusTime;   // >0 - ok.; <0 - hit was kicked out by threshold; =0-real data

public:
  HEmcCal() :
    nHits(0),
    time(-999.F),
    energy(-999.F),
    sector(-1),
    cell(0),
    row(-1),
    column(-1),
    isRpcMatch(kFALSE),
    clusterIndex(-1),
    statusTime(0)
  {
  }
  ~HEmcCal() {}
  virtual void clear(void);

  void setAddress(Char_t se, UChar_t ce, Char_t ro, Char_t co);
  void setNHits(UChar_t n)        {nHits  = n;}
  void setTime(Float_t t)         {time   = t;}
  void setEnergy(Float_t e)       {energy = e;}
  void setSector(Char_t s)        {sector = s;}
  void setCell(UChar_t c)         {cell   = c;}
  void setRow(Char_t r)           {row    = r;}
  void setColumn(Char_t c)        {column = c;}
  void setMatchedRpc(Bool_t f=kTRUE)    {isRpcMatch  = f;}
  void setClusterIndex(Short_t i) {clusterIndex = i;}
  void setStatus(Int_t f)         {statusTime = f;}

  void    getAddress(Char_t & se, UChar_t & ce, Char_t & ro, Char_t & co);
  UChar_t getNHits(void)        const {return nHits;}
  Float_t getTime(void)         const {return time;}
  Float_t getEnergy(void)       const {return energy;}
  Char_t  getSector(void)       const {return sector;}
  UChar_t getCell(void)         const {return cell;}
  Char_t  getRow(void)          const {return row;}
  Char_t  getColumn(void)       const {return column;}
  Bool_t  isMatchedRpc(void)    const {return isRpcMatch; }
  Short_t getClusterIndex(void) const {return clusterIndex;}
  Short_t getStatus(void)       const {return statusTime;}

  ClassDef(HEmcCal,1) //EMC cal data class
};

inline void HEmcCal::setAddress(Char_t se, UChar_t ce, Char_t ro, Char_t co) {
  sector = se;
  cell   = ce;
  row    = ro;
  column = co;
}

inline void HEmcCal::getAddress(Char_t & se, UChar_t & ce, Char_t & ro, Char_t & co) {
  se = sector;
  ce = cell;
  ro = row;
  co = column;
}

#endif /* !HEMCCAL_H */
