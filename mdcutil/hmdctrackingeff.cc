// : Author  J.Markert

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcTrackingEff
//
// This class fills two ntuples for single lepton tracks and pairs to estimate
// the tracking efficiency in simulation.
// This tasks needs as input GeantKine, GeantTof, GeantShower ,HMdcTrkCand, HMdcSegSim,
// HMdcTrkCandIdeal and HMdcSegIdeal categories. The MdcTaskSet and HMdcIdealTracking
// with setup of paralle output categories has to run in front to fill the categories.
//
// In the following the variables of the ntuples are described.
//----------------------------------
// SINGLE
// single[0] =evNr
//
// GEANT KINE information
// single[1] =sector    (from inner MdcSegIdeal)
// single[2] =ptrk      (parent Track number)
// single[3] =genInfo   GEANT generator
// single[4] =genInfo1  GEANT generator
// single[5] =genInfo2  GEANT generator
// single[6] =gID       GEANT particle ID
// single[7] =mom       GEANT momentum
// single[8] =vx        GEANT vertex
// single[9] =vy        GEANT vertex
// single[10]=vz        GEANT vertex

// inner SEG from ideal tracking
// single[11]=rId_1
// single[12]=z_1
// single[13]=thetaId_1 degree!
// single[14]=PhiId_1   degree!
// single[15]=nWId1_1   number of wires in first  module of inner seg
// single[16]=nWId2_1   number of wires in second module of inner seg

// outer SEG from ideal tracking
// single[17]=rId_2
// single[18]=zId_2
// single[19]=thetaId_2 degree!
// single[20]=PhiId_2   degree!
// single[21]=nWId1_2   number of wires in first  module of outer seg
// single[22]=nWId2_2   number of wires in first  module of outer seg

// inner SEG  from reconstruction
// single[23]=r_1
// single[24]=z_1
// single[25]=theta_1
// single[26]=Phi_1
// single[27]=nW1_1
// single[28]=nW2_1

// outer SEG  from reconstruction
// single[29]=r_2
// single[30]=z_2
// single[31]=theta_2
// single[32]=Phi_2
// single[33]=nW1_2
// single[34]=nW2_2
// single[35]=chi2_1;    // chi2 from inner segment
// single[36]=chi2_2;    // chi2 from outer segment
// single[37]=foundPair; // 0 : not part of reconstructed pair , 1: part of pair
// single[38]=nTracks;   // number of ideal tracks per event
// single[39]=meta;      // 0: hit in GeantShower,
//                          1: hit in GeantTof,
//                         -1: no Geant meta hit,
//                         -2: hit in GeantShower in other sector than MdcSeg
//                         -3: hit in GeantTof in other sector than MdcSeg
//
//------------------------------------------------
//
// PAIRS
// The variables are analog to the above besides
// the fact that two particles have to indexed
//
// pairs[0] =evNr
//
// GEANT KINE particel 1
// pairs[1] =sector_1
// pairs[2] =ptrk_1
// pairs[3] =genInfo_1
// pairs[4] =genInfo1_1
// pairs[5] =genInfo2_1
// pairs[6] =gID_1
// pairs[7] =mom_1
// pairs[8] =vx_1
// pairs[9] =vy_1
// pairs[10]=vz_1
//
// inner SEG from IDEAL tracking for particle 1
// pairs[11]=rId_1_1
// pairs[12]=z_1_1
// pairs[13]=thetaId_1_1
// pairs[14]=PhiId_1_1
// pairs[15]=nWId1_1_1
// pairs[16]=nWId2_1_1
//
// outer SEG from IDEAL tracking for particle 1
// pairs[17]=rId_2_1
// pairs[18]=zId_2_1
// pairs[19]=thetaId_2_1
// pairs[20]=PhiId_2_1
// pairs[21]=nWId1_2_1
// pairs[22]=nWId2_2_1
//
// inner SEG  from reconstruction for particle 1
// pairs[23]=r_1_1
// pairs[24]=z_1_1
// pairs[25]=theta_1_1
// pairs[26]=Phi_1_1
// pairs[27]=nW1_1_1
// pairs[28]=nW2_1_1
//
// outer SEG  from reconstruction for particle 1
// pairs[29]=r_2_1
// pairs[30]=z_2_1
// pairs[31]=theta_2_1
// pairs[32]=Phi_2_1
// pairs[33]=nW1_2_1
// pairs[34]=nW2_2_1
// pairs[35]=chi2_1_1;
// pairs[36]=chi2_2_1;
//
// GEANT KINE particle 2
// pairs[37]=sector_2
// pairs[38]=ptrk_2
// pairs[39]=genInfo_2
// pairs[49]=genInfo1_2
// pairs[41]=genInfo2_2
// pairs[42]=gID_2
// pairs[43]=mom_2
// pairs[44]=vx_2
// pairs[45]=vy_2
// pairs[46]=vz_2
//
// inner SEG from IDEAL tracking for particle 2
// pairs[47]=rId_1_2
// pairs[48]=z_1_2
// pairs[49]=thetaId_1_2
// pairs[50]=PhiId_1_2
// pairs[51]=nWId1_1_2
// pairs[52]=nWId2_1_2
//
// outer SEG from IDEAL tracking for particle 2
// pairs[53]=rId_2_2
// pairs[54]=zId_2_2
// pairs[55]=thetaId_2_2
// pairs[56]=PhiId_2_2
// pairs[57]=nWId1_2_2
// pairs[58]=nWId2_2_2

