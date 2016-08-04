//*-- Author   : Patrick Sellheim, Georgy Kornakov
//*-- Created  : 09/01/2014

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HParticleBtRingF
//
//  This class predicts fired pads based on theta and phi angle of a track.
//  Predicted pads and rich cal hits are stored and used to search for
//  fired pads matching to prediction area. 
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hparticlebtringf.h"
#include "TF2.h"
#include "hcategory.h"
#include "hparticlecand.h"
#include "heventheader.h"

#include "hrichcal.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hcategorymanager.h"
#include "hparticlebtangletrafo.h"



// ----------------------------------------------------------------

ClassImp(HParticleBtRingF)

// ----------------------------------------------------------------

HParticleBtRingF::HParticleBtRingF( ){

}

HParticleBtRingF::~HParticleBtRingF( ){

}


Bool_t HParticleBtRingF::init() {

    fNSector = 6;

    fBtPar =(HParticleBtPar*) gHades->getRuntimeDb()->getContainer("ParticleBtPar");
    if(!fBtPar) {
	Error ("init()", "Retrieve ZERO pointer for ParticleBtPar!");
	return kFALSE;
    }
    //Init constants
    fNVertex                = fBtPar->getNVertex();
    fNParMean               = fBtPar->getNParMean();
    fNParSigma              = fBtPar->getNParSigma();
    fNRingSeg               = fBtPar->getNRingSegments();
    fNRingSegStep           = fBtPar->getRingSegStep();
    fNRingSegOffset         = fBtPar->getRingSegOffset();

    fNRichSeg               = fBtPar->getNRichSeg();
    fRichSegBorderX         = fBtPar->getRichSegBorderX();
    fRichSegBorderY         = fBtPar->getRichSegBorderY();
  
    fThetaAngleMin          = fBtPar->getThetaAngleMin();
    fThetaAngleMax          = fBtPar->getThetaAngleMax();
    fSizeMatrix             = fBtPar->getSizeMatrix();
    memcpy(&fChargeLimit[0]        ,fBtPar->getChargeThres()   , sizeof(fChargeLimit) );
    memcpy(&fChargeLimitMaximum[0] ,fBtPar->getChargeThresMax(), sizeof(fChargeLimitMaximum) );

    fNSigma                 = fBtPar->getSigmaRange();
    fMaxSigmaRange          = fBtPar->getSigmaRangeMax();

    fVertexPosMin           = fBtPar->getVertexPosMin();
    fVertexPosMax           = fBtPar->getVertexPosMax();
    fVertexStep             = fBtPar->getVertexStep();

    memcpy(&fPhiOff[0] , fBtPar->getPhiOffset() , sizeof(fPhiOff ) );
    memcpy(&fPhiOff2[0], fBtPar->getPhiOffset2(), sizeof(fPhiOff2 ) );
    fPhiOffsetPar        = fBtPar->getOffsetPar();


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

    fFiredPads.resize( fNSector , vector<Int_t>( fNRichSeg , 0 ) );

    return kTRUE;
}



//--------------------------------------------------------------------
// -- Helper functions

void HParticleBtRingF::addressToColRow(const Int_t address, Int_t &sec, Int_t &row, Int_t &col)
{
    //Converts richCal address to sector, column and row numbers

    sec = (Int_t)address/10000;
    row = (Int_t)(address-sec*10000)/100;
    col = address-sec*10000-row*100;
    sec = sec==6 ? 0 : sec;
}

Int_t HParticleBtRingF::correctPhi(const Int_t sec,  const Float_t phi)
{
    //Correct phi angle if phi angle disagrees with sector number
    //Agreement is necessary for transformations from angle to RICH pad plane

    if(sec==0){
	if(phi>120)
	    return 120;
	else if(phi<60)
	    return 60;
	else
	    return phi;
    }else if(sec==1){
	if(phi<120)
            return 120;
	else if(phi>180)
	    return 180;
	else
	    return phi;
    }else if(sec==2){
	if(phi<180)
            return 180;
	else if(phi>240)
	    return 240;
	else
	    return phi;
    }else if(sec==3){
	if(phi<240)
	    return 240;
	else if(phi>300)
	    return 300;
	else
	    return phi;
    }else if(sec==4){
	if(phi>360 || (phi>=0 && phi < 60))
            return 360;
	else if(phi<300)
            return 300;
	else
	    return phi;
    }else{  //sec==5
	if(phi>300 || phi<0)
            return 0;
	else if(phi >60 && phi<=300)
	    return 60;
	else
            return phi;
    }
}

