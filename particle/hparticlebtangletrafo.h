#ifndef HPARTICLEBTANGLETRAFO_H
#define HPARTICLEBTANGLETRAFO_H

#include "TObject.h"
#include <TMath.h>
// version 1, Oct. 2013, W.Koenig
class HParticleBtAngleTrafo : public TObject {
protected:
    const static Int_t yParMax=6, thetaParMax=7, xParMax=2, xPadParMax=5, zParMax=6;
    Float_t yThetaPar[yParMax], yPadThetaPar[yParMax], yPadYPar[yParMax];
    Float_t thetaYPar[thetaParMax], thetaYPadPar[yParMax], yYPadPar[yParMax];
    Float_t xPar[xParMax], xPadPar[xPadParMax];
    Float_t dzPar[zParMax], dz2Par[zParMax], dzPadPar[zParMax], dz2PadPar[zParMax];
    Float_t dzYPar[zParMax], dz2YPar[zParMax];
    Float_t dzThetaPar[zParMax], dz2ThetaPar[zParMax],dzThetaPhiPar[2], dz2ThetaPhiPar[2];
    Float_t thetaOffset[3];     // 0: y fit, 1,2: yPad fit
    Float_t phiOffset;          // for theta yPad fit
    Float_t thetaSlope[2];      // correction for theta,phi->y: thetaSlope*theta*phi**2
    Float_t thetaPadSlope[6];   // correction for theta,phi->yPad: thetaPadSlope*theta*phi**2
    Float_t dyPadSlope[4];      // correction for x,y->yPad: dyPadSlope*dy*phi**2 and alike
    Float_t phiSlope[2];        // correction for theta,phi->y: phiSlope*phi**2
    Float_t phiPadSlope[4];     // 0-2: correction for theta,phi->yPad: phiPadSlope*phi**2; 3: y,phi->yPad
    Float_t phiThetaSlope[5];   // correction for y or yPad -> theta: phiThetaSlope*phi**2 (dPhi**2)
    Float_t padThetaSlope[4];   // correction for yPad -> theta: padThetaSlope*yPad*phi**2 (or dyPad**2)
    Float_t yThetaSlope[4];     // correction for y -> theta: yThetaSlope*y*phi**2 (or dy**2)
    Float_t yOffset[6];         // used for y,phi -> yPad, xPad, yPad -> Phi, z,y -> deltaTheta
    Float_t yProjOffset[2];     // used for y,phi -> theta
    Float_t yPadOffset[3];      // [0],[1]: yPad,phi -> theta, [2]: yPad,phi -> y
    Float_t xPadSize;           // constant value, does not depend on x nor y
    Float_t xPadOffset;         // x-pad number of the center of  a sector (phi=0)
    Float_t dxVertex;           // intermediate storage, calculated by zTheta2dY, called by zTheta2dX
    Float_t dxPadVertex;        // intermediate storage, calculated by zTheta2dYPad, called by zTheta2dXPad
                                // zRichCenter = aligned rich pos in simu (web interface) - 405mm (e.g. Rich at -440 -> richCenter = -35)
                                // dzTarg: shift used to define reference value for parametrization
    Float_t zRichCenter, dzTarg, zRef, zNorm, thetaRef, yRef;
    Float_t rad2deg ;

public:
    HParticleBtAngleTrafo(void) {initParam();};
    ~HParticleBtAngleTrafo(void) {};
    void initParam(void);

    Float_t x2xPad(const Float_t x)
    {
        //Converts x position to x pad number
	return x/xPadSize + xPadOffset;
    }
    Float_t xPad2x(const Float_t xPad)
    {
        //Converts x pad number to x position
	return (xPad-xPadOffset)*xPadSize;
    }
    Float_t yPhi2x(const Float_t y, const Float_t phi)
    {
        //Converts y position and phi angle to x position
	return -tan(phi/rad2deg)*(y*xPar[1]+xPar[0]);
    }
    Float_t yPhi2xPad(const Float_t y, const Float_t phi)
    {
        //Converts y position and phi angle to x pad number
	Float_t x = yPhi2x(y, phi);
	return x2xPad(x);
    }
    Float_t angles2y(const Float_t theta, const Float_t phi);
    Float_t yPhi2Theta(const Float_t y, const Float_t phi);

