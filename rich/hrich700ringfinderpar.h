
#ifndef HRICH700RINGFINDERPAR_H_
#define HRICH700RINGFINDERPAR_H_

#include "hparcond.h"
#include <string>


class HParamList;

class HRich700RingFinderPar : public HParCond {

private:
    Int_t  fNofParts;          // number of groups of hits for HT

    Float_t fMaxDistance;        // maximum distance between two hits
    Float_t fMinDistance;        // minimum distance between two hits

    Float_t fMinRadius;          // minimum radius of the ring
    Float_t fMaxRadius;          // maximum radius of the ring

    Int_t   fNofBinsX;          // number of bins in X direction
    Int_t   fNofBinsY;          // number of bins in Y direction

    Int_t   fHTCut;             // cut number of entries in maximum bin of XY histogram

    Int_t   fNofBinsR;          // number of bins in radius histogram
    Int_t   fHTCutR;            // cut number of entries in maximum bin of Radius histogram

    Int_t   fMinNofHitsInArea;  // minimum number of hits in the local area

    Float_t fUsedHitsAllCut;     // percent of used hits

    Float_t fRmsCoeffCOP;
    Float_t fMaxCutCOP;



public:

    HRich700RingFinderPar(const Char_t* name   ="RichRingFinderPar",
			  const Char_t* title  ="RingFinder parameters for RICH",
			  const Char_t* context="RichRingFinderParProduction");
    virtual ~HRich700RingFinderPar();

    Int_t   getNofParts   () { return fNofParts; }
    Float_t  getMaxDistance() { return fMaxDistance; }
    Float_t  getMinDistance() { return fMinDistance; }
    Float_t  getMaxRadius  () { return fMaxRadius; }
    Float_t  getMinRadius  () { return fMinRadius; }

    Int_t   getNofBinsX   () { return fNofBinsX; }
    Int_t   getNofBinsY   () { return fNofBinsY; }
    Int_t   getNofBinsR   () { return fNofBinsR; }
    Int_t   getHTCut      () { return fHTCut;    }
    Int_t   getHTCutR     () { return fHTCutR;   }
    Int_t   getMinNofHitsInArea() { return fMinNofHitsInArea; }
    Float_t  getUsedHitsAllCut  () { return fUsedHitsAllCut;   }
    Float_t  getRmsCoeffCOP     () { return fRmsCoeffCOP;}
    Float_t  getMaxCutCOP       () { return fMaxCutCOP; }

    void   getNofParts        (Int_t   a) { fNofParts = a; }
    void   getMaxDistance     (Float_t  a) { fMaxDistance = a; }
    void   getMinDistance     (Float_t  a) { fMinDistance = a; }
    void   getMaxRadius       (Float_t  a) { fMaxRadius = a; }
    void   getMinRadius       (Float_t  a) { fMinRadius = a; }

    void   setNofBinsX        (Int_t   a) { fNofBinsX = a; }
    void   setNofBinsY        (Int_t   a) { fNofBinsY = a; }
    void   setNofBinsR        (Int_t   a) { fNofBinsR = a; }
    void   setHTCut           (Int_t   a) { fHTCut = a;    }
    void   setHTCutR          (Int_t   a) { fHTCutR = a;   }
    void   setMinNofHitsInArea(Int_t   a) { fMinNofHitsInArea = a; }
    void   setUsedHitsAllCut  (Float_t  a) { fUsedHitsAllCut = a;   }
    void   setRmsCoeffCOP     (Float_t  a) { fRmsCoeffCOP = a;}
    void   setMaxCutCOP       (Float_t  a) { fMaxCutCOP = a; }




    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();
    void    printParam(void);
    std::string  getStringForParTxtFile();

    ClassDef(HRich700RingFinderPar, 1)
};

#endif
