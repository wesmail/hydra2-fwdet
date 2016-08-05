#ifndef HKALHIT_H
#define HKALHIT_H

#include "TObject.h"

class HKalSite : public TObject {

private:
    Int_t sec;        // Index of sector.
    Int_t mod;        // Index of module.
    Int_t lay;        // Index of layer.

    Float_t CxxReco;  // Covariance matrix elements.
    Float_t CyyReco;  // Covariance matrix elements.
    Float_t CtxReco;  // Covariance matrix elements.
    Float_t CtyReco;  // Covariance matrix elements.
    Float_t CqpReco;  // Covariance matrix elements.
    Float_t chi2;     // This site's contribution to the chi^2 of the fit.
    Float_t enerLoss; // Calculated energy loss up to this site.
    Int_t   idxFirst; // Index of site's first competing hit.
    Int_t   idxLast;  // Index of site's last competing hit.
    Float_t momFilt;  // Momentum from filter step.
    Float_t momReal;  // Momentum from simulation data.
    Float_t momSmoo;  // Smoothed momentum.
    Int_t   nComp;    // Number of competing hits.
    Int_t   trackNum; // Geant track number.
    Float_t txReco;   // tan(px/pz) from smoothing.
    Float_t txReal;   // tan(px/pz) from simulation
    Float_t tyReco;   // tan(py/pz) from smoothing.
    Float_t tyReal;   // tan(py/pz) from simulation

public:

    HKalSite();

    virtual ~HKalSite() { ; }

    virtual Float_t getChi2    () const { return chi2; }

    virtual Float_t getCxxReco () const { return CxxReco; }

    virtual Float_t getCyyReco () const { return CyyReco; }

    virtual Float_t getCtxReco () const { return CtxReco; }

    virtual Float_t getCtyReco () const { return CtyReco; }

    virtual Float_t getCqpReco () const { return CqpReco; }

    virtual Int_t   getSec     () const { return sec; }

    virtual Int_t   getMod     () const { return mod; }

    virtual Int_t   getLay     () const { return lay; }

    virtual Float_t getEnerLoss() const { return enerLoss; }

    virtual Int_t   getIdxFirst() const { return idxFirst; }

    virtual Int_t   getIdxLast () const { return idxLast; }

    virtual Float_t getMomFilt () const { return momFilt; }

    virtual Float_t getMomReal () const { return momReal; }

    virtual Float_t getMomSmoo () const { return momSmoo; }

    virtual Int_t   getNcomp   () const { return nComp; }

    virtual Int_t   getTrackNum() const { return trackNum; }

    virtual Float_t getTxReal  () const { return txReal; }

    virtual Float_t getTxReco  () const { return txReco; }

    virtual Float_t getTyReal  () const { return tyReal; }

    virtual Float_t getTyReco  () const { return tyReco; }

    virtual void    setSec     (Int_t s)      { sec = s; }

    virtual void    setMod     (Int_t m)      { mod = m; }

    virtual void    setLay     (Int_t l)      { lay = l; }

    virtual void    setCxxReco (Float_t c)    { CxxReco = c; }

    virtual void    setCyyReco (Float_t c)    { CyyReco = c; }

    virtual void    setCtxReco (Float_t c)    { CtxReco = c; }

    virtual void    setCtyReco (Float_t c)    { CtyReco = c; }

    virtual void    setCqpReco (Float_t c)    { CqpReco = c; }

    virtual void    setChi2    (Double_t c)   { chi2 = c; }

    virtual void    setEnerLoss(Float_t dedx) { enerLoss = dedx; }

    virtual void    setIdxFirst(Int_t i)      { idxFirst = i; }

    virtual void    setIdxLast (Int_t i)      { idxLast = i; }

    virtual void    setMomFilt (Float_t mom)  { momFilt = mom; }

    virtual void    setMomReal (Float_t mom)  { momReal = mom; }

    virtual void    setMomSmoo (Float_t mom)  { momSmoo = mom; }

    virtual void    setNcomp   (Int_t n)      { nComp = n; }

    virtual void    setTrackNum(Int_t n)      { trackNum = n; }

    virtual void    setTxReal  (Float_t tx)   { txReal = tx; }

    virtual void    setTxReco  (Float_t tx)   { txReco = tx; }

    virtual void    setTyReal  (Float_t ty)   { tyReal = ty; }

    virtual void    setTyReco  (Float_t ty)   { tyReco = ty; }

    ClassDef (HKalSite,1)
};



class HKalHit2d : public TObject {

private:

