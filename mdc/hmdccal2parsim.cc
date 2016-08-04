//*-- AUTHOR : J. Markert

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HMdcCal2ParSim
//
// Container class for the calibration parameters from Cal1 to Cal2 of the MDC
// distance -> time.Contains functions for calculating distance->time for track fitter
// and Digitizer of Mdc
////////////////////////////////////////////////////////////////////////////

#include "hmdccal2parsim.h"
#include "hmdccal2par.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hmessagemgr.h"
#include "TH2.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TMath.h"

#include <stdlib.h>
#include <iostream> 
#include <iomanip>
using namespace std;

ClassImp(HMdcCal2ParAngleSim)
ClassImp(HMdcCal2ParModSim)
ClassImp(HMdcCal2ParSecSim)
ClassImp(HMdcCal2ParSim)



void HMdcCal2ParAngleSim::fillTime1(HMdcCal2ParAngleSim& r) {
    for(Int_t i=0;i<100;i++)
    {
	drifttime1[i]=r.drifttime1[i];
    }
}
void HMdcCal2ParAngleSim::fillTime2(HMdcCal2ParAngleSim& r) {
    for(Int_t i=0;i<100;i++)
    {
	drifttime2[i]=r.drifttime2[i];
    }
}
void HMdcCal2ParAngleSim::fillTime1Error(HMdcCal2ParAngleSim& r) {
    for(Int_t i=0;i<100;i++)
    {
	drifttime1Err[i]=r.drifttime1Err[i];
    }
}
void HMdcCal2ParAngleSim::fillTime2Error(HMdcCal2ParAngleSim& r) {
    for(Int_t i=0;i<100;i++)
    {
	drifttime2Err[i]=r.drifttime2Err[i];
    }
}


HMdcCal2ParModSim::HMdcCal2ParModSim(Int_t sec, Int_t mod, Int_t angle) {
  // constructor takes the sector, module and angle
    array = new TObjArray(angle);
    for (Int_t i=0; i<angle; ++i)
        array->AddAt(new HMdcCal2ParAngleSim(),i);
}

HMdcCal2ParModSim::~HMdcCal2ParModSim() {
  // destructor
  array->Delete();
  delete array;
}

HMdcCal2ParSecSim::HMdcCal2ParSecSim(Int_t sec, Int_t mod) {
  // constructor takes the sector, module number
  array = new TObjArray(mod);
  for (Int_t i=0; i<mod; i++)
        array->AddAt(new HMdcCal2ParModSim(sec,i),i);
}

HMdcCal2ParSecSim::~HMdcCal2ParSecSim() {
  // destructor
  array->Delete();
  delete array;
}

HMdcCal2ParSim::HMdcCal2ParSim(const Char_t* name,const Char_t* title,
                         const Char_t* context,Int_t n)
               : HParSet(name,title,context) {
  // constructor
  comment="no comment";
  strcpy(detName,"Mdc");
  if (gHades) {
      fMdc = (HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector("Mdc"));
  } else {
      fMdc = 0;
  }
  array = new TObjArray(n);
  for (Int_t i=0; i<n; i++) array->AddAt(new HMdcCal2ParSecSim(i),i);
  linecounter=0;
  linecounterwrite=0;
  type=0;
  slopeOutside=100.;  // 100 ns/mm

  useConstErr=kFALSE;
  useConstVD =kFALSE;
  for (Int_t i=0; i<4; i++)
  {
      scaleError[i]=1.;
      constError[i]=140.;
      constVD[i]   =40.;
  }
}

