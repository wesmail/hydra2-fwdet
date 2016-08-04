//*************************************************************************
//
//  HGeantKine
//
//  GEANT KINE event data (GEANT data on primary and secondary tracks)
//
//  last modified on 23/03/2005 by R.Holzmann  (GSI)
//*************************************************************************
#ifndef HGEANTKINE_H
#define HGEANTKINE_H

#include "TMath.h"
#include "TObject.h"
#include "hades.h"
#include "hrecevent.h"
#include "hgeomvector.h"
#include "hphysicsconstants.h"
#include "hgeantrich.h"
#include "hgeantmdc.h"
#include "hgeanttof.h"
#include "hgeantrpc.h"
#include "hgeantemc.h"
#include "hgeantwall.h"
#include "hgeantstart.h"
#include "hgeantfwdet.h"

#include <vector>

using namespace std;

class HLinkedDataObject;
class HCategory;
class HLinearCategory;

class HGeantKine : public TObject
{
private:
  Int_t richIndex;          //! internal index variables
  Int_t mdcIndex;           //! used to set up linked chains
  Int_t tofIndex;           //! of hit objects
  Int_t rpcIndex;           //!
  Int_t showIndex;          //! 
  Int_t wallIndex;          //! 
  Int_t emcIndex;           //!
  Int_t startIndex;         //!
  Int_t fwDetIndex;         //!
  HCategory* pRich;         //! internal pointers to category
  HCategory* pMdc;          //! used to set up linked chains
  HCategory* pTof;          //! of hit objects
  HCategory* pRpc;          //!
  HCategory* pShow;         //!
  HCategory* pWall;         //!
  HCategory* pEmc;          //!
  HCategory* pStart;        //!
  HCategory* pFwDet;        //!

protected:
  Int_t trackNumber;        // GEANT track number
  Int_t parentTrack;        // GEANT track number of parent particle
  Int_t particleID;         // GEANT particle ID number
  Int_t mediumNumber;       // GEANT medium of creation number
  Int_t creationMechanism;  // GEANT creation mechanism number
  Float_t xVertex;          // x of track vertex  (in mm)
  Float_t yVertex;          // y
  Float_t zVertex;          // z
  Float_t xMom;             // x component of particle momentum (in MeV/c)
  Float_t yMom;             // y
  Float_t zMom;             // z
  Float_t generatorInfo;    // event generator info            PLUTO: SourceID : thermal (parentid + 500) or  decay code (example : pp->pppi0 ==> 141407)
  Float_t generatorInfo1;   // additional event generator info PLUTO: parentID
  Float_t generatorInfo2;   // additional event generator info PLUTO: parentIndex
  Float_t generatorWeight;  // associated weight
  Short_t firstRichHit;     // index of first hit in RICH category
  Short_t firstMdcHit;      // index of first hit in MDC category
  Short_t firstTofHit;      // index of first hit in TOF category
  Short_t firstRpcHit;      // index of first hit in RPC category
  Short_t firstShowerHit;   // index of first hit in SHOWER category
  Short_t firstWallHit;     // index of first hit in WALL category
  Short_t firstEmcHit;      // index of first hit in EMC category
  Short_t firstStartHit;    // index of first hit in START category
  Short_t firstFwDetHit;    // index of first hit in Forward Detector category
  Bool_t active;            // active flag (set if track participates in a hit)
  Bool_t suppressed;        // flag used to filter out particle hits before digitization 
  Float_t userVal;          // stores user specific values (for expample evtSeqnumber of PLUTO particles for embedding)
  UInt_t  acceptance ;      // bit wise : 1-25 MDCLAYER 26-27 sys0/sys1 28-31 layer crop  highest bit : filled/not filled
public:
  HGeantKine(void);
  HGeantKine(HGeantKine &aKine);
  ~HGeantKine(void);
  void setTrack(Int_t aTrack) { trackNumber = aTrack;}
  void setParticle(Int_t aTrack, Int_t aID);
  void setCreator(Int_t aPar, Int_t aMed, Int_t aMech);
  void setVertex(Float_t ax, Float_t ay, Float_t az);
  void setMomentum(Float_t apx, Float_t apy, Float_t apz);
  void setGenerator(Float_t aInfo, Float_t aWeight);
  void setGenerator(Float_t aInfo, Float_t aInfo1, Float_t aInfo2);
  void setWeight(Float_t aWeight) {generatorWeight = aWeight;}
  void getParticle(Int_t &aTrack, Int_t &aID);
  void getCreator(Int_t &aPar, Int_t &aMed, Int_t &aMech);
  void setID(Int_t aID) { particleID = aID; }
  void setMedium(Int_t aMed) { mediumNumber = aMed; }
  void setMechanism(Int_t aMech) { creationMechanism = aMech; }

