#ifndef __HPARTICLEPAIRDRAW_H__
#define __HPARTICLEPAIRDRAW_H__

#include "hparticledef.h"
#include "hparticletool.h"


#include "TH1F.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TLatex.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TObject.h"



#include <vector>
using namespace std;

class HParticleDraw : public TObject {

public:

    Int_t flineWidth;           //!    line with of all lines and ellipses
    Int_t frefColor;            //!    color of reference track (left leg of the pair)
    Int_t fusedColor;           //!    particle sorter flagged cand kIsUsed
    Int_t fleptonColor;         //!    particle sorter flagged cand kIsLepton
    Int_t ffittedStyle;         //!    line style of fitted MDC
    Int_t fnoFittedStyle;       //!    line style of not fitted MDC
    Int_t fmetaColor;           //!    color of meta hit
    Int_t frichColor;           //!    color of rich hit
    Bool_t fuseRICH;            //!    = kTRUE for leptons : draw RICH (see kNoUseRICH flag in eClosePairSelect (hparticledef.h))
    Float_t ftextSizeScale;     //!    scale factor for Title
    Float_t ftextSizeInfoScale; //!    scale factor for info

    HParticleDraw();
    ~HParticleDraw();

    void drawParticle   (Double_t xoff,Double_t yoff, Double_t scx,Double_t scy, UInt_t flag,TString nameCase,TString cuts="");
    void drawParticleNDC(Double_t xoffNDC,Double_t yoffNDC, Double_t heighNDC, UInt_t flag,TString nameCase,TString cuts="");

    ClassDef(HParticleDraw,0)

};

class HParticlePairDraw : public TObject {

public:

    Int_t flineWidth;           //!    line with of all lines and ellipses
    Int_t frefColor;            //!    color of reference track (left leg of the pair)
    Int_t fusedColor;           //!    particle sorter flagged cand2 kIsUsed
    Int_t fleptonColor;         //!    particle sorter flagged cand2 kIsLepton
    Int_t ffittedStyle;         //!    line style of fitted MDC
    Int_t fnoFittedStyle;       //!    line style of not fitted MDC
    Int_t fmetaColor;           //!    color of meta hit
    Int_t frichColor;           //!    color of rich hit
    Bool_t fuseRICH;            //!    = kTRUE for leptons : draw RICH (see kNoUseRICH flag in eClosePairSelect (hparticledef.h))
    Float_t ftextSizeScale;     //!    scale factor for Title
    Float_t ftextSizeInfoScale; //!    scale factor for info

    HParticlePairDraw();
    ~HParticlePairDraw();

    void drawPair   (Double_t xoff,Double_t yoff, Double_t scx,Double_t scy, UInt_t flag,TString nameCase,TString cuts="");
    void drawPairNDC(Double_t xoffNDC,Double_t yoffNDC, Double_t heighNDC,UInt_t flag,TString nameCase,TString cuts="");
    void drawPairCases();
    ClassDef(HParticlePairDraw,0)

};
#endif // __HPARTICLEPAIRDRAW_H__

