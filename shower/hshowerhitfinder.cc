#include "hshowerhit.h"
#include "hshowerhitsim.h"
#include "hshowerpid.h"
#include "hshowerhitheader.h"
#include "hshowerhitfinder.h"
#include "hshowercriterium.h"

#include "TArrayI.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "heventheader.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "hratreeext.h"
#include "hcategory.h"
#include "hmatrixcategory.h"
#include "hlinearcatiter.h"
#include "hlocation.h"
#include "hshowercal.h"
#include "hshowerhitfpar.h"
#include "hshowergeometry.h"
#include "hshowerpad.h"
#include "hiterator.h"
#include "hdebug.h"
#include "hades.h"
#include "hgeomvector.h"
#include "hgeomvector2.h"
#include "hspecgeompar.h"
#include "hgeomvolume.h"
#include "showerdef.h"
#include "hgeantdef.h"

#include <cmath>

using namespace std;
ClassImp(HShowerHitFinder)

    //_HADES_CLASS_DESCRIPTION
    ///////////////////////////////////////////////////////////////////////
    //HShowerHitFinder
    //
    // HShowerHitFinder reconstructor searches local maxima in CAL level
    // The Shower task is split into several tasks as shown
    // in the flow diagram below.
    //
    //   ----------------------
    //  |     HShowerUnpacker  |                                                                              //
    //  |   (embedding mode)   | \                                                                            //
    //  |                      |  \      ------------------                                                   //
    //   ----------------------   |     |  HGeantShower    |                                                  //
    //                            |      ------------------\                                                  //
    //                            |                         \                                                 //
    //                            |      ------------------  \------------->  ----------------------          //
    //                            |     |  HGeantWire      |   <------------ |  HShowerHitDigitizer |         //
    //                            |      ------------------\                  ----------------------          //
    //                            |                         \                                                 //
    //                 -------------     ------------------  \------------->  -----------------------         //
    //             -- | HShowerRaw  |   |  HShowerRawMatr  |   <------------ |  HShowerPadDigitizer  |        //
    //            |    -------------     ------------------\                 |( creates track objects|        //
    //            |                                         \                |  for real tracks in   |        //
    //   ----------------------          ------------------  \               |  embedding mode too)  |        //
    //  |   HShowerCalibrater  |        |  HShowerTrack    |  \<------------  -----------------------         //
    //  |   (embedding mode)   |         ------------------\   \                                              //
    //   ----------------------                             \   \             -----------------------         //
    //            |                      ------------------  \   ----------> |   HShowerCopy         |        //
    //             -------------------> |  HShowerCal      |  \<------------ |(add charge of real hit|        //
    //                                   ------------------\   \             | in embedding too )    |        //
    //                                                      \   \             -----------------------         //
    //                                   ------------------  ----\--------->  -----------------------         //
    //                                  |  HShowerHitHdr   |   <--\--------- |  HShowerHitFinder     |        //
    //                                   ------------------        \          -----------------------         //
    //                                   ------------------         \        |                                //
    //                                  |  HShowerPID      |   <-----\-------|                                //
    //                                   ------------------           \      |                                //
    //                                   ------------------            \     |                                //
    //                                  |  HShowerHit      |   <--------\----|                                //
    //                                   ------------------ <            \                                    //
    //                                                       \            \                                   //
    //                                                        \-------------> ------------------------        //
    //                                                                       | HShowerHitTrackMatcher |       //
    //                                                                        ------------------------        //
    //
    //
    //  In the case of TRACK EMBEDDING of simulated tracks into
    //  experimental data the real data are written by the HShowerUnpacker into
    //  HShowerRaw category. The real hits are taken into
    //  account by the digitizer (adding of charges). The embedding mode is recognized
    //  automatically by analyzing the
    //  gHades->getEmbeddingMode() flag.
    //            Mode ==0 means no embedding
    //                 ==1 realistic embedding (first real or sim hit makes the game)
    //                 ==2 keep GEANT tracks   (as 1, but GEANT track numbers will always
    //                     win against real data. besides the tracknumber the output will
    //                     be the same as in 1)
    //
    //////////////////////////////////////////////////////////////////////

    HShowerHitFinder::HShowerHitFinder(const Text_t *name,const Text_t *title) :
