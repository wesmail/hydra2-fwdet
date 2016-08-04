#ifndef __HZIP__
#define __HZIP__



#include "TObject.h"
#include "TFile.h"
#include "TSystem.h"
#include "TString.h"
#include "TRegexp.h"
#include "TArchiveFile.h"
#include "TArchiveFile.h"
#include "TList.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TChain.h"
#include "TChainElement.h"

#include <wordexp.h>

#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;






class HZip : public TObject {

public:
    HZip(){};
    virtual ~HZip(){};

    static TObjArray* glob(TString pattern)
    {
	// Expand 'pattern' as it would be done by the shell (see sh(1)):
	// '*', '?', '~', '$ENVIRONMENT_VARIABLE' and '[', ']' are expanded. In case
	// the expansion is successful, a TObjArray of TObjStrings with the
	// names of all existing files is returned - otherwise NULL.
	//
	// The returned TObjArray must be deleted by the caller of the function!
	//
	// This example returns all existing .root files in /tmp as well as all .root
	// files in your home directory starting with the characters 'a', 'b', 'c'
	// and 'e':
	//
	// TObjArray* files = HTool::glob( "/tmp/*.root $HOME/[a-c,e]*.root" );
	// if (files)
	// {
	//    TObjString* name = NULL;
	//    TIterator*  file = files->MakeIterator();
	//    while ((name = (TObjString*)file->Next()))
	//    {
	//       Char_t* input = name->GetString.Data(); ...
	//    }
	//    delete files;
	// }

	wordexp_t  file_list;
	Char_t**   file;
	TObjArray* filenames = NULL;

	if (pattern.IsNull())
	    return NULL;

	if (::wordexp( pattern.Data(), &file_list, 0 ))
	{
	    ::wordfree( &file_list );
	    return NULL;
	}

	file      = file_list.we_wordv;
	filenames = new TObjArray;

	for (UInt_t i = 0; i < file_list.we_wordc; i++)
	{
	    // check if files real exist, since ::wordexp(3) returns the unexpanded
	    // pattern, if e.g. the path does not exist
	    if (!gSystem->AccessPathName( file[i] ))
		filenames->Add( new TObjString( file[i] ) );
	}

	::wordfree( &file_list );
	if (filenames->GetEntries() == 0)
	{
	    delete filenames;
	    filenames = NULL;
	}

	return filenames;
    }

    static TObjArray* readFileList(TString listfile)
    {
	// read inputfiles form list (one file per line)
	// and add them to an TObjArray. The pointer to
	// the array is returned. In case of an error or
	// if no files are contained the pointer is NULL.
        // The user must take care of deleting the array.

 	if(gSystem->AccessPathName(listfile)){
  	    printf("Error: readFile() : list file does not exist!");
	    return NULL;
	}

	Char_t line[1000];
	ifstream inp;

	TObjArray* filenames = new TObjArray;

	inp.open(listfile.Data());
	TString name;
	while(!inp.eof()){
	    inp.getline (line, 1000);
	    name = line;

	    if(gSystem->AccessPathName(name)){
		printf("Error: readFileList() : file %s does not exist!",name.Data());
		continue;
	    }
	    filenames->Add( new TObjString( name ) );
	}
	inp.close();

	if (filenames->GetEntries() == 0) {
	    delete filenames;
	    filenames = NULL;
	}
	return filenames;
    }

    static Bool_t chainToTObjArray(TChain* chain=0,TObjArray* filenames=0)
    {
        //  add all filenames of TChain to TObjArray
	if(chain == 0){
	    printf("Error: chainToTObjArray() : TChain pointer is NULL!");
	    return kFALSE;
	}
        if(filenames == 0){
	    printf("Error: chainToTObjArray() : TObjArray pointer is NULL!");
	    return kFALSE;
	}

	TObjArray* elements = chain->GetListOfFiles();
	Int_t nfiles        = elements->GetEntries();
	for(Int_t i=0;i<nfiles;i++){
	    TChainElement* element = (TChainElement*)elements->At(i);
	    filenames->Add( new TObjString( element->GetTitle() ) );
	}
        return kTRUE;
    }


