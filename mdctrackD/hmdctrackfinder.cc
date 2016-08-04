//*-- Author : V.Pechenov
//*-- Modified : 07.05.2002 by V.Pechenov
//*-- Modified : 17.01.2002 by V.Pechenov
//*-- Modified : 12.05.2001 by V.Pechenov

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////
// HMdcTrackFinder
//
// The program for candidates to tracks finding.
//
// Using:
// Put in task list task:
// HMdcTrackFinder* trackFinder;
// ...
// tasks->connect((trackFinder=new HMdcTrackFinder("MdcTrackD","MdcTrackD",
//                                     isCoilOff,typeClustFinder)),"...");
//
// isCoilOff=kTRUE(kFULSE) - The current in coil is turn off(on) for this run(s).
// typeClustFinder=1(0) - type of the work of cluster finder
//
// The parameters isCoilOff and typeClustFinder define the type
// of the work of cluster finder (default values of isCoilOff
// and typeClustFinder are kFALSE and 0):
//
//    isCoilOff=kTRUE - the tracks in all MDC will finded to respect of target.
//    isCoilOff=kFALSE - the tracks in MDC1 & MDC2 will finded to respect
//      of target. In MDC3 & MDC4 - to respect of the the point on kick plane.
//
//    typeClustFinder=0 - the tracks will finded in segment or sector
//      (needs in a alignment of MDC's).
//
// 1. isCoilOff=kTRUE, typeClustFinder=1:
//      The tracks will be searched in each MDC (1-4) independently
// 2. isCoilOff=kTRUE, typeClustFinder=0:
//      The tracks will be searched in all MDC of sector together to
//    respect of target (needs an alignment of MDC's).
// 3. isCoilOff=kFALSE, typeClustFinder=1:
//      The tracks will be searched in MDC1, MDC2 (segment 1) independently,
//      in MDC3, MDC4 (segment 2) together.
// 4. isCoilOff=kFALSE, typeClustFinder=0:
//      The tracks will be searched in MDC1, MDC2 (segment 1) together
//      (needs an alignment of MDC's).
// 5. isCoilOff=kFALSE, typeClustFinder=2:
//      The tracks will be searched in MDC1, MDC2 (segment 1) together
//      and after it in MDC1, MDC2 (segment 1) independently.
// For isCoilOff=kFALSE the program needs HKickPlane parameters
//
// The member function
// trackFinder->setNLayers(nLayers);
// set the number of woring planes in MDC's
// Int_t nLayers[6][4]={ {3,6,6,6}, {6,6,6,6}, {6,6,6,6},
//                       {4,6,6,6}, {6,6,6,6}, {5,6,6,6} };
//
//
// The output of the program is container HMdcClus.
// For writing of HMdcClus to ROOT file use function:
// trackFinder->setPersitency(kTRUE);
//                           (put this line in macros befor makeTree()!)
//
// HMdcClusterToHit task copy the HMdcClus data to HMdcHit and HMdcSeg
//
// The HMdcHit and HMdcSeg containers are filled by HMdc12Fit task.
//
//////////////////////////////////////////////////////////////////////

#include "hevent.h"
#include "hmdctrackfinder.h"
#include "hmdctrackddef.h"
#include "hiterator.h"
#include "hcategory.h"
#include "hmdcgetcontainers.h"
#include "hmdcclussim.h"
#include "hmdclookuptb.h"
#include "hmdc34clfinder.h"
#include "hmdctrackdset.h"
#include "hmdclistcells.h"
#include "hmdcgeanttrack.h"
#include "hmdcclusmetamatch.h"
#include "hmdcsegsim.h"

HMdcTrackFinder::HMdcTrackFinder(Bool_t isCOff, Int_t typeClFn) {
  HMdcTrackDSet::setMdcTrackFinder(isCOff,typeClFn);
  setParContainers();
}

HMdcTrackFinder::HMdcTrackFinder(const Text_t *name, const Text_t *title,
     Bool_t isCOff, Int_t typeClFn) : HReconstructor(name,title) {
  HMdcTrackDSet::setMdcTrackFinder(isCOff,typeClFn);
  setParContainers();
}

HMdcTrackFinder::HMdcTrackFinder(void) {
  setParContainers();
}

