{
 // TString configFile="/u/slebedev/hades/hydra2/scripts/rich700/geom/test_output.dat";
 // TString outfile="/u/slebedev/hades/hydra2/scripts/rich700/Ggeom.root";

  TString configFile="/u/slebedev/hades/hydra2/rich/scripts/lamptest/geom/lamp_geant_output.dat";
  TString outfile="/u/slebedev/hades/hydra2/rich/scripts/lamptest/geom/lamp_geom.root";
  HGeomInterface* interface=new HGeomInterface;

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

  if (rc) {
    //    builder->checkOverlaps();
    //geom->CheckOverlaps(0.0001);
    //geom->PrintOverlaps();
    TBrowser* browser=new TBrowser;
    /*
    TGLViewer* v=(TGLViewer*)gPad->GetViewer3D();
    v->SetStyle(TGLRnrCtx::kWireFrame);
    geom->SetVisOption(1);
    geom->SetVisLevel(4);
    geom->SetMaxVisNodes(40000);
    geom->DefaultColors();
    geom->GetMasterVolume()->Draw("ogl");
    */
    //    TGeoVolume* vol=geom->GetVolume("GMOM");
    //    vol->Draw("ogl");
    //    vol->SetLineColor(7);
  } else {
    delete interface;
    delete geom;
    return;
  }
  TFile* output=new TFile(outfile.Data(),"RECREATE");
  output->cd();
  geom->Write();
  //builder->Write();
  output->Save();
  output->Close();

  //  delete interface;
}