HReconstructor(name,title)
{
    fIter = NULL;
    m_Loc.set(4, 0, 0, 0, 0);
    setFillPID();
    setSortFlag(kTRUE);
    m_pCriterium = new HShowerCriterium;


}

HShowerHitFinder::HShowerHitFinder()
{
    fIter = NULL;
    m_Loc.set(4, 0, 0, 0, 0);
    setFillPID();
    setSortFlag(kTRUE);
    m_pCriterium = new HShowerCriterium;
}


HShowerHitFinder::~HShowerHitFinder(void) {
    if (m_pCellArr) {
	delete m_pCellArr;
    }
    if (fIter) delete fIter;
    if (m_pCriterium) delete m_pCriterium;
}

Bool_t HShowerHitFinder::init()
{

    if(gHades->getCurrentEvent()->getCategory(catGeantKine)) isSim = kTRUE ;
    else                                                     isSim = kFALSE;


    TArrayI arr(4);
    TArrayI arr1(1);
    HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()->getDetector("Shower");

    arr[0] = pShowerDet->getMaxSectors();
    arr[1] = pShowerDet->getMaxModules();
    arr[2] = pShowerDet->getRows();
    arr[3] = pShowerDet->getColumns();
    arr1[0] = arr[0] * arr[1] * arr[2] * arr[3];

    m_pCalCat=gHades->getCurrentEvent()->getCategory(catShowerCal);
    if (m_pCalCat) {
	fIter = (HIterator*)getCalCat()->MakeIterator("native");
    }


    m_pHitCat=gHades->getCurrentEvent()->getCategory(catShowerHit);
    if (!m_pHitCat) {

	if(isSim) m_pHitCat = pShowerDet->buildLinearCat("HShowerHitSim");
        else      m_pHitCat = pShowerDet->buildLinearCat("HShowerHit");

	if (!m_pHitCat) return kFALSE;
	else gHades->getCurrentEvent()->addCategory(catShowerHit, m_pHitCat, "Shower");
    }

    m_pHitHdrCat=gHades->getCurrentEvent()->getCategory(catShowerHitHdr);
    if (!m_pHitHdrCat) {
	m_pHitHdrCat=pShowerDet->buildCategory(catShowerHitHdr);

	if (!m_pHitHdrCat) return kFALSE;
	else gHades->getCurrentEvent()->addCategory(catShowerHitHdr, m_pHitHdrCat, "Shower");
    }

    if (IsFillPID()) {
	m_pPIDCat=gHades->getCurrentEvent()->getCategory(catShowerPID);
	if (!m_pPIDCat) {
	    m_pPIDCat=pShowerDet->buildCategory(catShowerPID);

	    if (!m_pPIDCat) return kFALSE;
	    else gHades->getCurrentEvent()->addCategory(catShowerPID, m_pPIDCat, "Shower");
	}
    }
    else m_pPIDCat = NULL;

    m_pCellArr = new HRaTreeExt(m_pCalCat, &arr);
   

    return initParameters();
}

Bool_t HShowerHitFinder::initParameters()
{
    HRuntimeDb* rtdb=gHades->getRuntimeDb();

    m_pGeometry = (HShowerGeometry*)rtdb->getContainer("ShowerGeometry");
    if(!m_pGeometry) return kFALSE;

    m_pHitFPar = (HShowerHitFPar*)rtdb->getContainer("ShowerHitFPar");
    if (!m_pHitFPar) return kFALSE;
    m_pCriterium->setParams(m_pHitFPar);

    m_pHSpecGeomPar = (HSpecGeomPar *)rtdb->getContainer("SpecGeomPar");
    if(!m_pHSpecGeomPar) return kFALSE;

    return kTRUE;
}

void HShowerHitFinder::setCriterium(HShowerCriterium* pCrit) {
    if (m_pCriterium) delete m_pCriterium;
    m_pCriterium = pCrit;
}

Bool_t HShowerHitFinder::finalize() {
    return kTRUE;
}

HShowerHitFinder& HShowerHitFinder::operator=(HShowerHitFinder &c) {

    return c;
}

