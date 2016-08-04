//*-- AUTHOR : J.Wuestenfeld
//*-- Modified : 13/08/2003 by J. Wuestenfeld

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
// HMdcTdcChannel
//
// Container class for the TDC channle masks of the MDC
//
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>

#include "hmdctdcchannel.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hmdcrawstruct.h"
#include "hmdclookupgeom.h"
#include "hmessagemgr.h"
#include "TSystem.h"
#include <iostream>
#include <iomanip>
#include <cerrno>

ClassImp(HMdcTdcChannelTdc)
ClassImp(HMdcTdcChannelMbo)
ClassImp(HMdcTdcChannelMod)
ClassImp(HMdcTdcChannelSec)
ClassImp(HMdcTdcChannel)

void HMdcTdcChannelTdc::fill(HMdcTdcChannelTdc& r)
{
  channelMask[0]=r.getChannelMask(0);
	channelMask[1]=r.getChannelMask(1);
}

HMdcTdcChannelMbo::HMdcTdcChannelMbo(Int_t tdc, const Text_t* name)
{
  // constructor takes the number of TDCs  and the name of the MBO
  SetName(name);
  array = new TObjArray(tdc);
  for (Int_t i=0; i<tdc; ++i) array->AddAt(new HMdcTdcChannelTdc(),i);
}

HMdcTdcChannelMbo::~HMdcTdcChannelMbo()
{
  // destructor
  array->Delete();
  delete array;
}

HMdcTdcChannelMod::HMdcTdcChannelMod(Int_t mbo)
{
  // constructor takes the number of MBOs
  // The array objects of type HMdcTdcChannelMbo are created in the init function.
  array = new TObjArray(mbo);
}

HMdcTdcChannelMod::~HMdcTdcChannelMod()
{
  // destructor
  array->Delete();
  delete array;
}

void HMdcTdcChannelMod::createMbo(Int_t mbo, Int_t nTdcs, const Char_t* mboName)
{
  // creates the array object of type HMdcTdcChannelMbo
  if (mbo<array->GetSize() && array->At(mbo)==NULL) {
    array->AddAt(new HMdcTdcChannelMbo(nTdcs,mboName),mbo);
  }
}

HMdcTdcChannelSec::HMdcTdcChannelSec(Int_t mod)
{
  // constructor takes the number of modules
  array = new TObjArray(mod);
  for (Int_t i=0; i<mod; i++)
        array->AddAt(new HMdcTdcChannelMod(),i);
}

HMdcTdcChannelSec::~HMdcTdcChannelSec()
{
  // destructor
  array->Delete();
  delete array;
}

HMdcTdcChannel::HMdcTdcChannel(const Char_t* name,const Char_t* title,
                             const Char_t* context,Int_t n)
              : HParSet(name,title,context)
{
  // constructor
  strcpy(detName,"Mdc");
  oraVersion=-1;
  array = new TObjArray(n);
  mapNotConnectedChannels = kFALSE;
  status = kTRUE;
  for (Int_t i=0; i<n; i++) array->AddAt(new HMdcTdcChannelSec(),i);
  pRawStruct=(HMdcRawStruct*)(gHades->getRuntimeDb()->getContainer("MdcRawStruct"));
  pLookupGeom = (HMdcLookupGeom *)(gHades->getRuntimeDb()->getContainer("MdcLookupGeom"));
}

HMdcTdcChannel::~HMdcTdcChannel()
{
  // destructor
  array->Delete();
  delete array;
}

Bool_t HMdcTdcChannel::init(HParIo* inp,Int_t* set)
{
  // creates the array objects of type HMdcTdcChannelMbo using the data in the parameter container
  // MdcRawStruct and initializes the container from an input
  if (pRawStruct==NULL) {
    Error("HMdcTdcChannel::init()","MdcTdcChannel not initialized, container MdcRawStruct is NULL");
    return kFALSE;
  }
  for(Int_t s=0;s<getSize();s++) {
    HMdcTdcChannelSec& secPar=(*this)[s];
    HMdcRawSecStru& secStru=(*pRawStruct)[s];
    for(Int_t m=0;m<secPar.getSize();m++) {
      HMdcTdcChannelMod& modPar=secPar[m];
      HMdcRawModStru& modStru=secStru[m];
      for(Int_t i=0;i<modStru.getSize();i++) {
        modPar.createMbo(i,modStru[i].getNTdcs()/8,modStru[i].GetName());
      }
    }
  }
  if (pLookupGeom==NULL) {
    Error("HMdcTdcChannel::init()","Container MdcLookupGeom is NULL");
    return kFALSE;
  }
  HDetParIo* input=inp->getDetParIo("HMdcParIo");
  if(input) if  (!input->init(this,set)) return kFALSE;
  return kTRUE;
}

Int_t HMdcTdcChannel::write(HParIo* output)
{
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HMdcParIo");
  if (out) return out->write(this);
  return -1;
}

void HMdcTdcChannel::readline(const Char_t* buf, Int_t* set)
{
  // decodes one line read from ascii file I/O and calls HMdcTdcChannelTdc::fill(...)
  Int_t sec, mod, mbo, dbo;
  Int_t cal,meas;
  Char_t mboName[20];

  sscanf(buf,"%i%i%i%s%i%x%x",&sec, &mod, &mbo, mboName, &dbo, &cal,&meas);
  Int_t n=sec*4+mod;
  if (!set[n]) return;
  HMdcTdcChannelMbo& rMbo=(*this)[sec][mod][mbo];
  rMbo.SetName(mboName);
  HMdcTdcChannelTdc& tdc=rMbo[dbo];
  tdc.fill(cal,0);
  tdc.fill(meas,1);
  set[n]=999;
}

