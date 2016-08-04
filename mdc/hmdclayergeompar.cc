//*-- AUTHOR : Alexander Nekhaev
//*-- Modified : 17/01/2002 by I. Koenig

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
// HMdcLayerGeomPar
//
// Container class for the Mdc specific geometry parameters of
// the layers
// The parameter container is used by several tasks: digitizer,
// hit finder, event display.
// This class replaces the former class HMdcDigitGeomPar.
//
////////////////////////////////////////////////////////////////

#include "hmdclayergeompar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hmdcgeomstruct.h"
#include "hmessagemgr.h"

ClassImp(HMdcLayerGeomParLay)
ClassImp(HMdcLayerGeomParMod)
ClassImp(HMdcLayerGeomParSec)
ClassImp(HMdcLayerGeomPar)

void HMdcLayerGeomParLay::fill(HMdcLayerGeomParLay& r)
{
  pitch=r.getPitch();
  catDist=r.getCatDist();
  wireOrient=r.getWireOrient();
  centralWireNr=r.getCentralWireNr();
  cathodeWireThickness=r.getCathodeWireThickness();
}

void HMdcLayerGeomParLay::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcLayerGeomParLay.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> pitch;
      R__b >> catDist;
      R__b >> wireOrient;
      R__b >> centralWireNr;
      if (R__v>1) {
        R__b >> cathodeWireThickness;
      }
      R__b.CheckByteCount(R__s, R__c, HMdcLayerGeomParLay::IsA());
   } else {
      R__c = R__b.WriteVersion(HMdcLayerGeomParLay::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << pitch;
      R__b << catDist;
      R__b << wireOrient;
      R__b << centralWireNr;
      R__b << cathodeWireThickness;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

HMdcLayerGeomParMod::HMdcLayerGeomParMod() {
  // constructor creates an array of 6 pointers of type HMdcLayerGeomParLay
  array = new TObjArray(6);
  for (Int_t i = 0; i < 6; i++)
      array->AddAt( new HMdcLayerGeomParLay(),i);
}

HMdcLayerGeomParMod::~HMdcLayerGeomParMod() {
  // destructor deletes the pointer array
  array->Delete();
  delete array;
}

Int_t HMdcLayerGeomParMod::getSize() {
  // return the size of the pointer array 
  return array->GetEntries();
}

HMdcLayerGeomParSec::HMdcLayerGeomParSec() {
  // constructor creates an array of 4 pointers of type HMdcLayerGeomParMod
  array = new TObjArray(4);
  for (Int_t i = 0; i < 4; i++)
      array->AddAt( new HMdcLayerGeomParMod,i);
}

HMdcLayerGeomParSec::~HMdcLayerGeomParSec() {
  // destructor deletes the pointer array
  array->Delete();
  delete array;
}


Int_t HMdcLayerGeomParSec::getSize() {
  // return the size of the pointer array 
  return array->GetEntries();
}

HMdcLayerGeomPar::HMdcLayerGeomPar(const Char_t* name,const Char_t* title,
                                   const Char_t* context)
                 : HParSet(name,title,context) {
  // constructor creates an array of 6 pointers of type
  // HMdcLayerGeomParSec
  strcpy(detName,"Mdc");
  array = new TObjArray(6);
  for (Int_t i = 0; i < 6; i++) {
    array->AddAt(new HMdcLayerGeomParSec,i);
  }
  gHades->getRuntimeDb()->getContainer("MdcGeomStruct");
  ntuple=0;
}

HMdcLayerGeomPar::~HMdcLayerGeomPar() {
  // destructor deletes the pointer array
  array->Delete();
  delete array;
  if (ntuple) delete ntuple;
}

Int_t HMdcLayerGeomPar::getSize() {
  // return the size of the pointer array
  return array->GetEntries();
}

Bool_t HMdcLayerGeomPar::init(HParIo* inp,Int_t* set) {
  // initializes the container from an input
  // The number of wires is copied from the parameter container "MdcGeomStruct"
  // which must be initialized before calling the constructor.
  HDetParIo* input=inp->getDetParIo("HMdcParIo");
  Bool_t rc=kFALSE;
  if (input) {
    rc=input->init(this,set);
    if (rc&&changed) {
      calcLayerTransformations();
      HMdcGeomStruct* pMdc=
           (HMdcGeomStruct*)gHades->getRuntimeDb()->findContainer("MdcGeomStruct");
      if (pMdc) {
        for(Int_t i=0;i<6;i++) {
          for(Int_t j=0;j<4;j++) {
            for(Int_t k=0;k<6;k++) {
              (*this)[i][j][k].setNumWires(((*pMdc)[i][j][k]));
            }
          }
        }
      } else {
        Error("HMdcLayerGeomPar::init()","numWires in MdcLayerGeomPar not initialized, MdcGeomStruct not found");
        pMdc->setChanged(kFALSE);
        rc=kFALSE;
      }
    }
  }
  return rc;
}

void HMdcLayerGeomPar::calcLayerTransformations() {
  // Calculates the coordinate systems of the layers based on cell and cathode wire
  // thicknesses
  Float_t pos0, catDist;
  HGeomVector pos;
  for(Int_t s=0;s<6;s++) {
    HMdcLayerGeomParSec& sec=(*this)[s];
    for(Int_t m=0;m<4;m++) {
      HMdcLayerGeomParMod& mod=sec[m];
      pos0=mod[3].getCathodeWireThickness()/2.;
      for(Int_t l=0;l<3;l++) {
        pos0-=mod[l].getCatDist();
      }
      for(Int_t l=0;l<mod.getSize();l++) {
        HMdcLayerGeomParLay& lay=mod[l];
        catDist=lay.getCatDist();
        pos.setZ(pos0+catDist/2.);
        lay.getLayerPos().setTransVector(pos);
        pos0+=catDist;
      }
    }
  }
}

Int_t HMdcLayerGeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HMdcParIo");
  if (out) return out->write(this);
  return -1;
}

void HMdcLayerGeomPar::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ascii file I/O
    Int_t sector, module, layer, pos;
    Float_t pit, cd, wor, cwn, cwt;
    sscanf(buf,"%i%i%i%f%f%f%f%f",&sector,&module,&layer,&pit,&cd,&wor,&cwn,&cwt);
    pos=sector*4+module;
    if (!set[pos]) return;
    HMdcLayerGeomParLay& l=(*this)[sector][module][layer];
    l.fill(pit,cd,wor,cwn,cwt);
    set[pos]=999;
    return;
}

