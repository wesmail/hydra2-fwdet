#ifndef HMDCSIZESCELLS_H
#define HMDCSIZESCELLS_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "hmdcgeomobj.h"
#include "hgeomtransform.h"

class HMdcGetContainers;
class HMdcLayerGeomPar;
class HMdcGeomPar;
class HMdcTrap;
class HMdcGeomStruct;
class HMdcRawStruct;
class HMdcLookupRaw;
class HSpecGeomPar;
class HMdcLayerGeomParLay;
class HMdcLayerCorrPar;
class HGeomVolume;

class HMdcSizesCellsCell : public TObject {
  protected:
    HGeomVector wirePnt1;    // wire geometry, 2-poins in sector coor.sys.
    HGeomVector wirePnt2;    // first p. connected to readout
    Bool_t      status;      // =kTRUE if wire conected to readout
    Char_t      readOutSide; // Wire readout side:
                             // ='L' for left, 
                             // ='R' for right,
                             // ='0' for not connected wires
    Float_t     xReadout;    // position of the wire point connected to readout
                             // in coor.system of wire (y=0,z=0)
    Float_t     length;      // wire length
    Double_t    yWirePos;    // y[mm] wire position in rotated layer coor.sys.
  public:
    HMdcSizesCellsCell(void) {clear();}
    ~HMdcSizesCellsCell(void)   {}
    const HGeomVector* getWirePoint(Int_t n) const {
        if(n == 0) return &wirePnt1;
        if(n == 1) return &wirePnt2;
        return 0; }
        
    void     setStatus(Bool_t stat)        {status=stat;}
    Bool_t   getStatus(void) const         {return status;}
    Float_t  getWireLength(void) const     {return length;}
    Char_t   getReadoutSide(void) const    {return readOutSide;}
    Float_t  getXReadout(void) const       {return xReadout;}
    Double_t getWirePos(void) const        {return yWirePos;}
    void     clear(void);
    
    //Next functions NOT for user:
    Float_t getSignPath(Float_t x) const   {return readOutSide=='L' ? xReadout-x : x-xReadout;}
    HGeomVector& getWirePnt1(void)         {return wirePnt1;}
    HGeomVector& getWirePnt2(void)         {return wirePnt2;}
    void         setReadoutSide(Char_t rs) {readOutSide = rs;}
    void         setWireLength(Float_t wl) {length      = wl;}
    void         setXReadout(Float_t xr)   {xReadout    = xr;}
    void         setWirePos(Double_t y)    {yWirePos    = y;}
        
  ClassDef(HMdcSizesCellsCell,0)
};


class HMdcSizesCellsMod;
class HMdcSizesCells;

class HMdcSizesCellsLayer : public HMdcPlane {
  protected:
    Short_t              sector;                 // Address:           
    Short_t              module;
    Short_t              layer;                
    HMdcLayerGeomParLay* pLayerGeomParLay;      // Layer parameters
    HGeomVolume*         pGeomVol;              // Layer geometry
    Short_t              nCells;                // Number of wires in layer
    Double_t             halfCatDist;           // Half of cathodes distance
    Double_t             pitch;                 // Distance between wires
    Double_t             invPitch;              // = 1./pitch
    Double_t             halfPitch;             // Half of "pitch"
    Double_t             halfCDsDivPitch;       // halfCatDist/pitch
    Double_t             maxDriftDist;          // = sqrt(halfPitch^2+halfCatDist^2)
    HGeomTransform       sysRSec;               // 
    Double_t             tSysRSec[12];          // Transformation sector <-> layer
    HGeomTransform       sysRMod;               // Transformation module<->layer
    HMdcSizesCellsCell*  cellsArray;            // 
    HMdcSizesCellsMod*   pToMod;                // Pointer to corresponding module
                                               
    Double_t             wireOrient;            // Wire orientation in deg.
    Double_t             wireOffset;            // = (CentralWireNr()-1.)*pitch
    Double_t             cellOffset;            // = CentralWireNr()-0.5
    Double_t             cosWireOr;             // Cosine of wire orientation
    Double_t             sinWireOr;             // Sine of wire orientation angle
    Double_t             zModule;               // z of module plane in coor.sys. layer
    HGeomTransform       rotLaySysRMod;         // Transformation module <->
                                                // layer sys. rotated along WireOr.
    HGeomTransform       rotLaySysRSec;         // Transformation sector <->
    Double_t             tRLaySysRSec[12];      //  layer sys. rotated along WireOr.
    Double_t             pntToCell[4];          // Calc. cell# by point in sector coor.system 
      
    // Parameters of layer second part shift:
    Int_t                firstCellPart2;        // First cell of the layer second part
    Double_t             wireOrientPart2;       // Wire orientation in deg.
    Double_t             wireOffsetPart2;       // = wireOffset - shift
    Double_t             cellOffsetPart2;       // = cellOffset - shift/pitch
    Double_t             cosWireOrPart2;        // Cosine of wire orientation
    Double_t             sinWireOrPart2;        // Sine of wire orientation angle
    HGeomTransform       rotLaySysRModPart2;    // Transformation module <->
                                                // rotated on WireOr deg. layer
    HGeomTransform       rotLaySysRSecPart2;    // Transformation sector <->
    Double_t             tRLaySysRSecPart2[12]; //   rotated on WireOr deg. layer
    
    // Next data are intrested for geant data only, because plane of
    // HMdcGeant hits is entrance plane of sensitive wires volume!
    Double_t             sensWiresThick;        // sensitive wires thickness
    Double_t             zGeantHit;             // z position of geant hits
    HGeomTransform       geantSysRMod;          // Transf. mdc <-> geantHitsPlane
    HGeomTransform       geantSysRSec;          // Transf. sector <-> geantHitsPlane
    HMdcPlane            geantPlane;            // geant hits plane in sec.sys.
  public:
    HMdcSizesCellsLayer(void);
    ~HMdcSizesCellsLayer(void);
    HMdcSizesCellsCell& operator[](Int_t i) const        {return cellsArray[i];}

    Bool_t setSecTrans(Double_t corZ=0.);
    Bool_t fillLayerCont(const HMdcLayerCorrPar* fLayCorrPar,const Double_t* corr=0);
    HMdcLayerGeomParLay* getLayerGeomParLay(void)        {return pLayerGeomParLay;}
    HGeomVolume*         getGeomVolume(void)             {return pGeomVol;}

