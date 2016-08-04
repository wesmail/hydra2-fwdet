//*-- Author: M. Jurkovic

#ifndef HSTART2HIT_H
#define HSTART2HIT_H

#include "TObject.h"

#define MAXNSTRIP 5

class HStart2Hit : public TObject {
private:
   Int_t   fModule;        // module number
   Int_t   fStrip;         // strip number
   Int_t   fMultiplicity;  // Start det. Multip.
   Float_t fTime;          // Start det. time
   Float_t fWidth;         // Start det. ADC value
   Bool_t  fFlag;          // kTRUE if true start time has been found
   Int_t   fCorrFlag;      // flag of reconstruction method of HParticleStart2HitF
   Float_t fResolution;    // resolution smearing for simulation
   Float_t fSimWidth;      // width of gaussian smearing for simulation [ns]
   Int_t   fTrack;         // Geant track number of fastest hit
   UInt_t  fIteration;     // 0 init, 1 HStart2HitF ,2 HParticleStart2HitF

   Float_t fTime2;         // Start det. time of second cluster (closest to first clust)
   Int_t nFirstCluster;             // nNumber of cal objects of first cluster
   Int_t nSecondCluster;            // nNumber of cal objects of second cluster
   Int_t fFirstCluster [MAXNSTRIP]; // cal objects of first  cluster (index*1000+hitindex+1)
   Int_t fSecondCluster[MAXNSTRIP]; // cal objects of second cluster (index*1000+hitindex+1)


public:
    HStart2Hit(void);
    ~HStart2Hit(void) {}

    void    setFlag        (const Bool_t f)                   { fFlag = f;}
    void    setCorrFlag    (const Int_t f)                    { fCorrFlag = f;}
    void    setMultiplicity(const Int_t m)                    { fMultiplicity = m; }
    void    setAddress     (const Int_t m, const Int_t s)     { fModule = m; fStrip = s; }
    void    setTimeAndWidth(const Float_t t, const Float_t w) { fTime   = t; fWidth = w; }
    void    setResolution  (const Float_t res)                { fResolution = res; }
    void    setSimWidth    (const Float_t width)              { fSimWidth = width; }
    void    setTrack       (const Int_t t)                    { fTrack = t; }
    void    setIteration   (const UInt_t it)                  {fIteration = it;}
    Bool_t  getFlag        (void)  const   { return fFlag; }
    Int_t   getCorrFlag    (void)  const   { return fCorrFlag; }
    Int_t   getModule      (void)  const   { return fModule; }
    Int_t   getStrip       (void)  const   { return fStrip; }
    Int_t   getMultiplicity(void)  const   { return fMultiplicity; }
    Float_t getTime        (void)  const   { return fTime; }
    Float_t getWidth       (void)  const   { return fWidth; }
    void    getAddress     (Int_t& m  , Int_t& s  ) { m = fModule; s = fStrip; }
    void    getTimeAndWidth(Float_t& t, Float_t& w) { t = fTime;   w = fWidth; }
    Float_t getResolution  (void)  const { return fResolution; }
    Float_t getSimWidth    (void)  const { return fSimWidth; }
    Int_t   getTrack       (void)  const   { return fTrack; }
    UInt_t  getIteration   (void)  const   { return fIteration; }

    //------------------------------------------------------------------
    // cluster infos
    void    setSecondTime(Float_t t) { fTime2 = t;  }
    Float_t getSecondTime()          { return fTime2 ;  }

    void    resetClusterStrip(UInt_t firstOrSecond=2);
    Bool_t  setClusterStrip  (UInt_t firstOrSecond,Int_t stripandhitindex);
    Bool_t  setClusterStrip  (UInt_t firstOrSecond,Int_t stripindex,Int_t hitindex);
    Int_t   getClusterStrip  (UInt_t firstOrSecond,Int_t num);
    Int_t   getClusterStrip  (UInt_t firstOrSecond,Int_t num,Int_t& stripindex,Int_t& hitindex);


    Int_t   getFirstClusterSize () {return nFirstCluster;}
    Int_t   getSecondClusterSize() {return nSecondCluster;}
    Bool_t  getMaxClusterSize()    {return MAXNSTRIP;}
    //------------------------------------------------------------------



    // void    Streamer(TBuffer &R__b);

   ClassDef(HStart2Hit, 6) // START detector hit data
};



#endif /* ! HSTART2HIT_H */
