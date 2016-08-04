#ifndef  __HDISKSPACE_H__
#define  __HDISKSPACE_H__

#include "TSystem.h"
#include "TString.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "TCanvas.h"
#include <iostream>
#include <iomanip>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <vector>
#include <map>
#include <cstring>
#include <sstream>


using namespace std;

class  HDiskCatalog ;

R__EXTERN HDiskCatalog *gHDiskCatalog;


typedef struct stat mystat;

/*
class HTeeBuf: public std::streambuf
{

private:
    std::streambuf * sb1;
    std::streambuf * sb2;

    // This tee buffer has no buffer. So every character "overflows"
    // and can be put directly into the teed buffers.
    virtual int overflow(int c)
    {
        if (c == EOF)
        {
            return !EOF;
        }
        else
        {
            int const r1 = sb1->sputc(c);
            int const r2 = sb2->sputc(c);
            return r1 == EOF || r2 == EOF ? EOF : c;
        }
    }
    
    // Sync both teed buffers.
    virtual int sync()
    {
        int const r1 = sb1->pubsync();
        int const r2 = sb2->pubsync();
        return r1 == 0 && r2 == 0 ? 0 : -1;
    }   



public:
    // Construct a streambuf which tees output to both input
    // streambufs.
    HTeeBuf(std::streambuf * sb1, std::streambuf * sb2)
	: sb1(sb1)
    , sb2(sb2)
    {
    }


};
*/


class HRedirect
{
    // redirects cout to log file
    // redirects ends when destructor is called

private:

    std::ostream*   streamIn;
    std::streambuf* streamInBuffer;

    std::ostream*   streamOut;
    std::streambuf* streamOutBuffer;

public:

    HRedirect(std::ostream* inOriginal, std::ostream* inRedirect)
    {
	streamIn       = inOriginal;
	streamInBuffer = inOriginal->rdbuf();
	streamOut      = inRedirect;
	streamOutBuffer= inRedirect->rdbuf();
	inOriginal->rdbuf(inRedirect->rdbuf());
    }

    ~HRedirect()
    {
       streamIn ->rdbuf(streamInBuffer);
       streamOut->rdbuf(streamOutBuffer);
    }

};




class HFileSys {
private:

    static ULong64_t unitK ;
    static ULong64_t unitM ;
    static ULong64_t unitG ;
    static ULong64_t unitT ;
    static ULong64_t SmallerLimit ; //!
    static map<uid_t,TString> musers;
    static map<gid_t,TString> mgroups;
    static map<uid_t,TString>  initUsers();
    static map<gid_t,TString>  initGroups();

public:
    static ULong64_t           getSmallerLimit() { return SmallerLimit;}
    static map<uid_t,TString>& getUserMap()      { return musers;  }
    static map<gid_t,TString>& getGroupMap()     { return mgroups; }

    static void                setUnit(ULong64_t u) { unitK = u; unitM=u*u; unitG=unitM*u; unitT=unitG*u; }
    static ULong64_t           getUnitK()           { return unitK;}
    static ULong64_t           getUnitT()           { return unitT;}
    static ULong64_t           getUnitG()           { return unitG;}
    static ULong64_t           getUnitM()           { return unitM;}

    static void                getUsers(map<uid_t,TString>& musers);
    static void                getGroups(map<gid_t,TString>& mgroups);
    static void                fillSysInfo();
    static Bool_t              getUser(uid_t uid,TString& name);
    static Bool_t              getGroup(gid_t gid,TString& name);
    static void                getModTime(time_t lastmod,TString& modtime);
    static void                getFileSize(ULong64_t size, TString& out);
    static void                lsFiles(TString directory, vector<TString>& fileList,Bool_t clear=kTRUE,Bool_t fullpath=kFALSE);
    static void                lsFilesRecursive(TString directory, vector<TString>&fullList);
    static void                lsDirectory(TString directory, vector<TString>& fileList);
    static void                lsDirectoryRecursive(TString directory, vector<TString>&fullList);

};

class  HDiskFile : public TNamed {

private:

    ULong64_t size;             // size in byte
    UInt_t    group;
    UInt_t    owner;
    time_t    lastmod;
    UInt_t    dir;
public:

    HDiskFile(TString name="");
    ~HDiskFile();
    ULong64_t getSize()                { return size; }
    UInt_t    getGroup()               { return group ; }
    UInt_t    getOwner()               { return owner; }
    time_t    getLastMod()             { return lastmod;}
    UInt_t    getDir()                 { return dir; }
    void      setSize   (ULong64_t sz) { size  = sz ; }
    void      setGroup  (UInt_t gid)   { group   = gid ; }
    void      setOwner  (UInt_t uid)   { owner   = uid;  }
    void      setLastMod(time_t mod)   { lastmod = mod; }
    void      setDir(UInt_t ind)       { dir =ind; }
    void      print    (Int_t sp=0);
    Bool_t    isToSmall()            { if(size<HFileSys::getSmallerLimit()) return kTRUE; else return kFALSE;}

