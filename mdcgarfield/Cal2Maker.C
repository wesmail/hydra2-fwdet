{
    /*

     // not modified
     Int_t maxbinNumber[72]={25,28,28,31,33,34,  //25
                            35,35,36,37,36,35,  //55
			    35,34,33,31,29,27,

			    29,32,33,35,36,37,
			    40,39,39,39,38,38,
			    37,36,34,32,30,28,

			    60,63,66,68,70,71, //36
			    71,71,71,70,68,66,
			    64,61,57,54,49,45,

			    70,75,78,81,83,85,
			    86,87,86,85,84,82,
			    79,75,71,67,62,56 };
    */

    Int_t maxbinNumber[72]={25,28,28,31,33,34,  //25   // 6% treshold
                            35,35,36,37,36,35,  //55
			    35,34,33,31,29,27,

			    29,32,33,35,36,37,
			    40,39,39,39,38,38,
			    37,31,34,32,30,28, //65:36->31

			    60,63,65,68,69,70, //36  // 10:66->65 20:70->69 25:71->70
			    71,71,71,70,68,66,
			    //64,61,57,53,49,44,   // old    // 75:54->53 85: 45->44
                            63,61,57,53,49,44, // new      // 75:54->53 85: 45->44

			    70,75,78,81,83,85,
			    86,87,86,85,84,82,
			    79,75,71,67,62,56 };



    HMdcGarCal2Maker *maker=new HMdcGarCal2Maker();
    maker->setFileNameOut("/misc/kempter/batch/cal2makerout.txt");
    //maker->setFileNameIn ("/misc/kempter/scratchlocal1/jochen/garfield/signals/all-mdc_input_cal2_2_070102.root");
    maker->setFileNameIn ("/misc/kempter/batch/all-mdc_input_cal2_160902.root");
    //maker->setBatchMode(kTRUE);
    //maker->setVersion(1);
    maker->setBatchMode(kTRUE);
    maker->setPrintMode(kFALSE);
    maker->setWriteHists(kTRUE);
    maker->setWriteAscii(kTRUE);
    maker->setMakeMdc (1,1,1,1);
    maker->setMakeType(1,1,1,1);
    maker->setMaxBinNumber(&maxbinNumber[0]);
    maker->setVoltage(1750,1800,2000,2400);
    maker->setHists_1D(1,1,1,1);
    maker->setHists_2D(1,1,1,1); // raw,spike,smooth,graph
    //maker->setHists_1D(0,0,0,0);
    //maker->setHists_2D(1,0,0,0); // raw,spike,smooth,graph

    maker->make();
}