// inner SEG from reconstruction for particle 2
// pairs[59]=r_1_2
// pairs[60]=z_1_2
// pairs[61]=theta_1_2
// pairs[62]=Phi_1_2
// pairs[63]=nW1_1_2
// pairs[64]=nW2_1_2
//
// outer SEG from reconstruction for particle 2
// pairs[65]=r_2_2
// pairs[66]=z_2_2
// pairs[67]=theta_2_2
// pairs[68]=Phi_2_2
// pairs[69]=nW1_2_2
// pairs[70]=nW2_2_2
//
// pairs[71]=chi2_1_2;
// pairs[72]=chi2_2_2;
//
// pairs[73]=nTracks;      number of tracks per event
// pairs[74]=meta_1;       meta hit for particle 1 (see description above)
// pairs[75]=meta_2;       meta hit for particle 2
// pairs[76]=openingAngle; opening angle of the pair  (degree)
// pairs[77]=invM;         invariant Mass of the pair (Mev/c2)
// pairs[78]=pt;           transvers mom for pair
// pairs[79]=rapidity;
// pairs[80]=gPairPhi     phi   of pair in degree (Geant)
// pairs[81]=gPairTheta   theta of pair in degree (Geant)

// pairs[82]=openingAngle_segId
// pairs[83]=invM_segId;
// pairs[84]=pt_segId;
// pairs[85]=rapidity_segId;
// pairs[86]=pairPhi_segId    phi   of pair in degree (ideal segment)
// pairs[87]=pairTheta_segId  theta of pair in degree (ideal segment)

// pairs[88]=openingAngle_seg
// pairs[89]=invM_seg;
// pairs[90]=pt_seg;
// pairs[91]=rapidity_seg;
// pairs[92]=pairPhi_seg      phi   of pair in degree (segment)
// pairs[93]=pairTheta_seg    theta of pair in degree (segment)

// pairs[94]=gphi_1;      phi in degree sector coord. (geant) inner seg track1
// pairs[95]=gtheta_1;    theta in degree sector coord. (geant) inner seg track1
// pairs[96]=gphi_2;      phi in degree sector coord. (geant) inner seg track2
// pairs[97]=gtheta_2;    theta in degree sector coord. (geant) inner seg track2
//
///////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "hmdctrackingeff.h"
#include "hmdcdef.h"
#include "hgeantdef.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hevent.h"
#include "hcategory.h"

#include "hmatrixcategory.h"
#include "hlinearcategory.h"
#include "hmdcsegsim.h"
#include "hmdcidealclasses.h"
#include "hmdctrkcand.h"
#include "hgeantkine.h"
#include "hgeantshower.h"
#include "hgeanttof.h"
#include "hphysicsconstants.h"

#include <iostream> 
#include <iomanip>
#include <stdlib.h>
#include "TDirectory.h"
#include "TNtuple.h"
#include "TStopwatch.h"
#include "TLorentzVector.h"

ClassImp(HMdcTrackingEff)

HMdcTrackingEff::HMdcTrackingEff(void) {
    // Default constructor calls the function setParContainers().
    initVariables();
}

HMdcTrackingEff::HMdcTrackingEff(const Text_t* name,const Text_t* title)
:  HReconstructor(name,title)
{
    // Constructor calls the constructor of class HReconstructor with the name
    // and the title as arguments. It also calls the function setParContainers().
    initVariables();
}

HMdcTrackingEff::~HMdcTrackingEff(void) {
  // destructor deletes the iterator
}
void HMdcTrackingEff::initVariables()
{
    // init of all variables

    offsetSeg1Kine=nEvnt;                        //! starting index in data word for particle 1 (kine)
    offsetSeg2Kine=nEvnt+nKine+nSeg+nChi2;       //! starting index in data word for particle 2 (kine)
    offsetSeg1    =nEvnt+nKine;                  //! starting index in data word for seg1       (seg)
    offsetSeg2    =nEvnt+nKine+nSeg+nChi2+nKine; //! starting index in data word for seg2       (seg)
    offsetChi2    =nSeg;                         //! starting index in data word for chi2 relative to seg

    offsetPair    =nEvnt+nKine+nSeg+nChi2+nKine+nSeg+nChi2; //! starting index in data word for pair   infos
    offsetSingle  =nEvnt+nKine+nSeg+nChi2;                  //! starting index in data word for single infos

    resetIndexTable(indexTable,maxTrks);
    fNameRoot="ntuple_tracking_eff.root";
}

void HMdcTrackingEff::setOutputRoot(TString fname)
{
    // Sets rootfile output of HMdcTrackinEf where ntuples were written.
    //
    fNameRoot="";
    if(fname.CompareTo("")!=0) fNameRoot = fname;
    else {
        Warning("setOutputRoot()","Empty string for filename recieved, will use ntuple_tracking_eff.root instead");
	fNameRoot ="ntuple_tracking_eff.root";
    }
}

