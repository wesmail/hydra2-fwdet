#ifndef __HPARTICLETOOL_H__
#define __HPARTICLETOOL_H__

#include "TObject.h"
#include "TObjArray.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TF1.h"
#include "TCutG.h"


#include <vector>


using namespace std;

class HRichHit;
class HTofHit;
class HTofCluster;
class HRpcCluster;
class HShowerHit;
class HEmcCluster;
class HMetaMatch2;
class HMdcTrkCand;
class HMdcSeg;
class HMdcHit;
class HMdcCal1;
class HMdcClusInf;
class HMdcClusFit;
class HMdcWireFit;
class HMdcClus;
class HGeomVector;
class HParticleCand;
class HParticleCandSim;
class HParticlePair;
class HMdcPlane;
class HMdcLayer;
class HTofWalkPar;

class HGeantKine;

#define TOFSIGSIM 33
#define RPCSIG1 7.62
#define RPCSIG2 5.96
#define RPCSIG1SIM 8.32
#define RPCSIG2SIM 5.96

class HParticleTool : public TObject {

    static Float_t rpcCellHalfWidth[192];   //! half with of rpc cell for matching [col*32+cell]
    static Float_t tofCellHalfWidth[64] ;   //! half with of tof cell for matching [mod*8+cell]
    static Double_t scaleDyPars[4];         //! params for TF1 gScaledy
    static TF1* gf1;                        // helper functions for genereric intersection of tf1
    static TF1* gf2;                        // helper functions for genereric intersection of tf1
    static TF1* gfsum;                      // helper functions for genereric intersection of tf1
    static TF1* gScaledy;                   // scaling function (1/p) for dy matching boundary cut
    static  TF1* fdxoffset;                 // tof dx offset function
    static  TF1* fdxsigma ;                 // tof dx sigma function
    static Double_t parsSX[6][8][8][3];     // tof sigma dx normalization parameters
    static Double_t parsDX[6][8][8][5];     // tof offset dx normalization parameters

    static Double_t parsSX_apr12[6][8][8][3];     // tof sigma dx normalization parameters
    static Double_t parsDX_apr12[6][8][8][5];     // tof offset dx normalization parameters
    static TString  beamtime_tof;
public:

    HParticleTool();
    ~HParticleTool();

    static  Float_t    phiSecToLabDeg(Int_t sec, Float_t phiRad);
    static  Float_t    thetaToLabDeg(Float_t thetaRad);
    static  Float_t    phiLabToPhiSecDeg(Float_t phiLabDeg);
    static  Int_t      phiLabToSec(Float_t phiLabDeg);
    static  Float_t    getOpeningAngle(Float_t phi1,Float_t theta1,Float_t phi2,Float_t theta2);
    static  Float_t    getOpeningAngle(TLorentzVector& vec1,TLorentzVector& vec2);
    static  Float_t    getOpeningAngle(HParticleCand* cand1,HParticleCand* cand2);
    static  Float_t    getOpeningAngle(HGeantKine* kine1,HGeantKine* kine2);
    static  Bool_t     setCloseCandidates(Float_t oACut=15., Bool_t sameSector=kTRUE, Bool_t skipSameSeg=kTRUE);
    static  Int_t      getCloseCandidates(HParticleCand* cand,vector<HParticleCand*>& vcand,vector<Float_t >& vopeninAngle,Float_t oACut=15., Bool_t sameSector=kTRUE, Bool_t skipSameSeg=kTRUE);
    static  Int_t      getCloseCandidatesSegInd(HParticleCand* cand, vector<Int_t>& vSeg,Float_t oACut,Bool_t sameSector, Bool_t skipSameSeg);
    static  void       getTLorentzVector(HGeantKine* kine, TLorentzVector& vec,Int_t pid=-1);
    static  void       fillTLorentzVector(TLorentzVector& v,HParticleCand* cand,Float_t mass);
    static  void       fillTLorentzVector(TLorentzVector& v,HParticleCand* cand,Int_t pid,Bool_t correctMom=kTRUE);
    static  Float_t    getLabPhiDeg(TLorentzVector& vec);
    static  Float_t    calcRichQA(HMdcSeg* seg, HRichHit* hit);
    static  Float_t    calcRichQA(HMdcSeg* seg, Float_t richTheta,Float_t richPhi);
    static HGeomVector getGlobalVertex(Int_t v,Bool_t warn=kFALSE);
    static Double_t    getMinimumDistToVertex(HParticleCand*,HGeomVector& vertex);
    static HGeomVector getPointOfClosestApproachToVertex(HParticleCand*,HGeomVector& vertex);

    
    //--------------------------------------------------
    //  functions for metamatch stuff
    static Double_t scaledy    (Double_t* x, Double_t* par);
    static Double_t getScaledDy(HParticleCand* c,Double_t dyCut=-1);
    static TF1*     getScaleTF1()  { return gScaledy ;}
    static Bool_t   normDX(HParticleCand* c,TString beamtime="apr12");
    static Float_t  getNormDX(HParticleCand* c,TString beamtime="apr12");
    static Float_t  getSigmaDX(HParticleCand* c,TString beamtime="apr12");
    static Bool_t   normDX(HParticleCand* c,HTofWalkPar* p);
    static Float_t  getNormDX(HParticleCand* c,HTofWalkPar* p);
    static Float_t  getSigmaDX(HParticleCand* c,HTofWalkPar* p);
    static TF1*     getTofXOffsetTF1() {return fdxoffset;}
    static TF1*     getTofXSigmaTF1()  {return fdxsigma ;}
    static Float_t  getRpcCellHalfWidth  (Int_t mod,Int_t cell);
    static Float_t  getTofCellHalfWidth  (Int_t mod,Int_t cell);
    static Bool_t   isGoodMetaCell       (HParticleCand* c,Double_t bound=3.5,Bool_t doScaling=kTRUE);
    //--------------------------------------------------




