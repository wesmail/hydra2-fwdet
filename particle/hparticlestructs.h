#ifndef __HPARTICLESTRUCTS_H__
#define __HPARTICLESTRUCTS_H__


#include "hmetamatch2.h"
#include "htofcluster.h"
#include "htofclustersim.h"
#include "htofhit.h"
#include "htofhitsim.h"
#include "hrpccluster.h"
#include "hrpcclustersim.h"
#include "hshowerhit.h"
#include "hshowerhitsim.h"
#include "hemccluster.h"
#include "hemcclustersim.h"
#include "hrichhit.h"
#include "hrichhitsim.h"
#include "hmdctrkcand.h"
#include "hmdcseg.h"
#include "hmdcsegsim.h"
#include "hsplinetrack.h"
#include "hrktrackB.h"
#include "hkaltrack.h"
#include "hparticlecand.h"
#include "hparticlecandsim.h"
#include "hgeantkine.h"

#include "hcategory.h"

#include "hparticledef.h"
#include "hparticletool.h"
#include "hcategorymanager.h"
#include "hstltool.h"
#include "tofdef.h"

#include "TObject.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstdio>

using namespace std;
using namespace Particle;

//---------------------------------------------------------------------------------
// some helper objects
//
//  HParticleCandFiller

class pointers : public TObject {
public:
    pointers(){;}
    ~pointers(){;}

    //---------------------------
    // Object pointers simulation
    HMdcTrkCand*      pMdcTrk;    //
    HMdcSegSim*       pSeg1;      //
    HMdcSegSim*       pSeg2;      //
    HRichHitSim*      pRichHit;   //
    HTofClusterSim*   pTofClst;   //
    HTofHitSim*       pTofHit1;   //
    HTofHitSim*       pTofHit2;   //
    HRpcClusterSim*   pRpcClst;   //
    HShowerHitSim*    pShowerHit; //
    HEmcClusterSim*   pEmcClst; //
    HRKTrackB*        pRk;        //
    HSplineTrack*     pSpline;    //
    HKalTrack*        pKalman;    //
    //---------------------------

    void reset(void){
	pMdcTrk      = NULL;
	pSeg1        = NULL;
	pSeg2        = NULL;
	pRichHit     = NULL;
	pTofClst     = NULL;
	pTofHit1     = NULL;
	pTofHit2     = NULL;
	pRpcClst     = NULL;
	pShowerHit   = NULL;
	pEmcClst     = NULL;
	pRk          = NULL;
	pSpline      = NULL;
	pKalman      = NULL;
    }

  ClassDef(pointers,0)
};


class closetrack : public TObject {
public:
    closetrack(){;}
    ~closetrack(){;}
    Int_t ind;        // index of candidate
    Float_t oAngle;   // opening angle to candidate
    void print(Int_t ind=-1){
	printf("index %5i angle %8.5f\n",ind,oAngle);
    }
    ClassDef(closetrack,1)
};

class closeVec : public TObject {
public:
    vector < closetrack > tracks;  // vector of closetrack objects (index,oAngle)

    closeVec(){ ; }
    ~closeVec(){ ; }

    Int_t findTrack(Int_t ind){
	// return -1 if track does not exist, otherwise index
	for(UInt_t i = 0; i < tracks.size(); i++){
	    if((tracks[i].ind) == ind) return i;
	}
	return -1;
    }
    void addTrack(Int_t index,Float_t oAngle){
	// add track if does not exist
	Int_t ind = findTrack(index);
	if( ind < 0){
	    closetrack track;
	    track.ind    = index;
	    track.oAngle = oAngle;
	    tracks.push_back(track);
	}
    }
    void reset(){
	tracks.clear();
    }
    Bool_t static cmpAngle(const closetrack& a,const closetrack& b) { // sort tracks by opening angle
	return a.oAngle < b.oAngle;
    }
    void sortAngle(void) {
	std::sort(tracks.begin(),tracks.end(),closeVec::cmpAngle);
    }
    ClassDef(closeVec,1)
};

class trackinfo : public TObject {
public:
    trackinfo(){;}
    ~trackinfo(){;}
    Int_t tr;         // GEANT tr number
    Int_t n;          // n-times contributing to hit
    Int_t flag;       // flag in which detectors the track has been seen
    Float_t weight;   // weight of tr in hit
    Float_t scale;    // scale the weight by this factor
    Int_t ndet;       // number of detectors for this track
    Int_t isGhostTrack()             { return ((flag&Particle::kIsGhost) > 0)? 1 : 0;}
    Int_t isInDetector(eDetBits det) { return ((flag&det) > 0)? 1 : 0;}
    void print(Int_t ind=-1){
	Int_t prec = cout.precision();
	ios_base::fmtflags flg = cout.flags();
	cout<<"i "     <<setw(3)<<ind
            <<" tr "   <<setw(5)<<tr
            <<" n "    <<setw(3)<<n
            <<" ndet " <<setw(3)<<ndet
            <<" w "    <<fixed<<setw(9)<<weight
            <<" s "    <<fixed<<setw(3)<<setprecision(1)<<scale
            <<" fl "   <<setw(5)<<flag
            <<" R "    <<isInDetector(Particle::kIsInRICH)
	    <<" iM "   <<isInDetector(Particle::kIsInInnerMDC)
	    <<" oM "   <<isInDetector(Particle::kIsInOuterMDC)
	    <<" RPC "  <<isInDetector(Particle::kIsInRPC)
	    <<" S "    <<isInDetector(Particle::kIsInSHOWER)
	    <<" E "    <<isInDetector(Particle::kIsInEMC)
	    <<" T "    <<isInDetector(Particle::kIsInTOF)
	    <<" iGhost "<<isInDetector(Particle::kIsInnerGhost)
	    <<" oGhost "<<isInDetector(Particle::kIsOuterGhost)
	    <<" Ghost "<<isGhostTrack()<<endl;
	cout.precision(prec);  // set to old precision
        cout.flags(flg);       // set to old format
    }
    ClassDef(trackinfo,1)
};



class tracksVec : public TObject {
public:
    vector < trackinfo* > tracks;  // vector of trackinfo pointer (tr number, n times, flag)
    Float_t sum;                   // sum of n

