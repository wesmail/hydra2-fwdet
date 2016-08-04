#ifndef HRPCCLUSTERSIM_H
#define HRPCCLUSTERSIM_H

#include "hrpccluster.h"

class HRpcClusterSim : public HRpcCluster {

protected:

  Int_t TrackList[4];    //List of the four tracks contributing to the cluster, ordered as:
                         //UpLeft, UpRight, DownLeft, DownRight.
  Int_t RefList[4];      //List of the four references to the geantrpc objects contributing 
                         //to the cluster, ordered as: UpLeft, UpRight, DownLeft, DownRight.
                         //Note: In case of cluster type = 1, the order is simply:
                         //Left, Right, -, -

  Bool_t isAtBoxList[4]; //Boolean list to indicate if a mother was found at the box or not.

  Int_t Track;           //Track preferred by the cluster finder (typically the one contributing
			 //more to the 4 possible times).

  Int_t nTracksAtBox;    //Number of different tracks at the RPC box that contribute to this cluster

  Int_t nTracksAtCells;  //Number of different tracks at the RPC cells that contribute to this cluster	

  Bool_t isAtBox;        //Boolean to indicate if a mother was found at the box or not.

public:
  HRpcClusterSim(void);
  ~HRpcClusterSim(void) {}
  void clear(void);
  
  void   resetTrackList()                         {for(Int_t i=0;i<4;i++)TrackList[i]   = -999;    }
  void   resetRefList()                           {for(Int_t i=0;i<4;i++)RefList[i]     = -999;    }
  void   resetIsAtBoxList()                       {for(Int_t i=0;i<4;i++)isAtBoxList[i] = kFALSE;  }
  void   setTrackList (Int_t i, Int_t track)      {TrackList[i]=track;}
  void   setTrackList (Int_t* array)              {for(Int_t i=0;i<4;i++)TrackList[i]   = array[i];}
  void   setRefList (Int_t i, Int_t ref)          {RefList[i]=ref;}
  void   setRefList (Int_t* array)                {for(Int_t i=0;i<4;i++)RefList[i]     = array[i];}
  void   setIsAtBoxList (Int_t i, Bool_t is)      {isAtBoxList[i]=is;}
  void   setIsAtBoxList (Bool_t* array)           {for(Int_t i=0;i<4;i++)isAtBoxList[i] = array[i];}
  
  Int_t  howManyTracks()		   const  {return nTracksAtBox;}				
  Int_t  howManyTracksAtCells()		   const  {return nTracksAtCells;}

  void   setNTracksAtBox(Int_t aNTracksAtBox)     {nTracksAtBox   = aNTracksAtBox;}
  void   setNTracksAtCells(Int_t aNTracksAtCells) {nTracksAtCells = aNTracksAtCells;}	

  void   setTrack(Int_t atrack)                   {Track   = atrack;}
  void   setIsAtBox(Bool_t is)                    {isAtBox = is;    }

  Int_t  getTrack()                               {return Track;    }
  Bool_t getIsAtBox()                             {return isAtBox;  }

  void   getTrackList(Int_t* array)               {for(Int_t i=0;i<4;i++) array[i]=TrackList[i];}
  void   getRefList(Int_t* array)                 {for(Int_t i=0;i<4;i++) array[i]=RefList[i];}
  void   getIsAtBoxList(Bool_t* array)            {for(Int_t i=0;i<4;i++) array[i]=isAtBoxList[i];}

  Bool_t isTrack(Int_t track);                                   
  Bool_t isRef(Int_t ref);                                 

  ClassDef(HRpcClusterSim,2)
};

#endif  /* HRPCCLUSTERSIM_H */