void HParticleBtRingF::sortElements(Double_t &entry1 ,Double_t &entry2)
{
    //Sorts 2 Double_t values by size (ascending)
    if(entry1 >entry2){
	Float_t tmp = entry1;
	entry1      = entry2;
        entry2      = tmp;
    }
}

            
Int_t HParticleBtRingF::getVertexNum(const Float_t vertex)
{
    //Returns vertex number to given z-vertex position

    for(Int_t i=0; i < fNVertex;i++){
	if((vertex>-fVertexPosMin+(i*fVertexStep))&& (vertex<-fVertexPosMax+(i*fVertexStep))) return i;
    }
    if(vertex<=fVertexPosMin)
	return 0;
    else //v>-2.5
	return 14;
}



//--------------------------------------------------------------------
// -- Called once per event

void HParticleBtRingF::fillRichCal(HCategory* catRichCal)
{
    //Fills rich cal hit addresses and charges into vectors
    //Bad hits and hits from bad events are exluded

    Int_t sizeRichCal = catRichCal->getEntries();
    HRichCal* richCal;
    Int_t sec;
    Int_t col;
    Int_t row;
    for(Int_t l=0; l < sizeRichCal;l++){
	richCal = HCategoryManager::getObject(richCal,catRichCal,l);
	if(!richCal)
	    continue;
        if(!richCal->getIsCleanedSector() && !richCal->getIsCleanedHigh()){
	    addressToColRow(richCal->getAddress(),sec, row, col);
	    if(richCal->getCharge() > fChargeLimit[sec]){
		fRichHitAdd.push_back(richCal->getAddress());
		fRichHitCharge.push_back(richCal->getCharge());
	    }
	     if(row<fRichSegBorderY){
		 fFiredPads[sec][0]++;
	     }else{
		 if(col<fRichSegBorderX)
		     fFiredPads[sec][1]++;
		 else
		     fFiredPads[sec][2]++;
	     }
        }
    }
}

void HParticleBtRingF::clearData()
{
    //Empties rich and track vectors

    for(UInt_t i = 0; i < fPrediction.size() ; i++ )
	fPrediction[i].clear();
    fPrediction.clear();
    for(UInt_t i = 0; i < fRingMatrix.size() ; i++ )
	fRingMatrix[i].clear();
    fRingMatrix.clear();

    fRichHitAdd.clear();
    fRichHitCharge.clear();
    fTrackTheta.clear();
    fTrackPhi.clear();
    fTrackVertex.clear();
    fTrackSec.clear();
    fTrackPCandIdx.clear();
    fIsGoodTrack.clear();
    fPosXCenter.clear();
    fPosYCenter.clear();

    for(Int_t i = 0; i < fNSector ; i++ ){
	for(Int_t j = 0; j < fNRichSeg ; j++ ){
	    fFiredPads[i][j]=0;
	}
    }

}


//--------------------------------------------------------------------
//  -- Called once per track

//Marks pads in area around ring center
void HParticleBtRingF::fillMatrix(Int_t xPad, Int_t yPad, Int_t sec)
{
    //Marks area around ring center
    //Address are stored in ringMatrix vector

    Int_t address    = 0;

    vector <Int_t> ringMatrixVec;
    xPad -= fSizeMatrix*0.5;
    yPad -= fSizeMatrix*0.5;
    address = 10000 * (sec ? sec : 6) + 100 * yPad + xPad;

  
    for(Int_t posX=0; posX <fSizeMatrix; posX++){
	for(Int_t posY=0; posY <fSizeMatrix; posY++){
	    if( TMath::Sqrt( TMath::Power(TMath::Abs(fSizeMatrix*0.5-posX),2) + TMath::Power(TMath::Abs(fSizeMatrix*0.5-posY),2) ) < (Float_t)fSizeMatrix/2. ){
		ringMatrixVec.push_back(address+posY*100+posX);
	    }
	}
    }
    fRingMatrix.push_back(ringMatrixVec);
}


