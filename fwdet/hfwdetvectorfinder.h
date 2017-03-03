#ifndef HFWDETVECTORFINDER_H
#define HFWDETVECTORFINDER_H

#include "hreconstructor.h"

#include "TDecompLU.h"

#include <map>
#include <set>
#include <vector>

class HCategory;
class HFwDetStrawGeomPar;
class HFwDetStrawDigiPar;
class HFwDetVectorFinderPar;
class HVectorCand;

struct ComboSet {
    Int_t pattern;
    Double_t chi2;                          // store chi2
    Double_t pars[4];                       // store params for given best chi2
    Double_t z[FWDET_STRAW_MAX_VPLANES];   // store LR combination

    void print() const {
        printf("COMBO: chi2=%f  patt=%x\n     LR=", chi2, pattern);
        for (Int_t i = 0; i < FWDET_STRAW_MAX_VPLANES; ++i)
            printf("%f,", z[i]);
        printf("\n");
    }

    bool operator<(const ComboSet & cs) const { return chi2 < cs.chi2; }
};

class HFwDetVectorFinder : public HReconstructor
{
public:
    HFwDetVectorFinder();
    HFwDetVectorFinder(const Text_t *name, const Text_t *title);
    virtual ~HFwDetVectorFinder();

    void initVariables();

    virtual Bool_t init();
    virtual Bool_t reinit();
    virtual Int_t execute();
    virtual Bool_t finalize();

    void clear();

private:
    HCategory* pStrawHits;                  // Input array of straw hits
    HCategory* pRpcHits;                    // Input array of rpc hits
    HCategory* pVectorCand;                 // Output array of vectors
    HFwDetStrawGeomPar* pStrawGeomPar;      // straw geometry parameters
    HFwDetStrawDigiPar* pStrawDigiPar;      // straw digitizer parameters
    HFwDetVectorFinderPar * pStrawVFPar;    // vector finder parameters

    Bool_t isSimulation;                    // flag to mark simulation run

    typedef std::pair<Int_t, Int_t> HitPair;
    typedef std::pair<Int_t, Int_t> DoubletPair;

    Int_t fNpass;                           // Number of reco. passes
    Int_t fIndx0[FWDET_STRAW_MAX_MODULES];  // start indices of vectors for different passes

    // quick access variables
    Int_t nModules;
    Int_t nLayers[FWDET_STRAW_MAX_MODULES];

    std::multimap<Int_t,Int_t> fHitPl[FWDET_STRAW_MAX_MODULES][FWDET_STRAW_MAX_LAYERS * FWDET_STRAW_MAX_PLANES]; //! hit indices on planes vs tube No
    std::vector<HVectorCand*> fVectors[FWDET_STRAW_MAX_MODULES+1]; //! track vectors for stations
    std::vector<HVectorCand*> fVectorsHigh[FWDET_STRAW_MAX_MODULES]; //! track vectors for stations (high resolution)

    // vector finder helper vectors
    Double_t fUz[FWDET_STRAW_MAX_VPLANES];      // hit position
    Double_t fUzHit[FWDET_STRAW_MAX_VPLANES];   // hit no.
    Double_t fUzTube[FWDET_STRAW_MAX_VPLANES];  // tube no.
    Double_t fDz[FWDET_STRAW_MAX_VPLANES];      // Z-dist. from the first layer
    Double_t fCosa[FWDET_STRAW_MAX_VPLANES];    // cos of the stereo angle
    Double_t fSina[FWDET_STRAW_MAX_VPLANES];    // sin of the stereo angle
    Double_t fDrift[FWDET_STRAW_MAX_VPLANES];   // drift time
    Double_t fZ0[FWDET_STRAW_MAX_MODULES];      // Z-coord. of the first layers

    // cut variables
    Float_t fLRErrU;        // hit meas. error for LR
    Float_t fHRErrU;        // hit meas. error for HR
    Float_t fErrU;          // hit meas. error
    Float_t fLRCutChi2;     // Chi2-cut
    Float_t fHRCutChi2;     // Chi2-cut
    Float_t fTubesD;        // distances between tubes
    Float_t fCutX;
    Float_t fCutY;
    Int_t fMinHits;         // Min. number of hits on track to do fit

    // system matrices
    std::map<Int_t,TDecompLU*> fLus;
    std::map<Int_t,TMatrixDSym*> fMatr;

    // keeps pair of hit indexes in a double-layer
    std::vector<DoubletPair> fHit[FWDET_STRAW_MAX_MODULES][FWDET_STRAW_MAX_LAYERS];

    HVectorCand * current_track;

    Int_t nMaxBest;
    std::vector<ComboSet> combos;   // keeps combinations for best selection

    Float_t dt_p[5];        // drift radius parameters

    void computeMatrix();
    void getHits();
    void makeVectors();
    void processDouble(Int_t m, Int_t l, Int_t patt);
    HVectorCand * addVector(Int_t ista, Int_t patt, Double_t chi2, Double_t *pars, Bool_t lowRes = kTRUE);
    void setTrackId(HVectorCand *vec);
    void findLine(Int_t patt, Double_t *pars);
    Double_t findChi2(Int_t patt, Double_t *pars);
    void checkParams();
    void highRes();
    void processSingleHigh(Int_t ista, Int_t plane, Int_t plane_limit, Int_t patt, Int_t flag, Int_t nok, Double_t uu[FWDET_STRAW_MAX_VPLANES][3]);
    void moveVectors();
    void storeVectors(Int_t sel);
    void mergeVectors();
    void selectTracks(Int_t ipass);
    Double_t refit(Int_t patt, Int_t *hinds, Double_t *pars, TMatrixDSym *cov, Int_t *lr, Double_t tof, Double_t tofl);
    void addTrack(Int_t ista0, HVectorCand *tr1, HVectorCand *tr2,
            Int_t indx1, Int_t indx2, Double_t *parOk, Double_t c2, TMatrixDSym &w2);

    Int_t matchRpcHit(Double_t * params, Double_t z);
    Float_t calcDriftRadius(Float_t t) const;

    ClassDef(HFwDetVectorFinder,0);
};

#endif
