//*-- AUTHOR : Vladimir Pechenov
//*-- Modified : 31.05.2001 by Vladimir Pechenov

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//  HMdcSegSim
//
// This class is derived from HMdcSeg for simulated events.
// Additional data elements are following:
//   nTracks - the number of tracks in the list of tracks numbers
//   listTracks[5] - the list of tracks numbers (GEANT)
//   nTimes[5] - num. of drift times from each track in this segment hit
//     (not cells because one cell can have 2 drift times from 2 tracks)
//   Number of noise wires is kept in nTimes[4] and
//   if nTimes[4]>0 listTracks[4]=-99
// This information can be accessed via the functions:
//    Int_t getNTracks(void)
//    Int_t getTrack(const Int_t n)      0 <= n < getNTracks()
//    UChar_t getNTimes(const Int_t n)
//    Int_t   getNumNoiseWires(void)
//
// The list of tracks is sorted by flag "real track", matching flag and number of wires.
// The list of tracks is sorted after filling of HMdcSegSim container
// via function  void sortTrListByContr(void).
//
// Example of using getGoodTrack(...) function:
//  HMdcSegSim* innerSeg =...;
//  HMdcSegSim* outerSeg =...;
//  Int_t nWiresCut = 5; // 5 is default value
//  Int_t nGoodInnerTracks = innerSeg->getNNotFakeTracks();
//  for(Int_t i1=0;i1<nGoodInnerTracks;i1++) {
//    Int_t track = innerSeg->getGoodTrack(i1,outerSeg,nWiresCut);
//    if(track==0) {
//      fake!
//    } else if(track>0) {
//      good track!
//    } else { //<0
//      ...
//    }
//  }
// 
//
// Example of using getNextGoodTrack(...) function:
//  HMdcSegSim* innerSeg =...;
//  HMdcSegSim* outerSeg =...;
//  Int_t nWiresCut = 5; // 5 is default value
//  Int_t i1=0;
//  Int_t track;
//  while( (track=innerSeg->getNextGoodTrack(i1,outerSeg,nWiresCut)) >0) {
//    good track!
//  } 
//
///////////////////////////////////////////////////////////////////////////////

#include "hmdcsegsim.h"
#include "hades.h"
#include "hmdcdef.h"
#include "hmdccal1sim.h"
#include "hevent.h"
#include "hcategory.h"
#include "hmessagemgr.h"
#include "htool.h"

ClassImp(HMdcSegSim)

void HMdcSegSim::clear()
{
  // reset data members to default values
  clearSimInfo();
  HMdcSeg::Clear();
}

void HMdcSegSim::clearSimInfo(void) {
  // reset data members to default values
  status         = 0;
  nTracks        = 0;
  nNotFakeTracks = -1;
  ioSMatchStatus = -1;
  for(Int_t i=0;i<5;i++){
    listTracks[i]  = 0;
    nTimes[i]      = 0;
    nDigiTimes[i]  = 0;
    trackStatus[i] = 1;  // No metamatch flag any more!
  }
}

void HMdcSegSim::setNTracks(Int_t nTr, const Int_t* listTr,const UChar_t* nTm,const UChar_t* nDi) {
    // copies nTr track numbers from array listTr to
    // internal list of tracks array.

  if(nTr<=0) nTracks=-1;
  else {
    Int_t maxTr = (nTr<6) ? nTr:5;
    nTracks = 0;
    for(Int_t n=0; n<maxTr; n++) {
        listTracks[nTracks] = listTr[n];
        nTimes[nTracks]     = nTm[n];
        if(nDi != NULL) nDigiTimes[nTracks] =  nDi[n];
        nTracks++;
    }
  }
}

void HMdcSegSim::addTrack(Int_t track, UChar_t nTm,UChar_t nDi) {
  // add track info to internal list of tracks array.
  if(nTracks>=0 && nTracks < 5) {
    listTracks[nTracks] = track;
    nTimes[nTracks]     = nTm;
    nDigiTimes[nTracks] = nDi;
    nTracks++;
  }
}

