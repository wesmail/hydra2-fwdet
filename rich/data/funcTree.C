// File: funcTree.C
//
// Author: Thomas Eberl <Thomas.Eberl@physik.tu-muenchen.de>
// Last update by Thomas Eberl: 01/06/22 17:07:25
//
// collection of functions to be used within CINT
// load : CINT>.L funcIO.C
// purpose: provide templates for Hydra Tree reading 

void mergeRingExpTrees(char* cFileOut){
    // merge ROOT files with ROOT tree data
    // assumes that you call ROOT like this: root <filename> <filename>
    // this makes these files available in your session via the global gROOT

    // function serves as example for similar data trees
    TIter next(gROOT->GetListOfFiles());
    TFile *fileobj=0;
    gROOT->LoadMacro("~/development/hades/lib/macros/dataDefs.C"); // contains declaration of new data type RECOGNIZED_RING
    
    RECOGNIZED_RING ring;
    
    TChain chain("RingParams");
    TBranch *bRing;
    bRing = chain.GetBranch("ring");
    
    while ( fileobj = (TFile*) next() ){
	chain.Add(fileobj->GetName());
    }
    
    chain.Merge(cFileOut);
    
}

void mergeRingSimTrees(char* cFileOut){
    // merge ROOT files with ROOT tree data
    // assumes that you call ROOT like this: root <filename> <filename>
    // this makes these files available in your session via the global gROOT

    // function serves as example for similar data trees
    TIter next(gROOT->GetListOfFiles());
    TFile *fileobj=0;
    gROOT->LoadMacro("dataDefs.C"); // contains declaration of new data type RECOGNIZED_RING
    
    SIMUL_RING ring;
    
    TChain chain("RingParams");
    TBranch *bRing;
    bRing = chain.GetBranch("ring");
    
    while ( fileobj = (TFile*) next() ){
	chain.Add(fileobj->GetName());
    }
    
    chain.Merge(cFileOut);
    
}

TObjArray* createRichCalBranches(HTree *T){
    TObjArray *branches = new TObjArray(6);
    branches->Add(T->GetBranch("HRichCal.fData_"));
    branches->Add(T->GetBranch("HRichCal.fData.fCharge"));
    branches->Add(T->GetBranch("HRichCal.fData.fSector"));
    branches->Add(T->GetBranch("HRichCal.fData.fRow"));
    branches->Add(T->GetBranch("HRichCal.fData.fCol"));
    branches->Add(T->GetBranch("HRichCal.fData.fEventNr"));
    return branches;
}

TObjArray* createRichCalSimBranches(HTree *T){
    TObjArray *branches = new TObjArray(6);
    branches->Add(T->GetBranch("HRichCalSim.fData_"));
    branches->Add(T->GetBranch("HRichCalSim.fData.fCharge"));
    branches->Add(T->GetBranch("HRichCalSim.fData.fSector"));
    branches->Add(T->GetBranch("HRichCalSim.fData.fRow"));
    branches->Add(T->GetBranch("HRichCalSim.fData.fCol"));
    branches->Add(T->GetBranch("HRichCalSim.fData.fEventNr"));
    return branches;
}

TObjArray* createRichHitBranches(HTree *T){
    TObjArray *branches = new TObjArray(10);
    branches->Add(T->GetBranch("HRichHit.fData_"));
    branches->Add(T->GetBranch("HRichHit.fData.fTheta"));
    branches->Add(T->GetBranch("HRichHit.fData.fPhi"));
    branches->Add(T->GetBranch("HRichHit.fData.iRingX"));
    branches->Add(T->GetBranch("HRichHit.fData.iRingY"));
    branches->Add(T->GetBranch("HRichHit.fData.nSector"));
    branches->Add(T->GetBranch("HRichHit.fData.iRingPatMat"));
    branches->Add(T->GetBranch("HRichHit.fData.iRingHouTra"));
    branches->Add(T->GetBranch("HRichHit.fData.iRingAmplitude"));
    branches->Add(T->GetBranch("HRichHit.fData.iRingLocalMax4"));
    return branches;
}

TObjArray* createRichHitSimBranches(HTree *T){
    TObjArray *branches = new TObjArray(10);
    branches->Add(T->GetBranch("HRichHitSim.fData_"));
    branches->Add(T->GetBranch("HRichHitSim.fData.fTheta"));
    branches->Add(T->GetBranch("HRichHitSim.fData.fPhi"));
    branches->Add(T->GetBranch("HRichHitSim.fData.iRingX"));
    branches->Add(T->GetBranch("HRichHitSim.fData.iRingY"));
    branches->Add(T->GetBranch("HRichHitSim.fData.nSector"));
    branches->Add(T->GetBranch("HRichHitSim.fData.iRingPatMat"));
    branches->Add(T->GetBranch("HRichHitSim.fData.iRingHouTra"));
    branches->Add(T->GetBranch("HRichHitSim.fData.iRingAmplitude"));
    branches->Add(T->GetBranch("HRichHitSim.fData.iRingLocalMax4"));
    return branches;
}

TObjArray* createRichHitHeaderBranches(HTree *T){
    TObjArray *branches = new TObjArray(2);
    branches->Add(T->GetBranch("HRichHitHeader.fData_"));
    branches->Add(T->GetBranch("HRichHitHeader.fData.iRingNr"));
    return branches;
}