    //--------------------------------------------------
    //  functions for corrections
    static Float_t getCorrectedMomentum(HParticleCand* c);
    static Float_t setCorrectedMomentum(HParticleCand* c);
    static Bool_t  isParticledEdx(Int_t PID, HParticleCand* c, Float_t& deloss, Float_t& dsigma);
    static Bool_t  isParticleBeta(Int_t PID, HParticleCand* pCand, Float_t nsigma, Float_t momMin, Float_t momMax,Float_t& dtime, Float_t& dsigma,TString beamtime="apr12");
    static Bool_t  correctPathLength(HParticleCand* pCand, HGeomVector& vertex,const HMdcPlane* planes, const HGeomVector& targetMidPoint, Double_t beamEnergy = 1230);
    static Bool_t  checkCropedLayer(HGeantKine* kine,HMdcLayer* mdcLayer, Bool_t* croped=0,Bool_t checkHit=kTRUE);
    //--------------------------------------------------




    //--------------------------------------------------
    // kinematic helper functions
    static Double_t beta(Int_t id,Double_t p);
    static Double_t betaToP(Int_t id,Double_t beta);
    static Double_t gamma(Int_t id,Double_t p);
    static Double_t gammaToBeta(Int_t id,Double_t gamma);
    static Double_t gammaToP(Int_t id,Double_t gamma);
    static Double_t betagamma(Int_t id,Double_t p);
    static Double_t betagammaToP(Int_t id,Double_t betagamma);
    static Double_t kinEToMom(Int_t id = 14, Double_t Ekin = 3500);
    static Double_t momToKinE(Int_t id = 14, Double_t p = 3500);
    static Double_t dedxfunc(Double_t *x, Double_t *par);
    static Double_t betaandgammafunc(Double_t *x, Double_t *par);
    static Double_t ptyfunc (Double_t* x, Double_t* par);
    static Double_t fcross(Double_t* xin, Double_t* par);
    static Bool_t   getIntersectionPoint(TF1* f1,TF1* f2,Double_t &xout,Double_t& yout,Double_t xlow,Double_t xup,Int_t n=500);
    static TF1*     energyLossTF1(Int_t id,TString name="",TString opt="p",Double_t scaleY=1,Double_t xoffset=0,Double_t scaleX=1,Double_t theta=-1,Double_t frac=0.1,Double_t xmin=20,Double_t xmax=2000,Int_t linecolor = 2,Int_t linestyle = 1,Int_t npoints=500);
    static TF1*     betaAndGammaTF1(Int_t id,TString name="",TString opt="beta",Double_t scaleY=1,Double_t xoffset=0,Double_t scaleX=1,Double_t theta=-1,Double_t frac=0.1,Double_t xmin=20,Double_t xmax=2000,Int_t linecolor = 2,Int_t linestyle = 1,Int_t npoints=500);
    static TCutG*   makeCut(TF1* lower,TF1* upper,TString name,Double_t xlow,Double_t xup,Double_t ymin=-1000,Double_t ymax=1000,Int_t npoint=500,Int_t linecolor = 2,Int_t linestyle = 2);
    static TF1*     ptyTF1(Int_t id,Double_t val=45,TString name="",TString opt="theta",Double_t xmin=0,Double_t xmax=2,Double_t midRap=0,Int_t linecolor = 2,Int_t linestyle = 1);
    static vector<TF1*> ptyGrid(Int_t id,vector<Double_t>& vtheta,vector<Double_t>& vmom,TString name="",TString opt = "draw",Double_t xmin=0,Double_t xmax=2,Double_t midRap=0,Int_t linecolorTheta = 1,Int_t linestyleTheta = 2,Int_t linecolorMom = 1,Int_t linestyleMom = 2);
    static vector<TF1*> ptyGrid(Int_t id,TString setup="@theta:8,0,10@momentum:20,10,100",TString name="",TString opt = "draw",
				Double_t xmin=0,Double_t xmax=2,Double_t midRap=0,
				Int_t linecolorTheta=1,Int_t linestyleTheta=2,Int_t linecolorMom=1,Int_t linestyleMom=2,
				TString labels="@theta:draw=yes,format=%5.1f#circ,textsize=0.021,angle=0,align=-1@momentum:draw=yes,format=%5.1f MeV/c,textsize=0.023,angle=10,align=-1");
    static void drawPtyGrid(vector<TF1*>& grid,TString opt = "draw");
    //--------------------------------------------------



