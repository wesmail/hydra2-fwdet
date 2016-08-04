{
///////////////////////////////////////////////////////////
// This file has been automatically generated 
// (Wed Dec 13 14:59:27 2000 by ROOT version2.25/03)
// from HTree T/T.2
// found on file: /d/hades/koenig/testWK.root
///////////////////////////////////////////////////////////


//Reset ROOT and connect tree file
   gROOT->Reset();
//TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/d/hades/koenig/testWK.root");
//TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/scratch/titan1/hgeantoutput/testWK.root");
//TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/scratch/titan1/jupiter/GeantOutput/C1COct00/C1Cbrem/C1CthermCent30k21.root");
TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/lfabbi/hydra_v521/testCarbonNewPar.root");
   if (!f) {
       f = new TFile("/home/lfabbi/hydra_v521/testCarbonNewPar.root");
       //       f = new TFile("/scratch/titan1/hgeantoutput/testWK.root");
       //     //    f = new TFile("/d/hades/koenig/testWK.root");
   }
   HTree *T = (HTree*)gDirectory->Get("T");

   if(!T) {cout << "No tree in file!" << endl; exit();}

   HRecEvent* fEvent = (HRecEvent*)f->Get("Event");
   if(fEvent) HEventHeader* fHead = fEvent->getHeader();
   T->SetBranchAddress("Event",&fEvent);

   f->cd("dirRich"); 
   HRichHit* ptlHRichHit = new HRichHit();
   HLinearCategory* catlHRichHit = (HLinearCategory*) gDirectory->Get("HRichHit");
   TBranch* brlHRichHit = T->GetBranch("HRichHit");
   if(brlHRichHit) brlHRichHit->SetAddress(&catlHRichHit);

   HRichCalSim* ptmHRichCalSim = new HRichCalSim();
   HMatrixCategory* catmHRichCalSim = (HMatrixCategory*) gDirectory->Get("HRichCalSim");
   TBranch* brmHRichCalSim = T->GetBranch("HRichCalSim");
   if(brmHRichCalSim) brmHRichCalSim->SetAddress(&catmHRichCalSim);

   HRichTrack* ptlHRichTrack = new HRichTrack();
   HLinearCategory* catlHRichTrack = (HLinearCategory*) gDirectory->Get("HRichTrack");
   TBranch* brlHRichTrack = T->GetBranch("HRichTrack");
   if(brlHRichTrack) brlHRichTrack->SetAddress(&catlHRichTrack);

   HRichHitHeader* ptlHRichHitHeader = new HRichHitHeader();
   HLinearCategory* catlHRichHitHeader = (HLinearCategory*) gDirectory->Get("HRichHitHeader");
   TBranch* brlHRichHitHeader = T->GetBranch("HRichHitHeader");
   if(brlHRichHitHeader) brlHRichHitHeader->SetAddress(&catlHRichHitHeader);

   f->cd("dirSimul"); 
   HGeantKine* ptlHGeantKine = new HGeantKine();
   HLinearCategory* catlHGeantKine = (HLinearCategory*) gDirectory->Get("HGeantKine");
   TBranch* brlHGeantKine = T->GetBranch("HGeantKine");
   if(brlHGeantKine) brlHGeantKine->SetAddress(&catlHGeantKine);

   HGeantRichPhoton* ptmHGeantRichPhoton = new HGeantRichPhoton();
   HMatrixCategory* catmHGeantRichPhoton = (HMatrixCategory*) gDirectory->Get("HGeantRichPhoton");
   TBranch* brmHGeantRichPhoton = T->GetBranch("HGeantRichPhoton");
   if(brmHGeantRichPhoton) brmHGeantRichPhoton->SetAddress(&catmHGeantRichPhoton);

   HGeantRichDirect* ptmHGeantRichDirect = new HGeantRichDirect();
   HMatrixCategory* catmHGeantRichDirect = (HMatrixCategory*) gDirectory->Get("HGeantRichDirect");
   TBranch* brmHGeantRichDirect = T->GetBranch("HGeantRichDirect");
   if(brmHGeantRichDirect) brmHGeantRichDirect->SetAddress(&catmHGeantRichDirect);

   HGeantRichMirror* ptmHGeantRichMirror = new HGeantRichMirror();
   HMatrixCategory* catmHGeantRichMirror = (HMatrixCategory*) gDirectory->Get("HGeantRichMirror");
   TBranch* brmHGeantRichMirror = T->GetBranch("HGeantRichMirror");
   if(brmHGeantRichMirror) brmHGeantRichMirror->SetAddress(&catmHGeantRichMirror);

   f->cd();

//     This is the loop skeleton
//       To read only selected branches, Insert statements like:
// T->SetBranchStatus("*",0);  // disable all branches
// T->SetBranchStatus("branchname",1);  // activate branchname

   Int_t nentries = T->GetEntries();

   Int_t nbytes = 0;
   Int_t nMatch = 0;
   HGeantKine * kine;
   Int_t nTracks,trackNo,particleId;
   Int_t parent,medium,mechanism, mult, pairMult, ringMult;
   Float_t pX,pY,pZ,pT,pTot,pTheta,pPhi,x,y,z;
   HRichHit * hit;
   Float_t factor=1.0F/57.2958F;
   Float_t phi,theta,deltaPhi,deltaTheta;
   Float_t phiCorr=1.1F;
   Int_t pIdArray[800];
   TH1F * dTheta = new TH1F("dTheta","delta theta",361,-90,90);
   TH1F * dPhi = new TH1F("dPhi","delta phi",361,-90,90);
   TH1F * dThetaCut = new TH1F("dThetaCut","delta theta: cut on delta phi",360,-90,90);
   TH1F * dPhiCut = new TH1F("dPhiCut","delta phi: cut on delta theta",360,-90,90);
   TH1F * dThetaPh = new TH1F("dThetaPh","delta theta photon",360,-90,90);
   TH1F * dPhiPh = new TH1F("dPhiPh","delta phi photon",360,-90,90);
   TH1F * momentum = new TH1F("momentum","momentum",500,0,1000);
   TH1F * mediumId = new TH1F("mediumId","medium id",50,0,50);
   TH1F * parentId = new TH1F("parentId","parent id",50,0,50);
   TH1F * mechanismId = new TH1F("mechanismId","mechanism id",50,0,50);
   TH1F * multiplicity = new TH1F("multiplicity","multiplicity",5,0,5);
   TH1F * rVertex = new TH1F("rVertex","vertex radius",400,0,800);
   TH1F * pairMultiplicity = new TH1F("pairMultiplicity","pair multiplicity",5,0,5);
   TH2F * dPhiPhi = new TH2F("dPhiPhi","delta phi-phi",20,-10,10,360,0,360);

   for (Int_t i=0; i<nentries;i++) {
//   for (Int_t i=0; i<5000;i++) {
      nbytes += brlHRichHit->GetEntry(i);
      ringMult=catlHRichHit->getEntries();
      if(ringMult >=1) {
       pairMult=0;
       for(Int_t n=0;n<ringMult;++n) {
	mult=0;
	hit = (HRichHit *)catlHRichHit->getObject(n);
        phi=hit->fPhi;
	//if(phi<60.0F) phi=330.0F-(phi-30.0F)*phiCorr;
	//else if(phi<120.0F) phi=30.0F-(phi-90.0F)*phiCorr;
	//else if(phi<180.0F) phi=90.0F-(phi-150.0F)*phiCorr;
	//else if(phi<240.0F) phi=150.0F-(phi-210.0F)*phiCorr;
	//else if(phi<300.0F) phi=210.0F-(phi-270.0F)*phiCorr;
	//else phi=270.0F-(phi-330.0F)*phiCorr;
        theta=hit->fTheta+2.0F;
//	cout<<theta<<", "<<phi<<endl;
	brlHGeantKine->GetEntry(i);
        nTracks = catlHGeantKine->getEntries();
//	cout<<nTracks<<endl;
	if(nTracks>=800) nTracks=799;
	for(Int_t k=0;k<nTracks;++k) {
	 kine=(HGeantKine *)catlHGeantKine->getObject(k);
	 kine->getParticle(trackNo,particleId);
	 pIdArray[trackNo]=particleId;
//	 cout<<trackNo<<" id="<<particleId<<endl;
	 if(particleId<=3) {
	  kine->getVertex(x,y,z);
//	  cout<<x<<", "<<y<<", "<<z<<endl;
	  if(z>-40.0F && z<400.0F && x*x+y*y+(z+500.0F)*(z+500.0F) < 810000.0F) {
	   kine->getMomentum(pX,pY,pZ);
	   pT=pX*pX+pY*pY;
	   pTot=sqrt(pT+pZ*pZ);
	   pT=sqrt(pT);
	   pTheta=asin(pT/pTot)/factor;
	   pPhi=asin(pY/pT)/factor;
	   if(pX<0.0F) pPhi=180.0F-pPhi;
	   else if(pY<0.0F) pPhi+=360.0F;
	   if(particleId>1) {
//	    cout<<pTheta<<", "<<pPhi<<endl;
	    deltaPhi=pPhi-phi;
	    deltaTheta=pTheta-theta;
	    dTheta->Fill(deltaTheta);
	    dPhi->Fill(deltaPhi);
	    dPhiPhi->Fill(deltaPhi,pPhi);
	    if(deltaPhi > -4.0F && deltaPhi < 4.0F) dThetaCut->Fill(deltaTheta);
	    if(deltaTheta > -4.0F && deltaTheta < 4.0F) {
	     dPhiCut->Fill(deltaPhi);
	     if(deltaPhi > -4.0F && deltaPhi < 4.0F) {
	       ++nMatch;
	       kine->getCreator(parent,medium,mechanism);
	       cout<<nMatch<<" parent="<<pIdArray[parent]<<" medium="<<medium<<" mechanism="<<mechanism<<" p="<<pTot<<endl;
	       momentum->Fill(pTot);
	       mediumId->Fill(medium);
	       parentId->Fill(pIdArray[parent]);
	       mechanismId->Fill(mechanism);
	       rVertex->Fill(sqrt(x*x+y*y+z*z));
	       ++mult;
	     }
	    }
	   } else {
//	    cout<<"photon: "<<pTheta<<", "<<pPhi<<endl;
	    dThetaPh->Fill(deltaTheta);
	    dPhiPh->Fill(deltaPhi);
	   }
	  }
         }
	}
	multiplicity->Fill(mult);
	if(mult>0) ++pairMult;
       }
       pairMultiplicity->Fill(pairMult);
      }

      catlHRichHit->Clear();
      catlHGeantKine->Clear();
   }
}









