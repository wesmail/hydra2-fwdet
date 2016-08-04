//*-- Author : Jochen Markert
//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HldGrepFileSource
//
//  This "data source" reads the HLD data from files on disk.
//  The user provides a directory name and the source will grep
//  the newest files from the disk. If after a given check interval
//  a new file is found, the current one will be closed and the reading
//  continues with the new one. The source will run in an endless loop
//  and wait for new files. The user has to stop the source by calling
//  source->stop().
//-----------------------------------------------------------------------------
//  There are several options to setup the source:
//
//  HldGrepFileSource(TString dir,TString opt,Int_t grepInterval,Int_t refId,Int_t off, Int_t fileoffset)
//
//  dir          : directory where the hld files are located
//  opt          : select between the 2 possible modes of the source
//                 1. "Grep" (default) : search for the newest files in intervals
//                    of grepInterval seconds
//                 2. "Date: 02/05/2005 22:00:00" select all files newer than date
//                    and read them sequentional
//                 3. "File: myname.hld" select all files newer than this file and
//                    read them sequentional
//  grepInterval : time interval for checking for new files
//  refId        : -1 (default) reference run for init procedure
//  off          : 5 s (default) selected files have to be older than off seconds
//  fileoffset   :  distance to last file : last - distanceToLast (0 - n , default 1)
//
///////////////////////////////////////////////////////////////////////////////

#include "hldgrepfilesource.h"
#include "hrecevent.h"
#include "hldfilevt.h"
#include "hades.h"
#include "hruntimedb.h"
#include "TSystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <limits.h>
#include <unistd.h>
#include <sstream>

ClassImp(HldGrepFileSource)

time_t HldGrepFileSource::lastfile     =0;     //! last modification time of last used file
time_t HldGrepFileSource::timelimit    =0;     //! time stamp for the search of new files (files should be newer)
time_t HldGrepFileSource::tcurrent     =0;     //! current time stamp
Int_t  HldGrepFileSource::timeoffset   =0;     //! time offset to current time
TString HldGrepFileSource::fullname    ="";    //! full filename: path/file
TString HldGrepFileSource::fullnamesort="";    //! full filename for sorting: path/modtime_file
TString HldGrepFileSource::path        ="";    //! path to directory used for grep of new hld files

HldGrepFileSource::HldGrepFileSource(TString dir,TString opt,Int_t grepInterval,Int_t refId,Int_t off,Int_t fileoffset)
	: HldFileSourceBase()
{
    //  This "data source" reads the HLD data from files on disk.
    //  The user provides a directory name and the source will grep
    //  the newest files from the disk. If after a given check interval
    //  a new file is found, the current one will be closed and the reading
    //  continues with the new one. The source will run in an endless loop
    //  and wait for new files. The user has to exit the program by himself.
    //
    //  dir          : directory where the hld files are located
    //  opt          : select between the 2 possible modes of the source
    //                 1. "Grep" (default) : search for the newest files in intervals
    //                    of grepInterval seconds
    //                 2. "Date: 02/05/2005 22:00:00" select all files newer than date
    //                    and read them sequentional
    //                 3. "File: myname.hld" select all files newer than this file and
    //                    read them sequential
    //  grepInterval : time interval for checking for new files
    //  refId        : -1 (default) reference run for init procedure
    //  fileoffset   :  distance to last file : last - distanceToLast (0 - n , default 1)
    timeoffset=off;
    referenceId=refId;
    fCurrentFile=0;
    first=0;
    dostop=kFALSE;
    distanceToLast=fileoffset;
    if(distanceToLast < 0) distanceToLast = 0;
    //----------------------------------------
    // check input parameters
    if(grepInterval>0){

	interval=grepInterval;
    }
    else
    {
	Error("HldGrepFileSource()","time interval for directory is <=0 !");
        exit(1);
    }
    checkDir(dir);

    decodeOption(opt);
    //----------------------------------------

    // Default constructor for a Lmd file data source.
    fReadEvent=new HldFilEvt;
    iter=0;
    fCurrentFile=0;
    fEventNr = 0;
    fEventLimit = INT_MAX;

    //----------------------------------------
    // looking for first file on the disk
    // can be endless loop! Not very nice
    // in this place...but lets hope the best.
    loopForNewFile();
    //----------------------------------------
}


HldGrepFileSource::~HldGrepFileSource(void)
{
    // Destructor for a LMD file data source
    if (fReadEvent)
    {
	delete fReadEvent;
	fReadEvent = 0;
    }
    if (iter)
    {
	delete iter;
	iter = NULL;
    }
}

