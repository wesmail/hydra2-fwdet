//*-- AUTHOR : Ilse Koenig
//*-- Modified last : 22/10/2002 by Denis Bertini

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//  HRuntimeDb
//
//  Administration class for parameter input/output
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hades.h"
#include "hruntimedb.h"
#include "hcontfact.h"
#include "hparset.h"
#include "hpario.h"
#include "hparrootfileio.h"
#include "hspectrometer.h"
#include "hmessagemgr.h"
#include "hrun.h"
#include "hparamfilegenerator.h"
#include "hruninfo.h"
#include "TKey.h"
#include "TClass.h"
#include <iostream> 
#include <iomanip> 

ClassImp(HRuntimeDb)

HRuntimeDb* HRuntimeDb::gRtdb=0;

HRuntimeDb* HRuntimeDb::instance(void) {
  if (gRtdb==0) gRtdb=new HRuntimeDb;
  return gRtdb;
}

HRuntimeDb::HRuntimeDb(void) {
  // constructor creates an empty list for parameter containers
  // and an empty list of runs for the version management
  gRtdb=this;
  containerList=new TList();
  runs=new TList();
  firstInput=0;
  secondInput=0;
  output=0;
  versionsChanged=kFALSE;
  currentRun=0;
  isRootFileOutput=kFALSE;
  pParamFileGenerator=0;
}

HRuntimeDb::~HRuntimeDb() {
  // destructor
  // deletes the list of runs and all containers
  if (pParamFileGenerator) {
    delete pParamFileGenerator;
    pParamFileGenerator=0;
  } 
  closeFirstInput();
  closeSecondInput();
  closeOutput();
  if (containerList) {
    containerList->Delete();
    delete containerList;
  }
  if (runs) {
    runs->Delete();
    delete runs;
  }
  gRtdb=0;
}

void HRuntimeDb::addContFactory(HContFact* fact) {
  // Adds a container factory to the list of factories
  if (!(contFactories.FindObject(fact->GetName()))) {
    printf("- RTDB container factory %s \n",fact->GetName() );
    contFactories.Add(fact);
  }
}

Bool_t HRuntimeDb::addParamContext(const Char_t* context) {
  // Sets via the container factories the context of all parameter containers,
  // which accept this context 
  Bool_t found=kFALSE;
  TIter next(&contFactories);
  HContFact* fact;
  while((fact=(HContFact*)next())) {
    if (fact->addContext(context)) found=kTRUE;
  }
  if (!found) { ERROR_msg(HMessageMgr::DET_ALL,"Unknown context"); }
      //Error("addParamContext","Unknown context");
  return found;
}

void HRuntimeDb::printParamContexts() {
  // Prints the context of all parameter containers, which can be created by
  // the container factories
  TIter next(&contFactories);
  HContFact* fact;
  while((fact=(HContFact*)next())) fact->print();
}

Bool_t HRuntimeDb::addContainer(HParSet* container) {
  // adds a container to the list of containers
  Text_t* name=(Char_t*)container->GetName();
  if (!containerList->FindObject(name)) {
    containerList->Add(container);
    TIter next(runs);
    HRun* run;
    HParVersion* vers;
    while ((run=(HRun*)next())) {
      if (!run->getParVersion(name)) {
        vers=new HParVersion(name);
        run->addParVersion(vers);
      }
    }      
    return kTRUE;
  }
  gHades->getMsg()->warning(10,HMessageMgr::DET_ALL,this->GetName(),"Container with name %s exists already!",name);
  //Warning("addContainer","Container with name %s exists already!",name);
  return kFALSE;
}

HParSet* HRuntimeDb::getContainer(const Text_t* name) {
  // The function loops over the container factories to find the corresponding container
  // with the give name and its context.
  // The name is the original name of the parameter container without the concatination
  // with the context.
  // The factory checks, if the container exists already in the runtime database. Otherwise
  // it will be created and added by the factory.    
  // The function returns a pointer to the container or NULL, if not created.
  TIter next(&contFactories);
  HContFact* fact;
  HParSet* c=0;
  while(!c && (fact=(HContFact*)next())) {
    c=fact->getContainer(name);
  }
  if (!c) gHades->getMsg()->error(1,HMessageMgr::DET_ALL,this->GetName(),"Container %s not created!",name);
      //Error("getContainer","Container %s not created!",name); 
  return c;
}