    tracksVec(){
	sum = 0;
    }
    tracksVec(const tracksVec& v):TObject(v){ // special copy constructor needed because of aliasing
	for(UInt_t i = 0; i < v.tracks.size(); i++){
	    tracks.push_back(new trackinfo(*(v.tracks[i]) ));
	}
	sum = v.sum;
    }
    ~tracksVec(){
	reset();
    }
    void reset(){
	for(UInt_t i = 0; i < tracks.size(); i++){
	    delete tracks[i];
	}
	tracks.clear();
	sum = 0;
    }
    Int_t findTrack(Int_t tr){
	// return -1 if track does not exist, otherwise index
	for(UInt_t i = 0; i < tracks.size(); i++){
	    if((tracks[i]->tr) == tr) return i;
	}
	return -1;
    }
    void addTrack(Int_t tr,Int_t n, Int_t flag = 0,Float_t scale = 1.){
	// add track if does not exist
	Int_t ind = findTrack(tr);
	if( ind < 0){
	    trackinfo* info = new trackinfo;
	    info->tr     = tr;
	    info->n      = n;
	    info->flag   = flag;
	    info->weight = 0;
	    info->ndet   = 0;
	    info->scale  = scale;
	    sum += n;
	    tracks.push_back(info);
	} else {
	    trackinfo* info = tracks[ind];
	    info->n  += n;
	    sum      += n;
	}
    }
    void calcWeights(void){
	// loop existing tracks and calc n/sum = weight
	if(sum > 0){
	    for(UInt_t i = 0; i < tracks.size(); i ++){
		trackinfo* info = tracks[i];
		info-> weight = (info->n * info->scale) / sum;
	    }
	}
    }
    void addTrackWeight(trackinfo* inf){
	// add track if does not exist
	Int_t ind = findTrack(inf->tr);
	if( ind < 0){
	    trackinfo* info = new trackinfo;
	    info->tr     = inf->tr;
	    info->n      = inf->n;
	    info->flag   = inf->flag;
	    info->weight = inf->weight;
	    info->ndet   = 1;
	    info->scale  = 0;
	    sum += inf->n;
	    tracks.push_back(info);
	} else {
	    trackinfo* info = tracks[ind];
	    info->n      += inf->n;
	    info->flag    = info->flag | inf->flag; // add flag bits
	    info->weight += inf->weight;
	    info->ndet   += 1;
	    sum          += inf->n;
	}
    }
    void addTrackWeight(tracksVec& v){
	// add all tracks if they do not exist
	for(UInt_t i = 0; i < v.tracks.size(); i ++){
	    trackinfo* inf = v.tracks[i];
	    addTrackWeight(inf);
	}
    }
    //-------------------------------------------------------------------
    //  sorting of registered tracks

    //---------------------------------------------------------------
    // tripple sort
    Bool_t static cmpNdetWeightFlag(trackinfo* a, trackinfo* b) { // sort tracks by ndet ,then weight then by flag
	if( a->ndet == b->ndet ) {
	    if( a->weight == b->weight )  return a->flag   > b->flag;
	    else                          return a->weight > b->weight;
	}
	else                              return a->ndet   > b->ndet;
    }
    // double sort
    Bool_t static cmpWeightNdet(trackinfo* a, trackinfo* b) { // sort tracks by weight ,then ndet
	if( a->weight == b->weight ) { return a->ndet > b->ndet; }
	else                           return a->weight > b->weight;
    }
    void sortNdetWeightFlag(void) {
	std::sort(tracks.begin(),tracks.end(),tracksVec::cmpNdetWeightFlag);
    }
    void sortWeightNdet(void) {
	std::sort(tracks.begin(),tracks.end(),tracksVec::cmpWeightNdet);
    }

    void print(){
	cout<<"---------------------------------"<<endl;
	for(UInt_t i = 0; i < tracks.size(); i ++){
	    trackinfo* inf = tracks[i];
	    inf->print(i);
	}

    }
    ClassDef(tracksVec,1)
};


class mdc_trk : public TObject {
public:
    mdc_trk(){;}
    ~mdc_trk(){;}

    Int_t   ind;       // index of HMdcTrkCand
    Float_t dedx;      // Mdc dE/dx for inner + outer segment
    Float_t dedx0;     // Mdc dE/dx for inner segment
    Float_t dedx1;     // Mdc dE/dx for outer segment
    Bool_t  seg1fake;
    Bool_t  seg2fake;
    Bool_t  allfake;
    void reset(void){
	ind  = -1;
	dedx = -1;
	dedx0 = -1;
	dedx1 = -1;
        seg1fake = kFALSE;
        seg2fake = kFALSE;
	allfake = kFALSE;
    }

    void fillMeta(HMetaMatch2* meta){
	if(meta){
	    ind = meta->getTrkCandInd();
	}
    }
    void fill(HMdcTrkCand* trk){
	if(trk){
	    dedx = trk->getdedxCombinedSeg();
	    dedx0 = trk->getdedxInnerSeg();
	    dedx1 = trk->getdedxOuterSeg();
            seg1fake = trk->isSeg1Fake();
            seg2fake = trk->isSeg2Fake();
            allfake  = trk->isFake();
	}
    }

    ClassDef(mdc_trk,1)
};

class mdc_seg : public TObject {
public:
    mdc_seg(){;}
    ~mdc_seg(){;}

    Int_t   ind;       // index of seg
    UInt_t  layers;    // layer bits
    UShort_t wires;    // wires per segment
    Float_t chi2;      // chi2 of segment fitter
    Float_t phi;       // track's phi in cave coordinates (0..360 deg)
    Float_t theta;     // track's theta in cave coordinates (0..90 deg)
    Float_t r;         // distance of closest point to beamline [mm]
    Float_t z;         // z coordinate of closest point to beamline [mm]
    Bool_t  offvertexclust;

    void reset(void){
	ind   = -1;
	layers= 0;
        wires = 0;
	chi2  = -1;
	phi   = -1;
	theta = -1;
	r     = -1000;
	z     = -1000;
	offvertexclust = kFALSE;
    }

    void fill(HMdcSeg* seg){
	if(seg){
	    chi2  = seg->getChi2();
	    phi   = HParticleTool::phiSecToLabDeg(seg->getSec(),seg->getPhi());
	    theta = HParticleTool::thetaToLabDeg (seg->getTheta());
	    r     = seg->getR();
	    z     = seg->getZ();
            wires = seg->getSumWires();
	    layers = 0;
	    for(UInt_t l=0;l<12;l++){
                if(seg->getNCells(l)==0) continue;
		HParticleCand::setLayer(seg->getIOSeg(),l,layers);
	    }

	    if(seg->isOffVertexClus()) offvertexclust = kTRUE;

	}
    }
    ClassDef(mdc_seg,1)
};

class rpc_clst : public TObject {
public:
    rpc_clst(){;}
    ~rpc_clst(){;}

    Int_t   ind;       // Index of HRpcCluster object
    Float_t quality;   // Matching quality
    Float_t dx;        // Deviation in X-coordinate
    Float_t dy;        // Deviation in Y-coordinate
    Float_t tof;       // particle time of flight
    Int_t nhits;       // number of cells hit (1 or 2)
    UInt_t  metaAddress; // mod+cell of first and second hit

    void reset(void){
	ind     = -1;
	quality = -1;
	dx      = -1;
	dy      = -1;
	tof     = -1;
	nhits   = -1;
	metaAddress = 0;
    }

    void setMetaModule(UInt_t hit,UInt_t mod){
	if(hit<2 && mod < 8){
	    mod++;
	    metaAddress&= ~(0xF<<(hit*11+7));
	    metaAddress|=(mod&0xF)<<(hit*11+7);
	}
    }

    void setMetaCell(UInt_t hit,UInt_t cell) {
	if(hit<2 && cell < 33){
	    cell++;
	    metaAddress&= ~(0x7F<<(hit*11));
	    metaAddress|=(cell&0x7F)<<(hit*11);
	}
    }

