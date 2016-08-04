#ifndef HMDCSTOREEVENTS_H
#define HMDCSTOREEVENTS_H

#include "TObject.h"
#include "hgeomvector.h"
#include "hseqarr.h"

class HMdcEvntListCells;
class HMdcClus;

class HMdcStoreWires : public TObject {
  protected:
    HSeqArr      nDTPerEvent; // array of indexis of events
    HSeqArrIter* iterEvents;  // pointer to iterator of events
    HSeqArr      mdcWireAddr; // array of addresses of fired wires
    HSeqArrIter* iterWires;   // pointer to iterator of wires in event
    
    UInt_t       maxMemSize;  // upper limit for memory
    
    UInt_t       nEvents;     // number of events in array (for filling only)  
    UShort_t     nDrTimes;    // number of wires in current event (for filling)
    
    UShort_t     nDrTmPerEv;  // number of wires in current event (for reading)
    UInt_t       nRdEvents;   // number of read events  (for reading)
    UShort_t     nRdDrTimes;  // number of read wires  (for reading)

  public:
    HMdcStoreWires(void);
    ~HMdcStoreWires(void);
    void setMaxMemSize(UInt_t mSize); // mSize - in Mb!
    virtual UInt_t getMemSize(void) const;
    Bool_t testMemSize(void) const;
    
    // Storing functions:
    void addWireAddr(Int_t s, Int_t m, Int_t l, Int_t c);
    void setEndEvent(void);
    
    // Getting functions:
    virtual void resetIter(void);
    Bool_t getNextEvent(void);
    Bool_t getNextWireAddr(Int_t& s, Int_t& m, Int_t& l, Int_t& c);
    inline static UShort_t packAddress(Int_t s, Int_t m, Int_t l, Int_t c);
    inline static void unpackAddress(UShort_t& addr, Int_t& s, Int_t& m, 
        Int_t& l, Int_t& c);
    
    // Storing and getting events:
    void addEvent(const HMdcEvntListCells& event);
    Bool_t getNextEvent(HMdcEvntListCells& event);
  protected:
    
  ClassDef(HMdcStoreWires,0)
};

inline UShort_t HMdcStoreWires::packAddress(Int_t s,Int_t m,Int_t l,Int_t c) {
  return UShort_t((( ((s&7)<<5) + ((m&3)<<3) + (l&7) ) <<8) + (c&255));
}

inline void HMdcStoreWires::unpackAddress(UShort_t& addr, Int_t& s, 
    Int_t& m, Int_t& l, Int_t& c) {
  s=(Int_t)((addr>>13)&7);  // 13=5+8
  m=(Int_t)((addr>>11)&3);  // 11=3+8
  l=(Int_t)((addr>>8)&7);
  c=(Int_t)(addr&255);
}

//----------------------------------------------

class HMdcStoreEvents : public TObject {
  protected:
    HSeqArr      nDTPerEvent; // array of indexis of events
    HSeqArrIter* iterEvents;  // pointer to iterator of events
    HSeqArr      mdcCells;    // array of addresses & dr.times of fired wires
    HSeqArrIter* iterWires;   // pointer to iterator of wires in event
    
    UInt_t       maxMemSize;  // upper limit for memory
    
    UInt_t       nEvents;     // number of events in array (for filling only)  
    UShort_t     nDrTimes;    // number of wires in current event (for filling)
    
    UShort_t     nDrTmPerEv;  // number of wires in current event (for reading)
    UInt_t       nRdEvents;   // number of read events  (for reading)
    UShort_t     nRdDrTimes;  // number of read wires  (for reading)

  public:
    HMdcStoreEvents(void);
    ~HMdcStoreEvents(void);
    void setMaxMemSize(UInt_t mSize); // mSize - in Mb!
    virtual UInt_t getMemSize(void) const;
    Bool_t testMemSize(void) const;
    
    // Storing functions:
    void addWire(Int_t s,Int_t m,Int_t l,Int_t c,Float_t tm, Bool_t flag=kTRUE);
    void setEndEvent(void);
    