HParSet* HRuntimeDb::findContainer(const Char_t* name) {
  // returns a pointer to the container called by name
  // The name is the original name of the parameter container eventually concatinated with
  // a non-default context.
  return (HParSet*)(containerList->FindObject(name));
}

void HRuntimeDb::removeContainer(Text_t* name) {
  // removes the container from the list and deletes it
  TObject* c=containerList->FindObject(name);
  if (c) {
    containerList->Remove(c);
    delete c;
  }
}

void HRuntimeDb::removeAllContainers(void) {
  // removes all containers from the list and deletes them
  containerList->Delete();
}

HRun* HRuntimeDb::addRun(Int_t runId,Int_t refId) {
  // adds a run at the end of the list of runs
  // returns a pointer to the run newly added
  HRun* run;
  if (refId!=-1) {
    run=getRun(refId);
    if (!run) addRun(refId);
  }
  run=getRun(runId);
  if (!run) {
    run=new HRun(runId,refId);
    TIter next(containerList);
    HParSet* cont;
    HParVersion* vers;
    while ((cont=(HParSet*)next())) {
      vers=new HParVersion(((Char_t*)cont->GetName()));
      run->addParVersion(vers);
    }    
    runs->Add(run);
    currentRun=run;
  } else WARNING_msg(10,HMessageMgr::DET_ALL,"run exists already!");
      //cout<<"WARNING: run exists already!"<<endl;
  return run;
}

HRun* HRuntimeDb::getRun(Int_t id) {
  // returns a pointer to the run called by the run id
  Char_t name[255];
  sprintf(name,"%i",id);
  return (HRun*)(runs->FindObject(name));
}

HRun* HRuntimeDb::getRun(Text_t* name) {
  // returns a pointer to the run called by name
  return (HRun*)(runs->FindObject(name));
}

void HRuntimeDb::removeRun(Text_t* name) {
  // removes the run from the list and deletes it
  TObject* c=runs->FindObject(name);
  if (c) {
    runs->Remove(c);
    delete c;
    if(c==currentRun) currentRun=0;
  }
}

void HRuntimeDb::clearRunList() {
  runs->Delete();
}

void HRuntimeDb::writeSetup() {
  // writes the setup to the output if the setup has changed
  HSpectrometer* spec=gHades->getSetup();
  if (spec->hasChanged() && getOutput() && output->check()&& output->isAutoWritable()
                         && firstInput!=output) 
      spec->write(output);
}

void HRuntimeDb::writeVersions() {
  // writes the parameter versions for all runs to the output
  if (getOutput() && output->check()) {
    if (versionsChanged && isRootFileOutput) {
      output->cd();
      if (gFile->IsWritable()) runs->Write();
      versionsChanged=kFALSE;
    }
  }
}


Bool_t HRuntimeDb::writeContainers() {
  // writes all containers to the output
  // loops over the list of containers and calls for each the
  // function writeContainer(...)
  TIter next(containerList);
  HParSet* cont;
  Bool_t rc=kTRUE;
  HRun* refRun=0;
  if (currentRun) {
    const Char_t* refRunName=currentRun->getRefRun();
    if (strlen(refRunName)>0) {
      refRun=(HRun*)(runs->FindObject(refRunName));
    }
    while ((cont=(HParSet*)next()) && rc) {
      rc=writeContainer(cont,currentRun,refRun);
    }
  }
  if (!rc) { ERROR_msg(HMessageMgr::DET_ALL,"error while writing container to output"); }
      //cerr<<"error while writing container to output"<<endl;
  return rc;
}


