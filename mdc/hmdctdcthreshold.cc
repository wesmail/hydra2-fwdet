//*-- AUTHOR : J. Kempter
//*-- Modified : 17/01/2002 by I. Koenig
//*-- Modified : 06/09/2004 by S. Lang
//*-- Modified : 13/09/2004 by J. Wuestenfeld

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
// HMdcTdcThreshold
//
// Container class for the TDC threshold parameters of the MDC
//
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>

#include "hmdctdcthreshold.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hmdcrawstruct.h"
#include "hmessagemgr.h"
#include "TSystem.h"
#include <iostream>
#include <iomanip>

ClassImp(HMdcTdcThresholdDbo)
ClassImp(HMdcTdcThresholdMbo)
ClassImp(HMdcTdcThresholdMod)
ClassImp(HMdcTdcThresholdSec)
ClassImp(HMdcTdcThreshold)

void HMdcTdcThresholdDbo::fill(HMdcTdcThresholdDbo& r) {
    // copies the threshold
    threshold=r.getThreshold();
}

HMdcTdcThresholdMbo::HMdcTdcThresholdMbo(Int_t dbo, const Text_t* name) {
    // constructor takes to number of DBOs  and the name of the MBO
    SetName(name);
    array = new TObjArray(dbo);
    for (Int_t i=0; i<dbo; ++i) array->AddAt(new HMdcTdcThresholdDbo(),i);
}

HMdcTdcThresholdMbo::~HMdcTdcThresholdMbo() {
    // destructor
    array->Delete();
    delete array;
}

HMdcTdcThresholdMod::HMdcTdcThresholdMod(Int_t mbo) {
    // constructor takes the number of MBOs
    // The array objects of type HMdcThresholdMbo are created in the init function.
    array = new TObjArray(mbo);
}

HMdcTdcThresholdMod::~HMdcTdcThresholdMod() {
    // destructor
    array->Delete();
    delete array;
}

void HMdcTdcThresholdMod::createMbo(Int_t mbo, Int_t nDbos, const Char_t* mboName) {
  // creates the array object of type HMdcTdcThresholdMbo
  if (mbo<array->GetSize() && array->At(mbo)==NULL) {
    array->AddAt(new HMdcTdcThresholdMbo(nDbos,mboName),mbo);
  }
}

HMdcTdcThresholdSec::HMdcTdcThresholdSec(Int_t mod) {
    // constructor takes the number of modules
    array = new TObjArray(mod);
    for (Int_t i=0; i<mod; i++)
	array->AddAt(new HMdcTdcThresholdMod(),i);
}

HMdcTdcThresholdSec::~HMdcTdcThresholdSec() {
    // destructor
    array->Delete();
    delete array;
}

HMdcTdcThreshold::HMdcTdcThreshold(const Char_t* name,const Char_t* title,
				   const Char_t* context,Int_t n)
: HParSet(name,title,context) {
    // constructor
    strcpy(detName,"Mdc");
    oraVersion=-1;
    array = new TObjArray(n);
    for (Int_t i=0; i<n; i++) array->AddAt(new HMdcTdcThresholdSec(),i);
    pRawStruct=(HMdcRawStruct*)(gHades->getRuntimeDb()->getContainer("MdcRawStruct"));
}

HMdcTdcThreshold::~HMdcTdcThreshold() {
    // destructor
    array->Delete();
    delete array;
}

Bool_t HMdcTdcThreshold::init(HParIo* inp,Int_t* set) {
    // creates the array objects of type HMdcTdcThresholdMbo using the data in the parameter container
    // MdcRawStruct and initializes the container from an input
    if (pRawStruct==NULL) {
      Error("HMdcTdcThreshold::init()","MdcTdcThreshold not initialized, container MdcRawStruct is NULL");
      return kFALSE;
    }
    for(Int_t s=0;s<getSize();s++) {
      HMdcTdcThresholdSec& secPar=(*this)[s];
      HMdcRawSecStru& secStru=(*pRawStruct)[s];
      for(Int_t m=0;m<secPar.getSize();m++) {
        HMdcTdcThresholdMod& modPar=secPar[m];
        HMdcRawModStru& modStru=secStru[m];
        for(Int_t i=0;i<modStru.getSize();i++) {
          modPar.createMbo(i,modStru[i].getNTdcs()/16,modStru[i].GetName());
        }
      }
    }
    HDetParIo* input=inp->getDetParIo("HMdcParIo");
    if (input) return (input->init(this,set));
    return kFALSE;
}

