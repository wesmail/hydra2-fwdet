#ifndef  HPARTICLET0RECO_H
#define  HPARTICLET0RECO_H

#include "hreconstructor.h"
#include "hparticlepathlengthcorr.h"
#include "henergylosscorrpar.h"
#include "heventheader.h"




#include "TH1D.h"
#include "TF1.h"
#include "TString.h"

#include <vector>
#include <map>
#include <list>

using namespace std;



class HParticleCand;
class HCategory;


#define STMOD 2
#define STSTRIP 16
#define STBIN 500

class HParticleT0Reco : public HReconstructor {



    //-------------------------------------------------------
    // helper structs
    typedef struct  {
        Int_t   id;
	Bool_t  is;
	Float_t deltaTime;
	Float_t sigmaTime;
	Float_t normTime;
	Float_t deltadEdx;
	Float_t sigmadEdx;
	Float_t normdEdx;
	Float_t norm;

	void clear()
	{
            is        = kFALSE;
	    deltaTime = 999.;
	    sigmaTime = 0.1;
	    normTime  = 9999.;
	    deltadEdx = 999.;
	    sigmadEdx = 0.1;
	    normdEdx  = 9999.;
	    norm      = 9999.;
	}
    } pidvalues;



    typedef struct {

        Float_t meanq ;
        Float_t meanqall;
        Float_t scor;

        TH1D* h ;
        TH1D* he;

    } startvalues;

    typedef struct {
        HParticleCand* c;
	Int_t   index;
	Float_t times;
	Float_t error;

    } tofvalues;

    typedef struct {

	list <Float_t> t0   [2];       // systems
	list <Float_t> t0err[2];       // systems
	vector <vector < list<Float_t> > > t0forrpcstrip;
	vector <vector < list<Float_t> > > t0forrpcstriperr;
	vector <vector < list<Float_t> > > meanqstrip;

    } buffers;

    class momCut {
    public:
	Float_t min;
	Float_t max;
        Int_t   id;

	momCut(Int_t Id=2, Float_t Min=0,Float_t Max=3000){
	    id=Id;
	    min=Min;
            max=Max;
	}

	Bool_t eval(Float_t mom){
	    return   (mom>min && mom<max) ? kTRUE: kFALSE;
	}

    };

    //-------------------------------------------------------



private:

    //-------------------------------------------------------
    // input objects
    HCategory* candCat;
    HCategory* catStart;
    HCategory* catStartcal;
    HEventHeader* fEvHeader;
    //-------------------------------------------------------

    //-------------------------------------------------------
    vector<tofvalues>* eventMETA[2];   // helper array to switch systems
    vector<tofvalues> eventTOF;        // per event values of all candidates TOF
    vector<tofvalues> eventRPC;        // per event values of all candidates RPC
    startvalues start[STMOD][STSTRIP]; // values per start module:strip
    buffers buff;                      // all circular buffers
    vector <Int_t>         vparticle;  // all known ids
    map<Int_t,Int_t>      mIdToIndex;  // all known ids
    map<Int_t,momCut> mMomCut1Sys[2];  // mom cuts iteration 1 per sys
    map<Int_t,momCut> mMomCut2Sys[2];  // mom cuts iteration 2 per sys
    //-------------------------------------------------------

    //-------------------------------------------------------
    //Systematic offsets (running per event)!
    Float_t t0rpc;
    Float_t t0tof;
    Float_t t0startcont;
    Float_t t0startconterr;
    Double_t c;  // speed of light
    //-------------------------------------------------------

    //-------------------------------------------------------
    // flags
    TString run;                 // beamtime
    Bool_t  fUseFlagged;         // use flagged kIsUsed candidates for t0 reco (default=kTRUE)
    Bool_t  fisSimulation;       // for simulation START has to be treated different
    Bool_t  fisEmbedding;        // embedding mode
    //-------------------------------------------------------

    //-------------------------------------------------------
    // correction containers
    HEnergyLossCorrPar* eLoss;
    //-------------------------------------------------------

    void    initWalkHists();
    Bool_t  isEmbedded(HParticleCand* pCand);
    void    fill();
    void    fillMETA(Float_t t0, Float_t err,Int_t sys);
    Float_t getT0   (Int_t sys);
    Float_t getT0Err(Int_t sys);
    Float_t getBeta(HParticleCand* pCand);
    void    fillBuffer();
    void    fillCorrections();
    void    correctBeta();
    Float_t getOnlineStripCorrection(Int_t mod, Int_t strip);
    Float_t getOnlineMeanQ          (Int_t mod, Int_t strip);
    void    calculateStartT0Cont();
    void    setPIDs();



public:

    HParticleT0Reco(const Text_t* name="ParticleT0Reco",const Text_t* title="ParticleT0Reco", const Text_t* beamtime="apr12");
    ~HParticleT0Reco();
    Int_t  execute(void);
    Bool_t init(void);
    Bool_t finalize(){ return kTRUE;}
    void   setUseFlaggedCandidates(Bool_t use=kTRUE)    { fUseFlagged = use;}

    ClassDef(HParticleT0Reco,0)

};


#endif
