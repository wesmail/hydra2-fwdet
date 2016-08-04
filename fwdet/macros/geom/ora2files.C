{
  //--- Configuration -----------------------------------------------------

  // simulation reference run in Oracle
  TString simRefRun = "fwdet16sim_mediumfieldalign_pimpe";  // runid 14501

  // history date for Oracle
  TString historyDate = "now";

  // output directory
  TString outputDir = "/misc/ilse/svn/hydra2-fwdet/fwdet/macros/geom";

  // To write only a detector part to the output, one can specify the keyword
  // for example target, emc, rpc, ...
  // If empty all geo files are written to the output.
  TString detectorPart = "";

  //-----------------------------------------------------------------------

  HGeomInterface* interface=new HGeomInterface;

  HGeomOra2Io* oraInput=new HGeomOra2Io;
  oraInput->open();
  oraInput->setHistoryDate(historyDate.Data());
  oraInput->setSimulRefRun(simRefRun.Data());
  interface->setOracleInput(oraInput);

  HGeomAsciiIo* output=new HGeomAsciiIo;
  output->setDirectory(outputDir.Data());
  interface->setOutput(output);

  Bool_t rc=interface->readAll();
  if (rc) {
    if (detectorPart.Length() == 0) {
      interface->writeAll();
    } else {
      HGeomSet* set = interface->findSet(detectorPart.Data());
      if (set) interface->writeSet(set);
      else cout<<"Detector part " << detectorPart << " not found." << endl;
    }
  } else {
    cout<<"Unknown simulation reference run " << simRefRun.Data() << endl;
  }

  delete interface;
}
