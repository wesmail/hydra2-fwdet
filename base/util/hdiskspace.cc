#include "hdiskspace.h"

#include "TRegexp.h"
#include "TDatime.h"
#include "TStyle.h"
#include "TH2D.h"
#include "TLegend.h"

#include <algorithm>
#include <ctype.h>

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HDiskFile,HDiskDir,HDiskCatalog,HFileSys
//
// tool set to build a catalog of a full filesystem including all
// meta information of files and dirs. The catalog can be updated
// incremental by rescanning only dirs which have been modfied since
// last scan.
//
////////////////////////////////////////////////////////////////////////////////

ClassImp(HFileSys)
ClassImp(HDiskFile)
ClassImp(HDiskDir)
ClassImp(HDiskCatalog)
ClassImp(HDiskStat)


HDiskCatalog* gHDiskCatalog;
ULong64_t          HFileSys::unitK        = 1024L;
ULong64_t          HFileSys::unitM        = 1024L*1024L;
ULong64_t          HFileSys::unitG        = 1024L*1024L*1024L;
ULong64_t          HFileSys::unitT        = 1024L*1024L*1024L*1024L;
ULong64_t          HFileSys::SmallerLimit = 1024L*1024L;   // 1Mb

map<uid_t,TString> HFileSys::musers  =  initUsers();
map<uid_t,TString> HFileSys::mgroups =  initGroups();

void HFileSys::getUsers(map<uid_t,TString>& musers)
{
    // gather all user known by the system
    musers.clear();
    setpwent();
    struct passwd* entry = 0;

    while( (entry = getpwent() ) != 0){
	if(musers.find(entry->pw_uid) == musers.end()) musers[entry->pw_uid] = entry->pw_name;    }
    endpwent();
}

void HFileSys::getGroups(map<gid_t,TString>& mgroups)
{
    // gather all groups known by the system
    mgroups.clear();
    setgrent();
    struct group* entry =0;

    while( (entry = getgrent() ) != 0){
	if(mgroups.find(entry->gr_gid) == mgroups.end()) mgroups[entry->gr_gid] = entry->gr_name;
    }
    endgrent();
}

map<uid_t,TString> HFileSys::initUsers()
{
    // helper function to init static maps
    map<uid_t,TString> m;
    return m;
}

map<gid_t,TString> HFileSys::initGroups()
{
    // helper function to init static maps
    map<gid_t,TString> m;
    return m;
}

void HFileSys::fillSysInfo()
{
    // gather user and group infos from the
    // system
    getUsers (musers);
    getGroups(mgroups);
}

Bool_t HFileSys::getUser(uid_t uid,TString& name)
{
    // lookup of user uid in the map of
    // known users. if the user can not be
    // found the the user name will be unknown_uid
    name = Form("unknown_%i",uid);
    map<uid_t,TString>::iterator it = musers.find(uid);
    if(it!=musers.end()) { name = it->second; return kTRUE;}
    return kFALSE;
}

Bool_t HFileSys::getGroup(gid_t gid,TString& name)
{
    // lookup of group gid in the map of
    // known groups. if the group can not be
    // found the the group name will be unknown_gid
    name = Form("unknown_%i",gid);
    map<gid_t,TString>::iterator it = mgroups.find(gid);
    if(it!=mgroups.end()) { name = it->second; return kTRUE;}
    return kFALSE;
}

void HFileSys::getModTime(time_t lastmod,TString& modtime)
{
    // translates time_t lastmod into string
    // witrhout trailing \n
    modtime = ctime(&lastmod);
    modtime.ReplaceAll('\n',"");
}

void HFileSys::getFileSize(ULong64_t size, TString& out)
{
    // returns human readable file size (K,M,G,T units)
    out="";
    Float_t c = unitK;   // kB
    TString s = "K";

    if (size>unitM && size <= unitG){
	c=unitM;
        s = "M";
    } else if (size>unitG){
	c=unitG;
	s="G";
	if(size>unitT){
	    c=unitT;
	    s="T";
	}
    }
    Float_t val=size/c;
    if     (s=="K")   out= Form("%9.0fK",val);
    else if(s=="M")   out= Form("%9.3fM",val);
    else if(s=="G")   out= Form("%9.3fG",val);
    else              out= Form("%9.3fT",val);
}

void HFileSys::lsFiles(TString directory, vector<TString>& fileList,Bool_t clear,Bool_t fullpath)
{
    // fill all file names of directory to vector.
    // uses readdir()
    if(clear)fileList.clear();
    DIR *dir;
    struct dirent *ent = NULL;
    if ((dir=opendir(directory.Data())) != NULL)   // open a directory
    {
	while((ent=readdir(dir)) != NULL) { // loop until last entry
	    if(ent->d_type == DT_REG) {
		if(!fullpath)fileList.push_back(ent->d_name); // push filename to the list
                else         fileList.push_back(Form("%s/%s",directory.Data(),ent->d_name));
	    }
	}
	closedir(dir);
    }
}

void  HFileSys::lsFilesRecursive(TString directory, vector<TString>&fullList)
{
    // fill all dir names of directory rekursively to vector.
    // uses readdir()

    if(directory.EndsWith("/")) directory.Replace(directory.Length()-1,1,"");
    vector<TString> fileList;
    lsDirectory(directory,fileList);  // get the "root" directory's directories

    lsFiles(directory,fullList,kFALSE,kTRUE);

    for(vector<TString>::iterator i = fileList.begin(); i != fileList.end(); ++i) // loop thru the list
    {
	if (strcmp((*i).Data(), ".") &&     // skip . and ..
	    strcmp((*i).Data(), ".."))
	{
	    stringstream fullname;
	    fullname << directory << "/" << (*i);
	    lsFilesRecursive(fullname.str(), fullList);
	}
    }
}

void HFileSys::lsDirectory(TString directory, vector<TString>& fileList)
{
    // fill all dir names of directory to vector.
    // uses readdir()
    fileList.clear();
    DIR *dir;
    struct dirent *ent=NULL;
    if ((dir=opendir(directory.Data())) != NULL) // open a directory
    {
	while((ent=readdir(dir)) != NULL) { // loop until last entry
	    if (strcmp(ent->d_name, ".") &&     // skip . and ..
		strcmp(ent->d_name, ".."))
	    {
		if(ent->d_type == DT_DIR)fileList.push_back(ent->d_name); // push directory to the list
	    }
	}
	closedir(dir); // close up
    }
}

void  HFileSys::lsDirectoryRecursive(TString directory, vector<TString>&fullList)
{
    // fill all dir names of directory rekursively to vector.
    // uses readdir()
    vector<TString> fileList;
    lsDirectory(directory,fileList);  // get the "root" directory's directories

    for(vector<TString>::iterator i = fileList.begin(); i != fileList.end(); ++i) // loop thru the list
    {
	if (strcmp((*i).Data(), ".") &&     // skip . and ..
	    strcmp((*i).Data(), ".."))
	{
	    stringstream fullname;
	    fullname << directory << "/" << (*i);
	    fullList.push_back(fullname.str());
	    lsDirectoryRecursive(fullname.str(), fullList);
	}
    }
}