    void  unsetMetaModule(UInt_t hit) { if(hit<2) metaAddress&= ~(0xF<<(hit*11+7)); }
    void  unsetMetaCell(UInt_t hit)   { if(hit<2) metaAddress&= ~(0x7F<<(hit*11)); }
    Int_t getMetaModule(UInt_t hit)   {  if(hit<2){ return (metaAddress>>(hit*11+7)&(0xF))-1; } else return -1; }
    Int_t getMetaCell  (UInt_t hit)   {  if(hit<2){ return (metaAddress>>(hit*11)&(0x7F))-1;  } else return -1; }

    void fillMeta(HMetaMatch2* meta,Int_t n = 0){
	if(meta){
	    ind     = meta->getRpcClstInd(n);
	    quality = meta->getRpcClstQuality(n);
	    dx      = meta->getRpcClstDX(n);
	    dy      = meta->getRpcClstDY(n);
	}
    }

    void fill(HRpcCluster* clst){
	if(clst){
	    tof   = clst->getTof();
	    nhits = clst->getClusterType();
	    setMetaModule(0,clst->getColumn1());
	    setMetaCell  (0,clst->getCell1()  );
	    setMetaModule(1,clst->getColumn2());
	    setMetaCell  (1,clst->getCell2()  );
	}
    }
    ClassDef(rpc_clst,1)
};


class tof_hit : public TObject {
public:
    tof_hit(){;}
    ~tof_hit(){;}

    Int_t   ind;       // Index of HTofHit object
    Float_t quality;   // Matching quality
    Float_t dx;        // Deviation in X-coordinate
    Float_t dy;        // Deviation in Y-coordinate
    Float_t dedx;      // Tof dE/dx
    Float_t tof;       // particle time of flight
    UInt_t  metaAddress; // mod+cell of first and second hit
    void reset(void){
	ind     = -1;
	quality = -1;
	dx      = -1;
	dy      = -1;
	dedx    = -1;
	tof     = -1;
        metaAddress = 0;
    }

    void setMetaModule(UInt_t hit,UInt_t mod){
	if(hit<2 && mod < 8){
	    mod++;
	    metaAddress&= ~(0xF<<(hit*11+7));
	    metaAddress|=(mod&0xF)<<(hit*11+7);
	}
    }

    void setMetaCell(UInt_t hit,UInt_t cell) {
	if(hit<2 && cell < 33){
	    cell++;
	    metaAddress&= ~(0x7F<<(hit*11));
	    metaAddress|=(cell&0x7F)<<(hit*11);
	}
    }
    void  unsetMetaModule(UInt_t hit) {  if(hit<2) metaAddress&= ~(0xF<<(hit*11+7)); }
    void  unsetMetaCell(UInt_t hit)   {  if(hit<2) metaAddress&= ~(0x7F<<(hit*11)); }
    Int_t getMetaModule(UInt_t hit)   {  if(hit<2){ return (metaAddress>>(hit*11+7)&(0xF))-1; } else return -1; }
    Int_t getMetaCell  (UInt_t hit)   {  if(hit<2){ return (metaAddress>>(hit*11)&(0x7F))-1;  } else return -1; }

    void fillMeta(HMetaMatch2* meta,Int_t n = 0,Int_t hit = 0){
	if(meta){
	    if(hit == kTofClst) {
		if(meta->getTofClstInd(n)>=0){
		    ind     = meta->getTofClstInd(n);
		    quality = meta->getTofClstQuality(n);
		    dx      = meta->getTofClstDX(n);
		    dy      = meta->getTofClstDY(n);
		}
	    }
	    else if(hit == kTofHit1) {
		if(meta->getTofHit1Ind(n)>=0){
		    ind     = meta->getTofHit1Ind(n);
		    quality = meta->getTofHit1Quality(n);
		    dx      = meta->getTofHit1DX(n);
		    dy      = meta->getTofHit1DY(n);
		}
	    }
	    else if(hit == kTofHit2){
		if(meta->getTofHit2Ind(n)>=0){
		    ind     = meta->getTofHit2Ind(n);
		    quality = meta->getTofHit2Quality(n);
		    dx      = meta->getTofHit2DX(n);
		    dy      = meta->getTofHit2DY(n);
		}
	    } else {
		std::cout<<"tof_hit:fillMeta(): Illegal hit number!"<<endl;
	    }
	}
    }

    void fill(HTofHit* hit,Int_t type=1){ // default kTofHit1
	if(hit){
	    dedx = hit->getEdep();
	    tof  = hit->getTof();
	    setMetaModule(0,hit->getModule());
	    setMetaCell  (0,hit->getCell()  );

	    if(type==kTofClst){

		if(((HTofCluster*)hit)->getClusterSize()>1){
		    Int_t index = ((HTofCluster*)hit)->getIndexHit2();
		    if(index!=-1){
			HTofHit* hit2=0;
			hit2 = HCategoryManager::getObject(hit2,catTofHit,index);
			if(hit2){
			    setMetaModule(1,hit2->getModule());
			    setMetaCell  (1,hit2->getCell()  );
			} else { std::cout<<"fill() : NULL pointer recieved for tof cluster hit2"<<endl;}
		    } 
		}
	    }
	}
    }
    ClassDef(tof_hit,1)
};

class shower_hit : public TObject {
public:
    shower_hit(){;}
    ~shower_hit(){;}

    Int_t   ind;         // Index of HShowerHit object
    Float_t quality;     // Matching quality
    Float_t matchradius; // Matching quality
    Float_t dx;          // Deviation in X-coordinate
    Float_t dy;          // Deviation in Y-coordinate
    Float_t showerSum0;  // charge sum of PreShower cluster (0)
    Float_t showerSum1;  // charge sum of PreShower cluster (1)
    Float_t showerSum2;  // charge sum of PreShower cluster (2)

    void reset(void){
	ind        = -1;
	quality    = -1;
	matchradius= -1;
	dx         = -1;
	dy         = -1;
	showerSum0 = -1;
	showerSum1 = -1;
	showerSum2 = -1;
    }

    void fillMeta(HMetaMatch2* meta,Int_t n = 0,HRKTrackB* rk=0){
	if(meta){
	    ind     = meta->getShowerHitInd(n);
	    if(rk==0){
		quality = meta->getShowerHitQuality(n);
		dx      = meta->getShowerHitDX(n);
		dy      = meta->getShowerHitDY(n);
	    }
	    else {
		quality = rk->getQualityShower();
		dx      = rk->getMETAdx();
		dy      = rk->getMETAdy();
                matchradius = rk->getMetaRadius();
	    }
	}
    }

    void fillMetaKal(HMetaMatch2* meta,Int_t n = 0,HKalTrack*  rk=0){
	if(meta){
	    ind     = meta->getShowerHitInd(n);
	    if(rk==0){
		quality = meta->getShowerHitQuality(n);
		dx      = meta->getShowerHitDX(n);
		dy      = meta->getShowerHitDY(n);
	    }
	    else {
		quality = rk->getQualityShower();
		dx      = rk->getMETAdx();
		dy      = rk->getMETAdy();
		matchradius = rk->getMetaRadius();
	    }
	}
    }

