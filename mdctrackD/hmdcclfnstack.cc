#include "hmdcclfnstack.h"

//*-- AUTHOR : Pechenov Vladimir
//*-- Modified : 06/11/2002 by V.Pechenov

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
// HMdcClFnStack
//
// Stack for claster finder
//
////////////////////////////////////////////////////////////////

ClassImp(HMdcClFnStack)
ClassImp(HMdcClFnStacksArr)

HMdcClFnStack::HMdcClFnStack(Int_t size) {
 stack     = NULL;  
 stackEnd  = NULL;
 stackPos  = NULL;
 stackSize = size>20 ? size : 20;
}

void HMdcClFnStack::setSize(Int_t size) {
  if(size > stackSize) stackSize = size;
}

HMdcClFnStack::~HMdcClFnStack() {
  if(stack != NULL) delete [] stack;
  stack = NULL;
}

void HMdcClFnStack::increaseStack(void) {
  if(stack == NULL) create();
  else {
    Int_t *oldStack    = stack;
    Int_t *oldStackPos = stackPos;
    stackSize += stackSize/2;
    stack      = NULL;
    create();
    for(Int_t *n=oldStack; n<oldStackPos; n++) push(*n);
    delete [] oldStack;
    Warning("increaseStack","Stack No.%i New size = %i",indexInArr+1,stackSize);
  }
}

void HMdcClFnStack::create(void) {
  stack    = new Int_t [stackSize+2];
  stackPos = stack;
  stackEnd = stack+stackSize;
}

//===================================================================

HMdcClFnStacksArr::HMdcClFnStacksArr(Int_t size) {
  // Stack sizes:    [0] - for fixed level of cluster finder
  Int_t sizes[24] = {100000,    20,    20,   250, 32000, 74000,  // For one module projection
                       3000,  3000,  3000,  1000,   500,   500,  // For segment projection
                        100,   100,   100,   100,   100,   100,  // For sector projection
                        100,   300,   600,  1500,   900,   800}; //   (4 modules)

  for(Int_t i=0; i<24; i++) {
    stacksArr[i].setIndex(i);
    stacksArr[i].setSize(sizes[i]);
  }
}

void HMdcClFnStacksArr::init(UChar_t maxAmp) {
  amplitude = 0;
  for(Int_t am=0;am<maxAmp;am++) stacksArr[am].init();
}

UChar_t HMdcClFnStacksArr::pop(Int_t& binNum) {
  // return amplitude !!!
  binNum = stacksArr[amplitude].pop();
  if(binNum>=0) return amplitude+1;
  if(amplitude == 0) return 0;
  amplitude--;
  return pop(binNum);
}