HMdcCal2ParSim::~HMdcCal2ParSim() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HMdcCal2ParSim::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
    linecounter=0;
    linecounterwrite=0;
    type=0;

    HDetParIo* input=inp->getDetParIo("HMdcParIo");
    if(!input) return kFALSE;
    Bool_t init=input->init(this,set);
    if(!init)  return kFALSE;
    if(!hasChanged()) return kTRUE;

    if(scaleError[0]!=1||scaleError[1]!=1||scaleError[2]!=1||scaleError[3]!=1)
    {
	cout<<"HMdcCal2ParSim: scaling errors of time1 by : "<<endl;
	cout<<"\t module 0 : "<<scaleError[0]<<endl;
	cout<<"\t module 1 : "<<scaleError[1]<<endl;
	cout<<"\t module 2 : "<<scaleError[2]<<endl;
	cout<<"\t module 3 : "<<scaleError[3]<<endl;

    }
    if(useConstErr)
    {
	cout<<"HMdcCal2ParSim::init(): const errors of time1 : "<<endl;
	cout<<"\t module 0 : "<<constError[0]<<endl;
	cout<<"\t module 1 : "<<constError[1]<<endl;
	cout<<"\t module 2 : "<<constError[2]<<endl;
	cout<<"\t module 3 : "<<constError[3]<<endl;
    }
    if(useConstVD)
    {
	cout<<"HMdcCal2ParSim::init(): const Drift velocity : "<<endl;
	cout<<"\t module 0 : "<<constVD[0]<<endl;
	cout<<"\t module 1 : "<<constVD[1]<<endl;
	cout<<"\t module 2 : "<<constVD[2]<<endl;
	cout<<"\t module 3 : "<<constVD[3]<<endl;
    }
    for(Int_t s=0;s<getSize();s++) {
	HMdcCal2ParSecSim& sec=(*this)[s];
	for(Int_t m=0;m<sec.getSize();m++) {
	    HMdcCal2ParModSim& mod=sec[m];
	    for(Int_t l=0;l<18;l++) {
		HMdcCal2ParAngleSim& rAngle=mod[l];
		for(Int_t i=0;i<100;i++)
		{
                    if(useConstVD)  rAngle.drifttime1   [i]=(1./ constVD[m])*i*100.;
                    if(useConstErr) rAngle.drifttime1Err[i]=constError[m];
		    else
		    {
                        if(scaleError[m]==1) continue;
			rAngle.drifttime1Err[i]=rAngle.drifttime1Err[i]*scaleError[m];
		    }
		}
	    }
	}
    }

    return kTRUE;
}
Double_t HMdcCal2ParSim::calcTime(Int_t s,Int_t m,Double_t a,Double_t dist) {
    // This function calculates the drift time for a given distance
    // to sense wire.A Interpolation between the two closest angle steps
    // is performed and the drift time value returned.
    // Input is the angle of the track (alphaTrack:90 degree for perpendicular impact),which
    // has to be recalculated to the angle of minimum drift distance (alphaDrDist:0 degree for
    // perpendicular impact).
    //   y ^
    //     |  |------------*----|              cathod  plane
    //     |  | cell        *   |
    //     |  |            / *  |
    //     |  |           /90 * |
    //     |  | driftDist/     *|
    //     |  |         / |     *
    //   --|--|--------*^-|-----|*--------->   sense/potential plane
    //     |  |           |     | *        x
    //     |  |      alphaDrDist|  *
    //        |                 |/  *          alphaDriftDist=90-alphaTrack
    //        |      alphaTrack /    *
    //        |-----------------|     * track  cathod plane
    //
    // angles must be between 0 and 90 degree, all other angles have to be shifted
    // before calling the function.

    if(!finite(a)||!finite(dist))
    {
	Error("calcTime()","received non finite value alpha %f dist %f!",a,dist);
        return -1;
    }
    Double_t angle=90.-a;
    Double_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined
  
    HMdcCal2ParAngleSim& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngleSim& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Double_t dminF=dist/0.1;
    Int_t dminI=Int_t(dminF); 

    Double_t finalTime;
    if(dminI>98)
    {   // distance outside matrix
	Double_t y1=(dist-9.9)*slopeOutside + rAngle.drifttime1[99];
	Double_t y2=(dist-9.9)*slopeOutside + rAngle2.drifttime1[99];

	Double_t t=angleStepD - angleStepI;

        finalTime=y1-t*(y1-y2);
	return finalTime;
    }

    //#################################### calc drift time ######################   
    Double_t y1=rAngle.drifttime1 [dminI];
    Double_t y2=rAngle2.drifttime1[dminI];
    Double_t y3=rAngle2.drifttime1[dminI+1];
    Double_t y4=rAngle.drifttime1 [dminI+1];

    Double_t t=angleStepD - angleStepI;
    Double_t u=dminF - dminI;

    finalTime=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
    return finalTime;
}
Double_t HMdcCal2ParSim::calcTimeErr(Int_t s,Int_t m,Double_t a,Double_t dist) {
    // This function calculates the Error of drift time1 for a given distance
    // to sense wire.A Interpolation between the two closest angle steps
    // is performed and the drift time value returned.
    // Input is the angle of the track (alphaTrack:90 degree for perpendicular impact),which
    // has to be recalculated to the angle of minimum drift distance (alphaDrDist:0 degree for
    // perpendicular impact).
    //   y ^
    //     |  |------------*----|              cathod  plane
    //     |  | cell        *   |
    //     |  |            / *  |
    //     |  |           /90 * |
    //     |  | driftDist/     *|
    //     |  |         / |     *
    //   --|--|--------*^-|-----|*--------->   sense/potential plane
    //     |  |           |     | *        x
    //     |  |      alphaDrDist|  *
    //        |                 |/  *          alphaDriftDist=90-alphaTrack
    //        |      alphaTrack /    *
    //        |-----------------|     * track  cathod plane
    //
    // angles must be between 0 and 90 degree, all other angles have to be shifted
    // before calling the function.
    if(!finite(a)||!finite(dist))
    {
	Error("calcTimeErr()","received non finite value alpha %f dist %f!",a,dist);
        return -1;
    }

    Double_t angle=90.-a;
    Double_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined
  
    HMdcCal2ParAngleSim& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngleSim& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Double_t dminF=dist/0.1;
    Int_t dminI=Int_t(dminF); 

    Double_t finalErr;
    if(dminI>98)
    {   // distance outside matrix
	Double_t y1= rAngle.drifttime1Err[99];
	Double_t y2= rAngle2.drifttime1Err[99];

	Double_t t=angleStepD - angleStepI;

        finalErr=y1-t*(y1-y2);
	return finalErr;
    }

    //#################################### calc drift time Error ######################
    Double_t y1=rAngle.drifttime1Err [dminI];
    Double_t y2=rAngle2.drifttime1Err[dminI];
    Double_t y3=rAngle2.drifttime1Err[dminI+1];
    Double_t y4=rAngle.drifttime1Err [dminI+1];

    Double_t t=angleStepD - angleStepI;
    Double_t u=dminF - dminI;

    finalErr=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
    return finalErr;
}
void HMdcCal2ParSim::calcTime(Int_t s,Int_t m,Double_t a,Double_t dist
			      ,Double_t*time1,Double_t*time1Err)
{
    // This function calculates the drift time for a given distance
    // to sense wire.A Interpolation between the two closest angle steps
    // is performed and the drift time value returned.
    // Input is the angle of the track (alphaTrack:90 degree for perpendicular impact),which
    // has to be recalculated to the angle of minimum drift distance (alphaDrDist:0 degree for
    // perpendicular impact).
    //   y ^
    //     |  |------------*----|              cathod  plane
    //     |  | cell        *   |
    //     |  |            / *  |
    //     |  |           /90 * |
    //     |  | driftDist/     *|
    //     |  |         / |     *
    //   --|--|--------*^-|-----|*--------->   sense/potential plane
    //     |  |           |     | *        x
    //     |  |      alphaDrDist|  *
    //        |                 |/  *          alphaDriftDist=90-alphaTrack
    //        |      alphaTrack /    *
    //        |-----------------|     * track  cathod plane
    //
    // angles must be between 0 and 90 degree, all other angles have to be shifted
    // before calling the function.
    // Time1 and Error of Time1 (no random smearing) are returned to Float pointers

    if(!finite(a)||!finite(dist))
    {
	Error("calcTime()","received non finite value alpha %f dist %f!",a,dist);
	*time1   =-1;
        *time1Err=-1;
	return ;
    }
    Double_t angle=90.-a;
    Double_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined
  
    HMdcCal2ParAngleSim& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngleSim& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Double_t dminF=dist/0.1;
    Int_t dminI=Int_t(dminF); 

    if(dminI>98)
    {   // distance outside matrix
	Double_t y1=(dist-9.9)*slopeOutside + rAngle.drifttime1[99];
	Double_t y2=(dist-9.9)*slopeOutside + rAngle2.drifttime1[99];

	Double_t t=angleStepD - angleStepI;

        *time1=y1-t*(y1-y2);

	y1=rAngle.drifttime1Err[99];
	y2=rAngle2.drifttime1Err[99];

        *time1Err=y1-t*(y1-y2);
    }
    else
    {
	//#################################### calc drift time ######################
	Double_t y1=rAngle.drifttime1 [dminI];
	Double_t y2=rAngle2.drifttime1[dminI];
	Double_t y3=rAngle2.drifttime1[dminI+1];
	Double_t y4=rAngle.drifttime1 [dminI+1];

	Double_t t=angleStepD - angleStepI;
	Double_t u=dminF - dminI;

	*time1=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);

	//################################ calc errors ######################################
	y1=rAngle.drifttime1Err [dminI];
	y2=rAngle2.drifttime1Err[dminI];
	y3=rAngle2.drifttime1Err[dminI+1];
	y4=rAngle.drifttime1Err [dminI+1];

	*time1Err=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
    }
}
Float_t HMdcCal2ParSim::calcTime(Int_t s,Int_t m,Float_t a,Float_t dist) {
    // This function calculates the drift time for a given distance
    // to sense wire.A Interpolation between the two closest angle steps
    // is performed and the drift time value returned.
    // Input is the angle of the track (alphaTrack:90 degree for perpendicular impact),which
    // has to be recalculated to the angle of minimum drift distance (alphaDrDist:0 degree for
    // perpendicular impact).
    //   y ^
    //     |  |------------*----|              cathod  plane
    //     |  | cell        *   |
    //     |  |            / *  |
    //     |  |           /90 * |
    //     |  | driftDist/     *|
    //     |  |         / |     *
    //   --|--|--------*^-|-----|*--------->   sense/potential plane
    //     |  |           |     | *        x
    //     |  |      alphaDrDist|  *
    //        |                 |/  *          alphaDriftDist=90-alphaTrack
    //        |      alphaTrack /    *
    //        |-----------------|     * track  cathod plane
    //
    // angles must be between 0 and 90 degree, all other angles have to be shifted
    // before calling the function.

    if(!finite(a)||!finite(dist))
    {
	Error("calcTime()","received non finite value alpha %f dist %f!",a,dist);
	return -1;
    }
    if((Int_t)((90-a)/5.)<0)
    {   // dummy fix for over flow
	a=angleDeg;
	dist=distance;
    }

    Float_t angle=90.-a;
    Float_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined
  
    HMdcCal2ParAngleSim& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngleSim& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Float_t dminF=dist/0.1;
    Int_t dminI=Int_t(dminF); 

    Float_t finalTime;
    if(dminI>98)
    {   // distance outside matrix
	Float_t y1=(dist-9.9)*slopeOutside + rAngle.drifttime1[99];
	Float_t y2=(dist-9.9)*slopeOutside + rAngle2.drifttime1[99];

	Float_t t=angleStepD - angleStepI;

        finalTime=y1-t*(y1-y2);
	return finalTime;
    }

    //#################################### calc drift time ######################   
    Float_t y1=rAngle.drifttime1 [dminI];
    Float_t y2=rAngle2.drifttime1[dminI];
    Float_t y3=rAngle2.drifttime1[dminI+1];
    Float_t y4=rAngle.drifttime1 [dminI+1];

    Float_t t=angleStepD - angleStepI;
    Float_t u=dminF - dminI;

    finalTime=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);

    distance=dist; // just to cover the over flow
    angleDeg=a;

    return finalTime;
}
Float_t HMdcCal2ParSim::calcTimeErr(Int_t s,Int_t m,Float_t a,Float_t dist) {
    // This function calculates the Error of drift time1 for a given distance
    // to sense wire.A Interpolation between the two closest angle steps
    // is performed and the drift time value returned.
    // Input is the angle of the track (alphaTrack:90 degree for perpendicular impact),which
    // has to be recalculated to the angle of minimum drift distance (alphaDrDist:0 degree for
    // perpendicular impact).
    //   y ^
    //     |  |------------*----|              cathod  plane
    //     |  | cell        *   |
    //     |  |            / *  |
    //     |  |           /90 * |
    //     |  | driftDist/     *|
    //     |  |         / |     *
    //   --|--|--------*^-|-----|*--------->   sense/potential plane
    //     |  |           |     | *        x
    //     |  |      alphaDrDist|  *
    //        |                 |/  *          alphaDriftDist=90-alphaTrack
    //        |      alphaTrack /    *
    //        |-----------------|     * track  cathod plane
    //
    // angles must be between 0 and 90 degree, all other angles have to be shifted
    // before calling the function.
    if(!finite(a)||!finite(dist))
    {
	Error("calcTimeErr()","received non finite value alpha %f dist %f!",a,dist);
	return -1;
    }

    Float_t angle=90.-a;
    Float_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined

    HMdcCal2ParAngleSim& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngleSim& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Float_t dminF=dist/0.1;
    Int_t dminI=Int_t(dminF); 

    Float_t finalErr;
    if(dminI>98)
    {   // distance outside matrix

        Float_t t=angleStepD - angleStepI;

	Float_t y1=rAngle.drifttime1Err[99];
	Float_t y2=rAngle2.drifttime1Err[99];

	finalErr=y1-t*(y1-y2);
	return finalErr;
    }
    else
    {
	//################################ calc errors ######################################

        Float_t t=angleStepD - angleStepI;
	Float_t u=dminF - dminI;

        Float_t y1=rAngle.drifttime1Err [dminI];
	Float_t y2=rAngle2.drifttime1Err[dminI];
	Float_t y3=rAngle2.drifttime1Err[dminI+1];
	Float_t y4=rAngle.drifttime1Err [dminI+1];

	finalErr=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
        return finalErr;
    }
}

