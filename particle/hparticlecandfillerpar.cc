//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HParticleCandFillerPar
//
// Container class for parameters of HParticleCandFiller
// The parameters are stored in TArrayD of linearized from
// provided by HHistConverter. The TArrayD parameters will
// be converted to histograms, which are used to obtain the
// values. Because the parameters are stored in TArrayD format
// they can be inspected by the ORACLE web interface and
// standard ascii file output format. For the Foramt description
// see the documentaion of HHistConverter.
//
//  parameters :
//  1-dim RICH-MDC matching cuts per sector as function of momentum [MeV/c]
//
//  phiLow  , phiUp   per sector = lower/upper boundaries of RICH-MDC matching in delta Phi   [deg]
//  thetaLow, thetaUp per sector = lower/upper boundaries of RICH-MDC matching in delta Theta [deg]
//
//  RICH 2-dim correction for Theta as function of event vertex z position
//  1. dim  = z position
//  2. dim  = 8 parameters for polynom
//
// Usage:
//
// Float_t HParticleCandFillerPar::getRichCorr(Float_t zVertex, Float_t thetaRich);
//
// returns the correction for theta which should be added to thetarich (deg).
//
//
// Bool_t  HParticleCandFillerPar::acceptPhiTheta(Int_t s,Float_t mom,Float_t dphi,Float_t dtheta);
//
// check if the values for deltaPhi and deltaTheta, where
// deltaPhi   = (rich.phi   - MDC.phi ) * TMath::Sin(TMath::DegToRad() * MDC.theta) and
// deltaTheta =  rich.theta - MDC.theta,
// are inside the cut.
//
//
////////////////////////////////////////////////////////////////////////////
#include "hparticlecandfillerpar.h"
#include "hpario.h"
#include "hparamlist.h"
#include "hhistconverter.h"

#include "TMath.h"

#include <iostream>
using namespace std;


ClassImp(HParticleCandFillerPar)

HParticleCandFillerPar::HParticleCandFillerPar(const Char_t* name,const Char_t* title,
					       const Char_t* context)
    : HParCond(name,title,context)
{
    //
    clear();
}
HParticleCandFillerPar::~HParticleCandFillerPar()
{
  // destructor
    removeHists();
}

void HParticleCandFillerPar::removeHists(){

    for(Int_t s = 0; s < 6; s ++){
	if(hphiLow  [s]) delete hphiLow  [s];
        if(hphiUp   [s]) delete hphiUp   [s];
	if(hthetaLow[s]) delete hthetaLow[s];
	if(hthetaUp [s]) delete hthetaUp [s];
	hphiLow  [s] = 0;
	hphiUp   [s] = 0;
	hthetaLow[s] = 0;
	hthetaUp [s] = 0;
    }
}
void HParticleCandFillerPar::createHists(){

    for(Int_t s = 0; s < 6; s ++){
        hphiLow  [s] = (TH1D*)HHistConverter::createHist(Form("hphiLowS%i"  ,s),phiLow  [s]);
	hphiUp   [s] = (TH1D*)HHistConverter::createHist(Form("hphiUpS%i"   ,s),phiUp   [s]);
        hthetaLow[s] = (TH1D*)HHistConverter::createHist(Form("hthetaLowS%i",s),thetaLow[s]);
	hthetaUp [s] = (TH1D*)HHistConverter::createHist(Form("hthetaUpS%i" ,s),thetaUp [s]);
    }
}

void HParticleCandFillerPar::clear()
{
    for(Int_t s = 0; s < 6; s ++){
	hphiLow  [s] = 0;
	hphiUp   [s] = 0;
	hthetaLow[s] = 0;
	hthetaUp [s] = 0;
    }

    status=kFALSE;
    resetInputVersions();
    changed=kFALSE;
}
void HParticleCandFillerPar::printParam(void)
{

    cout<<"############################################################"<<endl;
    cout<<"HParticleCandFillerPar:"<<endl;
    for(Int_t s = 0; s < 6; s ++){
         HHistConverter::printArrayInfo(phiLow[s],Form("phiLowS%i",s),10,0);
    }
    for(Int_t s = 0; s < 6; s ++){
         HHistConverter::printArrayInfo(phiUp[s],Form("phiUpS%i",s),10,0);
    }
    for(Int_t s = 0; s < 6; s ++){
         HHistConverter::printArrayInfo(thetaLow[s],Form("thetaLowS%i",s),10,0);
    }
    for(Int_t s = 0; s < 6; s ++){
         HHistConverter::printArrayInfo(thetaUp[s],Form("thetaUpS%i",s),10,0);
    }
    cout<<"zRichCenter = "<<zRichCenter<<endl;

    cout<<"############################################################"<<endl;
}

Bool_t HParticleCandFillerPar::init(HParIo* inp,Int_t* set)
{
    // intitializes the container from an input
  Bool_t rc = HParCond::init(inp,set);
  if (rc && changed){
      // make hists new
      removeHists();
      createHists();
  }

  return rc;
}

void HParticleCandFillerPar::putParams(HParamList* l)
{
    // Puts all params of HParticleCandFillerPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    for(Int_t s = 0; s < 6; s ++){
	l->add(Form("phiLowS%i"  ,s), phiLow[s]);
    }
    for(Int_t s = 0; s < 6; s ++){
	l->add(Form("phiUpS%i"   ,s), phiUp[s]);
    }
    for(Int_t s = 0; s < 6; s ++){
	l->add(Form("thetaLowS%i",s), thetaLow[s]);
    }
    for(Int_t s = 0; s < 6; s ++){
	l->add(Form("thetaUpS%i" ,s), thetaUp[s]);
    }
    l->add("zRichCenter",zRichCenter);

}

