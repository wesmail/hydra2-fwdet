#include <iostream>
#include "hparticlebtangletrafo.h"

ClassImp(HParticleBtAngleTrafo)

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////
//
// calculate x,y (xPad,yPad) of a ring center on the Rich Padplane for a given theta and phi
// of a lepton track, Phi = 0: middle of the sector, sector edge: +-30°
// theta, phi in degree, x,y in mm, xPad, yPad in fraction of a pad
// target-center 5 mm downstream of rich-center (the latter normally shifted by about 35 mm upstream)
// Some transformations (angles2x, angles2xPad, xy2Phi,xy2yPad, xy2Theta, xy2yPadPad2x, zY2dTheta, zYPad2dTheta)
// are inline code implemented in hrichangles2xy.h. Refer to the '.h' file for full set of implemented transformations.
// Dependence on vertex position along the beam axis gives correction terms dy, dx or dyPad, dxPad.
// dx, dxPad are calculated simultaneously with dy, dyPad but called by special inline functions.
// The latter (zTheta2dx(), ztheta2dxPad() need a call to the corresponding functions zTheta2dy(), zTheta2dyPad() before.
// To correct for the vertex dependence of the theta, phi to y, x conversion requires to set the Rich Lab position (default -35 mm).
// This can be done via 'setRichPos(richLabPos)' function or by specifying a beamtime 'setRichPos("apr12")'
// Only Rich alignment for apr12, aug11 and sep08 is implemented by refering to beamtimes.
// The inverse Vertex correction deltaTheta calculated from z-vertex and x,y padplane is available as well.
// Version 2.0, Nov. 2013, W.Koenig
// V2.1: debugged V2.0, zTheta2dTheta added:
// correct Rich theta to match Track Theta as function of Vertex , Dec. 2013, W.Koenig
//
/////////////////////////////////////////////////////////////////////////////