  void getVertex(Float_t &ax, Float_t &ay, Float_t &az);
  void getVertex(HGeomVector& ver);
  Float_t  getDistFromVertex();
  void getMomentum(Float_t &apx, Float_t &apy, Float_t &apz);
  inline Int_t getTrack(void) const { return trackNumber; }
  inline Int_t getMedium()    const { return mediumNumber;      }
  inline Int_t getMechanism() const { return creationMechanism; }
  inline Int_t getID(void) const { return particleID; }
  inline Int_t getParentTrack(void) const { return parentTrack; }
  inline void setParentTrack(Int_t track) { parentTrack = track; }
  void setNewTrackNumber(Int_t track);
  Float_t getTotalMomentum2(void) const { return xMom*xMom + yMom*yMom + zMom*zMom; }
  Float_t getTotalMomentum(void) const { return TMath::Sqrt(getTotalMomentum2()); }
  Float_t getTransverseMomentum(void) const { return TMath::Sqrt(xMom*xMom + yMom*yMom); }
  Int_t   getSector();
  Float_t getPhiDeg  (Bool_t labSys = kTRUE);
  Float_t getThetaDeg();
  Int_t   getPhiThetaDeg(Float_t& theta,Float_t& phi, Bool_t labSys = kTRUE);

  Float_t getM(void) const {return HPhysicsConstants::mass(particleID);}
  Float_t getE(void) const {return TMath::Sqrt(getM()*getM() + getTotalMomentum2());}
  Float_t getEkin(void) const {return getE()-getM();}
  Float_t getRapidity(void) const{return 0.5*TMath::Log((getE()+zMom)/(getE()-zMom));}

  void getMomentum(HGeomVector &v) { v.setXYZ(xMom, yMom, zMom); }
  void getGenerator(Float_t &aInfo, Float_t &aWeight);
  void getGenerator(Float_t &aInfo, Float_t &aInfo1, Float_t &aInfo2);
  Int_t   getGeneratorInfo()   const  {return generatorInfo;}
  Int_t   getGeneratorInfo1()  const  {return generatorInfo1;}
  Int_t   getGeneratorInfo2()  const  {return generatorInfo2;}
  Float_t getGeneratorWeight() const  {return generatorWeight;}
  static Int_t   getGeneration(HGeantKine*);
  static Int_t   getMothers(HGeantKine*, vector<HGeantKine*>& mothers);
  static Int_t   getAllDaughters(HGeantKine* mother,vector<HGeantKine*>& daughters);
  static Int_t   getDaughters(HGeantKine* mother,vector<HGeantKine*>& daughters);
  static HGeantKine*  getChargedDecayDaughter(HGeantKine* mother);

  inline void setActive(Bool_t flag=kTRUE) {active=flag;}
  inline void setSuppressed(Bool_t flag=kTRUE) {suppressed=flag;}
  inline void setUserVal(Float_t val) {userVal=val;}
  inline Bool_t  isActive(void) const {return active;}
  inline Bool_t  isPrimary(void) const {return (parentTrack==0);}
  inline Bool_t  isSuppressed(void) const {return suppressed;}
  inline Float_t getUserVal(void)   const {return userVal;}
  Bool_t         hasAncestor(Int_t track, HLinearCategory* cat=NULL);
  void           printHistory();
  void           print();


  //--------------------------------------------------------
  // PLUTO helper functions
  Bool_t        isBugInfo();
  Bool_t        isExternalSource() ;
  Bool_t        isThermalSource()  ;
  static Bool_t isSameExternalSource(HGeantKine* kine1,HGeantKine* kine2);
  Bool_t        isSameExternalSource(HGeantKine* kine1);