    void fill(HShowerHit* hit){
	if(hit){
	    showerSum0 = hit->getSum(0);
	    showerSum1 = hit->getSum(1);
	    showerSum2 = hit->getSum(2);
	}
    }
    ClassDef(shower_hit,1)
};

class emc_clst : public TObject {
public:
    emc_clst(){;}
    ~emc_clst(){;}

    Int_t   ind;         // Index of HEmcClst object
    Int_t   indRpc;      // Index of HEmcClst object matched with RPC
    Float_t quality;     // Matching quality
    Float_t matchradius; // Matching quality
    Float_t dx;          // Deviation in X-coordinate
    Float_t dy;          // Deviation in Y-coordinate
    Float_t energy;      // energy deposited in the cluster
    Float_t time;        // mean time weighted by energy
    Float_t sigmaEnergy; // error of "energy"
    Float_t sigmaTime;   // error of "time"
    Int_t   ncells;      // number of cells in the cluster
    Bool_t  isMatchedRpc;// is the cluster matched to RPC (pos+time)

    void reset(void){
	ind         = -1;
	indRpc      = -1;
	quality     = -1;
	matchradius = -1;
	dx          = -1;
	dy          = -1;
	energy      = -1;
	time        = -1;
	sigmaEnergy = -1;
	sigmaTime   = -1;
	ncells      = -1;
        isMatchedRpc =kFALSE;
    }

    void fillMeta(HMetaMatch2* meta,Int_t n = 0,HRKTrackB* rk=0){
	if(meta){
	    ind     = meta->getEmcClusterInd(n);
	    if(rk==0){
		quality = meta->getEmcClusterQuality(n);
		dx      = meta->getEmcClusterDX(n);
		dy      = meta->getEmcClusterDY(n);
	    }
	    else {
		quality = rk->getQualityEmc();
		dx      = rk->getMETAdx();
		dy      = rk->getMETAdy();
                matchradius = rk->getMetaRadius();
	    }
	}
    }

    void fillMetaKal(HMetaMatch2* meta,Int_t n = 0,HKalTrack*  rk=0){
	if(meta){
	    ind     = meta->getEmcClusterInd(n);
	    if(rk==0){
		quality = meta->getEmcClusterQuality(n);
		dx      = meta->getEmcClusterDX(n);
		dy      = meta->getEmcClusterDY(n);
	    }
	    else {
		quality = rk->getQualityEmc();
		dx      = rk->getMETAdx();
		dy      = rk->getMETAdy();
		matchradius = rk->getMetaRadius();
	    }
	}
    }

    void fill(HEmcCluster* hit){
	if(hit){
	    energy       = hit->getEnergy();
	    time         = hit->getTime();
	    sigmaEnergy  = hit->getSigmaEnergy();
	    sigmaTime    = hit->getSigmaTime();
	    ncells       = hit->getNCells();
	    indRpc       = hit->getRpcIndex();
	    isMatchedRpc = hit->getNMatchedCells() > 0 ? kTRUE: kFALSE;
	}
    }
    ClassDef(emc_clst,1)
};

class rich_hit : public TObject {
public:
    rich_hit(){;}
    ~rich_hit(){;}

    Int_t    ind;           // Index of HRichHit object
    Short_t  numPads;       // number of fired pads typ. (-1 - 100)
    Short_t  amplitude;     // typ. (-1 - 4000)
    Short_t  houTra;        // typ. (-1 - 5000)
    Short_t  patternMatrix; // pattern matrix of ring
    Float_t  centroid;      // ring centroid
    Float_t  phi;           // rich phi   0-360 deg
    Float_t  theta;         // rich theta 0-90 deg

    // NEW RICH700
    Float_t centerx;       // center position in X [mm]
    Float_t centery;       // center position in Y [mm]
    Float_t radius ;       // radius of ring
    Float_t chi2   ;       // chi2 of ring fit
    Int_t   ncals  ;       // number of cal objects used

    Bool_t isNewRich() { return ncals == -1 ? kFALSE:  kTRUE;}

    void reset(void){
	ind           = -1;
	numPads       = -1;
	amplitude     = -1;
	houTra        = -1;
	patternMatrix = -1;
	centroid      = -1;
	phi           = -1;
	theta         = -1;
	centerx       = -1;
	centery       = -1;
	radius        = -1;
	chi2          = -1;
	ncals         = -1;
    }

    void fillMeta(HMetaMatch2* meta,Int_t n = 0){
	if(meta && n > -1){
	    ind = meta->getARichInd(n); // 0 = best match
	}
    }
    void fill(HRichHit* hit){
	if(hit){
	    if(hit->isNewRich()){
		centerx       = hit->getRich700CircleCenterX();
		centery       = hit->getRich700CircleCenterY();
		radius        = hit->getRich700CircleRadius();
		chi2          = hit->getRich700CircleChi2();
		ncals         = hit->getRich700NofRichCals();
	    } else {
		numPads       = hit->getRingPadNr();
		amplitude     = hit->getRingAmplitude();
		houTra        = hit->getRingHouTra();
		patternMatrix = hit->getRingPatMat();
		centroid      = hit->getCentroid();
	    }
	    phi           = hit->getPhi();
	    theta         = hit->getTheta();
	}
    }
    ClassDef(rich_hit,1)
};

class spline_track : public TObject {
public:
    spline_track(){;}
    ~spline_track(){;}

    Int_t   ind;        // Index of HSplineTrack object
    Float_t p;          // particle's momentum [MeV]

    void reset(void){
	ind = -1;
	p   = -1;
    }
    void fillMeta(HMetaMatch2* meta){
	if(meta){
	    ind = meta->getSplineInd();
	}
    }
    void fill(HSplineTrack* trk){
	if(trk){
	    p = trk->getP();
	}
    }
    ClassDef(spline_track,1)
};



class rk_track : public TObject {
public:
    rk_track(){;}
    ~rk_track(){;}

    //-------------------------------
    // to be set by hand
    Int_t usedMeta;     // see enum selMeta
    Int_t selectTof;    // see enum selMeta
    //-------------------------------
    Int_t   ind;        // Index of HRKBaseTrack object
    Float_t beta;       // particle's beta
    Float_t p;          // particle's momentum [MeV]
    Float_t mass;       // particle's mass [MeV]
    Float_t path;       // length of particle trajetory [mm]
    Float_t chi2;       // chi2 of fit
    Float_t phi;        // track's phi in cave coordinates (0..360 deg)
    Float_t theta;      // track's theta in cave coordinates (0..90 deg)
    Float_t r;          // distance of closest point to beamline [mm]
    Float_t z;          // z coordinate of closest point to beamline [mm]
    Int_t   pol;        // polarity
    Float_t quality;    // quality rk -Meta matching
    Float_t matchradius;// rk -Meta matching sqrt(dx2+dy2) [mm]
    Float_t metadx;     // rk -Meta matching in x [mm]
    Float_t metady;     // rk -Meta matching in y [mm]

