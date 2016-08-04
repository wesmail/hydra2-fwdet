{
  // create the Hades object and get the pointer to the runtime database  
  Hades* myHades=new Hades;
  HRuntimeDb* rtdb=gHades->getRuntimeDb();

  // define the input
  HParOra2Io* input = new HParOra2Io;
  input->open();
  rtdb->setFirstInput(input);

  // define the output
  HParRootFileIo* output = new HParRootFileIo;
  output->open("test.root","RECREATE");
  rtdb->setOutput(output);

  // create the parameter container(s)
  HMagnetPar* pMagnetPar = (HMagnetPar*)(rtdb->getContainer("MagnetPar"));

  // initialize the parameter container
  rtdb->initContainers(133602648);  // magnet off
  rtdb->initContainers(133656363);  // magnet on
  rtdb->initContainers(133656526);  // magnet on  

  // print the parameters on the screen  
  pMagnetPar->printParams();

  // save the output (writes the data to the file)
  rtdb->saveOutput();

  // print content of the runtime database on the screen
  rtdb->print();

  // delete the Hades object
  delete myHades;
}