Int_t HRuntimeDb::findOutputVersion(HParSet* cont) {
  Int_t in1=cont->getInputVersion(1);
  Int_t in2=cont->getInputVersion(2);
  HRun* run;
  HParVersion* vers;
  Text_t* name=(Char_t*)cont->GetName();
  Int_t v=0;
  if (in1==-1 && in2==-1) {
    if (cont->hasChanged()) return 0;
    else {
      Int_t i=runs->IndexOf(currentRun); //FIXME: This can be optimized with a backwards iter.
      while (i>=0) {
        run=(HRun*)runs->At(i);
        vers=run->getParVersion(name);
        if (vers->getInputVersion(1)==in1 && vers->getInputVersion(2)==in2) {
          if ((v=vers->getRootVersion())!=0) return v;
        }
        --i;
      }
      return 0;
    }
  }
  if ((firstInput==output) && (in1>0 && in2==-1)) return in1;
  TIter next(runs);
  v=0;
  while ((run=(HRun*)next())) {
    vers=run->getParVersion(name);
    if (vers->getInputVersion(1)==in1 && vers->getInputVersion(2)==in2) {
      if ((v=vers->getRootVersion())!=0) return v;
    }
  }
  return 0;
}


Bool_t HRuntimeDb::writeContainer(HParSet* cont, HRun* run, HRun* refRun) {
  // writes a container to the output if the containers has changed
  // The output might be suppressed if the changes is due an
  // initialisation from a ROOT file which serves also as output
  // or if it was already written
  Text_t* c=(Char_t*)cont->GetName();
  HParVersion* vers=run->getParVersion(c);
  Bool_t rc=kTRUE;
  Int_t cv=0;
  if (getOutput() && output->check() && output->isAutoWritable()) {
    if (isRootFileOutput) {
      if (cont->hasChanged()) {
        cv=findOutputVersion(cont);
        if (cv==0){
          cv=cont->write(output);
          if (cv>0) {
	      gHades->getMsg()->info(10,HMessageMgr::DET_ALL,this->GetName(),"%s written to ROOT file, version: %d",c,cv);
	      //cout<<"***  "<<c<<" written to ROOT file   version: "<<cv<<'\n';
          }
          else if (cv==-1) return kFALSE;
             // -1 indicates and error during write
	     // 0 is allowed for all containers which have no write function
        }
        vers->setRootVersion(cv);
      } else {
        if (vers->getRootVersion()==0) {
          cv=findOutputVersion(cont);
          vers->setRootVersion(cv);
        }
      }
    } else {   // might be Ascii I/O
      if (cont->hasChanged()) {
        cv=cont->write(output);
        if (cv<0) return kFALSE;
        if (cv>0) gHades->getMsg()->info(10,HMessageMgr::DET_ALL,this->GetName(),"%s written to output",c);
	    //cout<<"***  "<<c<<" written to output"<<'\n';
        vers->setRootVersion(cv);
      }
    }
  }
  vers->setInputVersion(cont->getInputVersion(1),1);
  vers->setInputVersion(cont->getInputVersion(2),2);
  cont->setChanged(kFALSE);
  if (refRun) {
    HParVersion* refVers=refRun->getParVersion(c);
    if (refVers) {
      refVers->setInputVersion(cont->getInputVersion(1),1);
      refVers->setInputVersion(cont->getInputVersion(2),2);
      refVers->setRootVersion(cv);
    }
  }
  return rc;
}

Bool_t HRuntimeDb::initContainers(Int_t runId,Int_t refId,
                                  const Text_t* fileName) {
  // loops over the list of containers and calls the init() function of each
  // container if it is not static
  // (typically called by Hades::eventLoop(...))
  if (currentRun && currentRun->getRunId()!=runId) writeContainers();
  HRun* run=getRun(runId);
  if (!run) run=addRun(runId,refId);
  else {
    run->setRefRun(refId);
    if (refId!=-1 && !getRun(refId)) addRun(refId);
  }
  currentRun=run;
  currentFileName=fileName;
  return initContainers();
}

