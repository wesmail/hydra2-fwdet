//*-- AUTHOR : J. Markert

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HMdcCal2Par
//
// Container class for the calibration parameters from Cal1 to Cal2 of the MDC
// time -> distance.Contains functions for calculating time ->distance for santiago track fitter
// and Calibrater2 of Mdc
////////////////////////////////////////////////////////////////////////////

#include "hmdccal2par.h"
#include "hmdccal2parsim.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hdetector.h"
#include "hmessagemgr.h"

#include "TH2.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TMath.h"


using namespace std;
#include <stdlib.h>
#include <iostream> 
#include <iomanip>

ClassImp(HMdcCal2ParAngle)
ClassImp(HMdcCal2ParMod)
ClassImp(HMdcCal2ParSec)
ClassImp(HMdcCal2Par)



void HMdcCal2ParAngle::fillDistance(HMdcCal2ParAngle& r) {
    for(Int_t i=0;i<100;i++)
    {
	distance[i]=r.distance[i];
    }
}
void HMdcCal2ParAngle::fillDistanceError(HMdcCal2ParAngle& r) {
    for(Int_t i=0;i<100;i++)
    {
	distanceErr[i]=r.distanceErr[i];
    }
}
HMdcCal2ParMod::HMdcCal2ParMod(Int_t sec, Int_t mod, Int_t angle) {
  // constructor takes the sector, module and angle
    array = new TObjArray(angle);
    for (Int_t i=0; i<angle; ++i)
        array->AddAt(new HMdcCal2ParAngle(),i);
}

HMdcCal2ParMod::~HMdcCal2ParMod() {
  // destructor
  array->Delete();
  delete array;
}

HMdcCal2ParSec::HMdcCal2ParSec(Int_t sec, Int_t mod) {
  // constructor takes the sector, module number
  array = new TObjArray(mod);
  for (Int_t i=0; i<mod; i++)
        array->AddAt(new HMdcCal2ParMod(sec,i),i);
}

HMdcCal2ParSec::~HMdcCal2ParSec() {
  // destructor
  array->Delete();
  delete array;
}

HMdcCal2Par::HMdcCal2Par(const Char_t* name,const Char_t* title,
                         const Char_t* context,Int_t n)
            : HParSet(name,title,context)
{
  // constructor
  comment="no comment";

  cal2parsim=0;
  cal2parsim=(HMdcCal2ParSim*)gHades->getRuntimeDb()->getContainer("MdcCal2ParSim");
  if(!cal2parsim)
  {
      Error("HMdcCal2Par()","ZERO POINTER RETRIEVED FOR HMDCCAL2PARSIM!");
      exit(1);
  }
  strcpy(detName,"Mdc");
  if (gHades) {
      fMdc = (HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector("Mdc"));
  } else {
      fMdc = 0;
  }
  array = new TObjArray(n);
  for (Int_t i=0; i<n; i++) array->AddAt(new HMdcCal2ParSec(i),i);
  linecounter=0;
  linecounterwrite=0;
  type=0;
  myslopeOutside=0.01;
}

