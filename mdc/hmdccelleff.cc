//*-- AUTHOR : J. Markert

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HMdcCellEff
//
// Container class for the cell efficiency  parameters for HMdcDigitizer
//
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>

#include "hmdccelleff.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include "TMath.h"

#include <iostream> 
#include <iomanip>

ClassImp(HMdcCellEffAngle)
ClassImp(HMdcCellEffMod)
ClassImp(HMdcCellEff)

HMdcCellEffMod::HMdcCellEffMod(Int_t sec, Int_t mod, Int_t angle) {
  // constructor takes the sector, module and angle
    array = new TObjArray(angle);
    for (Int_t i=0; i<angle; ++i)
        array->AddAt(new HMdcCellEffAngle(),i);
}

HMdcCellEffMod::~HMdcCellEffMod() {
  // destructor
  array->Delete();
  delete array;
}

HMdcCellEff::HMdcCellEff(const Char_t* name,const Char_t* title,
                       const Char_t* context,Int_t n)
            : HParSet(name,title,context) {
  // constructor
  comment="no comment";
  strcpy(detName,"Mdc");
  if (gHades) {
      fMdc=(HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector("Mdc"));
  } else {
      fMdc = 0;
  }

  array = new TObjArray(n);
  for (Int_t i=0; i<n; i++) array->AddAt( new HMdcCellEffMod(i),i);
}

HMdcCellEff::~HMdcCellEff() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HMdcCellEff::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
    Int_t myset[4]={1,1,1,1};
    HDetParIo* input=inp->getDetParIo("HMdcParIo");
  if (input) return (input->init(this,myset));
  return kFALSE;

}
Int_t HMdcCellEff::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HMdcParIo");
  if (out) return out->write(this);
  return -1;
}

void HMdcCellEff::readline(const Char_t* buf, Int_t* set) {
  // decodes one line read from ascii file I/O and calls HMdcCellEffAngle::fillTime1(...)
  Int_t   mod,angle;
  Float_t maximumR,slopeEff;


  sscanf(buf,"%i%i%f%f", &mod, &angle, &maximumR, &slopeEff);
  Int_t n=mod;

  HMdcCellEffMod &module=(*this)[mod];

  if (angle<=module.getSize())
  {

      HMdcCellEffAngle& rAngle=module[angle];

      rAngle.setFunctionValues(maximumR,slopeEff);
  }
  else
  {
      Error("HMdcCellEff::readline()","array out of bounds");
  }

  set[n]=999;
}

void HMdcCellEff::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HMdcParAsciiFileIo
  header=
	"# Cell Efficiency parameters of the HMdcDigitizer\n"
	"# Format: \n"
	"# module  angle maxR slope\n";
}