Int_t HMdcSegSim::calcNTracks(void) {
    // calculate number of tracks which
    // contribute to the segment and fills the
    // list of tracks array. Needs the
    // event structure and Cal1Sim category
    // to deliver results.

  nTracks=0;
  HCategory *fCalCat=gHades->getCurrentEvent()->getCategory(catMdcCal1);
  if(!fCalCat) return -1;
  HLocation loc;
  loc.set(4,getSec(),0,0,0);
  Int_t seg=getIOSeg();
  Int_t tmpListTracks[96];
  Int_t nTimesTot=0;
  for(Int_t layer=0; layer<12; layer++) {
    Int_t nCells=getNCells(layer);
    if(nCells==0) continue;
    loc[1]=seg*2+layer/6;
    loc[2]=layer%6;
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
  Short_t counter[96];
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
      if(nTracks>1) HTool::sort(nTracks,counter,index,kTRUE,kTRUE);  // sort: down, change counter array directly
      else index[0]=0;

      //Fill output:
      if(nTracks>4) nTracks=(nTimes[4]==0) ? 5:4;
      for(Int_t n=0; n<nTracks; n++) {
	  listTracks[n]=tmpListTracks[index[n]];
	  nTimes[n]=counter[n];
      }
  }
  return nTracks;
}

Int_t HMdcSegSim::getNumNoiseWires(void) const {
  for(Int_t n=0; n<5; n++) if(listTracks[n]==-99) return nTimes[n];
  return 0;
}

void HMdcSegSim::sortTrListByContr(void) {
  // Sorting track list "listTr" by "FakeContribution" flag.
  // After sorting tracks with "not Fake" status will be moved to top of arrays.
  nNotFakeTracks = 0;
  for(Int_t t=0;t<nTracks;t++) if( !isFakeContribution(t) ) {
    nNotFakeTracks++;
  }
  if(nTracks==1) return;
  Int_t nTimesSort[5];
  for(Int_t t=0;t<nTracks;t++) {
    nTimesSort[t] = nTimes[t];
    if(isFakeContribution(t)) continue;
    nTimesSort[t] <<= 16;
  }
  for(Int_t t1=0;t1<nTracks-1;t1++) {
    Int_t maxInd = t1;
    for(Int_t t2=t1+1;t2<nTracks;t2++) if(nTimesSort[t2] > nTimesSort[maxInd]) maxInd = t2;
    if(maxInd == t1) continue;
    nTimesSort[maxInd] = nTimesSort[t1];
    exchangeTrPos(t1,maxInd);
  }
}

void HMdcSegSim::exchangeTrPos(Int_t t1,Int_t t2) {
  Int_t   v1 = listTracks[t1];
  Short_t v2 = nTimes[t1];
  UChar_t v3 = nDigiTimes[t1];
  UChar_t v4 = trackStatus[t1];

  listTracks[t1]  = listTracks[t2];
  nTimes[t1]      = nTimes[t2];
  nDigiTimes[t1]  = nDigiTimes[t2];
  trackStatus[t1] = trackStatus[t2];

  listTracks[t2]  = v1;
  nTimes[t2]      = v2;
  nDigiTimes[t2]  = v3;
  trackStatus[t2] = v4;
}

void HMdcSegSim::print(void) {
    // prints the information about wires (number of wires,
    // track numbers and how often a track has been contributing
    // to the the segment) in segment to the screen.
  HMdcSeg::print();
  printf("  GEANT: %i tracks in segment",nTracks);
  if(nNotFakeTracks==0) printf(".  FAKE SEGMENT!\n");
  else  {
    if(nNotFakeTracks>=0 && nNotFakeTracks != nTracks) 
        printf(",  %i of them - fakes contribution.\n",nTracks-nNotFakeTracks);
  }
  
  for(Int_t n=0; n<5; n++) if(nTimes[n]>0) {
    printf("%6i(%2iw.",listTracks[n],nTimes[n]);
    if(listTracks[n] > 0) {
      printf(" from %2iw.)",nDigiTimes[n]);
      if((trackStatus[n]&254) == 254)      printf(" Ok.");  // No metamatch flag any more!
      else {
        if((trackStatus[n]&2)   == 0)      printf(" FakeContribution");
        else {
          if((trackStatus[n]&2)      == 0) printf(" FakeSegment");
          if((trackStatus[n]&4)      == 0) printf(" ClFnLevelTooHigh");
          if((trackStatus[n]&16)     == 0) printf(" SegNotRec");
          else if((trackStatus[n]&8) == 0) printf(" 1or2HitsNotRec");
          if((trackStatus[n]&32)     == 0) printf(" TrackNotRec");
          if((trackStatus[n]&64)     == 0) printf(" NoMeta");
          if((trackStatus[n]&128)    == 0) printf(" GEANT_Bug");
        }
      }
    }
    else if(listTracks[n] == -500) printf(") Embeded real track"); /*gHades->getEmbeddingRealTrackId() - not static*/
    else if(listTracks[n] ==  -99) printf(") Noised wires");
    else if(listTracks[n] ==   -9) printf(") GEANT bug wires");
    else                           printf(") Not known track number!");
    printf("\n");
  }
}