Bool_t HRuntimeDb::readAll() {
  // reads all containers with all versions for all runs and writes the
  // containers, the setup information and the version table to the output
  if (!(getOutput() && output->check())) {
      SEPERATOR_msg("*",80);
      WARNING_msg(10,HMessageMgr::DET_ALL,"no output !");
      SEPERATOR_msg("*",80);
      //cout<<"***********************************************************"<<endl;
      //cout<<"*********************  W A R N I N G  *********************"<<endl;
      //cout<<"*********************   no output !   *********************"<<endl;
      //cout<<"***********************************************************"<<endl;
  }
  currentRun=0;
  Bool_t rc=kTRUE;
  TIter next(runs);
  while ((currentRun=(HRun*)next())!=0 &&rc) {
    rc=initContainers();
    writeContainers();
  }
  saveOutput();
  currentRun=0;
  return kTRUE;
}

Bool_t HRuntimeDb::initContainers(void) {
  // private function 
  Text_t* refRunName=(Char_t*)currentRun->getRefRun();
  Int_t len=strlen(refRunName);
  if (len<1) {
    if (firstInput) firstInput->readVersions(currentRun);
    if (secondInput) secondInput->readVersions(currentRun);
  } else {
    HRun* refRun=getRun(refRunName);
    if (firstInput) firstInput->readVersions(refRun);    
    if (secondInput) secondInput->readVersions(refRun);
  }
  TIter next(containerList);
  HParSet* cont;
  Bool_t rc=kTRUE;
  //cout<<'\n'<<"*************************************************************"<<'\n';
  SEPERATOR_msg("*",60);
  if (currentFileName.IsNull())
    {
      gHades->getMsg()->info(10,HMessageMgr::DET_ALL,this->GetName()," initialisation for run id %s",currentRun->GetName());
    }
    //cout<<"     initialisation for run id "<<currentRun->GetName();
  else {
    gHades->getMsg()->info(10,HMessageMgr::DET_ALL,this->GetName()," initialisation for event file %s",currentFileName.Data());
    gHades->getMsg()->info(10,HMessageMgr::DET_ALL,this->GetName(),"  run id %s",currentRun->GetName());
    //cout<<"     initialisation for event file "<<currentFileName.Data()<<'\n';
    //cout<<"     run id "<<currentRun->GetName();
  }
  if (len>0) 
    {
      gHades->getMsg()->info(10,HMessageMgr::DET_ALL,this->GetName(),"%10s %s","-->",refRunName);
      //cout << " --> " << refRunName;
    }
  SEPERATOR_msg("*",60);
  //cout << '\n';
  //cout<<"*************************************************************"<<'\n';
  while ((cont=(HParSet*)next()) && rc) {
    if (!cont->isStatic()) rc=cont->init();
    //cont->print();
  }
  if (!rc){ ERROR_msg(HMessageMgr::DET_ALL,"error occured during initialization"); }
    //cerr<<"error occured during initialization"<<endl;
  return rc;
}


void HRuntimeDb::printContainers(void) {
  // calls the print() function of all containers
  TIter next(containerList);
  HParSet* cont;
  SEPERATOR_msg("*",60);
  while ((cont=(HParSet*)next())) {
    cont->print();
    SEPERATOR_msg("*",60);
  }
}

void HRuntimeDb::setContainersStatic(Bool_t flag) {
  // sets the status flag in all containers
  // flag kTRUE sets all 'static'
  // flag kFALSE sets all 'not static'
  TIter next(containerList);
  HParSet* cont;
  while ((cont=(HParSet*)next())) {
    cont->setStatic(flag);
  }
}


Bool_t HRuntimeDb::setInputVersion(Int_t run,Text_t* container,
                               Int_t version,Int_t inp) {
  // sets the input version of a container defined by its name and a
  // run defined by its id taken from input with inputNumber inp
  // (1 for first input and 2 for second input)
  HRun* r=getRun(run);
  if (r) {
    HParVersion* v=r->getParVersion(container);
    if (v) {
      v->setInputVersion(version,inp);
      return kTRUE;
    }
    else ERROR_msg(HMessageMgr::DET_ALL,"Container not found");
  }
  else ERROR_msg(HMessageMgr::DET_ALL,"run not found");
  return kFALSE;
}