Int_t HldGrepFileSource::getRunId(const Text_t *fileName)
{
    // Get the run Id from the file filename
    // In case of failure returns -1
    TString locname=fileName;
    HldFilEvt evt;
    Int_t r=-1;
    if (fCurrentFile && locname.CompareTo(fCurrentFile->GetName())==0) {
	r=fCurrentFile->getRunId();
    } else {
	if (!evt.setFile(fileName)){r=-1;}
	else {
	    evt.read();
	    r=evt.getRunNr();
	}
    }

    return r;
}


EDsState HldGrepFileSource::getNextEvent(Bool_t doUnpack)
{
    // Tries to read a new event from the LMD file and put
    // the info into the HEvent structure.
    // The set directory is checked in interval seconds
    // for new files. If a new file is available
    // it is opened for reading. If the last event of
    // the current file is reached an endless loop
    // for new files on the disk is performed. The
    // loop ends only in case of success!


    //---------------------------------------------
    // check if reading should be stopped
    if(dostop)
    {
	return kDsEndData;
    }
    //---------------------------------------------

    fEventNr++;

    //---------------------------------------------
    // check if the end of file is reached
    // If so, look for new file on disk
    if ( !((HldFilEvt*)fReadEvent)->execute() )
    {
	//End of current file
	fEventNr = 0;
	loopForNewFile();
    }
    //---------------------------------------------

    //---------------------------------------------
    // if the set time interval has ellapsed, check
    // if a new file is on the disk. This is only
    // in "Grep" mode active.
    if(mode==1)
    {
	timer.Stop();
	if(timer.RealTime()>interval)
	{
	    getNewFile();
	}
	else timer.Continue();
    }
    //---------------------------------------------

    //---------------------------------------------
    // finally do the unpacking of the new event
    if(doUnpack)
    {
	decodeHeader((*fEventAddr)->getHeader());

	iter->Reset();
	HldUnpack *unpacker;
	while ( (unpacker=(HldUnpack *)iter->Next())!=NULL)
	{
	    Int_t ret=unpacker->execute();
	    if(!ret) return kDsError;
	    if(ret==kDsSkip) return kDsSkip;
	}
    }
    //---------------------------------------------

    return kDsOk;
}
 
Int_t HldGrepFileSource::loopForNewFile()
{
    // check if a new file is on the disk.
    // performs endless loop until succes!
    // A sleep of the set time interval in seconds
    // is performed before next try.

    Int_t found=0;
    while(found==0)
    {
        // found=0 if no success otherwise =1
	found=getNewFile();
	if(found==0)
	{   // in case of no succes sleep
            // a moment before next try
	    sleep(interval);
	}
    }
    return 0;
}


