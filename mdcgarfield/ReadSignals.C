void ReadSignals()
{
    TString input="";
    Char_t path[300];
    Char_t inputfile[300];
    //sprintf(path,"%s","/misc/kempter/scratchlocal1/jochen/garfield/signals/");
    sprintf(path,"%s","/dataext/data/garfield/signals/");
    TStopwatch timer;
    timer.Reset();
    timer.Start();
    HMdcGarSignalReader* reader=new HMdcGarSignalReader();
    reader->setSimResponse(kTRUE);
    reader->setSigma1(5.);
    reader->setSigma2(20.);
    //reader->setMinSignal(-60000,-65000,-380000,-530000);  // old simple integration
    reader->setMinSignal(-85500,-65000,-380000,-530000);
    reader->setNSignals(100);
    reader->setNBinSignal(1000);
    reader->setRangeSignal(1000);
    reader->setWriteHists(1,1,1,1,1,1); // sig,sigint,sum,dist,timedistance,charge
    //reader->setFileNameOut("/misc/kempter/scratchlocal1/jochen/garfield/signals/mdc4/2200/test.root");
    reader->setFileNameOut("~/HYDRA/mdcgarfield/test.root");
    //reader->setFileNameIn(input);
    Int_t voltage[4]={1750,1800,2000,2200};
    for(Int_t mdc=0;mdc<1;mdc++){
	for(Int_t angle=0;angle<1;angle++){
	    if(mdc==3)
	    {
		sprintf(inputfile,"%s%s%i%s%i%s%i%s%i%s",path,"mdc",mdc+1,"/",voltage[mdc],"/s",angle*5,"-mdc",mdc+1,"_all.txt");
                input=inputfile;
	    }
	    else
	    {
                sprintf(inputfile,"%s%s%i%s%i%s%i%s%i%s",path,"mdc",mdc+1,"/",voltage[mdc],"/s",angle*5,"-mdc",mdc+1,"_all.txt");
	        input=inputfile;
	    }
            cout<<input.Data()<<endl;
	    reader->make(mdc,angle,input);
	}
    }

    timer.Stop();
    timer.Print();
    reader->closeOutput();
}
