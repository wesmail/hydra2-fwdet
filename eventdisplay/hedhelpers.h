#ifndef __EDHELPERS__
#define __EDHELPERS__

#include "TObject.h"
#include "hreconstructor.h"



using namespace std;

class HParticleCand;
class HMdcSeg;
class HTofHit;
class HWallHit;
class HShowerHit;
class HEmcCluster;
class HRichHit;
class HRpcCluster;
class HWallHit;
class HRichCal;
class HGeantKine;
class HGeantKine;

class TEveFrameBox;
class TEveTrack;
class TEveTrackPropagator;
class HGeomVector;
class HGeomTransform;
class TGeoSphere;

#define TO_CM 0.1



//----------------------------------------------------------------
class HEDTransform : public TObject {
    static HGeomTransform* richSecTrans;
    static HGeomTransform* richMirrorTrans;
    static TGeoSphere*     richMirror;
    static Bool_t          fisNewRich;
    static Bool_t          fisEmc;
public:
    static void            setIsNewRich(Bool_t newrich=kTRUE) { fisNewRich = newrich;}
    static Bool_t          isNewRich()             { return fisNewRich;}
    static void            setIsEmc(Bool_t is=kTRUE) { fisEmc = is;}
    static Bool_t          isEmc()                   { return fisEmc;}
    static Float_t         calcPhiToLab            (Int_t sec);
    static void            setRichSecTrans         (Double_t x,Double_t y,Double_t z,Double_t rot1,Double_t rot2,Double_t rot3);
    static void            setRichMirrorTrans      (Double_t x,Double_t y,Double_t z,Double_t rot1,Double_t rot2,Double_t rot3);
    static HGeomTransform* getRichSecTrans         ();
    static HGeomTransform* getRichMirrorTrans      ();
    static void            setRichMirror(TGeoSphere* mirr) {richMirror = mirr;}
    static TGeoSphere*     getRichMirror           () { return richMirror;}
    static Bool_t          calcSegPointsLab        (HMdcSeg* seg ,HGeomVector& p1,HGeomVector& p2);
    static Bool_t          calcWirePointsLab       (Int_t s,Int_t m,Int_t l,Int_t c,HGeomVector& p1,HGeomVector& p2);
    static Bool_t          calcSegKickPlanePointLab(HMdcSeg* seg,HGeomVector& p);
    static Bool_t          calcWallHitPointLab     (HWallHit* hit ,HGeomVector& p);
    static Bool_t          calcTofHitPointLab      (HTofHit* hit ,HGeomVector& p);
    static Bool_t          calcShowerHitPointLab   (HShowerHit* hit ,HGeomVector& p);
    static Bool_t          calcEmcClusterPointLab  (HEmcCluster* hit ,HGeomVector& p);
    static Bool_t          calcRpcClustPointLab    (HRpcCluster* hit ,HGeomVector& p);
    static Bool_t          calcVertexPoint         (HGeomVector& p);
    static Bool_t          calcRichLinePointLab    (HRichHit* hit,HGeomVector& p1,HGeomVector& p2,HParticleCand* cand=0);
    static Bool_t          calcRichPadPlaneToLab   (Int_t sec,HGeomVector& p, HGeomTransform& trans);
    static TEveFrameBox*   calcRichSectorFrame     (Int_t sec,HGeomTransform& trans);
    static TEveFrameBox*   calcWallFrame           ();
    static Bool_t          calcWallCell            (HWallHit* hit,Float_t* coord /* [12], xyz 4 corners*/);
    static Bool_t          calcRichPadSector       (HRichCal* cal, HGeomTransform& trans, Float_t* coord /* [12], xyz 4 corners*/);
    static Bool_t          calcRichMirrorHit       (const HGeomVector& p1, const HGeomVector& p2, HGeomVector& pout );
    static TEveTrack*      createParticle          (Int_t pid,Double_t vx,Double_t vy,Double_t vz,Double_t px,Double_t py,Double_t pz,TEveTrackPropagator* prop);
    static TEveTrack*      createKineParticle      (HGeantKine*,TEveTrackPropagator*);

    static Bool_t          calcRichGeantPadplanePointLab(Int_t s,HGeomVector& p);
    static Bool_t          calcMdcGeantLayerPointLab    (Int_t s,Int_t m,Int_t l,HGeomVector& p);
    static Bool_t          calcWallGeantPointLab        (Int_t c,HGeomVector& p);
    static Bool_t          calcTofGeantPointLab         (Int_t s,Int_t m,Int_t c,HGeomVector& p);
    static Bool_t          calcRpcGeantPointLab         (Int_t s,HGeomVector& p);
    static Bool_t          calcShowerGeantPointLab      (Int_t s,Int_t m,HGeomVector& p);
    ClassDef(HEDTransform,0)
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDMdcWireManager : public TObject {

    Int_t wires[6][4][6][220]; //! wire address -> ntimes
    void  clear();
    Int_t fillAllWires();
    Int_t fillSegmentWires();


public:
    HEDMdcWireManager();
    ~HEDMdcWireManager(void);
    Int_t fill();
    Int_t isUsedNtimes(Int_t s,Int_t m,Int_t l,Int_t c);
    void  getAddress(Int_t& s,Int_t& m,Int_t& l,Int_t& c, Int_t& addr) {
	s = ((addr >> 0) & 0x7 );   // 3 bits starting at bit 1
        m = ((addr >> 3) & 0x3 );   // 2 bits starting at bit 4
        l = ((addr >> 5) & 0x7 );   // 3 bits starting at bit 6
        c = ((addr >> 8) & 0xFF);   // 8 bits starting at bit 9
    }
    Int_t setAddress(Int_t s,Int_t m,Int_t l,Int_t c) {
        Int_t addr = 0;
        addr = ( ((s & 0x7 ) << 0 ) | addr); // 3 bits starting at bit 1
	addr = ( ((m & 0x3 ) << 3 ) | addr); // 2 bits starting at bit 4
        addr = ( ((l & 0x7 ) << 5 ) | addr); // 3 bits starting at bit 6
        addr = ( ((c & 0xFF) << 8 ) | addr); // 8 bits starting at bit 9
        return addr;
    }

    ClassDef(HEDMdcWireManager,0) // helper class to do wire statistics
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDMakeContainers : public HReconstructor {
public:
    HEDMakeContainers(const Text_t* name="EDMakeContainers",const Text_t* title="Container for ED"){;}
    ~HEDMakeContainers(void){;}
    Bool_t init(void);
    Bool_t reinit(void);
    Int_t  execute(void){ return 0;}
    Bool_t finalize(void) {return kTRUE;}
    ClassDef(HEDMakeContainers,0) // dummy tasks to create all needed containers
};
//----------------------------------------------------------------




#endif  // helpers definition







