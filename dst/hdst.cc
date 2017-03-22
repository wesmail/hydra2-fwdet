
#include "hdst.h"

//------- detectors -------------------
#include "hspectrometer.h"
#include "hrichdetector.h"
#include "hmdcdetector.h"
#include "htofdetector.h"
#include "hrpcdetector.h"
#include "hshowerdetector.h"
#include "hemcdetector.h"
#include "htboxdetector.h"
#include "hstart2detector.h"
#include "hwalldetector.h"
#include "hpiontrackerdetector.h"
//-------------------------------------

//--------- data source ---------------
#include "hldsource.h"
#include "hldfilesource.h"
#include "hldgrepfilesource.h"
#include "hldremotesource.h"
#include "hrootsource.h"
#include "hgeantmergesource.h"
//-------------------------------------

//-------- unpackers -----------------
#include "hmdcunpacker.h"
#include "hshowerunpacker.h"
#include "hrichunpacker.h"
#include "hwalltrb2unpacker.h"
#include "hrpctrb2unpacker.h"
#include "htoftrb2unpacker.h"
#include "hstart2trb2unpacker.h"
#include "hstart2trb3unpacker.h"
#include "hlatchunpacker.h"
#include "htboxunpacker.h"
#include "hpiontrackertrb3unpacker.h"
//-------------------------------------


//-------- parameters -----------------
#include "hades.h"
#include "hruntimedb.h"
#ifdef ORACLE_SUPPORT
  #include "hparora2io.h"
#endif
#include "hparasciifileio.h"
#include "hparrootfileio.h"
//-------------------------------------

#include "TObjString.h"


#include <iostream>
using namespace std;


ClassImp(HDst)

