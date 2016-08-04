#ifndef __HPARTICLEBTRINGF_H__
#define __HPARTICLEBTRINGF_H__

#include "TObject.h"
#include "hparticledef.h"
#include "hparticlebtpar.h"
#include "hparticlebtringinfo.h"
#include "hparticleanglecor.h"

using namespace std;


class TF2;
class HCategory;
class HParticleCand;
class HEventHeader;
class HVertex;

class HParticleBtRingF : public TObject {

protected:

    HParticleBtPar* fBtPar;
    HParticleAngleCor fAngleCor;

    Int_t    fNSector;
    Int_t    fNVertex;
    Int_t    fNParMean;
    Int_t    fNParSigma;
    Int_t    fNRingSeg;
    Int_t    fNRingSegStep;
    Int_t    fNRingSegOffset;

    Int_t    fNRichSeg;
    Int_t    fRichSegBorderX;
    Int_t    fRichSegBorderY;
   
    Float_t  fThetaAngleMin;
    Float_t  fThetaAngleMax;
    Float_t  fParThetaAngleMin;
    Float_t  fParThetaAngleMax;
    Int_t    fSizeMatrix;
    Float_t  fChargeLimit[6];
    Float_t  fChargeLimitMaximum[6];

    Int_t    fNSigma;
    Int_t    fMaxSigmaRange;
    Float_t  fMinimumSigmaValue;
    Float_t  fSigmaErrorPad;

    Float_t  fVertexPosMin;
    Float_t  fVertexPosMax;
    Float_t  fVertexStep;

    Float_t  fPhiOff[6];
    Float_t  fPhiOff2[6];
    Int_t    fPhiOffsetPar;


    vector <Float_t> fRad2Deg;
    vector <Float_t> fRad2DegX;
    vector <Float_t> fRad2DegY;

    vector <vector <TF2*> >   fPol2DMean;
    vector <vector <TF2*> >   fPol2DSigma;

    vector < vector <Int_t> > fFiredPads;

    vector < vector<Int_t> > fPrediction;
    vector < vector<Int_t> > fRingMatrix;
    vector <Int_t>           fRichHitAdd;
    vector <Float_t>         fRichHitCharge;
    vector <Float_t>         fTrackTheta;
    vector <Float_t>         fTrackPhi;
    vector <Int_t>           fTrackVertex;
    vector <Int_t>           fTrackSec;
    vector <Int_t>           fTrackPCandIdx;
    vector <Bool_t>          fIsGoodTrack;
    vector <Float_t>         fPosXCenter;
    vector <Float_t>         fPosYCenter;

 
public:

    HParticleBtRingF(void);

    ~HParticleBtRingF(void);

  
    // -----------------------------------------------------

    Bool_t init(void);
    Bool_t finalize(void) { return kTRUE; }

    // -----------------------------------------------------

    void fillRichCal(HCategory* catRichCal);
    void fillPrediction(const HParticleCand* cand,HVertex &vertex, Bool_t isGoodTrack, const Bool_t doAngleCorr);
    void clearData(void);
   

    // -----------------------------------------------------

    Float_t                   getRingMatrix(const Int_t trackNo);
    vector < vector<Int_t> >& getPrediction(void) { return fPrediction;}
    vector <Int_t>&           getRichHitAdd(void) { return fRichHitAdd; }
    vector <Float_t>&         getRichHitCharge(void) { return fRichHitCharge; }
    Float_t                   getTrackTheta(Int_t trackNo);
    Float_t                   getTrackPhi(Int_t trackNo);
    Int_t     	              getTrackVertex(Int_t trackNo);
    Int_t     	              getTrackSec(Int_t trackNo);
    Float_t                   getPosXCenter(Int_t trackNo);
    Float_t                   getPosYCenter(Int_t trackNo);
    Bool_t                    isGoodTrack(Int_t trackNo);
    Bool_t                    fillRingInfo(HParticleBtRingInfo* btRingInfo);
    Bool_t                    hasNoisyRichSeg(Bool_t *trackInSec);
    Int_t                     plotPrediction(Int_t trackNo);
    void                      plotRichHit(Int_t trackNo);


private:
    void   addressToColRow(const Int_t address, Int_t &sec, Int_t &row, Int_t &col);
    Int_t  correctPhi(const Int_t sec,  const Float_t phi);
    Int_t  getVertexNum(const Float_t vertex);
    void   sortElements(Double_t &entry1 , Double_t &entry2);
    void   fillMatrix(Int_t xPad, Int_t yPad, Int_t sec);
    ClassDef(HParticleBtRingF, 0)
};

#endif  // __HPARTICLEBTRING_H__