//----------------------------------------------------------------------------



HDiskFile::HDiskFile(TString name)
{
    SetName(name.Data());
    size   = 0;
    group  = 0;
    owner  = 0;
    lastmod= 0;
}

HDiskFile::~HDiskFile()
{

}
void HDiskFile::print(Int_t sp)
{
    // prints the file info. if sp!=0
    // the info will be printed with sp leading spaces.
    TString modtime ;
    TString sowner;
    TString sgroup;
    TString fsize;
    HFileSys::getModTime (lastmod,modtime);
    HFileSys::getUser    (owner,sowner);
    HFileSys::getGroup   (group,sgroup);
    HFileSys::getFileSize(size,fsize);
    if(sp==0){
	cout<<setw(8)                     <<fsize.Data()
	    <<" "<<modtime.Data()
	    <<" "<<setw(15)<<sowner
	    <<" "<<setw(15)<<sgroup
	    <<" "<<GetName()
	    <<endl;
    } else {
	cout<<setw(8)<<std::string(sp,' ')<<fsize.Data()
	    <<" "<<modtime.Data()
	    <<" "<<setw(15)<<sowner
	    <<" "<<setw(15)<<sgroup
	    <<" "<<GetName()
	    <<endl;
    }
}

//----------------------------------------------------------------------------

HDiskDir::HDiskDir(TString name)
{
    SetName(name.Data());
    size                    = 0;
    group                   = 0;
    owner                   = 0;
    lastmod                 = 0;
    nFilesSmallerLimit      = 0;
    nFilesSmallerLimitTotal = 0;
    nFilesTotal             = 0;
    level                   = 0;
    nEmptyDirs              = 0;
    nDirsTotal              = 0;
    nEmptyDirsTotal         = 0;
    //files.reserve(100);
    if(gHDiskCatalog) {
	name.ReplaceAll(gHDiskCatalog->getDiskName().Data(),"");
	level = name.CountChar('/');
    }
}
HDiskDir::~HDiskDir()
{
}
void HDiskDir::update()
{
    // update the summray info of this
    // directopry rekursively
    nFilesSmallerLimitTotal = 0;
    nFilesTotal             = 0;
    size                    = 0;
    nEmptyDirsTotal         = 0;
    nDirsTotal              = 0;

    updateRecursive(this,this);
}

void HDiskDir::update(HDiskDir* mother)
{
    // add stats of dauhgter to stats of mother
    mother->addNSmallFilesTotal(nFilesSmallerLimit);
    mother->addNFilesTotal(files.size());
    mother->addNDirsTotal(dirs.size());
    mother->addNEmptyDirsTotal(nEmptyDirs);

    for(UInt_t i=0;i<files.size(); ++i){ // loop thru the list
       mother->addSize(files[i].getSize());
    }
}

void  HDiskDir::updateRecursive(HDiskDir* daughter,HDiskDir* mother)
{
    // fill stats of subdirs rekursively to mother
    if(!gHDiskCatalog) {
	cout<<"ERROR: HDiskDir::updateRecursive() : gHDiskCatatlog==NULL! "<<endl;
	return ;
    }
    daughter->update(mother);

    for(UInt_t i=0;i<daughter->getDirs().size(); ++i) // loop thru the list
    {
	HDiskDir* d = (HDiskDir*)gHDiskCatalog->getList()->At(daughter->getDirs()[i]);
	if(!d) {
	    cout<<"ERROR: HDiskDir::updateRecursive() : index wrong, retrived NULL! "<<endl;
	    return ;
	}
	updateRecursive(d,mother);
    }
}



void HDiskDir::print(Int_t sp, TString base)
{
    // prints the dir info. if sp!=0
    // the info will be printed with sp leading spaces.
    // if base!="" base will be replaced in dirname by ""
    TString modtime ;
    TString sowner;
    TString sgroup;
    TString fsize;
    TString name = GetName();
    if(base != "") name.ReplaceAll(base.Data(),"");
    HFileSys::getModTime (lastmod,modtime);
    HFileSys::getUser    (owner,sowner);
    HFileSys::getGroup   (group,sgroup);
    HFileSys::getFileSize(size,fsize) ;

    if(sp==0){
	cout                     <<setw(8)<<fsize.Data()
	    <<" "<<modtime.Data()
	    <<" "<<setw(12)<<sowner
	    <<" "<<setw(10)<<sgroup
	    <<" "<<setw(9)<<nFilesTotal
	    <<" "<<setw(9)<<nFilesSmallerLimitTotal
	    <<" "<<setw(9)<<nDirsTotal
	    <<" "<<setw(9)<<nEmptyDirsTotal
	    <<" "<<name
	    <<endl;
    } else {
	cout<<std::string(sp,'-')<<setw(8)<<fsize.Data()
	    <<" "<<modtime.Data()
	    <<" "<<setw(12)<<sowner
	    <<" "<<setw(10)<<sgroup
	    <<" "<<setw(9)<<nFilesTotal
	    <<" "<<setw(9)<<nFilesSmallerLimitTotal
	    <<" "<<setw(9)<<nDirsTotal
	    <<" "<<setw(9)<<nEmptyDirsTotal
	    <<" "<<name
	    <<endl;
    }
}

void HDiskDir::printFiles(Int_t sp)
{
    // prints all file infos of the dir
    for(UInt_t i = 0; i < files.size(); i++){
	files[i].print(sp);
    }
}

void HDiskDir::printDirs(Int_t sp, TString base)
{
    // prints all subdirs of dir
    if(!gHDiskCatalog) {
	cout<<"ERROR: HDiskDir::printDirs() : gHDiskCatatlog==NULL! "<<endl;
	return ;
    }

    for(UInt_t i = 0; i < dirs.size(); i++){
	HDiskDir* d = (HDiskDir*)gHDiskCatalog->getList()->At(dirs[i]);
	if(!d) {
	    cout<<"ERROR: HDiskDir::printDirs() : index wrong, retrived NULL! "<<endl;
	    continue ;
	}
	d->print(sp,base);
    }
}

Bool_t HDiskDir::scan(UInt_t index)
{
    // Scans the files inside the directory
    // and gather the file size and the number
    // of files smaller than the limit

    Bool_t problem = kFALSE;
    files.clear();
    struct stat status;
    if(stat(GetName(),&status) == 0)
    {
	lastmod            = status.st_mtime;
	owner              = status.st_uid;
	group              = status.st_gid;
	size               = status.st_size;
	nFilesSmallerLimit = 0;

	vector<TString> fileList;
	HFileSys::lsFiles(GetName(),fileList);
	TString fullname;
	for(UInt_t i = 0; i < fileList.size(); i++){
	    fullname = Form("%s/%s",GetName() ,fileList[i].Data());
	    if(stat(fullname.Data() ,&status) == 0)
	    {
		HDiskFile newfile(fileList[i]);
		newfile.setSize   (status.st_size);
		newfile.setGroup  (status.st_gid);
		newfile.setOwner  (status.st_uid);
		newfile.setLastMod(status.st_mtime);
                newfile.setDir(index);
		if(newfile.isToSmall()) nFilesSmallerLimit++;
		files.push_back(newfile);

	    } else {
		cout<<"ERROR : HDiskDir:scan() : Could not stat file = "<<fileList[i].Data()<<endl;
		problem=kTRUE;
	    }
	}

    } else {
	cout<<"ERROR : HDiskDir:scan() : Could not stat dir = "<<GetName()<<endl;
	problem = kTRUE;
    }
    return problem;
}