    // Getting functions:
    virtual void resetIter(void);
    Bool_t getNextEvent(void);
    Bool_t getNextCell(Int_t& s, Int_t& m, Int_t& l, Int_t& c, Float_t& tm);
    Bool_t getNextCell(Int_t& s, Int_t& m, Int_t& l, Int_t& c, Float_t& tm,
        Bool_t& flag);
    Bool_t getNextCell(Int_t& s, Int_t& m, Int_t& l, Int_t& c, Float_t& tm,
        Bool_t& flag,UInt_t** addr);
    inline static UInt_t packCell(Int_t s, Int_t m, Int_t l, Int_t c, 
        Float_t tm, Bool_t flag=kTRUE);
    inline static Bool_t unpackCell(UInt_t& addr, Int_t& s, Int_t& m, 
        Int_t& l, Int_t& c, Float_t& tm);
    static void unsetFlag(UInt_t& addr) {addr &= 0xFFFEFFFF;}
    static void setFlag(UInt_t& addr)   {addr |=    0x10000;} // 0x10000=1<<16;
    
    // Storing and getting events:
    void   addEvent(const HMdcEvntListCells& event);
    Bool_t getNextEvent(HMdcEvntListCells& event,Bool_t checkFlag=kFALSE);
  protected:
    
  ClassDef(HMdcStoreEvents,0)
};

inline UInt_t HMdcStoreEvents::packCell(Int_t s,Int_t m,Int_t l,Int_t c,
    Float_t tm,Bool_t flag) {
  UInt_t sign = tm>0 ? 0 : 0x4000;       // 0x4000 = 1<<14
  if(sign) tm = -tm;
  UInt_t dt   = UInt_t(tm*10. + 0.5);
  if(dt > 0x3FFF) dt=0x3FFF;
  dt = (dt|sign)<<17;
  if(flag) setFlag(dt);
  return UInt_t((( ((s&7)<<5) + ((m&3)<<3) + (l&7) ) <<8) + (c&0xFF)) +dt;
}

inline Bool_t HMdcStoreEvents::unpackCell(UInt_t& addr, Int_t& s, 
    Int_t& m, Int_t& l, Int_t& c, Float_t& tm) {
  // return user flag
  s  = (Int_t)((addr>>13)&7);               // 13=5+8
  m  = (Int_t)((addr>>11)&3);               // 11=3+8
  l  = (Int_t)((addr>>8)&7);
  c  = (Int_t)(addr&0xFF);
  tm = ((addr>>17)&0x3FFF)*0.1;
  if(addr&0x80000000) tm = -tm;             // negative dr.time.
  return (addr&0x10000) > 0;                // 0x10000=1<<16
}

//----------------------------------------------

class HMdcStoreTracks : public TObject {
  protected:
    HSeqArr      address;      // keep sector and mod.for x1,y1 (mod1)
                               // and mod.for x2,y2 (mod2) and flag
                               // if mod1=mod2 - x1,y1 - layer 1, x2,y2 - layer 6
    HSeqArr      trackPos;     // array of cluster positions
    HSeqArr      nTracksEvent; // array of numbers of tracks per event
    HSeqArrIter* iterAddress;  // pointer to iterator of track address
    HSeqArrIter* iterTrackPos; // pointer to iterator of clusters positions
    HSeqArrIter* iterNTrckEv;  // pointer to iterator of num.of clusters/event
    UChar_t      flagAddress;  // trackFlag and address for current track
    Float_t      x1,y1;        // current cluster parameters:
    Float_t      x2,y2;        // two points in coor.sys. mdc's
    UShort_t     nEventTracks; // num. tracks in one event (<=32767)
    UShort_t     nReadTracks;  // counter of the read tracks in this event
    Bool_t       eventFlag;    // event flag
    Bool_t       arrayEnd;     // end of array flag
  public:
    HMdcStoreTracks(void);
    ~HMdcStoreTracks(void);
    virtual UInt_t getMemSize(void) const;
    