void HParticleBtRingF::fillPrediction(const HParticleCand* cand, HVertex &vertex, Bool_t isGoodTrack, const Bool_t doAngleCorr)
{
    //Theta and phi angles and vertex position is used to determine track position on pad plane
    //Information about optical ring distortion is used to predict fired pads in defined sigma area
    //Mean values with neighbouring sectors assure a more smooth ring prediction
    //Predicted pads are stored in prediction vector

    HParticleBtAngleTrafo richAngles;

    Int_t address = 0;
    Double_t theta        = cand->getTheta();
    Double_t phi          = cand->getPhi();
    Int_t   sec           = cand->getSector();
    Float_t candZ         = cand->getZ();
    Float_t vertexZ       = vertex.getZ();
    Int_t   vertexNum;
    Double_t radius;
    Double_t sigma;
    Double_t sigmaX0;
    Double_t sigmaY0;
    Double_t radiusSeg0;
    Double_t sigmaSeg0;
    Double_t sigmaX0Seg0;
    Double_t sigmaY0Seg0;
    Double_t radiusNext;
    Double_t sigmaNext;
    Double_t sigmaX0Next;
    Double_t sigmaY0Next;
    Double_t sigNum;
    vector <Int_t> predictionVec;

    phi = correctPhi(sec,phi);

    if(theta>fThetaAngleMin && theta < fThetaAngleMax)
    {
	//Cases if no vertex was reconstructed
	if(vertexZ > -80. && vertexZ < 20.){
	    vertexNum     = getVertexNum(vertexZ);
	}else if(candZ > -80. && candZ < 20.){
	    vertexZ       = candZ;
	    vertexNum     = getVertexNum(candZ);
	}else if(candZ <= -80.){
	    vertexZ       = -80.;
	    vertexNum     = getVertexNum(-80.);
	}else{
	    vertexZ       = 20.;
	    vertexNum     = getVertexNum(20.);
	}

     

	//Correct theta and phi angle
	Double_t thetaCor;
	Double_t phiCor;
	if(doAngleCorr){
	    if(fAngleCor.alignRichRing(theta,phi, thetaCor, phiCor)){
		theta = thetaCor;
		phi   = phiCor;
	    }
	}

	thetaCor = richAngles.zTheta2dTheta(vertexZ,theta, -(phi-fPhiOff[sec]));
	theta-=thetaCor;



	//Transform angles to x,y Padplane coordinates
	Double_t xPad  = richAngles.angles2xPad(theta,-(phi-fPhiOff[sec]));
	Double_t yPad  = richAngles.angles2yPad(theta,-(phi-fPhiOff[sec]));
	Double_t posX  = richAngles.angles2x(theta,-(phi-fPhiOff[sec]));
	Double_t posY  = richAngles.angles2y(theta,-(phi-fPhiOff[sec]));

       
	//z vertex correction
	Double_t yPadCorr = richAngles.zTheta2dYPad(vertexZ,theta,-(phi-fPhiOff2[sec]));
	Double_t xPadCorr = richAngles.zTheta2dXPad();
	Double_t posXCorr = richAngles.zTheta2dY(vertexZ,theta,-(phi-fPhiOff2[sec]));
	Double_t posYCorr = richAngles.zTheta2dX();
	posX += posXCorr;
	posY += posYCorr;
	xPad += xPadCorr;
	yPad += yPadCorr;


	//Coordinates for n sigma areas
	Double_t sigmaX[2]     = {0.,0.};
	Double_t sigmaY[2]     = {0.,0.};
	Double_t padX[2]       = {0.,0.};
	Double_t padY[2]       = {0.,0.};
	Int_t   padXRound[2]   = {0,0};
	Int_t   padYRound[2]   = {0,0};

	sigNum=fNSigma;

        //Loop over different phi angles of the ring
        for( Int_t bin = 0; bin < fNRingSeg; bin++ ) {
	  
            //Find neighbouring segments
	    Int_t binPrev;
            Int_t binNext;
	    if(bin==0){
                binPrev = fNRingSeg-1;
		binNext = 2;
	    }
            else if(bin==fNRingSeg-1){
                binPrev = fNRingSeg-2;
		binNext = 0;
	    }
	    else{
                binPrev = bin-1;
                binNext = bin+1;
	    }
	    radius  = fPol2DMean[bin][vertexNum]->Eval(theta,-(phi-fPhiOff[sec])+fPhiOffsetPar);
	    sigma   = fPol2DSigma[bin][vertexNum]->Eval(theta,-(phi-fPhiOff[sec])+fPhiOffsetPar);
	    sigmaX0 = fRad2DegX[bin]*sigNum*sigma;
	    sigmaY0 = fRad2DegY[bin]*sigNum*sigma;


	    //Smooth ring prediction area width neighbour sigma and radius
	    radiusSeg0  = fPol2DMean[binPrev][vertexNum]->Eval(theta,-(phi-fPhiOff[sec])+fPhiOffsetPar);
	    sigmaSeg0   = fPol2DSigma[binPrev][vertexNum]->Eval(theta,-(phi-fPhiOff[sec])+fPhiOffsetPar);
	    sigmaX0Seg0 = fRad2DegX[binPrev]*sigNum*sigmaSeg0;
	    sigmaY0Seg0 = fRad2DegY[binPrev]*sigNum*sigmaSeg0;
           
	    radiusNext  = fPol2DMean[binNext][vertexNum]->Eval(theta,-(phi-fPhiOff[sec])+fPhiOffsetPar);
	    sigmaNext   = fPol2DSigma[binNext][vertexNum]->Eval(theta,-(phi-fPhiOff[sec])+fPhiOffsetPar);
	    sigmaX0Next = fRad2DegX[binNext]*sigNum*sigmaNext;
	    sigmaY0Next = fRad2DegY[binNext]*sigNum*sigmaNext;

	   
	    radius  = (radius + radiusSeg0  + radiusNext  )/3.;
	    if(sigma<sigmaSeg0){
		sigma   = (sigma  + sigmaSeg0   + sigmaNext   )/3.;
		sigmaX0 = (sigmaX0+ sigmaX0Seg0 + sigmaX0Next )/3.;
		sigmaY0 = (sigmaY0+ sigmaY0Seg0 + sigmaY0Next )/3.;
	    }
       
	    //Smooth Overlap region
	    if(bin==fNRingSeg-1 || bin==fNRingSeg-2 || bin==fNRingSeg-3){
                radiusSeg0  = fPol2DMean[0][vertexNum]->Eval(theta,-(phi-fPhiOff[sec])+fPhiOffsetPar);
		sigmaSeg0   = fPol2DSigma[0][vertexNum]->Eval(theta,-(phi-fPhiOff[sec])+fPhiOffsetPar);
		sigmaX0Seg0 = fRad2DegX[0]*sigNum*sigmaSeg0;
		sigmaY0Seg0 = fRad2DegY[0]*sigNum*sigmaSeg0;

		radius  = (radius + radiusSeg0 )/2.;
		if(sigma<sigmaSeg0){
		    sigma   = (sigma  + sigmaSeg0  )/2.;
		    sigmaX0 = (sigmaX0+ sigmaX0Seg0)/2.;
		    sigmaY0 = (sigmaY0+ sigmaY0Seg0)/2.;
		    sigma   = sigmaSeg0  ;
		    sigmaX0 = sigmaX0Seg0;
		    sigmaY0 = sigmaY0Seg0;
		}
	    }
	    if(bin==0 || bin==1 || bin==2){
                radiusSeg0  = fPol2DMean[fNRingSeg-1][vertexNum]->Eval(theta,-(phi-fPhiOff[sec])+fPhiOffsetPar);
		sigmaSeg0   = fPol2DSigma[fNRingSeg-1][vertexNum]->Eval(theta,-(phi-fPhiOff[sec])+fPhiOffsetPar);
		sigmaX0Seg0 = fRad2DegX[fNRingSeg-1]*sigNum*sigmaSeg0;
		sigmaY0Seg0 = fRad2DegY[fNRingSeg-1]*sigNum*sigmaSeg0;

		radius  = (radius + radiusSeg0 )/2.;
		if(sigma<sigmaSeg0){
		    sigma   = (sigma  + sigmaSeg0  )/2.;
		    sigmaX0 = (sigmaX0+ sigmaX0Seg0)/2.;
		    sigmaY0 = (sigmaY0+ sigmaY0Seg0)/2.;
		    sigma   = sigmaSeg0  ;
	            sigmaX0 = sigmaX0Seg0;
		    sigmaY0 = sigmaY0Seg0;
		}
	    }


	    sigmaX[0] = fRad2DegX[bin]*radius-sigmaX0;
	    sigmaY[0] = fRad2DegY[bin]*radius-sigmaY0;
	    sigmaX[1] = fRad2DegX[bin]*radius+sigmaX0;
	    sigmaY[1] = fRad2DegY[bin]*radius+sigmaY0;
	    sortElements(sigmaX[0],sigmaX[1]);
	    sortElements(sigmaY[0],sigmaY[1]);
	    padX[0]   = richAngles.x2xPad(posX+sigmaX[0]);
	    padY[0]   = richAngles.xy2yPad(posX+sigmaX[0],posY+sigmaY[0]);
	    padX[1]   = richAngles.x2xPad(posX+sigmaX[1]);
	    padY[1]   = richAngles.xy2yPad(posX+sigmaX[1],posY+sigmaY[1]);



	    padXRound[0] = TMath::Floor(padX[0]);
	    padXRound[1] = TMath::Floor(padX[1]);
	    padYRound[0] = TMath::Floor(padY[0]);
	    padYRound[1] = TMath::Floor(padY[1]);
	
            //Convert pad position in address and store address in vector
	    Int_t padY0=padY[0];
	    while(padXRound[0] <= padXRound[1]){
		
		while(padYRound[0] <= padYRound[1]){
		    address = 10000 * (sec ? sec : 6) + 100 * padYRound[0] + padXRound[0];

		    //Fill only if object is not yet stored in vector
		    Bool_t kFound=kFALSE;
		    UInt_t pred=0;
		    while(pred < predictionVec.size() && !kFound){
			if(predictionVec[pred] == address)
			    kFound=kTRUE;
			pred++;

		    }
		    if(!kFound)
			predictionVec.push_back(address);

		    padYRound[0]++;
		}
		padYRound[0]=padY0;
		padXRound[0]++;
	    }
	}
        fPrediction.push_back(predictionVec);
	fTrackTheta.push_back(theta);
	fTrackPhi.push_back(phi);
	fTrackVertex.push_back(vertexNum);
	fTrackSec.push_back(sec);
	fTrackPCandIdx.push_back(cand->getIndex());
        fIsGoodTrack.push_back(isGoodTrack);
	fPosXCenter.push_back(xPad);
	fPosYCenter.push_back(yPad);

	fillMatrix(TMath::Floor(xPad),TMath::Floor(yPad),sec);
  
    } else {
	predictionVec.clear();
        predictionVec.push_back(-1);
	fPrediction.push_back(predictionVec);
	fTrackTheta.push_back(-1);
	fTrackPhi.push_back(-1);
	fTrackVertex.push_back(-1);
	fTrackSec.push_back(-1);
	fTrackPCandIdx.push_back(-1);
        fIsGoodTrack.push_back(kFALSE);
	fRingMatrix.push_back(predictionVec);
	fPosXCenter.push_back(-1.);
	fPosYCenter.push_back(-1.);

    }

}



