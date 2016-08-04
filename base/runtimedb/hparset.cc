//*-- AUTHOR : Ilse Koenig
//*-- Last modified : 13/07/2007 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////////////////
//  HParSet
//
//  Base class for all parameter containers
//
//  Parameter containers are typically created via the container factories (library
//  specific classes derived from HContFact) and stored in the runtime database HRuntimeDb
//  for initialization. Each parameter container is directly derived from HParSet or from
//  HParCond (derived itself from HParSet).
//
//  Data members:
//    name, title     name and title of the parameter container and set in the constructor
//    context         String to define a special flavor for the parameters
//                    For classes directly derived from HParSet, the context in not
//                    mandatory. The context defined in the constructor is the default
//                    context. Others may be set via the container factory.
//    detName         name of the corresponding detector
//                    It is mandatory for all parameter containers which might initialize
//                    only a subset of the modules or want to check if all modules in the
//                    setup are initialized from one or two inputs
//    versions[3]     version[0]   actually not used
//                    version[1] = version of the parameters taken from the first input
//                    version[2] = version of the parameters taken from the second input
//    status          By default this flag is kFALSE and the parameters are initialized
//                    for each run. Once set kTRUE, the containers is skipped in the
//                    automatic initialization in the runtime database.
//                    Set function:  void setStatic(Bool_t flag=kTRUE)
//                    Get function:  Bool_t isStatic()
//    changed         By default this flag is kFALSE and set kTRUE after each initialization,
//                    which signals, that the container must be written to the output before
//                    the data are overwritten by the next initialization or before the
//                    container is deleted. The write function then again resets the flag.
//                    Set function:  void setChanged(Bool_t flag=kTRUE)
//                    Get function:  Bool_t hasChanged()
//    author, description    comments for the parameters in Oracle
//
//  Virtual functions which must be implemented in the derived class:
//    Bool_t init(HParIo*,Int_t*)
//        This function initializes the container from an input using the detector specific
//        interface class of type HDetParIo.
//        Eventually the integer array is used to read only a subset of data.
//    Int_t write(HParIo*)
//        This function writes the container to an output using the detector specific
//        interface class of type HDetParIo.
//    void clear()      
//        This functions clears the container and resets the input/output versions
//
////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hparset.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include <iostream> 
#include <iomanip>

ClassImp(HParSet)

HParSet::HParSet(const Char_t* name,const Char_t* title,const Char_t* context)
        : TNamed(name,title) {
  // constructor sets the name, title, context and default values for the other data elements
  paramContext=context;
  for(Int_t i=0;i<3;i++) {versions[i]=-1;}
  status=kFALSE;
  changed=kFALSE;
  detName[0]='\0';
}

Bool_t HParSet::init(HParIo* io) {
  // Tries to initializes the container from the first input in the runtime database.
  // If this is not successful, it is initialized from the second input. 
  // If this fails too, it returns an error.
  // (calls internally the init(HParIo*,Int_t*) function in the derived class)
  // If the detector name is defined, a setup array is temporarily created to initialize
  // only the modules in the detector setup and to check at the end, if all modules in
  // the setup are initialized. 
  HRuntimeDb* rtdb=gHades->getRuntimeDb();
  Int_t* set=0;
  if (strlen(detName)>0) {
    HDetector* det=gHades->getSetup()->getDetector(detName);
    if (det) {
      Int_t len=0;
      Int_t nSec=det->getMaxSectors();
      if (nSec<0) len=det->getMaxModules();
      else len=det->getMaxModules()*nSec;
      Int_t* s=det->getModules();
      set=new Int_t[len];
      for(Int_t i=0;i<len;i++) set[i]=s[i];
    } else {
      cerr<<"********  "<<GetName()<<"  not initialized  ********"<<endl;
      cerr<<"********  Detector "<<detName<<" not in setup  ********"<<endl;
      return kFALSE;
    }
  }
  Bool_t allFound=kFALSE;
  if (versions[1]!=-1 && versions[2]!=-1) resetInputVersions();
  if (io) {
    allFound=init(io,set);
  } else {
    io=rtdb->getFirstInput();
    if (io) allFound=init(io,set);
      if (!allFound || versions[1]==-1) {
      io=rtdb->getSecondInput();
      if (io) allFound=init(io,set);
    } else setInputVersion(-1,2);
  }
  if (set) delete[] set;
  if (allFound) return kTRUE;
  cerr<<"********  "<<GetName()<<"  not initialized  ********"<<endl;
  return kFALSE;
}

Int_t HParSet::write() {
  // writes the container to the output defined in the runtime database
  // returns the output version in the ROOT file
  // returns -1 if an error occured
  // (calls internally the write function in the derived class)
  HParIo* output=gHades->getRuntimeDb()->getOutput();
  if (output) return write(output);
  cerr<<GetName()<<" could not be written to output"<<endl;
  return -1;
}

void HParSet::print() {
  // prints information about the container (context,author,description,versions,status,changed...)
  cout<<"-----  "<<GetName()<<"  -----"<<'\n';
  if (!paramContext.IsNull()) cout<<"Context/Purpose:       "<<paramContext<<'\n';
  if (!author.IsNull()) cout<<"Author:                "<<author<<'\n';
  if (!description.IsNull()) cout<<"Description:           "<<description<<'\n';
  cout<<"first input version:   "<<versions[1]<<'\n';
  cout<<"second input version:  "<<versions[2]<<'\n';
  if (changed) cout<<"has changed"<<'\n';
  else cout<<"has not changed"<<'\n';
  if (status) cout<<"is static"<<'\n';
  else cout<<"is not static"<<'\n';
}

void HParSet::resetInputVersions() {
  // resets the input versions if the container is not static
  if (!status) {
    for(Int_t i=0;i<3;i++) {versions[i]=-1;}
    changed=kFALSE;
  }
}

void HParSet::setParamContext(const Char_t* context) {
  // Sets the context (normally done via the constructor)
  paramContext=context;
}

void HParSet::Streamer(TBuffer &R__b) {
  // Streams an object of class HParSet.
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TNamed::Streamer(R__b);
      if (R__v > 1) {
        paramContext.Streamer(R__b);
        author.Streamer(R__b);
        description.Streamer(R__b);
      }
   } else {
      R__b.WriteVersion(HParSet::IsA());
      TNamed::Streamer(R__b);
      paramContext.Streamer(R__b);
      author.Streamer(R__b);
      description.Streamer(R__b);
   }
}