TObjArray* createTofHitBranches(HTree *T){
    TObjArray *branches = new TObjArray(8);
    branches->Add(T->GetBranch("HTofHit.fData_"));
    branches->Add(T->GetBranch("HTofHit.fData.tof"));
    branches->Add(T->GetBranch("HTofHit.fData.xpos"));
    branches->Add(T->GetBranch("HTofHit.fData.cell"));
    branches->Add(T->GetBranch("HTofHit.fData.module"));
    branches->Add(T->GetBranch("HTofHit.fData.sector"));
    branches->Add(T->GetBranch("HTofHit.fData.theta"));
    branches->Add(T->GetBranch("HTofHit.fData.phi"));
    return branches;
}

TObjArray* createHGeantRichPhotonBranches(HTree *T){
    TObjArray *branches = new TObjArray(6);
    branches->Add(T->GetBranch("HGeantRichPhoton.fData_"));
    branches->Add(T->GetBranch("HGeantRichPhoton.fData.eHit"));
    branches->Add(T->GetBranch("HGeantRichPhoton.fData.sector"));
    branches->Add(T->GetBranch("HGeantRichPhoton.fData.xHit"));
    branches->Add(T->GetBranch("HGeantRichPhoton.fData.yHit"));
    branches->Add(T->GetBranch("HGeantRichPhoton.fData.parentTrack"));
    return branches;
}

