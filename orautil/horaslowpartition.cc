//*-- AUTHOR : Ilse Koenig
//*-- Created : 16/08/2004 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HOraSlowPartition
//
// Class for a slowcontrol data partition
//
//////////////////////////////////////////////////////////////////////////////

#include "horaslowpartition.h"
#include "horaslowperiod.h"
#include "horaslowarchrateobj.h"
#include "TGraph.h"
#include "TDatime.h"
#include "TH1F.h"
#include "TList.h"
#include <stdlib.h>

ClassImp(HOraSlowPartition)

HOraSlowPartition::HOraSlowPartition(const Char_t* name) {
  // Constructor with argument name = name for the data partition
  SetName(name);
  pRunPeriods=0;
  periodIter=0;
  pRates=0;
  ratesIter=0;
  pGraph=0;
}

HOraSlowPartition::~HOraSlowPartition() {
  // Destructor
  clearRunPeriods();
  clearRates();
}

void HOraSlowPartition::clearRunPeriods() {
  // Deletes the array of periods
  if (periodIter) {
    delete periodIter;
    periodIter=0;
  }
  if (pRunPeriods) {
    pRunPeriods->Delete();
    delete pRunPeriods;
    pRunPeriods=0;
  }
}

void HOraSlowPartition::clearRates() {
  // Deletes the array of archiver rates
  deleteGraph();
  if (ratesIter) {
    delete ratesIter;
    ratesIter=0;
  }
  if (pRates) {
    pRates->Delete();
    delete pRates;
    pRates=0;
  }
}

void HOraSlowPartition::deleteGraph() {
  // Deletes the graph of the archiver rates
  if (pGraph) {
    delete pGraph;
    pGraph=0;
  }
}

TObjArray* HOraSlowPartition::setNumPeriods(Int_t n) {
  // Creates an array of size n for the periods 
  clearRunPeriods();
  pRunPeriods=new TObjArray(n);
  periodIter=pRunPeriods->MakeIterator();
  return pRunPeriods;
}

void HOraSlowPartition::setRates(TObjArray* p) {
  // Sets the array of archiver rates
  clearRates();
  pRates=p;
  ratesIter=pRates->MakeIterator();
}

HOraSlowPeriod* HOraSlowPartition::getPeriod(Int_t i) {
  // Returns the pointer to the period at index i in the array
  if (pRunPeriods) return (HOraSlowPeriod*)(pRunPeriods->At(i));
  else return 0;
}

HOraSlowPeriod* HOraSlowPartition::getRun(Int_t runId) {
  // Returns the pointer to the period corresponding to the run with id runId 
  HOraSlowPeriod* p=0;
  if (periodIter) {
    periodIter->Reset();
    while ((p=((HOraSlowPeriod*)(periodIter->Next())))!=0) {
      if (p->getRunId()==runId) break;
    }
  }
  return p;
}

HOraSlowPeriod* HOraSlowPartition::getRun(const Char_t* fname) {
  // Returns the pointer to the period corresponding to the run with hld-filename fname 
  HOraSlowPeriod* p=0;
  if (periodIter) {
    periodIter->Reset();
    while ((p=((HOraSlowPeriod*)(periodIter->Next())))!=0) {
      if (strcmp(p->getFilename(),fname)==0) break;
    }
  }
  return p;
}

void HOraSlowPartition::print(Int_t opt) {
  // Prints the data of all periods to stdout
  // option opt 0 = all periods
  //            1 = only hld-files
  //            2 = apply hld-file filter
  if (periodIter) {
    periodIter->Reset();
    HOraSlowPeriod* p=0;
    Int_t i=0;
    cout<<"---------------------------------------------------------------------\n";
    cout<<"  Run periods of partition "<<GetName()<<'\n';
    cout<<"  Format: array index, startime, endtime, run id, filename\n";
    cout<<"---------------------------------------------------------------------\n";
    while ((p=((HOraSlowPeriod*)(periodIter->Next())))!=0) {
      if (opt==0
          ||(opt==1&&strlen(p->getFilename())>4)
          ||(opt==2&&p->getFilterFlag()==1)) {
        cout<<setw(5)<<i<<"  ";
        p->print();
      }
      i++;
    }
    cout<<"---------------------------------------------------------------------\n";
  } else cout<<"  NO DATA\n";
}
  
