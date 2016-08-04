//*-- AUTHOR : Vladimir Pechenov
//*-- Modified : 08/02/2007 by V.Pechenov
//*-- Modified : 05/06/2002 by V.Pechenov
//*-- Modified : 30/05/2001 by V.Pechenov
//*-- Modified : 7/11/2000 by V.Pechenov


//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
// HMdcClusSim
//
// Data object for Mdc cluster.
//
//  Int_t getTrackIndex(Int_t tr) - return geant track "tr" index
//      in listTr[5] array or -1 if there no track "tr" in the array.
//
// Int_t getNTracks() - num. of tracks which have hits in cluster
//      (5 tracks maximum) (noise don't increase tracks counter)
//
// Int_t getTrack(Int_t indx) - return track number.
//      indx= 0, ..., (getNTracks()-1).
//      List of tracks is sorted: first track (indx=0) 
//      have most number of hits, ...
//      Track_number==-9 - wires produced due to geant bug
//      Track_number==gHades->getEmbeddingRealTrackId() -
//          real track embeded in simulation.
//
// Int_t getNoiseIndex(void) - return index of noise wires inf.
//      if index < 0 - no noise in cluster.
//      Track number for noise wires =-99
//      Example:
//      Int_t ind = clus->getNoiseIndex();
//      if(ind>0) {
//        clus->getTrack(ind);         // will return -99
//        clus->getNTimesInTrack(ind); // number of noise wires
//        ...
//      }
//
// Short_t getNTimesInTrack(Int_t indx) - num. of hits
//      in cluster from track getTrack(indx)
//
// Int_t getNLayersInTrack(Int_t indx, Int_t m=-1) num. layers 
//      with hits from track getTrack(indx):
//      m=0 - in first module of segment,
//      m=1 - in second module of segment,
//      m!=0 && m!=1 - in segment
//
// Track num.=-99 - noise wires
// Track num.=-9  - GEANT bug
//
//
// Meaning of trackStatus[tr] bits:
// ------------------------------------
//  Bit 8: =0 - GEANT bug was found in this track
//  Bit 7: =0 - track don't reach META
//  Bit 6: =0 - not reconstructable track. It can be if
//              track not present in mdc,
//              track cross more then one sector,
//              numder of segments <1 or >2,
//              upstream direction of at list one segment,
//              track doesn't cross inner or outer segment,
//              num. of segments with digi.wires <1 or >2,
//              num.digi.wires in track < 5*num.segments,
//              at list one seg. has <3 layers or <5 wires
//  Bit 5: =0 - segment has <3 layers or <5 digi.wires
//  Bit 4: =0 - at list one mdc in segment has
//              <3 fired layers or <5 wires
//  Bit 3: =0 - segment can't be found due to high level
//              of cluster finder
//  Bit 2: =0 - fake contribution in cluster
//  Bit 1: =0 - fake inner-outer segments matching
//              (for outer segment only!)
//
////////////////////////////////////////////////////////////////
using namespace std;
#include "hmdcclussim.h"
#include "hades.h"
#include "hmdctrackddef.h"
#include "hmdccal1sim.h"
#include "hevent.h"
#include "hmatrixcategory.h"
#include "hmdclistcells.h"
#include <iostream> 
#include <iomanip>

ClassImp(HMdcClusSim)


void HMdcClusSim::setDef(void) {
  nTracks        = 0;
  nTracksM[0]    = 0;
  nTracksM[1]    = 0;
  nNotFakeTracks = -1;
  ioSMatchStatus = -1;
}

void HMdcClusSim::cleanRest(void) {
  for(Int_t n=nTracks;n<5;n++) {
    listTr[n]      = 0;
    nTimes[n]      = 0;
    nLayers[n][0]  = 0;
    nLayers[n][1]  = 0;
    nDigiTimes[n]  = 0;
    trackStatus[n] = 0;
    xGeant[n]      = 0.;
    yGeant[n]      = 0.;
  }
}