    ClassDef(HDiskFile,1)
};



class  HDiskDir : public TNamed {


private:

    ULong64_t size;             // size in byte
    UInt_t    group;            // system gid
    UInt_t    owner;            // system uid
    time_t    lastmod;
    UInt_t    nFilesSmallerLimit;
    UInt_t    nFilesSmallerLimitTotal;
    UInt_t    nFilesTotal;
    UInt_t    nEmptyDirs;
    UInt_t    nDirsTotal;
    UInt_t    nEmptyDirsTotal;
    UChar_t   level;
    UInt_t    mother;
    vector<HDiskFile> files;
    vector<Int_t>     dirs;
    void   update         (HDiskDir* mother);
    void   updateRecursive(HDiskDir* daughter,HDiskDir* mother);

public:

    HDiskDir(TString name="");
    ~HDiskDir();
    ULong64_t          getSize()                      { return size; }
    UInt_t             getGroup()                     { return group; }
    UInt_t             getOwner()                     { return owner; }
    time_t             getLastMod()                   { return lastmod;}
    UInt_t             getNFiles()                    { return files.size();}
    UInt_t             getNFilesTotal()               { return nFilesTotal;}
    UInt_t             getNSmallFiles()               { return nFilesSmallerLimit;}
    UInt_t             getNSmallFilesTotal()          { return nFilesSmallerLimitTotal;}
    UInt_t             getNEmptyDirs()                { return nEmptyDirs;}
    UInt_t             getNDirsTotal()                { return nDirsTotal;}
    UInt_t             getNEmptyDirsTotal()           { return nEmptyDirsTotal;}
    UInt_t             getNDirs()                     { return dirs.size();}
    UChar_t            getLevel()                     { return level;}
    UInt_t             getMother()                    { return mother; }
    vector<HDiskFile>& getFiles()                     { return files;}
    vector<Int_t>&     getDirs()                      { return dirs;}

    void               setSize   (ULong64_t sz)       { size = sz; }
    void               setGroup  (UInt_t gid)         { group = gid; }
    void               setOwner  (UInt_t uid)         { owner = uid;  }
    void               setLastMod(time_t mod)         { lastmod = mod; }
    void               setNFilesTotal(UInt_t n)       { nFilesTotal = n;}
    void               setNSmallFiles(UInt_t n)       { nFilesSmallerLimit = n;}
    void               setNSmallFilesTotal(UInt_t n)  { nFilesSmallerLimitTotal = n;}
    void               setNEmptyDirs(UInt_t n)        { nEmptyDirs = n;}
    void               setNDirsTotal(UInt_t n)        { nDirsTotal = n;}
    void               setNEmptyDirsTotal(UInt_t n)   { nEmptyDirsTotal = n;}
    void               setLevel(UChar_t lvl)          { level = lvl;}
    void               setMother(UInt_t index)        { mother = index; }
    void               setDirToFiles(UInt_t index);

    void               addSize( ULong64_t sz)         { size += sz;}
    void               addNFilesTotal(UInt_t n)       { nFilesTotal += n;}
    void               addNSmallFiles(UInt_t n)       { nFilesSmallerLimit += n;}
    void               addNSmallFilesTotal(UInt_t n)  { nFilesSmallerLimitTotal += n;}
    void               addNEmptyDirs(UInt_t n)        { nEmptyDirs += n;}
    void               addNDirsTotal(UInt_t n)        { nDirsTotal += n;}
    void               addNEmptyDirsTotal(UInt_t n)   { nEmptyDirsTotal += n;}
    void               addFile(HDiskFile f)           { files.push_back(f); }
    void               addDir (Int_t index)           { dirs .push_back(index); }
    void               resetDirs()                    { dirs .clear();}

    void               update();
    void               print     (Int_t sp=0, TString base="");
    void               printFiles(Int_t sp=0);
    void               printDirs (Int_t sp=0, TString base="");
    Bool_t             scan(UInt_t indDir);
    Bool_t             scan(mystat& statu, UInt_t index,vector<TString>& dirList);
    Bool_t             isDaughter(HDiskDir* mother);
    Bool_t             isEmpty() { if(files.size()==0&&dirs.size()==0) return kTRUE; else return kFALSE;}
    ClassDef(HDiskDir,1)
};


class  HDiskCatalog : public TNamed {
private:


    TString diskname;
    TObjArray*  list; //->

    map<uid_t,TString> musers;
    map<gid_t,TString> mgroups;

    time_t lastScanStart;
    time_t lastScanStop;
    map<TString,Int_t>     mDirToInd;

    //-------------------------------------------
    // temp helpers for scan
    map<TString,HDiskDir*> mDirOld; //!
    Int_t fnfiles;        //!
    Int_t fnfilestotal;   //!
    Int_t fndirs;         //!
    TObjArray*  flisttmp; //!
    struct stat status ;  //!
    //-------------------------------------------