Bool_t HDiskDir::scan(struct stat& status, UInt_t index,vector<TString>& dirList)
{
    // Scans the files inside the directory
    // and gather the file size and the number
    // of files smaller than the limit

    Bool_t problem = kFALSE;
    files.clear();
    dirList.clear();

    lastmod            = status.st_mtime;
    owner              = status.st_uid;
    group              = status.st_gid;
    size               = status.st_size;
    nFilesSmallerLimit = 0;

    TString fullname;
    DIR *dir;
    struct dirent *ent = NULL;

    if ((dir=opendir(GetName())) != NULL)   // open a directory
    {
	while((ent=readdir(dir)) != NULL) { // loop until last entry
	    if(ent->d_type == DT_DIR) {
		if (strcmp(ent->d_name, ".") &&     // skip . and ..
		    strcmp(ent->d_name, "..")){
		    dirList.push_back(Form("%s/%s",GetName() ,ent->d_name)); // push filename to the list
		}
	    }
	    else if(ent->d_type == DT_REG)
	    {
		fullname = Form("%s/%s",GetName() ,ent->d_name);
		if(stat(fullname.Data() ,&status) == 0)
		{
		    HDiskFile newfile(ent->d_name);
		    newfile.setSize   (status.st_size);
		    newfile.setGroup  (status.st_gid);
		    newfile.setOwner  (status.st_uid);
		    newfile.setLastMod(status.st_mtime);
		    newfile.setDir(index);
		    if(newfile.isToSmall()) nFilesSmallerLimit++;
		    files.push_back(newfile);

		} else {
		    cout<<"ERROR : HDiskDir:scan() : Could not stat file = "<<ent->d_name<<endl;
		    problem=kTRUE;
		}
	    }
	}
	closedir(dir);
    }
    return problem;
}

void  HDiskDir::setDirToFiles(UInt_t index)
{
    for(UInt_t i = 0; i < files.size(); i++){
         files[i].setDir(index);
    }
}

Bool_t HDiskDir::isDaughter(HDiskDir* mother)
{
    // returns kTRUE if this dir is a daughter of mother
    // evaluation is done by path name.
    if(!mother) return kFALSE;
    if(strncmp(GetName(),mother->GetName(),strlen(mother->GetName())) == 0) return kTRUE;
    else return kFALSE;

}

//----------------------------------------------------------------------------


HDiskCatalog::HDiskCatalog(TString name)
{
    diskname=name;
    if(diskname.EndsWith("/")) {
	diskname.Replace(diskname.Length()-1,1,"");
    }

    list = new TObjArray();

    HFileSys::fillSysInfo();
    musers  = HFileSys::getUserMap();
    mgroups = HFileSys::getGroupMap();

    setCurrentCatalog();
}

HDiskCatalog::~HDiskCatalog()
{

    for(Int_t i=0;i<list->GetEntries();i++){
	HDiskDir* d = (HDiskDir*)list->At(i);
	delete d;
    }

    delete list;
    list=0;
}

Bool_t HDiskCatalog::cmpName(HDiskDir* a,HDiskDir* b)
{
    // compare function for sorting dirs alphabetically by
    // owner name
    TString usera;  HFileSys::getUser((uid_t)a->getOwner(),usera);
    TString userb;  HFileSys::getUser((uid_t)b->getOwner(),userb);

    string first (usera.Data());
    string second(userb.Data());

    UInt_t i = 0;
    while ( (i<first.length()) && (i<second.length()) )
    {
	if      (tolower(first[i]) < tolower(second[i])) return kTRUE;
	else if (tolower(first[i]) > tolower(second[i])) return kFALSE;
	++i;
    }
    return ( first.length() < second.length() );
}

Bool_t HDiskCatalog::cmpSize(HDiskDir* a,HDiskDir* b)
{
    // compare function for sorting dirs by size descending
    return ( a->getSize() > b->getSize() );
}

Bool_t HDiskCatalog::cmpNFiles(HDiskDir* a,HDiskDir* b)
{
    // compare function for sorting dirs by number of files descending
    return ( a->getNFilesTotal() > b->getNFilesTotal() );
}

Bool_t HDiskCatalog::cmpNSmallFiles(HDiskDir* a,HDiskDir* b)
{
    // compare function for sorting dirs by number of small files descending
    return ( a->getNSmallFilesTotal() > b->getNSmallFilesTotal() );
}

Bool_t HDiskCatalog::cmpNFilesRatio(HDiskDir* a,HDiskDir* b)
{
    // compare function for sorting dirs by number of small files descending
    Float_t ratioa = 0;
    Float_t ratiob = 0;
    if(a->getNFilesTotal()>0) ratioa= a->getNSmallFilesTotal()/(Float_t)a->getNFilesTotal();
    if(b->getNFilesTotal()>0) ratiob= b->getNSmallFilesTotal()/(Float_t)b->getNFilesTotal();

    return ( ratioa > ratiob );
}


HDiskDir* HDiskCatalog::getDir(TString name, Int_t* ind)
{
    // returns HDiskDir* p from index by name.
    // pointer is NULL if not found
    Int_t i = getDirMap(name);
    if(i>-1){
	if(ind!=0) *ind = i ;
	return (HDiskDir*) list->At(i);

    } else return NULL;
}

HDiskDir* HDiskCatalog::getDir(TString name,TObjArray* listtmp,Int_t* ind)
{
    // returns HDiskDir* p from index by name.
    // pointer is NULL if not found
    for(Int_t i = 0; i < listtmp->GetEntries(); i++){
	HDiskDir* dir = (HDiskDir*) listtmp->At(i);
	if(strcmp(dir->GetName(),name.Data()) == 0 ) {
	    if(ind!=0) *ind = i ;
	    return dir;
	}
    }
    return NULL;
}


void HDiskCatalog::print(UChar_t maxlevel)
{
    // prints all dirs (up to maxlevel, default is all)
    for(Int_t i=0; i< list->GetEntries();i++){
	HDiskDir* dir = ((HDiskDir*)list->At(i));
        if(dir->getLevel() > maxlevel) continue;
	dir->print();
    }
    printExecution();
}




