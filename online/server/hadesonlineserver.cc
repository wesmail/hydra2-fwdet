

#include "hades.h"
#include "hruntimedb.h"
#include "honlineclientservercom.h"
#include "honlinemonhist.h"

#include "TApplication.h"
#include "TString.h"
#include "TList.h"

#include "helpers.h"
#include "createHades.C"
#include "Mdc.C"
#include "Rich.C"
#include "Tof.C"
#include "Shower.C"
#include "Wall.C"
#include "PionTracker.C"
#include "Hodo.C"
#include "Rpc.C"
#include "Start.C"
#include "Physics.C"



#include <map>
#include <vector>
#include <iostream> 
#include <iomanip>
#include <cstdlib>
#include <unistd.h>
using namespace std;

map < TString , HOnlineMonHistAddon* > currentMap;


void usage()
{
    cout << "usage: OnlineServer servername serverhostname connectorport parameterfile"<<endl;
    cout << "EXAMPLE ./hadesonlineserver.exe  SUPER lxg0453  9876  analysisParams.txt"<<endl;
    exit(1);
}

int main(int argc, char **argv)
{
    TString name;
    TString hostname;
    TString connector;
    UInt_t  con;
    TString param;

    if(argc < 4)
    {
	usage();
	return 1;
    }
    else{
	name      = argv[1];
	hostname  = argv[2];
	connector = argv[3];
	con       = atoi(connector);
	param     = argv[4];
	cout << "Server:"<<name.Data()<<",\tHost:"<<hostname.Data()<<",\tConnector:"<<con<<",\n Parameter:"<<param.Data()<<endl;

	TApplication theApp("App", &argc, argv);

        //--------------------------------------------------------------------
        // setup Hades

        Int_t   datasource   = 1; // 0 = hld, 1 = hldgrep 2 = hldremote
	Int_t   refId        = 1;
        TString eventbuilder = "lxhadeb02.gsi.de";         // needed by dataosoure = 2
	TString inputDir     = "/home/hades-qa/DataMount/";
//	TString inputDir     = "/home/hadaq/cerberos/tmphld/";

	TString inputFile    = "";      // needed by dataosoure = 1
	TString paramSource  ="ASCII,ROOT";  // ASCII, ROOT, ORACLE (ASCII always first input, ORACLE sencond if ASCII or ROOT is used)
        TString asciiParFile ="/home/hades-qa/online/may14/params/params_start_piontracker_May14.txt";  // can be empty if not used
	TString rootParFile  ="/home/hades-qa/online/may14/params/paramsFromOra_June14_MDC.root";

        Int_t evtCtInterval  = 100;

	Bool_t ok = createHades(datasource,
				inputDir,
				inputFile,
				refId,
				eventbuilder,
				paramSource,
				asciiParFile,
				rootParFile
			       );
	if(!ok){
             cout<<"Error: Hades could not be created!"<<endl;
             exit(1);
	}
	//--------------------------------------------------------------------



	//--------------------------------------------------------------------
        // create connection class
	HOnlineClientServerCom* clientservercom = new HOnlineClientServerCom(name, hostname, con);
	clientservercom -> setDebug(kFALSE);
	clientservercom ->setEvtCounterInterval(evtCtInterval);

	if (!clientservercom->init()) {
	    cout<<"Error: Hades Monitoring Server could not be initialised."<<endl;;
	    exit(1);
	}

	cout << "Hades Monitoring Server initialised"<<endl;
	cout << "Created Online Hades Server Client Instance: " << name.Data() << endl;
        //--------------------------------------------------------------------

	gHades->eventLoop(2);
        gHades->getRuntimeDb()->saveOutput();

	cout<<"Hades Monitoring Server: ADD Histograms"<<endl;


	//####################### USER CODE ##############################################
	// CREATE Histograms
	// Names must be unique !! NAMING CONVENTIONS :  h{det}somename

	mapHolder::getMapHolder();     // create mapHolder object (helpers.h)

	TList histpool;                // list of all histograms

	createHistMdc        (histpool);
	createHistRich       (histpool);
	createHistShower     (histpool);
	createHistRpc        (histpool);
	createHistTof        (histpool);
	createHistWall       (histpool);
        createHistPionTracker(histpool);
        createHistHodo       (histpool);
        createHistStart      (histpool);
        createHistPhysics    (histpool);


	printList(histpool);           // (helpers.h)


	//###############################################################################

	vector < fillptrHOnlineClientServerCom > fillfunctions;


	fillfunctions.push_back(&fillHistMdc);
        fillfunctions.push_back(&fillHistRich);
	fillfunctions.push_back(&fillHistShower);
	fillfunctions.push_back(&fillHistRpc);
	fillfunctions.push_back(&fillHistTof);
	fillfunctions.push_back(&fillHistWall);
        fillfunctions.push_back(&fillHistPionTracker);
        fillfunctions.push_back(&fillHistHodo);
        //fillfunctions.push_back(&fillHistStart);
        fillfunctions.push_back(&fillHistPhysics);



	cout<<"Hades Monitoring Server: START eventloop"<<endl;
 	clientservercom->eventLoop(&fillfunctions); // start eventLoop thread
        clientservercom->processClients(histpool);  // start communication loop


        cout<<"DELETING HADES!"<<endl;
        delete clientservercom;
 	delete gHades;

	return 0;
    }
}

