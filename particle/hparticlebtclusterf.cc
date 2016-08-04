//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//
//  HParticleBTClusterF
//
//  This class searches for clusters and calculates its properties.
//  Rich hit and fired pad information is obtained from HParticleBTRing
//  class. If at least 1 rich hit is found on fPrediction cluster properties
//  are calculated.
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hparticlebtclusterf.h"
#include "hades.h"
#include "TF2.h"
#include "TH2F.h"
#include "TMath.h"
#include "hparticlebtangletrafo.h"
#include "hruntimedb.h"
#include "hparticlebtpar.h"
#include "hparticlebtringf.h"
#include "hparticlebtringinfo.h"


// ----------------------------------------------------------------

ClassImp(HParticleBtClusterF)

// ----------------------------------------------------------------

HParticleBtClusterF::HParticleBtClusterF( )
{
	fRing = NULL;
}

HParticleBtClusterF::~HParticleBtClusterF(){
    fRing = NULL;
}
void HParticleBtClusterF::setRingF(HParticleBtRingF* ring)
{
    fRing             = ring;
    fPrediction       = &fRing->getPrediction();
    fRichHitAdd       = &fRing->getRichHitAdd();
    fRichHitCharge    = &fRing->getRichHitCharge();
}
Bool_t HParticleBtClusterF::init() {

    fBtPar =(HParticleBtPar*) gHades->getRuntimeDb()->getContainer("ParticleBtPar");
    if(!fBtPar) {
	Error ("init()", "Retrieve ZERO pointer for ParticleBtPar!");
	return kFALSE;
    }

    fPadUp                 = fBtPar->getNeighbourPad(0);
    fPadUpRight            = fBtPar->getNeighbourPad(1);
    fPadRight              = fBtPar->getNeighbourPad(2);
    fPadDownRight          = fBtPar->getNeighbourPad(3);
    fPadDown               = fBtPar->getNeighbourPad(4);
    fPadDownLeft           = fBtPar->getNeighbourPad(5);
    fPadLeft               = fBtPar->getNeighbourPad(6);
    fPadUpLeft             = fBtPar->getNeighbourPad(7);

    memcpy(&fChargeLimit[0]        ,fBtPar->getChargeThres()   , sizeof(fChargeLimit) );
    memcpy(&fChargeLimitMaximum[0] ,fBtPar->getChargeThresMax(), sizeof(fChargeLimitMaximum) );
    fPadWidthX             = fBtPar->getPadWidthX();
    fPadWidthY             = fBtPar->getPadWidthY();

    fNSigma                = fBtPar->getSigmaRange();
    fMaxSigmaRange         = fBtPar->getSigmaRangeMax();
    fMaxSigmaRangeSmall    = fBtPar->getSigmaRangeSmall();
    fSigmaErrorPad         = fBtPar->getSigmaRangePadError();

    fNMaxLimit             = fBtPar->getClusMaximaLimit();
    fClusSizeLimit         = fBtPar->getClusSizeLimit();
    fMinimumSigmaValue     = fBtPar->getMinimumSigmaValue();

    fMaximumType           = fBtPar->getMaximumType();
    memcpy(&fSigmaGaus[0]     , fBtPar->getSigmaGaus()     , sizeof(fSigmaGaus) ) ;
    memcpy(&fSigmaGausRange[0], fBtPar->getSigmaGausRange(), sizeof(fSigmaGausRange) ) ;

    memcpy(&fChargeDiagonalPar[0]  , fBtPar->getSharedChargeDiagonal()  , sizeof(fChargeDiagonalPar) ) ;
    memcpy(&fChargeVerticalPar[0]  , fBtPar->getSharedChargeVertical()  , sizeof(fChargeVerticalPar) ) ;
    memcpy(&fChargeHorizontalPar[0], fBtPar->getSharedChargeHorizontal(), sizeof(fChargeHorizontalPar) ) ;

    memcpy(&fPhiOff[0] , fBtPar->getPhiOffset() , sizeof(fPhiOff ) );
    memcpy(&fPhiOff2[0], fBtPar->getPhiOffset2(), sizeof(fPhiOff2 ) );
    fPhiOffsetPar          = fBtPar->getOffsetPar();

    fNRingSeg              = fBtPar->getNRingSegments();
    fNRingSegStep          = fBtPar->getRingSegStep();
    fNRingSegOffset        = fBtPar->getRingSegOffset();

    fNVertex               = fBtPar->getNVertex();
    fNParMean              = fBtPar->getNParMean();
    fNParSigma             = fBtPar->getNParSigma();
    fParThetaAngleMin      = fBtPar->getParThetaAngleMin();
    fParThetaAngleMax      = fBtPar->getParThetaAngleMax();
   
    fPol2DMean.resize(fNRingSeg);
    fPol2DSigma.resize(fNRingSeg);

    for(Int_t bin=0; bin<fNRingSeg; bin++){
	for(Int_t vertex=0; vertex<fNVertex; vertex++){
	    //Theta,Phi Range, + x: x is half step width of binning used for calculating theses values
	    fPol2DMean[bin].push_back( new TF2(Form("meanPol_%i_v%i",bin,vertex),"x*x*x*[0]+x*x*[1]+x*[2]+y*y*[3]+y*[4]+[5]+y*(x*x*[6]+x*[7])",20.+3.3,86.,0+7.51,60.)); 
	    fPol2DSigma[bin].push_back(new TF2(Form("sigmaPol_%i_v%i",bin,vertex),"TMath::Power(x,3)*[0]+TMath::Power(x,2)*[1]+x*[2]+TMath::Power(y,4)*[3]+TMath::Power(y,3)*[4]+TMath::Power(y,2)*[5]+y*[6]+[7]+y*(x*x*[8]+x*[9]+[10]) + x*(y*y*y*[11]+y*y*[12])",20.+3.3,86,8+7.51,60.));
	    for(Int_t par=0; par<fNParMean; par++)
		fPol2DMean[bin][vertex] ->SetParameter(par, fBtPar->getTF2ParMean( bin, vertex, par));
	    for(Int_t par=0; par<fNParSigma; par++)
		fPol2DSigma[bin][vertex]->SetParameter(par, fBtPar->getTF2ParSigma(bin, vertex, par));
	}
	fRad2Deg.push_back(TMath::DegToRad()*(bin*fNRingSegStep+fNRingSegOffset));
	fRad2DegX.push_back(TMath::Sin(fRad2Deg[bin]));
	fRad2DegY.push_back(-1*TMath::Cos(fRad2Deg[bin]));
    }

    fPrediction   = NULL;
    fRichHitAdd   = NULL;
    fRichHitCharge= NULL;

    return kTRUE;
}

//-----------------------------------------------------------
//Helper functions


void HParticleBtClusterF::addressToColRow(const Int_t address, Int_t &sec, Int_t &row, Int_t &col)
{
    //Converts richCal address to sector, column and row numbers

    sec = (Int_t)address/10000;
    row = (Int_t)(address-sec*10000)/100;
    col = address-sec*10000-row*100;
    sec = sec==6 ? 0 : sec;
}


Float_t HParticleBtClusterF::getPadCharge(const Int_t pos)
{
    //Returns pad charge for given pad address

    Int_t charge = 0;
    for(UInt_t add=0; add < fRichHitAdd->size() && charge==0; add++){
	if((*fRichHitAdd)[add]==pos)
	    charge = (*fRichHitCharge)[add];
    }
    return charge;
}

Int_t HParticleBtClusterF::getShared(const Int_t trackNo,const Int_t clusNum, const Float_t sigmaRange, const Bool_t trackType, const Int_t trackNo2 = -1)
{
    //Returns number of maxima that are inside sigma region of track with trackNo
    //Values are calculated for a single cluster with clusNum
    //Function call sets search area (sigma range) and track type ( good Track, close track with bad quality)
    Int_t counter = 0;
    vector <Int_t> padAdd;
    //Check which maxima are inside sigma region around track with trackNo
    for(UInt_t j = 0; j < fClusTrackNoShared[clusNum].size(); j++){
        if(fClusTrackNoShared[clusNum][j] != trackNo) continue;
	for(UInt_t k = 0; k < fClusChi2[clusNum][j].size(); k++){
	    if(fClusChi2[clusNum][j][k] < sigmaRange)
		padAdd.push_back(fClusNMaximaPad[clusNum][j][k]);
	}
    }
   

    //Search for maxima that are shared with trackNo
    vector <Int_t> counterShared ( padAdd.size(),0 );
    for(UInt_t j = 0; j < fClusTrackNoShared[clusNum].size(); j++){
	if(fClusTrackNoShared[clusNum][j] == trackNo) continue;
    
	if(fRing->isGoodTrack(fClusTrackNoShared[clusNum][j]) != trackType) continue;
        if(trackNo2 != -1 && fClusTrackNoShared[clusNum][j] != trackNo2) continue;
	for(UInt_t k = 0; k < fClusChi2[clusNum][j].size(); k++){
	    if(fClusChi2[clusNum][j][k] < sigmaRange){
		for(UInt_t l = 0 ; l < padAdd.size();  l++){
		    if(padAdd[l] == fClusNMaximaPad[clusNum][j][k]){
			counterShared[l]++;
		    }
		}
	    }
	}
    }

    for(UInt_t i = 0 ; i < counterShared.size();  i++){
	if(counterShared[i] > 0)
	    counter++;
    }
    return counter;
}
Float_t HParticleBtClusterF::getSharedCharge(const Int_t trackNo,const Int_t clusNum, const Float_t sigmaRange, const Bool_t trackType, const Int_t trackNo2 = -1)
{
    //Returns number of charge maxima that are inside sigma region of track with trackNo
    //Values are calculated for a single cluster with clusNum
    //Function call sets search area (sigma range) and track type ( good Track, close track with bad quality)
    Float_t chargeSum = 0;
    vector <Int_t> padAdd;
    //Check which maxima are inside sigma region around track with trackNo
    for(UInt_t j = 0; j < fClusTrackNoShared[clusNum].size(); j++){
        if(fClusTrackNoShared[clusNum][j] != trackNo) continue;
	for(UInt_t k = 0; k < fClusChi2[clusNum][j].size(); k++){
	    if(fClusChi2[clusNum][j][k] < sigmaRange)
		padAdd.push_back(fClusNMaximaPad[clusNum][j][k]);
	}
    }
   

    //Search for maxima that are shared with trackNo
    vector <Int_t> counterShared ( padAdd.size(),0 );
    for(UInt_t j = 0; j < fClusTrackNoShared[clusNum].size(); j++){
	if(fClusTrackNoShared[clusNum][j] == trackNo) continue;
    
	if(fRing->isGoodTrack(fClusTrackNoShared[clusNum][j]) != trackType) continue;
        if(trackNo2 != -1 && fClusTrackNoShared[clusNum][j] != trackNo2) continue;
	for(UInt_t k = 0; k < fClusChi2[clusNum][j].size(); k++){
	    if(fClusChi2[clusNum][j][k] < sigmaRange){
		for(UInt_t l = 0 ; l < padAdd.size();  l++){
		    if(padAdd[l] == fClusNMaximaPad[clusNum][j][k]){
			counterShared[l]++;
		    }
		}
	    }
	}
    }
    for(UInt_t i = 0 ; i < counterShared.size();  i++){
	if(counterShared[i] > 0){
	    chargeSum+=getPadCharge(padAdd[i]);
	}
    }
       
    return chargeSum;
}