Bool_t HParticleCandFillerPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    for(Int_t s = 0; s < 6; s ++){
	if(! (l->fill(Form("phiLowS%i"  ,s), &phiLow[s])   )) return kFALSE;
    }
    for(Int_t s = 0; s < 6; s ++){
	if(! (l->fill(Form("phiUpS%i"   ,s), &phiUp[s])    )) return kFALSE;
    }
    for(Int_t s = 0; s < 6; s ++){
	if(! (l->fill(Form("thetaLowS%i",s), &thetaLow[s]) )) return kFALSE;
    }
    for(Int_t s = 0; s < 6; s ++){
	if(! (l->fill(Form("thetaUpS%i" ,s), &thetaUp[s])  )) return kFALSE;
    }
    if(! l->fill("zRichCenter", &zRichCenter)) return kFALSE;
    return kTRUE;
}

void HParticleCandFillerPar::setPhiLow  (Int_t s, const TArrayD& linData) {
    if(s >= 0 && s < 6) {
	phiLow  [s].Set(linData.GetSize(),linData.GetArray());
	if(hphiLow[s]) delete hphiLow[s];
        hphiLow[s] = (TH1D*) HHistConverter::createHist(Form("hphiLowS%i",s),phiLow[s]);
    }
}
void HParticleCandFillerPar::setPhiUp   (Int_t s, const TArrayD& linData) {
    if(s >= 0 && s < 6) {
	phiUp   [s].Set(linData.GetSize(),linData.GetArray());
	if(hphiUp[s]) delete hphiUp[s];
        hphiUp[s] = (TH1D*) HHistConverter::createHist(Form("hphiUpS%i",s),phiUp[s]);
    }
}
void HParticleCandFillerPar::setThetaLow(Int_t s, const TArrayD& linData) {
    if(s >= 0 && s < 6) {
	thetaLow[s].Set(linData.GetSize(),linData.GetArray());
	if(hthetaLow[s]) delete hthetaLow[s];
        hthetaLow[s] = (TH1D*) HHistConverter::createHist(Form("hthetaLowS%i",s),thetaLow[s]);
    }
}
void HParticleCandFillerPar::setThetaUp (Int_t s, const TArrayD& linData) {
    if(s >= 0 && s < 6) {
	thetaUp [s].Set(linData.GetSize(),linData.GetArray());
	if(hthetaUp[s]) delete hthetaUp[s];
        hthetaUp[s] = (TH1D*) HHistConverter::createHist(Form("hthetaUpS%i",s),thetaUp[s]);
    }
}

Float_t HParticleCandFillerPar::getRichCorr(Float_t zVertex, Float_t thetaRich, Float_t phiRich){
    // returns the correction for theta  which should be added to thetarich (deg)
    const static Float_t dzTarg = 5.0; // Reference target shift relative to nominal Rich-position=0 in mm. Do NOT touch
    const static Float_t zRef = zRichCenter + dzTarg;
    const static Float_t zNorm = 22.5F; // reference for calculating vertex dependence
    const static Float_t thetaRef = 50.0F; // reference for fit of theta dependence of the vertex dependence
    const static Int_t zParMax=6;
    const static Float_t dzThetaPar[zParMax] =
    {-0.820714F,-0.021845F,0.00050965F,2.57468e-5F,1.19138e-7F,-5.83638e-9F};
    const static Float_t dz2ThetaPar[zParMax] =
    {-0.0662483F,-0.00375807F,-9.81675e-6F,3.5676e-6F,1.2312e-8F,-1.99097e-9F};
    // correction of delta Theta with quadratic in phi - phiSectorCenter normalized to phi=16°
    const static Float_t dzThetaPhiPar[2] = {0.0408321F/256.0F, 0.00150907F/256.0F};
    const static Float_t dz2ThetaPhiPar[2] = {0.00107495F/256.0F, 9.0156e-5F/256.0F};

    Float_t dz = (zVertex - zRef)/zNorm;;
    Float_t dz2 = dz*dz;
    Float_t theta0 = thetaRich - thetaRef;
    Float_t dTheta = dzThetaPar[0]*dz + dz2ThetaPar[0]*dz2;
    Float_t thetaPower = 1.0F;
    for (Int_t n=1; n<zParMax; ++n) {
        thetaPower *= theta0;
	dTheta += dzThetaPar[n]*thetaPower*dz + dz2ThetaPar[n]*thetaPower*dz2;
    }
    // phi0 = 0 in the middle of each sector. Range -30° to 30°
    Float_t phi0 = fmod(phiRich,60.0F) - 30.0F;
    dTheta += ((dzThetaPhiPar[0] + dzThetaPhiPar[1]*theta0)*dz +
	       (dz2ThetaPhiPar[0] + dz2ThetaPhiPar[1]*theta0)*dz2)*phi0*phi0;
    return dTheta;
}



Bool_t  HParticleCandFillerPar::acceptPhiTheta(Int_t s,Float_t mom,Float_t dphi,Float_t dtheta){

    // check if the values for deltaPhi and deltaTheta, where
    // deltaPhi   = (rich.phi   - MDC.phi ) * TMath::Sin(TMath::DegToRad() * MDC.theta)
    // deltaTheta =  rich.theta - MDC.theta;
    // are inside the cut.

    if(dtheta < HParticleTool::getInterpolatedValue(hthetaUp [s],mom,kFALSE) &&
       dtheta > HParticleTool::getInterpolatedValue(hthetaLow[s],mom,kFALSE) &&
       dphi   < HParticleTool::getInterpolatedValue(hphiUp   [s],mom,kFALSE) &&
       dphi   > HParticleTool::getInterpolatedValue(hphiLow  [s],mom,kFALSE)
      ) return kTRUE;
    return kFALSE;
}