Bool_t HMdcTrackingEff::init(void)
{

    catmHMdcTrkCand=(HMatrixCategory*)gHades->getCurrentEvent()->getCategory(catMdcTrkCand);
    if (!catmHMdcTrkCand) {
	Error("init()","No MdcTrkCand cat available!");
	exit(1);
    } else iterTrkCand=(TIterator*)catmHMdcTrkCand->MakeIterator();

    catmHMdcTrkCandIdeal=(HMatrixCategory*)gHades->getCurrentEvent()->getCategory(catMdcTrkCandIdeal);
    if (!catmHMdcTrkCandIdeal) {
	Error("init()","No MdcTrkCandIdeal cat available!");
	exit(1);
    } else iterTrkCandIdeal=(TIterator*)catmHMdcTrkCandIdeal->MakeIterator();

    catmHMdcSegSim=(HMatrixCategory*)gHades->getCurrentEvent()->getCategory(catMdcSeg);
    if (!catmHMdcSegSim) {
	Error("init()","No MdcSegSim cat available!");
	exit(1);
    }

    catmHMdcSegIdeal=(HMatrixCategory*)gHades->getCurrentEvent()->getCategory(catMdcSegIdeal);
    if (!catmHMdcSegIdeal) {
	Error("init()","No MdcSegIdeal cat available!");
	exit(1);
    }

    catlHGeantKine=(HLinearCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);
    if (!catlHGeantKine) {
	Error("init()","No GeantKine cat available!");
	exit(1);
    }

    catmHGeantShower=(HMatrixCategory*)gHades->getCurrentEvent()->getCategory(catShowerGeantRaw);
    if (!catmHGeantShower) {
	Error("init()","No GeantShower cat available!");
	exit(1);
    }  iterGeantShower=(TIterator*)catmHGeantShower->MakeIterator();

    catmHGeantTof=(HMatrixCategory*)gHades->getCurrentEvent()->getCategory(catTofGeantRaw);
    if (!catmHGeantTof) {
	Error("init()","No GeantShower cat available!");
	exit(1);
    }  iterGeantTof=(TIterator*)catmHGeantTof->MakeIterator();




    out=new TFile(fNameRoot.Data(),"RECREATE");
    out->cd();
    single=new TNtuple("single","single","evNr:sector:ptrk:genInfo:genInfo1:genInfo2:gID:mom:vx:vy:vz:"
		       "rId_1:zId_1:thetaId_1:phiId_1:nWId1_1:nWId2_1:"
		       "rId_2:zId_2:thetaId_2:phiId_2:nWId1_2:nWId2_2:"
		       "r_1:z_1:theta_1:phi_1:nW1_1:nW2_1:"
		       "r_2:z_2:theta_2:phi_2:nW1_2:nW2_2:"
		       "chi2_1:chi2_2:foundPair:nTracks:meta");

    pairs=new TNtuple("pairs","pairs","evNr:"
		      "sector_1:ptrk_1:genInfo_1:genInfo1_1:genInfo2_1:gID_1:mom_1:vx_1:vy_1:vz_1:"
		      "rId_1_1:zId_1_1:thetaId_1_1:phiId_1_1:nWId1_1_1:nWId2_1_1:"
		      "rId_2_1:zId_2_1:thetaId_2_1:phiId_2_1:nWId1_2_1:nWId2_2_1:"
		      "r_1_1:z_1_1:theta_1_1:phi_1_1:nW1_1_1:nW2_1_1:"
		      "r_2_1:z_2_1:theta_2_1:phi_2_1:nW1_2_1:nW2_2_1:"
		      "chi2_1_1:chi2_2_1:"
		      "sector_2:ptrk_2:genInfo_2:genInfo1_2:genInfo2_2:gID_2:mom_2:vx_2:vy_2:vz_2:"
		      "rId_1_2:zId_1_2:thetaId_1_2:phiId_1_2:nWId1_1_2:nWId2_1_2:"
		      "rId_2_2:zId_2_2:thetaId_2_2:phiId_2_2:nWId1_2_2:nWId2_2_2:"
		      "r_1_2:z_1_2:theta_1_2:phi_1_2:nW1_1_2:nW2_1_2:"
		      "r_2_2:z_2_2:theta_2_2:phi_2_2:nW1_2_2:nW2_2_2:"
		      "chi2_1_2:chi2_2_2:"
		      "nTracks:meta_1:meta_2:"
		      "openingAngle:invM:pt:rapidity:pairPhi:pairTheta:"
		      "openingAngle_segId:invM_segId:pt_segId:rapidity_segId:pairPhi_segId:pairTheta_segId:"
		      "openingAngle_seg:invM_seg:pt_seg:rapidity_seg:pairPhi_seg:pairTheta_seg:"
		      "gphi_1:gtheta_1:gphi_2:gtheta_2");


    fActive=kTRUE;
    return kTRUE;
}
void HMdcTrackingEff::getSegPointers(Int_t* iSId_1,HMdcSegSim** pSId_1,
				     HMdcTrkCand* trkCandIdeal,
				     HMatrixCategory* catmHMdcSegIdeal,
				     Int_t mode)
{
    // get pointer to segments and index values for TrkCand
    // mode ==0 both segments are done
    // mode ==1 inner segment is done + indseg2=-2

    iSId_1[0]=trkCandIdeal->getSeg1Ind();
    iSId_1[1]=trkCandIdeal->getSeg2Ind();

    for(Int_t iseg=0;iseg<2;iseg++)
    {
	if(iSId_1[iseg]!=-1)
	{
	    pSId_1[iseg]=(HMdcSegSim*)catmHMdcSegIdeal->getObject(iSId_1[iseg]);
	}  else pSId_1[iseg]=0;
    }
    if(mode==1)
    {
	// looking just for inner segment
        pSId_1[1] =0;
	iSId_1[1] =-2;
    }
}
HGeantKine* HMdcTrackingEff::getKineInfo(Int_t trkNr,Int_t& parentTrk,Int_t& gID,
					 Float_t& generatorInfo,
					 Float_t& generatorInfo1,
					 Float_t& generatorInfo2,
					 HLinearCategory* catlHGeantKine)
{
    // retrieves the Geant ID, parentTrack , generatorInfo,
    // generatorInfo1 and generatorInfo2 for Geant track trkNr
    HGeantKine* kine=(HGeantKine*)catlHGeantKine->getObject(trkNr-1);
    gID=kine->getID();
    kine->getGenerator(generatorInfo,generatorInfo1,generatorInfo2);
    parentTrk=kine->getParentTrack();
    return kine;
}
Bool_t HMdcTrackingEff::sameTrkSegments(Int_t trkNr,HMdcSegSim** pSeg,Int_t mode)
{
    // checks if the Geant trkNr of inner and outer segment is the same
    // mode==0 both segments are checked
    // mode==1 inner segment is checked
    if(mode==0)
    {
	if(!pSeg[0] || !pSeg[1])
	{
	    Error("sameTrkSegments()","Zero pointer in mode 0 : %p %p !",pSeg[0],pSeg[1]);
            exit(1);
	}
	if(trkNr==pSeg[0]->getTrack(0)&&
	   trkNr==pSeg[1]->getTrack(0)) {return kTRUE ;}
	else                            {return kFALSE;}
    }else{

	if(!pSeg[0])
	{
	    Error("sameTrkSegments()","Zero pointer in mode 1 : %p !",pSeg[0]);
            exit(1);
	}
        if(trkNr==pSeg[0]->getTrack(0)) {return kTRUE ;}
	else                            {return kFALSE;}
    }
}