    void reset(void){
	usedMeta  = -1;
	selectTof = -1;
	ind       = -1;
	beta      = -1;
	p         = -1;
	mass      = -1;
	path      = -1;
        chi2      = -1;
	phi       = -1;
	theta     = -1;
	r         = -1;
	z         = -1;
	pol       =  0;
	quality   = -1;
	matchradius = -1.;
	metadx = -1000;
        metady = -1000;

    }
    void fillInd(Int_t n = 0){
	ind = n;
    }
    void fill(HRKTrackB* rk){
	if(rk){
	    beta  = rk->getBeta();
	    p     = rk->getP();
	    mass  = rk->getMass2();
	    path  = rk->getTofDist();
	    chi2  = rk->getChiq();
	    phi   = HParticleTool::phiSecToLabDeg(rk->getSector(),rk->getPhiSeg1RK());
	    theta = HParticleTool::thetaToLabDeg (rk->getThetaSeg1RK());
	    r     = rk->getRSeg1RK();
	    z     = rk->getZSeg1RK();
	    pol   = rk->getPolarity();
             metadx = -1000; metady = -1000; 
	    if     (usedMeta >= 0 && usedMeta < 3) { quality = rk->getQualityTof();    matchradius = rk->getMetaRadius();  if(rk->getMETAdx()!=-10000)metadx=rk->getMETAdx();  if(rk->getMETAdy()!=-10000)metady=rk->getMETAdy();}
	    else if(usedMeta == kRpcClst )         { quality = rk->getQualityRpc();    matchradius = rk->getMetaRadius();  if(rk->getMETAdx()!=-10000)metadx=rk->getMETAdx();  if(rk->getMETAdy()!=-10000)metady=rk->getMETAdy();}
	    else if(usedMeta == kShowerHit )       { quality = rk->getQualityShower(); matchradius = rk->getMetaRadius();  if(rk->getMETAdx()!=-10000)metadx=rk->getMETAdx();  if(rk->getMETAdy()!=-10000)metady=rk->getMETAdy();}
	    else if(usedMeta == kEmcClst )         { quality = rk->getQualityEmc();    matchradius = rk->getMetaRadius();  if(rk->getMETAdx()!=-10000)metadx=rk->getMETAdx();  if(rk->getMETAdy()!=-10000)metady=rk->getMETAdy();}
	    else                                   { quality = -1; matchradius = -1; metadx = -1000; metady = -1000; }
	}
    }
    ClassDef(rk_track,1)
};

class kal_track : public TObject {
public:
    kal_track(){;}
    ~kal_track(){;}

    //-------------------------------
    // to be set by hand
    Int_t usedMeta;     // see enum selMeta
    Int_t selectTof;    // see enum selMeta
    //-------------------------------
    Int_t   ind;        // Index of HRKBaseTrack object
    Float_t beta;       // particle's beta
    Float_t p;          // particle's momentum [MeV]
    Float_t mass;       // particle's mass [MeV]
    Float_t path;       // length of particle trajetory [mm]
    Float_t chi2;       // chi2 of fit
    Float_t phi;        // track's phi in cave coordinates (0..360 deg)
    Float_t theta;      // track's theta in cave coordinates (0..90 deg)
    Float_t r;          // distance of closest point to beamline [mm]
    Float_t z;          // z coordinate of closest point to beamline [mm]
    Int_t   pol;        // polarity
    Float_t quality;    // quality rk -Meta matching
    Float_t matchradius;// rk -Meta matching sqrt(dx2+dy2) [mm]
    Float_t metadx;     // rk -Meta matching in x [mm]
    Float_t metady;     // rk -Meta matching in y [mm]

    void reset(void){
	usedMeta  = -1;
	selectTof = -1;
	ind       = -1;
	beta      = -1;
	p         = -1;
	mass      = -1;
	path      = -1;
        chi2      = -1;
	phi       = -1;
	theta     = -1;
	r         = -1;
	z         = -1;
	pol       =  0;
	quality   = -1;
	matchradius = -1;
	metadx = -1000;
        metady = -1000;
    }
    void fillInd(Int_t n = 0){
	ind = n;
    }
    void fill(HKalTrack* trk){
	if(trk){
	    beta  = trk->getBeta();
	    p     = trk->getP();
	    mass  = trk->getMass2();
	    path  = trk->getTofDist();
	    chi2  = trk->getChi2();
	    phi   = HParticleTool::phiSecToLabDeg(trk->getSector(),trk->getPhi());
	    theta = HParticleTool::thetaToLabDeg (trk->getTheta());
	    r     = trk->getR();
	    z     = trk->getZ();
	    pol   = trk->getPolarity();
            metadx = -1000; metady = -1000;
	    if     (usedMeta >= 0 && usedMeta < 3) { quality = trk->getQualityTof();    matchradius = trk->getMetaRadius(); if(trk->getMETAdx()!=-10000.)metadx=trk->getMETAdx();  if(trk->getMETAdy()!=-10000.)metady=trk->getMETAdy(); }
	    else if(usedMeta == kRpcClst )         { quality = trk->getQualityRpc();    matchradius = trk->getMetaRadius(); if(trk->getMETAdx()!=-10000.)metadx=trk->getMETAdx();  if(trk->getMETAdy()!=-10000.)metady=trk->getMETAdy(); }
	    else if(usedMeta == kShowerHit )       { quality = trk->getQualityShower(); matchradius = trk->getMetaRadius(); if(trk->getMETAdx()!=-10000.)metadx=trk->getMETAdx();  if(trk->getMETAdy()!=-10000.)metady=trk->getMETAdy(); }
	    else if(usedMeta == kEmcClst )         { quality = trk->getQualityEmc();    matchradius = trk->getMetaRadius(); if(trk->getMETAdx()!=-10000.)metadx=trk->getMETAdx();  if(trk->getMETAdy()!=-10000.)metady=trk->getMETAdy(); }
	    else                                   { quality = -1;  matchradius = -1; metadx = -1000; metady = -1000;}
	}
    }
    ClassDef(kal_track,1)
};


class candidate : public TObject {
public:
    candidate(){ reset(); }
    ~candidate(){ ;}

    pointers objects;          //!  keep pointers for this candidate
    //---------------------------
    // to be set by hand
    Bool_t  isEMC;              // switch SHOWER/EMC
    Int_t   used;               // should this candidate be used ?
    Int_t   system;             // detector system
    Bool_t  rkSuccess;          // fitted ?
    Int_t   usedMeta;           // noUse = -1, 0 =TOFclst, 1 = TOHHIT1, 2 = TOFHIT2, 3 =RPC 4=SHR
    Int_t   selectTof;          // noUse = -1, 0 =TOFclst, 1 = TOHHIT1, 2 = TOFHIT2
    Int_t   nCand;              // number of candidate in metamatch objects
    Int_t   hasRingCorrelation; // pass MDC-RICH correlation cut ?
    Float_t corrThetaRich;      // z pos theta correction for Rich (to be added to theta)
    Float_t alignThetaRich;     // align theta correction for Rich (to be added to theta)
    Float_t alignPhiRich;       // align phi   correction for Rich (to be added to theta)
    Float_t richMdcQuality;     // match RICH inner Segment quality sqrt( dThetaSeg1 * dThetaSeg1 + dPhiSeg1 * dPhiSeg1)
    Float_t richRkQuality;      // match RICH RK quality
    //---------------------------