Int_t HShowerHitFinder::execute()
{

    if(!fIter) return 0; // no input

    HShowerCal *pCal;
    m_pCellArr->update();

    Int_t n = 0;
    Int_t hits = 0;

    fIter->Reset();
    while((pCal = (HShowerCal *)fIter->Next()))
    {
	m_Loc[0] = pCal->getSector();
	m_Loc[1] = pCal->getModule();
	m_Loc[2] = pCal->getRow();
	m_Loc[3] = pCal->getCol();

	if (lookForHit(pCal, m_Loc)) hits++;
	n++;
    }

    if (IsSortFlagSet()) {
	m_pHitCat->sort();
	if(m_pPIDCat) m_pPIDCat->sort();
    }

    return 0;
}

Bool_t HShowerHitFinder::lookForHit(HShowerCal* cal, HLocation& fLoc) {
#if DEBUG_LEVEL>2
    gDebuger->enterFunc("HShowerHitFinder::execute");
    gDebuger->message("Cal cat points to %p",cal);
#endif
    HShowerHit *hit  = NULL;
    HShowerPID *pid  = NULL;
    Bool_t isHit = kFALSE;

    if (cal && isLocalMax(fLoc)) {
	Float_t fCharge, fShower = 0.0;
	Int_t ret = 0;


	hit=(HShowerHit *)m_pHitCat->getNewSlot(fLoc);
	if (hit) {

	    if(isSim){
		hit = new (hit) HShowerHitSim;
	    } else {
		hit = new (hit) HShowerHit;
	    }
	    fillSums(hit, fLoc);
	    //calcCoord(hit, fLoc);
	    calcCoordWithSigma(hit, fLoc,1);

	    fCharge = cal->getCharge();

	    hit->setCharge(fCharge);
	    hit->setSector(cal->getSector());
	    hit->setModule(cal->getModule());
	    hit->setRow(cal->getRow());
	    hit->setCol(cal->getCol());

	    if (IsSortFlagSet()) hit->calcAddress();
	    hit->copyToTrueAddress();  // later used in low efficiency mode!

	    fShower = m_pCriterium->showerCriterium(hit, ret,m_pHitFPar);

	    hit->setShower(fShower);

	    if (m_pPIDCat && fShower > 0) {
		pid=(HShowerPID *)m_pPIDCat->getNewSlot(fLoc);
		if (pid) {
		    pid=new(pid) HShowerPID;
		    fillPID(hit, pid);
		}
	    }

	    isHit = kTRUE;
	    updateLocalMax(fLoc);
	}
    }
#if DEBUG_LEVEL>2
    gDebuger->leaveFunc("HShowerHitFinder::execute");
#endif
    updateFiredCells(fLoc);
    return isHit;
}

void HShowerHitFinder::fillPID(HShowerHit* hit, HShowerPID* pid) {
    Float_t fX, fY, fZ;
    Float_t fR, fPhi, fTheta;

    pid->setCharge(hit->getCharge());
    pid->setSector(hit->getSector());
    pid->setModule(hit->getModule());
    pid->setRow(hit->getRow());
    pid->setCol(hit->getCol());
    pid->setAddress(hit->getAddress());
    pid->setShower(hit->getShower());

    hit->getLabXYZ(&fX, &fY, &fZ);
    pid->setXYZ(fX, fY, fZ);
    hit->getSphereCoord(&fR, &fPhi, &fTheta);
    pid->setSphereCoord(fR, fPhi, fTheta);

    //   if (IsSortFlagSet())
    //       getPIDCat()->sort();
}

void HShowerHitFinder::fillSums(HShowerHit* hit, HLocation &fLoc) {
    HLocation fTmpLoc;
    Int_t nModules = m_pHitFPar->getModules();
    Float_t sum, var;
    Int_t cs;

    fTmpLoc = fLoc;
    for(fTmpLoc[1] = 0; fTmpLoc[1] < nModules; fTmpLoc[1]++) {
	sum = calculateSum(fTmpLoc, 1, &cs);
	var = calculateVar(fTmpLoc, 1, sum/9.0);
	hit->setSum(fTmpLoc[1], sum);
	hit->setVar(fTmpLoc[1], var);
	hit->setClusterSize(fTmpLoc[1], cs);
    }

    calculateSum(fLoc, 1, &cs);
    if (cs > 1) updateClusters(fLoc);

    if (nModules == 3) {
	fTmpLoc[1] = 2;
	hit->setSum25(calculateSum(fTmpLoc, 2));
    }

    hit->updateCalc();
}