HMdcTrkCand* HMdcTrackingEff::findSameRealCand(Int_t trkNr,Int_t* iSeg,HMdcSegSim** pSeg,
					       TIterator* iterTrkCand,
					       HMatrixCategory* catmHMdcTrkCand,
					       HMatrixCategory* catmHMdcSeg,
					       HLinearCategory* catlHGeantKine)
{
    // checks if the Geant trkNr of inner and outer segment is the same
    // if only inner segment is available only this is checked
    // if both segments are there both are checked
    // a loop over all candidates is performed
    // if the match of outer segment in all candidates fails
    // the first object of trkCand is used
    // if no match is found 0 is returned.
    // the indexes of both segments are stored in iSeg. indseg==-1 means
    // the segment has not been found. indseg==-2 says thare have been
    // candidates, but none of them mathed the trk number.

    HMdcTrkCand* trkCnd=0;

    iSeg[0]=iSeg[1]=-999;
    pSeg[0]=pSeg[1]= 0;

    Bool_t found=kFALSE;
    iterTrkCand->Reset();
    while ( (trkCnd=(HMdcTrkCand*)iterTrkCand->Next()) != 0 )
    {
	Int_t nOuterSeg=trkCnd->getNCandForSeg1();

	if(nOuterSeg==-1)
	{   // this is not the first object , skip it!
	    continue;
	}
	getSegPointers(iSeg,pSeg,trkCnd,catmHMdcSeg,1);
	if(!sameTrkSegments(trkNr,pSeg,1)) continue; // inner segment does not match track number!
        
	if(nOuterSeg==0)
	{
	    //-------------------------------------------------------------------
	    // no outer segments:
	    found=kTRUE;
	    break; // go out of outer while loop
            //-------------------------------------------------------------------
	}
	else
	{
            //-------------------------------------------------------------------
	    // up to here the inner segment is matched,
	    // but we have more than 1 outer candidate:
	    // loop over all candidates to match the track.
	    // if no outer candidate is matched the first
	    // object will be used.

	    Int_t  nextObjInd=0;
            Bool_t foundOuter=kFALSE;

	    HMdcTrkCand* pTrkCnd=trkCnd;
	    while (nextObjInd>=0)
	    {
		// Next HMdcTrkCand object
		nextObjInd = pTrkCnd->getNextCandInd();
		if(nextObjInd>=0)
		{
		    pTrkCnd = (HMdcTrkCand*)catmHMdcTrkCand->getObject(nextObjInd);
		    getSegPointers(iSeg,pSeg,pTrkCnd,catmHMdcSeg);
	  
		    if(sameTrkSegments(trkNr,pSeg))
		    {   // matched inner and outer segment found
                        found     =kTRUE;
			foundOuter=kTRUE;
                        trkCnd    =pTrkCnd;
			break; // go out of inner while loop
		    }
		    else
		    {
			found     =kFALSE;
			foundOuter=kFALSE;
		    }
		}
	    }
            if(foundOuter==kTRUE) break; // go out of outer while loop
	    if(foundOuter==kFALSE)
	    { // inner segment found but no outer match
              // in this case we take the first object
		getSegPointers(iSeg,pSeg,trkCnd,catmHMdcSeg,1);
                found=kTRUE;
                break;
	    }
            //-------------------------------------------------------------------
	}
    }
    if(found==kFALSE)
    {
	trkCnd =0;
	iSeg[0]=iSeg[1]=-999;
	pSeg[0]=pSeg[1]= 0;
    }

    return trkCnd;
}
TString HMdcTrackingEff::getProcessString(Float_t generatorInfo1_1)
{
    // decodes the generatorinfo1 into a string for printing

    TString info="Unknown process ";
    if     (generatorInfo1_1==7051) info="pi0 dalitz";
    else if(generatorInfo1_1==7001) info="conversion";
    else if(generatorInfo1_1==17051)info="eta dalitz";
    else if(generatorInfo1_1==34051)info="delta dalitz";
    else if(generatorInfo1_1==41)   info="rho direct";
    else if(generatorInfo1_1==52051)info="omega dalitz";
    else if(generatorInfo1_1==52)   info="omega direct";
    else   info+=generatorInfo1_1;
    return info;
}
Bool_t HMdcTrackingEff::sameDecay(Int_t parenttrk_1,Int_t parenttrk_2,
				  Float_t generatorInfo1_1,Float_t generatorInfo1_2,
				  Float_t generatorInfo2_1,Float_t generatorInfo2_2)
{
    // checks if a lepton track comes from
    // the same source
    if(parenttrk_1 == 0 &&
       parenttrk_2 == 0 &&
       generatorInfo1_1 == generatorInfo1_2 &&
       generatorInfo2_1 == generatorInfo2_2 )
    {
	return kTRUE;
    }
    else
    {
	return kFALSE;
    }
}
void HMdcTrackingEff::resetIndexTable(Int_t* indexTable,Int_t n)
{
    // resets the first n slots of indextable
    if(n>maxTrks) n=maxTrks;
    for(Int_t ii=0;ii<sizeInd*n;ii++)
    {
	indexTable[ii]=-99;
    }
}
Bool_t HMdcTrackingEff::fillIndexTable(Int_t* indexTable,Int_t& track_ct,
				       Int_t trkNrId_1,
				       Int_t* indSegId_1,
				       Int_t* indSeg_1,
				       HLinearCategory* catlHGeantKine)
{
    // fills the index table for track track_ct
    // the maxium number of tracks is maxTrks.
    // the tracks are stored in format
    // indexTable[track_ct*sizeInd+xi]=varx where xi=0-sizeInd and varx are the
    // values in the following table:
    // indKine, indSeg1_id, indSeg2_id, indSeg1, indSeg2, indUsed
    // 0        1           2           3        4        5
    // parentTrk, geantInfo, geantInfo1, geantInfo2
    // 6          7          8           9


    if(track_ct<maxTrks)
    {
        //-------------------------------------------------------------
	// check for Double_t filling
        Bool_t found=kFALSE;
	for(Int_t i=0;i<track_ct;i++)
	{
	    if(indexTable[i*sizeInd+3]==indSeg_1[0]&&indSeg_1[0]>=0)
	    {
		Warning("fillIndexTable()","Same candidate put twice! Skipping!");
                found=kTRUE;
		break;
	    }
	}
	if(!found)
	{
	    //--------------------------------------------------------------
	    Int_t pTrk,gID;
	    Float_t Info,Info1,Info2;
	    getKineInfo(trkNrId_1,pTrk,gID,Info,Info1,Info2,catlHGeantKine);

	    indexTable[track_ct*sizeInd+0]=trkNrId_1;
	    indexTable[track_ct*sizeInd+1]=indSegId_1[0];
	    indexTable[track_ct*sizeInd+2]=indSegId_1[1];
	    indexTable[track_ct*sizeInd+3]=indSeg_1[0];
	    indexTable[track_ct*sizeInd+4]=indSeg_1[1];
	    indexTable[track_ct*sizeInd+5]=0;

	    indexTable[track_ct*sizeInd+6]=pTrk;
	    indexTable[track_ct*sizeInd+7]=(Int_t)Info;
	    indexTable[track_ct*sizeInd+8]=(Int_t)Info1;
	    indexTable[track_ct*sizeInd+9]=(Int_t)Info2;

	    track_ct++; // count up slot number

	} else return kFALSE;
    }else{
	Warning("fillIndexTable()","Number of Tracks larger than maxTrks! skipped!!");
	return kFALSE;
    }
    return kTRUE;
}


