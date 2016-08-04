//*-- AUTHOR : J. Kempter
//*-- Modified : 17/01/2002 by I. Koenig

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HMdcCalParRaw
//
// Container class for the TDC calibration parameters of the MDC
//
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>

#include "hmdccalparraw.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hmdcrawstruct.h"
#include "hmessagemgr.h"
#include <iostream> 
#include <iomanip>

ClassImp(HMdcCalParTdc)
ClassImp(HMdcCalParMbo)
ClassImp(HMdcCalParRawMod)
ClassImp(HMdcCalParRawSec)
ClassImp(HMdcCalParRaw)

void HMdcCalParTdc::fill(HMdcCalParTdc& r) {
  slope=r.getSlope();
  offset=r.getOffset();
  slopeErr=r.getSlopeErr();
  offsetErr=r.getOffsetErr();
  slopeMethod=r.getSlopeMethod();
  offsetMethod=r.getOffsetMethod();
}

HMdcCalParMbo::HMdcCalParMbo(Int_t tdc, const Text_t* name) {
  // constructor takes to number of TDC channels and the name of the MBO
  SetName(name);
  array = new TObjArray(tdc);
  for (Int_t i=0; i<tdc; ++i) array->AddAt(new HMdcCalParTdc(),i);
}

HMdcCalParMbo::~HMdcCalParMbo() {
  // destructor
  array->Delete();
  delete array;
}

HMdcCalParRawMod::HMdcCalParRawMod(Int_t mbo) {
  // constructor takes the number of MBOs
  array = new TObjArray(mbo);
}

HMdcCalParRawMod::~HMdcCalParRawMod() {
  // destructor
  array->Delete();
  delete array;
}

void HMdcCalParRawMod::createMbo(Int_t mbo, Int_t nTdcs, const Char_t* mboName) {
  // creates the array object of type HMdcCalParMbo
  if (mbo<array->GetSize() && array->At(mbo)==NULL) {
    array->AddAt(new HMdcCalParMbo(nTdcs,mboName),mbo);
  }
}

HMdcCalParRawSec::HMdcCalParRawSec(Int_t mod) {
  // constructor takes the number of modules
  array = new TObjArray(mod);
  for (Int_t i=0; i<mod; i++)
        array->AddAt(new HMdcCalParRawMod(),i);
}

HMdcCalParRawSec::~HMdcCalParRawSec() {
  // destructor
  array->Delete();
  delete array;
}

HMdcCalParRaw::HMdcCalParRaw(const Char_t* name,const Char_t* title,
                             const Char_t* context,Int_t n)
              : HParSet(name,title,context) {
  // constructor
  strcpy(detName,"Mdc");
  array = new TObjArray(n);
  for (Int_t i=0; i<n; i++) array->AddAt(new HMdcCalParRawSec(),i);
  gHades->getRuntimeDb()->getContainer("MdcRawStruct");
}

HMdcCalParRaw::~HMdcCalParRaw() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HMdcCalParRaw::init(HParIo* inp,Int_t* set) {
  // creates the array objects of type HMdcCalParRawMbo using the data in the parameter container
  // MdcRawStruct and initializes the container from an input
  HMdcRawStruct* pRawStruct=
       (HMdcRawStruct*)gHades->getRuntimeDb()->findContainer("MdcRawStruct");
  if (pRawStruct==NULL) {
    Error("HMdcCalParRaw::init()","MdcCalParRaw not initialized, container MdcRawStruct not found");
    return kFALSE;
  }
  for(Int_t s=0;s<getSize();s++) {
    HMdcCalParRawSec& secPar=(*this)[s];
    HMdcRawSecStru& secStru=(*pRawStruct)[s];
    for(Int_t m=0;m<secPar.getSize();m++) {
      HMdcCalParRawMod& modPar=secPar[m];
      HMdcRawModStru& modStru=secStru[m];
      for(Int_t i=0;i<modStru.getSize();i++) {
        modPar.createMbo(i,modStru[i].getNTdcs(),modStru[i].GetName());
      }
    }
  }
  HDetParIo* input=inp->getDetParIo("HMdcParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HMdcCalParRaw::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HMdcParIo");
  if (out) return out->write(this);
  return -1;
}

