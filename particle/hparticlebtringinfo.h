#ifndef __HPARTICLEBTRINGINFO_H__
#define __HPARTICLEBTRINGINFO_H__

#include "TObject.h"

class HParticleBtRingInfo : public TObject
{
private:


    Int_t   fPrediction      [128][128]; //tracks,predicted pads
    Int_t   fRingMatrix      [128][128];
    Int_t   fRichHitAdd      [1024];    //fired pads
    Float_t fRichHitCharge   [1024];
    Int_t   fIsInCluster     [1024];    

    Float_t fTrackTheta      [128];
    Float_t fTrackPhi        [128];
    Int_t   fTrackVertex     [128];
    Int_t   fTrackSec        [128];
    Int_t   fTrackPCandIdx   [128];
    Bool_t  fIsGoodTrack     [128];
    Float_t fPosXCenter      [128];
    Float_t fPosYCenter      [128];


    //Cluster information
    Int_t   fClusTrackNo     [32][32];  //clusters, tracks
    Bool_t  fClusIsGood      [32];      //clusters
    Int_t   fClusPadSum      [32];
    Int_t   fClusPadRing     [32];
    Float_t fClusChargeSum   [32];
    Float_t fClusChargeRing  [32];
    Float_t fClusPosX        [32];
    Float_t fClusPosY        [32];
    Int_t   fClusClass       [32];

    //Maxima and maxima position
    Int_t   fClusNMaxima     [32][32];       //clusters,tracks
    Int_t   fClusNMaximaPad  [32][32][32];   //clusters,tracks,maxima
    Float_t fClusNMaximaPosX [32][32][32];
    Float_t fClusNMaximaPosY [32][32][32];

    //Chi2
    Float_t fClusChi2        [32][32][32];
    Float_t fClusChi2XMM     [32][32][32];
    Float_t fClusChi2YMM     [32][32][32];

    //Debug info
    Float_t fClusCircleX     [32][32][32];
    Float_t fClusCircleY     [32][32][32];
    Float_t fClusSig1X       [32][32][32];
    Float_t fClusSig1Y       [32][32][32];
    Float_t fClusSig2X       [32][32][32];
    Float_t fClusSig2Y       [32][32][32];
    void init();

public:

    HParticleBtRingInfo();
    ~HParticleBtRingInfo();

    // ------------ set functions -----------

    void setPrediction       ( Int_t    val[][128] )   { for(Int_t i=0;i<128;i++){ for(Int_t j=0;j<128;j++){ fPrediction[i][j] = val[i][j];}}}
    void setRingMatrix       ( Int_t    val[][128] )   { for(Int_t i=0;i<128;i++){ for(Int_t j=0;j<128;j++){ fRingMatrix[i][j] = val[i][j];}}}
    void setRichHitAdd       ( Int_t*   val        )   { for(Int_t i=0;i<1024;i++) fRichHitAdd[i]    = val[i];}
    void setRichHitCharge    ( Float_t* val        )   { for(Int_t i=0;i<1024;i++) fRichHitCharge[i] = val[i];}
    void setIsInCluster      ( Int_t*   val        )   { for(Int_t i=0;i<1024;i++) fIsInCluster[i]   = val[i];}


    void setTrackTheta       ( Float_t* val )       { for(Int_t i=0;i<128;i++) fTrackTheta[i]    = val[i];}
    void setTrackPhi         ( Float_t* val )       { for(Int_t i=0;i<128;i++) fTrackPhi[i]      = val[i];}
    void setTrackVertex      ( Int_t*   val )       { for(Int_t i=0;i<128;i++) fTrackVertex[i]   = val[i];}
    void setTrackSec         ( Int_t*   val )       { for(Int_t i=0;i<128;i++) fTrackSec[i]      = val[i];}
    void setTrackPCandIdx    ( Int_t*   val )       { for(Int_t i=0;i<128;i++) fTrackPCandIdx[i] = val[i];}
    void setIsGoodTrack      ( Bool_t*  val )       { for(Int_t i=0;i<128;i++) fIsGoodTrack[i]   = val[i];}
    void setPosXCenter       ( Float_t* val )       { for(Int_t i=0;i<128;i++) fPosXCenter[i]    = val[i];}
    void setPosYCenter       ( Float_t* val )       { for(Int_t i=0;i<128;i++) fPosYCenter[i]    = val[i];}