void HParticleBtClusterF::addressIsShared(Int_t address)
{
    //Checks if rich pad is inside fPrediction area of additional tracks

    UInt_t vectorPos = fClusTrackNoShared.size()-1;
    for(UInt_t i=0; i<fPrediction->size(); i++){
	for(UInt_t pred=0; pred < (*fPrediction)[i].size(); pred++){
	    if(address == (*fPrediction)[i][pred]){
                if( ( std::find(fClusTrackNoShared[vectorPos].begin(), fClusTrackNoShared[vectorPos].end(), i) ==  fClusTrackNoShared[vectorPos].end() ) )
		    fClusTrackNoShared[vectorPos].push_back(i);
	    }
	}
    }
}


Float_t  HParticleBtClusterF::sharedChargeDiagonal(const Float_t maximumCharge)
{
    //Diagonal charge sharing in dependence of maximum size
    //Determined by quadratic 4 pad clusters

    Int_t charge = maximumCharge * (fChargeDiagonalPar[0]+TMath::Power(maximumCharge*fChargeDiagonalPar[1],fChargeDiagonalPar[2]));
    if(charge<maximumCharge)
       return charge;
    else
        return maximumCharge;
}


Float_t  HParticleBtClusterF::sharedChargeVertical(const Float_t maximumCharge)
{
    //Vertical charge sharing in dependence of maximum size
    //Determined by 2 pad clusters

    if(maximumCharge>fChargeVerticalPar[0])
	return  maximumCharge * ((TMath::Power(maximumCharge-fChargeVerticalPar[0],1./fChargeVerticalPar[2]))/fChargeVerticalPar[1]);
    else
        return 0;
}


Float_t  HParticleBtClusterF::sharedChargeHorizontal(const Float_t maximumCharge)
{
    //Horizontal charge sharing in dependence of maximum size
    //Determined by 2 pad clusters

    if(maximumCharge>fChargeHorizontalPar[0])
	return  maximumCharge * ((TMath::Power(maximumCharge-fChargeHorizontalPar[0],1./fChargeHorizontalPar[2]))/fChargeHorizontalPar[1]);
    else
	return 0;
}



void  HParticleBtClusterF::findNeighbour(const Int_t trackNo,const Int_t address)
{
    //Searches for pads directly connected to initial pad and adds them to the same cluster
    addressIsShared(address);
    fIsInCluster.push_back(address);

    if((std::find(fRichHitAdd->begin(), fRichHitAdd->end(), address+fPadUp) !=  fRichHitAdd->end()) && ((std::find(fIsInCluster.begin(), fIsInCluster.end(), address+fPadUp) ==  fIsInCluster.end()) ))
	findNeighbour(trackNo,address+fPadUp);
    if((std::find(fRichHitAdd->begin(), fRichHitAdd->end(), address+fPadDown) !=  fRichHitAdd->end()) && ((std::find(fIsInCluster.begin(), fIsInCluster.end(), address+fPadDown) ==  fIsInCluster.end())  ))
        findNeighbour(trackNo,address+fPadDown);
    if((std::find(fRichHitAdd->begin(), fRichHitAdd->end(), address+fPadRight) !=  fRichHitAdd->end()) && ((std::find(fIsInCluster.begin(), fIsInCluster.end(), address+fPadRight) ==  fIsInCluster.end())  ))
        findNeighbour(trackNo,address+fPadRight);
    if((std::find(fRichHitAdd->begin(), fRichHitAdd->end(), address+fPadLeft) !=  fRichHitAdd->end()) && ((std::find(fIsInCluster.begin(), fIsInCluster.end(), address+fPadLeft) ==  fIsInCluster.end())   ))
	findNeighbour(trackNo,address+fPadLeft);
}


void HParticleBtClusterF::fillCluster(const Int_t trackNo, const UInt_t beginclus)
{
    //Fills basic cluster information in vectors
    fClusPadSum.push_back(fIsInCluster.size()-beginclus);
    fClusPadRing.push_back(0);
    fClusPosX.push_back(0);
    fClusPosY.push_back(0);
    fClusChargeSum.push_back(0);
    fClusChargeRing.push_back(0);


    UInt_t clusNum = fClusPadSum.size()-1;
    Int_t sec           = -1;
    Int_t padX          = -1;
    Int_t padY          = -1;
    for(UInt_t hit=beginclus; hit < fIsInCluster.size(); hit++){
	for(UInt_t add=0; add < fRichHitAdd->size(); add++){
	    if(fIsInCluster[hit]==(*fRichHitAdd)[add]){
		addressToColRow(fIsInCluster[hit],sec,padX,padY);
		fClusPosX[clusNum]+=padX*(*fRichHitCharge)[add];
		fClusPosY[clusNum]+=padY*(*fRichHitCharge)[add];
                if((*fRichHitCharge)[add]>fChargeLimit[fRing->getTrackSec(trackNo)])
		    fClusChargeSum[clusNum]+=(*fRichHitCharge)[add];
	    }
	}
    }
    fClusPosX[clusNum]=fClusPosX[clusNum]/(Float_t)fClusChargeSum[clusNum];
    fClusPosY[clusNum]=fClusPosY[clusNum]/(Float_t)fClusChargeSum[clusNum];



}