void HMdcTrackingEff::printIndexTable(Int_t* indexTable,Int_t track_ct)
{
    // prints one slot track_ct to the screen
    if(track_ct<maxTrks)
    {

	cout<<"trkNr "    <<indexTable[track_ct*sizeInd+0]<<endl;
	cout<<"\t s1_id " <<indexTable[track_ct*sizeInd+1]<<endl;
	cout<<"\t s2_id " <<indexTable[track_ct*sizeInd+2]<<endl;
	cout<<"\t s1    " <<indexTable[track_ct*sizeInd+3]<<endl;
	cout<<"\t s2    " <<indexTable[track_ct*sizeInd+4]<<endl;
	cout<<"\t used  " <<indexTable[track_ct*sizeInd+5]<<endl;
	cout<<"\t pTrk  " <<indexTable[track_ct*sizeInd+6]<<endl;
	cout<<"\t I     " <<indexTable[track_ct*sizeInd+7]<<endl;
	cout<<"\t I1    " <<indexTable[track_ct*sizeInd+8]<<endl;
	cout<<"\t I2    " <<indexTable[track_ct*sizeInd+9]<<endl;

    }else{
	Warning("fillPrintTable()","Number of Tracks larger than maxTrks! skipped!!");
    }
}

Int_t HMdcTrackingEff::findPartner(Int_t* iT,Int_t size,Int_t i)
{
    // returns the slot in indexTable which contains a partner track
    // to track i from the same decay (pair)
    // if nothing is found -1 is returned
    // the found tracks are marked in the table as used
    // self combinations are rejected.
    if(size<maxTrks)
    {
	for(Int_t j=0;j<size;j++)
	{
	    if(iT[i*sizeInd+5]==1  ||iT[j*sizeInd+5]==1  ) continue; // already used combinations
	    if(iT[i*sizeInd+6]==-99||iT[j*sizeInd+6]==-99) continue; // not filled slots
	    if(i==j)                                       continue; // don't combine the same tracks

	    if(sameDecay(iT[i*sizeInd+6],iT[j*sizeInd+6],
			 iT[i*sizeInd+8],iT[j*sizeInd+8],
			 iT[i*sizeInd+9],iT[j*sizeInd+9]))
	    {
		// mark the tracks as used
		iT[i*sizeInd+5]=1;
		iT[j*sizeInd+5]=1;
		return j;
	    }
	}
    }else{
	Warning("findPartner()","Number of Tracks larger than maxTrks! skipped!!");
    }
    return -1;
}
Bool_t HMdcTrackingEff::hasPartner(Int_t* iT,Int_t size,Int_t i)
{
    // checks if track has been used in pair
    if(size<maxTrks)
    {
	if(iT[i*sizeInd+5]==1) return kTRUE;
	else return kFALSE;
    }else{
	Warning("hasPartner()","Number of Tracks larger than maxTrks! skipped!!");
    }
    return kFALSE;
}

