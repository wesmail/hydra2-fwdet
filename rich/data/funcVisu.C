// File: funcVisu.C
//
// Author: Thomas Eberl <Thomas.Eberl@physik.tu-muenchen.de>
// Last update by Thomas Eberl: 01/03/15 16:58:40
//
// collection of functions to be used within CINT
// load : CINT>.L funcVisu.C
// purpose: provide general visualization functionality for HADES RICH

const Double_t sd = 0.0;
const Int_t nBinsPerPad = 6;
const Int_t nRndHistos = 9;
const Float_t wave = 1240.;// 1 eV in nm
void visuExpFit(char *cFileIn){

    gSystem->Load("libFit.so");
    gROOT->LoadMacro("macros/dataDefs.C");
    gROOT->LoadMacro("macros/funcIO.C");
    gStyle->SetPalette(1);
    TTree *pTreeIn = getTree("FittedRingParams",
			  openFile(cFileIn));
    // was: "Parameters of fitted rings"
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
    TBranch *bRing;
    bRing = pTreeIn -> GetBranch("ring");
    bRing -> SetAddress(&ring);

    Int_t nMinEntry = 0;
    Stat_t nMaxEntry = pTreeIn->GetEntries();


    TObjArray *pHist1D       = createFitHistos();
    TObjArray *pHistPhoton2D = createPhotonHistos();
    TObjArray *pHistPad2D    = createSingleRingHistos();
    TObjArray *rings         = new TObjArray(nRndHistos);
    TObjArray *photonCntrs   = new TObjArray(nRndHistos);
    gRandom->SetSeed(0);
    Int_t nRandomRings = TMath::Nint((nMaxEntry-(nRndHistos+1))*(gRandom->Rndm(1)) );
    //nRandomRings = 1023;
    Int_t nHistCounter = 0;

    for (Int_t i=nMinEntry; i<nMaxEntry; i++){

	bRing            -> GetEntry(i);
	bRingFitted      -> GetEntry(i);
	bNumberOfPhotons -> GetEntry(i);
	bPhotonX         -> GetEntry(i);
	bPhotonY         -> GetEntry(i);
	bPhotonCharge    -> GetEntry(i);
	
	// fill 1D histos
	((TH1D*)(pHist1D->FindObject("fitted radius"))) 
	    -> Fill(ringFitted.radius);
	((TH1D*)(pHist1D->FindObject("fitted center_x"))) 
	    -> Fill(ringFitted.centerX);
	((TH1D*)(pHist1D->FindObject("fitted center_y"))) 
	    -> Fill(ringFitted.centerY);
	((TH1D*)(pHist1D->FindObject("estim radius variance"))) 
	    -> Fill(ringFitted.variance);
	((TH1D*)(pHist1D->FindObject("center delta"))) 
	    -> Fill(ringFitted.diffXY);
	((TH1D*)(pHist1D->FindObject("no of photons"))) 
	    -> Fill(ringFitted.numberOfPhotons);
	// fill 2D histos with rings	
	if (i>=nRandomRings && i<nRandomRings+nRndHistos){
	    char *cnt  = new char[20];
	    char *cnt1 = new char[20];
  	    sprintf(cnt,"single ring ph %d",nHistCounter+1);
	    sprintf(cnt1,"single ring %d",nHistCounter+1);
	    TH2F* tmp  = (TH2F*)(pHistPhoton2D -> FindObject(cnt));
	    TH2F* tmp1 = (TH2F*)(pHistPad2D    -> FindObject(cnt1));
	    rings->Add(new TArc(ringFitted.centerX+sd,ringFitted.centerY+sd,
			       ringFitted.radius,0,360)); 
	    for (Int_t k=0;k<ring.nPads;k++){
		tmp1 -> Fill(ring.xcoord[k],ring.ycoord[k],ring.charge[k]);
	    }
	    TObjArray *photons = new TObjArray(NumberOfPhotonsInEvent);
	    for (Int_t j=0;j<NumberOfPhotonsInEvent;j++){
		tmp -> Fill(x[j],y[j],c[j]);
		photons->Add(new TArc(x[j]+sd,y[j]+sd,.2,0,360));
	    }
	    photonCntrs->Add(photons);
	    nHistCounter++;
	}
    
    
    } // end of event loop
    
    TObjArray *pCanvases = new TObjArray(3);
    pCanvases->Add(draw2DPadsPhotons(photonCntrs,pHistPad2D,rings));
    pCanvases->Add(draw2DHistos(pHistPhoton2D,rings));
    pCanvases->Add(draw1DHistos(pHist1D));
    createGifImages(pCanvases,cFileIn);
    createPSImages(pCanvases,cFileIn);
}