HMdcTrackFinder::HMdcTrackFinder(const Text_t *name, const Text_t *title) :
    HReconstructor(name,title) {
  setParContainers();
}

HMdcTrackFinder::~HMdcTrackFinder() {
  HMdcEvntListCells::deleteCont();
  HMdcGeantEvent::deleteCont();
  HMdcLookUpTb::deleteCont();
  HMdc34ClFinder::deleteCont();
  HMdcTrackDSet::clearPTrackFinder();
  if(iterClus) {
    delete iterClus;
    iterClus = 0;
  }
  if(pClusMetaMatch != NULL) delete pClusMetaMatch;
  pClusMetaMatch = NULL;
}

void HMdcTrackFinder::setParContainers() {
  HMdcTrackDSet::setTrackFinder(this);
  isClustToHit    = kFALSE;
  event           = NULL;
  isMdcLCellsOwn  = kFALSE;
  geantEvent      = NULL;
  isGeantEventOwn = kFALSE;
  prntSt          = kFALSE;
  isInitialised   = kFALSE;
  fLookUpTb       = NULL;
  p34ClFinder     = NULL;
  level4[0]       = 0;
  level5[0]       = 0;
  level4[1]       = 0;
  level5[1]       = 0;
  fClusCat        = NULL;
  iterClus        = NULL;
  fSegCat         = NULL;
  perMdcClus      = kFALSE;
//  nBinXS1         = 319;  //!!!!!!!!!!!!319!
//  nBinYS1         = -1;   // If <=1 nBinYS1 will be calculated from nBinXS1
  pClusMetaMatch  = NULL;
  tarSFinderOnly  = kFALSE;
  locClus.set(2,0,0);
  locNewCl.set(3,0,0,0);
  for(Int_t s=0;s<6;++s) for(Int_t m=0;m<4;++m) numLayers[s][m] = 0;
}

Bool_t HMdcTrackFinder::init(void) {
  if(event == 0)      event      = HMdcEvntListCells::getObject(isMdcLCellsOwn);
  if(geantEvent == 0) geantEvent = HMdcGeantEvent::getObject(isGeantEventOwn);
  
  fClusCat = HMdcGetContainers::getObject()->getCatMdcClus(kTRUE);
  if( !fClusCat ) return kFALSE;
  fClusCat->setPersistency(perMdcClus);
  iterClus = (HIterator *)fClusCat->MakeIterator();

  fLookUpTb = HMdcLookUpTb::getObject();

  p34ClFinder = HMdcTrackDSet::isCoilOff() ? NULL : HMdc34ClFinder::getObject();
  isClustToHit  = HMdcTrackDSet::getMdcClusterToHit() != NULL;
  if(geantEvent!=NULL && isClustToHit) {
    fSegCat = gHades->getCurrentEvent()->getCategory(catMdcSeg);
  } else fSegCat = NULL;

  if( !initMetaMatch() ) return kFALSE;
  return kTRUE;
}

Bool_t HMdcTrackFinder::initMetaMatch(void) {
  if(p34ClFinder == NULL) return kTRUE;
  Bool_t fillMMPlot;       // fill plots in ClustMetaMatch
  Bool_t doMetaMatch = HMdcTrackDSet::doMetaMatching(fillMMPlot);
  if( !doMetaMatch ) return kTRUE;
  if(pClusMetaMatch == NULL) {
    pClusMetaMatch = new HMdcClusMetaMatch;
    if(!pClusMetaMatch->init()) return kFALSE;
    p34ClFinder->doMetaMatch(pClusMetaMatch);
  }
  if(fillMMPlot) pClusMetaMatch->setFillPlotsFlag();
  return kTRUE;
}

