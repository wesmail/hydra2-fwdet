//*-- AUTHOR : Ilse Koenig
//*-- Modified : 25/08/2009 by I. Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//HMdcLookupGeom
//
//  Container class for lookup parameters to map the hardware
//  tree of the Mdc (sectors, modules, motherboards, TDC,
//  TDC channel) to the geometrie tree (sectors, modules,
//  layer, cell).
//  This information is needed for data mapping from raw
//  level to cal1 level
//
/////////////////////////////////////////////////////////////

#include "hmdclookupgeom.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hmdcrawstruct.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hmessagemgr.h"

ClassImp(HMdcLookupChan)
ClassImp(HMdcLookupMoth)
ClassImp(HMdcLookupGMod)
ClassImp(HMdcLookupGSec)
ClassImp(HMdcLookupGeom)

void HMdcLookupChan::Streamer(TBuffer &R__b) {
  // Stream an object of class HMdcLookupChan.
  UInt_t R__s, R__c;
  if (R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
    TObject::Streamer(R__b);
    R__b >> nLayer;
    R__b >> nCell;
    if (R__v>1) {
      R__b >> readoutSide;
    } else {
      readoutSide='0';
    }
    R__b.CheckByteCount(R__s, R__c, HMdcLookupChan::IsA());
  } else {
    R__c = R__b.WriteVersion(HMdcLookupChan::IsA(), kTRUE);
    TObject::Streamer(R__b);
    R__b << nLayer;
    R__b << nCell;
    R__b << readoutSide;
    R__b.SetByteCount(R__c, kTRUE);
  }
}

HMdcLookupMoth::HMdcLookupMoth(Int_t n, const Text_t* name) {
  // constructor creates an array of pointers of type
  // HMdcLookupChan (array of all Tdc channels on a
  // motherboard)
  SetName(name);
  array = new TObjArray(n);
  for (Int_t i = 0; i < n; i++) {
    array->AddAt(new HMdcLookupChan(),i);
  }
}

HMdcLookupMoth::~HMdcLookupMoth() {
  // destructor
  array->Delete();
  delete array;
}

HMdcLookupGMod::HMdcLookupGMod(Int_t n) {
  // constructor creates an array for the motherboards
  // the array objects of type HMdcLookupMoth are created in the init function
  array = new TObjArray(n);
}

HMdcLookupGMod::~HMdcLookupGMod() {
  // destructor
  array->Delete();
  delete array;
}

void HMdcLookupGMod::createMoth(Int_t moth, Int_t nTdcs, const Char_t* mothName) {
  // creates the array object of type HMdcLookupMoth
  if (moth<array->GetSize()&&array->At(moth)==NULL) {
    array->AddAt(new HMdcLookupMoth(nTdcs,mothName),moth);
  }
}

HMdcLookupGSec::HMdcLookupGSec(Int_t n) {
  // constructor creates an array of pointers of type HMdcLookupGMod
  array = new TObjArray(n);
  for(Int_t i=0;i<n;i++) array->AddAt(new HMdcLookupGMod(),i);
}

HMdcLookupGSec::~HMdcLookupGSec() {
  // destructor
  array->Delete();
  delete array;
}

HMdcLookupGeom::HMdcLookupGeom(const Char_t* name,const Char_t* title,
                               const Char_t* context,Int_t n)
               : HParSet(name,title,context) {
  // constructor creates an array of pointers of type HMdcLookupGSec
  strcpy(detName,"Mdc");
  array = new TObjArray(n);
  for(Int_t i=0;i<n;i++) array->AddAt(new HMdcLookupGSec(),i);
  gHades->getRuntimeDb()->getContainer("MdcRawStruct");
}