HMdcCal2Par::~HMdcCal2Par() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HMdcCal2Par::init(HParIo* inp) {
  // intitializes the container from an input
    linecounter=0;
    linecounterwrite=0;
    type=0;

    if (!cal2parsim) return kFALSE;
    Int_t v1=cal2parsim->getInputVersion(1);
    Int_t v2=cal2parsim->getInputVersion(2);
    if (v1==versions[1] && v2==versions[2]) return kTRUE;
    // needs reinitialization

    HDetector *mdcDet = (HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector("Mdc"));
    if (!mdcDet)
    {
	Error("HMdcCal2Par:init()","Detector setup (gHades->getSetup()->getDetector(\"Mdc\")) missing.");
    }
    else
    {
      INFO_msg(10,HMessageMgr::DET_MDC,"HMdcCal2Par initialized from HMdcCal2ParSim for modules ");
      Char_t *buf;
      buf =  new char[200];
      strcpy(buf,"\n");
	for(Int_t s=0; s<6; s++)
	{  //loop over sectors
	    for(Int_t m=0; m<4; m++)
	    {  //loop over modules
		if (!mdcDet->getModule(s, m)) continue;
		sprintf(buf,"%s %i",buf,s*4+m);
		for(Int_t a=0;a<18;a++)
		{
		    cal2parsim->transformToDistance(s,m,a,4.0,this);
		}
	    }
	}
	gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"%s ",buf);
	delete []buf;
    }
    myslopeOutside=cal2parsim->getSlopeOutside();

    versions[1]=v1;
    versions[2]=v2;
    changed=kTRUE;
    return kTRUE;
}
void HMdcCal2Par::plot(Int_t s,Int_t m)
{
    // plots data into TGraphErrors for distance including errors

    cout<<"HMdcCal2Par: plotting sector "<<s<<" module "<<m<<endl;
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    Char_t nameCanvas[300];
    sprintf(nameCanvas,"%s %i %s %i","time -> distance sector ",s," module ",m);
    TH2F* dummy=new TH2F("dummy","dummy",2,0,410,2,0,10.1);
    dummy->SetXTitle("drift time [ns]");
    dummy->SetYTitle("distance from wire [mm]");

    TCanvas* result=new TCanvas(nameCanvas,nameCanvas,1000,800);
    result->Divide(6,3);
    result->Draw();
    Float_t x[100];
    Float_t y[100];
    Float_t ex[100];
    Float_t ey[100];

    for(Int_t a=0;a<18;a++)
    {
	for(Int_t i=0;i<100;i++)
	{
	    x[i] =i*4.0;
	    y[i] =(*this)[s][m][a].distance[i];
            ex[i]=0;
	    ey[i]=(*this)[s][m][a].distanceErr[i];
	}
	result->cd(a+1);
        dummy->DrawCopy();
	TGraphErrors* g=new TGraphErrors(100,x,y,ex,ey);
	g->SetLineColor(2);
        g->Draw();
    }
    result->Update();
    delete dummy;
}
void HMdcCal2Par::plot2D(Int_t s,Int_t m,Int_t type)
{
    // Plots data into 2d-Hists
    // type = 0 ->distance
    // type = 1 ->Error distance

    cout<<"HMdcCal2Par: plotting 2D sector "<<s<<" module "<<m<<endl;

    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetPalette(1);

    Char_t nameCanvas[300];
    if(type==0)sprintf(nameCanvas,"%s %i %s %i","distance sector ",s," module ",m);
    if(type==1)sprintf(nameCanvas,"%s %i %s %i","Error distance sector ",s," module ",m);

    TH2F* dummy=new TH2F("dummy","dummy",18,0,90,100,0,400);
   
    dummy->SetXTitle("angle");
    dummy->SetYTitle("drift time [ns]");

    if(type==0)dummy->SetZTitle("distance [mm]");
    if(type==1)dummy->SetZTitle("Error distance [mm]");

    TCanvas* result=new TCanvas(nameCanvas,nameCanvas,1000,800);

    for(Int_t a=0;a<18;a++)
    {
	for(Int_t i=0;i<100;i++)
	{
	    if(type==0)dummy->SetBinContent(a+1,i,(*this)[s][m][a].distance[i]);
            if(type==1)dummy->SetBinContent(a+1,i,(*this)[s][m][a].distanceErr[i]);
	}
    }
    result->cd();
    dummy->DrawCopy("lego2");

    result->Update();
    delete dummy;
}
Double_t HMdcCal2Par::calcDistance(Int_t s,Int_t m,Double_t a,Double_t time)
{
    // This function calculates the distance from sense wire for a given drift time.
    // A Interpolation between the two closest angle steps
    // is performed and the distance value returned.
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
    if(time<0)
    {
	return -1;
    }
    Double_t angle=90.-a;
    Double_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined
  
    HMdcCal2ParAngle& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngle& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Double_t dminF=time/4.;
    Int_t dminI=Int_t(dminF); 

    Double_t finalDistance;
    if(dminI>98)
    {   // drift time outside matrix
	Double_t y1=(time-396)/myslopeOutside + rAngle.distance[99];
	Double_t y2=(time-396)/myslopeOutside + rAngle2.distance[99];

	Double_t t=angleStepD - angleStepI;

        finalDistance=y1-t*(y1-y2);
	return finalDistance;
    }

    //#################################### calc distance ######################
    Double_t y1=rAngle.distance [dminI];
    Double_t y2=rAngle2.distance[dminI];
    Double_t y3=rAngle2.distance[dminI+1];
    Double_t y4=rAngle.distance [dminI+1];

    Double_t t=angleStepD - angleStepI;
    Double_t u=dminF - dminI;

    finalDistance=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
    return finalDistance;
}
Double_t HMdcCal2Par::calcDistanceErr(Int_t s,Int_t m,Double_t a,Double_t time) {
    // This function calculates the Error of distance from sense wire for a given drift time.
    // A Interpolation between the two closest angle steps
    // is performed and the distance value returned.
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
    if(time<0)
    {
	return -1;
    }
    Double_t angle=90.-a;
    Double_t angleStepD=angle/5;
    Int_t angleStepI=Int_t(angleStepD);
    if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined
  
    HMdcCal2ParAngle& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
    HMdcCal2ParAngle& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

    Double_t dminF=time/4.;
    Int_t dminI=Int_t(dminF); 

    Double_t finalErr;
    if(dminI>98)
    {   // drift time outside matrix
	Double_t y1= rAngle.distance[99];
	Double_t y2= rAngle2.distance[99];

	Double_t t=angleStepD - angleStepI;

        finalErr=y1-t*(y1-y2);
	return finalErr;
    }

    //#################################### calc distance Error ######################
    Double_t y1=rAngle.distanceErr [dminI];
    Double_t y2=rAngle2.distanceErr[dminI];
    Double_t y3=rAngle2.distanceErr[dminI+1];
    Double_t y4=rAngle.distanceErr [dminI+1];

    Double_t t=angleStepD - angleStepI;
    Double_t u=dminF - dminI;

    finalErr=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
    return finalErr;
}
void HMdcCal2Par::calcDistance(Int_t s,Int_t m,Double_t a,Double_t time
			      ,Double_t*dist,Double_t*distErr)
{
    // This function calculates the distance from sense wire for a given drift time.
    // A Interpolation between the two closest angle steps
    // is performed and the distance value returned.
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
    // Distance and Error of Distance are returned to Float pointers
    if(time<0)
    {
	*dist   =-1;
	*distErr=-1;
    }
    else
    {
	Double_t angle=90.-a;
	Double_t angleStepD=angle/5;
	Int_t angleStepI=Int_t(angleStepD);
	if(angleStepI==18)angleStepI=17;      // if angle==90, angleStepI=18 ->not defined

	HMdcCal2ParAngle& rAngle =(*this)[s][m][angleStepI];// pointer to the first set
	HMdcCal2ParAngle& rAngle2=(angleStepI<17) ? (*this)[s][m][angleStepI+1] : rAngle;

	Double_t dminF=time/4.;
	Int_t dminI=Int_t(dminF);

	if(dminI>98)
	{   // drift time outside matrix
	    Double_t y1=(time-396)/myslopeOutside + rAngle.distance[99];
	    Double_t y2=(time-396)/myslopeOutside + rAngle2.distance[99];

	    Double_t t=angleStepD - angleStepI;

	    *dist=y1-t*(y1-y2);

	    y1=rAngle.distance[99];
	    y2=rAngle2.distance[99];

	    *distErr=y1-t*(y1-y2);
	}
	else
	{
	    //#################################### calc distance ######################
	    Double_t y1=rAngle.distance [dminI];
	    Double_t y2=rAngle2.distance[dminI];
	    Double_t y3=rAngle2.distance[dminI+1];
	    Double_t y4=rAngle.distance [dminI+1];

	    Double_t t=angleStepD - angleStepI;
	    Double_t u=dminF - dminI;

	    *dist=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);

	    //################################ calc errors ######################################
	    y1=rAngle.distanceErr [dminI];
	    y2=rAngle2.distanceErr[dminI];
	    y3=rAngle2.distanceErr[dminI+1];
	    y4=rAngle.distanceErr [dminI+1];

	    *distErr=y1-t*(y1-y2)-u*(y1-y4)+t*u*(y1-y2+y3-y4);
	}
    }
}
Int_t HMdcCal2Par::write(HParIo* output) {
    // writes the container to an output
    if (strcmp(output->IsA()->GetName(),"HParAsciiFileIo")==0)
    {
	HDetParIo* out=output->getDetParIo("HMdcParIo");
	if (out) return out->write(this);
    }
    changed=kFALSE;
    return 0;
}
void HMdcCal2Par::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HMdcParAsciiFileIo
  header=
	"# Cal2 Calibration parameters of the MDC\n"
	"# time -> distance\n"
	"# Format: type: 0=dist, 1=distErr\n"
	"# sector  module  angle type par0 par1 par2 par3 par4 par5 pa6 par7 par8 par9 par10\n";
}

