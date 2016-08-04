#ifndef __HPARTICLEBTCLUSTERF_H__
#define __HPARTICLEBTCLUSTERF_H__

#include "TObject.h"
//#include "hreconstructor.h"

class HParticleBtPar;
class HParticleBtRingF;
class TF2;
class HVertex;
class HParticleBtRingInfo;
using namespace std;


class HParticleBtClusterF : public TObject {



protected:

    HParticleBtPar* fBtPar;
    HParticleBtRingF* fRing;

    Int_t fPadUp;
    Int_t fPadUpRight;
    Int_t fPadRight;
    Int_t fPadDownRight;
    Int_t fPadDown;
    Int_t fPadDownLeft;
    Int_t fPadLeft;
    Int_t fPadUpLeft;

    Float_t fChargeLimit[6];
    Float_t fChargeLimitMaximum[6];
    Float_t fPadWidthX;
    Float_t fPadWidthY;
  
    Float_t fNSigma;
    Float_t fMaxSigmaRange;
    Float_t fMaxSigmaRangeSmall;
    Float_t fSigmaErrorPad;
   
    UInt_t  fNMaxLimit;
    UInt_t  fClusSizeLimit;
    Float_t fMinimumSigmaValue;

    Int_t    fMaximumType;
    Float_t  fSigmaGaus[2]; //x,y
    Float_t  fSigmaGausRange[2];//x,y

    Float_t  fChargeDiagonalPar[3];  //3 Params per function
    Float_t  fChargeVerticalPar[3];
    Float_t  fChargeHorizontalPar[3];

    Float_t  fPhiOff[6];
    Float_t  fPhiOff2[6];
    Int_t    fPhiOffsetPar;

    Int_t    fNRingSeg;
    Int_t    fNRingSegStep;
    Int_t    fNRingSegOffset;

    Int_t    fNVertex;
    Int_t    fNParMean;
    Int_t    fNParSigma;
    Float_t  fParThetaAngleMin;
    Float_t  fParThetaAngleMax;

    vector <vector <TF2*> >   fPol2DMean;
    vector <vector <TF2*> >   fPol2DSigma;

    vector <Float_t> fRad2Deg;
    vector <Float_t> fRad2DegX;
    vector <Float_t> fRad2DegY;


    //Pads in clusters
    vector<Int_t> fIsInCluster;
    //Cluster track relation
    vector<vector <Int_t> >  fClusTrackNoShared;

    //Cluster information
    vector<Bool_t>           fClusIsGood;
    vector<Int_t>            fClusPadSum;
    vector<Int_t>            fClusPadRing;
    vector<Float_t>          fClusChargeSum;
    vector<Float_t>          fClusChargeRing;
    vector<Float_t>          fClusPosX;
    vector<Float_t>          fClusPosY;
    vector<Int_t>            fClusClass;
  
    //Maxima and maxima position
    vector<vector <Int_t> >              fClusNMaxima;
    vector<vector <vector  <Int_t> > >   fClusNMaximaPad;
    vector<vector <vector  <Float_t> > > fClusNMaximaPosX;
    vector<vector <vector  <Float_t> > > fClusNMaximaPosY;

    //Chi2
    vector<vector <vector  <Float_t> > > fClusChi2;
    vector<vector <vector  <Float_t> > > fClusChi2XMM;
    vector<vector <vector  <Float_t> > > fClusChi2YMM;

    //Debug info
    vector<vector <vector  <Float_t> > > fClusCircleX;
    vector<vector <vector  <Float_t> > > fClusCircleY;
    vector<vector <vector  <Float_t> > > fClusSig1X;
    vector<vector <vector  <Float_t> > > fClusSig1Y;
    vector<vector <vector  <Float_t> > > fClusSig2X;
    vector<vector <vector  <Float_t> > > fClusSig2Y;

