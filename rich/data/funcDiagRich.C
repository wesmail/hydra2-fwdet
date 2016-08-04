// File: funcDiagRICH.C
//
// Author: Thomas Eberl <Thomas.Eberl@physik.tu-muenchen.de>
// Last update by Thomas Eberl: 01/04/27 20:11:41
// RICH HIT and RICH IPU diagnostics

void showRichDiagnosis(char *cFileIn, char *cFileOut){

TFile *fileobj= new TFile(cFileIn,"READ");

// Histogram definition
    TH1D *hitsector = 0;
    TH1D *hittheta = 0;
    TH1D *hitphi = 0;
    TH1D *hitradius = 0;
    TH1D *hitcentroid =0;
    TH1D *hithoutra = 0;
    TH1D *hitpatmat = 0;
    TH1D *hitringampl = 0;
    TH1D *hitringpadnr = 0;
    TH1D *hitringx = 0;
    TH1D *hitringy = 0;
    TH1D *hitringlocMax4 = 0;
    TH2F *hitringxy = 0;
    TH2F *hithoutrapatmat = 0;
    TH2F *hitthetaphi1 = 0;
    TH2F *hitthetaphi2 = 0;
    TH2F *hitthetaphi3 = 0;
    TH2F *hitthetaphi4 = 0;
    TH2F *hitthetaphi5 = 0;
    TH2F *hitthetaphi6 = 0;
    TH2F *hitpatmattheta = 0;
    TH2F *hithoutratheta = 0;
    TH2F *ringampltheta = 0;
    TH2F *radiustheta = 0;
    TH1D *ringmult = 0;

// Histograms for the RICH IPU
    TH1D *ipucolpat = 0;
    TH1D *ipurow = 0;
    TH1D *ipucol = 0;
    TH1D *ipusegid = 0;
    TH1D *ipufifonb = 0;
    TH1D *ipurownb = 0;
    TH2F *ipurowcol = 0;
    TH2F *ipurowhitcol = 0;
    TH2F *ipucolhitrow = 0;

    TFile outfile(cFileOut,"RECREATE","RICH Diagnosis Histograms for DSTs");


    HTree *T = (HTree *) fileobj->Get ("T");
    
    Stat_t nEntries = T->GetEntries();
    
    //    Stat_t nMaxEntry = 3000;
    
    hitsector        = new TH1D("Ring_sector_distribution",
			        "Ring_sector_distribution",25,0,6);
    hittheta         = new TH1D("Ring_theta","Ring_theta",200,0,100);
    hitphi           = new TH1D("Ring_phi","Ring_phi",360,0,360);
    hitradius        = new TH1D("Ring_radius","Ring_radius",80,2,6);
    hitcentroid      = new TH1D("Centroid","Centroid",100,0,3);
    hithoutra        = new TH1D("RingQual_HouTra","RingQual_HouTra",100,0,500);
    hitpatmat        = new TH1D("RingQual_PatMat","RingQual_PatMat",100,0,1000);
    hitringampl      = new TH1D("RingChargeAmpl","RingChargeAmpl",100,0,1000);
    hitringpadnr     = new TH1D("RingPadNr","RingPadNr",35,5,40);
    hitringx         = new TH1D("RingCol","RingCol",92,0,92);
    hitringy         = new TH1D("RingRow","RingRow",90,0,90);
    hitringlocMax4   = new TH1D("RingLocMax4","RingLocMax4",15,0,15);
    hitringxy        = new TH2F("Ring_padXY","Ring_padXY",92,0,92,90,0,90);
    hithoutrapatmat  = new TH2F("RingQual_HT_PM","RingQual_HT_PM",200,0,200,200,0,800);
    hitthetaphi1     = new TH2F("Lepton_angle_sec1","Lepton_angle_sec1",60,60,120,90,0,90);
    hitthetaphi2     = new TH2F("Lepton_angle_sec2","Lepton_angle_sec2",60,120,180,90,0,90);
    hitthetaphi3     = new TH2F("Lepton_angle_sec3","Lepton_angle_sec3",60,180,240,90,0,90);
    hitthetaphi4     = new TH2F("Lepton_angle_sec4","Lepton_angle_sec4",60,240,300,90,0,90);
    hitthetaphi5     = new TH2F("Lepton_angle_sec5","Lepton_angle_sec5",60,300,360,90,0,90);
    hitthetaphi6     = new TH2F("Lepton_angle_sec6","Lepton_angle_sec6",60,0,60,90,0,90);
    hitpatmattheta   = new TH2F("PatMatvsTheta","PatMatvsTheta",200,200,800,90,0,90);
    hithoutratheta   = new TH2F("HouTravsTheta","HouTravsTheta",200,0,200,90,0,90);
    ringampltheta    = new TH2F("RingAmplvsTheta","RingAmplvsTheta",100,0,1000,90,0,90);
    radiustheta      = new TH2F("RadiusvsTheta","RadiusvsTheta",40,2,6,90,0,90);
    ringmult         = new TH1D("RingMultPerEvt","RingMultPerEvt",16,1,8);

    // IPU
    ipucolpat    = new TH1D("RICH_IPU_Column_Pattern","RICH_IPU_Column_Pattern",180,0,180);
    ipurow       = new TH1D("RICH_IPU_Row","RICH_IPU_Row",90,0,90);
    ipucol       = new TH1D("RICH_IPU_Col","RICH_IPU_Col",92,0,92);
    ipusegid     = new TH1D("RICH_IPU_Segment_Id","RICH_IPU_Segment_Id",8,0,8);
    ipufifonb    = new TH1D("RICH_IPU_Fifo_Nb","RICH_IPU_Fifo_Nb",10,0,10);
    ipurownb     = new TH1D("RICH_IPU_Row_Nb","RICH_IPU_Row_Nb",100,20,120);
    ipurowcol    = new TH2F("RICH_IPU_Row_Col","IPU_RICH_Row_Col",90,0,90,92,0,92);
    ipurowhitcol = new TH2F("RICH_IPU_Row_HIT_Col","RICH_IPU_Row_HIT_Col",90,0,90,90,0,90);
    ipucolhitrow = new TH2F("RICH_IPU_Col_HIT_Row","RICH_IPU_Col_HIT_Row",92,0,92,92,0,92);


    T->Draw("HRichHit.fData.nSector>>Ring_sector_distribution");
    T->Draw("HRichHit.fData.fTheta>>Ring_theta");
    T->Draw("HRichHit.fData.fPhi>>Ring_phi");
    T->Draw("HRichHit.fData.radius>>Ring_radius");
    T->Draw("HRichHit.fData.centroid>>Centroid");
    T->Draw("HRichHit.fData.iRingHouTra>>RingQual_HouTra");
    T->Draw("HRichHit.fData.iRingPatMat>>RingQual_PatMat");
    T->Draw("HRichHit.fData.iRingAmplitude>>RingChargeAmpl");
    T->Draw("HRichHit.fData.iRingPadNr>>RingPadNr");
    T->Draw("HRichHit.fData.iRingX>>RingCol");
    T->Draw("HRichHit.fData.iRingY>>RingRow");
    T->Draw("HRichHit.fData.iRingLocalMax4>>RingLocMax4");
    T->Draw("HRichHit.fData.iRingX:HRichHit.fData.iRingY>>Ring_padXY");
    T->Draw("HRichHit.fData.iRingPatMat:HRichHit.fData.iRingHouTra>>RingQual_HT_PM");
    T->Draw("HRichHit.fData.fTheta:HRichHit.fData.fPhi>>Lepton_angle_sec1","HRichHit.fData.nSector==0");
    T->Draw("HRichHit.fData.fTheta:HRichHit.fData.fPhi>>Lepton_angle_sec2","HRichHit.fData.nSector==1");
    T->Draw("HRichHit.fData.fTheta:HRichHit.fData.fPhi>>Lepton_angle_sec3","HRichHit.fData.nSector==2");
    T->Draw("HRichHit.fData.fTheta:HRichHit.fData.fPhi>>Lepton_angle_sec4","HRichHit.fData.nSector==3");
    T->Draw("HRichHit.fData.fTheta:HRichHit.fData.fPhi>>Lepton_angle_sec5","HRichHit.fData.nSector==4");
    T->Draw("HRichHit.fData.fTheta:HRichHit.fData.fPhi>>Lepton_angle_sec6","HRichHit.fData.nSector==5");
    T->Draw("HRichHit.fData.fTheta:HRichHit.fData.iRingPatMat>>PatMatvsTheta");
    T->Draw("HRichHit.fData.fTheta:HRichHit.fData.iRingHouTra>>HouTravsTheta");
    T->Draw("HRichHit.fData.fTheta:HRichHit.fData.iRingAmplitude>>RingAmplvsTheta");
    T->Draw("HRichHit.fData.fTheta:HRichHit.fData.radius>>RadiusvsTheta");
    T->Draw("HRichHitHeader.fData.iRingNr>>RingMultPerEvt");
    T->Draw("HMatchURich.fData.fColumnPattern>>RICH_IPU_Column_Pattern");
    T->Draw("HMatchURich.fData.fRow>>RICH_IPU_Row");
    T->Draw("HMatchURich.fData.fColumn>>RICH_IPU_Col");
    T->Draw("HMatchURich.fData.fSegmentId>>RICH_IPU_Segment_Id");
    T->Draw("HMatchURich.fData.fFifoNb>>RICH_IPU_Fifo_Nb");
    T->Draw("HMatchURich.fData.fRowNb>>RICH_IPU_Row_Nb");
    T->Draw("HMatchURich.fData.fColumn:HMatchURich.fData.fRow>>RICH_IPU_Row_Col");
    T->Draw("HRichHit.fData.iRingX:HMatchURich.fData.fRow>>RICH_IPU_Row_HIT_Col");
    T->Draw("HRichHit.fData.iRingY:HMatchURich.fData.fColumn>>RICH_IPU_Col_HIT_Row");
    Int_t nRings = Ring_sector_distribution->GetEntries();
    Int_t nIPURings = RICH_IPU_Row->GetEntries();
    Int_t RingsPerSec[6];
    Int_t nRings = 0;
    TString filename(fileobj->GetName());
    // filename.Remove(0, filename.Length()-22 );
    filename.Remove(filename.Length()-5, filename.Length());
    filename.Append("_stat.txt");
    ofstream statfile(filename.Data() ,ios::out);
    statfile<<"============================================================"<<endl;
    statfile << "Total number of entries in "<<fileobj->GetName()<<" : " << nEntries << endl;
    for (Int_t i=0;i<6;i++){
	Int_t bin = (Ring_sector_distribution->GetXaxis())->FindBin(i);
	RingsPerSec[i] = Ring_sector_distribution->GetBinContent(bin);
	statfile<<"Number of rings in sector "<<i<<" : "<<RingsPerSec[i]<<" ( "<<RingsPerSec[i]/nEntries<<" per evt)"<<endl;
	
	nRings += RingsPerSec[i];
    }
    statfile<<"Total number of rings :  "<<nRings<<" ( "<<nRings/nEntries<<" per evt)"<<endl;
    statfile<<"Total number of IPU rings :  "<<nIPURings<<" ( "<<nIPURings/nEntries<<" per evt)"<<endl;
    Int_t RingMult[6];
    for (Int_t i=1;i<6;i++){
	Int_t bin = (RingMultPerEvt->GetXaxis())->FindBin(i);
	RingMult[i] = RingMultPerEvt->GetBinContent(bin);
	if (RingMult[i]!=0){
	    statfile<<i<<" rings detected in "<<RingMult[i]<<" events"<<" ( "<<RingMult[i]/nEntries<<" per evt)"<<endl;
	}
    }
    statfile<<"Mean theta:              "<<Ring_theta->GetMean()<<" ("<<Ring_theta->GetRMS()<<")"<<endl;
    statfile<<"Mean radius:             "<<Ring_radius->GetMean()<<" ("<<Ring_radius->GetRMS()<<")"<<endl;
    statfile<<"Mean ring qual HT:       "<<RingQual_HouTra->GetMean()<<" ("<<RingQual_HouTra->GetRMS()<<")"<<endl;
    statfile<<"Mean ring qual PM:       "<<RingQual_PatMat->GetMean()<<" ("<<RingQual_PatMat->GetRMS()<<")"<<endl;
    statfile<<"Mean charge in ring:     "<<RingChargeAmpl->GetMean()<<" ("<<RingChargeAmpl->GetRMS()<<")"<<endl;
    statfile<<"Mean nr of pads:         "<<RingPadNr->GetMean()<<" ("<<RingPadNr->GetRMS()<<")"<<endl;
    statfile<<"Mean loc max of 4:       "<<RingLocMax4->GetMean()<<" ("<<RingLocMax4->GetRMS()<<")"<<endl;
    statfile.close();
    outfile->cd();
    hitsector->Write();
    hittheta->Write();
    hitphi->Write();
    hitradius->Write();
    hithoutra->Write();
    hitpatmat->Write();
    hitringampl->Write();
    hitringpadnr->Write();
    hitringx->Write();
    hitringy->Write();
    hitringlocMax4->Write();
    hitringxy->Write();
    hithoutrapatmat->Write();
    hitthetaphi1->Write();
    hitthetaphi2->Write();
    hitthetaphi3->Write();
    hitthetaphi4->Write();
    hitthetaphi5->Write();
    hitthetaphi6->Write();
    hitpatmattheta->Write();
    hithoutratheta->Write();
    ringampltheta->Write();
    radiustheta->Write();
    ringmult->Write();
    ipucolpat->Write();
    ipurow   ->Write();
    ipucol   ->Write();
    ipusegid ->Write();
    ipufifonb->Write();
    ipurownb ->Write();
    ipurowcol->Write();
    ipurowhitcol ->Write();
    ipucolhitrow ->Write();
    //outfile->Print();
    outfile->Close();
    fileobj->Close();
}