  static Bool_t setChainActive(Int_t track, Bool_t flag=kTRUE, HLinearCategory* cat=NULL);
  static Bool_t setAllDescendentsActive(Int_t track, Bool_t flag=kTRUE, HLinearCategory* cat=NULL);
  static Bool_t setAllDescendentsSuppressed(Int_t track, Bool_t flag=kTRUE, HLinearCategory* cat=NULL);
  static HGeantKine* getParent(Int_t track, HLinearCategory* cat=NULL);
  static HGeantKine* getGrandParent(Int_t track, HLinearCategory* cat=NULL);
  static HGeantKine* getPrimary(Int_t track, HLinearCategory* cat=NULL);
  static HGeantKine* getCommonAncestor(Int_t track1, Int_t track2, HLinearCategory* cat=NULL);
  static Bool_t suppressTracks(Int_t id, Float_t acceptedFraction, HLinearCategory* cat=NULL);

  inline Int_t setRichHitIndex(Int_t index);
  inline Int_t setMdcHitIndex(Int_t index);
  inline Int_t setTofHitIndex(Int_t index);
  inline Int_t setRpcHitIndex(Int_t index);
  inline Int_t setShowerHitIndex(Int_t index);
  inline Int_t setWallHitIndex(Int_t index);
  inline Int_t setEmcHitIndex(Int_t index);
  inline Int_t setStartHitIndex(Int_t index);
  inline Int_t setFwDetHitIndex(Int_t index);

  Int_t getNRichHits(void);  // return number of hits in list
  Int_t getFirstRichHit() {return firstRichHit;}
  Int_t getNMdcHits(void);
  Int_t getFirstMdcHit() {return firstMdcHit;}
  Int_t getNMdcHits(Int_t module);
  Int_t getNTofHits(void);
  Int_t getFirstTofHit() {return firstTofHit;}
  Int_t getNRpcHits(void);
  Int_t getFirstRpcHit() {return firstRpcHit;}
  Int_t getNShowerHits(void);
  Int_t getFirstShowerHit() {return firstShowerHit;}
  Int_t getNWallHits(void);
  Int_t getFirstWallHit() {return firstWallHit;}
  Int_t getNEmcHits(void);
  Int_t getFirstEmcHit() {return firstEmcHit;}
  Int_t getNStartHits(void);
  Int_t getFirstStartHit() {return firstStartHit;}
  Int_t getNFwDetHits(void);
  Int_t getFirstFwDetHit() {return firstFwDetHit;}

  Int_t getRichHits (vector<HGeantRichPhoton*>& v);
  Int_t getMdcHits  (vector<HGeantMdc*>& v);
  Int_t getTofHits  (vector<HGeantTof*>& v);
  Int_t getRpcHits  (vector<HGeantRpc*>& v);
  Int_t getWallHits (vector<HGeantWall*>& v);
  Int_t getEmcHits  (vector<HGeantEmc*>& v);
  Int_t getStartHits(vector<HGeantStart*>& v);
  Int_t getFwDetHits(vector<HGeantFwDet*>& v);

  void setFirstRichHit  (Int_t index) { firstRichHit  = index;}
  void setFirstMdcHit   (Int_t index) { firstMdcHit   = index;}
  void setFirstTofHit   (Int_t index) { firstTofHit   = index;}
  void setFirstRpcHit   (Int_t index) { firstRpcHit   = index;}
  void setFirstShowerHit(Int_t index) { firstShowerHit= index;}
  void setFirstWallHit  (Int_t index) { firstWallHit  = index;}
  void setFirstEmcHit   (Int_t index) { firstEmcHit   = index;}
  void setFirstStartHit (Int_t index) { firstStartHit = index;}


  void sortRichHits(void);  // sort hits in list
  void sortMdcHits(void);
  void sortTofHits(void);
  void sortRpcHits(void);
  void sortShowerHits(void);
  void sortWallHits(void);

