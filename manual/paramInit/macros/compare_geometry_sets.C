{
  TString configFile1="apr12sim_from_oracle.dat";
  TString configFile2="apr12sim_from_files.dat";        

  HGeomInterface* interface1=new HGeomInterface;
  HGeomOra2Io* oraInput=new HGeomOra2Io;
  oraInput->open();
  interface1->setOracleInput(oraInput);
  interface1->readGeomConfig(configFile1.Data());
  interface1->readAll();  

  HGeomInterface* interface2=new HGeomInterface;
  interface2->readGeomConfig(configFile2.Data());
  interface2->readAll();  
  
  TString sets[12] = {"cave", "sect", "rich", "target", "start", "mdc",
                      "tof", "rpc", "shower", "wall", "coils", "frames"};

  HGeomSet *set1 = NULL, *set2 = NULL;

  for (Int_t i=0; i<12; i++) {
    const char* name = sets[i].Data();
    set1=interface1->findSet(name);
    set2=interface2->findSet(name);
    if (set1 == NULL && set2 == NULL) continue;
    if (set1 != NULL && set2 != NULL) {
      cout << "****************************************************************" << endl;
      cout << "***  "<< name << endl;
      cout << "****************************************************************" << endl;
      set1->compare(*set2);
    } else {
      cout << "****************************************************************" << endl;
      if (set1 != NULL && set2 == NULL) cout << name << " missing in config 2" << endl;
      if (set1 == NULL && set2 != NULL) cout << name << " missing in config 1" << endl;
      cout << "****************************************************************" << endl;
    }
  }

  delete interface1;
  delete interface2;
}
{
  TString configFile1="apr12sim_from_oracle.dat";
  TString configFile2="geaini.dat";

  HGeomInterface* interface1=new HGeomInterface;
  HGeomOra2Io* oraInput=new HGeomOra2Io;

  oraInput->open();
  interface1->setOracleInput(oraInput);
  interface1->readGeomConfig(configFile1.Data());
  interface1->readAll();  

  HGeomInterface* interface2=new HGeomInterface;
  interface2->readGeomConfig(configFile2.Data());
  interface2->readAll();  
  
  HGeomSet* set1=interface1->findSet("rich");
  HGeomSet* set2=interface2->findSet("rich");
  if (set1 && set2) set1->compare(*set2);

  set1=interface1->findSet("target");
  set2=interface2->findSet("target");
  (set1 && set2) set1->compare(*set2);

  // more ...

  delete interface1;
  delete interface2;
}