void HParticleBtClusterF::nMaxima(const Int_t trackNo,const UInt_t beginclus)
{
    // Searches for maxima in clusters:
    // Maximum8: All 8 surrounding pads must have measured a smaller charge than maximum pad
    // Maximum4: 4 directly connected pads must have measured a smaller charge than maximum pad
    // Maximum7: 7 directly connected pads must have measured a smaller charge than maximum pad
    //          Adds maxima with smaller charge that are located directly next to maximum
    //          Indicated by larger charge of diagonal pad compared to directly connected pad
    //          of primary maximum


    const Int_t nNeighbours  = 8;
    const Int_t nNeighbours4 = 4;
    const Int_t size         = fIsInCluster.size()-beginclus;
    Int_t neighborAdd[nNeighbours]    = {fPadUp,fPadDown,fPadRight,fPadLeft,fPadUpRight,fPadDownLeft,fPadDownRight,fPadUpRight};
    Int_t vetoPads[nNeighbours4][3]   = {
	{fPadDownRight,fPadDown, fPadRight},
	{fPadUpRight,  fPadRight,fPadDown},
	{fPadUpLeft,   fPadUp,   fPadLeft},
	{fPadDownLeft, fPadLeft, fPadDown}};
    // Only necessary for diagonal pads
    //{{99,-1,100},
    //{101,100,1},
    //{-99,1,-100},
    //{-101,-100,-1}

    vector <Int_t> isPeak7Tmp;
    vector <Int_t> isPeak4Tmp;
    vector <Int_t> isPeak8Tmp;


    vector <Int_t>  vetoPeak;


    UInt_t lastElement     = fClusTrackNoShared.size()-1;
    UInt_t lastElementSize = fClusTrackNoShared[lastElement].size();

    //NMaxima8
    vector <Int_t> peakCounterVec8(lastElementSize);
    vector < vector <Int_t> > peakPadsVec8(lastElementSize);

    //NMaxima4
    vector <Int_t> peakCounterVec4(lastElementSize);
    vector < vector <Int_t> > peakPadsVec4(lastElementSize);

    //NMaxima7
    vector <Int_t> peakCounterVec7(lastElementSize);
    vector < vector <Int_t> > peakPadsVec7(lastElementSize);


    //Searching loop to find Maxima4 and Maxima8
    for(UInt_t hit=beginclus; hit < fIsInCluster.size(); hit++) {
	Bool_t isPeak8 = kTRUE;
	Bool_t isPeak4 = kTRUE;
	Bool_t isPeak  = kTRUE;

	Int_t i=0;

	Int_t posPad = fIsInCluster[hit];
	Float_t chargePad = 0.;
	chargePad = getPadCharge(posPad);

	while(i<nNeighbours && isPeak) {
	    Bool_t  padInCluster   = kFALSE;
	    Int_t   posNeigh       = posPad+neighborAdd[i];
	    Float_t chargeNeigh    = 0.;
	    chargeNeigh = getPadCharge(posNeigh);

	    for( UInt_t hit2 = beginclus; hit2 < fIsInCluster.size(); hit2++ ){
		if(fIsInCluster[hit2]==posNeigh)
		    padInCluster = kTRUE;
	    }
	    if(chargeNeigh>chargePad && padInCluster){
		isPeak8 = kFALSE;
		if(i<nNeighbours4){
		    isPeak4 = kFALSE;
		    isPeak  = kFALSE;
		}
	    }
	    i++;
	}

	if(isPeak8)
	    isPeak8Tmp.push_back(posPad);
	if(isPeak4){
	    isPeak4Tmp.push_back(posPad);
	    isPeak7Tmp.push_back(posPad);
	}

    }

    //Check if second maximum was located next to maximum (Maxima7)
    if(size>3) {
	for( UInt_t peak=0; peak < isPeak4Tmp.size(); peak++ ) {

	    //Each entry in vector stands for different neighbouring pad
	    //Stores address of a possible new maximum pad
	    Int_t posPad[nNeighbours]     = {-1,-1,-1,-1,-1,-1,-1,-1};
	    //Check if at least 7 pads have lower charge than maximum pad
	    Int_t isGoodPeakNeigh[nNeighbours]  = {1,1,1,1,1,1,1,1};
	    //Check if increased charge of diagonal pad is not due to shared charge of diagonal pads
	    Int_t isGoodPeakCharge[nNeighbours] = {1,1,1,1,1,1,1,1};


	    for(Int_t veto=0; veto < nNeighbours4; veto++) {
		Float_t chargeVeto[3] = {0,0,0};
		Int_t kMaximum          = 0;
		Int_t kMaximumNeigh     = 0;
		Int_t kMaximumDiag      = 0;
		//Int_t chargeMaximumNeigh= 0;     // unused
		Int_t chargeMaximumDiag = 0;


		if((std::find(isPeak4Tmp.begin(), isPeak4Tmp.end(), isPeak4Tmp[peak]+vetoPads[veto][0]) !=  isPeak4Tmp.end())) {
		    //Continue if diagonal pad is already maximum
		    kMaximum=kTRUE;
		}
		for(Int_t i=0; i <nNeighbours4; i++){
		    if((std::find(isPeak4Tmp.begin(), isPeak4Tmp.end(), isPeak4Tmp[peak]+vetoPads[veto][0]+neighborAdd[i]) !=  isPeak4Tmp.end())  ) {
			// Check if horizontal or vertical pad are maximum
			kMaximumNeigh++;
			//chargeMaximumNeigh = getPadCharge(isPeak4Tmp[peak]+vetoPads[veto][0]+neighborAdd[i]);   //unused
		    }

		}
		for(Int_t i=4; i <nNeighbours; i++){
		    if((std::find(isPeak4Tmp.begin(), isPeak4Tmp.end(), isPeak4Tmp[peak]+vetoPads[veto][0]+neighborAdd[i]) !=  isPeak4Tmp.end())  ) {
			//Continue if diagonal pads are maximum
			if(isPeak4Tmp[peak]+vetoPads[veto][0]+neighborAdd[i]!=isPeak4Tmp[peak]){
			    kMaximumDiag++;
			    chargeMaximumDiag = getPadCharge(isPeak4Tmp[peak]+vetoPads[veto][0]+neighborAdd[i]);
			}
		    }
		}
		if(kMaximum)
		    continue;
		if(kMaximumNeigh>0)
		    continue;
		if(kMaximumDiag>1)
		    continue;

		//Get charge of diagonal pad [0] and horizontal and vertical pad [1] [2]
		for(UInt_t add=0; add < fRichHitAdd->size(); add++) {
		    if((*fRichHitAdd)[add]==isPeak4Tmp[peak]+vetoPads[veto][0])
			chargeVeto[0] = (*fRichHitCharge)[add];
		    if((*fRichHitAdd)[add]==isPeak4Tmp[peak]+vetoPads[veto][1])
			chargeVeto[1] = (*fRichHitCharge)[add];
		    if((*fRichHitAdd)[add]==isPeak4Tmp[peak]+vetoPads[veto][2])
			chargeVeto[2] = (*fRichHitCharge)[add];
		}

		//If charge of diagonal pads is larger -> second maximum must exist
		//Depending of diagonal pad position and if condition different pad must be maximum
		//   {{99,-1,100},{101,100,1},{-99,1,-100},{-101,-100,-1}
		//{fPadDownRight,fPadDown, fPadRight
		//{fPadUpRight,  fPadRight,fPadDown}
		//{fPadUpLeft,   fPadUp,   fPadLeft}
		//{fPadDownLeft, fPadLeft, fPadDown}
		if(chargeVeto[0]>chargeVeto[1] && chargeVeto[0]<chargeVeto[2] && chargeVeto[2]>fChargeLimitMaximum[fRing->getTrackSec(trackNo)]){    // Bigger than 1 neighbour pad
		    if(vetoPads[veto][0]==fPadDownRight){
			posPad[2]=isPeak4Tmp[peak]+vetoPads[veto][2];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[1]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[2]=-1;
		    }if(vetoPads[veto][0]==fPadUpRight){
			posPad[0]=isPeak4Tmp[peak]+vetoPads[veto][2];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[1]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[0]=-1;
		    }if(vetoPads[veto][0]==fPadUpLeft){
			posPad[3]=isPeak4Tmp[peak]+vetoPads[veto][2];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[1]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[3]=-1;
		    }if(vetoPads[veto][0]==fPadDownLeft){
			posPad[1]=isPeak4Tmp[peak]+vetoPads[veto][2];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[1]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[1]=-1;
		    }
		}else if(chargeVeto[0]>chargeVeto[2] && chargeVeto[0]<chargeVeto[1] && chargeVeto[1]>fChargeLimitMaximum[fRing->getTrackSec(trackNo)]){   // Bigger than 1 neighbour pad
		    if(vetoPads[veto][0]==fPadDownRight){
			posPad[1]=isPeak4Tmp[peak]+vetoPads[veto][1];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[2]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[1]=-1;
		    }if(vetoPads[veto][0]==fPadUpRight){
			posPad[2]=isPeak4Tmp[peak]+vetoPads[veto][1];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[2]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[2]=-1;
		    }if(vetoPads[veto][0]==fPadUpLeft){
			posPad[0]=isPeak4Tmp[peak]+vetoPads[veto][1];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[2]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[0]=-1;
		    }if(vetoPads[veto][0]==fPadDownLeft){
			posPad[3]=isPeak4Tmp[peak]+vetoPads[veto][1];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[2]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[3]=-1;
		    }
		}else if(chargeVeto[0]>chargeVeto[2] && chargeVeto[0]>chargeVeto[1] && chargeVeto[0]>fChargeLimitMaximum[fRing->getTrackSec(trackNo)]) {    // Bigger than both neighbour pads
		    if(vetoPads[veto][0]==fPadDownRight){
			posPad[6]=isPeak4Tmp[peak]+vetoPads[veto][0];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[2]+sharedChargeDiagonal(chargeMaximumDiag)) && chargeVeto[0]<(chargeVeto[1]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[6]=-1;
		    }if(vetoPads[veto][0]==fPadUpRight){
			posPad[4]=isPeak4Tmp[peak]+vetoPads[veto][0];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[2]+sharedChargeDiagonal(chargeMaximumDiag)) && chargeVeto[0]<(chargeVeto[1]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[4]=-1;
		    }if(vetoPads[veto][0]==fPadUpLeft){
			posPad[7]=isPeak4Tmp[peak]+vetoPads[veto][0];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[2]+sharedChargeDiagonal(chargeMaximumDiag)) && chargeVeto[0]<(chargeVeto[1]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[7]=-1;
		    }if(vetoPads[veto][0]==fPadDownLeft){
			posPad[5]=isPeak4Tmp[peak]+vetoPads[veto][0];
			if(kMaximumDiag==1 && chargeVeto[0]<(chargeVeto[2]+sharedChargeDiagonal(chargeMaximumDiag))  && chargeVeto[0]<(chargeVeto[1]+sharedChargeDiagonal(chargeMaximumDiag)))
			    isGoodPeakCharge[5]=-1;
		    }
		}
	    }

	    //Check if 7 pads around possible maximum have smaller charge (only exception should be maximum which was already found
	    for(Int_t veto=0; veto < nNeighbours ; veto++) {
		if(posPad[veto]<0) //Skip if no possible maximum was found
		    continue;
		//Get charge of possible maximum
		Float_t chargePad = getPadCharge(posPad[veto]);

		Int_t i=0;
		while(i<nNeighbours && isGoodPeakNeigh[veto]){
		    Int_t posNeigh      = posPad[veto] + neighborAdd[i];
		    Float_t chargeNeigh = getPadCharge(posNeigh);
		    if(chargeNeigh>chargePad){
			isGoodPeakNeigh[veto]--;
		    }
		    i++;
		}
		Bool_t inCluster = kFALSE;// Check if Maxima pad is in Cluster
		for(UInt_t hit=beginclus; hit < fIsInCluster.size(); hit++){
		    if(posPad[veto]==fIsInCluster[hit])
			inCluster=kTRUE;
		}
		if(inCluster){
		    if(isGoodPeakNeigh[veto]>=0 && isGoodPeakCharge[veto]>0){
			isPeak7Tmp.push_back(posPad[veto]);
		    }
		}
	    }
	}


	//Search for nMaxima7 which are not in nMaxima4 and are located next to each other
	vector < Int_t > tmpErase;
	for(UInt_t peak=0; peak < isPeak7Tmp.size(); peak++) {
	    Int_t posPad1 = isPeak7Tmp[peak];
	    if(std::find(isPeak4Tmp.begin(), isPeak4Tmp.end(), posPad1) !=  isPeak4Tmp.end())
		continue;
	    for(UInt_t peak2=peak+1; peak2 < isPeak7Tmp.size(); peak2++){
		Int_t posPad2 = isPeak7Tmp[peak2];
		if(std::find(isPeak4Tmp.begin(), isPeak4Tmp.end(), posPad2) !=  isPeak4Tmp.end())
		    continue;

		//Get col and row to check if peaks are next to each other
		Int_t sec1;
		Int_t row1;
		Int_t col1;
		Int_t sec2;
		Int_t row2;
		Int_t col2;

		addressToColRow(posPad1, sec1, row1, col1);
		addressToColRow(posPad2, sec2, row2, col2);

		if((TMath::Abs(row1-row2)==1 && (col1-col2)==0 ) || (TMath::Abs(col1-col2)==1 && (row1-row2)==0)){
		    Float_t charge1 =  getPadCharge(posPad1);
		    Float_t charge2 =  getPadCharge(posPad2);
		    if(charge1>charge2)
			tmpErase.push_back(peak2);
		    else if(charge1<charge2)
			tmpErase.push_back(peak);
		}
	    }
	}

	//Remove found max
	for(UInt_t i = 0; i < tmpErase.size(); i++){
	    // once the position i is reached the element to be erased has a position pos - i
	    isPeak7Tmp.erase(isPeak7Tmp.begin()+tmpErase[i]-i);
	}

    }
    //Fill all max, max on fPrediction,max next to fPrediction
    for(UInt_t peak = 0; peak < isPeak7Tmp.size(); peak++) {
	Bool_t isPeak4 = kFALSE;
        Bool_t isPeak8 = kFALSE;
	Bool_t isPeak7 = kFALSE;

	Int_t posPad = isPeak7Tmp[peak];
	if(std::find(isPeak4Tmp.begin(), isPeak4Tmp.end(), posPad) !=  isPeak4Tmp.end())
	    isPeak4 = kTRUE;
        if(std::find(isPeak8Tmp.begin(), isPeak8Tmp.end(), posPad) !=  isPeak8Tmp.end())
	    isPeak8 = kTRUE;
        if(std::find(isPeak7Tmp.begin(), isPeak7Tmp.end(), posPad) !=  isPeak7Tmp.end())
	    isPeak7 = kTRUE;

	for(UInt_t i=0; i< fPrediction->size(); i++){
	    Int_t pos = -1;
	    for(UInt_t j=0; j < fClusTrackNoShared[lastElement].size(); j++) {
		if((UInt_t)fClusTrackNoShared[lastElement][j]==i)
		    pos = j;
	    }
	       
	    if(pos!=-1) {
		if(getPadCharge(posPad)>fChargeLimitMaximum[fRing->getTrackSec(trackNo)]){
		    if(isPeak7){
			peakCounterVec7[pos]++;
			peakPadsVec7[pos].push_back(posPad);
		    }
		    if(isPeak8){
			peakCounterVec8[pos]++;
			peakPadsVec8[pos].push_back(posPad);

		    }
		    if(isPeak4){
			peakCounterVec4[pos]++;
			peakPadsVec4[pos].push_back(posPad);

		    }
		}
	    }
	}
    }
    if( fMaximumType==7 ) {
	fClusNMaxima.push_back(peakCounterVec7);
	fClusNMaximaPad.push_back(peakPadsVec7);
    } else if( fMaximumType == 4 ){
	fClusNMaxima.push_back(peakCounterVec4);
	fClusNMaximaPad.push_back(peakPadsVec4);
    } else if(fMaximumType == 8 ){
	fClusNMaxima.push_back(peakCounterVec8);
	fClusNMaximaPad.push_back(peakPadsVec8);
    } else {
	fClusNMaxima.push_back(peakCounterVec7);
	fClusNMaximaPad.push_back(peakPadsVec7);
    }

}

