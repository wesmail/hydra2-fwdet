void EffMaker()
{
    TString input="/misc/kempter/batch/all-mdc_input_cal2_160902.root";

    HMdcGarEffMaker* maker=new HMdcGarEffMaker();
    maker->setVersion(1);
    maker->setFileNameAsciiOut("/misc/kempter/batch/testeff.txt");
    maker->setFileNameRootOut ("/misc/kempter/batch/testeff.root");
    maker->setFileNameRootIn(input);
    maker->make();
}
