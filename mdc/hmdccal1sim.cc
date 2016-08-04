//*-- AUTHOR : Ilse Koenig
//*-- Modified : 18/06/99 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcCal1Sim
//
// Data object for a Mdc cell containing digitized drift times
// from simulation
// This class inherits from the class HMdcCal1 used for real data.
//
// As additional data elements the track numbers of the two hits are stored
// and a status number set by the digitizer.
// The track number of the hits can be accessed via the inline functions    
//     void setNTrack1(const Int_t nTrack1)     of time1
//     void setNTrack2(const Int_t nTrack2)     of time2
//     Int_t getNTrack1(void) const             of time1
//     Int_t getNTrack2(void) const             of time2
// and the status via the inline functions 
//     void setStatus1(const Int_t flag)        of time1
//     Int_t getStatus1() const                 of time1
//     void setStatus2(const Int_t flag)        of time2
//     Int_t getStatus2() const                 of time2
//     void setAngle1   (const Float_t angle1)  of time1
//     void setAngle2   (const Float_t angle2)  of time2
//     Float_t getAngle1(void)                  of time1
//     Float_t getAngle2(void)                  of time2
//     void setMinDist1 (const Float_t mindist1)of time1
//     void setMinDist2 (const Float_t mindist2)of time2
//     Float_t getMinDist1(void)                of time1
//     Float_t getMinDist2(void)                of time2
//     void setError1 (const Float_t error1)    of time1
//     void setError2 (const Float_t error2)    of time2
//     Float_t getError1(void)                  of time1
//     Float_t getError2(void)                  of time2
//     void setTof1 (const Float_t tof1)        of time1
//     void setTof2 (const Float_t tof2)        of time2
//     void setWireOffset1 (const Float_t off1) of time1
//     void setWireOffset2 (const Float_t off2) of time2
//     Float_t getTof1(void)                    of time1
//     Float_t getTof2(void)                    of time2
//     Float_t getWireOffset1(void)             of time1
//     Float_t getWireOffset2(void)             of time2
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
//         angle1  ==   impact of track1 or -99 if not filled
//         angle2  ==   impact of track2 or -99 if not filled
//         minDist1==   minimum distance of track1 or -99 if not filled
//         minDist2==   minimum distance of track2 or -99 if not filled
//         error1  ==   error of time1
//         error2  ==   error of time1 of the second valid hit or -99 if not filled
//         tof1    ==   tof of time1
//         tof2    ==   tof of time1 of the second valid hit or -99 if not filled
//         wireOff1==   signal time on wire of track1 or -99 if not filled
//         wireOff2==   signal time on wire of track2 or -99 if not filled
//         nTrack1 ==   track number of the first valid hit
//                 ==  -99 if not filled
//         nTrack2 ==   track number of the second valid hit
//                 ==  -99 if not filled
//         time1   ==   drift time1 of the first valid hit
//                 ==  -999 if not filled
//         time2   ==   drift time1 of the second valid hit
//                 ==  -999 if not filled
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
//         angle1  ==   impact of track1 or -99 if not filled
//         angle2  ==   impact of track1 or -99 if not filled
//         minDist1==   minimum distance of track1 or -99 if not filled
//         minDist2==   minimum distance of track1 or -99 if not filled
//         error1  ==   error of time1 or -99 if not filled
//         error2  ==   error of time2 or -99 if not filled
//         tof1    ==   tof of time1 or -999 if not filled
//         tof2    ==   tof of time2 or -999 if not filled
//         wireOff1==   signal time on wire of track1 or -99 if not filled
//         wireOff2==   signal time on wire of track1 or -99 if not filled
//         nTrack1 ==   track number of first valid hit
//                 ==  -99 if not filled
//         nTrack2 ==   track number of first valid hit
//                 ==  -99 if not filled
//         time1   ==   drift time1 of the first valid hit
//                 ==  -999 if not filled
//         time2   ==   drift time2 of the first valid hit
//                 ==  -999 if not filled
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
///////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "hmdccal1sim.h"
#include <iostream> 
#include <iomanip>
ClassImp(HMdcCal1Sim)

void HMdcCal1Sim::clear(void) {
  // clears the object
  HMdcCal1::clear();
  nTrack1  =nTrack2=-99;
  status1  =0;
  status2  =0;
  angle1   =-99;
  angle2   =-99;
  minDist1 =-99;
  minDist2 =-99;
  error1   =-99;
  error2   =-99;
  tof1     =-999;
  tof2     =-999;
  wireOff1 =-99;
  wireOff2 =-99;
  for(Int_t i=0;i<5;i++)
  {
      listTrack[i]=-99;
      listStatus[i]=0;
  }
}
