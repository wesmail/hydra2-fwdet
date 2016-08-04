//*-- AUTHOR : Ilse Koenig
//*-- Modified : 03/02/2000 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////
//  HRun
//
//  class for the parameter versions of an event file
//
//  The name of each run is the run id converted to a string.
//  The run id number identifies the event file. Each event has
//  this number in the event header. It is also stored in Oracle
//  and can be used to find the name of the corresponding LMD-file
//  and the valid parameter versions.
//  In the parameter ROOT file the valid parameter versions are
//  accessible via the name. 
//  Associated with the run is a list of container
//  names with the versions of the containers in the two
//  possible inputs and the output (class HParVersions).
//  The input versions are used during the initialisation
//  used during the initialisation of the containers.
//
///////////////////////////////////////////////////////////////////

#include "hrun.h"
#include "hades.h"
#include "hmessagemgr.h"

ClassImp(HParVersion)
ClassImp(HRun)

HParVersion::HParVersion(const Text_t* name) : TNamed(name,"version info") {
  // constructor with the name of the container
  rootVersion=0;
  for(Int_t i=0;i<3;i++) {inputVersions[i]=-1;}
}


HRun::HRun(const Text_t* name,const Text_t* refName)
              : TNamed(name,"run parameters") {
  // constructor with the run id and reference run as strings
  parVersions=new TList();
  refRun=refName;
}

HRun::HRun(Int_t r,Int_t rr) {
  // constructor with the run id and reference run as numbers
  parVersions=new TList();
  Char_t name[255];
  sprintf(name,"%i",r);
  SetName(name);
  setRefRun(rr);
}

HRun::HRun(HRun &run):TNamed(run) {
  // copy constructor
  SetName(run.GetName());
  parVersions=new TList();
  TList* lv=run.getParVersions();
  TIter next(lv);
  HParVersion* pv;
  while ((pv=(HParVersion*)next())) {
    parVersions->Add(pv);
  }
  refRun=run.refRun;
}

HRun::~HRun() {
  // destructor
  if (parVersions) {
    parVersions->Delete();
    delete  parVersions;
    parVersions=0;
  }
}

void HRun::addParVersion(HParVersion *pv) {
  // adds a container version object to the list
  parVersions->Add(pv);
}

HParVersion* HRun::getParVersion(const Text_t* name) {
  // return a container version object called by the name of
  // the container
  return (HParVersion*)parVersions->FindObject(name);
}

void HRun::resetInputVersions() {
  TIter next(parVersions);
  HParVersion* v;
  while ((v=(HParVersion*)next())) {
    v->resetInputVersions();
  }
}

void HRun::resetOutputVersions() {
  TIter next(parVersions);
  HParVersion* v;
  while ((v=(HParVersion*)next())) {
    v->setRootVersion(0);
  }
}

void HRun::print() {
  // prints the list of container versions for this run

  //cout<<"run: "<<fName<<'\n';
  gHades->getMsg()->info(10,HMessageMgr::DET_RUNTIMEDB,this->GetName(),"run: %d",getRunId());
  TIter next(parVersions);
  HParVersion* v;
  while ((v=(HParVersion*)next())) {
    gHades->getMsg()->info(10,HMessageMgr::DET_RUNTIMEDB,this->GetName(),"%-22s%10d%11d%9d", v->GetName(),v->getInputVersion(1),v->getInputVersion(2), v->getRootVersion());
  }
}

void HRun::write(fstream& fout) {
  // writes the list of container versions for this run to fstream
  fout<<"run: "<<GetName()<<'\n';
  HParVersion* v;
  TIter next(parVersions);
  while ((v=(HParVersion*)next())) {
    fout.setf(ios::left,ios::adjustfield);
    fout<<"  "<<setw(45)<<v->GetName();
    fout.setf(ios::right,ios::adjustfield);
    fout<<setw(11)<<v->getInputVersion(1)
        <<setw(11)<<v->getInputVersion(2)
        <<setw(11)<<v->getRootVersion()<<'\n';
  }
}