Bool_t HMdcCellEff::writeline(Char_t *buf, Int_t mod, Int_t angle) {
  // writes one line to the buffer used by ascii file I/O
 Bool_t r = kTRUE;

 HMdcCellEffMod &module = (*this)[mod];
 if (angle<=module.getSize())
 {
     HMdcCellEffAngle& rAngle=module[angle];
     sprintf(buf,"%1i %2i %7.5f %7.5f\n", mod, angle, rAngle.getMaxR(), rAngle.getSlope());
 }
 else
 {
     r = kFALSE;
     Error("HMdcCellEff::writeline()","array out of bounds");
 }
 return r;
}
Int_t HMdcCellEff::calcEfficiency(Int_t m, Float_t r , Float_t b, Float_t l)
{
    // Function checks if given minimum distance from wire is larger than
    // the allowed value (calculated from the level which was set before).
    // If r > rCut the function returns -1 for the efficiency flag.
    //  m : module 0-3
    //  r : mindist from wire [mm]
    //  b : impact angle      [deg]
    //  l : level %
    Int_t status=0;
    Float_t angleDeg=0;
    Int_t angleStep=0;

    if(b<=90  && b>=0)
    {
        angleDeg=90-b;           // 90 deg impact ->0
    }
    else
    {
	if(b<=180 && b> 90 ) angleDeg=90-(180-b);
	if(b<=270 && b> 180) angleDeg=90-(270-b);
	if(b< 360 && b> 270) angleDeg=90-(360-b);
    }
    
    angleStep=((Int_t)((angleDeg)/5));
    if(angleStep==18)angleStep=17; 

    HMdcCellEffAngle& rAngle=(*this)[m][angleStep];
    HMdcCellEffAngle rAngle2;

    if(angleStep<17)
    {
	rAngle2=(*this)[m][angleStep+1];               // pointer to the second set
    }
    else
    {
	rAngle2=(*this)[m][angleStep];
    }
    Float_t rCut=0;
    Float_t slope=0;
    Float_t delta=0;
    Float_t r1=0;
    Float_t r2=0;

    //#################################### calc efficiency cut ######################
    r1= rAngle.getMaxR() +( l*(rAngle.getSlope()));
    r2= rAngle2.getMaxR()+( l*(rAngle2.getSlope()));

    slope= (Float_t) (r2-r1)/5.;
    delta= angleDeg-(angleStep*5);
    rCut=r1+slope*delta;
	if(r > rCut )  // larger distances than rCut are rejected
	{
	    status=-1;
	}
	else
	{
	    status=1;
	}
    return status;
}
Float_t HMdcCellEff::calcEffval(Int_t m, Float_t r , Float_t b, Float_t l)
{
    // Function calculates the efficiency value which would correspond
    // to a cut at the given value of r.
    //  m : module 0-3
    //  r : mindist from wire [mm]
    //  b : impact angle      [deg]
    //  l : level %

    Float_t angleDeg=0;
    Int_t angleStep=0;

    if(b<=90  && b>=0)
    {
        angleDeg=90-b;           // 90 deg impact ->0
    }
    else
    {
	if(b<=180 && b> 90 ) angleDeg=90-(180-b);
	if(b<=270 && b> 180) angleDeg=90-(270-b);
	if(b< 360 && b> 270) angleDeg=90-(360-b);
    }
    
    angleStep=((Int_t)((angleDeg)/5));
    if(angleStep==18)angleStep=17; 

    HMdcCellEffAngle& rAngle=(*this)[m][angleStep];
    HMdcCellEffAngle rAngle2;

    if(angleStep<17)
    {
	rAngle2=(*this)[m][angleStep+1];               // pointer to the second set
    }
    else
    {
	rAngle2=(*this)[m][angleStep];
    }

    Float_t myfraction=0;
    Float_t myslope=0;
    Float_t mydelta=0;
    Float_t d1=0;
    Float_t d2=0;

    if(rAngle.getSlope()!=0 && rAngle2.getSlope()!=0)
    {
	d1=(r - rAngle .getMaxR())/(rAngle.getSlope() );
	d2=(r - rAngle2.getMaxR())/(rAngle2.getSlope());
    }
    else
    {
	if(rAngle.getSlope()!=0 && rAngle2.getSlope()==0)
	{
	    d1=(r - rAngle .getMaxR())/(rAngle.getSlope() );
	    d2=(r - rAngle2.getMaxR())/(-0.001);
	}
	else if(rAngle.getSlope()==0 && rAngle2.getSlope()!=0)
	{
	    d1=(r - rAngle .getMaxR())/(-0.001);
	    d2=(r - rAngle2.getMaxR())/(rAngle2.getSlope());
 
	}
	else
	{
	    d1=(r - rAngle .getMaxR())/(-0.001);
	    d2=(r - rAngle2.getMaxR())/(-0.001);
	}
    }

    if((d1<=100&&d2<=100)&&(d1>=0&&d2>=0)) // both are good
    {
	myslope= (Float_t) (d2-d1)/5.;
	mydelta=angleDeg-(angleStep*5);
	myfraction=100-TMath::Abs(d1+myslope*mydelta);
    }
    else
    {
	if((d1<=100&&d1>=0)) // first is good
	{
	    myfraction=100-TMath::Abs(d1);
	}
	else if((d2<=100&&d2>=0)) // second is good
	{
	    myfraction=100-TMath::Abs(d2);
	}
    }

    if(myfraction>100||myfraction<=0)
    {
	Float_t testR=rAngle.getMaxR();
        if(rAngle2.getMaxR()>rAngle.getMaxR())testR=rAngle2.getMaxR();
	if(r>(testR*0.9))
	{
	    myfraction=100;
	}
	else
	{
          myfraction=0;
	}
    }
    return myfraction;
}

void HMdcCellEff::clear() {
  // clears the container
    for(Int_t m=0;m<getSize();m++) {
	HMdcCellEffMod& mod=(*this)[m];
	for(Int_t a=0;a<mod.getSize();a++) {
	    HMdcCellEffAngle& angle=mod[a];
	    angle.clear();
	}
    }

    status=kFALSE;
    resetInputVersions();
}