    //---------------------------
    // simulation
    tracksVec commonTracks; //  collected GEANT track number for all detector hits
    tracksVec richTracks;   //  collected GEANT track number for RICH detector hit
    tracksVec mdc1Tracks;   //  collected GEANT track number for inner MDC segment
    tracksVec mdc2Tracks;   //  collected GEANT track number for outer MDC segment
    tracksVec tofTracks;    //  collected GEANT track number for TOF detector hit
    tracksVec rpcTracks;    //  collected GEANT track number for RPC detector hit
    tracksVec showerTracks; //  collected GEANT track number for SHOWER detector hit
    tracksVec emcTracks;    //  collected GEANT track number for EMC detector cluster
    //---------------------------

    //---------------------------
    // close tracks
    closeVec closeTracks;    // keep index and opening angle to close tracks
    Float_t  oAFitted   ;    // neg angle = hadron, pos angle lepton , -99 nothing
    Float_t  oANoFitted ;    // neg angle = hadron, pos angle lepton , -99 nothing
    //---------------------------


    //---------------------------
    Int_t        ind;        // metamatch index
    Int_t        sector;     // sector number

    mdc_trk      mdctrk;
    mdc_seg      seg1;
    mdc_seg      seg2;
    tof_hit      tof[3];
    rpc_clst     rpcclst;
    shower_hit   showerhit;
    emc_clst     emcclst;
    rich_hit     richhit;
    spline_track spline;
    rk_track     rk;
    kal_track    kal;