HMdcLookupGeom::~HMdcLookupGeom() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HMdcLookupGeom::init(HParIo* inp,Int_t* set) {
  // creates the array objects of type HMdcLookupMoth using the data in the parameter container
  // MdcRawStruct and initializes the container from an input
  HMdcRawStruct* pRawStruct=
       (HMdcRawStruct*)gHades->getRuntimeDb()->findContainer("MdcRawStruct");
  if (pRawStruct==NULL) {
    Error("HMdcLookupGeom::init()","HMdcLookupGeom not initialized, container MdcRawStruct not found");
    return kFALSE;
  }
  for(Int_t s=0;s<getSize();s++) {
    HMdcLookupGSec& secLookup=(*this)[s];
    HMdcRawSecStru& secStru=(*pRawStruct)[s];
    for(Int_t m=0;m<secLookup.getSize();m++) {
      HMdcLookupGMod& modLookup=secLookup[m];
      HMdcRawModStru& modStru=secStru[m];
      for(Int_t i=0;i<modStru.getSize();i++) {
        modLookup.createMoth(i,modStru[i].getNTdcs(),modStru[i].GetName());
      }
    }
  }
  HDetParIo* input=inp->getDetParIo("HMdcParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HMdcLookupGeom::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HMdcParIo");
  if (out) return out->write(this);
  return -1;
}

void HMdcLookupGeom::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ascii file I/O
   Int_t sec, mod, mboInd, chan, lay, wire;
   Char_t side;
   sscanf(buf,"%i%i%i%i%i%i %c",
          &sec, &mod, &mboInd, &chan, &lay, &wire, &side);
   Int_t n=sec*4+mod;
   if (!set[n]) return;
   HMdcLookupChan& r=(*this)[sec][mod][mboInd][chan];
   r.fill(lay,wire,side);
   set[n]=999;
   return;
}

void HMdcLookupGeom::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HMdcParAsciiFileIo
  header=
    "# Mdc lookup table (hardware->geometry)\n"
    "# Format:\n"
    "# sector  module  motherboard index  TDC channel  layer  cell  readout side\n";
}

Bool_t HMdcLookupGeom::writeline(Char_t *buf, Int_t sec, Int_t mod, Int_t lay,
                               Int_t cell) {
  // writes one line to the buffer used by ascii file I/O
  HMdcLookupChan &chan=(*this)[sec][mod][lay][cell];
  Int_t nCell=chan.getNCell();
  if (nCell==-1) return kFALSE;
  sprintf(buf,"%1i %1i %1i %3i %1i %3i %c\n",
          sec, mod, lay, cell, chan.getNLayer(), nCell, chan.getReadoutSide());
  return kTRUE;
}

void HMdcLookupGeom::clear() {
  // clears the container
  for(Int_t s=0;s<6;s++) {
    HMdcLookupGSec& sec=(*this)[s];
    for(Int_t m=0;m<4;m++) {
      HMdcLookupGMod& mod=sec[m];
      Int_t nl=mod.getSize();
      for(Int_t l=0;l<nl;l++) {
        HMdcLookupMoth& moth=mod[l];
        Int_t nm=moth.getSize();
        for(Int_t c=0;c<nm;c++) {
          HMdcLookupChan& chan=moth[c];
          chan.clear();
        }
      }
    }
  }
  status=kFALSE;
  resetInputVersions();
}
void HMdcLookupGeom::printParam() {
  // prints the container
  SEPERATOR_msg("-",60);
  INFO_msg(10,HMessageMgr::DET_MDC,"HMdcLookupGeom :");
  for(Int_t s=0;s<6;s++) {
    HMdcLookupGSec& sec=(*this)[s];
    for(Int_t m=0;m<4;m++) {
      HMdcLookupGMod& mod=sec[m];
      Int_t nl=mod.getSize();
      for(Int_t l=0;l<nl;l++) {
        HMdcLookupMoth& moth=mod[l];
        Int_t nm=moth.getSize();
        for(Int_t c=0;c<nm;c++) {
          HMdcLookupChan& chan=moth[c];
	  Int_t nCell=chan.getNCell();
	  Int_t nLay =chan.getNLayer();
          Char_t side=chan.getReadoutSide();
	  gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
				 ,"%1i %1i %1i %3i %1i %3i %c\n",
				 s, m, l, c, nLay, nCell, side);
	}
      }
    }
  }
  SEPERATOR_msg("-",60);
}
