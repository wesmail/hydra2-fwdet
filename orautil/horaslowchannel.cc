//*-- AUTHOR   : Ilse Koenig
//*-- Created  : 13/08/2004 by Ilse Koenig
//*-- Modified : 13/04/2005 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HOraSlowChannel
//
// Class for a slowcontrol channel
//
//////////////////////////////////////////////////////////////////////////////

#include "horaslowchannel.h"
#include "horaslowchanrunsum.h"
#include "horaslowchanmeta.h"
#include "horaslowchanraw.h"
#include "horaslowpartition.h"
#include "horaslowperiod.h"
#include "horaslowmanager.h"
#include "horaslowreader.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TDatime.h"
#include "TH1F.h"
#include "TAxis.h"

#include <stdlib.h>

ClassImp(HOraSlowChannel)

HOraSlowChannel::HOraSlowChannel(const Char_t* name) {
  // Constructor with argument channel name
  SetName(name);
  channelType="F";
  channelId=-1;
  pRunSumData=0;
  pMetaData=0;
  pRawData=0;
  runSumDataIter=0;
  rawIter=0;
  pGraph=0;
  pRawGraph=0;
  partition=0;
  maxPrecision=0;
}

HOraSlowChannel::~HOraSlowChannel() {
  // Destructor
  clearRunSumData();
  if (pMetaData) {
    pMetaData->Delete();
    delete pMetaData;
    pMetaData=0;
  }
  clearRawData();
}

void HOraSlowChannel::deleteGraph() {
  // Deletes the graph of the summary data
  if (pGraph) {
    delete pGraph;
    pGraph=0;
  }
}

void HOraSlowChannel::deleteRawGraph() {
  // Deletes the graph of the raw data
  if (pRawGraph) {
    delete pRawGraph;
    pRawGraph=0;
  }
}

void HOraSlowChannel::clearRunSumData() {
// Clears all 
  if (runSumDataIter) {
    delete runSumDataIter;
    runSumDataIter=0;
  }
  if (pRunSumData) {
    pRunSumData->Delete();
    delete pRunSumData;
    pRunSumData=0;
  }  
  deleteGraph();
}

void HOraSlowChannel::clearRawData() {
  // Deletes the raw data and the corresponding graph
  if (rawIter) {
    delete rawIter;
    rawIter=0;
  }
  if (pRawData) {
    pRawData->Delete();
    delete pRawData;
    pRawData=0;
  }
  rawStart="";
  rawEnd="";
  deleteRawGraph();
}

void HOraSlowChannel::setPartition(HOraSlowPartition* p) {
  // Sets the partition pointer and clears all summary data
  clearRunSumData();
  partition=p;
}


void HOraSlowChannel::setRunSumData(TObjArray* p) {
  // Sets the pointer to the array of summary data and creates the iterator
  clearRunSumData();
  pRunSumData=p;
  if (p) runSumDataIter=p->MakeIterator();      
}

void HOraSlowChannel::setRawData(TObjArray* p,const Char_t* t1,const Char_t* t2) {
  // Sets the pointer to the array of raw data in the time interval t1 - t2
  // and creates the iterator
  clearRawData();
  pRawData=p;
  rawStart=t1;
  rawEnd=t2;
  if (p) rawIter=p->MakeIterator();
}
  

void HOraSlowChannel::setMetaData(TObjArray* p){
  // Sets the pointer to the array of meta data
  if (pMetaData) {
    pMetaData->Delete();
    delete pMetaData;
    pMetaData=0;
  }
  pMetaData=p;
}

void HOraSlowChannel::printMetaData() {
  // Prints the meta data to stdout
  if (pMetaData) {
    cout<<"---------------------------------------------------------------------\n";
    cout<<"  Meta data of "<<GetName()<<'\n';
    cout<<"---------------------------------------------------------------------\n";
    for(Int_t i=0;i<pMetaData->GetSize();i++) {
      HOraSlowChanMeta* p=(HOraSlowChanMeta*)(pMetaData->At(i));
      p->print();
    }
    cout<<"---------------------------------------------------------------------\n";
  } else cout<<"No meta data for channel "<<GetName()<<endl;
}

