//*-- Author : R.Schicker
//*-- Modified : M.Sanchez (01.06.2000)
//*-- Modified : V.Pechenov (10.07.2002)

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////
// HMdcHit
//
//	This class represents one hit in one of the MDC
// planes. A hit is defined as a piece of a straight line
// reconstructed by a MDC chamber. It is, therefore, the highest
// level of information an individual chamber can provide.
//
//	The coordinate system used in the MDC is the local
// MDC coordinate system. That is, in the z-midplane. For a
// particle moving outwards in the spectrometer, the z-midplane is
// defined at the entrance into the fourth cathode plane. x and y are
// expressed in mm
//
//	xDir,yDir are the components of the three dimensional
// tangent unit vector to the track in the z-midplane, values equal
// to -1 indicate that the direction has not been calculated
//
//      cov is the 5x5 covariance matrix of the fit.
//
// 	The base class HMdcCellGroup  provides a mechanism to know
//      which cells contributed to a particular hit. This info is retrieved
//      with the following functions of this base class:	
//
// * Int_t getNCells(Int_t layer)
//	Returns the number of cells in layer "layer" which contributed
//	to the hit.
//
// * Int_t getCell(Int_t layer,Int_t idx)
//      For idx=0 returns the cell number of the first fired cell
//      For idx=1 returns the cell number of the second fired cell
//	.... (maximum idx=3)
//
// * Int_t HMdcHit::getSignId(Int_t layer,Int_t idx)
//	Returns signal id (time/distance) in cell designed by
//	idx (same criterium as before).
//
// * Int_t setSignId(Int_t layer,Int_t cell, Int_t t1,t2=0,t3=0,t4=0)
//	Sets the signal ids and base cell number for layer "layer"
//	Input:
//	  layer --> Number of layer to set
//	  cell  --> Number of the first cell fired in this layer
//	  t1 - t4 --> SignalId for cells: "cell"+0, "cell"+1 ... "cell"+4
//	              Possible values are:
//	                0 --> This cell has not fired
//	                1 --> First signal (time/distance) was used
//	                2 --> Second signal (time/distance) was used
//
// * Short_t getTrackFinder()
//      Returns the type of track finder (Dubna, Santiago, ...)
//      For Dubna track finder it retutns 1.
//
//    The meanings of data member "index" depend from type of track finder 
// (Dubna, Santiago, ...)
//
// Dubna track finder:
//   "index" is index of HMdcClusInf object
//   in category catMdcClusInf (see mdctrackD/hmdcclusinf.cc)
//   Function  getClusInfIndex() return this index.
//
//   Number and oder of HMdcClusInf objects in catMdcClusInf 
//   are the very same as number and order of HMdcHit objects in
//   catMdcHit. You can use it in T->Draw(...) function:
//   T-Draw("HMdcHit.fData.y : HMdcClusInf.fData.clusSize", ...)
//
////////////////////////////////////////////////////////////

using namespace std;
#include "hmdchit.h"
#include <iostream> 
#include <iomanip>

ClassImp(HMdcHit)

void HMdcHit::Clear(void) {
  // reset data members to default values
  x=0.;
  y=0.;
  xDir=0.;
  yDir=0.;
  offset=0.;
  chi2=0.;
  flag=0;
  cov.Clear();
  trFinder=-1;
  index=-1;
  clear();
}
