#include "hmdcstoreevents.h"
#include "hmdclistcells.h"
#include "hmdcclus.h"

//*-- AUTHOR : Pechenov Vladimir
//*-- Modified : 14/05/2004 by V.Pechenov

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
// HMdcStoreWires
//
// HMdcStoreEvents
//
// HMdcStoreClusters
//
////////////////////////////////////////////////////////////////

ClassImp(HMdcStoreWires)
ClassImp(HMdcStoreEvents)
ClassImp(HMdcStoreTracks)
ClassImp(HMdcStoreClusters)

HMdcStoreWires::HMdcStoreWires(void) {
  nDTPerEvent.setArrType(kShort);
  iterEvents=nDTPerEvent.makeIterator();
  mdcWireAddr.setArrType(kUShort);
  iterWires=mdcWireAddr.makeIterator();
  nEvents=0;
  nDrTimes=0;
  nRdEvents=0;
  nRdDrTimes=0;
  maxMemSize=200*1024*1024;
}

HMdcStoreWires::~HMdcStoreWires(void) {
  if(iterEvents) delete iterEvents;
  if(iterWires) delete iterWires;
  iterEvents=0;
  iterWires=0;
}

void HMdcStoreWires::setMaxMemSize(UInt_t mSize) {
  maxMemSize=mSize*1024*1024;
}

UInt_t HMdcStoreWires::getMemSize(void) const {
  return mdcWireAddr.getMemSize()+nDTPerEvent.getMemSize();
}

Bool_t HMdcStoreWires::testMemSize(void) const {
  return getMemSize() < maxMemSize;
}

void HMdcStoreWires::setEndEvent(void) {
  if(nDrTimes) {
    nDTPerEvent.addNext(nDrTimes);
    nDrTimes=0;
    nEvents++;
  }
}

void HMdcStoreWires::addWireAddr(Int_t s,Int_t m,Int_t l,Int_t c) {
  mdcWireAddr.addNext(packAddress(s,m,l,c));
  nDrTimes++;
}

void HMdcStoreWires::resetIter(void) {
  iterEvents->reset();
  iterWires->reset();
  nRdEvents=0;
  nRdDrTimes=0;
  nDrTmPerEv=0;
}

Bool_t HMdcStoreWires::getNextEvent(void) {
  if(nRdEvents==nEvents) return kFALSE;
  if(!iterEvents->getNext(nDrTmPerEv)) return kFALSE;
  nRdEvents++;
  nRdDrTimes=0;
  return kTRUE;
}

Bool_t HMdcStoreWires::getNextWireAddr(Int_t& s, Int_t& m, 
    Int_t& l, Int_t& c) {
  UShort_t addr; 
  if(nRdDrTimes==nDrTmPerEv || !iterWires->getNext(addr)) return kFALSE;
  unpackAddress(addr,s,m,l,c);
  nRdDrTimes++;
  return kTRUE;
}

void HMdcStoreWires::addEvent(const HMdcEvntListCells& event) {
  Int_t s,m,l,c;
  s=m=l=c=-1;
  while(event.nextCell(s,m,l,c)) addWireAddr(s,m,l,c);
  setEndEvent();
}

Bool_t HMdcStoreWires::getNextEvent(HMdcEvntListCells& event) {
  event.clear();
  if(!getNextEvent()) return kFALSE;
  Int_t s,m,l,c;
  s=m=l=c=-1;
  while(getNextWireAddr(s,m,l,c)) event.setTime(s,m,l,c,1);  // tim1 only !!!
  return kTRUE;
}

//====================================================

HMdcStoreEvents::HMdcStoreEvents(void) {
  nDTPerEvent.setArrType(kUShort);
  iterEvents=nDTPerEvent.makeIterator();
  mdcCells.setArrType(kUInt);
  iterWires=mdcCells.makeIterator();
  nEvents=0;
  nDrTimes=0;
  nRdEvents=0;
  nRdDrTimes=0;
  maxMemSize=200*1024*1024;
}

HMdcStoreEvents::~HMdcStoreEvents(void) {
  if(iterEvents) delete iterEvents;
  if(iterWires) delete iterWires;
  iterEvents=0;
  iterWires=0;
}