void showRichStatistics(char *cFileIn, char *cFileOut=0){
    //give a diagnostic histogram ROOT file as input
    //if you don't supply an output filename, the input file and path is used
    // to generate the resulting ASCII text file
    // this function is meant to be used on a ROOT file that resulted from 
    // the addHistos("") function is addH.C
    TFile fileobj(cFileIn);
    Int_t nRings = Ring_sector_distribution->GetEntries();
    Int_t nIPURings = RICH_IPU_Row->GetEntries();
    Int_t RingsPerSec[6];
    Int_t nRings = 0;
    TString *filename = 0;
    if (!cFileOut){
	filename = new TString(fileobj->GetName());
        filename.Remove(filename.Length()-5, filename.Length());
	filename.Append("_stat.txt");
    }
    if (!filename) filename = new TString(cFileOut);
	ofstream statfile(filename.Data() ,ios::out);
	
	for (Int_t i=0;i<6;i++){
	    Int_t bin = (Ring_sector_distribution->GetXaxis())->FindBin(i);
	    RingsPerSec[i] = Ring_sector_distribution->GetBinContent(bin);
	    statfile<<"Number of rings in sector "<<i<<" : "<<RingsPerSec[i]<<endl;
	    
	    nRings += RingsPerSec[i];
	}
	statfile<<"Total number of rings :  "<<nRings<<endl;
	statfile<<"Total number of IPU rings :  "<<nIPURings<<endl;
	Int_t RingMult[6];
	for (Int_t i=1;i<6;i++){
	    Int_t bin = (RingMultPerEvt->GetXaxis())->FindBin(i);
	    RingMult[i] = RingMultPerEvt->GetBinContent(bin);
	    if (RingMult[i]!=0){
		statfile<<i<<" rings detected in "<<RingMult[i]<<" events"<<endl;
	    }
	}
	statfile<<"Mean theta:              "<<Ring_theta->GetMean()<<" ("<<Ring_theta->GetRMS()<<")"<<endl;
	statfile<<"Mean radius:             "<<Ring_radius->GetMean()<<" ("<<Ring_radius->GetRMS()<<")"<<endl;
	statfile<<"Mean ring qual HT:       "<<RingQual_HouTra->GetMean()<<" ("<<RingQual_HouTra->GetRMS()<<")"<<endl;
	statfile<<"Mean ring qual PM:       "<<RingQual_PatMat->GetMean()<<" ("<<RingQual_PatMat->GetRMS()<<")"<<endl;
	statfile<<"Mean charge in ring:     "<<RingChargeAmpl->GetMean()<<" ("<<RingChargeAmpl->GetRMS()<<")"<<endl;
	statfile<<"Mean nr of pads:         "<<RingPadNr->GetMean()<<" ("<<RingPadNr->GetRMS()<<")"<<endl;
	statfile<<"Mean loc max of 4:       "<<RingLocMax4->GetMean()<<" ("<<RingLocMax4->GetRMS()<<")"<<endl;
	statfile.close();
}

