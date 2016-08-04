#include "TClass.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TString.h"
#include "TApplication.h"
#include "honlinemonclientmain.h"
#include <iostream>
#include <cstdlib>


using namespace std;

void usage()
{
    cout << "usage:" << endl;
    cout << "a) to get a list of available histograms from the server or" << endl;
    cout << "b) start client with configuration from xml file" << endl;
    cout << endl;
    cout << "a) hadesonlineclient <serverhostname> <port> list" << endl;
    cout << "b) hadesonlineclient <xml-filename>" << endl;
    
    exit(1);
}

int main(int argc, char *argv[]) {
	
    TString hostname = "";
    TString port = "";
    Int_t serverport = 0;
    TString cmd = "";
    TString configfile = "";

    gStyle->SetPalette(1);

    if (argc == 4) {
        // a) get list of histograms from server
        hostname = argv[1];
        port = argv[2];
        serverport = atoi(port);
        cmd = argv[3];
        TApplication app("testapp", &argc, argv);
	    HOnlineMonClientMain *clientmain = new HOnlineMonClientMain();
	    clientmain->Init(kTRUE, hostname, serverport, cmd);
	    delete clientmain;
        return 0;
    } else {
        if (argc == 2) {
            // b) start client with configfile
            configfile = argv[1];
            TApplication app("testapp", &argc, argv);
	        HOnlineMonClientMain *clientmain = new HOnlineMonClientMain(configfile);
	        clientmain->Init();
	        delete clientmain;
	        return 0;
        } else {
            // error
            usage();
            return 1;
        }
    }

	return 0;
}