void HMdcStoreEvents::setMaxMemSize(UInt_t mSize) {
  maxMemSize=mSize*1024*1024;
}

UInt_t HMdcStoreEvents::getMemSize(void) const {
  return mdcCells.getMemSize()+nDTPerEvent.getMemSize();
}

Bool_t HMdcStoreEvents::testMemSize(void) const {
  return getMemSize() < maxMemSize;
}

void HMdcStoreEvents::setEndEvent(void) {
  if(nDrTimes) {
    nDTPerEvent.addNext(nDrTimes);
    nDrTimes=0;
    nEvents++;
  }
}

void HMdcStoreEvents::addWire(Int_t s,Int_t m,Int_t l,Int_t c,
    Float_t tm, Bool_t flag) {
  mdcCells.addNext(packCell(s,m,l,c,tm,flag));
  nDrTimes++;
}

void HMdcStoreEvents::resetIter(void) {
  iterEvents->reset();
  iterWires->reset();
  nRdEvents=0;
  nRdDrTimes=0;
  nDrTmPerEv=0;
}

Bool_t HMdcStoreEvents::getNextEvent(void) {
  if(nRdEvents==nEvents) return kFALSE;
  if(!iterEvents->getNext(nDrTmPerEv)) return kFALSE;
  nRdEvents++;
  nRdDrTimes=0;
  return kTRUE;
}

Bool_t HMdcStoreEvents::getNextCell(Int_t& s, Int_t& m, 
    Int_t& l, Int_t& c, Float_t& tm) {
  UInt_t addr;
  if(nRdDrTimes==nDrTmPerEv || !iterWires->getNext(addr)) return kFALSE;
  unpackCell(addr,s,m,l,c,tm);
  nRdDrTimes++;
  return kTRUE;
}

Bool_t HMdcStoreEvents::getNextCell(Int_t& s, Int_t& m, 
    Int_t& l, Int_t& c, Float_t& tm, Bool_t& flag, UInt_t** addr) {
  if(nRdDrTimes==nDrTmPerEv || !iterWires->getNext(addr)) return kFALSE;
  flag = unpackCell(**addr,s,m,l,c,tm);
  nRdDrTimes++;
  return kTRUE;
}

Bool_t HMdcStoreEvents::getNextCell(Int_t& s, Int_t& m, 
    Int_t& l, Int_t& c, Float_t& tm, Bool_t& flag) {
  UInt_t addr; 
  if(nRdDrTimes==nDrTmPerEv || !iterWires->getNext(addr)) return kFALSE;
  flag = unpackCell(addr,s,m,l,c,tm);
  nRdDrTimes++;
  return kTRUE;
}


void HMdcStoreEvents::addEvent(const HMdcEvntListCells& event) {
  Int_t s,m,l,c;
  Float_t tm;
  s=m=l=c=-1;
  while(event.nextTime(s,m,l,c,tm)) addWire(s,m,l,c,tm);
  setEndEvent();
}

Bool_t HMdcStoreEvents::getNextEvent(HMdcEvntListCells& event,
    Bool_t checkFlag) {
  event.clear();
  if(!getNextEvent()) return kFALSE;
  Int_t s,m,l,c;
  Float_t tm;
  Bool_t flag;
  s=m=l=c=-1;
  while(getNextCell(s,m,l,c,tm,flag)) if(!checkFlag || flag)
    event.addTime1(s,m,l,c,tm);  // time1 only !!!
  return kTRUE;
}

//----------------------------------------------------------

HMdcStoreTracks::HMdcStoreTracks(void) {
  address.setArrType(kUChar);
  trackPos.setArrType(kFloat);
  nTracksEvent.setArrType(kUShort);
  iterAddress  = address.makeIterator();
  iterTrackPos = trackPos.makeIterator();
  iterNTrckEv  = nTracksEvent.makeIterator();
  clear();
}

void HMdcStoreTracks::clear(void) {
  nEventTracks=0;
  nReadTracks=0;
  arrayEnd=kFALSE;
  eventFlag=kFALSE;
  x1=y1=x2=y2=0.;
  flagAddress=0;
}