    static Bool_t exists(TString name,Bool_t silent=kFALSE,Bool_t isZip=kTRUE)
    {
	// returns kTRUE if file exists and ends with .zip

	if(isZip && name.EndsWith(".zip") == 0) {
	    if(!silent) printf("Error: File %s does not end with .zip\n!",name.Data());
	    return kFALSE;
	}
	if(gSystem->AccessPathName(name.Data()) != 0) {
	    if(!silent) printf("Error: File %s does not exist\n!",name.Data());
	    return kFALSE;
	}
	return kTRUE;
    }

    static Bool_t splitName(TString fullname, TString& zipname,TString& membername)
    {
	// if filename is   .zip#membername the
	// name is splitted into the zip file name and
	// the archive member name. return kTRUE
        // if success els kFALSE

	if(fullname.Contains(".zip#") != 0){

	    zipname    = fullname;
	    zipname.Remove(zipname.First('#'));

	    membername = fullname;
	    membername.Replace(0,membername.First('#') + 1,"");

            return kTRUE;
	}
	printf("Error: isInside() : Filename does not contain .zip#membername!\n");
	return kFALSE;
    }

    static Bool_t isInside(TString name, Bool_t print=kFALSE)
    {
	// test if the member  (format: zipfilename.zip#membername)
	// is inside the zip file. Return kTRUE if
        // success else kFALSE

	TString zipname;
	TString membername;

	if(!HZip::splitName(name,zipname,membername)) { return kFALSE; }

	if(!HZip::exists(zipname)) return kFALSE;

	Bool_t found = kFALSE;

	TFile* fzip = TFile::Open(zipname.Data());
	if(fzip) {
	    TArchiveFile* archive = fzip->GetArchive();
	    if(archive){
		TObjArray* members = archive->GetMembers();
		if(members->FindObject(membername.Data()) == 0) {
		    if(print) printf("Info: isInside() : File %s not found ind zipfile %s\n",membername.Data(),zipname.Data());
		    found = kFALSE;
		} else {
		    if(print) printf("Info: isInside() : File %s found ind zipfile %s\n"    ,membername.Data(),zipname.Data());
		    found = kTRUE;
		}
	    } else {
		printf("Error: isInside() : Retrieved NULL pointer for Archive!\n");
	    }
	    fzip->Close();
	    delete fzip;

	}
	return found;
    }

    static Bool_t isInside(TString zipname, TString membername ,Bool_t print=kFALSE)
    {
	// test if the member is inside the zip file.
	// Return kTRUE if success else kFALSE
	TString name = zipname + "#" + membername;
	return  isInside(name,print);
    }

    static Int_t list(TString name, TString filter=".*",Int_t size=0,Int_t time=0)
    {
	// lists the members of the zip file which pass
	// the name filter (see TRegexp). returns the number
        // of matched files

	if(!HZip::exists(name)) return 0;

	TFile* fzip = TFile::Open(name.Data());
	if(fzip) {
	    TArchiveFile* archive = fzip->GetArchive();
	    if(archive){
		cout<<"-------------------------------------------------------------------------------"<<endl;
		cout<<"Listing all file inside zip file "<<name.Data()<<" (matching "<<filter.Data()<<") :"<<endl;
		TObjArray* members = archive->GetMembers();
		TString fname = "";
		TRegexp expr(filter);
		Int_t ct = 0;
		for(Int_t i = 0; i < members->GetEntries(); i++){
		    TArchiveMember* member = (TArchiveMember*) members->At(i);
		    fname = member->GetName();
		    if(fname(expr) != ""){
			cout<<setw(5)<<ct
			    <<" size "   <<setw(12)<<member->GetCompressedSize()
			    <<" mod time "<<member->GetModTime().AsString()
			    <<" "<<member->GetName()<<endl;
			ct++;
		    }
		}
		cout<<"-------------------------------------------------------------------------------"<<endl;
		return ct;

	    } else {
		printf("Error: list() : Retrived NULL pointer for Archive!\n");
	    }
	    fzip->Close();
	    delete fzip;
	}
        return 0;
    }