//Double_t HParticleBtClusterF::gaussf(Double_t *x, Double_t *par)
Double_t gaussf(Double_t *x, Double_t *par)
{
    //Gauss function consisting out of 1 2D gauss per maximum
    Double_t myfval=0.0;
    for(Int_t i=0; i< par[0]; i++){
        myfval += par[1+i*5]*TMath::Gaus(x[0],par[2+i*5],par[3+i*5],kFALSE)*TMath::Gaus(x[1],par[4+i*5],par[5+i*5],kFALSE);
    }
    return myfval;
}

void HParticleBtClusterF::fitMaximaPos(const UInt_t beginclus)
{
    //Fit charge distribution of clusters to determine more precise maxima positions

    UInt_t lastElementClus   = fClusNMaximaPad.size()-1;
    UInt_t lastElementTrack  = fClusNMaximaPad[lastElementClus].size()-1;
    const UInt_t nMaxima     = fClusNMaximaPad[lastElementClus][lastElementTrack].size();
    const UInt_t clusSize    = fIsInCluster.size()-beginclus;
    vector < vector <Float_t> > peakVecX(lastElementTrack+1);
    vector < vector <Float_t> > peakVecY(lastElementTrack+1);



    Int_t secMax[nMaxima];
    Int_t colMax[nMaxima];
    Int_t rowMax[nMaxima];
    Int_t secClus[clusSize];
    Int_t colClus[clusSize];
    Int_t rowClus[clusSize];

    if(nMaxima < fNMaxLimit && nMaxima > 0 && fClusPadSum[lastElementClus] > 1  ) {

	for(UInt_t max=0; max<nMaxima; max++){
	    addressToColRow(fClusNMaximaPad[lastElementClus][lastElementTrack][max],secMax[max],colMax[max],rowMax[max]);
	}

	for(UInt_t hit=beginclus; hit < fIsInCluster.size(); hit++){
	    addressToColRow(fIsInCluster[hit],secClus[hit-beginclus],colClus[hit-beginclus],rowClus[hit-beginclus]);

	}

	Int_t DistCol = 0;
	Int_t DistRow = 0;
	Int_t minCol  = 1000; 
	Int_t minRow  = 1000;

	//Determine maximum distance of pad rows and pad cols
	for(UInt_t hit=0; hit < clusSize; hit++){
	    if(rowClus[hit]<minRow)
		minRow=rowClus[hit];
	    if(colClus[hit]<minCol)
		minCol=colClus[hit];
	    for(UInt_t hit2=hit+1; hit2 < clusSize; hit2++){
		Int_t tmpDistRow =  TMath::Abs(rowClus[hit]-rowClus[hit2])+1;
		Int_t tmpDistCol =  TMath::Abs(colClus[hit]-colClus[hit2])+1;
		if(tmpDistRow>DistRow)
		    DistRow=tmpDistRow;
		if(tmpDistCol>DistCol)
		    DistCol=tmpDistCol;
	    }
	}

	Int_t maxDistRow = DistRow;
	Int_t maxDistCol = DistCol;

        // Creation of the histogram for 2D fitting. TODO check if setting bin error can help...
	TH2F* hCluster  = new TH2F("hCluster","hCluster",maxDistRow,minRow,minRow+maxDistRow,maxDistCol,minCol,minCol+maxDistCol);
	for(UInt_t clus=0; clus< clusSize; clus++){
	    hCluster->SetBinContent(rowClus[clus]-minRow+1,colClus[clus]-minCol+1,getPadCharge(fIsInCluster[clus+beginclus]));
	}

	//Init fit function and set parameters
	TF2 *fGaus     = new TF2("fGaus",gaussf,(Double_t)minRow,(Double_t)(minRow+maxDistRow),(Double_t)minCol,(Double_t)(minCol+maxDistCol),nMaxima*5+1);//TODO init somewhere else
	fGaus->FixParameter(0,fNMaxLimit);
        fGaus->SetLineColor(kBlack);
        //Prepare fit
	for(UInt_t max=0; max < nMaxima; max++){
	    Float_t maximumCharge = getPadCharge(fClusNMaximaPad[lastElementClus][lastElementTrack][max]);
	    fGaus->SetParameter(1+max*5,maximumCharge*1.1);
	    fGaus->SetParLimits(1+max*5,maximumCharge*0.1,maximumCharge*3000);
	    fGaus->SetParameter(2+max*5,rowMax[max]+0.5);
	    fGaus->SetParLimits(2+max*5,rowMax[max],rowMax[max]+1.0);
	    fGaus->SetParameter(3+max*5,fSigmaGaus[0]);
	    fGaus->SetParLimits(3+max*5,fSigmaGaus[0]-fSigmaGausRange[0],fSigmaGaus[0]+fSigmaGausRange[0]);
	    fGaus->SetParameter(4+max*5,colMax[max]+0.5);
	    fGaus->SetParLimits(4+max*5,colMax[max],colMax[max]+1.0);
	    fGaus->SetParameter(5+max*5,fSigmaGaus[1]);
	    fGaus->SetParLimits(5+max*5,fSigmaGaus[1]-fSigmaGausRange[1],fSigmaGaus[1]+fSigmaGausRange[1]);
	}

	hCluster->Fit(fGaus,"QRWW");

        //Get pad Position
	for(UInt_t max=0; max < nMaxima; max++) {
	    for(UInt_t track=0; track <= lastElementTrack; track++){
		peakVecX[track].push_back(fGaus->GetParameter(2+max*5));
		peakVecY[track].push_back(fGaus->GetParameter(4+max*5));
	    }
	}
	delete hCluster;
	delete fGaus;
    }else if(fClusPadSum[lastElementClus]==1 && nMaxima > 0){
	//Set value if cluster consists of one pad.
	Int_t tmpAdd[3];

	addressToColRow(fClusNMaximaPad[lastElementClus][lastElementTrack][0],tmpAdd[0],tmpAdd[1],tmpAdd[2]);
	for(UInt_t track=0; track <= lastElementTrack; track++){
	    peakVecX[track].push_back(tmpAdd[2]+0.5);
	    peakVecY[track].push_back(tmpAdd[1]+0.5);
	}
       
    }else if( nMaxima ==0 ){
	//Cluster had too many maxima or 0
            for(UInt_t track=0; track <= lastElementTrack; track++){
		peakVecX[track].push_back(-1);
		peakVecY[track].push_back(-1);
	    }
  
    }else{
	//Cluster had too many maxima or 0
	for(UInt_t max=0; max < nMaxima; max++){
	    for(UInt_t track=0; track <= lastElementTrack; track++){
		peakVecX[track].push_back(-1);
		peakVecY[track].push_back(-1);
	    }
	}
    }
   
    fClusNMaximaPosX.push_back(peakVecX);
    fClusNMaximaPosY.push_back(peakVecY);

}