    //--------------------------------------------------
    // functions from GeomFunct.h (A.Schmah)
    static void        calcSegVector(Double_t z, Double_t rho, Double_t phi, Double_t theta, HGeomVector &base, HGeomVector &dir);
    static Double_t    calcRMS(const Double_t* valArr, Double_t Mean,Int_t valNum);
    static Double_t    calcDeterminant(HGeomVector& v1, HGeomVector& v2, HGeomVector& v3);
    static Double_t    calculateMinimumDistanceStraightToPoint(HGeomVector &base, HGeomVector &dir,HGeomVector &point);
    static HGeomVector calculatePointOfClosestApproachStraightToPoint(HGeomVector &base, HGeomVector &dir,HGeomVector &point);
    static Double_t    calculateMinimumDistance(HGeomVector &base1, HGeomVector &dir1, HGeomVector &base2, HGeomVector &dir2);
    static HGeomVector calculatePointOfClosestApproach(HGeomVector &base1, HGeomVector &dir1,HGeomVector &base2, HGeomVector &dir2);
    static HGeomVector calculateCrossPoint(HGeomVector &base1, HGeomVector &dir1,HGeomVector &base2, HGeomVector &dir2);
    static HGeomVector calcVertexAnalytical(HGeomVector &base1, HGeomVector &dir1,HGeomVector &base2, HGeomVector &dir2);
    //--------------------------------------------------




    //--------------------------------------------------
    // geant helper functions
    static  Int_t    findFirstHitInTof         (Int_t trackID,Int_t modeTrack = 2);
    static  Int_t    findFirstHitShowerInTofino(Int_t trackID,Int_t modeTrack = 2);
    static  Int_t    findFirstHitShowerInRpc   (Int_t trackID,Int_t modeTrack = 2);
    //--------------------------------------------------

    static  Float_t  getInterpolatedValue(TH1* h, Float_t xVal,Bool_t warn = kTRUE);
    static  Stat_t   getValue(TH1* h,Float_t xVal, Float_t yVal = 0.0f, Float_t zVal = 0.0f);


    //--------------------------------------------------
    // functions to retrive hit objects from candidate
    static HRichHit*    getRichHit   (Int_t richind);
    static HTofHit*     getTofHit    (Int_t tofind);
    static HTofCluster* getTofCluster(Int_t tofind);
    static HRpcCluster* getRpcCluster(Int_t rpcind);
    static HShowerHit*  getShowerHit (Int_t showerind);
    static HEmcCluster* getEmcCluster(Int_t emcind);