  Bool_t isInAcceptance     (Int_t m0=4,Int_t m1=4,Int_t m2=4,Int_t m3=4,Int_t sys0=1,Int_t sys1=1);
  Bool_t isInAcceptanceDecay(Int_t m0,Int_t m1,Int_t m2,Int_t m3,Int_t sys0,Int_t sys1);
  Bool_t isInAcceptanceBit     (Int_t m0=4,Int_t m1=4,Int_t m2=4,Int_t m3=4,Int_t sys0=1,Int_t sys1=1);
  Bool_t isInAcceptanceDecayBit(Int_t m0,Int_t m1,Int_t m2,Int_t m3,Int_t sys0,Int_t sys1);
  void   getNHits        (Int_t& m0,Int_t& m1,Int_t& m2,Int_t& m3,Int_t& sys0,Int_t& sys1);
  void   getNHitsDecay   (Int_t& m0,Int_t& m1,Int_t& m2,Int_t& m3,Int_t& sys0,Int_t& sys1);
  void   getNHitsBit     (Int_t& m0,Int_t& m1,Int_t& m2,Int_t& m3,Int_t& sys0,Int_t& sys1);
  void   getNHitsDecayBit(Int_t& m0,Int_t& m1,Int_t& m2,Int_t& m3,Int_t& sys0,Int_t& sys1);
  Int_t  getSystem(void);
  Int_t  getSectorFromBitArray(void);
  UInt_t getMdcSectorBitArray(void);
  UInt_t getMdcSectorDecimalArray(void); 
  UInt_t getShowerSectorBitArray(void); 
  UInt_t getShowerSectorDecimalArray(void); 
  UInt_t getTofSectorBitArray(void); 
  UInt_t getTofSectorDecimalArray(void); 
  UInt_t getRpcSectorBitArray(void); 
  UInt_t getRpcSectorDecimalArray(void); 
  UInt_t getRichSectorBitArray(void); 
  UInt_t getRichSectorDecimalArray(void);
  UInt_t getSectorBitArray(void);
  UInt_t getSectorDecimalArray(void);

  inline HLinkedDataObject* nextRichHit();
  inline HLinkedDataObject* nextMdcHit();
  inline HLinkedDataObject* nextTofHit();
  inline HLinkedDataObject* nextRpcHit();
  inline HLinkedDataObject* nextShowerHit();
  inline HLinkedDataObject* nextWallHit();
  inline HLinkedDataObject* nextEmcHit();
  inline HLinkedDataObject* nextStartHit();
  inline HLinkedDataObject* nextFwDetHit();

  // The following 4 are needed if the categories are created outside the HYDRA event
  // and are hence not accessible via HRecEvent::getCategory(cat), e.g. in a macro.
  inline void setRichCategory(HCategory* p) {pRich = p;} 
  inline void setMdcCategory(HCategory* p) {pMdc = p;}   
  inline void setTofCategory(HCategory* p) {pTof = p;}
  inline void setRpcCategory(HCategory* p) {pRpc = p;}
  inline void setShowerCategory(HCategory* p) {pShow = p;}
  inline void setWallCategory(HCategory* p) {pWall = p;}
  inline void setEmcCategory(HCategory* p) {pEmc = p;}
  inline void setStartCategory(HCategory* p) {pStart = p;}
  inline void setFwDetCategory(HCategory* p) {pFwDet = p;}

  inline void resetRichIter(void);
  inline void resetMdcIter(void);
  inline void resetTofIter(void);
  inline void resetRpcIter(void);
  inline void resetShowerIter(void);
  inline void resetWallIter(void);
  inline void resetEmcIter(void);
  inline void resetStartIter(void);
  inline void resetFwDetIter(void);


  //-----------------------------------------------------------------------
  // acceptance bits
  void    setLayers(UInt_t io,UInt_t layers) { acceptance|=(layers&(0xFFF<<(io*12)));}
  void    setLayer (UInt_t io,UInt_t lay)    { acceptance |=  ( 0x01 << (io*12+lay) );          }
  void    unsetAllLayers()                   { acceptance &= ~0xFFFFFF; }
  static void setLayer(UInt_t io,UInt_t lay,UInt_t& layers) { layers |=  ( 0x01 << (io*12+lay) ); }
  Bool_t  getLayer    (UInt_t io,UInt_t lay)                { return ( acceptance &  ( 0x01 << (io*12+lay) )); }
  Bool_t  hasLayers   (UInt_t io,UInt_t layerstest)         { return (((acceptance>>(io*12))&0xFFF)==((layerstest>>(io*12))&0xFFF));}
  Int_t   getNLayer   (UInt_t io);
  Int_t   getNLayerMod(UInt_t mod);
  void    printLayers();
  void    setSys  (UInt_t sys)   { acceptance |=  ( 0x01 << (24+sys) ); }
  void    unsetSys(UInt_t sys)   { acceptance &= ~( 0x01 << (24+sys) ); }
  Bool_t  getSys  (UInt_t sys)   { return ( acceptance &  ( 0x01 << (24+sys) )); }