void HDst::setupSpectrometer(TString beamtime,Int_t mdcsetup[6][4],TString detectors)
{
    // beamtime aug11, aug11_3sec ,apr12
    // Int_t mdcset[6][4] setup mdc. If not used put NULL (default).
    // if not NULL it will overwrite settings given by beamtime
    // detectors (default)= rich,mdc,tof,rpc,shower,wall,tbox,start

    beamtime.ToLower();
    detectors.ToLower();
    ::Info("setupSpectrometer()", "\n----------- setting up HADES spectrometer ----------------");

    if(beamtime.CompareTo("may14")==0) beamtime = "jul14";
    if(beamtime.CompareTo("aug14")==0) beamtime = "jul14";

    if     (beamtime.CompareTo("aug11")     ==0);
    else if(beamtime.CompareTo("aug11_3sec")==0);
    else if(beamtime.CompareTo("apr12")     ==0);
    else if(beamtime.CompareTo("jul14")     ==0);
    else {
        ::Error("","Beam time = '%s' not supported !",beamtime.Data());
        exit(1);
    }
    ::Info("setupSpectrometer()", "Using %s setup",beamtime.Data());

    Int_t mdcMods_aug11[6][4]={
        {1,1,1,1},
        {1,1,0,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1} };

    Int_t mdcMods_aug11_3sec[6][4]={
        {0,0,0,0},
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {1,1,1,1},
        {1,1,1,1} };

    Int_t mdcMods_apr12[6][4]={
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1} };


    Int_t mdcMods[6][4];

    ::Info("", "setting up MDC modules");
    if(mdcsetup) ::Info("", "settings will be taken by input array");
    else         ::Info("", "settings will be taken by %s",beamtime.Data());
    for(Int_t s=0;s<6;s++){
        cout<<"    sec "<<s;
        for(Int_t m=0;m<4;m++){
            if(mdcsetup == NULL){
                if     (beamtime.CompareTo("aug11")     ==0) mdcMods[s][m] = mdcMods_aug11[s][m];
                else if(beamtime.CompareTo("aug11_3sec")==0) mdcMods[s][m] = mdcMods_aug11_3sec[s][m];
                else if(beamtime.CompareTo("apr12")     ==0) mdcMods[s][m] = mdcMods_apr12[s][m];
                else if(beamtime.CompareTo("jul14")     ==0) mdcMods[s][m] = mdcMods_apr12[s][m];
                else {
                    ::Error("","Beam time = %s not supported !",beamtime.Data());
                    exit(1);
                }
            } else {
                mdcMods[s][m] = mdcsetup[s][m];
            }
            cout<<" "<<mdcMods[s][m];
        }
        cout<<endl;
    }



    Int_t tofMods   [22] = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    Int_t rpcMods   [1]  = {1};
    Int_t showerMods[3]  = {1,2,3};
    Int_t emcMods   [1]  = {1};
    Int_t nStartMods[10]      = {1,1,1,1,0,0,0,0,0,0};  // before apr12
    Int_t nStartModsApr12[10] = {1,1,1,1,1,0,0,0,0,0};  // apr12
    Int_t nStartModsJul14[10] = {1,0,0,0,1,0,0,0,0,0};  // jul14 (+may14)  start+hodo
    Int_t pionTrackerMods[10] = {1,1,1,1,0,0,0,0,0,0};  // jul14
    Int_t wallMods  [1]  = {1};
    Int_t nTrigMods [1]  = {1};
    Int_t richMods  [1]  = {1};

    HSpectrometer* spec = gHades->getSetup();

    if(detectors.Contains("tbox")){
        ::Info("","Adding TBox");
        spec->addDetector(new HTBoxDetector);
        spec->getDetector("TBox")->setModules(-1,nTrigMods);
    }
    if(detectors.Contains("start")) {
        ::Info("","Adding START");
        spec->addDetector(new HStart2Detector);
        if( beamtime.CompareTo("aug11")      ==0 ||
            beamtime.CompareTo("aug11_3sec") ==0
          ) {
            spec->getDetector("Start")->setModules(-1,nStartMods);
        } else if (beamtime.CompareTo("apr12") ==0) {
            spec->getDetector("Start")->setModules(-1,nStartModsApr12);
        } else if (beamtime.CompareTo("jul14") ==0) {
            spec->getDetector("Start")->setModules(-1,nStartModsJul14);
        }

    }
    if(detectors.Contains("wall"))  {
        ::Info("","Adding WALL");
        spec->addDetector(new HWallDetector);
        spec->getDetector("Wall")->setModules(-1,wallMods);
    }
    if(detectors.Contains("piontracker"))  {
        ::Info("","Adding PionTracker");
        spec->addDetector(new HPionTrackerDetector);
        spec->getDetector("PionTracker")->setModules(-1,pionTrackerMods);
    }

    if(detectors.Contains("rich"))  {spec->addDetector(new HRichDetector);   ::Info("","Adding RICH");  }
    if(detectors.Contains("mdc"))   {spec->addDetector(new HMdcDetector);    ::Info("","Adding MDC");   }
    if(detectors.Contains("tof"))   {spec->addDetector(new HTofDetector);    ::Info("","Adding TOF");   }
    if(detectors.Contains("rpc"))   {spec->addDetector(new HRpcDetector);    ::Info("","Adding RPC");   }
    if(detectors.Contains("shower")){spec->addDetector(new HShowerDetector); ::Info("","Adding SHOWER");}
    else if(detectors.Contains("emc")) {spec->addDetector(new HEmcDetector); ::Info("","Adding EMC");}

    for (Int_t is=0; is<6; is++) {
        if(detectors.Contains("rich"))  spec->getDetector("Rich")  ->setModules(is,richMods);
        if(detectors.Contains("mdc"))   spec->getDetector("Mdc")   ->setModules(is,mdcMods[is]);
        if(detectors.Contains("tof"))   spec->getDetector("Tof")   ->setModules(is,tofMods);
        if(detectors.Contains("rpc"))   spec->getDetector("Rpc")   ->setModules(is,rpcMods);
        if(detectors.Contains("shower"))spec->getDetector("Shower")->setModules(is,showerMods);
        else if(detectors.Contains("emc"))spec->getDetector("Emc")->setModules(is,emcMods);
    }
}
void HDst::setupParameterSources(TString parsource,TString asciiParFile,TString rootParFile,TString histDate)
{
    // parsource = oracle,ascii,root (order matters)
    // if source is "ascii" a ascii param file has to provided
    // if source is "root" a root param file has to provided
    // The histDate paramter (default "now") is used wit the oracle source
    parsource.ToLower();
    parsource.ReplaceAll(" ","");

    ::Info("setupParameterSources()", "\n----------- setting up parameter sources ----------------");

    Bool_t useOra   = kFALSE;
    Bool_t useAscii = kFALSE;
    Bool_t useRoot  = kFALSE;

    if(parsource.Contains("oracle")) useOra   = kTRUE;
    if(parsource.Contains("ascii"))  useAscii = kTRUE;
    if(parsource.Contains("root"))   useRoot  = kTRUE;


    if(useAscii && asciiParFile == "") {
        ::Error( "","Ascii source selected, but ascii file empty!");
        exit(1);

    }
    if(useRoot  && rootParFile  == "") {
        ::Error( "","Root source selected, but root file empty!");
        exit(1);
    }
    if(useOra && useRoot && useAscii) {

        ::Error( "","Root , Ascii and Ora cannot be active at the same time (only 2 of them)!");
        exit(1);
    }

    TObjArray* ar = parsource.Tokenize(",");

    if(ar){

        Int_t n  = ar->GetEntries();

        if(n > 2 || n == 0){

            ::Error( "","To many ( > 2) or no param source specified !");
            exit(1);
        }
        HRuntimeDb *rtdb = gHades->getRuntimeDb();

        for(Int_t i = 0; i < n; i ++){
            TString parsrc = ((TObjString*)ar->At(i))->GetString();
            if(parsrc == "ascii"){

                ::Info("", "Add ASCII as source %i reading from : %s",i+1,asciiParFile.Data());

                HParAsciiFileIo *input = new HParAsciiFileIo;
                input->open((Text_t*)asciiParFile.Data(),"in");
                if(i == 0) rtdb->setFirstInput(input);
                else       rtdb->setSecondInput(input);
            }
            else if(parsrc == "root"){

                ::Info("", "Add ROOT  as source %i reading from : %s",i+1,rootParFile.Data());

                HParRootFileIo *input=new HParRootFileIo;
                input->open((Text_t*)rootParFile.Data(),"READ");
                if(i == 0) rtdb->setFirstInput(input);
                else       rtdb->setSecondInput(input);
            }
            else if(parsrc == "oracle"){
#ifdef ORACLE_SUPPORT
                ::Info("", "Add ORACLE as source %i histdate = %s",i+1,histDate.Data());

                HParOra2Io *ora=new HParOra2Io() ;
                ora->open();

                if(histDate.CompareTo("now")==0 || histDate.Contains("-")){
                    ora->setHistoryDate(histDate.Data());
                } else {
                    ora->setParamRelease(histDate.Data());
                }
                if(i == 0) rtdb->setFirstInput(ora);
                else       rtdb->setSecondInput(ora);
#else
                ::Error( "","ORCALE source specified but not supported !");
                exit(1);
#endif
            }
        }

        ar->Delete();
        delete ar;

    }

}