    Float_t xMeas; // Measured position in sector coordinates in mm.
    Float_t yMeas; // Measured position in sector coordinates in mm.
    Float_t xReal; // Geant position in sector coordinates in mm.
    Float_t yReal; // Geant position in sector coordinates in mm.
    Float_t xReco; // Reconstructed position in sector coordinates in mm.
    Float_t yReco; // Reconstructed position in sector coordinates in mm.

public:

    HKalHit2d();

    virtual ~HKalHit2d() { ; }

    virtual Float_t getXmeas() const { return xMeas; }

    virtual Float_t getYmeas() const { return yMeas; }

    virtual Float_t getXreal() const { return xReal; }

    virtual Float_t getYreal() const { return yReal; }

    virtual Float_t getXreco() const { return xReco; }

    virtual Float_t getYreco() const { return yReco; }

    virtual void    setXmeas   (Float_t x) { xMeas = x; }

    virtual void    setYmeas   (Float_t y) { yMeas = y; }

    virtual void    setXreal   (Float_t x) { xReal = x; }

    virtual void    setYreal   (Float_t y) { yReal = y; }

    virtual void    setXreco   (Float_t x) { xReco = x; }

    virtual void    setYreco   (Float_t y) { yReco = y; }

    ClassDef (HKalHit2d,1)
};



class HKalHitWire : public TObject {

private:

    static const Int_t nDafs = 5; // Max. number of iterations done in the annealing filter.

    Float_t alpha;           // Reconstructed angle of track and wire plane in degrees. 0 < alpha < 90°
    Int_t   cell;            // Student's prison cell number.
    Float_t chi2Daf;         // Hit's chi2, i.e. the weighted, squared distance of the measurement to the smoothed track state.
    Float_t mindist;         // Reconstructed drift radius in mm.
    Float_t t1;              // Drift time of first hit in ns = time1 from HMdcCal1Sim.
    Float_t t2;              // Drift time of second hit in ns = time 2 from HMdcCal1Sim.
    Float_t t1Err;           // Error of time 1 from HMdcCal1Sim in ns.
    Float_t tReco;           // Reconstructed drift time 1 in ns. Calculated from alpha and mindist.
    Float_t tMeas;           // Measured time. Input for the Kalman filter. Includes tWireOffset and tTof.
    Float_t tTof;            // Time of flight correction of time 1 used by the Kalman filter.
    Float_t tTofCal1;        // Time of flight correction of time 1 from HMdcCal1Sim.
    Float_t tWireOffset;     // Travel time on wire correction of time 1.
    Float_t weight[nDafs];   // Assignment probabilities in each iteration.

public:

    HKalHitWire();

    virtual ~HKalHitWire() { ; }

    virtual Float_t getAlpha         () const  { return alpha; }

    virtual Int_t   getCell          () const  { return cell; }

    virtual Float_t getChi2Daf       () const { return chi2Daf; }

    virtual Float_t getMinDist       () const  { return mindist; }

    virtual Int_t   getNdafs         () const { return nDafs; }

    virtual Float_t getTime1         () const  { return t1; }

    virtual Float_t getTime2         () const  { return t2; }

    virtual Float_t getTime1Err      () const  { return t1Err; }

    virtual Float_t getTimeMeas      () const  { return tMeas; }

    virtual Float_t getTimeReco      () const  { return tReco; }

    virtual Float_t getTimeTof       () const  { return tTof; }

    virtual Float_t getTimeTofCal1   () const  { return tTofCal1; }

    virtual Float_t getTimeWireOffset() const  { return tWireOffset; }

    virtual Float_t getWeight        (Int_t i) const { if(i >=0 && i < nDafs) { return weight[i]; } return -1.F; }

    virtual void    setAlpha         (Float_t a)          { alpha = a; }

    virtual void    setCell          (Int_t c)            { cell = c; }

    virtual void    setChi2Daf       (Float_t chi2) { chi2Daf = chi2; }

    virtual void    setTime1         (Float_t t)          { t1 = t; }

    virtual void    setTime2         (Float_t t)          { t2 = t; }

    virtual void    setTime1Err      (Float_t tErr)       { t1Err = tErr; }

    virtual void    setTimeMeas      (Float_t t)          { tMeas = t; }

    virtual void    setTimeReco      (Float_t t)          { tReco = t; }

    virtual void    setTimeTof       (Float_t t)          { tTof = t; }

    virtual void    setTimeTofCal1   (Float_t t)          { tTofCal1 = t; }

    virtual void    setTimeWireOffset(Float_t t)          { tWireOffset = t; }

    virtual void    setMinDist       (Float_t d)          { mindist = d; }

    virtual void    setWeight        (Float_t w, Int_t i) { if(i >= 0 && i < nDafs) weight[i] = w; }

    ClassDef (HKalHitWire,1)
};

#endif