void HMdcTrackingEff::fillSegData(Int_t ind,Int_t* iT,Float_t* data,Int_t offset,
				  HMatrixCategory* catmHMdcSegIdeal,
				  HMatrixCategory* catmHMdcSegSim)
{
    // indKine, indSeg1_id, indSeg2_id, indSeg1, indSeg2, indUsed
    // 0        1           2           3        4        5
    // parentTrk, geantInfo, geantInfo1, geantInfo2
    // 6          7          8           9
    HMdcSegSim* seg=0;
    if(iT[ind*sizeInd+1]>=0)
    {
	seg=(HMdcSegSim*)catmHMdcSegIdeal->getObject(iT[ind*sizeInd+1]);
	data[offset+0]=seg->getRprime();
	data[offset+1]=seg->getRprime();
	data[offset+2]=seg->getTheta()*TMath::RadToDeg();
	data[offset+3]=seg->getPhi()  *TMath::RadToDeg();
	data[offset+4]=seg->getSumWires(0);
	data[offset+5]=seg->getSumWires(1);
    }
    if(iT[ind*sizeInd+2]>=0)
    {
	seg=(HMdcSegSim*)catmHMdcSegIdeal->getObject(iT[ind*sizeInd+2]);
	data[offset+6] =seg->getRprime();
	data[offset+7] =seg->getRprime();
	data[offset+8] =seg->getTheta()*TMath::RadToDeg();
	data[offset+9] =seg->getPhi()  *TMath::RadToDeg();
	data[offset+sizeInd]=seg->getSumWires(0);
	data[offset+11]=seg->getSumWires(1);
    }
    if(iT[ind*sizeInd+3]>=0)
    {
	seg=(HMdcSegSim*)catmHMdcSegSim->getObject(iT[ind*sizeInd+3]);
	data[offset+12]=seg->getRprime();
	data[offset+13]=seg->getRprime();
	data[offset+14]=seg->getTheta()*TMath::RadToDeg();
	data[offset+15]=seg->getPhi()  *TMath::RadToDeg();
	data[offset+16]=seg->getSumWires(0);
	data[offset+17]=seg->getSumWires(1);

        data[offset+offsetChi2+0]=seg->getChi2();
    }
    if(iT[ind*sizeInd+4]>=0)
    {
	seg=(HMdcSegSim*)catmHMdcSegSim->getObject(iT[ind*sizeInd+4]);
	data[offset+18]=seg->getRprime();
	data[offset+19]=seg->getRprime();
	data[offset+20]=seg->getTheta()*TMath::RadToDeg();
	data[offset+21]=seg->getPhi()  *TMath::RadToDeg();
	data[offset+22]=seg->getSumWires(0);
	data[offset+23]=seg->getSumWires(1);

	data[offset+offsetChi2+1]=seg->getChi2();
    }
}
void HMdcTrackingEff::fillKineData(Int_t& sec,Int_t ind,Int_t* iT,Float_t* data,Int_t offset,
				   HMatrixCategory* catmHMdcSegIdeal,
				   HLinearCategory* catlHGeantKine)
{
     // indKine, indSeg1_id, indSeg2_id, indSeg1, indSeg2, indUsed
    // 0        1           2           3        4        5
    // parentTrk, geantInfo, geantInfo1, geantInfo2
    // 6          7          8           9
    HGeantKine* kine=0;
    HMdcSegSim* seg=0;
    seg=(HMdcSegSim*)catmHMdcSegIdeal->getObject(iT[ind*sizeInd+1]);

    if(iT[ind*sizeInd+0]>=0&&seg)
    {
        sec=seg->getSec();

	Int_t pTrk,gID;
	Float_t Info,Info1,Info2;
	kine=getKineInfo(iT[ind*sizeInd+0],pTrk,gID,Info,Info1,Info2,catlHGeantKine);
	Float_t vx,vy,vz;
        kine->getVertex(vx,vy,vz);
	data[offset+0]=seg->getSec();
	data[offset+1]=pTrk;
	data[offset+2]=Info;
        data[offset+3]=Info1;
        data[offset+4]=Info2;
        data[offset+5]=gID;
        data[offset+6]=kine->getTotalMomentum();
        data[offset+7]=vx;
        data[offset+8]=vy;
        data[offset+9]=vz;

        

    }else{
	Warning("fillKineData()","Segment Pointer 0 or kine index <0 !");
    }
}
Int_t HMdcTrackingEff::findGeantMeta(Int_t trkNr,Int_t sector,
				     TIterator* iterGeantTof,
				     HMatrixCategory* catmHGeantTof,
				     TIterator* iterGeantShower,
				     HMatrixCategory* catmHGeantShower)
{
    // search meta (shower and meta) for the same Geant track
    // in sector sector.
    // return codes:
    // -1 if nothing has been found
    // -2 if showerhit has been found in other sector
    // -3 if tofhit has been found in other sector
    // 0 if hit has been found in shower (system 0)
    // 1 if hit has been found in tof    (system 1)

    HGeantShower* showerhit;
    iterGeantShower->Reset();
    while ( (showerhit=(HGeantShower*)iterGeantShower->Next()) != 0 )
    {
        if(showerhit->getTrack() !=trkNr)  continue; // find same track
        if(showerhit->getSector()==sector) return 0; // if track was found in same sector
        else return -2;                              // no match in same sector
    }
    HGeantTof* tofhit;
    iterGeantTof->Reset();
    while ( (tofhit=(HGeantTof*)iterGeantTof->Next()) != 0 )
    {
        if(tofhit->getTrack() !=trkNr)  continue;   // find same track
        if(tofhit->getSector()==sector) return 1;   // if track was found in same sector
        else return -3;                             // no match in same sector
    }

    return -1;
}

