#ifndef HEMCCAL_H
#define HEMCCAL_H

using namespace std;

#include "TObject.h"

class HEmcCal : public TObject {
protected:
  UChar_t nHits;        // number of hits in this cell
  Float_t time1;        // time of first hit
  Float_t time2;        // time of second hit
  Float_t energy;       // energy deposited in one crystal
  Char_t  sector;       // sector number (0..5)
  UChar_t cell;         // cell number (0..254)
  Char_t  row;          // row number (0..14)
  Char_t  column;       // column number (0..16)
  Float_t sigmaEnergy;  //
  Float_t sigmaTime1;   //
  Float_t sigmaTime2;   // ?
  Bool_t  isRpcMatch;   // kTRUE Emc cell has matching with RPC
  Short_t clusterIndex; // index of EmcCluster object
  Short_t statusTime1;  // >0 - ok.; <0 - hit was kicked out by threshold; =0-real data
  Short_t statusTime2;  // >0 - ok.; <0 - hit was kicked out by threshold; =0-real data

public:
  HEmcCal() :
    nHits(0),
    time1(-999.F),
    time2(-999.F),
    energy(-999.F),
    sector(-1),
    cell(0),
    row(-1),
    column(-1),
    sigmaEnergy(0.F),
    sigmaTime1(0.F),
    sigmaTime2(0.F),
    isRpcMatch(kFALSE),
    clusterIndex(-1),
    statusTime1(0),
    statusTime2(0)
  {
  }
  ~HEmcCal() {}
  virtual void clear(void);

  void setAddress(Char_t se, UChar_t ce, Char_t ro, Char_t co);
  void setNHits(UChar_t n)        {nHits  = n;}
  void setTime1(Float_t t)        {time1  = t;}
  void setTime2(Float_t t)        {time2  = t;}
  void setEnergy(Float_t e)       {energy = e;}
  void setSector(Char_t s)        {sector = s;}
  void setCell(UChar_t c)         {cell   = c;}
  void setRow(Char_t r)           {row    = r;}
  void setColumn(Char_t c)        {column = c;}
  void setMatchedRpc(Bool_t f=kTRUE)    {isRpcMatch  = f;}
  void setSigmaEnergy(Float_t e)  {sigmaEnergy = e;}
  void setSigmaTime1(Float_t t)   {sigmaTime1  = t;}
  void setSigmaTime2(Float_t t)   {sigmaTime2  = t;}
  void setStatus1(Int_t f)        {statusTime1 = f;}
  void setStatus2(Int_t f)        {statusTime2 = f;}
  void setClusterIndex(Short_t i) {clusterIndex = i;}

  UChar_t getNHits(void)        const {return nHits;}
  Float_t getTime1(void)        const {return time1;}
  Float_t getTime2(void)        const {return time2;}
  Float_t getEnergy(void)       const {return energy;}
  Char_t  getSector(void)       const {return sector;}
  UChar_t getCell(void)         const {return cell;}
  Char_t  getRow(void)          const {return row;}
  Char_t  getColumn(void)       const {return column;}
  Bool_t  isMatchedRpc(void)    const {return isRpcMatch; }
  Float_t getSigmaEnergy(void)  const {return sigmaEnergy ;}
  Float_t getSigmaTime1(void)   const {return sigmaTime1;}
  Float_t getSigmaTime2(void)   const {return sigmaTime2;}
  Short_t getStatus1(void)      const {return statusTime1;}
  Short_t getStatus2(void)      const {return statusTime2;}
  Short_t getClusterIndex(void) const {return clusterIndex;}

  ClassDef(HEmcCal,1) //EMC cal data class
};

inline void HEmcCal::setAddress(Char_t se, UChar_t ce, Char_t ro, Char_t co) {
  sector = se;
  cell   = ce;
  row    = ro;
  column = co;
}

#endif /* !HEMCCAL_H */
