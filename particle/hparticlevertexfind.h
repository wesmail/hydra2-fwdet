#ifndef HPARTICLEVERTEXFIND_H
#define HPARTICLEVERTEXFIND_H

#include "hreconstructor.h"
#include "hgeomvector.h"
#include "hgeomvertexfit.h"
#include "hspecgeompar.h"
#include "TIterator.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TClonesArray.h"

#include <vector>
using namespace std;

class HCategory;
class HMdcGeomPar;
class HIterator;
class HParticleCand;

class HParticleVertexFind : public HReconstructor {
public:
  HParticleVertexFind(const Text_t name[] = "",const Text_t title[] = "",Bool_t tukey = kTRUE);
  virtual ~HParticleVertexFind(void);
  Bool_t      init(void);
  Bool_t      finalize(void);
  Int_t       execute(void);
  void        setCut(Double_t minz=-100,Double_t maxz=50,Double_t maxr=50) { fminZ = minz; fmaxZ = maxz; fmaxR = maxr;}
  void        useTukeyWeights        (Bool_t flag) { fUsingTukey    = flag; }
  void        setTukeyConstant       (Double_t d)  { fTukeyConst    = d; }
  void        setMaxIterations       (Int_t max)   { fMaxIterations = max; }
  void        setEpsilon             (Double_t ep) { fEpsilon       = ep; }
  void        setMomChi2Cut(Float_t cut)    { fmomChi2Cut  = cut; }
  void        setSeg0Chi2Cut(Float_t cut)   { fseg0Chi2Cut = cut;}
  void        setMinReqTracks(Int_t min)    { fminReqTrack = min;}
  void        setMinWindow(Double_t wind)   { fminWindow = wind;}
  void        setProgessiveTukey(Bool_t prog){ fProgressiveTukey = prog;}
  void        setCallExecuteManual   (Bool_t call) { fcallExecuteManual = call;}
  Double_t    getTukeyConstant       (void) { return fTukeyConst; }
  Int_t       getMaxIterations       (void) { return fMaxIterations; }
  Double_t    getEpsilon             (void) { return fEpsilon; }
  Float_t     getMomChi2Cut()               { return fmomChi2Cut; }
  Float_t     getSeg0Chi2Cut()              { return fseg0Chi2Cut;}
  Int_t       getMinReqTracks()             { return fminReqTrack;}
  Double_t    getMinWindow()                { return fminWindow;}
  Bool_t      getProgessiveTukey(void)      { return fProgressiveTukey;}
  Int_t       doFit();
  static void setSkipFakes(Bool_t skip)     { doSkipFakes = skip;}
protected:
  void        initVars(void);
  Bool_t      pointsToTarget(const HGeomVector &r,HGeomVector &alpha,Int_t sector,Int_t module,Double_t zmin,Double_t zmax);
  Bool_t      readInput     (HGeomVector &estimate);
  void        transform     (HParticleCand *data, HGeomVector &r, HGeomVector &alpha);
  Bool_t      hasConverged  (HGeomVector &v,HGeomVector &oldV);

  HGeomVertexFit fFitter;              //!
  TClonesArray   fPos;                 //! r for each track
  TClonesArray   fAlpha;               //! alpha vector for each track
  vector<HParticleCand*> fCands;       //! vector of candidate pointers passed to the fit
  vector<Double_t>     fweights;       //! vector of candidate weights
  HCategory      *fInput;              //! Geant MDC data
  HMdcGeomPar    *fGeometry;           //! Geometry transformation
  HSpecGeomPar   *fSpecGeometry;       //! Spectrometer's geometry
  Double_t       fTukeyConst;          // Value used as Tukey constant
  Int_t          fMaxIterations;       // Maximum number of iterations
  Bool_t         fUsingTukey;          // Flag turning on and off Tukey weights
  Double_t       fEpsilon;             // Used for convergence criterium
  Double_t       fminZ;                //! minz range for segments
  Double_t       fmaxZ;                //! maxz range for segments
  Double_t       fmaxR;                //! maxr range for segments
  Float_t        fmomChi2Cut;          //! cut off in inner segment chi2
  Float_t        fseg0Chi2Cut;         //! cut off in mom fit chi2
  Int_t          fminReqTrack;         //! minimum required tracks after mean z filter
  Double_t       fminWindow;           //! minimum window  for mean z filter
  Bool_t         fProgressiveTukey;    //! make stronger cut with rising mult
  Bool_t         fcallExecuteManual;   //!
  static Bool_t  doSkipFakes;          //! skip candidate smarked as fake (default=kTRUE)
public:
 
  ClassDef(HParticleVertexFind,1)
};

#endif
