#include "hparticleanglecor.h"

#include <string.h>
#include <iostream>

ClassImp(HParticleAngleCor)

// author: W. Koenig, creation date: Nov 2012

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
// recalculate the primary emission angles theta, phi obtained from
// Runge Kutta by assuming:
// 1) emission from the global vertex
// 2) the RICH mirror and shell are the dominant sources of changing
//    the emission angles via multiple scattering
// Assuming a field free region inside the RICH (quite good assumption),
// the Runge Kutta track is tracked to an effective mirror/ shell sphere
// From the crossing point of the track with the sphere, the particle is
// backtracked to the global vertes.
// The x- and y-values of the event by event global vertex can be replaced
// by their mean values (better in case of small beamspots)
//
//
//  Posibble corrections:
//  1. alignment of Rich (theta,phi tables) : alignRichRing()
//  2. Emission from event vertex
//  The order of correction matters!
//
//--------------------------------------------------------------------------
//  example:
//  HParticleAngleCor  anglecor;
//
//
//  for(Int_t i=0;i<particleCandCat->getEntries();i++)
//  {
//
//     HParticleCand* cand =0;
//     cand = HCategoryManager::getObject(particleCandCat,i);
//     if(cand.isFlagBit(kIsLepton)){ // leptons only
//        Double_t thetacor=0;
//        Double_t phicor=0;
//        Double_t rhocor=0;
//        Double_t zcor=0;
//        //anglecor.alignRichRing(cand);  // changes values inside candidate,
//                                         // should be only be applied if not already done on dst level (default)
//        anglecor.recalcEmission(cand,zcor,rhocor,thetacor,phicor); // z,r,theta,phi: return new segment variables (candidate unchanged)
//        // anglecor.recalcAngles(cand,thetacor,phicor);            // theta,phi : return to segment angles (candidate unchanged)
//        anglecor.realignRichRing(cand,thetacor,phicor);            // corr for MS correction      (candidate unchanged)
//
//        // anglecor.recalcSetEmission(cand);  // z,r,theta,phi: changes values inside candidate
//        // anglecor.recalcSetAngles(cand);    // theta,phi :changes values inside candidate
//        // anglecor.realignRichRing(cand);    // corr for MS correction . changes values inside candidate
//     }
//  }
////////////////////////////////////////////////////////////////////////////////

HParticleAngleCor::HParticleAngleCor(void)
{
    initParam();
};

void HParticleAngleCor::initParam()
{
    meanZTarget  = -30.0F;             // typical shift of target in lab coordinates
    zRichCenter  = meanZTarget - 5.0F; // typical shift of rich as compared to design value
    zMirror      = zRichCenter - 493.5F;
    rMirror      = 879.0F;   //effective radius for dominant multiple scattering source (mirror, shell)
    drShield     = 10.0F;    //additional distance from target center to effective multiple scattering point due to delta electron shield
    maxRhoShield = 376.0F;   // end of delta electron shield.
    useMeanXY    = kFALSE;
    useShield    = kFALSE;
    vertexType   = Particle::kVertexParticle;
    maxAverage   = 500;
    bDoWarn      = kFALSE;
    resetMeanVertex();
};

void HParticleAngleCor::resetMeanVertex()
{
    // reset moving average and start with X,Y values
    // from global vertex of next event

    sumXVertex = 0.0F;
    sumYVertex = 0.0F;
    while (!qxVertex.empty()) qxVertex.pop();
    while (!qyVertex.empty()) qyVertex.pop();
}