Bool_t HMdcClusSim::resetTrack(Int_t ind, Int_t tr, Short_t nTm, UChar_t lM1, UChar_t lM2) {
  // reset geant track;
  if(indOk(ind)) {
    listTr[ind]     = tr;    
    nTimes[ind]     = nTm;    
    nLayers[ind][0] = lM1;
    nLayers[ind][1] = lM2;
    if(nTracks <= ind) nTracks = ind+1;
    return kTRUE;
  }
  return kFALSE;
}

Int_t HMdcClusSim::addTrack(Int_t tr, Short_t nTm, UChar_t lM1, UChar_t lM2) {
  // add geant track. Return index of track in array
  if(nTracks>=5) return -1;
  listTr[nTracks]     = tr;    
  nTimes[nTracks]     = nTm;    
  nLayers[nTracks][0] = lM1;
  nLayers[nTracks][1] = lM2;
  xGeant[nTracks]     = -10000.;
  yGeant[nTracks]     = -10000.;
  nTracks++;
  return nTracks-1;
}

void HMdcClusSim::setNDigiTimes(Int_t ind,Int_t nt) {
  if(indOk(ind)) {
    if(nt<0)        nDigiTimes[ind] = 0;
    else if(nt>255) nDigiTimes[ind] = 255;
    else            nDigiTimes[ind] = nt;
  }
}

void HMdcClusSim::setXYGeant(Int_t ind,Float_t xg,Float_t yg) {
  if(indOk(ind)) {
    xGeant[ind] = xg;
    yGeant[ind] = yg;
  }
}

void HMdcClusSim::setTrackStatus(Int_t ind,UChar_t status) {
  if(indOk(ind)) trackStatus[ind] = status;
}

UChar_t HMdcClusSim::getTrackStatus(Int_t ind) const {
  return indOk(ind) ? trackStatus[ind] : 0;
}

void HMdcClusSim::setNTracksM(Int_t modi, Int_t nTrs) {
  if(modi<0 || modi>1) return;
  nTracksM[modi] = nTrs;
}

void HMdcClusSim::setTrackM(Int_t modi, Int_t ind, Int_t tr, Short_t nTm) {
  if(modi<0 || modi>1) return;
  if(ind<0 || ind>4) return;
  listTrM[modi][ind] = tr;
  nTimesM[modi][ind] = nTm;
}


Int_t HMdcClusSim::getNoiseIndex(void) const {
  for(Int_t i=0;i<nTracks;i++) if(listTr[i] == -99) return i;
  return -1;
}
    
Int_t HMdcClusSim::getTrackIndex(Int_t tr) const {
  for(Int_t i=0;i<nTracks;i++) if(listTr[i]==tr) return i;
  return -1;
}

Int_t HMdcClusSim::getTrackMod(Int_t m,Int_t indx) const {
  return (indx>=0 && indx<nTracksM[m&1]) ? listTrM[m&1][indx] : -1;
}

Short_t HMdcClusSim::getNTimesInMod(Int_t m,Int_t indx) const {
  return (indx<0 || indx>=nTracksM[m&1]) ? 0:nTimesM[m&1][indx];
}


Int_t   HMdcClusSim::getTrack(Int_t indx) const {
  return (indx>=0&&indx<nTracks) ? listTr[indx] : 0;
}

Short_t HMdcClusSim::getNTimesInTrack(Int_t indx) const {
  return (indx<0||indx>=nTracks) ? 0:nTimes[indx];
}

