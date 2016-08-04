#ifndef HMDCGEANTTRACK_H
#define HMDCGEANTTRACK_H

#include "TObject.h"
#include "TObjArray.h"
#include "hlocation.h"
#include "hmdclistgroupcells.h"
#include "hgeomvector.h"

class HCategory;
class HIterator;
class HGeantKine;
class HGeantMdc;
class HMdcEvntListCells;
class HMdcClusSim;
class HMdcSegSim;
class HMdcHitSim;
class HMdcWiresArr;

class HMdcGeantSeg : public HMdcList12GroupCells {
protected:
  Char_t       sec;             // address of each mdc segment
  Char_t       ioseg;           // ...
  Char_t       mod;             // mod = -2 for two mdc segment
  Int_t        trackNumber;     // GEANT track number
  HGeantMdc*   geantLay[2][7];  // [mod%2][lay] lay=6 - mid-plane mdc
  Short_t      segNumber;       // segment sequential number in track
  Char_t       nHitsMdc[2];     // num. of geant hits in each mdc (layers only!)
  Char_t       direction;       // segment direction (+1 - good one)
  Bool_t       areWiresColl;    // =kTRUE if wires are collected
  UChar_t      segmentStatus;   // bits array (see hmdcgeanttrack.cc)

  HMdcClusSim* pClusBest;       // the best cluster for this geant seg.
  UChar_t      trIndBest;       // index of track in HMdcClusSim
  Float_t      dX;              // Xgeant-Xcluster
  Float_t      dY;              // Ygeant-Ycluster
  Short_t      nLayers;         // number of track layers in cluster
  UChar_t      nWires;          // number of track wires in cluster
  HMdcClusSim* pClusBestCh[2];  // For chamber clusters
  UChar_t      trIndBestCh[2];  // index of track in HMdcClusSim
  Float_t      dXCh[2];         // Xgeant-Xcluster
  Float_t      dYCh[2];         // Ygeant-Ycluster
  Short_t      nLayersCh[2];    // number of track layers in cluster
  UChar_t      nWiresCh[2];     // number of track wires in cluster

  HMdcSegSim*  pSegBest;        // the best MdcSeg for this geant seg.
  UChar_t      sTrIndBest;      // index of track in HMdcSegSim
  Float_t      chi2;            // chi2
  Short_t      sNLayers;        // number of track layers in segment
  UChar_t      sNWires;         // number of track wires in segment
  Int_t        nRestWires;      // Num.WiresInSegment - sNWires

  Int_t        userFlag;        // User's flag
  
public:
  HMdcGeantSeg(Short_t ns=-1);
  ~HMdcGeantSeg(void) {}
  void    clear(Short_t ns);
  void    setAddress(Char_t sc,Char_t ios) {sec=sc; ioseg=ios;}
  void    setDirection(Char_t hitDir)      {direction=hitDir;}
  void    setWiresAreColl(void)            {areWiresColl=kTRUE;}
  void    setWiresAreNotColl(void)         {areWiresColl=kFALSE;}
  void    setMod(Char_t m)                 {mod=m;}
  void    analyseClust(HMdcClusSim* pClus,Int_t trInd,Int_t modICl);
  void    analyseSeg(HMdcSegSim* pSeg,Int_t trInd,HMdcList12GroupCells& wrList,Float_t ch2,Int_t nR);