void HDiskCatalog::loopDirectory(TString directory, vector<TString>& dirList)
{
    // fill all dir names of directory to vector.
    // uses readdir()
    dirList.clear();
    if(stat(directory.Data(),&status)==0)
    {
	HDiskDir* mydir  = 0;
	HDiskDir* olddir = getDirMapOld(directory.Data());
	Bool_t changed = kFALSE;

	if(olddir && olddir->getLastMod() != status.st_mtime) changed = kTRUE;
        Int_t index = -1;
	//-----------------------------------------------------------------
	// found a new dir to scan
	if(changed || !olddir) {
	    mydir = new HDiskDir(directory.Data());
	    flisttmp->AddLast(mydir);
	    index = flisttmp->GetEntries() -1;
	    mydir->scan(status,index,dirList);

	    cout<<"new : "<<setw(8)<<mydir->getNFiles()<<" files "<<mydir->GetName()<<endl;
	    fnfiles      += mydir->getNFiles();
	    fnfilestotal += mydir->getNFiles();
	    fndirs       ++;
	    addDir(mydir->GetName(),index);
	}
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	// found an old unchanged dir
	if (olddir && !changed){   // copy old directory
	    mydir  = new HDiskDir(*olddir);
	    flisttmp->AddLast(mydir);
            index = flisttmp->GetEntries() -1;

	    mydir->setDirToFiles(index);

	    vector<Int_t>& dirs = mydir->getDirs();
	    for(UInt_t i = 0; i < dirs.size(); i++){
		HDiskDir* d = (HDiskDir*)list->At(dirs[i]);
                if(d) dirList.push_back(d->GetName());
                else  cout<<"ERROR : HDiskCatalog:loopDirectory() : Could not dir from old index "<<mydir->GetName()<<endl;
	    }
	    mydir->resetDirs(); // clear old indices

	    // missing recurse set ind files
	    fnfiles      += mydir->getNFiles();
	    fnfilestotal += mydir->getNFiles();
	    fndirs       ++;
	    addDir(mydir->GetName(),index);
	}
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	// add dir to list of dirs of the mother dir

	HDiskDir* motherdir = 0 ;
	TString mothername  = gSystem->DirName(mydir->GetName());
	Int_t ind = getDirMap(mothername);
	if(ind >-1){
	    motherdir = (HDiskDir*)flisttmp->At(ind);
	}
	if(motherdir) {
	    motherdir->addDir(index);
	    mydir    ->setMother(ind);
	} else {
	    cout<<"ERROR : HDiskCatalog:loopDirectory() : Could not find mother dir of "<<mydir->GetName()<<" ("<<mothername<<" ?)"<<endl;
	}
	//-----------------------------------------------------------------
    } else {
	// TODO :  set timestamp of mother dir = 0 to froce rescan next time!

	cout<<"ERROR : HDiskCatalog:loopDirectory() : Could not stat dir "<<directory<<endl;

    }
}

void  HDiskCatalog::loopDirectoryRecursive(TString directory)
{
    // fill all dir names of directory rekursively to vector.
    // uses readdir()
    vector<TString> dirList;
    loopDirectory(directory,dirList);  // get the "root" directory's directories

    for(UInt_t i = 0; i < dirList.size(); ++i) // loop thru the list
    {
	loopDirectoryRecursive(dirList[i]);
    }
}