    HMdcSizesCells* getSizesCells(void);
    Short_t  getSec(void) const                          {return sector;} 
    Short_t  getMod(void) const                          {return module;} 
    Short_t  getLayer(void) const                        {return layer;}  
    Double_t getCatDist(void) const                      {return 2.*halfCatDist;}
    Double_t getHalfCatDist(void) const                  {return halfCatDist;}
    Double_t getPitch(void) const                        {return pitch;}
    Double_t getInvPitch(void) const                     {return invPitch;}
    Double_t getHalfPitch(void) const                    {return halfPitch;}
    Double_t getMaxDriftDist(void) const                 {return maxDriftDist;}
    Double_t getCosWireOr(Int_t c) const;
    Double_t getSinWireOr(Int_t c) const;
    Double_t getTanWireOr(Int_t c) const;
    Double_t getWireOffset(Int_t c) const;
    Double_t getCellOffset(void) const                   {return cellOffset;}
    Double_t getCellOffsetPart2(void) const              {return cellOffsetPart2;}
    Short_t  getNCells(void) const                       {return nCells;}
    const HGeomTransform* getSecTrans(void) const        {return &sysRSec;}
    const HGeomTransform* getModTrans(void) const        {return &sysRMod;}
    Int_t    getFirstCellPart2(void) const               {return firstCellPart2;}
    Int_t    getLayerNParts(void) const                  {return nCells<firstCellPart2 ? 1:2;}
    Int_t    getLayerPart(Int_t c) const                 {return c<firstCellPart2 ? 0:1;}
    const HGeomTransform* getRotLayP1SysRSec(void) const {return &rotLaySysRSec;}
    const HGeomTransform* getRotLayP2SysRSec(void) const {return &rotLaySysRSecPart2;}
    const HGeomTransform* getRotLayP1SysRMod(void) const {return &rotLaySysRMod;}
    const HGeomTransform* getRotLayP2SysRMod(void) const {return &rotLaySysRModPart2;}
    inline const HGeomTransform& getRotLaySysRSec(Int_t c) const;
    inline const Double_t* getRLSysRSec(Int_t c) const;
    const Double_t* getRLP1SysRSec(void) const           {return tRLaySysRSec;}
    const Double_t* getRLP2SysRSec(void) const           {return tRLaySysRSecPart2;}
    void            transSecToRotLayer(Int_t c,HGeomVector& point) const;
    Int_t           calcCellNum(Double_t x, Double_t y) const;
    Int_t           calcCell(Double_t x, Double_t y) const {return calcWire(x,y) + 0.5;}
    Bool_t          calcCrCellsGeantMdc(Float_t &x,Float_t &y,Float_t theta,Float_t phi,
                                             Int_t& c1, Int_t& c2) const;
    Bool_t          calcCrossedCells(Double_t x1,Double_t y1,Double_t z1,
                                     Double_t x2,Double_t y2,Double_t z2,
                                     Float_t& cell1,Float_t& cell2) const;
    Bool_t          calcCrossedCells(Double_t x1,Double_t y1,Double_t z1, Double_t x2,Double_t y2,Double_t z2,
                                     Int_t& firstCell, Int_t& lastCell,
                                     Float_t &firstCellPath,Float_t &midCellPath,Float_t &lastCellPath) const;
    Bool_t          calcSegCrossCells(Double_t z,Double_t r,Double_t theta,Double_t phi,
                                      Int_t& firstCell, Int_t& lastCell, 
                                      Float_t &firstCellPath,Float_t &midCellPath,Float_t &lastCellPath) const;
        
    Bool_t          calcCrossedCells(const HMdcLineParam& ln,Float_t& cell1, Float_t& cell2) const;
    Double_t        getAlpha(Int_t c,const HGeomVector& p1,const HGeomVector& p2) const;
    Double_t        getDist(const HGeomVector& p1,const HGeomVector& p2,Int_t cell);
    inline Double_t getDist(Double_t x1,Double_t y1,Double_t z1,
                            Double_t x2,Double_t y2,Double_t z2,Int_t cell) const;
    inline void     getYZinWireSys(Double_t x,Double_t y,Double_t z,Int_t cell,
                                   Double_t& yw, Double_t& zw) const;
    inline Double_t getDistToZero(Double_t y1,Double_t z1,
                                  Double_t y2, Double_t z2) const;
    inline Double_t getImpactToZero(Double_t y1,Double_t z1,
                              Double_t y2,Double_t z2,Double_t &alpha) const;
    Double_t        getImpact(Double_t x1,Double_t y1,Double_t z1,
                              Double_t x2,Double_t y2,Double_t z2,
                              Int_t cell,Double_t &alphaPerp) const;
    inline Double_t getImpact(const HMdcLineParam& ln, Int_t cell, Double_t &alpha) const;
    inline Double_t getDist(const HMdcLineParam& ln, Int_t cell) const;
    inline Double_t getAlpha(const HMdcLineParam& ln,Int_t cell) const;
    inline Double_t getImpact(const HMdcLineParam& ln, Int_t cell,
                              Double_t &alpha, Double_t &y,Double_t &z,
                              Double_t &dy,Double_t &dz) const;
    void            getImpact(const HGeomVector& p1, const HGeomVector& p2,
                              Int_t cell,Double_t &alpha, Double_t &per) const;
    Double_t        getImpactLSys(const HGeomVector& p1l,const HGeomVector& p2l,
                                  Int_t cell, Double_t &alpha,Int_t& distSign) const;
    Bool_t          getImpact(Double_t x1,Double_t y1,Double_t z1,
                              Double_t x2,Double_t y2,Double_t z2,Int_t cell,
                              Double_t &alphaDrDist, Double_t &driftDist) const;
    inline Bool_t   getImpact(const HMdcLineParam& ln, Int_t cell,
                              Double_t &alpha, Double_t &minDist) const;
    inline Bool_t   getImpact(const HMdcLineParam& ln, Int_t cell,
                              Double_t &alpha,Double_t &minDist,Double_t &y,
                              Double_t &z,Double_t &dy,Double_t &dz) const;
    Int_t           distanceSign(const HMdcLineParam& ln, Int_t cell) const;
    Bool_t          getDriftDist(Double_t xP1,Double_t yP1,Double_t zP1,
                                 Double_t xP2,Double_t yP2,Double_t zP2,
                                 Int_t cell, Double_t &alphaDrDist,
                                 Double_t &driftDist) const;
    Double_t        getXSign(Double_t x1, Double_t y1, Double_t z1, 
                             Double_t x2, Double_t y2, Double_t z2,
                             Int_t cell) const;
    Double_t        getXSign(const HMdcLineParam& ln, Int_t cell) const;
    Float_t         getSignPath(Double_t x1,Double_t y1,Double_t z1,
                                Double_t x2,Double_t y2,Double_t z2, 
                                Int_t cell) const;
    Double_t        getSignPath(const HMdcLineParam& ln, Int_t cell) const;
    Float_t         getSignPath(Double_t x1,Double_t y1,Double_t z1,
                                Double_t x2,Double_t y2,Double_t z2,
                                Int_t cell, Float_t& outside) const;
    inline void     transTo(Double_t& x, Double_t& y, Double_t& z) const;
    inline void     transFrom(Double_t& x, Double_t& y, Double_t& z) const;
    