void HMdcTrackingEff::fillNTuple(Int_t evNr,Int_t* indexTable,Int_t size,
				 HMatrixCategory* catmHMdcSegSim,
				 HMatrixCategory* catmHMdcSegIdeal,
				 HLinearCategory* catlHGeantKine,
				 TIterator* iterGeantTof,
				 HMatrixCategory* catmHGeantTof,
				 TIterator* iterGeantShower,
				 HMatrixCategory* catmHGeantShower,
				 TNtuple* single,TNtuple* pairs)
{
    resetDataArray();

    TLorentzVector vL1;
    TLorentzVector vL2;

    TLorentzVector vL1_segId;
    TLorentzVector vL2_segId;
    TLorentzVector vL1_seg;
    TLorentzVector vL2_seg;

    TVector3 v1;
    TVector3 v2;

    Int_t sec_1,sec_2;
    if(size<maxTrks)
    {
	for(Int_t i=0;i<size;i++)
	{
	    Int_t j=findPartner(indexTable,size,i);
	    if(j!=-1)
	    {
		dPairs[0]=evNr;

		fillKineData(sec_1,i,indexTable,dPairs,offsetSeg1Kine,catmHMdcSegIdeal,catlHGeantKine);
		fillSegData (      i,indexTable,dPairs,offsetSeg1    ,catmHMdcSegIdeal,catmHMdcSegSim);
                fillKineData(sec_2,j,indexTable,dPairs,offsetSeg2Kine,catmHMdcSegIdeal,catlHGeantKine);
		fillSegData (      j,indexTable,dPairs,offsetSeg2    ,catmHMdcSegIdeal,catmHMdcSegSim);

		Float_t gphi_1,gphi_2,gtheta_1,gtheta_2;

		HGeantKine* kine;
                kine=(HGeantKine*)catlHGeantKine->getObject(indexTable[i*sizeInd+0]-1);
		fillVectorFromKine(vL1,i);

		kineToSegPhiThetaDeg(kine,gtheta_1,gphi_1);

		if(indexTable[i*sizeInd+1]>=0&&indexTable[j*sizeInd+1]>=0)
		{
		    fillVectorFromSeg(vL1_segId,i,0);
		}
                if(indexTable[i*sizeInd+3]>=0&&indexTable[j*sizeInd+3]>=0)
		{
                    fillVectorFromSeg(vL1_seg,i,1);
		}

		kine=(HGeantKine*)catlHGeantKine->getObject(indexTable[j*sizeInd+0]-1);

		fillVectorFromKine(vL2,j);

		kineToSegPhiThetaDeg(kine,gtheta_2,gphi_2);

		if(indexTable[i*sizeInd+1]>=0&&indexTable[j*sizeInd+1]>=0)
		{
		    fillVectorFromSeg(vL2_segId,j,0);
		}

		if(indexTable[i*sizeInd+3]>=0&&indexTable[j*sizeInd+3]>=0)
		{
		    fillVectorFromSeg(vL2_seg,j,1);
		}

                TLorentzVector dilep      = vL1       + vL2;
                TLorentzVector dilep_segId= vL1_segId + vL2_segId;
                TLorentzVector dilep_seg  = vL1_seg   + vL2_seg;

                dPairs[offsetPair+0] =size;
                dPairs[offsetPair+1] =findGeantMeta(indexTable[i*sizeInd+0],sec_1,iterGeantTof,catmHGeantTof,iterGeantShower,catmHGeantShower);
                dPairs[offsetPair+2] =findGeantMeta(indexTable[j*sizeInd+0],sec_2,iterGeantTof,catmHGeantTof,iterGeantShower,catmHGeantShower);
		dPairs[offsetPair+3] =vL2.Angle(vL1.Vect())*TMath::RadToDeg();
                dPairs[offsetPair+4] =dilep.M();
		dPairs[offsetPair+5] =dilep.Pt();
                dPairs[offsetPair+6] =dilep.Rapidity();
                dPairs[offsetPair+7] =pairPhiToLabDeg(dilep.Phi());
                dPairs[offsetPair+8] =pairThetaToLabDeg(dilep.Theta());

		if(indexTable[i*sizeInd+1]>=0&&indexTable[j*sizeInd+1]>=0)
		{
		    dPairs[offsetPair+9] =vL2_segId.Angle(vL1_segId.Vect())*TMath::RadToDeg();
		    dPairs[offsetPair+10]=dilep_segId.M();
		    dPairs[offsetPair+11]=dilep_segId.Pt();
		    dPairs[offsetPair+12]=dilep_segId.Rapidity();
		    dPairs[offsetPair+13]=pairPhiToLabDeg(dilep_segId.Phi());
		    dPairs[offsetPair+14]=pairThetaToLabDeg(dilep_segId.Theta());
		}
		if(indexTable[i*sizeInd+3]>=0&&indexTable[j*sizeInd+3]>=0)
		{
		    dPairs[offsetPair+15]=vL2_seg.Angle(vL1_seg.Vect())*TMath::RadToDeg();
		    dPairs[offsetPair+16]=dilep_seg.M();
		    dPairs[offsetPair+17]=dilep_seg.Pt();
		    dPairs[offsetPair+18]=dilep_seg.Rapidity();
		    dPairs[offsetPair+19]=pairPhiToLabDeg(dilep_seg.Phi());
		    dPairs[offsetPair+20]=pairThetaToLabDeg(dilep_seg.Theta());
		}
		dPairs[offsetPair+21]=gphi_1;
                dPairs[offsetPair+22]=gtheta_1;
                dPairs[offsetPair+23]=gphi_2;
                dPairs[offsetPair+24]=gtheta_2;

                pairs->Fill(dPairs);
	    }
            dSingle[0]=evNr;
	    fillKineData(sec_1,i,indexTable,dSingle,offsetSeg1Kine,catmHMdcSegIdeal,catlHGeantKine);
            fillSegData (      i,indexTable,dSingle,offsetSeg1    ,catmHMdcSegIdeal,catmHMdcSegSim);
	    if(hasPartner(indexTable,size,i))dSingle[offsetSingle+0]=1;
            else                             dSingle[offsetSingle+0]=0;
            dSingle[offsetSingle+1]=size;
	    dSingle[offsetSingle+2]=findGeantMeta(indexTable[i*sizeInd+0],sec_1,iterGeantTof,catmHGeantTof,iterGeantShower,catmHGeantShower);

	    single->Fill(dSingle);

	}
    }else{
	Warning("fillNTuple()","Number of Tracks larger than maxTrks! skipped !");
    }
}

