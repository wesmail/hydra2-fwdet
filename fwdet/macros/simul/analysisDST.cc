#include "analysisDST.h"
#include "TSystem.h"


Int_t analysisDST(TString inFile="tmp1", Int_t nEvents=1000, Int_t startEvt=0)
{
    Hades *myHades=NULL;
    TStopwatch timer;
    myHades=new Hades;
    myHades->setTreeBufferSize(8000);
    printf("Setting configuration...+++\n");

    TString inputFile = inFile + ".root";
    TString outFile   = inFile + "_dst.root";

    TString asciiParFile = "fwdetdigipar.txt";
    TString rootParFile = "";
    Bool_t ora_input=kFALSE;

    //------List-Of-Categories-which-are-notneeded-in-this-DST-production------//
    Cat_t notPersistentCat[] =
      {
      };

    HSpectrometer* spec = gHades->getSetup();
    Int_t fwdetMods[]   = {1,1,0,0,1,0,1,0,0};
    spec->addDetector(new HFwDetDetector);
    spec->getDetector("FwDet")->setModules(-1,fwdetMods);

    HRuntimeDb *rtdb = myHades -> getRuntimeDb();

    if (!asciiParFile.IsNull()) {
	HParAsciiFileIo *input1=new HParAsciiFileIo;
	input1->open((Text_t*)asciiParFile.Data(),"in");
	rtdb->setFirstInput(input1);
    }

    if (!rootParFile.IsNull()) {
	HParRootFileIo *input2=new HParRootFileIo;
	input2->open((Text_t*)rootParFile.Data(),"READ");
	rtdb->setSecondInput(input2);
    }

#ifdef ORACLE_SUPPORT
    if(ora_input) {
	HParOra2Io *ora=new HParOra2Io() ;
	ora->open();
	ora->setHistoryDate("now");
	if (!ora->check()) {
	    Error("ora->check","no connection to Oracle \n exiting...!");
	    exit(EXIT_FAILURE);
	}
	rtdb->setSecondInput(ora);
    }
#endif

    //Data source
    HRootSource *source=new HRootSource;
    //    source->setDirectory("");
    source->addFile((Text_t*)inputFile.Data());
    myHades->setDataSource(source);

    // ----------- Build TASK SETS (using H***TaskSet::make) -------------
    HFwDetTaskSet* fwdetTaskSet = new HFwDetTaskSet("FwDetStrawTaskSet","FwDetStrawTaskSet");
    HTask* fwdetTasks = fwdetTaskSet->make("simulation","strawcal,rpccal,rpchitf,vf");

    //------------------------ Master task set ---------------------------
    HTaskSet *masterTaskSet = gHades->getTaskSet("simulation");
    masterTaskSet->add(fwdetTasks);

    myHades->makeCounter(100);

    if (!myHades->init()){
	Error("init()","Hades did not initialize ... once again");
	exit(1);
    }

    //----------------- Set not persistent categories --------------------
    HEvent *event = myHades->getCurrentEvent();
    for(UInt_t i=0;i<sizeof(notPersistentCat)/sizeof(Cat_t);i++){
	HCategory *cat = ((HCategory *)event->getCategory(notPersistentCat[i]));
	if(cat)cat->setPersistency(kFALSE);
    }

    gHades->setOutputFile((Text_t*)outFile.Data(),"RECREATE","Test",2);
    myHades->makeTree();
    Int_t nProcessed = myHades->eventLoop(nEvents,startEvt);
    printf("Events processed: %i\n",nProcessed);

    cout<<"--Input file:   "<<inputFile<<endl;
    cout<<"--Output file:  "<<outFile<<endl;

    printf("Real time: %f\n",timer.RealTime());
    printf("Cpu time: %f\n",timer.CpuTime());
    if (nProcessed) printf("Performance: %f s/ev\n",timer.CpuTime()/nProcessed);

    //    rtdb->saveOutput();
    //    rtdb->print();

    delete myHades;
    timer.Stop();

    return EXIT_SUCCESS;

}

int usage(const TString name)
{
    Info(name.Data(),"Usage:");
#ifdef __CINT__
    TString es=".cc(";
    TString sp=",";
    TString e=")";
#else
    TString es=" ";
    TString sp=" ";
    TString e=" ";
#endif
    Info(name.Data(),"          inFile: %s%sinputfile [%snumber of events [%sstart evt]]%s",name.Data(),es.Data(),sp.Data(),sp.Data(),e.Data());
    Info(name.Data(),"          get output file name: %s%sinputfile%sOUTPUTNAME%s",name.Data(),es.Data(),sp.Data(),e.Data());
    Info(name.Data(),"          get output dir name : %s%sinputfile%sOUTPUTDIR%s",name.Data(),es.Data(),sp.Data(),e.Data());
    Info(name.Data(),"          get input  dir name : %s%sinputfile%sINPUTDIR%s",name.Data(),es.Data(),sp.Data(),e.Data());
    Info(name.Data(),"          get executable path : %s%sinputfile%sEXECUTABLE%s",name.Data(),es.Data(),sp.Data(),e.Data());

    return EXIT_SUCCESS;
}

#ifndef __CINT__
int main(int argc, char **argv)
{
    TROOT AnalysisDST("AnalysisDST","compiled analysisDST macros");
    switch (argc)
    {
    case 1:
	return usage(TString(argv[0]));
	break;
    case 2:
	return analysisDST(TString(argv[1]));
	break;
    case 3:
	if (atoi(argv[2])!=0)
	{
	    return analysisDST(TString(argv[1]),atoi(argv[2]));
	}
	else
	{
	    TString name = argv[2];
	    name.ToUpper();
	    if (name.CompareTo("OUTPUTNAME")==0)
	    {
		return analysisDST(TString(argv[1]),-1111);
	    }
	    if (name.CompareTo("OUTPUTDIR")==0)
	    {
		return analysisDST(TString(argv[1]),-2222);
	    }
	    if (name.CompareTo("INPUTDIR")==0)
	    {
		return analysisDST(TString(argv[1]),-3333);
	    }
	    if (name.CompareTo("EXECUTABLE")==0)
	    {
		printf("executable %s\n",gSystem->DirName(argv[0]));
		return EXIT_SUCCESS;
	    }
	    return usage(TString(argv[0]));
	}
	break;
    case 4:
	if ((atoi(argv[2])!=0) && (atoi(argv[3])!=0))
	{
	    return analysisDST(TString(argv[1]),atoi(argv[2]),atoi(argv[3]));
	}
	else
	{
	    return usage(TString(argv[0]));
	}
	break;
    default:
	usage(TString(argv[0]));
	return EXIT_SUCCESS;
    }
}
#endif