Bool_t HRuntimeDb::setRootOutputVersion(Int_t run,Text_t* container,
                               Int_t version) {
  // sets the Root file output version of a container defined by its name
  // and a run defined by its id
  // should only be used after initialization 'by hand' on the interpreter level
  HRun* r=getRun(run);
  if (r) {
    HParVersion* v=r->getParVersion(container);
    if (v) {
      v->setRootVersion(version);
      return kTRUE;
    }
    else ERROR_msg(HMessageMgr::DET_ALL,"Container not found");
  }
  else ERROR_msg(HMessageMgr::DET_ALL,"run not found");
  return kFALSE;
}

void HRuntimeDb::print() {
  // prints the list of the actual containers, the list of the
  // runs/versions and information about input/output
  //cout<<"-----------------------------------------------------------"<<'\n';
  //cout<<"---------  actual containers in runtime database  ---------"<<'\n';
  SEPERATOR_msg("-",60);
  INFO_msg(10,HMessageMgr::DET_ALL,"actual containers in runtime database");
  TIter nextCont(containerList);
  HParSet* cont;
  while((cont=(HParSet*)nextCont())) {
    //printf("%s\t\t%s\n",cont->GetName(),cont->GetTitle());
    gHades->getMsg()->info(10,HMessageMgr::DET_ALL,this->GetName(),"%s\t\t%s",cont->GetName(),cont->GetTitle());
  }
  TIter next(runs);
  HRun* run;
  //cout<<"-----------------  runs, versions  -----------------"<<'\n';
  INFO_msg(10,HMessageMgr::DET_RUNTIMEDB,"runs, versions");
  //cout<<"run id"<<'\n';
  SEPERATOR_msg("-",80);
  INFO_msg(10,HMessageMgr::DET_RUNTIMEDB,"run id");
  SEPERATOR_msg("-",40);
  gHades->getMsg()->info(10,HMessageMgr::DET_RUNTIMEDB,this->GetName(),"%-22s%10s%10s%10s","container","1st-input","2nd-input","output");
  //cout<<"  "<<setiosflags(2)<<setw(45)<<"container"<<" "
  //            <<setiosflags(4)<<setw(10)<<"1st-inp"<<" "
  //                            <<setw(10)<<"2nd-inp"<<" "
  //	                      <<setw(10)<<" output"<<'\n';
  while ((run=(HRun*)next())) {
    run->print();
  }
  //cout<<"---------------------  input/output  ----------------------"<<'\n';
  SEPERATOR_msg("-",80);
  INFO_msg(10,HMessageMgr::DET_RUNTIMEDB,"input/output");
  SEPERATOR_msg("-",40);
  //cout<<"first Input:"<<'\n';
  if (firstInput) 
      {
	  INFO_msg(10,HMessageMgr::DET_RUNTIMEDB,"first Input:");
	  firstInput->print();
      }
  else 
      INFO_msg(10,HMessageMgr::DET_RUNTIMEDB,"first Input: none");
      //cout<<" none"<<'\n';
  //cout<<'\n'<<"second Input:"<<'\n';
  SEPERATOR_msg("-",80);
  if (secondInput) 
      {
	  INFO_msg(10,HMessageMgr::DET_RUNTIMEDB,"second Input:");
	  secondInput->print();
      }
  else INFO_msg(10,HMessageMgr::DET_RUNTIMEDB,"second Input: none");
      //cout<<" none"<<'\n';
  //cout<<'\n'<<"Output:"<<'\n';
  SEPERATOR_msg("-",80);
  if (output)
      {
	  INFO_msg(10,HMessageMgr::DET_RUNTIMEDB,"Output:");
	  output->print();
      }
  else INFO_msg(10,HMessageMgr::DET_RUNTIMEDB,"Output: none");
  //cout<<" none"<<'\n';
  //cout<<"-----------------------------------------------------------"<<'\n';
  SEPERATOR_msg("-",80);
}