    inline void     rotVectToLay(Double_t xi,Double_t yi,Double_t zi,
                                 Double_t& xo,Double_t& yo,Double_t& zo) const;
    Double_t        getXinRotLay(Int_t c, Double_t xi, Double_t yi) const;
    Double_t        getYinRotLay(Int_t c, Double_t xi, Double_t yi) const;
    inline Int_t    calcInnerSegCell(Double_t y) const;
    inline Int_t    calcInnerSegCell(HGeomVector &p) const;
    inline void     getPntToCell(Double_t *arr) const;
    void            print(void) const;
    
    // Next method are intrested for geant data only, because plane of
    // HMdcGeant hits is entrance plane of sensitive wires volume!
    Double_t        getSensWiresThickness(void) const   {return sensWiresThick;}
    Double_t        getZGeantHits(void) const           {return zGeantHit;}
    const HGeomTransform& getSecTransGeant(void) const  {return geantSysRSec;}
    const HGeomTransform& getModTransGeant(void) const  {return geantSysRMod;}
    const HMdcPlane&      getGeantHitsPlane(void) const {return geantPlane;}
    Bool_t calcCrossedCellsPar(const Double_t *v,Double_t extendCells,Int_t &c1,Int_t &c2,Int_t &c3,
                               Double_t *al,Double_t *md,Double_t *st) const;
    void            getRotLSysForOtherSecSys(Int_t othSec,Int_t cell,HGeomTransform& trans) const;
    Bool_t          calcCrossedCellsPar(const HMdcLineParam& ln,Double_t extendCells,Int_t &c1,Int_t &c2,Int_t &c3,
                                        Double_t *al,Double_t *md,Double_t *st) const;
    inline static Float_t calcAlphaImpact(Double_t dydz);
    inline Double_t getWireOrient(Int_t c) const  {return c < firstCellPart2 ? wireOrient:wireOrientPart2;};
    // Next function NOT for user:
    inline Double_t calcWireY(Int_t cell) const;
    void            initLayer(HMdcSizesCellsMod* pMod, Int_t lay);
  private:
    Bool_t          getParamCont(void);
    Double_t        calcWire(Double_t x, Double_t y) const;
    void            calcInOtherSecSys(const HMdcLineParam& ln,Int_t cell,
                       Double_t &y,Double_t &z,Double_t &dy,Double_t &dz) const;
    inline static void rotateTo(const Double_t* trans,Double_t xi,Double_t yi,Double_t zi,
                                Double_t& yo,Double_t& zo);
    inline static void rotateTo(const Double_t* trans,Double_t xi,Double_t yi,Double_t zi,
                                Double_t& xo,Double_t& yo,Double_t& zo);
    inline static void rotateTo(const Double_t* trans,const HGeomVector& d,
                                Double_t& yo, Double_t& zo);
    inline static void rotateTo(const Double_t* trans,const HGeomVector& d,
                                Double_t& xo, Double_t& yo, Double_t& zo);
    Double_t calcImpactParam(const HMdcLineParam& ln,Double_t &tanAlpha,Bool_t isLayerFirstPart=kTRUE) const;
    
  ClassDef(HMdcSizesCellsLayer,0)
};

class HMdcSizesCellsSec;
class HMdcSizesCells;

class HMdcSizesCellsMod : public HMdcPlane {
  protected:
    Short_t             sector;         // Address
    Short_t             module;         //
    HGeomTransform      sysRSec;        // Transformation sector<->module
    Double_t            tSysRSec[12];   // ---
    Double_t            tSysRSecOp[12]; // Optimized version of tSysRSec
    Double_t            ct[6];          // Lookup table for funct. calcMdcHit, ...
    HMdcSizesCellsLayer mdcSCLayers[6];
    HMdcSizesCellsSec*  pToSec;         // Pointer to corresponding sector
  public:
    HMdcSizesCellsMod(HMdcSizesCellsSec* pSec, Int_t mod);
    ~HMdcSizesCellsMod(void);
    HMdcSizesCellsLayer& operator[](Int_t l) {return mdcSCLayers[l];}
    Bool_t setSecTrans(const HGeomTransform *alignTrans=0,Int_t sysOfAlignTrans=0);
    
    Short_t getSec(void) const {return sector;}
    Short_t getMod(void) const {return module;}
    HMdcSizesCells* getSizesCells(void);
    const HGeomTransform* getSecTrans(void) const { return &sysRSec; }
    inline void transTo(const Double_t *v, Double_t *vo) const;
    inline void transTo(Double_t& x, Double_t& y, Double_t& z) const;
    inline void transFrom(Double_t& x, Double_t& y, Double_t& z) const;
    inline void transFromZ0(Double_t& x, Double_t& y, Double_t& z) const;
    inline void transFromZ0(Float_t& x, Float_t& y, Float_t& z) const;
    inline void rotVectTo(Double_t xi,Double_t yi,Double_t zi,
                          Double_t& xo, Double_t& yo, Double_t& zo) const;
    inline void calcInterTrMod(Double_t x1, Double_t y1, Double_t z1,
                               Double_t x2, Double_t y2, Double_t z2,
                               Double_t& x, Double_t& y) const;
    inline void calcMdcHit(Double_t x1, Double_t y1, Double_t z1,
                           Double_t x2, Double_t y2, Double_t z2,
                           Double_t& x, Double_t& y,
                           Double_t& xDir, Double_t& yDir) const;
    void calcMdcHit(Double_t x1, Double_t y1, Double_t z1,
                    Double_t x2, Double_t y2, Double_t z2,
                    Double_t eX1, Double_t eY1, Double_t eZ1,
                    Double_t eX2, Double_t eY2, Double_t dZ1dX1, Double_t dZ1dY1, 
                    Double_t dZ2dX2, Double_t dZ2dY2,
                    Double_t& x, Double_t& eX, Double_t& y, Double_t& eY,
                    Double_t& xDir, Double_t& eXDir,
                    Double_t& yDir, Double_t& eYDir) const;
    const Double_t* getTfSysRSec(void) const      {return tSysRSec;}
    const Double_t* getMdcHitLookupTb(void) const {return ct;}

  ClassDef(HMdcSizesCellsMod,0)
};