void HOraSlowChannel::writeMetaData(fstream& fout) {
  // Streams the meta data
  if (pMetaData) {
    fout<<"---------------------------------------------------------------------\n";
    fout<<"  Meta data of "<<GetName()<<'\n';
    fout<<"---------------------------------------------------------------------\n";
    for(Int_t i=0;i<pMetaData->GetSize();i++) {
      HOraSlowChanMeta* p=(HOraSlowChanMeta*)(pMetaData->At(i));
      p->write(fout);
    }
    fout<<"---------------------------------------------------------------------\n";
  }
}

void HOraSlowChannel::printRunSumData(Int_t opt) {
  // Prints the summary data to stdout
  // option opt 0 = all periods
  //            1 = only hld-files
  //            2 = apply hld-file filter
  if (pRunSumData&&partition) {
    runSumDataIter->Reset();
    HOraSlowChanRunSum* p=0;
    TObjArray* runs=partition->getRunPeriods();
    Int_t i=0;
    cout<<"---------------------------------------------------------------------\n";
    cout<<"  Run summary of channel "<<GetName()<<'\n';
    cout<<"  Format: array index, mean, sigma, min value, max value,\n";
    cout<<"          number of data, status, monitor channel rate,\n";
    cout<<"          run id, filename\n";
    cout<<"---------------------------------------------------------------------\n";
    while ((p=((HOraSlowChanRunSum*)(runSumDataIter->Next())))!=0) {
      if (opt==0
          ||(opt==1&&strlen(((HOraSlowPeriod*)(runs->At(i)))->getFilename())>4)
          ||(opt==2&&((HOraSlowPeriod*)(runs->At(i)))->getFilterFlag()==1)) {
        cout<<setw(5)<<i;
        p->print(maxPrecision+1);
      }
      i++;
    }
    cout<<"---------------------------------------------------------------------\n";
  } else cout<<"NO DATA"<<endl;
}
  
void HOraSlowChannel::writeRunSumData(fstream& fout,Int_t opt) {
  // Streams the summary data to stdout
  // option opt 0 = all periods
  //            1 = only hld-files
  //            2 = apply hld-file filter
  if (pRunSumData&&partition) {
    runSumDataIter->Reset();
    HOraSlowChanRunSum* p=0;
    TObjArray* runs=partition->getRunPeriods();
    Int_t i=0;
    fout<<"---------------------------------------------------------------------\n";
    fout<<"  Run summary of channel "<<GetName()<<'\n';
    fout<<"  Format: array index, mean, sigma, min value, max value,\n";
    fout<<"          number of data, status, monitor channel rate,\n";
    fout<<"          run id, filename\n";
    fout<<"---------------------------------------------------------------------\n";
    while ((p=((HOraSlowChanRunSum*)(runSumDataIter->Next())))!=0) {
      if (opt==0
          ||(opt==1&&strlen(((HOraSlowPeriod*)(runs->At(i)))->getFilename())>4)
          ||(opt==2&&((HOraSlowPeriod*)(runs->At(i)))->getFilterFlag()==1)) {
        fout<<setw(5)<<i;
        p->write(fout,maxPrecision+1);
      }
      i++;
    }
    fout<<"---------------------------------------------------------------------\n";
  }
}

Bool_t HOraSlowChannel::readRawData(Int_t index) {
  // Reads the raw data of a period specified by the array index
  HOraSlowPeriod* period=0;
  if (gHOraSlowManager==0||partition==0||(period=partition->getPeriod(index))==0) {
    Error("*** readRawData","Period not found!");
    return kFALSE;
  }
  Bool_t rc=partition->openOraInput();
  if (!rc) return rc;
  clearRawData();
  if (pRunSumData) {
    HOraSlowChanRunSum* p=(HOraSlowChanRunSum*)(pRunSumData->At(index));
    if (p) {
      if (p->getNData()==0) {
        Error("readRawData","No raw data found.");
        return kFALSE;
      }
    }
  }
  return partition->getOraReader()->readRawData(
                 this,period->getStartTime(),period->getEndTime());
}