void HRuntimeDb::resetInputVersions() {
  // resets all input versions in the list of runs and in all containers
  //   which are not static
  // is called each time a new input is set
  TIter nextRun(runs);
  HRun* run;
  while ((run=(HRun*)nextRun())) {
    run->resetInputVersions();
  }
  TIter nextCont(containerList);
  HParSet* cont;
  while ((cont=(HParSet*)nextCont())) {
    if (!cont->isStatic()) cont->resetInputVersions();
  }
}

void HRuntimeDb::resetOutputVersions() {
  // resets all output versions in the list of runs
  // is called each time a new output is set
  // is called also each time a new input is set which is not
  // identical with the output
  TIter next(runs);
  HRun* run;
  while ((run=(HRun*)next())) {
    run->resetOutputVersions();
  }
}

void HRuntimeDb::resetAllVersions() {
  // resets all input and output versions in the list of runs
  // and in all containers which are not static
  resetInputVersions();
  resetOutputVersions();
}

Bool_t HRuntimeDb::setFirstInput(HParIo* inp1) {
  // sets the first input pointer
  firstInput=inp1;
  if (inp1->check()==kTRUE) {
    inp1->setInputNumber(1);
    resetInputVersions();
    if (output && firstInput!=output) resetOutputVersions();
    return kTRUE;
  }
  else ERROR_msg(HMessageMgr::DET_RUNTIMEDB, "no connection to input");
  //cerr<<"no connection to input"<<endl;
  return kFALSE;
}

Bool_t HRuntimeDb::setSecondInput(HParIo* inp2) {
  // sets the second input pointer
  secondInput=inp2;
  if (inp2->check()==kTRUE) {
    inp2->setInputNumber(2);
    resetInputVersions();
    if (output && firstInput!=output) resetOutputVersions();
    return kTRUE;
  }
  else ERROR_msg(HMessageMgr::DET_RUNTIMEDB,"no connection to input");
      //cerr<<"no connection to input"<<endl;
  return kFALSE;
}

Bool_t HRuntimeDb::setOutput(HParIo* op) {
  // sets the output pointer
  output=op;
  if (output->check()==kTRUE) {
    resetOutputVersions();
    if (strcmp(output->IsA()->GetName(),"HParRootFileIo")==0)
        isRootFileOutput=kTRUE;
    return kTRUE;
  }
  else ERROR_msg(HMessageMgr::DET_RUNTIMEDB,"no connection to output");
      //cerr<<"no connection to output"<<endl;
  return kFALSE;
}

HParIo* HRuntimeDb::getFirstInput() {
  // return a pointer to the first input
  if (firstInput) firstInput->cd();
  return firstInput;
}

HParIo* HRuntimeDb::getSecondInput() {
  // return a pointer to the second input
  if (secondInput) secondInput->cd();
  return secondInput;
}
HParIo* HRuntimeDb::getOutput() {
  // return a pointer to the output
  if (output) output->cd();
  return output;
}

void HRuntimeDb::closeFirstInput() {
  if (firstInput) {
    firstInput->cd();
    firstInput->close();
    firstInput=0;
  }
}

void HRuntimeDb::closeSecondInput() {
  if (secondInput) {
    secondInput->cd();
    secondInput->close();
    secondInput=0;
  }
}

Bool_t HRuntimeDb::reconnectInputs() {
  // reconnects the inputs (actually only used for the Oracle interface)
  Bool_t rc=kTRUE;
  if (firstInput) rc=firstInput->reconnect();
  if (rc&&secondInput) rc=secondInput->reconnect();
  return rc;
}

void HRuntimeDb::disconnectInputs() {
  // disconnects the inputs (actually only used for the Oracle interface)
  if (firstInput) firstInput->disconnect(); 
  if (secondInput) secondInput->disconnect();  
}

void HRuntimeDb::saveOutput() {
  // writes the setup, the versions and the containers (if not
  // yet written out) 
  // without the setup and version information the containers cannot
  // be read again!
  Bool_t rc=kTRUE;
  writeSetup();
  if (currentRun!=0) rc=writeContainers();
  if (rc&&pParamFileGenerator) fillParamFile();
  writeVersions();
  if (!rc) { ERROR_msg(HMessageMgr::DET_RUNTIMEDB,"error occured during write"); }
      //cerr<<"error occured during write"<<endl;
}

