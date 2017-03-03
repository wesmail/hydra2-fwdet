/** HFwDetStrawVectorFinder.h
 *@author A.Zinchenko <Alexander.Zinchenko@jinr.ru>
 *@since 2016
 **
 ** Task class for vector finding in FwDet.
 ** Input: HFwDetStrawCalSim hits
 ** Output: HFwDetStrawVector objects
 **
 **/

#ifndef HFWDETSTRAWVECTORFINDER_H
#define HFWDETSTRAWVECTORFINDER_H

#include "hlocation.h"
#include "hreconstructor.h"

#include "fwdetdef.h"

#include "TDecompLU.h"

#include <map>
#include <set>
#include <vector>

class HCategory;
class HFwDetStrawGeomPar;
class HFwDetStrawCalSim;
class HFwDetStrawDigiPar;
class HFwDetStrawVector;
class HFwDetStrawVectorFinderPar;

class HFwDetStrawVectorFinder : public HReconstructor
{
public:
    /** Default constructor **/
    HFwDetStrawVectorFinder();

    /** Constructor **/
    HFwDetStrawVectorFinder(const Text_t *name, const Text_t *title);

    /** Destructor **/
    virtual ~HFwDetStrawVectorFinder();

    /** Initialisation **/
    virtual Bool_t init();

    /** Initialisation **/
    void initVariables();

    /** Initialisation **/
    virtual Bool_t reinit();

    /** Task execution **/
    virtual Int_t execute();

    /** Finish at the event end **/
    virtual Bool_t finalize();

private:
    HCategory* pKine;                           // categor for GeantKine (test sim/real data)
    HCategory* pHits;                           // Input array of hits
    HCategory* pTrackArray;                     // Output array of vectors
    HFwDetStrawGeomPar* pStrawGeomPar;
    HFwDetStrawVectorFinderPar * pStrawVFPar;

    Bool_t isSimulation;                        // flag to mark simulation run

    typedef std::pair<Int_t, Int_t> HitPair;
    typedef std::pair<Int_t, Int_t> DoubletPair;

    Int_t fNpass;                               // Number of reco. passes
    Int_t fIndx0[FWDET_STRAW_MAX_MODULES];      // start indices of vectors for different passes

    // quick access variables
    Int_t nModules;
    Int_t nLayers[FWDET_STRAW_MAX_MODULES];
    Int_t nVplanes;
    Int_t nVplanesH;

    // helper to identify fields in the matrices
    enum VecFields { HITNR, TUBENR, VF_SIZE };
    std::multimap<Int_t,Int_t> fHitPl[FWDET_STRAW_MAX_MODULES][FWDET_STRAW_MAX_LAYERS * FWDET_STRAW_MAX_PLANES]; //! hit indices on planes vs tube No
    std::vector<HFwDetStrawVector*> fVectors[FWDET_STRAW_MAX_MODULES+1]; //! track vectors for stations
    std::vector<HFwDetStrawVector*> fVectorsHigh[FWDET_STRAW_MAX_MODULES]; //! track vectors for stations (high resolution)
    Double_t fUz[FWDET_STRAW_MAX_VPLANES/2];   // hit float data
    Double_t fUzi[FWDET_STRAW_MAX_VPLANES/2][VF_SIZE];  // hit int data
    Double_t fDz[FWDET_STRAW_MAX_VPLANES];      // Z-distances from layer 0
    Double_t fCosa[FWDET_STRAW_MAX_VPLANES];    // cos of stereo angles
    Double_t fSina[FWDET_STRAW_MAX_VPLANES];    // sin of stereo angles
    std::map<Int_t,TDecompLU*> fLus;            //! system matrices (for different hit layer patterns)
    Double_t fLRErrU;                   // hit meas. error for LR
    Double_t fHRErrU;                   // hit meas. error for HR
    Double_t fErrU;                     // hit meas. error
    Double_t fLRCutChi2;                // Chi2-cut
    Double_t fHRCutChi2;                // Chi2-cut
    Int_t fMinHits;                     // Min. number of hits on track to do fit
    Double_t fZ0[FWDET_STRAW_MAX_MODULES];  // Z-positions of the first layers
    Double_t fDtubes[FWDET_STRAW_MAX_MODULES][FWDET_STRAW_MAX_LAYERS * FWDET_STRAW_MAX_PLANES];   // max. tube difference between views
    std::map<Int_t,TMatrixDSym*> fMatr;           //! system matrices (for different hit layer patterns)

    std::vector<DoubletPair> fHit[FWDET_STRAW_MAX_MODULES][FWDET_STRAW_MAX_LAYERS];     //! Indx1,Indx2 of doublet hits

    void computeMatrix();
    void getHits();
    void makeVectors();
    void processDouble(Int_t m, Int_t l, Int_t patt);
    void addVector(Int_t ista, Int_t patt, Double_t chi2, Double_t *pars, Bool_t lowRes = kTRUE);
    void setTrackId(HFwDetStrawVector *vec);
    void findLine(Int_t patt, Double_t *pars);
    Double_t findChi2(Int_t ista, Int_t patt, Double_t *pars);
    void checkParams();
    void highRes();
    void processSingleHigh(Int_t ista, Int_t plane, Int_t patt, Int_t flag, Int_t nok, Double_t uu[FWDET_STRAW_MAX_VPLANES/2][FWDET_STRAW_MAX_PLANES]);
    void moveVectors();
    void storeVectors(Int_t sel);
    void mergeVectors();
    void selectTracks(Int_t ipass);
    Double_t refit(Int_t patt, Int_t *hinds, Double_t *pars, TMatrixDSym *cov, Int_t *lr);
    void addTrack(Int_t ista0, HFwDetStrawVector *tr1, HFwDetStrawVector *tr2,
            Int_t indx1, Int_t indx2, Double_t *parOk, Double_t c2, TMatrixDSym &w2);

    ClassDef(HFwDetStrawVectorFinder,0);
};

#endif
