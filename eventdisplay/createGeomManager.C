{
    //  if (gSystem->Load("libGeom")!=0) {
    //   Error("Unable to load libGeom.so\n");
    //    return;
    //  }

    // **********  Put here your own config file!  *******************
    //TString configFile="/u/hadesdst/sim/apr12/hgeant/geom/geaini.dat";
    //TString outname="GeomManager_Apr12_runID_12001.root";
    TString configFile="agag1650.dat";
    TString outname="GeomManager_prop2017_agag1650_runID_15000_rich700_ecal.root";
    HGeomInterface* interface=new HGeomInterface;
    /*
     HGeomOra2Io* oraInput=new HGeomOra2Io;
     oraInput->open();
     interface->setOracleInput(oraInput);
     */
    Bool_t rc=interface->readGeomConfig(configFile.Data());
    if (!rc) printf("Read of GEANT config file failed!\n");

    // Read the geometry from the files
    if (rc) rc=interface->readAll();
    if (!rc) printf("Read of geometry failed!\n");

    TGeoManager* geom = new TGeoManager("HadesGeom", "HADES geometry");
    HGeomRootBuilder* builder=new HGeomRootBuilder("builder","geom builder");
    builder->setGeoManager(geom);
    interface->setGeomBuilder(builder);

    if (rc) rc=interface->createAll();
    if (!rc) printf("Creation of geometry failed!\n");

    //if (rc) builder->checkOverlaps();

    if (rc) TBrowser* browser=new TBrowser;
    else {
	delete interface;
	delete geom;
	return;
    }

    TFile* output=new TFile(outname.Data(),"RECREATE");
    output->cd();
    geom->Write();
    //builder->Write();
    output->Save();
    output->Close();


}