void HShowerHitFinder::calcCoord(HShowerHit* hit, HLocation &fLoc) {
    HGeomVector vLocalCoord;
    HGeomVector2 vLabCoord;

    HGeomVector targetPos = m_pHSpecGeomPar->getTarget(0)->getTransform().getTransVector();

    m_pGeometry->getLocalCoord(fLoc, vLocalCoord);
    m_pGeometry->getLabCoord(fLoc, vLabCoord);
    m_pGeometry->getSphereCoord(fLoc, vLabCoord, &targetPos);
    hit->setXY(vLocalCoord.getX(), vLocalCoord.getY());
    hit->setLabXYZ(vLabCoord.getX(), vLabCoord.getY(), vLabCoord.getZ());
    hit->setSphereCoord(vLabCoord.getRad(), vLabCoord.getPhi(), vLabCoord.getTheta());
}

////////////////////////////////////////////////////////////////////////////
//
//			 		HShowerHitFinder::calcCoordWithSigma
// 		Calculate the Position of the hit and SigmaX,SigmaY.
//      SigmaX,SigmaY are determined based on the geometry of the pad.
//  	Weighted mean value for X,Y position determination is used.
//      Procedure:
//		1.) calculate weighted X_Loc, Y_Loc in local coord. system
//		2.) determine Sigma_X, Sigma_Y (based on geometry of the pad)
//		3.) transform X_Loc, Y_Loc -> X_Lab, Y_Lab, Z_Lab
// 		4.) transform X_Lab, Y_Lab, Z_Lab -> Sphere Coord. Sys.
//
///////////////////////////////////////////////////////////////////////////
void HShowerHitFinder::calcCoordWithSigma(HShowerHit* hit, HLocation &fLoc, Int_t nRange) {
    HGeomVector vLocalCoord;

    HShowerPadTab *pPads = m_pGeometry->getPadParam(fLoc[1]); // Access to the pad plane (module== fLoc[1])

    Int_t rowL, rowU, colL, colR,iRow1, iRow2, iCol1, iCol2;

    Int_t row = fLoc[2];
    Int_t col = fLoc[3];
    // Check pads around Loc_Max
    m_pHitFPar->getRowBord(fLoc[0], fLoc[1], &rowL, &rowU);
    m_pHitFPar->getColBord(fLoc[0], fLoc[1], &colL, &colR);
    if((row < rowL) || (row > rowU) || (col < colL) || (col > colR))
	cout<<"--We are outside of the detector. "<<endl;
    //return 0.0f;

    if((iRow1 = row - nRange) < rowL)
	iRow1 = rowL;

    if((iRow2 = row + nRange) > rowU)
	iRow2 = rowU;

    if((iCol1 = col - nRange) < colL)
	iCol1 = colL;

    if((iCol2 = col + nRange) > colR)
	iCol2 = colR;

    Float_t fSum = 0;
    Float_t fWSumX_Loc = 0;
    Float_t fWSumY_Loc = 0;
    Float_t sigma_X_tmp = 0;
    Float_t sigma_X = 0;
    Float_t sigma_Y_tmp = 0;
    Float_t sigma_Y = 0;

    HLocation fTmpLoc;
    fTmpLoc = fLoc;

    HShowerCal* pCell;
    /////////////////////////////////////////////////////////////
    // actual calculation of weighted_X_Loc and weighted_Y_Loc //
    /////////////////////////////////////////////////////////////
    for(fTmpLoc[2] = iRow1; fTmpLoc[2] <= iRow2; fTmpLoc[2]++)
	for(fTmpLoc[3] = iCol1; fTmpLoc[3] <= iCol2; fTmpLoc[3]++)
	{
	    pCell =  (HShowerCal*)m_pCellArr->getObject(fTmpLoc);
	    if((float)pCell->getCharge()<=0.0) continue;

	    // position of the pad in Local coord. sys.
	    m_pGeometry->getLocalCoord(fTmpLoc, vLocalCoord); //position in Local Sys.

	    fSum  = fSum +  pCell->getCharge();
	    fWSumX_Loc = fWSumX_Loc +  ((pCell->getCharge()) * (vLocalCoord.getX()) ); //weighted X_Loc
	    fWSumY_Loc = fWSumY_Loc +  ((pCell->getCharge()) * (vLocalCoord.getY()) ); //weighted Y_Loc

	    // Determine Sigma_X (geometry), pad_size/sqrt(12), sqrt(12)-standard deviation of a uniform density
	    Float_t fXLD = pPads->getPad(fTmpLoc[2],fTmpLoc[3])->getXld_mm();
	    Float_t fXRD = pPads->getPad(fTmpLoc[2],fTmpLoc[3])->getXrd_mm();
	    Float_t fXLU = pPads->getPad(fTmpLoc[2],fTmpLoc[3])->getXlu_mm();
	    Float_t fXRU = pPads->getPad(fTmpLoc[2],fTmpLoc[3])->getXru_mm();

	    Float_t x1=fabs(fXLU-fXRD);
	    Float_t x2=fabs(fXLD-fXRU);
	    sigma_X_tmp = (x1>x2)? x1:x2;

	    sigma_X_tmp = sigma_X_tmp/sqrt(12.);
	    sigma_X_tmp = ((float)pCell->getCharge()) * sigma_X_tmp;
	    sigma_X_tmp = sigma_X_tmp * sigma_X_tmp;  // so I have Q(i)^2 * Sigma(i)^2
	    sigma_X =  sigma_X + sigma_X_tmp;  // and here: Q(i)^2 * Sigma(i)^2 + Q(j)^2 * Sigma(j)^2

	    // Determine Sigma_Y (geometry)
	    Float_t fYLD = pPads->getPad(fTmpLoc[2],fTmpLoc[3])->getYld_mm();
	    Float_t fYRD = pPads->getPad(fTmpLoc[2],fTmpLoc[3])->getYrd_mm();
	    Float_t fYLU = pPads->getPad(fTmpLoc[2],fTmpLoc[3])->getYlu_mm();
	    Float_t fYRU = pPads->getPad(fTmpLoc[2],fTmpLoc[3])->getYru_mm();

	    x1=fabs(fYLU-fYRD);
	    x2=fabs(fYLD-fYRU);
	    sigma_Y_tmp = (x1>x2)? x1:x2;

	    sigma_Y_tmp = sigma_Y_tmp/sqrt(12.);
	    sigma_Y_tmp = ((float)pCell->getCharge()) * sigma_Y_tmp;
	    sigma_Y_tmp = sigma_Y_tmp * sigma_Y_tmp;  // so I have Q(i)^2 * Sigma(i)^2
	    sigma_Y =  sigma_Y + sigma_Y_tmp;  //and here: Q(i)^2 * Sigma(i)^2 + Q(j)^2 * Sigma(j)^2
	}
    if(fSum<=0.0) cout<<"--Shw_Hit_F-- Chagrge = 0, should not happen"<<endl;
    // transform X_Loc,Y_Loc to Lab. coord. system
    HGeomVector vv; 		// hit pos. in local coord. system
    HGeomVector2 vvv; 	// hit pos. in lab. coord. syss.
    vv.setX(fWSumX_Loc/fSum);
    vv.setY(fWSumY_Loc/fSum);
    vv.setZ(0.0);

    // get Lab position of the hit (vvv) from Local position (vv)
    m_pGeometry->transVectToLab(fLoc, vv, vvv);

    hit->setXY((float)fWSumX_Loc/fSum,(float)fWSumY_Loc/fSum ); //  Hit in Local System.
    hit->setSigmaXY(sqrt(sigma_X/(fSum*fSum)),sqrt(sigma_Y/(fSum*fSum)));
    hit->setLabXYZ(vvv.getX(), vvv.getY(), vvv.getZ());  // Hit in Lab. Sysstem

    HGeomVector2 sphereOut; // Sphere coordin.
    HGeomVector targetPos = m_pHSpecGeomPar->getTarget(0)->getTransform().getTransVector();
    m_pGeometry->transLabToSphereCoord(vvv, sphereOut, &targetPos);
    hit->setSphereCoord(sphereOut.getRad(), sphereOut.getPhi(), sphereOut.getTheta());

    /*      if(0){ // create control file
     Int_t nEvt=gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(); // get Event Seq. Nb.
     ofstream out1("results.txt", ios::app);

     HGeomVector vLocalCoord_Tmp;
     m_pGeometry->getLocalCoord(fLoc, vLocalCoord_Tmp);
     HGeomVector2 vLabCoord;
     m_pGeometry->getSphereCoord(fLoc, vLabCoord, &targetPos);
     out1<<" evnt_Nb "<<nEvt<<" Sec "<<fLoc[0]<<" mod "<<fLoc[1]<<" cluSize "
     //cout<<" evnt_Nb "<<nEvt<<" Sec "<<fLoc[0]<<" mod "<<fLoc[1]<<" cluSize "
     <<hit->getClusterSize(fLoc[1])<<
     " old_X "<<(float)(vLocalCoord_Tmp.getX())<<" new_X "<< (float)fWSumX_Loc/fSum<<
     " old_Y "<<(float)(vLocalCoord_Tmp.getY())<<" new_Y "<< ((float)fWSumY_Loc/fSum)<<
     " sig_X "<<hit->getSigmaX()<<" sigY "<<hit->getSigmaY()<<
     //" sig_X "<<sqrt(sigma_X/(fSum*fSum))<<" sigY "<<sqrt(sigma_Y/(fSum*fSum))<<
     " X_Lab "<<vvv.getX()<<" Y_Lab "<<vvv.getY()<<" Z_Lab "<< vvv.getZ()<<
     " Rad_old "<<vLabCoord.getRad()<<" Rad_new "<<sphereOut.getRad()<<
     " Phi_old "<<vLabCoord.getPhi()<<" Phi_new "<<sphereOut.getPhi()<<
     " Theta_old "<<vLabCoord.getTheta()<<" Theta_new "<<sphereOut.getTheta()<<
     endl;
     out1.close();
     }
     */
}