void  HMdcTrackingEff::fillVectorFromSeg(TLorentzVector& v, Int_t slot,Int_t type)
{
    // fills vector v with one leg of a pair from slot in indextable with segment information
    // type has to be 0 (segment from ideal tracking) or 1 (segments from reconstruction)
    if(type==0||type==1)
    {
	HMdcSegSim* seg=0;
	Int_t off=type*2;
	if(type==0)seg=(HMdcSegSim*)catmHMdcSegIdeal->getObject(indexTable[slot*sizeInd+1+off]);
	if(type==1)seg=(HMdcSegSim*)catmHMdcSegSim  ->getObject(indexTable[slot*sizeInd+1+off]);
	HGeantKine* kine=(HGeantKine*)catlHGeantKine->getObject(indexTable[slot*sizeInd+0]-1);
	Int_t sec     =seg->getSec();
	Float_t theta =seg->getTheta();
	Float_t phi   =getLabPhiDeg(sec,seg->getPhi())*TMath::DegToRad();
	Float_t totmom=kine->getTotalMomentum();
	v.SetXYZM(totmom * TMath::Sin(theta) * TMath::Cos(phi),
		  totmom * TMath::Sin(theta) * TMath::Sin(phi),
		  totmom * TMath::Cos(theta),HPhysicsConstants::mass(kine->getID()));
    }else
    {
	Error("fillVectorFromSeg()","Wrong type, should be 0 or 1!");
        exit(1);
    }
}
void  HMdcTrackingEff::fillVectorFromKine(TLorentzVector& v, Int_t slot)
{
    // fills vector v with one leg of a pair from slot in indextable with kine information

    Float_t xmom,ymom,zmom;
    HGeantKine* kine=(HGeantKine*)catlHGeantKine->getObject(indexTable[slot*sizeInd+0]-1);

    kine->getMomentum(xmom,ymom,zmom);
    Double_t mass  =HPhysicsConstants::mass(kine->getID());
    Double_t energy=TMath::Sqrt(mass*mass + xmom*xmom + ymom*ymom + zmom*zmom);
    v.SetPxPyPzE(xmom,ymom,zmom,energy);
}
void HMdcTrackingEff::kineToSegPhiThetaDeg(HGeantKine* kine,Float_t& theta,Float_t& phi)
{
    // Convert geant mom to MDC phi/theta angle in degree
    Float_t xmom,ymom,zmom,mom;
    kine->getMomentum(xmom,ymom,zmom);
    mom   =kine->getTotalMomentum();
    theta = (mom>0.) ? (TMath::RadToDeg() * TMath::Abs(TMath::ACos(zmom / mom))) : 0.;
    phi   = TMath::RadToDeg() * TMath::ATan2( ymom, xmom);
    if (phi < 0.) phi += 360.;
    phi=fmod(phi,60.f)+60.;

}
Float_t  HMdcTrackingEff::pairPhiToLabDeg(Float_t phi)
{
    // Convert phi from TLorentzVector to phi Lab in degree
    phi   = TMath::RadToDeg() * phi;
    if (phi < 0.) phi += 360.;
    return phi;
}
Float_t  HMdcTrackingEff::pairThetaToLabDeg(Float_t theta)
{
    // Convert theta from TLorentzVector to theta Lab in degree
    theta   = TMath::Abs((TMath::RadToDeg() * theta));
    return theta;
}
Float_t HMdcTrackingEff::getLabPhiDeg(Int_t sector, Float_t phi)
{
    // Convert MDC phi angle to the lab coordinate system  and degree

    phi = TMath::RadToDeg() * phi;

    switch(sector)
    {
        case 0:
            break;

        case 1:
        case 2:
        case 3:
        case 4:
            phi += 60.0 * sector;
            break;

        default:
            phi -= 60.0f;
            break;
    }

    return phi;
}
Float_t HMdcTrackingEff::getThetaPairDeg(TLorentzVector& v)
{
    // transform theta of vector to lab system and degree
    return TMath::RadToDeg() * v.Theta();
}

// -----------------------------------------------------------------------------

Float_t HMdcTrackingEff::getPhiPairDeg(TLorentzVector& v)
{
    // transform phi of vector to lab system and degree
    Float_t phi;
    if((phi = TMath::RadToDeg() * v.Phi()) < 0.0)  phi += 360.0;

    return phi;
}
Bool_t HMdcTrackingEff::finalize(void)
{
    out->cd();
    single->Write();
    pairs->Write();
    out->Save();
    out->Close();
    return kTRUE;
}

Int_t HMdcTrackingEff::execute()
{
    Int_t track_ct=0;
    //-------------------------------------
    resetIndexTable(indexTable,track_ct);
    track_ct=0;

    HMdcTrkCandIdeal* trkCandIdeal=0;


    iterTrkCandIdeal->Reset();
    while ( (trkCandIdeal=(HMdcTrkCandIdeal*)iterTrkCandIdeal->Next()) != 0 )
    {
	Int_t nOuterSeg=trkCandIdeal->getNCandForSeg1();
        
	if(nOuterSeg==-1)
	{
	    // not first object ,skip it!
	    continue;
	}
	if(nOuterSeg==0)
	{
            // no outer segment available
            // nothing special has to be done
	}
        else
	{
	    // first object of list of matched candidates (only inner seg)
	    // To get the combination with outer seg one has to go to next object
	    // In case the outer segment is missing this part is skipped (nOuterSeg==0)
	    Int_t nextObjInd = trkCandIdeal->getNextCandInd();
	    trkCandIdeal     = (HMdcTrkCandIdeal*)catmHMdcTrkCandIdeal->getObject(nextObjInd);
	    if(!trkCandIdeal)
	    {
		Error("tracking()","Zero pointer for trkCandIdeal retrieved!");
		exit(1);
	    }
	}
	getSegPointers(indSegId,pSegId,trkCandIdeal,catmHMdcSegIdeal);
	trkNrId=pSegId[0]->getTrack(0); // both segments have the same track
	findSameRealCand(trkNrId,indSeg,pSeg,iterTrkCand,catmHMdcTrkCand,catmHMdcSegSim,catlHGeantKine);
	fillIndexTable(indexTable,track_ct,trkNrId,indSegId,indSeg,catlHGeantKine);
    }
    fillNTuple(gHades->getEventCounter(),indexTable,track_ct,catmHMdcSegSim,catmHMdcSegIdeal,catlHGeantKine,
	       iterGeantTof,catmHGeantTof,iterGeantShower,catmHGeantShower,single,pairs);
    return 0;
}

