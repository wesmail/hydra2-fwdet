//*-- Author   : Pablo Cabanelas
//*-- Created  : 30/08/07
//*-- Modified : 18/12/07 Diego Gonzalez-Diaz
//*-- Modified : 13/12/09 Diego Gonzalez-Diaz Removed hard-coded
//*-- Modified :          parameters.
//*-- Modified : 03/07/12 Alessio Mangiarotti Added filling of Theta,
//*-- Modified :          Phi and InsideCellFlag for simulation.
//*-- Modified : 19/07/12 Alessio Mangiarotti General code cleanup.
//*-- Modified :          Geometry parameters used in several places
//*-- Modified :          are now stored as local variables (see D,
//*-- Modified :          HD, HDT, XL, W). Some fixed values have
//*-- Modified :          been introduced as constants (see rad2deg,
//*-- Modified :          ovang, invsqrt12). Namespace TMath used
//*-- Modified :          consistently everywhere. The different
//*-- Modified :          processing necessary for simulation and
//*-- Modified :          data has been merged in a single place.
//*-- Modified :          The variable xMod is now calculated in the
//*-- Modified :          same way for data and simulation.
//*-- Modified : 22/04/13 Georgy Kornakov. Correction of the time
//*-- Modified :          uncertainty calculated from RpcDigiPar,
//*-- Modified :          suggested by Alessio.
//*-- Modified : 28/08/13 Georgy Kornakov Added the new Slewing and
//*-- Modified :          Time-position (walking) correction. The
//*-- Modified :          parameters are stored in the HRpcTimePosPar
//*-- Modified :          and HRpcSlewingPar objects. The previous
//*-- Modified :          method was removed. The new time-charge
//*-- Modified :          correction uses two exponentials (3 params
//*-- Modified :          each) and a qcut to set the crossing point
//*-- Modified :          of the two behaviours. The position-time
//*-- Modified :          dependence correction uses a coposition
//*-- Modified :          of 5 sinusoidal terms (15 params) to
//*-- Modified :          compensate the effect.
//*-- Modified : 04/12/13 G.Kornakov Added RPC embedding mode.
//*-- Modified :          TODO: The Hit finder could be substantially
//*-- Modified :          simplified if the execute code is introduced
//*-- Modified :          in functions as fillHits(), calculateRealCorrections()
//*-- Modified :          calculateSimCorrection(), mergeHits()...
//*-- Modified :          That would improve the understanding
//*-- Modified :          of the code.
//*-- Modified :
//
//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////
//
//  HRpcHitF: RPC detector hit finder
//
//  Gets calibrated data from RpcCal(Sim) and writes to RpcHit(Sim)
//  In case of embedding mode, creates RpcHitSim from temporal category.
//  At the end of the procedure both are merged.
//
///////////////////////////////////////////////////////////////////

using namespace std;
#include "TRandom.h"
#include <time.h>
#include "hrpcdetector.h"
#include "hrpchitf.h"
#include "rpcdef.h"
#include "hrpccal.h"
#include "hrpccalsim.h"
#include "hrpchitsim.h"
#include "hgeantrpc.h"
#include "hrpcgeomcellpar.h"
#include "hrpcgeompar.h"
#include "hrpccellstatuspar.h"
#include "hrpcdigipar.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hspecgeompar.h"
#include "hdetector.h"
#include "hevent.h"
#include "hgeantkine.h"
#include "hcategory.h"
#include "hstart2detector.h"
#include "hstart2hit.h"
#include "hstartdef.h"
#include "hgeomvector.h"
#include "hgeomvector2.h"
#include "hgeomvolume.h"
#include "hgeomtransform.h"
#include "hrpcwtoqpar.h"
#include "hrpchitfpar.h"
#include "hrpctimepospar.h"
#include "hrpcslewingpar.h"

#include <iostream>
#include <iomanip>