void visuSimFit(char *cFileIn){

    gSystem->Load("libFit.so");
    gROOT->LoadMacro("macros/dataDefs.C");
    gROOT->LoadMacro("macros/funcIO.C");
    gStyle->SetPalette(1);
    TTree *pTreeIn = getTree("FittedRingParams",
			  openFile(cFileIn));
    // was: "Parameters of fitted rings"
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

    SIMUL_RING ring;
    TBranch *bRing;
    bRing = pTreeIn -> GetBranch("simul_ring");
    bRing -> SetAddress(&ring);

    Int_t nMinEntry = 0;
    Stat_t nMaxEntry = pTreeIn->GetEntries();


    TObjArray *pHist1D       = createFitHistos();
    TObjArray *pHistPhoton2D = createPhotonHistos();
    TObjArray *pHistPad2D    = createSingleRingHistos();
    TObjArray *rings         = new TObjArray(nRndHistos);
    TObjArray *photonCntrs   = new TObjArray(nRndHistos);
    gRandom->SetSeed(0);
    Int_t nRandomRings = TMath::Nint((nMaxEntry-(nRndHistos+1))*(gRandom->Rndm(1)) );
    //nRandomRings = 1023;
    Int_t nHistCounter = 0;

    for (Int_t i=nMinEntry; i<nMaxEntry; i++){

	bRing            -> GetEntry(i);
	bRingFitted      -> GetEntry(i);
	bNumberOfPhotons -> GetEntry(i);
	bPhotonX         -> GetEntry(i);
	bPhotonY         -> GetEntry(i);
	bPhotonCharge    -> GetEntry(i);
	
	// fill 1D histos
	((TH1D*)(pHist1D->FindObject("fitted radius"))) 
	    -> Fill(ringFitted.radius);
	((TH1D*)(pHist1D->FindObject("fitted center_x"))) 
	    -> Fill(ringFitted.centerX);
	((TH1D*)(pHist1D->FindObject("fitted center_y"))) 
	    -> Fill(ringFitted.centerY);
	((TH1D*)(pHist1D->FindObject("estim radius variance"))) 
	    -> Fill(ringFitted.variance);
	((TH1D*)(pHist1D->FindObject("center delta"))) 
	    -> Fill(ringFitted.diffXY);
	((TH1D*)(pHist1D->FindObject("no of photons"))) 
	    -> Fill(ringFitted.numberOfPhotons);
	// fill 2D histos with rings	
	if (i>=nRandomRings && i<nRandomRings+nRndHistos){
	    char *cnt  = new char[20];
	    char *cnt1 = new char[20];
  	    sprintf(cnt,"single ring ph %d",nHistCounter+1);
	    sprintf(cnt1,"single ring %d",nHistCounter+1);
	    TH2F* tmp  = (TH2F*)(pHistPhoton2D -> FindObject(cnt));
	    TH2F* tmp1 = (TH2F*)(pHistPad2D    -> FindObject(cnt1));
	    rings->Add(new TArc(ringFitted.centerX+sd,ringFitted.centerY+sd,
			       ringFitted.radius,0,360)); 
	    for (Int_t k=0;k<ring.nPads;k++){
		tmp1 -> Fill(ring.xcoord[k],ring.ycoord[k],ring.charge[k]);
	    }
	    TObjArray *photons = new TObjArray(NumberOfPhotonsInEvent);
	    for (Int_t j=0;j<NumberOfPhotonsInEvent;j++){
		tmp -> Fill(x[j],y[j],c[j]);
		photons->Add(new TArc(x[j]+sd,y[j]+sd,.2,0,360));
	    }
	    photonCntrs->Add(photons);
	    nHistCounter++;
	}
    
    
    } // end of event loop
    
    TObjArray *pCanvases = new TObjArray(3);
    pCanvases->Add(draw2DPadsPhotons(photonCntrs,pHistPad2D,rings));
    pCanvases->Add(draw2DHistos(pHistPhoton2D,rings));
    pCanvases->Add(draw1DHistos(pHist1D));
    createGifImages(pCanvases,cFileIn);
    createPSImages(pCanvases,cFileIn);
}

