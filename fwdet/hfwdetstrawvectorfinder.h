/** HFwDetStrawVectorFinder.h
 *@author A.Zinchenko <Alexander.Zinchenko@jinr.ru>
 *@since 2016
 **
 ** Task class for vector finding in FwDet.
 ** Input: HFwDetStrawCalSim hits
 ** Output: HFwDetStrawVector objects
 **
 **/

#ifndef HFWDETSTRAWVECTORFINDER_H_
#define HFWDETSTRAWVECTORFINDER_H_ 1

#include "hlocation.h"
#include "hreconstructor.h"

#include <TDecompLU.h>
#include <map>
#include <set>
#include <vector>

class HCategory;
class HFwDetStrawCalSim;
class HFwDetStrawDigiPar;
class HFwDetStrawVector;

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
  virtual Bool_t reinit() { return kTRUE; }
  
  /** Task execution **/
  virtual Int_t execute();
  
  /** Finish at the event end **/
  virtual Bool_t finalize();
  
  /** Accessors **/
  //Int_t GetNofStat() const       { return fgkStat; }
  //TDecompLU *GetLU(Int_t patt) { return fLus[patt]; }
  //TMatrixDSym *GetMatr(Int_t patt) { return fMatr[patt]; }
  
 private:
  // Some constants
  static const Int_t fgkStat = 2;               // Number of stations
  static const Int_t fgkPlanes = 8;             // Number of straw planes per station
  static const Int_t fgkPlanes2 = 16;           // Number of straw planes in 2 stations

 private:
  HCategory* fPoints;                           // Input array of GEANT hits
  HCategory* fHits;                             // Input array of hits
  HCategory* fTrackArray;                       // Output array of vectors
  HFwDetStrawDigiPar* fStrawDigiPar;

  Int_t fNpass;                                 // Number of reco. passes
  Int_t fIndx0[fgkStat];                        // start indices of vectors for different passes
  std::multimap<Int_t,Int_t> fHitPl[fgkStat][fgkPlanes]; //! hit indices on planes vs tube No
  std::vector<HFwDetStrawVector*> fVectors[fgkStat+1]; //! track vectors for stations
  std::vector<HFwDetStrawVector*> fVectorsHigh[fgkStat]; //! track vectors for stations (high resolution)
  Double_t fUz[fgkPlanes][3];                   // hit float data
  Double_t fUzi[fgkPlanes][3];                  // hit int data
  Double_t fDz[fgkPlanes2];                     // geometrical constants (Z-distances from layer 0)
  Double_t fCosa[fgkPlanes2];                   // geometrical constants (cos of stereo angles)
  Double_t fSina[fgkPlanes2];                   // geometrical constants (sin of stereo angles)
  std::map<Int_t,TDecompLU*> fLus;              //! system matrices (for different hit layer patterns)
  Double_t fErrU;                               // hit measurement error
  Double_t fDiam;                               // tube diameter
  Double_t fCutChi2;                            // Chi2-cut
  Int_t fMinHits;                               // Min. number of hits on track to do fit 
  Double_t fZ0[fgkStat];                        // Z-positions of the first layers
  Double_t fRmin[fgkStat];                      // inner radii of stations
  Double_t fRmax[fgkStat];                      // outer radii of stations
  Double_t fDtubes[fgkStat][fgkPlanes];         // max. tube difference between views
  std::map<Int_t,TMatrixDSym*> fMatr;           //! system matrices (for different hit layer patterns)

  std::vector<std::pair<Int_t,Int_t> > fHit2d[fgkStat][fgkPlanes/2]; //! Indx1,Indx2 of doublet hits

  void ComputeMatrix();
  void GetHits();
  Bool_t SelectHitId(HFwDetStrawCalSim *hit, Int_t idSel);
  void MakeVectors();
  void ProcessDouble(Int_t ista, Int_t lay2, Int_t patt, Int_t flag, Int_t tube0, Int_t segment0);
  void AddVector(Int_t ista, Int_t patt, Double_t chi2, Double_t *pars, Bool_t lowRes = kTRUE); 
  void SetTrackId(HFwDetStrawVector *vec);
  Bool_t SelDoubleId(Int_t indx1, Int_t indx2);
  void FindLine(Int_t patt, Double_t *pars);
  Double_t FindChi2(Int_t ista, Int_t patt, Double_t *pars);
  void CheckParams();
  void HighRes();
  void ProcessSingleHigh(Int_t ista, Int_t plane, Int_t patt, Int_t flag, Int_t nok, Double_t uu[fgkPlanes][2]); 
  void MoveVectors();
  void RemoveClones();
  void RemoveShorts();
  void StoreVectors(Int_t sel);
  void MergeVectors();
  void SelectTracks(Int_t ipass);
  Double_t Refit(Int_t patt, Int_t *hinds, Double_t *pars, TMatrixDSym *cov, Int_t *lr);
  void AddTrack(Int_t ista0, HFwDetStrawVector *tr1, HFwDetStrawVector *tr2, 
		Int_t indx1, Int_t indx2, Double_t *parOk, Double_t c2, TMatrixDSym &w2);

  HFwDetStrawVectorFinder(const HFwDetStrawVectorFinder&);
  HFwDetStrawVectorFinder& operator=(const HFwDetStrawVectorFinder&);
  
  ClassDef(HFwDetStrawVectorFinder,0);
};

#endif