Bool_t HShowerHitFinder::isLocalMax(HLocation &fLoc) {
    HShowerCal* pCell;
    HShowerCal* pRefCell = (HShowerCal*)m_pCellArr->getObject(fLoc);
    Float_t charge, charge1;
    Int_t rowL, rowU, colL, colR, iRow1, iRow2, iCol1, iCol2;;

    Int_t row = fLoc[2];
    Int_t col = fLoc[3];
    HLocation fTmpLoc;

    m_pHitFPar->getRowBord(fLoc[0], fLoc[1], &rowL, &rowU);
    m_pHitFPar->getColBord(fLoc[0], fLoc[1], &colL, &colR);
    if((row < rowL) || (row > rowU) || (col < colL) || (col > colR))
	return 0;

    if((iRow1 = row - 1) < rowL)
	iRow1 = rowL;

    if((iRow2 = row + 1) > rowU)
	iRow2 = rowU;

    if((iCol1 = col - 1) < colL)
	iCol1 = colL;

    if((iCol2 = col + 1) > colR)
	iCol2 = colR;


    fTmpLoc = fLoc;

    charge = pRefCell->getCharge();

    for(fTmpLoc[2] = iRow1; fTmpLoc[2] <= iRow2; fTmpLoc[2]++)
	for(fTmpLoc[3] = iCol1; fTmpLoc[3] <= iCol2; fTmpLoc[3]++)
	{
	    if((fTmpLoc[2] == row) && (fTmpLoc[3] == col))
		continue;

	    pCell =  (HShowerCal*)m_pCellArr->getObject(fTmpLoc);

	    if (pCell->isLocalMax() != 0)
		return 0;

	    charge1 =  pCell->getCharge();
	    if(charge < charge1)
		return 0;

	}

    pRefCell->setLocalMax();
    return 1;
}