Int_t HldGrepFileSource::checkDir(TString dir)
{
    // Checks the directory dir. In case of empty
    // strings (default) or not existing directory
    // this function exit the program. No chance
    // to go on for lazy users ...Strategy of breaking
    // as soon as possible. If the directory name does
    // not end with "/" the slash is added

    if(dir.CompareTo("")==0)
    {
	Error("HldGrepFileSource()","String for hld directory is empty!");
	exit(1);
    }
    else
    {
	if(gSystem->AccessPathName(dir))
	{
	    Error("HldGrepFileSource()","Directory : %s does not exist!",dir.Data());
	    exit(1);
	}
        if(!dir.EndsWith("/")) dir+="/";
	path=dir;
    }
    return 0;
}
Int_t HldGrepFileSource::decodeOption(TString opt)
{
    // Decodes the option string. In case of not known
    // option or wrong format this function exit the program.
    // No chance to go on for lazy users ...Strategy of breaking
    // as soon as possible. Depending on the decoded option
    // the mode (=1 for "Grep", =2 for sequential reading) switch
    // is set.
    //  opt  : select between the 2 possible modes of the source
    //         1. "Grep" (default) : search for the newest files in intervals
    //            of grepInterval seconds
    //         2. "Date: 02/05/2005 22:00:00" select all files newer than date
    //             and read them sequentional
    //         3. "File: myname.hld" select all files newer than this file and
    //            read them sequential

    mode=0;
    if(opt.Contains("Date:"))
    {   // Start of the file list which should be read
	// is selected by a give date/time. If the format
        // of the string is wrong the program exits.
	mode=2;

        TString option=opt;
	option.ReplaceAll("Date:","");
	//-----------handling date/time selection-----------------
	option.ReplaceAll("/"," ");
	option.ReplaceAll(":"," ");

	Int_t var[6]={-1};
	Int_t nVar=0;

	stringstream input;
	input.str("");
        input<<option.Data();

	while(input.good()&&nVar<10)
	{
	    if(nVar<6&&input>>var[nVar])
	    {
		nVar++;
	    }
	    else
	    {
		break;
	    }
	}

	if(nVar!=6)
	{
	    Error("HldGrepFileSource()","wrong number of arguments found ! \n "
		  "nVar=%i  => dd/mm/yyyy hh:mm:ss :: %02i/%02i/%i %02i:%02i:%02i  ! \n "
		  "input: %s ", nVar,var[0],var[1],var[2],var[3],var[4],var[5],opt.Data());
	    exit(1);
	}
	else {
	    struct tm time_limit;

	    time_limit.tm_mday=var[0];
	    time_limit.tm_mon =var[1]-1;
	    time_limit.tm_year=var[2]-1900;
	    time_limit.tm_hour=var[3];
	    time_limit.tm_min =var[4];
	    time_limit.tm_sec =var[5];

	    timelimit=mktime(&time_limit);
	    lastfile=timelimit;

	    cout<<"HldGrepFileSource(): You selected to start by DATE :"<<ctime(&timelimit)<<endl;
	}
	//--------------------------------------------------------

    }
    else if(opt.Contains("File:"))
    {
	// Start of the file list which should be read
	// is selected by a file name and therefore
	// by the last modification time of the selected
	// file. If the file does not exit the program exits.
	mode=2;
	opt.ReplaceAll("File:","");
	opt.ReplaceAll(" ","");

	if(gSystem->AccessPathName(Form("%s%s",path.Data(),opt.Data())))
	{
	    Error("HldGrepFileSource()","File does not exist!");
	    exit(1);
	}

	//-----------handling file selection----------------------
	struct stat statloc;
	stat(Form("%s%s",path.Data(),opt.Data()),&statloc);
	timelimit=statloc.st_mtime;
	lastfile=timelimit;

	cout<<"HldGrepFileSource(): You selected to start by DATE from File :"<<ctime(&timelimit)<<endl;
	//--------------------------------------------------------

    }
    else if(opt.Contains("Grep"))
    {
	mode=1;
    }
    else
    {
	Error("HldGrepFileSource()","Unknown option:%s!",opt.Data());
	exit(1);
    }
    return 0;
}
Int_t HldGrepFileSource::selectFiles(const struct dirent *entry)
{
    // This is the select function called with scandir(.....)
    // Selects files going to the list of files for sorting/selection.
    // Returns 1 if a file is selected otherwise 0.
    // Only hld files and files newer or equal than the time stamp
    // lastfile are taken into account. Files smaller 500k are
    // ignored.

    if(entry->d_type==DT_REG ||
       entry->d_type==DT_UNKNOWN)
    {   // only regular files are of interest.
        // Some systems return ONLY unknow!
	TString a=entry->d_name;
	if(!a.EndsWith(".hld")) return 0;

	// only files newer than last read files
        // should go to list. Improves sorting speed!
	struct stat statloc;
	fullname=Form("%s%s",path.Data(),entry->d_name);
	stat(fullname.Data(),&statloc);

	Double_t diff=difftime(tcurrent,statloc.st_mtime);

	if(diff<0)
	{
	    cout<<"HldGrepFileSource::selectFiles(): time difference t_current - t_lastmod < 0 ==> mod time in future!"<<endl;
            return 0;
	}

	if(statloc.st_mtime<lastfile
	   ||statloc.st_size<1024*500
	   ||diff<timeoffset
	  ) return 0;

	return 1;
    }
    else return 0;
}