    Float_t angles2x(const Float_t theta, const Float_t phi)
    {
        //Converts theta and phi angle to x position
        Float_t y = angles2y(theta, phi);
        return yPhi2x(y, phi);
    }
    Float_t angles2yPad(const Float_t theta, const Float_t phi);
    Float_t yPadPhi2Theta(const Float_t yPad, const Float_t phi);

    Float_t angles2xPad(const Float_t theta, const Float_t phi)
    {
        //Convertes theta and phi angle to x pad number
        Float_t y = angles2y(theta, phi);
        return yPhi2xPad(y, phi);
    }
    Float_t xy2Phi(const Float_t x, const Float_t y)
    {
        //Converts x and y position to phi angle
	return atan(-x/(y*xPar[1]+xPar[0]))*rad2deg;
    }
    Float_t xPadyPad2Phi(const Float_t xPad, const Float_t yPad);

    Float_t yPad2y(const Float_t yPad)
    {
	// Converts y pad number to y position for phi = 8°
	// helper function, do not use!
	return xPadPar[0]+yPad*(xPadPar[1]+yPad*(xPadPar[2]+yPad*(xPadPar[3]+yPad*xPadPar[4])));
    }
    Float_t yPadPhi2x(const Float_t yPad, const Float_t phi)
    {
        //Converts y pad number and phi angle to x position
        return yPad2y(yPad) * tan(phi/rad2deg);
    }
   
    Float_t yPhi2yPad(const Float_t y, const Float_t phi);
    Float_t yPadPhi2y(const Float_t yPad, const Float_t phi);

    Float_t xy2yPad(const Float_t x, const Float_t y)
    {
        //Convert x and y position to y pad
        Float_t phi =  xy2Phi(x,y);
        return yPhi2yPad(y, phi);
    }
    Float_t xPadyPad2y(const Float_t xPad, const Float_t yPad)
    {
        //Converts x and y pad number to y position
        Float_t phi =  xPadyPad2Phi(xPad,yPad);
        return yPadPhi2y(yPad, phi);
    }
    Float_t xy2Theta(const Float_t x, const Float_t y)
    {
        //Converts x and y position to theta angle
	Float_t phi = xy2Phi(x,y);
        return yPhi2Theta(y,phi);
    }
    Float_t xPadyPad2Theta(const Float_t xPad, const Float_t yPad)
    {
        //Converts x and y pad to theta angle
	Float_t phi = xPadyPad2Phi(xPad,yPad);
        return yPadPhi2Theta(yPad,phi);
    }
    Float_t zTheta2dY(const Float_t z, const Float_t theta, const Float_t phi);
    Float_t zTheta2dYPad(const Float_t z, const Float_t theta, const Float_t phi);

    Float_t zTheta2dX()
    {
	//calculated by zTheta2dY
	return dxVertex;
    }
    Float_t zTheta2dXPad()
    {
	//calculated by zTheta2dYPad
	return dxPadVertex;
    }
    Float_t zY2dTheta(const Float_t z, const Float_t y, const Float_t x)
    {
        //Theta correction for given vertex position y and x position
	return zYPhi2dTheta(z, y, xy2Phi(x,y));
    }
    Float_t zYPad2dTheta(const Float_t z, const Float_t yPad, const Float_t xPad)
    {
	//Theta correction for given vertex position y and x pad number
	Float_t phi = xPadyPad2Phi(xPad,yPad);
	return zYPhi2dTheta(z, yPadPhi2y(yPad, phi), phi);
    }
    Float_t zYPhi2dTheta(const Float_t z, const Float_t y, const Float_t phi);

    Float_t zTheta2dTheta(const Float_t z, const Float_t theta, const Float_t phi);

    void setRichPos(const Float_t zRichPos) {zRichCenter = zRichPos;} // depends on Rich alagnment, default -35 mm
    Float_t getRichPos() {return zRichCenter;}
    void setRichPos(const char * beamtime); // available for "apr12", "oct11", "sep08"

    ClassDef(HParticleBtAngleTrafo,0)
};
#endif //HPARTICLEBTANGLETRAFO_H