Bool_t HParticleAngleCor::recalcEmission(const Double_t z, const Double_t rho, const Double_t theta, const Double_t phi,
					 Double_t & zCor, Double_t & rhoCor, Double_t & thetaCor, Double_t & phiCor)
{
    // calculate new emission angles and emission point based on global vertex.
    // Assume that the Rich mirror and Rich housing (+ delta electron shield)
    // produce the dominant multiple scattering kick which generate an off-vertex
    // behaviour of a particle originating from global vertex.
    // Returns kFALSE, if correction fails.In this case: New values = old values

    HGeomVector offsetHit;
    HGeomVector dirHit;
    HGeomVector mirrorHit;

    calcSegVector(z, rho, phi/TMath::RadToDeg(), theta/TMath::RadToDeg(), offsetHit, dirHit);
    //calculate intersection of Runge Kutta track with Dummy mirror
    //(used as effective scattering plane (mirror + housing + shield)
    zCor = z;
    rhoCor = rho;
    thetaCor = theta;
    phiCor = phi;
    HGeomVector centerMirror(0.0,0.0,zMirror);
    offsetHit -= centerMirror; // shift Hit into mirror coordinates
    Float_t a = dirHit.scalarProduct(dirHit);
    Float_t b = 2.0*dirHit.scalarProduct(offsetHit);
    Float_t c = offsetHit.scalarProduct(offsetHit) - rMirror*rMirror;
    Float_t discr = b*b - 4.0*a*c;
    if (discr <= 0.0)  return kFALSE;
    Float_t discrSqrt = sqrt(discr);
    Float_t q;
    if (b < 0) { q = (-b - discrSqrt)/2.0; }
    else       { q = (-b + discrSqrt)/2.0; }

    // compute t0 and t1
    Float_t t0 = q / a;
    Float_t t1 = c / q;
    // make sure t0 is smaller than t1
    if (t0 > t1) {
	Float_t temp = t0;
	t0 = t1;
	t1 = temp;
    }
    // going backward does not make sense, sphere covers forward cone
    if (t1 < 0) return kFALSE;
    // calculate hit point on sphere
    if (t0 < 0)  t0 = t1; //intersection point is at t1
    dirHit *= t0;
    mirrorHit  = offsetHit + dirHit;
    mirrorHit += centerMirror;  // sphere to LAB
    // calculate emission angles from line: vertex to an
    // effective mirror shell representing multiple scattering point
    Float_t dx = mirrorHit.getX();
    Float_t dy = mirrorHit.getY();
    Float_t dz = mirrorHit.getZ();
    if (useShield) {
	if (sqrt(dx*dx + dy*dy) < maxRhoShield) {
	    Float_t shift =drShield/t0;
	    dx += shift*dirHit.getX();
	    dy += shift*dirHit.getY();
	    dz += shift*dirHit.getZ();
	}
    }
    HGeomVector globalVertex = getGlobalVertex(vertexType,bDoWarn);
    if(globalVertex.getX() < -500.0 || globalVertex.getY() < -500.0)  return kFALSE;

    if (useMeanXY) {

	if(qxVertex.size() == maxAverage) { // queue is full
           sumXVertex -= qxVertex.front();
           sumYVertex -= qyVertex.front();
	   qxVertex.pop();
           qyVertex.pop();
	}
	sumXVertex += globalVertex.getX();
	sumYVertex += globalVertex.getY();
	qxVertex.push(globalVertex.getX());
	qyVertex.push(globalVertex.getY());

	dx -= sumXVertex/Float_t(qxVertex.size());
	dy -= sumYVertex/Float_t(qyVertex.size());

    } else {
	dx -= globalVertex.getX();
	dy -= globalVertex.getY();
    }

    dz -= globalVertex.getZ();
    thetaCor = acos(dz/sqrt(dx*dx + dy*dy + dz*dz)) * TMath::RadToDeg();
    phiCor = asin(dy/sqrt(dx*dx + dy*dy)) * TMath::RadToDeg();
    if(dx < 0.0) phiCor = 180.0 - phiCor;
    else { if (dy < 0.0) phiCor += 360.0; }
    zCor = globalVertex.getZ();
    rhoCor = sqrt(globalVertex.getX()* globalVertex.getX() +  globalVertex.getY()* globalVertex.getY());
    return kTRUE;
};

Bool_t HParticleAngleCor::recalcEmission(const HParticleCand * cand,
					 Double_t & zCor, Double_t & rhoCor, Double_t & thetaCor, Double_t & phiCor)
{
    // same as above, get old emission values from ParticleCandidate
    Double_t myTheta = cand->getTheta();
    Double_t myPhi   = cand->getPhi();
    Double_t myRho   = cand->getR();
    Double_t myZ     = cand->getZ();
    return recalcEmission(myZ, myRho, myTheta, myPhi, zCor, rhoCor, thetaCor, phiCor);
};

Bool_t HParticleAngleCor::recalcAngles(const HParticleCand * cand, Double_t & thetaCor, Double_t & phiCor)
{
    // return only angles theta, phi. Emission point is global vertex
    // abyway (x,y could be mean values of global vertex)
    Double_t myRhoCor = cand->getR();
    Double_t myZCor   = cand->getZ();
    return recalcEmission(cand, myZCor, myRhoCor, thetaCor, phiCor);
};

Bool_t HParticleAngleCor::recalcSetAngles(HParticleCand * cand )
{
    // calculate new emission angles based on global vertex
    // and replace theta and phi of the particle candidate
    Double_t myRhoCor   = cand->getR();
    Double_t myZCor     = cand->getZ();
    Double_t myThetaCor = cand->getTheta();
    Double_t myPhiCor   = cand->getPhi();
    Bool_t isCalculated = recalcEmission( cand, myZCor, myRhoCor, myThetaCor, myPhiCor);
    if(isCalculated) {
	cand -> setTheta(myThetaCor);
	cand -> setPhi(myPhiCor);
    }
    return isCalculated;
};

Bool_t HParticleAngleCor::recalcSetEmission(HParticleCand * cand )
{
    // calculate new emission point and angles based on global vertex
    // and replace theta, phi, z and rho of the particle candidate
    Double_t myRhoCor   = cand->getR();
    Double_t myZCor     = cand->getZ();
    Double_t myThetaCor = cand->getTheta();
    Double_t myPhiCor   = cand->getPhi();
    Bool_t isCalculated = recalcEmission( cand, myZCor, myRhoCor, myThetaCor, myPhiCor);
    if(isCalculated) {
	cand -> setTheta(myThetaCor);
	cand -> setPhi(myPhiCor);
	cand -> setR(myRhoCor);
	cand -> setZ(myZCor);
    }
    return isCalculated;
};