void HMdcClusSim::calcTrList(void) {
  HCategory *fCalCat=gHades->getCurrentEvent()->getCategory(catMdcCal1);
  if( !fCalCat ) return;

  HLocation loc;
  loc.set(4,sec,0,0,0);
  Int_t nTr = 0;
  Int_t lsTr[100];
  Int_t ind[100];
  UChar_t  nLay[100][2];
  UChar_t nHt[100];
  for(Int_t lay=0; lay<12; lay++) {
    Int_t modInd=lay/6;
    Int_t cell=-1;
    Short_t add=1<<(lay%6);
    loc[1] = modInd+seg*2;
    loc[2] = lay%6;
    while((cell=next(lay,cell)) >= 0) {
      UChar_t nTms = getTime(lay,cell);
      loc[3] = cell;
      HMdcCal1Sim *sim=(HMdcCal1Sim*)fCalCat->getObject(loc);
      if(!sim) {
        Error("getNTracks()"," Can't get object HMdcCal1Sim for address [%i][%i][%i][ %i ]",
            loc[0]+1,loc[1]+1,loc[2]+1,loc[3]+1);
        continue;
      }
      Int_t track=0;
      for(Int_t t=0; t<2; t++) {
        if(t==0 && (nTms&1)) track=sim->getNTrack1();
        else if(t==1 && (nTms&2)) track=sim->getNTrack2();
        else continue;
        if(track == 0) continue;
        Int_t indx = nTr;
        if(track<0 && track!=gHades->getEmbeddingRealTrackId()) track = -99; // noise
        for(Int_t n=0; n<nTr; n++) {
          if(lsTr[n]==track) {
            indx = n;
            break;
          }
        }
        if(indx==nTr) {
          lsTr[indx]    = track;
          nLay[indx][0] = 0;
          nLay[indx][1] = 0;
          nHt[indx]     = 0;
          ind[indx]     = indx;
          nTr++;
        }
        nLay[indx][modInd] |= add;
        nHt[indx]++;
        
      }
    }
  }
  //Sorting:
  if(nTr>1) {
    Bool_t sort=kTRUE;
    while(sort) {
      sort=kFALSE;
      for(Int_t n=0; n<nTr-1; n++) if(nHt[ind[n]]<nHt[ind[n+1]]) {
        sort     = kTRUE;
        Int_t i  = ind[n];
        ind[n]   = ind[n+1];
        ind[n+1] = i;
      }
    }
  }
  nTracks = nTr>5 ? 5:nTr;
  for(Int_t n=0; n<nTracks; n++) {
    listTr[n]      = lsTr[ind[n]];
    nTimes[n]      = nHt[ind[n]];
    nLayers[n][0]  = nLay[ind[n]][0];
    nLayers[n][1]  = nLay[ind[n]][1];
    nDigiTimes[n]  = 0;
    trackStatus[n] = 255;
    xGeant[n]      = -10000.;
    yGeant[n]      = -10000.;
  }
  cleanRest();
}

void HMdcClusSim::calcTrListMod(HMdcList12GroupCells& list,Int_t m) {
  HCategory *fCalCat=gHades->getCurrentEvent()->getCategory(catMdcCal1);
  if( !fCalCat ) {
    nTracksM[0]=nTracksM[1]=0;
    return;
  }
  Int_t mod=m&1;
  
  HLocation loc;
  loc.set(4,sec,0,0,0);
  Int_t nTr=0;
  Int_t lsTr[100];
  Int_t ind[100];
  UChar_t nHt[100];
  for(Int_t lay=mod*6; lay<(mod+1)*6; lay++) {
    Int_t modInd=lay/6;
    Int_t cell=-1;
    loc[1]=modInd+seg*2;
    loc[2]=lay%6;
    while((cell=list.next(lay,cell)) >= 0) {
      UChar_t nTms=list.getTime(lay,cell);
      loc[3]=cell;
      HMdcCal1Sim *sim=(HMdcCal1Sim*)fCalCat->getObject(loc);
      if(!sim) {
        Error("getNTracks()"," Can't get object HMdcCal1Sim for address [%i][%i][%i][ %i ]",
            loc[0]+1,loc[1]+1,loc[2]+1,loc[3]+1);
        continue;
      }
      Int_t track=0;
      for(Int_t t=0; t<2; t++) {
        if(t==0 && (nTms&1)) track=sim->getNTrack1();
        else if(t==1 && (nTms&2)) track=sim->getNTrack2();
        else continue;
        Int_t indx=nTr;
        for(Int_t n=0; n<nTr; n++) {
          if(lsTr[n]==track) {
            indx=n;
            break;
          }
        }
        if(indx==nTr) {
          lsTr[indx]=track;
          nHt[indx]=0;
          ind[indx]=indx;
          nTr++;
        }
        nHt[indx]++;
      }
    }
  }
  //Sorting:
  if(nTr>1) {
    Bool_t sort=kTRUE;
    while(sort) {
      sort=kFALSE;
      for(Int_t n=0; n<nTr-1; n++) {
        if(nHt[ind[n]]<nHt[ind[n+1]]) {
          sort=kTRUE;
          Int_t i=ind[n];
          ind[n]=ind[n+1];
          ind[n+1]=i;
        }
      }
    }
  }
  nTracksM[mod]=(nTr>5) ? 5:nTr;
  for(Int_t n=0; n<nTracksM[mod]; n++) {
    listTrM[mod][n]=lsTr[ind[n]];
    nTimesM[mod][n]=nHt[ind[n]];
  }
}