void visuSimPhotons(char *cFileIn){

    gSystem->Load("libFit.so");
    gROOT->LoadMacro("macros/dataDefs.C");
    gROOT->LoadMacro("macros/funcIO.C");
    gStyle->SetPalette(1);
    TTree *pTreeIn = getTree("FittedRingParams",
			  openFile(cFileIn));
    // was: "Parameters of fitted rings"
 
    FITTED_RING ringFitted;
    TBranch *bRingFitted;
    bRingFitted = pTreeIn -> GetBranch("ringFitted");
    bRingFitted -> SetAddress(&ringFitted);

    SIMUL_RING ring;
    TBranch *bRingSimul;
    bRingSimul = pTreeIn -> GetBranch("simul_ring");
    bRingSimul -> SetAddress(&ring);

    Int_t nMinEntry = 0;
    Stat_t nMaxEntry = pTreeIn->GetEntries();


    TObjArray *pHist1D       = createFitHistos();
    TObjArray *pHistPhoton2D = createPhotonHistos();
    TObjArray *pHistPad2D    = createSingleRingHistos();
    TObjArray *rings         = new TObjArray(nRndHistos);
    TObjArray *photonCntrs   = new TObjArray(nRndHistos);
    gRandom->SetSeed(0);
    Int_t nRandomRings = TMath::Nint((nMaxEntry-(nRndHistos+1))*(gRandom->Rndm(1)) );
    //nRandomRings = 1023;
    Int_t nHistCounter = 0;

    for (Int_t i=nMinEntry; i<nMaxEntry; i++){

	bRingSimul       -> GetEntry(i);
	bRingFitted      -> GetEntry(i);
	
	// fill 1D histos
	((TH1D*)(pHist1D->FindObject("fitted radius"))) 
	    -> Fill(ringFitted.radius);
	((TH1D*)(pHist1D->FindObject("fitted center_x"))) 
	    -> Fill(ringFitted.centerX);
	((TH1D*)(pHist1D->FindObject("fitted center_y"))) 
	    -> Fill(ringFitted.centerY);
	((TH1D*)(pHist1D->FindObject("estim radius variance"))) 
	    -> Fill(ringFitted.variance);
	((TH1D*)(pHist1D->FindObject("center delta"))) 
	    -> Fill(ringFitted.diffXY);
	((TH1D*)(pHist1D->FindObject("no of photons"))) 
	    -> Fill(ringFitted.numberOfPhotons);
	// fill 2D histos with rings	
	if (i>=nRandomRings && i<nRandomRings+nRndHistos){
	    char *cnt  = new char[20];
	    char *cnt1 = new char[20];
  	    sprintf(cnt,"single ring ph %d",nHistCounter+1);
	    sprintf(cnt1,"single ring %d",nHistCounter+1);
	    TH2F* tmp  = (TH2F*)(pHistPhoton2D -> FindObject(cnt));
	    TH2F* tmp1 = (TH2F*)(pHistPad2D    -> FindObject(cnt1));
	    rings->Add(new TArc(ringFitted.centerX+sd,ringFitted.centerY+sd,
			       ringFitted.radius,0,360)); 
	    for (Int_t k=0;k<ring.nPads;k++){
		tmp1 -> Fill(ring.xcoord[k],ring.ycoord[k],ring.charge[k]);
	    }
	    TObjArray *photons = new TObjArray(ring.nPhotons);
	    for (Int_t j=0;j<ring.nPhotons;j++){
		tmp -> Fill(ring.fPhX[j],ring.fPhY[j],ring.fPhE[j]);
		photons->Add(new TArc(ring.fPhX[j]+sd,ring.fPhY[j]+sd,.075,0,360));
	    }
	    photonCntrs->Add(photons);
	    nHistCounter++;
	}
    
    
    } // end of event loop
    
    TObjArray *pCanvases = new TObjArray(3);
    pCanvases->Add(draw2DPadsPhotons(photonCntrs,pHistPad2D,rings));
    pCanvases->Add(draw2DHistos(pHistPhoton2D,rings));
    pCanvases->Add(draw1DHistos(pHist1D));
    createGifImages(pCanvases,cFileIn);
    createPSImages(pCanvases,cFileIn);
}

