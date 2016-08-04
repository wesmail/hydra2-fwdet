#include "hzip.h"

#include "TFile.h"
#include "TSystem.h"
#include "TString.h"
#include "TList.h"
#include "TObjString.h"
#include "TObjArray.h"

#include <cstdlib>
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;

#include <getopt.h>

int main(int argc, char **argv)
{
    TString zipname   = "unset";
    TString membername= "unset";
    TString filter    = "unset";
    TString filelist  = "unset";


    TString help = "usage: hzip -o zipfile [-i filefilter] [-f filelist] [-u outputdir] [-msth] \n"
	           " \t -f input ascii filelist (1 file per line) \n"
                   " \t -h help \n"
                   " \t -i input filefilter (like \"be*.root\") \n"
                   " \t -l list file in zip files \n"
	           " \t -m maxsize of file [bytes, max 20GB] (default = 2 Gbyte, will be splitted if larger) \n"
	           " \t -o outputzip file name (required) \n"
	           " \t -s save mode. do not overide existing zip files (default is overwrite) \n"
	           " \t -t test. show what would be done \n"
	           " \t -u dir unzip zip files to dir \n"
                   " \t -w print in which file membername is contained\n"
	           " \t examples: \n"
	           " \t test zip root files      : hzip -t -o test.zip -i \"/mydir/be*.root\" \n"
	           " \t zip root files           : hzip -o test.zip -i \"/mydir/be*.root\" \n"
	           " \t zip root files from list : hzip -o test.zip -f filelist \n"
	           " \t unzip root files to dir  : hzip -i \"test_*.zip\" -u /mydir \n"
	           " \t list files in zip files  : hzip -i \"test_*.zip\" -l  \n";



    Long64_t  maxsize = 20000000000;   // 20GB
    Long64_t  defsize = 2000000000 ;   // 2 GB

    if(argc < 2)
    {
	fprintf (stderr, "Error: Requiring at least output zip file name!\n");
	fprintf (stdout,"%s",help.Data());
	return 1;
    }
    else{

	Int_t c;
	opterr = 0;

        Int_t input    = 0;
        Bool_t test    = kFALSE;
	Bool_t save    = kFALSE;
	Bool_t listZip = kFALSE;
	Bool_t unZip   = kFALSE;
        Bool_t find    = kFALSE;
        TString  unZipDir ="unset";

	Long64_t tmp=0;
	//---------------------------------------------------------------
        // get options from command line
	while ((c = getopt (argc, argv, ":i:f:o:m:u:w:tslh")) != -1){   // ':' = required argument, '::' = optional argument
	    switch (c)
	    {
	    case 'h':
		fprintf (stdout,"%s",help.Data());
                return 0;
		break;
	    case 'i':
		filter = optarg;
		break;
            case 'f':
		filelist = optarg;
		break;
	    case 'o':
		zipname = optarg;
		break;
	    case 'm':
                tmp = atol(optarg);
		if(tmp < maxsize) defsize = tmp;
		break;
	    case 't':
		test = kTRUE;
		break;
	    case 'l':
		listZip = kTRUE;
		break;
	    case 'u':
		unZip = kTRUE;
                unZipDir = optarg;
		break;
	    case 'w':
		membername = optarg;
                find = kTRUE;
		break;
	    case 's':
		save = kTRUE;
		break;
	    case ':':   // option i,o,l without argument
               fprintf (stderr, "Option -%c requires an argument.\n", optopt);
               return 1;
	       break;
	    case '?':
		fprintf(stderr,"Unrecognized option: -%c\n", optopt);
                return 1;
		break;
	    default:
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
		fprintf (stdout,"%s",help.Data());
	        return 1;
	    }
	}
        //---------------------------------------------------------------

	//---------------------------------------------------------------
        // do some checking on input parameters
	if(zipname == "unset" && !listZip && !unZip && !find) {
	    fprintf (stderr, "Output file name is required!\n");
	    fprintf (stdout,"%s",help.Data());
           return 1;
	}
        if(filelist != "unset" && filter   != "unset") {
	    fprintf (stderr, "File selection has to be \'-l filelist\' or \'-i filefilter\'!\n");
	    fprintf (stdout,"%s",help.Data());
	    return 1;
	}
        if(filelist == "unset" && filter   == "unset") {
	    fprintf (stderr, "File selection has to be \'-l filelist\' or \'-i filefilter\'!\n");
	    fprintf (stdout,"%s",help.Data());
	    return 1;
	}
	if(filelist != "unset" ){
	    if(gSystem->AccessPathName(filelist.Data())){
		fprintf (stderr, "List File '%s' does not exist!\n",filelist.Data() );
		fprintf (stdout,"%s",help.Data());
                return 1;
	    }

	}
        //---------------------------------------------------------------


        //---------------------------------------------------------------
        // prepare file lists
	TObjArray* files = 0;

	    cout<<"  output file                  : "<<zipname.Data()
	    <<", \n  filter                       : "<<filter.Data()
	    <<", \n  filelist                     : "<<filelist.Data()
            <<", \n  do not overwrite zip files   : "<<(Int_t)save
            <<", \n  do test run                  : "<<(Int_t)test
            <<", \n  list zip                     : "<<(Int_t)listZip
            <<", \n  unzip to dir                 : "<<unZipDir
            <<", \n  max file size [byte]         : "<<defsize
	    <<endl;

	if(filter != "unset"){
	    files = HZip::glob(filter);
	}
	if(filelist != "unset"){
	    files = HZip::readFileList(filelist);
	}
        //---------------------------------------------------------------

        //---------------------------------------------------------------
        // prepare output file
	FileStat_t fileinfo;
	Int_t nfiles = 0;

	if(files) nfiles = files->GetEntries();
	Long64_t sumSize   = 0;
	Long64_t totalSize = 0;
	zipname.ReplaceAll(".zip","");
        TString zipName = zipname;
        //---------------------------------------------------------------

	if(listZip){
           for(Int_t j=0;j<nfiles;j++){
               TString fna = ((TObjString*)(files->At(j)))->GetString();
	       HZip::list(fna);
	   }
           if(files) delete files;
           return 0;
	}
        if(unZip){
           for(Int_t j=0;j<nfiles;j++){
               TString fna = ((TObjString*)(files->At(j)))->GetString();
	       if(!test) HZip::unzip(fna,unZipDir);
	       else {
                 cout<<setw(5)<<j<<" unzip "<<fna.Data()<<" to "<<unZipDir.Data()<<endl;
	       }
	   }
	   if(files) delete files;
           return 0;
	}

	if(find){
            cout<<endl;
	    for(Int_t j=0;j<nfiles;j++){
               TString fna = ((TObjString*)(files->At(j)))->GetString();
	       Bool_t found = HZip::isInside(fna, membername , kFALSE);
	       if(found) cout<<"File : "<<fna.Data()<< " contains a member "<<membername.Data()<<endl;
	   }
           return 0;
	}



        //---------------------------------------------------------------
        // loop over file list
	Int_t fileNum = 0;
	for(Int_t j=0;j<nfiles;j++){
	    //---------------------------------------------------------------
            // for first file only
	    if(j == 0) {
		fileNum++;
		zipName = Form("%s_%03i.zip",zipname.Data(),fileNum);

		if(save && HZip::exists(zipName,kTRUE,kTRUE)){ // silent check
		    fprintf (stderr, "zipFile = '%s' exists already !\n",zipName.Data() );
		    if(!test) {
                        if(files) delete files;
			return 1;
		    }
		}
		if(!save && HZip::exists(zipName,kTRUE,kTRUE)){
                    gSystem->Unlink(zipName.Data());
		}
	    }
	    //---------------------------------------------------------------

	    TString fna = ((TObjString*)(files->At(j)))->GetString();
	    gSystem->GetPathInfo(fna.Data(), fileinfo);



	    totalSize += fileinfo.fSize;

	    //---------------------------------------------------------------
            // add file to zip , change file if needed
	    if(sumSize+fileinfo.fSize<defsize){ // old file
		sumSize+=fileinfo.fSize;
		cout<<setw(5)<<j<<" "<<fna.Data()
		    <<" = "<<fileinfo.fSize<<" bytes, "
		    <<" total "<<totalSize
		    <<", this file "<<sumSize
		    <<" "<<zipName.Data()<<endl;

		if(fileinfo.fSize > defsize){
                    fprintf (stderr, "File = '%s' is larger than maximum zipfile size !\n",fna.Data() );
		    if(!test) {
                        if(files) delete files;
			return 1;
		    }
		}

		if(!test) HZip::addFile(zipName,fna);

	    } else {  // new file
	       fileNum++;
	       zipName = Form("%s_%03i.zip",zipname.Data(),fileNum);

	       if(save && HZip::exists(zipName,kTRUE,kTRUE)){ // silent check
		   fprintf (stderr, "zipFile = '%s' exists already !\n",zipName.Data() );
		   if(!test) {
                       if(files) delete files;
		       return 1;
		   }
	       }

	       if(!save && HZip::exists(zipName,kTRUE,kTRUE)){
		   gSystem->Unlink(zipName.Data());
	       }

	       sumSize=fileinfo.fSize;
	       cout<<setw(5)<<j<<" "<<fna.Data()
		   <<" = "<<fileinfo.fSize<<" bytes, "
		   <<" total "<<totalSize
		   <<", this file "<<sumSize
		   <<" "<<zipName.Data()<<endl;

	       if(fileinfo.fSize > defsize){
		   fprintf (stderr, "File = '%s' is larger than maximum zipfile size !\n",fna.Data() );
		   if(!test) {
                       if(files) delete files;
		       return 1;
		   }
	       }

	       if(!test) HZip::addFile(zipName,fna);
	    }
           //---------------------------------------------------------------
	    
	}
	//---------------------------------------------------------------
        if(files) delete files;
        cout<<fileNum<<" files with "<<totalSize<<" bytes zipped"<<endl;

    }

    return 0;
}
