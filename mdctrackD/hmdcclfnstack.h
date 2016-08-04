#ifndef HMDCCLFNSTACK_H
#define HMDCCLFNSTACK_H

#include "TObject.h"

class HMdcClFnStack : public TObject {
  protected:
    Int_t *stack;         // stack array
    Int_t *stackEnd;      // address of end stack
    Int_t *stackPos;      // address of stack position
    Int_t  stackSize;     // stackSize;
    Int_t  indexInArr;    // index in HMdcClFnStacksArr == clusterAmplitude - 1
  public:
    HMdcClFnStack(Int_t size = 20);
    ~HMdcClFnStack();
    void  setSize(Int_t size);
    void  init(void)          {stackPos = stack;}
    void  push(Int_t n)       {checkSize(); *stackPos = n; stackPos++;}
    Int_t pop(void)           {return stackPos>stack ? *(--stackPos) : -1;}
    void  checkSize(void)     {if(stackPos>=stackEnd) increaseStack();}
    void  setIndex(Int_t ind) {indexInArr = ind;}
  protected:
    void  create(void);
    void  increaseStack(void);
    
  ClassDef(HMdcClFnStack,0)
};

class HMdcClFnStacksArr : public TObject {
  protected:
    HMdcClFnStack stacksArr[24]; //
    UChar_t       amplitude;
  public:
    HMdcClFnStacksArr(Int_t size = 1000);
    ~HMdcClFnStacksArr() {}
    void  init(UChar_t maxAmp=24);
    void  push(Int_t n,UChar_t amp)   {amp--; stacksArr[amp].push(n);
                                       if(amp>amplitude) amplitude=amp;}
    UChar_t pop(Int_t& binNum);
    HMdcClFnStack* getOneStack(void) {return stacksArr;}
  protected:
    
  ClassDef(HMdcClFnStacksArr,0)
};  

#endif  /*!HMDCCLFNSTACK_H*/