  Char_t  getSec(void) const               {return sec;}
  Char_t  getIOSeg(void) const             {return ioseg;}
  Char_t  getMod(void) const               {return mod;}
  Int_t   getTrack(void) const             {return trackNumber;}
  Char_t  getDirection(void) const         {return direction;}
  Char_t  getNGMdcHits(void) const         {return nHitsMdc[0]+nHitsMdc[1];}
  Int_t   getNGMdcs(void) const;
  Char_t  getNGMdcHits(Char_t m) const     {return nMdcOk(m) ? nHitsMdc[m%2]:0;}
  UChar_t getStatus(void) const            {return segmentStatus;}
  Bool_t  areWiresCollected(void) const    {return areWiresColl;}
  Int_t   getFirstLayer12(void) const;
  Int_t   getLastLayer12(void) const;
  Int_t   getFirstGeantMdcLayer(Int_t m) const;
  Int_t   getLastGeantMdcLayer(Int_t m) const;
  Bool_t  getFirstAndLastGMdcLayers(Int_t m,Int_t& lFisrt,Int_t& lLast) const;
  Int_t   getModIOfGeantTrack(void) const;
  HGeantMdc* getMdcMidPlaneHit(Int_t m) {return nMdcOk(m) ? geantLay[m%2][6]:0;}
  HGeantMdc* getMdcLayerHit(Int_t m,Int_t l);
  void print(void);
  static Char_t dirHit(HGeantMdc* pGeantMdc);
  Bool_t  getMdcHitPos(Int_t m, HGeomVector& hit);
  Bool_t  getMdcHitPosSec(Int_t m, HGeomVector& hit);
  Bool_t  getMdcHitPosLab(Int_t m, HGeomVector& hit);
  Bool_t  getLayerHitPos(Int_t m,Int_t l,HGeomVector& hit,Bool_t extrFlag=kTRUE);
  Bool_t  getLayerHitPosSec(Int_t m,Int_t l,HGeomVector& hit,Bool_t extrFlag=kTRUE);
  Bool_t  getLayerHitPosLab(Int_t m,Int_t l, HGeomVector& hit,Bool_t extrFlag=kTRUE);
  Bool_t  isGntTrackOk(void) const      {return testBit(255);}
  Bool_t  isGEANTBug(void) const        {return !testBit(128);}
  Bool_t  isNoMeta(void) const          {return !testBit(64);}
  Bool_t  isTrackNotRec(void) const     {return !testBit(32);}
  Bool_t  isSegNotRec(void) const       {return !testBit(16);}
  Bool_t  is1or2HitsNotRec(void) const  {return !testBit(8);}
  void    clearClus(void)               {pClusBest      = NULL;
                                         pClusBestCh[0] = NULL;
                                         pClusBestCh[1] = NULL;}
  void    clearSeg(void)                {pSegBest       = NULL;}
  void    setUserFlag(Int_t fl)         {userFlag=fl;}
  Int_t   getUserFlag(void) const       {return userFlag;}
  static Char_t dirTheta(Float_t th)    {return (th<=90.) ? +1 : -1;}
  
  // Next functions are NOT for user:
  void    setAnotherHit(HGeantMdc* pGeantMdc);
  Bool_t  addHit(HGeantMdc* pGeantMdc);
  void    addFirstHit(HGeantMdc* pGeantMdc,Bool_t* mdcSecSetup,Short_t ns);
  void    setStatusFlags(UChar_t& trackStatus);
  void    setTrackNumber(Int_t trk)      {trackNumber = trk;}
  
protected:
  Bool_t  nMdcOk(Int_t m) const         {return m>=0 && m<4 && ioseg==m/2;}
  Int_t   mdcInd(Int_t m) const         {return (m%2)*7+6;}
  void    calcMdcHitPos(Int_t modI, Int_t lay1, Int_t lay2, HGeomVector& hit,
                        Int_t lay=6);
  Bool_t  testBit(UChar_t bit) const    {return (segmentStatus&bit)==bit;}
  Bool_t  isSegClusBetter(HMdcClusSim* pClus,Int_t trInd);
  Bool_t  isModClusBetter(HMdcClusSim* pClus,Int_t trInd,Int_t mInd);
  Int_t   setModClustPos(HMdcClusSim* pMdcClusSim,Int_t indtr,Int_t mInd);
  Int_t   setSegClustPos(HMdcClusSim* pMdcClusSim,Int_t indtr);
  Bool_t  isSegBetter(HMdcSegSim* pSeg,Int_t trInd,Int_t nLayersN,Float_t chi2N,Int_t nR);

  ClassDef(HMdcGeantSeg,0) // Mdc GEANT segments
};

class HMdcGeantTrack : public TObject {
protected:
  HCategory*      pMdcCal1Cat;    // category MdcCal1
  HIterator*      iterMdcCal1;    // iterator for MdcCal1 category
  HLocation       locMdcCal1;     // location of HMdcCal1Sim object
  static Bool_t   mdcSetup[6][4]; // [sec][mod]=kFALSE - skip this MDC

  HGeantKine*     pGeantKine;     // pointer to HGeantKine object
  Int_t           trackNumber;    // GEANT track number
  Float_t         mom;            // momentum (in MeV/c)
  Bool_t          isInMdcFlag;    // =kTRUE if GEANT track has hits in MDC's
  Bool_t          isInRichFlag;   // =kTRUE if GEANT track has hits in RICH
  Bool_t          isInMetaFlag;   // =kTRUE if GEANT track reach META

