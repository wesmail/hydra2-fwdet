///////////////////////////////////////////////////////////////////////////
//  HGeantRpc
//  GEANT RPC hit data
///////////////////////////////////////////////////////////////////////////

#ifndef HGEANTRPC_H
#define HGEANTRPC_H

#include "hlinkeddataobject.h"

class HGeantRpc : public HLinkedDataObject
{
private:

  Int_t    trackNumber;       // GEANT track number
  Float_t  trackLength;       // track length at the RPC gap/cell.                                   [mm]
  Float_t  loctrackLength;    // local track length (inside the gap/cell).                           [mm]
  Float_t  eHit;              // energy deposited in the RPC gap/cell.                               [MeV]
  Float_t  xHit;              // x at the RPC gap/cell, in module ref. system (EBOX).                [mm]
  Float_t  yHit;              // y at the RPC gap/cell, in module ref. system (EBOX).                [mm]
  Float_t  zHit;              // z at the RPC gap/cell, in module ref. system (EBOX).                [mm]
  Float_t  tofHit;            // time of flight at the RPC gap/cell.                                 [ns]
  Float_t  momHit;            // momentum at the RPC gap/cell.                                       [MeV/c]
  Float_t  thetaHit;          // polar angle at the RPC gap/cell, in module ref. system (EBOX).      [deg]
  Float_t  phiHit;            // azimuthal angle at the RPC gap/cell, in module ref. system (EBOX).  [deg]
  Short_t  detectorID;        // detector ID (codified sector/column/cell/gap info)
  // New elements after Jan 2013.
  Float_t  loctrackLength1;   // local track length (inside the gap).                                [mm]
  Float_t  eHit1;             // energy deposited in the RPC gap.                                    [MeV]
  Float_t  xHit1;             // x at the RPC gap, in module ref. system (EBOX).                     [mm]
  Float_t  yHit1;             // y at the RPC gap, in module ref. system (EBOX).                     [mm]
  Float_t  momHit1;           // momentum at the RPC gap.                                            [MeV/c]
  Float_t  loctrackLength2;   // local track length (inside the gap).                                [mm]
  Float_t  eHit2;             // energy deposited in the RPC gap.                                    [MeV]
  Float_t  xHit2;             // x at the RPC gap, in module ref. system (EBOX).                     [mm]
  Float_t  yHit2;             // y at the RPC gap, in module ref. system (EBOX).                     [mm]
  Float_t  momHit2;           // momentum at the RPC gap.                                            [MeV/c]
  Float_t  loctrackLength3;   // local track length (inside the gap).                                [mm]
  Float_t  eHit3;             // energy deposited in the RPC gap.                                    [MeV]
  Float_t  xHit3;             // x at the RPC gap, in module ref. system (EBOX).                     [mm]
  Float_t  yHit3;             // y at the RPC gap, in module ref. system (EBOX).                     [mm]
  Float_t  momHit3;           // momentum at the RPC gap.                                            [MeV/c]
  Short_t  HGeantRpc_version; //! HGeantRpc class version when reading from file.

public:
  HGeantRpc(void);
//  HGeantRpc(HGeantRpc &aRpc);
  ~HGeantRpc(void);


  // Functions setVariable
  inline void    setTrack(Int_t atrackNumber) {trackNumber = atrackNumber;}
  inline void    setDetectorID(Short_t adetectorID) {detectorID = adetectorID;}
  inline void    setAddress(Int_t sec, Int_t col, Int_t cel, Int_t gap);
  inline void    setVersion(Int_t aHGeantRpc_version) {HGeantRpc_version = aHGeantRpc_version;}
         void    setIncidence(Float_t athetaHit, Float_t aphiHit);
  //
  // OLD set functions for backward compatibility.
  //
         void    setHit(Float_t axHit, Float_t ayHit, Float_t azHit, Float_t atofHit,
                        Float_t amomHit, Float_t eHit);
         void    setTLength(Float_t atracklength, Float_t aloctracklength);
  //
  // NEW HIT set functions.
  //
         void    setHit(Float_t axHit, Float_t ayHit, Float_t azHit, Float_t atofHit,
                        Float_t amomHit, Float_t eHit, Float_t aloctracklength);
  //
         void    setGap(Int_t nGap, Float_t axHit, Float_t ayHit, Float_t amomHit,
                        Float_t eHit, Float_t aloctrackLength);
  inline void    setTLengthHit(Float_t atrackLength) {trackLength = atrackLength;};
  inline void    setZHit(Float_t azHit) {zHit = azHit;};
  inline void    setTofHit(Float_t atofHit) {tofHit = atofHit;};