void HParticleBtClusterF::calcChi2(const Int_t trackNo)
{
    //Calculates distance between maxima positions and ring predictions for tracks
    //Distance is normalized by sigma of optical distortioan and error of maximum position determination

    UInt_t lastElementClus  = fClusNMaximaPad.size()-1;
    UInt_t lastElementTrack = fClusNMaximaPad[lastElementClus].size();
    const UInt_t nMaxima    = fClusNMaximaPad[lastElementClus][lastElementTrack-1].size();
    vector < vector <Float_t> > vecChi2(lastElementTrack);
    vector < vector <Float_t> > vecChi2XMM(lastElementTrack);
    vector < vector <Float_t> > vecChi2YMM(lastElementTrack);
    vector < vector <Float_t> > vecCircleX(lastElementTrack);
    vector < vector <Float_t> > vecCircleY(lastElementTrack);
    vector < vector <Float_t> > vecSig1X(lastElementTrack);
    vector < vector <Float_t> > vecSig1Y(lastElementTrack);
    vector < vector <Float_t> > vecSig2X(lastElementTrack);
    vector < vector <Float_t> > vecSig2Y(lastElementTrack);


    Float_t chi2XMM = 0;
    Float_t chi2YMM = 0;

    Int_t theta     = 0;
    Int_t phi       = 0;
    Int_t vertexNum = 0;
    Int_t sec       = 0;

    HParticleBtAngleTrafo richAngles;


    for(UInt_t trackNo = 0; trackNo<lastElementTrack; trackNo++){
        //get ring information
	theta        = fRing->getTrackTheta(fClusTrackNoShared[lastElementClus][trackNo] );
	phi          = fRing->getTrackPhi(fClusTrackNoShared[lastElementClus][trackNo]   );
        vertexNum    = fRing->getTrackVertex(fClusTrackNoShared[lastElementClus][trackNo]);
        sec          = fRing->getTrackSec(fClusTrackNoShared[lastElementClus][trackNo]   );
       
    
	Double_t posXCenter = richAngles.xPad2x(fRing->getPosXCenter(fClusTrackNoShared[lastElementClus][trackNo]));
	Double_t posYCenter = richAngles.xPadyPad2y(fRing->getPosXCenter(fClusTrackNoShared[lastElementClus][trackNo]),fRing->getPosYCenter(fClusTrackNoShared[lastElementClus][trackNo]));


	for(UInt_t max=0; max < nMaxima ; max++){

            Double_t padXMax = fClusNMaximaPosX[lastElementClus][trackNo][max];
	    Double_t padYMax = fClusNMaximaPosY[lastElementClus][trackNo][max];

	    if(padXMax<0 || padYMax<0){
		//Bad cluster with too many maxima
		vecChi2[trackNo].push_back(999);
	        vecChi2XMM[trackNo].push_back(999);
		vecChi2YMM[trackNo].push_back(999);
		vecCircleX[trackNo].push_back(999);
		vecCircleY[trackNo].push_back(999);
		vecSig1X[trackNo].push_back(999);
		vecSig1Y[trackNo].push_back(999);
		vecSig2X[trackNo].push_back(999);
		vecSig2Y[trackNo].push_back(999);
	    }else{
		Double_t posXMax = richAngles.xPad2x(padXMax);
		Double_t posYMax = richAngles.xPadyPad2y(padXMax,padYMax);
	      

		Double_t distX = posXMax-posXCenter;
		Double_t distY = posYMax-posYCenter;
		Double_t posXCircle = -1;
		Double_t posYCircle = -1;
		Double_t angle      = -1;
		Int_t    bin        = -1;
		Int_t    bin2       = -1;
		Double_t rest       = -1;
		Double_t radius     = -1;
		Double_t radius2    = -1;
		Double_t sigma      = -1;
		Double_t sigmaXRing = -1;
		Double_t sigmaYRing = -1;
                Double_t sigmaX     = -1;
		Double_t sigmaY     = -1;
                //Double_t sigmaTotal = -1; //unused


		Double_t sigma2  = -1;

	     

                //Use maxima position to calculate corresponding phi angle of ring
		if(distX>=0){
		    if(distY<=0){
			angle = TMath::Abs(TMath::ATan(distX/distY))*TMath::RadToDeg();
		    }else{
			angle = TMath::Abs(TMath::ATan(distY/distX))*TMath::RadToDeg()+90;
		    }
		}else{
		    if(distY>=0)
			angle = TMath::Abs(TMath::ATan(distX/distY))*TMath::RadToDeg()+180;
		    else
			angle = TMath::Abs(TMath::ATan(distY/distX))*TMath::RadToDeg()+270;

		}
                //Get bin corresponding to phi angle
                if(angle>-1){
		    bin  = (Int_t)angle/fNRingSegStep;
		    rest = angle-(bin*fNRingSegStep);
		}
		if(bin!=fNRingSeg-1)
		    bin2=bin+1;
		else
		    bin2=0;

                //Set limits to avoid very small sigma values
		if(theta<fParThetaAngleMin) theta=fParThetaAngleMin;
		if(theta>fParThetaAngleMax) theta=fParThetaAngleMax;

                //Get radius and sigma information
		radius   = fPol2DMean[bin][vertexNum]   -> Eval(theta,-(phi-fPhiOff[sec])+30);
        	radius2  = fPol2DMean[bin2][vertexNum]  -> Eval(theta,-(phi-fPhiOff[sec])+30);
		sigma    = fPol2DSigma[bin][vertexNum]  -> Eval(theta,-(phi-fPhiOff[sec])+30);
		sigma2   = fPol2DSigma[bin2][vertexNum] -> Eval(theta,-(phi-fPhiOff[sec])+30);
		radius     =  (((fNRingSegStep-1)-rest)*radius + rest*radius2)/(fNRingSegStep-1); //weight radius to get better approximation
		sigma      =  (((fNRingSegStep-1)-rest)*sigma + rest*sigma2)/(fNRingSegStep-1);

                //Combine ring width with error of maximum position determination
		sigmaXRing = TMath::Sin(TMath::DegToRad()*angle)*sigma;
		sigmaYRing = (-1)*TMath::Cos(TMath::DegToRad()*angle)*sigma;
		sigmaX     = TMath::Sqrt(sigmaXRing*sigmaXRing+(fSigmaErrorPad*fPadWidthX)*(fSigmaErrorPad*fPadWidthX));
		sigmaY     = TMath::Sqrt(sigmaYRing*sigmaYRing+(fSigmaErrorPad*fPadWidthY)*(fSigmaErrorPad*fPadWidthY));
                //sigmaTotal = TMath::Sqrt(sigmaX*sigmaX+sigmaY+sigmaY);   //unused

                //Calulate most likely position for photon hit
		posXCircle = posXCenter + radius*TMath::Sin(TMath::DegToRad()*angle);
		posYCircle = posYCenter + (-1)*radius*TMath::Cos(TMath::DegToRad()*angle);
		chi2XMM = posXMax-posXCircle;
		chi2YMM = posYMax-posYCircle;

                //Distance between maxima and most likely hit position
		Float_t sigmaPad = TMath::Sqrt(TMath::Power(richAngles.x2xPad(posXCircle)-richAngles.x2xPad(posXCircle+sigmaX),2)
					       +TMath::Power(richAngles.xy2yPad(posXCircle,posYCircle)
							     -richAngles.xy2yPad(posXCircle+sigmaX,posYCircle+sigmaY),2));
		Float_t chi2    = TMath::Sqrt(TMath::Power(richAngles.x2xPad(posXCircle)-richAngles.x2xPad(posXMax),2)
					       +TMath::Power(richAngles.xy2yPad(posXCircle,posYCircle)-richAngles.xy2yPad(posXMax,posYMax),2))
	                                     /sigmaPad;

		vecChi2[trackNo].push_back(chi2);
		vecChi2XMM[trackNo].push_back(chi2XMM);
		vecChi2YMM[trackNo].push_back(chi2YMM);
		vecCircleX[trackNo].push_back(richAngles.x2xPad(posXCircle));
		vecCircleY[trackNo].push_back(richAngles.xy2yPad(posXCircle,posYCircle));

		vecSig1X[trackNo].push_back(richAngles.x2xPad(posXCircle)-TMath::Sin(TMath::DegToRad()*angle)*sigmaPad);
		vecSig1Y[trackNo].push_back(richAngles.xy2yPad(posXCircle,posYCircle)-(-1)*TMath::Cos(TMath::DegToRad()*angle)*sigmaPad);
	        vecSig2X[trackNo].push_back(richAngles.x2xPad(posXCircle)+TMath::Sin(TMath::DegToRad()*angle)*sigmaPad);
		vecSig2Y[trackNo].push_back(richAngles.xy2yPad(posXCircle,posYCircle)+(-1)*TMath::Cos(TMath::DegToRad()*angle)*sigmaPad);
	   

	    }
	}
    }

    fClusChi2.push_back(vecChi2);
    fClusChi2XMM.push_back(vecChi2XMM);
    fClusChi2YMM.push_back(vecChi2YMM);
    fClusCircleX.push_back(vecCircleX);
    fClusCircleY.push_back(vecCircleY);
    fClusSig1X.push_back(vecSig1X);
    fClusSig1Y.push_back(vecSig1Y);
    fClusSig2X.push_back(vecSig2X);
    fClusSig2Y.push_back(vecSig2Y);

}

void HParticleBtClusterF::setGoodClus(){
    UInt_t lastElementClus  = fClusNMaximaPad.size()-1;

    if((UInt_t)fClusPadSum[lastElementClus]>=fClusSizeLimit || (UInt_t)fClusNMaxima[lastElementClus][0]>=fNMaxLimit)
	fClusIsGood.push_back(kFALSE);
    else
	fClusIsGood.push_back(kTRUE);
}

