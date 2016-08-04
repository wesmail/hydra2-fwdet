#ifndef __HKALDEF_H__
#define __HKALDEF_H__

namespace Kalman {

#define kNumKalStates 4 //! Number of different states for the Kalman filter: predicted, filtered, smoothed and inverse filtered state.
#define dafDebug      0 //! Debug level for DAF. 1 - additional consistency checks. 2 - print debug messages. Higher number -> more debug info.
#define kalDebug      0 //! Debug level for Kalman filter. 1 - additional consistency checks. 2 - print debug messages. Higher number -> more debug info.
#define metaDebug     0 //! Debug level for Meta matching.
#define rkDebug       0 //! Debug level for Runge-Kutta. 1 - additional consistency checks. 2 - print debug messages. Higher number -> more debug info.

    enum coordSys         {
        kSecCoord    = 0, // Sector coordinate system.
        kLayCoord    = 1  // Virtual layer coordinate system.
    };

    enum filtMethod       {
	kKalConv     = 0,
	kKalJoseph   = 1,
	kKalUD       = 2,
	kKalSeq      = 3,
	kKalSwer     = 4
    };

    enum kalHitTypes {
        kWireHit     = 0,  // Work with drift chamber hits.
        kSegHit      = 1   // Work with 2D segment hits.
    };

    enum kalFilterTypes   {
	kPredicted   = 0,
        kFiltered    = 1,
        kSmoothed    = 2,
        kInvFiltered = 3
    };

    enum kalStateIdx      {
	kIdxX0       = 0,
        kIdxY0       = 1,
        kIdxTanPhi   = 2, //? change names to tx, ty
        kIdxTanTheta = 3,
	kIdxQP       = 4,
        kIdxZ0       = 5
    };

    enum kalRotateOptions {
	kNoRot       = 0,
        kVarRot      = 1  // Rotate coordinate so that it points in inital track direction.
    };

    enum matIdx           {
	kMatIdxA         = 0,
        kMatIdxZ         = 1,
        kMatIdxDensity   = 2,
        kMatIdxExEner    = 3,
        kMatIdxRadLength = 4
    };

} // namespace

#endif //__HKALDEF_H__