Float_t HParticleBtRingF::getRingMatrix(const Int_t trackNo)
{
    //Returns fraction of  pads fired on ring prediction compared to pads fired in ring matrix around ring center

    Int_t padsFired     = 0;
    Int_t padsFiredPred = 0;
    Float_t chargeFired     = 0;
    Float_t chargeFiredPred = 0;
 
    for(UInt_t i=0; i<fRingMatrix[trackNo].size(); i++){
	for(UInt_t j=0; j<fRichHitAdd.size(); j++){
	    if(fRingMatrix[trackNo][i]==fRichHitAdd[j]){
		padsFired++;
                chargeFired +=fRichHitCharge[j];
		for(UInt_t k=0; k<fPrediction[trackNo].size(); k++){
		    if(fRingMatrix[trackNo][i]==fPrediction[trackNo][k]){
			padsFiredPred++;
			chargeFiredPred +=fRichHitCharge[j];
		    }
		}
	    }
	}
    }
    return (Float_t)chargeFiredPred/(Float_t)chargeFired;
}

Float_t HParticleBtRingF::getTrackTheta(Int_t trackNo)
{
    //Returns theta angle for given trackNo
    if(trackNo<(Int_t)fTrackTheta.size()) {
	return fTrackTheta[trackNo];
    } else return -1;
}

