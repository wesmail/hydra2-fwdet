// File: funcChsumMult.C
//
// Author: Thomas Eberl <Thomas.Eberl@physik.tu-muenchen.de>
// Last update by Thomas Eberl: 01/04/05 14:44:25
//
// create diagnosis for the RICH detector
// fill two histos per sector: 1) cal charge sum versus pad multiplicity event wise
//                             2) same as 1) but with a cut on rings
void chsumVSMult(char *cFileOut){

TIter next(gROOT->GetListOfFiles());
 gROOT->LoadMacro("/home/teberl/development/hades/lib/macros/funcTree.C");
 TFile *pFileIn=0;  
 TTree *tree= 0;
 // iterate over all files currently associated with ROOT session 

    TObjArray *pHistosNoCut = new TObjArray(6);
    TObjArray *pHistosRichCut = new TObjArray(6);

    pHistosNoCut->Add(new TH2F("charge_sum_VS_pad_mult_sec1","charge_sum_VS_pad_mult_sec1",1000,0,10000,500,0,500));
    pHistosNoCut->Add(new TH2F("charge_sum_VS_pad_mult_sec2","charge_sum_VS_pad_mult_sec2",1000,0,10000,500,0,500));
    pHistosNoCut->Add(new TH2F("charge_sum_VS_pad_mult_sec3","charge_sum_VS_pad_mult_sec3",1000,0,10000,500,0,500));
    pHistosNoCut->Add(new TH2F("charge_sum_VS_pad_mult_sec4","charge_sum_VS_pad_mult_sec4",1000,0,10000,500,0,500));
    pHistosNoCut->Add(new TH2F("charge_sum_VS_pad_mult_sec5","charge_sum_VS_pad_mult_sec5",1000,0,10000,500,0,500));
    pHistosNoCut->Add(new TH2F("charge_sum_VS_pad_mult_sec6","charge_sum_VS_pad_mult_sec6",1000,0,10000,500,0,500));

    pHistosRichCut->Add(new TH2F("charge_sum_VS_pad_mult_sec1_R","charge_sum_VS_pad_mult_sec1_R",1000,0,10000,500,0,500));
    pHistosRichCut->Add(new TH2F("charge_sum_VS_pad_mult_sec2_R","charge_sum_VS_pad_mult_sec2_R",1000,0,10000,500,0,500));
    pHistosRichCut->Add(new TH2F("charge_sum_VS_pad_mult_sec3_R","charge_sum_VS_pad_mult_sec3_R",1000,0,10000,500,0,500));
    pHistosRichCut->Add(new TH2F("charge_sum_VS_pad_mult_sec4_R","charge_sum_VS_pad_mult_sec4_R",1000,0,10000,500,0,500));
    pHistosRichCut->Add(new TH2F("charge_sum_VS_pad_mult_sec5_R","charge_sum_VS_pad_mult_sec5_R",1000,0,10000,500,0,500));
    pHistosRichCut->Add(new TH2F("charge_sum_VS_pad_mult_sec6_R","charge_sum_VS_pad_mult_sec6_R",1000,0,10000,500,0,500));

    while ( pFileIn = (TFile*) next() ){
	TString sFileIn( pFileIn->GetName() );
	cout<<"Now processing file: "<<sFileIn<<endl;
	HTree *T = (HTree *) pFileIn->Get("T"); // libHydra.so must be loaded in CINT
	Int_t nMinEntry = 0;
	Stat_t nMaxEntry = T->GetEntries();
	cout << "Total number of entries in File : " << nMaxEntry << endl;
	//nMaxEntry = 10000;


	TObjArray pSelectedBr(10);
	// *** RICH CAL ***
	const Int_t RICHCALMAX = 50000 ; // max number of objects in a TClonesArray per event.
	// declaration of leaf variables in the split tree
	Short_t iRichCalSector[RICHCALMAX];
	Float_t fRichCalCharge[RICHCALMAX];
	Int_t   nRichCalData; //number of data object in tclones.

	TObjArray *pRichCalBr = createRichCalBranches(T);
	TBranch *pBrTmp=0;
	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCal.fData_"));
	pBrTmp -> SetAddress(&nRichCalData);
	pSelectedBr.Add(pBrTmp);

	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCal.fData.fCharge"));
	pBrTmp -> SetAddress(fRichCalCharge);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCal.fData.fSector"));
	pBrTmp -> SetAddress(iRichCalSector);
	pSelectedBr.Add(pBrTmp);

	// *** RICH HIT ***
	
	const Int_t RICHHITMAX = 20;
	Int_t   iRichHitSector[RICHHITMAX];
	Int_t   nRichHitData;
	
	TObjArray *pRichHitBr = createRichHitBranches(T);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData_"));
	pBrTmp -> SetAddress(&nRichHitData);
	pSelectedBr.Add(pBrTmp);

	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.nSector"));
	pBrTmp -> SetAddress(iRichHitSector);
	pSelectedBr.Add(pBrTmp);


	for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	    
	    if (i%5000 == 0 && i != 0) {
		cout<<i<<" events processed"<<endl;
	    }

	    for (Int_t ii=0;ii<((pSelectedBr->GetLast())+1);ii++){
		((TBranch*)pSelectedBr[ii]).GetEntry(i); 
	    }
	    Int_t calCntr[6] = {0,0,0,0,0,0};
	    Float_t calChrgSum[6] = {0.,0.,0.,0.,0.,0.};
	    for (Int_t cal=0;cal<nRichCalData;cal++){
		calCntr[iRichCalSector[cal]]++;
		calChrgSum[iRichCalSector[cal]]+= fRichCalCharge[cal];
	    } // FIXME: use this for the case with a ring !!
	    for (Int_t cal1=0;cal1<6;cal1++){
		((TH2F*)((*pHistosNoCut)[cal1])) 
			-> Fill(calChrgSum[cal1],calCntr[cal1]);
	    }
	    
	    if (nRichHitData > 0){ // new evt with a found ring
		Int_t hitsector = -1;
		for (Int_t k=0;k<nRichHitData;k++){
		    //	    cout<<"evt: "<<i<<" hit no: "<<k<<" sec: "<<iRichHitSector[k]<<endl;
		    if (iRichHitSector[k] != hitsector){
			hitsector = iRichHitSector[k];
			Int_t calCntr1 = 0;
			Float_t calChrgSum1 = 0.;
			for (Int_t m=0;m<nRichCalData;m++){
			    if (iRichCalSector[m] == iRichHitSector[k]){
				calCntr1++;
				calChrgSum1 += fRichCalCharge[m];
			    } // endif sum over sector with ring
			} // end cal data loop
			//cout<<"Charge: "<<calChrgSum<<" Pad Multiplicity: "
			//			<<calCntr<<endl;
			//    cout<<((TH2F*)((*pHistosRichCut)[iRichHitSector[k]]))
			//->GetName()<<" filled with sec "<<iRichHitSector[k]<<" data"<<endl;;
			((TH2F*)((*pHistosRichCut)[iRichHitSector[k]])) 
			    -> Fill(calChrgSum1,calCntr1);  
		    }
		} // end loop over rings in evt
		
	    } // endif evt with ring
	}
	
    }

    // visualize and write out to file
    gStyle->SetPalette(1);
    TCanvas *canRichCut = new TCanvas("Pad_Multiplicity_vs_ChargeSum_Rings",
				      "Pad_Multiplicity_vs_ChargeSum_Rings",0,0,900,600);
    TCanvas *canNoCut   = new TCanvas("Pad_Multiplicity_vs_ChargeSum",
				      "Pad_Multiplicity_vs_ChargeSum",0,0,900,600);
    canRichCut -> Divide(3,2);
    canRichCut -> Draw();
    canNoCut   -> Divide(3,2);
    canNoCut   -> Draw();
    TFile *pFileOut = new TFile(cFileOut,"RECREATE");
    for (Int_t j1=0;j1<6;j1++){
	canRichCut->cd(j1+1);
	((TH2F*)((*pHistosRichCut)[j1])) -> Draw("colz");
	((TH2F*)((*pHistosRichCut)[j1])) -> Write();
    }
    for (Int_t j2=0;j2<6;j2++){
	canNoCut->cd(j2+1);
	((TH2F*)((*pHistosNoCut)[j2])) -> Draw("colz");
	((TH2F*)((*pHistosNoCut)[j2])) -> Write();
    }
    pFileOut->Close();
}
