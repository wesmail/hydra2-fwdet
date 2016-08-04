//*-- AUTHOR : Vladimir Pechenov                                                         
//*-- Modified : 31.05.2001 by Vladimir Pechenov

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//  HMdcHitSim
// 
// This class is derived from HMdcHit for simulated events.
// Additional data elements are following:
//   nTracks - the number of tracks in the list of tracks numbers
//   listTracks[5] - the list of tracks numbers
//   nTimes[5] - num. of drift times from each track in this hit
//     (not cells because one cell can have 2 drift times from 2 tracks)
//   Number of noise wires is kept in nTimes[4] and
//   if nTimes[4]>0 listTracks[4]=-99
// This information can be accessed via the functions:
//    Int_t getNTracks(void)
//    Int_t getTrack(const Int_t n)      0 <= n < getNTracks()
//    UChar_t getNTimes(const Int_t n)
//    Int_t   getNumNoiseWires(void)
//
// The list of tracks is sorted. listTracks[0] is a track which have
// most number of drift times and etc.
// 
// The list of tracks is calculated after filling of HMdcHit container
// via function  Int_t calcNTracks(void) which return nTracks.
// 
///////////////////////////////////////////////////////////////////////////////

#include "hmdchitsim.h"
#include "hades.h"
#include "hmdcdef.h"
#include "hmdccal1sim.h"
#include "hevent.h"
#include "hcategory.h"
#include "hmessagemgr.h"
#include "TMath.h"

ClassImp(HMdcHitSim)

void HMdcHitSim::clear()
{
    // reset data members to default values
    nTracks=0;
    status=0;
    HMdcHit::Clear();
    for(Int_t i=0;i<5;i++){
       listTracks[i]=0;
       nTimes[i]    =0;
    }
}

void HMdcHitSim::setNumNoiseWires(Int_t n) {
  // setting number of noise wires
  if(n<=0) return;
  if(nTracks>4) nTracks=4;
  listTracks[4]=-99;
  nTimes[4]=n;
}

void HMdcHitSim::setNTracks(Int_t nTr,const Int_t* listTr,const UChar_t* nTm) {
    // copies nTr track numbers from array listTr to
    // internal list of tracks array.

  if(nTr<=0) nTracks=-1;
  else {
    Int_t maxTr = (nTr<6) ? nTr:5;
    nTracks = 0;
    for(Int_t n=0; n<maxTr; n++) {
      if(listTr[n] == -99) {
        listTracks[4]       = listTr[n];
        nTimes[4]           = nTm[n];
      } else {
        listTracks[nTracks] = listTr[n];
        nTimes[nTracks]     = nTm[n];
        nTracks++;
      }
    }
  }
}

Int_t HMdcHitSim::calcNTracks(void) {
    // calculate number of tracks which
    // contribute to the hit and fills the
    // list of tracks array. Needs the
    // event structure and Cal1Sim category
    // to deliver results.
 
  nTracks=0;
  HCategory *fCalCat=gHades->getCurrentEvent()->getCategory(catMdcCal1);
  if(!fCalCat) return -1;
  HLocation loc;
  loc.set(4,sector,module,0,0);
  Int_t tmpListTracks[48];
  Int_t nTimesTot=0;
  for(Int_t layer=0; layer<6; layer++) {
    Int_t nCells=getNCells(layer);
    if(nCells==0) continue;
    loc[2]=layer;
    for(Int_t n=0; n<nCells; n++) {
      UChar_t nCTimes=getSignId(layer,n);
      loc[3]=getCell(layer,n);
      HMdcCal1Sim *fCal1Sim=(HMdcCal1Sim*)fCalCat->getObject(loc);
      if(fCal1Sim)
      {
	  if(nCTimes&1) tmpListTracks[nTimesTot++]=fCal1Sim->getNTrack1();
	  if(nCTimes&2) tmpListTracks[nTimesTot++]=fCal1Sim->getNTrack2();
      } else Error("calcNTracks","Zero pointer received!");
    }
  }
  Short_t counter[48];  
  listTracks[4]=-99;        // [4] - for noise wires
  nTimes[4]=0;              // ...
  for(Int_t n=0; n<nTimesTot; n++) {
      if(tmpListTracks[n]<0 &&
	 tmpListTracks[n]!=gHades->getEmbeddingRealTrackId()) {
      nTimes[4]++;          // noise wires
      continue;
      } else if(tmpListTracks[n]>0 ||
		tmpListTracks[n]==gHades->getEmbeddingRealTrackId()) {
      tmpListTracks[nTracks]=tmpListTracks[n];
      counter[nTracks]=1;
      for(Int_t m=n+1; m<nTimesTot; m++) {
        if(tmpListTracks[m] != tmpListTracks[nTracks]) continue;
        counter[nTracks]++;
        tmpListTracks[m]=0;
      }
      nTracks++;
    }
  }
  //Sorting:
  if(nTracks>0)
  {
      Int_t index[nTracks];
      if(nTracks>1) TMath::Sort(nTracks,counter,index);
      else index[0]=0;

      //Fill output:
      if(nTracks>4) nTracks=(nTimes[4]==0) ? 5:4;
      for(Int_t n=0; n<nTracks; n++) {
	  listTracks[n]=tmpListTracks[index[n]];
	  nTimes[n]=counter[index[n]];
      }
  }
  return nTracks;
}

void HMdcHitSim::print(void) {
    // prints the information about wires (number of wires,
    // track numbers and how often a track has been contributing
    // to the the hit) in Hit to the screen.

    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
			   ,"Sec.%i Mod.%i chi2=%g, %i tracks: ",
			   sector,module,chi2,nTracks);
  for(Int_t n=0; n<nTracks; n++)
      gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
			     ,"%i(%i) ",listTracks[n],nTimes[n]);
  if(listTracks[4]==-99 && nTimes[4]>0) gHades->getMsg()->info(10,
    HMessageMgr::DET_MDC,GetName(),"  noise_wires(%i) ",nTimes[4]);
  INFO_msg(10,HMessageMgr::DET_MDC,"[trNum(nOfTimes)]");
  HMdcHit::print();
}
