#ifndef HSHOWERDIGIPAR_H
#define HSHOWERDIGIPAR_H

#include "hparcond.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TArrayF.h"
#include "TArrayD.h"
#include "TObjArray.h"

class HShowerDigiPar : public HParCond {
private:
  TArrayF gain;                // gain of the 18 chambers
  TArrayF threshold;           // charge threshold for efficiency determination for 18 chambers

  TArrayF effScaleMap;         // efficiency mapping sector, module, row, col [6*3*32*32]
  TArrayD globalEff;           // global function - Efficiency (Beta) scaled up & down
  TArrayD chargeVsBeta[6][3];  // charge arrays per Beta

  Float_t fThickDet;           // thickness of gas chamber
  Int_t   nMatrixRange;        // size of pfChargeMatrix = 2*nMatrixRange+1
  Float_t fBoxSize;            // size of element of charge density matrix
  Float_t fChargeSlope;        // charge distribution slope parameter
  Float_t fPlaneDist;          // plane cathod to sense wires plane distance
  Float_t fPadThreshold;       // charge threshold for a pad
  Float_t fUpdatePadThreshold; // charge threshold for track updating

  Int_t   chargeMatrixSize;    //! size of pfChargeMatrix = 2*nMatrixRange+1
  Float_t *pfChargeMatrix;     //! charge density matrix

  Int_t   nGlobalEffBins;      //! number of bins in efficiency histograms
  Int_t   nQvBxbins[6][3];     //! number of Q bins in Q(beta) propability histogram

  TH1D      *phEff;            //! efficiency(beta) histogram
  TH2D      *ph2QvB[6][3];     //! Q(beta) propability histogram
  TObjArray *pArrayQvB[6][3];  //! array of 1-dim Q histograms

public:
  HShowerDigiPar(const Char_t* name="ShowerDigiPar",
                 const Char_t* title="Digitisation parameters for Shower",
                 const Char_t* context="ShowerStandardDigiPar");
  ~HShowerDigiPar();

  void    clear(void);
  Bool_t  init(HParIo*, Int_t*);
  void    putParams(HParamList*);
  Bool_t  getParams(HParamList*);

  Float_t getGain(Int_t sec, Int_t mod)      { return gain.At(sec*3+mod);      }
  Float_t getThreshold(Int_t sec, Int_t mod) { return threshold.At(sec*3+mod); }

  Float_t getThickDet()           { return fThickDet;           }
  Int_t   getMatrixRange()        { return nMatrixRange;        }
  Float_t getBoxSize()            { return fBoxSize;            }
  Float_t getChargeSlope()        { return fChargeSlope;        }
  Float_t getPlaneDist()          { return fPlaneDist;          }
  Float_t getPadThreshold()       { return fPadThreshold;       }
  Float_t getUpdatePadThreshold() { return fUpdatePadThreshold; }

  void setGain(Int_t sec, Int_t mod, Float_t fGain)       { gain.AddAt(fGain,sec*3+mod);       }
  void setThreshold(Int_t sec, Int_t mod, Float_t fThres) { threshold.AddAt(fThres,sec*3+mod); }

  void setThickDet(Float_t fThick)           { fThickDet           = fThick; }
  void setMatrixRange(Int_t iRange)          { nMatrixRange        = iRange; }
  void setBoxSize(Float_t fSize)             { fBoxSize            = fSize;  }
  void setChargeSlope(Float_t fSlope)        { fChargeSlope        = fSlope; }
  void setPlaneDist(Float_t fDist)           { fPlaneDist          = fDist;  }
  void setPadThreshold(Float_t fThres)	     { fPadThreshold       = fThres; }
  void setUpdatePadThreshold(Float_t fThres) { fUpdatePadThreshold = fThres; }

  void   setEffScaleMap(TArrayF&);
  TArrayF& getEffScaleMap() { return effScaleMap;}
  void   setGlobalEff(TArrayD&);
  void   setChargeVsBeta(Int_t, Int_t, TArrayD&);
  Bool_t recreateHistograms();

  const Float_t* getChargeMatrix()                { return pfChargeMatrix;   }
  const TH1D*    getEfficiencyHist()              { return phEff;            } 
  const TH2D*    getQvBHist(Int_t sec, Int_t mod) { return ph2QvB[sec][mod]; }

  Float_t getEfficiency(Int_t sec,Int_t mod,Int_t row,Int_t col, Float_t fBeta);
  Bool_t  checkEfficiency(Int_t sec,Int_t mod,Int_t row,Int_t col, Float_t fBeta);
  Float_t getCharge(Int_t sec,Int_t mod, Float_t fBeta);

  //private:
  void    removeHistograms(void);
  void    removeChargeHistograms(void);
  void    removeEfficiencyHistograms(void);
  void    removeQvBHistograms(void);
  Float_t calcCharge(Float_t fCharge, Float_t fDist, Float_t fXd,
                     Float_t fYd, Float_t fXu, Float_t fYu);
  void    setChargeMatrix(Int_t nRange, const Float_t *pMatrix = NULL);
  Bool_t  initChargeMatrix(void);
  Bool_t  initEffHistogram(void);
  Bool_t  initSumVersBetaHistograms(void);
  Int_t   padIndex(Int_t sec,Int_t mod,Int_t row,Int_t col) {
            return sec*3072 + mod*1024 + row*32 + col;
          } // 3*32*32=3072, 32*32=1024

  ClassDef(HShowerDigiPar,1) //ROOT extension
};
#endif