Int_t HldGrepFileSource::getNewFile()
{
    // Loops on the set directory for new hld files.
    // the dir is scanned with scandir(). The selected
    // files newer or equal time stamp lastfile goes
    // to the list of candidates. The list will be sorted
    // by last modification of the files.
    // In "Grep" mode (mode=1) the second newest file
    // is opened (preventing from reading a file which is
    // in writing process). The time stamp lastfile is set
    // to the selected files last modification time.
    // In "File/Date" mode (mode=2) the list is sequential
    // read.

    struct dirent **namelist;
    Int_t n;
    struct stat status;

    fullname    ="";
    fullnamesort="";
    
    Int_t newFile=0;

    //----------------------------------------
    // get the current time
    time(&tcurrent);
    //----------------------------------------
    n = scandir(path.Data(), &namelist,selectFiles,alphasort);
    if (n < 0)
    {
	perror("scandir");
    }
    else
    {
	TList mylist;

 	//----------------------------------------
	// loop over the file list provided by
	// scandir() Strings with the format path/modtime_filename
	// are added to TList of files for later sorting
        // by last modification time
	while(n--)
	{
	    fullname=Form("%s%s",path.Data(),namelist[n]->d_name);
	    stat(fullname.Data(),&status);

	    fullnamesort=Form("%s%li_%s",path.Data(),(Long_t)status.st_mtime,namelist[n]->d_name);
	    mylist.AddLast(new TObjString(fullnamesort));

	    free(namelist[n]);
	}
	free(namelist);
	//----------------------------------------

	//----------------------------------------
	// The list will be sort by name of the files
	// As the file name contains the last modificaton time
	// they will be sorted by mod time. The newest file
	// will be the last in the list.
	mylist.Sort();
	//----------------------------------------

	//----------------------------------------
	// in mode=1 take the second newest file
	// to sure that the file is closed!
        // otherwise the first file in list
	Int_t index;
	Int_t lastindex=mylist.LastIndex();
	if(mode==1)                   {index=lastindex-distanceToLast;} // grep mode
	else if(mode==2&&lastindex>0) {index=1;}           // reading since a give date/time or file
        else                          {index=-1;}          // default
	//----------------------------------------

	if(index>0)
	{
	    cout<<"\n----------------------------------------------------------------"<<endl;
	    TString myselect=((TObjString*)(mylist.At(index)))->GetString();

	    //----------------------------------------
	    // removing mod time from name
	    TString temp  =myselect;
	    TString base  =gSystem->BaseName(temp.Data());
	    TString parent=temp.Remove(temp.Length()-base.Length());
  	    Ssiz_t last_  = base.First('_');
	    base.Replace(0,last_+1,"");
            myselect      =parent + base;
	    //----------------------------------------

	    //----------------------------------------
	    // getting mod time of file and current time
	    // for print out
	    stat(myselect.Data(),&status);
	    time_t tfile=status.st_mtime;
	    TString a=ctime(&tfile);
	    TString b=ctime(&tcurrent);
	    a.ReplaceAll("\n","");
	    b.ReplaceAll("\n","");
	    //----------------------------------------

	    //----------------------------------------
            // print infos about selected file
	    if(mode==2)
	    {
		cout<<mylist.LastIndex()<<" files in list."<<endl;
	    }
            Int_t diff=(Int_t)difftime(tcurrent,tfile);
	    cout<<myselect.Data()<<" last mod time : "<<diff<<" seconds ago." <<endl;
	    cout<<"mod time: "<<a.Data()<<" now: "<<b.Data()<<endl;
 	    //----------------------------------------


	    //----------------------------------------
	    // set the time stamp to the selected files
	    //last modification time
	    lastfile=tfile;
	    first   =1;  // now we know it is not anlylonger the first try to select a file
            newFile =1;  // file selection was succesful
	    //----------------------------------------


	    //----------------------------------------
	    // delete old filew descriptot, generate a new one
            // and open the new file for reading
	    if(fCurrentFile) delete fCurrentFile;
            fCurrentFile=0;
	    Int_t run=getRunId(myselect.Data());
	    fCurrentFile=new HldFileDesc(myselect.Data(),run,referenceId);
	    ((HldFilEvt *)fReadEvent)->setFile(fCurrentFile->GetName());
	    //----------------------------------------

	    cout<<"----------------------------------------------------------------"<<endl;
	}
	else
	{
	    if(first==0)
	    {   // if this is before the first valid file
		// was found. Otherwise it should be quiet!
		cout<<"----------------------------------------------------------------"<<endl;
		cout<<"No new File on Disk! trying again...."<<endl;
		cout<<"----------------------------------------------------------------"<<endl;
	    }
	    else
	    {   // Short_t notification that a scan of the
		// directory has been done with no result!
		cout <<"."<<flush;
	    }
	}

	//----------------------------------------
	// clean the working list from memory
	mylist.Delete();
 	//----------------------------------------

    }
    // time interval is taken after selection to keep
    // the interval independent of the number of files
    // in dir start resets the timer!
    timer.Start(kTRUE); 
    return newFile;
}





