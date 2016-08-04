
#ifndef __CINT__
using namespace std;
# include <iostream>
# include <cstdlib>
# include "TreeMacro.h"

#endif

Int_t TreeAnalysis()
{
    TChain* Chain= new TChain("T");

    //Chain->Add("/lustre/hades/user/jsiebens/qa/227/root/QA_227_FullTree.root");
    Chain->Add("QA_227_FullTree_1.root");

    char * home, *user;
    home = getenv("HOME");
    if (!home) {
            std::cerr << "ERROR: HOME variable doesn't exists! Exiting....\n";
	    exit(EXIT_FAILURE);
    }

    user = getenv("USER");
    if (!home) {
	std::cerr << "ERROR: USER variable doesn't exists! Exiting....\n";
	exit(EXIT_FAILURE);
    }

    TString ofilename("");
    ofilename.Append("./QAHistogram_");
    ofilename.Append(user);
    ofilename.Append(".root");

    TFile *outputfile = new TFile(ofilename,"RECREATE");

    AnalysisMacro AM(outputfile,Chain);

    AM.Loop();
    AM.finalize(outputfile);
       
    return 0;
}

#ifndef __CINT__
int main(int argc, char **argv)
{
    return TreeAnalysis();
}
#endif
