//*-- Author   : Patrick Sellheim, Georgy Kornakov
//*-- Created  : 09/01/2014

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HParticleBtRingInfo
//  Class stores ring, rich and cluster information
//  Data collected by HParticleBtRing and HParticleCluster are stored.
//  Basic information like ring prediction, rich hits and cluster variables
//  including detailed information of maximas are stored in arrays.
//
//
//
//  Cluster classes
//  ---------------
//
//   # pad
//   x maximum
//
//   - - - - - - - - - - - -
//   0: charge < 65
//   - - - - - - - - - - - -
//   1: #
//   - - - - - - - - - - - -
//   2: ##
//   - - - - - - - - - - - -
//   3: #
//      #
//   - - - - - - - - - - - -
//   4: #x#
//   - - - - - - - - - - - -
//   5: #
//      x
//      #
//   - - - - - - - - - - - -
//   6: #x    (all orientations)
//       #
//   - - - - - - - - - - - -
//   7: x##   (all orientations)
//   - - - - - - - - - - - -
//   8: ##    (all orientations)
//       x
//   - - - - - - - - - - - -
//   9: x#    (all orientations)
//      ##
//   - - - - - - - - - - - -
//   10:  #   (all orientations)
//       #x#
//   - - - - - - - - - - - -
//   11: #### (all orientations)
//   - - - - - - - - - - - -
//   12: Rest of clusters with size 4
//   - - - - - - - - - - - -
//   13:  ##  #x  x#   (all orientations)
//        x## ### ###
//   - - - - - - - - - - - -
//   14: Rest of clusters with size 5
//   - - - - - - - - - - - -
//   x : Cluster size x -9
//   - - - - - - - - - - - -
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hparticlebtringinfo.h"

// ----------------------------------------------------------------

ClassImp(HParticleBtRingInfo)

// ----------------------------------------------------------------

HParticleBtRingInfo::HParticleBtRingInfo( ){

}

HParticleBtRingInfo::~HParticleBtRingInfo( ){

}

void HParticleBtRingInfo::init() {

    for(Int_t i = 0; i < 128; i++ ){
	for(Int_t j = 0; j < 128; j++ ){
	    fPrediction[i][j]  = -1;
	    fRingMatrix[i][j]  = -1;
	}
    }

    for(Int_t i = 0; i < 1024; i++ ){
	fRichHitAdd[i]    = -1;
	fRichHitCharge[i] = -1.;
	fIsInCluster[i]   = -1;
    }

    for(Int_t i = 0; i < 128; i++ ){
	fTrackTheta[i]    = -1.;
	fTrackPhi[i]      = -1.;
	fTrackVertex[i]   = -1;
	fTrackSec[i]      = -1;
	fTrackPCandIdx[i] = -1;
	fIsGoodTrack[i]   = kFALSE;
	fPosXCenter[i]   = -1.;
	fPosYCenter[i]   = -1.;

    }

    for(Int_t i = 0; i < 32; i++ ){
	for(Int_t j = 0; j < 32; j++ ){
	    fClusTrackNo[i][j] = -1;
	}
	fClusIsGood[i]     = kTRUE;
	fClusPadSum[i]     = -1;
	fClusPadRing[i]    = -1;
	fClusChargeSum[i]  = -1.;
	fClusChargeRing[i] = -1.;
	fClusPosX[i]       = -1.;
	fClusPosY[i]       = -1.;
        fClusClass[i]      = -1;

    }

    for(Int_t i = 0; i < 32; i++ ){
	for(Int_t j = 0; j < 32; j++ ){
	    fClusNMaxima [i][j]  = -1.;
	    for(Int_t k = 0; k < 32; k++ ){
	            fClusNMaximaPad[i][j][k]     = -1;
		    fClusNMaximaPosX[i][j][k]    = -1;
		    fClusNMaximaPosY[i][j][k]    = -1.;

		    fClusChi2[i][j][k]           = -1.;
		    fClusChi2XMM[i][j][k]        = -1.;
		    fClusChi2YMM[i][j][k]        = -1.;

		    fClusCircleX[i][j][k]        = -1.;
		    fClusCircleY[i][j][k]        = -1.;
		    fClusSig1X[i][j][k]          = -1.;
		    fClusSig1Y[i][j][k]          = -1.;
		    fClusSig2X[i][j][k]          = -1.;
		    fClusSig2Y[i][j][k]          = -1.;
	    }
	}
    }


}
