//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HGeomAsciiIo
//
// Class for geometry I/O from ASCII file
//
///////////////////////////////////////////////////////////////////////////////

#include "hgeomasciiio.h"
#include "hgeommedia.h"
#include "hgeomset.h"
#include "hgeominterface.h"
#include "hgeomhit.h"

ClassImp(HGeomAsciiIo)

HGeomAsciiIo::HGeomAsciiIo() {
  // Constructor
  file=0;
  filename="";
  filedir="";
  writable=kFALSE;
}

HGeomAsciiIo::~HGeomAsciiIo() {
  // Destructor
  close();
  if (file) {
    delete file;
    file=0;
  }
}

Bool_t HGeomAsciiIo::open(const Char_t* fname,const Text_t* status) {
  // Opens the file fname
  close();
  if (!file) file=new fstream();
  else (file->clear());
  if (!filedir.IsNull()) filename=filedir+"/"+fname;
  else filename=fname;
  filename=filename.Strip();  
  if (strcmp(status,"in")==0) {
    file->open(filename,ios::in);
    writable=kFALSE;
  } else {
    if (strcmp(status,"out")==0) {
      file->open(filename,ios::in);
      if (!isOpen()) {
        file->close();
        file->clear();
        file->open(filename,ios::out);
        writable=kTRUE;
      } else {
        file->close();
        Error("open","Output file %s exists already and will not be recreated.",
              filename.Data());
        return kFALSE;
      }
    } else Error("open","Invalid file option!");
  }
  if (file->rdbuf()->is_open()==0) {
    Error("open","Failed to open file %s",filename.Data());
    return kFALSE;
  }
  return kTRUE;
}

Bool_t HGeomAsciiIo::isOpen() {
  // Returns kTRUE, if the file is open
  if (file && file->rdbuf()->is_open()==1) return kTRUE; 
  return kFALSE;
}

Bool_t HGeomAsciiIo::isWritable() {
  // Returns kTRUE, if the file is open and writable
  if (isOpen() && writable) return kTRUE; 
  return kFALSE;
}

void HGeomAsciiIo::close() {
  // Closes the file
  if (isOpen()) { 
    file->close();
    filename="";
  }
}

void HGeomAsciiIo::print() {
  // Prints file information
  if (isOpen()) {
     if (writable) cout<<"Open output file: "<<filename<<endl;
     else cout<<"Open input file: "<<filename<<endl;
  }
  else cout<<"No file open."<<endl;
}

Bool_t HGeomAsciiIo::read(HGeomMedia* media) {
  // Reads the media from file
  if (!isOpen() || writable || media==0) return kFALSE;
  media->read(*file);
  return kTRUE;
}

Bool_t HGeomAsciiIo::read(HGeomHit* hit) {
  // Reads the hit definition from file
  if (!isOpen() || writable || hit==0) return kFALSE;
  hit->read(*file);
  return kTRUE;
}

Bool_t HGeomAsciiIo::read(HGeomSet* set,HGeomMedia* media) {
  // Reads the geometry set from file
  if (!isOpen() || writable || set==0) return kFALSE;
  set->read(*file,media);
  return kTRUE;
}

Bool_t HGeomAsciiIo::write(HGeomMedia* media) {
  // Writes the media to file
  if (!isOpen() || !writable || media==0) return kFALSE;
  media->write(*file);
  return kTRUE;
}

Bool_t HGeomAsciiIo::write(HGeomSet* set) {
  // Writes the geometry set to file
  if (!isOpen() || !writable || set==0) return kFALSE;
  set->write(*file);
  return kTRUE;
}

Bool_t HGeomAsciiIo::write(HGeomHit* hit) {
  // Writes the geometry set to file
  if (!isOpen() || !writable || hit==0) return kFALSE;
  hit->write(*file);
  return kTRUE;
}

Bool_t HGeomAsciiIo::readGeomConfig(HGeomInterface* interface) {
  // Reads the GEANT configuration file
  if (!isOpen() || writable || interface==0) return kFALSE;
  TString buf(256);
  TString simRefRun,historyDate,paramFile;  
  Int_t k=-1;
  while(!(*file).eof()) {
    buf.ReadLine(*file);
    buf=buf.Strip(buf.kBoth);
    if (!buf.IsNull() && buf(0,2)!="//" && buf(0,1)!="*") {
      if (buf.Contains(".geo")||buf.Contains("_gdb")||buf.Contains(".hit")) {
        interface->addInputFile(buf.Data());
      } else {
        if (buf.Contains(".setup")) {
          interface->addSetupFile(buf.Data());
        } else {
          if (buf.Contains("SimulRefRunDb:")) {
            k=buf.Last(' ')+1;
            if (k) simRefRun=buf(k,buf.Length()-k);
          } else {
            if (buf.Contains("HistoryDateDb:")) {
              k=buf.First(' ')+1;
              if (buf.Length()>k) {
                historyDate=buf(k,buf.Length()-k);
                historyDate=historyDate.Strip(historyDate.kBoth);
	      }
            } else {
              if (buf.Contains("ParameterFile:")) {
                k=buf.First(' ')+1;
                if (buf.Length()>k) {
                  paramFile=buf(k,buf.Length()-k);
                  paramFile=paramFile.Strip(paramFile.kBoth);
                  interface->addParamFile(paramFile.Data());
		}
              }
            }
          }
        }
      }
    }
  }
  Bool_t rc=kTRUE;
  HGeomIo* oraIo=interface->getOraInput(); 
  if (oraIo) {
    if (historyDate.Length()>0) rc=oraIo->setHistoryDate(historyDate.Data());
    if (rc&&simRefRun.Length()>0) rc=oraIo->setSimulRefRun(simRefRun.Data());
  }
  return rc;
}

Bool_t HGeomAsciiIo::readDetectorSetup(HGeomInterface* interface) {
  // Reads the detector setups, needed for create only subsets
  if (!isOpen() || writable || interface==0) return kFALSE;
  const Int_t maxbuf=256;
  Char_t buf[maxbuf];
  TString s, det;
  HGeomSet* set=0;
  Int_t maxModules=0, secNo=-1;
  Int_t* mod=0;
  const Char_t d[]=" ";
  while(!(*file).eof()) {
    (*file).getline(buf,maxbuf);
    if (strlen(buf)>=3 && buf[1]!='/') { 
      if (buf[0]=='[') {
        set=0;
        if (mod) {
          delete [] mod;
          mod=0;
        }
        s=buf;
        Ssiz_t n=s.First(']');
        det=s(1,n-1);
        det.ToLower();
        set=interface->findSet(det);
        if (!set) {
          Error("readDetectorSetup","Detector %s not found",det.Data());
          if (mod) delete [] mod;
          return kFALSE;
        }
        maxModules=set->getMaxModules();
        mod=new Int_t[maxModules];
      } else {
        if (set&&mod) {
          Char_t* ss=strtok(buf,d);
          if (ss&&strlen(ss)>3) {
            secNo=(Int_t)(ss[3]-'0')-1;
            for(Int_t i=0;i<maxModules&&mod;i++) {
              ss=strtok(NULL,d);
              if (ss) sscanf(ss,"%i",&mod[i]);
            }
            set->setModules(secNo,mod);
          }
        } else {
          if (mod) delete [] mod;
          return kFALSE;
        }
      }
    }
  }
  if (mod) delete [] mod;
  return kTRUE;
}
