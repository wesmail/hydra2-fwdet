#ifndef HSHOWERHITFPAR_H
#define HSHOWERHITFPAR_H

#include "hparcond.h"
#include "TArrayI.h"
#include "TArrayF.h"

class HShowerHitFPar : public HParCond {
private:
  Int_t   nSectors;        //! number of sectors
  Int_t   nModules;        //! number of modules
  TArrayI pRowBorder;      // It defines active area for local maxima searching
  TArrayI pColBorder;      // It defines active area for local maxima searching
  Int_t   nThreshold;      // Local maxima searching threshold
  Float_t fGainPost1;      // Gain parameter in shower criterium for post1
  Float_t fThresholdPost1; // Threshold parameter in shower criterium for post1
  Float_t fGainPost2;      // Gain parameter in shower criterium for post2
  Float_t fThresholdPost2; // Threshold parameter in shower criterium for post2
public:
  HShowerHitFPar(const Char_t* name="ShowerHitFPar",
		 const Char_t* title="Parameter for Shower hit finder",
		 const Char_t* context="ShowerStandardHitFinder");
  ~HShowerHitFPar() {}

  void clear();
  void defaultInit();

  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);

  Int_t getColBord(Int_t nSect, Int_t nMod, Int_t* nLowerCol, Int_t* nUpperCol);
  Int_t getRowBord(Int_t nSect, Int_t nMod, Int_t* nLowerRow, Int_t* nUpperRow);

  Int_t getModules(){return nModules;}
  Int_t getSectors(){return nSectors;}
  Int_t getThreshold(){return nThreshold;}

  Float_t getGainPost1(){return fGainPost1;}
  Float_t getThresholdPost1(){return fThresholdPost1;}
  Float_t getGainPost2(){return fGainPost2;}
  Float_t getThresholdPost2(){return fThresholdPost2;}

  Int_t setColBord(Int_t nSect, Int_t nMod, Int_t nLowerCol, Int_t nUpperCol);
  Int_t setRowBord(Int_t nSect, Int_t nMod, Int_t nLowerRow, Int_t nUpperRow);

  void setThreshold(Int_t t){nThreshold = t;}

  void setGainPost1(Float_t gain){fGainPost1 = gain;}
  void setThresholdPost1(Float_t threshold){fThresholdPost1 = threshold;}
  void setGainPost2(Float_t gain){fGainPost2 = gain;}
  void setThresholdPost2(Float_t threshold){fThresholdPost2 = threshold;}

private:
  Int_t getIndex(Int_t nSect, Int_t nMod);
  ClassDef(HShowerHitFPar,3) //ROOT extension
};

#endif /* !HSHOWERHITFPAR_H */
