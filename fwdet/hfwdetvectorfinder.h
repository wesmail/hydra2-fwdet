/** HFwDetVectorFinder.h
 *@author A.Zinchenko <Alexander.Zinchenko@jinr.ru>
 *@since 2016
 **
 ** Task class for vector finding in FwDet.
 ** Input: HFwDetStrawCalSim hits
 ** Output: HVectorCand objects
 **
 **/

#ifndef HFWDETVECTORFINDER_H
#define HFWDETVECTORFINDER_H

#include "hreconstructor.h"
#include "fwdetdef.h"

#include "TDecompLU.h"

#include <map>
#include <set>
#include <vector>

class HCategory;
class HFwDetStrawGeomPar;
class HFwDetVectorFinderPar;
class HVectorCand;

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

private:
    HCategory* pHits;                       // Input array of hits
    HCategory* pTrackArray;                 // Output array of vectors
    HFwDetStrawGeomPar* pStrawGeomPar;
    HFwDetVectorFinderPar * pStrawVFPar;

    Bool_t isSimulation;                    // flag to mark simulation run

    typedef std::pair<Int_t, Int_t> HitPair;
    typedef std::pair<Int_t, Int_t> DoubletPair;

    Int_t fNpass;                               // Number of reco. passes
    Int_t fIndx0[FWDET_STRAW_MAX_MODULES];      // start indices of vectors for different passes

    // quick access variables
    Int_t nModules;
    Int_t nLayers[FWDET_STRAW_MAX_MODULES];

    // helper to identify fields in the matrices
    enum VecFields { HITNR, TUBENR, VF_SIZE };
    std::multimap<Int_t,Int_t> fHitPl[FWDET_STRAW_MAX_MODULES][FWDET_STRAW_MAX_LAYERS * FWDET_STRAW_MAX_PLANES]; //! hit indices on planes vs tube No
    std::vector<HVectorCand*> fVectors[FWDET_STRAW_MAX_MODULES+1]; //! track vectors for stations
    std::vector<HVectorCand*> fVectorsHigh[FWDET_STRAW_MAX_MODULES]; //! track vectors for stations (high resolution)

    Double_t fUz[FWDET_STRAW_MAX_VPLANES];   // hit float data
    Double_t fUzi[FWDET_STRAW_MAX_VPLANES][VF_SIZE];  // hit int data
    Double_t fDz[FWDET_STRAW_MAX_VPLANES];      // Z-distances from layer 0
    Double_t fCosa[FWDET_STRAW_MAX_VPLANES];    // cos of stereo angles
    Double_t fSina[FWDET_STRAW_MAX_VPLANES];    // sin of stereo angles
    Double_t fDrift[FWDET_STRAW_MAX_VPLANES];   // drift time
    std::map<Int_t,TDecompLU*> fLus;            // system matrices
    Float_t fLRErrU;                   // hit meas. error for LR
    Float_t fHRErrU;                   // hit meas. error for HR
    Float_t fErrU;                     // hit meas. error
    Float_t fLRCutChi2;                // Chi2-cut
    Float_t fHRCutChi2;                // Chi2-cut
    Float_t fTubesD;                   // distances between tubes
    Int_t fMinHits;                     // Min. number of hits on track to do fit
    Double_t fZ0[FWDET_STRAW_MAX_MODULES];  // Z-positions of the first layers
    Double_t fDtubes[FWDET_STRAW_MAX_MODULES][FWDET_STRAW_MAX_LAYERS * FWDET_STRAW_MAX_PLANES];   // max. tube difference between views
    std::map<Int_t,TMatrixDSym*> fMatr;     // system matrices (for different hit layer patterns)

    std::vector<DoubletPair> fHit[FWDET_STRAW_MAX_MODULES][FWDET_STRAW_MAX_LAYERS];     //! Indx1,Indx2 of doublet hits

    Bool_t hasBest;         // does track has best chi2 value
    Double_t bestChi2;      // store best chi2
    Double_t bestPars[4];   // store best params for given best chi2

    void computeMatrix();
    void getHits();
    void makeVectors();
    void processDouble(Int_t m, Int_t l, Int_t patt);
    void addVector(Int_t ista, Int_t patt, Double_t chi2, Double_t *pars, Bool_t lowRes = kTRUE);
    void setTrackId(HVectorCand *vec);
    void findLine(Int_t patt, Double_t *pars);
    Double_t findChi2(Int_t patt, Double_t *pars);
    void checkParams();
    void highRes();
    void processSingleHigh(Int_t ista, Int_t plane, Int_t plane_limit, Int_t patt, Int_t flag, Int_t nok, Double_t uu[FWDET_STRAW_MAX_VPLANES][2]);
    void moveVectors();
    void storeVectors(Int_t sel);
    void mergeVectors();
    void selectTracks(Int_t ipass);
    Double_t refit(Int_t patt, Int_t *hinds, Double_t *pars, TMatrixDSym *cov, Int_t *lr);
    void addTrack(Int_t ista0, HVectorCand *tr1, HVectorCand *tr2,
            Int_t indx1, Int_t indx2, Double_t *parOk, Double_t c2, TMatrixDSym &w2);

    ClassDef(HFwDetVectorFinder,0);
};

#endif