void HParticleBtClusterF::findClusterShape(const UInt_t beginclus)
{
    //Shapes of clusters is detected and marked with number.
    //Only most common shapes are considered
    //Clusters with more than 5 pads are just sorted by size

    const UInt_t size = fIsInCluster.size()-beginclus;
    Int_t sizeCharge = size; //Pads with charge larger 65.0
    Int_t sec[size];
    Int_t row[size];
    Int_t col[size];

    Int_t nNeighboursMax = 0;
    Int_t nBorderMax     = 0;
    Int_t nBorder        = 0;
    Int_t padDist        = 0;
 
    //Detect Maximum
    Int_t   pos = std::find(fRichHitAdd->begin(), fRichHitAdd->end(), fIsInCluster[beginclus]) - fRichHitAdd->begin();
    Float_t maximumCharge = (*fRichHitCharge)[pos];
    Int_t   maximum       = beginclus;
    Float_t minimumCharge = (*fRichHitCharge)[pos];
    Float_t restCharge    = 0;

    for(UInt_t hit=beginclus+1; hit < fIsInCluster.size(); hit++){
	pos = std::find(fRichHitAdd->begin(), fRichHitAdd->end(), fIsInCluster[hit]) - fRichHitAdd->begin();
	if((*fRichHitCharge)[pos]>maximumCharge){
	    maximum       = hit;
	    maximumCharge = (*fRichHitCharge)[pos];
	}
        if((*fRichHitCharge)[pos]<minimumCharge){
	    minimumCharge = (*fRichHitCharge)[pos];
	}
    }
    for(UInt_t hit=beginclus; hit < fIsInCluster.size(); hit++){
	pos = std::find(fRichHitAdd->begin(), fRichHitAdd->end(), fIsInCluster[hit]) - fRichHitAdd->begin();
        restCharge+=(*fRichHitCharge)[pos];
    }
    restCharge -= maximumCharge;
    restCharge -= minimumCharge;

    for(UInt_t hit=beginclus; hit < fIsInCluster.size(); hit++){
	addressToColRow(fIsInCluster[hit], sec[hit-beginclus], row[hit-beginclus], col[hit-beginclus]);
    }

    //Determine number of fired neighbours for fIsInCluster[maximum]
    if(std::find(fIsInCluster.begin(), fIsInCluster.end(),  fIsInCluster[maximum]+fPadUp)    !=  fIsInCluster.end()){
	nNeighboursMax++;
    }if(std::find(fIsInCluster.begin(), fIsInCluster.end(), fIsInCluster[maximum]+fPadDown)  !=  fIsInCluster.end()){
	nNeighboursMax++;
    }if(std::find(fIsInCluster.begin(), fIsInCluster.end(), fIsInCluster[maximum]+fPadRight) !=  fIsInCluster.end()){
	nNeighboursMax++;
    }if(std::find(fIsInCluster.begin(), fIsInCluster.end(), fIsInCluster[maximum]+fPadLeft)  !=  fIsInCluster.end()){
	nNeighboursMax++;
    }

    //Determine length of border in length of pads  for InCluster[maximum]
    if(!(std::find(fIsInCluster.begin(), fIsInCluster.end(),  fIsInCluster[maximum]+fPadUp)   !=  fIsInCluster.end())){
	nBorderMax++;
    }if(!(std::find(fIsInCluster.begin(), fIsInCluster.end(), fIsInCluster[maximum]+fPadDown) !=  fIsInCluster.end())){
	nBorderMax++;
    }if(!(std::find(fIsInCluster.begin(), fIsInCluster.end(), fIsInCluster[maximum]+fPadRight)!=  fIsInCluster.end())){
	nBorderMax++;
    }if(!(std::find(fIsInCluster.begin(), fIsInCluster.end(), fIsInCluster[maximum]+fPadLeft) !=  fIsInCluster.end())){
	nBorderMax++;
    }

    for(UInt_t hit=beginclus; hit < fIsInCluster.size(); hit++) {
	if(!(std::find(fIsInCluster.begin(), fIsInCluster.end(),  fIsInCluster[hit]+fPadUp)   !=  fIsInCluster.end())){
	    nBorder++;
	}if(!(std::find(fIsInCluster.begin(), fIsInCluster.end(), fIsInCluster[hit]+fPadDown) !=  fIsInCluster.end())){
	    nBorder++;
	}if(!(std::find(fIsInCluster.begin(), fIsInCluster.end(), fIsInCluster[hit]+fPadRight)!=  fIsInCluster.end())){
	    nBorder++;
	}if(!(std::find(fIsInCluster.begin(), fIsInCluster.end(), fIsInCluster[hit]+fPadLeft) !=  fIsInCluster.end())){
	    nBorder++;
	}
    }
    //Determine maximum distance of pad rows and pad cols
    for(UInt_t hit=0; hit < size; hit++){
	for(UInt_t hit2=hit+1; hit2 < size; hit2++){
	    Int_t tmpDistRow=  TMath::Abs(row[hit]-row[hit2]);
	    Int_t tmpDistCol=  TMath::Abs(col[hit]-col[hit2]);
            if(tmpDistRow>padDist)
		padDist=tmpDistRow;
	    if(tmpDistCol>padDist)
		padDist=tmpDistCol;
	}
    }
    //Pad clus categorization
    if(sizeCharge==0)
	fClusClass.push_back(0);
    if(sizeCharge==1)
        if(maximumCharge>65)
	    fClusClass.push_back(1);
        else
            fClusClass.push_back(0);
    
    else if(sizeCharge==2){
        if(row[0]==row[1] && minimumCharge<maximumCharge ){
	    fClusClass.push_back(2);
      
	}if(col[0]==col[1] && minimumCharge<maximumCharge){
	    fClusClass.push_back(3);
	}
    }else if(sizeCharge==3){
     
	if((row[0]==row[1] && row[0]==row[2]) || (col[0]==col[1] && col[0]==col[2])){  
    
	    if(nNeighboursMax==2){             //Shape: #x#
                if(row[0]==row[1])
		    fClusClass.push_back(4);
                if(col[0]==col[1])
		    fClusClass.push_back(5);
	    }else{                              //Shape: x##
		fClusClass.push_back(7);
	    }
	}else{
	    if(nNeighboursMax==2){            //Shape:#x
		fClusClass.push_back(6);      //Shape: #
	    }else{                                //Shape: ##
		fClusClass.push_back(8);           //Shape: x
	    }
	}
    }
    else if(sizeCharge==4) {
	if(nNeighboursMax==2 && nBorder==8 && padDist==1){
	    fClusClass.push_back(9);           //Shape:   #x
	}else  if(nNeighboursMax==3 && nBorder==10 && padDist==2){     //    #
	    fClusClass.push_back(10);                                    //   #x#
	}else if (padDist==3)   {                     // ####
	    fClusClass.push_back(11);                  //
	}else{
	    fClusClass.push_back(12);
	}
    }else if(sizeCharge==5){
        Int_t counterRow=0;
        Int_t counterCol=0;
	for(Int_t i=0; i< 5 ; i++){
	    if(row[maximum-beginclus]==row[i])
		counterRow++;
            if(col[maximum-beginclus]==col[i])
		counterCol++;
	}
	if(nBorder==10){
	    if(nBorderMax==2){
	        if(counterCol==3 || counterRow==3)
		    fClusClass.push_back(13);
                else
	            fClusClass.push_back(14);
	    }else
	        fClusClass.push_back(14);
        }else
	    fClusClass.push_back(14);
    }
    else{
	fClusClass.push_back(sizeCharge+9);
    }
}

void HParticleBtClusterF::calcRingShift(const Int_t trackNo)
{
    //Calculates mean maxima position deviation from ring prediction
    //Mean deviation in x,y direction is  stored


    HParticleBtAngleTrafo richAngles;
    //Correct theta and phi angle
    //Float_t theta     = fRing->getTrackTheta(trackNo);   //unused
    //Float_t phi       = fRing->getTrackPhi(trackNo);     //unused
    //Int_t   sec       = fRing->getTrackSec(trackNo);     //unused
    //Int_t   vertexNum = fRing->getTrackVertex(trackNo);  //unused
   

    //Transform angles to x,y Padplane coordinates
    Double_t posXCenter = richAngles.xPad2x(fRing->getPosXCenter(trackNo));
    Double_t posYCenter = richAngles.xPadyPad2y(fRing->getPosXCenter(trackNo),fRing->getPosYCenter(trackNo));

    Float_t meanDistX = 0;
    Float_t meanDistY = 0;
    Float_t meanDistXPad = 0;
    Float_t meanDistYPad = 0;
    Int_t nMaxima     = 0;

   
    //Add deviations in x and y direction
    for(UInt_t clus = 0; clus<fClusNMaximaPad.size(); clus++){
	for(UInt_t track=0; track < fClusNMaximaPad[clus].size();track++){
	    for(UInt_t max=0; max<fClusNMaximaPad[clus][track].size(); max++){
		if(fClusTrackNoShared[clus][track]==trackNo){
		    Double_t padXMax = fClusNMaximaPosX[clus][track][max];
		    Double_t padYMax = fClusNMaximaPosY[clus][track][max];
                    if(padXMax >= 0 || padYMax >= 0){
		        if(fClusChi2[clus][track][max]<fMaxSigmaRange){
			    meanDistX+=fClusChi2XMM[clus][track][max];
			    meanDistY+=fClusChi2YMM[clus][track][max];
			    nMaxima++;
			}
		    }
		}
	    }
	}
    }
    if(nMaxima>0){
	meanDistX = meanDistX/(Float_t)nMaxima;
	meanDistY = meanDistY/(Float_t)nMaxima;
	meanDistXPad = TMath::Abs(richAngles.x2xPad(posXCenter)-richAngles.x2xPad(posXCenter+meanDistX));
	meanDistYPad = TMath::Abs(richAngles.xy2yPad(posXCenter,posYCenter)-richAngles.xy2yPad(posXCenter+meanDistX,posYCenter+meanDistY));

    }else{
	meanDistXPad = 999;
	meanDistYPad = 999;

    }
		  
    fRingMeanDistX.push_back(meanDistXPad);
    fRingMeanDistY.push_back(meanDistYPad);
}



void HParticleBtClusterF::clearCluster()
{
    //Clears vectors that contain cluster information of one event

    fIsInCluster.clear();               
    fRingMeanDistX.clear();
    fRingMeanDistY.clear();
    fClusPadSum.clear();
    fClusPadRing.clear();
    fClusClass.clear();
    fClusPosX.clear();
    fClusPosY.clear();
    fClusChargeSum.clear();
    fClusChargeRing.clear();
  
    for(UInt_t i=0; i<fClusTrackNoShared.size(); i++)
	fClusTrackNoShared[i].clear();
    fClusTrackNoShared.clear();
    for(UInt_t i=0; i<fClusNMaxima.size(); i++){
	fClusNMaxima[i].clear();
	for(UInt_t j=0; j<fClusNMaxima[i].size();j++){
	    fClusNMaximaPad[i][j].clear();
	    fClusNMaximaPosX[i][j].clear();
	    fClusNMaximaPosY[i][j].clear();
	    fClusCircleX[i][j].clear();
	    fClusCircleY[i][j].clear();
	    fClusSig1X[i][j].clear();
	    fClusSig1Y[i][j].clear();
	    fClusSig2X[i][j].clear();
	    fClusSig2Y[i][j].clear();
	    fClusChi2[i][j].clear();
            fClusChi2XMM[i][j].clear();
            fClusChi2YMM[i][j].clear();
        }
	fClusNMaximaPad[i].clear();
        fClusNMaximaPosX[i].clear();
        fClusNMaximaPosY[i].clear();
	fClusCircleX[i].clear();
	fClusCircleY[i].clear();
	fClusSig1X[i].clear();
	fClusSig1Y[i].clear();
	fClusSig2X[i].clear();
	fClusSig2Y[i].clear();
	fClusChi2[i].clear();
        fClusChi2XMM[i].clear();
	fClusChi2YMM[i].clear();

    }
    fClusNMaxima.clear();
    fClusNMaximaPad.clear();
    fClusNMaximaPosX.clear();
    fClusNMaximaPosY.clear();
    fClusCircleX.clear();
    fClusCircleY.clear();
    fClusSig1X.clear();
    fClusSig1Y.clear();
    fClusSig2X.clear();
    fClusSig2Y.clear();
    fClusChi2.clear();
    fClusChi2XMM.clear();
    fClusChi2YMM.clear();

      
}

