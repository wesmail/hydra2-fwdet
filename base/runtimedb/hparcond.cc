//*-- AUTHOR : Ilse Koenig
//*-- Created : 28/01/2009

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////
//
//  HParCond
//
//  Base class for all condition-stype parameter containers
//
//  The following functions must be implemented by the derived class:
//
//    void putParams(HParamList*)    Fills all persistent data members into
//                                   the list for write.
//
//    Bool_t getParams(HParamList*)  Fills all persistent data members from
//                                   the list after reading. The function
//                                   returns false, when a data member is
//                                   not in the list.
//
///////////////////////////////////////////////////////////////////////////

#include "hparcond.h"
#include "hparamlist.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HParCond)

Bool_t HParCond::init(HParIo* inp,Int_t* set) {
  // intitializes the parameter container from an input
  HDetParIo* input=inp->getDetParIo("HCondParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HParCond::write(HParIo* output) {
  // writes the parameter container to the output
  HDetParIo* out=output->getDetParIo("HCondParIo");
  if (out) return out->write(this);
  return -1;
}

void HParCond::printParams() {
  // prints information about the container
  std::cout<<"\n---------------------------------------------\n";
  std::cout<<"-----  "<<GetName()<<"  -----\n";
  if (!paramContext.IsNull()) std::cout<<"--  Context/Purpose:  "<<paramContext<<'\n';
  if (!author.IsNull())       std::cout<<"--  Author:           "<<author<<'\n';
  if (!description.IsNull())  std::cout<<"--  Description:      "<<description<<'\n';
  std::cout<<"---------------------------------------------\n";
  HParamList* condList = new HParamList;
  putParams(condList);
  condList->print();
  std::cout<<"---------------------------------------------\n\n";
  delete condList;
} 
 