Float_t HParticleBtRingF::getTrackPhi(Int_t trackNo)
{
    //Returns phi angle for given trackNo
    if(trackNo<(Int_t)fTrackPhi.size()) {
	return fTrackPhi[trackNo];
    } else return -1;
}

Int_t HParticleBtRingF::getTrackVertex(Int_t trackNo)
{
    //Returns vertex number for given trackNo
    if(trackNo<(Int_t)fTrackVertex.size()) {
	return fTrackVertex[trackNo];
    } else return -1;
}

Int_t HParticleBtRingF::getTrackSec(Int_t trackNo)
{
    //Returns sector for given trackNo
    if(trackNo<(Int_t)fTrackSec.size()) {
	return fTrackSec[trackNo];
    } else return -1;
}

Float_t HParticleBtRingF::getPosXCenter(Int_t trackNo)
{
    //Returns phi angle for given trackNo
    if(trackNo<(Int_t)fPosXCenter.size()) {
	return fPosXCenter[trackNo];
    } else return -1;
}

Float_t HParticleBtRingF::getPosYCenter(Int_t trackNo)
{
    //Returns phi angle for given trackNo
    if(trackNo<(Int_t)fPosYCenter.size()) {
	return fPosYCenter[trackNo];
    } else return -1;
}

Bool_t HParticleBtRingF::isGoodTrack(Int_t trackNo)
{
    //Returns sector for given trackNo
    if(trackNo<(Int_t)fTrackSec.size()) {
	return fIsGoodTrack[trackNo];
    } else return 0;
}