void extractRingsExp(){
    // this function reads in a HYDRA Tree, extracts all rings and saves them in a 
    // ROOT tree of basic types: "DST"-File
    // advantage: speed of access to rings in further analysis
    //
    // start ROOT: $ root <filename.root>
    //             CINT> .L funcIO.C
    //             CINT> extractRingsExp();
    // then you can merge the files, cf.: mergeTrees()
    // libHydra.so ... must be loaded in CINT, because of use of HTree ...
    gROOT->LoadMacro("dataDefs.C");
    TIter next(gROOT->GetListOfFiles());
    TFile *pFileIn=0;  
    TTree *tree= 0;
    // iterate over all files currently associated with ROOT session 
    while ( pFileIn = (TFile*) next() ){
	TString sFileIn( pFileIn->GetName() );
	cout<<"Now processing file: "<<sFileIn<<endl;
	Ssiz_t ln = sFileIn.Length();
	if (sFileIn.Contains(".root") && !sFileIn.Contains("DST")){
	    sFileIn.Remove(ln-5,ln);
	    sFileIn.Append("_DST.root");
	}
	TFile FileOut(sFileIn,"RECREATE","Extracted rings");

	HTree *T = (HTree *) pFileIn->Get("T"); // libHydra.so must be loaded in CINT
	Int_t nMinEntry = 0;
	Stat_t nMaxEntry = T->GetEntries();
	cout << "Total number of entries in File : " << nMaxEntry << endl;
	TObjArray pSelectedBr(10);
	// *** RICH CAL ***
	const Int_t RICHCALMAX = 50000 ; // max number of objects in a TClonesArray per event.
	// declaration of leaf variables in the split tree
	Short_t iRichCalSector[RICHCALMAX];
	Char_t  iRichCalRow[RICHCALMAX],    iRichCalCol[RICHCALMAX];
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
	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCal.fData.fRow"));
	pBrTmp -> SetAddress(iRichCalRow);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCal.fData.fCol"));
	pBrTmp -> SetAddress(iRichCalCol);
	pSelectedBr.Add(pBrTmp);

	// *** RICH HIT ***
	
	const Int_t RICHHITMAX = 20;
	Float_t fRichHitTheta[RICHHITMAX],   fRichHitPhi[RICHHITMAX];
	Int_t   iRichHitPatMat[RICHHITMAX],  iRichHitHouTra[RICHHITMAX];
	Int_t   iRichHitSector[RICHHITMAX];
	Int_t   iRichHitRingX[RICHHITMAX],   iRichHitRingY[RICHHITMAX];
	Short_t iRichHitRingAmp[RICHHITMAX], iRichHitLocMax4[RICHHITMAX];
	Int_t   nRichHitData;
	
	TObjArray *pRichHitBr = createRichHitBranches(T);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData_"));
	pBrTmp -> SetAddress(&nRichHitData);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.fTheta"));
	pBrTmp -> SetAddress(fRichHitTheta);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.fPhi"));
	pBrTmp -> SetAddress(fRichHitPhi);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingX"));
	pBrTmp -> SetAddress(iRichHitRingX);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingY"));
	pBrTmp -> SetAddress(iRichHitRingY);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.nSector"));
	pBrTmp -> SetAddress(iRichHitSector);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingPatMat"));
	pBrTmp -> SetAddress(iRichHitPatMat);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingHouTra"));
	pBrTmp -> SetAddress(iRichHitHouTra);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingAmplitude"));
	pBrTmp -> SetAddress(iRichHitRingAmp);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingLocalMax4"));
	pBrTmp -> SetAddress(iRichHitLocMax4);
	pSelectedBr.Add(pBrTmp);
	

	// *** RICH HIT HEADER ***
	
	const Int_t RICHHITHEADERMAX = 10;
	
	Int_t iRichHitHeaderRingNr[RICHHITHEADERMAX];
	Int_t nRichHitHeaderData;    
	TObjArray *pRichHitHeaderBr = createRichHitHeaderBranches(T);
	pBrTmp = (TBranch*)(pRichHitHeaderBr->FindObject("HRichHitHeader.fData_"));
	pBrTmp -> SetAddress(&nRichHitHeaderData);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitHeaderBr->FindObject("HRichHitHeader.fData.iRingNr"));
	pBrTmp -> SetAddress(iRichHitHeaderRingNr);
	pSelectedBr.Add(pBrTmp);
	
	// Create output ROOT Tree
	pTreeOut = new TTree("RingParams","Tree with parameters of extracted rings");
	RECOGNIZED_RING ring; // new datatype to be serialized into a branch 
	// note that the address of the pointer to the object for the branch is needed
	TString sBranchDefinition(256);
	char *var1 = new char[15];
	sprintf(var1,"xcoord[%d]/I:",fieldsize*fieldsize);
	char *var2 = new char[15];
	sprintf(var2,"ycoord[%d]/I:",fieldsize*fieldsize);
	char *var3 = new char[15];
	sprintf(var3,"charge[%d]/F:",fieldsize*fieldsize);
	sBranchDefinition.Append("RingX/I:");
	sBranchDefinition.Append("RingY/I:");
	sBranchDefinition.Append("nPads/I:");
	sBranchDefinition.Append("Phi/F:");
	sBranchDefinition.Append("Theta/F:");
	sBranchDefinition.Append(var1);
	sBranchDefinition.Append(var2);
	sBranchDefinition.Append(var3);
	sBranchDefinition.Append("Ampli/I:");
	sBranchDefinition.Append("LocMax4/I:");
	sBranchDefinition.Append("MatQual/I:");
	sBranchDefinition.Append("HoughQual/I");

	pTreeOut -> Branch("recognized_ring",&ring,sBranchDefinition.Data());
	
	for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	    
	    if (i%1000 == 0 && i != 0) {
		cout<<i<<" events processed and "<<
		    pTreeOut->GetEntries()<<" rings extracted"<<endl;
	    }
	    for (Int_t ii=0;ii<((pSelectedBr->GetLast())+1);ii++){
		((TBranch*)pSelectedBr[ii]).GetEntry(i); 
	    }

	    // traverse current tree entry and make selections
	    for (Int_t j=0; j<nRichHitHeaderData; j++){
		if ( iRichHitHeaderRingNr[j] == 1 ){         // one ring per sector
		    
		   for (Int_t l=0; l<nRichHitData; l++){     // traverse data of ring
		       
		       ring.RingX     = iRichHitRingX[l];    // save ring params in struct 
		       ring.RingY     = iRichHitRingY[l];
		       ring.Phi       = fRichHitPhi[l];
		       ring.Theta     = fRichHitTheta[l];
		       ring.Ampli     = iRichHitRingAmp[l];
		       ring.LocMax4   = iRichHitLocMax4[l];
		       ring.MatQual   = iRichHitPatMat[l];
		       ring.HoughQual = iRichHitHouTra[l];
			   
		       for (int a=0;a<fieldsize*fieldsize;a++){// reset struct arrays
			   ring.xcoord[a] = 0;
			   ring.ycoord[a] = 0;
			   ring.charge[a] = 0;
		       }
		       
		       Int_t n=0; // counter for non-zero cal values
		       for (Int_t m=0; m<nRichCalData; m++){ // loop over ring data
			   if ( iRichHitSector[l] == iRichCalSector[m]){
			       
			       if (TMath::Abs(iRichCalRow[m] - iRichHitRingY[l]) <= 6 &&
				   TMath::Abs(iRichCalCol[m] - iRichHitRingX[l]) <= 6){   
				   // cut out a field of 13x13 pads around ring center
				   ring.xcoord[n] = iRichCalCol[m];
				   ring.ycoord[n] = iRichCalRow[m];   
				   ring.charge[n] = fRichCalCharge[m];
				   n++;
			       }
			   }
		       }//end of loop over cal container for this ring
		       ring.nPads = n;
		   } // end of loop over ring data
		   
		   pTreeOut->Fill();
		}// if: only for Ring !
		
	    } // end of loop over rings in one event
	    
	    
	}//  end of for loop over events in Tree
	
	pTreeOut->Print();
	pFileIn->Close();
	FileOut->cd();
	pTreeOut->Write();
	FileOut.Close();
    }
}