void visuSimPhoton(char *cFileIn){

    gSystem->Load("libFit.so");
    gROOT->LoadMacro("macros/dataDefs.C");
    gROOT->LoadMacro("macros/funcIO.C");
    gStyle->SetPalette(1);
    TTree *pTreeIn = getTree("FittedRingParams",
			  openFile(cFileIn));
    // was: "Parameters of fitted rings"
 
    FITTED_RING ringFitted;
    TBranch *bRingFitted;
    bRingFitted = pTreeIn -> GetBranch("ringFitted");
    bRingFitted -> SetAddress(&ringFitted);

    SIMUL_RING ring;
    TBranch *bRingSimul;
    bRingSimul = pTreeIn -> GetBranch("simul_ring");
    bRingSimul -> SetAddress(&ring);

    Int_t nMinEntry = 0;
    Stat_t nMaxEntry = pTreeIn->GetEntries();
    cout<<nMaxEntry<<endl;

    TCanvas* c1 = new TCanvas("single_ring",
			      "single_ring",
			      4,0,600,600);
   gStyle->SetOptStat(0);
   single_ring->ToggleEventStatus();
   single_ring->Range(-2.21429,-2.11765,14.8143,14.1176);
   single_ring->SetFillColor(19);
   single_ring->SetBorderSize(2);
   single_ring->SetRightMargin(0.135906);
   

    TH2F *hist = new TH2F("sim_ring","sim_ring",
			      fieldsize,-0.5,fieldsize-.5,
			      fieldsize,-0.5,fieldsize-.5);
    sim_ring->GetXaxis()->SetLabelSize(0.06);
    sim_ring->GetYaxis()->SetLabelSize(0.06);
    sim_ring->SetMaximum(100);
    
    TArc *fitted_circle = 0;
    TArc *photon = 0;
    TArc *circ_orig = new TArc(6.+sd,6.+sd,4,0,360);
    circ_orig->SetLineColor(1);
    circ_orig->SetLineWidth(2);
    for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	//Int_t i=200;
	bRingSimul       -> GetEntry(i);
	bRingFitted      -> GetEntry(i);
	hist->Reset();
	for (Int_t k=0;k<ring.nPads;k++){
	    hist -> Fill(ring.xcoord[k],ring.ycoord[k],ring.charge[k]);
	}
	hist->Draw("colz");
	circ_orig->Draw();
	fitted_circle = new TArc(ringFitted.centerX+sd,
				 ringFitted.centerY+sd,
				 ringFitted.radius,0,360);
	
	fitted_circle->SetLineColor(2);
	fitted_circle->SetLineWidth(2);
	fitted_circle->Draw();
	
	
	for (Int_t j=0;j<ring.nPhotons;j++){
	    photon = new TArc(ring.fPhX[j]+sd,ring.fPhY[j]+sd,.075,0,360);
	    photon->SetLineColor(1);
	    photon->SetLineWidth(1);
	    photon->SetFillColor(1);
	    photon->Draw();
	}
	c1 -> Modified();
	c1 -> Update();
	//  char* decision = new char[2];
//  	scanf("%[^\n]%*c",decision);
//  	if(!strcmp(decision,"f")) {
//  	    char *num = new char[10];
//  	    sprintf(num,"ring_%d.ps",i);
//  	    c1->Print(num);
//  	    cout<< " canvas written to file: ring.ps"<<endl;
//  	}
	char *num = new char[30];
        sprintf(num,"pix/simringphfit/ring_%d.eps",i);
        c1->Print(num);
	
    }
}