Bool_t HParticleBtRingF::fillRingInfo(HParticleBtRingInfo* btRingInfo)
{
    //Stores ring,track and rich information in arrays
    //Arrays are stored in HParticleBtRingFInfo for output
    Bool_t kOverflow = kFALSE;

    Int_t   prediction      [128][128]; //tracks,predicted pads
    Int_t   ringMatrix      [128][128];
    Int_t   richHitAdd      [1024];    //fired pads
    Float_t richHitCharge   [1024];

    Float_t trackTheta    [128];
    Float_t trackPhi      [128];
    Int_t trackVertex     [128];
    Int_t trackSec        [128];
    Int_t trackPCandIdx   [128];
    Bool_t  isGoodTrack   [128];
    Float_t posXCenter    [128];
    Float_t posYCenter    [128];


    //Initialization
    for(Int_t i = 0; i < 128; i++ ){
	for(Int_t j = 0; j < 128; j++ ){
	    prediction[i][j]  = -1;
	    ringMatrix[i][j]  = -1;
	}
    }

    for(Int_t i = 0; i < 1024; i++ ){
	richHitAdd[i]    = -1;
	richHitCharge[i] = -1.;
    }

    for(Int_t i = 0; i < 128; i++ ){
	trackTheta[i]    = -1.;
	trackPhi[i]      = -1.;
	trackVertex[i]   = -1;
	trackSec[i]      = -1;
	trackPCandIdx[i] = -1;
	isGoodTrack[i]   = kFALSE;
	posXCenter[i]    = -1.;
	posYCenter[i]    = -1.;
    }

    //Fill vector to array and check if vector size is too large
    if(fPrediction.size() <= 128){
	for(UInt_t i = 0; i < fPrediction.size(); i++ ){
	    if(fPrediction[i].size() <= 128){
		for(UInt_t j = 0; j < fPrediction[i].size(); j++ ){
		    prediction[i][j] = fPrediction[i][j];
		    ringMatrix[i][j] = fRingMatrix[i][j];
		}
	    }
	    else
		kOverflow = kTRUE;
	}
    }else
        kOverflow = kTRUE;


    if(fRichHitAdd.size() <= 1024){
	for(UInt_t i = 0; i < fRichHitAdd.size(); i++ ){
	    richHitAdd[i]     =  fRichHitAdd[i];
	    richHitCharge[i]  =  fRichHitCharge[i];
	}
    }else
	kOverflow =kTRUE;
    if(fTrackTheta.size()<=128){
	for(UInt_t i = 0; i < fTrackTheta.size(); i++ ){
	    trackTheta[i]    =  fTrackTheta[i];
	    trackPhi[i]      =  fTrackPhi[i];
	    trackVertex[i]   =  fTrackVertex[i];
	    trackSec[i]      =  fTrackSec[i];
	    trackPCandIdx[i] =  fTrackPCandIdx[i];
	    isGoodTrack[i]   =  fIsGoodTrack[i];
	    posXCenter[i]    =  fPosXCenter[i];
	    posYCenter[i]    =  fPosYCenter[i];
	}
    }else
	kOverflow =kTRUE;

	btRingInfo->setPrediction       ( prediction   );
        btRingInfo->setRingMatrix       ( ringMatrix   );
        btRingInfo->setRichHitAdd       ( richHitAdd   );
        btRingInfo->setRichHitCharge    ( richHitCharge);
      
	btRingInfo->setTrackTheta       ( trackTheta   );
	btRingInfo->setTrackPhi         ( trackPhi     );
	btRingInfo->setTrackVertex      ( trackVertex  );
	btRingInfo->setTrackSec         ( trackSec     );
	btRingInfo->setTrackPCandIdx    ( trackPCandIdx);
	btRingInfo->setIsGoodTrack      ( isGoodTrack  );
        btRingInfo->setPosXCenter       ( posXCenter   );
	btRingInfo->setPosYCenter       ( posYCenter   );

     	return kOverflow;

}