  TObjArray*      segments;       // array of HMdcGeantSeg objects
  Int_t           arrSizeLoc;     // size of segments array
  Int_t*          arrSize;        // size of segments array
  Int_t           arrOffset;      // offset of this track first segment in array
  Int_t*          arrGlobOffset;  // global offset in array for this track
  Short_t         nGeantMdcHits;  // number HGeantMdc hits in track
  Short_t         nSegments;      // number of segments
  Int_t           nWires;         // number of wires in track (=-1 no MdcCal1Cat)
  Short_t         nWSegments;     // number of segments with wires from track
  Char_t          mdcSector;      // if <0 - num.of crossed sectors
  Char_t          mdcWSector;     // the same as mdcSector but num.of wires>0
  Char_t          nIOSeg;         // =1,2,3 - inner seg.only, outer only, both
  Char_t          nWIOSeg;        // the same as nIOSeg but num.of wires>0
  Bool_t          directionFlag;  // =kTRUE if all segments have dir.=+1
  Bool_t          geantBugFlag;   // =kTRUE if geant bug was found
  UChar_t         trackStatus;    // bits array (see hmdcgeanttrack.cc)

  Bool_t          debugPrintFlag;
  HGeantMdc*      pGeantMdc;      // current GeantMdc hit
  HGeantMdc*      pGeantMdcPrev;  // previous GeantMdc hit
  HMdcGeantSeg*   segment;        // pointer to current segment

  Int_t           userFlag;       // User's flag
  
public:
  HMdcGeantTrack(void);
  HMdcGeantTrack(TObjArray* arr,Int_t* size,Int_t* offst);
  ~HMdcGeantTrack(void);
  void clear(void);
  void setDefault(void);

//  void   setMdcEvntListCells(HMdcEvntListCells* pt=0);
  Bool_t        setMdcCal1Cat(void);
  static void   unsetMdc(Int_t s, Int_t m);
  static Bool_t isMdcActive(Int_t s, Int_t m) {return mdcSetup[s][m];}
  static void   testMdcSetup(void);
  void          setDebugPrintFlag(Bool_t fg=kTRUE) {debugPrintFlag=fg;}

  Short_t       testGeantTrack(HGeantKine* pGK);
  Short_t       testGeantTrack(Int_t trNum);

  HGeantMdc*    getMdcMidPlaneHit(Int_t nseg,Int_t mod);
  HGeantMdc*    getMdcLayerHit(Int_t nseg,Int_t mod,Int_t lay);
  HMdcGeantSeg* getSegment(Int_t ns)            {return nSegOk(ns) ?
                                   (HMdcGeantSeg*)segments->At(ns+arrOffset):0;}
  Int_t         getTrack(void) const            {return trackNumber;}
  Float_t       getMomentum(void) const         {return mom;}
  Bool_t        isInMdc(void) const             {return isInMdcFlag;}
  Bool_t        isInRich(void) const            {return isInRichFlag;}
  Bool_t        isInMeta(void) const            {return isInMetaFlag;}
  Short_t       getNSegments(void) const        {return nSegments;}
  Short_t       getNDigiSegments(void) const    {return nWSegments;}
  Short_t       getNGeantMdcHits(void) const    {return nGeantMdcHits;}
  Int_t         getNWires(void) const           {return nWires;}
  Char_t        getGeantMdcSector(void) const   {return mdcSector;}
  Char_t        getDigiMdcSector(void) const    {return mdcWSector;}
  Char_t        getNGeantMdcSectors(void) const {return (mdcSector<0) ? -mdcSector-1:1;}
  Char_t        getNDigiMdcSectors(void) const  {return (mdcWSector<0) ? 1-mdcWSector-1:1;}
  Bool_t        isDirectionGood(void) const     {return directionFlag;}
  Bool_t        isGeantBug(void) const          {return geantBugFlag;}
  Int_t         getNSegsInMdc(Int_t m,Int_t sec=-1);
  Int_t         getNSegments(Int_t seg,Int_t sec=-1);
  Char_t        getSegDirection(Int_t ns);
  Char_t        getSector(Int_t ns);
  Char_t        getIOSeg(Int_t ns);
  Char_t        getFirstLayer12(Int_t ns);
  Char_t        getLastLayer12(Int_t ns);
  Char_t        getNGMdcHits(Int_t ns);

