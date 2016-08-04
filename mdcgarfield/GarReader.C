{
    TCanvas* result=new TCanvas("result","result",1000,800);

    //TString input="magboltz";
    TString input="xt";

    if(input.CompareTo("xt")==0)
    {
	TH2F* dummy=new TH2F("xt","xt",2,-3,+3,2,0,120);
	dummy->SetXTitle("distance from wire [mm]");
	dummy->SetYTitle("drift time [ns]");

	dummy->Draw();
	TGraph* gxt=0;
	gxt=HMdcGarReader::readXT("/misc/kempter/test/xtmdc1.dat",2);
	if(gxt)
	{
	    gxt->Draw();
	}
	else
	{cout<<"POINTER EQUAL ZERO!"<<endl;}
    }
    if(input.CompareTo("magboltz")==0)
    {
        result->SetLogx();
	TH2F* dummy=new TH2F("Vdrift vs E/P","Vdrift vs E/P",200,-10,130,2,0,250);
	dummy->SetXTitle("Log E/P [kV/cmbar]");
	dummy->SetYTitle("Vdrift [ns/mu]");
	dummy->Draw();

	TGraph* gxt=0;
	gxt=HMdcGarReader::readMagboltz("/misc/kempter/promotion/garfieldnew/variP/P800.dat",2);
	if(gxt)
	{
	    gxt->Draw();
	}
	else
	{cout<<"POINTER EQUAL ZERO!"<<endl;}
    }
}