Int_t HParticleBtClusterF::getPadsRing(const Int_t trackNo)
{
    //Returns number of pads on predicted pads

    if(!fPrediction){
	Error("calcCluster()","fPrediction pionter is NULL, call setRingF() first!");
        exit(1);
    }
   
    Int_t nHits=0;
    for(UInt_t pred=0; pred < (*fPrediction)[trackNo].size(); pred++){
	for(UInt_t hit=0; hit < fRichHitAdd->size(); hit++){
	    if((*fRichHitAdd)[hit] == (*fPrediction)[trackNo][pred]){
	        if((*fRichHitCharge)[hit]>fChargeLimit[fRing->getTrackSec(trackNo)]){
	            nHits++;
	        }
	    }
	}
    }
    return  nHits;
}

Int_t HParticleBtClusterF::getPadsSum(const Int_t trackNo)
{
    //Returns number of pads of clusters which have at least one predicted pad

    Int_t nHits=0;
    for(UInt_t i=0; i<fClusPadSum.size(); i++){
        Bool_t isClusterOfTrack = kFALSE;
	for(UInt_t j=0; j<fClusTrackNoShared[i].size(); j++){
	    if(fClusTrackNoShared[i][j]==trackNo)
		isClusterOfTrack = kTRUE;
	}
	if(fClusIsGood[i] && fClusChargeSum[i] > 0 && isClusterOfTrack)
	    nHits+=fClusPadSum[i];

    }
    return  nHits;
}


Float_t HParticleBtClusterF::getChargeRing(const Int_t trackNo)
{
    //Returns charge of pads that are predicted
    Float_t charge = 0;
    for(UInt_t pred=0; pred < (*fPrediction)[trackNo].size(); pred++){
	for(UInt_t hit=0; hit < fRichHitAdd->size(); hit++){
	    if((*fRichHitAdd)[hit]==(*fPrediction)[trackNo][pred]){
		if((*fRichHitCharge)[hit]>fChargeLimit[fRing->getTrackSec(trackNo)]){
		    charge+=(*fRichHitCharge)[hit];
		}
	    }
	}
    }
    return  charge;
}

Float_t HParticleBtClusterF::getChargeSum(const Int_t trackNo)
{
    //Returns charge of clusters which have at least one predicted pad
    Float_t charge = 0;
    for(UInt_t i=0; i<fClusChargeSum.size(); i++){
	Bool_t isClusterOfTrack = kFALSE;
	for(UInt_t j=0; j<fClusTrackNoShared[i].size(); j++){
	    if(fClusTrackNoShared[i][j]==trackNo)
		isClusterOfTrack = kTRUE;
	}
	if(fClusIsGood[i] && isClusterOfTrack)
	    charge+=fClusChargeSum[i];
    }
    return  charge;
}

Int_t HParticleBtClusterF::getNClusters(const Int_t trackNo)
{
    //Get number of clusters which have at least one predicted pad
    Int_t counter = 0;
    for(UInt_t i=0; i<fClusTrackNoShared.size(); i++){
	if(!fClusIsGood[i] || fClusChargeSum[i] <= 0) continue;
	for(UInt_t j=0; j<fClusTrackNoShared[i].size(); j++){
	    if(fClusTrackNoShared[i][j] == trackNo){
		//Include only clusters where at least 20% is inside TODO CHECK THE OUTPUT OF THIS METHOD
		if((fClusPadRing[i]>=fClusPadSum[i]*0.2 && fClusPadSum[i]>3) || fClusPadSum[i]<=4)
		    counter++;
	    }
	}
    }
    return counter;
}

Int_t HParticleBtClusterF::getMaxima(const Int_t trackNo)
{
    //Return  number of maxima per ring
    Int_t counter=0;
    for(UInt_t i=0; i<fClusTrackNoShared.size(); i++){
	if(!fClusIsGood[i]) continue;
	for(UInt_t j=0; j<fClusTrackNoShared[i].size(); j++){
	    if(fClusTrackNoShared[i][j]==trackNo){
		for(UInt_t k=0; k<fClusChi2[i][j].size(); k++){
		    if(fClusChi2[i][j][k] < fMaxSigmaRange )
			counter++;
		}
	    }
	}
    }
    return counter;
}

Float_t HParticleBtClusterF::getMaximaCharge(const Int_t trackNo)
{
    //Return  charge of maxima per ring
    Float_t counter=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	for(UInt_t j=0; j<fClusChi2[i].size(); j++){
	    if(fClusTrackNoShared[i][j]==trackNo){
		for(UInt_t k=0; k<fClusChi2[i][j].size(); k++){
		    if(fClusChi2[i][j][k]<fMaxSigmaRange && fClusIsGood[i])
			counter+=getPadCharge(fClusNMaximaPad[i][j][k]);
		}
	    }
	}
    }
    return counter;
}

Int_t HParticleBtClusterF::getNearbyMaxima(const Int_t trackNo)
{
    //Return number of maxima per ring which are in restricted sigma area ( fMaxSigmaRangeSmall)

    Int_t counter=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	for(UInt_t j=0; j<fClusChi2[i].size(); j++){
	    if(fClusTrackNoShared[i][j]==trackNo){
		for(UInt_t k=0; k<fClusChi2[i][j].size(); k++){
                    if(fClusChi2[i][j][k]<fMaxSigmaRangeSmall && fClusIsGood[i])   
			counter++;
		}
	    }
	}
    }
    return counter;
}

Float_t HParticleBtClusterF::getChi2Value(const Int_t trackNo)
{
    // Returns chi2 value

    Float_t chi2=0;
    Int_t counter=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	for(UInt_t j=0; j<fClusChi2[i].size(); j++){
	    if(fClusTrackNoShared[i][j]==trackNo){
		for(UInt_t k=0; k<fClusChi2[i][j].size(); k++){
		    if(fClusChi2[i][j][k]<fMaxSigmaRange && fClusIsGood[i]){
			chi2 += TMath::Power(fClusChi2[i][j][k],2);
			counter++;
		    }
		}
	    }
	}
    }
    if (counter==0) return 999.;
    return TMath::Sqrt(chi2)/(Float_t)counter;
}


Float_t HParticleBtClusterF::getChi2ValuePrimary(const Int_t trackNo)
{
    //Returns chi2 value
    //Maxima are only used if distance is closest to current track

    Float_t chi2=0;
    Int_t counter=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	for(UInt_t j=0; j<fClusChi2[i].size(); j++){
	    if(fClusTrackNoShared[i][j]==trackNo){
		for(UInt_t k=0; k<fClusChi2[i][j].size(); k++){
		    if(fClusChi2[i][j][k]<fMaxSigmaRange){
                        Bool_t kPrimary =kTRUE;
			for(UInt_t l=0; l<fClusChi2[i].size(); l++){//loop for checking
			    if(l==j)
				kPrimary =kTRUE;
		            if(fClusChi2[i][l][k]<fClusChi2[i][j][k])// check if maximum  has smaller dist to other track
				kPrimary=kFALSE;
			}
			if(kPrimary && fClusChi2[i][j][k]<fMaxSigmaRange && fClusIsGood[i]){
			    chi2+=TMath::Power(fClusChi2[i][j][k],2);
			    counter++;
			}
		    }
		}
	    }
	}
    }
    if (counter==0) return 999.;
    return TMath::Sqrt(chi2)/(Float_t)counter;
}

Int_t HParticleBtClusterF::getMaximaShared(const Int_t trackNo)
{
    //Returns number of maxima which are inside shared clusters

    Int_t counterShared=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	counterShared += getShared(trackNo,i, fMaxSigmaRange, kTRUE );
    }
    return counterShared;

}

Int_t HParticleBtClusterF::getMaximaSharedTrack(const Int_t trackNo, const Int_t trackNo2)
{
    //Returns number of maxima which are inside a cluster which is shared with track 2

    Int_t counterShared=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	counterShared += getShared(trackNo,i, fMaxSigmaRange, kTRUE, trackNo2 );
   
    }
     return counterShared;
}

Int_t HParticleBtClusterF::getMaximaSharedBad(const Int_t trackNo)
{
    //Returns number of maxima which are inside shared clusters

    Int_t counterShared=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	counterShared += getShared(trackNo,i, fMaxSigmaRange, kFALSE );
    }
    return counterShared;

}

Int_t HParticleBtClusterF::getMaximaSharedBadTrack(const Int_t trackNo, const Int_t trackNo2)
{
    //Returns number of maxima which are inside a cluster which is shared with track 2

    Int_t counterShared=0;

    for(UInt_t i=0; i<fClusChi2.size(); i++){
	counterShared += getShared(trackNo,i, fMaxSigmaRange, kFALSE, trackNo2 );
    }
    return counterShared;
}

Int_t HParticleBtClusterF::getNearbyMaximaShared(const Int_t trackNo)
{
    //Returns number of maxima which are inside shared clusters and inside a small sigma range

    Int_t counterShared=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	counterShared += getShared(trackNo,i, fMaxSigmaRangeSmall, kTRUE );
    }
    return counterShared;
}

Int_t HParticleBtClusterF::getNearbyMaximaSharedTrack(const Int_t trackNo, const Int_t trackNo2)
{
    //Returns number of maxima which are inside a cluster which is shared with track 2 and inside a restricted sigma range of both tracks

    Int_t counterShared=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	counterShared += getShared(trackNo,i, fMaxSigmaRangeSmall, kTRUE, trackNo2 );
    }
    return counterShared;
}

Float_t HParticleBtClusterF::getMaximaChargeShared(const Int_t trackNo)
{
    //Returns charge of maxima pads which are inside shared clusters

    Float_t counterShared=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	counterShared += getSharedCharge(trackNo,i, fMaxSigmaRange, kTRUE);
    }
    return counterShared;
}


Float_t HParticleBtClusterF::getMaximaChargeSharedTrack(const Int_t trackNo, const Int_t trackNo2)
{
    //Returns charge of maxima pads which are inside a cluster which is shared with track 2

    Float_t counterShared=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
        counterShared += getSharedCharge(trackNo,i, fMaxSigmaRange, kTRUE, trackNo2);
  
    }
    return counterShared;
}

Float_t HParticleBtClusterF::getMaximaChargeSharedBad(const Int_t trackNo)
{
    //Returns charge of maxima pads which are inside shared clusters

    Float_t counterShared=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	counterShared += getSharedCharge(trackNo,i, fMaxSigmaRange, kFALSE);
    }
    return counterShared;
}