void HRpcHitF::initParContainer() {
  pRpcGeometry   = (HRpcGeomPar*)(gHades->getRuntimeDb()->getContainer("RpcGeomPar"));
  pGeomCellPar   = (HRpcGeomCellPar*)(gHades->getRuntimeDb()->getContainer("RpcGeomCellPar"));
  pCellStatusPar = (HRpcCellStatusPar*)(gHades->getRuntimeDb()->getContainer("RpcCellStatusPar"));
  pDigiPar       = (HRpcDigiPar*)(gHades->getRuntimeDb()->getContainer("RpcDigiPar"));
  pWtoQPar       = (HRpcWtoQPar*)(gHades->getRuntimeDb()->getContainer("RpcWtoQPar"));
  pHitFPar       = (HRpcHitFPar*)(gHades->getRuntimeDb()->getContainer("RpcHitFPar"));
  pSpecGeomPar   = (HSpecGeomPar*)(gHades->getRuntimeDb()->getContainer("SpecGeomPar"));
  pTimePosPar    = (HRpcTimePosPar*)(gHades->getRuntimeDb()->getContainer("RpcTimePosPar"));
  pSlewingPar    = (HRpcSlewingPar*)(gHades->getRuntimeDb()->getContainer("RpcSlewingPar"));
}

HRpcHitF::HRpcHitF(void)
{
  pCalCat        = NULL;
  pHitCat        = NULL;
  iter           = NULL;
  pRpcGeometry   = NULL;
  pGeomCellPar   = NULL;
  pCellStatusPar = NULL;
  pDigiPar       = NULL;
  pWtoQPar       = NULL;
  pHitFPar       = NULL;
  pStartHitCat   = NULL;
  pTimePosPar    = NULL;
  pSlewingPar    = NULL;
}

HRpcHitF::HRpcHitF(const Text_t *name, const Text_t *title) :
HReconstructor(name,title)
{
  pCalCat        = NULL;
  pHitCat        = NULL;
  iter           = NULL;
  pRpcGeometry   = NULL;
  pGeomCellPar   = NULL;
  pCellStatusPar = NULL;
  pDigiPar       = NULL;
  pWtoQPar       = NULL;
  pHitFPar       = NULL;
  pStartHitCat   = NULL;
  pTimePosPar    = NULL;
  pSlewingPar    = NULL;
}

HRpcHitF::~HRpcHitF(void)
{
  if (iter) delete iter;
  iter=NULL;
}

Bool_t HRpcHitF::init(void)
{

  initParContainer();

  if (!pRpcGeometry){
    Error("init","No RPC Geometry!!");
    return kFALSE;
  }
  if (!pGeomCellPar){
    Error("init","No RpcGeomCellPar Parameters");
    return kFALSE;
  }
  if (!pCellStatusPar){
    Error("init","No RpcCellStatusPar Parameters");
    return kFALSE;
  }
  if (!pDigiPar){
    Error("init","No RpcDigiPar Parameters");
    return kFALSE;
  }
  if (!pWtoQPar){
    Error("init","No RpcWtoQPar Parameters");
    return kFALSE;
  }
  if (!pHitFPar){
    Error("init","No RpcHitFPar Parameters");
    return kFALSE;
  }
  if (!pSpecGeomPar){
    Error("init","No SpecGeomPar Parameters");
    return kFALSE;
  }

  if(!pTimePosPar) {
    Error("init","No RpcTimePosPar Parameters");
    return kFALSE;
  }

  if(!pSlewingPar) {
    Error("init","No RpcSlewingPar Parameters");
    return kFALSE;
  }


  HRpcDetector *rpc;
  rpc=(HRpcDetector *)gHades->getSetup()->getDetector("Rpc");

  if(!rpc){
    Error("init","No Rpc Detector found");
    return kFALSE;
  }


  pStartHitCat=gHades->getCurrentEvent()->getCategory(catStart2Hit);
  if (!pStartHitCat) Warning("init","Start hit level not defined; setting start time to 0");

  // Create category for calibrated data
  pCalCat=gHades->getCurrentEvent()->getCategory(catRpcCal);
  if (!pCalCat) {
    Error("init","No RpcCal Category");
    return kFALSE;
  }
  // Create category for embedded data
  if(gHades->getEmbeddingMode()>0) {
    pCalCatTmp = gHades->getCurrentEvent()->getCategory(catRpcCalTmp);
    if (!pCalCatTmp) {
      Error("init","No RpcCalTmp Category for embedding, whereas EmbeddingMode Flag ACTIVE!");
      return kFALSE;
    }
  }

  // Decide whether we are running for simulation or real data  (also useful for embedding mode!!!)
  HCategory* catKin=gHades->getCurrentEvent()->getCategory(catGeantKine);

  if(catKin) simulation=kTRUE;
  else       simulation=kFALSE;

  // Create category for hit data
  pHitCat=gHades->getCurrentEvent()->getCategory(catRpcHit);

  if (!pHitCat) {

    if (simulation)
      pHitCat=rpc->buildMatrixCategory("HRpcHitSim",0.5);
    else   pHitCat=rpc->buildMatrixCategory("HRpcHit",0.5);

    gHades->getCurrentEvent()->addCategory(catRpcHit,pHitCat,"Rpc");
  }

  // This is not used at all!! Can be removed
  if( simulation ) {
    pGeantRpcCat = gHades->getCurrentEvent()->getCategory(catRpcGeantRaw);
    if (!pGeantRpcCat) {
      Error("HRpcHitF::init()","No HGeant RPC Category");
      return kFALSE;
    }
  }

  iter=(HIterator *)pCalCat->MakeIterator();
  loc.set(3,0,0,0);

  return kTRUE;
}

