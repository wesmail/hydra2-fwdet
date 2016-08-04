{
  Hades* myHades=new Hades;
  HSpectrometer* spec=gHades->getSetup();
  HRuntimeDb* rtdb=gHades->getRuntimeDb();

  HMdcDetector* mdc=new HMdcDetector;
  Int_t mdcMods[6][4]=
      { {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1} };
  for(Int_t i=0;i<6;i++) mdc->setModules(i,mdcMods[i]);
  spec->addDetector(mdc);

  HParOra2Io* input = new HParOra2Io;
  input->open();
  rtdb->setFirstInput(input);

  HSpecGeomPar* specGeomPar = (HSpecGeomPar*)(rtdb->getContainer("SpecGeomPar"));
  HMdcGeomPar*  mdcGeomPar  = (HMdcGeomPar*)(rtdb->getContainer("MdcGeomPar"));

  rtdb->initContainers(12000);  // ideal geometry for APR12

  // --------------------------------------------------------------------------------------
  // example Geant hit with x, y position of track entering the volume in forward direction
  // --------------------------------------------------------------------------------------
  //  HGeantMdc* fGeant;
  //  Int_t sectorNum = (Int_t)(fGeant->getSector());
  //  Int_t moduleNum = (Int_t)(fGeant->getModule());
  //  Int_t layerNum  = (Int_t)(fGeant->getLayer());
  //  Float_t x, y, tof, ptot;
  //  fGeant->getHit(x, y, tof, ptot);

  // Example values for testing:
  Int_t sectorNum = 1, moduleNum = 0, layerNum = 3;
  Float_t x = 0.F, y = 0.F;

  // get sector transformation
  HGeomVolume* sector = specGeomPar->getSector(sectorNum);
  HGeomTransform& sectorTrans = sector->getTransform();

  // get module transformation
  HModGeomPar* mdcModule = mdcGeomPar->getModule(sectorNum,moduleNum);
  HGeomTransform& modTransLab = mdcModule->getLabTransform();

  // get layer transformation
  HGeomVolume* layer = mdcModule->getRefVolume()->getComponent(layerNum);
  HGeomTransform& layerTrans = layer->getTransform();

  HGeomVector hitLayer, hitModule, hitLab, hitSector;

  // set x, y from Geant hit, z at entrance into volume (points 0-3)
  hitLayer.setX(x);
  hitLayer.setY(y);
  hitLayer.setZ(layer->getPoint(0)->getZ());
  hitLayer.print();

  // transform from layer to module coordinate system
  hitModule = layerTrans.transFrom(hitLayer); 
  hitModule.print();

  // transform from module to LAB system
  hitLab = modTransLab.transFrom(hitModule);
  hitLab.print();

  // transform from LAB system to sector coordinate system
  hitSector = sectorTrans.transTo(hitLab); 
  hitSector.print();

  // or do it all in one line
  hitSector = sectorTrans.transTo(modTransLab.transFrom(layerTrans.transFrom(hitLayer)));

  delete gHades;
}