Float_t HShowerHitFinder::calculateVar(HLocation &fLoc, Int_t nRange, Float_t avg)
{
    Int_t rowL, rowU, colL, colR, iRow1, iRow2, iCol1, iCol2;

    Int_t row = fLoc[2];
    Int_t col = fLoc[3];
    m_pHitFPar->getRowBord(fLoc[0], fLoc[1], &rowL, &rowU);
    m_pHitFPar->getColBord(fLoc[0], fLoc[1], &colL, &colR);

    if((row < rowL) || (row > rowU) || (col < colL) || (col > colR))
	return 0.0f;

    if((iRow1 = row - nRange) < rowL)
	iRow1 = rowL;

    if((iRow2 = row + nRange) > rowU)
	iRow2 = rowU;

    if((iCol1 = col - nRange) < colL)
	iCol1 = colL;

    if((iCol2 = col + nRange) > colR)
	iCol2 = colR;

    Float_t fDiff = 0.0, fVar = 0.0;
    HLocation fTmpLoc;
    fTmpLoc = fLoc;

    HShowerCal* pCell;

    //  for(fTmpLoc[2] = row - nRange; fTmpLoc[2] <= row + nRange; fTmpLoc[2]++)
    //    for(fTmpLoc[3] = col - nRange; fTmpLoc[3] <= col + nRange; fTmpLoc[3]++)
    for(fTmpLoc[2] = iRow1; fTmpLoc[2] <= iRow2; fTmpLoc[2]++)
	for(fTmpLoc[3] = iCol1; fTmpLoc[3] <= iCol2; fTmpLoc[3]++)
	{
	    pCell =  (HShowerCal*)m_pCellArr->getObject(fTmpLoc);
	    fDiff +=  (pCell->getCharge() - avg);
	    fVar += fDiff * fDiff;
	}

    return fVar;
}