void HMdcCalParRaw::readline(const Char_t* buf, Int_t* set) {
  // decodes one line read from ascii file I/O and calls HMdcCalParTdc::fill(...) 
  Int_t   sec, mod, mbo, tdc, slopeMethod,offsetMethod;
  Float_t slope, offset, slopeErr,offsetErr;
  Char_t mboName[10];
  sscanf(buf,"%i%i%i%s%i%f%f%f%f%i%i",
         &sec, &mod, &mbo, mboName, &tdc, &slope, &offset, &slopeErr,
         &offsetErr, &slopeMethod, &offsetMethod);
  Int_t n=sec*4+mod;
  if (!set[n]) return;
  HMdcCalParMbo& rMbo=(*this)[sec][mod][mbo];
  rMbo.SetName(mboName);
  HMdcCalParTdc& t=rMbo[tdc];
  t.fill(slope,offset,slopeErr,offsetErr,slopeMethod,offsetMethod);
  set[n]=999;
}

void HMdcCalParRaw::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HMdcParAsciiFileIo
  header=
    "# Calibration parameters of the MDC\n"
    "# Format:\n"
    "# sector  module  mbo  mboName  tdc  slope  offset  slopeErr  offsetErr  slopeMethod  offsetMethod\n";
}

Bool_t HMdcCalParRaw::writeline(Char_t *buf, Int_t sec, Int_t mod, Int_t mbo, Int_t tdc) {
  // writes one line to the buffer used by ascii file I/O
  HMdcCalParMbo& rMbo=(*this)[sec][mod][mbo];
  HMdcCalParTdc& chan=rMbo[tdc];
  sprintf(buf,"%1i %1i %2i %s %3i % 10.5f % 10.2f % 12.5f % 12.2f %1i %1i \n",
          sec, mod, mbo, rMbo.GetName(), tdc,
          chan.getSlope(), chan.getOffset(), chan.getSlopeErr(),
          chan.getOffsetErr(),chan.getSlopeMethod(), chan.getOffsetMethod());
  return kTRUE;
}

void HMdcCalParRaw::clear() {
  // clears the container
  for(Int_t s=0;s<getSize();s++) {
    HMdcCalParRawSec& sec=(*this)[s];
    for(Int_t m=0;m<sec.getSize();m++) {
      HMdcCalParRawMod& mod=sec[m];
      for(Int_t l=0;l<mod.getSize();l++) {
        HMdcCalParMbo& mbo=mod[l];
        for(Int_t c=0;c<mbo.getSize();c++) mbo[c].clear();
      }
    }
  }
  status=kFALSE;
  resetInputVersions();
}
void HMdcCalParRaw::printParam() {
  // prints the container
    SEPERATOR_msg("-",60);
    INFO_msg(10,HMessageMgr::DET_MDC,"HMdcCalParRaw");
    for(Int_t s=0;s<getSize();s++) {
	HMdcCalParRawSec& sec=(*this)[s];
	for(Int_t m=0;m<sec.getSize();m++) {
	    HMdcCalParRawMod& mod=sec[m];
	    for(Int_t l=0;l<mod.getSize();l++) {
		HMdcCalParMbo& mbo=mod[l];
		for(Int_t c=0;c<mbo.getSize();c++){
		  gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
			"%1i %1i %2i %s %3i % 10.5f % 10.2f % 12.5f % 12.2f %1i %1i\n",
			   s, m, l, mbo.GetName(),c,
			   mbo[c].getSlope(),mbo[c].getOffset(),
			   mbo[c].getSlopeErr(),mbo[c].getOffsetErr(),
			   mbo[c].getSlopeMethod(),mbo[c].getOffsetMethod()
			  );		 
		}
	    }
	}
    }
    SEPERATOR_msg("-",60);
}
void HMdcCalParRaw::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcCalParRaw.
  TString fAuthor;
  TString fDescription;
  TString fRunsUsed;
  TString fValidity;

   UInt_t R__s, R__c;
   if (R__b.IsReading())
   {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      if(R__v==1)
      {
	  HParSet::Streamer(R__b);
	  R__b >> array;
	  fAuthor.Streamer(R__b);
	  fDescription.Streamer(R__b);
	  fRunsUsed.Streamer(R__b);
	  fValidity.Streamer(R__b);
	  R__b.CheckByteCount(R__s, R__c, HMdcCalParRaw::IsA());
      }
      else if(R__v==2)
      {
	  HParSet::Streamer(R__b);
	  R__b >> array;
	  R__b.CheckByteCount(R__s, R__c, HMdcCalParRaw::IsA());
      }
      else{Error("HMdcCalParRaw:Streamer()","Read version not known!");}
   }
   else
   {
       HMdcCalParRaw::Class()->WriteBuffer(R__b,this);
   }
}

