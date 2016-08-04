#include "hsrckeeper.h"
#include "hdiskspace.h"


#include "TSystem.h"
#include "TRegexp.h"
#include "TPRegexp.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;



//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HSrcKeeper
//
// Tool to store source code along root output files.
// REMARK: To work with hadd for histogram merging
// a special hadd version has to be compiled which is linked
// against hydra libs (Dictionary is needed)
//
//------------------------------------------------------------------------------
// USAGE:
//
//    TFile* out = new TFile(outfile.Data(),"RECREATE");
//
//    HSrcKeeper keeper("hydra","hydra");
//
//    //keeper.addSourceFile("test.C");
//    //keeper.addSourceFiles(dir,"\\..*[cCh]+$");         // single files or files matching the regexp inside dir
//    keeper.addSourceFilesRecursive(dir,"\\..*[cCh]+$");  // recursive add through all subdir of dir
//
//    //keeper.print(".*",kFALSE); // print all files names (kTRUE: print also source code)
//    //keeper.extract(destinationdir,replacedir,".*"); // extract source code matching the regexp to destinationdir, replace replacedir in path names
//    // TMacro* m=keeper.getFile(name); // return NULL if nothing was found
//
//    keeper.Write();
//
//
//    out->Save();
//    out->Close();
//
////////////////////////////////////////////////////////////////////////////////

ClassImp(HSrcKeeper)


HSrcKeeper::HSrcKeeper(const char* name,const char* title)
    : TNamed(name,title)
{

}

HSrcKeeper::~HSrcKeeper(void)
{

}

TMacro*  HSrcKeeper::newFile(TString fname)
{
    // opens the input source fname
    // and creates TMacro by addiding line
    // by line the source code. If the soure
    // cannot be read Null will be returned

    TString n = fname;
    TMacro* m = NULL;

    ifstream input;
    input.open(fname);

    if(input.fail()) return m;

    m = new TMacro();
    m ->SetName(n.Data());

    Char_t line[4000];
    while(!input.eof()){
	input.getline(line,4000);
	m->AddLine(line);
    }
    input.close();
    return  m;
}

Bool_t HSrcKeeper::addSourceFile(TString name)
{
    // add a single source file. return kFALSE
    // if a problems occurs.
    
    TMacro* m = newFile(name);
    if(m) {
	cout<<name.Data()<<endl;
	source.AddLast(m);
	return kTRUE;
    }
    else  Error("addSourceFiles()","could not create macro for %s !",name.Data());
    return kFALSE;
}


Int_t HSrcKeeper::addSourceFiles(TString dir,TString regexp)
{
    // add all source files matching regexp
    // subdir will be ignored. returns the number of
    // added files
    vector<TString> files;
    HFileSys::lsFiles(dir,files,kTRUE);

    sort(files.begin(),files.end());
    TPRegexp expr(regexp);
    Int_t n=0;
    for(UInt_t i=0;i<files.size();i++){
	TString name = files[i];
	if(name(expr) != "") {
	    cout<<name.Data()<<endl;
	    TMacro* m = newFile(name);
	    if(m) {source.AddLast(m);n++;}
	    else  Error("addSourceFiles()","could not create macro!");
	}
    }
    cout<<"added n "<<n<<" files"<<endl;
    return n;


}

Int_t HSrcKeeper::addSourceFilesRecursive(TString dir,TString regexp)
{
    // add all source files of dir and its subdirs matching regexp
    // returns the number of added files

    vector<TString> files;
    HFileSys::lsFilesRecursive(dir,files);

    sort(files.begin(),files.end());
    TPRegexp expr(regexp);
    Int_t n=0;
    for(UInt_t i=0;i<files.size();i++){
	TString name = files[i];
	if(name(expr) != "") {
	    cout<<name.Data()<<endl;
	    TMacro* m = newFile(name);
	    if(m) {source.AddLast(m);n++;}
	    else  Error("addSourceFiles()","could not create macro!");
	}
    }
    cout<<"added n "<<n<<" files"<<endl;
    return n;
}

void HSrcKeeper::print(TString regexp,Bool_t show)
{
    // print all files matching regexp (".*" for all files)
    // if show==kTRUE the source code is printed too
    TPRegexp expr(regexp);
    TString name;
    for(Int_t i=0;i<source.GetEntries();i++){
	TMacro* m = (TMacro*) source.At(i);
        name = m->GetName() ;
	if(name(expr)!= ""){
	    
	    if(show)  {
                cout<<"-------------------------------------------------------------"<<endl;
		cout<<m->GetName()<<endl;
		m->Print();
	    } else cout<<m->GetName()<<endl;
	}
    }
}

void HSrcKeeper::extract(TString destinationdir,TString replacedir, TString regexp)
{
    // extracts all matching source file to destination dir. subdirs
    // will be created. if replacedir is set the original path of the
    // file will be replaced in the beginning of path by ""

    TPRegexp expr(regexp);
    if(destinationdir.EndsWith("/")) destinationdir.Replace(destinationdir.Length()-1,1,"");
    if(replacedir.EndsWith("/")    ) replacedir    .Replace(replacedir    .Length()-1,1,"");
    if(gSystem->AccessPathName(destinationdir.Data())!=0){
	if( gSystem->mkdir(destinationdir.Data()) ==-1) {
	    Error("extract()","Could not create destination dir %s. skipped.",destinationdir.Data());
            return;
	}
    }

    TString name;
    for(Int_t i=0;i<source.GetEntries();i++){
	TMacro* m = (TMacro*) source.At(i);
        name = m->GetName() ;
	if(name(expr)!= ""){
	    cout<<m->GetName()<<endl;
	    name.Replace(0,replacedir.Length(),"");
	    TString dir = gSystem->DirName(name.Data());
	    if(gSystem->AccessPathName(Form("%s/%s",destinationdir.Data(),dir.Data()))!=0)
	    {
		if( gSystem->mkdir(Form("%s/%s",destinationdir.Data(),dir.Data()),kTRUE) ==-1)
		{
		    Error("extract()","Could not create destination dir %s. skipped.",destinationdir.Data());
                    continue;
		}
	    }
	    m->SaveSource(Form("%s/%s",destinationdir.Data(),name.Data()));
	}
    }
}

TMacro* HSrcKeeper::getFile(TString fname,Bool_t usePath)
{
    // get TMacro of file fname. returns 0 if
    // the file does not exist in the container
    if(fname == "") return 0;
    TString name;
    for(Int_t i=0;i<source.GetEntries();i++){
	TMacro* m = (TMacro*) source.At(i);
	name = m->GetName() ;
        if(!usePath) name = gSystem->BaseName(name.Data());
	if(name.CompareTo(fname.Data())==0) return m;
    }
    return 0;
}

Long64_t HSrcKeeper::Merge(TCollection *li){

    // dummy function for hadd like merge of hist files

    if (!li) return 0;
    return (Long64_t) 1;

}

