//*-- AUTHOR : Ilse Koenig
//*-- Created : 25/05/2010 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HParOra2Io
//
// Hydra2 interface class to database Oracle
// It is derived from interface base class HParIo.
//
// It contains a pointer to the connection class (type HOra2Conn)  and a list
// of interface classes for the detectors. Each detector has its own interface
// class all inherited from a common base class.
//
///////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hparora2io.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hdetpario.h"
#include "hora2info.h"
#include "hcondparora2io.h"
#include "hspecparora2io.h"
#include "hmdcparora2io.h"
#include "htofparora2io.h"
#include "hrichparora2io.h"
#include "hshowerparora2io.h"
#include "hstartparora2io.h"
#include "hwallparora2io.h"
#include "hrpcparora2io.h"
#include "hemcparora2io.h"
#include "hpiontrackerparora2io.h"
#include <stdio.h>
#include <iostream> 
#include <iomanip>

ClassImp(HParOra2Io)

HParOra2Io::HParOra2Io() {
  // private default constructor
  // creates the connection class but doesn't open a connection
  pConn=new HOra2Conn();
  autoWritable=kFALSE;
  inputName="Oracle";
}


HParOra2Io::~HParOra2Io() {
  // default constructor closes an open connection
  close();
  if (pConn) delete pConn;
}


Bool_t HParOra2Io::open() {
  // opens connection to database Hades as user Hades
  // user Hades has Readonly access to Oracle tables
  // all open()-functions activate the detetctor I/Os
  isConnected=pConn->open();
  return activateDetIo();
}


Bool_t HParOra2Io::open(Char_t *userName) {
  // opens connection to database Hades for user given by name
  // asks for password
  isConnected=pConn->open(userName);
  return activateDetIo();
}


Bool_t HParOra2Io::open(Char_t *dbName, Char_t *userName) {
  // opens connection to database with name dbName for user given by name
  // asks for password
  isConnected=pConn->open(dbName,userName);
  return activateDetIo();
} 


Bool_t HParOra2Io::reconnect() {
  // reopens the connection (only applicable for default user hades_ana)
  isConnected=pConn->reconnect();
  return isConnected;
}


void HParOra2Io::close() {
  // closes the connection with automatic ROLLBACK
  pConn->close();
  isConnected=kFALSE;
  if (detParIoList) detParIoList->Delete();
  if (pInfo) {
    delete pInfo;
    pInfo=0;
  }
}


void HParOra2Io::disconnect() {
  pConn->disconnect();
  isConnected=kFALSE;
}


void HParOra2Io::print() {
  // prints information about the database connection
  pConn->print();
  if (isConnected) {
    TIter next(detParIoList);
    HDetParIo* io;
    cout<<"Hydra2 Oracle interface"<<endl;
    cout<<"detector I/Os: ";
    while ((io=(HDetParIo*)next())) {
      cout<<" "<<io->GetName();
    }
    cout<<endl;
  }
}

Bool_t HParOra2Io::setHistoryDate(const Char_t* timestamp) {
  // Sets the date to retrieve historic data
  // Returns kFALSE when the date string cannot be converted to a valid date.
  return pConn->setHistoryDate(timestamp);
}

Bool_t HParOra2Io::setParamRelease(const Char_t* releaseName) {
  // Sets the history date to the creation date of the parameter release give by name
  // Returns kFALSE when the release is not found.
  return pConn->setParamRelease(releaseName);
}

Bool_t HParOra2Io::activateDetIo() {
  // creates the I/O for all detector in the setup
  if (isConnected==kFALSE) return kFALSE;
  pInfo=new HOra2Info(pConn);
  HDetParIo* io=new HCondParOra2Io(pConn);
  detParIoList->Add(io);
  io=new HSpecParOra2Io(pConn);
  detParIoList->Add(io);
  TList* detSet=gHades->getSetup()->getListOfDetectors();
  TIter next(detSet);
  HDetector* det;
  while ((det=(HDetector*)next())) {
    const Char_t* name=det->GetName();
    io=0;
    if (strcmp(name,"Mdc")==0)         io=new HMdcParOra2Io(pConn);
    if (strcmp(name,"Tof")==0)         io=new HTofParOra2Io(pConn);
    if (strcmp(name,"Rich")==0)        io=new HRichParOra2Io(pConn);
    if (strcmp(name,"Shower")==0)      io=new HShowerParOra2Io(pConn);
    if (strcmp(name,"Start")==0)       io=new HStartParOra2Io(pConn);
    if (strcmp(name,"Wall")==0)        io=new HWallParOra2Io(pConn);
    if (strcmp(name,"Rpc")==0)         io=new HRpcParOra2Io(pConn);
    if (strcmp(name,"Emc")==0)         io=new HEmcParOra2Io(pConn);
    if (strcmp(name,"PionTracker")==0) io=new HPionTrackerParOra2Io(pConn);
    if (io) detParIoList->Add(io);
  }
  return kTRUE;
}

void HParOra2Io::setDetParIo(const Text_t* ioName) {
  // creates the specified I/O
  if (isConnected) {
    TObject* io=detParIoList->FindObject(ioName);
    if (!io) {
      if (strcmp(ioName,"HMdcParIo")==0)         io=new HMdcParOra2Io(pConn);
      if (strcmp(ioName,"HTofParIo")==0)         io=new HTofParOra2Io(pConn);
      if (strcmp(ioName,"HRichParIo")==0)        io=new HRichParOra2Io(pConn);
      if (strcmp(ioName,"HShowerParIo")==0)      io=new HShowerParOra2Io(pConn);
      if (strcmp(ioName,"HStartParIo")==0)       io=new HStartParOra2Io(pConn);
      if (strcmp(ioName,"HWallParIo")==0)        io=new HWallParOra2Io(pConn);
      if (strcmp(ioName,"HRpcParIo")==0)         io=new HRpcParOra2Io(pConn);
      if (strcmp(ioName,"HEmcParIo")==0)         io=new HEmcParOra2Io(pConn);
      if (strcmp(ioName,"HPionTrackerParIo")==0) io=new HPionTrackerParOra2Io(pConn);
      if (io) detParIoList->Add(io);
    }
  }
}    

TList* HParOra2Io::getListOfRuns(const Char_t* experiment,
                                const Char_t* startAt,const Char_t* endAt) {
  // returns the list of runs for the specifies experiment and range
  // accepts dates (format DD-MON-YYYY HH24:MI:SS), hld-filenames and run ids
  if (isConnected) {
      return pInfo->getListOfRuns(experiment,startAt,endAt);
  }
  Error("getListOfRuns","No connection to Oracle");
  return 0;
}