Float_t HMdcCal2ParSim::calcTime2(Int_t s,Int_t m,Float_t a,Float_t dist) {
    // This function calculates the drift time2 for a given distance
    // to sense wire.A Interpolation between the two closest angle steps
    // is performed and the drift time value returned.
    // Input is the angle of the track (alphaTrack:90 degree for perpendicular impact),which
    // has to be recalculated to the angle of minimum drift distance (alphaDrDist:0 degree for
    // perpendicular impact).
    //   y ^
    //     |  |------------*----|              cathod  plane
    //     |  | cell        *   |
    //     |  |            / *  |
    //     |  |           /90 * |
    //     |  | driftDist/     *|
    //     |  |         / |     *
    //   --|--|--------*^-|-----|*--------->   sense/potential plane
    //     |  |           |     | *        x
    //     |  |      alphaDrDist|  *
    //        |                 |/  *          alphaDriftDist=90-alphaTrack
    //        |      alphaTrack /    *
    //        |-----------------|     * track  cathod plane
    //
    // angles must be between 0 and 90 degree, all other angles have to be shifted
    // before calling the function.

    Float_t angle=90.-a;
    Float_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined

    HMdcCal2ParAngleSim& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngleSim& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Float_t dminF=dist/0.1;
    Int_t dminI=Int_t(dminF); 

    Float_t finalTime;

    if(dminI>98)
    {   // distance outside matrix
	Float_t y1=(dist-9.9)*slopeOutside + rAngle.drifttime2[99];
	Float_t y2=(dist-9.9)*slopeOutside + rAngle2.drifttime2[99];

	Float_t t=angleStepD - angleStepI;

        finalTime=y1-t*(y1-y2);
        return finalTime;

    }
    else
    {
	//################################ calc time2 ######################################

	Float_t y1=rAngle.drifttime2 [dminI];
	Float_t y2=rAngle2.drifttime2[dminI];
	Float_t y3=rAngle2.drifttime2[dminI+1];
	Float_t y4=rAngle.drifttime2 [dminI+1];

	Float_t t=angleStepD - angleStepI;
	Float_t u=dminF - dminI;

	finalTime=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
        return finalTime;
    }
}
Float_t HMdcCal2ParSim::calcTime2Err(Int_t s,Int_t m,Float_t a,Float_t dist) {
    // This function calculates the Error drift time2 for a given distance
    // to sense wire.A Interpolation between the two closest angle steps
    // is performed and the drift time value returned.
    // Input is the angle of the track (alphaTrack:90 degree for perpendicular impact),which
    // has to be recalculated to the angle of minimum drift distance (alphaDrDist:0 degree for
    // perpendicular impact).
    //   y ^
    //     |  |------------*----|              cathod  plane
    //     |  | cell        *   |
    //     |  |            / *  |
    //     |  |           /90 * |
    //     |  | driftDist/     *|
    //     |  |         / |     *
    //   --|--|--------*^-|-----|*--------->   sense/potential plane
    //     |  |           |     | *        x
    //     |  |      alphaDrDist|  *
    //        |                 |/  *          alphaDriftDist=90-alphaTrack
    //        |      alphaTrack /    *
    //        |-----------------|     * track  cathod plane
    //
    // angles must be between 0 and 90 degree, all other angles have to be shifted
    // before calling the function.

    Float_t angle=90.-a;
    Float_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined

    HMdcCal2ParAngleSim& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngleSim& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Float_t dminF=dist/0.1;
    Int_t dminI=Int_t(dminF); 

    Float_t finalErr;
    if(dminI>98)
    {   // distance outside matrix
	Float_t t=angleStepD - angleStepI;

	Float_t y1=rAngle.drifttime2Err[99];
	Float_t y2=rAngle2.drifttime2Err[99];

	finalErr=y1-t*(y1-y2);
        return finalErr;
    }
    else
    {
	//################################ calc errors ######################################
 	Float_t t=angleStepD - angleStepI;
	Float_t u=dminF - dminI;

        Float_t y1=rAngle.drifttime2Err [dminI];
	Float_t y2=rAngle2.drifttime2Err[dminI];
	Float_t y3=rAngle2.drifttime2Err[dminI+1];
	Float_t y4=rAngle.drifttime2Err [dminI+1];

	finalErr=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
        return finalErr;
    }
}
Float_t HMdcCal2ParSim::calcDriftVelocity(Int_t s,Int_t m,Float_t a,Float_t dist) {
    // This function calculates the drift velocity for a given distance
    // to sense wire and impact angle. Vd [mu/ns] = deltaT1/deltaDist

    Float_t finalVd=0;
    Float_t t1_1=0;
    Float_t t1_2=0;
    Float_t mystep=0.1; // mm->100mu

    if(dist>=0)
    {
	t1_1=calcTime(s,m,a,dist);
        t1_2=calcTime(s,m,a,dist+mystep);
    }
    if(t1_1-t1_2!=0)finalVd=1000*fabs(mystep/(t1_1-t1_2)); // mu/ns

    return finalVd;
}
Float_t HMdcCal2ParSim::calcResolution(Int_t s,Int_t m,Float_t a,Float_t dist) {
    // This function calculates the reslution in nm for a given distance
    // to sense wire and impact angle. Res[nm]=timeErr/Vd.

    Float_t finalRes=0;

    Float_t Vd=calcDriftVelocity(s,m,a,dist);
    if(Vd>0)
    {
      finalRes=fabs(Vd*calcTimeErr(s,m,a,dist)); // (mu/ns)*ns=>mu
    }

    return finalRes;
}
void HMdcCal2ParSim::calcTimeDigitizer(Int_t s,Int_t m,Float_t a,
				       Float_t dist,Float_t*time1,Float_t*time1Err) {
    // This function calculates the drift time for a given distance
    // to sense wire.A Interpolation between the two closest angle steps
    // is performed and the drift time value returned.
    // Input is the angle of the track (alphaTrack:90 degree for perpendicular impact),which
    // has to be recalculated to the angle of minimum drift distance (alphaDrDist:0 degree for
    // perpendicular impact).
    //   y ^
    //     |  |------------*----|              cathod  plane
    //     |  | cell        *   |
    //     |  |            / *  |
    //     |  |           /90 * |
    //     |  | driftDist/     *|
    //     |  |         / |     *
    //   --|--|--------*^-|-----|*--------->   sense/potential plane
    //     |  |           |     | *        x
    //     |  |      alphaDrDist|  *
    //        |                 |/  *          alphaDriftDist=90-alphaTrack
    //        |      alphaTrack /    *
    //        |-----------------|     * track  cathod plane
    //
    // angles must be between 0 and 90 degree, all other angles have to be shifted
    // before calling the function.

    Float_t angle=90.-a;
    Float_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined

    HMdcCal2ParAngleSim& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngleSim& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Float_t dminF=dist/0.1;
    Int_t dminI=Int_t(dminF); 

    if(dminI>98)
    {   // distance outside matrix
	Float_t y1=(dist-9.9)*slopeOutside + rAngle.drifttime1[99];
	Float_t y2=(dist-9.9)*slopeOutside + rAngle2.drifttime1[99];

	Float_t t=angleStepD - angleStepI;

        *time1=y1-t*(y1-y2);

	y1=rAngle.drifttime1Err[99];
	y2=rAngle2.drifttime1Err[99];

	Float_t TimeResolution=y1-t*(y1-y2);
	Float_t gauss=gRandom->Gaus(0,1); // random value between -1 and 1

	*time1Err=TimeResolution*gauss;

    }
    else
    {
	//################################ calc time1 ######################################

	Float_t y1=rAngle.drifttime1 [dminI];
	Float_t y2=rAngle2.drifttime1[dminI];
	Float_t y3=rAngle2.drifttime1[dminI+1];
	Float_t y4=rAngle.drifttime1 [dminI+1];

	Float_t t=angleStepD - angleStepI;
	Float_t u=dminF - dminI;

	Float_t Time1=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);

	//################################ calc errors ######################################
	y1=rAngle.drifttime1Err [dminI];
	y2=rAngle2.drifttime1Err[dminI];
	y3=rAngle2.drifttime1Err[dminI+1];
	y4=rAngle.drifttime1Err [dminI+1];

	Float_t TimeResolution=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
	Float_t gauss=gRandom->Gaus(0,1); // random value between -1 and 1

	*time1=Time1;
	*time1Err=TimeResolution*gauss;
    }
}
void HMdcCal2ParSim::calcTime2Digitizer(Int_t s,Int_t m,Float_t a,
					Float_t dist,Float_t*time2,Float_t*time2Err) {
    // This function calculates the drift time for a given distance
    // to sense wire.A Interpolation between the two closest angle steps
    // is performed and the drift time value returned.
    // Input is the angle of the track (alphaTrack:90 degree for perpendicular impact),which
    // has to be recalculated to the angle of minimum drift distance (alphaDrDist:0 degree for
    // perpendicular impact).
    //   y ^
    //     |  |------------*----|              cathod  plane
    //     |  | cell        *   |
    //     |  |            / *  |
    //     |  |           /90 * |
    //     |  | driftDist/     *|
    //     |  |         / |     *
    //   --|--|--------*^-|-----|*--------->   sense/potential plane
    //     |  |           |     | *        x
    //     |  |      alphaDrDist|  *
    //        |                 |/  *          alphaDriftDist=90-alphaTrack
    //        |      alphaTrack /    *
    //        |-----------------|     * track  cathod plane
    //
    // angles must be between 0 and 90 degree, all other angles have to be shifted
    // before calling the function.

    Float_t angle=90.-a;
    Float_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined

    HMdcCal2ParAngleSim& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngleSim& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Float_t dminF=dist/0.1;
    Int_t dminI=Int_t(dminF); 

    if(dminI>98)
    {   // distance outside matrix
	Float_t y1=(dist-9.9)*slopeOutside + rAngle.drifttime2[99];
	Float_t y2=(dist-9.9)*slopeOutside + rAngle2.drifttime2[99];

	Float_t t=angleStepD - angleStepI;

        *time2=y1-t*(y1-y2);

	y1=rAngle.drifttime2Err[99];
	y2=rAngle2.drifttime2Err[99];

	Float_t TimeResolution=y1-t*(y1-y2);
	Float_t gauss=gRandom->Gaus(0,1); // random value between -1 and 1

	*time2Err=TimeResolution*gauss;

    }
    else
    {

	//################################ calc time2 ######################################

	Float_t y1=rAngle.drifttime2 [dminI];
	Float_t y2=rAngle2.drifttime2[dminI];
	Float_t y3=rAngle2.drifttime2[dminI+1];
	Float_t y4=rAngle.drifttime2 [dminI+1];

	Float_t t=angleStepD - angleStepI;
	Float_t u=dminF - dminI;

	Float_t Time2=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);

	//################################ calc errors ######################################
	y1=rAngle.drifttime2Err [dminI];
	y2=rAngle2.drifttime2Err[dminI];
	y3=rAngle2.drifttime2Err[dminI+1];
	y4=rAngle.drifttime2Err [dminI+1];

	Float_t Time2Resolution=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
	Float_t gauss=gRandom->Gaus(0,1); // random value between -1 and 1

	*time2=Time2;
	*time2Err=Time2Resolution*gauss;
    }
}
void HMdcCal2ParSim::transformToDistance(Int_t s,Int_t m,Int_t a,Float_t binsizetime,HMdcCal2Par* cal2par)
{
 // fills arrays of MdcCal2Par (time) from arrays of MdcCal2ParSim (distance)
    Float_t time1_1=0;
    Float_t time1_2=0;
    Float_t time1_1Err=0;
    Float_t time1_2Err=0;
    Int_t timebin =0;
    Float_t dsearch   =0; // mm
    Float_t dsearchErr=0; // ns
    Float_t vdrift=0;     // 0.1 *mm/ns
    Float_t unit=0.1;
    Float_t dsearchold=0;    // mm
    Float_t dsearchErrold=0; // mm

    for(Int_t b=1;b<100;b++)
    {   // time bin (4ns)
	time1_1=0;
	time1_2=0;
	time1_1Err=0;
	time1_2Err=0;
	timebin =0;
	dsearch   =0;
	dsearchErr=0;
	vdrift=0;
   
	while(time1_1<b*binsizetime && timebin<99)
	{   // find bin which corresponds to given time
	    timebin++; // bin number in htime (0.1mm)
	    time1_1 = (*this)[s][m][a].drifttime1[timebin];
	}
	// doing interpolation between two closest points
	time1_2    = (*this)[s][m][a].drifttime1   [timebin];
	time1_2Err = (*this)[s][m][a].drifttime1Err[timebin];
        time1_1    = (*this)[s][m][a].drifttime1   [timebin-1];
	time1_1Err = (*this)[s][m][a].drifttime1Err[timebin-1];

	if( (time1_2-time1_1)!=0)
	{
	    dsearch    = ((b*binsizetime - time1_1)*(unit/(time1_2-time1_1))) + (timebin-1)*unit;

	    if(dsearch==dsearchold)
	    {  // just check if value does not change (should never be the case)
		gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
				       ,"dsearch==dsearchold %i %i %i vd (um/ns) %f ds %f es (ns) %f e1 %f e2 %f res (um) %f"
				       ,m,a,b,vdrift*1000,dsearch,dsearchErr,time1_1Err,time1_2Err,dsearchErr*vdrift*1000);
	    }

	    if( (time1_2Err-time1_1Err)!=0 || useConstErr)
	    {
		if(!useConstErr){
		    dsearchErr = time1_1Err + ( (time1_2Err-time1_1Err)/unit)*(dsearch-(timebin-1)*unit);
		    dsearchErrold=dsearchErr;
		} else {
		    dsearchErr = dsearchold = time1_1Err;
		}

	    }
	    else
	    {   // take the error from previous point
		if(dsearch==dsearchold)
		{
		    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
					   ,"take the error from previous point %i %i %i vd (um/ns) %f ds %f es(ns) %f e1 %f e2 %f res(um) %f"
					   ,m,a,b,vdrift*1000,dsearch,dsearchErr,time1_1Err,time1_2Err,dsearchErr*vdrift*1000);
		}
		dsearchErr=dsearchErrold;
	    }
	    vdrift= (unit/ (time1_2-time1_1)); //

            (*cal2par)[s][m][a].distance   [b]=dsearch;
            (*cal2par)[s][m][a].distanceErr[b]=dsearchErr*vdrift;

	    if(dsearch==0)
	    {
		gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
				       ,"dsearch==0 %i %i %i e1 %f e2 %f t1 %f t2 %f"
				       ,m,a,b,time1_1Err,time1_2Err,time1_1,time1_2);
	    }
	}
	else
	{
	    if(b>2)
	    {
		gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
				       ,"time1_2-time1_1==0 %i %i %i e1 %f e2 %f t1 %f t2 %f"
				       ,m,a,b,time1_1Err,time1_2Err,time1_1,time1_2);
	    }
	}
    }
    // doing linear interpolation between first real value and 0
    // to avoid natsty peak at 0
    Int_t first=0;
    while((*cal2par)[s][m][a].distance[first]==0)
    {
	first++;
    }
    if(first>0)
    {
        Float_t slope= (*cal2par)[s][m][a].distance[first]/first;
	for(Int_t bin=0;bin<first;bin++)
	{
	    (*cal2par)[s][m][a].distance   [bin]=bin*slope;
            (*cal2par)[s][m][a].distanceErr[bin]=(*cal2par)[s][m][a].distanceErr[first];
	}
    }
}
void HMdcCal2ParSim::plot(Int_t s,Int_t m)
{
    // plots data into TGraphErrors for time1 and time2 including errors

    cout<<"HMdcCal2ParSim: plotting sector "<<s<<" module "<<m<<endl;

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetPalette(1);

    Char_t nameCanvas[300];
    sprintf(nameCanvas,"%s %i %s %i","distance -> time sector ",s," module ",m);

    TH2F* dummy=new TH2F("dummy","dummy",2,0,10.1,2,0,700);
    dummy->SetXTitle("distance from wire [mm]");
    dummy->SetYTitle("drift time [ns]");

    TCanvas* result=new TCanvas(nameCanvas,nameCanvas,1000,800);
    result->Divide(6,3);
    result->Draw();
    Float_t x[100];
    Float_t y[100];
    Float_t ex[100];
    Float_t ey[100];
    Float_t x2[100];
    Float_t y2[100];
    Float_t ex2[100];
    Float_t ey2[100];

    for(Int_t a=0;a<18;a++)
    {
	for(Int_t i=0;i<100;i++)
	{
		x[i] =i*0.1;
		y[i] =(*this)[s][m][a].drifttime1[i];
		ex[i]=0;
		ey[i]=(*this)[s][m][a].drifttime1Err[i];
		x2[i] =i*0.1;
		y2[i] =(*this)[s][m][a].drifttime2[i];
		ex2[i]=0;
		ey2[i]=(*this)[s][m][a].drifttime2Err[i];
	}
	result->cd(a+1);
        dummy->DrawCopy();
	TGraphErrors* g=new TGraphErrors(100,x,y,ex,ey);
	g->SetLineColor(2);
        g->Draw();
        TGraphErrors* g2=new TGraphErrors(100,x2,y2,ex2,ey2);
	g2->SetLineColor(4);
        g2->Draw();
    }
    result->Update();
    delete dummy;
}
void HMdcCal2ParSim::plot2D(Int_t s,Int_t m,Int_t type)
{
    // Plots data into 2d-Hists
    // type = 0 ->time1
    // type = 1 ->Error time1
    // type = 2 ->time2
    // type = 3 ->Error time2

    cout<<"HMdcCal2ParSim: plotting 2D sector "<<s<<" module "<<m<<endl;

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetPalette(1);

    Char_t nameCanvas[300];
    if(type==0)sprintf(nameCanvas,"%s %i %s %i","time1 sector ",s," module ",m);
    if(type==1)sprintf(nameCanvas,"%s %i %s %i","Error time1 sector ",s," module ",m);
    if(type==2)sprintf(nameCanvas,"%s %i %s %i","time2 sector ",s," module ",m);
    if(type==3)sprintf(nameCanvas,"%s %i %s %i","Error time2 sector ",s," module ",m);

    TH2F* dummy=new TH2F("dummy","dummy",18,0,90,100,0,10);
   
    dummy->SetXTitle("angle");
    dummy->SetYTitle("distance from wire [mm]");

    if(type==0)dummy->SetZTitle("time1 [ns]");
    if(type==1)dummy->SetZTitle("Error time1 [ns]");
    if(type==2)dummy->SetZTitle("time2 [ns]");
    if(type==3)dummy->SetZTitle("Error time2 [ns]");

    TCanvas* result=new TCanvas(nameCanvas,nameCanvas,1000,800);

    for(Int_t a=0;a<18;a++)
    {
	for(Int_t i=0;i<100;i++)
	{
	    if(type==0)dummy->SetBinContent(a+1,i,(*this)[s][m][a].drifttime1[i]);
            if(type==1)dummy->SetBinContent(a+1,i,(*this)[s][m][a].drifttime1Err[i]);
            if(type==2)dummy->SetBinContent(a+1,i,(*this)[s][m][a].drifttime2[i]);
            if(type==3)dummy->SetBinContent(a+1,i,(*this)[s][m][a].drifttime2Err[i]);
	}
    }
    result->cd();
    dummy->DrawCopy("lego2");

    result->Update();
    delete dummy;
}
Int_t HMdcCal2ParSim::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HMdcParIo");
  if (out) return out->write(this);
  return -1;
}
void HMdcCal2ParSim::readline(const Char_t* buf, Int_t* set) {
  // decodes one line read from ascii file I/O and calls HMdcCal2ParAngleSim::fillTime1(...)
  Int_t   sec,mod,angle,type;
  Float_t par1,par2,par3,par4,par5,par6,par7,par8,par9,par10;

  if(linecounter%10==0) linecounter=0;

  sscanf(buf,"%i%i%i%i%*s",&sec, &mod, &angle, &type);
  sscanf(&buf[9],"%f%f%f%f%f%f%f%f%f%f",
         &par1, &par2,
	 &par3, &par4, &par5,
	 &par6, &par7, &par8,
	 &par9, &par10 );

  Int_t n=sec*4+mod;

  if (!set[n]) return;
  HMdcCal2ParAngleSim& rAngle=(*this)[sec][mod][angle];
  switch (type){
  case  0: rAngle.fillTime1(linecounter,par1,par2,par3,par4,par5,par6,par7,par8,par9,par10);
  break;
  case  1:rAngle.fillTime1Error(linecounter,par1,par2,par3,par4,par5,par6,par7,par8,par9,par10);
  break;
  case  2:rAngle.fillTime2(linecounter,par1,par2,par3,par4,par5,par6,par7,par8,par9,par10);
  break;
  case  3:rAngle.fillTime2Error(linecounter,par1,par2,par3,par4,par5,par6,par7,par8,par9,par10);
  break;
  default : Error("HMdcCal2Parsim::readline()","argument \"type\" out of range");
  break;
  }
  linecounter++;

  set[n]=999;
}

