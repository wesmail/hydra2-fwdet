#ifndef  HTOFHITF2_H
#define  HTOFHITF2_H

#include "hreconstructor.h"

class HCategory;
class HTofCalPar;
class HTofWalkPar;
class HTofDigiPar;
class HIterator;
class HTofGeomPar;
class HSpecGeomPar;
class HSpectrometer;
class HRuntimeDb;
class HTofHit;
class HTofRaw;

class HTofHitF2 : public HReconstructor {
protected:
  HCategory *fRawCat;        //!Raw data category
  HCategory *fRawCatTmp;     //!Raw tmp data category
  HCategory *fHitCat;        //!Pointer to the hits category
  HCategory *fHitCatTmp;     //!Pointer to the tmp hits category
  HCategory *fStartHitCat;   //! Category with start hits
  HTofCalPar *fCalPar;       //!Calibration parameters
  HTofCalPar *fCalParSim;    //!Calibration parameters of sim for embedding
  HTofDigiPar*fDigiPar;
  HLocation fLoc;            //!
  HIterator *iter;           //!
  HIterator *iterTmp;        //!
  HTofGeomPar *fTofGeometry; //!Container for TOF geometry
  HSpecGeomPar *fSpecGeometry;
  Bool_t fTofSimulation;
  HTofWalkPar* fTofWalkPar;

  static Bool_t  bforceKeepGeant;  //! overwrite embedding mode from 1 to 2 if true (default : false)
  void    fillGeometry(HTofHit *);
  void    fillHitCat  (Bool_t sim=kFALSE,Bool_t embed=kFALSE);
  void    mergeHitCats(Bool_t sim,Bool_t emb);
  Float_t tot2amp(Float_t tot);

public:
  HTofHitF2(void);
  HTofHitF2(const Text_t *name,const Text_t *title);
  ~HTofHitF2(void);
  static void setForceKeepGeant(Bool_t force) { bforceKeepGeant = force;}

  Bool_t init(void);
  void   initParContainer(HSpectrometer *,HRuntimeDb *);
  Bool_t finalize(void) {return kTRUE;}
  Int_t  execute(void);
  ClassDef(HTofHitF2,0) //Finds TOF hits
};

#endif /* !HTOFHITF2_H */