    // Storing functions:
    Bool_t setEndTrack(void);
    void setEndEvent(Bool_t flag=kTRUE);
    void setTrackPar(UChar_t s, UChar_t m1, Float_t xf, Float_t yf,
        UChar_t m2, Float_t xl, Float_t yl, UChar_t flag=1);
    
    // Getting functions:
    virtual void resetIter(void);  // must be called ones per clusters loop
    Bool_t nextEvent(void);
    Bool_t nextTrack(void);
    
    Bool_t  getEventFlag(void)            {return eventFlag;}
    Bool_t  getTrackFlag(void)            {return (flagAddress&128) > 0;}
    UChar_t getSector(void)               {return flagAddress&7;}
    UChar_t getMod1(void)                 {return (flagAddress>>3)&3;}
    UChar_t getMod2(void)                 {return (flagAddress>>5)&3;}
    Int_t   getNumEventTracks(void) const {return nEventTracks;}
    Float_t getX1(void) const             {return x1;}
    Float_t getY1(void) const             {return y1;}
    Float_t getZ1(void) const             {return 0.;}
    Float_t getX2(void) const             {return x2;}
    Float_t getY2(void) const             {return y2;}
    Float_t getZ2(void) const             {return 0.;}
    void getTrackPar(Float_t& xf,Float_t& yf, Float_t& xl,Float_t& yl) const;
    void getPoint1(HGeomVector& p1) const {p1.setXYZ(x1,y1,0.);}
    void getPoint2(HGeomVector& p2) const {p2.setXYZ(x2,y2,0.);}
    void getPoints(HGeomVector& p1,HGeomVector& p2) const {
        getPoint1(p1); getPoint2(p2);}

    // Use this function at the reading data only!!!    
    void resetEventIter(void);   // for reading the same event again
    Bool_t resetTrackPar(Float_t xt,Float_t yt,Float_t xp,Float_t yp);
    Bool_t resetTrackFlag(Bool_t flag);
    Bool_t resetEventFlag(Bool_t flag);
    Bool_t isEnd(void) {return arrayEnd;}
    
  protected:
    void clear();
    Bool_t getTrackPar(void);
      
  ClassDef(HMdcStoreTracks,0)  // storage of track parameters
};

//----------------------------------------------

class HMdcStoreClusters : public HMdcStoreTracks {
  protected:
    HMdcStoreEvents wires;        // storage of cluster(track) wires
  
    UInt_t* wiresListPack[1200];  // list of packed wires for current cluster
    Int_t   numWires;
  public:
    HMdcStoreClusters(void) {}
    ~HMdcStoreClusters(void) {}
    virtual UInt_t getMemSize(void) const;
    
    // Storing functions:
    void setEndCluster(void);
    
    // Getting functions:
    virtual void resetIter(void);  // must be called ones per clusters loop
    Bool_t getNextCluster(void);
    Bool_t getNextCell(Int_t& s, Int_t& m, Int_t& l, Int_t& c, Float_t& tm) {
        return wires.getNextCell(s,m,l,c,tm);}
    Bool_t getNextCell(Int_t& s, Int_t& m, Int_t& l, Int_t& c, Float_t& tm,
        Bool_t& flag) {
        return wires.getNextCell(s,m,l,c,tm,flag);}
    
    // Storing and getting clusters:
    void addClustWires(const HMdcEvntListCells& event, 
        const HMdcClus* cl);                        // store wires list only
    void addClustWires(const HMdcEvntListCells& event,
        const HMdcClus* cl1, const HMdcClus* cl2);  // store wires list only
    Bool_t getNextCluster(HMdcEvntListCells& event,Bool_t checkFlag=kFALSE);
    Bool_t unsetFlag(Int_t si,Int_t mi,Int_t li,Int_t ci);
    Bool_t setFlag(Int_t si,Int_t mi,Int_t li,Int_t ci);
    
  protected:
      
  ClassDef(HMdcStoreClusters,0)  // storage of track param. and wires
};

#endif  /*!HMDCSTOREEVENTS_H*/
