{
  TString configFile="geaini.dat";

  HGeomInterface* interface=new HGeomInterface;

  HGeomOra2Io* oraInput=new HGeomOra2Io;
  oraInput->open();
  interface->setOracleInput(oraInput);

  Bool_t rc=interface->readGeomConfig(configFile.Data());

  HGeomAsciiIo* output=new HGeomAsciiIo;
  output->setDirectory("./");
  interface->setOutput(output);

  Bool_t rc=interface->readAll();
  if (rc) {
    interface->writeAll();
  }
  delete interface;
}