Int_t HMdcClusSim::getNLayersInTrack(Int_t indx, Int_t m) const {
  if(indOk(indx)) {
    if(m==0 || m==1) return HMdcBArray::getNSet(nLayers[indx][m]);
    else return HMdcBArray::getNSet(nLayers[indx][0])+
                HMdcBArray::getNSet(nLayers[indx][1]);
  }
  return 0;
}

UChar_t HMdcClusSim::getLayListForMod(Int_t indx,Int_t mi) const {
  if(indOk(indx) && mi>=0 && mi<2) return nLayers[indx][mi];
  return 0;
}

UChar_t HMdcClusSim::getNLayOrientation(Int_t indx,Int_t mi) const {
  if(!indOk(indx)) return 0;
  UChar_t list;
  if(mi>=0 && mi<2) list = nLayers[indx][mi];
  else              list = nLayers[indx][0] | nLayers[indx][1];
  return HMdcBArray::getNLayOrientation(list);
}

Bool_t HMdcClusSim::is40degCross(Int_t indx,Int_t mi) const {
  // return kTRUE if exist at list one wire pair with 
  // 40deg. or more angle between.
  if(!indOk(indx)) return 0;
  UChar_t list;
  if(mi>=0 && mi<2) list = nLayers[indx][mi];
  else              list = nLayers[indx][0] | nLayers[indx][1];
  return  HMdcBArray::is40DegWireCross(list);
}

Bool_t HMdcClusSim::getXYGeant(Int_t i,Float_t& xg, Float_t& yg) const {
  if(indOk(i) && xGeant[i]>-9000.) {
    xg = xGeant[i];
    yg = yGeant[i];
    return kTRUE;
  }
  return kFALSE;
}

void HMdcClusSim::sortTrListByContr(void) {
  // Sorting track list "listTr" by "FakeContribution" flag.
  // After sorting tracks with not FakeContribution status
  // will be placed at the begining of arrais.
  nNotFakeTracks = 0;
  for(Int_t t1=0;t1<nTracks;t1++) {
    if(isFakeContribution(t1)) {
      for(Int_t t2=t1+1;t2<nTracks;t2++) {
        if(isFakeContribution(t2)) continue;
        exchangeTrPos(t1,t2);
        nNotFakeTracks++;
        break;
      }
    } else nNotFakeTracks++;
  }
  for(Int_t t1=0;t1<nNotFakeTracks-1;t1++) if(!isIOSegMatch(t1)) {
    for(Int_t t2=t1+1;t2<nNotFakeTracks;t2++) {
      if(!isIOSegMatch(t2)) continue;
      exchangeTrPos(t1,t2);
      break;
    }
  }
  ioSMatchStatus = 0;
  for(Int_t t=0;t<nNotFakeTracks;t++) if(isIOSegMatch(t)) ioSMatchStatus++;
  cleanRest();
}

Int_t HMdcClusSim::getNMatchedTracks(void) const {
  // Can be used after sortTrListByContr() only!
  Int_t n = 0;
  for(Int_t t=0; t<nNotFakeTracks; t++) {
    if(isFakeContribution(t)) continue;
    if(isIOSegMatch(t)) n++;
  }
  return n;
}