Bool_t HParticleAngleCor::alignRichRing( HParticleCand * cand )
{
    // correct angles theta and phi of a rich ring. Accounts for the missing rich
    // sector alignment.The lookup table was obtained from fits to ring-track
    // matches in theta and phi. Alignment parameters where obtained for beamtime
    // Apr12 based on Gen1 Dst's. Values are changed inside HParticleCand!
    Double_t thetaCor, phiCor;
    Bool_t ok = HParticleAngleCor::alignRichRing(cand->getRichTheta(),cand->getRichPhi(),thetaCor,phiCor);

    if(ok) {
	cand->setRichTheta(thetaCor);
	cand->setRichPhi(phiCor);
    }
    return ok;
}

Bool_t HParticleAngleCor::alignRichRing(const Double_t theta, const Double_t phi, Double_t & thetaCor, Double_t & phiCor)
{
    // correct angles theta and phi of a rich ring. Accounts for the missing rich
    // sector alignment.The lookup table was obtained from fits to ring-track
    // matches in theta and phi. Alignment parameters where obtained for beamtime
    // Apr12 based on Gen1 Dst's. theta, phi in degree, lab coordinate system

    const static Int_t nThetaMax = 4, iPhiMax = 4;
    const static Float_t thetaRangeMax[nThetaMax] = {30,45,60,90};
    const static Float_t phiRangeMax[iPhiMax] = {20,30,40,60};
    const Float_t shiftPar[6][2][iPhiMax][nThetaMax] = {{{
 	{0.56847,0.35541,0.31349,-0.079804},
	{0.58678,0.41001,0.31194,-0.016983},
	{0.55839,0.42173,0.15541,-0.16086},
	{0.57542,0.29867,0.006334,-0.49413}},{
	{0.27234,0.27925,0.3343,0.47981},
	{0.079126,0.13995,0.16474,0.18882},
	{0.028984,0.12106,0.13845,0.06516},
	{-0.099233,0.030413,0.026281,-0.016054}}},{{
	{-0.62106,-0.68579,-0.62576,-0.18773},
	{-0.62868,-0.60131,-0.65868,-0.11186},
	{-0.547,-0.59218,-0.66754,-0.62592},
	{-0.54411,-0.50647,-0.52213,-0.74901}},{
	{0.16542,0.24503,0.20487,0.14333},
	{0.24607,0.27335,0.3276,0.18789},
	{0.26814,0.35008,0.4058,0.46018},
	{0.30533,0.42488,0.51889,0.52347}}},{{
	{-0.35137,-0.29072,-0.19218,-0.25986},
	{-0.2464,-0.19682,-0.14667,0.18021},
	{-0.30104,-0.1597,-0.034497,0.18179},
	{-0.21222,-0.09149,0.10629,0.25746}},{
	{0.487,0.38449,0.31712,0.42932},
	{0.46812,0.46086,0.4263,0.51243},
	{0.55227,0.52525,0.64938,0.72021},
	{0.34025,0.5199,0.66043,0.6877}}},{{
	{0.58643,0.55622,0.35129,0.028304},
	{0.67317,0.60042,0.54704,0.21637},
	{0.73818,0.67505,0.54371,0.21095},
	{0.70242,0.62959,0.47486,0.12533}},{
	{0.61073,0.54326,0.64655,0.53829},
	{0.40976,0.45259,0.43612,0.45173},
	{0.29182,0.37078,0.31641,0.33808},
	{0.10628,0.16099,0.10268,0.10144}}},{{
	{0.67657,0.78895,0.9884,1.1216},
	{0.6292,0.77602,1.0867,0.94518},
	{0.58755,0.70365,1.0226,0.81241},
	{0.47614,0.60966,0.82907,0.80813}},{
	{0.032472,0.14918,0.12206,-0.074051},
	{-0.12312,-0.022246,0.026606,-0.15131},
	{-0.27893,-0.13872,-0.052954,0.16061},
	{-0.42002,-0.3216,-0.13963,0.10483}}},{{
	{0.11211,0.21833,0.34406,0.52112},
	{0.029877,0.21069,0.26046,0.37594},
	{0.057053,0.067113,0.10103,-0.22338},
	{-0.23348,-0.11148,-0.022356,-0.27308}},{
	{-0.21211,-0.15537,-0.041881,0.0074485},
	{-0.35027,-0.27137,-0.11973,-0.035381},
	{-0.38277,-0.33309,-0.13335,0.040826},
	{-0.45303,-0.30742,-0.15834,0.1868}}}};
    //find sector index 0: 0°-60°, 5: 300°-360°
    thetaCor = theta;
    phiCor = phi;
    if(phi < 0.0F || theta < 0.0F) return kFALSE;
    Int_t nSector = int(phi/60.0F);
    if(nSector>5) return kFALSE;
    Float_t sectorPhi = phi- nSector*60.0F;
    Int_t iPhi = 0;
    while(sectorPhi > phiRangeMax[iPhi] && iPhi < iPhiMax-1) ++iPhi;
    Int_t nTheta = 0;
    while(theta > thetaRangeMax[nTheta] && nTheta < nThetaMax-1) ++nTheta;
    thetaCor -= shiftPar[nSector][0][iPhi][nTheta];
    phiCor -= shiftPar[nSector][1][iPhi][nTheta]/sin(theta*TMath::DegToRad());
    return kTRUE;
}