  void   setAtMdcEdge  (UInt_t i)   {  if(i<4) { acceptance|=((0x1)<<(i+26));  }  }
  void   unsetAtMdcEdge(UInt_t i)   {  if(i<4) { acceptance&=~((0x1)<<(i+26)); }  }
  Bool_t isAtMdcEdge   (UInt_t i)   {  if(i<4) { return ( acceptance &  ( 0x01 << (26+i) ));} else return kFALSE; }
  Bool_t isAtAnyMdcEdge(UInt_t io=2){
	    if(io<3){
		if(io==0||io==2){
		    if( acceptance &  ( 0x01 << (26+0) )) return kTRUE;
		    if( acceptance &  ( 0x01 << (26+1) )) return kTRUE;
		}
		if(io>=1){
		    if( acceptance &  ( 0x01 << (26+2) )) return kTRUE;
		    if( acceptance &  ( 0x01 << (26+3) )) return kTRUE;
		}
                return kFALSE;
	    } else return kFALSE;
  }

  void   setCropedFilled()       { acceptance |=((0x1)<<(30));  }
  void   unsetCropedFilled()     { acceptance &=~((0x1)<<(30)); }
  Bool_t isCropedFilled()        { return ( acceptance &  ( 0x01 << (30) )); }
  void   setAcceptanceFilled()   { acceptance |=((0x1)<<(31));  }
  void   unsetAcceptanceFilled() { acceptance &=~((0x1)<<(31)); }
  Bool_t isAcceptanceFilled()    { return ( acceptance &  ( 0x01 << (31) )); }
  void   fillAcceptanceBit();
//-----------------------------------------------------------------------





private:
  Int_t setHitIndex(HCategory* p, Short_t& first, Int_t index);
  HLinkedDataObject* nextHit(HCategory* p, Int_t& next);
   
  ClassDef(HGeantKine,12) // GEANT KINE data class
};

//------------------inlines----------------------------------
//----------------------hit index----------------------------
  inline Int_t HGeantKine::setRichHitIndex(Int_t index) {
  // set next RICH photon hit index in linked list
    resetRichIter();
    return setHitIndex(pRich,firstRichHit,index);
  }

  inline Int_t HGeantKine::setMdcHitIndex(Int_t index) {
  // set next MDC hit index in linked list
    resetMdcIter();
    return setHitIndex(pMdc,firstMdcHit,index);
  }

  inline Int_t HGeantKine::setTofHitIndex(Int_t index) {
  // set next TOF hit index in linked list
    resetTofIter();
    return setHitIndex(pTof,firstTofHit,index);
  }

  inline Int_t HGeantKine::setRpcHitIndex(Int_t index) {
  // set next RPC hit index in linked list
    resetRpcIter();
    return setHitIndex(pRpc,firstRpcHit,index);
  }

  inline Int_t HGeantKine::setShowerHitIndex(Int_t index) {
  // set next SHOWER hit index in linked list
    resetShowerIter();
    return setHitIndex(pShow, firstShowerHit,index);
  }

  inline Int_t HGeantKine::setWallHitIndex(Int_t index) {
  // set next WALL hit index in linked list
    resetWallIter();
    return setHitIndex(pWall, firstWallHit,index);
  }

  inline Int_t HGeantKine::setEmcHitIndex(Int_t index) {
  // set next EMC hit index in linked list
    resetEmcIter();
    return setHitIndex(pEmc,firstEmcHit,index);
  }

  inline Int_t HGeantKine::setStartHitIndex(Int_t index) {
  // set next START hit index in linked list
    resetStartIter();
    return setHitIndex(pStart,firstStartHit,index);
  }

  inline Int_t HGeantKine::setFwDetHitIndex(Int_t index) {
  // set next Forward detector hit index in linked list
    resetFwDetIter();
    return setHitIndex(pFwDet,firstFwDetHit,index);
  }

