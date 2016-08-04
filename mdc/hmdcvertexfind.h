#ifndef HMDCVERTEXFIND_H
#define HMDCVERTEXFIND_H

#include "hreconstructor.h"
#include "hgeomvector.h"
#include "hgeomvertexfit.h"
#include "hspecgeompar.h"
#include "TIterator.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TClonesArray.h"

class HCategory;
class HMdcGeomPar;
class HIterator;
class HMdcSeg;

class HMdcVertexFind : public HReconstructor {
public:
  enum EInputMode {kSegments,kHits};
  HMdcVertexFind(const Text_t name[] = "",const Text_t title[] = "",EInputMode m = kSegments,Bool_t tukey = kTRUE);
  virtual ~HMdcVertexFind(void);
  Bool_t init(void);
  Bool_t finalize(void);
  Int_t  execute(void);
  void        setCut(Double_t minz=-100,Double_t maxz=50,Double_t maxr=50) { fminZ = minz; fmaxZ = maxz; fmaxR = maxr;}
  void        useTukeyWeights        (Bool_t flag) { fUsingTukey    = flag; }
  void        setDebugMode           (Bool_t flag) { fDebugMode     = flag; }
  void        setTukeyConstant       (Double_t d)  { fTukeyConst    = d; }
  void        setMaxIterations       (Int_t max)   { fMaxIterations = max; }
  void        setEpsilon             (Double_t ep) { fEpsilon       = ep; }
  void        setSeg0Chi2Cut(Float_t cut)   { fseg0Chi2Cut = cut;}
  void        setMinReqTracks(Int_t min)    { fminReqTrack = min;}
  void        setMinWindow(Double_t wind)   { fminWindow = wind;}
  void        setProgessiveTukey(Bool_t prog){ fProgressiveTukey = prog;}
  void        setCallExecuteManual   (Bool_t call) { fcallExecuteManual = call;}
  EInputMode  getInputMode           (void) { return fInputMode; }
  Double_t    getTukeyConstant       (void) { return fTukeyConst; }
  Int_t       getMaxIterations       (void) { return fMaxIterations; }
  Double_t    getEpsilon             (void) { return fEpsilon; }
  Float_t     getSeg0Chi2Cut()              { return fseg0Chi2Cut;}
  Int_t       getMinReqTracks()             { return fminReqTrack;}
  Double_t    getMinWindow()                { return fminWindow;}
  Bool_t      getProgessiveTukey(void)      { return fProgressiveTukey;}
  Int_t       doFit();


  static void setRejectEmbeddedTracks(Bool_t reject = kTRUE){rejectEmbeddedTracks = reject;}
  static void setUseEventSeqNumber   (Bool_t use    = kTRUE){useEventSeqNumber    = use;   }
  static void setSkipNoVertex        (Bool_t skip   = kTRUE){doSkipNoVertex       = skip;  }
  static void setSkipFakeSegments    (Bool_t skip   = kTRUE){doSkipFakeSegments   = skip;  }
protected:
  void initVars(void);
  Bool_t pointsToTarget(const HGeomVector &r,HGeomVector &alpha,Int_t sector,Int_t module,Double_t zmin,Double_t zmax);
  Bool_t readSegments  (HGeomVector &estimate);
  Bool_t readHits      (HGeomVector &estimate);
  void transform       (HMdcSeg *data, HGeomVector &r, HGeomVector &alpha);
  Bool_t hasConverged  (HGeomVector &v,HGeomVector &oldV);

  HGeomVertexFit fFitter;              //!
  TNtuple        *fControl;            //!
  TClonesArray   fPos;                 //! r for each track
  TClonesArray   fAlpha;               //! alpha vector for each track
  HCategory      *fInput;              //! Geant MDC data
  HMdcGeomPar    *fGeometry;           //! Geometry transformation
  HSpecGeomPar   *fSpecGeometry;       //! Spectrometer's geometry
  HIterator      *fIter;               //!
  Double_t       fTukeyConst;          // Value used as Tukey constant
  Int_t          fMaxIterations;       // Maximum number of iterations
  Bool_t         fUsingTukey;          // Flag turning on and off Tukey weights
  Bool_t         fDebugMode;           // Debug mode on/off
  Double_t       fEpsilon;             // Used for convergence criterium
  Double_t       fminZ;                //! minz range for segments
  Double_t       fmaxZ;                //! maxz range for segments
  Double_t       fmaxR;                //! maxr range for segments
  Float_t        fseg0Chi2Cut;         //! cut off in mom fit chi2
  Int_t          fminReqTrack;         //! minimum required tracks after mean z filter
  Double_t       fminWindow;           //! minimum window  for mean z filter
  Bool_t         fProgressiveTukey;    //! make stronger cut with rising mult
  Bool_t         fcallExecuteManual;   //!
  EInputMode     fInputMode;           //! Read segments or hits?
  Bool_t         isEmbedding;          //! kTRUE if is in embedding mode
  static Bool_t  rejectEmbeddedTracks; //! = kTRUE (default) reject embedded tracks from vertex calculation
  static Bool_t  useEventSeqNumber;    //! use the eventSeq Number decide of event skipping in embedding mode
  static Bool_t  doSkipNoVertex;       //! skip events if no vertex is calculated
  static Bool_t  doSkipFakeSegments;   //! skip fake segments (default = kTRUE)
  TIterator*     geantKineIter;        //! Iterator over GeantKine category

public:
 
  ClassDef(HMdcVertexFind,1)
};

#endif