void visuExpPhoton(char *cFileIn){

    gSystem->Load("libFit.so");
    gROOT->LoadMacro("macros/dataDefs.C");
    gROOT->LoadMacro("macros/funcIO.C");
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

    TCanvas* c1 = new TCanvas("single_ring",
			      "single_ring",
			      4,0,600,600);
   gStyle->SetOptStat(0);
   single_ring->ToggleEventStatus();
   single_ring->Range(-2.21429,-2.11765,14.8143,14.1176);
   single_ring->SetFillColor(19);
   single_ring->SetBorderSize(2);
   single_ring->SetRightMargin(0.135906);
   

    TH2F *hist = new TH2F("exp_ring","exp_ring",
			      fieldsize,-0.5,fieldsize-.5,
			      fieldsize,-0.5,fieldsize-.5);
    exp_ring->GetXaxis()->SetLabelSize(0.06);
    exp_ring->GetYaxis()->SetLabelSize(0.06);
    exp_ring->SetMaximum(100);
    
    TArc *fitted_circle = 0;
    TArc *photon = 0;
    TArc *circ_orig = new TArc(6.+sd,6.+sd,4,0,360);
    circ_orig->SetLineColor(1);
    circ_orig->SetLineWidth(2);
    for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	//Int_t i=200;
	bRingSimul       -> GetEntry(i);
	bRingFitted      -> GetEntry(i);
	bNumberOfPhotons -> GetEntry(i);
	bPhotonX         -> GetEntry(i);
	bPhotonY         -> GetEntry(i);
	bPhotonCharge    -> GetEntry(i);
	cout<<"evt no: "<<i<<endl;
	if (1>0){
	    hist->Reset();
	    for (Int_t k=0;k<ring.nPads;k++){
		hist -> Fill(ring.xcoord[k],ring.ycoord[k],ring.charge[k]);
	    }
	    hist->Draw("colz");
	    circ_orig->Draw();
	    fitted_circle = new TArc(ringFitted.centerX+sd,
				     ringFitted.centerY+sd,
				     ringFitted.radius,0,360);
	    
	    fitted_circle->SetLineColor(2);
	    fitted_circle->SetLineWidth(2);
	    fitted_circle->Draw();
	    
	    
	    for (Int_t j=0;j<NumberOfPhotonsInEvent;j++){
		photon = new TArc(x[j]+sd,y[j]+sd,.2,0,360);
		photon->SetLineColor(1);
		photon->SetLineWidth(1);
		photon->SetFillColor(1);
		photon->Draw();
	    }
	    c1 -> Modified();
	    c1 -> Update();
	    char* decision = new char[2];
	    scanf("%[^\n]%*c",decision);
	    if(!strcmp(decision,"f")) {
		char *num = new char[35];
		sprintf(num,"pix/expringfit/ring_%d.eps",i);
		c1->Print(num);
	    }
	    
	    //char *num = new char[30];
	    //sprintf(num,"pix/expringfit/ring_%d.eps",i);
	    //c1->Print(num);
	}
	
    }// end of data loop
}