Bool_t HParticleAngleCor::realignRichRing(const HParticleCand* pCand, Double_t & thetaCor, Double_t & phiCor)
{
    // correct angles theta and phi of a rich ring. Its supposed to called after
    // recalulating emmission from target ("MS suppresion") for candidate RK theta/phi.
    // Accounts for the missing rich sector alignment.
    // Lookup table was obtained from fits to ring-track matches in theta and phi.
    // Alignment parameters where obtained for beamtime Apr12 based on Gen3 Dst's.
    // theta, phi in degree, lab coordinate system.
    // nSector is NOT the standard labeling: Sector0 corresponds to nSector=1
	const static Float_t momCut[2] = {300.0F, 250.0F};
	const static Int_t nThetaMax = 4, iPhiMax = 4;
	const static Float_t thetaRangeMax[nThetaMax] = {30.0F, 45.0F, 60.0F, 90.0F};
	const static Float_t phiRangeMax[iPhiMax] = {20.0F, 30.0F, 40.0F, 60.0F};
	const static Float_t shiftPar1[6][2][iPhiMax][nThetaMax] = {{{
	{-0.092739,0.023653,0.016968,-0.0024708},
	{-0.11308,0.044434,-0.016988,0.03469},
	{-0.074908,-0.059965,0.057346,0.044692},
	{-0.094272,0.002754,0.016716,0.086105}},{
	{-0.16276,-0.082827,-0.06083,-0.09322},
	{-0.002822,0.017274,0.010158,0.016152},
	{-0.017163,-0.014132,-0.040986,-0.024062},
	{0.025919,0.036879,-0.016991,-0.029996}}},{{
	{0.011434,0.10048,0.048845,-0.014609},
	{0.10352,0.047945,0.050549,-0.007719},
	{0.08095,0.089099,0.047918,0.0035124},
	{0.076309,0.10224,-0.0027016,-0.026528}},{
	{0.03586,-0.005282,0.0054312,0.02817},
	{0.004666,-0.003001,-0.020239,0.018498},
	{0.0054667,-0.042004,-0.035509,-0.15294},
	{0.040516,-0.071509,-0.10748,-0.14374}}},{{
	{-0.16896,-0.098246,-0.15132,-0.065578},
	{-0.19841,-0.071942,-0.15598,-0.14158},
	{-0.030301,-0.069394,-0.094452,-0.09163},
	{-0.060476,0.01587,-0.046836,-0.072952}},{
	{0.042005,0.036159,-0.012102,-0.1258},
	{0.036985,0.0031095,-0.04017,-0.16126},
	{0.089353,0.056012,-0.13377,-0.16524},
	{0.2898,0.034105,-0.072114,-0.12049}}},{{
	{-0.10125,-0.02439,0.054406,0.070275},
	{-0.095919,-0.0027452,-0.006579,0.007045},
	{-0.075563,-0.04556,0.015622,0.026437},
	{-0.055282,0.027411,0.0096561,0.025066}},{
	{-0.10226,-0.014848,-0.06262,-0.051449},
	{-0.026369,-0.031205,0.026242,-0.067515},
	{0.02094,-0.036626,-0.005096,-0.10511},
	{0.041851,0.006958,-0.0059644,-0.002775}}},{{
	{-0.11222,-0.023138,-0.041625,0.02989},
	{-0.093553,-0.088438,-0.1416,-0.042768},
	{-0.081391,-0.04766,-0.06972,0.028859},
	{-0.11118,0.007131,-0.040263,0.026113}},{
	{-0.032704,-0.10333,-0.052341,-0.1845},
	{-0.035602,-0.061539,-0.045447,-0.10769},
	{0.033031,-0.025438,-0.044704,-0.079134},
	{-0.023192,-0.068683,-0.083119,-0.092149}}},{{
	{-0.0012393,0.030286,0.010814,-0.041932},
	{-0.034064,0.017286,-0.024381,-0.040926},
	{-0.094453,0.015471,0.061431,-0.045244},
	{0.043032,0.075558,0.088217,-0.046712}},{
	{-0.099706,-0.07086,-0.14158,-0.13907},
	{0.011922,0.003569,-0.071808,-0.099851},
	{-0.067405,0.017341,-0.10277,-0.070434},
	{0.054543,-0.045816,-0.10609,-0.1164}}}};

 	const Float_t shiftPar2[6][2][iPhiMax][nThetaMax] = {{{
	{-0.11406,0.046498,-0.075155,0.0024726},
	{-0.059022,-0.057893,-0.042205,0.011758},
	{-0.042918,-0.083163,0.02,0.1018},
	{-0.099402,0.029654,0.049998,0.058895}},{
	{-0.16517,-0.038156,-0.028804,-0.057853},
	{-0.001923,-0.022126,0.019011,-0.011899},
	{0.007916,-0.046474,-0.049339,-0.019734},
	{0.077363,0.018855,-0.014678,-0.058308}}},{{
	{0.042217,0.067663,0.078126,0.002413},
	{0.12178,0.041864,0.086282,-0.015231},
	{0.040257,0.060694,0.06548,0.0034334},
	{0.11603,0.067885,0.078771,0.048102}},{
	{-0.010168,-0.033484,0.07379,0.005976},
	{0.009626,0.017373,0.003253,0.047387},
	{-0.0067593,-0.044735,-0.099524,-0.14054},
	{0.10932,-0.13085,-0.044854,-0.088218}}},{{
	{-0.2002,-0.097437,-0.016317,-0.092651},
	{-0.15671,-0.14322,-0.054332,-0.13401},
	{-0.06327,-0.03246,-0.059684,-0.09835},
	{-0.050708,-0.040492,-0.010997,-0.040209}},{
	{0.040994,0.023971,-0.078649,-0.12261},
	{0.095405,0.014448,-0.04638,-0.17486},
	{0.10105,0.060204,-0.09996,-0.20345},
	{0.20297,-0.019825,-0.094345,-0.17653}}},{{
	{-0.084385,-0.046647,-0.010375,0.10826},
	{-0.10908,-0.048116,0.10018,0.021859},
	{-0.10308,0.014434,0.030704,0.078475},
	{0.033566,-0.02909,-0.0030719,0.070762}},{
	{-0.14508,-0.028961,-0.064345,-0.071851},
	{-0.031258,0.004128,-0.037568,-0.074356},
	{0.021726,-0.069162,0.01152,-0.11648},
	{0.11381,0.03152,-0.0056074,-0.0334}}},{{
	{-0.12302,-0.071161,0.044689,0.057078},
	{-0.1179,-0.098449,0.012518,-0.061866},
	{-0.052183,0.002128,-0.054111,-0.007125},
	{-0.020314,0.0097511,-0.010061,0.035873}},{
	{-0.089082,-0.055882,-0.056756,-0.18039},
	{-0.046809,-0.11161,-0.007961,-0.075349},
	{0.01845,-0.04639,-0.002527,-0.14643},
	{0.045509,0.045282,-0.099269,-0.073656}}},{{
	{0.041931,0.063217,-0.036776,-0.025957},
	{0.016196,-0.014366,0.014601,-0.002182},
	{-0.031912,0.0051631,0.13385,0.018121},
	{0.10238,0.10332,0.12031,-0.038174}},{
	{-0.12781,-0.047991,-0.11326,-0.17948},
	{-0.059842,-0.024242,-0.075628,-0.10223},
	{0.012009,-0.003801,-0.13173,-0.069966},
	{0.036544,0.006294,-0.085144,-0.070706}}}};
 
	//find sector index: 0 -> 0°-60°, 5 -> 300°-360°
       Float_t theta = pCand->getRichTheta();
       thetaCor = theta;
       Float_t phi = pCand->getRichPhi();
       phiCor = phi;
       if(phi <= 0.0F || theta <= 0.0F) return kFALSE;
       Int_t nSector = int(phi/60.0F);
       if(nSector>5) return kFALSE;
       Float_t sectorPhi = phi- nSector*60.0F;
       Int_t iPhi = 0;
       while(sectorPhi > phiRangeMax[iPhi] && iPhi < iPhiMax-1) ++iPhi;
       Int_t nTheta = 0;
       while(theta > thetaRangeMax[nTheta] && nTheta < nThetaMax-1) ++nTheta;
       Int_t system = pCand->getSystemUsed();
       if(system < 0) return kFALSE;

       if(pCand->getMomentum() < momCut[system]) {
           thetaCor -= shiftPar1[nSector][0][iPhi][nTheta];
           phiCor -= shiftPar1[nSector][1][iPhi][nTheta]/sin(theta*TMath::DegToRad());
       } else {
           thetaCor -= shiftPar2[nSector][0][iPhi][nTheta];
           phiCor -= shiftPar2[nSector][1][iPhi][nTheta]/sin(theta*TMath::DegToRad());
       }
       return kTRUE;
}

