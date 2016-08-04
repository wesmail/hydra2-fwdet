//*-- AUTHOR : Diego Gonzalez-Diaz 17/12/2007
//*-- Modified: 12/12/2009
//(Included flags 'isAtBox'). Only relevant for hgeantrpc hits
//created by interaction of secondary charged particles stemming
//from *neutral particles* impinging at the RPC virtual box.  
//*-- Modified: 15/12/2009
// Geant references and tracks extended to 10-dimensional arrays
//
//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
// HRpcCalSim                                                                         //
// This is the cal data level for simulated data.                                     //
//                                                                                    //
// It stores 4 references to hgeantrpc objects and the corresponding tracks           //
// (mother and daughter, left and right). In order to facilitate track embedding      //
// and tracking debugging it has been extended to arrays of 10 elements               //
// The first track carries the resolution   					      //
// but the others carry also information about the total collected charge.            //
// Tracks 2-10 are stored in the array sorted according to the geant arrival time to  // 
// the most upstream gap. Note that, due to the smearing procedure, it can            //
// mathematically happen that the first track of the array has a larger geant time    //
// than the second. Still, the one that gave the signal is taken always as first.     //
// This technical nuissance is not likely to be any problem. The                      //
// number of tracks per cell is also stored for later purposes as 'ntracks'.          //
//                                                                                    //
//    Int_t RefL;                                                                     //
//  ->index of the hgeantrpc object created in EBOX by the track whose                //
//  daughter (or herself) made the left signal of this cell.                          //
//    Int_t RefR; (right)                                                             //
//    Int_t RefLDgtr;                                                                 //
//  ->index of the hgeantrpc object created in the gap EGij by the                    // 
//  track that made the left signal of this cell.                                     //
//    Int_t RefRDgtr; (right)                                                         //
//    Int_t TrackL[10];                                                               //
//  ->TrackL[1] is the track that created the hgeantrpc object at EBOX and whose      //
//  daughter (or herself) made the left signal of this cell. Elements 2-10 are sorted //
//  according to the geant times of daughter tracks at the most upstream fired gap.   //
//    Int_t TrackR[10]; (right)                                                       //
//    Int_t TrackLDgtr[10];                                                           //
//  ->TrackLDgtr[1] is the track that created the hgeantrpc object at gap EGij that   //
//  made the left signal.                                                             //
//    Int_t TrackRDgtr[10]; (right)                                                   //
//    Bool_t LisAtBox[10];                                                            //
//  ->Boolean to indicate if a mother was found at the box or not for TrackLDgtr[i]   //
//    Bool_t RisAtBox[10];  (right)                                                   //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "hrpccalsim.h"

ClassImp(HRpcCalSim)

HRpcCalSim::HRpcCalSim(void) {
  clear();
}
void HRpcCalSim::clear(void) {

  RefL       = RefR       = -1;
  RefLDgtr   = RefRDgtr   = -1;
  nTracksL   = nTracksR   =  0;

  for(Int_t i=0;i<10;i++)     {
    TrackL[i]   = TrackLDgtr[i] = TrackR[i] = TrackRDgtr[i] = -999;
    LisAtBox[i] = RisAtBox[i]   = kFALSE;
  }
}