Int_t HMdcTdcThreshold::write(HParIo* output) {
    // writes the container to an output
    HDetParIo* out=output->getDetParIo("HMdcParIo");
    if (out) return out->write(this);
    return -1;
}

void HMdcTdcThreshold::readline(const Char_t* buf, Int_t* set) {
    // decodes one line read from ascii file I/O and calls HMdcTdcThresholdDbo::setThreshold(...)
    Int_t   sec, mod, mbo, dbo;
    Int_t thresh;
    Char_t mboName[10];
    sscanf(buf,"%i%i%i%s%i%x",
	   &sec, &mod, &mbo, mboName, &dbo, &thresh);
    Int_t n=sec*4+mod;
    if (!set[n]) return;
    HMdcTdcThresholdMbo& rMbo=(*this)[sec][mod][mbo];
    rMbo.SetName(mboName);
    HMdcTdcThresholdDbo& db=rMbo[dbo];
    db.setThreshold(thresh);
    set[n]=999;
}

void HMdcTdcThreshold::readlinefromfile(const Char_t* buf) {
    // decodes one line read from ascii file and calls HMdcTdcThresholdDbo::setThreshold(...)
    Int_t   sec, mod, mbo, dbo;
    Int_t thresh;
    sscanf(buf,"%i%i%i%i%x",
	   &mod, &sec, &mbo, &dbo, &thresh);
    HMdcRawModStru & rawMod=(*pRawStruct)[sec][mod];
    //  cout << rawMod.getSize() << endl;
    if(rawMod.getSize()>mbo)
    {
	HMdcRawMothStru &rawMbo=rawMod[mbo];
	// cout << rawMbo.getNTdcs() << endl;
	if (rawMbo.getNTdcs() > dbo*16 )
	{
	    HMdcTdcThresholdMbo& rMbo=(*this)[sec][mod][mbo];
	    HMdcTdcThresholdDbo& db=rMbo[dbo];
	    db.setThreshold(thresh);
	}
	//      else{cout<<"dbo number to big"<<endl;};
    }
    //  else{cout<<"mbo number"<<endl;};
}

void HMdcTdcThreshold::writelinetofile(Char_t* buf, Int_t sec, Int_t mod, Int_t mbo, Int_t dbo) {
    // writes one line of threshold information into buffer, based on
    // sec mod mbo dbo address. If no dbo found sets threshold to 60!
    Int_t thresh;

    HMdcRawModStru & rawMod=(*pRawStruct)[sec][mod];
    //cout << rawMod.getSize() << endl;
    if(rawMod.getSize()>mbo)
    {
	HMdcRawMothStru &rawMbo=rawMod[mbo];
	//cout << rawMbo.getNTdcs() << endl;
	if (rawMbo.getNTdcs() > dbo*16 )
	{
	    HMdcTdcThresholdMbo& rMbo=(*this)[sec][mod][mbo];
	    HMdcTdcThresholdDbo& db=rMbo[dbo];
	    thresh = db.getThreshold();
	    sprintf(buf, "%i %i %i %i %x", mod, sec, mbo, dbo, thresh);

	}
	else sprintf(buf,"%i %i %i %i 60",mod,sec,mbo,dbo);
	//else{cout<<"dbo number to big"<<endl;};
    }
    else sprintf(buf,"%i %i %i %i 60",mod,sec,mbo,dbo);
    //else{cout<<"mbo number"<<endl;};
}