void HMdcTdcChannel::readLineFromFile(const Char_t* buf)
{
  // decodes one line read from ascii file I/O and calls HMdcTdcChannelTdc::fill(...)
  Int_t sec, mod, mbo, dbo;
  Int_t cal,meas;
  sscanf(buf,"%i%i%i%i%x%x", &mod, &sec, &mbo, &dbo, &cal,&meas);
  HMdcRawModStru & rawMod=(*pRawStruct)[sec][mod];
  HMdcRawMothStru &rawMbo=rawMod[mbo];
  if (rawMbo.getNTdcs() >= dbo*16 ) {
      HMdcTdcChannelMbo& rMbo=(*this)[sec][mod][mbo];
      HMdcTdcChannelTdc& db=rMbo[dbo-1];
      db.fill(cal,0);
      db.fill(meas,1);
    }
}

void HMdcTdcChannel::putAsciiHeader(TString& header)
{
  // puts the ascii header to the string used in HMdcParAsciiFileIo
  header=
    "# Channelmasks of the MDC TDC\n"
    "# Format:\n"
    "# sector  module  mbo  mboName  TDC#  cal. mask  meas. mask\n";
}

Bool_t HMdcTdcChannel::writeline(Char_t *buf, Int_t sec, Int_t mod, Int_t mbo, Int_t dbo) {
  // writes one line to the buffer used by ascii file I/O
  HMdcTdcChannelMbo& rMbo=(*this)[sec][mod][mbo];
  HMdcTdcChannelTdc& db=rMbo[dbo];
  sprintf(buf,"%1i %1i %2i %s %2i %2x %2x\n",
          sec, mod, mbo, rMbo.GetName(), dbo, db.getChannelMask(0),db.getChannelMask(1));
  return kTRUE;
}

void HMdcTdcChannel::clear() {
  // clears the container
  oraVersion=-1;
  for(Int_t s=0;s<getSize();s++) {
    HMdcTdcChannelSec& sec=(*this)[s];
    for(Int_t m=0;m<sec.getSize();m++) {
      HMdcTdcChannelMod& mod=sec[m];
      for(Int_t l=0;l<mod.getSize();l++) {
        HMdcTdcChannelMbo& mbo=mod[l];
        for(Int_t c=0;c<mbo.getSize();c++) mbo[c].clear();
      }
    }
  }
  status=kFALSE;
  resetInputVersions();
}

void HMdcTdcChannel::printParam() {
  // prints the container
	SEPERATOR_msg("-",60);
	INFO_msg(10,HMessageMgr::DET_MDC,"HMdcTdcChannel");
	INFO_msg(10,HMessageMgr::DET_MDC,"sector module layer mbo channel calMask measurementMask");
	for(Int_t s=0;s<getSize();s++) {
		HMdcTdcChannelSec& sec=(*this)[s];
		for(Int_t m=0;m<sec.getSize();m++) {
			HMdcTdcChannelMod& mod=sec[m];
			for(Int_t l=0;l<mod.getSize();l++) {
				HMdcTdcChannelMbo& mbo=mod[l];
				for(Int_t c=0;c<mbo.getSize();c++){
					gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
												 "%1i %1i %2i %s %1i %2x %2x",
						 s, m, l, mbo.GetName(), c, mbo[c].getChannelMask(0),
																	mbo[c].getChannelMask(1));
				}
			}
		}
	}
	SEPERATOR_msg("-",60);
}

void HMdcTdcChannel::mapOutUnusedChannels(void)
{
  // Maps out all channels that have no wire connected. Test is done against HMdcLookupGeom container.
	for(Int_t s=0;s<getSize();s++)
	{
		HMdcTdcChannelSec& sec=(*this)[s];
		for(Int_t m=0;m<sec.getSize();m++)
		{
			HMdcTdcChannelMod& mod=sec[m];
			for(Int_t l=0;l<mod.getSize();l++)
			{
				HMdcTdcChannelMbo& mbo=mod[l];
				for(Int_t c=0;c<mbo.getSize();c++)
				{
					HMdcTdcChannelTdc &tdc=mbo[c];
					for(Int_t ch=0;ch<8;ch++)
					{
						if((*pLookupGeom)[s][m][l][c*8+ch].getNCell() < 0)
						{
							tdc.setChannelMask(tdc.getChannelMask(0)&(~(0x1<<ch)),0);
							tdc.setChannelMask(tdc.getChannelMask(1)&(~(0x1<<ch)),1);
						}
					}
				}
			}
		}
	}
}

void HMdcTdcChannel::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcTdcChannel.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HParSet::Streamer(R__b);
      R__b >> array;
      R__b >> oraVersion;
      R__b >> status;
      R__b >> mapNotConnectedChannels;
      if (R__v==1) {
        Int_t* dummy;
        R__b >> dummy;  // HMdcEvReadout*
      }
      R__b.CheckByteCount(R__s, R__c, HMdcTdcChannel::IsA());
   } else {
      R__c = R__b.WriteVersion(HMdcTdcChannel::IsA(), kTRUE);
      HParSet::Streamer(R__b);
      R__b << array;
      R__b << oraVersion;
      R__b << status;
      R__b << mapNotConnectedChannels;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