    static Int_t getList(TString name,TList* list,TString filter=".*",Int_t size=0,Int_t time=0)
    {
	// fills a list with the members of the zip file which pass
	// the name filter (see TRegexp). returns the number
        // of matched files

	if(!HZip::exists(name)) return 0;

	TFile* fzip = TFile::Open(name.Data());
	if(fzip) {
	    TArchiveFile* archive = fzip->GetArchive();
	    if(archive){
		TObjArray* members = archive->GetMembers();
		TString fname = "";
		TRegexp expr(filter);
		Int_t ct = 0;
		for(Int_t i = 0; i < members->GetEntries(); i++){
		    TArchiveMember* member = (TArchiveMember*) members->At(i);
		    fname = member->GetName();
		    if(fname(expr) != ""){

			list->Add(new TObjString(member->GetName()));
			ct++;
		    }
		}
		return ct;

	    } else {
		printf("Error: list() : Retrived NULL pointer for Archive!\n");
	    }
	    fzip->Close();
	    delete fzip;
	}
        return 0;
    }

    static Bool_t makeChain(TString zipname,TChain* chain = 0,TString filter=".*",Int_t size=0,Int_t time=0)
    {
	// adds all root members of the zip file matching the filter
        // to TChain (has to be created before)

	if(chain == 0){
	    printf("Error: makeChain() : TChain pointer is NULL!");
	    return kFALSE;
	}

	TList* list = new TList();
	HZip::getList(zipname,list,filter,size,time);
        HZip::list(zipname,filter,size,time);

	TObjString* member;

	TIterator* iter = list->MakeIterator();

	while((member = (TObjString*)iter->Next()) != 0){
	    TString membername = member->GetString();
	    TString fullname = zipname + "#" + membername;
            chain->Add(fullname.Data());
	}
	delete iter;
	delete list;

	return kTRUE;
    }

    static Bool_t makeChainGlob(TString expressionzip,TChain* chain = 0,TString filter=".*",Int_t size=0,Int_t time=0)
    {
	// adds all root members match the filter of all zip files matching the expression
        // to TChain (has to be created before)
	if(chain == 0){
	    printf("Error: makeChainGlob() : TChain pointer is NULL!");
	    return kFALSE;
	}

        TObjArray* files = HZip::glob(expressionzip);

        Int_t nfiles = files->GetEntries();
	if(nfiles == 0) return kFALSE;

	for(Int_t j=0;j<nfiles;j++){
               TString fna = ((TObjString*)(files->At(j)))->GetString();
               HZip::makeChain(fna,chain,filter,size,time);
	}

        delete files;
	return kTRUE;
    }

    static Bool_t makeChainList(TString listfile,TChain* chain = 0,TString filter=".*",Int_t size=0,Int_t time=0)
    {
	// adds all root members match the filter of all zip files matching the expression
	// to TChain (has to be created before). The file list should contain
        // one filename perline.
	if(chain == 0){
	    printf("Error: makeChainList() : TChain pointer is NULL!");
	    return kFALSE;
	}

	if(gSystem->AccessPathName(listfile)){
  	    printf("Error: makeChainList() : list file does not exist!");
	    return kFALSE;
	}

	Char_t line[1000];
	ifstream inp;


	inp.open(listfile.Data());
	TString name;
	while(!inp.eof()){
	    inp.getline (line, 1000);
	    name = line;

	    if(gSystem->AccessPathName(name)){
		printf("Error: makeChainList() : file %s does not exist!",name.Data());
		continue;
	    }

	    HZip::makeChain(name,chain,filter,size,time);
	}
	inp.close();
	return kTRUE;
    }

