#ifndef HMDCCAL1SIM_H
#define HMDCCAL1SIM_H

#include "hmdccal1.h"

class HMdcCal1Sim : public HMdcCal1 {
protected:
  Int_t nTrack1;       // number of Track for first hit
  Int_t nTrack2;       // number of Track for second hit
  Int_t status1;       // status of cell (positive if ok, negative if it should not be used)
  Int_t status2;       // status of cell (positive if ok, negative if it should not be used)
  Int_t listTrack [5]; // list of 5 tracknumbers (GEANT) for tracks
  Int_t listStatus[5]; // list of status for 5 tracks
  Float_t angle1;      // impact angle1 in cell system 0-90 [degree]
  Float_t angle2;      // impact angle2 in cell system 0-90 [degree]
  Float_t minDist1;    // minimum distance to sense wire for track1 [mm]
  Float_t minDist2;    // minimum distance to sense wire for track2 [mm]
  Float_t error1;      // error of time1 [ns]
  Float_t error2;      // error of time2 [ns]
  Float_t tof1;        // tof of track1  [ns]
  Float_t tof2;        // tof of track2  [ns]
  Float_t wireOff1;    // signal time on the wire of track1 [ns]
  Float_t wireOff2;    // signal time on the wire of track2 [ns]

public:
    HMdcCal1Sim(void) : nTrack1(-99),nTrack2(-99),status1(0),status2(0)
	                ,angle1(-99),angle2(-99),minDist1(-99),minDist2(-99)
	                ,error1(-99),error2(-99),tof1(-999),tof2(-999)
	                ,wireOff1(-99),wireOff2(-99) {}
  ~HMdcCal1Sim(void) {}
  void clear(void);
  void setNTrack1  (const Int_t n) {nTrack1=n;}
  void setNTrack2  (const Int_t n) {nTrack2=n;}
  void setStatus1  (const Int_t f) {status1=f;}
  void setStatus2  (const Int_t f) {status2=f;}
  void setAngle1   (const Float_t f) {angle1=f;}
  void setAngle2   (const Float_t f) {angle2=f;}
  void setMinDist1 (const Float_t f) {minDist1=f;}
  void setMinDist2 (const Float_t f) {minDist2=f;}
  void setError1   (const Float_t f) {error1=f;}
  void setError2   (const Float_t f) {error2=f;}
  void setTof1     (const Float_t f) {tof1=f;}
  void setTof2     (const Float_t f) {tof2=f;}
  void setWireOffset1 (const Float_t f) {wireOff1=f;}
  void setWireOffset2 (const Float_t f) {wireOff2=f;}
  void resetTrackList(Int_t track=-99){  for(Int_t i=0;i<5;i++)listTrack[i]=track;}
  void setTrackList(Int_t i,Int_t track) {listTrack[i]=track;}
  void setTrackList(Int_t* array)
  {
   for(Int_t i=0;i<5;i++)
     {
	 if(array[i]!=0 && array[i]>=0)
	 {
	     listTrack[i]=array[i];
	 }
	 else
	 {
	     listTrack[i]=-99;
	 }
     }
  };
  void getTrackList(Int_t* array)
  {
   for(Int_t i=0;i<5;i++)
     {
	 array[i]=listTrack[i];
     }
  };
  void resetStatusList(Int_t stat=0){  for(Int_t i=0;i<5;i++)listStatus[i]=stat;}
  void setStatusList(Int_t i,Int_t stat) {listStatus[i]=stat;}
  void setStatusList(Int_t* array)
  {
   for(Int_t i=0;i<5;i++)
     {
	 listStatus[i]=array[i];
     }
  };
  void getStatusList(Int_t* array)
  {
   for(Int_t i=0;i<5;i++)
     {
	 array[i]=listStatus[i];
     }
  };
  Int_t getNTracks()
  {
      Int_t i=0;
      while(listTrack[i]!=-99 && i<5)
      {
	  i++;
      }
      return i;
  };
  Int_t* getStatusList(){return (Int_t*)listStatus;}
  Int_t* getTrackList() {return (Int_t*)listTrack;}
  Int_t getTrackFromList (Int_t element){return listTrack [element];}
  Int_t getStatusFromList(Int_t element){return listStatus[element];}
  Int_t getNTrack1 (void) const {return nTrack1;}
  Int_t getNTrack2 (void) const {return nTrack2;}
  Int_t getStatus1 (void) const { return status1; }
  Int_t getStatus2 (void) const { return status2; }
  Int_t getStatus  (void) const { return status1; }  // will be removed later
  Float_t getAngle1(void) const { return angle1; }
  Float_t getAngle2(void) const { return angle2; }
  Float_t getMinDist1(void) const { return minDist1; }
  Float_t getMinDist2(void) const { return minDist2; }
  Float_t getError1  (void) const { return error1; }
  Float_t getError2  (void) const { return error2; }
  Float_t getTof1    (void) const { return tof1; }
  Float_t getTof2    (void) const { return tof2; }
  Float_t getWireOffset1(void) const { return wireOff1; }
  Float_t getWireOffset2(void) const { return wireOff2; }

  ClassDef(HMdcCal1Sim,1) // simulated cal1 hit on a MDC
};

#endif  /* HMDCCAL1SIM_H */

