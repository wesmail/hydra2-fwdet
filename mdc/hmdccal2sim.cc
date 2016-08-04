//*-- AUTHOR : Ilse Koenig
//*-- Modified : 18/06/99 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcCal2Sim
//
// Data object for a Mdc cell containing distances to the wires for simulated
// events
// This class inherits from the class HMdcCal1 used for real data.
//
// As additional data elements the track numbers of the two hits are stored
// and a status number.
// The track number of the hits can be accessed via the inline functions    
//     void setNTrack1(const Int_t nTrack1) of dist1
//     void setNTrack2(const Int_t nTrack2) of dist2
//     Int_t getNTrack1(void) const         of dist1
//     Int_t getNTrack2(void) const         of dist2
// and the status via the inline functions 
//     void setStatus1(const Int_t flag)    of dist1
//     Int_t getStatus1() const             of dist1
//     void setStatus2(const Int_t flag)    of dist2
//     Int_t getStatus2() const             of dist2
//     void  setTrackList(Int_t* array) array of tracks[5]  can be coppied
//           to TrackList
//     void  getTrackList(Int_t* array) array of tracks[5]  can be filled
//           from TrackList
//     void  setStatusList(Int_t* array) array of status[5] can be coppied
//           to StatusList
//     void  getStatusList(Int_t* array) array of status[5] can be filled
//           from StatusList
//     Int_t getNTracks() returns number of tracks in track list,
//           can be used for loops
//           for(Int_t i=0;i<(HMdcCal1Sim*)cal->getNTracks();i++)
//              { (HMdcCal1Sim*)cal->getTrackFromList(i);}
//     Int_t* getStatusList(){return (Int_t*)listStatus;}
//     Int_t* getTrackList() {return (Int_t*)listTrack;}
//     Int_t getTrackFromList (Int_t element){return listTrack [element];}
//     Int_t getStatusFromList(Int_t element){return listStatus[element];}
//
// According to the two different TDC modes the HMdcCal1Sim category is filled.
//
// MODE 1 (two times LEADING EDGE of the TDC signal)
//           nHits ==  -2 for 2 valid hits
//                 ==  -1 for 1 valid hit
//                 ==   0 for a not filled hit (e.g. 1 hit was kicked out by efficiency)
//         status1 ==   1 for a valid first hit
//                 ==   2 for a valid first hit caused by noise
//                 ==  -3 for a not valid hit
//                 ==   0 for REAL data (embedding) or no hit
//         status2 ==   1 for a valid second hit
//                 ==   2 for a valid first hit caused by noise
//                 ==  -3 for a not valid hit
//                 ==   0 for REAL data (embedding) or no hit
//         nTrack1 ==   track number of the first valid hit
//                 ==  -99 if not filled
//         nTrack2 ==   track number of the second valid hit
//                 ==  -99 if not filled
//         dist1   ==   distance from sense wire of irst valid hit
//                 ==  -99 if not filled
//         dist2   ==   distance from sense wire of the second valid hit
//                 ==  -99 if not filled
//     listTrack[5] :   contains the track number of the first 5 hits per cell
//                      == -99 if no hit was filled
//     listStatus[5]:   contains the status flags of the first 5 hits per cell
//                      == -1 if hit was kicked out by cell efficiency cut
//                      == -2 if hit was kicked out by layer efficiency cut
//                      ==  1 if hit is valid
//                      ==  2 if hit is noise
//                      ==  3 if hit is REAL data (embedding)
//                      ==  0 if no hit was filled
//     both lists will be filled even if no vaild hit was found
//
// MODE 2 (LEADING AND TRAILING EDGE of the TDC signal)
//           nHits ==  +2 for 2 valid hits
//                 ==   0 for not filled hit (e.g. 1 hit was kicked out by efficiency)
//         status1 ==   1 for a valid first hit
//                 ==   2 for a valid first hit caused by noise
//                 ==  -3 for a not valid hit
//                 ==   0 for REAL data (embedding) or no hit
//         status2 ==   1 for a valid first hit
//                 ==   2 for a valid first hit caused by noise
//                 ==  -3 for a not valid hit
//                 ==   0 for REAL data (embedding) or no hit
//         nTrack1 ==   track number of first valid hit
//                 ==  -99 if not filled
//         nTrack2 ==   track number of first valid hit
//                 ==  -99 if not filled
//         dist1   ==   distance from sense wire of the first valid hit
//                 ==  -99 if not filled
//         dist2   ==  -99 if not filled
//     listTrack[5] :   contains the track number of the first 5 hits per cell
//                      == -99 if no hit was filled
//     listStatus[5]:   contains the status flags of the first 5 hits per cell
//                      == -1 if hit was kicked out by cell efficiency cut
//                      == -2 if hit was kicked out by layer efficiency cut
//                      ==  1 if hit is valid
//                      ==  2 if hit is noise
//                      ==  3 if hit is REAL data (embedding)
//                      ==  0 if no hit was filled
//     both lists will be filled even if no vaild hit was found
//
////////////////////////////////////////////////////////////////////////////////

#include "hmdccal2sim.h"

ClassImp(HMdcCal2Sim)

void HMdcCal2Sim::clear() {
  // clears the object
  HMdcCal2::clear();
  nTrack1=nTrack2=-99;
  status1=0;
  status2=0;
  for(Int_t i=0;i<5;i++)
  {
      listTrack[i]=-99;
      listStatus[i]=0;
  }
}