    void reset(void){

        objects     .reset();
	commonTracks.reset();
	richTracks  .reset();
	mdc1Tracks  .reset();
	mdc2Tracks  .reset();
	tofTracks   .reset();
	rpcTracks   .reset();
	showerTracks.reset();
	emcTracks   .reset();

	closeTracks .reset();
	oAFitted           = -99;
        oANoFitted         = -99;
	isEMC              =   kFALSE;
	used               =   1;
	system             =  -1;
	rkSuccess          =   kFALSE;
	usedMeta           =  -1;
	selectTof          =  -1;
	nCand              =  -1;
	hasRingCorrelation =   0;
	corrThetaRich      =   0;
	alignThetaRich     =   0;
	alignPhiRich       =   0;
	richMdcQuality     =  -1;
	richRkQuality      =  -1;

	ind        = -1;
	sector     = -1;
	mdctrk    .reset();
	seg1      .reset();
	seg2      .reset();
	for(Int_t i=0;i<3;i++) tof[i].reset();
	rpcclst   .reset();
	showerhit .reset();
	emcclst   .reset();
	richhit   .reset();
	spline    .reset();
	rk        .reset();
	kal       .reset();
    }
    void fillMeta(HMetaMatch2* meta){
	if(meta){
	    ind    = meta->getOwnIndex();
	    sector = meta->getSector();
	}
    }
    void fillParticleCand(HParticleCand* part,Int_t index,Int_t momSwitch=Particle::kMomRK){
	if(part){

	    Int_t selMeta = -1;
            Int_t selTof  = -1;

	    part->setIndex(index);
	    part->setSector(sector);
	    part->setSystem(system) ;
	    part->setRingCorr(hasRingCorrelation); //
	    part->setTofRec(0);                    // later filled by starttime reconstructor
	    part->setMdcdEdx(mdctrk.dedx);

	    //########################################################
            // MOM SELECTION
	    //------------------------------------------
	    if(momSwitch == Particle::kMomRK)
	    {
		if(rk.chi2 >= 0){
		    rkSuccess= kTRUE;
		    selMeta  = rk.usedMeta;
		    selTof   = rk.selectTof;
		} else {
		    selMeta = usedMeta;
		    selTof  = selectTof;
		}

		part->setCharge  (rk.pol);
		part->setBeta    (rk.beta);
		part->setBetaOrg (rk.beta);
		part->setMomentum(rk.p);
		part->setMomentumOrg(rk.p);
		part->setMass2   (rk.mass);

		//------------------------------------------
		if(rk.chi2 >= 0){
		    part->setPhi  (rk.phi);
		    part->setTheta(rk.theta);
		    part->setR    (rk.r);
		    part->setZ    (rk.z);
		} else {  // if no RK take segment info
		    part->setPhi  (seg1.phi);
		    part->setTheta(seg1.theta);
		    part->setR    (seg1.r);
		    part->setZ    (seg1.z);
		}
		//------------------------------------------

		part->setChi2             (rk.chi2);
		part->setDistanceToMetaHit(rk.path);
		part->setDistanceToMetaHitOrg(rk.path);

		//------------------------------------------
		if(rk.chi2 >= 0){
		    part->setMetaMatchQuality (rk.quality);
	            part->setMetaMatchRadius  (rk.matchradius);
	        } else { // if no RK take MetaMatch info instead
		    if     (selMeta >= 0 && selMeta < 3)  part-> setMetaMatchQuality(tof[selMeta].quality);
		    else if(selMeta == kRpcClst )         part-> setMetaMatchQuality(rpcclst.quality);
		    else if(selMeta == kShowerHit)        part-> setMetaMatchQuality(showerhit.quality);
		    else if(selMeta == kEmcClst)          part-> setMetaMatchQuality(emcclst.quality);
		    else                                  part-> setMetaMatchQuality(-1);
		}
		//------------------------------------------

	    } else if (momSwitch == Particle::kMomKalman){

		if(kal.chi2 >= 0){
		    rkSuccess= kTRUE;
		    selMeta = kal.usedMeta;
		    selTof  = kal.selectTof;
		} else {
		    selMeta = usedMeta;
		    selTof  = selectTof;
		}

		part->setCharge  (kal.pol);
		part->setBeta    (kal.beta);
		part->setBetaOrg (kal.beta);
		part->setMomentum(kal.p);
		part->setMomentumOrg(kal.p);
		part->setMass2   (kal.mass);

		//------------------------------------------
		if(kal.chi2 >= 0){
		    part->setPhi  (kal.phi);
		    part->setTheta(kal.theta);
		    part->setR    (kal.r);
		    part->setZ    (kal.z);
		} else {  // if no RK take segment info
		    part->setPhi  (seg1.phi);
		    part->setTheta(seg1.theta);
		    part->setR    (seg1.r);
		    part->setZ    (seg1.z);
		}
		//------------------------------------------

		part->setChi2             (kal.chi2);
		part->setDistanceToMetaHit(kal.path);
		part->setDistanceToMetaHitOrg(kal.path);

		//------------------------------------------
		if(kal.chi2 >= 0){
		    part->setMetaMatchQuality (kal.quality);
		    part->setMetaMatchRadius  (kal.matchradius);
	        } else { // if no Kalman take MetaMatch info instead
		    if     (selMeta >= 0 && selMeta < 3)  { part-> setMetaMatchQuality(tof[selMeta].quality); }
		    else if(selMeta == kRpcClst )         { part-> setMetaMatchQuality(rpcclst.quality);      }
		    else if(selMeta == kShowerHit)        { part-> setMetaMatchQuality(showerhit.quality);    }
		    else if(selMeta == kEmcClst)          { part-> setMetaMatchQuality(emcclst.quality);      }
		    else                                  { part-> setMetaMatchQuality(-1);                   }
		}
		//------------------------------------------

	    } else {

               Error("fillParticleCand","momentum reco option %i not know!",momSwitch);

	    }
	    //------------------------------------------

            //------------------------------------------
	    // filling dx,dy Meta : Val is RK or Kalman if fit succeed
             if (momSwitch == Particle::kMomKalman){
		 if(kal.chi2 >= 0)  {
		     part->setRkMetaDx(kal.metadx);
		     part->setRkMetaDy(kal.metady);
		 }
	     } else if(momSwitch == Particle::kMomRK){
		 if(rk.chi2 >= 0) {
		     part->setRkMetaDx(rk.metadx);
		     part->setRkMetaDy(rk.metady);
		 }
	     } else {
               Error("fillParticleCand","momentum reco option %i not know!",momSwitch);
	    }
            // filling dx,dy Meta : Val is taken from seg if fit failed
	    if( (momSwitch == Particle::kMomKalman && kal.chi2 <0) || (momSwitch == Particle::kMomRK && rk.chi2 <0)){
		if     (selMeta >= 0 && selMeta < 3)  { part-> setRkMetaDx(tof[selMeta].dx); part-> setRkMetaDy(tof[selMeta].dy);  }
		else if(selMeta == kRpcClst )         { part-> setRkMetaDx(rpcclst.dx)     ; part-> setRkMetaDy(rpcclst.dy);       }
		else if(selMeta == kShowerHit)        { part-> setRkMetaDx(showerhit.dx);  ; part-> setRkMetaDy(showerhit.dy);     }
		else if(selMeta == kEmcClst)          { part-> setRkMetaDx(emcclst.dx);    ; part-> setRkMetaDy(emcclst.dy);       }
		else                                  { part-> setRkMetaDx(-1000)          ; part-> setRkMetaDy(-1000);            }
	    }
            //------------------------------------------




	    //########################################################
 

	    //------------------------------------------
	    if(selTof >= 0 && selTof < 3) part-> setTofdEdx(tof[selTof].dedx);
	    //------------------------------------------



	    part->setInnerSegmentChi2 (seg1.chi2);
	    part->setOuterSegmentChi2 (seg2.chi2);
            part->setLayers(0,seg1.layers);
	    part->setLayers(1,seg2.layers);
            part->setNWSeg(0,seg1.wires);
	    part->setNWSeg(1,seg2.wires);

            if(seg1.offvertexclust) part->setOffVertexClust();

	    if(mdctrk.seg1fake) part->setFakeRejected(0);
            if(mdctrk.seg2fake) part->setFakeRejected(1);
            if(mdctrk.allfake ) part->setFakeRejected(-1);


	    part->setAngleToNearbyFittedInner  (oAFitted)  ;
	    part->setAngleToNearbyUnfittedInner(oANoFitted);

	    if(richhit.isNewRich()){
		part->setRingRadius  (richhit.radius);
		part->setRingChi2    (richhit.chi2);
		part->setRingNumCals (richhit.ncals);
	    } else {
		part->setRingNumPads      (richhit.numPads);
		part->setRingAmplitude    (richhit.amplitude);
		part->setRingHouTra       (richhit.houTra);
		part->setRingPatternMatrix(richhit.patternMatrix);
		part->setRingCentroid     (richhit.centroid);
	    }
	    part->setRichPhi          (richhit.phi) ;
	    part->setRichTheta        (richhit.theta);

	    part->setShowerSum0       (showerhit.showerSum0);
	    part->setShowerSum1       (showerhit.showerSum1);
	    part->setShowerSum2       (showerhit.showerSum2);

	    if(isEMC){
		part->setMetaMatchQualityEmc(emcclst.quality);
		part->setMetaMatchRadiusEmc (emcclst.matchradius);

		part->setEmcEnergy(emcclst.energy);
		part->setEmcTime(emcclst.time);
		part->setIsEmc();
		if(emcclst.isMatchedRpc) part->setIsEmcMatchedToRpc();
		if(emcclst.indRpc==emcclst.ind && emcclst.ind>=0) part->setIsEmcMatchedToSameRpc();
		part->setEmcNCells(emcclst.ncells);
       

	    } else {
		part->setMetaMatchQualityShower(showerhit.quality);
		part->setMetaMatchRadiusShower (showerhit.matchradius);
	    }
	    //------------------------------------------
	    // set only indices of used META hits
	    part->setMetaInd    (ind);
	    part->setRichInd    (richhit.ind);
            part->setInnerSegInd(seg1.ind);
            part->setOuterSegInd(seg2.ind);
            if(isEMC){
		part->setEmcInd (emcclst.ind);
	    } else {
		part->setShowerInd (showerhit.ind);
	    }

	    if     (selMeta == kRpcClst ){
		part->setRpcInd    (rpcclst.ind);

		part->setMetaModule(0,rpcclst.getMetaModule(0));
                part->setMetaCell  (0,rpcclst.getMetaCell  (0));
                part->setMetaModule(1,rpcclst.getMetaModule(1));
                part->setMetaCell  (1,rpcclst.getMetaCell  (1));
	    }

	    if     (selMeta == kTofClst )  {
		part->setTofClstInd(tof[kTofClst].ind);

	        part->setMetaModule(0,tof[selMeta].getMetaModule(0));
                part->setMetaCell  (0,tof[selMeta].getMetaCell  (0));
                part->setMetaModule(1,tof[selMeta].getMetaModule(1));
                part->setMetaCell  (1,tof[selMeta].getMetaCell  (1));
	    } else if(selMeta == kTofHit1 || selMeta == kTofHit2  ) {
		part->setTofHitInd (tof[selMeta].ind);

		part->setMetaModule(0,tof[selMeta].getMetaModule(0));
                part->setMetaCell  (0,tof[selMeta].getMetaCell  (0));
	    }


	    if(system == 2 && selMeta == kRpcClst){
                // set in addition tofhit/tofclstind
		part->setTofClstInd(tof[kTofClst].ind);

		part->setMetaModule(0,tof[kTofClst].getMetaModule(0));
                part->setMetaCell  (0,tof[kTofClst].getMetaCell  (0));
                part->setMetaModule(1,tof[kTofClst].getMetaModule(1));
                part->setMetaCell  (1,tof[kTofClst].getMetaCell  (1));

                // for the tofhits we have to check if the exist
		if     (tof[kTofHit1].ind != -1 && tof[kTofHit2].ind == -1) part->setTofHitInd ( tof[kTofHit1].ind );
		else if(tof[kTofHit1].ind == -1 && tof[kTofHit2].ind != -1) part->setTofHitInd ( tof[kTofHit2].ind );
		else if(tof[kTofHit1].ind != -1 && tof[kTofHit2].ind != -1) {
                    // if both exist take the better matched
		    if(tof[kTofHit1].quality < tof[kTofHit2].quality )  part->setTofHitInd ( tof[kTofHit1].ind );
                    else                                                part->setTofHitInd ( tof[kTofHit2].ind );
		}
	    }

	    //------------------------------------------

            part->setSelectedMeta( selMeta );


	}
    }