class HMdcSizesCellsSec : public TObject {
  protected:
    Short_t         sector;     // Address                                         
    TObjArray*      array;      // HMdcSizesCellsMod array                         
    HGeomTransform  sysRLab;    // Transformation sector<->lab.sys.     
    Bool_t*         mdcStatSec; // kTRUE - mdc exist 
    Int_t           numTargets; // number of targets
    HGeomVector*    targets;    // targets in coor.sys. of sector
    HGeomVector     targ3p[3];  // [0] First point of target in sector coor.sys.
                                // [1] Middle point of target in sec.coor.sys.
                                // [2] Last point of target in sec.coor.sys.
    HMdcSizesCells* pToSC;      // Pointer to HMdcSizesCells object
  public:
    HMdcSizesCellsSec(HMdcSizesCells* pSC, Int_t sec);
    ~HMdcSizesCellsSec(void);
    HMdcSizesCellsMod& operator[](Int_t i) const {return *static_cast<HMdcSizesCellsMod*>((*array)[i]);}
    Int_t getSize(void) const;
    const HGeomTransform* getLabTrans(void) const       {return &sysRLab;}
    Short_t            getSector(void)                  {return sector;}
    Bool_t             modStatus(Int_t m) const         {return m>=0 && m<4 ?
                                                          mdcStatSec[m]:kFALSE;}
    HMdcSizesCells*    getSizesCells(void);
    const HGeomVector& getTargetFirstPoint(void) const  {return targ3p[0];}
    const HGeomVector& getTargetMiddlePoint(void) const {return targ3p[1];}
    const HGeomVector& getTargetLastPoint(void) const   {return targ3p[2];}
    void               calcRZToTargLine(Double_t x1,Double_t y1,Double_t z1, 
                                        Double_t x2,Double_t y2,Double_t z2,
                                        Double_t &zm,Double_t &r0) const;
    Int_t              getNumOfTargets(void) const      {return numTargets;}
    HGeomVector*       getTargets(void)                 {return targets;}
    HGeomVector*       getTarget(Int_t i) {return i>=0 && i<numTargets ? &(targets[i]) : 0;}
    
    // Nex functions are NOT for user!!!
    HGeomTransform    &getLabTransform(void)            {return sysRLab;}
    void               setNumOfTargets(HGeomVector* targ3pLab,Int_t nt, HGeomVector* targetsLab);

  ClassDef(HMdcSizesCellsSec,0)
};

class HMdcSizesCells : public TObject {
  protected:
    static HMdcSizesCells* fMdcSizesCells;
    HMdcGetContainers*     fGetCont;
    HMdcLayerGeomPar*      fLayerGeomPar;
    Int_t             verLayerGeomPar[3];
    HMdcGeomPar*      fGeomPar;
    Int_t             verGeomPar[3];
    Bool_t            mdcStatus[6][4]; // kTRUE - mdc exist
    Int_t             nModsSeg[6][2];  // number of mdc per segment
    HMdcGeomStruct*   fMdcGeomStruct;
    HMdcLookupRaw*    fMdcLookupRaw;
    Int_t             verLookupRaw[3];
    HMdcRawStruct*    fMdcRawStruct;
    HSpecGeomPar*     fSpecGeomPar;
    HMdcLayerCorrPar* fLayerCorrPar;
    TObjArray*        array;           // array of pointers to HMdcSizesCellsSec
    Bool_t            changed;         // kTRUE if SizesCells was recalculated
    Bool_t            geomModified;    // kTRUE if mdc geom. was changed by user
    Int_t             numTargets;      // number of targets
    HGeomVector*      targets;         // targets
    Double_t*         tarHalfThick;    //
    HGeomVector       targ3p[3];       // [0] First point of target in lab.sys.
                                       // [1] Middle point of target in lab.sys.
                                       // [2] Last point of targ3p in lab.sys.
  public:
    static HMdcSizesCells* getObject(void);
    static HMdcSizesCells* getExObject(void) {return fMdcSizesCells;}
    static void            deleteCont(void);
    HMdcSizesCellsSec& operator[](Int_t i) const {
      return *static_cast<HMdcSizesCellsSec*>((*array)[i]);}
    Int_t  getSize(void) const;
    
    Bool_t initContainer(void);
    Bool_t hasChanged(void) const {return changed;}
    void   clear(void);
    Bool_t getCellVol(Int_t sec,Int_t mod,Int_t lay,Int_t cell,
                      HMdcTrap& volCell,Double_t sizeFactor=-1.) const;
    Char_t findReadOutSide(Int_t s,Int_t m,Int_t l,Int_t c) const;
    Bool_t modStatus(Int_t s, Int_t m) const   {return secOk(s) && modOk(m) ? 
                                                    mdcStatus[s][m] : kFALSE;}
    Int_t  nModInSeg(Int_t s, Int_t seg) const {return secOk(s) && segOk(seg) ?
                                                    nModsSeg[s][seg] : 0;}
    Bool_t fillModCont(Int_t sec, Int_t mod,
		       HGeomTransform * alignTrans=0, Int_t sysOfAlignTrans=0);
    Bool_t fillModCont(Int_t sec, Int_t mod, Double_t * corr);

    const HGeomVector& getTargetFirstPoint(void) const  {return targ3p[0];}
    const HGeomVector& getTargetMiddlePoint(void) const {return targ3p[1];}
    const HGeomVector& getTargetLastPoint(void) const   {return targ3p[2];}
    void         calcRZToTargLine(Double_t x1,Double_t y1,Double_t z1,
                                  Double_t x2,Double_t y2,Double_t z2,
                                  Double_t &zm,Double_t &r0) const;
    Int_t        getNumOfTargets(void) const            {return numTargets;}
    HGeomVector* getTargets(void)                       {return targets;}
    Double_t*    getTarHalfThick(void)                  {return tarHalfThick;}
    Double_t     getMaxTargHThick(void) const;
    void         setNotGeomModified(void)               {geomModified=kFALSE;}
    
    inline static void calcMdcSeg(Double_t x1,Double_t y1,Double_t z1, 
                                  Double_t x2, Double_t y2, Double_t z2,
                                  Double_t &zm, Double_t &r0, Double_t &theta,
                                  Double_t &phi);
    static void  setTransform(Double_t* par, HGeomTransform& trans);
    static void  copyTransfToArr(const HGeomTransform& trans, Double_t* arr);
    static void  calcRZToLineXY(Double_t x1, Double_t y1, Double_t z1, 
                                Double_t x2, Double_t y2, Double_t z2, 
                                Double_t xBeam, Double_t yBeam,
                                Double_t &zm,Double_t &r0);
    static void  calcMdcSeg(Double_t x1, Double_t y1, Double_t z1, 
                            Double_t x2, Double_t y2, Double_t z2,
                            Double_t eX1, Double_t eY1, Double_t eZ1,
                            Double_t eX2, Double_t eY2, Double_t dZ1dX1,
                            Double_t dZ1dY1, Double_t dZ2dX2, Double_t dZ2dY2,
                            Double_t& zm, Double_t& eZm, Double_t& r0,
                            Double_t& eR0, Double_t& theta, Double_t& eTheta,
                            Double_t& phi, Double_t& ePhi);
    static void  rotateYZ(const HGeomRotation& rot,
                          Double_t xi,Double_t yi,Double_t zi,
                          Double_t& yo, Double_t& zo);
    static void  rotateXYZ(const HGeomRotation& rot,
                          Double_t xi,Double_t yi,Double_t zi,
                          Double_t& xo,Double_t& yo,Double_t& zo);
    static void  rotateDir(const HGeomRotation& rot,const HGeomVector& d,
                           Double_t& dy, Double_t& dz);
    static void  rotateDir(const HGeomRotation& rot,const HGeomVector& d,
                           Double_t& dx,Double_t& dy, Double_t& dz);
    void         transLineToOtherSec(const HMdcLineParam& ln,Int_t sec,
                                     HGeomVector& p1,HGeomVector& p2);
    void         transLineToAnotherSec(HMdcLineParam& ln,Int_t anotherSec);
    Int_t        calcTargNum(Double_t z,Double_t* dz=0) const;
        
