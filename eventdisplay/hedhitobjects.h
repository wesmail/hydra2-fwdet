#ifndef __EDHITOBJECTS__
#define __EDHITOBJECTS__

#include "TEveElement.h"
#include "TEvePointSet.h"
#include "TEveLine.h"
#include "TEveCompound.h"
#include "TEveQuadSet.h"
#include "TEveFrameBox.h"


class HMdcSegSim;
class HMdcCal1Sim;
class HTofHitSim;
class HWallHitSim;
class HTofClusterSim;
class HShowerHit;
class HShowerHitSim;
class HEmcCluster;
class HEmcClusterSim;
class HRichHitSim;
class HRpcClusterSim;
class HMetaMatch2;
class candidate;
class HParticleCandSim;
class HGeantKine;
class HGeantRichMirror;
class HGeantRichDirect;


class HGeomVector;


//----------------------------------------------------------------
class HEDVertex : public TEvePointSet {
private:

public:
    HEDVertex();
    virtual ~HEDVertex();
    void Print();

    ClassDef(HEDVertex,0)    // event vertex
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDSegment : public TEveLine {
private:

public:
    HEDSegment(HMdcSegSim* seg = 0);
    virtual ~HEDSegment();
    void Print();

    ClassDef(HEDSegment,0)   // MDC segment
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDMdcWire : public TEveLine {
private:
    Int_t nTimes;                  // how many times this wire has been used
    Bool_t makeWire(Int_t s,Int_t m,Int_t l,Int_t c,HMdcCal1Sim* cal = 0);
public:
    HEDMdcWire(HMdcCal1Sim* cal = 0);
    HEDMdcWire(Int_t s,Int_t m,Int_t l,Int_t c);
    virtual ~HEDMdcWire();
    void  setNtimes(Int_t times = 0) { nTimes = times;}
    Int_t getNtimes()                { return  nTimes ; }
    void  countUp()                  { nTimes++; }
    void  countDown()                { nTimes--; if(nTimes < 0) nTimes = 0;}
    void  Print();

    ClassDef(HEDMdcWire,0)   // MDC wire
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDRichHit : public TEveLine {
private:

public:
    HEDRichHit(HRichHitSim* hit = 0,HParticleCandSim* cand = 0);
    virtual ~HEDRichHit();
    void Print();

    ClassDef(HEDRichHit,0)    // RICH hit
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDWallHit : public TEvePointSet {
private:

public:
    HEDWallHit(HWallHitSim* hit = 0);
    virtual ~HEDWallHit();
    void Print();

    ClassDef(HEDWallHit,0)     // WALL hit
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDWallPlane : public TEveQuadSet {
private:
    TEveFrameBox* box;
public:
    HEDWallPlane();
    virtual ~HEDWallPlane();
    void Print();

    ClassDef(HEDWallPlane,0)   // Wall plane + fired cells
};
//----------------------------------------------------------------


//----------------------------------------------------------------
class HEDTofHit : public TEvePointSet {
private:

public:
    HEDTofHit(HTofHitSim* hit = 0);
    virtual ~HEDTofHit();
    void Print();

    ClassDef(HEDTofHit,0)     // TOF hit
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDTofCluster : public TEvePointSet {
private:

public:
    HEDTofCluster(HTofClusterSim* hit = 0);
    virtual ~HEDTofCluster();
    void Print();

    ClassDef(HEDTofCluster,0)   // TOF cluster
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDRpcCluster : public TEvePointSet {
private:

public:
    HEDRpcCluster(HRpcClusterSim* hit = 0);
    virtual ~HEDRpcCluster();
    void Print();

    ClassDef(HEDRpcCluster,0)  // RPC cluster
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDShowerHit : public TEvePointSet {
private:

public:
    HEDShowerHit(HShowerHitSim* hit = 0);
    virtual ~HEDShowerHit();
    void Print();

    ClassDef(HEDShowerHit,0)   // SHOWER hit
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDEmcCluster : public TEvePointSet {
private:

public:
    HEDEmcCluster(HEmcClusterSim* hit = 0);
    virtual ~HEDEmcCluster();
    void Print();

    ClassDef(HEDEmcCluster,0)   // EMC Cluster
};
//----------------------------------------------------------------


//----------------------------------------------------------------
class HEDParticleCand : public TEveCompound {

private:
public:
    HEDParticleCand(HParticleCandSim* cand = 0);
    virtual ~HEDParticleCand();
    void Print();
    void SetLineColor  (Color_t val) { ((TEveLine*)FindChild("EDParticleCand"))->SetLineColor(val);}
    void SetLineStyle  (Style_t val) { ((TEveLine*)FindChild("EDParticleCand"))->SetLineStyle(val);}
    void SetLineWidth  (Style_t val) { ((TEveLine*)FindChild("EDParticleCand"))->SetLineWidth(val);}
    void SetMarkerColor(Color_t val) { ((TEveLine*)FindChild("EDParticleCand"))  ->SetLineColor(val); }
    void SetMarkerStyle(Style_t val) { ((TEveLine*)FindChild("EDParticleCand"))->SetMarkerStyle(val);}
    void SetMarkerSize (Size_t val)  { ((TEveLine*)FindChild("EDParticleCand"))->SetMarkerSize(val) ;}
    void SetRnrLine    (Bool_t val)  { ((TEveLine*)FindChild("EDParticleCand"))  ->SetRnrLine(val);}
    void SetRnrPoints  (Bool_t val)  { ((TEveLine*)FindChild("EDParticleCand"))->SetRnrPoints(val);}
    ClassDef(HEDParticleCand,0)  // RICH hit + inner/outer segment + kickplane hit + META hit
};
//----------------------------------------------------------------


//----------------------------------------------------------------
class HEDRichPadPlane : public TEveQuadSet {
private:
    TEveFrameBox* box;
public:
    HEDRichPadPlane(Int_t sec,Int_t cleaned=0);
    virtual ~HEDRichPadPlane();
    void Print();

    ClassDef(HEDRichPadPlane,0)   // RICH pad plane + fired cells per sector
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDRich700PadPlane : public TEveQuadSet {
private:
public:
    HEDRich700PadPlane();
    virtual ~HEDRich700PadPlane();
    void Print();

    ClassDef(HEDRich700PadPlane,0)   // RICH700 pad plane fired cells
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDRichRing : public TEveLine {
private:

public:
    HEDRichRing(HRichHitSim* hit = 0);
    virtual ~HEDRichRing();
    void Print();

    ClassDef(HEDRichRing,0)       // RICH ring at pad plane
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDRichHitPadPlane : public TEvePointSet {
private:

public:
    HEDRichHitPadPlane(HRichHitSim* hit = 0);
    virtual ~HEDRichHitPadPlane();
    void Print();

    ClassDef(HEDRichHitPadPlane,0) // RICH hit at pad plane
};
//----------------------------------------------------------------

class HEDRichGeantPadPlane : public TEvePointSet {
private:

public:
    HEDRichGeantPadPlane(HGeantKine* kine = 0,Int_t select=0,HGeantRichDirect* geaDir=0); // select = 0 -> photon, 1 = direct
    virtual ~HEDRichGeantPadPlane();
    void Print();

    ClassDef(HEDRichGeantPadPlane,0) // RICH hit at pad plane (GEANT)
};
//----------------------------------------------------------------

class HEDRichGeantMirror : public TEvePointSet {
private:

public:
    HEDRichGeantMirror(HGeantRichMirror*);
    virtual ~HEDRichGeantMirror();
    void Print();

    ClassDef(HEDRichGeantMirror,0) // RICH hit at Mirror (GEANT)
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDRichCompound : public TEveCompound {

private:

public:
    HEDRichCompound(HRichHitSim* hit = 0);
    virtual ~HEDRichCompound();
    void Print();
    void SetLineColor  (Color_t val) { ((TEveLine*)    FindChild("EDRichRing")) ->SetLineColor(val);}
    void SetLineStyle  (Style_t val) { ((TEveLine*)    FindChild("EDRichRing")) ->SetLineStyle(val);}
    void SetLineWidth  (Style_t val) { ((TEveLine*)    FindChild("EDRichRing")) ->SetLineWidth(val);}
    void SetColor(Color_t val) { // change Mirror + hit at the same time
	((TEvePointSet*)FindChild("EDRichHitPadPlane"))->SetMarkerColor(val);
        ((TEveLine*)    FindChild("EDRichHitMirror"))  ->SetLineColor(val);
    }
    void SetColorMirrorHit(Color_t val) { // change Mirror hit
	((TEveLine*)    FindChild("EDRichHitMirror"))  ->SetLineColor(val);
    }
    void SetColorPadPlaneHit(Color_t val) { // change Mirror hit
	((TEvePointSet*)FindChild("EDRichHitPadPlane"))->SetMarkerColor(val);
    }
    void SetMarkerStyle(Style_t val) { ((TEvePointSet*)FindChild("EDRichHitPadPlane"))->SetMarkerStyle(val);}
    void SetMarkerSize (Size_t val)  { ((TEvePointSet*)FindChild("EDRichHitPadPlane"))->SetMarkerSize(val) ;}
    void SetRnrLine    (Bool_t val)  { ((TEveLine*)    FindChild("EDRichHitMirror"))  ->SetRnrLine(val);}
    void SetRnrPoints  (Bool_t val)  { ((TEvePointSet*)FindChild("EDRichHitPadPlane"))->SetRnrSelf(val);}

    ClassDef(HEDRichCompound,0)  // RICH hit at pad plane + ring + mirror hit


};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDGroup : public TEveElementList {

private:

    Int_t size;
    TEveElementList** list;

public:
    HEDGroup(const Char_t* name = "",const Char_t* title = "",Int_t n = 6, const Char_t* subname = "Sector");
    virtual ~HEDGroup();
    TEveElementList* getList(Int_t n)                     const { if(n >= 0 && n < size) {return  list[n];} else { return 0;}}
    void             AddElement(Int_t n, TEveElement* el)       { if(n >= 0 && n < size) (list[n])->AddElement(el);}
    void             DestroyElements()                          { for(Int_t n = 0; n < size; n ++) list[n]->DestroyElements(); }
    ClassDef(HEDGroup,0)  // Group objects in n lists like fashion


};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDGroup2D : public TEveElementList {

private:
    Int_t size1,size2;;

    TEveElementList** list1;
    TEveElementList*** list2;

public:
    HEDGroup2D(const Char_t* name = "",const Char_t* title = "",Int_t n1 = 6,Int_t n2 = 4, const Char_t* subname1 = "Sector", const Char_t* subname2 = "Module");
    virtual ~HEDGroup2D();
    TEveElementList* getList(Int_t i,Int_t j)                   const { if(i >= 0 && i < size1 && j >= 0 && j < size2) {return  list2[i][j];} else { return 0;}}
    void             AddElement(Int_t i,Int_t j,TEveElement* el)      { if(i >= 0 && i < size1 && j >= 0 && j < size2) (list2[i][j])->AddElement(el);}
    void             DestroyElements()                                { for(Int_t i = 0; i < size1; i ++) for(Int_t j = 0; j < size2; j ++) list2[i][j]->DestroyElements(); }
    ClassDef(HEDGroup2D,0)  // Group objects in 2 dim array lists


};
//----------------------------------------------------------------





#endif  // hit object definition