void HDst::setDataSource(Int_t sourceType,
                         TString inDir,
                         TString inFile,
                         Int_t refId,
                         TString eventbuilder
                        )
{

    // datasource 0 = hld, 1 = hldgrep 2 = hldremote, 3 root 4 geantmerge
    // like "lxhadeb02.gsi.de"  needed by dataosoure = 2
    // inputDir needed by dataosoure = 1,2
    // inputFile needed by dataosoure = 1,3,4
    // inputDir should be empty if not used (dataosoure = 0,3)
    // inputFile can contain path if inputDir is empty
    // for datasource 4 inputFile is a comma seprated list
    // "file1_with_path,file2_with_path,file3_with_path"

    ::Info("setDataSource()","\n----------- setting up data sources ---------------------");

    if( sourceType == 0 ){
        ::Info("", "Add HldFileSource");

        HldFileSource* source = new HldFileSource;
        if(inDir !="") source->setDirectory((Text_t*) inDir.Data());
        source->addFile((Text_t*) inFile.Data(),refId);
        gHades->setDataSource(source);
    } else if ( sourceType == 1 ){
        ::Info("", "Add HldGrepFileSource");
        Int_t GrepInterval   = 5 ; // look for new file each n seconds
        Int_t GrepTimeOffset = 10; // at least n seconds old
        Int_t GrepFileOffset = 0;  // latest - GrepFileOffset file (minimum 0)
        HldGrepFileSource* source = new HldGrepFileSource(inDir,"Grep",GrepInterval,refId,GrepTimeOffset,GrepFileOffset);
        gHades->setDataSource(source);
    } else if ( sourceType == 2 ){
        ::Info("", "Add HldRemoteSource");
        HldRemoteSource* source = new HldRemoteSource(eventbuilder.Data());
        source->setRefId(refId);
        gHades->setDataSource(source);
    } else if ( sourceType == 3 ){
        ::Info("", "Add HRootSource");
        HRootSource* source = new HRootSource();
        if(inDir !="") source->setDirectory((Text_t*) inDir.Data());
        source->addFile((Text_t *)inFile.Data());
        source->setGlobalRefId(refId);
        gHades->setDataSource(source);
    } else if ( sourceType == 4 ){
        ::Info("", "Add HGeantMergeSource");
        HGeantMergeSource* source = new HGeantMergeSource();
        source->addMultFiles(inFile);
        source->setGlobalRefId(refId);
        gHades->setDataSource(source);
    } else {
        ::Error("","Unknown source type = %i !",sourceType);
        exit(1);
    }
}