void extractRingsTOFconditionExp(){
    // this function reads in a HYDRA Tree, extracts all rings and saves them in a 
    // ROOT tree of basic types: "DST"-File
    // advantage: speed of access to rings in further analysis
    //
    // start ROOT: $ root <filename.root>
    //             CINT> .L funcIO.C
    //             CINT> extractRingsExp();
    // then you can merge the files, cf.: mergeTrees()
    // libHydra.so ... must be loaded in CINT, because of use of HTree ...
    gROOT->LoadMacro("~/development/hades/lib/macros/dataDefs.C");
    TIter next(gROOT->GetListOfFiles());
    TFile *pFileIn=0;  
    TTree *tree= 0;
    // iterate over all files currently associated with ROOT session 
    while ( pFileIn = (TFile*) next() ){
	TString sFileIn( pFileIn->GetName() );
	cout<<"Now processing file: "<<sFileIn<<endl;
	Ssiz_t ln = sFileIn.Length();
	if (sFileIn.Contains(".root") && !sFileIn.Contains("RINGS")){
	    sFileIn.Remove(ln-5,ln);
	    sFileIn.Append("_ringsANDtof_DST.root");
	}
	TFile FileOut(sFileIn,"RECREATE","Extracted rings");

	HTree *T = (HTree *) pFileIn->Get("T"); // libHydra.so must be loaded in CINT
	Int_t nMinEntry = 0;
	Stat_t nMaxEntry = T->GetEntries();
	cout << "Total number of entries in File : " << nMaxEntry << endl;
	TObjArray pSelectedBr(10);
	// *** RICH CAL ***
	const Int_t RICHCALMAX = 50000 ; // max number of objects in a TClonesArray per event.
	// declaration of leaf variables in the split tree
	Short_t iRichCalSector[RICHCALMAX];
	Char_t  iRichCalRow[RICHCALMAX],    iRichCalCol[RICHCALMAX];
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
	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCal.fData.fRow"));
	pBrTmp -> SetAddress(iRichCalRow);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCal.fData.fCol"));
	pBrTmp -> SetAddress(iRichCalCol);
	pSelectedBr.Add(pBrTmp);

	// *** RICH HIT ***
	
	const Int_t RICHHITMAX = 20;
	Float_t fRichHitTheta[RICHHITMAX],   fRichHitPhi[RICHHITMAX];
	Int_t   iRichHitPatMat[RICHHITMAX],  iRichHitHouTra[RICHHITMAX];
	Int_t   iRichHitSector[RICHHITMAX];
	Int_t   iRichHitRingX[RICHHITMAX],   iRichHitRingY[RICHHITMAX];
	Short_t iRichHitRingAmp[RICHHITMAX], iRichHitLocMax4[RICHHITMAX];
	Int_t   nRichHitData;
	
	TObjArray *pRichHitBr = createRichHitBranches(T);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData_"));
	pBrTmp -> SetAddress(&nRichHitData);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.fTheta"));
	pBrTmp -> SetAddress(fRichHitTheta);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.fPhi"));
	pBrTmp -> SetAddress(fRichHitPhi);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingX"));
	pBrTmp -> SetAddress(iRichHitRingX);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingY"));
	pBrTmp -> SetAddress(iRichHitRingY);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.nSector"));
	pBrTmp -> SetAddress(iRichHitSector);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingPatMat"));
	pBrTmp -> SetAddress(iRichHitPatMat);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingHouTra"));
	pBrTmp -> SetAddress(iRichHitHouTra);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingAmplitude"));
	pBrTmp -> SetAddress(iRichHitRingAmp);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHit.fData.iRingLocalMax4"));
	pBrTmp -> SetAddress(iRichHitLocMax4);
	pSelectedBr.Add(pBrTmp);
	

	// *** RICH HIT HEADER ***
	
	const Int_t RICHHITHEADERMAX = 10;
	
	Int_t iRichHitHeaderRingNr[RICHHITHEADERMAX];
	Int_t nRichHitHeaderData;    
	TObjArray *pRichHitHeaderBr = createRichHitHeaderBranches(T);
	pBrTmp = (TBranch*)(pRichHitHeaderBr->FindObject("HRichHitHeader.fData_"));
	pBrTmp -> SetAddress(&nRichHitHeaderData);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitHeaderBr->FindObject("HRichHitHeader.fData.iRingNr"));
	pBrTmp -> SetAddress(iRichHitHeaderRingNr);
	pSelectedBr.Add(pBrTmp);

	// *** TOF HIT  ***

      const Int_t TOFHITMAX = 5000;
      
      Float_t fTofHitTof[TOFHITMAX],   fTofHitXpos[TOFHITMAX], 
	      fTofHitTheta[TOFHITMAX], fTofHitPhi[TOFHITMAX];
      Char_t  iTofHitCell[TOFHITMAX],  iTofHitModule[TOFHITMAX], 
	      iTofHitSector[TOFHITMAX];
      Int_t   nTofHitData;
      
      TObjArray *pTofHitBr = createTofHitBranches(T);

      pBrTmp = (TBranch*)(pTofHitBr->FindObject("HTofHit.fData_"));
      pBrTmp -> SetAddress(&nTofHitData);
      pSelectedBr.Add(pBrTmp);
      pBrTmp = (TBranch*)(pTofHitBr->FindObject("HTofHit.fData.tof"));
      pBrTmp -> SetAddress(fTofHitTof);
      pSelectedBr.Add(pBrTmp);
      pBrTmp = (TBranch*)(pTofHitBr->FindObject("HTofHit.fData.xpos"));
      pBrTmp -> SetAddress(fTofHitXpos);
      pSelectedBr.Add(pBrTmp);
      pBrTmp = (TBranch*)(pTofHitBr->FindObject("HTofHit.fData.cell"));
      pBrTmp -> SetAddress(iTofHitCell);
      pSelectedBr.Add(pBrTmp);
      pBrTmp = (TBranch*)(pTofHitBr->FindObject("HTofHit.fData.module"));
      pBrTmp -> SetAddress(iTofHitModule);
      pSelectedBr.Add(pBrTmp);
      pBrTmp = (TBranch*)(pTofHitBr->FindObject("HTofHit.fData.sector"));
      pBrTmp -> SetAddress(iTofHitSector);
      pSelectedBr.Add(pBrTmp);
      pBrTmp = (TBranch*)(pTofHitBr->FindObject("HTofHit.fData.theta"));
      pBrTmp -> SetAddress(fTofHitTheta);
      pSelectedBr.Add(pBrTmp);
      pBrTmp = (TBranch*)(pTofHitBr->FindObject("HTofHit.fData.phi"));
      pBrTmp -> SetAddress(fTofHitPhi);
      pSelectedBr.Add(pBrTmp);
	// Create output ROOT Tree
	pTreeOut = new TTree("RingParams","Tree with parameters of extracted rings");
	RECOGNIZED_RING ring; // new datatype to be serialized into a branch 
	// note that the address of the pointer to the object for the branch is needed
	TString sBranchDefinition(256);
	char *var1 = new char[15];
	sprintf(var1,"xcoord[%d]/I:",fieldsize*fieldsize);
	char *var2 = new char[15];
	sprintf(var2,"ycoord[%d]/I:",fieldsize*fieldsize);
	char *var3 = new char[15];
	sprintf(var3,"charge[%d]/F:",fieldsize*fieldsize);
	sBranchDefinition.Append("RingX/I:");
	sBranchDefinition.Append("RingY/I:");
	sBranchDefinition.Append("nPads/I:");
	sBranchDefinition.Append("Phi/F:");
	sBranchDefinition.Append("Theta/F:");
	sBranchDefinition.Append(var1);
	sBranchDefinition.Append(var2);
	sBranchDefinition.Append(var3);
	sBranchDefinition.Append("Ampli/I:");
	sBranchDefinition.Append("LocMax4/I:");
	sBranchDefinition.Append("MatQual/I:");
	sBranchDefinition.Append("HoughQual/I");
	sBranchDefinition.Append("DTheta/F:");
	sBranchDefinition.Append("DPhi/F");

	pTreeOut -> Branch("recognized_ring",&ring,sBranchDefinition.Data());
	
	for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	    
	    if (i%1000 == 0 && i != 0) {
		cout<<i<<" events processed and "<<
		    pTreeOut->GetEntries()<<" rings extracted"<<endl;
	    }
	    for (Int_t ii=0;ii<((pSelectedBr->GetLast())+1);ii++){
		((TBranch*)pSelectedBr[ii]).GetEntry(i); 
	    }

	    // traverse current tree entry and make selections

	    // we want to look for correlation between all Single RIngs and Tof Hits.
	    for (Int_t j=0; j<nRichHitHeaderData; j++){
		if ( iRichHitHeaderRingNr[j] == 1 ){ // one ring per sector

		    for(Int_t k=0; k<nTofHitData; k++){  
			if( fTofHitTof[k] > 0. && fTofHitTof[k] < 8. ){
			    
			    for (Int_t l=0; l<nRichHitData; l++){ // traverse data of ring
				if (TMath::Abs(fRichHitPhi[l]-fTofHitPhi[k]) <=20.){
				    if (iRichHitSector[l]==iTofHitSector[k] ){
					
					ring.DTheta    = fRichHitTheta[l] - fTofHitTheta[k];
					ring.DPhi      = fRichHitPhi[l]   - fTofHitPhi[k]; 
					ring.RingX     = iRichHitRingX[l];  
					ring.RingY     = iRichHitRingY[l];
					ring.Phi       = fRichHitPhi[l];
					ring.Theta     = fRichHitTheta[l];
					ring.Ampli     = iRichHitRingAmp[l];
					ring.LocMax4   = iRichHitLocMax4[l];
					ring.MatQual   = iRichHitPatMat[l];
					ring.HoughQual = iRichHitHouTra[l];
					
					for (int a=0;a<169;a++){ // reset struct arrays
					    ring.xcoord[a] = 0;
					    ring.ycoord[a] = 0;
					    ring.charge[a] = 0;
					}
					
					Int_t n=0; // counter for non-zero cal values
					for (Int_t m=0; m<nRichCalData; m++){ // loop over ring data
					    if ( iRichHitSector[l] == iRichCalSector[m]){
						
						if (TMath::Abs(iRichCalRow[m] - iRichHitRingY[l]) <= 6 &&
						    TMath::Abs(iRichCalCol[m] - iRichHitRingX[l]) <= 6){   
						    ring.xcoord[n] = iRichCalCol[m];
						    ring.ycoord[n] = iRichCalRow[m];   
						    ring.charge[n] = fRichCalCharge[m];
						    n++;
						}
					    }
					}//end of loop over cal container for this ring
					
					ring.nPads = n;
					pTreeOut->Fill();
				    }//conditions on rich-tof sectors
				} // end of delta phi condition
			    } // end of loop over ring data
			}// condition on time of flight
		    }// for loop on tof hits !
		} // end of loop over rings in one event
	    }//loop on richheader container
	}//  end of for loop over events in Tree
	
	pTreeOut->Print();
	pFileIn->Close();
	FileOut->cd();
	pTreeOut->Write();
	FileOut.Close();
    }
}

