//*-- AUTHOR Ilse Koenig
//*-- created : 25/06/2009 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
//
// HTrbnetAddressMapping
//
///////////////////////////////////////////////////////////////////////

using namespace std;
#include "htrbnetaddressmapping.h"
#include "htrb2correction.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <stdlib.h>

ClassImp(HTrbnetAddressMapping)

HTrbnetAddressMapping::HTrbnetAddressMapping(const Char_t* name,
                               const Char_t* title,
                               const Char_t* context,
                               Int_t minTrbnetAddress,
                               Int_t maxTrbnetAddress)
  : HParSet(name,title,context) {
  // constructor creates an empty array of size nBoards
  arrayOffset=minTrbnetAddress;
  array = new TObjArray(maxTrbnetAddress-minTrbnetAddress+1);
}

HTrbnetAddressMapping::~HTrbnetAddressMapping() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HTrbnetAddressMapping::init(HParIo* inp,Int_t* set) {
  // initializes the container from an input
  Bool_t rc=kFALSE;
  HDetParIo* input=inp->getDetParIo("HSpecParIo");
  if (input) rc=(input->init(this,set));
  if (rc) {
    Int_t n=0;
    for(Int_t i=0;i<=array->GetLast();i++) {
      HTrb2Correction* b=(*this)[i];
      if (b&&strcmp(b->getBoardType(),"TRB")==0&&b->getNChannels()==0) {
        if (n==0) cout<<"  WARNING:\n";
        cout<<"    No TDC corrections for trbnet-address 0x"<<setw(4)<<left
 	    <<hex<<(arrayOffset+i)<<dec<<endl;
        n++;
      }
    }
  }
  return rc;
}

Int_t HTrbnetAddressMapping::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HSpecParIo");
  if (out) return out->write(this);
  return -1;
}

void HTrbnetAddressMapping::clear() {
  // deletes all HTrb2Correction objects from the array and resets the input versions
  array->Delete();
  status=kFALSE;
  resetInputVersions();
}

void HTrbnetAddressMapping::printParam() {
  // prints the mapping table
  printf("Mapping of trbnet-addresses to readout boards\n");
  printf("Format: trbnet-address temperatureSensor  boardType  nChannelsWithCorrections\n");
  for(Int_t i=0;i<=array->GetLast();i++) {
    HTrb2Correction* b=(*this)[i];
    if (b) {
      cout<<"  0x"<<setw(4)<<left<<hex<<(arrayOffset+i)<<dec<<"  "<<b->GetName()
          <<"  "<<b->getBoardType()<<setw(5)<<right<<b->getNChannels()<<left<<endl;
    }
  }
}

HTrb2Correction* HTrbnetAddressMapping::addBoard(Int_t address,
                                                 const Char_t* temperatureSensor,
                                                 const Char_t* boardtype,
                                                 Int_t subEventId,
                                                 Int_t tdcResolMode) {
  HTrb2Correction* p=0;
  p=getBoard(address);
  if (!p) {
    p=new HTrb2Correction(temperatureSensor);
    p->setBoardType(boardtype);
    p->setSubeventId(subEventId);
    p->setHighResolutionFlag(tdcResolMode);
    array->AddAt(p,address-arrayOffset);
  } else if (strcmp(p->GetName(),temperatureSensor)!=0) {
    Error("addBoard","Board exists already for trbnet-address 0x%x:\n"
	  "  old temperature sensor: %s   new: %s\n",
	  address,p->GetName(),temperatureSensor);    
  }
  return p;
}  

Bool_t HTrbnetAddressMapping::readline(const Char_t *buf) {
  // decodes one line read from ASCII file I/O and adds a trb board
  Bool_t rc=kFALSE;
  Int_t address=-1, subevtId=-1, tdcResolMode=-1;
  Char_t tempSensor[30], boardType[30];
  boardType[0]=tempSensor[0]='\0';
  Int_t n=sscanf(buf," 0x%x %s %s %i %i",
                 &address,tempSensor,boardType,&subevtId,&tdcResolMode);
  if (n==5) {
    if (addBoard(address,tempSensor,boardType,subevtId,tdcResolMode)) rc=kTRUE;
  } else {
    Error("readline","Not enough or too many values in %s\n",buf);
  }
  return rc;
}

void HTrbnetAddressMapping::putAsciiHeader(TString& header) {
  // puts the ASCII header to the string used in HSpecParAsciiFileIo
  header=
    "# Mapping of trbnet-address to TRB boards\n"
    "# Format: trbnetAddress  temperatureSensor  boardType  subeventId  tdc resolution mode \n";
}

void HTrbnetAddressMapping::write(fstream& fout) {
  // writes the information of all non-zero HTrb2Correction objects to the ASCII file
  for(Int_t i=0;i<=array->GetLast();i++) {
    HTrb2Correction* b=(*this)[i];
    if (b) {
      fout<<"  0x"<<setw(4)<<left<<hex<<(arrayOffset+i)<<"  "<<b->GetName()
          <<"  "<<b->getBoardType();
      if (b->getSubeventId()>=0) {
        fout<<"  0x"<<setw(4)<<left<<hex<<b->getSubeventId();
      } else {
        fout<<setw(8)<<right<<dec<<b->getSubeventId();
      }
      fout<<"  "<<dec<<b->getHighResolutionFlag()<<endl;
    }
  }
  fout<<"# ----------------------------------------------------------------------------"
      <<endl;
  fout<<"[Trb2Corrections]\n";
  for(Int_t i=0;i<=array->GetLast();i++) {
    HTrb2Correction* b=(*this)[i];
    if (b) {
      b->write(fout);
    }
  }
}