Float_t HParticleBtClusterF::getMaximaChargeSharedBadTrack(const Int_t trackNo, const Int_t trackNo2)
{
    //Returns charge of maxima pads which are inside a cluster which is shared with track 2

    Float_t counterShared=0;
    for(UInt_t i=0; i<fClusChi2.size(); i++){
	counterShared += getSharedCharge(trackNo,i, fMaxSigmaRange, kFALSE, trackNo2);
    }
    return counterShared;
}

Float_t HParticleBtClusterF::getMeanDistX(const Int_t trackNo)
{
    //Returns mean maxima deviation of maxima from ring prediction in x direction

    return fRingMeanDistX[trackNo];
}

Float_t HParticleBtClusterF::getMeanDistY(const Int_t trackNo)
{
    //Returns mean maxima deviation of maxima from ring prediction in y direction

    return fRingMeanDistY[trackNo];
}

Float_t HParticleBtClusterF::getMeanDist(const Int_t trackNo)
{
    //Returns mean maxima deviation of maxima from ring prediction
    if(fRingMeanDistX[trackNo] < 999)
	return TMath::Sqrt(fRingMeanDistX[trackNo]*fRingMeanDistX[trackNo]+fRingMeanDistY[trackNo]*fRingMeanDistY[trackNo]);
    else
        return 999;
}


Bool_t HParticleBtClusterF::fillRingInfo(HParticleBtRingInfo* btRingInfo)
{

    Bool_t kOverflow = kFALSE;

    Int_t   isInCluster     [1024];   //fired pads

    //Cluster information
    Int_t   clusTrackNo     [32][32];  //clusters, tracks
    Bool_t  clusIsGood      [32];      //clusters
    Int_t   clusPadSum      [32];
    Int_t   clusPadRing     [32];
    Float_t clusChargeSum   [32];
    Float_t clusChargeRing  [32];
    Float_t clusPosX        [32];
    Float_t clusPosY        [32];
    Int_t   clusClass       [32];

    //Maxima and maxima position
    Int_t   clusNMaxima     [32][32];       //clusters,tracks
    Int_t   clusNMaximaPad  [32][32][32];   //clusters,tracks,maxima
    Float_t clusNMaximaPosX [32][32][32];
    Float_t clusNMaximaPosY [32][32][32];

    //Chi2
    Float_t clusChi2        [32][32][32];
    Float_t clusChi2XMM     [32][32][32];
    Float_t clusChi2YMM     [32][32][32];

    //Debug info
    Float_t clusCircleX     [32][32][32];
    Float_t clusCircleY     [32][32][32];
    Float_t clusSig1X       [32][32][32];
    Float_t clusSig1Y       [32][32][32];
    Float_t clusSig2X       [32][32][32];
    Float_t clusSig2Y       [32][32][32];


    //Initialization
    for(Int_t i = 0; i < 1024; i++ ){
	isInCluster[i]   = -1;
    }

    for(Int_t i = 0; i < 32; i++ ){
	for(Int_t j = 0; j < 32; j++ ){
	    clusTrackNo[i][j] = -1;
	}
	clusIsGood[i]     = kTRUE;
	clusPadSum[i]     = -1;
	clusPadRing[i]    = -1;
	clusChargeSum[i]  = -1.;
        clusChargeRing[i]  = -1.;
	clusPosX[i]       = -1.;
	clusPosY[i]       = -1.;
        clusClass[i]      = -1;
    }
    for(Int_t i = 0; i < 32; i++ ){
	for(Int_t j = 0; j < 32; j++ ){
	    clusNMaxima [i][j]  = -1;
	    for(Int_t k = 0; k < 32; k++ ){
	            clusNMaximaPad[i][j][k]     = -1;
		    clusNMaximaPosX[i][j][k]    = -1.;
		    clusNMaximaPosY[i][j][k]    = -1.;

		    clusChi2[i][j][k]           = -1.;
		    clusChi2XMM[i][j][k]        = -1.;
		    clusChi2YMM[i][j][k]        = -1.;

		    clusCircleX[i][j][k]        = -1.;
		    clusCircleY[i][j][k]        = -1.;
		    clusSig1X[i][j][k]          = -1.;
		    clusSig1Y[i][j][k]          = -1.;
		    clusSig2X[i][j][k]          = -1.;
		    clusSig2Y[i][j][k]          = -1.;
	    }
	}
    }

    //Fill vector to array and check if vector size is too large
    if(fIsInCluster.size() <= 1024){
	for(UInt_t i = 0; i < fIsInCluster.size(); i++ ){
	    isInCluster[i]    =  fIsInCluster[i];
	}
    }else
        kOverflow =kTRUE;

    if(fClusTrackNoShared.size() <= 32 ){
	for(UInt_t i = 0; i < fClusTrackNoShared.size(); i++ ){
	    if(fClusTrackNoShared[i].size() <= 32 ){
		for(UInt_t j = 0; j < fClusTrackNoShared[i].size(); j++ ){
		    clusTrackNo[i][j] =  fClusTrackNoShared[i][j];
		}
	    }else
		kOverflow = kTRUE;
	    clusIsGood[i]       = fClusIsGood [i];
	    clusPadSum[i]       = fClusPadSum[i];
	    clusPadRing[i]      = fClusPadRing[i];
	    clusChargeSum[i]    = fClusChargeSum[i];
            clusChargeRing[i]   = fClusChargeRing[i];
	    clusPosX[i]         = fClusPosX[i];
	    clusPosY[i]         = fClusPosY[i];
            clusClass[i]        = fClusClass[i];
        }
    }else
        kOverflow =kTRUE;


    if(fClusNMaximaPad.size() <= 32 ){
	for(UInt_t i = 0; i < fClusNMaximaPad.size(); i++ ){
	    if(fClusNMaximaPad[i].size() <= 32 ){
		for(UInt_t j = 0; j < fClusNMaximaPad[i].size(); j++ ){
		    clusNMaxima[i][j]   = fClusNMaxima[i][j];
		    if(fClusNMaximaPad[i][j].size() <= 32 ){
		        for(UInt_t k = 0; k < fClusNMaximaPad[i][j].size(); k++ ){
			    clusNMaximaPad[i][j][k]   = fClusNMaximaPad[i][j][k];
			    clusNMaximaPosX[i][j][k]  = fClusNMaximaPosX[i][j][k];
			    clusNMaximaPosY[i][j][k]  = fClusNMaximaPosY[i][j][k];

			    clusChi2[i][j][k]         = fClusChi2[i][j][k];
			    clusChi2XMM[i][j][k]      = fClusChi2XMM[i][j][k];
			    clusChi2YMM[i][j][k]      = fClusChi2YMM[i][j][k];

			    clusCircleX[i][j][k]      = fClusCircleX[i][j][k];
			    clusCircleY[i][j][k]      = fClusCircleY[i][j][k];
			    clusSig1X[i][j][k]        = fClusSig1X[i][j][k];
			    clusSig1Y[i][j][k]        = fClusSig1Y[i][j][k];
			    clusSig2X[i][j][k]        = fClusSig2X[i][j][k];
			    clusSig2Y[i][j][k]        = fClusSig2Y[i][j][k];
			}
		    }else
			kOverflow = kTRUE;
		}
	    }else
                kOverflow = kTRUE;
	}
    }else
	kOverflow = kTRUE;
	    
	btRingInfo->setIsInCluster      ( isInCluster    );
        btRingInfo->setClusTrackNo      ( clusTrackNo    );
        btRingInfo->setClusIsGood       ( clusIsGood     );
        btRingInfo->setClusPadSum       ( clusPadSum     );
        btRingInfo->setClusPadRing      ( clusPadRing    );
	btRingInfo->setClusChargeSum    ( clusChargeSum  );
        btRingInfo->setClusChargeRing   ( clusChargeRing );
	btRingInfo->setClusPosX         ( clusPosX       );
        btRingInfo->setClusPosY         ( clusPosY       );
        btRingInfo->setClusClass        ( clusClass      );
       
	btRingInfo->setClusNMaxima      ( clusNMaxima     );
        btRingInfo->setClusNMaximaPad   ( clusNMaximaPad  );
        btRingInfo->setClusNMaximaPosX  ( clusNMaximaPosX );
        btRingInfo->setClusNMaximaPosY  ( clusNMaximaPosY );
       
	btRingInfo->setClusChi2         ( clusChi2      );
        btRingInfo->setClusChi2XMM      ( clusChi2XMM   );
        btRingInfo->setClusChi2YMM      ( clusChi2YMM   );
       

	btRingInfo->setClusCircleX      ( clusCircleX );
        btRingInfo->setClusCircleY      ( clusCircleY );
        btRingInfo->setClusSig1X        ( clusSig1X   );
        btRingInfo->setClusSig1Y        ( clusSig1Y   );
        btRingInfo->setClusSig2X        ( clusSig2X   );
        btRingInfo->setClusSig2Y        ( clusSig2Y   );

	return kOverflow;

}

void HParticleBtClusterF::calcCluster(Int_t trackNo)
{
    // Main method of the HParticleBtClusterF class. It makes all the relevant calculation on the clusters

    if(!fPrediction){
	Error("calcCluster()","fPrediction pionter is NULL, call setRingF() first!");
        exit(1);
    }

    for( UInt_t hit = 0; hit < fRichHitAdd->size(); hit++){
	for( UInt_t pred = 0; pred < (*fPrediction)[trackNo].size(); pred++){
            if( (*fPrediction)[trackNo][pred] == (*fRichHitAdd)[hit] &&  ( std::find(fIsInCluster.begin(), fIsInCluster.end(), (*fPrediction)[trackNo][pred]) ==  fIsInCluster.end() )  ){
		//Hit found first time
	        vector <Int_t> track;
	        track.push_back(trackNo);
		fClusTrackNoShared.push_back(track);    //add new vector for new cluster and fill first trackNo
		UInt_t beginclus = fIsInCluster.size();
		findNeighbour(trackNo,(*fPrediction)[trackNo][pred]);
		fillCluster(trackNo,beginclus);
	        nMaxima(trackNo,beginclus);
	        fitMaximaPos(beginclus);
	        calcChi2(trackNo);
	        setGoodClus();
		findClusterShape(beginclus);
	    }
	}
    }
    calcRingShift(trackNo);
}

void HParticleBtClusterF::noHitFound()
{
    fRingMeanDistX.push_back(999);
    fRingMeanDistY.push_back(999);
   

}