void HRuntimeDb::closeOutput() {
  // calls saveOutput() and deletes then the output
  if (output) {
    if (output->isAutoWritable()) saveOutput();
    output->close();
    resetOutputVersions();
    output=0;
    isRootFileOutput=kFALSE;
  }
}

Bool_t HRuntimeDb::makeParamFile(const Char_t* fName,const Char_t* experiment,
                                 const Char_t* startAt,const Char_t* endAt) {
  // Creates the parameter file generator, which opens a ROOT file and a log file
  // Returns kFALSE if the ROOT file already exists
  gHades->setEnableCloseInput(kFALSE);
  closeOutput();
  pParamFileGenerator=new HParamFileGenerator(experiment,startAt,endAt);
  output=pParamFileGenerator->openParameterFile(fName);
  if (output&&output->check()==kTRUE) {
    resetOutputVersions();
    isRootFileOutput=kTRUE;
    return kTRUE;
  } else {
    delete pParamFileGenerator;
    pParamFileGenerator=0;
    return kFALSE;
  } 
}

Bool_t HRuntimeDb::fillParamFile() {
  // This function is called in saveOutput, if the parameter file generator exists
  // It gets the lists of runs from Oracle and initializes the parameter containers for each run
  // The loop does not break, if an error occurs, but error information is written to a log file
  // It returns kFALSE if there is not open Oracle connection
  HParIo* pio=0;
  if (firstInput&&firstInput->InheritsFrom("HParOra2Io")) pio=firstInput;
  else if (secondInput&&secondInput->InheritsFrom("HParOra2Io")) pio=secondInput;
  if (!pio) {
      Error("fillParamFile","No Oracle Input");
    return kFALSE;
  }
  SEPERATOR_msg("*",60);
  gHades->getMsg()->info(10,HMessageMgr::DET_RUNTIMEDB,this->GetName(),
          "Parameter file generator started");
  TList * pRuns=pio->getListOfRuns(pParamFileGenerator->getExperiment(),
                                   pParamFileGenerator->getRangeStart(),
                                   pParamFileGenerator->getRangeEnd());
  if (!pRuns) {
    Error("fillParamFile","List of Runs not generated");
    return kFALSE;
  }
  SEPERATOR_msg("-",60);
  pParamFileGenerator->setListOfRuns(pRuns);
  pParamFileGenerator->writeRuns();
  fstream* logFile=pParamFileGenerator->getLogFile();
  fstream& fout=*logFile;
  fout<<"//------------------------------------------------------------------------------\n";
  fout<<"---------  actual containers in runtime database  ---------\n";
  TIter iterCont(containerList);
  HParSet* cont;
  while((cont=(HParSet*)iterCont())) {
    fout<<setw(45)<<cont->GetName()<<"  "<<cont->GetTitle()<<'\n';
  }
  fout<<"//------------------------------------------------------------------------------\n";
  HRunInfo* pRunInfo=0;
  TIter iterInfo(pRuns);
  Int_t nRuns=0, numReadErrors=0, numWriteErrors=0;
  while ((pRunInfo=(HRunInfo*)iterInfo())!=0) {
    Int_t runId=pRunInfo->getRunId();
    HRun* run=getRun(runId);
    iterCont.Reset();
    Bool_t noError=kTRUE;
    TString contNames("  Error in init of");
    if (!run) {
      run=addRun(runId,-1);
      currentRun=run;
      currentFileName=pRunInfo->GetName();
      Bool_t rc=kTRUE;
      while ((cont=(HParSet*)iterCont())) {
        if (!cont->isStatic()) rc=cont->init();
        if (!rc) {
          noError=kFALSE;
          contNames.Append(" ");
          contNames+=cont->GetName();
	} else {
          rc=writeContainer(cont,currentRun,0);
          if (!rc) {
            fout<<"Error during write of run "<<currentFileName.Data()
                <<" (runId "<<runId<<") for container "<<cont->GetName()<<'\n';
            numWriteErrors++;
          }
        }
      }
    } else {
      while ((cont=(HParSet*)iterCont())) {
        HParVersion* v=run->getParVersion((Char_t*)(cont->GetName()));
        if (!v||(v->getInputVersion(1)==-1&&v->getInputVersion(2)==-1)) {
          noError=kFALSE;
          contNames.Append(" ");
          contNames+=cont->GetName();
        }
      }
    }
    if (!noError) { 
      fout<<"Error during initialization of run "<<currentFileName.Data()
          <<" (runId "<<runId<<")"<<'\n'<<contNames<<'\n';
      numReadErrors++;
    }
    if ((++nRuns % 1000)==0) {
      gHades->getMsg()->info(10,HMessageMgr::DET_ALL,this->GetName(),
                             "%5i runs processed",nRuns);
    }
  }
  fout<<"Number runs of not fully initialized: "<<numReadErrors<<'\n';
  fout<<"Number of write errors :              "<<numWriteErrors<<'\n';
  fout<<"//------------------------------------------------------------------------------\n";
  TIter iterRuns(runs);
  HRun* run;
  fout<<"-----------------  runs, versions  -----------------\n";
  fout<<"run id"<<'\n';
  fout.setf(ios::left,ios::adjustfield);
  fout<<"  "<<setw(45)<<"container";
  fout.setf(ios::right,ios::adjustfield);
  fout<<setw(11)<<"1st-inp"
      <<setw(11)<<" 2nd-inp"
      <<setw(11)<<" output"<<'\n';
  while ((run=(HRun*)iterRuns())) {
    run->write(fout);
  }
  fout<<"---------------------  output  ----------------------\n";
  fout<<"Parameter output: "<<pParamFileGenerator->getParamFilename()<<endl;
  gHades->getMsg()->info(10,HMessageMgr::DET_ALL,this->GetName(),
                         "%5i runs processed",nRuns);
  SEPERATOR_msg("-",60);
  if (numReadErrors==0 && numWriteErrors==0) {
    gHades->getMsg()->info(10,HMessageMgr::DET_RUNTIMEDB,this->GetName(),
            "All runs initialized without errors");
  } else {
    gHades->getMsg()->info(10,HMessageMgr::DET_RUNTIMEDB,this->GetName(),
            "Number of runs not initialized: %5i",pParamFileGenerator->getParamFilename());
    gHades->getMsg()->info(10,HMessageMgr::DET_RUNTIMEDB,this->GetName(),
            "Number of write errors:         %5i",pParamFileGenerator->getParamFilename());
    SEPERATOR_msg("-",60);
  }
  gHades->getMsg()->info(10,HMessageMgr::DET_RUNTIMEDB,this->GetName(),
          "Parameter output: %s",pParamFileGenerator->getParamFilename());
  gHades->getMsg()->info(10,HMessageMgr::DET_RUNTIMEDB,this->GetName(),
          "Log file:         %s",pParamFileGenerator->getLogFilename());
  SEPERATOR_msg("*",60);
  return kTRUE;
}

void HRuntimeDb::Streamer(TBuffer &R__b)
{
 Int_t nentries=0;
 HParSet *param=0;
   // Stream an object of class HRuntimeDb
   if (R__b.IsReading()) {
      TObject::Streamer(R__b);

      R__b >> nentries;
      containerList = new TList();
      for(Int_t i=0 ; i < nentries ; i++) {

         R__b >> param;
	 gHades->getMsg()->info(10,HMessageMgr::DET_RUNTIMEDB,this->GetName(),"Loading container %s",param->GetName());
         //printf(" Loading container:  %s \n",param->GetName());
         containerList->Add( param );
      }
      R__b >> runs;

   } else {
     // R__b.WriteVersion(HRuntimeDb::IsA());
      TObject::Streamer(R__b);
      nentries = containerList->GetSize();
      R__b << nentries ;

      for (Int_t i=0; i < nentries ; i++) {
           param =  (HParSet*) containerList->At( i );
           R__b << param;
      }
      R__b << runs;
   }
}
