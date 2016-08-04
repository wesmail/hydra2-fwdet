//*-- AUTHOR Ilse Koenig
//*-- created : 12/03/2014 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
//
// HStart2Trb3Calpar
//
///////////////////////////////////////////////////////////////////////

#include "hstart2trb3calpar.h"
#include "hdetpario.h"
#include "hpario.h"

using namespace std;
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <stdlib.h>

ClassImp(HStart2Trb3Calpar)

Bool_t HStart2Trb3Calpar::init(HParIo* inp,Int_t* set) {
  // initializes the container from an input
  Bool_t rc=kFALSE;
  HDetParIo* input=inp->getDetParIo("HStartParIo");
  if (input) rc=(input->init(this,set));
  if (rc) {
    Int_t n=0;
    for(Int_t i=0;i<=array->GetLast();i++) {
      HTrb3CalparTdc* tdc=(*this)[i];
      if (tdc&&tdc->getNChannels()==0) {
        if (n==0) cout<<"  WARNING:\n";
        cout<<"    No Trb3 TDC calibration parameters for trbnet-address 0x"<<setw(4)<<left
            <<hex<<(arrayOffset+i)<<dec<<endl;
        n++;
      }
    }
  }
  return rc;
}

Int_t HStart2Trb3Calpar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HStartParIo");
  if (out) return out->write(this);
  return -1;
}

 
