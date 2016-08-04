#ifndef __HPARTICLECANDFILLERPAR_H__
#define __HPARTICLECANDFILLERPAR_H__
#include "hparcond.h"
#include "hparticletool.h"

#include "TArrayD.h"
#include "TH1D.h"
#include "TH2D.h"

class HParamList;

class HParticleCandFillerPar : public HParCond {
protected:

    TArrayD phiLow   [6];     // momentum dependend cut per sec : lower phi boundary
    TArrayD phiUp    [6];     // momentum dependend cut per sec : upper phi boundary
    TArrayD thetaLow [6];     // momentum dependend cut per sec : lower theta boundary
    TArrayD thetaUp  [6];     // momentum dependend cut per sec : upper theta boundary
    Float_t zRichCenter;      // Rich shift with respect to nominal lab-position=0 in mm


    TH1D*   hphiLow  [6];     //!
    TH1D*   hphiUp   [6];     //!
    TH1D*   hthetaLow[6];     //!
    TH1D*   hthetaUp [6];     //!



    void    createHists();   // be careful with this function! needs to called after filling!
    void    removeHists();   // be careful with this function!

public:
    HParticleCandFillerPar(const Char_t* name   = "ParticleCandFillerPar",
			   const Char_t* title  = "parameters for PartticleCandFiller",
			   const Char_t* context= "ParticleCandFillerParProduction");
    ~HParticleCandFillerPar();

    void     setPhiLow  (Int_t s, const TArrayD& linData);
    void     setPhiUp   (Int_t s, const TArrayD& linData);
    void     setThetaLow(Int_t s, const TArrayD& linData);
    void     setThetaUp (Int_t s, const TArrayD& linData);
    void     setZRichCenter(Float_t shift) { zRichCenter = shift; }
    const TArrayD* getPhiLow  (Int_t s)  {return (s >= 0 && s < 6)? &phiLow  [s] : 0;}
    const TArrayD* getPhiUp   (Int_t s)  {return (s >= 0 && s < 6)? &phiUp   [s] : 0;}
    const TArrayD* getThetaLow(Int_t s)  {return (s >= 0 && s < 6)? &thetaLow[s] : 0;}
    const TArrayD* getThetaUp (Int_t s)  {return (s >= 0 && s < 6)? &thetaUp [s] : 0;}



    Float_t getPhiLow  (Int_t s, Float_t mom) { return HParticleTool::getInterpolatedValue(hphiLow  [s],mom,kFALSE) ;}
    Float_t getPhiUp   (Int_t s, Float_t mom) { return HParticleTool::getInterpolatedValue(hphiUp   [s],mom,kFALSE) ;}
    Float_t getThetaLow(Int_t s, Float_t mom) { return HParticleTool::getInterpolatedValue(hthetaLow[s],mom,kFALSE) ;}
    Float_t getThetaUp (Int_t s, Float_t mom) { return HParticleTool::getInterpolatedValue(hthetaUp [s],mom,kFALSE) ;}
    Float_t getRichCorr(Float_t zVertex, Float_t thetaRich, Float_t phiRich);
    Float_t getZRichCenter() { return zRichCenter ;}
    Bool_t  acceptPhiTheta(Int_t s,Float_t mom,Float_t dphi,Float_t dtheta);

    Bool_t  init(HParIo*, Int_t*);
    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();
    void    printParam(void);
    ClassDef(HParticleCandFillerPar,2) // Container for ParticleCandFiller parameters
};
#endif  /*!__HPARTICLECANDFILLERPAR_H_*/