Bool_t HMdcCal2Par::writeline(Char_t *buf, Int_t sec, Int_t mod, Int_t angle) {
  // writes one line to the buffer used by ascii file I/O
 Bool_t r = kTRUE;
 if (fMdc) {
     if (fMdc->getModule(sec,mod) != 0) {
	 if  (sec>-1 && sec<getSize()) {
	     HMdcCal2ParSec &sector = (*this)[sec];
	     if (mod>-1 && mod<sector.getSize()) {

		 HMdcCal2ParAngle& rAngle=(*this)[sec][mod][angle];
		 Char_t dummy[20];

		 if(linecounterwrite%10==0&&linecounterwrite>1)
		 {
		     linecounterwrite=0;
		     type++;
		 }
                 if(type>1)
		 {
		    type=0;
		 }
		 sprintf(dummy,"%1i %1i %2i %1i",sec, mod, angle, type);
		 switch (type){
		 case 0:
		     sprintf(buf,"%s %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f\n",
			     dummy,
			     rAngle.getDistance(linecounterwrite,0),rAngle.getDistance(linecounterwrite,1),
			     rAngle.getDistance(linecounterwrite,2),rAngle.getDistance(linecounterwrite,3),
			     rAngle.getDistance(linecounterwrite,4),rAngle.getDistance(linecounterwrite,5),
			     rAngle.getDistance(linecounterwrite,6),rAngle.getDistance(linecounterwrite,7),
			     rAngle.getDistance(linecounterwrite,8),rAngle.getDistance(linecounterwrite,9) );
		     break;
		 case 1:
		     sprintf(buf,"%s %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f\n",
			     dummy,
			     rAngle.getDistanceError(linecounterwrite,0),rAngle.getDistanceError(linecounterwrite,1),
			     rAngle.getDistanceError(linecounterwrite,2),rAngle.getDistanceError(linecounterwrite,3),
			     rAngle.getDistanceError(linecounterwrite,4),rAngle.getDistanceError(linecounterwrite,5),
			     rAngle.getDistanceError(linecounterwrite,6),rAngle.getDistanceError(linecounterwrite,7),
			     rAngle.getDistanceError(linecounterwrite,8),rAngle.getDistanceError(linecounterwrite,9) );
		     break;
		 default:Error("HMdcCal2Par::writeline()","argument \"type\" out of range ");
                 break;
		 }

		 linecounterwrite++;
	     } else r = kFALSE;
	 }  else r = kFALSE;
     } else { strcpy(buf,""); }
 }

 return r;
}

