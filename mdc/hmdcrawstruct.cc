//*-- AUTHOR : Ilse Koenig
//*-- Modified : 17/01/2002 by I. Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HMdcRawStruct
//
// Container class for parameters decribing the hardware
// tree of an Mdc (sectors,modules,motherboards with the
// maximum number of TDCs on each motherboards.
// This information is needed to create the data tree and to
// create the lookup table for the Mdc. 
//
/////////////////////////////////////////////////////////////

#include "hmdcrawstruct.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hmessagemgr.h"

ClassImp(HMdcRawMothStru)
ClassImp(HMdcRawModStru)
ClassImp(HMdcRawSecStru)
ClassImp(HMdcRawStruct)

HMdcRawModStru::HMdcRawModStru(Int_t n) {
  // constructor creates an array of pointers of type
  // HMdcRawMothStru
  array = new TObjArray(n);
  for(Int_t i=0;i<n;i++) array->AddAt(new HMdcRawMothStru(),i);
}


HMdcRawModStru::~HMdcRawModStru() {
  // destructor
  array->Delete();
  delete array;
}


Int_t HMdcRawModStru::getMboIndex(const Text_t* mbo) {
  // returns the index of the MBO given by its logical position
  for(Int_t i=0;i<getNMotherboards();i++) {
    const Text_t* name=array->At(i)->GetName();
    if (strcmp(name,mbo)==0) return i;
  }
  return -1;
}


HMdcRawSecStru::HMdcRawSecStru(Int_t n) {
  // constructor creates an array of pointers of type
  // HMdcRawModStru
  array = new TObjArray(n);
  for(Int_t i=0;i<n;i++) array->AddAt(new HMdcRawModStru(16),i);
}


HMdcRawSecStru::~HMdcRawSecStru() {
  // destructor
  array->Delete();
  delete array;
}


HMdcRawStruct::HMdcRawStruct(const Char_t* name,const Char_t* title,
                             const Char_t* context,Int_t n)
              : HParSet(name,title,context) {
  // constructor creates an array of pointers of type HMdcRawSecStru
  strcpy(detName,"Mdc");
  array = new TObjArray(n);
  for(Int_t i=0;i<n;i++) array->AddAt(new HMdcRawSecStru(4),i);
}


HMdcRawStruct::~HMdcRawStruct() {
  // destructor
  array->Delete();
  delete array;
}


Bool_t HMdcRawStruct::init(HParIo* inp,Int_t* set) {
  // initializes the container from an input
  HDetParIo* input=inp->getDetParIo("HMdcParIo");
  Bool_t rc=kFALSE;
  if (input) {
    rc=input->init(this,set);
    if (rc) status=kTRUE;
  }
  return rc;
}


Int_t HMdcRawStruct::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HMdcParIo");
  if (out) return out->write(this);
  return -1;
}

void HMdcRawStruct::clear() {
  // clears the container
  for(Int_t i=0;i<6;i++) {
    HMdcRawSecStru& sec=(*this)[i];
    for(Int_t j=0;j<4;j++) {
      HMdcRawModStru& mod=sec[j];
      for(Int_t k=0;k<16;k++) {
        HMdcRawMothStru& moth=mod[k];
        moth.clear();
      }
    }
  }
  status=kFALSE;
  resetInputVersions();
}


void HMdcRawStruct::printParam() {
  // prints the names (characterizes the position on the chamber) of the
  // motherboards and the number of TDCs on each of them;
  Char_t buf[255];
  SEPERATOR_msg("-",60);
  INFO_msg(10,HMessageMgr::DET_MDC,"Position identifier of motherboards");
  INFO_msg(10,HMessageMgr::DET_MDC,"number of TDCs");

  HMdcDetector* set=(HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector("Mdc"));
  if (set) {
    for(Int_t i=0;i<6;i++) {
      HMdcRawSecStru& sec=(*this)[i];
      for(Int_t j=0;j<4;j++) {
        HMdcRawModStru& mod=sec[j];
        if (set->getModule(i,j)) {
	    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
				   ,"sector: %i  module: %i",i,j);
          for(Int_t k=0;k<16;k++) {
	    sprintf(buf,"%s %s",buf,mod[k].GetName());
          }
	  INFO_msg(10,HMessageMgr::DET_MDC,buf);
					strcpy(buf,"");
          for(Int_t k=0;k<16;k++) {
            Int_t nt=mod[k].getNTdcs();
	    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"%3i ",nt);
          }
        }
      }
    }
    SEPERATOR_msg("-",60);
  }
}
          

void HMdcRawStruct::getMaxIndices(TArrayI* ind) {
  // returns the maximum indices of the sectors, modules, motherborads,
  // tdc channels in the actual setup
  Int_t maxSec=-1, maxMod=-1, maxMbo=-1, maxTdc=-1;
  for(Int_t i=0;i<6;i++) {
    HMdcRawSecStru& sec=(*this)[i];
    for(Int_t j=0;j<4;j++) {
      HMdcRawModStru& mod=sec[j];
      for(Int_t k=0;k<16;k++) {
        Int_t nt=mod[k].getNTdcs()-1; // index is number of tds -1
        if (nt>=0) {
          maxSec=(i>maxSec)?i:maxSec;
          maxMod=(j>maxMod)?j:maxMod;
          maxMbo=(k>maxMbo)?k:maxMbo;
          maxTdc=(nt>maxTdc)?nt:maxTdc;
        }
      }
    }
  }
  ind->AddAt(maxSec,0);
  ind->AddAt(maxMod,1);
  ind->AddAt(maxMbo,2);
  ind->AddAt(maxTdc,3);
}

void HMdcRawStruct::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ascii file I/O
  Int_t sec, mod, mboInd, num;
  Char_t mboName[155];
  sscanf(buf,"%i%i%i%s%i", &sec, &mod, &mboInd, mboName, &num);
  Int_t n=sec*4+mod;
  if (!set[n]) return;
  HMdcRawMothStru& r=(*this)[sec][mod][mboInd];
  r.SetName(mboName);
  r.setNTdcs(num);
  set[n]=999;
}

void HMdcRawStruct::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HMdcParAsciiFileIo
  header=
    "# Parameters describing the hardware of the MDC\n"
    "# Format:\n"
    "# sector   module   motherboard index   motherboard name\n"
    "#    number of TDC channels on the board\n";
}

Bool_t HMdcRawStruct::writeline(Char_t *buf, Int_t sec, Int_t mod, Int_t mbo) {
  // writes one line to the buffer used by ascii file I/O
  HMdcRawMothStru &moth=(*this)[sec][mod][mbo];
  Int_t nTdcs=moth.getNTdcs();
  if (nTdcs<=0) return kFALSE;
  sprintf(buf,"%1i %1i %2i %s %2i\n",sec,mod,mbo,moth.GetName(),nTdcs);
  return kTRUE;
}