Float_t HParticleBtAngleTrafo::yPhi2yPad(const Float_t y, const Float_t phi)
{
    //Converts y position and phi angle  to y pad number

    Float_t yPad = yPadYPar[0];
    Float_t yPower = 1.0F;
    for (Int_t i=1; i<yParMax;++i) {
	yPower *= y;
	yPad += yPadYPar[i]*yPower;
    }
    Float_t phi2 = phi*phi;
    yPad += (phiPadSlope[3] + dyPadSlope[0]*y)*phi2;
    if(y > yOffset[0]) {
	Float_t dy = y - yOffset[0];
	yPad += dyPadSlope[1]*phi2*dy*dy;
	Float_t yOff = yOffset[1] - 0.050*phi2;  // yPadYOffset[1]-0.050*phi2 = cos(phi)*yPadYOffset[1]
	if(y > yOff) {
	    dy = y - yOff;
	    yPad += dyPadSlope[2]*phi2*dy*dy;
	    yOff = yOffset[2] - 0.072*phi2;  // yPadYOffset[2]-0.072*phi2 = cos(phi)*yPadYOffset[2]
	    if(y > yOff) {
		dy = y - yOff;
		yPad += dyPadSlope[3]*phi2*dy*dy;
	    }
	}
    }
    return yPad;
}
Float_t HParticleBtAngleTrafo::yPadPhi2y(const Float_t yPad, const Float_t phi)
{
    //Converts y pad number and phi angle to y position

    Float_t y = yYPadPar[0];
    Float_t yPower = 1.0F;
    for (Int_t i=1; i<yParMax;++i) {
	yPower *= yPad;
	y += yYPadPar[i]*yPower;
    }
    Float_t phi2 = phi*phi;
    Float_t dyPad = yPad - yPadOffset[2];
    y += 1.14e-3F*phi2;
    if(dyPad > 0.0F) {
	y -= 6.85e-6F*dyPad*dyPad*phi2;
	Float_t y0 = 48.6F*(1.0F - 4.9e-4*phi2);
	Float_t dy0 = y0 - yPadOffset[2];
	dyPad = yPad - y0;
	y += 7.7e-7F*(dy0*dy0 - dyPad*dyPad)*phi2;
    } else {
	y -= 3.5e-6F*dyPad*dyPad*phi2;
    }
    return y;
}
Float_t HParticleBtAngleTrafo::angles2yPad(const Float_t theta, const Float_t phi)
{
    //Convertes theta and phi angle to y pad number

    Float_t yPad = yPadThetaPar[0];
    Float_t thetaPower = 1.0F;
    for (Int_t i=1; i<yParMax; ++i) {
	thetaPower *= theta;
	yPad += yPadThetaPar[i]*thetaPower;
    }
    Float_t phi2 = phi*phi;
    if(theta > thetaOffset[1]) {
	Float_t dTheta = theta-thetaOffset[1];
	yPad += ((thetaPadSlope[2]*dTheta+thetaPadSlope[3])*dTheta+phiPadSlope[2])*phi2;
	if(theta > thetaOffset[2]) {
	    dTheta = theta - thetaOffset[2];
	    yPad += (thetaPadSlope[4]*dTheta*dTheta)*phi2;
	}
    } else {
	yPad += (phiPadSlope[0] + (thetaPadSlope[0]+thetaPadSlope[5]*theta)*theta)*phi2;
    }
    Float_t dPhi = fabs(phi) - phiOffset;
    if(dPhi>0) yPad -= (phiPadSlope[1] + thetaPadSlope[1]*theta)*dPhi*dPhi;
    return yPad*cos(phi/rad2deg);
}
Float_t HParticleBtAngleTrafo::angles2y(const Float_t theta, const Float_t phi)
{
    //Converts theta and phi angle to y position

    Float_t y = yThetaPar[0];
    Float_t thetaPower = 1.0F;
    for (Int_t i=1; i<yParMax; ++i) {
	thetaPower *= theta;
	y += yThetaPar[i]*thetaPower;
    }
    Float_t phi2 = phi*phi;
    if(theta > thetaOffset[0]) {
	Float_t dTheta = theta-thetaOffset[0];
	y += (phiSlope[1] + thetaSlope[1]*dTheta*dTheta)*phi2;
    } else {
	y += (phiSlope[0]+thetaSlope[0]*theta)*phi2;
    }
    return y*cos(phi/rad2deg);
}
Float_t HParticleBtAngleTrafo::xPadyPad2Phi(const Float_t xPad, const Float_t yPad)
{
    //Converts x and y pad number to phi angle

    Float_t x = fabs(xPad2x(xPad));
    Float_t y = yPad2y(yPad);
    Float_t xyRatio = x/y;
    // tan(8°) = 0.1405
    Float_t fac = xyRatio - 0.1405F; // reference fit of xPadPar's at xyRatio = 0.1405
    if(fac > 0) {
	x -= 2.8F*fac*fac;
    }
    Float_t dx = x - xyRatio*yOffset[3];
    if(dx > 0.0F && xyRatio > 5.0e-4F) { // phi = 0.03 cuttoff: avoid division by nearly zero
	x += (8.0e-4F + 9.2e-5/xyRatio)*dx*dx*fac;
    }
    xyRatio = x/y;
    if(xPad > xPadOffset) xyRatio *= -1.0F;
    return atan(xyRatio)*rad2deg;

}
Float_t HParticleBtAngleTrafo::yPhi2Theta(const Float_t y, const Float_t phi)
{
    //Converts y position and phi angle to theta angle

    Float_t theta = thetaYPar[0];
    Float_t yPower = 1.0F;
    Float_t yProj = y/cos(phi/rad2deg);
    for (Int_t i=1; i<thetaParMax;++i) {
	yPower *= yProj;
	theta += thetaYPar[i]*yPower;
    }
    Float_t phi2 = phi*phi;
    theta += (phiThetaSlope[0] + yThetaSlope[0]*yProj)*phi2;
    Float_t dy = yProj - yProjOffset[0];
    if(dy > 0.0F) {
	theta += yThetaSlope[1]*dy*dy*phi2;
	dy = yProj - yProjOffset[1];
	if(dy > 0.0F) {
	    theta += yThetaSlope[2]*dy*dy*phi2;
	}
    }
    Float_t dPhi = fabs(phi)- phiOffset;
    if(dPhi>0.0F) theta += (phiThetaSlope[1] + yThetaSlope[3]*yProj)*dPhi*dPhi;
    return theta;
}
Float_t HParticleBtAngleTrafo::yPadPhi2Theta(const Float_t yPad, const Float_t phi)
{
    //Converts y pad number and phi angle to theta angle

    Float_t theta = thetaYPadPar[0];
    Float_t yPower = 1.0F;
    Float_t yProj = yPad/cos(phi/rad2deg);
    for (Int_t i=1; i<yParMax;++i) {
	yPower *= yProj;
	theta += thetaYPadPar[i]*yPower;
    }
    Float_t phi2 = phi*phi;
    theta += phiThetaSlope[2]*phi2;
    Float_t dyPad = yProj - yPadOffset[0];
    if(dyPad < 0.0F) {
	theta += padThetaSlope[0]*yProj*phi2;
    } else {
	theta += (phiThetaSlope[3] + (2.465e-6F + padThetaSlope[1]*dyPad)*dyPad)*phi2;
	dyPad = yProj - yPadOffset[1];
	if(dyPad >0.0F) theta += padThetaSlope[2]*dyPad*dyPad*phi2;
    }
    Float_t dPhi = fabs(phi) - phiOffset;
    if(dPhi>0.0F) theta += phiThetaSlope[4]*dPhi*dPhi;
    return theta;
}
Float_t HParticleBtAngleTrafo::zTheta2dY(const Float_t z, const Float_t theta, const Float_t phi)
{
    // calculate deviation of the y position of the ring-center as function of vertex position.
    // depends on theta but not on phi.
    Float_t dz = (z - zRef)/zNorm;;
    Float_t dz2 = dz*dz;
    Float_t dy = dzPar[0]*dz + dz2Par[0]*dz2;
    Float_t theta0 = theta - thetaRef;
    Float_t thetaPower = 1.0F;
    for (Int_t n=1; n<zParMax; ++n) {
	thetaPower *= theta0;
	dy += dzPar[n]*thetaPower*dz + dz2Par[n]*thetaPower*dz2;
    }
    Float_t phi2dz = phi*phi*dz;
    dy -= (9.581e-4F + 2.651e-5F*theta0)*phi2dz;
    Float_t phi0 = fabs(phi/rad2deg);
    dxVertex = -dy*sin(phi0) + 1.56e-4*phi2dz;
    if(phi < 0) dxVertex = -dxVertex;
    return dy*cos(phi0);
}
Float_t HParticleBtAngleTrafo::zTheta2dYPad(const Float_t z, const Float_t theta, const Float_t phi)
{
    // calculate deviation of the yPad position of the ring-center as function of vertex position.
    // depends on theta but not on phi.
    Float_t dz = (z - zRef)/zNorm;;
    Float_t dz2 = dz*dz;
    Float_t dyPad = dzPadPar[0]*dz + dz2PadPar[0]*dz2;
    Float_t theta0 = theta - thetaRef;
    Float_t thetaPower = 1.0F;
    for (Int_t n=1; n<zParMax; ++n) {
	thetaPower *= theta0;
	dyPad += dzPadPar[n]*thetaPower*dz + dz2PadPar[n]*thetaPower*dz2;
    }
    Float_t phi2dz = phi*phi*dz;
    dyPad -= (1.57e-4F + 5.0e-6F*theta0)*phi2dz;
    Float_t absPhi = fabs(phi);
    Float_t phi0 = absPhi/rad2deg;
    dxPadVertex = -dyPad*sin(phi0) + (7.78e-5 + 3.9e-6*theta0)*phi2dz;
    if( theta0 > 0.0F) dxPadVertex -= 4.330e-9*theta0*theta0*theta0*phi2dz;
    if (dz > 0.0F) {
	if(absPhi > 16.0F) absPhi = 32.0F - absPhi;
	dxPadVertex += (4.72e-4 + 2.08e-5*theta0)*absPhi*dz;
    }
    if(phi < 0) dxPadVertex = -dxPadVertex;
    return dyPad*cos(phi0);
}
Float_t HParticleBtAngleTrafo::zYPhi2dTheta(const Float_t z, const Float_t y, const Float_t phi)
{
    //Calculates theta correction for different vertices
    //for y position and phi angle as input
    Float_t dz = (z - zRef)/zNorm;;
    Float_t dz2 = dz*dz;
    Float_t dTheta = dzYPar[0]*dz + dz2YPar[0]*dz2;
    Float_t cosPhi = cos(phi/rad2deg);
    Float_t y0 = (y - yRef)/cosPhi;
    Float_t yPower = 1.0F;
    for (Int_t n=1; n<zParMax; ++n) {
	yPower *= y0;
	dTheta += dzYPar[n]*yPower*dz + dz2YPar[n]*yPower*dz2;
    }
    if (dz <0.0F) {
	dTheta -= 1.15e-4F*phi*phi*dz;
	if (y > yOffset[4]) {
	    Float_t dyPhi = (y-yOffset[4])/cosPhi*phi;
	    dTheta += 1.72e-8F*dyPhi*dyPhi*dz;
	}
    } else {
	dTheta -= 1.41e-4F*phi*phi*dz;
	if (y > yOffset[5]) {
	    Float_t dyPhi = (y-yOffset[5])/cosPhi*phi;
	    dTheta += 2.25e-8F*dyPhi*dyPhi*dz;
	}
    }
    return -dTheta;
}
Float_t HParticleBtAngleTrafo::zTheta2dTheta(const Float_t z, const Float_t theta, const Float_t phi)
{
    //Calculates theta correction for different vertices
    //for theta and phi angle as input
    Float_t dz = (z - zRef)/zNorm;;
    Float_t dz2 = dz*dz;
    Float_t theta0 = theta - thetaRef;
    Float_t dTheta = dzThetaPar[0]*dz + dz2ThetaPar[0]*dz2;
    Float_t thetaPower = 1.0F;
    for (Int_t n=1; n<zParMax; ++n) {
	thetaPower *= theta0;
	dTheta += dzThetaPar[n]*thetaPower*dz + dz2ThetaPar[n]*thetaPower*dz2;
    }
    dTheta += ((dzThetaPhiPar[0] + dzThetaPhiPar[1]*theta0)*dz +
	       (dz2ThetaPhiPar[0] + dz2ThetaPhiPar[1]*theta0)*dz2)*phi*phi;
    return dTheta;
}
void HParticleBtAngleTrafo::setRichPos(const char * beamtime)
{
    //Sets RICH position by consideration of beamtime
    if (strcmp(beamtime, "apr12") == 0) {zRichCenter = -35.1F; return;}
    if (strcmp(beamtime, "aug11") == 0) {zRichCenter = -36.52F; return;}
    if (strcmp(beamtime, "sep08") == 0) {zRichCenter = -33.5F; return;}
    Error("setRichPos()","Unknown Beamtime = %s !",beamtime);
    return;
}

