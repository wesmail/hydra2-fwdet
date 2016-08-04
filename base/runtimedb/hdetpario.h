#ifndef HDETPARIO_H
#define HDETPARIO_H
using namespace std;
#include <iostream> 
#include <iomanip>

#include "Rtypes.h"
#include "TNamed.h"

class HParSet;
class HDetector;

class HDetParIo : public TNamed {
protected:
  Int_t inputNumber; // input number (first or second input in runtime database)
public:
  HDetParIo() {}
  virtual ~HDetParIo() {}

  // sets the input number
  void setInputNumber(Int_t n) {inputNumber=n;}

  // returns the input number
  Int_t getInputNumber() {return inputNumber;}

  // initializes parameter container
  virtual Bool_t init(HParSet*,Int_t*) {return kFALSE;}

  // writes parameter container to output
  virtual Int_t write(HParSet*) {return kFALSE;}

  // writes detector setup to output
  virtual Bool_t write(HDetector*) {return kFALSE;}

  ClassDef(HDetParIo,0)  // Base class for detector parameter IO
};

#endif  /* !HDETPARIO_H */