void visuExpRing(char *cFileIn){

    gSystem->Load("libFit.so");
    gROOT->LoadMacro("macros/dataDefs.C");
    gROOT->LoadMacro("macros/funcIO.C");
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

    Int_t nMinEntry = 100;
    Stat_t nMaxEntry = pTreeIn->GetEntries();
    cout<<nMaxEntry<<endl;
    nMaxEntry = 500;
    TCanvas* c1 = new TCanvas("single_ring",
			      "single_ring",
			      4,0,600,600);
   gStyle->SetOptStat(0);
   single_ring->ToggleEventStatus();
   single_ring->Range(-2.21429,-2.11765,14.8143,14.1176);
   single_ring->SetFillColor(19);
   single_ring->SetBorderSize(2);
   single_ring->SetRightMargin(0.135906);
   
   Int_t zoom = 10;
    TH2F *hist = new TH2F("exp_ring","exp_ring",
			      fieldsize+zoom,-0.5,fieldsize+zoom-.5,
			      fieldsize+zoom,-0.5,fieldsize+zoom-.5);
    exp_ring->GetXaxis()->SetLabelSize(0.06);
    exp_ring->GetYaxis()->SetLabelSize(0.06);
    exp_ring->SetMaximum(100);
    
    TArc *fitted_circle = 0;
    TArc *photon = 0;
    TArc *circ_orig = new TArc(6.+sd,6.+sd,4,0,360);
    circ_orig->SetLineColor(1);
    circ_orig->SetLineWidth(2);
    for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	//Int_t i=200;
	bRingSimul       -> GetEntry(i);
	bRingFitted      -> GetEntry(i);
	bNumberOfPhotons -> GetEntry(i);
	bPhotonX         -> GetEntry(i);
	bPhotonY         -> GetEntry(i);
	bPhotonCharge    -> GetEntry(i);
	cout<<"evt no: "<<i<<endl;
	if (1>0){
	    hist->Reset();
	    for (Int_t k=0;k<ring.nPads;k++){
		hist -> Fill(ring.xcoord[k]+zoom/2,ring.ycoord[k]+zoom/2,ring.charge[k]);
	    }
	    hist->Draw("colz");
	    //	    circ_orig->Draw();
	    fitted_circle = new TArc(ringFitted.centerX+sd+zoom/2,
				     ringFitted.centerY+sd+zoom/2,
				     ringFitted.radius,0,360);
	    
	    fitted_circle->SetLineColor(2);
	    fitted_circle->SetLineWidth(2);
	    fitted_circle->Draw();
	    
	    
	    for (Int_t j=0;j<NumberOfPhotonsInEvent;j++){
		photon = new TArc(x[j]+sd,y[j]+sd,.2,0,360);
		photon->SetLineColor(1);
		photon->SetLineWidth(1);
		photon->SetFillColor(1);
		//photon->Draw();
	    }
	    c1 -> Modified();
	    c1 -> Update();
	    char* decision = new char[2];
	    //scanf("%[^\n]%*c",decision);
	    //if(!strcmp(decision,"f")) {
		char *num = new char[35];
		sprintf(num,"pix/expringfit/ring_%d.gif",i);
		c1->Print(num);
		//}
	    
	    //char *num = new char[30];
	    //sprintf(num,"pix/expringfit/ring_%d.eps",i);
	    //c1->Print(num);
	}
	
    }// end of data loop
}

void createGifImages(TObjArray* pCanvases,char *cFileIn){
    TString fileName(cFileIn);
    fileName.Remove(fileName.Length()-5, fileName.Length() );
    for (Int_t i=0;i<((pCanvases->GetLast())+1);i++){
	TString *canvasName = new TString(((TCanvas*)((*pCanvases)[i]))->GetName());
	canvasName->Append("_");
	canvasName->Append(fileName.Data());
	canvasName->Append(".gif");
	((TCanvas*)((*pCanvases)[i])) -> Print(canvasName.Data());
	delete canvasName;
    }
}

void createPSImages(TObjArray* pCanvases,char *cFileIn){
    TString fileName(cFileIn);
    fileName.Remove(fileName.Length()-5, fileName.Length() );
    for (Int_t i=0;i<((pCanvases->GetLast())+1);i++){
	TString *canvasName = new TString(((TCanvas*)((*pCanvases)[i]))->GetName());
	canvasName->Append("_");
	canvasName->Append(fileName.Data());
	canvasName->Append(".ps");
	((TCanvas*)((*pCanvases)[i])) -> Print(canvasName.Data());
	delete canvasName;
    }
}

TCanvas* draw2DPadsPhotons(TObjArray *photonCntrs, TObjArray* pHistPad2D, 
		       TObjArray *rings){
    TCanvas* c1 = new TCanvas("2D_Fitted_pad_photon_rings",
			      "2D_Fitted_pad_photon_rings",
			      4,0,900,900);
    c1->Divide(3,3);
    c1->cd();
    gStyle->SetOptStat(0);
    TArc *circ_orig = new TArc(6.+sd,6.+sd,4,0,360);
    circ_orig->SetLineColor(1);
    circ_orig->SetLineWidth(2);
    TObjArray *photons;
    for (int i=0;i<nRndHistos;i++){
	c1->cd(i+1);
	((TH2F*)((*pHistPad2D)[i])) -> SetMaximum(100);
	((TH2F*)((*pHistPad2D)[i])) -> Draw("colz");
	photons = ((TObjArray*)((*photonCntrs)[i]));
	for (int j=0;j<photons->GetLast();j++){
	    ((TArc*)((*photons)[j])) -> SetLineColor(1);
	    ((TArc*)((*photons)[j])) -> SetLineWidth(1);
	    ((TArc*)((*photons)[j])) -> SetFillColor(1);
	    ((TArc*)((*photons)[j])) -> Draw();
	}
	((TArc*)((*rings)[i]))     -> SetLineColor(2);
	((TArc*)((*rings)[i]))     -> SetLineWidth(1);
	((TArc*)((*rings)[i]))     -> Draw();
	circ_orig->Draw();
    }
    return c1;
}

