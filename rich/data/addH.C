void addHistos(char* cOutFile){
    // adds files with histograms
    TFile outfile(cOutFile,"RECREATE");
    TIter filelist(gROOT->GetListOfFiles());
    TFile *fileobj=0;
    fileobj = (TFile*) filelist(); 
    TIter keylist(fileobj->GetListOfKeys());
    cout<<"Adding histograms"<<endl;
    TKey *key;
    TH1 *h1;
    
    while ((key=(TKey*)keylist())){ 
	char* name1=key->GetName();
	h1 = (TH1*) fileobj->Get(name1);
	TIter filelist2(gROOT->GetListOfFiles());
	TFile *fileobj2;
	fileobj2 = (TFile*) filelist2();
	while ( fileobj2 = (TFile*) filelist2() ){ 
	    TIter keylist2(fileobj2->GetListOfKeys());
	    TKey *key1;
	    while ((key1=(TKey*)keylist2())){ 
		char* name2=key1->GetName();
		if (!strcmp(name1,name2)) {
		    h2  = (TH1*) fileobj2->Get(name2);
		    if (h2) h1->Add(h1,h2,1.,1.); 
		}
	    }
	}
	outfile->cd();
	h1->Write();
    }
}

void printHistos(char *cFileIn){
    // prints all histos found in file
    TFile file(cFileIn,"READ");
    gStyle->SetPalette(1);
    TIter keylist(file.GetListOfKeys());
    TKey *key;
    TH1 *h1;
    TCanvas * c = new TCanvas("c1","c1",10,10,700,500);
    Int_t cntr = 0;
    while ((key=(TKey*)keylist())){
	cntr++;
	char* name1=key->GetName();
	h1 = (TH1*) file.Get(name1);
	TString filename(name1);
	c->SetName(name1);
	c->SetTitle(name1);
	c->cd();
	if ( !strcmp((h1->IsA())->GetName(),"TH2F") || 
	     !strcmp((h1->IsA())->GetName(),"TH2D") ||
	     !strcmp((h1->IsA())->GetName(),"TH2S")     )
	{
	    h1->DrawCopy("colz");
	}else{
	    h1->DrawCopy();
	}
	//	h1->Draw();
	filename.Append(".eps");
	c->Print(filename.Data());
	filename.Remove(filename.Length()-4, filename.Length() );
	filename.Append(".gif");
	c->Print(filename.Data());
	//char *opt = new char[2];
	//scanf("%[^\n]%*c",opt);
    }
}

void printMicro(char *cFileIn){
    // prints all histos found in file
    TFile file(cFileIn,"READ");
    TString canvasName(cFileIn);
    canvasName.Remove(canvasName.Length()-5, canvasName.Length() );
    gStyle->SetPalette(1);
    TIter keylist(file.GetListOfKeys());
    TKey *key;
    TH1 *h1;
    Int_t cntr = 0;
    while ((key=(TKey*)keylist())){
	cntr++;
    }
    TCanvas * c1 = new TCanvas("c2","c2",10,10,1200,1000); 
    c1->SetTitle(canvasName.Data());
    c1->SetName(canvasName.Data());
    Float_t help = cntr;
    Int_t plusone=0;
    if (TMath::Sqrt(help)-TMath::Nint(TMath::Sqrt(help)) >0) plusone=1;
    Int_t div = TMath::Nint(TMath::Sqrt(help));
    c1->Divide(div,div+plusone);
    keylist.Reset();
    cntr=0;TIter keylist2(file.GetListOfKeys());
    while ((key=(TKey*)keylist2())){
	cntr++;
	char* name1=key->GetName();
	h1 = (TH1*) file.Get(name1);
	c1->cd(cntr);
	if ( 
	    !strcmp((h1->IsA())->GetName(),"TH2F") || 
	    !strcmp((h1->IsA())->GetName(),"TH2D") ||
	    !strcmp((h1->IsA())->GetName(),"TH2S")   )
	{
	    //    cout<<(h1->IsA())->GetName()<<"  colz  "<<endl;
	    h1->DrawCopy("colz");
	}else{
	    //cout<<(h1->IsA())->GetName()<<"        "<<endl;
	    h1->DrawCopy();
	}
    }
    c1->Modified();
    c1->Update();
    canvasName.Append(".ps");
    c1->Print(canvasName.Data());
    canvasName.Remove(canvasName.Length()-3, canvasName.Length() );
    canvasName.Append(".gif");
    c1->Print(canvasName.Data());
}
