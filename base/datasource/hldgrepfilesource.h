#ifndef HLDGREPFILESOURCE_H
#define HLDGREPFILESOURCE_H

#include "hldfilesourcebase.h"
#include "TIterator.h"
#include "TString.h"
#include "TObjString.h"
#include "TList.h"
#include "TStopwatch.h"

#include <time.h>
#include <cstdlib>

class HldGrepFileSource : public HldFileSourceBase {

protected:

    static time_t lastfile;       //! last modification time of last used file
    static time_t timelimit;      //! time stamp for the search of new files (files should be newer)
    static time_t tcurrent;       //! current time
    static Int_t timeoffset;      //! offset in seconds required to t_current
    static TString fullname;      //! full filename: path/file
    static TString fullnamesort;  //! full filename for sorting: path/modtime_file
    static TString path;          //! path to directory used for grep of new hld files
    Int_t first;                  //! flag : 0 if no file has be selected
    Int_t mode;                   //! flag : 1 take newest file , 1: start from given file/time than take the next in list
    Int_t interval;               //! time interval in seconds for grep on dir
    TStopwatch timer;             //! timer for grep interval
    Int_t referenceId;            //! referrence ID for files
    Bool_t dostop;                //! flag to stop the source from file reading (ckecked each event)
    Int_t distanceToLast;         //! distance to last file : last - distanceToLast
    Int_t getNewFile();
    Int_t checkDir(TString dir);
    Int_t decodeOption(TString opt);
    static Int_t selectFiles(const struct dirent *entry);
    Int_t loopForNewFile();


public:
  HldGrepFileSource(TString dir="",TString opt="Grep",Int_t grepInterval=5,Int_t refId=-1,Int_t off=5,Int_t fileoffset=1);
  virtual ~HldGrepFileSource(void);
  EDsState getNextEvent(Bool_t doUnpack=kTRUE);
  Int_t getRunId(const Text_t *fileName);
  void setMaxEventPerFile(Int_t n)              {fEventLimit = n; }
  void setRefId(Int_t id=-1)                    {referenceId=id;}
  void stop()                                   {dostop=kTRUE;}
  void addFile(const Text_t *fileName, Int_t refId=-1){Error("addFile()","This function is not implemented!");exit(1);}
  Bool_t getNextFile()                          {Warning("getNextFile()","This function is a dummy implementation!");return kTRUE;}
  Bool_t rewind()                               {Error("rewind()","This function is not implemented!")     ;exit(1);return kTRUE;}

  ClassDef(HldGrepFileSource,0) //! Data source to read LMD files;
};
#endif /* !HLDGREPFILESOURCE_H */