TCanvas* draw1DHistos(TObjArray *pHist1D){
    TCanvas *c1 = new TCanvas("1D_Ring_characteristics","1D_Ring_characteristics",
			      100,0,900,600);
    gStyle->SetOptStat(0);
    c1->Divide(3,2);
    c1->cd();
    TH1D *h = 0;
    for (Int_t i=0;i<((pHist1D->GetLast())+1);i++){
	if ( (h = ( (TH1D*) (*pHist1D)[i] ))){
	    c1->cd(i+1);
	    h->Draw();
	}
    }
    return c1;
}

TCanvas* draw2DHistos(TObjArray *pHistPhoton2D, TObjArray *rings){
    TCanvas* c1 = new TCanvas("2D_Fitted_photon_rings","2D_Fitted_photon_rings",
			      4,0,900,900);
    c1->Divide(3,3);
    c1->cd();
    gStyle->SetOptStat(0);
    TArc *circ_orig = new TArc(6.+sd,6.+sd,4,0,360);
    circ_orig->SetLineColor(1);
    circ_orig->SetLineWidth(2);
    for (int i=0;i<9;i++){
	c1->cd(i+1);
	((TH2F*)((*pHistPhoton2D)[i])) -> SetMaximum(10);
	((TH2F*)((*pHistPhoton2D)[i])) -> SetMinimum(5);
	((TH2F*)((*pHistPhoton2D)[i])) -> Draw("colz");
	((TArc*)((*rings)[i]))         -> SetLineColor(2);
	((TArc*)((*rings)[i]))         -> SetLineWidth(1);
	((TArc*)((*rings)[i]))         -> Draw();
	circ_orig->Draw();
    }
    return c1;
}



TObjArray* createFitHistos(){
    TObjArray *hist_list = new TObjArray(10);
    TH1D *hist = new TH1D("fitted radius","fitted radius",100,3.5,6.);
    hist_list->Add(hist);
    TH1D *hist = new TH1D("fitted center_x","fitted center_x",100,5,9);
    hist_list->Add(hist);
    TH1D *hist = new TH1D("fitted center_y","fitted center_y",100,5,9);
    hist_list->Add(hist);
    TH1D *hist = new TH1D("estim radius variance","estim radius variance",100,0,5);
    hist_list->Add(hist);
    TH1D *hist = new TH1D("center delta","center delta",100,0,5);
    hist_list->Add(hist);
    TH1D *hist = new TH1D("no of photons","no of photons",100,3,50);
    hist_list->Add(hist);
            
    return hist_list;
}

TObjArray* createSingleRingHistos(){
    const Int_t numberOfRingHistos = nRndHistos;
    TObjArray *ring_histos = new TObjArray(numberOfRingHistos);
    for (int i=0;i<numberOfRingHistos;i++){
	
	char *cnt = new char[20];
	sprintf(cnt,"single ring %d",ring_histos.GetEntries()+1);
	
	TH2F *hist = new TH2F(cnt,cnt,
			      fieldsize,-0.5,fieldsize-.5,
			      fieldsize,-0.5,fieldsize-.5);
	hist->SetMaximum(100);
	ring_histos->Add(hist);
    }
    return ring_histos;
}

TObjArray* createPhotonHistos(){
    const Int_t numberOfRingHistos = nRndHistos;
    TObjArray *ring_histos = new TObjArray(numberOfRingHistos);
    for (int i=0;i<numberOfRingHistos;i++){
	
	char *cnt = new char[20];
	sprintf(cnt,"single ring ph %d",ring_histos.GetEntries()+1);
	
	TH2F *hist = new TH2F(cnt,cnt,
			      fieldsize*nBinsPerPad,-0.5,fieldsize-.5,
			      fieldsize*nBinsPerPad,-0.5,fieldsize-.5 );
	hist->SetMaximum(100);
	ring_histos->Add(hist);
    }
    return ring_histos;
}