void HMdcLayerGeomPar::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HMdcParAsciiFileIo
  header=
    "# Geometry parameters for digitization of the MDC\n"
    "# Format:\n"
    "# sector   module   layer\n"
    "#       pitch\n"
    "#       distance of cathode planes\n"
    "#       wire orientation\n"
    "#       number of central wire\n"
    "#       thickness of cathode wires\n";
}

Bool_t HMdcLayerGeomPar::writeline(Char_t *buf, Int_t sec, Int_t mod, Int_t lay) {
  // writes one line to the buffer used by ascii file I/O
    HMdcLayerGeomParLay &l = (*this)[sec][mod][lay];
    if (l.getNumWires()<=0) return kFALSE;
    sprintf(buf,"%1i %1i %1i %8.4f %8.4f %8.4f %8.4f %8.3f\n",
            sec, mod, lay,
            l.getPitch(), l.getCatDist(), l.getWireOrient(),
            l.getCentralWireNr(),l.getCathodeWireThickness());
    return kTRUE;
}

void HMdcLayerGeomPar::clear() {
  // clears the container
  for(Int_t s=0;s<6;s++) {
    HMdcLayerGeomParSec& sec=(*this)[s];
    for(Int_t m=0;m<4;m++) {
      HMdcLayerGeomParMod& mod=sec[m];
      for(Int_t l=0;l<mod.getSize();l++) {
        HMdcLayerGeomParLay& lay=mod[l];
	lay.clear();
      }
    }
  }
  status=kFALSE;
  resetInputVersions();
}

TNtuple* HMdcLayerGeomPar::getNtuple() {
  // fills the parameters into an TNtuple for drawing
  if (ntuple) ntuple->Reset();
  else ntuple=new TNtuple("MdcLayerGeomParNtuple","Ntuple of Mdc layer geometry",
                          "s:m:l:pt:nw:cd:wor:wof:cwt");
  Int_t nw;
  Float_t pt, cd, wor, wof, cwt;
  for(Int_t s=0;s<6;s++) {
    HMdcLayerGeomParSec& sec=(*this)[s];
    for(Int_t m=0;m<4;m++) {
      HMdcLayerGeomParMod& mod=sec[m];
      for(Int_t l=0;l<mod.getSize();l++) {
        HMdcLayerGeomParLay& lay=mod[l];
        pt=lay.getPitch();
        nw=lay.getNumWires();
        cd=lay.getCatDist();
        wor=lay.getWireOrient();
        wof=lay.getCentralWireNr();
        cwt=lay.getCathodeWireThickness();
        ntuple->Fill(s,m,l,pt,nw,cd,wor,wof,cwt);
      }
    }
  }
  return ntuple;
}

void HMdcLayerGeomPar::printParam() {
  for(Int_t s=0;s<6;s++) {
    HMdcLayerGeomParSec& sec=(*this)[s];
    for(Int_t m=0;m<4;m++) {
      HMdcLayerGeomParMod& mod=sec[m];
      for(Int_t l=0;l<mod.getSize();l++) {
        HMdcLayerGeomParLay& lay=mod[l];
        Int_t nw=lay.getNumWires();
        if (nw)
	  gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
				 "%1i %1i %1i %8.4f %5i %8.4f %8.4f %8.4f %8.4f %8.4f\n",
				 s, m, l, lay.getPitch(), nw, lay.getCatDist(),
				 lay.getWireOrient(), lay.getCentralWireNr(),
                                 lay.getCathodeWireThickness(),
                                 lay.getLayerPos().getTransVector().getZ());
      }
    }
  }
  SEPERATOR_msg("-",60);
}