    // Next functions are NOT for user:
    void         setGeomModifiedFlag(void)       {geomModified = kTRUE;}
    Bool_t*      modStatusArr(Int_t s)           {return mdcStatus[s];}
    Int_t        nCells(Int_t s,Int_t m,Int_t l) const;
    HGeomVector& getTargetP(Int_t p)             {return targ3p[p];}
    
  private:
    HMdcSizesCells(void);
    ~HMdcSizesCells(void);
    Bool_t fillCont(Int_t sec);
    Bool_t fillTargetPos(HGeomVector* targetShift=0);
    Bool_t secOk(Int_t s) const  {return s>=0 && s<6;}
    Bool_t segOk(Int_t sg) const {return sg>=0 && sg<2;}
    Bool_t modOk(Int_t m) const  {return m>=0 && m<4;}

  ClassDef(HMdcSizesCells,0)
};

//----------------------- Inlines ------------------------------
inline const HGeomTransform& HMdcSizesCellsLayer::getRotLaySysRSec(Int_t c) const {
  return c<firstCellPart2 ? rotLaySysRSec : rotLaySysRSecPart2;
}

inline const Double_t* HMdcSizesCellsLayer::getRLSysRSec(Int_t c) const {
  return c<firstCellPart2 ? tRLaySysRSec : tRLaySysRSecPart2;
}

inline void HMdcSizesCellsLayer::transTo(Double_t& x, Double_t& y, Double_t& z) const {
  // transform. point x,y,z from sector coor.sys. to layer coor.sys.
  rotVectToLay(x-tSysRSec[9],y-tSysRSec[10],z-tSysRSec[11],x,y,z);
}

inline void HMdcSizesCellsLayer::transFrom(Double_t& x, Double_t& y, Double_t& z) const {
  // transform. point x,y,z from layer coor.sys. to sector coor.sys.
  Double_t pmtx = x;
  Double_t pmty = y;
  Double_t pmtz = z;
  x = tSysRSec[0]*pmtx+tSysRSec[1]*pmty+tSysRSec[2]*pmtz+tSysRSec[ 9];
  y = tSysRSec[3]*pmtx+tSysRSec[4]*pmty+tSysRSec[5]*pmtz+tSysRSec[10];
  z = tSysRSec[6]*pmtx+tSysRSec[7]*pmty+tSysRSec[8]*pmtz+tSysRSec[11];
}

inline Double_t HMdcSizesCellsLayer::calcWireY(Int_t cell) const {
  // Return Y in rot.layer coor.sys.
  if(cell < firstCellPart2) return cell*pitch - wireOffset;
  else                      return cell*pitch - wireOffsetPart2;
}

inline Int_t HMdcSizesCellsLayer::calcInnerSegCell(Double_t y) const {
  // return cell number for y 
  // y must be on the layer plane and in the rot.layer.coor.sys. !
  // Don't take into account layer SecondPart !
  // Use it for MDC plane I & II ONLY!
  // Now it is used in HMdcLookUpTb for vertex finder only.
  return (Int_t)(y*invPitch + cellOffset);
}

inline Int_t HMdcSizesCellsLayer::calcInnerSegCell(HGeomVector &p) const {
  // return cell number for point p
  // It assume that "p" belong the layer plane and in the sector coor.system !
  // Don't take into account layer SecondPart !
  // Use it for MDC plane I & II ONLY!
  // Now it is used in HMdcLookUpTb for vertex finder and offVertex track finder only.
  return (Int_t)(pntToCell[0]*p.X() + pntToCell[1]*p.Y() + pntToCell[2]*p.Z() + pntToCell[3]);
}
inline void HMdcSizesCellsLayer::getPntToCell(Double_t *arr) const {
  for(Int_t i=0;i<4;i++) arr[i] = pntToCell[i];
}

inline Double_t HMdcSizesCellsLayer::getDist(Double_t x1, Double_t y1, Double_t z1,
                                             Double_t x2, Double_t y2, Double_t z2, Int_t cell) const {
  // calc.  the minimal distance from line x1,y1,z1-x2,y2,z2  to wire.
  // x1,y1,z1,x2,y2,z2 - in sector coor.sys.
  Double_t y,z,dy,dz;
  const Double_t *transArr = getRLSysRSec(cell);
  rotateTo(transArr,x1-transArr[9],y1-transArr[10],z1-transArr[11],y,z);
  y -= cellsArray[cell].getWirePos();
  rotateTo(transArr,x2-x1,y2-y1,z2-z1,dy,dz);
  return TMath::Abs(y*dz-z*dy) / TMath::Sqrt(dz*dz+dy*dy);
}

inline void HMdcSizesCellsLayer::getYZinWireSys(Double_t x, Double_t y,
      Double_t z, Int_t cell, Double_t& yw, Double_t& zw) const {
  // Output: yw,zw - in wire coor.sys.
  // Wire coor.sys. is line y=z=0, Y - along layer
  // x,y,z - in sector coor.sys.
  const Double_t *transArr = getRLSysRSec(cell);
  rotateTo(transArr,x-transArr[9],y-transArr[10],z-transArr[11],yw,zw);
  yw -= cellsArray[cell].getWirePos();
}

inline Double_t HMdcSizesCellsLayer::getDistToZero(Double_t y1,Double_t z1,
    Double_t y2, Double_t z2) const {
  // calc. the minimal distance from line (y1,z1) - (y2,z2)  to poin
  // y=z=0 in plane y-z.
  Double_t dz  = z1-z2;
  Double_t dy  = y1-y2;
  Double_t lng = 1./TMath::Sqrt(dz*dz+dy*dy); //dz^2+dy^2=0 if tr.paral.to wire,only
  return TMath::Abs((z1*y2-z2*y1)*lng);
}

