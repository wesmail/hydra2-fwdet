#ifndef HBASETRACK_H
#define HBASETRACK_H

#include "TObject.h"
#include "hsymmat.h"

class HBaseTrack:public TObject
{
  protected:
   Float_t z;            // From HMdcSeg               (in Lab.?)
   Float_t r;            // From HMdcSeg
   Float_t theta;        // From HMdcSeg
   Float_t phi;          // From HMdcSeg
   Short_t tofHitInd;    // Index of HTofHit
   Short_t tofClustInd;  // Index of HTofCluster
   Short_t showerHitInd; // Index of HShowerHit or HEmcCluster
   Short_t rpcClustInd;  // Index of HRpcCluster
   Float_t p;            // Momentum of particle
   Float_t beta;         // speed of paricle
   Float_t mass2;        // mass squared
   Float_t tof;          // time of flight
   Char_t polarity;      // polarity (+1 or -1)
   Char_t sector;        // Sector number
   HSymMat6 cov;         // covariance matrix of size 6
   Float_t metaEloss;    // corrected energy loss in TOF or TOFINO
   Float_t tarDist;
   Double_t tofdist;     // Track length from Target-to-META detector
   Float_t qIOMatch;     //Matching quality if inner and outer segments
 public:
  HBaseTrack();
  HBaseTrack(HBaseTrack &);
  virtual ~HBaseTrack(){;};
  void     setIOMatching  (Float_t _qIOMatch)           { qIOMatch = _qIOMatch;}
  void     setZ           (Float_t Z,Float_t err)       { z = Z; cov.setErr(0,err);}
  void     setR           (Float_t R,Float_t err)       { r = R; cov.setErr(1,err);}
  void     setP           (Float_t P,Float_t err)       { p = P; cov.setErr(2,err);}
  void     setTheta       (Float_t THETA, Float_t err)  { theta = THETA; cov.setErr(3,err);}
  void     setPhi         (Float_t PHI,Float_t err)     { phi   = PHI;   cov.setErr(4,err);}
  void     setTofHitInd   (Short_t tof)                 { tofHitInd    = tof;}
  void     setTofClustInd (Short_t tof)                 { tofClustInd  = tof;}
  void     setShowerHitInd(Short_t shower)              { showerHitInd = shower;}
  void     setEmcClustInd (Short_t emc)                 { showerHitInd = emc;}
  void     setRpcClustInd (Short_t rpc)                 { rpcClustInd  = rpc;}
  void     setMass2       (Float_t MASS2,Float_t err)   { mass2 = MASS2; cov.setErr(5,err);}
  void     setTof         (Float_t TOF)                 { tof = TOF;}
  void     setPolarity    (Char_t POLARYTY)             { polarity = POLARYTY;}
  void     setSector      (Char_t SECTOR)               { sector = SECTOR;}
  void     setBeta        (Float_t BETA)                { beta = BETA;}
  void     setMetaEloss   (Float_t e)                   { metaEloss = e;}
  void     setTarDist     (Float_t _tarDist)            { tarDist = _tarDist;}
  void     setTofDist     (Double_t d)                  { tofdist = d; }
  Float_t  getIOMatch     (void) const                  { return qIOMatch; }
  Float_t  getIOMatching  (void) const                  { return qIOMatch; }
  Float_t  getZ           (void) const                  { return z;}
  Float_t  getErrZ        (void)                        { return cov.getErr(0);}
  Float_t  getR           (void) const                  { return r;}
  Float_t  getErrR        (void)                        { return cov.getErr(1);}
  Float_t  getTheta       (void) const                  { return theta;}
  Float_t  getErrTheta    (void)                        { return cov.getErr(3);}
  Float_t  getPhi         (void) const                  { return phi;}
  Float_t  getErrPhi      (void)                        { return cov.getErr(4);}
  Bool_t   isOverlap      (void) const                  { return ((showerHitInd>-1 || rpcClustInd>-1) && (tofHitInd>-1 || tofClustInd>-1) ) ? kTRUE:kFALSE; }
  Int_t    getSystem      (void) const                  { return ( tofHitInd>-1 || tofClustInd>-1 ) ? 1 : ((showerHitInd>-1 || rpcClustInd>-1) ? 0:-1); } //???
  Short_t  getShowerHitInd(void) const                  { return showerHitInd;}
  Short_t  getEmcClustInd (void) const                  { return showerHitInd;}
  Short_t  getTofHitInd   (void) const                  { return tofHitInd;}
  Short_t  getTofClustInd (void) const                  { return tofClustInd;}
  Short_t  getRpcClustInd (void) const                  { return rpcClustInd;}
  Float_t  getP           (void) const                  { return p;}
  Float_t  getErrP        (void)                        { return cov.getErr(2);}
  Float_t  getMass2       (void)                        { return mass2;}
  Float_t  getErrMass2    (void)                        { return cov.getErr(5);}
  Float_t  getTof         (void) const                  { return tof;}
  Char_t   getPolarity    (void) const                  { return polarity;}
  Char_t   getSector      (void) const                  { return sector;}
  Float_t  getBeta        (void) const                  { return beta;}
  Float_t  getMetaEloss   (void) const                  { return metaEloss;}
  HSymMat &getCovariance  (void)                        { return cov; }
  Float_t  getTarDist     (void) const                  { return tarDist;}
  Double_t getTofDist     (void)                        { return tofdist; }
   
  ClassDef(HBaseTrack,2)
};
#endif
