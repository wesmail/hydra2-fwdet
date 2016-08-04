//*-- Author   : Diego Gonzalez-Diaz
//*-- Created  : 20/12/07
//*-- Modified : 30/12/09 Diego Gonzalez-Diaz
//*-- Modified : 24/11/10 Pablo Cabanelas (ClusFPar moved to condition)
//*-- Modified : 27/06/12 Alessio Mangiarotti Added filling of xlab,
//*-- Modified :          ylab and zlab for simulation.
//
//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////
//
//  HRpcClusterF: RPC detector cluster finder
//
//  Gets hits from RpcHit(Sim) and writes to RpcCluster(Sim)
//
///////////////////////////////////////////////////////////////////

#include "TRandom.h"
#include "hrpcdetector.h"
#include "hrpcclusterf.h"
#include "rpcdef.h"
#include "hrpchit.h"
#include "hrpchitsim.h"
#include "hrpccluster.h"
#include "hrpcclustersim.h"
#include "hgeantrpc.h"
#include "hrpcgeomcellpar.h"
#include "hrpcclusfpar.h"
#include "hdebug.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hevent.h"
#include "hgeantkine.h"
#include "hcategory.h"
#include <iostream>
#include <iomanip>
#include <time.h>
using namespace std;

void HRpcClusterF::initParContainer() {
    pGeomCellPar   = (HRpcGeomCellPar*)(gHades->getRuntimeDb()->getContainer("RpcGeomCellPar"));
    pClusFPar      = (HRpcClusFPar*)(gHades->getRuntimeDb()->getContainer("RpcClusFPar"));
}

HRpcClusterF::HRpcClusterF(void)
{
    pClusterCat  = NULL;
    pHitCat      = NULL;
    pGeomCellPar = NULL;
    pClusFPar    = NULL;
    doSingleCluster = kFALSE;
}

HRpcClusterF::HRpcClusterF(const Text_t *name, const Text_t *title) :
HReconstructor(name,title)
{
    pClusterCat  = NULL;
    pHitCat      = NULL;
    pGeomCellPar = NULL;
    pClusFPar    = NULL;
    doSingleCluster = kFALSE;
}

HRpcClusterF::~HRpcClusterF(void)
{
}

Bool_t HRpcClusterF::init(void)
{
    initParContainer();

    HRpcDetector *rpc;
    rpc=(HRpcDetector *)gHades->getSetup()->getDetector("Rpc");

    maxcells=rpc->getMaxCells();

    if (!pGeomCellPar){
	Error("init","No RpcGeomCellPar Parameters");
	return kFALSE;
    }
    if (!pClusFPar){
	Error("init","No RpcClusFPar Parameters");
	return kFALSE;
    }
    if(!rpc){
	Error("HRpcClusterF::init()","No Rpc Detector");
	return kFALSE;
    }

    // Create category for hit
    pHitCat=gHades->getCurrentEvent()->getCategory(catRpcHit);
    if (!pHitCat) {
	Error("HRpcClusterF::init()","No RpcHit Category");
	return kFALSE;
    }

    // Decide whether we are running for simulation or real data
    HCategory* catKin=gHades->getCurrentEvent()->getCategory(catGeantKine);

    if(catKin) simulation=kTRUE;
    else       simulation=kFALSE;

    // Create category for cluster data
    pClusterCat=gHades->getCurrentEvent()->getCategory(catRpcCluster);

    if (!pClusterCat) {

	if (simulation) pClusterCat=rpc->buildMatrixCategory("HRpcClusterSim",0.5);
	else            pClusterCat=rpc->buildMatrixCategory("HRpcCluster",0.5);

	gHades->getCurrentEvent()->addCategory(catRpcCluster,pClusterCat,"Rpc");

    }


    // Search for hgeantrpc in case of simulation

    if(simulation) {
	pGeantRpcCat = gHades->getCurrentEvent()->getCategory(catRpcGeantRaw);
	if (!pGeantRpcCat) {
	    Error("HRpcClusterF::init()","No HGeant RPC Category");
	    return kFALSE;
	}
    }

    loc.set(3,0,0,0);
    loc_cluster.set(1,0);

    return kTRUE;
}