inline Double_t HMdcSizesCellsLayer::getImpactToZero(Double_t y1,Double_t z1,
    Double_t y2, Double_t z2, Double_t &alpha) const {
  // calc. the angle alpha (in degree) between line (y1,z1) - (y2,z2)
  // in Y-Z plane and Y axis and the minimal distance from this line
  // to poin y=z=0 in plane y-z.
  Double_t dz  = z1-z2;
  Double_t dy  = y1-y2;
  alpha        = TMath::ATan2(TMath::Abs(dz),TMath::Abs(dy))*TMath::RadToDeg();
  Double_t lng = 1./TMath::Sqrt(dz*dz+dy*dy); //dz^2+dy^2=0 if tr.paral.to wire,only
  return TMath::Abs((z1*y2-z2*y1)*lng);
}

inline void HMdcSizesCellsLayer::rotVectToLay(Double_t xi,Double_t yi,Double_t zi,
    Double_t& xo, Double_t& yo, Double_t& zo) const {
  // Input:  xi,yi,zi - sector coor.sys.
  // Output: xo,yo,zo - layer coor.sys.
  xo = tSysRSec[0]*xi+tSysRSec[3]*yi+tSysRSec[6]*zi;
  yo = tSysRSec[1]*xi+tSysRSec[4]*yi+tSysRSec[7]*zi;
  zo = tSysRSec[2]*xi+tSysRSec[5]*yi+tSysRSec[8]*zi;
}

inline void HMdcSizesCellsLayer::rotateTo(const Double_t* trans,
                                          Double_t xi,Double_t yi,Double_t zi,
                                          Double_t& yo, Double_t& zo) {
  // rotation by rotation martix "trans"
  yo = trans[1]*xi+trans[4]*yi+trans[7]*zi;
  zo = trans[2]*xi+trans[5]*yi+trans[8]*zi;
}

inline void HMdcSizesCellsLayer::rotateTo(const Double_t* trans,
                                          Double_t xi,Double_t yi,Double_t zi,
                                          Double_t& xo,Double_t& yo,Double_t& zo) {
  // rotation by rotation martix "trans"
  xo = trans[0]*xi+trans[3]*yi+trans[6]*zi;
  yo = trans[1]*xi+trans[4]*yi+trans[7]*zi;
  zo = trans[2]*xi+trans[5]*yi+trans[8]*zi;
}

inline void HMdcSizesCellsLayer::rotateTo(const Double_t* trans,const HGeomVector& d,
                                          Double_t& xo, Double_t& yo, Double_t& zo) {
  // Input:  xi,yi,zi - sector coor.sys.
  // Output: yo,zo - rotated layer coor.sys.
  xo = trans[0]*d.getX()+trans[3]*d.getY()+trans[6]*d.getZ();
  yo = trans[1]*d.getX()+trans[4]*d.getY()+trans[7]*d.getZ();
  zo = trans[2]*d.getX()+trans[5]*d.getY()+trans[8]*d.getZ();
}

inline void HMdcSizesCellsLayer::rotateTo(const Double_t* trans,const HGeomVector& d,
                                          Double_t& yo, Double_t& zo) {
  // Input:  xi,yi,zi - sector coor.sys.
  // Output: yo,zo - rotated layer coor.sys.
  yo = trans[1]*d.getX()+trans[4]*d.getY()+trans[7]*d.getZ();
  zo = trans[2]*d.getX()+trans[5]*d.getY()+trans[8]*d.getZ();
}

inline Double_t HMdcSizesCellsLayer::getImpact(const HMdcLineParam& ln,
    Int_t cell,Double_t &alpha) const {
  // calc. the angle alpha (in degree) between projection of line
  // "ln" on the Y-Z plane and Y axis in coor.sys.
  // of wires and the minimal distance from "ln" to wire.
  // "ln" - in sector coor.sys.
  Double_t y,z,dy,dz;
  return getImpact(ln,cell,alpha,y,z,dy,dz);
}

inline Double_t HMdcSizesCellsLayer::getImpact(const HMdcLineParam& ln,Int_t cell,
    Double_t &alpha, Double_t &y,Double_t &z,Double_t &dy,Double_t &dz) const {
  // calc. the angle alpha (in degree) between projection of line
  // "ln" on the Y-Z plane and Y axis in coor.sys.
  // of wires and the minimal distance from "ln" to wire.
  // "ln" - in sector coor.sys.
  Int_t parSec = ln.getSec();
  if(parSec==sector || parSec<0) {
    const Double_t *trans = getRLSysRSec(cell);
    rotateTo(trans,ln.x1()-trans[9],ln.y1()-trans[10],ln.z1()-trans[11],y,z);
    y -= cellsArray[cell].getWirePos();
    rotateTo(trans,ln.getDir(),dy,dz);
  } else calcInOtherSecSys(ln,cell,y,z,dy,dz);
//  alpha = TMath::ATan2(TMath::Abs(dz),TMath::Abs(dy))*TMath::RadToDeg();
  alpha = calcAlphaImpact(dy/dz);
  return TMath::Abs((y*dz-z*dy)/TMath::Sqrt(dz*dz+dy*dy));
}

inline Double_t HMdcSizesCellsLayer::getDist(const HMdcLineParam& ln,Int_t cell) const {
  // calc. the angle alpha (in degree) between projection of line
  // "ln" on the Y-Z plane and Y axis in coor.sys.
  // of wires and the minimal distance from "ln" to wire.
  // "ln" - in sector coor.sys.
  Int_t parSec = ln.getSec();
  Double_t y,z,dy,dz;
  if(parSec==sector || parSec<0) {
    const Double_t *trans = getRLSysRSec(cell);
    rotateTo(trans,ln.x1()-trans[9],ln.y1()-trans[10],ln.z1()-trans[11],y,z);
    y -= cellsArray[cell].getWirePos();
    rotateTo(trans,ln.getDir(),dy,dz);
  } else calcInOtherSecSys(ln,cell,y,z,dy,dz);
  return TMath::Abs((y*dz-z*dy)/TMath::Sqrt(dz*dz+dy*dy));
}

inline Double_t HMdcSizesCellsLayer::getAlpha(const HMdcLineParam& ln,Int_t cell) const {
  // calc. the angle alpha (in degree) between projection of line
  // "ln" on the Y-Z plane and Y axis in coor.sys. of wire
  Int_t parSec = ln.getSec();
  Double_t dy,dz;
  if(parSec==sector || parSec<0) rotateTo(getRLSysRSec(cell),ln.getDir(),dy,dz);
  else {
    HGeomTransform rotLSysROtherSec;
    getRotLSysForOtherSecSys(parSec,cell,rotLSysROtherSec);
    const HGeomRotation& rot = rotLSysROtherSec.getRotMatrix();
    HMdcSizesCells::rotateDir(rot,ln.getDir(),dy,dz);
  }
  return calcAlphaImpact(dy/dz); //TMath::ATan2(TMath::Abs(dz),TMath::Abs(dy))*TMath::RadToDeg(); // alpha
}

