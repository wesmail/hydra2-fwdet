#include "hparticlemdc.h"
#include "hmdcseg.h"
#include "hmdctrkcand.h"


// ROOT's IO and RTTI stuff is added here
ClassImp(HParticleMdc)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleMdc
//
// helper object keeping additional information for investigation.
// The object is stored in the same order as the HParticleCand objects
////////////////////////////////////////////////////////////////////////////////

HParticleMdc::HParticleMdc() :
    fIndex(0),
    fMdcFlag(0),
    fMdc1x(0),
    fMdc1y(0),
    fMdc2x(0),
    fMdc2y(0),
    fMdc3x(0),
    fMdc3y(0),
    fMdc4x(0),
    fMdc4y(0),
    dedxInner(-1),
    dedxSigmaInner(-1),
    dedxOuter(-1),
    dedxSigmaOuter(-1),
    dedxCombined(-1),
    dedxSigmaCombined(-1),
    dedxNWireCutCombined(0),
    dedxNWireCutInner(0),
    dedxNWireCutOuter(0)
{

    for(Int_t i=0;i<4;i++){
	dedx        [i] = -1;
	dedxSigma   [i] = -1;
	dedxNWire   [i] =  0;
	dedxNWireCut[i] =  0;
    }

}

HParticleMdc::~HParticleMdc()
{


}

Int_t   HParticleMdc::getNRemovedCellsInnerSeg()
{
    // returns the number of wires remove by the fit
    // for innner segment
     return getNCellsClus() - (dedxNWire[0]+dedxNWire[1]);
}


Float_t HParticleMdc::getdedxSeg(Int_t seg)
{
    // returns dedx in
    // combined segment (seg==2)
    // inner seg (seg==0)
    // outer seg (seg==1)
    if(seg==0)     { return dedxInner;}
    else if(seg==1){ return dedxOuter;}
    else if(seg==2){ return dedxCombined;}
    else {return -1;}
}

Float_t HParticleMdc::getSigmadedxSeg(Int_t seg)
{
    // returns sigma dedx in
    // combined segment (seg==2)
    // inner seg (seg==0)
    // outer seg (seg==1)
    if(seg==0)     {return dedxSigmaInner;}
    else if(seg==1){return dedxSigmaOuter;}
    else if(seg==2){return dedxSigmaCombined;}
    else {return -1;}
}

UChar_t HParticleMdc::getNWireCutdedxSeg(Int_t seg)
{
    // returns the number of removed wires in
    // combined segment (seg==2)
    // inner seg (seg==0)
    // outer seg (seg==1)
    if(seg==0)     { return dedxNWireCutInner;}
    else if(seg==1){ return dedxNWireCutOuter;}
    else if(seg==2){ return dedxNWireCutCombined;}
    else {return 0;}
}

void HParticleMdc::fill(HMdcSeg* seg)
{
    // fills all need vairables from HMdcSeg
    // depending if it is inner or outer segment

    if(!seg) return;

    if(seg->getIOSeg() == 0)
    {
	fMdcFlag = seg->getFlag();
	fMdc1x   = seg->getX1Sh();
	fMdc1y   = seg->getY1Sh();
	fMdc2x   = seg->getX2Sh();
	fMdc2y   = seg->getY2Sh();

    } else {
	fMdc3x   = seg->getX1Sh();
	fMdc3y   = seg->getY1Sh();
	fMdc4x   = seg->getX2Sh();
	fMdc4y   = seg->getY2Sh();
    }

}
void HParticleMdc::fill(HMdcTrkCand* trk)
{
    // fills all need variables from HMdcTrkCand
    if(!trk) return;

    dedxInner            = trk->getdedxInnerSeg();
    dedxSigmaInner       = trk->getSigmadedxInnerSeg();
    dedxNWireCutInner    = trk->getNWireCutdedxInnerSeg();
    dedxOuter            = trk->getdedxOuterSeg();
    dedxSigmaOuter       = trk->getSigmadedxOuterSeg();
    dedxNWireCutOuter    = trk->getNWireCutdedxOuterSeg();
    dedxCombined         = trk->getdedxCombinedSeg();
    dedxSigmaCombined    = trk->getSigmadedxCombinedSeg();
    dedxNWireCutCombined = trk->getNWireCutdedxCombinedSeg();

    for(Int_t i=0;i<4;i++){
	dedx        [i] = trk->getdedx(i);
	dedxSigma   [i] = trk->getSigmadedx(i);
	dedxNWire   [i] = trk->getNWirededx(i);
	dedxNWireCut[i] = trk->getNWireCutdedx(i);
    }

}
