#ifndef __HPARTICLECANDFILLER_H__
#define __HPARTICLECANDFILLER_H__

#include "hreconstructor.h"
#include "hparticlestructs.h"
#include "hmdcsizescells.h"
#include "hmdcgeomobj.h"
#include "hgeomvector.h"

using  std::vector;

class HCategory;
class HIterator;
class HMetaMatch2;
class HMdcSeg;
class HParticleCandFillerPar;
class HTofWalkPar;
class HMdcLayer;

class HParticleCandFiller : public HReconstructor {
protected:
	
	// input data categories
	HCategory              *fCatMetaMatch;		   //!
	HCategory              *fCatMdcTrkCand;	           //!
	HCategory              *fCatMdcSeg;    	           //!
	HCategory              *fCatTofHit;    	           //!
	HCategory              *fCatTofCluster;	           //!
	HCategory              *fCatShowerHit;	           //!
	HCategory              *fCatEmcCluster;	           //!
	HCategory              *fCatRichHit;   	           //!
	HCategory              *fCatRpcCluster;	           //!
	HCategory              *fCatSpline;    	           //!
	HCategory              *fCatRK;		           //!
	HCategory              *fCatKalman;   	           //!
	HCategory              *fCatGeantKine;	           //!
	
	HIterator              *fMetaMatchIter;	           //!

	// output data categories
	HCategory              *fCatParticleCand;	   //!
        HCategory              *fCatParticleDebug;         //!
        HCategory              *fCatParticleMdc;         //!

	// flags
	Bool_t                  fbIsSimulation;	      	   //!
	Bool_t                  fbIsDebug;                 //! debug mode ?
	Bool_t                  fbFillMdc;                 //! true: fill catParticleMdc
	Bool_t                  fbgoodSeg0;                //! true: keep only fitted seg0
        Bool_t                  fbgoodSeg1;                //! true: keep only fitted seg1
        Bool_t                  fbgoodMeta;                //! true: keep only candidate with META
        Bool_t                  fbgoodRK;                  //! true: keep only fitted RK
        Bool_t                  fbgoodLepton;              //! true: keep only candidate with ring match
        Bool_t                  fbnoFake;                  //! true: keep only candidate which is not markerd as fake rejected
        Bool_t                  fbdoRichAlign;             //! true: (default)align the rich (real data)
        Bool_t                  fbdoRichVertexCorr;        //! true: (default) do correction for vertexz pos in ring sorting
        Bool_t                  fbdoMETAQANorm;            //! true: (default) do normalization of RK meta dx
        Bool_t                  fbdoMomentumCorr;          //! true: (default) do systematic corrections on momentum
        Bool_t                  fbdoPathLengthCorr;        //! true: (default) do correction of path length to vertex
	Bool_t                  fbdoGeantAcceptance;       //! true: (default) loop geant kine and fill acceptance + crop bits
	Int_t                   fmomSwitch;                //! : Particle::kMomRK or Particle::kMomKalman
        Int_t                   fsortSwitch;               //! sort by quality (0), radius(1)
	// parameter container
        HParticleCandFillerPar *fFillerPar;                //!
        HTofWalkPar            *fTofWalkPar;                //!
        HMdcSizesCells         *fSizesCells;               //!
        HMdcLayer* cropLay;                                //!
	// working structures
	vector < candidate * > all_candidates;  //! vector for all metaMatch objects candidates.

        // cut parameters
        Int_t   fMinWireGoodTrack;              //! minium number of wire from same track for good track (MDC), otherwise ghost
	Float_t fScaleGhostTrack;               //! scale weight of wires of ghost track (MDC)
        Float_t fScaleGoodTrack;                //! scale weight of wires of good  track (MDC)
        Float_t fAngleCloseTrack;               //! take into account for filling neighbour tracks with open angle < this

	void   initVars();
	void   setConditions(const Option_t par[]);
        Int_t  findBestRich(HMetaMatch2* meta,HMdcSeg* seg);

	void   fillCandNoMeta(Bool_t rkSuccess,HMetaMatch2* meta,candidate& cand,Int_t num);
        void   fillCandTof   (Bool_t rkSuccess,HMetaMatch2* meta,candidate& cand,Int_t num,Int_t slot);
        void   fillCandRpc   (Bool_t rkSuccess,HMetaMatch2* meta,candidate& cand,Int_t num,Int_t slot);
        void   fillCandShower(Bool_t rkSuccess,HMetaMatch2* meta,candidate& cand,Int_t num,Int_t slot);
        void   fillCandEmc   (Bool_t rkSuccess,HMetaMatch2* meta,candidate& cand,Int_t num,Int_t slot);

	void   fillCand(HMetaMatch2* meta);
	void   fillCandSim(candidate& cand);
	void   fillCollectiveProperties(void);
	void   fillSingleProperties    (void);
	void   fillOutput(void);
	void   clearVector(void);
        void   fillGeantAcceptance();
	void   checkCropLayer(HParticleCand* c);
public:
	HParticleCandFiller(const Option_t pOpt[] = "");
	HParticleCandFiller(const Text_t* name,const Text_t* title,const Option_t pOpt[] = "");
	~HParticleCandFiller(void);
	
	Bool_t init(void);
	Bool_t  reinit(void);
	Int_t  execute(void);
	Bool_t finalize(void);

	void setMinWireGoodTrack(Int_t w)  { fMinWireGoodTrack = w; }
        void setScaleGoodTrack (Float_t s) { fScaleGoodTrack   = s; }
        void setScaleGhostTrack(Float_t s) { fScaleGhostTrack  = s; }
        void setAngleCloseTrack(Float_t a) { fAngleCloseTrack  = a; }
        void setMomSwitch      (Int_t s  ) { fmomSwitch        = s; }
        void setDoAlignRich    (Bool_t align ) { fbdoRichAlign = align; }
        void setDoRichVertexCorr(Bool_t corr ) { fbdoRichVertexCorr = corr; }
        void setDoMETAQANorm   (Bool_t norm )  { fbdoMETAQANorm = norm; }
        void setDoMomentumCorr (Bool_t corr )  { fbdoMomentumCorr = corr; }
        void setDoPathLengthCorr(Bool_t corr ) { fbdoPathLengthCorr = corr; }
        void setDoGeantAcceptance(Bool_t acc)  { fbdoGeantAcceptance = acc; }
	void setFillMdc        (Bool_t fill)   { fbFillMdc     = fill;}
	void setSortMeta       (Int_t s=0)     { fsortSwitch = (s>=0&&s<2) ? s:0;}
	ClassDef(HParticleCandFiller,0) // HMetaMatch -> HParticleCand
};

#endif /* !__HPARTICLECAND_H__ */