HMdcStoreTracks::~HMdcStoreTracks(void) {
  if(iterAddress) delete iterAddress;
  iterAddress=0;
  if(iterTrackPos) delete iterTrackPos;
  iterTrackPos=0;
  if(iterNTrckEv) delete iterNTrckEv;
  iterNTrckEv=0;
}
    
UInt_t HMdcStoreTracks::getMemSize(void) const {
  return trackPos.getMemSize()+nTracksEvent.getMemSize()+address.getMemSize();
}

void HMdcStoreTracks::resetIter(void) {
  iterAddress->reset();
  iterTrackPos->reset();
  iterNTrckEv->reset();
  clear();
}

void HMdcStoreTracks::setTrackPar(UChar_t s, UChar_t m1,Float_t xf,Float_t yf,
    UChar_t m2,Float_t xl,Float_t yl, UChar_t flag) {
  x1=xf;
  y1=yf;
  x2=xl;
  y2=yl;
  flagAddress=(s&7) + ((m1&3)<<3) + ((m2&3)<<5) + ((flag&1)<<7);
}

Bool_t HMdcStoreTracks::setEndTrack(void) {
  iterAddress ->setNext(flagAddress);
  iterTrackPos->setNext(x1);
  iterTrackPos->setNext(y1);
  iterTrackPos->setNext(x2);
  iterTrackPos->setNext(y2);
  if(nEventTracks>=32767) {
    Error("setEndTrack","Number of tracks in event >=32767 !");
    exit(1);
  }
  nEventTracks++;
  return kTRUE;
}

void HMdcStoreTracks::setEndEvent(Bool_t flag) {
  if(flag) nEventTracks+=32768;
  iterNTrckEv->setNext(nEventTracks);
  nEventTracks=0;
}

Bool_t HMdcStoreTracks::getTrackPar(void) {
  if(!iterAddress->getNext(flagAddress)) return kFALSE;
  if(!iterTrackPos->getNext(x1)) return kFALSE;
  if(!iterTrackPos->getNext(y1)) return kFALSE;
  if(!iterTrackPos->getNext(x2)) return kFALSE;
  if(!iterTrackPos->getNext(y2)) return kFALSE;
  return kTRUE;
}

Bool_t HMdcStoreTracks::nextEvent(void) {
  if(arrayEnd) return kFALSE;
  while(nReadTracks<nEventTracks) if(!nextTrack()) return kFALSE;
  if(!iterNTrckEv->getNext(nEventTracks)) {
    arrayEnd=kTRUE;
    return kFALSE;
  }
  eventFlag = (nEventTracks&32768) != 0;
  nEventTracks &= 32767;
  nReadTracks=0;
  return kTRUE;
}

Bool_t HMdcStoreTracks::nextTrack(void) {
  // reading cluster without reading wires!!!
  if(arrayEnd) return kFALSE;
  if(nReadTracks==nEventTracks) return kFALSE;
  nReadTracks++;
  return getTrackPar();
}

void HMdcStoreTracks::getTrackPar(Float_t& xf,Float_t& yf,
                                      Float_t& xl,Float_t& yl) const {
  xf=x1;
  yf=y1;
  xl=x2;
  yl=y2;
}

Bool_t HMdcStoreTracks::resetTrackPar(Float_t xf,Float_t yf,
                                      Float_t xl,Float_t yl) {
  x1=xf;
  y1=yf;
  x2=xl;
  y2=yl;
  Int_t cursor = (Int_t)iterTrackPos->getCursor();
  iterTrackPos->setCursor(cursor-4);
  iterTrackPos->setNext(x1);
  iterTrackPos->setNext(y1);
  iterTrackPos->setNext(x2);
  iterTrackPos->setNext(y2);
  return kTRUE;
}

void HMdcStoreTracks::resetEventIter(void) {
  // for reading the same event again
  if(nReadTracks<=0) return;
  Int_t cursor = (Int_t)iterTrackPos->getCursor();
  iterTrackPos->setCursor(cursor-4*nReadTracks);
  cursor = iterAddress->getCursor();
  iterAddress->setCursor(cursor-nReadTracks);
  nReadTracks=0;
}