//------------------------linked data objects--------------------
  inline HLinkedDataObject* HGeantKine::nextRichHit() {
  // return next RICH photon hit made by present track
     return nextHit(pRich,richIndex);
  }

  inline HLinkedDataObject* HGeantKine::nextMdcHit() {
  // return next MDC hit made by present track
     return nextHit(pMdc,mdcIndex);
  }

  inline HLinkedDataObject* HGeantKine::nextTofHit() {
  // return next TOF hit made by present track
     return nextHit(pTof,tofIndex);
  }

  inline HLinkedDataObject* HGeantKine::nextRpcHit() {
  // return next RPC hit made by present track
     return nextHit(pRpc,rpcIndex);
  }

  inline HLinkedDataObject* HGeantKine::nextShowerHit() {
  // return next SHOWER hit made by present track
     return nextHit(pShow,showIndex);
  }

  inline HLinkedDataObject* HGeantKine::nextWallHit() {
  // return next WALL hit made by present track
     return nextHit(pWall,wallIndex);
  }

  inline HLinkedDataObject* HGeantKine::nextEmcHit() {
  // return next EMC hit made by present track
     return nextHit(pEmc,emcIndex);
  }

  inline HLinkedDataObject* HGeantKine::nextStartHit() {
  // return next START hit made by present track
     return nextHit(pStart,startIndex);
  }

  inline HLinkedDataObject* HGeantKine::nextFwDetHit() {
  // return next Forward detector hit made by present track
     return nextHit(pFwDet,fwDetIndex);
  }

//----------------------iterators--------------------------------
  inline void HGeantKine::resetRichIter(void) {  // reset list iterator
     richIndex = (Int_t)firstRichHit;
     if(!pRich) pRich = ((HRecEvent*)gHades->getCurrentEvent())
                      ->getCategory(catRichGeantRaw);
  }
  inline void HGeantKine::resetMdcIter(void) {
     mdcIndex = (Int_t)firstMdcHit;
     if(!pMdc) pMdc = ((HRecEvent*)gHades->getCurrentEvent())
                    ->getCategory(catMdcGeantRaw);
  }
  inline void HGeantKine::resetTofIter(void) {
     tofIndex = (Int_t)firstTofHit;
     if(!pTof) pTof = ((HRecEvent*)gHades->getCurrentEvent())
                    ->getCategory(catTofGeantRaw);
  }
 
  inline void HGeantKine::resetRpcIter(void) {
     rpcIndex = (Int_t)firstRpcHit;
     if(!pRpc) pRpc = ((HRecEvent*)gHades->getCurrentEvent())
                    ->getCategory(catRpcGeantRaw);
  }

  inline void HGeantKine::resetShowerIter(void) {
     showIndex = (Int_t)firstShowerHit;
     if(!pShow) pShow = ((HRecEvent*)gHades->getCurrentEvent())
                      ->getCategory(catShowerGeantRaw);
  }

  inline void HGeantKine::resetWallIter(void) {
     wallIndex = (Int_t)firstWallHit;
     if(!pWall) pWall = ((HRecEvent*)gHades->getCurrentEvent())
                      ->getCategory(catWallGeantRaw);
  }

  inline void HGeantKine::resetEmcIter(void) {
     emcIndex = (Int_t)firstEmcHit;
     if(!pEmc) pEmc = ((HRecEvent*)gHades->getCurrentEvent())
                    ->getCategory(catEmcGeantRaw);
  }

  inline void HGeantKine::resetStartIter(void) {
     startIndex = (Int_t)firstStartHit;
     if(!pStart) pStart = ((HRecEvent*)gHades->getCurrentEvent())
                        ->getCategory(catStartGeantRaw);
  }

  inline void HGeantKine::resetFwDetIter(void) {
     fwDetIndex = (Int_t)firstFwDetHit;
     if(!pFwDet) pFwDet = ((HRecEvent*)gHades->getCurrentEvent())
                        ->getCategory(catFwDetGeantRaw);
  }

#endif  /*! HGEANTKINE_H */