inline Bool_t HMdcSizesCellsLayer::getImpact(const HMdcLineParam& ln,Int_t cell,
    Double_t &alpha, Double_t &minDist) const {
  // return kTRUE if line x1,y1,z1-x2,y2,z2 intersect cell or kFALSE
  // if not intersect and calc. the angle alpha (in degree) between projection
  // of the line  x1,y1,z1-x2,y2,z2 on the Y-Z plane and Y axis in coor.
  // sys. of wires and the minimal distance from line x1,y1,z1-x2,y2,z2
  // to wire.
  // x1,y1,z1,x2,y2,z2 - in sector coor.sys.
  Double_t y,z,dy,dz;
  return getImpact(ln,cell,alpha,minDist,y,z,dy,dz);
}

inline Bool_t HMdcSizesCellsLayer::getImpact(const HMdcLineParam& ln,Int_t cell,
    Double_t &alpha, Double_t &minDist,
    Double_t &y,Double_t &z,Double_t &dy,Double_t &dz) const {
  // return kTRUE if line x1,y1,z1-x2,y2,z2 intersect cell or kFALSE
  // if not intersect and calc. the angle alpha (in degree) between projection
  // of the line  x1,y1,z1-x2,y2,z2 on the Y-Z plane and Y axis in coor.
  // sys. of wires and the minimal distance from line x1,y1,z1-x2,y2,z2
  // to wire.
  // x1,y1,z1,x2,y2,z2 - in sector coor.sys.
  Int_t parSec = ln.getSec();
  if(parSec==sector || parSec<0) {
    const Double_t *trans = getRLSysRSec(cell);
    rotateTo(trans,ln.x1()-trans[9],ln.y1()-trans[10],ln.z1()-trans[11],y,z);
    y -= cellsArray[cell].getWirePos();
    rotateTo(trans,ln.getDir(),dy,dz);
  } else calcInOtherSecSys(ln,cell,y,z,dy,dz);
  
  Double_t lng   = 1./TMath::Sqrt(dz*dz+dy*dy);
  Double_t yDist = TMath::Abs(y*dz-z*dy); // abs(Ywire-Ycross_track)=yDist/dz
  minDist        = yDist*lng;
  Double_t dza   = TMath::Abs(dz);
  Double_t dya   = TMath::Abs(dy);
  alpha          = calcAlphaImpact(dya/dza); //TMath::ATan2(dza,dya)*TMath::RadToDeg();
  if(minDist*dza*lng > halfPitch) {
    if(dya==0.0) return kFALSE;
    if((yDist-halfPitch*dza)/dya > halfCatDist)  return kFALSE;
  } else if(minDist*dya*lng > halfCatDist &&    // dya*lng = cos(alpha)
      (yDist-halfCatDist*dya)/dza > halfPitch) return kFALSE;
  return kTRUE;
}

inline void HMdcSizesCellsMod::transTo(Double_t& x, Double_t& y, Double_t& z)
    const {
  // transform. point x,y,z from sector coor.sys. to mdc coor.sys.
  rotVectTo(x-tSysRSec[ 9],y-tSysRSec[10],z-tSysRSec[11],x,y,z);
}

inline void HMdcSizesCellsMod::transTo(const Double_t *v, Double_t *vo) const {
  vo[0] = tSysRSecOp[0]*v[0]+tSysRSecOp[1]*v[1]+tSysRSecOp[2]*v[2]+tSysRSecOp[ 9];
  vo[1] = tSysRSecOp[3]*v[0]+tSysRSecOp[4]*v[1]+tSysRSecOp[5]*v[2]+tSysRSecOp[10];
  vo[2] = tSysRSecOp[6]*v[0]+tSysRSecOp[7]*v[1]+tSysRSecOp[8]*v[2]+tSysRSecOp[11];
  
  vo[3] = tSysRSecOp[0]*v[3]+tSysRSecOp[1]*v[4]+tSysRSecOp[2]*v[5];
  vo[4] = tSysRSecOp[3]*v[3]+tSysRSecOp[4]*v[4]+tSysRSecOp[5]*v[5];
  vo[5] = tSysRSecOp[6]*v[3]+tSysRSecOp[7]*v[4]+tSysRSecOp[8]*v[5];
}

inline void HMdcSizesCellsMod::rotVectTo(Double_t xi,Double_t yi,Double_t zi,
    Double_t& xo, Double_t& yo, Double_t& zo) const {
  // Input:  xi,yi,zi - sector coor.sys.
  // Output: xo,yo,zo - module coor.sys.
  xo = tSysRSec[0]*xi+tSysRSec[3]*yi+tSysRSec[6]*zi;
  yo = tSysRSec[1]*xi+tSysRSec[4]*yi+tSysRSec[7]*zi;
  zo = tSysRSec[2]*xi+tSysRSec[5]*yi+tSysRSec[8]*zi;
}

inline void HMdcSizesCellsMod::transFrom(Double_t& x, Double_t& y, Double_t& z) const {
  // transform. point x,y,z from mdc coor.sys. to sector coor.sys.
  Double_t pmtx = x;
  Double_t pmty = y;
  Double_t pmtz = z;
  x = tSysRSec[0]*pmtx+tSysRSec[1]*pmty+tSysRSec[2]*pmtz+tSysRSec[ 9];
  y = tSysRSec[3]*pmtx+tSysRSec[4]*pmty+tSysRSec[5]*pmtz+tSysRSec[10];
  z = tSysRSec[6]*pmtx+tSysRSec[7]*pmty+tSysRSec[8]*pmtz+tSysRSec[11];
}

inline void HMdcSizesCellsMod::transFromZ0(Double_t& x,Double_t& y,Double_t& z) const {
  // transform. point x,y on the mdc plane (z=0) from mdc coor.sys. 
  // to sector coor.sys.
  Double_t pmtx = x;
  Double_t pmty = y;
  x = tSysRSec[0]*pmtx+tSysRSec[1]*pmty+tSysRSec[ 9];
  y = tSysRSec[3]*pmtx+tSysRSec[4]*pmty+tSysRSec[10];
  z = tSysRSec[6]*pmtx+tSysRSec[7]*pmty+tSysRSec[11];
}

