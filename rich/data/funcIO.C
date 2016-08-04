// File: funcIO.C
//
// Author: Thomas Eberl <Thomas.Eberl@physik.tu-muenchen.de>
// Last update by Thomas Eberl: 01/02/25 21:15:31
//
// collection of functions to be used within CINT
// load : CINT>.L funcIO.C
// purpose: provide Input Output functionality for HADES RICH

TFile* openFile(char *name){
    if (name){
	TFile *pFile = new TFile(name,"READ");
	if (pFile) pFile->Print();
    }else{
	cout<<"Error: no filename provided !"<<endl;
    }
return pFile;
}

TTree* getTree(char *name,TFile *pFile){
    TTree *pTreeIn = 0; 
    pTreeIn = 	(TTree*) pFile->Get(name);
    if (pTreeIn) {
	pTreeIn -> Print();
	return pTreeIn;
    } else {
	cout<<"Error: no such Tree in file"<<endl;
	return pTreeIn;
    }
}

void countEvents(){
    TIter next(gROOT->GetListOfFiles());
    TFile *pFileIn=0;
    Int_t nTotEntries = 0;
    Int_t nEntries = 0;
    while ( pFileIn = (TFile*) next() ){
	TString sFileIn( pFileIn->GetName() );
	cout<<"Now processing file: "<<sFileIn<<endl;
	HTree *T = (HTree *) pFileIn->Get("T");
	nEntries = T->GetEntries();
	nTotEntries += nEntries;
	cout<<"Entries in file: "<<nEntries<<"; total number of entries counted: "<<nTotEntries<<endl; 
	pFileIn->Close();
    }
}

void printAttachedFiles(){
    TIter next(gROOT->GetListOfFiles());
    TFile *pFileIn=0;
    while ( pFileIn = (TFile*) next() ){
	TString sFileIn( pFileIn->GetName() );
	cout<<"file: "<<sFileIn<<endl;
	pFileIn->Close();
    }
}

void closeFile(char *cFileIn){
   TFile f1(cFileIn,"UPDATE");
   f1.Close(); 
}

void countDiscreteEvents(char* cFileIn){
    gSystem->Load("libFit.so");
    gROOT->LoadMacro("dataDefs.C");
    //    gROOT->LoadMacro("funcIO.C");
    gStyle->SetPalette(1);
    TTree *pTreeIn = getTree("Parameters of fitted rings",
			  openFile(cFileIn));

    TBranch *bNumberOfPhotons,*bPhotonX,*bPhotonY,*bPhotonCharge;
    Int_t NumberOfPhotonsInEvent;
    Double_t x[maxNumberOfPhotons],y[maxNumberOfPhotons],
	c[maxNumberOfPhotons];

    bNumberOfPhotons = pTreeIn -> GetBranch("Photon_");
    bPhotonX =         pTreeIn -> GetBranch("Photon.x");
    bPhotonY =         pTreeIn -> GetBranch("Photon.y");
    bPhotonCharge =    pTreeIn -> GetBranch("Photon.c");
    
    bNumberOfPhotons ->  SetAddress(&NumberOfPhotonsInEvent);
    bPhotonX         ->  SetAddress(x);
    bPhotonY         ->  SetAddress(y);
    bPhotonCharge    ->  SetAddress(c);

    FITTED_RING ringFitted;
    TBranch *bRingFitted;
    bRingFitted = pTreeIn -> GetBranch("ringFitted");
    bRingFitted -> SetAddress(&ringFitted);

    RECOGNIZED_RING ring;
    TBranch *bRingSimul;
    bRingSimul = pTreeIn -> GetBranch("ring");
    bRingSimul -> SetAddress(&ring);

    Int_t nMinEntry = 0;
    Stat_t nMaxEntry = pTreeIn->GetEntries();
    cout<<nMaxEntry<<endl;  
    Int_t ringXtmp,ringYtmp;
    ringXtmp = ringYtmp = 0;
    Float_t Phitmp,Thetatmp;
    Phitmp = Thetatmp = 0.;
    Int_t nCntr = 1;
    for (Int_t i=nMinEntry; i<nMaxEntry; i++){

	bRingSimul       -> GetEntry(i);
	//bRingFitted      -> GetEntry(i);
	//bNumberOfPhotons -> GetEntry(i);
	//bPhotonX         -> GetEntry(i);
	//bPhotonY         -> GetEntry(i);
	//bPhotonCharge    -> GetEntry(i);
	if (i==0){
	    ringXtmp = ring.RingX;
	    ringYtmp = ring.RingY;
	    Phitmp = ring.Phi;
	    Thetatmp = ring.Theta;
	}
	if (ring.RingX==ringXtmp && 
	    ring.RingY== ringYtmp &&
	    ring.Phi  == Phitmp &&
	    ring.Theta== Thetatmp ){

	}else{
	    ringXtmp = ring.RingX;
	    ringYtmp = ring.RingY;
	    Phitmp = ring.Phi;
	    Thetatmp = ring.Theta;
	    nCntr++;
	}
    }
    cout<<nCntr<<" discrete events !"<<endl;
}