  // Functions getVariable
  inline Int_t   getSector(void) const;
  inline Int_t   getColumn(void) const;
  inline Int_t   getCell(void)   const;
  inline Int_t   getGap(void)    const;

  inline Int_t   getTrack(void)      {return trackNumber;};
  inline Int_t   getDetectorID(void) {return detectorID;};
  inline Int_t   getNLocationIndex(void) {return 4;};
  inline Int_t   getLocationIndex(Int_t i);
  inline Int_t   getVersion(void) {return HGeantRpc_version;};
  //
  //  OLD get functions for backward compatibility.
  //
         void    getIncidence(Float_t& athetaHit, Float_t& aphiHit);
         void    getTLength(Float_t& atracklength, Float_t& aloctracklength);
         void    getHit(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                        Float_t& atofHit, Float_t& amomHit, Float_t& aeHit);
  //
  // NEW GAP get functions.
  //
  // Various options are available to avoid retrieving unnecessary information.
         Float_t getlocTLengthGap(Int_t nGap);
         void    getGap(Int_t nGap, Float_t& axHit, Float_t& ayHit, Float_t& amomHit,
                        Float_t& aeHit, Float_t& aloctrackLength);
         void    getGap(Int_t nGap, Float_t& axHit, Float_t& ayHit, Float_t& amomHit,
                        Float_t& aeHit);
         void    getGap(Int_t nGap, Float_t& axHit, Float_t& ayHit, Float_t& amomHit);
  //
  // NEW HIT get functions.
  //
  inline Float_t getTLengthHit(void) {return trackLength;};
  inline Float_t getZHit(void) {return zHit;};
  inline Float_t getTofHit(void) {return tofHit;};
  // Various options are available to avoid retrieving unnecessary information.
         void    getHit(Float_t& axHit, Float_t& ayHit, Float_t& azHit, Float_t& atofHit,
                        Float_t& amomHit, Float_t& aeHit, Float_t& aloctrackLength);
         void    getHit(Float_t& axHit, Float_t& ayHit, Float_t& azHit, Float_t& atofHit,
                        Float_t& amomHit);
         void    getHit(Float_t& axHit, Float_t& ayHit, Float_t& azHit, Float_t& atofHit);
  // Optimized for digitizer.
         void    getHitDigi(Float_t& axHit, Float_t& atofHit, Float_t& amomHit,
                            Float_t& aloctrackLength);
  //
  // NEW CELL get functions.
  //
  // Various options are available to avoid retrieving unnecessary information.
         void    getCellAverage(Float_t gap, Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                                Float_t& atofHit, Float_t& amomHit, Float_t& aeHit,
                                Float_t& aloctrackLength);
         void    getCellAverage(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                                Float_t& atofHit, Float_t& amomHit, Float_t& aeHit);
         void    getCellAverage(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                                Float_t& atofHit, Float_t& amomHit);
         void    getCellAverage(Float_t& axHit, Float_t& ayHit, Float_t& azHit,
                                Float_t& atofHit);
  // Optimized for digitizer.
         void    getCellAverageDigi(Float_t gap, Float_t& axHit, Float_t& atofHit,
                                    Float_t& amomHit, Float_t& aloctrackLength);
  //
  ClassDef(HGeantRpc,5)
};

inline Int_t  HGeantRpc::getSector(void) const{
  if (detectorID<0) return -1*(detectorID+1);
  else return detectorID>>11;
}

inline Int_t  HGeantRpc::getColumn(void) const{
  if (detectorID<0) return -1;
  else return ((detectorID>>8)  & 7);
}

inline Int_t  HGeantRpc::getCell(void) const{
  if (detectorID<0) return -1;
  else return ((detectorID>>2)  & 63);
}

inline Int_t  HGeantRpc::getGap(void) const {
  if (detectorID<0) return -1;
  else return (detectorID & 3);
}

inline void HGeantRpc::setAddress(Int_t sec, Int_t col, Int_t cel, Int_t gap) {
  detectorID = (sec<<11) + (col<<8) + (cel<<2) + gap;
}

inline Int_t HGeantRpc::getLocationIndex(Int_t i) {
  switch (i) {
    case 0 : return getSector();
    case 1 : return getColumn();
    case 2 : return getCell();
    case 3 : return getGap();
  }
  return -1;
}


#endif  /*! HGEANTRPC_H */
