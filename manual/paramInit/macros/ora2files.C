{
  // configuration file
  TString configFile = "apr12sim_from_oracle.dat";

  // output directory (must exist)
  TString outputDir = "./geom12001";

  // ******************************************************

  HGeomInterface* interface=new HGeomInterface;

  HGeomOra2Io* oraInput=new HGeomOra2Io;
  oraInput->open();
  interface->setOracleInput(oraInput);

  Bool_t rc=interface->readGeomConfig(configFile.Data());

  HGeomAsciiIo* output=new HGeomAsciiIo;
  output->setDirectory(outputDir.Data());
  interface->setOutput(output);

  Bool_t rc=interface->readAll();
  if (rc) {
    interface->writeAll();
    // HGeomSet* set=interface->findSet("target");
    // if (set) interface->writeSet(set);
    // interface->writeMedia();
  }
  delete interface;
}