  Bool_t        getMdcHitPos(Int_t ns, Int_t m, HGeomVector& hit);
  Bool_t        getMdcHitPosSec(Int_t ns, Int_t m, HGeomVector& hit);
  Bool_t        getMdcHitPosLab(Int_t ns, Int_t m, HGeomVector& hit);
  Bool_t        getLayerHitPos(Int_t ns, Int_t m, Int_t l, HGeomVector& hit,Bool_t extrFlag=kTRUE);
  Bool_t        getLayerHitPosSec(Int_t ns,Int_t m,Int_t l, HGeomVector& hit,Bool_t extrFlag=kTRUE);
  Bool_t        getLayerHitPosLab(Int_t ns,Int_t m,Int_t l, HGeomVector& hit,Bool_t extrFlag=kTRUE);
  HGeantKine*   getGeantKine(void)              {return pGeantKine;}
  Bool_t        isGntTrackOk(void)              {return testBit(255);}
  Bool_t        isGEANTBug(void)                {return !testBit(128);}
  Bool_t        isNoMeta(void)                  {return !testBit(64);}
  Bool_t        isTrackNotRec(void)             {return !testBit(32);}

  void          setUserFlag(Int_t fl)           {userFlag=fl;}
  Int_t         getUserFlag(void) const         {return userFlag;}
  void          print(void);

  static void   printDebug(HGeantMdc* pGeantMdc,Int_t i,const Char_t* st=0);

private:
  Bool_t        nSegOk(Short_t ns)              {return ns>=0 && ns<nSegments;}
  Bool_t        nSecOk(Int_t ns)                {return ns>=0 && ns<6;}
  Bool_t        nMdcOk(Int_t nm)                {return nm>=0 && nm<4;}
  void          testHitsInDetectors(void);
  Short_t       testMdcHits(void);
  Bool_t        addSegAtAndExpand(Short_t segNum);
  Bool_t        addSegment(void);
  Int_t         collectWires(Char_t sec, Char_t mod, Char_t lay, Float_t atof);
  Bool_t        isGeantBug1(Float_t momLay);
  Bool_t        isGeantBug2(Char_t sec,Char_t dMod,Char_t dLay,Char_t hitDir,Float_t dTof);
  Bool_t        isGeantBug3(void);
  Bool_t        mayBeGeantBug(Float_t dMom);
  void          printDebug(Float_t dMom,const Char_t dMod);
  Char_t        dirDLayer(Char_t dir)           {return dir == 0 ? 0 : (dir<0) ? -1:+1;}
  void          calcNSectors(void);
  void          setStatusFlags(void);
  static Int_t  calcLay14(Int_t m, Int_t l);
  Bool_t        testBit(UChar_t bit) const      {return (trackStatus&bit)==bit;}

  ClassDef(HMdcGeantTrack,0) // Mdc GEANT segments collection for one track
};

class HMdcTrackInfSim : public TObject {
protected:
  UChar_t               sector;              //
  UChar_t               segment;             //
  Int_t                 modInd;              // 0,1 or <0(for two mdc segment)
  Int_t                 nWiresTot;           // number of wires in current cluster or segment

  Bool_t                isWrCollected;       //
  Int_t                 numTracks;           // for tracks list calculation
  enum {aSize=20};                           //
  Short_t               numWires[aSize+1];   // -/-
  HMdcList12GroupCells  wiresList[aSize+1];  // -/-
  HMdcGeantTrack*       gntTrackList[aSize]; // -/-
  HMdcGeantSeg*         gntSegList[aSize];   // -/-
  Int_t                 segIngGTrack[aSize]; // index of segment in HMdcGeantTrack
  Int_t                 tracksNum[aSize];    // -/-
  Int_t                 sortedInd[aSize];    // -/-
  Int_t                 embedInd;
  Int_t                 noiseInd;
  HMdcList12GroupCells  listTmp;
  HMdcWiresArr         *pWiresArr;           //

  Bool_t                isGntBugEvent;       //
  Int_t                 isGntBugSeg;         //
public:
  HMdcTrackInfSim(void)  {}
  ~HMdcTrackInfSim(void) {}

  void    fillClusTrackInf(HMdcClusSim* pClusSim);
  void    addClusModTrackInf(HMdcClusSim* pClusSim);
  void    fillClusModTrackInf(HMdcClusSim* pClusSim,HMdcList12GroupCells* wrLst,Int_t modi);
  void    fillSegTrackInf(HMdcSegSim* pSegSim,HMdcList24GroupCells* wrLst,HMdcSegSim* pSegPar);
  void    fillHitTrackInf(HMdcHitSim* pHitSim,HMdcList24GroupCells* wrLst);
  Bool_t  collectTracksInf(UChar_t sec,UChar_t seg,HMdcList12GroupCells* wrLst);
  void    testIOMatching(Int_t ind,HMdcClusSim* pClusSim,Int_t bin,HMdcClusSim* pClusSimPar);
  void    testIOMatching(Int_t ind,HMdcSegSim* pSegSim,Int_t bin,HMdcSegSim* pSegPar);


