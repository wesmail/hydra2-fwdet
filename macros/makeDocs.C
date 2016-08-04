#ifndef __CINT__

#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include <cstdio>
#include <iostream>


using namespace std;

#endif


TString getSourceDirs(TString dirname = "/misc/hadessoftware/etch32/hades/hydra-dev/")
{
    TString     files = "";
    FILE*       pipe  = NULL;
    TString     output;


    TString cmd = "find  " + dirname + " -type d | xargs -i echo {} \
	| grep -v build | grep -v include | grep -v macros | grep -v admin | grep -v CVS | grep -v Go4  \
	| grep -v Parameter | grep -v tools | grep -v Monitor | grep -v scripts   \
	| grep -v bin | grep -v data | grep -v ntuple \
	| xargs -i echo -n {} | sed 's/\\/misc/:\\/misc/g'";


    pipe = gSystem->OpenPipe( cmd.Data(), "r" );
    output.Gets( pipe );

    output.Remove(TString::EStripType::kLeading, ':');

    files = output;
    if (gSystem->ClosePipe( pipe )) return NULL;

    return files;
}

void makeDocs()
{
    gROOT->Reset();

    TString rootpath="http://root.cern.ch/root/html522/";
    TString docuout="/misc/halo/web-docs/docs/hydra/classDocumentation/root5.22/out/";

    cout<<"#################################################"<<endl;
    cout<<"OUTPUTPATH= "<<gSystem->ExpandPathName(docuout.Data())<<endl;
    cout<<"ROOTPATH= "  <<rootpath.Data()<<endl;
    cout<<"#################################################"<<endl;

    TString dirs = getSourceDirs();
    cout<<dirs.Data()<<endl;



    gEnv->SetValue("Root.Html.OutputDir"       ,docuout.Data());
    gEnv->SetValue("Root.Html.Root"            ,rootpath.Data());
    gEnv->SetValue("Unix.*.Root.Html.SourceDir",dirs.Data());

    gEnv->SetValue("Unix.*.Root.IncludePath"   ,"-I. $(HADDIR)/include");

    gEnv->SetValue("Root.Html.SourcePrefix"    ,"$(HADDIR)");
    gEnv->SetValue("Root.Html.Description"     ,"//_HADES_CLASS_DESCRIPTION");

    gEnv->PrintEnv();

    THtml html;
    html.SetDotDir("/usr/bin/");
    html.SetProductName("HYDRA - THE HADES ANALYSIS PACKAGE");
    html.MakeAll();


}