Bool_t HMdcTrackFinder::reinit(void) {
  if(pClusMetaMatch!=NULL && !pClusMetaMatch->reinit()) return kFALSE;
  
  fLookUpTb->setIsCoilOffFlag(HMdcTrackDSet::isCoilOff());
  Int_t typeClFinder = HMdcTrackDSet::getClFinderType();
  fLookUpTb->setTypeClFinder(typeClFinder);
  if(!fLookUpTb->initContainer()) return kFALSE;
  
  if( p34ClFinder && !p34ClFinder->initContainer(*event) ) return kFALSE;
  if( isInitialised ) return kTRUE;
  
  // Parameters from HMdcTrackDSet:
  for(Int_t s=0;s<6;++s) for(Int_t m=0;m<4;++m) numLayers[s][m] = HMdcTrackDSet::getNLayersInMod(s,m);
  level4[0] = HMdcTrackDSet::getLevel4(0);
  level5[0] = HMdcTrackDSet::getLevel5(0);
  level4[1] = HMdcTrackDSet::getLevel4(1);
  level5[1] = HMdcTrackDSet::getLevel5(1);
  Int_t nModsF = (HMdcTrackDSet::isCoilOff()) ? 4:2;
  
  for(Int_t sec=0; sec<6; sec++) {
    HMdcLookUpTbSec* pLookUpTbSec=&((*fLookUpTb)[sec]);
    for(Int_t mod=0; mod<nModsF; mod++) {
      HMdcLookUpTbMod* pLookUpTbMod = pLookUpTbSec ? &((*pLookUpTbSec)[mod]) : NULL;
      if( pLookUpTbMod ) pLookUpTbMod->setNLayers(numLayers[sec][mod]);
      else numLayers[sec][mod] = 0;
    }
    HMdc34ClFinderSec* p34ClFinderSec = p34ClFinder ? &((*p34ClFinder)[sec]) : NULL;
    if(p34ClFinderSec) p34ClFinderSec->setTypeClFinder(typeClFinder);
    for(Int_t mod=nModsF; mod<4; mod++) {
      if(p34ClFinderSec==NULL || (&((*p34ClFinderSec)[mod]))==0) numLayers[sec][mod] = 0;
    }
  }
  isInitialised = kTRUE;
  if(!prntSt) printStatus();
  return kTRUE;
}

void HMdcTrackFinder::setPersistency(Bool_t perCl) {
  perMdcClus=perCl;
  if(fClusCat) fClusCat->setPersistency(perMdcClus);
}


Int_t HMdcTrackFinder::execute(void) {
  Int_t nFiredWires = 0;
  if(isMdcLCellsOwn) {
    event->clear();
    nFiredWires = event->collectWires();
  } else nFiredWires = event->getNCells();
  if(isGeantEventOwn) geantEvent->collectTracks(); // MUST be after collectWires()!
  if(nFiredWires<3) return 0;                      // MUST be after collectTracks()!
 
  calcClFndrLevel();             // Cluster finder levels determination
  
  Int_t numcl = clFndrBeforField(); // Cl.finder in MDCI-II/all(without mag.field)
  if(numcl<0) return 0;  // No reconstruction for this event

  if(pClusMetaMatch != NULL) pClusMetaMatch->collectMetaHits();

  if(p34ClFinder != NULL) {              // Track finder in MDCIII-IV (with mag.field)
    p34ClFinder->clear();
    setMod34Levels();
    if(isClustToHit) numcl += findOuterSegClusters();
  }
  if(geantEvent && isClustToHit) sortTracksInClusters(); //otherwise 12Fit do it
  return 0;
}

void HMdcTrackFinder::calcClFndrLevel(void) {
  // Cluster finder levels determination:
  for(Int_t sec=0; sec<6; sec++) {
    Int_t* levelSec = clFndrLevel[sec];
    for(Int_t mod=0; mod<4; mod++) {
      levelSec[mod] = numLayers[sec][mod];
      if( levelSec[mod]>0 ) {
        Int_t occup=((*event)[sec][mod].getNCells()*6)/levelSec[mod];
	if( occup <= level5[mod/2] ) levelSec[mod]--;
	if( occup <= level4[mod/2] ) levelSec[mod]--;
	if( occup < 3 && HMdcTrackDSet::getClFinderType()==0) levelSec[mod]=3;
        if( levelSec[mod]<3 ) levelSec[mod]=3;
        
        if( HMdcTrackDSet::getClFinderType()==2 && mod>1) {
          if(numLayers[sec][mod]<4) {
            levelSec[mod]=numLayers[sec][mod]-1;
            if(levelSec[mod]<0) levelSec[mod]=0;
          }
        }
      }
    }
  }
}