void HMdcCal2ParSim::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HMdcParAsciiFileIo
  header=
	"# Cal2 Calibration parameters of the MDC\n"
	"# distance->time\n"
	"# Format: type: 0=time1, 1=time2, 2=variation of time1, 3=variation of time2\n"
	"# sector  module  angle type par0 par1 par2 par3 par4 par5 pa6 par7 par8 par9 par10\n";
}

Bool_t HMdcCal2ParSim::writeline(Char_t *buf, Int_t sec, Int_t mod, Int_t angle) {
  // writes one line to the buffer used by ascii file I/O
 Bool_t r = kTRUE;

 if (fMdc) {
     if (fMdc->getModule(sec,mod) != 0) {
	 if  (sec>-1 && sec<getSize()) {
	     HMdcCal2ParSecSim &sector = (*this)[sec];
	     if (mod>-1 && mod<sector.getSize()) {

		 HMdcCal2ParAngleSim& rAngle=(*this)[sec][mod][angle];
		 Char_t dummy[20];

		 if(linecounterwrite%10==0&&linecounterwrite>1)
		 {
		     linecounterwrite=0;
		     type++;
		 }
                 if(type>3)
		 {
		    type=0;
		 }
		 sprintf(dummy,"%1i %1i %2i %1i",sec, mod, angle, type);
		 switch (type){
		 case 0:
		     sprintf(buf,"%s %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f\n",
			     dummy,
			     rAngle.getDriftTime1(linecounterwrite,0),rAngle.getDriftTime1(linecounterwrite,1),
			     rAngle.getDriftTime1(linecounterwrite,2),rAngle.getDriftTime1(linecounterwrite,3),
			     rAngle.getDriftTime1(linecounterwrite,4),rAngle.getDriftTime1(linecounterwrite,5),
			     rAngle.getDriftTime1(linecounterwrite,6),rAngle.getDriftTime1(linecounterwrite,7),
			     rAngle.getDriftTime1(linecounterwrite,8),rAngle.getDriftTime1(linecounterwrite,9) );
		     break;
		 case 1:
		     sprintf(buf,"%s %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f\n",
			     dummy,
			     rAngle.getDriftTime1Error(linecounterwrite,0),rAngle.getDriftTime1Error(linecounterwrite,1),
			     rAngle.getDriftTime1Error(linecounterwrite,2),rAngle.getDriftTime1Error(linecounterwrite,3),
			     rAngle.getDriftTime1Error(linecounterwrite,4),rAngle.getDriftTime1Error(linecounterwrite,5),
			     rAngle.getDriftTime1Error(linecounterwrite,6),rAngle.getDriftTime1Error(linecounterwrite,7),
			     rAngle.getDriftTime1Error(linecounterwrite,8),rAngle.getDriftTime1Error(linecounterwrite,9) );
		     break;
		 case 2:
		     sprintf(buf,"%s %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f\n",
			     dummy,
			     rAngle.getDriftTime2(linecounterwrite,0),rAngle.getDriftTime2(linecounterwrite,1),
			     rAngle.getDriftTime2(linecounterwrite,2),rAngle.getDriftTime2(linecounterwrite,3),
			     rAngle.getDriftTime2(linecounterwrite,4),rAngle.getDriftTime2(linecounterwrite,5),
			     rAngle.getDriftTime2(linecounterwrite,6),rAngle.getDriftTime2(linecounterwrite,7),
			     rAngle.getDriftTime2(linecounterwrite,8),rAngle.getDriftTime2(linecounterwrite,9) );
		     break;
		 case 3:
		     sprintf(buf,"%s %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f\n",
			     dummy,
			     rAngle.getDriftTime2Error(linecounterwrite,0),rAngle.getDriftTime2Error(linecounterwrite,1),
			     rAngle.getDriftTime2Error(linecounterwrite,2),rAngle.getDriftTime2Error(linecounterwrite,3),
			     rAngle.getDriftTime2Error(linecounterwrite,4),rAngle.getDriftTime2Error(linecounterwrite,5),
			     rAngle.getDriftTime2Error(linecounterwrite,6),rAngle.getDriftTime2Error(linecounterwrite,7),
			     rAngle.getDriftTime2Error(linecounterwrite,8),rAngle.getDriftTime2Error(linecounterwrite,9) );
                 break;
		 default:Error("HMdcCal2ParSim::writeline()","argument \"type\" out of range ");
                 break;
		 }

		 linecounterwrite++;
	     } else r = kFALSE;
	 }  else r = kFALSE;
     } else { strcpy(buf,""); }
 }

 return r;
}