void HMdcCal2Par::clear() {
  // clears the container
  for(Int_t s=0;s<getSize();s++) {
    HMdcCal2ParSec& sec=(*this)[s];
    for(Int_t m=0;m<sec.getSize();m++) {
      HMdcCal2ParMod& mod=sec[m];
      for(Int_t l=0;l<18;l++) {
          HMdcCal2ParAngle& angle=mod[l];
          angle.clear();
      }
    }
  }
  status=kFALSE;
  resetInputVersions();
}
void HMdcCal2Par::printParam() {
  // prints the container
    for(Int_t s=0;s<getSize();s++) {
	HMdcCal2ParSec& sec=(*this)[s];
	for(Int_t m=0;m<sec.getSize();m++) {
	    HMdcCal2ParMod& mod=sec[m];
	    for(Int_t l=0;l<18;l++) {
		HMdcCal2ParAngle& rAngle=mod[l];
		for(Int_t mytype=0;mytype<2;mytype++){
		    for(Int_t myline=0;myline<10;myline++){
		        gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"%1i %1i %2i %1i",s, m, l, mytype);
			switch (mytype){
			case 0:
			    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
				"%7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f\n",
				rAngle.getDistance(myline,0),rAngle.getDistance(myline,1),
				rAngle.getDistance(myline,2),rAngle.getDistance(myline,3),
				rAngle.getDistance(myline,4),rAngle.getDistance(myline,5),
				rAngle.getDistance(myline,6),rAngle.getDistance(myline,7),
				rAngle.getDistance(myline,8),rAngle.getDistance(myline,9) );
			    break;
			case 1:
			    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
				"%7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f\n",
				rAngle.getDistanceError(myline,0),rAngle.getDistanceError(myline,1),
				rAngle.getDistanceError(myline,2),rAngle.getDistanceError(myline,3),
				rAngle.getDistanceError(myline,4),rAngle.getDistanceError(myline,5),
				rAngle.getDistanceError(myline,6),rAngle.getDistanceError(myline,7),
				rAngle.getDistanceError(myline,8),rAngle.getDistanceError(myline,9) );
			    break;
			default:Error("HMdcCal2Par::printParam()","argument \"type\" out of range ");
			break;
			}
		    }
		}
	    }
	}
    }
}