Int_t HMdcTrackFinder::clFndrBeforField(void) {
  // Track finder in MDCI-II or all MDC's without mag.field
  fLookUpTb->clear();
  
  fLookUpTb->calcTdcDrDist();

  Int_t verTar = fLookUpTb->findVertex();
  if(verTar == -100) return -1; // No reconstruction for this event
//   if(verTar >= 0 && geantEvent != NULL) { //!dlya vodorodnoy misheni ne idet!
//     Int_t gntTar = geantEvent->getTargetNum();
//     if(gntTar != verTar) { // Print ...............
//       Int_t *vertexStat  = fLookUpTb->getVertexStat();
//       Int_t  nVertexPnts = fLookUpTb->getNVertexPnts();
//       printf("GeantTarget=%2i ReconstructedTarget=%2i G-R=%3i: ",gntTar,verTar,vertexStat[gntTar+1]-vertexStat[verTar+1]);
//       for(Int_t nt=0;nt<nVertexPnts;nt++) {
//         printf(" %4i",vertexStat[nt]);
//         if(gntTar+1 == nt) printf("G");
//         else if(verTar+1 == nt) printf("R");
//         else printf(" ");
//       }
//       printf("\n");
//     }
//   }
  if( tarSFinderOnly ) return 0;
  
  Int_t numcl = 0;
  for(Int_t sec=0; sec<6; sec++) if(&(*fLookUpTb)[sec] != NULL)
      numcl += (*fLookUpTb)[sec].findClusters(clFndrLevel[sec]);
  return numcl;
}

void HMdcTrackFinder::setMod34Levels(void) {  
  for(Int_t sec=0; sec<6; sec++) {
    HMdc34ClFinderSec &fSec = (*p34ClFinder)[sec];
    if( (&fSec) == NULL ) continue;
    fSec.resetCounter();
    fSec.setMinBin(clFndrLevel[sec]);
  }
}

Int_t HMdcTrackFinder::findOuterSegClusters(void) {
  // This function is used if segment fitter is not used.
  Int_t numcl=0;
  iterClus->Reset();
  for(Int_t sec=0; sec<6; sec++) {
    HMdc34ClFinderSec &fSec = (*p34ClFinder)[sec];
    if( !(&fSec) ) continue;
    if(fSec.notEnoughWires()) continue;
    locClus[0]=sec;
    iterClus->gotoLocation(locClus);
    HMdcClus* clus;
    while ((clus=(HMdcClus *)iterClus->Next())!=0) {
      Int_t typeClFn = clus->getTypeClFinder();
      Int_t mod      = clus->getMod();
      if(typeClFn == 1) {
        if(HMdcTrackDSet::getModForSeg1() != mod) continue;
      } else if(typeClFn==2 && mod>=0) {
        //                                           mixFitCut for fit!
        if(clus->getNLayersMod(mod^1)<HMdcTrackDSet::getMixClusCut()) continue;
      }
      numcl+=fSec.findClustersSeg2(clus);
    }
  }
  return numcl;
}

void HMdcTrackFinder::sortTracksInClusters(void) {
  // For isClustToHit == kTRUE only!
  // Soting geant track information in clusters and segments:
  Bool_t isSetted[5];
  iterClus->Reset();
  HMdcClusSim* pClusSim;
  while ((pClusSim=(HMdcClusSim*)iterClus->Next())!=0) {
    pClusSim->sortTrListByContr();
    Int_t segInd = pClusSim->getSegIndex();
    if(segInd<0) continue;
    HMdcSegSim* pSegSim = (HMdcSegSim*)(fSegCat->getObject(segInd));
    if(pSegSim == NULL) {
      Error("execute","Pointer to HMdcSeg object eq.0! (HMdcSeg index=%i)",segInd);
      continue;
    }
    if(pSegSim->getSumWires() == pClusSim->getNCells()) { // No wires removed
      pSegSim->setNTracks(pClusSim->getNTracks(),pClusSim->getNNotFakeTracks(),
                          pClusSim->getIOSegMatchingStatus());
      pSegSim->setTrackInf(pClusSim->getTrackList(),pClusSim->getNumWires(),
                           pClusSim->getNDigiTimes(),pClusSim->getTrackStatus());
    } else {
      Int_t nClTracks  = pClusSim->getNTracks();
      Int_t nSegTracks = pSegSim->getNTracks();
      for(Int_t i=0;i<nSegTracks;i++) isSetted[i] = kFALSE;
      for(Int_t nc=0;nc<nClTracks;nc++) {
        Int_t   track    = pClusSim->getTrack(nc);
        Short_t nTimes   = pClusSim->getNTimesInTrack(nc);
        UChar_t nDigiTms = pClusSim->getNDigiTimesInTrack(nc);
        for(Int_t ns=0;ns<nSegTracks;ns++) if(pSegSim->getTrack(ns) == track) {
          if(isSetted[ns]) continue;
          if(nDigiTms != pSegSim->getNDigiTimes(ns)) continue;
          if(nTimes   <  pSegSim->getNTimes(ns)) continue;
          pSegSim->setTrackStatus(ns,pClusSim->getTrackStatus(nc));
          isSetted[ns]=kTRUE;
          break;
        }
      }
      pSegSim->sortTrListByContr();
    }
  }
}