    void fillParticleCandSim(HParticleCandSim* part,HCategory* kineCat = 0){
	if(part && kineCat){

	    if(commonTracks.tracks.size() > 0){
		HGeantKine* kine = 0;
		Int_t tr = commonTracks.tracks[0]->tr;
		if(tr > 0) { // GEANT track
		    kine     = HCategoryManager::getObject(kine,kineCat,tr-1);
		    if(kine){
			Float_t px,py,pz;
			Float_t vx,vy,vz;
			Float_t geninfo,geninfo1,geninfo2,genweight;
			Int_t   parentTr,medium,mechanism,pid,parentPid=-1,grandparentTr=0,grandparentPid=-1;
			pid = kine->getID();
			kine->getMomentum (px,py,pz);
			kine->getVertex   (vx,vy,vz);
			kine->getCreator  (parentTr,medium,mechanism);
			kine->getGenerator(geninfo,genweight);
			kine->getGenerator(geninfo,geninfo1,geninfo2);
			if(parentTr > 0){
			    kine          = HCategoryManager::getObject(kine,kineCat,parentTr-1);
			    parentPid     = kine->getID();
			    grandparentTr = kine->getParentTrack();
			    if(grandparentTr > 0){
				kine           = HCategoryManager::getObject(kine,kineCat,grandparentTr-1);
				grandparentPid = kine->getID();
			    }
			}

			part->setGeantPID(pid);
			part->setGeantTrack(tr);
			part->setGeantCorrTrackIds(commonTracks.tracks[0]->flag);
			part->setGeantxMom(px);
			part->setGeantyMom(py);
			part->setGeantzMom(pz);
			part->setGeantxVertex(vx);
			part->setGeantyVertex(vy);
			part->setGeantzVertex(vz);
			part->setGeantParentTrackNum(parentTr);
			part->setGeantParentPID(parentPid);
			part->setGeantGrandParentTrackNum(grandparentTr);
			part->setGeantGrandParentPID(grandparentPid);
			part->setGeantCreationMechanism(mechanism);
			part->setGeantMediumNumber(medium);
			part->setGeantGeninfo  (geninfo);
			part->setGeantGeninfo1 (geninfo1);
			part->setGeantGeninfo2 (geninfo2);
			part->setGeantGenweight(genweight);

		    } else {
                       Error("fillParticleCandSim()","NULL pointer received for kine object of trackNum = %i",tr);
		    }

		} else if (tr == gHades->getEmbeddingRealTrackId()){ // REAL data track
                    part->setGeantTrack(tr);
		}

		Int_t ntr = 0;
		Int_t selMeta = part->getSelectedMeta();

		if        (selMeta == kTofClst || selMeta == kTofHit1 || selMeta == kTofHit2 ) {
		    ntr = tofTracks.tracks.size();
		    if(ntr > 4) ntr = 4;
		    for(Int_t i = 0; i < ntr; i++){
			part->setGeantTrackMeta(tofTracks.tracks[i]->tr,i);
		    }

		} else if (selMeta == kRpcClst){

		    ntr = rpcTracks.tracks.size();
		    if(ntr > 4) ntr = 4;
		    for(Int_t i = 0; i < ntr; i++){
			part->setGeantTrackMeta(rpcTracks.tracks[i]->tr,i);
		    }


		} else if (selMeta == kShowerHit){

		    ntr = showerTracks.tracks.size();
		    if(ntr > 4) ntr = 4;
		    for(Int_t i = 0; i < ntr; i++){
			part->setGeantTrackMeta(showerTracks.tracks[i]->tr,i);
		    }
		} else if (selMeta == kEmcClst){

		    ntr = emcTracks.tracks.size();
		    if(ntr > 4) ntr = 4;
		    for(Int_t i = 0; i < ntr; i++){
			part->setGeantTrackMeta(emcTracks.tracks[i]->tr,i);
		    }
		}
		if(isEMC)
		{
		    ntr = emcTracks.tracks.size();
		    if(ntr > 4) ntr = 4;
		    for(Int_t i = 0; i < ntr; i++){
			part->setGeantTrackEmc(emcTracks.tracks[i]->tr,i);
		    }
		} else {
		    ntr = showerTracks.tracks.size();
		    if(ntr > 4) ntr = 4;
		    for(Int_t i = 0; i < ntr; i++){
			part->setGeantTrackShower(showerTracks.tracks[i]->tr,i);
		    }
		}

		ntr = richTracks.tracks.size();
		if(ntr > 3) ntr = 3;
		for(Int_t i = 0; i < ntr; i++){
		    part->setGeantTrackRich(richTracks.tracks[i]->tr,i);
		}

		ntr = mdc1Tracks.tracks.size();
                if(ntr > 2) ntr = 2;
                for(Int_t i = 0; i < ntr; i++){
		    part->setGeantTrackInnerMdc(mdc1Tracks.tracks[i]->tr,i,mdc1Tracks.tracks[i]->n);
		}

		ntr = mdc2Tracks.tracks.size();
                if(ntr > 2) ntr = 2;
                for(Int_t i = 0; i < ntr; i++){
		    part->setGeantTrackOuterMdc(mdc2Tracks.tracks[i]->tr,i,mdc2Tracks.tracks[i]->n);
		}

	    }
	}
    }
    Float_t deltaThetaSeg1(){ return   richhit.theta - seg1.theta; } // delta theta Rich-MDC
    Float_t deltaPhiSeg1()  { return ( richhit.phi - seg1.phi ) * TMath::Sin(TMath::DegToRad() * seg1.theta);} // delta phi Rich-MDC
    Float_t deltaThetaRk()  { return   richhit.theta - rk.theta; }   // delta theta Rich-MDC
    Float_t deltaPhiRk()    { return ( richhit.phi - rk.phi ) * TMath::Sin(TMath::DegToRad() * rk.theta);}     // delta phi Rich-MDC
    Float_t deltaThetaKal() { return   richhit.theta - kal.theta; }   // delta theta Rich-MDC
    Float_t deltaPhiKal()   { return ( richhit.phi - kal.phi ) * TMath::Sin(TMath::DegToRad() * kal.theta);}   // delta phi Rich-MDC


    ClassDef(candidate,1);
};


//---------------------------------------------------------------------------------


#endif // __HPARTICLESTRUCTS_H__