Int_t HParticleBtRingF::plotPrediction(Int_t trackNo = -1)
{
    Int_t returnVal = 0 ;
    for(UInt_t j = 0; j < fPrediction[trackNo].size(); j++ ){
	Bool_t hasHit = kFALSE;
	for(UInt_t k = 0; k <  fRichHitAdd.size(); k++ ){
	    if(fRichHitAdd[k] == fPrediction[trackNo][j]){
		hasHit = kTRUE;
		returnVal++;
	    }
	}
	cout << fPrediction[trackNo][j] <<  " - " <<"["<< hasHit << "]"  << endl;
    }

return returnVal;
}


void HParticleBtRingF::plotRichHit(Int_t trackNo = -1)
{
    Int_t sec[2];
    Int_t row[2];
    Int_t col[2];
    Int_t plotCounter = 0 ;
    for(UInt_t i = 0; i < fRichHitAdd.size(); i++ ){
	addressToColRow(fRichHitAdd[i]         ,sec[0], row[0], col[0]);
	addressToColRow(fPrediction[trackNo][1],sec[1], row[1], col[1]);
	if(sec[0] == sec[1]){
	    cout << fRichHitAdd[i] << " --  " ;
            plotCounter++;
	    if(plotCounter%5 == 0)
		cout << endl;
	}
    }
     
}



Bool_t HParticleBtRingF::hasNoisyRichSeg(Bool_t *trackInSec)
{
    Bool_t secUsed[6];
    for(Int_t i=0;i<6;i++) secUsed[0+i] = trackInSec[i];
    Bool_t isNoisy = kFALSE;
    for(Int_t i = 0;  i < fNSector;  i++){
	for(Int_t j = 0;  j < fNRichSeg;  j++){
	    if( fFiredPads[i][j] > 100 && secUsed[i])
                isNoisy = kTRUE;
	}
    }
    if(isNoisy)
	cout << "hparticlebtringf: Noisy RICH segment detected and removed" << endl;
    return isNoisy;
}