Bool_t HOraSlowChannel::readRawData(const Char_t* t1, const Char_t* t2) {
  // Reads the raw data of a period specified by a time range t1 - t2
  if (gHOraSlowManager==0||partition==0) return kFALSE;
  Bool_t rc=partition->openOraInput();
  if (!rc) return rc;
  Int_t l1=strlen(t1);
  Int_t l2=strlen(t2);
  Bool_t isWrongFormat=kFALSE;
  TString start=t1;
  TString end=t2;
  switch(l1) {
    case 10: {start+=" 00:00:00"; break;}
    case 13: {start+=":00:00";    break;}
    case 16: {start+=":00";       break;}
    case 19: {                    break;}
    default: {isWrongFormat=kTRUE;}
  }
  switch(l2) {
    case 10: {end+=" 00:00:00"; break;}
    case 13: {end+=":00:00";    break;}
    case 16: {end+=":00";       break;}
    case 19: {                  break;}
    default: {isWrongFormat=kTRUE;}
  }
  if (isWrongFormat||t1[4]!='-'||t1[7]!='-'||t2[4]!='-'||t2[7]!='-') {
    Error("readRawData","Dates must be specified in date format yyyy-mm-dd hh:mi:ss");
    return kFALSE;
  }
  clearRawData();
  return partition->getOraReader()->readRawData(this,start.Data(),end.Data());
}

void HOraSlowChannel::printRawData() {
  // Prints the raw data to stdout
  if (pRawData) {
    rawIter->Reset();
    HOraSlowChanRaw* p=0;
    cout<<"---------------------------------------------------------------------\n";
    cout<<"  Raw of channel "<<GetName()<<'\n';
    cout<<"  Time range:  "<<rawStart<<" - "<<rawEnd<<'\n';
    cout<<"  Format:      timestamp, nano seconds, value, status\n";
    cout<<"  Time Format: yyyy-mm-dd hh:mi:ss (TDatime SQL compatible string)\n";
    cout<<"---------------------------------------------------------------------\n";
    while ((p=((HOraSlowChanRaw*)(rawIter->Next())))!=0) {
      p->print(maxPrecision);
    }
    cout<<"---------------------------------------------------------------------\n";
  } else cout<<"NO DATA"<<endl;
}

void HOraSlowChannel::writeRawData(fstream& fout) {
  // Streams the raw data
  if (pRawData) {
    rawIter->Reset();
    HOraSlowChanRaw* p=0;
    fout<<"---------------------------------------------------------------------\n";
    fout<<"  Raw of channel "<<GetName()<<'\n';
    fout<<"  Time range: "<<rawStart<<" - "<<rawEnd<<'\n';
    fout<<"  Format: seconds since start time, nano seconds, value, status\n";
    fout<<"  Time Format: yyyy-mm-dd hh:mi:ss (TDatime SQL compatible string)\n";
    fout<<"---------------------------------------------------------------------\n";
    while ((p=((HOraSlowChanRaw*)(rawIter->Next())))!=0) {
      p->write(fout,maxPrecision);
    }
    fout<<"---------------------------------------------------------------------\n";
  }
}

