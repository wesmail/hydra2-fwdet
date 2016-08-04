#include "TSystem.h"
#include "TString.h"

#include "hhistmap.h"

#include <cstdlib>
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;

#include <getopt.h>

int main(int argc, char **argv)
{

    TString help = "usage: histmapadd -i input -o output  \n"
	" \t examples: \n"
	" \t   histmapadd -i \"histmap_file_*.root\" -o merge.root [has to end with .root] \n"
        " \t   histmapadd -i \"histmap_file_1.root,histmap_file_2.root,histmap_file_3.root\" -o merge.root \n"
        " \t   histmapadd -i \"filelist.txt\" -o merge.root \n";


    if(argc < 3)
    {
	fprintf (stderr, "Error: Requiring inputfile and outputfile name!\n");
	fprintf (stdout,"%s",help.Data());
	return 1;
    }
    else{

	Int_t c;
	opterr = 0;


	TString inputfile="";
        TString outputfile="";


	//---------------------------------------------------------------
        // get options from command line
	while ((c = getopt (argc, argv, ":i:o:")) != -1){   // ':' = required argument, '::' = optional argument
	    switch (c)
	    {
	    case 'i':
		inputfile = optarg;
		break;
	    case 'o':
		outputfile = optarg;
		break;
	    default:
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
		fprintf (stdout,"%s",help.Data());
	        return 1;
	    }
	}
        //---------------------------------------------------------------
        if(inputfile.EndsWith(",")) inputfile.Replace(inputfile.Length()-1,1,"");

	cout<<"input  file = "<<inputfile.Data() <<";"<<endl;
	cout<<"output file = "<<outputfile.Data()<<";"<<endl;
	HHistMap::addHistMap(inputfile,outputfile);

    }

    return 0;
}