Int_t HRpcClusterF::execute(void)
{

    HRpcHitSim     *pHit        = NULL; //Only reading, use the same class for HRpcHitSim and HRpcHit objects
    HRpcClusterSim *pClusterSim = NULL; //Reading and STORING in category, use 2 different classes.
    HRpcCluster    *pCluster    = NULL; //Reading and STORING in category, use 2 different classes.

    Int_t cluster_type  = -1;
    Int_t sign          = 0;            //Function sign;
    Float_t tof, charge, xmod, ymod, zmod, xsec, ysec, zsec;
    Float_t xlab, ylab, zlab, theta, phi;

    Float_t tof_neigb, charge_neigb, xmod_neigb, ymod_neigb, xsec_neigb, ysec_neigb, zsec_neigb;
    Float_t xlab_neigb, ylab_neigb, zlab_neigb, theta_neigb, phi_neigb;
    Float_t tof_neigb2;
    Float_t sigma_X, sigma_Y, sigma_Z, sigma_T;
    Float_t tof_cut, x_cut, DPlanes, tof_next; //[ns], [mm], [mm], [ns]
    Float_t v_light=300; //300mm/ns

    Int_t Track1L=-999, Track1R=-999, Track2L=-999, Track2R=-999, Track=-999;
    Int_t Ref1L=-999, Ref1R=-999, Ref2L=-999, Ref2R=-999;
    Bool_t LisAtBox1=kFALSE, LisAtBox2=kFALSE, RisAtBox1=kFALSE, RisAtBox2=kFALSE, IsAtBox=kFALSE;

    Float_t NsigmasT, NsigmasX;
    Int_t cell1=-1, column1=-1, sector1=-1, cell2=-1, column2=-1, sector2=-1;
    Bool_t inFlag1=kFALSE, inFlag2=kFALSE;
    //NOTE: Biggest cluster is assumed to include only 2 cells. More rare cases are likely to not be relevant.
    //Even if in these very rare cases (i.e. 3 cells firing) the only problem of this assumption is
    //that only the closest neighbouring cell will be considered for the cluster, the remaining cell will be
    //counted as a different cluster --> so 1 track would produce 2 clusters but even so, this is not conceptually
    //a problem. Tracking and matching should take care of this.

    Int_t weight[4], array[4], ref_array[4];
    Bool_t isAtBox_array[4];

    Bool_t BC1IsNotIn=kFALSE; //FIXME: Flag to indicate if the BC1 is in or not. It is not needed and
    //it must dissappear

    for (Int_t i=0;i<4;i++) weight[i]        =      0;
    for (Int_t i=0;i<4;i++) array[i]         =   -999;
    for (Int_t i=0;i<4;i++) ref_array[i]     =   -999;
    for (Int_t i=0;i<4;i++) isAtBox_array[i] = kFALSE;

    //HRpcGeomCellParCell& pGeoCell  = (*pGeomCellPar)[0][0][0];
    DPlanes = pGeomCellPar->getDPlanes();



    //---------------------------------------------------------------
    if(doSingleCluster && pHitCat && pClusterCat){ // just do a copy from hit to cluster
	TIterator *pRpcHitIter  = pHitCat->MakeIterator("native");

	if (pRpcHitIter) {
	    HRpcHit* rpc_hit=0;

	    pRpcHitIter->Reset();
	    while ((rpc_hit = (HRpcHit*)pRpcHitIter->Next())) {

		loc[0] = rpc_hit->getSector();
		loc[1] = rpc_hit->getColumn();
		loc[2] = rpc_hit->getCell();
                Int_t index_cluster;
                loc_cluster[0] = rpc_hit->getSector(); //sector

		pClusterSim = (HRpcClusterSim*)pClusterCat->getNewSlot(loc_cluster,&index_cluster);
		if ( !pClusterSim ) {
		    Error ("execute()", "Can't get slot in cluster finder: sec %i", loc_cluster[0]);
		    return EXIT_FAILURE;
		}
		if (simulation)pClusterSim = new(pClusterSim) HRpcClusterSim;
		else           pClusterSim = (HRpcClusterSim*) new(pClusterSim) HRpcCluster;

		pClusterSim->setClusterType   (1);
		pClusterSim->setInsideCellFlag(rpc_hit->getInsideCellFlag());
		pClusterSim->setDetID1 (rpc_hit->getSector(),rpc_hit->getColumn(),rpc_hit->getCell());
		pClusterSim->setCluster(rpc_hit->getTof()   ,rpc_hit->getCharge(),rpc_hit->getXMod(),rpc_hit->getYMod(),rpc_hit->getZMod());
		pClusterSim->setRSEC   (rpc_hit->getXSec()  ,rpc_hit->getYSec()  ,rpc_hit->getZSec());
		pClusterSim->setRMS    (rpc_hit->getTOFRMS(),rpc_hit->getXRMS()  ,rpc_hit->getYRMS(),rpc_hit->getZRMS());
		pClusterSim->setAddress(rpc_hit->getSector(),index_cluster);
		pClusterSim->setTheta  (rpc_hit->getTheta());
		pClusterSim->setPhi    (rpc_hit->getPhi());
		rpc_hit    ->getXYZLab (xlab,ylab,zlab);
		pClusterSim->setXYZLab (xlab,ylab,zlab);


		if (simulation) {

		    //Initialization of parameters
		    for (Int_t t=0;t<4;t++) weight[t]        = 0;
		    for (Int_t t=0;t<4;t++) array[t]         = -999;
		    for (Int_t t=0;t<4;t++) ref_array[t]     = -999;
		    for (Int_t t=0;t<4;t++) isAtBox_array[t] = kFALSE;

		    array        [0] = pHit->getTrackL();
		    array        [1] = pHit->getTrackR();
		    ref_array    [0] = pHit->getRefL();
		    ref_array    [1] = pHit->getRefR();
		    isAtBox_array[0] = pHit->getLisAtBox();
		    isAtBox_array[1] = pHit->getRisAtBox();

		    //Algorithm for deciding the preferred track
		    for(Int_t m=0;m<4;m++){
			for(Int_t n=0;n<4;n++){
			    if(array[m]==array[n]) weight[m]++;
			}
		    }
		    //Note that double-counting and self-counting does not affect the meaning of weight here :)

		    Track   = array        [0];
		    IsAtBox = isAtBox_array[0];

		    for(Int_t m=0;m<3;m++)   {
			if (weight[m+1]>weight[m]) {
			    Track=array[m+1];
			    IsAtBox = isAtBox_array[m];}
		    }

		    pClusterSim->setTrack(Track);
		    pClusterSim->setIsAtBox(IsAtBox);


		    //Fill the lists
		    for(Int_t m=0;m<4;m++) {
			pClusterSim->setTrackList  (m,array[m]);
			pClusterSim->setRefList    (m,ref_array[m]);
			pClusterSim->setIsAtBoxList(m,isAtBox_array[m]);
		    }

		    pClusterSim->setNTracksAtBox  (howManyTracks       (pClusterSim));
		    pClusterSim->setNTracksAtCells(howManyTracksAtCells(pClusterSim));

		}



	    }
	}

        delete  pRpcHitIter ;


	return EXIT_SUCCESS;
    }
    //---------------------------------------------------------------





    //Simple and robust sequencial algorithm for cluster finding enforcing no double-counting
    //If algorithm becomes slow, this must be done in a more clever way

    for(Int_t i=0;i<6;i++) {                 //6 sectors

	NsigmasX = pClusFPar->getNsigmasX();
	NsigmasT = pClusFPar->getNsigmasT();

	for(Int_t j=0;j<3;j++) {               //3 groups of 2 columns
	    for(Int_t k=0;k<(maxcells*2-1);k++){ //2 columns with maxcells

		//Initialization of parameters
		for (Int_t t=0;t<4;t++) weight[t]        = 0;
		for (Int_t t=0;t<4;t++) array[t]         = -999;
		for (Int_t t=0;t<4;t++) ref_array[t]     = -999;
		for (Int_t t=0;t<4;t++) isAtBox_array[t] = kFALSE;

		cell1 = column1 = sector1 = -1;
		cell2 = column2 = sector2 = -1;
		//End of initialization of parameters

		loc[0] = i;
		loc[1] = 2*j+1-k%2;                //Alternates the columns (down, up, down, up...)
		loc[2] = k/2;

		if(BC1IsNotIn && j==1) loc[1] = 2+k%2;

		pHit = (HRpcHitSim*)pHitCat->getObject(loc);

		if(!pHit) continue;

		//Otherwise. A cluster will be created. Check if it can be allocated.

		loc_cluster[0]=i; //sector
		Int_t index_cluster;

		sigma_X= pHit->getXRMS();
		sigma_Y= pHit->getYRMS();
		sigma_Z= pHit->getZRMS();
		sigma_T= pHit->getTOFRMS();

		tof_cut = sigma_T*NsigmasT;
		x_cut   = sigma_X*NsigmasX;

		if (simulation) {
		    pClusterSim = (HRpcClusterSim*)pClusterCat->getNewSlot(loc_cluster,&index_cluster);
		    if ( !pClusterSim ) {
			Error ("execute()", "Can't get slot in cluster finder: sec %i", loc_cluster[0]);
			return EXIT_FAILURE;
		    }
		    pClusterSim = new(pClusterSim) HRpcClusterSim;
		}
		else {
		    pCluster = (HRpcCluster*)pClusterCat->getNewSlot(loc_cluster,&index_cluster);
		    if ( !pCluster ) {
			Error ("execute()", "Can't get slot in cluster finder: sec %i", loc_cluster[0]);
			return EXIT_FAILURE;
		    }
		    pCluster = new(pCluster) HRpcCluster;
		}

		//Read data members from hit

		tof     = pHit->getTof();
		charge  = pHit->getCharge();
		xmod    = pHit->getXMod();
		ymod    = pHit->getYMod();
		zmod    = DPlanes*(0.5-k%2);
		if(BC1IsNotIn && j==1) zmod   = DPlanes*(k%2-0.5);
		xsec    = pHit->getXSec();
		ysec    = pHit->getYSec();
		zsec    = pHit->getZSec();
		theta   = pHit->getTheta();
		phi     = pHit->getPhi();
		sector1 = pHit->getSector();
		column1 = pHit->getColumn();
		cell1   = pHit->getCell();
		inFlag1 = pHit->getInsideCellFlag();

		pHit->getXYZLab(xlab,ylab,zlab);

		cluster_type=1;

		if (simulation){

		    Ref1L       = pHit->getRefL();
		    Ref1R       = pHit->getRefR();
		    Track1L     = pHit->getTrackL();
		    Track1R     = pHit->getTrackR();
		    LisAtBox1   = pHit->getLisAtBox();
		    RisAtBox1   = pHit->getRisAtBox();
		}

		//Look for next cell in next column. True cluster finder starts here.

		loc[1] = 2*j + 1 - (k+1)%2;
		loc[2] = (k+1)/2;
		if(BC1IsNotIn && j==1) loc[1] = 2 + (k+1)%2;

		pHit = (HRpcHitSim*)pHitCat->getObject(loc);


		if (pHit) {	//Neighbour

		    tof_neigb    = pHit->getTof();
		    charge_neigb = pHit->getCharge();
		    xmod_neigb   = pHit->getXMod();
		    ymod_neigb   = pHit->getYMod();
		    xsec_neigb   = pHit->getXSec();
		    ysec_neigb   = pHit->getYSec();
		    zsec_neigb   = pHit->getZSec();
		    theta_neigb  = pHit->getTheta();
		    phi_neigb    = pHit->getPhi();
		    sector2      = pHit->getSector();
		    column2      = pHit->getColumn();
		    cell2        = pHit->getCell();
		    inFlag2      = pHit->getInsideCellFlag();

		    pHit->getXYZLab(xlab_neigb,ylab_neigb,zlab_neigb);

		    //Offset to the next station. Positive or negative, it depends
		    sign         = (2*loc[2]%2-1);
		    tof_next     = sign*DPlanes/v_light;
		    //NOTE: Probably it is *enough* in practice to consider that *all*
		    //particles travel at the speed of light and are perpendicular?.

		    //Cluster condition
		    if((fabs(xmod-xmod_neigb)<x_cut) && fabs(tof+tof_next-tof_neigb)<tof_cut){

			tof    = (tof    + tof_neigb)/2;
			charge = (charge + charge_neigb)/2;
			xmod   = (xmod   + xmod_neigb)/2;
			ymod   = (ymod   + ymod_neigb)/2;
			zmod   =  0.001; //must be different from zero for metamatch
			xsec   = (xsec   + xsec_neigb)/2;
			ysec   = (ysec   + ysec_neigb)/2;
			zsec   = (zsec   + zsec_neigb)/2;
			xlab   = (xlab   + xlab_neigb)/2;
			ylab   = (ylab   + ylab_neigb)/2;
			zlab   = (zlab   + zlab_neigb)/2;
			theta  = (theta  + theta_neigb)/2;
			phi    = (phi    + phi_neigb)/2;

			cluster_type=2;

			if (simulation) {

			    Ref2L       = pHit->getRefL();
			    Ref2R       = pHit->getRefR();
			    Track2L     = pHit->getTrackL();
			    Track2R     = pHit->getTrackR();
			    LisAtBox2   = pHit->getLisAtBox();
			    RisAtBox2   = pHit->getRisAtBox();
			}

			//At last, it must be checked that the cell can NOT form a cluster with the
			//next neigbour, then the counter -k- should be increased. Otherwise,
			//this will be double-counted in the next iteration as a new cluster
			//with cluster_type=1 [!!]

			loc[1] = 2*j + 1 - (k+2)%2;
			loc[2] = (k+2)/2;
			if(BC1IsNotIn && j==1) loc[1] = 2 + (k+2)%2;

			pHit = (HRpcHitSim*)pHitCat->getObject(loc);

			if (pHit) {

			    tof_neigb2   = pHit->getTof();

			    sign         = (2*loc[2]%2-1);
			    tof_next     = sign*DPlanes/v_light;
			    if((fabs(xmod-xmod_neigb)>=x_cut) || fabs(tof+tof_next-tof_neigb2)>=tof_cut) k=k+1;
			}
			else k=k+1;
			//Note that only if a new cluster will be formed, the loop goes on normaly.
			//If not, -k- is increased.
		    } // end cluster condition
		} // end neighbour

		//Fill cluster

		//If simulation
		if (simulation) {

		    pClusterSim->setClusterType(cluster_type);
		    pClusterSim->setCluster(tof, charge, xmod, ymod, zmod);
		    pClusterSim->setRSEC(xsec,ysec,zsec);
		    pClusterSim->setXYZLab(xlab,ylab,zlab);
		    pClusterSim->setTheta(theta);
		    pClusterSim->setPhi(phi);
		    pClusterSim->setAddress(loc_cluster[0], index_cluster);

		    //Storing cell info of cluster size, trackID and Ref for simulation:

		    if(cluster_type==1) {
			pClusterSim->setDetID1(sector1, column1, cell1);
			array[0]=Track1L; array[1]=Track1R;
			ref_array[0]=Ref1L; ref_array[1]=Ref1R;
			isAtBox_array[0]=LisAtBox1; isAtBox_array[1]=RisAtBox1;
			pClusterSim->setRMS(sigma_T, sigma_X, sigma_Y, sigma_Z);
		    }
		    //Just for convenience the first cell upstream is stored as DetID1
		    else{
			if(column1==0 || column1==2 || column1==4) {
			    pClusterSim->setDetID1(sector1, column1, cell1);
			    pClusterSim->setDetID2(sector2, column2, cell2);
			    array[0]=Track1L; array[1]=Track1R; array[2]=Track2L; array[3]=Track2R;
			    ref_array[0]=Ref1L; ref_array[1]=Ref1R; ref_array[2]=Ref2L; ref_array[3]=Ref2R;
			    isAtBox_array[0]=LisAtBox1; isAtBox_array[1]=RisAtBox1; isAtBox_array[2]=LisAtBox2; isAtBox_array[3]=RisAtBox2;
			}

			if(column1==1 || column1==3 || column1==5) {
			    pClusterSim->setDetID1(sector2, column2, cell2);
			    pClusterSim->setDetID2(sector1, column1, cell1);
			    array[0]=Track2L; array[1]=Track2R; array[2]=Track1L; array[3]=Track1R;
			    ref_array[0]=Ref2L; ref_array[1]=Ref2R; ref_array[2]=Ref1L; ref_array[3]=Ref1R;
			    isAtBox_array[0]=LisAtBox2; isAtBox_array[1]=RisAtBox2; isAtBox_array[2]=LisAtBox1; isAtBox_array[3]=RisAtBox1;
			}
			pClusterSim->setRMS(sigma_T/sqrt(2.), sigma_X/sqrt(2.), sigma_Y/sqrt(2.), sigma_Z/sqrt(2.));
		    }

		    //Algorithm for deciding the preferred track
		    for(Int_t m=0;m<4;m++){
			for(Int_t n=0;n<4;n++){
			    if(array[m]==array[n]) 		  weight[m]++;
			}
		    }
		    //Note that double-counting and self-counting does not affect the meaning of weight here :)

		    Track   = array[0];
		    IsAtBox = isAtBox_array[0];

		    for(Int_t m=0;m<3;m++)   {
			if (weight[m+1]>weight[m]) {
			    Track=array[m+1];
			    IsAtBox = isAtBox_array[m];}
		    }

		    pClusterSim->setTrack(Track);
		    pClusterSim->setIsAtBox(IsAtBox);


		    //Fill the lists
		    for(Int_t m=0;m<4;m++) {
			pClusterSim->setTrackList(m,array[m]);
			pClusterSim->setRefList(m,ref_array[m]);
			pClusterSim->setIsAtBoxList(m,isAtBox_array[m]);
		    }

		    pClusterSim->setNTracksAtBox(howManyTracks(pClusterSim));
		    pClusterSim->setNTracksAtCells(howManyTracksAtCells(pClusterSim));
		}

		//if real data
		else {

		    pCluster->setClusterType(cluster_type);
		    pCluster->setCluster(tof, charge, xmod, ymod, zmod);
		    pCluster->setRSEC(xsec,ysec,zsec);
		    pCluster->setXYZLab(xlab,ylab,zlab);
		    pCluster->setTheta(theta);
		    pCluster->setPhi(phi);
		    pCluster->setAddress(loc_cluster[0], index_cluster);

		    if(cluster_type==1) {
			pCluster->setDetID1(sector1, column1, cell1);
			pCluster->setRMS(sigma_T, sigma_X, sigma_Y, sigma_Z);
			if(inFlag1) pCluster->setInsideCellFlag(1);
		    }
		    else{
			if(column1==0 || column1==2 || column1==4) {
			    pCluster->setDetID1(sector1, column1, cell1);
			    pCluster->setDetID2(sector2, column2, cell2);
			}

			if(column1==1 || column1==3 || column1==5) {
			    pCluster->setDetID1(sector2, column2, cell2);
			    pCluster->setDetID2(sector1, column1, cell1);
			}

			pCluster->setRMS(sigma_T/sqrt(2.), sigma_X/sqrt(2.), sigma_Y/sqrt(2.), sigma_Z/sqrt(2.));

			if(inFlag1 && inFlag2) {
			    pCluster->setInsideCellFlag(1);
			} else {
			    pCluster->setInsideCellFlag(0);
			    if(inFlag1) pCluster->setInsideCellFlag(2);
			    if(inFlag2) pCluster->setInsideCellFlag(3);
			}
		    }
		}
	    } // end loop k (cells)
	} // end loop j (groups of columns)
    } // end sector loop

    return EXIT_SUCCESS;
}

