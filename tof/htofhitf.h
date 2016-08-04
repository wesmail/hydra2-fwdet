#ifndef  HTOFHITF_H
#define  HTOFHITF_H

#include "hreconstructor.h"

class HCategory;
class HTofCalPar;
class HIterator;
class HTofGeomPar;
class HSpecGeomPar;
class HSpectrometer;
class HRuntimeDb;
class HTofHit;
class HTofRaw;

class HTofHitF : public HReconstructor {
protected:
  HCategory *fRawCat;      //!Raw data category
  HCategory *fRawCatTmp;   //!Raw tmp data category
  HCategory *fHitCat;      //!Pointer to the hits category
  HCategory *fHitCatTmp;   //!Pointer to the tmp hits category
  HCategory *fStartHitCat; //! Category with start hits
  HTofCalPar *fCalPar;    //!Calibration parameters
  HTofCalPar *fCalParSim; //!Calibration parameters of sim for embedding
  HLocation fLoc;         //!
  HIterator *iter;        //!
  HIterator *iterTmp;     //!
  HTofGeomPar *fTofGeometry; //!Container for TOF geometry
  HSpecGeomPar *fSpecGeometry;
  Bool_t fTofSimulation;

  void fillGeometry(HTofHit *);
  void fillHitCat(Bool_t sim=kFALSE,Bool_t embed=kFALSE);
  void mergeHitCats(Bool_t sim,Bool_t emb);
  virtual HTofHit *makeHit(TObject *address);
  virtual void fillHit(HTofHit *, HTofRaw *);
  Float_t tot2amp(Float_t tot);

public:
  HTofHitF(void);
  HTofHitF(const Text_t *name,const Text_t *title);
  ~HTofHitF(void);
  Bool_t init(void);
  void initParContainer(HSpectrometer *,HRuntimeDb *);
  Bool_t finalize(void) {return kTRUE;}
  Int_t execute(void);
  ClassDef(HTofHitF,0) //Finds TOF hits
};

#endif /* !HTOFHITF_H */