Int_t HRpcHitF::execute(void)
{
  Float_t tof, charge, xCell, yCell, xSec, ySec, zSec, xMod, yMod, zMod, xLab, yLab, zLab;//[ns], [pC],[mm]
  Float_t D,HD,HDT,XL,W;
  Float_t startTime; //[ns]
  Float_t startTimeSmearing; //[ns]
  Float_t leftT, rightT;
  const Float_t rad2deg = 180./TMath::Pi();
  const Float_t ovang=TMath::Tan(24.5/rad2deg);
  const Float_t invsqrt12=1./TMath::Sqrt(12.);
  Float_t theta, phi; //[degrees]
  Float_t sigma_X, sigma_Y, sigma_Z, sigma_T;
  Float_t vprop, DPlanes; //[mm/ns], [mm], [mm]
  Int_t trackL[10], trackR[10], trackLDgtr[10], trackRDgtr[10];
  Bool_t isLMotherAtBox[10], isRMotherAtBox[10];

  // New RpcDigiPar params.
  // sigma_T = sigma0_T+sigma1_T/(1.+TMath::Exp(-sigma2_T*(1.-sigma3_T))); beta = 1, the worst possible case.
  Float_t sigma0_T, sigma1_T, sigma2_T, sigma3_T;
  sigma0_T    = (pDigiPar->getSigmaT())  / 1000.;                      //[ns]
  sigma1_T    = (pDigiPar->getSigmaT1()) / 1000.;                      //[ns]
  sigma2_T    = (pDigiPar->getSigmaT2());                              //pure number
  sigma3_T    = (pDigiPar->getSigmaT3());                              //pure number

  HRpcCalSim *pCal       = NULL; //Only reading, use the same class for HRpcCalSim and HRpcCal objects
  HRpcHitSim *pHitSim    = NULL; //Reading and STORING in category, use 2 different classes.
  HRpcHit    *pHit       = NULL; //Reading and STORING in category, use 2 different classes.
  HStart2Hit *pStartH    = NULL; //Start Hit object

  HGeomVector  rRpcInMod;
  HGeomVector2 rRpcInLab,rRpcInSec;
  //HGeomVector rRpcInLab;

  Float_t fQcut = pHitFPar->getQcut();

  //Getting start time. Only for real data!
  startTime         = 0.0;
  startTimeSmearing = 0.0;

  if (pStartHitCat) {
      if((pStartH = (HStart2Hit *) pStartHitCat->getObject(0)) != NULL) {
	    startTime = pStartH->getTime();
	    if(pStartH->getResolution()!=-1000) startTimeSmearing = pStartH->getResolution();
	}
  }



  if(gHades->getEmbeddingMode()>0 && gHades->getEmbeddingDebug()==1) pCalCat->Clear();  // remove real data in embedding ==3
  iter->Reset();

  while ((pCal = (HRpcCalSim *)iter->Next())!= NULL) {   // real data  or pure simulation objects

    loc[0] = pCal->getSector();
    loc[1] = pCal->getColumn();
    loc[2] = pCal->getCell();

    //Skip deactivated cells
    if (pCellStatusPar->getCellStatus(loc[0],loc[1],loc[2])!=1) continue;

    //Getting cell geometry params for cell dimensions and operations inside the module.

    DPlanes = pGeomCellPar->getDPlanes();
    vprop   = pDigiPar->getVprop();

    //Find object RpcHit

    if (simulation) {
      pHitSim = (HRpcHitSim*)pHitCat->getSlot(loc);
      if ( !pHitSim ) {
	Error ("execute()", "Can't get slot in hit finder: sec %i, col %i, cell %i",loc[0],loc[1],loc[2]);
	return EXIT_FAILURE;
      }
      pHitSim = new(pHitSim) HRpcHitSim;
    }
    else {
      pHit = (HRpcHit*)pHitCat->getSlot(loc);
      if ( !pHit ) {
	Error ("execute()", "Can't get slot in hit finder: sec %i, col %i, cell %i",loc[0],loc[1],loc[2]);
	return EXIT_FAILURE;
      }
      pHit = new(pHit) HRpcHit;
    }

    //Change to cell coordinate system (with origin at the cell center)
    D   =  pGeomCellPar->getLength(loc[1],loc[2]);    // Cell length in mm
    HD  =  0.5*D;                                     // Cell half length in mm
    HDT =  HD/vprop;                                  // Cell half length in ns
    XL  =  pGeomCellPar->getX(loc[1],loc[2]);         // Coordinates of cell LEFT corner in mm
    W   =  pGeomCellPar->getWidth(loc[1],loc[2]);     // Cell width in mm

    //Calculate the variables for filling the object RpcHit

    charge = 0.5*( pCal->getRightCharge() + pCal->getLeftCharge() );

    //
    //REAL DATA
    //
    if(!simulation || gHades->getEmbeddingMode()>0 ) {
      //conversion from tdc width to fC
      charge = pWtoQPar->getPar0() + pWtoQPar->getPar1()*charge +
	pWtoQPar->getPar2()*charge*charge +
	pWtoQPar->getPar3()*TMath::Power(charge,3) +
	pWtoQPar->getPar4()*TMath::Power(charge,4) +
	pWtoQPar->getPar5()*TMath::Power(charge,5);
      xCell  = pCal->getRightTime()*vprop;  // FIXME in analysis: temporary!!!!!!
      tof    = pCal->getLeftTime() - HDT;   // FIXME in analysis: temporary!!!!!!
      tof   -= startTime;
      //Applying Slewing Correction

      if(charge<fQcut) {
	const Float_t *params = pSlewingPar->getExpo1(loc[0],loc[1],loc[2]);
	tof = tof - (params[0] + params[1]*exp(charge*params[2]));
      } else {
	const Float_t *params = pSlewingPar->getExpo2(loc[0],loc[1],loc[2]);
	tof = tof - (params[0] + params[1]*exp(charge*params[2]));
      }

      //correction of the position - time correlation
      const Float_t *parPos =  pTimePosPar->getPars(loc[0],loc[1],loc[2]);
      tof -= parPos[1]*sin(pCal->getRightTime()*parPos[2]+parPos[3]) +
	parPos[4]*sin(pCal->getRightTime()*parPos[5]+parPos[6]) +
	parPos[7]*sin(pCal->getRightTime()*parPos[8]+parPos[9]) +
	parPos[10]*sin(pCal->getRightTime()*parPos[11]+parPos[12]) +
	parPos[13]*sin(pCal->getRightTime()*parPos[14]+parPos[15]);

      //
      //SIMULATION
      //
    } else {
	tof    = 0.5*( pCal->getRightTime() + pCal->getLeftTime()   ) - HDT;
        tof   -= startTimeSmearing;
	xCell  = 0.5*( pCal->getLeftTime() - pCal->getRightTime()  )*vprop;
    }

    yCell  = 0.5*W;

    xMod   = XL - HD - xCell;
    yMod   = pGeomCellPar->getY(loc[1],loc[2]) + yCell;
    zMod   = DPlanes*(loc[1]%2-0.5);

    rRpcInMod.setX(xMod);
    rRpcInMod.setY(yMod);
    rRpcInMod.setZ(zMod);

    //Flag for cell outliers
    Bool_t isInsideCell = kTRUE;
    Float_t extra    = W * ovang;
    Float_t minRange = XL-D-extra;
    Float_t maxRange = XL+extra;

    if(xMod<minRange || xMod>maxRange) isInsideCell = kFALSE;


    //Lab system: all sectors in Lab
    HGeomTransform& TransRpc2Lab = pRpcGeometry->getModule(loc[0],0)->getLabTransform();
    rRpcInLab = TransRpc2Lab.transFrom(rRpcInMod);

    xLab = rRpcInLab.getX();
    yLab = rRpcInLab.getY();
    zLab = rRpcInLab.getZ();

    //Sec system: all sectors referred as sector 0
    HGeomTransform &TransRpc2Sec = pSpecGeomPar->getSector(loc[0])->getTransform();
    rRpcInSec = TransRpc2Sec.transTo(rRpcInLab);

    xSec = rRpcInSec.getX();
    ySec = rRpcInSec.getY();
    zSec = rRpcInSec.getZ();

    theta = (zSec>0.) ? (rad2deg * TMath::ATan(ySec/zSec)) : 0.;
    phi = rad2deg * TMath::ATan2(yLab,xLab);
    if (phi < 0.) phi += 360.;

    //Obtain the resolutions from both the digitizer info and the geometrical one
    sigma_X = pDigiPar->getSigmaX();

    //sigma_T = pDigiPar->getSigmaT()/1000; OLD VERSION of RpcDigiPar
    if((sigma1_T<=0.)||(sigma2_T<=0.)||(sigma3_T<=0.)) {
      // Digitizing model I.
      sigma_T=sigma0_T;
    } else {
      // Digitizing model II.
      sigma_T = sigma0_T+sigma1_T/(1.+TMath::Exp(-sigma2_T*(1.-sigma3_T)));
    }

    sigma_Z = (pGeomCellPar->getDeltaZ())*invsqrt12;
    sigma_Y = W*invsqrt12;

    //Fill the Hit

    if(simulation && gHades->getEmbeddingMode()==0) {    // pure simulation
      pHitSim->setAddress(pCal->getAddress());
      pHitSim->setHit(tof,charge,xMod,yMod,zMod);
      pHitSim->setXSec(xSec);
      pHitSim->setYSec(ySec);
      pHitSim->setZSec(zSec);
      pHitSim->setXYZLab(xLab,yLab,zLab);
      pHitSim->setRMS(sigma_T, sigma_X, sigma_Y, sigma_Z);
      pHitSim->setTheta(theta);
      pHitSim->setPhi(phi);
      pHitSim->setInsideCellFlag(isInsideCell);

      pHitSim->setRefL(pCal->getRefL());
      pHitSim->setRefR(pCal->getRefR());
      pHitSim->setRefLDgtr(pCal->getRefLDgtr());
      pHitSim->setRefRDgtr(pCal->getRefRDgtr());

      pCal->getLisAtBoxArray(isLMotherAtBox);
      pCal->getRisAtBoxArray(isRMotherAtBox);
      pCal->getTrackLArray(trackL);
      pCal->getTrackRArray(trackR);
      pCal->getTrackLDgtrArray(trackLDgtr);
      pCal->getTrackRDgtrArray(trackRDgtr);

      pHitSim->setLisAtBoxArray(isLMotherAtBox);
      pHitSim->setRisAtBoxArray(isRMotherAtBox);
      pHitSim->setTrackLArray(trackL);
      pHitSim->setTrackRArray(trackR);
      pHitSim->setTrackLDgtrArray(trackLDgtr);
      pHitSim->setTrackRDgtrArray(trackRDgtr);

    } else if (gHades->getEmbeddingMode()>0) {           // real data embedding
      pHitSim->setAddress(pCal->getAddress());
      pHitSim->setHit(tof,charge,xMod,yMod,zMod);
      pHitSim->setXSec(xSec);
      pHitSim->setYSec(ySec);
      pHitSim->setZSec(zSec);
      pHitSim->setXYZLab(xLab,yLab,zLab);
      pHitSim->setRMS(sigma_T, sigma_X, sigma_Y, sigma_Z);
      pHitSim->setTheta(theta);
      pHitSim->setPhi(phi);
      pHitSim->setInsideCellFlag(isInsideCell);

      pHitSim->setRefL(gHades->getEmbeddingRealTrackId());
      pHitSim->setRefR(gHades->getEmbeddingRealTrackId());
      pHitSim->setRefLDgtr(gHades->getEmbeddingRealTrackId());
      pHitSim->setRefRDgtr(gHades->getEmbeddingRealTrackId());


      for(Int_t ini = 0;ini<10; ini++) {
	trackL[ini]         = gHades->getEmbeddingRealTrackId();
	trackR[ini]         = gHades->getEmbeddingRealTrackId();
	trackLDgtr[ini]     = gHades->getEmbeddingRealTrackId();
	trackRDgtr[ini]     = gHades->getEmbeddingRealTrackId();
	isLMotherAtBox[ini] = kFALSE;
	isRMotherAtBox[ini] = kFALSE;
      }

      pHitSim->setLisAtBoxArray(isLMotherAtBox);
      pHitSim->setRisAtBoxArray(isRMotherAtBox);
      pHitSim->setTrackLArray(trackL);
      pHitSim->setTrackRArray(trackR);
      pHitSim->setTrackLDgtrArray(trackLDgtr);
      pHitSim->setTrackRDgtrArray(trackRDgtr);
    }
    else {       // real data
      pHit->setAddress(pCal->getAddress());
      pHit->setLogBit(pCal->getLogBit());
      pHit->setHit(tof,charge,xMod,yMod,zMod);
      pHit->setXSec(xSec);
      pHit->setYSec(ySec);
      pHit->setZSec(zSec);
      pHit->setXYZLab(xLab,yLab,zLab);
      pHit->setRMS(sigma_T, sigma_X, sigma_Y, sigma_Z);
      pHit->setTheta(theta);
      pHit->setPhi(phi);
      pHit->setInsideCellFlag(isInsideCell);
    }

  }
  // --------------------------------------------------------------
  // Repeat procedure for embedded mode (read from the temporary category)
  if(gHades->getEmbeddingMode()>0) {
    for( Int_t j = 0; j < pCalCatTmp->getEntries(); j++ ) {
      // This category Stores olways HitSim objects!
      // Location index:
      // pCal was already CalSim, we can recycle it
      pCal = (HRpcCalSim*) pCalCatTmp->getObject(j);
      if(!pCal) continue;

      loc[0] = pCal->getSector();
      loc[1] = pCal->getColumn();
      loc[2] = pCal->getCell();

      if (pCellStatusPar->getCellStatus(loc[0],loc[1],loc[2])!=1) continue;

      // Lets alocate the pHitSimTmp. At the end the information have to be or copied either combined with real hits
      HRpcHitSim pHitSimTmp;

      //Getting cell geometry params for cell dimensions and operations inside the module.

      DPlanes = pGeomCellPar->getDPlanes();
      vprop   = pDigiPar->getVprop();


      //Change to cell coordinate system (with origin at the cell center)
      D   =  pGeomCellPar->getLength(loc[1],loc[2]);    // Cell length in mm
      HD  =  0.5*D;                                     // Cell half length in mm
      HDT =  HD/vprop;                                  // Cell half length in ns
      XL  =  pGeomCellPar->getX(loc[1],loc[2]);         // Coordinates of cell LEFT corner in mm
      W   =  pGeomCellPar->getWidth(loc[1],loc[2]);     // Cell width in mm

      //Calculate the variables for filling the object RpcHit

      charge = 0.5*( pCal->getRightCharge() + pCal->getLeftCharge() );

      tof    = 0.5*( pCal->getRightTime() + pCal->getLeftTime()   ) - HDT;
      tof   -= startTimeSmearing;
      xCell  = 0.5*( pCal->getLeftTime() - pCal->getRightTime()  )*vprop;
      yCell  = 0.5*W;


      xMod   = XL - HD - xCell;
      yMod   = pGeomCellPar->getY(loc[1],loc[2]) + yCell;
      zMod   = DPlanes*(loc[1]%2-0.5);

      rRpcInMod.setX(xMod);
      rRpcInMod.setY(yMod);
      rRpcInMod.setZ(zMod);

      //Flag for cell outliers
      Bool_t isInsideCell = kTRUE;
      Float_t extra    = W * ovang;
      Float_t minRange = XL-D-extra;
      Float_t maxRange = XL+extra;

      if(xMod<minRange || xMod>maxRange) isInsideCell = kFALSE;


      //Lab system: all sectors in Lab
      HGeomTransform& TransRpc2Lab = pRpcGeometry->getModule(loc[0],0)->getLabTransform();
      rRpcInLab = TransRpc2Lab.transFrom(rRpcInMod);

      xLab = rRpcInLab.getX();
      yLab = rRpcInLab.getY();
      zLab = rRpcInLab.getZ();

      //Sec system: all sectors referred as sector 0
      HGeomTransform &TransRpc2Sec = pSpecGeomPar->getSector(loc[0])->getTransform();
      rRpcInSec = TransRpc2Sec.transTo(rRpcInLab);

      xSec = rRpcInSec.getX();
      ySec = rRpcInSec.getY();
      zSec = rRpcInSec.getZ();

      theta = (zSec>0.) ? (rad2deg * TMath::ATan(ySec/zSec)) : 0.;
      phi = rad2deg * TMath::ATan2(yLab,xLab);
      if (phi < 0.) phi += 360.;

      //Obtain the resolutions from both the digitizer info and the geometrical one
      sigma_X = pDigiPar->getSigmaX();

      //sigma_T = pDigiPar->getSigmaT()/1000; OLD VERSION of RpcDigiPar
      if((sigma1_T<=0.)||(sigma2_T<=0.)||(sigma3_T<=0.)) {
	// Digitizing model I.
	sigma_T=sigma0_T;
      } else {
	// Digitizing model II.
	sigma_T = sigma0_T+sigma1_T/(1.+TMath::Exp(-sigma2_T*(1.-sigma3_T)));
      }

      sigma_Z = (pGeomCellPar->getDeltaZ())*invsqrt12;
      sigma_Y = W*invsqrt12;

      pHitSimTmp.setAddress(pCal->getAddress());
      pHitSimTmp.setHit(tof,charge,xMod,yMod,zMod);
      pHitSimTmp.setXSec(xSec);
      pHitSimTmp.setYSec(ySec);
      pHitSimTmp.setZSec(zSec);
      pHitSimTmp.setXYZLab(xLab,yLab,zLab);
      pHitSimTmp.setRMS(sigma_T, sigma_X, sigma_Y, sigma_Z);
      pHitSimTmp.setTheta(theta);
      pHitSimTmp.setPhi(phi);
      pHitSimTmp.setInsideCellFlag(isInsideCell);

      pHitSimTmp.setRefL(pCal->getRefL());
      pHitSimTmp.setRefR(pCal->getRefR());
      pHitSimTmp.setRefLDgtr(pCal->getRefLDgtr());
      pHitSimTmp.setRefRDgtr(pCal->getRefRDgtr());

      pCal->getLisAtBoxArray(isLMotherAtBox);
      pCal->getRisAtBoxArray(isRMotherAtBox);
      pCal->getTrackLArray(trackL);
      pCal->getTrackRArray(trackR);
      pCal->getTrackLDgtrArray(trackLDgtr);
      pCal->getTrackRDgtrArray(trackRDgtr);

      pHitSimTmp.setLisAtBoxArray(isLMotherAtBox);
      pHitSimTmp.setRisAtBoxArray(isRMotherAtBox);
      pHitSimTmp.setTrackLArray(trackL);
      pHitSimTmp.setTrackRArray(trackR);
      pHitSimTmp.setTrackLDgtrArray(trackLDgtr);
      pHitSimTmp.setTrackRDgtrArray(trackRDgtr);


      // Let check wether there is already a hit at this location, otherwise create alocate new memory

      pHitSim = (HRpcHitSim*) pHitCat-> getObject(loc);
      if(pHitSim && gHades->getEmbeddingMode()!=2 ) {  // embedding mode == 2 -> keep geanthits

	// an rpc hit was already stored!
	// We have to merge

	// Getting left and right time of the real hit


	rightT =  (pHitSim->getTof() + HDT) -   (XL - HD -pHitSim->getXMod())/vprop  ;
	leftT  =  (pHitSim->getTof() + HDT) +   (XL - HD -pHitSim->getXMod())/vprop  ;


	if(pCal->getRightTime()<rightT && pCal->getLeftTime()<leftT ) {
	  //!!OVERWRITE!! the real with the atributes of the embedded
	  new (pHitSim) HRpcHitSim(pHitSimTmp);
	}

	if(pCal->getRightTime()<rightT && pCal->getLeftTime()>leftT) {
	  // Combine BOTH Hits. Q should be the sum of BOTH!


	  pHitSim->setRisAtBoxArray(isRMotherAtBox);
	  pHitSim->setTrackRArray(trackR);
	  pHitSim->setTrackRDgtrArray(trackRDgtr);

	  //Recombine tof and position!
	  xCell  = 0.5*( pCal->getLeftTime() - rightT)*vprop;
	  xMod   = XL - HD - xCell;

	  rRpcInMod.setX(xMod);
	  rRpcInMod.setY(yMod);
	  rRpcInMod.setZ(zMod);

	  //Flag for cell outliers
	  Bool_t isInsideCell = kTRUE;
	  Float_t extra    = W * ovang;
	  Float_t minRange = XL-D-extra;
	  Float_t maxRange = XL+extra;
	  if(xMod<minRange || xMod>maxRange) isInsideCell = kFALSE;


	  //Lab system: all sectors in Lab
	  HGeomTransform& TransRpc2Lab = pRpcGeometry->getModule(loc[0],0)->getLabTransform();
	  rRpcInLab = TransRpc2Lab.transFrom(rRpcInMod);

	  xLab = rRpcInLab.getX();
	  yLab = rRpcInLab.getY();
	  zLab = rRpcInLab.getZ();

	  //Sec system: all sectors referred as sector 0
	  HGeomTransform &TransRpc2Sec = pSpecGeomPar->getSector(loc[0])->getTransform();
	  rRpcInSec = TransRpc2Sec.transTo(rRpcInLab);

	  xSec = rRpcInSec.getX();
	  ySec = rRpcInSec.getY();
	  zSec = rRpcInSec.getZ();

	  theta = (zSec>0.) ? (rad2deg * TMath::ATan(ySec/zSec)) : 0.;
	  phi = rad2deg * TMath::ATan2(yLab,xLab);
	  if (phi < 0.) phi += 360.;
	  pHitSim->setHit(0.5*(rightT+pCal->getLeftTime())-HDT,charge+pHitSim->getCharge(),xMod,yMod,zMod);
	  pHitSim->setXSec(xSec);
	  pHitSim->setYSec(ySec);
	  pHitSim->setZSec(zSec);
	  pHitSim->setXYZLab(xLab,yLab,zLab);
	  pHitSim->setTheta(theta);
	  pHitSim->setPhi(phi);
	  pHitSim->setInsideCellFlag(isInsideCell);

	}

	if (pCal->getRightTime()>rightT && pCal->getLeftTime()<leftT) {
	  // Combine BOTH Hits. Q should be the sum of BOTH!

	  pHitSim->setLisAtBoxArray(isLMotherAtBox);
	  pHitSim->setTrackLArray(trackL);
	  pHitSim->setTrackLDgtrArray(trackLDgtr);

	  //Recombine tof and position!
	  xCell  = 0.5*( leftT -  pCal->getRightTime())*vprop;
	  xMod   = XL - HD - xCell;

	  rRpcInMod.setX(xMod);
	  rRpcInMod.setY(yMod);
	  rRpcInMod.setZ(zMod);

	  //Flag for cell outliers
	  Bool_t isInsideCell = kTRUE;
	  Float_t extra    = W * ovang;
	  Float_t minRange = XL-D-extra;
	  Float_t maxRange = XL+extra;

	  if(xMod<minRange || xMod>maxRange) isInsideCell = kFALSE;


	  //Lab system: all sectors in Lab
	  HGeomTransform& TransRpc2Lab = pRpcGeometry->getModule(loc[0],0)->getLabTransform();
	  rRpcInLab = TransRpc2Lab.transFrom(rRpcInMod);

	  xLab = rRpcInLab.getX();
	  yLab = rRpcInLab.getY();
	  zLab = rRpcInLab.getZ();

	  //Sec system: all sectors referred as sector 0
	  HGeomTransform &TransRpc2Sec = pSpecGeomPar->getSector(loc[0])->getTransform();
	  rRpcInSec = TransRpc2Sec.transTo(rRpcInLab);

	  xSec = rRpcInSec.getX();
	  ySec = rRpcInSec.getY();
	  zSec = rRpcInSec.getZ();

	  theta = rad2deg * TMath::ATan2(sqrt(ySec*ySec+xSec*xSec),zSec);
	  phi = rad2deg * TMath::ATan2(yLab,xLab);
	  if (phi < 0.) phi += 360.;
	  pHitSim->setHit(0.5*(rightT+pCal->getLeftTime())-HDT,charge+pHitSim->getCharge(),xMod,yMod,zMod);
	  pHitSim->setXSec(xSec);
	  pHitSim->setYSec(ySec);
	  pHitSim->setZSec(zSec);
	  pHitSim->setXYZLab(xLab,yLab,zLab);
	  pHitSim->setTheta(theta);
	  pHitSim->setPhi(phi);
	  pHitSim->setInsideCellFlag(isInsideCell);

	  pHitSim->setHit(0.5*(leftT+pCal->getRightTime())-HDT,charge+pHitSim->getCharge(),xMod,yMod,zMod);
	}
      } else {
	//Just copy the temporal hit to the output
	pHitSim = (HRpcHitSim*) pHitCat ->getSlot(loc);
	if(pHitSim){
	    new (pHitSim) HRpcHitSim(pHitSimTmp);
	}
	else {
	    Error("HRpcHitF at embedding","Could not retrieve slot in catRpcHit!");
	}

      }
    }
  }
  return EXIT_SUCCESS;
}

ClassImp(HRpcHitF)