TH1D* creaHist1D(Double_t *d_arr, Int_t size, Int_t bins, Int_t ini, Int_t end){
    TH1D *hist = new TH1D("h1","h1",bins,ini,end);
    for (int i=0;i<size;i++){
	    hist->Fill(d_arr[i]);
    }
    return hist;
}

TCanvas* createDivCanvas(Int_t col, Int_t row, char *canvasName){
    TCanvas *canvas = new TCanvas(canvasName,canvasName,0,0,300*col,300*row);
    canvas -> Divide(col,row);
    canvas -> Draw();
    return canvas;
}

void visu1D(char *cFileIn){
gSystem->Load("libFit.so");
    gROOT->LoadMacro("macros/dataDefs.C");
    gROOT->LoadMacro("macros/funcIO.C");
    gStyle->SetPalette(1);
    TTree *pTreeIn = getTree("FittedRingParams",
			  openFile(cFileIn));
    TCanvas *c = new TCanvas("HADES_X","HADES_X",4,0,600,600);
   gStyle->SetOptStat(1);
   HADES_X->ToggleEventStatus();
   HADES_X->Range(-2.21429,-2.11765,14.8143,14.1176);
   HADES_X->SetFillColor(19);
   HADES_X->SetBorderSize(2);
   HADES_X->SetRightMargin(0.135906);
   HADES_X->SetFrameFillColor(10);
   TH1F *hist = new TH1F("ringFitted_variance_EXP","ringFitted_variance_EXP",100,0.,1.5);
    ringFitted_variance_EXP->GetXaxis()->SetLabelSize(0.06);
    ringFitted_variance_EXP->SetFillColor(2);
    pTreeIn->Draw("ringFitted.variance>>ringFitted.variance_EXP");
    c->Modified();
    c->Draw();
}

void visu1Dradius(char *cFileIn){
gSystem->Load("libFit.so");
    gROOT->LoadMacro("macros/dataDefs.C");
    gROOT->LoadMacro("macros/funcIO.C");
    gStyle->SetPalette(1);
    TTree *pTreeIn = getTree("FittedRingParams",
			  openFile(cFileIn));
    TCanvas *c = new TCanvas("HADES_X","HADES_X",4,0,600,600);
   gStyle->SetOptStat(1);
   HADES_X->ToggleEventStatus();
   HADES_X->Range(-2.21429,-2.11765,14.8143,14.1176);
   HADES_X->SetFillColor(19);
   HADES_X->SetBorderSize(2);
   HADES_X->SetRightMargin(0.135906);
   HADES_X->SetFrameFillColor(10);
   TH1F *hist = new TH1F("ringFitted_radius_EXP","ringFitted_radius_EXP",100,3.,5.);
    ringFitted_radius_EXP->GetXaxis()->SetLabelSize(0.06);
    ringFitted_radius_EXP->SetFillColor(2);
    pTreeIn->Draw("ringFitted.radius>>ringFitted_radius_EXP");
    c->Modified();
    c->Draw();
}


void visu2DradiusTheta(char *cFileIn){
gSystem->Load("libFit.so");
    gROOT->LoadMacro("macros/dataDefs.C");
    gROOT->LoadMacro("macros/funcIO.C");
    gStyle->SetPalette(1);
    TTree *pTreeIn = getTree("FittedRingParams",
			  openFile(cFileIn));
    TCanvas *c = new TCanvas("HADES_X","HADES_X",4,0,600,600);
    gStyle->SetOptStat(0);
    HADES_X->ToggleEventStatus();
    //HADES_X->Range(-2.21429,-2.11765,14.8143,14.1176);
    HADES_X->SetFillColor(19);
    HADES_X->SetBorderSize(2);
    HADES_X->SetRightMargin(0.135906);
    HADES_X->SetFrameFillColor(10);
    TH2F *hist = new TH2F("Radius_vs_PolarAngle","Radius_vs_PolarAngle",
			  10,15,80,10,1.5,6.5);
    Radius_vs_PolarAngle->GetXaxis()->SetLabelSize(0.06);
    //->SetFillColor(2);
    pTreeIn->Draw("ringFitted.radius:simul_ring.Theta>>Radius_vs_PolarAngle");
    c->Modified();
    c->Draw();
}

// HTreeDrawPanel *a = new HTreeDrawPanel(T,gClient->GetRoot(), 280, 250);
