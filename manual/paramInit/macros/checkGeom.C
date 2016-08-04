{
  TString configFile="geaini.dat";

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

  delete interface;

  if (rc) {
    // check for overlaps and extrusions
    geom->CheckOverlaps(0.0001);
    geom->PrintOverlaps();

    // draw geometry and overlaps with TBrowser 
    TBrowser* browser=new TBrowser;

    // draw volumes with OpenGL
    TGLViewer* v=(TGLViewer*)gPad->GetViewer3D();
    v->SetStyle(TGLRnrCtx::kWireFrame);
    geom->SetVisOption(0);
    geom->SetMaxVisNodes(4000);
    geom->DefaultColors();
    // draw mother volume of target and Start detector
    TGeoVolume* vol=geom->GetVolume("RTAM"); 
    vol->Draw("ogl");
    geom->SetVisLevel(1);  // visibility level
  } else {
    delete geom;
  }
}