    static HMetaMatch2* getMetaMatch (Int_t metaind);
    static HMdcTrkCand* getMdcTrkCand(Int_t metaind);
    static HMdcSeg*     getMdcSeg    (Int_t segind);
    static HMdcHit*     getMdcHit    (Int_t segind,Int_t nhit = 0);
    static HMdcClusInf* getMdcClusInf(Int_t segind,Int_t nhit = 0);
    static HMdcClusFit* getMdcClusFit(Int_t segind);
    static HMdcClus*    getMdcClus   (Int_t segind);
    static TObjArray*   getMdcWireFitSeg (Int_t segind);
    static TObjArray*   getMdcCal1Seg    (Int_t segind);
    static TObjArray*   getMdcCal1Cluster(Int_t segind);
    static Int_t        getMdcWireFitSeg (Int_t segind, vector<HMdcWireFit*>& v, Bool_t clear=kTRUE);
    static Int_t        getMdcCal1Seg    (Int_t segind, vector<HMdcCal1*>& v, Bool_t clear=kTRUE);
    static Int_t        getMdcCal1Cluster(Int_t segind, vector<HMdcCal1*>& v, Bool_t clear=kTRUE);

    static Bool_t       printSimTracks(HParticleCandSim* c);
    static Bool_t       getSimTracks  (HParticleCandSim* c,
				       vector<Int_t>&tracksMeta,
				       vector<Int_t>&tracksShowerEcal,
				       vector<Int_t>&tracksRich,
				       vector<Int_t>&weightRich,
				       vector<Int_t>&tracksInnerMdc,
				       vector<Int_t>&weightInnerMdc,
				       vector<Int_t>&tracksOuterMdc,
				       vector<Int_t>&weightOuterMdc,
				       Bool_t print=kFALSE
				      );
    //--------------------------------------------------


    //--------------------------------------------------
    // paits evaluation functions
    static Bool_t       setPairFlags  (UInt_t& flag,HParticleCand* cand2=0,HParticleCand* cand1=0);
    static Bool_t       evalPairsFlags(UInt_t flag,UInt_t fl);
    static Bool_t       isPairsFlagsBit(UInt_t flag,UInt_t fl);
    static Bool_t       evalPairsFlags(UInt_t flag,HParticleCand* cand1,HParticleCand* cand2);
    static Bool_t       evalPairsFlags(UInt_t flag,HParticlePair& pair);
    static Bool_t       evalPairsFlags(vector<UInt_t>& flags,HParticlePair& pair);
    static Bool_t       evalPairsFlags(vector<UInt_t>& flags,vector<Bool_t>& results,HParticlePair& pair);
    //--------------------------------------------------

    //--------------------------------------------------
    // event checkers
    static Bool_t       isGoodClusterVertex(Float_t minZ);
    static Bool_t       isGoodRecoVertex   (Float_t minZ);
    static Bool_t       isGoodSTART        (Int_t minFlag);
    static Bool_t       isNoVETO           (Float_t minStart=-15.,Float_t maxStart=15.);
    static Bool_t       isNoSTART          (Float_t minStart=-15.,Float_t maxStart=15.,Float_t window=1.);
    static Bool_t       isGoodSTARTVETO    (Float_t minStart=15.,Float_t maxStart = 350., Float_t window=1.) ;
    static Bool_t       isGoodSTARTMETA    (Float_t minStart=80.,Float_t maxStart = 350.,Int_t tresh=4,Float_t window=5., Float_t offset=7.);
    static Bool_t       isNoSTARTPileUp    ();
    static Bool_t       isNoMETAPileUp     (Float_t ftimeTofCut,Int_t threshold);
    static Bool_t       isGoodTrigger      (Int_t triggerbit);
    static Int_t        getTofHitMult      (Float_t minTof=0,Float_t maxTof=35., Int_t* sector=NULL);
    static Int_t        getRpcHitMult      (Float_t minTof=0,Float_t maxTof=25., Int_t* sector=NULL);
    //--------------------------------------------------



    ClassDef(HParticleTool,0)
};

#endif //__HPARTICLETOOL_H__