void writeDiscreteEvents(char* cFileIn, char* cFileOut){
    gSystem->Load("libFit.so");
    gROOT->LoadMacro("dataDefs.C");
    //    gROOT->LoadMacro("funcIO.C");
    gROOT->LoadMacro("funcRingFit.C");
    gStyle->SetPalette(1);
    TTree *pTreeIn = getTree("FittedRingParams",
			  openFile(cFileIn));
    TClonesArray *pPhotonContainer = 
	new TClonesArray("HPhotonS", maxNumberOfPhotons);
    TTree *pTreeOut = 
	new TTree("FittedRingParams",
		  "Tree containing paramters of fitted rings");
    TFile pFileOut(cFileOut,"RECREATE","Fitted rings and their photons");
    TBranch *bNumberOfPhotons,*bPhotonX,*bPhotonY,*bPhotonCharge;
    Int_t NumberOfPhotonsInEvent;
    Double_t x[maxNumberOfPhotons],y[maxNumberOfPhotons],
	c[maxNumberOfPhotons];

    bNumberOfPhotons = pTreeIn -> GetBranch("Photon_");
    bPhotonX =         pTreeIn -> GetBranch("Photon.x");
    bPhotonY =         pTreeIn -> GetBranch("Photon.y");
    bPhotonCharge =    pTreeIn -> GetBranch("Photon.c");
    
    bNumberOfPhotons ->  SetAddress(&NumberOfPhotonsInEvent);
    bPhotonX         ->  SetAddress(x);
    bPhotonY         ->  SetAddress(y);
    bPhotonCharge    ->  SetAddress(c);

    FITTED_RING ringFitted;
    TBranch *bRingFitted;
    bRingFitted = pTreeIn -> GetBranch("ringFitted");
    bRingFitted -> SetAddress(&ringFitted);

    RECOGNIZED_RING ring;
    TBranch *bRingSimul;
    bRingSimul = pTreeIn -> GetBranch("ring");
    bRingSimul -> SetAddress(&ring);
    pTreeOut -> Branch("Photon",&pPhotonContainer,64000,1);
    pTreeOut -> Branch("ringFitted",&ringFitted,
		       "centerX/D:centerY/D:radius/D:variance/D:diffXY/D:numberOfPhotons/I");
    // comment this line out, if you don't need the full ring data
    TString *sBranchDefinition  = getBranchDef("exp");
    
    pTreeOut -> Branch("ring",&ring,sBranchDefinition->Data());

    Int_t nMinEntry = 0;
    Stat_t nMaxEntry = pTreeIn->GetEntries();
    cout<<nMaxEntry<<endl;  

    
    Float_t Phitmp,Thetatmp;
    Phitmp = Thetatmp = 0.;
    Float_t dThetatmp = 0.;
    Float_t dPhitmp = 0;
    Int_t *eventList = new Int_t[nMaxEntry-1];
    Float_t *deltaList = new Float_t[nMaxEntry-1];
    for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	eventList[i] = 0;
	deltaList[i] = 0;
    }
    Int_t nCntr = 1;
    for (Int_t i=nMinEntry; i<nMaxEntry; i++){

	bRingSimul       -> GetEntry(i);
	bRingFitted      -> GetEntry(i);
	bNumberOfPhotons -> GetEntry(i);
	bPhotonX         -> GetEntry(i);
	bPhotonY         -> GetEntry(i);
	bPhotonCharge    -> GetEntry(i);
	if (i==0){
	    
	    Phitmp = ring.Phi;
	    Thetatmp = ring.Theta;
	    dThetatmp = ring.DTheta;
	    dPhitmp   = ring.DPhi;
	}
	deltaList[i] = TMath::Abs(ring.DTheta) + TMath::Abs(ring.DPhi);
	//cout<<deltaList[i]<<endl;
	eventList[i] = 1;
	if (ring.Phi  == Phitmp &&
	    ring.Theta== Thetatmp ){
	    
	    if (i>=1 && deltaList[i]<deltaList[i-1]){
		eventList[i-1] = 0;
	    }else{
		eventList[i]=0;
	    }
	    
	}else{
	   
	    Phitmp = ring.Phi;
	    Thetatmp = ring.Theta;
	    //	    pTreeOut->Fill();
	    nCntr++;
	}
    }
    cout<<nCntr<<" discrete events !"<<endl;
    for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	if (eventList[i] == 1){
	    bRingSimul       -> GetEntry(i);
	    bRingFitted      -> GetEntry(i);
	    bNumberOfPhotons -> GetEntry(i);
	    bPhotonX         -> GetEntry(i);
	    bPhotonY         -> GetEntry(i);
	    bPhotonCharge    -> GetEntry(i);
	    pTreeOut->Fill();

	}
    }


    pFileOut->cd();
    pTreeOut->Write();
    pFileOut->Close();

}