void extractRingsSim(char *cFileOut){
    // this function reads in a HYDRA Tree, extracts all rings and saves them in a 
    // ROOT tree of basic types: "DST"-File
    // advantage: speed of access to rings in further analysis
    //
    // start ROOT: $ root <filename.root>
    //             CINT> .L funcIO.C
    //             CINT> extractRingsSim();
    // then you can merge the files, cf.: mergeTrees()
    // libHydra.so ... must be loaded in CINT, because of use of HTree ...


// ***************** HYDRA INI ******************
    Hades myHades;
    HRuntimeDb* rtdb=gHades->getRuntimeDb();
    HRichDetector rich; 
    Int_t mods[1]={1}; // in RICH there is only 1 module 
    rich.setModules(0,mods);   // 0 sector
    rich.setModules(1,mods);   // 1 sector
    rich.setModules(2,mods);   // 2 sector
    rich.setModules(3,mods);   // 3 sector
    rich.setModules(4,mods);   // 4 sector
    rich.setModules(5,mods);   // 5 sector
    gHades->getSetup()->addDetector(&rich);
    
    rtdb->addRun(944433336);
    
    HParRootFileIo *input=new HParRootFileIo;
    input->open("/home/teberl/hades/hydra/v530muc/macros/All/params_day324.root","READ");
    rtdb->setFirstInput(input);
    HRichGeometryPar geopar;
    rtdb->addContainer(&geopar);
    rtdb->initContainers(944433336);
    HRichGeometryPar *fGeometryPar = 
	(HRichGeometryPar*)rtdb->getContainer("RichGeometryParameters");
    HRichPadTab *pPadsPar = ((HRichGeometryPar*)fGeometryPar)->getPadsPar();
    HRichPad *pPad = NULL;
    // ***************** END of HYDRA INI *********************
    
    gROOT->LoadMacro("dataDefs.C");
    TIter next(gROOT->GetListOfFiles());
    TFile *pFileIn=0;  
    TTree *tree= 0;
    Int_t nFileCnt = 0;
    // iterate over all files currently associated with ROOT session 

    while ( pFileIn = (TFile*) next() ){
	TString sFileIn( pFileIn->GetName() );
	cout<<"Now processing file: "<<sFileIn<<endl;
	TString sFileOut(cFileOut);
	
	//	if (sFileOut.Contains(".root")) sFileOut.Remove( sFileOut.Length()-5,sFileOut.Length() );
	char *cNo = new char[2];
	sprintf(cNo,"%d",nFileCnt);
	sFileOut.Append("_DST_");
	sFileOut.Append(cNo);
	sFileOut.Append(".root");
	nFileCnt++;
	TFile FileOut(sFileOut.Data(),"RECREATE","Extracted simul rings");
	
	HTree *T = (HTree *) pFileIn->Get("T"); // libHydra.so must be loaded in CINT
	Int_t nMinEntry = 0;
	Stat_t nMaxEntry = T->GetEntries();
	cout << "Total number of entries in File : " << nMaxEntry << endl;
	//nMaxEntry = 500;
	TObjArray pSelectedBr(10);
	
	// *** RICH CAL ***

	const Int_t RICHCALMAX = 50000 ; // max number of objects in a TClonesArray per event.
	// declaration of leaf variables in the split tree
	Short_t iRichCalSector[RICHCALMAX];
	Char_t  iRichCalRow[RICHCALMAX],    iRichCalCol[RICHCALMAX];
	Float_t fRichCalCharge[RICHCALMAX];
	Int_t   nRichCalData; //number of data object in tclones.

	TObjArray *pRichCalBr = createRichCalSimBranches(T);
	TBranch *pBrTmp=0;
	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCalSim.fData_"));
	pBrTmp -> SetAddress(&nRichCalData);
	pSelectedBr.Add(pBrTmp);

	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCalSim.fData.fCharge"));
	pBrTmp -> SetAddress(fRichCalCharge);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCalSim.fData.fSector"));
	pBrTmp -> SetAddress(iRichCalSector);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCalSim.fData.fRow"));
	pBrTmp -> SetAddress(iRichCalRow);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichCalBr->FindObject("HRichCalSim.fData.fCol"));
	pBrTmp -> SetAddress(iRichCalCol);
	pSelectedBr.Add(pBrTmp);

	// *** RICH HIT ***
	
	const Int_t RICHHITMAX = 20;
	Float_t fRichHitTheta[RICHHITMAX],   fRichHitPhi[RICHHITMAX];
	Int_t   iRichHitPatMat[RICHHITMAX],  iRichHitHouTra[RICHHITMAX];
	Int_t   iRichHitSector[RICHHITMAX];
	Int_t   iRichHitRingX[RICHHITMAX],   iRichHitRingY[RICHHITMAX];
	Short_t iRichHitRingAmp[RICHHITMAX], iRichHitLocMax4[RICHHITMAX];
	Int_t   nRichHitData;
	
	TObjArray *pRichHitBr = createRichHitSimBranches(T);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHitSim.fData_"));
	pBrTmp -> SetAddress(&nRichHitData);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHitSim.fData.fTheta"));
	pBrTmp -> SetAddress(fRichHitTheta);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHitSim.fData.fPhi"));
	pBrTmp -> SetAddress(fRichHitPhi);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHitSim.fData.iRingX"));
	pBrTmp -> SetAddress(iRichHitRingX);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHitSim.fData.iRingY"));
	pBrTmp -> SetAddress(iRichHitRingY);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHitSim.fData.nSector"));
	pBrTmp -> SetAddress(iRichHitSector);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHitSim.fData.iRingPatMat"));
	pBrTmp -> SetAddress(iRichHitPatMat);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHitSim.fData.iRingHouTra"));
	pBrTmp -> SetAddress(iRichHitHouTra);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHitSim.fData.iRingAmplitude"));
	pBrTmp -> SetAddress(iRichHitRingAmp);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitBr->FindObject("HRichHitSim.fData.iRingLocalMax4"));
	pBrTmp -> SetAddress(iRichHitLocMax4);
	pSelectedBr.Add(pBrTmp);
	

	// *** RICH HIT HEADER ***
	
	const Int_t RICHHITHEADERMAX = 10;
	
	Int_t iRichHitHeaderRingNr[RICHHITHEADERMAX];
	Int_t nRichHitHeaderData;    
	TObjArray *pRichHitHeaderBr = createRichHitHeaderBranches(T);
	pBrTmp = (TBranch*)(pRichHitHeaderBr->FindObject("HRichHitHeader.fData_"));
	pBrTmp -> SetAddress(&nRichHitHeaderData);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pRichHitHeaderBr->FindObject("HRichHitHeader.fData.iRingNr"));
	pBrTmp -> SetAddress(iRichHitHeaderRingNr);
	pSelectedBr.Add(pBrTmp);
	

	// *** RICH PHOTONS ***

	const Int_t RICHPHOTONSMAX = 10000;

	Char_t cRichPhotonSector[RICHPHOTONSMAX];
	Float_t fRichPhotonXhit[RICHPHOTONSMAX], fRichPhotonYhit[RICHPHOTONSMAX];
	Float_t fRichPhotonEnergy[RICHPHOTONSMAX];
	Int_t iRichPhotonParentTrack[RICHPHOTONSMAX];
	Int_t nRichPhotonData; //number of data object in tclones.

	TObjArray *pHGeantRichPhotonBr = createHGeantRichPhotonBranches(T);

	pBrTmp = (TBranch*)(pHGeantRichPhotonBr->FindObject("HGeantRichPhoton.fData_"));
	pBrTmp -> SetAddress(&nRichPhotonData);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pHGeantRichPhotonBr->FindObject("HGeantRichPhoton.fData.eHit"));
	pBrTmp -> SetAddress(fRichPhotonEnergy);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pHGeantRichPhotonBr->FindObject("HGeantRichPhoton.fData.sector"));
	pBrTmp -> SetAddress(cRichPhotonSector);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pHGeantRichPhotonBr->FindObject("HGeantRichPhoton.fData.xHit"));
	pBrTmp -> SetAddress(fRichPhotonXhit);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pHGeantRichPhotonBr->FindObject("HGeantRichPhoton.fData.yHit"));
	pBrTmp -> SetAddress(fRichPhotonYhit);
	pSelectedBr.Add(pBrTmp);
	pBrTmp = (TBranch*)(pHGeantRichPhotonBr->FindObject("HGeantRichPhoton.fData.parentTrack"));
	pBrTmp -> SetAddress(iRichPhotonParentTrack);
	pSelectedBr.Add(pBrTmp);

	
	// Create output ROOT Tree
	pTreeOut = new TTree("RingParams","Tree with parameters of extracted simul rings");
	SIMUL_RING ring; // new datatype to be serialized into a branch 
	// note that the address of the pointer to the object for the branch is needed
	TString sBranchDefinition(256);
	char *var1 = new char[15];
	sprintf(var1,"xcoord[%d]/I:",fieldsize*fieldsize);
	char *var2 = new char[15];
	sprintf(var2,"ycoord[%d]/I:",fieldsize*fieldsize);
	char *var3 = new char[15];
	sprintf(var3,"charge[%d]/F:",fieldsize*fieldsize);
	char *var4 = new char[15];
	sprintf(var4,"fPhX[%d]/F:",maxNumberOfPhotonHits);
	char *var5 = new char[15];
	sprintf(var5,"fPhY[%d]/F:",maxNumberOfPhotonHits);
	char *var6 = new char[15];
	sprintf(var6,"fPhE[%d]/F:",maxNumberOfPhotonHits);
	char *var7 = new char[15];
	sprintf(var7,"iPhParentTrack[%d]/I:",maxNumberOfPhotonHits);
	sBranchDefinition.Append("RingX/I:");
	sBranchDefinition.Append("RingY/I:");
	sBranchDefinition.Append("nPads/I:");
	sBranchDefinition.Append("Phi/F:");
	sBranchDefinition.Append("Theta/F:");
	sBranchDefinition.Append(var1);
	sBranchDefinition.Append(var2);
	sBranchDefinition.Append(var3);
	sBranchDefinition.Append("Ampli/I:");
	sBranchDefinition.Append("LocMax4/I:");
	sBranchDefinition.Append("MatQual/I:");
	sBranchDefinition.Append("HoughQual/I:");
	sBranchDefinition.Append(var4);
	sBranchDefinition.Append(var5);
	sBranchDefinition.Append(var6);
	sBranchDefinition.Append(var7);
	sBranchDefinition.Append("nPhotons/I");
	

	pTreeOut -> Branch("simul_ring",&ring,sBranchDefinition.Data());
	
	for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	    
	    if (i%1000 == 0 && i != 0) {
		cout<<i<<" events processed and "<<
		    pTreeOut->GetEntries()<<" rings extracted"<<endl;
	    }
	    for (Int_t ii=0;ii<((pSelectedBr->GetLast())+1);ii++){
		((TBranch*)pSelectedBr[ii]).GetEntry(i); 
	    }

	    // traverse current tree entry and make selections

	    for (Int_t j=0; j<nRichHitHeaderData; j++){
		if ( iRichHitHeaderRingNr[j] == 1 ){ // one ring in event per sector
		    
		    for (Int_t l=0; l<nRichHitData; l++){ // traverse data of ring
			
			ring.RingX     = iRichHitRingX[l]; // save ring params in struct 
			ring.RingY     = iRichHitRingY[l];
			ring.Phi       = fRichHitPhi[l];
			ring.Theta     = fRichHitTheta[l];
			ring.Ampli     = iRichHitRingAmp[l];
			ring.LocMax4   = iRichHitLocMax4[l];
			ring.MatQual   = iRichHitPatMat[l];
			ring.HoughQual = iRichHitHouTra[l];
			
			for (int a=0;a<169;a++){ // reset struct arrays
			    ring.xcoord[a] = 0;
			    ring.ycoord[a] = 0;
			    ring.charge[a] = 0;
			}
			
			Int_t n=0; // counter for non-zero cal values
			for (Int_t m=0; m<nRichCalData; m++){ // loop over ring data
			    if ( iRichHitSector[l] == iRichCalSector[m]){
				if (TMath::Abs(iRichCalRow[m] - iRichHitRingY[l]) <= 6 &&
				    TMath::Abs(iRichCalCol[m] - iRichHitRingX[l]) <= 6){   
				    ring.xcoord[n] = iRichCalCol[m];
				    ring.ycoord[n] = iRichCalRow[m];   
				    ring.charge[n] = fRichCalCharge[m];
				    n++;
				}
			    }
			} //end of loop over cal container for this ring
			ring.nPads = n;
			for (int b=0;b<maxNumberOfPhotonHits;b++){ // reset struct arrays
			    ring.fPhX[b] = 0.;
			    ring.fPhY[b] = 0.;
			    ring.fPhE[b] = 0.;
			    ring.iPhParentTrack[b] = 0; 
			}
			Int_t nPhCntr = 0;
			for (Int_t nPh=0; nPh<nRichPhotonData; nPh++){
			    if (cRichPhotonSector[nPh] == iRichHitSector[l]){
				Int_t iPadX,iPadY;
				Float_t fPadX,fPadY;
				pPad = pPadsPar->getPad(fRichPhotonYhit[nPh]/10.,
							fRichPhotonXhit[nPh]/10.);
				if (pPad) {
				    pPad->getPadXY(&iPadX,&iPadY);
				    pPad->getXY(&fPadX,&fPadY);
				    if (TMath::Abs(iPadY - iRichHitRingY[l]) < 6 &&
					TMath::Abs(iPadX - iRichHitRingX[l]) < 6){
					Int_t iCornersNr = pPad->getCornersNr();
					HRichPadCorner *pCorner=0;
					Float_t *fCornersX = new Float_t[iCornersNr];
					Float_t *fCornersY = new Float_t[iCornersNr];
					if (iCornersNr == 4){
					    for (Int_t c=0;c<iCornersNr;c++){
						pCorner = pPad -> getCorner(c);
						Float_t cX = pCorner -> getX();
						Float_t cY = pCorner -> getY();
						fCornersX[c] = cX;
						fCornersY[c] = cY; 
					    }
					    Float_t fXRatio = (fRichPhotonYhit[nPh]/10. -
							       fCornersX[0]) / 
						(fCornersX[1] - fCornersX[0]);
					    Float_t fPhotonCoordX = iPadX + fXRatio;
					    Float_t a1 = (fCornersY[1]-fCornersY[0])/
						(fCornersX[1]-fCornersX[0]);
					    Float_t b1 = fCornersY[0] - a1*fCornersX[0];
					    Float_t a2 = (fCornersY[2]-fCornersY[3])/
						(fCornersX[2]-fCornersX[3]);
					    Float_t b2  = fCornersY[3] - a2*fCornersX[3];
					    Float_t y2tmp = a2 * fRichPhotonYhit[nPh]/10. + b2;
					    Float_t y1tmp = a1 * fRichPhotonYhit[nPh]/10. + b1;
					    Float_t fYRatio = (fRichPhotonXhit[nPh]/10. - y1tmp)/
						(y2tmp - y1tmp);
					    Float_t fPhotonCoordY = iPadY + fYRatio;
					    ring.fPhX[nPhCntr] = fPhotonCoordX;
					    ring.fPhY[nPhCntr] = fPhotonCoordY;
					    ring.fPhE[nPhCntr] = fRichPhotonEnergy[nPh];
					    ring.iPhParentTrack[nPhCntr] = iRichPhotonParentTrack[nPh];
					    nPhCntr++;
					} else cout<<"too many corners !!"<<endl;
					delete fCornersX;
					delete fCornersY;
				    }
				}
			    }
			}
			ring.nPhotons = nPhCntr;
		    } // end of loop over ring data
		    
		    pTreeOut->Fill();
		}// if: only for Ring !
		
	    } // end of loop over rings in one event
	    
	    
	}//  end of for loop over events in Tree
	
	pTreeOut->Print();
	pFileIn->Close();
	FileOut->cd();
	pTreeOut->Write();
	FileOut.Close();
    }
}
