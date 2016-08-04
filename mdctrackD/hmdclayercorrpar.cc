//*-- AUTHOR : Olga Pechenova
//*-- Modified : 2007 by Olga Pechenova
//
//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////
//
//  HMdcLayerCorrPar
//
//  Container class keep parameters for distance correction (between 2 parts of layer)
//
//
//
//  Alternative method of getting parameters (without usage of runTimeDb):
//  setDefaultPar();
//
/////////////////////////////////////////////////////////////////////////


#include "TString.h"
#include "hmdclayercorrpar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"

#include <stdlib.h> 

ClassImp(HMdcLayerCorrPar)

HMdcLayerCorrPar::HMdcLayerCorrPar(const Char_t* name,const Char_t* title,
                const Char_t* context)
           : HParCond(name,title,context) {
  // constructor
  nMdc = 4;
  nLay = 6;
  numCorrLayers = 72;
  for(Int_t s=0;s<numCorrLayers;s++) {
    firstWire[s]      = 777;
    shift[s]          = 0.f;
    wireOrientCorr[s] = 0.f;
  }    
}

Bool_t HMdcLayerCorrPar::init(HParIo* inp,Int_t* set) {
 // initializes the container from an input
  HDetParIo* input=inp->getDetParIo("HCondParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

void HMdcLayerCorrPar::clear(void) {
  for(Int_t s=0;s<numCorrLayers;s++) {
    firstWire[s]      = 777;
    shift[s]          = 0.f;
    wireOrientCorr[s] = 0.f;
  }    
  status=kFALSE;
  resetInputVersions();
  changed=kFALSE;
}

Int_t HMdcLayerCorrPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HCondParIo");
  if (out) return out->write(this);
  return 0;
}

void HMdcLayerCorrPar::putParams(HParamList* l) {
  // puts all parameters to the parameter list, which is used by the io
  if (!l) return;
  l->add("firstWire",firstWire,numCorrLayers);
  l->add("shift",shift,numCorrLayers);
  l->add("wireOrientCorr",wireOrientCorr,numCorrLayers);
}

Bool_t HMdcLayerCorrPar::getParams(HParamList* l) {
  // gets all parameters from the parameter list, which is used by the io
  if (!l) return kFALSE;
  if (!(l->fill("firstWire",firstWire,numCorrLayers)))           return kFALSE;
  if (!(l->fill("shift",shift,numCorrLayers)))                   return kFALSE;
  if (!(l->fill("wireOrientCorr",wireOrientCorr,numCorrLayers))) return kFALSE;
  return kTRUE;
}


Bool_t HMdcLayerCorrPar::addLayerShift(Int_t s,Int_t m,Int_t l,
    Int_t fstWr,Float_t sh,Float_t orCorr) {
  // add or change correction parameters
  if(fstWr>0 && isAddOk(s,m,l)) {
    Int_t ind = (s*(nMdc-2)+(m-2))*nLay+l;
    firstWire[ind]      = fstWr;
    shift[ind]          = sh;
    wireOrientCorr[ind] = orCorr;
    return kTRUE;
  }
  return kFALSE;
}

void HMdcLayerCorrPar::setDefaultPar(void) {
  addLayerShift(0,2,1,96,0.25);
  addLayerShift(0,2,2,88,0.15);
//  addLayerShift(0,2,3,82,0.1);
  addLayerShift(0,2,4,100,0.15);
  addLayerShift(0,2,5,59,0.1);
  addLayerShift(0,3,0,92,0.2);
  addLayerShift(0,3,1,104,0.42);
  addLayerShift(0,3,2,82,0.65);
  addLayerShift(0,3,3,87,0.1);
  addLayerShift(0,3,4,93,0.1);
  addLayerShift(0,3,5,48,0.52);

  addLayerShift(1,2,0,60,0.2);
  addLayerShift(1,2,1,96,0.05);
  addLayerShift(1,2,3,83,0.1);
  addLayerShift(1,2,4,106,0.4);
  addLayerShift(1,2,5,55,0.22);
  addLayerShift(1,3,0,93,0.15);
  addLayerShift(1,3,1,102,0.45);
  addLayerShift(1,3,2,77,0.25);
  addLayerShift(1,3,3,109,-0.1);
  addLayerShift(1,3,5,100,0.15);

  addLayerShift(2,2,0,59,0.2);
  addLayerShift(2,2,2,92,0.15);
  addLayerShift(2,2,3,50,-0.1);
  addLayerShift(2,2,4,98,0.25);
  addLayerShift(2,2,5,70,0.2);
  addLayerShift(2,3,0,73,-0.25);
  addLayerShift(2,3,2,64,0.23);
  addLayerShift(2,3,3,99,0.3);
  addLayerShift(2,3,4,84,-0.15);
  addLayerShift(2,3,5,78,-0.25);

  addLayerShift(3,2,1,97,0.5);
  addLayerShift(3,2,2,83,0.25);
  addLayerShift(3,2,4,59,0.2);
  addLayerShift(3,2,5,59,0.2);
//  addLayerShift(3,3,0,87,0.27);
  addLayerShift(3,3,1,99,0.42);
  addLayerShift(3,3,2,95,0.22);
  addLayerShift(3,3,3,101,0.53);
  addLayerShift(3,3,4,99,0.27);
  addLayerShift(3,3,5,84,0.27);

  addLayerShift(4,2,0,49,0.15);
  addLayerShift(4,2,1,95,0.1);
  addLayerShift(4,2,2,87,0.24);
  addLayerShift(4,2,3,109,-0.05);
  addLayerShift(4,2,4,58,0.15);
//  addLayerShift(4,2,5,22,-0.15);
  addLayerShift(4,3,0,91,0.67);
  addLayerShift(4,3,1,100,0.25);
  addLayerShift(4,3,2,99,0.4);
//  addLayerShift(4,3,3,87,0.1);
  addLayerShift(4,3,4,73,-0.1);
  addLayerShift(4,3,5,44,-0.1);

  addLayerShift(5,2,0,52,0.03);
  addLayerShift(5,2,1,92,-0.05);
  addLayerShift(5,2,2,78,0.05);
  addLayerShift(5,2,3,91,0.15);
  addLayerShift(5,2,4,110,0.2);
  addLayerShift(5,2,5,62,0.15);
  addLayerShift(5,3,0,90,-0.2);
  addLayerShift(5,3,1,83,-0.05);
  addLayerShift(5,3,2,86,-0.15);
  addLayerShift(5,3,3,85,-0.08);
  addLayerShift(5,3,4,95,0.15);
  addLayerShift(5,3,5,95,0.15);
}

Bool_t HMdcLayerCorrPar::getLayerCorrPar(Int_t s,Int_t m,Int_t l,
    Int_t& fstWr,Float_t& sh,Float_t& orCorr) const {
  if(isAddOk(s,m,l)) {
    Int_t ind = (s*(nMdc-2)+(m-2))*nLay+l;
    fstWr  = firstWire[ind];
    sh     = shift[ind];
    orCorr = wireOrientCorr[ind];
  } else {
    fstWr  = 777;
    sh     = 0.f;
    orCorr = 0.f;
    return kFALSE;
  }
  return kTRUE;
}

