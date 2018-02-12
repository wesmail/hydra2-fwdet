Int_t run_rich_digi_par_creator(Int_t nEvents=100000000, Int_t startEvt=0)
{
    TString inFiles = "/lustre/nyx/cbm/users/slebedev/hades/2018.rich_params.dst.e.*.root";

    string outputFilePath = "richpar_output.dat";
    string outputHistFilePath = "richpar_hist.root";

    TString asciiParFile = "rich_params_nowls.txt";

    TString rootParFile = "/cvmfs/hades.gsi.de/param/sim/apr12/allParam_APR12_sim_run_12001_gen8_27012016.root";
    TString paramSource = "ascii,root"; // root, ascii, oracle
    TString paramRelease = "APR12SIM_dst_gen8a"; // 27012016

    // create loop obejct and hades
    HLoop loop(kTRUE);

    Int_t refId = 12001; // apr12sim_mediumfieldalign_auau;
    TString beamtime = "apr12";

    HDst::setupSpectrometer(beamtime,NULL,"start,rich,mdc,tof,rpc,shower,wall");
    HDst::setupParameterSources(paramSource,asciiParFile,rootParFile,paramRelease);

    Bool_t ret = kFALSE;
    if(inFiles.Contains(",")){
        ret = loop.addMultFiles(inFiles);// file1,file2,file3
    } else{
        ret = loop.addFiles(inFiles); // myroot*.root
    }
    if(ret == kFALSE) {
    	cout<<"READBACK: ERROR : cannot find inputfiles : "<<inFiles.Data()<<endl;
    	return 1;
    }

    if(!loop.setInput("-*,+HGeantKine,+HRichHitSim")) {
       cout << "READBACK: ERROR : cannot read input !" << endl;
       exit(1);
    }
    loop.printCategories();
    loop.printChain();


    HTaskSet *masterTaskSet = gHades->getTaskSet("all");

    HRich700DigiParCreator *digiParCreator = new HRich700DigiParCreator();
    digiParCreator->setOutputFilePath(outputFilePath);
    digiParCreator->setOutputHistFilePath(outputHistFilePath);
    masterTaskSet->add(digiParCreator);

    Int_t entries = loop.getEntries();
    if(nEvents < entries && nEvents >= 0 ) entries = nEvents;

    for (Int_t i = 0; i < entries; i++) {
        Int_t nbytes =  loop.nextEvent(i);
        if(nbytes <= 0) {
            cout<<nbytes<<endl;
            break;
        }
    }

    if(gHades)gHades->finalizeTasks();

    delete gHades;

    return 0;
}