Bool_t HParticleAngleCor::realignRichRing(HParticleCand* pCand)
{
    // same as realignRichRing(const HParticleCand* pCand, Float_t & thetaCor, Float_t & phiCor)
    // but changes the RICH angles inside the candidate
    Double_t thetaCor;
    Double_t phiCor;
    Bool_t isSuccess = realignRichRing(pCand, thetaCor, phiCor);
    pCand->setRichTheta((Float_t)thetaCor);
    pCand->setRichPhi((Float_t)phiCor);
    return isSuccess;
}



Float_t HParticleAngleCor::matchRingTrack(HParticleCand* cand)
{
    // match angles theta and phi of a track with a rich ring attached in terms of resolution sigma
    // Lookup table for sigma was obtained from fits to ring-track matches in 4 theta and 4 phi bins.
    // momentum dependence was calculated from 2 momentum bins assuming a constant term
    // and a term which reflects multiple scattering: sigma proportional to (1/p + const).
    // return value is a normalized matching parameter: radial deviation/sigma
    // Matching parameters where obtained for beamtime Apr12 based on Gen2 Dst's.
    // theta, phi in degree, lab coordinate system
    // thetaSwitch: <= low theta range, > high theta range

    const static Float_t thetaSwitch = 45.0F;
    // 1st index: theta range, 2nd: momentum range
    const static Float_t meanMom[2][2] = {{167.8,375.8},{129,317.7}};
    // 1st index: theta range, 2nd: sector No
    const static Float_t offset[2][6] = {{0.041,0.037,0.029,0.03,0.036,0.026},{0.046,0.053,0.046,0.054,0.046,0.043}};

    // choice of parameter set depends on momCut[system]
    const static Float_t momCut[2] = {300.0F, 250.0F};
    const static Int_t iPhiMax = 4, nThetaMax = 4;
    const static Float_t thetaRangeMax[nThetaMax] = {30,45,60,100};
    const static Float_t phiRangeMax[iPhiMax] = {20,30,40,60};

    const static Float_t sigma1[6][2][iPhiMax][nThetaMax] = {{{
	    {0.55924,0.55353,0.65307,0.59934},
	    {0.552,0.57492,0.61581,0.5875},
	    {0.54223,0.60793,0.61734,0.58786},
	    {0.55226,0.5611,0.6028,0.5968}},{
	    {0.52752,0.51162,0.47095,0.42359},
	    {0.52067,0.4978,0.44986,0.41902},
	    {0.53515,0.51425,0.4738,0.40452},
	    {0.5804,0.53215,0.48439,0.42827}}},{{
	    {0.55297,0.55339,0.56329,0.56661},
	    {0.54665,0.55351,0.56629,0.58141},
	    {0.57289,0.53113,0.58841,0.59241},
	    {0.56055,0.52817,0.57644,0.59531}},{
	    {0.60059,0.49979,0.4568,0.40923},
	    {0.53156,0.48956,0.46635,0.42021},
	    {0.5573,0.48273,0.46385,0.42725},
	    {0.59745,0.49024,0.48633,0.43068}}},{{
	    {0.57941,0.52969,0.6083,0.62983},
	    {0.54779,0.60733,0.6413,0.5888},
	    {0.48017,0.5617,0.63537,0.56407},
	    {0.57225,0.53583,0.55962,0.5894}},{
	    {0.62081,0.51635,0.50081,0.42072},
	    {0.59706,0.48601,0.44729,0.42},
	    {0.52491,0.51799,0.47185,0.43233},
	    {0.55405,0.54764,0.46426,0.40859}}},{{
	    {0.54363,0.56977,0.63411,0.58217},
	    {0.56767,0.58413,0.61839,0.61216},
	    {0.56231,0.60754,0.62282,0.60909},
	    {0.58223,0.55791,0.59059,0.6052}},{
	    {0.56448,0.52074,0.4777,0.43136},
	    {0.54545,0.527,0.46688,0.4406},
	    {0.54778,0.48931,0.50287,0.42133},
	    {0.49962,0.51172,0.45036,0.41108}}},{{
	    {0.53164,0.52888,0.5356,0.54863},
	    {0.53663,0.53406,0.58514,0.54699},
	    {0.48597,0.4792,0.62128,0.56482},
	    {0.51513,0.5308,0.60863,0.57046}},{
	    {0.5094,0.49822,0.49799,0.41528},
	    {0.498,0.47634,0.46575,0.43153},
	    {0.52222,0.47632,0.45053,0.41884},
	    {0.57416,0.47978,0.50019,0.41287}}},{{
	    {0.5578,0.57014,0.56695,0.62433},
	    {0.55982,0.56062,0.58259,0.62821},
	    {0.51614,0.51091,0.61677,0.59801},
	    {0.53174,0.53656,0.60819,0.58528}},{
	    {0.55615,0.49903,0.45319,0.42909},
	    {0.56409,0.4777,0.45968,0.45209},
	    {0.57695,0.52999,0.47387,0.41775},
	    {0.56346,0.52195,0.47053,0.42081}}}};

    const static Float_t sigma2[6][2][iPhiMax][nThetaMax] = {{{
	    {0.45771,0.53346,0.5247,0.56995},
	    {0.49291,0.50047,0.50079,0.46371},
	    {0.51234,0.48009,0.58033,0.53733},
	    {0.53277,0.50919,0.5711,0.53597}},{
	    {0.51009,0.47409,0.43211,0.41316},
	    {0.45758,0.4655,0.49349,0.36047},
	    {0.45455,0.51386,0.37637,0.34599},
	    {0.45009,0.45938,0.47267,0.34109}}},{{
	    {0.58147,0.49582,0.57037,0.47843},
	    {0.40931,0.52619,0.51894,0.49776},
	    {0.52098,0.49079,0.5236,0.5746},
	    {0.5011,0.44435,0.46818,0.53281}},{
	    {0.48926,0.44519,0.42704,0.39841},
	    {0.51055,0.44539,0.46123,0.38329},
	    {0.54625,0.53152,0.45712,0.3504},
	    {0.57401,0.48516,0.40324,0.35965}}},{{
	    {0.52916,0.58008,0.49645,0.55672},
	    {0.51228,0.55506,0.59478,0.57584},
	    {0.5598,0.49021,0.49185,0.57201},
	    {0.4688,0.52858,0.52325,0.51514}},{
	    {0.49937,0.46497,0.41605,0.40693},
	    {0.47506,0.47048,0.44949,0.40721},
	    {0.49271,0.47596,0.44041,0.39895},
	    {0.44821,0.4718,0.43138,0.36295}}},{{
	    {0.4918,0.46699,0.57979,0.52932},
	    {0.55769,0.53604,0.65559,0.49582},
	    {0.51844,0.50844,0.54143,0.57815},
	    {0.43999,0.46667,0.57485,0.52254}},{
	    {0.50169,0.47542,0.39707,0.38185},
	    {0.50924,0.47311,0.39824,0.34953},
	    {0.49114,0.45783,0.44473,0.40946},
	    {0.51603,0.43071,0.4741,0.35501}}},{{
	    {0.4926,0.49008,0.52404,0.45456},
	    {0.49771,0.4968,0.54331,0.54112},
	    {0.45677,0.49014,0.537,0.53572},
	    {0.46341,0.50932,0.58995,0.50982}},{
	    {0.47159,0.4615,0.39681,0.36881},
	    {0.44189,0.40323,0.46292,0.3699},
	    {0.47197,0.4136,0.47239,0.36576},
	    {0.4466,0.4384,0.47389,0.37719}}},{{
	    {0.47351,0.45473,0.52511,0.55014},
	    {0.4992,0.54192,0.52696,0.54268},
	    {0.5082,0.53629,0.54844,0.57538},
	    {0.53756,0.51909,0.54631,0.47716}},{
	    {0.52572,0.45112,0.42051,0.37135},
	    {0.49375,0.4172,0.40619,0.42758},
	    {0.52197,0.46314,0.41963,0.42208},
	    {0.53107,0.5101,0.46943,0.36592}}}};

    if(cand->getRichInd()<0) return -1.;     // NO RICH

    Float_t theta = cand->getTheta();
    Float_t phi = cand->getPhi();
    Int_t nSector = Int_t(phi/60.0F); // sector 0: 0°-60° internal labeling
    Float_t sectorPhi = phi- nSector*60.0F;
    Int_t iPhi = 0;
    while(sectorPhi > phiRangeMax[iPhi] && iPhi < iPhiMax-1) ++iPhi;
    Int_t nTheta = 0;
    while(theta > thetaRangeMax[nTheta] && nTheta < nThetaMax-1) ++nTheta;
    Int_t lowHighTheta = theta < thetaSwitch ? 0:1;
    Float_t momentum = cand->getMomentum();

    if(momentum < 0) momentum = 30;        // ASSUME LOW MOMENTUM

    Float_t widthTheta, widthPhi;
    Float_t offsetCor = offset[lowHighTheta][nSector];
    Int_t sys = cand->getSystemUsed();
    if(sys==-1) {
	if(theta < 45 ) sys=0;
        else            sys=1;
    }
    if(momentum < momCut[sys]) {
	Float_t momCor = (1.0F/momentum + offsetCor)/(1.0F/meanMom[lowHighTheta][0] + offsetCor);
	widthTheta = sigma1[nSector][0][iPhi][nTheta]*momCor;
	widthPhi = sigma1[nSector][1][iPhi][nTheta]*momCor;
    } else {
	Float_t momCor = (1.0F/momentum + offsetCor)/(1.0F/meanMom[lowHighTheta][1] + offsetCor);
	widthTheta = sigma2[nSector][0][iPhi][nTheta]*momCor;
	widthPhi = sigma2[nSector][1][iPhi][nTheta]*momCor;
    }
    Float_t deltaThetaNorm = cand->getDeltaTheta()/widthTheta;
    Float_t deltaPhiNorm = cand->getDeltaPhi()/widthPhi;
    return sqrt(deltaThetaNorm*deltaThetaNorm + deltaPhiNorm*deltaPhiNorm);
}