void HMdcTdcThreshold::addvalue(Int_t factor) {
    //to increase or decrease value of threshold by the constant factor
    //if you want to increast threshold then the value of the factor
    //should be positive, for decreasing the factor must be negative.

    Int_t sec, mbo, mod, dbo;
    for(sec=0; sec<6; sec++)
    {
	HMdcTdcThresholdSec & rawSec= (*this)[sec];
	for(mod=0; mod<rawSec.getSize(); mod++)
	{
	    HMdcTdcThresholdMod & rawMod=rawSec[mod];
	    for(mbo =0 ; mbo < rawMod.getSize() ; mbo++)
	    {
		HMdcTdcThresholdMbo &rawMbo=rawMod[mbo];
		for(dbo =0; dbo < rawMbo.getSize(); dbo++ )
		{
		    HMdcTdcThresholdDbo& db=rawMbo[dbo];
		    db.setThreshold((db.getThreshold())+factor);
		}
	    }
	}
    }
}

void HMdcTdcThreshold::putAsciiHeader(TString& header) {
    // puts the ascii header to the string used in HMdcParAsciiFileIo
    header=
	"# Calibration parameters of the MDC\n"
	"# Format:\n"
	"# sector  module  mbo  mboName  threshold\n";
}

Bool_t HMdcTdcThreshold::writeline(Char_t *buf, Int_t sec, Int_t mod, Int_t mbo, Int_t dbo) {
    // writes one line to the buffer used by ascii file I/O
    HMdcTdcThresholdMbo& rMbo=(*this)[sec][mod][mbo];
    HMdcTdcThresholdDbo& db=rMbo[dbo];
    sprintf(buf,"%1i %1i %2i %s %2i %3x\n",
	    sec, mod, mbo, rMbo.GetName(), dbo, db.getThreshold());
    return kTRUE;
}

void HMdcTdcThreshold::clear() {
    // clears the container
    oraVersion=-1;
    for(Int_t s=0;s<getSize();s++) {
	HMdcTdcThresholdSec& sec=(*this)[s];
	for(Int_t m=0;m<sec.getSize();m++) {
	    HMdcTdcThresholdMod& mod=sec[m];
	    for(Int_t l=0;l<mod.getSize();l++) {
		HMdcTdcThresholdMbo& mbo=mod[l];
		for(Int_t c=0;c<mbo.getSize();c++) mbo[c].clear();
	    }
	}
    }
    status=kFALSE;
    resetInputVersions();
}
void HMdcTdcThreshold::printParam() {
    // prints the container
    SEPERATOR_msg("-",60);
    INFO_msg(10,HMessageMgr::DET_MDC,"HMdcTdcThreshold");
    for(Int_t s=0;s<getSize();s++) {
	HMdcTdcThresholdSec& sec=(*this)[s];
	for(Int_t m=0;m<sec.getSize();m++) {
	    HMdcTdcThresholdMod& mod=sec[m];
	    for(Int_t l=0;l<mod.getSize();l++) {
		HMdcTdcThresholdMbo& mbo=mod[l];
		for(Int_t c=0;c<mbo.getSize();c++){
		    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
					   "%1i %1i %2i %s % 1i %3x",
					   s, m, l, mbo.GetName(), c, mbo[c].getThreshold());
		}
	    }
	}
    }
    SEPERATOR_msg("-",60);
}

void HMdcTdcThreshold::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcTdcThreshold.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HParSet::Streamer(R__b);
      R__b >> array;
      R__b >> oraVersion;
      if (R__v==3) {
        Int_t* dummy;
        R__b >> dummy;  // HMdcRawStruct*
        R__b >> dummy;  // HMdcRawSecStru*
        R__b >> dummy;  // HMdcMboReadout*
      }
      R__b.CheckByteCount(R__s, R__c, HMdcTdcThreshold::IsA());
   } else {
      R__c = R__b.WriteVersion(HMdcTdcThreshold::IsA(), kTRUE);
      HParSet::Streamer(R__b);
      R__b << array;
      R__b << oraVersion;
      R__b.SetByteCount(R__c, kTRUE);
   }
}
