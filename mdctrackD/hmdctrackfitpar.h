#ifndef HMDCTRACKFITPAR_H
#define HMDCTRACKFITPAR_H
#include "TArrayF.h"
#include "TArrayD.h"
#include "TArrayI.h"
#include "hparcond.h"

class HParamList;

class HMdcTrackFitPar : public HParCond {
protected:


    Double_t cutWeight      ;
    Int_t    tofFlag        ;
    Int_t    doTargScan     ;

    Double_t minTimeOffset  ;  // Time offset cut
    Double_t maxTimeOffset  ;  // -/-
    Int_t    minCellsNum    ;
    Int_t    chi2CutFlag    ;  // kTRUE - do cut for funct., else for chi2/ndf
    Double_t totalChi2Cut   ;  // default value for funct. cut
    Double_t chi2PerNdfCut  ;  // default value for chi2/ndf cut

    // Tukey weight constants:
    Int_t    useTukeyFlag   ;
    Double_t cnWs           ;  //2.54*2.54;
    Double_t cn2s           ;  //4.19*4.19;
    Double_t cn4s           ;  //3.26*3.26;
    Double_t minSig2        ;
    Int_t    maxNFilterIter ;
    Double_t minWeight      ;               // wt[time]=(wt[time]<minWeight) ? 0.:1.;
    Double_t maxChi2        ;  /*36.;6.0*/ // wt[time]=(chi2[time]>maxChi2) ? 0.:1.;

    Double_t minTOffsetIter ; // if(timeOffset<minTOffsetIter) timeOffset=minTOffsetIter

    // Fit parameters for derivatives calc.:
    Double_t funCt1 ;     // if(fun0 < funCt1)
    Double_t stepD1 ;     //               stepD = stepD1;
    Double_t funCt2 ;     // else if(fun0 < funCt2)
    Double_t stepD2 ;     //                stepD = stepD2;
    Double_t stepD3 ;     // else stepD = stepD3;



public:
    HMdcTrackFitPar(const Char_t* name = "MdcTrackFitPar",
		    const Char_t* title   = "parameters for Dubna track fitter",
		    const Char_t* context = "MdcTrackFitParProduction");
    ~HMdcTrackFitPar();

    Double_t getCutWeight     ()  { return cutWeight              ; }
    Int_t    getTofFlag       ()  { return tofFlag                ; }
    Bool_t   getDoTargScan    ()  { return (Bool_t) doTargScan    ; }

    Double_t getMinTimeOffset ()  { return minTimeOffset          ; }
    Double_t getMaxTimeOffset ()  { return maxTimeOffset          ; }
    Int_t    getMinCellsNum   ()  { return minCellsNum            ; }

    Bool_t   getChi2CutFlag   ()  { return (Bool_t )chi2CutFlag   ; }
    Double_t getTotalChi2Cut  ()  { return totalChi2Cut           ; }
    Double_t getChi2PerNdfCut ()  { return chi2PerNdfCut          ; }

    Bool_t   getUseTukeyFlag  ()  { return (Bool_t) useTukeyFlag  ; }
    Double_t getCnWs          ()  { return cnWs                   ; }
    Double_t getCn2s          ()  { return cn2s                   ; }
    Double_t getCn4s          ()  { return cn4s                   ; }
    Double_t getMinSig2       ()  { return minSig2                ; }
    Int_t    getMaxNFilterIter()  { return maxNFilterIter         ; }
    Double_t getMinWeight     ()  { return minWeight              ; }
    Double_t getMaxChi2       ()  { return maxChi2                ; }

    Double_t getMinTOffsetIter()  { return minTOffsetIter         ; }

    Double_t getFunCt1        ()  { return funCt1                 ; }
    Double_t getStepD1        ()  { return stepD1                 ; }
    Double_t getFunCt2        ()  { return funCt2                 ; }
    Double_t getStepD2        ()  { return stepD2                 ; }
    Double_t getStepD3        ()  { return stepD3                 ; }

    void setCutWeight     (Double_t cut  ){ cutWeight      = cut  ; }
    void setTofFlag       (Int_t    flag ){ tofFlag        = flag ; }
    void setDoTargScan    (Bool_t   scan ){ doTargScan     = scan ; }

    void setMinTimeOffset (Double_t off  ){ minTimeOffset  = off  ; }
    void setMaxTimeOffset (Double_t off  ){ maxTimeOffset  = off  ; }
    void setMinCellsNum   (Int_t    min  ){ minCellsNum    = min  ; }

    void setChi2CutFlag   (Bool_t   cut  ){ chi2CutFlag    = cut  ; }
    void setTotalChi2Cut  (Double_t cut  ){ totalChi2Cut   = cut  ; }
    void setChi2PerNdfCut (Double_t cut  ){ chi2PerNdfCut  = cut  ; }

    void setUseTukeyFlag  (Bool_t   flag ){ useTukeyFlag   = flag ; }
    void setCnWs          (Double_t cut  ){ cnWs           = cut  ; }
    void setCn2s          (Double_t cut  ){ cn2s           = cut  ; }
    void setCn4s          (Double_t cut  ){ cn4s           = cut  ; }
    void setMinSig2       (Double_t min  ){ minSig2        = min  ; }
    void setMaxNFilterIter(Int_t    max  ){ maxNFilterIter = max  ; }
    void setMinWeight     (Double_t min  ){ minWeight      = min  ; }
    void setMaxChi2       (Double_t max  ){ maxChi2        = max  ; }

    void setMinTOffsetIter(Double_t min  ){ minTOffsetIter = min  ; }

    void setFunCt1        (Double_t val  ){ funCt1         = val  ; }
    void setStepD1        (Double_t val  ){ stepD1         = val  ; }
    void setFunCt2        (Double_t val  ){ funCt2         = val  ; }
    void setStepD2        (Double_t val  ){ stepD2         = val  ; }
    void setStepD3        (Double_t val  ){ stepD3         = val  ; }

    void     putParams(HParamList*);
    Bool_t   getParams(HParamList*);
    void clear();
    void printParam();
    ClassDef(HMdcTrackFitPar,1) // Container for the MDC Digitizer parameters
};
#endif  /*!HMDCTRACKFITPAR_H*/