void HParticleAngleCor::calcSegVector(const Double_t z, const Double_t rho, const Double_t phi, const Double_t theta,
				      HGeomVector &base, HGeomVector &dir)
{
    // calculate position and direction vectors from a segment which
    // is defined by z-point and closest distance to z-axis (rho) and
    // by emission angles theta, phi. TMath::PiOver2() = pi/2
    base.setXYZ(rho*cos(phi+TMath::PiOver2()), rho*sin(phi+TMath::PiOver2()), z);
    dir.setXYZ(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
};

void   HParticleAngleCor::setMeanZTarget(const Float_t zValue)
{
    meanZTarget = zValue;
    zRichCenter = meanZTarget - 5.0F; // target shifted downstream by 5 mm to avoid conversion at RICH Flange
    zMirror     = zRichCenter -493.5F;
};

void HParticleAngleCor::setShield(const Bool_t shieldFlag)
{
    // activate / deactivate delta-electron shield.
    // This changes the multiple scattering point at small angles
    // by an increased radius of the sphere
    useShield = shieldFlag;
};

Bool_t HParticleAngleCor::setDefaults(const TString beamtime)
{
    // beamtime is a character string like "apr12".
    // If beamtime defaults are not implemented return value
    // is kFALSE otherwise kTRUE
    if (beamtime.CompareTo("apr12") == 0) {
	setShield(kTRUE);
	setUseMeanXYVertex();
	setMeanZTarget(-30.0F);
	return kTRUE;
    } else if (beamtime.CompareTo("oct12") == 0) {
	setShield(kFALSE);
	setUseMeanXYVertex();
	setMeanZTarget(-30.0F);
	return kTRUE;
    } else return kFALSE;
};

void HParticleAngleCor::setUseMeanXYVertex(void)
{
    // Use the center of the X-, Y- event vertex distribution.
    // For small target/beamspot diameters appropriate. Beamspot smaller
    // than global vertex resolution. Set mean Values, if different
    // from zero (default)
    useMeanXY = kTRUE;
};


void HParticleAngleCor::setUseEventXYVertex(void)
{
    // Use X-, Y- values from the global vertex determined event by event.
    // For large beam spots and large target diameters appropriate.
    // Good, if beam spot on target is larger than resolution of global vertex.
    useMeanXY = kFALSE;
};

Bool_t HParticleAngleCor::getMeanXYVertex(Float_t & xValue, Float_t & yValue)
{
    // return the mean x- and y- vertex points.
    // returns kTRUE, if these poins replace the x- and y values
    // of the global event vertex
    if(qxVertex.size() > 0) {
	yValue = sumYVertex/Float_t(qxVertex.size());
	xValue = sumXVertex/Float_t(qyVertex.size());
    } else {
	yValue = 0.0F;
	xValue = 0.0F;
    }
    return useMeanXY;
};

HGeomVector HParticleAngleCor::getGlobalVertex(Int_t v,Bool_t warn)
{
    //  return the global vertex position
    //  v can be Particle::kVertexCluster, Particle::kVertexSegment,
    //  Particle::kVertexParticle. In case of Particle::kVertexSegment
    //  and Particle::kVertexParticle the chi2 of the vertex fit is
    //  checked. If the the fit Particle::kVertexSegment failed the position of
    //  Particle::kVertexCluster is returned instead. If Particle::kVertexParticle
    //  fails Particle::kVertexSegment is used if possible otherwise fallback
    //  is Particle::kVertexCluster.

    if(!gHades) {
	HGeomVector vec;
        Error("getGlobalVertex()","No Hades object found !");
	return vec;
    }
    if(!gHades->getCurrentEvent()) {
	HGeomVector vec;
        Error("getGlobalVertex()","No Event structure found !");
	return vec;
    }
    if(v == Particle::kVertexCluster){
	return gHades->getCurrentEvent()->getHeader()->getVertexCluster().getPos();
    } else if (v == Particle::kVertexSegment){

	if(gHades->getCurrentEvent()->getHeader()->getVertex().getChi2() < 0){

	    if(warn)Warning("getGlobalVertex()","Vertex option v = %i  failed, return Particle::kVertexCluster !",v);
	    return gHades->getCurrentEvent()->getHeader()->getVertexCluster().getPos();
	}

	return gHades->getCurrentEvent()->getHeader()->getVertex().getPos();
    } else if (v == Particle::kVertexParticle ){

	if(gHades->getCurrentEvent()->getHeader()->getVertexReco().getChi2() < 0){

	    if(gHades->getCurrentEvent()->getHeader()->getVertex().getChi2() < 0){
		if(warn)Warning("getGlobalVertex()","Vertex option v = %i  failed, return Particle::kVertexCluster !",v);
		return gHades->getCurrentEvent()->getHeader()->getVertexCluster().getPos();
	    }
	    if(warn)Warning("getGlobalVertex()","Vertex option v = %i  failed, return Particle::kVertexSegment !",v);
	    return gHades->getCurrentEvent()->getHeader()->getVertex().getPos();
	}

	return gHades->getCurrentEvent()->getHeader()->getVertexReco().getPos();

    } else {
        Error("getGlobalVertex()","Vertex option v = %i  unknow, return Particle::kVertexSegment !",v);
	return gHades->getCurrentEvent()->getHeader()->getVertex().getPos();
    }

};