TGraphErrors* HOraSlowChannel::getRunSumMeanSigmaGraph(Int_t opt,
                               Int_t mStyle,Int_t mColor) {
  // Returns a graph with mean and sigma values
  // Default style: 7, default color 4 (blue)
  // option opt 0 = all periods
  //            1 = only hld-files
  //            2 = apply hld-file filter
  // The x-axis is the index of the periods
  if (pRunSumData==0||partition==0) return 0;
  deleteGraph();
  Int_t len=pRunSumData->GetLast()+1;
  TGraphErrors* graph=new TGraphErrors(len);
  pGraph=graph;
  runSumDataIter->Reset();
  TObjArray* runs=partition->getRunPeriods();
  HOraSlowChanRunSum* p=0;
  Int_t i=0;
  while ((p=((HOraSlowChanRunSum*)(runSumDataIter->Next())))!=0) {
    if (opt==0
        ||(opt==1&&strlen(((HOraSlowPeriod*)(runs->At(i)))->getFilename())>4)
        ||(opt==2&&((HOraSlowPeriod*)(runs->At(i)))->getFilterFlag()==1)) {
      graph->SetPoint(i,i,p->getMean());
      graph->SetPointError(i,0,p->getSigma());
    }
    i++;
  }
  graph->SetTitle(GetName());
  graph->SetMarkerStyle(mStyle);
  graph->SetMarkerColor(mColor);
  return graph;
}

TGraphAsymmErrors* HOraSlowChannel::getRunSumMeanMinMaxGraph(Int_t opt,
                                    Int_t mStyle,Int_t mColor) {
  // Returns a graph with the mean value and the min and max values as error bar
  // Default style: 7, default color 2 (red)
  // option opt 0 = all periods
  //            1 = only hld-files
  //            2 = apply hld-file filter
  // The x-axis is the index of the periods
  if (pRunSumData==0||partition==0) return 0;
  deleteGraph();
  Int_t len=pRunSumData->GetLast()+1;
  TGraphAsymmErrors* graph=new TGraphAsymmErrors(len);
  pGraph=graph;
  runSumDataIter->Reset();
  TObjArray* runs=partition->getRunPeriods();
  HOraSlowChanRunSum* p=0;
  Int_t i=0;
  while ((p=((HOraSlowChanRunSum*)(runSumDataIter->Next())))!=0) {
    if (opt==0
        ||(opt==1&&strlen(((HOraSlowPeriod*)(runs->At(i)))->getFilename())>4)
        ||(opt==2&&((HOraSlowPeriod*)(runs->At(i)))->getFilterFlag()==1)) {
      graph->SetPoint(i,i,p->getMean());
      graph->SetPointError(i,0,0,p->getMean()-p->getMinVal(),p->getMaxVal()-p->getMean());
    }
    i++;
  }
  graph->SetTitle(GetName());
  graph->SetMarkerStyle(mStyle);
  graph->SetMarkerColor(mColor);
  return graph;
}

TGraph* HOraSlowChannel::getRawDataGraph(Int_t mStyle,Int_t mColor) {
  // Returns a graph of the raw data
  // Default style: 7, default color 4 (blue)
  if (pRawData==0) return 0;
  deleteRawGraph();
  Int_t len=pRawData->GetLast()+1;
  TGraph* graph=new TGraph(len);
  pRawGraph=graph;
  rawIter->Reset();
  HOraSlowChanRaw* p=0;
  Int_t i=0;
  while ((p=((HOraSlowChanRaw*)(rawIter->Next())))!=0) {
    graph->SetPoint(i,p->getTimeDiff(rawStart.Data()),p->getValue());
    i++;
  }
  graph->SetTitle(GetName());
  graph->SetMarkerStyle(mStyle);
  graph->SetMarkerColor(mColor);
  TH1F* hist=graph->GetHistogram();
  TAxis* xaxis=hist->GetXaxis();
  TString s("Time (Start at ");
  s+=rawStart;
  s+=")";
  xaxis->SetTitle(s);
  TDatime t1(rawStart.Data());
  // Fix for daylight (ROOT is one hour off!) 
  UInt_t utc=t1.Convert();
  time_t timeoff=(time_t)((Long_t)(utc));
  struct tm* loctis=localtime(&timeoff);
  Char_t tmp[20];
  strftime(tmp,256,"%z",loctis);
  if (strcmp(tmp,"+0200")==0) utc+=3600;
  xaxis->SetTimeDisplay(1);
  xaxis->SetTimeOffset(utc);
  xaxis->SetTimeFormat("%H:%M");
  return graph;
}