Bool_t HDiskCatalog::scan()
{
    // scans the disk and rescans dirs
    // which have been modified since last scan
    lastScanStart = time(&lastScanStart);
    TString mytime;
    HFileSys::getModTime(lastScanStart,mytime);
    cout<<"HDiskCatalog:scan() : disk is "<<diskname<<endl;
    cout<<"HDiskCatalog:scan() : start scannning  at "<<mytime<<endl;

    setCurrentCatalog();
    flisttmp = new TObjArray() ;
    mDirToInd.clear();

    //-----------------------------------------------------------------
    // for faster access remap old catalog
    mDirOld.clear();
    for(Int_t i = 0; i < list->GetEntries(); i++){
	HDiskDir* dir = (HDiskDir*)list->At(i);
        mDirOld[dir->GetName()] = dir;
    }
    //-----------------------------------------------------------------

    vector<TString>listofentries;
    vector<TString>listsubdirs;

    Int_t index        = -1;
    fnfilestotal       =  0;
    HDiskDir* base     =  0;

    //-----------------------------------------------------------------
    // create base dir
    if(stat(diskname.Data(),&status) == 0)
    {
	base = new HDiskDir(diskname.Data());
	flisttmp->AddLast(base);
	index = flisttmp->GetEntries()-1;
	base->scan(index);
	fnfilestotal += base->getNFiles();
        addDir(base->GetName(),index);

    } else {
	cout<<"ERROR : HDiskCatalog:scan() : Could not stat dir = "<<diskname.Data()<<endl;
        return kFALSE;
    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // get all 1st level subdirs of base
    HFileSys::lsDirectory(diskname,listsubdirs);

    for(UInt_t j = 0; j < listsubdirs.size(); j++)
    {
	fnfiles= 0;
        fndirs = 0;
	TString actdir=Form("%s/%s",diskname.Data(),listsubdirs[j].Data());

	//-----------------------------------------------------------------
        // create 1st level subdirs
	if(stat(actdir.Data(),&status) == 0)
	{
	    HDiskDir* level1 = new HDiskDir(actdir);
	    flisttmp->AddLast(level1);
	    index = flisttmp->GetEntries()-1;
            level1->scan(index);
	    base  ->addDir(index);
	    level1->setMother(0);
	    fnfilestotal +=level1->getNFiles();
	    fnfiles      +=level1->getNFiles();
	    fndirs      ++;
	    addDir(level1->GetName(),index);
 	} else {
	    cout<<"ERROR : HDiskCatalog:scan() : Could not stat dir = "<<diskname.Data()<<endl;
	    return kFALSE;
	}
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
        // now check all subdirs of each 1st level dir

	vector<TString>listofentries;
	HFileSys::lsDirectory(actdir, listofentries);
	cout<<"Scanning list of directories for "<<actdir<<endl;
	for(UInt_t i = 0; i < listofentries.size(); i++)
	{
	    loopDirectoryRecursive(Form("%s/%s",actdir.Data(),listofentries[i].Data()));
	}
	cout<<"Scanned "<<setw(8)<<fndirs<<" directories with "<<setw(8)<<fnfiles<<" files"<<endl;
    } // end loop level1 dirs
    //-----------------------------------------------------------------

    cout<<"Total Scanned "<<setw(8)<<index<<" directories with "<<setw(8)<<fnfilestotal<<" files"<<endl;

    //-----------------------------------------------------------------
    // exchange catalog
    for(Int_t i = 0; i < list->GetEntries(); i++){
	HDiskDir* dir = (HDiskDir*)list->At(i);
	delete dir;
    }
    delete list;

    list = flisttmp;
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // update the summary infos rekursively for each dir
    // and its subdirs
    cout<<"Updating list of directories "<<endl;
    for(Int_t i = 0; i < list->GetEntries(); i++){  // calulate sums of subdirs
	HDiskDir* dir = (HDiskDir*)list->At(i);
	vector<Int_t>& dirs = dir->getDirs();
        UInt_t n = 0;
	for(UInt_t j = 0;j < dirs.size(); j++){
	    if( ((HDiskDir*)list->At(dirs[j]))->isEmpty()) n++;
	}
        dir->setNEmptyDirs(n);
    }

    for(Int_t i = 0; i < list->GetEntries(); i++){  // calulate sums of subdirs
	HDiskDir* dir = (HDiskDir*)list->At(i);
	dir->update();
    }
    //-----------------------------------------------------------------


    lastScanStop = time(&lastScanStop);

    printExecution();
    return kTRUE;
}


void  HDiskCatalog::updateDirIndices()
{
    // set indices of daughter dirs
    // does not change nFilesTotal etc.
    TString mothername;
    Int_t indexMoth;
    for(Int_t i = 0; i < list->GetEntries(); i++){
	HDiskDir* dir = (HDiskDir*)list->At(i);
	mothername = gSystem->DirName(dir->GetName());
	HDiskDir* dmother =  getDir(mothername,&indexMoth);
        if(dmother) dmother->addDir(i);
    }


}


Int_t HDiskCatalog::addDir(HDiskDir* dir)
{
    // use this to produce super catalogs (new empty base,
    // than add dirs, last call updateDirIndices())
    Int_t   index        = -1;
    Int_t   indexMoth    = -1;
    TString  mothname = gSystem->DirName(dir->GetName());
    HDiskDir* dmother =  getDir(mothname,&indexMoth);

    HDiskDir* mydir  = new HDiskDir(*dir);
    list->AddLast(mydir);
    index = list->GetEntries()-1;
    mydir->setDirToFiles(index);     // correct index for file -> dir
    mydir->setLevel(mydir->getLevel()+1);
    addDir(mydir->GetName(),index);  // to map
    mydir->getDirs().clear();        // will be set in updateDirIndices()

    if(dmother)
    {
	if(indexMoth == 0)
	{
	    // adding summary infos for base
	    // all other levels are assumed to be correct
	    // already
	    dmother->addSize(mydir->getSize());
	    dmother->addNFilesTotal     (dir->getNFilesTotal());
	    dmother->addNSmallFilesTotal(dir->getNSmallFilesTotal());
	    dmother->addNDirsTotal      (dir->getNDirsTotal()+1);  // plus self
	    dmother->addNEmptyDirsTotal (dir->getNEmptyDirsTotal());
	}
    }
    return index;
}

void HDiskCatalog::getDaughterDirs(HDiskDir* dir, vector<HDiskDir*>& daughters)
{
    // get all daughters of dir by index
    daughters.clear();
    for(UInt_t i = 0; i < dir->getNDirs(); i++){
	HDiskDir* d = (HDiskDir*)list->At(dir->getDirs()[i]);
	if(d) {
	    daughters.push_back(d);
	}
    }
}

void HDiskCatalog::getDaughterDirsRecursive(HDiskDir* dir, vector<HDiskDir*>& list)
{
    // get all daughters of dir rekursively by index
    vector<HDiskDir*> daughters;
    getDaughterDirs(dir,daughters);

    for(UInt_t i = 0; i < daughters.size(); i++)
    {
	HDiskDir* d = daughters[i];
        list.push_back(d);
	getDaughterDirsRecursive(d,list);
    }
}

void HDiskCatalog::sortDirs(HDiskDir* dir,vector<HDiskDir*>& daughters,TString option)
{
    // sort daughters of dir by option
    // option: name        (name of owner)
    //         size        (size of directory)
    //         nfiles      (number of files in directory (rekursively))
    //         nsmallfiles (number of small files in directory(rekursively))
    //         filesratio  (number of small files in directory / number of total files)
    getDaughterDirs(dir,daughters);
    if(daughters.size()==0) return;
    if(option=="name")        { sort(daughters.begin(),daughters.end(),cmpName);       }
    if(option=="size")        { sort(daughters.begin(),daughters.end(),cmpSize);       }
    if(option=="nfiles")      { sort(daughters.begin(),daughters.end(),cmpNFiles);     }
    if(option=="nsmallfiles") { sort(daughters.begin(),daughters.end(),cmpNSmallFiles);}
    if(option=="filesratio")  { sort(daughters.begin(),daughters.end(),cmpNFilesRatio);}

}

void HDiskCatalog::sortDirsRecursive(HDiskDir* dir, vector<HDiskDir*>& dlist,TString option)
{
    // sort daughters of dir rekursively by option
    // option: name        (name of owner)
    //         size        (size of directory)
    //         nfiles      (number of files in directory (rekursively))
    //         nsmallfiles (number of small files in directory(rekursively))
    //         filesratio  (number of small files in directory / number of total files)
    vector<HDiskDir*> daughters;
    sortDirs(dir,daughters,option);

    for(UInt_t i = 0; i < daughters.size(); i ++) {
	HDiskDir* d = daughters[i];
	dlist.push_back(d);
        sortDirsRecursive(d,dlist,option);
    }
}

void HDiskCatalog::printDisk(UChar_t maxlevel,TString option ,Int_t nfill,TString filler)
{
    // prints the catalog of the disk
    // maxlevel : number of subdir levels to be shown
    // sort option: name        (name of owner)
    //              size        (size of directory)
    //              nfiles      (number of files in directory (rekursively))
    //              nsmallfiles (number of small files in directory(rekursively))
    //              filesratio  (number of small files / number of total files (rekursively))
    // nfill      :  number of character to be inserted in front for each level of subdirs
    // filler     :  character to be inserted in front for each level of subdirs
    TString replace = diskname ;
    replace +="/";
    Int_t maxreplace = 36;

    if(nfill==0) {
	if(replace.Length()<maxreplace) maxreplace = replace.Length();

	cout<<replace<<std::string(10+26-maxreplace,' ')
	    <<setw(8)<<""
	    <<" "<<""
	    <<" "<<setw(12)<<"uid"
	    <<" "<<setw(10)<<"gid"
	    <<" "<<setw(9)<<"files"
	    <<" "<<setw(9)<<"small f"
	    <<" "<<setw(9)<<"dirs"
	    <<" "<<setw(9)<<"empty d"
	    <<endl;
    } else {

	if(replace.Length()<maxlevel*nfill+26) maxreplace = replace.Length();
        else                                   maxreplace = maxlevel*nfill+26;

	cout<<replace<<std::string(maxlevel*nfill+26-maxreplace,' ')
	    <<setw(8)<<""
	    <<" "<<""
	    <<" "<<setw(12)<<"uid"
	    <<" "<<setw(10)<<"gid"
	    <<" "<<setw(9)<<"files"
	    <<" "<<setw(9)<<"small f"
	    <<" "<<setw(9)<<"dirs"
	    <<" "<<setw(9)<<"empty d"
	    <<endl;
    }


    HDiskDir* base = (HDiskDir*) list->At(0);
    vector<HDiskDir*> level1;
    vector<HDiskDir*> full;
    full.push_back(base);
    if(base){
	for(UInt_t i = 0; i < base->getNDirs(); i ++) {
            HDiskDir* lvl1 = (HDiskDir*) list->At(base->getDirs()[i]);
	    if(lvl1){
		level1.push_back(lvl1);
	    }
	}
    }


    if(option=="name")        { sort(level1.begin(),level1.end(),cmpName);       }
    if(option=="size")        { sort(level1.begin(),level1.end(),cmpSize);       }
    if(option=="nfiles")      { sort(level1.begin(),level1.end(),cmpNFiles);     }
    if(option=="nsmallfiles") { sort(level1.begin(),level1.end(),cmpNSmallFiles);}


    for(UInt_t i = 0; i < level1.size(); i ++) {
	full.push_back(level1[i]);
	sortDirsRecursive(level1[i],full,option);
    }

    for(UInt_t i=0;i<full.size();i++)
    {
	HDiskDir* dir = full[i];
	if(dir->getLevel() > maxlevel) continue;

	TString dirname = dir->GetName();
	dirname.ReplaceAll(replace.Data(),"");

	TString modtime ;
	TString sowner;
	TString sgroup;
	TString fsize;
	HFileSys::getModTime (dir->getLastMod(),modtime);
	HFileSys::getUser    (dir->getOwner(),sowner);
	HFileSys::getGroup   (dir->getGroup(),sgroup);
	HFileSys::getFileSize(dir->getSize(),fsize);


	if(nfill==0) {
	    cout<<std::string(10,' ')
		<<setw(8)<<fsize.Data()
		<<" "<<modtime.Data()
		<<" "<<setw(12)<<sowner
		<<" "<<setw(10)<<sgroup
		<<" "<<setw(9)<<dir->getNFilesTotal()
		<<" "<<setw(9)<<dir->getNSmallFilesTotal()
		<<" "<<setw(9)<<dir->getNDirsTotal()
		<<" "<<setw(9)<<dir->getNEmptyDirsTotal()
		<<" "<<dirname
		<<endl;
	} else {
	    cout<<std::string(dir->getLevel()*nfill,*filler.Data())
		<<setw(8)<<fsize.Data()
		<<std::string((maxlevel-dir->getLevel())*nfill,' ')
		<<" "<<modtime.Data()
		<<" "<<setw(12)<<sowner
		<<" "<<setw(10)<<sgroup
		<<" "<<setw(9)<<dir->getNFilesTotal()
		<<" "<<setw(9)<<dir->getNSmallFilesTotal()
		<<" "<<setw(9)<<dir->getNDirsTotal()
		<<" "<<setw(9)<<dir->getNEmptyDirsTotal()
		<<" "<<dirname<<endl;
	}
    }
    printExecution();
}

void HDiskCatalog::printExecution()
{
    // prints the execution start and stop time and the duration
    TString mytime;
    HFileSys::getModTime(lastScanStart,mytime);
    cout<<"scan started  at "<<mytime<<endl;
    HFileSys::getModTime(lastScanStop,mytime);
    cout<<"scan finished at "<<mytime<<" ("<<(lastScanStop-lastScanStart)/60.<<" minutes)"<<endl;
}

UInt_t HDiskCatalog::filterDirs(TString regexp,vector<HDiskDir*>& dirs,TString range1S,TString range2S,Long64_t size1)
{
    // filters all dirs fitting the regular expression regexp
    // TRegexp is used, read about the limitations
    // dirs cand be filtered by last modification range1 to range2
    // (format yyyy-mm-dd hh:mm:ss). default is no filter
   ULong64_t range1=0;
   ULong64_t range2=kMaxULong64-1;;

   TDatime damin;
   TDatime damax;

   struct tm t_min;
   struct tm t_max;


   ULong64_t size = abs(size1);

   if(range1S.CompareTo("")!=0) {
       damin.Set(range1S.Data());
       range1 = damin.Convert();
       localtime_r((time_t*)(&range1),&t_min);
   }


   if(range2S.CompareTo("")!=0) {
       damax.Set(range2S.Data());
       range2 = damax.Convert();
       localtime_r((time_t*)(&range2),&t_max);
   }
   TRegexp expr(regexp);
   dirs.clear();
   for(Int_t i=0;i< list->GetEntries();i++){
       HDiskDir* dir = (HDiskDir*)list->At(i);
       TString name = dir->GetName();
       if((ULong64_t)dir->getLastMod() < range1 || (ULong64_t)dir->getLastMod() > range2 ) continue;
       if     (size1 > 0 && dir->getSize()<size) continue;
       else if(size1 < 0 && dir->getSize()>size) continue;

       if(name(expr) != "") dirs.push_back(dir);
   }
   return dirs.size();
}

UInt_t HDiskCatalog::filterFiles(TString regexpdir,TString regexpfile,vector<HDiskFile*>& files,TString range1S,TString range2S,Long64_t size1)
{
    // filter all files fitting the regular expression regexpdir for
    // the path and regexpfile for the file name
    // TRegexp is used, read about the limitations
    // files cand be filtered by last modification range1 to range2
    // (format yyyy-mm-dd hh:mm:ss). default is no filter
   files.clear();

   ULong64_t range1=0;
   ULong64_t range2=kMaxULong64-1;
   ULong64_t size = abs(size1);

   TDatime damin;
   TDatime damax;

   struct tm t_min;
   struct tm t_max;

   if(range1S.CompareTo("")!=0) {
       damin.Set(range1S.Data());
       range1 = damin.Convert();
       localtime_r((time_t*)(&range1),&t_min);
   }


   if(range2S.CompareTo("")!=0) {
       damax.Set(range2S.Data());
       range2 = damax.Convert();
       localtime_r((time_t*)(&range2),&t_max);
   }

    TRegexp exprdir (regexpdir);
    TRegexp exprfile(regexpfile);
    TString modtime;
    TString sowner;
    TString sgroup;
    TString fsize;
    TString name;
    TString fname;
    for(Int_t i=0;i< list->GetEntries();i++){
	HDiskDir* dir = (HDiskDir*)list->At(i);
	name = dir->GetName();
	if(name(exprdir) != "") {
	    for(UInt_t j=0;j<dir->getNFiles();j++){
		HDiskFile& f = dir->getFiles()[j];
		fname = f.GetName();
		if((ULong64_t)f.getLastMod() < range1 || (ULong64_t)f.getLastMod() > range2 ) continue;
		if     (size1 > 0 && f.getSize()<size) continue;
		else if(size1 < 0 && f.getSize()>size) continue;

		if(fname(exprfile) != "") {
                   files.push_back(&f);
		}
	    }
	}
    }

    return files.size();
}

void HDiskCatalog::printDirs(TString regexp,TString range1S,TString range2S,Long64_t size)
{
    // prints all dirs fitting the regular expression regexp
    // TRegexp is used, read about the limitations
    // dirs cand be filtered by last modification range1 to range2
    // (format yyyy-mm-dd hh:mm:ss). default is no filter
   vector<HDiskDir*> dirs;
   filterDirs(regexp,dirs,range1S,range2S,size);
   for(UInt_t i=0;i < dirs.size();i++){
       HDiskDir* dir = dirs[i];
       dir->print();
   }
}

void HDiskCatalog::printFiles(TString regexpdir,TString regexpfile,TString range1S,TString range2S,Long64_t size)
{
    // prints all files fitting the regular expression regexpdir for
    // the path and regexpfile for the file name
    // TRegexp is used, read about the limitations
    // files cand be filtered by last modification range1 to range2
    // (format yyyy-mm-dd hh:mm:ss). default is no filter
    vector<HDiskFile*> files;
    filterFiles(regexpdir,regexpfile,files,range1S,range2S,size);
    TString modtime;
    TString sowner;
    TString sgroup;
    TString fsize;
    for(UInt_t i=0;i < files.size();i++){
	HDiskFile* f = files[i];
	HFileSys::getModTime (f->getLastMod(),modtime);
	HFileSys::getUser    (f->getOwner(),sowner);
	HFileSys::getGroup   (f->getGroup(),sgroup);
	HFileSys::getFileSize(f->getSize(),fsize);
        HDiskDir* dir = (HDiskDir*)list->At(f->getDir());
	cout<<setw(8) <<fsize.Data()
	    <<" "<<modtime.Data()
	    <<" "<<setw(15)<<sowner
	    <<" "<<setw(15)<<sgroup
	    <<" "<<dir->GetName()
	    <<"/"<<f->GetName()<<endl;
    }
}

Bool_t HDiskCatalog::getUser(uid_t uid,TString& name)
{
    // lookup of user uid in the map of
    // known users. if the user can not be
    // found the the user name will be unknown_uid
    name = Form("unknown_%i",uid);
    map<uid_t,TString>::iterator it = musers.find(uid);
    if(it!=musers.end()) { name = it->second; return kTRUE;}
    return kFALSE;
}

Bool_t HDiskCatalog::getGroup(gid_t gid,TString& name)
{
    // lookup of group gid in the map of
    // known groups. if the group can not be
    // found the the group name will be unknown_gid
    name = Form("unknown_%i",gid);
    map<gid_t,TString>::iterator it = mgroups.find(gid);
    if(it!=mgroups.end()) { name = it->second; return kTRUE;}
    return kFALSE;
}

//----------------------------------------------------------------------------

HDiskStat::HDiskStat(TString name)
{
    diskname = name;

    Int_t colors[]=
    {
	kRed  ,kMagenta  ,kBlue  ,kCyan  ,kGreen  ,kOrange,
	kRed-6,kMagenta-6,kBlue-6,kCyan-6,kGreen-6,kOrange-6,
	kRed-7,kMagenta-7,kBlue-7,kCyan-7,kGreen-7,kOrange-7,
	kRed-2,kMagenta-2,kBlue-2,kCyan-2,kGreen-2,kOrange-2,kBlack
    };
   Int_t markers[]={
       20,21,22,23,24,25,26,27,28
   };

   Int_t styles[]={
       1,2
   };

   vcolors .assign(colors ,colors + sizeof(colors)  /sizeof(Int_t));
   vmarkers.assign(markers,markers + sizeof(markers)/sizeof(Int_t));
   vstyles .assign(styles ,styles  + sizeof(styles) /sizeof(Int_t));
   unit =  HFileSys::getUnitG() ;
}

HDiskStat::~HDiskStat()
{
    ;
}

Int_t HDiskStat::evalOpt(TString opt)
{
    Int_t index =-1;
    if     (opt=="size")        index = 0;
    else if(opt=="nfiles")      index = 1;
    else if(opt=="nsmallfiles") index = 2;
    else {
	cout<<"ERROR: HDiskStat::evalOpt(): Unknown option ="<<opt<<endl;
    }
    return index;
}

Bool_t HDiskStat::findMinMaxGraph(Double_t& xmin,Double_t& xmax,Double_t& ymin,Double_t& ymax,TGraph* g)
{
    xmax = 0;
    xmin = 0;
    ymax = 0;
    ymin = 0;
    Double_t x,y;

    for(Int_t i=0; i<g->GetN();i++){
	g->GetPoint(i,x,y);
	if(x<xmin || xmin==0) xmin = x;
	if(x>xmax || xmax==0) xmax = x;
	if(y<ymin || ymin==0) ymin = y;
	if(y>ymax || ymax==0) ymax = y;

    }
    return kTRUE;
}

Bool_t HDiskStat::findMinMaxAll(Double_t& xmin,Double_t& xmax,Double_t& ymin,Double_t& ymax,vector<TGraph*>& vg)
{
    xmax = 0;
    xmin = 0;
    ymax = 0;
    ymin = 0;
    TGraph*g  = 0;
    Double_t x1,x2,y1,y2;
    for(UInt_t i=0; i<vg.size(); i++ ) {
	g  = vg[i];
	findMinMaxGraph(x1,x2,y1,y2,g);

	if(x1<xmin || xmin==0) xmin = x1;
	if(x2>xmax || xmax==0) xmax = x2;

	if(y1<ymin || ymin==0) ymin = y1;
	if(y2>ymax || ymax==0) ymax = y2;
    }

    if(ymin==ymax && ymin!=0)   ymin=0;
    if(ymin==ymax && ymin==0) { ymin=0; ymax = 10;}

    return kTRUE;
}

Bool_t HDiskStat::findMinMaxAll(Double_t& xmin,Double_t& xmax,Double_t& ymin,Double_t& ymax,TString opt)
{
    Int_t index = 0 ;
    xmax = 0;
    xmin = 0;
    ymax = 0;
    ymin = 0;
    index=evalOpt(opt);
    if(index < 0 ) return kFALSE;
    TGraph*g  = 0;
    Double_t x1,x2,y1,y2;
    for(map<TString,vector<TGraph> >::iterator iter = mDirToVal.begin(); iter != mDirToVal.end(); ++iter ) {
	g  = &(iter->second[index]);
	findMinMaxGraph(x1,x2,y1,y2,g);

	if(x1<xmin || xmin==0) xmin = x1;
	if(x2>xmax || xmax==0) xmax = x2;

	if(y1<ymin || ymin==0) ymin = y1;
	if(y2>ymax || ymax==0) ymax = y2;
    }

    if(ymin==ymax && ymin!=0)   ymin=0;
    if(ymin==ymax && ymin==0) { ymin=0; ymax = 10;}

    return kTRUE;
}

TGraph* HDiskStat::getDir(TString dirname,TString opt)
{
    Int_t index = evalOpt(opt);
    if(index<0) return NULL;

    map<TString, vector<TGraph> >::iterator it = mDirToVal.find(dirname);
    if(it != mDirToVal.end()) {
	return &it->second[index];
    } else return NULL;
}

vector<TGraph>* HDiskStat::getDirVec(TString dirname)
{
    map<TString, vector<TGraph> >::iterator it = mDirToVal.find(dirname);
    if(it != mDirToVal.end()) {
	return &(it->second);
    }
    else return NULL;
}

void HDiskStat::addEntry(time_t scanstart,HDiskDir* dir)
{

    map<TString, vector<TGraph> >::iterator it = mDirToVal.find(dir->GetName());

    if(it != mDirToVal.end()) { // directory found, add entry
	it->second[0].SetPoint(it->second[0].GetN(),scanstart,dir->getSize());
        it->second[1].SetPoint(it->second[1].GetN(),scanstart,dir->getNFilesTotal());
        it->second[2].SetPoint(it->second[2].GetN(),scanstart,dir->getNSmallFilesTotal());

    } else { // new dir added, create all graphs
	vector<TGraph> v;

	TGraph gsize;
	gsize.SetName (Form("%s_size",dir->GetName()));
	gsize.SetPoint(gsize.GetN(),scanstart,dir->getSize());
	gsize.SetLineColor  (vcolors [(mDirToVal.size()+1)%vcolors .size()]);
	gsize.SetMarkerColor(vcolors [(mDirToVal.size()+1)%vcolors .size()]);
	gsize.SetMarkerStyle(vmarkers[(mDirToVal.size()+1)%vmarkers.size()]);
	gsize.SetLineStyle  (vstyles [(mDirToVal.size()+1)%vstyles .size()]);
	v.push_back(gsize);

	TGraph gnfiles;
	gnfiles.SetName (Form("%s_nfiles",dir->GetName()));
	gnfiles.SetPoint(gnfiles.GetN(),scanstart,dir->getNFilesTotal());
	gnfiles.SetLineColor  (vcolors [(mDirToVal.size()+1)%vcolors .size()]);
	gnfiles.SetMarkerColor(vcolors [(mDirToVal.size()+1)%vcolors .size()]);
	gnfiles.SetMarkerStyle(vmarkers[(mDirToVal.size()+1)%vmarkers.size()]);
	gnfiles.SetLineStyle  (vstyles [(mDirToVal.size()+1)%vstyles .size()]);
        v.push_back(gnfiles);

	TGraph gnsmallfiles;
	gnsmallfiles.SetName (Form("%s_nsmallfiles",dir->GetName()));
	gnsmallfiles.SetPoint(gnsmallfiles.GetN(),scanstart,dir->getNSmallFilesTotal());
	gnsmallfiles.SetLineColor  (vcolors [(mDirToVal.size()+1)%vcolors .size()]);
	gnsmallfiles.SetMarkerColor(vcolors [(mDirToVal.size()+1)%vcolors .size()]);
	gnsmallfiles.SetMarkerStyle(vmarkers[(mDirToVal.size()+1)%vmarkers.size()]);
	gnsmallfiles.SetLineStyle  (vstyles [(mDirToVal.size()+1)%vstyles .size()]);
	v.push_back(gnsmallfiles);

        mDirToVal[dir->GetName()] = v;

    }


}

TCanvas* HDiskStat::draw(TString opt,UInt_t lastDays,ULong64_t u,TString select)
{
    // select =  mother draws only mother dir
    //        =  default "daughters"

    Int_t index = evalOpt(opt);
    if (index < 0) return NULL;

    gStyle->SetOptStat(0);
    gStyle->SetPadTopMargin(0.08);
    gStyle->SetPadBottomMargin(0.2);
    gStyle->SetPadRightMargin(0.18);
    gStyle->SetPadLeftMargin(0.12);

    TString timeformat="#splitline{%d}{%b}";

    ULong64_t    U = unit;
    if( u != 0 ) U = u;
    if(opt != "size"  ) U = 1;

    time_t tnow=0;
    tnow=time(&tnow);

    TString timeStamp;
    HFileSys::getModTime(tnow,timeStamp);

    time_t tlowlimit = tnow - lastDays*86400L;   // low limit for range
    Double_t dtlow   = (Double_t)tlowlimit;

    //-----------------------------------------------------------
    // find range of data and convert to used unit
    Double_t xmax = 0;
    Double_t xmin = 0;
    Double_t ymax = 0;
    Double_t ymin = 0;

    findMinMaxAll(xmin,xmax,ymin,ymax,opt);   // full range all graphs
    ymax/=U;                                  // correct units
    ymin/=U;

    //-----------------------------------------------------------

    if(xmin > dtlow) {  // range of data was smaller than used limit
	dtlow     = xmin;
        tlowlimit = (time_t) xmin;
    }
    //-----------------------------------------------------------


    //-----------------------------------------------------------
    // extract the graphs using range
    vector<TGraph*> vg;
    Double_t x,y;
    for (map<TString,vector<TGraph> >::iterator it = mDirToVal.begin(); it != mDirToVal.end(); ++it) {
        TGraph& gin= it->second[index];
	TString name=it->first;
	name.ReplaceAll(diskname.Data(),"");
	if(select != "mother") {
          if(name == "") continue;  // skip base dir
	} else {
            if(name != "") continue;  // skip daughter dirs
	    name=gSystem->BaseName(it->first.Data()); // keep only last dir
	}

	TGraph* g = new TGraph();
	g->SetName(name.Data());
	g->SetLineColor(gin.GetLineColor());
	g->SetLineStyle(gin.GetLineStyle());
	g->SetLineWidth(gin.GetLineWidth());
	g->SetMarkerStyle(gin.GetMarkerStyle());
	g->SetMarkerColor(gin.GetMarkerColor());
	g->SetMarkerSize(gin.GetMarkerSize());
       


        Int_t ct=0;
	for(Int_t i=0;i<gin.GetN();i++){
	    gin.GetPoint(i,x,y);
	    if(x>=dtlow){
		g->SetPoint(ct,x,y/U);
		ct++;
	    }
	}
	if(g->GetN() > 0){             // skip grap with entry in range
	    Double_t x1 = 0;
	    Double_t x2 = 0;
	    Double_t y1 = 0;
	    Double_t y2 = 0;
            findMinMaxGraph(x1,x2,y1,y2,g);
	    if(y2>0) vg.push_back(g);  // skip empty dirs
            else     delete g;
	} else     delete g;
    }
    //-----------------------------------------------------------

    findMinMaxAll(xmin,xmax,ymin,ymax,vg); // range from selected graphs in range of interest


    //-----------------------------------------------------------
    // graphic output
    TCanvas* c = new TCanvas(Form("cuser_%s_%s",opt.Data(),select.Data()),"user stat",1000,800);
    gPad->SetGridy();
    gPad->SetGridx();

    TH2D* h = new TH2D(Form("hdir_%s_%s",opt.Data(),select.Data()),Form("%s : %s ",diskname.Data(),timeStamp.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    h->GetXaxis()->SetTimeDisplay(1);
    h->GetXaxis()->SetTimeFormat(timeformat.Data());
    h->GetXaxis()->SetLabelSize(0.02);
    h->GetXaxis()->LabelsOption("v");
    h->GetXaxis()->SetTickLength(0);

    if     (opt == "size"  )      {
	if(U==HFileSys::getUnitK() ) h->SetYTitle("diskspace [KB]");
	if(U==HFileSys::getUnitM() ) h->SetYTitle("diskspace [MB]");
        if(U==HFileSys::getUnitG() ) h->SetYTitle("diskspace [GB]");
        if(U==HFileSys::getUnitT() ) h->SetYTitle("diskspace [TB]");
    }
    else if(opt == "nfiles")      h->SetYTitle("number of files");
    else if(opt == "nsmallfiles") h->SetYTitle("number of small files");

    h->GetYaxis()->SetTitleOffset(1.7);

    h->Draw();


    TLegend* l = new TLegend(0.83,0.97,0.995,0.03,"","brNDC");
    for(UInt_t i=0;i<vg.size();i++){
        TGraph* g = vg[i];
	l->AddEntry(g,g->GetName(),"lp");
    }
    l->Draw();

    for(UInt_t i=0;i<vg.size();i++){
        TGraph* g = vg[i];
	g->Draw("L");
	g->Draw("Psame");
    }
    //-----------------------------------------------------------

    return c;
}