inline void HMdcSizesCellsMod::transFromZ0(Float_t& x,Float_t& y,Float_t& z) const {
  // transform. point x,y on the mdc plane (z=0) from mdc coor.sys. 
  // to sector coor.sys.
  Double_t pmtx = x;
  Double_t pmty = y;
  x = tSysRSec[0]*pmtx+tSysRSec[1]*pmty+tSysRSec[ 9];
  y = tSysRSec[3]*pmtx+tSysRSec[4]*pmty+tSysRSec[10];
  z = tSysRSec[6]*pmtx+tSysRSec[7]*pmty+tSysRSec[11];
}

inline void HMdcSizesCellsMod::calcInterTrMod(Double_t x1, Double_t y1, Double_t z1,
      Double_t x2, Double_t y2, Double_t z2,
      Double_t& x, Double_t& y) const {
  Double_t dX  = x2-x1;
  Double_t dY  = y2-y1;
  Double_t dZ  = z2-z1;
  Double_t del = 1/(parA*dX+parB*dY+dZ);
  Double_t xt  = (dX*(parD-z1-parB*y1)+x1*(parB*dY+dZ))*del-tSysRSec[ 9];
  Double_t yt  = (dY*(parD-z1-parA*x1)+y1*(parA*dX+dZ))*del-tSysRSec[10];
  x = ct[0]*xt+ct[1]*yt+ct[2];
  y = ct[3]*xt+ct[4]*yt+ct[5];
}

inline void HMdcSizesCellsMod::calcMdcHit(Double_t x1, Double_t y1, Double_t z1,
      Double_t x2, Double_t y2, Double_t z2,
      Double_t& x, Double_t& y, Double_t& xDir, Double_t& yDir) const {
  // Calcul. a cross of the line with plane MDC (parA*x+parB*y+c*z=parD),
  // transform. this point to mdc coor.sys. (z=0) and calc.
  // hit direction in mdc coor.sys.
  Double_t dX  = x2-x1;
  Double_t dY  = y2-y1;
  Double_t dZ  = z2-z1;
  Double_t del = 1/(parA*dX+parB*dY+dZ);
  Double_t xt  = (dX*(parD-z1-parB*y1)+x1*(parB*dY+dZ))*del-tSysRSec[ 9];
  Double_t yt  = (dY*(parD-z1-parA*x1)+y1*(parA*dX+dZ))*del-tSysRSec[10];
  x = ct[0]*xt+ct[1]*yt+ct[2];
  y = ct[3]*xt+ct[4]*yt+ct[5];
  //---Hit direction----------------------------------------------------
  Double_t length = 1/TMath::Sqrt(dX*dX+dY*dY+dZ*dZ);
  xDir = (tSysRSec[0]*dX+tSysRSec[3]*dY+tSysRSec[6]*dZ)*length; // unit vector
  yDir = (tSysRSec[1]*dX+tSysRSec[4]*dY+tSysRSec[7]*dZ)*length;
}

inline void HMdcSizesCells::calcMdcSeg(Double_t x1, Double_t y1, Double_t z1, 
                                Double_t x2, Double_t y2, Double_t z2, 
    Double_t &zm, Double_t &r0, Double_t &theta, Double_t &phi) {
  // Input and output are in sector coor.sys.
  // theta=atan(TMath::Sqrt(dX*dX+dY*dY)/dZ);  phi=atan(dY/dX)
  // zm= z1 - cos(theta)/sin(theta) * (x1*cos(phi)+y1*sin(phi))
  // r0=y1*cos(phi)-x1*sin(phi)
  //
  // If (x1,y1,z1)=(x2,y2,z2) dZ will eq.1.! 
  Double_t dX    = x2-x1;
  Double_t dY    = y2-y1;
  Double_t dZ    = z2-z1;
  Double_t lenXY = TMath::Sqrt(dX*dX+dY*dY);
  if(lenXY<2.E-5) {
    dX    = x2 * 1.E-5/TMath::Sqrt(x2*x2+y2*y2);
    dY    = y2 * 1.E-5/TMath::Sqrt(x2*x2+y2*y2);
    lenXY = 1.E-5;            //dX*dX+dY*dY;
    dZ    = 1.;
  }
  dX   /= lenXY;
  dY   /= lenXY;
  dZ   /= lenXY;
  phi   = TMath::ATan2(dY,dX);
  theta = TMath::ATan2(1.,dZ);
  zm    = z1-dZ*(x1*dX+y1*dY);
  r0    = y1*dX-x1*dY;
}

Float_t HMdcSizesCellsLayer::calcAlphaImpact(Double_t dydz) {
  // return impact angle alpha for the tan=dy/dz
  // dy,dz - must be in coor.sys. of rotated layer (wires are pararalel to x-axis)
  //
  // alpha[n] = impact angle calculated by 
  // for(Int_t i=0;i<120;i++) alpha[i] = TMath::ATan2(1,i*0.001)*TMath::RadToDeg();
  //  bin step = 0.01 !
  static Float_t alpha[120] = {
    90,89.4271,88.8542,88.2816,87.7094,87.1376,86.5664,85.9958,85.4261,84.8572,84.2894,83.7227,
    83.1572,82.5931,82.0304,81.4692,80.9097,80.352,79.796,79.242,78.6901,78.1402,77.5926,77.0472,
    76.5043,75.9638,75.4258,74.8904,74.3577,73.8278,73.3008,72.7766,72.2553,71.7371,71.222,70.71,
    70.2011,69.6955,69.1932,68.6942,68.1986,67.7064,67.2176,66.7323,66.2505,65.7723,65.2976,64.8265,
    64.359,63.8951,63.4349,62.9784,62.5256,62.0764,61.631,61.1892,60.7512,60.3169,59.8863,59.4594,
    59.0362,58.6168,58.2011,57.7891,57.3808,56.9761,56.5752,56.1779,55.7843,55.3943,55.008,54.6252,
    54.2461,53.8706,53.4986,53.1301,52.7652,52.4037,52.0458,51.6913,51.3402,50.9925,50.6482,50.3073,
    49.9697,49.6355,49.3045,48.9767,48.6522,48.3309,48.0128,47.6978,47.3859,47.0772,46.7715,46.4688,
    46.1691,45.8725,45.5787,45.2879,45,44.715,44.4327,44.1533,43.8767,43.6028,43.3317,43.0632,
    42.7974,42.5342,42.2737,42.0157,41.7603,41.5074,41.257,41.0091,40.7636,40.5205,40.2799,40.0415};
//  if(!TMath::Finite(dydz)) return 90.;     
  if( dydz < 0.) dydz = -dydz;
  if( dydz < 1.19) {
    Double_t bind = dydz*100;
    Int_t    bin  = bind;
    return alpha[bin] + (alpha[bin+1]-alpha[bin])*(bind - bin);
  }
  return TMath::ATan2(1.,dydz)*TMath::RadToDeg();
}

#endif  /*!HMDCSIZESCELLS_H*/
