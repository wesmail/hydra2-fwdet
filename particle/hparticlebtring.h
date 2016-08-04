#ifndef __HPARTICLEBTRING_H__
#define __HPARTICLEBTRING_H__

#include "TObject.h"

class HParticleBtRing : public TObject
{
private:
    Int_t fPadsRing;
    Int_t fPadsSum;
    Float_t fChargeRing;
    Float_t fChargeSum;

    Int_t fClusters;
    Int_t fMaxima;
    Float_t fMaximaCharge;
    Int_t fNearbyMaxima;

    Float_t fChi2;
    Float_t fMeanDist;
    Float_t fRingMatrix;

    Int_t fMaximaShared;
    Int_t fMaximaSharedTrack[10];
    Int_t fMaximaSharedTrackIdx[10];
    Int_t fMaximaSharedBad;
    Int_t fMaximaSharedBadTrack[10];
    Int_t fMaximaSharedBadTrackIdx[10];
    Float_t fMaximaChargeShared;
    Float_t fMaximaChargeSharedTrack[10];
    Int_t fMaximaChargeSharedTrackIdx[10];
    Float_t fMaximaChargeSharedBad;
    Float_t fMaximaChargeSharedBadTrack[10];
    Int_t fMaximaChargeSharedBadTrackIdx[10];
    Int_t fNearbyMaximaShared;
    Int_t fNearbyMaximaSharedTrack[10];
    Int_t fNearbyMaximaSharedTrackIdx[10];

    void init();

public:

    HParticleBtRing();
    ~HParticleBtRing();

    // ------------ set functions -----------
    void setPadsRing(    Int_t val )    { fPadsRing   = val;}
    void setPadsClus(    Int_t val )    { fPadsSum    = val;}
    void setChargeRing(  Float_t val )  { fChargeRing = val;}
    void setChargeClus(  Float_t val )  { fChargeSum  = val;}
  
    void setClusters(     Int_t val )   { fClusters     = val;}
    void setMaxima(       Int_t val )   { fMaxima       = val;}
    void setMaximaCharge( Float_t val ) { fMaximaCharge = val;}
    void setNearbyMaxima( Int_t val )   { fNearbyMaxima = val;}

    void setChi2(        Float_t val )  { fChi2        = val;}
    void setMeanDist(    Float_t val )  { fMeanDist    = val;}
    void setRingMatrix(  Float_t val )  { fRingMatrix  = val;}

    void setMaximaShared(               Int_t  val )     { fMaximaShared          = val;}
    void setMaximaSharedTrack(          Int_t* val )     { for(Int_t i=0;i<10;i++) fMaximaSharedTrack[i]          = val[i];}
    void setMaximaSharedTrackIdx(       Int_t* val )     { for(Int_t i=0;i<10;i++) fMaximaSharedTrackIdx[i]       = val[i];}

    void setMaximaSharedFragment(            Int_t  val )     { fMaximaSharedBad       = val;}
    void setMaximaSharedFragmentTrack(       Int_t* val )     { for(Int_t i=0;i<10;i++) fMaximaSharedBadTrack[i]       = val[i];}
    void setMaximaSharedFragmentTrackIdx(    Int_t* val )     { for(Int_t i=0;i<10;i++) fMaximaSharedBadTrackIdx[i]    = val[i];}

    void setMaximaChargeShared(         Float_t  val )   { fMaximaChargeShared    = val;}
    void setMaximaChargeSharedTrack(    Float_t* val )   { for(Int_t i=0;i<10;i++) fMaximaChargeSharedTrack[i]    = val[i];}
    void setMaximaChargeSharedTrackIdx( Int_t* val   )   { for(Int_t i=0;i<10;i++) fMaximaChargeSharedTrackIdx[i] = val[i];}

    void setMaximaChargeSharedFragment(         Float_t  val )   { fMaximaChargeSharedBad    = val;}
    void setMaximaChargeSharedFragmentTrack(    Float_t* val )   { for(Int_t i=0;i<10;i++) fMaximaChargeSharedBadTrack[i]    = val[i];}
    void setMaximaChargeSharedFragmentTrackIdx( Int_t* val   )   { for(Int_t i=0;i<10;i++) fMaximaChargeSharedBadTrackIdx[i] = val[i];}
  
    void setNearbyMaximaShared(         Int_t  val )     { fNearbyMaximaShared    = val;}
    void setNearbyMaximaSharedTrack(    Int_t* val )     { for(Int_t i=0;i<10;i++) fNearbyMaximaSharedTrack[i]    = val[i];}
    void setNearbyMaximaSharedTrackIdx( Int_t* val )     { for(Int_t i=0;i<10;i++) fNearbyMaximaSharedTrackIdx[i] = val[i];}




    // ------------ get functions -----------

    Int_t   getPadsRing(void)       { return fPadsRing  ;}
    Int_t   getPadsClus(void)       { return fPadsSum   ;}
    Float_t getChargeRing(void)     { return fChargeRing;}
    Float_t getChargeClus(void)     { return fChargeSum ;}
  
    Int_t   getClusters(void)       { return fClusters    ;}
    Int_t   getMaxima(void)         { return fMaxima      ;}
    Float_t getMaximaCharge(void)   { return fMaximaCharge;}
    Int_t   getNearbyMaxima(void)   { return fNearbyMaxima;}

    Float_t getChi2(void)           { return fChi2       ;}
    Float_t getMeanDist(void)       { return fMeanDist   ;}
    Float_t getRingMatrix(void)     { return fRingMatrix ;}

    Int_t   getMaximaShared(void)                { return fMaximaShared       ;}
    Int_t   getMaximaSharedFragment(void)        { return fMaximaSharedBad    ;}
    Float_t getMaximaChargeShared(void)          { return fMaximaChargeShared ;}
    Float_t getMaximaChargeSharedFragment(void)  { return fMaximaChargeSharedBad ;}
    Int_t   getNearbyMaximaShared(void)          { return fNearbyMaximaShared ;}
    Int_t   getMaximaSharedTrack(Int_t idx);
    //Int_t   getMaximaSharedFragmentTrack(Int_t idx);
    Float_t getMaximaChargeSharedTrack(Int_t idx);
    //Float_t getMaximaChargeSharedFragmentTrack(Int_t idx);
    Int_t   getNearbyMaximaSharedTrack(Int_t idx);


    ClassDef(HParticleBtRing,2)
};
#endif  // __HPARTICLEBTRING_H__