void HMdcClusSim::exchangeTrPos(Int_t t1,Int_t t2) {
  Int_t   v1 = listTr[t1];
  Short_t v2 = nTimes[t1];
  UChar_t v3 = nLayers[t1][0];
  UChar_t v4 = nLayers[t1][1];
  UChar_t v5 = nDigiTimes[t1];
  UChar_t v6 = trackStatus[t1];
  Float_t v7 = xGeant[t1];
  Float_t v8 = yGeant[t1];

  listTr[t1]      = listTr[t2];
  nTimes[t1]      = nTimes[t2];
  nLayers[t1][0]  = nLayers[t2][0];
  nLayers[t1][1]  = nLayers[t2][1];
  nDigiTimes[t1]  = nDigiTimes[t2];
  trackStatus[t1] = trackStatus[t2];
  xGeant[t1]      = xGeant[t2];
  yGeant[t1]      = yGeant[t2];

  listTr[t2]      = v1;
  nTimes[t2]      = v2;
  nLayers[t2][0]  = v3;
  nLayers[t2][1]  = v4;
  nDigiTimes[t2]  = v5;
  trackStatus[t2] = v6;
  xGeant[t2]      = v7;
  yGeant[t2]      = v8;
}

void HMdcClusSim::printCont(Bool_t fl) const {
  printf("  GEANT: %i tracks in cluster",nTracks);
  if(nNotFakeTracks==0) printf(".  FAKE CLUSTER!\n");
  else  {
    if(nNotFakeTracks>=0 && nNotFakeTracks != nTracks) 
        printf(",  %i of them - fakes contribution",nTracks-nNotFakeTracks);
    if(ioSMatchStatus > 0)
        printf(".  %i tr.have IOseg.matching.\n",ioSMatchStatus);
    else if(ioSMatchStatus < 0)  printf(".\n");
    else if(indCh1 >= 0 || indexPar >= 0)
                                 printf(".  IO seg.matching - FAKE!\n");
    else printf(".  No %s segments.\n",seg==0 ? "outer":"inner");
  }
  for(Int_t n=0; n<nTracks; n++) if(nTimes[n]>0) {
    printf("%6i(%2iw.",listTr[n],nTimes[n]);
    if(listTr[n] > 0) {
      printf(" from %2iw., lay.:",nDigiTimes[n]);
      for(Int_t lay=0;lay<6;lay++) printf("%i",(nLayers[n][0]>>lay)&1);
      printf(" ");
      for(Int_t lay=0;lay<6;lay++) printf("%i",(nLayers[n][1]>>lay)&1);
      printf(")");
      if(xGeant[n]>-9000. && yGeant[n]>-9000.) printf(" dX=%.1f dY=%.1f",dX(n),dY(n));
      if(trackStatus[n] == 255)            printf(" Ok.");
      else {
        if((trackStatus[n]&2)   == 0)      printf(" FakeContribution");
        else {
          if( (trackStatus[n]&1) == 0 &&
              (indCh1>=0 || indexPar>=0) ) printf(" FakeIOSegMatch");
          if((trackStatus[n]&2)      == 0) printf(" FakeCluster");
          if((trackStatus[n]&4)      == 0) printf(" ClFnLevelTooHigh");
          if((trackStatus[n]&16)     == 0) printf(" SegNotRec");
          else if((trackStatus[n]&8) == 0) printf(" 1or2HitsNotRec");
          if((trackStatus[n]&32)     == 0) printf(" TrackNotRec");
          if((trackStatus[n]&64)     == 0) printf(" NoMeta");
          if((trackStatus[n]&128)    == 0) printf(" GEANT_Bug");
        }
      }
    }
    else if(listTr[n] == -500) printf(") Embeded real track"); /*gHades->getEmbeddingRealTrackId() - not static*/
    else if(listTr[n] ==  -99) printf(") Noised wires");
    else if(listTr[n] ==   -9) printf(") GEANT bug wires");
    else                       printf(") Not known track number!");
    printf("\n");
  }
  for(Int_t m=0;m<2;m++) if(nTracksM[m]>0) {
    printf("         %i tracks in Mod%i:",nTracksM[m],m+seg*2+1);
    for(Int_t n=0; n<nTracksM[m]; n++) printf(" %i(%i)",listTrM[m][n],nTimesM[m][n]);
    printf("\n");
  }
  if(fl) HMdcClus::printCont(fl);
}