void HParticleBtAngleTrafo::initParam()
{
    rad2deg = 57.29578F;


    {
	const Float_t yTheta[yParMax] =
	{-13.9019,8.2065,-0.00230282,0.000319631,-1.16559e-5,7.70673e-8};
	for (Int_t n=0; n<yParMax; ++n) yThetaPar[n]=yTheta[n];
    }
    {
	const Float_t thetaY[thetaParMax] =
	{-0.409201,0.172229,-0.000483011,2.35766e-6,-6.24602e-9,8.89312e-12,-5.03386e-15};
	for (Int_t n=0; n<thetaParMax; ++n) thetaYPar[n]=thetaY[n];
    }
    {
	const Float_t yPadY[yParMax] =
	{-9.20874,0.138557,-1.22646e-05,9.40742e-8,-1.00015e-10,1.01867e-13};
	for (Int_t n=0; n<yParMax; ++n) yPadYPar[n]=yPadY[n];
    }
    {
	const Float_t yYPad[yParMax] =
	{66.5283,7.27125,-0.00376257,-0.000119067, 2.75981e-7, 5.24191e-10};
	for (Int_t n=0; n<yParMax; ++n) yYPadPar[n]=yYPad[n];
    }
    {
	const Float_t yPadTheta[yParMax] =
	{-12.0324, 1.26572,-0.0080772,0.000266112,-3.98786e-6, 2.06571e-8};
	for (Int_t n=0; n<yParMax; ++n) yPadThetaPar[n]=yPadTheta[n];
    }
    {
	const Float_t thetaYPad[yParMax] =
	{10.0927, 0.829933, 0.00328383,-0.000130755,2.32883e-06,-1.35292e-08};
	for (Int_t n=0; n<yParMax; ++n) thetaYPadPar[n]=thetaYPad[n];
    }
    {
	const Float_t xParam[xParMax] = {12.83F, 0.9403F};
	for (Int_t n=0; n<xParMax; ++n) xPar[n]=xParam[n];

	const Float_t xPadParam[xPadParMax] = {75.5559,6.83342,-0.00328614,-0.000126939,4.19646e-7};
	for (Int_t n=0; n<xPadParMax; ++n) xPadPar[n]=xPadParam[n];
    }
    {
	const Float_t thOffset[3] = {50.0F, 43.0F, 71.0F}; // 0: y fit, 1,2: yPad fit
	for (Int_t n=0; n<3; ++n) thetaOffset[n]=thOffset[n];

	phiOffset = 12.0F;
	const Float_t dySlope[4] = {-5e-7, 2.21e-8, 2.964e-8, 4.12e-8}; // for x,y to yPad
	for (Int_t n=0; n<4; ++n) dyPadSlope[n] = dySlope[n];// slope *dy*phi**2

	const Float_t thSlope[6] = {-2.4e-5, 0.982e-5, 1.375e-6, 1.723e-6, 4.45e-6, 1.5e-7};
	for (Int_t n=0; n<6; ++n) thetaPadSlope[n] = thSlope[n];// correction for theta,phi->yPad

	thetaSlope[0] =-9.0e-6F;
	thetaSlope[1] = 6.2e-6F;

	phiSlope[0] = -0.00265;
	phiSlope[1] = phiSlope[0]+thetaOffset[0]*thetaSlope[0];

	phiPadSlope[0] = -0.00156; //for theta, phi to yPad
	phiPadSlope[1] = 0.00033;
	phiPadSlope[2] = phiPadSlope[0] + (thetaPadSlope[0]+thetaPadSlope[5]*thetaOffset[1])*thetaOffset[1];
	phiPadSlope[3] = -8.2e-5F; // for y to yPad
    }
    yOffset[0] = 190.0F; // used for y,phi -> yPad
    yOffset[1] = 350.0F; // used for y,phi -> yPad
    yOffset[2] = 470.0F; // used for y,phi -> yPad
    yOffset[3] = 234.7F; // used for xPad,yPad -> phi;
    yOffset[4] = 300.0F; // used for z,Y -> deltaTheta (vertex correction)
    yOffset[5] = 325.0F; // used for z,Y -> deltaTheta (vertex correction)

    xPadSize = 6.6F;
    xPadOffset = 47.5F;

    yProjOffset[0] =350.0F; // startpoints of y,phi**2 dependend theta correction
    yProjOffset[1] =500.0F;
    yPadOffset[0] = 40.0F;  // startpoints of yPad,phi**2 dependend theta correction
    yPadOffset[1] = 64.5F;
    yPadOffset[2] = 18.0F; //  yPad, phi -> y , (yPad-offset)**2 * phi**2 correction

    yThetaSlope[0] = 6.4e-7;
    yThetaSlope[1] = -1.57e-8F; // parametrisation to calculate theta from y
    yThetaSlope[2] = -6.125e-8F;
    yThetaSlope[3] = -9.01e-7F;

    padThetaSlope[0] = 1.188e-5F; // parametrisation to calculate theta from yPad
    padThetaSlope[1] = -1.191e-6F;
    padThetaSlope[2] = -2.05e-6F;

    phiThetaSlope[0] = 0.000214; // phi**2 dependent correction of theta calculated from y,yPad
    phiThetaSlope[1] = 0.000335;
    phiThetaSlope[2] = 0.00159;
    phiThetaSlope[3] = padThetaSlope[0] * yPadOffset[0];
    phiThetaSlope[4] = 0.00059;
    { // Parametrization of vertex dependance at zVertex - zRichCenter = +-20 mm as function of theta
	const Float_t dzParam[zParMax] =
	{5.6788,0.0958689,-0.00465984,-0.000112338,3.45014e-7,2.29705e-8};
	for (Int_t n=0; n<zParMax; ++n) dzPar[n] = dzParam[n];
	const Float_t dz2Param[zParMax] =
	{0.434261,0.0200656,-7.63692e-5,-1.84764e-5,2.7613e-8,1.13782e-8};
	for (Int_t n=0; n<zParMax; ++n) dz2Par[n] = dz2Param[n];
    }
    {
	const Float_t dzPadParam[zParMax] =
	{0.903236,0.0203734,-0.00061401,-2.12993e-5,-5.40314e-8,3.29523e-9};
	for (Int_t n=0; n<zParMax; ++n) dzPadPar[n] = dzPadParam[n];
	const Float_t dz2PadParam[zParMax] =
	{0.0713911,0.00374504,2.89311e-6,-3.25952e-6,2.16649e-9,2.08433e-9};
	for (Int_t n=0; n<zParMax; ++n) dz2PadPar[n] = dz2PadParam[n];
    }
    { // Parametrization of vertex dependance at zVertex - zRichCenter = +-22.5 mm
	// as function of y (det. plane)
	const Float_t dzYParam[zParMax] =
	{-0.871874,-0.00265706,1.45749e-05,9.64097e-8,1.33382e-10,-1.23551e-13};
	for (Int_t n=0; n<zParMax; ++n) dzYPar[n] = dzYParam[n];
	const Float_t dz2YParam[zParMax] =
	{-0.0599679,-0.000680895,-2.2915e-06,6.49625e-9,5.34e-11,7.35146e-14};
	for (Int_t n=0; n<zParMax; ++n) dz2YPar[n] = dz2YParam[n];
    }
    {
	const Float_t dzThParam[zParMax] =
	{-0.820714,-0.021845,0.00050965,2.57468e-5,1.19138e-7,-5.83638e-9};
	for (Int_t n=0; n<zParMax; ++n) dzThetaPar[n] = dzThParam[n];
	const Float_t dz2ThParam[zParMax] =
	{-0.0662483,-0.00375807,-9.81675e-6,3.5676e-6,1.2312e-8,-1.99097e-9};
	for (Int_t n=0; n<zParMax; ++n) dz2ThetaPar[n] = dz2ThParam[n];
	const Float_t dzThPhParam[2] = {0.0408321, 0.00150907};
	for (Int_t n=0; n<2; ++n) dzThetaPhiPar[n] = dzThPhParam[n]/256.0F;
	const Float_t dz2ThPhParam[2] = {0.00107495, 9.0156e-5};
	for (Int_t n=0; n<2; ++n) dz2ThetaPhiPar[n] = dz2ThPhParam[n]/256.0F;
    }
    // zRichCenter depends on alignment (beamtime). Rich pos. from Webinterface - 405 mm (Rich-Offset)
    zRichCenter = -35.0F; //used to determine parameters, depends on rich alignment
    dzTarg = 5.0F; // distance between rich center and target center, reference point for parametrization
    zRef = zRichCenter + dzTarg;
    zNorm = 22.5F;
    thetaRef = 50.0F;
    yRef = 400.0F;
}