Int_t HRpcClusterF::howManyTracks(HRpcClusterSim* pClusterSim) {

    HRpcHitSim     *pHit1        = 0;
    HRpcHitSim     *pHit2        = 0;
    HLocation 	 loc1;
    HLocation 	 loc2;
    loc1.set(3,0,0,0);
    loc2.set(3,0,0,0);

    Int_t TrackL1[10], TrackR1[10];
    Int_t TrackL2[10], TrackR2[10];
    Int_t TrackTot[40], TrackDifferent[40];

    loc1[0] = pClusterSim->getSector1();
    loc1[1] = pClusterSim->getColumn1();
    loc1[2] = pClusterSim->getCell1();
    pHit1 = (HRpcHitSim*)pHitCat->getObject(loc1);
    pHit1->getTrackLArray(TrackL1);
    pHit1->getTrackRArray(TrackR1);
    for(Int_t i=0;i<10;i++) TrackL2[i]    = -999;
    for(Int_t i=0;i<10;i++) TrackR2[i]    = -999;

    if((pClusterSim->getClusterType())==2) {
	loc2[0] = pClusterSim->getSector2();
	loc2[1] = pClusterSim->getColumn2();
	loc2[2] = pClusterSim->getCell2();

	pHit2 = (HRpcHitSim*)pHitCat->getObject(loc2);
	pHit2->getTrackLArray(TrackL2);
	pHit2->getTrackRArray(TrackR2);
    }

    for(Int_t i=0;i<40;i++) TrackDifferent[i]=-999;

    for(Int_t i=0;i<10;i++) TrackTot[i]    = TrackL1[i];
    for(Int_t i=0;i<10;i++) TrackTot[i+10] = TrackR1[i];
    for(Int_t i=0;i<10;i++) TrackTot[i+20] = TrackL2[i];
    for(Int_t i=0;i<10;i++) TrackTot[i+30] = TrackR2[i];

    Int_t nTracks=0;

    for(Int_t i=0;i<40;i++) {
	if(TrackTot[i]<0) continue;
	Int_t count=0;
	for(Int_t j=0;j<40;j++) if(TrackDifferent[j]==TrackTot[i]) count++;
	if(count==0) {
	    TrackDifferent[nTracks] = TrackTot[i];
	    nTracks++;
	}
    }

    return nTracks;
}