    void setClusTrackNo      ( Int_t    val[][32])  { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ fClusTrackNo[i][j] = val[i][j];}}}
    void setClusIsGood       ( Bool_t*  val )       { for(Int_t i=0;i<32;i++) fClusIsGood[i]     = val[i];}
    void setClusPadSum       ( Int_t*   val )       { for(Int_t i=0;i<32;i++) fClusPadSum[i]     = val[i];}
    void setClusPadRing      ( Int_t*   val )       { for(Int_t i=0;i<32;i++) fClusPadRing[i]    = val[i];}
    void setClusChargeSum    ( Float_t* val )       { for(Int_t i=0;i<32;i++) fClusChargeSum[i]  = val[i];}
    void setClusChargeRing   ( Float_t* val )       { for(Int_t i=0;i<32;i++) fClusChargeRing[i] = val[i];}
    void setClusPosX         ( Float_t* val )       { for(Int_t i=0;i<32;i++) fClusPosX[i]       = val[i];}
    void setClusPosY         ( Float_t* val )       { for(Int_t i=0;i<32;i++) fClusPosY[i]       = val[i];}
    void setClusClass        ( Int_t*   val )       { for(Int_t i=0;i<32;i++) fClusClass[i]      = val[i];}

    void setClusNMaxima      ( Int_t   val[][32]     )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ fClusNMaxima[i][j] = val[i][j];}}}
    void setClusNMaximaPad   ( Int_t   val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusNMaximaPad[i][j][k]  = val[i][j][k];}}}}
    void setClusNMaximaPosX  ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusNMaximaPosX[i][j][k] = val[i][j][k];}}}}
    void setClusNMaximaPosY  ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusNMaximaPosY[i][j][k] = val[i][j][k];}}}}

    void setClusChi2         ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusChi2[i][j][k]     = val[i][j][k];}}}}
    void setClusChi2XMM      ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusChi2XMM[i][j][k]  = val[i][j][k];}}}}
    void setClusChi2YMM      ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusChi2YMM[i][j][k]  = val[i][j][k];}}}}


    void setClusCircleX      ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusCircleX[i][j][k] = val[i][j][k];}}}}
    void setClusCircleY      ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusCircleY[i][j][k] = val[i][j][k];}}}}
    void setClusSig1X        ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusSig1X[i][j][k]   = val[i][j][k];}}}}
    void setClusSig1Y        ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusSig1Y[i][j][k]   = val[i][j][k];}}}}
    void setClusSig2X        ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusSig2X[i][j][k]   = val[i][j][k];}}}}
    void setClusSig2Y        ( Float_t val[][32][32] )       { for(Int_t i=0;i<32;i++){ for(Int_t j=0;j<32;j++){ for(Int_t k=0;k<32;k++){ fClusSig2Y[i][j][k]   = val[i][j][k];}}}}


    // ------------ get functions -----------

    Int_t*   getPrediction       ( void )       { return &fPrediction[0][0] ;}
    Int_t*   getRingMatrix       ( void )       { return &fRingMatrix[0][0] ;}
    Int_t*   getRichHitAdd       ( void )       { return &fRichHitAdd[0]    ;}
    Float_t* getRichHitCharge    ( void )       { return &fRichHitCharge[0] ;}
    Int_t*   getIsInCluster      ( void )       { return &fIsInCluster[0]   ;}


    Float_t* getTrackTheta       ( void )       { return &fTrackTheta[0]   ;}
    Float_t* getTrackPhi         ( void )       { return &fTrackPhi[0]     ;}
    Int_t*   getTrackVertex      ( void )       { return &fTrackVertex[0]  ;}
    Int_t*   getTrackSec         ( void )       { return &fTrackSec[0]     ;}
    Int_t*   getTrackPCandIdx    ( void )       { return &fTrackPCandIdx[0];}
    Bool_t*  getIsGoodTrack      ( void )       { return &fIsGoodTrack[0]  ;}
    Float_t* getPosXCenter       ( void )       { return &fPosXCenter[0]   ;}
    Float_t* getPosYCenter       ( void )       { return &fPosYCenter[0]   ;}

    Int_t*   getClusTrackNo      ( void )       { return &fClusTrackNo[0][0] ;}
    Bool_t*  getClusIsGood       ( void )       { return &fClusIsGood[0]     ;}
    Int_t*   getClusPadSum       ( void )       { return &fClusPadSum[0]     ;}
    Int_t*   getClusPadRing      ( void )       { return &fClusPadRing[0]    ;}
    Float_t* getClusChargeSum    ( void )       { return &fClusChargeSum[0]  ;}
    Float_t* getClusChargeRing   ( void )       { return &fClusChargeRing[0] ;}
    Float_t* getClusPosX         ( void )       { return &fClusPosX[0]       ;}
    Float_t* getClusPosY         ( void )       { return &fClusPosY[0]       ;}
    Int_t*   getClusClass        ( void )       { return &fClusClass[0]      ;}

    Int_t*   getClusNMaxima      ( void )       { return &fClusNMaxima[0][0]        ;}
    Int_t*   getClusNMaximaPad   ( void )       { return &fClusNMaximaPad[0][0][0]  ;}
    Float_t* getClusNMaximaPosX  ( void )       { return &fClusNMaximaPosX[0][0][0] ;}
    Float_t* getClusNMaximaPosY  ( void )       { return &fClusNMaximaPosY[0][0][0] ;}

    Float_t* getClusChi2         ( void )       { return &fClusChi2[0][0][0]     ;}
    Float_t* getClusChi2XMM      ( void )       { return &fClusChi2XMM[0][0][0]  ;}
    Float_t* getClusChi2YMM      ( void )       { return &fClusChi2YMM[0][0][0]  ;}


    Float_t* getClusCircleX      ( void )       { return &fClusCircleX[0][0][0] ;}
    Float_t* getClusCircleY      ( void )       { return &fClusCircleY[0][0][0] ;}
    Float_t* getClusSig1X        ( void )       { return &fClusSig1X[0][0][0]   ;}
    Float_t* getClusSig1Y        ( void )       { return &fClusSig1Y[0][0][0]   ;}
    Float_t* getClusSig2X        ( void )       { return &fClusSig2X[0][0][0]   ;}
    Float_t* getClusSig2Y        ( void )       { return &fClusSig2Y[0][0][0]   ;}




    ClassDef(HParticleBtRingInfo,1)
};

#endif  //--HPARTICLEBTRINGINFO_H__