    static Bool_t cmpName       (HDiskDir*, HDiskDir*);
    static Bool_t cmpSize       (HDiskDir*, HDiskDir*);
    static Bool_t cmpNFiles     (HDiskDir*, HDiskDir*);
    static Bool_t cmpNSmallFiles(HDiskDir*, HDiskDir*);
    static Bool_t cmpNFilesRatio(HDiskDir*, HDiskDir*);
    HDiskDir* getDir(TString name, TObjArray* listtmp,Int_t* ind);
    void      addDir(TString n, Int_t ind)  { if(mDirToInd.find(n) == mDirToInd.end()) mDirToInd[n] = ind;}
    Int_t     getDirMap(TString n)             {
	map<TString,Int_t>::iterator it = mDirToInd.find(n);
	if(it==mDirToInd.end()) return -1;
	else                    return it->second;
    }
    HDiskDir* getDirMapOld(TString n) {
	map<TString,HDiskDir*>::iterator it = mDirOld.find(n);
	if(it==mDirOld.end()) return 0;
	else                  return it->second;
    }
    void loopDirectory         (TString directory, vector<TString>& dirList);
    void loopDirectoryRecursive(TString directory);

public:
    HDiskCatalog(TString name="");

    ~HDiskCatalog ();
    TString              getDiskName()                        { return diskname;}
    TObjArray*           getList()                            { return list;}
    time_t               getLastScanStart()                   { return lastScanStart;}
    time_t               getLastScanStop()                    { return lastScanStop;}
    map<uid_t,TString>&  getUserMap()                         { return musers;}
    map<gid_t,TString>&  getGroupMap()                        { return mgroups;}
    Bool_t               getUser   (uid_t uid,TString& name);
    Bool_t               getGroup  (gid_t gid,TString& name);

    void                 setLastScanStart(time_t t)           { lastScanStart = t;}
    void                 setLastScanStop(time_t t)            { lastScanStop  = t;}
    void                 setUserMap (map<uid_t,TString>& mu)  { musers = mu;}
    void                 setGroupMap(map<gid_t,TString>& mg)  { mgroups = mg;}
    void                 setCurrentCatalog()                  { gHDiskCatalog = this;}
    Bool_t               scan();
    Int_t                addDir(HDiskDir* dir);
    void                 updateDirIndices();


    HDiskDir*            getDir                  (TString name,Int_t* ind=0);
    void                 getDaughterDirs         (HDiskDir* dir, vector<HDiskDir*>& daughters);
    void                 getDaughterDirsRecursive(HDiskDir* dir, vector<HDiskDir*>& daughters);
    void                 sortDirs                (HDiskDir* dir, vector<HDiskDir*>& daughters,TString option);
    void                 sortDirsRecursive       (HDiskDir* dir, vector<HDiskDir*>& daughters,TString option);
    void                 print                   (UChar_t maxlevel=255);
    void                 printDisk               (UChar_t maxlevel=255,TString option="size",Int_t nfill=0,TString filler=" ");
    void                 printExecution();
    UInt_t               filterDirs              (TString regexp,vector<HDiskDir*>& dirs,TString range1S="",TString range2S="",Long64_t size=0);
    UInt_t               filterFiles             (TString regexpdir,TString regexpfile,vector<HDiskFile*>& files,TString range1S="",TString range2S="",Long64_t size=0);
    void                 printDirs               (TString regexp,TString range1S="",TString range2S="",Long64_t size=0);
    void                 printFiles              (TString regexpdir,TString regexpfile,TString range1S="",TString range2S="",Long64_t size=0);
    ClassDef(HDiskCatalog,1)
};


class HDiskStat : public TNamed {
private:

    map<TString,vector<TGraph> > mDirToVal;
    vector<Int_t> vcolors;
    vector<Int_t> vmarkers;
    vector<Int_t> vstyles;
    Int_t evalOpt(TString opt);
    ULong64_t unit;
    TString diskname;

public:
    HDiskStat(TString name="");
    ~HDiskStat();

    void            setDiskName(TString name) { diskname = name;}
    void            setUnit(ULong64_t u)      {unit = u;}

    TString         getDiskName()  {return diskname;}
    TGraph*         getDir         (TString dirname,TString opt="size");
    vector<TGraph>* getDirVec      (TString dirname);
    void            addEntry       (time_t scanstart,HDiskDir* dir);
    Bool_t          findMinMaxGraph(Double_t& xmin,Double_t& xmax,Double_t& ymin,Double_t& ymax,TGraph* g);
    Bool_t          findMinMaxAll  (Double_t& xmin,Double_t& xmax,Double_t& ymin,Double_t& ymax,TString opt);
    Bool_t          findMinMaxAll  (Double_t& xmin,Double_t& xmax,Double_t& ymin,Double_t& ymax,vector<TGraph*>& vg);
    TCanvas*        draw(TString opt,UInt_t lastDays,ULong64_t u,TString select = "daughters");
    ClassDef(HDiskStat,1)
};


#endif /* !__HDISKSPACE_H__ */



