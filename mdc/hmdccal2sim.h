#ifndef HMDCCAL2SIM_H
#define HMDCCAL2SIM_H

#include "hmdccal2.h"

class HMdcCal2Sim : public HMdcCal2 {
protected:
  Int_t nTrack1;   // number of Track for first hit
  Int_t nTrack2;   // number of Track for second hit
  Int_t status1;    // status of the cell (positive if ok, negative if it should not be used)
  Int_t status2;    // status of the cell (positive if ok, negative if it should not be used)
  Int_t listTrack [5]; // list of 5 tracknumbers (GEANT) for tracks
  Int_t listStatus[5]; // list of status for 5 tracks

public:
    HMdcCal2Sim(void) {clear();}
  ~HMdcCal2Sim(void) {}
  void clear(void);
  void setNTrack1(const Int_t n) {nTrack1=n;}
  void setNTrack2(const Int_t n) {nTrack2=n;}
  void setStatus1(const Int_t f) { status1=f; }
  void setStatus2(const Int_t f) { status2=f; }
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

  Int_t getTrackFromList(Int_t element) {return listTrack [element];}
  Int_t getStatusFromList(Int_t element){return listStatus[element];}

  Int_t getNTrack1(void) const {return nTrack1;}
  Int_t getNTrack2(void) const {return nTrack2;}
  Int_t getStatus1() const {return status1;}
  Int_t getStatus2() const {return status2;}
  Int_t getStatus() const {return status1;} // will be removed later

  ClassDef(HMdcCal2Sim,1) // simulated cal2 hit on a MDC
};

#endif  /* HMDCCAL2SIM_H */