Float_t HShowerHitFinder::calculateSum(HLocation &fLoc, Int_t nRange, Int_t* pncs)
{
    Int_t nThreshold = m_pHitFPar->getThreshold();
    Int_t rowL, rowU, colL, colR,iRow1, iRow2, iCol1, iCol2;

    Int_t row = fLoc[2];
    Int_t col = fLoc[3];
    m_pHitFPar->getRowBord(fLoc[0], fLoc[1], &rowL, &rowU);
    m_pHitFPar->getColBord(fLoc[0], fLoc[1], &colL, &colR);

    if((row < rowL) || (row > rowU) || (col < colL) || (col > colR))
	return 0.0f;

    if((iRow1 = row - nRange) < rowL)
	iRow1 = rowL;

    if((iRow2 = row + nRange) > rowU)
	iRow2 = rowU;

    if((iCol1 = col - nRange) < colL)
	iCol1 = colL;

    if((iCol2 = col + nRange) > colR)
	iCol2 = colR;

    Float_t fSum = 0;
    HLocation fTmpLoc;
    fTmpLoc = fLoc;

    Int_t cs = 0;
    HShowerCal* pCell;

    //  for(fTmpLoc[2] = row - nRange; fTmpLoc[2] <= row + nRange; fTmpLoc[2]++)
    //   for(fTmpLoc[3] = col - nRange; fTmpLoc[3] <= col + nRange; fTmpLoc[3]++)
    for(fTmpLoc[2] = iRow1; fTmpLoc[2] <= iRow2; fTmpLoc[2]++)
	for(fTmpLoc[3] = iCol1; fTmpLoc[3] <= iCol2; fTmpLoc[3]++)
	{
	    pCell =  (HShowerCal*)m_pCellArr->getObject(fTmpLoc);
	    fSum +=  pCell->getCharge();
	    if (pCell->getCharge() > nThreshold)
		cs++;
	}

    if (pncs)
	*pncs = cs;

    return fSum;
}

HShowerHitHeader* HShowerHitFinder::getHitHeader(HLocation &fLoc) {
    HLocation fTmpLoc;

    fTmpLoc.set(2, fLoc.getIndex(0), fLoc.getIndex(1));
    fTmpLoc[0] = fLoc[0];
    fTmpLoc[1] = fLoc[1];

    HShowerHitHeader *pHitHdr = (HShowerHitHeader*)
	((HMatrixCategory*)m_pHitHdrCat)->getObject(fTmpLoc);
    if (!pHitHdr) {
	pHitHdr = (HShowerHitHeader*)((HMatrixCategory*)m_pHitHdrCat)
	    ->getSlot(fTmpLoc);
	if (pHitHdr != NULL) {
	    pHitHdr = new(pHitHdr) HShowerHitHeader;
	    pHitHdr->setSector(fTmpLoc[0]);
	    pHitHdr->setModule(fTmpLoc[1]);
	}

    }

    return pHitHdr;
}

void HShowerHitFinder::updateClusters(HLocation &fLoc) {
    HShowerHitHeader *pHitHdr = getHitHeader(fLoc);
    if (pHitHdr)
	pHitHdr->incClusters();
}

void HShowerHitFinder::updateLocalMax(HLocation &fLoc) {
    HShowerHitHeader *pHitHdr = getHitHeader(fLoc);
    if (pHitHdr)
	pHitHdr->incLocalMax();
}

void HShowerHitFinder::updateFiredCells(HLocation &fLoc) {
    HShowerHitHeader *pHitHdr = getHitHeader(fLoc);
    if (pHitHdr)
	pHitHdr->incFiredCells();
}