    static Bool_t addFile(TString zipname,TString membername, Int_t mode=0)
    {
	// mode = 0  ==> do not replace existing members and warn
        // mode = 1  ==> do not replace existing members and do not warn
        // mode = 2  ==> replace existing members and do not warn

	TString zipName = zipname;
	zipName.ReplaceAll(".zip","");
	TString cmd = Form("zip -j -g -n .root %s %s 1>/dev/null",zipName.Data(),membername.Data());

	if(!HZip::exists(membername,kFALSE,kFALSE)) return kFALSE; // complain if not existing
	if(!HZip::exists(zipname,kTRUE))                           // do not complain if not existing
	{ // create zipfile new
	    Int_t rc = gSystem->Exec(cmd.Data());
	    if(rc == 0) {
		cout<<"adding: "<<membername.Data()<<" (new)"<<endl;
		return kTRUE;
	    }
	    else {
		printf("Error: addFile() : zip returned with error!\n");
		return kFALSE;
	    }

	} else { // add to existing zipfile
            Bool_t inside = HZip::isInside(zipname + "#" + membername);
	    if(mode == 2){ // replace
		Int_t rc = gSystem->Exec(cmd.Data());
		if(rc == 0) {
		    if(!inside){
			cout<<"adding: "<<membername.Data()<<" (added)"<<endl;
		    } else {
			cout<<"adding: "<<membername.Data()<<" (replace)"<<endl;
		    }
		    return kTRUE; }
		else {
		    printf("Error: addFile() : zip returned with error!\n");
		    return kFALSE;
		}

	    } else {  // mode 0 + 1
		if(!HZip::isInside(zipname + "#" + membername)){ // add new member
		    Int_t rc = gSystem->Exec(cmd.Data());
		    if(rc == 0) {
			cout<<"adding: "<<membername.Data()<<" (new)"<<endl;
			return kTRUE;
		    }
		    else {
			printf("Error: addFile() : zip returned with error!\n");
			return kFALSE;
		    }
		} else { // exists already
		    if (mode == 0) { // warn + kFALSE
			printf("Error: addFile() : Member = %s already existing inside zipfile %s\n!",membername.Data(),zipname.Data());
                        return kFALSE;
		    } else { // do not warn + kTRUE
                        cout<<"adding: "<<membername.Data()<<" (replace)"<<endl;
			return kTRUE;
		    }
		}
	    }

	}


      return kFALSE;
    }

    static Bool_t addFiles(TString zipname,TList* list, Int_t mode=0)
    {
	// mode = 0  ==> do not replace existing members and warn
        // mode = 1  ==> do not replace existing members and do not warn
        // mode = 2  ==> replace existing members and do not warn
	cout<<"-------------------------------------------------------------------------------"<<endl;
	cout<<"Adding files to file "<<zipname.Data()<<" :"<<endl;

	TObjString* member;
	TIterator* iter = list->MakeIterator();

	while((member = (TObjString*)iter->Next()) != 0){
	    TString membername = member->GetString();
	    if(!addFile(zipname,membername,mode)) {
                delete iter;
		cout<<"-------------------------------------------------------------------------------"<<endl;
		return kFALSE;
	    }
	}
	cout<<"-------------------------------------------------------------------------------"<<endl;
        delete iter;
	return kTRUE;
    }

    static Bool_t test(TString zipname){
        // run unzip test on zipfile
	TString cmd = Form("unzip -t %s",zipname.Data());
	Int_t rc = gSystem->Exec(cmd.Data());
	if(rc == 0) { return kTRUE; }
	else        {
	    printf("Error: test() : zip returned with error!");
	    return kFALSE;
	}
    }

    static Bool_t unzip(TString zipname,TString outDir=""){
        // unzip content of zipfile to outDir (optional, default is local dir)
	TString cmd;
	if(outDir != "")  cmd = Form("unzip %s -d %s",zipname.Data(),outDir.Data());
	else              cmd = Form("unzip %s"      ,zipname.Data());
	Int_t rc = gSystem->Exec(cmd.Data());
	if(rc == 0) { return kTRUE; }
	else        {
	    printf("Error: unzip() : zip returned with error!");
	    return kFALSE;
	}
    }



    ClassDef(HZip,0);
};

//ClassImp(HZip)

#endif