void HMdcTrackFinder::setNLayers(Int_t *lst) {
  HMdcTrackDSet::setTrFnNLayers(lst);
  for(Int_t sec=0; sec<6; sec++) for(Int_t mod=0; mod<4; mod++) {
    Int_t modInd = sec*4+mod;
    if(lst[modInd]>3 && lst[modInd]<=6) numLayers[sec][mod] = lst[modInd];
  }
}

void HMdcTrackFinder::setLevel(Int_t l4s1, Int_t l5s1, Int_t l4s2, Int_t l5s2) {
  HMdcTrackDSet::setTrFnLevel(l4s1,l5s1,l4s2,l5s2);
  level4[0]=l4s1; 
  level5[0]=l5s1; 
  level4[1]=l4s2; 
  level5[1]=l5s2;
}

void HMdcTrackFinder::setLevelSeg1(Int_t l4, Int_t l5) {
  HMdcTrackDSet::setTrFnSegLevel(0,l4,l5);
  level4[0]=l4; 
  level5[0]=l5;
}

void HMdcTrackFinder::setLevelSeg2(Int_t l4, Int_t l5) {
  HMdcTrackDSet::setTrFnSegLevel(1,l4,l5);
  level4[1]=l4; 
  level5[1]=l5;
}

void HMdcTrackFinder::printStatus(void) {
  // prints the parameters to the screen
  Int_t *l = &numLayers[0][0];
  Bool_t isInited = kFALSE;
  for(Int_t i=0; i<24&&!isInited; i++) if(l[i]>0) isInited = kTRUE;
  if( !isInited ) return;
  const Char_t* hCoil ="0 = MagnetOn,  1 = MagnetOff";
  const Char_t* hType ="0 = combined chamber clusters, 1 = single cham. cl., 2 = mixed cl.(0+1)";
  const Char_t* hLevS1="max. occupancy of MDC for level of cl.finding 4 & 5 layers";
  const Char_t* hLevS2="max. occupancy of MDC for level of cl.finding 4 & 5 layers";
  printf("--------------------------------------------------------------------------------------------\n");
  printf("HMdcTrackFinderSetup:\n");
  printf("MagnetStatus    =  %i :  %s\n",HMdcTrackDSet::isCoilOff(),hCoil);
  printf("ClustFinderType =  %i :  %s\n",HMdcTrackDSet::getClFinderType(),hType);
  printf("LevelsSegment1  =  %i,%i  : %s\n",level4[0],level5[0],hLevS1);
  printf("LevelsSegment2  =  %i,%i  : %s\n",level4[1],level5[1],hLevS2);
  printf("NumLayersPerMod = {{%i,%i,%i,%i},\n",l[ 0],l[ 1],l[ 2],l[ 3]);
  printf("                   {%i,%i,%i,%i},\n",l[ 4],l[ 5],l[ 6],l[ 7]);
  printf("                   {%i,%i,%i,%i},\n",l[ 8],l[ 9],l[10],l[11]);
  printf("                   {%i,%i,%i,%i},\n",l[12],l[13],l[14],l[15]);
  printf("                   {%i,%i,%i,%i},\n",l[16],l[17],l[18],l[19]);
  printf("                   {%i,%i,%i,%i}}\n",l[20],l[21],l[22],l[23]);
  printf ("--------------------------------------------------------------------------------------------\n");
  prntSt = kTRUE;
}

ClassImp(HMdcTrackFinder)
