//*-- Author : Anar Rustamov
//HSplineTrack
//The output of HSplineTrackF task, like HKickTrack;
//There is no matching with META yet, there is the 
//matched MDC tracks corresponding momentum defined 
//from spline fitting;
#include "hsplinetrack.h"
using namespace std;

HSplineTrack::HSplineTrack() 
{
    qSpline=-1;
    numOfChambers=0;
}

ClassImp(HSplineTrack)