  Int_t   getNumTracks(void) const       {return numTracks;}
  Int_t   getTrack(Int_t trInd) const;
  Short_t getNumWires(Int_t trInd) const;
  Bool_t  isGeantBugEvent(void)          {return isGntBugEvent;}
  Bool_t  isGeantBugSeg(void)            {return isGntBugSeg;}
  void    setWiresArr(HMdcWiresArr* wl)  {pWiresArr = wl;}

  UChar_t getNumLayers(Int_t trInd, Int_t modi=-2) const;
  HMdcList12GroupCells* getListCells(Int_t trInd);
  HMdcGeantSeg*         getMdcGeantSeg(Int_t trInd);
  HMdcGeantTrack*       getMdcGeantTrack(Int_t trInd);

private:
  Bool_t isTrIndOk(Int_t ind) {return ind>=0 && ind<numTracks;}
  Bool_t  collectTracksInf(void);

  ClassDef(HMdcTrackInfSim,0)  // Mdc GEANT segments collection for one event
};

class HMdcGeantEvent : public TObjArray {
protected:
  static HMdcGeantEvent* pGlobalGEvent;  // pointer to the global object
  Int_t                  size;           // size of TObjArray
  Float_t                xVertex;        // x of event vertex  (in mm)
  Float_t                yVertex;        // y (from first primary track)
  Float_t                zVertex;        // z
  Int_t                  targNum;        // target numer (0,1,...) of vertex
  HCategory*             pGeantKineCat;  // category GeantKine
  HCategory*             pGeantMdcCat;   // category GeantMdc
  Bool_t                 geantBugFlag;   // =kTRUE if geant bug was found
  Bool_t                 debugPrintFlag;
  TObjArray              geantSegments;  // array of HMdcGeantSeg objects
  Int_t                  nGSegments;     // num.of HMdcGeantSeg objects in event
  Int_t                  sizeGSegArr;    // size of geantSegments array

  Int_t                  nTracks;        // number of tracks in array
  HMdcEvntListCells*     pMdcListCells;  // object keep list of fired wires
  Bool_t                 isMdcLCellsOwn; // =kTRUE if MdcListCells is own

  HMdcTrackInfSim        mdcTrackInfSim;
public:
  HMdcGeantEvent(void);
  ~HMdcGeantEvent(void);
  static HMdcGeantEvent*  getExObject(void)  {return pGlobalGEvent;}
  static HMdcGeantEvent*  getObject(Bool_t& isCreated);
  static void             deleteCont(void);
  static HMdcTrackInfSim* getMdcTrackInfSim(void) {return pGlobalGEvent ?
                                      &(pGlobalGEvent->mdcTrackInfSim) : 0;}

  Bool_t  setGeantKineCat(void);
  void    unsetMdc(Int_t s, Int_t m)          {HMdcGeantTrack::unsetMdc(s,m);}
  Bool_t  isMdcActive(Int_t s, Int_t m) const {return HMdcGeantTrack::isMdcActive(s,m);}
  void    setDebugPrintFlag(Bool_t fg=kTRUE)  {debugPrintFlag=fg;}

  Bool_t  collectTracks(void);
  Int_t   getNTracks(void) const              {return nTracks;}
  HMdcGeantTrack* next(Int_t& i);
  HMdcGeantTrack* getGeantTrack(Int_t trackNum);

  Bool_t  isGeantBug(void) const              {return geantBugFlag;}
  void    print(void);
  void    clearOSegClus(void);
  void    clearOClus(void);
  void    clearOSeg(void);
  void    getEventVertex(Float_t &ax, Float_t &ay, Float_t &az) const;
  Float_t getXvertex(void) const              {return xVertex;}
  Float_t getYvertex(void) const              {return yVertex;}
  Float_t getZvertex(void) const              {return zVertex;}
  Int_t   getTargetNum(void) const            {return targNum;}

private:
  ClassDef(HMdcGeantEvent,0)  // Mdc GEANT segments collection for one event
};

#endif /*!HMDCGEANTTRACK_H*/