void HMdcCal2ParSim::clear() {
  // clears the container
  for(Int_t s=0;s<getSize();s++) {
    HMdcCal2ParSecSim& sec=(*this)[s];
    for(Int_t m=0;m<sec.getSize();m++) {
      HMdcCal2ParModSim& mod=sec[m];
      for(Int_t l=0;l<18;l++) {
          HMdcCal2ParAngleSim& angle=mod[l];
          angle.clear();
      }
    }
  }
  status=kFALSE;
  resetInputVersions();
}
void HMdcCal2ParSim::printParam() {
  // prints the container
    for(Int_t s=0;s<getSize();s++) {
	HMdcCal2ParSecSim& sec=(*this)[s];
	for(Int_t m=0;m<sec.getSize();m++) {
	    HMdcCal2ParModSim& mod=sec[m];
	    for(Int_t l=0;l<18;l++) {
		HMdcCal2ParAngleSim& rAngle=mod[l];
		for(Int_t mytype=0;mytype<4;mytype++){
		    for(Int_t myline=0;myline<10;myline++){

		        gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"%1i %1i %2i %1i",s, m, l, mytype);
			switch (mytype){
			case 0:
			    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
						   "%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f\n",
						   rAngle.getDriftTime1(myline,0),rAngle.getDriftTime1(myline,1),
						   rAngle.getDriftTime1(myline,2),rAngle.getDriftTime1(myline,3),
						   rAngle.getDriftTime1(myline,4),rAngle.getDriftTime1(myline,5),
						   rAngle.getDriftTime1(myline,6),rAngle.getDriftTime1(myline,7),
						   rAngle.getDriftTime1(myline,8),rAngle.getDriftTime1(myline,9) );
			    break;
			case 1:
			    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
						   "%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f\n",
						   rAngle.getDriftTime1Error(myline,0),rAngle.getDriftTime1Error(myline,1),
						   rAngle.getDriftTime1Error(myline,2),rAngle.getDriftTime1Error(myline,3),
						   rAngle.getDriftTime1Error(myline,4),rAngle.getDriftTime1Error(myline,5),
						   rAngle.getDriftTime1Error(myline,6),rAngle.getDriftTime1Error(myline,7),
						   rAngle.getDriftTime1Error(myline,8),rAngle.getDriftTime1Error(myline,9) );
			    break;
			case 2:
			    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
						   "%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f\n",
						   rAngle.getDriftTime2(myline,0),rAngle.getDriftTime2(myline,1),
						   rAngle.getDriftTime2(myline,2),rAngle.getDriftTime2(myline,3),
						   rAngle.getDriftTime2(myline,4),rAngle.getDriftTime2(myline,5),
						   rAngle.getDriftTime2(myline,6),rAngle.getDriftTime2(myline,7),
						   rAngle.getDriftTime2(myline,8),rAngle.getDriftTime2(myline,9) );
			    break;
			case 3:
			    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
						   "%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f\n",
						   rAngle.getDriftTime2Error(myline,0),rAngle.getDriftTime2Error(myline,1),
						   rAngle.getDriftTime2Error(myline,2),rAngle.getDriftTime2Error(myline,3),
						   rAngle.getDriftTime2Error(myline,4),rAngle.getDriftTime2Error(myline,5),
						   rAngle.getDriftTime2Error(myline,6),rAngle.getDriftTime2Error(myline,7),
						   rAngle.getDriftTime2Error(myline,8),rAngle.getDriftTime2Error(myline,9) );
			    break;
			default:Error("HMdcCal2ParSim::printParam()","argument \"type\" out of range ");
			break;
			}
		    }
		}
	    }
	}
    }
}
void HMdcCal2ParSim::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcCal2ParSim.

   UInt_t R__s, R__c;
   if (R__b.IsReading())
   {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      if(R__v==2)
      {
	  HParSet::Streamer(R__b);
	  R__b >> array;
	  R__b >> fMdc;
	  comment.Streamer(R__b);
	  R__b >> sector;
	  R__b >> module;
	  R__b >> angleDeg;
	  R__b >> time;
	  R__b >> time2;
	  R__b >> finaltime;
	  R__b >> angleStep;
	  R__b >> distance;
	  R__b >> linecounter;
	  R__b >> linecounterwrite;
	  R__b >> slopeOutside;
	  R__b >> type;
	  R__b >> dmin;
	  R__b >> dmax;
	  R__b.CheckByteCount(R__s, R__c, HMdcCal2ParSim::IsA());
      }
      else if(R__v==1)
      {
	  Warning("HMdcCal2ParSim:Streamer()","Reading old version 1!");
	  HParSet::Streamer(R__b);
	  R__b >> array;
	  R__b >> fMdc;
	  comment.Streamer(R__b);
	  R__b >> sector;
	  R__b >> module;
	  R__b >> angleDeg;
	  R__b >> time;
	  R__b >> time2;
	  R__b >> finaltime;
	  R__b >> angleStep;
	  R__b >> distance;
	  R__b >> linecounter;
	  R__b >> linecounterwrite;
	  R__b >> type;
	  R__b >> dmin;
	  R__b >> dmax;
	  R__b.CheckByteCount(R__s, R__c, HMdcCal2ParSim::IsA());
      }
   }
   else
   {
       R__c = R__b.WriteVersion(HMdcCal2ParSim::IsA(), kTRUE);
       HParSet::Streamer(R__b);
       R__b << array;
       R__b << fMdc;
       comment.Streamer(R__b);
       R__b << sector;
       R__b << module;
       R__b << angleDeg;
       R__b << time;
       R__b << time2;
       R__b << finaltime;
       R__b << angleStep;
       R__b << distance;
       R__b << linecounter;
       R__b << linecounterwrite;
       R__b << slopeOutside;
       R__b << type;
       R__b << dmin;
       R__b << dmax;
       R__b.SetByteCount(R__c, kTRUE);
   }
}