void HDst::setSecondDataSource(TString inDir,
                               TString inFile,
                               Int_t refId
                              )
{

    // second datasource is a root source (used for embedding)
    // inputDir should be empty if not used
    // inputFile can contain path if inputDir is empty

    ::Info("setDataSource()","\n----------- setting up data sources ---------------------");

    ::Info("", "Add second DataSource");

    HRootSource* source = new HRootSource(kTRUE,kTRUE);
    if(inDir !="") source->setDirectory((Text_t*) inDir.Data());
    source->addFile((Text_t*) inFile.Data());
    source->setGlobalRefId(refId);
    gHades->setSecondDataSource(source);
}

void HDst::setupUnpackers(TString beamtime,TString detectors,Bool_t correctINL)
{
    // beamtime aug11
    // detectors (default)= rich,mdc,tof,rpc,shower,wall,tbox,latch,start
    // The data source has to be set before!
    ::Info("setupUnpackers()", "\n----------- setting up unpackers    ---------------------");

    beamtime.ToLower();
    detectors.ToLower();

    if(beamtime.CompareTo("may14")==0) beamtime = "jul14";
    if(beamtime.CompareTo("aug14")==0) beamtime = "jul14";

    if     (beamtime.CompareTo("aug11")      ==0);
    else if(beamtime.CompareTo("apr12")      ==0);
    else if(beamtime.CompareTo("jul14")      ==0);
    else {
        ::Error("","Beam time = %s not supported !",beamtime.Data());
        exit(1);
    }


    //Data source
    HldSource *source = (HldSource *)gHades->getDataSource();


    if(source == NULL) {
        ::Error( "","Datasource source not set !");
        exit(1);
    }

    TString classname = source->ClassName();

    if(classname == "HRootSource") {
        ::Error( "","Datasource is HRootSource , not compatibel with unpackers !");
        exit(1);
    }


    if(beamtime == "aug11")
    {
        ::Info("", "Using aug11 setup");

        Int_t mdcUnpackers   [12] = {0x1100,0x1110,0x1120,0x1130,0x1140,0x1150,0x1000,0x1010,0x1020,0x1030,0x1040,0x1050};
        Int_t rpcUnpackers   [2]  = {0x8400,0x8410}; //
        Int_t startUnpackers [1]      = {0x8800};        //
        Int_t tofUnpackers   [1]  = {0x8600};        //
        Int_t wallUnpackers  [1]  = {0x8700};        //
        Int_t showerUnpackers[6]  = {0x3200,0x3210,0x3220,0x3230,0x3240,0x3250}; //
        Int_t richUnpackers  [3]  = {0x8300,0x8310,0x8320};


        if(detectors.Contains("wall")) {
            ::Info("", "Adding WALL unpackers");

            for(UInt_t i=0; i<(sizeof(wallUnpackers)/sizeof(Int_t)); i++)
            {
                cout<<hex<<wallUnpackers[i]<<", "<<dec<<flush;
                HWallTrb2Unpacker* wallUnpacker=new HWallTrb2Unpacker(wallUnpackers[i]);
                wallUnpacker->setQuietMode();
                wallUnpacker->removeTimeRef();
                source->addUnpacker( wallUnpacker);
            }
            cout<<endl;
        }

        if(detectors.Contains("latch")) {source->addUnpacker( new HLatchUnpacker(startUnpackers[0]) ); ::Info("", "Adding LATCH unpacker");}
        if(detectors.Contains("tbox"))  {source->addUnpacker( new HTBoxUnpacker(startUnpackers[0]) );  ::Info("", "Adding TBOX unpacker");}

        if(detectors.Contains("rich")){
            ::Info("", "Adding RICH unpackers");

            for(UInt_t i=0; i<(sizeof(richUnpackers)/sizeof(Int_t)); i++){
                cout<<hex<<richUnpackers[i]<<", "<<dec<<flush;
                source->addUnpacker( new HRichUnpacker(richUnpackers[i]) );
            }
            cout<<endl;
        }

        if(detectors.Contains("mdc")){
            ::Info("", "Adding MDC unpackers");

            for(UInt_t i=0; i<(sizeof(mdcUnpackers)/sizeof(Int_t)); i++) {
                cout<<hex<<mdcUnpackers[i]<<", "<<dec<<flush;
                HMdcUnpacker* mdc_unpacker = new HMdcUnpacker(  mdcUnpackers[i], kFALSE );
                mdc_unpacker->setQuietMode(kTRUE,kFALSE);

                mdc_unpacker->setFillDataWord(kFALSE);
                //mdc_unpacker->setFillOepStatus(kTRUE);
                mdc_unpacker->setPersistencyDataword(kTRUE);
                mdc_unpacker->setPersistencyOepStatusData(kFALSE);
                source->addUnpacker( (HMdcUnpacker*)mdc_unpacker );
            }
            cout<<endl;
        }

        if(detectors.Contains("shower")){
            ::Info("", "Adding SHOWER unpackers");

            for(UInt_t i=0; i<(sizeof(showerUnpackers)/sizeof(Int_t)); i++) {
                cout<<hex<<showerUnpackers[i]<<", "<<dec<<flush;
                source->addUnpacker( new HShowerUnpacker(showerUnpackers[i]) );
            }
            cout<<endl;
        }

        if(detectors.Contains("tof")){
            ::Info("", "Adding TOF unpackers");

            for(UInt_t i=0; i<(sizeof(tofUnpackers)/sizeof(Int_t)); i++){
                cout<<hex<<tofUnpackers[i]<<", "<<dec<<flush;
                HTofTrb2Unpacker *tofUnp = new HTofTrb2Unpacker(tofUnpackers[i]);
                tofUnp->setDebugFlag(0);
                //tofUnp->setQuietMode();
                if(correctINL)tofUnp->setcorrectINL();
                tofUnp->removeTimeRef();
                source->addUnpacker( tofUnp );
            }
            cout<<endl;
        }

        if(detectors.Contains("start")){
            ::Info("", "Adding START unpackers");

                for(UInt_t i=0; i<(sizeof(startUnpackers)/sizeof(Int_t)); i++){
                    cout<<hex<<startUnpackers[i]<<", "<<dec<<flush;
                    HStart2Trb2Unpacker *startUnp = new HStart2Trb2Unpacker(startUnpackers[i]);
                    startUnp->setDebugFlag(0);
                    if(correctINL)startUnp->setcorrectINL();
                    //startUnp->setQuietMode();

                    source->addUnpacker( startUnp );
                }
            cout<<endl;
        }

        if(detectors.Contains("rpc")){
            ::Info("", "Adding RPC unpackers");
            for(UInt_t i=0; i<(sizeof(rpcUnpackers)/sizeof(Int_t)); i++){

                cout<<hex<<rpcUnpackers[i]<<", "<<dec<<flush;
                HRpcTrb2Unpacker *rpcTrb2Unpacker = new HRpcTrb2Unpacker(rpcUnpackers[i]);
                //rpcTrb2Unpacker->setQuietMode();
                rpcTrb2Unpacker->setDebugFlag(0);
                if(correctINL)rpcTrb2Unpacker->setcorrectINL();
                source->addUnpacker(rpcTrb2Unpacker);
            }
        }
        cout<<endl;

    } else if(beamtime == "apr12") {
        ::Info("", "Using apr12 setup");

        Int_t mdcUnpackers   [12] = {0x1100,0x1110,0x1120,0x1130,0x1140,0x1150,0x1000,0x1010,0x1020,0x1030,0x1040,0x1050};
        Int_t rpcUnpackers   [2]  = {0x8400,0x8410}; //
        Int_t startUnpackers [1]  = {0x8800};        //
        Int_t tofUnpackers   [1]  = {0x8600};        //
        Int_t wallUnpackers  [1]  = {0x8700};        //
        Int_t showerUnpackers[6]  = {0x3200,0x3210,0x3220,0x3230,0x3240,0x3250}; //
        Int_t richUnpackers  [3]  = {0x8300,0x8310,0x8320};


        if(detectors.Contains("wall")) {
            ::Info("", "Adding WALL unpackers");

            for(UInt_t i=0; i<(sizeof(wallUnpackers)/sizeof(Int_t)); i++)
            {
                cout<<hex<<wallUnpackers[i]<<", "<<dec<<flush;
                HWallTrb2Unpacker* wallUnpacker=new HWallTrb2Unpacker(wallUnpackers[i]);
                wallUnpacker->setQuietMode();
                wallUnpacker->removeTimeRef();
                source->addUnpacker( wallUnpacker);
            }
            cout<<endl;
        }

        if(detectors.Contains("latch")) {source->addUnpacker( new HLatchUnpacker(startUnpackers[0]) ); ::Info("", "Adding LATCH unpacker");}
        if(detectors.Contains("tbox"))  {source->addUnpacker( new HTBoxUnpacker(startUnpackers[0]) );  ::Info("", "Adding TBOX unpacker");}

        if(detectors.Contains("rich")){
            ::Info("", "Adding RICH unpackers");

            for(UInt_t i=0; i<(sizeof(richUnpackers)/sizeof(Int_t)); i++){
                cout<<hex<<richUnpackers[i]<<", "<<dec<<flush;
                source->addUnpacker( new HRichUnpacker(richUnpackers[i]) );
            }
            cout<<endl;
        }

        if(detectors.Contains("mdc")){
            ::Info("", "Adding MDC unpackers");

            for(UInt_t i=0; i<(sizeof(mdcUnpackers)/sizeof(Int_t)); i++) {
                cout<<hex<<mdcUnpackers[i]<<", "<<dec<<flush;
                HMdcUnpacker* mdc_unpacker = new HMdcUnpacker(  mdcUnpackers[i], kFALSE );
                mdc_unpacker->setQuietMode(kTRUE,kFALSE);

                mdc_unpacker->setFillDataWord(kFALSE);
                mdc_unpacker->setFillOepStatus(kTRUE);
                mdc_unpacker->setPersistencyDataword(kTRUE);
                mdc_unpacker->setPersistencyOepStatusData(kTRUE);
                source->addUnpacker( (HMdcUnpacker*)mdc_unpacker );
            }
            cout<<endl;
        }

        if(detectors.Contains("shower")){
            ::Info("", "Adding SHOWER unpackers");

            for(UInt_t i=0; i<(sizeof(showerUnpackers)/sizeof(Int_t)); i++) {
                cout<<hex<<showerUnpackers[i]<<", "<<dec<<flush;
                source->addUnpacker( new HShowerUnpacker(showerUnpackers[i]) );
            }
            cout<<endl;
        }

        if(detectors.Contains("tof")){
            ::Info("", "Adding TOF unpackers");

            for(UInt_t i=0; i<(sizeof(tofUnpackers)/sizeof(Int_t)); i++){
                cout<<hex<<tofUnpackers[i]<<", "<<dec<<flush;
                HTofTrb2Unpacker *tofUnp = new HTofTrb2Unpacker(tofUnpackers[i]);
                tofUnp->setDebugFlag(0);
                //tofUnp->setQuietMode();
                if(correctINL)tofUnp->setcorrectINL();
                tofUnp->removeTimeRef();
                source->addUnpacker( tofUnp );
            }
            cout<<endl;
        }

        if(detectors.Contains("start")){
            ::Info("", "Adding START unpackers");

            for(UInt_t i=0; i<(sizeof(startUnpackers)/sizeof(Int_t)); i++){
                cout<<hex<<startUnpackers[i]<<", "<<dec<<flush;
                HStart2Trb2Unpacker *startUnp = new HStart2Trb2Unpacker(startUnpackers[i]);
                startUnp->setDebugFlag(0);
                if(correctINL)startUnp->setcorrectINL();
                //startUnp->setQuietMode();
                //startUnp->disableTimeRef(); // new since apr12 , only for cosmics
                source->addUnpacker( startUnp );
            }
            cout<<endl;
        }

        if(detectors.Contains("rpc")){
            ::Info("", "Adding RPC unpackers");
            for(UInt_t i=0; i<(sizeof(rpcUnpackers)/sizeof(Int_t)); i++){

                cout<<hex<<rpcUnpackers[i]<<", "<<dec<<flush;
                HRpcTrb2Unpacker *rpcTrb2Unpacker = new HRpcTrb2Unpacker(rpcUnpackers[i]);
                //rpcTrb2Unpacker->setQuietMode();
                rpcTrb2Unpacker->setDebugFlag(0);
                if(correctINL)rpcTrb2Unpacker->setcorrectINL();
                source->addUnpacker(rpcTrb2Unpacker);
            }
        }
        cout<<endl;

    } else if(beamtime == "jul14") {
        ::Info("", "Using jul14 setup");

        Int_t mdcUnpackers   [12] = {0x1100,0x1110,0x1120,0x1130,0x1140,0x1150,0x1000,0x1010,0x1020,0x1030,0x1040,0x1050};
        Int_t rpcUnpackers   [3]  = {0x8400,0x8410,0x8420};
        Int_t startUnpackers [1]      = {0x8800};         // CTS-Hub
	Int_t startUnpackersTrb3 [2]  = {0x8880,0x8890};  // start+hodo
        Int_t pionTrackerUnpackers[2] = {0x8900,0x8910};
        Int_t tofUnpackers   [1]  = {0x8600};        //
        Int_t wallUnpackers  [1]  = {0x8700};        //
        Int_t showerUnpackers[6]  = {0x3200,0x3210,0x3220,0x3230,0x3240,0x3250}; //
        Int_t richUnpackers  [3]  = {0x8300,0x8310,0x8320};


        if(detectors.Contains("wall")) {
            ::Info("", "Adding WALL unpackers");

            for(UInt_t i=0; i<(sizeof(wallUnpackers)/sizeof(Int_t)); i++)
            {
                cout<<hex<<wallUnpackers[i]<<", "<<dec<<flush;
                HWallTrb2Unpacker* wallUnpacker=new HWallTrb2Unpacker(wallUnpackers[i]);
                wallUnpacker->setQuietMode();
                wallUnpacker->removeTimeRef();
                source->addUnpacker( wallUnpacker);
            }
            cout<<endl;
        }

        if(detectors.Contains("latch")) {source->addUnpacker( new HLatchUnpacker(startUnpackers[0]) ); ::Info("", "Adding LATCH unpacker");}
        if(detectors.Contains("tbox"))  {source->addUnpacker( new HTBoxUnpacker(startUnpackers[0]) );  ::Info("", "Adding TBOX unpacker");}

        if(detectors.Contains("rich")){
            ::Info("", "Adding RICH unpackers");

            for(UInt_t i=0; i<(sizeof(richUnpackers)/sizeof(Int_t)); i++){
                cout<<hex<<richUnpackers[i]<<", "<<dec<<flush;
                source->addUnpacker( new HRichUnpacker(richUnpackers[i]) );
            }
            cout<<endl;
        }

        if(detectors.Contains("mdc")){
            ::Info("", "Adding MDC unpackers");

            for(UInt_t i=0; i<(sizeof(mdcUnpackers)/sizeof(Int_t)); i++) {
                cout<<hex<<mdcUnpackers[i]<<", "<<dec<<flush;
                HMdcUnpacker* mdc_unpacker = new HMdcUnpacker(  mdcUnpackers[i], kFALSE );
                mdc_unpacker->setQuietMode(kTRUE,kFALSE);

                mdc_unpacker->setFillDataWord(kFALSE);
                mdc_unpacker->setFillOepStatus(kTRUE);
                mdc_unpacker->setPersistencyDataword(kTRUE);
                mdc_unpacker->setPersistencyOepStatusData(kFALSE);
                source->addUnpacker( (HMdcUnpacker*)mdc_unpacker );
            }
            cout<<endl;
        }

        if(detectors.Contains("shower")){
            ::Info("", "Adding SHOWER unpackers");

            for(UInt_t i=0; i<(sizeof(showerUnpackers)/sizeof(Int_t)); i++) {
                cout<<hex<<showerUnpackers[i]<<", "<<dec<<flush;
                source->addUnpacker( new HShowerUnpacker(showerUnpackers[i]) );
            }
            cout<<endl;
        }

        if(detectors.Contains("tof")){
            ::Info("", "Adding TOF unpackers");

            for(UInt_t i=0; i<(sizeof(tofUnpackers)/sizeof(Int_t)); i++){
                cout<<hex<<tofUnpackers[i]<<", "<<dec<<flush;
                HTofTrb2Unpacker *tofUnp = new HTofTrb2Unpacker(tofUnpackers[i]);
                tofUnp->setDebugFlag(0);
                //tofUnp->setQuietMode();
                if(correctINL)tofUnp->setcorrectINL();
                tofUnp->removeTimeRef();
                source->addUnpacker( tofUnp );
            }
            cout<<endl;
        }

        if(detectors.Contains("start")){
            ::Info("", "Adding START unpackers");

	  /*
            for(UInt_t i=0; i<(sizeof(startUnpackers)/sizeof(Int_t)); i++){
                cout<<hex<<startUnpackers[i]<<", "<<dec<<flush;
                HStart2Trb2Unpacker *startUnp = new HStart2Trb2Unpacker(startUnpackers[i]);
                startUnp->setDebugFlag(0);
                startUnp->setcorrectINL();
                //startUnp->setQuietMode();
                //startUnp->disableTimeRef(); // new since apr12 , only for cosmics
                source->addUnpacker( startUnp );
            }
	  */
            for(UInt_t i=0; i<(sizeof(startUnpackersTrb3)/sizeof(Int_t)); i++){
                cout<<hex<<startUnpackersTrb3[i]<<", "<<dec<<flush;
                HStart2Trb3Unpacker *startUnp = new HStart2Trb3Unpacker(startUnpackersTrb3[i]);
                startUnp->setDebugFlag(0);
		startUnp->setQuietMode();
                startUnp->setCTSId(0x8000);
                source->addUnpacker( startUnp );
            }
            cout<<endl;
        }

        if(detectors.Contains("piontracker")){
            ::Info("", "Adding PionTracker unpackers");

            for(UInt_t i=0; i<(sizeof(pionTrackerUnpackers)/sizeof(Int_t)); i++){
                cout<<hex<<pionTrackerUnpackers[i]<<", "<<dec<<flush;
                HPionTrackerTrb3Unpacker *pionTrackerUnp = new HPionTrackerTrb3Unpacker(pionTrackerUnpackers[i]);
                pionTrackerUnp->setDebugFlag(0);
                pionTrackerUnp->setQuietMode();
                source->addUnpacker( pionTrackerUnp );
            }
            cout<<endl;
        }

        if(detectors.Contains("rpc")){
            ::Info("", "Adding RPC unpackers");
            for(UInt_t i=0; i<(sizeof(rpcUnpackers)/sizeof(Int_t)); i++){

                cout<<hex<<rpcUnpackers[i]<<", "<<dec<<flush;
                HRpcTrb2Unpacker *rpcTrb2Unpacker = new HRpcTrb2Unpacker(rpcUnpackers[i]);
                //rpcTrb2Unpacker->setQuietMode();
                rpcTrb2Unpacker->setDebugFlag(0);
                if(correctINL)rpcTrb2Unpacker->setcorrectINL();
                source->addUnpacker(rpcTrb2Unpacker);
            }
        }
        cout<<endl;

    }

    else {

        ::Error( "setupUnpackers()","unknown beamtime = %s!",beamtime.Data());
        exit(1);
    }
}