void HOraSlowPartition::write(fstream& fout,Int_t opt) {
  // Streams the data of all periods
  // option opt 0 = all periods
  //            1 = only hld-files
  //            2 = apply hld-file filter
  if (periodIter) {
    periodIter->Reset();
    HOraSlowPeriod* p=0;
    Int_t i=0;
    fout<<"---------------------------------------------------------------------\n";
    fout<<"  Run periods of partition "<<GetName()<<'\n';
    fout<<"  Format: array index, startime, endtime, run id, filename\n";
    fout<<"---------------------------------------------------------------------\n";
    while ((p=((HOraSlowPeriod*)(periodIter->Next())))!=0) {
      if (opt==0
          ||(opt==1&&strlen(p->getFilename())>4)
          ||(opt==2&&p->getFilterFlag()==1)) {
        fout<<setw(5)<<i<<"  ";
        p->write(fout);
      }
      i++;
    }
    fout<<"---------------------------------------------------------------------\n";
  }
}

void HOraSlowPartition::setHldFileFilter(TList* l) {
  // Takes a list of hld-filenames, loops over all periods and sets a flag in the corresponding periods
  // Depending on the option, this filter is used to show the data or to skip the periods not in the list 
  if (periodIter) {
    periodIter->Reset();
    HOraSlowPeriod* p=0;
    while ((p=((HOraSlowPeriod*)(periodIter->Next())))!=0) {
      Int_t f=1;
      if (l) {
        const Char_t* s=p->getFilename();   
        if (strlen(s)==0||l->FindObject(s)==0) f=0;
      }
      p->setFilterFlag(f);
    }
  }
} 

void HOraSlowPartition::printRates() {
  // Prints the archiver rates to stdout
  if (ratesIter) {
    ratesIter->Reset();
    HOraSlowArchRateObj* p=0;
    Int_t i=0;
    cout<<"---------------------------------------------------------------------\n";
    cout<<"  Archiver rates of partition "<<GetName()<<'\n';
    cout<<"  Format: startime, entries/minute \n";
    cout<<"---------------------------------------------------------------------\n";
    while ((p=((HOraSlowArchRateObj*)(ratesIter->Next())))!=0) {
      p->print();
      i++;
    }
    cout<<"---------------------------------------------------------------------\n";
  } else cout<<"  NO DATA\n";
}  
 
void HOraSlowPartition::writeRates(fstream& fout) {
  // Streams the archiver rates
  if (ratesIter) {
    ratesIter->Reset();
    HOraSlowArchRateObj* p=0;
    Int_t i=0;
    fout<<"---------------------------------------------------------------------\n";
    fout<<"  Archiver rates of partition "<<GetName()<<'\n';
    fout<<"  Format: startime, entries/minute \n";
    fout<<"---------------------------------------------------------------------\n";
    while ((p=((HOraSlowArchRateObj*)(ratesIter->Next())))!=0) {
      p->write(fout);
      i++;
    }
    fout<<"---------------------------------------------------------------------\n";
  } else fout<<"  NO DATA\n";
}  

TGraph* HOraSlowPartition::getRatesGraph(Int_t mStyle,Int_t mColor) {
  // Returns a graph of the archiver rates
  // Default style: 7, default color 4 (blue)
  if (pRates==0) return 0;
  deleteGraph();
  Int_t len=pRates->GetLast()+1;
  pGraph=new TGraph(len);
  ratesIter->Reset();
  HOraSlowArchRateObj* p=0;
  Int_t i=0;
  TString startTime;
  while ((p=((HOraSlowArchRateObj*)(ratesIter->Next())))!=0) {
    if (i==0) {
      startTime=p->getStartTime();
      pGraph->SetPoint(i,0,p->getRate());      
    } else {
      pGraph->SetPoint(i,p->getTimeDiff(startTime.Data()),p->getRate());
    }
    i++;
  }
  pGraph->SetTitle(GetName());
  pGraph->SetMarkerStyle(mStyle);
  pGraph->SetMarkerColor(mColor);
  TH1F* hist=pGraph->GetHistogram();
  TAxis* xaxis=hist->GetXaxis();
  TString s("Time (Start at ");
  s+=startTime;
  s+=")";
  xaxis->SetTitle(s);
  TDatime t1(startTime.Data());
  // Fix for daylight (ROOT is one hour off!) 
  UInt_t utc=t1.Convert();
  time_t timeoff=(time_t)((Long_t)(utc));
  struct tm* loctis=localtime(&timeoff);
  Char_t tmp[20];
  strftime(tmp,256,"%z",loctis);
  if (strcmp(tmp,"+0200")==0) utc+=3600;
  xaxis->SetTimeDisplay(1);
  xaxis->SetTimeOffset(utc);
  xaxis->SetTimeFormat("%d/%m:%H");
  TAxis* yaxis=hist->GetYaxis();
  yaxis->SetTitle("Entries per minute");
  return pGraph;
}


