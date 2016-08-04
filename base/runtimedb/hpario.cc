//*-- AUTHOR : Ilse Koenig
//*-- Modified : 21/02/2000 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//  HParIo
//
//  Base class for different parameter input/output sources:
//  Oracle, Rootfiles, Ascifiles
//  It contains a list of detector I/Os.
//
/////////////////////////////////////////////////////////////

#include "hpario.h"
#include "hdetpario.h"

ClassImp(HParIo)

HParIo::HParIo() {
  //default constructor creates an empty list of detector I/Os
  detParIoList= new TList();
  autoWritable=kTRUE;
}

HParIo::~HParIo() {
  // default destructor
 if(detParIoList){
  detParIoList->Delete();
  delete detParIoList;
  detParIoList=0;
  }
}

void HParIo::setDetParIo(HDetParIo* detParIo) {
  // stores pointer of the input/output class for a detector
  // used for I/O from ROOT file or Asci file
  Text_t* detName=(Char_t*)detParIo->GetName();
  if (!detParIoList->FindObject(detName)) detParIoList->Add(detParIo);
}

void HParIo::setInputNumber(Int_t num) {
  // sets in all detector I/Os the number of the input
  TIter next(detParIoList);
  HDetParIo* io;
  while ((io=(HDetParIo*)next())) {
    io->setInputNumber(num);
  }
} 

HDetParIo* HParIo::getDetParIo(const Text_t* detName) {
  // returns pointer to input/output class for a detector
  return ((HDetParIo*)detParIoList->FindObject(detName));
}

void HParIo::removeDetParIo(const Text_t* detName) {
  // removes input/output class for a detector
  TObject* p=detParIoList->FindObject(detName);
  if (p) {
    delete p;
    p=0;
  }
}
