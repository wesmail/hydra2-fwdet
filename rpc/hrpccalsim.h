#ifndef HRPCCALSIM_H
#define HRPCCALSIM_H

#include "hrpccal.h"

class HRpcCalSim : public HRpcCal {

protected:
  Int_t RefL;           //Reference to the first mother contributing to the cell 
  Int_t RefR;    
  Int_t RefLDgtr;       //Reference to the first daughter contributing to the cell
  Int_t RefRDgtr;   

  Int_t TrackL[10];     //Array of tracks of mothers at the RPC box for this cell
  Int_t TrackR[10];  
  Int_t TrackLDgtr[10]; //Array of tracks of daughters
  Int_t TrackRDgtr[10];

  Int_t nTracksL;       //Number of daughter tracks at this cell
  Int_t nTracksR;     

  Bool_t LisAtBox[10];  //Are they at box?
  Bool_t RisAtBox[10]; 

public:
  HRpcCalSim(void);
  ~HRpcCalSim(void) {}
  void clear(void);
  
  void  setRefL(Int_t aRefL)                  { RefL=aRefL;         }
  void  setRefR(Int_t aRefR)                  { RefR=aRefR;         }
  void  setRefLDgtr(Int_t aRefLDgtr)          { RefLDgtr=aRefLDgtr; }
  void  setRefRDgtr(Int_t aRefRDgtr)          { RefRDgtr=aRefRDgtr; }

  void  setTrackL(Int_t aTrackL)              { TrackL[0]     = aTrackL;     }
  void  setTrackR(Int_t aTrackR)              { TrackR[0]     = aTrackR;     }
  void  setTrackLDgtr(Int_t aTrackLDgtr)      { TrackLDgtr[0] = aTrackLDgtr; }
  void  setTrackRDgtr(Int_t aTrackRDgtr)      { TrackRDgtr[0] = aTrackRDgtr; }

  void  setLisAtBox(Bool_t aLisAtBox)         { LisAtBox[0]=aLisAtBox; }
  void  setRisAtBox(Bool_t aRisAtBox)         { RisAtBox[0]=aRisAtBox; }

  void  setNTracksL(Int_t anTracksL)          { nTracksL=anTracksL; }
  void  setNTracksR(Int_t anTracksR)          { nTracksR=anTracksR; }

  void  setTrackLArray(Int_t* trackLarray)         {for(Int_t i=0;i<10;i++) TrackL[i]     = trackLarray[i];     }
  void  setTrackLDgtrArray(Int_t* trackLDgtrarray) {for(Int_t i=0;i<10;i++) TrackLDgtr[i] = trackLDgtrarray[i]; }
  void  setTrackRArray(Int_t* trackRarray)         {for(Int_t i=0;i<10;i++) TrackR[i]     = trackRarray[i];     }
  void  setTrackRDgtrArray(Int_t* trackRDgtrarray) {for(Int_t i=0;i<10;i++) TrackRDgtr[i] = trackRDgtrarray[i]; }
  void  setLisAtBoxArray(Bool_t* LisAtBoxarray)    {for(Int_t i=0;i<10;i++) LisAtBox[i]   = LisAtBoxarray[i];   }
  void  setRisAtBoxArray(Bool_t* RisAtBoxarray)    {for(Int_t i=0;i<10;i++) RisAtBox[i]   = RisAtBoxarray[i];   }

  Int_t getRefL()                             { return RefL;     }
  Int_t getRefR()                             { return RefR;     }
  Int_t getRefLDgtr()                         { return RefLDgtr; }
  Int_t getRefRDgtr()                         { return RefRDgtr; }

  Int_t getTrackL()                const      { return TrackL[0];     }
  Int_t getTrackR()                const      { return TrackR[0];     }
  Int_t getTrackLDgtr()            const      { return TrackLDgtr[0]; }
  Int_t getTrackRDgtr()            const      { return TrackRDgtr[0]; }

  Bool_t getLisAtBox()             const      { return LisAtBox[0]; }
  Bool_t getRisAtBox()             const      { return RisAtBox[0]; }

  Int_t getNTracksL()                         { return nTracksL; }
  Int_t getNTracksR()                         { return nTracksR; }

  void  getTrackLArray(Int_t* trackLarray)         {for(Int_t i=0;i<10;i++) trackLarray[i]     = TrackL[i];     }
  void  getTrackLDgtrArray(Int_t* trackLDgtrarray) {for(Int_t i=0;i<10;i++) trackLDgtrarray[i] = TrackLDgtr[i]; }
  void  getTrackRArray(Int_t* trackRarray)         {for(Int_t i=0;i<10;i++) trackRarray[i]     = TrackR[i];     }
  void  getTrackRDgtrArray(Int_t* trackRDgtrarray) {for(Int_t i=0;i<10;i++) trackRDgtrarray[i] = TrackRDgtr[i]; }
  void  getLisAtBoxArray(Bool_t* LisAtBoxarray)    {for(Int_t i=0;i<10;i++) LisAtBoxarray[i]   = LisAtBox[i];   } 
  void  getRisAtBoxArray(Bool_t* RisAtBoxarray)    {for(Int_t i=0;i<10;i++) RisAtBoxarray[i]   = RisAtBox[i];   } 

  ClassDef(HRpcCalSim,4)
};

#endif  /* HRPCCALSIM_H */