Bool_t HMdcStoreTracks::resetTrackFlag(Bool_t flag) {
  if(getTrackFlag()) {if( flag) return kTRUE;}
  else               {if(!flag) return kTRUE;}
  flagAddress &= 127;
  if(flag) flagAddress += 128;
  iterAddress->set(flagAddress);
  return kTRUE;
}

Bool_t HMdcStoreTracks::resetEventFlag(Bool_t flag) {
  eventFlag=flag;
  if(eventFlag) iterNTrckEv->set(nEventTracks+32768);
  else          iterNTrckEv->set(nEventTracks);
  return kTRUE;
}

//----------------------------------------------
    
UInt_t HMdcStoreClusters::getMemSize(void) const {
  return HMdcStoreTracks::getMemSize()+wires.getMemSize();
}

void HMdcStoreClusters::resetIter(void) {
  HMdcStoreTracks::resetIter();
  wires.resetIter();
}

void HMdcStoreClusters::setEndCluster(void) {
  HMdcStoreTracks::setEndTrack();
  wires.setEndEvent();
}

Bool_t HMdcStoreClusters::getNextCluster(void) {
  if(!nextTrack()) return kFALSE;
  return wires.getNextEvent();
}

void HMdcStoreClusters::addClustWires(const HMdcEvntListCells& event, 
        const HMdcClus* cl1, const HMdcClus* cl2) {
  addClustWires(event,cl1);
  addClustWires(event,cl2);
}

void HMdcStoreClusters::addClustWires(const HMdcEvntListCells& event, 
        const HMdcClus* cl) {
  // store wires list only
  if(cl==0) return;
  Int_t sec=cl->getSec();
  Int_t seg=cl->getIOSeg();
  Int_t lay=-1;
  Int_t cell=-1;
  while(cl->getNextCell(lay,cell)) {
    Int_t l=lay%6;
    Int_t mod=(lay/6)+seg*2;
    wires.addWire(sec,mod,l,cell,event.getTimeValue(sec,mod,l,cell));
  }   
}

// Bool_t HMdcStoreClusters::getNextCluster(HMdcEvntListCells& event) {
//   if(!nextTrack()) return kFALSE;
//   return wires.getNextEvent(event,wiresListPack,numWires);
// }

Bool_t HMdcStoreClusters::getNextCluster(HMdcEvntListCells& event,
    Bool_t checkFlag) {
  if(!nextTrack()) return kFALSE;
  event.clear();
  if(!wires.getNextEvent()) return kFALSE;
  Int_t s,m,l,c;
  Float_t tm;
  Bool_t flag;
  s=m=l=c=-1;
  numWires=0;
  while(wires.getNextCell(s,m,l,c,tm,flag,wiresListPack+numWires)) {
    if(!checkFlag || flag) event.addTime1(s,m,l,c,tm);  // time1 only !!!
    if(numWires<1200) numWires++;
  }
  return kTRUE;
}

Bool_t HMdcStoreClusters::unsetFlag(Int_t si,Int_t mi,Int_t li,Int_t ci) {
  UInt_t addr = HMdcStoreEvents::packCell(si,mi,li,ci,0.)&0xFFFF;
  for(Int_t ind=0;ind<numWires;ind++) {
    UInt_t& wire = *(wiresListPack[ind]);
    if((wire&0xFFFF) != addr) continue;
    HMdcStoreEvents::unsetFlag(wire);
    return kTRUE;
  }
  return kFALSE;
}

Bool_t HMdcStoreClusters::setFlag(Int_t si,Int_t mi,Int_t li,Int_t ci) {
  UInt_t addr = HMdcStoreEvents::packCell(si,mi,li,ci,0.)&0xFFFF;
  for(Int_t ind=0;ind<numWires;ind++) {
    UInt_t& wire = *(wiresListPack[ind]);
    if((wire&0xFFFF) != addr) continue;
    HMdcStoreEvents::setFlag(wire);
    return kTRUE;
  }
  return kFALSE;
}