Int_t HRpcClusterF::howManyTracksAtCells(HRpcClusterSim* pClusterSim)
{

    HRpcHitSim     *pHit1        = 0;
    HRpcHitSim     *pHit2        = 0;
    HLocation 	 loc1;
    HLocation 	 loc2;
    loc1.set(3,0,0,0);
    loc2.set(3,0,0,0);

    Int_t TrackLDgtr1[10], TrackRDgtr1[10];
    Int_t TrackLDgtr2[10], TrackRDgtr2[10];
    Int_t TrackTot[40], TrackDifferent[40];

    loc1[0] = pClusterSim->getSector1();
    loc1[1] = pClusterSim->getColumn1();
    loc1[2] = pClusterSim->getCell1();
    pHit1 = (HRpcHitSim*)pHitCat->getObject(loc1);
    pHit1->getTrackLDgtrArray(TrackLDgtr1);
    pHit1->getTrackRDgtrArray(TrackRDgtr1);
    for(Int_t i=0;i<10;i++) TrackLDgtr2[i]    = -999;
    for(Int_t i=0;i<10;i++) TrackRDgtr2[i]    = -999;

    if((pClusterSim->getClusterType())==2) {
	loc2[0] = pClusterSim->getSector2();
	loc2[1] = pClusterSim->getColumn2();
	loc2[2] = pClusterSim->getCell2();

	pHit2 = (HRpcHitSim*)pHitCat->getObject(loc2);
	pHit2->getTrackLDgtrArray(TrackLDgtr2);
	pHit2->getTrackRDgtrArray(TrackRDgtr2);
    }

    for(Int_t i=0;i<40;i++) TrackDifferent[i]=-999;

    for(Int_t i=0;i<10;i++) TrackTot[i]    = TrackLDgtr1[i];
    for(Int_t i=0;i<10;i++) TrackTot[i+10] = TrackRDgtr1[i];
    for(Int_t i=0;i<10;i++) TrackTot[i+20] = TrackLDgtr2[i];
    for(Int_t i=0;i<10;i++) TrackTot[i+30] = TrackRDgtr2[i];

    Int_t nTracks=0;

    for(Int_t i=0;i<40;i++) {
	if(TrackTot[i]<0) continue;
	Int_t count=0;
	for(Int_t j=0;j<40;j++) if(TrackDifferent[j]==TrackTot[i]) count++;
	if(count==0) {
	    TrackDifferent[nTracks] = TrackTot[i];
	    nTracks++;
	}
    }

    return nTracks;
}

ClassImp(HRpcClusterF)