void HMdcSegSim::setTrackInf(const Int_t *lst,const Short_t *nTms,
                             const UChar_t *nDTms,const UChar_t *tSt) {
  for(Int_t i=0;i<5;i++) {
    listTracks[i]  = lst[i];
    nTimes[i]      = nTms[i]; 
    nDigiTimes[i]  = nDTms[i];
    trackStatus[i] = tSt[i];
  }
}

UChar_t HMdcSegSim::getTrackStatus(Int_t n) const {
  return indOk(n) ? trackStatus[n] : 0;
}

Int_t HMdcSegSim::getGoodTrack(Int_t i1,HMdcSegSim* outerSeg,Int_t nWiresCut) const {
  // Function return geant track number if track this->listTracks[i1] marked as real 
  // in inner and outer segments
  // and segments have at least nWiresCut wires from this track in each segment.
  // Otherwise -  return 0.
  // "this" object must be inner segment
  // "outerSeg" object must be corresponding (not any one!) outer segment
  if(getIOSeg() != 0) {
    Error("getGoodTrack","call this function for inner segment only!");
    return 0;
  }
  if(outerSeg == 0 || outerSeg->getIOSeg() != 1) {
    Error("getGoodTrack","Parameter outerSeg must be outer segment!");
    return 0;
  }
  if(i1<0 || i1>=nNotFakeTracks) return 0;
  if(outerSeg->nNotFakeTracks<1) return 0;
  if(nTimes[i1]<nWiresCut)       return 0;
  for(Int_t i2=0;i2<outerSeg->nNotFakeTracks;i2++) {
    if(listTracks[i1] != outerSeg->listTracks[i2]) continue;
    if(outerSeg->nTimes[i2]<nWiresCut)             continue;
    return listTracks[i1];
  }
  return 0;
}

Int_t HMdcSegSim::getNextGoodTrack(Int_t& i1,HMdcSegSim* outerSeg,Int_t nWiresCut) const {
  // Function searchs the good track (see. funct.getGoodTrack) starting from index i1.
  // Function return geant track number and track index(i1) if searching was successful
  // otherwise -  return 0.
  // "this" object must be inner segment
  // "outerSeg" object must be corresponding (not any one!) outer segment
  if(getIOSeg() != 0) {
    Error("getGoodTrack","call this function for inner segment only!");
    return 0;
  }
  if(outerSeg == 0 || outerSeg->getIOSeg() != 1) {
    Error("getGoodTrack","Parameter outerSeg must be outer segment!");
    return 0;
  }
  if(outerSeg->nNotFakeTracks<1) return 0;
  if(i1<0) i1=0;
  if(i1>=nNotFakeTracks) return 0;
  for(;i1<nNotFakeTracks;i1++) {
    if(nTimes[i1]<nWiresCut) continue;
    Int_t track = listTracks[i1];
    for(Int_t i2=0;i2<outerSeg->nNotFakeTracks;i2++) {
      if(track != outerSeg->listTracks[i2]) continue;
      if(outerSeg->nTimes[i2]<nWiresCut)             continue;
      i1++;
      return track;
    }
  }
  return 0;
}

Int_t HMdcSegSim::findTrack(Int_t geantNum,Int_t nDgTimes) const {
  // return index of geantTrack in listTracks[]
  // return -1 if there no such track
  for(Int_t n=0;n<nTracks;n++) if(geantNum == listTracks[n]) {
     if(nDgTimes==0 || nDgTimes == nDigiTimes[n]) return n;
   }
   return -1;
}
    
Int_t HMdcSegSim::getTrackIndex(Int_t tr) const {
  for(Int_t i=0;i<nTracks;i++) if(listTracks[i] == tr) return i;
  return -1;
}

Bool_t HMdcSegSim::resetFakeContributionFlag(void) {
  // For procedure of fake suppression only!
  // For track listTracks[0] only.
  if( !isFakeContribution(0) || nTimes[0] < 5 || nTimes[0]*2 <= getSumWires() ) return kFALSE;
  trackStatus[0] |= 2; // Not fake
  nNotFakeTracks++;
  return kTRUE;
}

Bool_t HMdcSegSim::setFakeContributionFlag(void) {
  // For procedure of fake suppression only!
  // For track listTracks[0] only.
  if( isFakeContribution(0) ) return kFALSE;
  trackStatus[0] &= 253; //  Fake
  nNotFakeTracks--;
  return kTRUE;
}