    //RingInfo
    vector < vector<Int_t> >* fPrediction;
    vector <Int_t>          * fRichHitAdd;
    vector <Float_t>        * fRichHitCharge;
    vector <Float_t>         fRingMeanDistX;
    vector <Float_t>         fRingMeanDistY;



public:

    HParticleBtClusterF(void);

    ~HParticleBtClusterF(void);

    // -----------------------------------------------------

    Bool_t init(void);

    // -----------------------------------------------------

    void setRingF(HParticleBtRingF* ring);

    void    calcCluster(Int_t trackNo);
    void    noHitFound();
    void    clearCluster(void);

    Int_t   getPadsRing(  const Int_t trackNo);
    Int_t   getPadsSum(   const Int_t trackNo);
    Float_t getChargeRing(const Int_t trackNo);
    Float_t getChargeSum( const Int_t trackNo);

    Int_t   getNClusters(   const Int_t trackNo);
    Int_t   getMaxima(      const Int_t trackNo);
    Float_t getMaximaCharge(const Int_t trackNo);
    Int_t   getNearbyMaxima(const Int_t trackNo);

    Float_t getChi2Value(       const Int_t trackNo);
    Float_t getChi2ValuePrimary(const Int_t trackNo);

    Int_t   getMaximaShared(           const Int_t trackNo);
    Int_t   getMaximaSharedBad(        const Int_t trackNo);
    Int_t   getMaximaSharedTrack(      const Int_t trackNo, const Int_t trackNo2);
    Int_t   getMaximaSharedBadTrack(      const Int_t trackNo, const Int_t trackNo2);
    Float_t getMaximaChargeShared(     const Int_t trackNo);
    Float_t getMaximaChargeSharedTrack(const Int_t trackNo, const Int_t trackNo2);
    Float_t getMaximaChargeSharedBad(     const Int_t trackNo);
    Float_t getMaximaChargeSharedBadTrack(const Int_t trackNo, const Int_t trackNo2);
    Int_t   getNearbyMaximaShared(     const Int_t trackNo);
    Int_t   getNearbyMaximaSharedTrack(const Int_t trackNo, const Int_t trackNo2);
  
    Float_t getMeanDistX(const Int_t trackNo);
    Float_t getMeanDistY(const Int_t trackNo);
    Float_t getMeanDist( const Int_t trackNo);


    Bool_t fillRingInfo(HParticleBtRingInfo* BtRingInfo);
    //Double_t gaussf(Double_t *x, Double_t *par);


private:

    // -----------------------------------------------------
    //Helper functions
    void     addressToColRow(const Int_t address, Int_t &sec, Int_t &row, Int_t &col);
    Float_t  getPadCharge(const Int_t pos);
    Int_t    getShared(const Int_t trackNo,const Int_t clusNum, const Float_t sigmaRange, const Bool_t trackType, const Int_t trackNo2);
    Float_t  getSharedCharge(const Int_t trackNo,const Int_t clusNum, const Float_t sigmaRange, const Bool_t trackType, const Int_t trackNo2);
    void     addressIsShared(Int_t address);
    Float_t  sharedChargeDiagonal(const Float_t maximumCharge);
    Float_t  sharedChargeVertical(const Float_t maximumCharge);
    Float_t  sharedChargeHorizontal(const Float_t maximumCharge);


    //Cluster info calculation functions
    void findNeighbour( const Int_t trackNo, const Int_t address);
    void fillCluster(const Int_t trackNo, const UInt_t beginclus);
    void nMaxima(const Int_t trackNo, const  UInt_t beginclus);
    void fitMaximaPos(const UInt_t beginclus);
    void calcChi2(const Int_t trackNo);
    void setGoodClus(void);
    void findClusterShape(const UInt_t beginclus);
    void calcRingShift(const Int_t trackNo);
   
    ClassDef(HParticleBtClusterF, 0)

};


#endif  // __HPARTICLEBTCLUSTER_H__

