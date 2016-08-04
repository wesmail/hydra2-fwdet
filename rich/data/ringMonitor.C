// File: ringMonitor.C
//
// Author: Thomas Eberl <Thomas.Eberl@physik.tu-muenchen.de>
// Last update by Thomas Eberl: 01/07/02 20:54:40
//

// BUG:
// showRichEvent() gives sometimes seg viol after the first event
// if then called a second time it runs w/o problem. Reason unclear.


/*


use .L ~/development/hades/lib/macros/ringMonitor.C
showRichCalEvent();


cf. printEventListFancy
TChain example for Hydra ROOT files
TChain *T = new TChain("T");
T->Add("xx01112020509_DST.root");
T->Add("xx01112021206_DST.root");
T->Add("xx01112025319_DST.root");
T->Add("xx01112040044_DST.root");
TFile *f = new TFile("xx01112020509_DST.root");

TEventList example:
TFile ff("evtsfromtargetlist.root","READ");
// how to retrieve the list back from the file
TEventList *list = (TEventList*)ff->Get("evtsfromTarget");
*/


/*
// ====================================================
	      // this part is used when displaying in pad units 
	      // instead of lepton angles
	      //   if (iRichCalSector[m] == 5) {
	      //  		       fak = 0;
	      //  		   }else{
	      //  		       fak = (iRichCalSector[m] + 1)*60 - 120;
	      //  		   }
	      //			       
	      //  		   Float_t x = TMath::Sin((col + fak)/57.296) * 
	      //  		       TMath::Sin(row/57.296);
	      //  		   Float_t y = TMath::Cos((col + fak)/57.296) * 
	      //  		       TMath::Sin(row/57.296);
	      //=====================================================

*/


void printHelp(){

 cout << "#####################################################"<<endl;
 cout << "#####################################################"<<endl;
 cout << "##                                                 ##"<<endl;
 cout << "## show all events:          press return          ##"<<endl;    
 cout << "##                                                 ##"<<endl;
 cout << "##                                                 ##"<<endl;
 cout << "## go to next event:         press n+return        ##"<<endl;
 cout << "##                                                 ##"<<endl;
 cout << "## print canvas into file:   press p+return        ##"<<endl;
 cout << "##                                                 ##"<<endl;
 cout << "##                                                 ##"<<endl;
 cout << "#####################################################"<<endl;
 cout << "#####################################################"<<endl;


}
TChain* chainAttachedFiles(){
    TIter next(gROOT->GetListOfFiles());
    TFile *fileobj=0;
    TChain *T = new TChain("T");
    while ( fileobj = (TFile*) next() ){
	T->Add(fileobj->GetName());
    }
    next.Reset();
    fileobj = (TFile*) next();
    TFile *f = new TFile(fileobj->GetName());
    return T;
}



void showRichCalEvent(){
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
 
 TChain* chain = chainAttachedFiles();  
 printHelp();

// put first entry here
Stat_t nMinEntry = 0; // first entry to get from Tree 

// *** DISPLAY

gStyle->SetOptStat(0);
gStyle->SetPalette(1);


TH2F *hist0 = new TH2F("RICH single cal event","RICH single cal event",190,-1.05,1.05,190,-1.05,1.05);

TCanvas* canvas1D = new TCanvas("RICH_Ring_Monitor","RICH_Ring_Monitor",100,100,800,900);
TPad* pad0 = new TPad("pad0","pad0",0.00,0.00,1.0,1.0);
pad0->SetLeftMargin(0.1);
pad0->SetRightMargin(0.15);
pad0->SetBottomMargin(0.15);
pad0->SetTopMargin(0.15);
pad0->SetBorderMode(0);
pad0->SetFrameLineColor(10);
pad0->Modified();
pad0->Draw();
pad0->cd();

 hist0->SetMaximum(100);
 hist0->GetZaxis()->SetNdivisions(6);
 hist0->GetXaxis()->SetLabelOffset(10000);
 hist0->GetYaxis()->SetLabelOffset(10000);
 hist0->GetXaxis()->SetTickLength(.00001);
 hist0->GetYaxis()->SetTickLength(.00001);
 
 hist0->GetXaxis()->SetAxisColor(10);
 hist0->GetYaxis()->SetAxisColor(10);
 hist0->Draw("colz");

 Float_t angle = 120.0/57.296;
 TLine* lin = new TLine(-1,0,1.00,0.00);
 lin->SetLineWidth(2);
 lin->Draw();

 TLine* lin1 = new TLine(-1.0*TMath::Cos(angle),1.0*TMath::Sin(angle),1.0*TMath::Cos(angle),-1.0*TMath::Sin(angle));
 lin1->SetLineWidth(2);
 lin1->Draw();

 TLine* lin2 = new TLine(1.0*TMath::Cos(angle),1.0*TMath::Sin(angle),-1.0*TMath::Cos(angle),-1.0*TMath::Sin(angle));
 lin2->SetLineWidth(2);
 lin2->Draw();
 
 TEllipse* cir = new TEllipse(0,0,1.03,1.03);
 cir->SetLineWidth(2);
 cir->Draw();

 TPaveLabel *pl = new TPaveLabel(-0.05,0.4,0.15,0.6,"#phi = 0","b1NDC");
 pl->SetBorderSize(0);
 pl->SetFillStyle(0);
 pl->SetTextSize(0.99);
 pl->SetTextAngle(90);
 pl->Draw();

 TPaveLabel *filelabel = new TPaveLabel(0.3,0.95,.95,.99,"","b1NDC");
 filelabel->SetBorderSize(0);
 filelabel->SetFillStyle(0);
 filelabel->SetTextSize(0.7);
 filelabel->Draw();

 TPaveLabel *persplabel = new TPaveLabel(-.55,0.90,.95,.95,"photon hits on mirror (projection in x-y, +z)" ,"b1NDC");
 persplabel->SetBorderSize(0);
 persplabel->SetFillStyle(0);
 persplabel->SetTextSize(0.35);
 persplabel->Draw();

 TPaveLabel *evtlabel = new TPaveLabel(0.3,0.9,0.95,0.94,"event # ","b1NDC");
 evtlabel->SetBorderSize(0);
 evtlabel->SetFillStyle(0);
 evtlabel->SetTextSize(0.7);
 evtlabel->Draw();

 TPaveLabel *ringlabel = new TPaveLabel(0.3,0.85,0.95,0.89,"ring # ","b1NDC");
 ringlabel->SetBorderSize(0);
 ringlabel->SetFillStyle(0);
 ringlabel->SetTextSize(0.7);
 ringlabel->Draw(); 
 
 hist0->SetYTitle("");
 hist0->SetTitleOffset(1.2,"y");
 hist0->SetXTitle("");
 canvas1D->cd();

Stat_t nMaxEntry = chain->GetEntries();
//Stat_t nMaxEntry = 100;
cout << "Total number of entries in Chain : " << nMaxEntry << endl;

chain->SetBranchStatus("*",0); 
chain->SetBranchStatus("HRichCal",1);
chain->SetBranchStatus("HRichCal.fData_",1);
chain->SetBranchStatus("HRichCal.fData.fCharge",1);
chain->SetBranchStatus("HRichCal.fData.fCol",1);
chain->SetBranchStatus("HRichCal.fData.fRow",1);
chain->SetBranchStatus("HRichCal.fData.fSector",1);
chain->SetBranchStatus("HRichCal.fData.fEventNr",1);

const Int_t RICHCALMAX = 30000;
     HLinearCategory HRichCal; 
     Int_t iRichCalEventNr[RICHCALMAX];
     Short_t iRichCalSector[RICHCALMAX];
     Char_t iRichCalRow[RICHCALMAX],iRichCalCol[RICHCALMAX];
     Float_t fRichCalCharge[RICHCALMAX];
     Int_t nRichCalData;
     chain->SetBranchAddress("HRichCal", &HRichCal);
     chain->SetBranchAddress("HRichCal.fData_", &nRichCalData);
     chain->SetBranchAddress("HRichCal.fData.fEventNr", iRichCalEventNr);
     chain->SetBranchAddress("HRichCal.fData.fSector",  iRichCalSector);
     chain->SetBranchAddress("HRichCal.fData.fRow",     iRichCalRow);
     chain->SetBranchAddress("HRichCal.fData.fCol",     iRichCalCol);
     chain->SetBranchAddress("HRichCal.fData.fCharge",  fRichCalCharge);
     
     Int_t count = 0;
     
     char *filena = new char[12];
     char *evtnum = new char[15];
     //char *ringnum = new char[15];
     
     for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	 chain -> GetEntry(i);
	  
	 hist0->Reset();
	  
	 for (Int_t m=0; m<nRichCalData; m++){
		   
	      Int_t col = (Int_t) iRichCalCol[m];
	      Int_t row = (Int_t) iRichCalRow[m];
	      Float_t fak = 0.;
	      
	      Float_t fTheta,fPhi;
	      pPad = pPadsPar->getPad(col,row);
			       
	      if (pPad) {
		  fTheta = pPad->getTheta();
		  fPhi   = pPad->getPhi(iRichCalSector[m]);
	      }
	      
	      fak = -90.;
	      
	      Float_t x = TMath::Sin((fPhi + fak)/57.296) * 
		  TMath::Sin(fTheta/57.296);
	      Float_t y = TMath::Cos((fPhi + fak)/57.296) * 
		  TMath::Sin(fTheta/57.296);
	      
	      hist0->Fill(x,y,fRichCalCharge[m]);
	  }
	  
	  count++;
	  sprintf(filena,"evt.%04d",count-1);
	  strcat(filena,".gif");
	  
	  pad0->cd();
	  hist0->SetMaximum(100);
	  hist0->GetZaxis()->SetNdivisions(6);
	  hist0->GetXaxis()->SetLabelOffset(10000);
	  hist0->GetYaxis()->SetLabelOffset(10000);
	  hist0->GetXaxis()->SetTickLength(.00001);
	  hist0->GetYaxis()->SetTickLength(.00001);
	  pad0->SetFrameLineColor(10);
	  hist0->GetXaxis()->SetAxisColor(10);
	  hist0->GetYaxis()->SetAxisColor(10);
	  hist0->Draw("colz");
	  
	  lin->Draw();
	  lin1->Draw();
	  lin2->Draw();
	  cir->Draw();
	  pl->Draw();
	  TString rootfilena((chain->GetFile())->GetName());
	  filelabel->SetLabel(rootfilena.Data());
	  filelabel->Draw();
	  persplabel->Draw();
	  sprintf(evtnum,"event: %07d",i);
	  evtlabel->SetLabel(evtnum);
	  evtlabel->Draw();
	  //	  sprintf(ringnum,"ring: %05d",count);
	  //ringlabel->SetLabel(ringnum);
	  //ringlabel->Draw();
	  
	  canvas1D -> Modified();
	  canvas1D -> Update();
	  
	  char* decision = new char[2];
	  scanf("%[^\n]%*c",decision);
	  if(!strcmp(decision,"p")) {
	      canvas1D->Print(filena);
	  }
	  //} // endif conditions for ring quality
	  //} // endloop over rings in event
	  //} // endif condition for number of rings in event 
	  
	  //} // endloop over rings in event
      } // end of event loop
} // end of function

void showRichCalEventList(TEventList *evtlist){
// You need a TEventList object that corresponds to the merged files !!!!
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
 
 TChain* chain = chainAttachedFiles();  
 printHelp();

// put first entry here
Stat_t nMinEntry = 0; // first entry to get from Tree 

// *** DISPLAY

gStyle->SetOptStat(0);
gStyle->SetPalette(1);


TH2F *hist0 = new TH2F("RICH single cal event","RICH single cal event",190,-1.05,1.05,190,-1.05,1.05);

TCanvas* canvas1D = new TCanvas("RICH_Ring_Monitor","RICH_Ring_Monitor",100,100,800,900);
TPad* pad0 = new TPad("pad0","pad0",0.00,0.00,1.0,1.0);
pad0->SetLeftMargin(0.1);
pad0->SetRightMargin(0.15);
pad0->SetBottomMargin(0.15);
pad0->SetTopMargin(0.15);
pad0->SetBorderMode(0);
pad0->SetFrameLineColor(10);
pad0->Modified();
pad0->Draw();
pad0->cd();

 hist0->SetMaximum(100);
 hist0->GetZaxis()->SetNdivisions(6);
 hist0->GetXaxis()->SetLabelOffset(10000);
 hist0->GetYaxis()->SetLabelOffset(10000);
 hist0->GetXaxis()->SetTickLength(.00001);
 hist0->GetYaxis()->SetTickLength(.00001);
 
 hist0->GetXaxis()->SetAxisColor(10);
 hist0->GetYaxis()->SetAxisColor(10);
 hist0->Draw("colz");

 Float_t angle = 120.0/57.296;
 TLine* lin = new TLine(-1,0,1.00,0.00);
 lin->SetLineWidth(2);
 lin->Draw();

 TLine* lin1 = new TLine(-1.0*TMath::Cos(angle),1.0*TMath::Sin(angle),1.0*TMath::Cos(angle),-1.0*TMath::Sin(angle));
 lin1->SetLineWidth(2);
 lin1->Draw();

 TLine* lin2 = new TLine(1.0*TMath::Cos(angle),1.0*TMath::Sin(angle),-1.0*TMath::Cos(angle),-1.0*TMath::Sin(angle));
 lin2->SetLineWidth(2);
 lin2->Draw();
 
 TEllipse* cir = new TEllipse(0,0,1.03,1.03);
 cir->SetLineWidth(2);
 cir->Draw();

 TPaveLabel *pl = new TPaveLabel(-0.05,0.4,0.15,0.6,"#phi = 0","b1NDC");
 pl->SetBorderSize(0);
 pl->SetFillStyle(0);
 pl->SetTextSize(0.99);
 pl->SetTextAngle(90);
 pl->Draw();

 TPaveLabel *filelabel = new TPaveLabel(0.3,0.95,.95,.99,"","b1NDC");
 filelabel->SetBorderSize(0);
 filelabel->SetFillStyle(0);
 filelabel->SetTextSize(0.7);
 filelabel->Draw();

 TPaveLabel *persplabel = new TPaveLabel(-.55,0.90,.95,.95,"photon hits on mirror (projection in x-y, +z)" ,"b1NDC");
 persplabel->SetBorderSize(0);
 persplabel->SetFillStyle(0);
 persplabel->SetTextSize(0.35);
 persplabel->Draw();

 TPaveLabel *evtlabel = new TPaveLabel(0.3,0.9,0.95,0.94,"event # ","b1NDC");
 evtlabel->SetBorderSize(0);
 evtlabel->SetFillStyle(0);
 evtlabel->SetTextSize(0.7);
 evtlabel->Draw();

 TPaveLabel *ringlabel = new TPaveLabel(0.3,0.85,0.95,0.89,"ring # ","b1NDC");
 ringlabel->SetBorderSize(0);
 ringlabel->SetFillStyle(0);
 ringlabel->SetTextSize(0.7);
 ringlabel->Draw(); 
 
 hist0->SetYTitle("");
 hist0->SetTitleOffset(1.2,"y");
 hist0->SetXTitle("");
 canvas1D->cd();

Stat_t nMaxEntry = chain->GetEntries();
//Stat_t nMaxEntry = 100;
cout << "Total number of entries in Chain : " << nMaxEntry << endl;

chain->SetBranchStatus("*",0); 
chain->SetBranchStatus("HRichCal",1);
chain->SetBranchStatus("HRichCal.fData_",1);
chain->SetBranchStatus("HRichCal.fData.fCharge",1);
chain->SetBranchStatus("HRichCal.fData.fCol",1);
chain->SetBranchStatus("HRichCal.fData.fRow",1);
chain->SetBranchStatus("HRichCal.fData.fSector",1);
chain->SetBranchStatus("HRichCal.fData.fEventNr",1);

const Int_t RICHCALMAX = 30000;
     HLinearCategory HRichCal; 
     Int_t iRichCalEventNr[RICHCALMAX];
     Short_t iRichCalSector[RICHCALMAX];
     Char_t iRichCalRow[RICHCALMAX],iRichCalCol[RICHCALMAX];
     Float_t fRichCalCharge[RICHCALMAX];
     Int_t nRichCalData;
     chain->SetBranchAddress("HRichCal", &HRichCal);
     chain->SetBranchAddress("HRichCal.fData_", &nRichCalData);
     chain->SetBranchAddress("HRichCal.fData.fEventNr", iRichCalEventNr);
     chain->SetBranchAddress("HRichCal.fData.fSector",  iRichCalSector);
     chain->SetBranchAddress("HRichCal.fData.fRow",     iRichCalRow);
     chain->SetBranchAddress("HRichCal.fData.fCol",     iRichCalCol);
     chain->SetBranchAddress("HRichCal.fData.fCharge",  fRichCalCharge);
     
     Int_t count = 0;
     
     char *filena = new char[12];
     char *evtnum = new char[15];
          
     for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	 if (evtlist->Contains(i)){
	 chain -> GetEntry(i);
	  
	 hist0->Reset();
	  
	 for (Int_t m=0; m<nRichCalData; m++){
		   
	      Int_t col = (Int_t) iRichCalCol[m];
	      Int_t row = (Int_t) iRichCalRow[m];
	      Float_t fak = 0.;
	      
	      Float_t fTheta,fPhi;
	      pPad = pPadsPar->getPad(col,row);
			       
	      if (pPad) {
		  fTheta = pPad->getTheta();
		  fPhi   = pPad->getPhi(iRichCalSector[m]);
	      }
	      
	      fak = -90.;
	      
	      Float_t x = TMath::Sin((fPhi + fak)/57.296) * 
		  TMath::Sin(fTheta/57.296);
	      Float_t y = TMath::Cos((fPhi + fak)/57.296) * 
		  TMath::Sin(fTheta/57.296);
	      
	      hist0->Fill(x,y,fRichCalCharge[m]);
	  }
	  
	  count++;
	  sprintf(filena,"evt.%04d",count-1);
	  strcat(filena,".gif");
	  
	  pad0->cd();
	  hist0->SetMaximum(100);
	  hist0->GetZaxis()->SetNdivisions(6);
	  hist0->GetXaxis()->SetLabelOffset(10000);
	  hist0->GetYaxis()->SetLabelOffset(10000);
	  hist0->GetXaxis()->SetTickLength(.00001);
	  hist0->GetYaxis()->SetTickLength(.00001);
	  pad0->SetFrameLineColor(10);
	  hist0->GetXaxis()->SetAxisColor(10);
	  hist0->GetYaxis()->SetAxisColor(10);
	  hist0->Draw("colz");
	  
	  lin->Draw();
	  lin1->Draw();
	  lin2->Draw();
	  cir->Draw();
	  pl->Draw();
	  TString rootfilena((chain->GetFile())->GetName());
	  filelabel->SetLabel(rootfilena.Data());
	  filelabel->Draw();
	  persplabel->Draw();
	  sprintf(evtnum,"event: %07d",i);
	  evtlabel->SetLabel(evtnum);
	  evtlabel->Draw();
	  //	  sprintf(ringnum,"ring: %05d",count);
	  //ringlabel->SetLabel(ringnum);
	  //ringlabel->Draw();
	  
	  canvas1D -> Modified();
	  canvas1D -> Update();
	  
	  char* decision = new char[2];
	  scanf("%[^\n]%*c",decision);
	  if(!strcmp(decision,"p")) {
	      canvas1D->Print(filena);
	  }
	  //} // endif conditions for ring quality
	  //} // endloop over rings in event
	  //} // endif condition for number of rings in event 
	  
	  //} // endloop over rings in event
	 } //end evt list condition
     } // end of event loop
} // end of function

void showRichHitEvent(Int_t minNrHits=1){
    
    // THRESHOLDS
    Int_t nHT = 80;
    Int_t nPM = 550;
    Int_t nMult = 100; // total cal mult lower than
    Int_t nLM4 = 8;    // min nLM4 local maxima in ring
    Int_t nAmp = 100;  // min nAmp charge in ring
    Int_t nPNr = 10;    // min nr of fired pads in ring
    // ------------------------------------------------
    
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
    
    TChain* chain = chainAttachedFiles();  
    printHelp();
    
    // put first entry here
    Stat_t nMinEntry = 0; // first entry to get from Tree 
    
    // *** DISPLAY
    
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1);
    
    TH2F *hist0 = new TH2F("RICH single ring event","RICH single ring event",190,-1.05,1.05,190,-1.05,1.05);
    
    TCanvas* canvas1D = new TCanvas("RICH_Ring_Monitor","RICH_Ring_Monitor",100,100,800,900);
    
    TPad* pad0 = new TPad("pad0","pad0",0.00,0.00,1.0,1.0);
    pad0->SetLeftMargin(0.1);
    pad0->SetRightMargin(0.15);
    pad0->SetBottomMargin(0.15);
    pad0->SetTopMargin(0.15);
    pad0->SetBorderMode(0);
    pad0->SetFrameLineColor(10);
    pad0->Modified();
    pad0->Draw();
    pad0->cd();
    
    hist0->SetMaximum(100);
    hist0->GetZaxis()->SetNdivisions(6);
    hist0->GetXaxis()->SetLabelOffset(10000);
    hist0->GetYaxis()->SetLabelOffset(10000);
    hist0->GetXaxis()->SetTickLength(.00001);
    hist0->GetYaxis()->SetTickLength(.00001);
    
    hist0->GetXaxis()->SetAxisColor(10);
    hist0->GetYaxis()->SetAxisColor(10);
    hist0->Draw("colz");
    
    Float_t angle = 120.0/57.296;
    TLine* lin = new TLine(-1,0,1.00,0.00);
    lin->SetLineWidth(2);
    lin->Draw();
    TLine* lin1 = new TLine(-1.0*TMath::Cos(angle),1.0*TMath::Sin(angle),1.0*TMath::Cos(angle),-1.0*TMath::Sin(angle));
    lin1->SetLineWidth(2);
    lin1->Draw();
    TLine* lin2 = new TLine(1.0*TMath::Cos(angle),1.0*TMath::Sin(angle),-1.0*TMath::Cos(angle),-1.0*TMath::Sin(angle));
    lin2->SetLineWidth(2);
    lin2->Draw();
    TEllipse* cir = new TEllipse(0,0,1.03,1.03);
    cir->SetLineWidth(2);
    cir->Draw();
    
    TPaveLabel *pl = new TPaveLabel(-0.05,0.4,0.15,0.6,"#phi = 0","b1NDC");
    pl->SetBorderSize(0);
    pl->SetFillStyle(0);
    pl->SetTextSize(0.99);
    pl->SetTextAngle(90);
    pl->Draw();
    TPaveLabel *filelabel = new TPaveLabel(0.3,0.95,.95,.99,"","b1NDC");
    filelabel->SetBorderSize(0);
    filelabel->SetFillStyle(0);
    filelabel->SetTextSize(0.7);
    filelabel->Draw();
    
    TPaveLabel *persplabel = new TPaveLabel(-.55,0.90,.95,.95,"photon hits on mirror (projection in x-y, +z)" ,"b1NDC");
    persplabel->SetBorderSize(0);
    persplabel->SetFillStyle(0);
    persplabel->SetTextSize(0.35);
    persplabel->Draw();
    
    TPaveLabel *evtlabel = new TPaveLabel(0.3,0.9,0.95,0.94,"event # ","b1NDC");
    evtlabel->SetBorderSize(0);
    evtlabel->SetFillStyle(0);
    evtlabel->SetTextSize(0.7);
    evtlabel->Draw();
    
    TPaveLabel *ringlabel = new TPaveLabel(0.3,0.85,0.95,0.89,"ring # ","b1NDC");
    ringlabel->SetBorderSize(0);
    ringlabel->SetFillStyle(0);
    ringlabel->SetTextSize(0.7);
    ringlabel->Draw(); 
    
    TPaveLabel *htlabel = new TPaveLabel(-1.02749,-1.30634,-0.559648,-1.16549,"HT: ","br");
    htlabel->SetBorderSize(0);
    htlabel->SetFillStyle(0);
    htlabel->SetTextSize(0.7);
    htlabel->Draw(); 
    
    TPaveLabel *pmlabel = new TPaveLabel(-0.335542,-1.30634,0.0488413,-1.16549,"PM: ","br");
    pmlabel->SetBorderSize(0);
    pmlabel->SetFillStyle(0);
    pmlabel->SetTextSize(0.7);
    pmlabel->Draw(); 
    
    TPaveLabel *amplilabel = new TPaveLabel(0.457909,-1.30634,0.88461,-1.16549,"amplitude: ","br");
    amplilabel->SetBorderSize(0);
    amplilabel->SetFillStyle(0);
    amplilabel->SetTextSize(0.7);
    amplilabel->Draw();
    
    TPaveLabel *padnrlabel = new TPaveLabel(-1.02749,-1.42588,-0.543602,-1.32706,"pad#: ","br");
    padnrlabel->SetBorderSize(0);
    padnrlabel->SetFillStyle(0);
    padnrlabel->SetTextSize(0.7);
    padnrlabel->Draw();
    
    TPaveLabel *locmax4label = new TPaveLabel(-0.384912,-1.42588,0.00652394,-1.32706,"locmax4: ","br");
    locmax4label->SetBorderSize(0);
    locmax4label->SetFillStyle(0);
    locmax4label->SetTextSize(0.7);
    locmax4label->Draw();
    
    hist0->SetYTitle("");
    hist0->SetTitleOffset(1.2,"y");
    hist0->SetXTitle("");
    canvas1D->cd();
    // ***
    
    Stat_t nMaxEntry = chain->GetEntries();
    //Stat_t nMaxEntry = 100;
    cout << "Total number of entries in Chain : " << nMaxEntry << endl;
    
    chain->SetBranchStatus("*",0); 
    
    chain->SetBranchStatus("HRichCal",1);
    chain->SetBranchStatus("HRichCal.fData_",1);
    chain->SetBranchStatus("HRichCal.fData.fCharge",1);
    chain->SetBranchStatus("HRichCal.fData.fCol",1);
    chain->SetBranchStatus("HRichCal.fData.fRow",1);
    chain->SetBranchStatus("HRichCal.fData.fSector",1);
    chain->SetBranchStatus("HRichCal.fData.fEventNr",1);
    const Int_t RICHCALMAX = 30000;
    HLinearCategory HRichCal; 
    Int_t iRichCalEventNr[RICHCALMAX];
    Short_t iRichCalSector[RICHCALMAX];
    Char_t iRichCalRow[RICHCALMAX],iRichCalCol[RICHCALMAX];
    Float_t fRichCalCharge[RICHCALMAX];
    Int_t nRichCalData;
    chain->SetBranchAddress("HRichCal", &HRichCal);
    chain->SetBranchAddress("HRichCal.fData_", &nRichCalData);
    chain->SetBranchAddress("HRichCal.fData.fEventNr", iRichCalEventNr);
    chain->SetBranchAddress("HRichCal.fData.fSector",  iRichCalSector);
    chain->SetBranchAddress("HRichCal.fData.fRow",     iRichCalRow);
    chain->SetBranchAddress("HRichCal.fData.fCol",     iRichCalCol);
    chain->SetBranchAddress("HRichCal.fData.fCharge",  fRichCalCharge);
    
    
    chain->SetBranchStatus("HRichHit",1);
    chain->SetBranchStatus("HRichHit.fData_",1);
    chain->SetBranchStatus("HRichHit.fData.fTheta",1);
    chain->SetBranchStatus("HRichHit.fData.fPhi",1);
    chain->SetBranchStatus("HRichHit.fData.iRingPatMat",1);
    chain->SetBranchStatus("HRichHit.fData.iRingHouTra",1);
    chain->SetBranchStatus("HRichHit.fData.nSector",1);
    chain->SetBranchStatus("HRichHit.fData.iRingLocalMax4",1);
    chain->SetBranchStatus("HRichHit.fData.iRingX",1);
    chain->SetBranchStatus("HRichHit.fData.iRingY",1);
    chain->SetBranchStatus("HRichHit.fData.iRingAmplitude",1);
    chain->SetBranchStatus("HRichHit.fData.iRingPadNr",1);
    const Int_t RICHHITMAX = 20;
    HLinearCategory HRichHit; 
    Float_t fRichHitTheta[RICHHITMAX], fRichHitPhi[RICHHITMAX];
    Int_t iRichHitPatMat[RICHHITMAX], iRichHitHouTra[RICHHITMAX];
    Int_t iRichHitSector[RICHHITMAX];
    Short_t iRichHitLocMax4[RICHHITMAX];
    Int_t iRichHitRingX[RICHHITMAX], iRichHitRingY[RICHHITMAX];
    Short_t iRichHitAmpli[RICHHITMAX], iRichHitPadNr[RICHHITMAX];
    Int_t nRichHitData;
    chain->SetBranchAddress("HRichHit", &HRichHit);
    chain->SetBranchAddress("HRichHit.fData_", &nRichHitData);
    chain->SetBranchAddress("HRichHit.fData.fTheta", fRichHitTheta);
    chain->SetBranchAddress("HRichHit.fData.fPhi",   fRichHitPhi);
    chain->SetBranchAddress("HRichHit.fData.iRingPatMat", iRichHitPatMat);
    chain->SetBranchAddress("HRichHit.fData.iRingHouTra", iRichHitHouTra);
    chain->SetBranchAddress("HRichHit.fData.nSector",  iRichHitSector);
    chain->SetBranchAddress("HRichHit.fData.iRingLocalMax4", iRichHitLocMax4);
    chain->SetBranchAddress("HRichHit.fData.iRingX",  iRichHitRingX);
    chain->SetBranchAddress("HRichHit.fData.iRingY",  iRichHitRingY);
    chain->SetBranchAddress("HRichHit.fData.iRingAmplitude", iRichHitAmpli);
    chain->SetBranchAddress("HRichHit.fData.iRingPadNr", iRichHitPadNr);
    
    
    chain->SetBranchStatus("HRichHitHeader",1);
    chain->SetBranchStatus("HRichHitHeader.fData_",1);
    chain->SetBranchStatus("HRichHitHeader.fData.iRingNr",1);
    const Int_t RICHHITHEADERMAX = 10;
    
    HLinearCategory HRichHitHeader; 
    Int_t iRichHitHeaderRingNr[RICHHITHEADERMAX];
    Int_t iRichHitHeaderEventNr[RICHHITHEADERMAX];
    Int_t nRichHitHeaderData;    
    
    chain->SetBranchAddress("HRichHitHeader", &HRichHitHeader);
    chain->SetBranchAddress("HRichHitHeader.fData_", &nRichHitHeaderData);
    chain->SetBranchAddress("HRichHitHeader.fData.iRingNr", iRichHitHeaderRingNr);
    
    Int_t count = 0;
    
    
    char *filena = new char[12];
    char *evtnum = new char[15];
    char *ringnum = new char[15];
    char *ht = new char[15];
    char *pm = new char[15];
    char *padnr = new char[15];
    char *ampli = new char[15];
    char *locmax4 = new char[15];
    
    for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	chain -> GetEntry(i);
	
	Int_t iNoDoubleImages = 0;
	
	for (Int_t j=0; j<nRichHitHeaderData; j++){
	    if ( iRichHitHeaderRingNr[j] >= minNrHits ){
		
		hist0->Reset();
		
		for (Int_t n=0; n<nRichHitData; n++){
		    
		    if ( iNoDoubleImages == 0 && iRichHitHouTra[n] > nHT && 
			 iRichHitPatMat[n] > nPM && nRichCalData < nMult &&
			 iRichHitLocMax4 >= nLM4 && iRichHitAmpli >= nAmp &&
			 iRichHitPadNr >= nPNr
			 ){
			
			iNoDoubleImages++;
			
			for (Int_t m=0; m<nRichCalData; m++){
			    
			    Int_t col = (Int_t) iRichCalCol[m];
			    Int_t row = (Int_t) iRichCalRow[m];
			    Float_t fak = 0.;
			    
			    Float_t fTheta,fPhi;
			    pPad = pPadsPar->getPad(col,row);
			    
			    if (pPad) {
				fTheta = pPad->getTheta();
				fPhi   = pPad->getPhi(iRichCalSector[m]);
			    }
			    
			    fak = -90.;
			    
			    Float_t x = TMath::Sin((fPhi + fak)/57.296) * 
				TMath::Sin(fTheta/57.296);
			    Float_t y = TMath::Cos((fPhi + fak)/57.296) * 
				TMath::Sin(fTheta/57.296);
			    
			    hist0->Fill(x,y,fRichCalCharge[m]);
			}
			
			count++;
			sprintf(filena,"ring.%04d",count-1);
			strcat(filena,".eps");
			
			pad0->cd();
			hist0->SetMaximum(100);
			hist0->GetZaxis()->SetNdivisions(6);
			hist0->GetXaxis()->SetLabelOffset(10000);
			hist0->GetYaxis()->SetLabelOffset(10000);
			hist0->GetXaxis()->SetTickLength(.00001);
			hist0->GetYaxis()->SetTickLength(.00001);
			pad0->SetFrameLineColor(10);
			hist0->GetXaxis()->SetAxisColor(10);
			hist0->GetYaxis()->SetAxisColor(10);
			hist0->Draw("colz");
			
			lin->Draw();
			lin1->Draw();
			lin2->Draw();
			cir->Draw();
			pl->Draw();
			
			TString rootfilena((chain->GetFile())->GetName());
			filelabel->SetLabel(rootfilena.Data());
			filelabel->Draw();
			
			persplabel->Draw();
			sprintf(evtnum,"event: %07d",i);
			evtlabel->SetLabel(evtnum);
			evtlabel->Draw();
			sprintf(ringnum,"ring: %05d",count);
			ringlabel->SetLabel(ringnum);
			ringlabel->Draw();
			
			sprintf(ht,"HT: %d",iRichHitHouTra[n]);
			htlabel->SetLabel(ht);
			htlabel->Draw();
			
			sprintf(pm,"PM: %d",iRichHitPatMat[n]);
			pmlabel->SetLabel(pm);
			pmlabel->Draw();
			
			sprintf(ampli,"amplitude: %d",iRichHitAmpli[n]);
			amplilabel->SetLabel(ampli);
			amplilabel->Draw();
			
			
			sprintf(padnr,"pad#: %d",iRichHitPadNr[n]);
			padnrlabel->SetLabel(padnr);
			padnrlabel->Draw();
			
			sprintf(locmax4,"locmax4: %d",iRichHitLocMax4[n]);
			locmax4label->SetLabel(locmax4);
			locmax4label->Draw();
			
			
			canvas1D -> Modified();
			canvas1D -> Update();
			
			char* decision = new char[2];
			scanf("%[^\n]%*c",decision);
			if(!strcmp(decision,"p")) {
			    canvas1D->Print(filena);
			}
			
			
			
		    } // endif conditions for ring quality
		} // endloop over ring
	    } // endif condition for number of rings in event 
	    
	} // endloop over rings in event
    } // end of event loop
    
    
}

void showRichHitEventList(TEventList *evtlist, Int_t minNrHits=1){
    
    
    // THRESHOLDS
    Int_t nHT = 80;
    Int_t nPM = 550;
    Int_t nMult = 100; // total cal mult lower than
    Int_t nLM4 = 8;    // min nLM4 local maxima in ring
    Int_t nAmp = 100;  // min nAmp charge in ring
    Int_t nPNr = 10;    // min nr of fired pads in ring
    // ------------------------------------------------
    
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
    
    TChain* chain = chainAttachedFiles();  
    printHelp();
    
    // put first entry here
    Stat_t nMinEntry = 0; // first entry to get from Tree 
    
    // *** DISPLAY
    
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1);
    
    TH2F *hist0 = new TH2F("RICH single ring event","RICH single ring event",190,-1.05,1.05,190,-1.05,1.05);
    
    TCanvas* canvas1D = new TCanvas("RICH_Ring_Monitor","RICH_Ring_Monitor",100,100,800,900);
    
    TPad* pad0 = new TPad("pad0","pad0",0.00,0.00,1.0,1.0);
    pad0->SetLeftMargin(0.1);
    pad0->SetRightMargin(0.15);
    pad0->SetBottomMargin(0.15);
    pad0->SetTopMargin(0.15);
    pad0->SetBorderMode(0);
    pad0->SetFrameLineColor(10);
    pad0->Modified();
    pad0->Draw();
    pad0->cd();
    
    hist0->SetMaximum(100);
    hist0->GetZaxis()->SetNdivisions(6);
    hist0->GetXaxis()->SetLabelOffset(10000);
    hist0->GetYaxis()->SetLabelOffset(10000);
    hist0->GetXaxis()->SetTickLength(.00001);
    hist0->GetYaxis()->SetTickLength(.00001);
    
    hist0->GetXaxis()->SetAxisColor(10);
    hist0->GetYaxis()->SetAxisColor(10);
    hist0->Draw("colz");
    
    Float_t angle = 120.0/57.296;
    TLine* lin = new TLine(-1,0,1.00,0.00);
    lin->SetLineWidth(2);
    lin->Draw();
    TLine* lin1 = new TLine(-1.0*TMath::Cos(angle),1.0*TMath::Sin(angle),1.0*TMath::Cos(angle),-1.0*TMath::Sin(angle));
    lin1->SetLineWidth(2);
    lin1->Draw();
    TLine* lin2 = new TLine(1.0*TMath::Cos(angle),1.0*TMath::Sin(angle),-1.0*TMath::Cos(angle),-1.0*TMath::Sin(angle));
    lin2->SetLineWidth(2);
    lin2->Draw();
    TEllipse* cir = new TEllipse(0,0,1.03,1.03);
    cir->SetLineWidth(2);
    cir->Draw();
    
    TPaveLabel *pl = new TPaveLabel(-0.05,0.4,0.15,0.6,"#phi = 0","b1NDC");
    pl->SetBorderSize(0);
    pl->SetFillStyle(0);
    pl->SetTextSize(0.99);
    pl->SetTextAngle(90);
    pl->Draw();
    TPaveLabel *filelabel = new TPaveLabel(0.3,0.95,.95,.99,"","b1NDC");
    filelabel->SetBorderSize(0);
    filelabel->SetFillStyle(0);
    filelabel->SetTextSize(0.7);
    filelabel->Draw();
    
    TPaveLabel *persplabel = new TPaveLabel(-.55,0.90,.95,.95,"photon hits on mirror (projection in x-y, +z)" ,"b1NDC");
    persplabel->SetBorderSize(0);
    persplabel->SetFillStyle(0);
    persplabel->SetTextSize(0.35);
    persplabel->Draw();
    
    TPaveLabel *evtlabel = new TPaveLabel(0.3,0.9,0.95,0.94,"event # ","b1NDC");
    evtlabel->SetBorderSize(0);
    evtlabel->SetFillStyle(0);
    evtlabel->SetTextSize(0.7);
    evtlabel->Draw();
    
    TPaveLabel *ringlabel = new TPaveLabel(0.3,0.85,0.95,0.89,"ring # ","b1NDC");
    ringlabel->SetBorderSize(0);
    ringlabel->SetFillStyle(0);
    ringlabel->SetTextSize(0.7);
    ringlabel->Draw(); 
    
    TPaveLabel *htlabel = new TPaveLabel(-1.02749,-1.30634,-0.559648,-1.16549,"HT: ","br");
    htlabel->SetBorderSize(0);
    htlabel->SetFillStyle(0);
    htlabel->SetTextSize(0.7);
    htlabel->Draw(); 
    
    TPaveLabel *pmlabel = new TPaveLabel(-0.335542,-1.30634,0.0488413,-1.16549,"PM: ","br");
    pmlabel->SetBorderSize(0);
    pmlabel->SetFillStyle(0);
    pmlabel->SetTextSize(0.7);
    pmlabel->Draw(); 
    
    TPaveLabel *amplilabel = new TPaveLabel(0.457909,-1.30634,0.88461,-1.16549,"amplitude: ","br");
    amplilabel->SetBorderSize(0);
    amplilabel->SetFillStyle(0);
    amplilabel->SetTextSize(0.7);
    amplilabel->Draw();
    
    TPaveLabel *padnrlabel = new TPaveLabel(-1.02749,-1.42588,-0.543602,-1.32706,"pad#: ","br");
    padnrlabel->SetBorderSize(0);
    padnrlabel->SetFillStyle(0);
    padnrlabel->SetTextSize(0.7);
    padnrlabel->Draw();
    
    TPaveLabel *locmax4label = new TPaveLabel(-0.384912,-1.42588,0.00652394,-1.32706,"locmax4: ","br");
    locmax4label->SetBorderSize(0);
    locmax4label->SetFillStyle(0);
    locmax4label->SetTextSize(0.7);
    locmax4label->Draw();
    
    hist0->SetYTitle("");
    hist0->SetTitleOffset(1.2,"y");
    hist0->SetXTitle("");
    canvas1D->cd();
    // ***
    
    Stat_t nMaxEntry = chain->GetEntries();
    //Stat_t nMaxEntry = 100;
    cout << "Total number of entries in Chain : " << nMaxEntry << endl;
    
    chain->SetBranchStatus("*",0); 
    
    chain->SetBranchStatus("HRichCal",1);
    chain->SetBranchStatus("HRichCal.fData_",1);
    chain->SetBranchStatus("HRichCal.fData.fCharge",1);
    chain->SetBranchStatus("HRichCal.fData.fCol",1);
    chain->SetBranchStatus("HRichCal.fData.fRow",1);
    chain->SetBranchStatus("HRichCal.fData.fSector",1);
    chain->SetBranchStatus("HRichCal.fData.fEventNr",1);
    const Int_t RICHCALMAX = 30000;
    HLinearCategory HRichCal; 
    Int_t iRichCalEventNr[RICHCALMAX];
    Short_t iRichCalSector[RICHCALMAX];
    Char_t iRichCalRow[RICHCALMAX],iRichCalCol[RICHCALMAX];
    Float_t fRichCalCharge[RICHCALMAX];
    Int_t nRichCalData;
    chain->SetBranchAddress("HRichCal", &HRichCal);
    chain->SetBranchAddress("HRichCal.fData_", &nRichCalData);
    chain->SetBranchAddress("HRichCal.fData.fEventNr", iRichCalEventNr);
    chain->SetBranchAddress("HRichCal.fData.fSector",  iRichCalSector);
    chain->SetBranchAddress("HRichCal.fData.fRow",     iRichCalRow);
    chain->SetBranchAddress("HRichCal.fData.fCol",     iRichCalCol);
    chain->SetBranchAddress("HRichCal.fData.fCharge",  fRichCalCharge);
    
    
    chain->SetBranchStatus("HRichHit",1);
    chain->SetBranchStatus("HRichHit.fData_",1);
    chain->SetBranchStatus("HRichHit.fData.fTheta",1);
    chain->SetBranchStatus("HRichHit.fData.fPhi",1);
    chain->SetBranchStatus("HRichHit.fData.iRingPatMat",1);
    chain->SetBranchStatus("HRichHit.fData.iRingHouTra",1);
    chain->SetBranchStatus("HRichHit.fData.nSector",1);
    chain->SetBranchStatus("HRichHit.fData.iRingLocalMax4",1);
    chain->SetBranchStatus("HRichHit.fData.iRingX",1);
    chain->SetBranchStatus("HRichHit.fData.iRingY",1);
    chain->SetBranchStatus("HRichHit.fData.iRingAmplitude",1);
    chain->SetBranchStatus("HRichHit.fData.iRingPadNr",1);
    const Int_t RICHHITMAX = 20;
    HLinearCategory HRichHit; 
    Float_t fRichHitTheta[RICHHITMAX], fRichHitPhi[RICHHITMAX];
    Int_t iRichHitPatMat[RICHHITMAX], iRichHitHouTra[RICHHITMAX];
    Int_t iRichHitSector[RICHHITMAX];
    Short_t iRichHitLocMax4[RICHHITMAX];
    Int_t iRichHitRingX[RICHHITMAX], iRichHitRingY[RICHHITMAX];
    Short_t iRichHitAmpli[RICHHITMAX], iRichHitPadNr[RICHHITMAX];
    Int_t nRichHitData;
    chain->SetBranchAddress("HRichHit", &HRichHit);
    chain->SetBranchAddress("HRichHit.fData_", &nRichHitData);
    chain->SetBranchAddress("HRichHit.fData.fTheta", fRichHitTheta);
    chain->SetBranchAddress("HRichHit.fData.fPhi",   fRichHitPhi);
    chain->SetBranchAddress("HRichHit.fData.iRingPatMat", iRichHitPatMat);
    chain->SetBranchAddress("HRichHit.fData.iRingHouTra", iRichHitHouTra);
    chain->SetBranchAddress("HRichHit.fData.nSector",  iRichHitSector);
    chain->SetBranchAddress("HRichHit.fData.iRingLocalMax4", iRichHitLocMax4);
    chain->SetBranchAddress("HRichHit.fData.iRingX",  iRichHitRingX);
    chain->SetBranchAddress("HRichHit.fData.iRingY",  iRichHitRingY);
    chain->SetBranchAddress("HRichHit.fData.iRingAmplitude", iRichHitAmpli);
    chain->SetBranchAddress("HRichHit.fData.iRingPadNr", iRichHitPadNr);
    
    
    chain->SetBranchStatus("HRichHitHeader",1);
    chain->SetBranchStatus("HRichHitHeader.fData_",1);
    chain->SetBranchStatus("HRichHitHeader.fData.iRingNr",1);
    const Int_t RICHHITHEADERMAX = 10;
    
    HLinearCategory HRichHitHeader; 
    Int_t iRichHitHeaderRingNr[RICHHITHEADERMAX];
    Int_t iRichHitHeaderEventNr[RICHHITHEADERMAX];
    Int_t nRichHitHeaderData;    
    
    chain->SetBranchAddress("HRichHitHeader", &HRichHitHeader);
    chain->SetBranchAddress("HRichHitHeader.fData_", &nRichHitHeaderData);
    chain->SetBranchAddress("HRichHitHeader.fData.iRingNr", iRichHitHeaderRingNr);
    
    Int_t count = 0;
    
    
    char *filena = new char[12];
    char *evtnum = new char[15];
    char *ringnum = new char[15];
    char *ht = new char[15];
    char *pm = new char[15];
    char *padnr = new char[15];
    char *ampli = new char[15];
    char *locmax4 = new char[15];
    
    for (Int_t i=nMinEntry; i<nMaxEntry; i++){
	if (evtlist->Contains(i)){
	chain -> GetEntry(i);
	
	Int_t iNoDoubleImages = 0;
	
	for (Int_t j=0; j<nRichHitHeaderData; j++){
	    if ( iRichHitHeaderRingNr[j] >= minNrHits ){
		
		hist0->Reset();
		
		for (Int_t n=0; n<nRichHitData; n++){
		    
		    if ( iNoDoubleImages == 0 && iRichHitHouTra[n] > nHT && 
			 iRichHitPatMat[n] > nPM && nRichCalData < nMult &&
			 iRichHitLocMax4 >= nLM4 && iRichHitAmpli >= nAmp &&
			 iRichHitPadNr >= nPNr
			 ){
			
			iNoDoubleImages++;
			
			for (Int_t m=0; m<nRichCalData; m++){
			    
			    Int_t col = (Int_t) iRichCalCol[m];
			    Int_t row = (Int_t) iRichCalRow[m];
			    Float_t fak = 0.;
			    
			    Float_t fTheta,fPhi;
			    pPad = pPadsPar->getPad(col,row);
			    
			    if (pPad) {
				fTheta = pPad->getTheta();
				fPhi   = pPad->getPhi(iRichCalSector[m]);
			    }
			    
			    fak = -90.;
			    
			    Float_t x = TMath::Sin((fPhi + fak)/57.296) * 
				TMath::Sin(fTheta/57.296);
			    Float_t y = TMath::Cos((fPhi + fak)/57.296) * 
				TMath::Sin(fTheta/57.296);
			    
			    hist0->Fill(x,y,fRichCalCharge[m]);
			}
			
			count++;
			sprintf(filena,"ring.%04d",count-1);
			strcat(filena,".eps");
			
			pad0->cd();
			hist0->SetMaximum(100);
			hist0->GetZaxis()->SetNdivisions(6);
			hist0->GetXaxis()->SetLabelOffset(10000);
			hist0->GetYaxis()->SetLabelOffset(10000);
			hist0->GetXaxis()->SetTickLength(.00001);
			hist0->GetYaxis()->SetTickLength(.00001);
			pad0->SetFrameLineColor(10);
			hist0->GetXaxis()->SetAxisColor(10);
			hist0->GetYaxis()->SetAxisColor(10);
			hist0->Draw("colz");
			
			lin->Draw();
			lin1->Draw();
			lin2->Draw();
			cir->Draw();
			pl->Draw();
			
			TString rootfilena((chain->GetFile())->GetName());
			filelabel->SetLabel(rootfilena.Data());
			filelabel->Draw();
			
			persplabel->Draw();
			sprintf(evtnum,"event: %07d",i);
			evtlabel->SetLabel(evtnum);
			evtlabel->Draw();
			sprintf(ringnum,"ring: %05d",count);
			ringlabel->SetLabel(ringnum);
			ringlabel->Draw();
			
			sprintf(ht,"HT: %d",iRichHitHouTra[n]);
			htlabel->SetLabel(ht);
			htlabel->Draw();
			
			sprintf(pm,"PM: %d",iRichHitPatMat[n]);
			pmlabel->SetLabel(pm);
			pmlabel->Draw();
			
			sprintf(ampli,"amplitude: %d",iRichHitAmpli[n]);
			amplilabel->SetLabel(ampli);
			amplilabel->Draw();
			
			
			sprintf(padnr,"pad#: %d",iRichHitPadNr[n]);
			padnrlabel->SetLabel(padnr);
			padnrlabel->Draw();
			
			sprintf(locmax4,"locmax4: %d",iRichHitLocMax4[n]);
			locmax4label->SetLabel(locmax4);
			locmax4label->Draw();
			
			
			canvas1D -> Modified();
			canvas1D -> Update();
			
			char* decision = new char[2];
			scanf("%[^\n]%*c",decision);
			if(!strcmp(decision,"p")) {
			    canvas1D->Print(filena);
			}
			
			
			
		    } // endif conditions for ring quality
		} // endloop over ring
	    } // endif condition for number of rings in event 
	    
	} // endloop over rings in event
	} // event list cond
    } // end of event loop
    
    
}


void printSinglePadRingsFancy(char *cFileIn){
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
    
 TFile *f = new TFile (cFileIn, "READ");
printHelp();

// put first entry here
Stat_t nMinEntry = 1; // first entry to get from Tree 

// *** DISPLAY

gStyle->SetOptStat(0);
gStyle->SetPalette(1);


TH2F *hist0 = new TH2F("RICH single ring event","RICH single ring event",190,-1.05,1.05,190,-1.05,1.05);

TCanvas* canvas1D = new TCanvas("RICH_Ring_Monitor","RICH_Ring_Monitor",100,100,800,900);
//TPad* pad0 = new TPad("pad0","pad0",0.00,1.0,0.00,1.0);
TPad* pad0 = new TPad("pad0","pad0",0.00,0.00,1.0,1.0);
pad0->SetLeftMargin(0.1);
pad0->SetRightMargin(0.15);
pad0->SetBottomMargin(0.15);
pad0->SetTopMargin(0.15);
pad0->SetBorderMode(0);
pad0->SetFrameLineColor(10);
pad0->Modified();
pad0->Draw();
pad0->cd();

hist0->SetMaximum(100);
 hist0->GetZaxis()->SetNdivisions(6);
 hist0->GetXaxis()->SetLabelOffset(10000);
 hist0->GetYaxis()->SetLabelOffset(10000);
 hist0->GetXaxis()->SetTickLength(.00001);
 hist0->GetYaxis()->SetTickLength(.00001);
 
 hist0->GetXaxis()->SetAxisColor(10);
 hist0->GetYaxis()->SetAxisColor(10);
 hist0->Draw("colz");
 //hist0->Draw();
 Float_t angle = 120.0/57.296;
TLine* lin = new TLine(-1,0,1.00,0.00);
 lin->SetLineWidth(2);
 lin->Draw();
 TLine* lin1 = new TLine(-1.0*TMath::Cos(angle),1.0*TMath::Sin(angle),1.0*TMath::Cos(angle),-1.0*TMath::Sin(angle));
 lin1->SetLineWidth(2);
 lin1->Draw();
TLine* lin2 = new TLine(1.0*TMath::Cos(angle),1.0*TMath::Sin(angle),-1.0*TMath::Cos(angle),-1.0*TMath::Sin(angle));
 lin2->SetLineWidth(2);
 lin2->Draw();
 TEllipse* cir = new TEllipse(0,0,1.03,1.03);
 cir->SetLineWidth(2);
 cir->Draw();

 TPaveLabel *pl = new TPaveLabel(-0.05,0.4,0.15,0.6,"#phi = 0","b1NDC");
 pl->SetBorderSize(0);
 pl->SetFillStyle(0);
 pl->SetTextSize(0.99);
 pl->SetTextAngle(90);
 pl->Draw();
TPaveLabel *filelabel = new TPaveLabel(0.3,0.95,.95,.99,cFileIn,"b1NDC");
 filelabel->SetBorderSize(0);
 filelabel->SetFillStyle(0);
 filelabel->SetTextSize(0.7);
 // filelabel->SetTextAngle(90);
 filelabel->Draw();

TPaveLabel *persplabel = new TPaveLabel(-.55,0.90,.95,.95,"photon hits on padplane (projection in x-y, +z)" ,"b1NDC");
 persplabel->SetBorderSize(0);
 persplabel->SetFillStyle(0);
 persplabel->SetTextSize(0.35);
 // persplabel->SetTextAngle(90);
 persplabel->Draw();

 TPaveLabel *evtlabel = new TPaveLabel(0.3,0.9,0.95,0.94,"event # ","b1NDC");
 evtlabel->SetBorderSize(0);
 evtlabel->SetFillStyle(0);
 evtlabel->SetTextSize(0.7);
 // evtlabel->SetTextAngle(90);
 evtlabel->Draw();

 TPaveLabel *ringlabel = new TPaveLabel(0.3,0.85,0.95,0.89,"ring # ","b1NDC");
 ringlabel->SetBorderSize(0);
 ringlabel->SetFillStyle(0);
 ringlabel->SetTextSize(0.7);
 // ringlabel->SetTextAngle(90);
 ringlabel->Draw(); 
 //canvas1D->cd();
 
 
 
hist0->SetYTitle("");
hist0->SetTitleOffset(1.2,"y");
hist0->SetXTitle("");
canvas1D->cd();
// ***

//TFile *f = new TFile (iname, "READ");
HTree *T = (HTree *) f->Get ("T");

// *** ENTRIES 

Stat_t nMaxEntry = T->GetEntries();
//Stat_t nMaxEntry = 100;
cout << "Total number of entries in File : " << nMaxEntry << endl;

f->cd("dirRich");


const Int_t RICHCALMAX =100000 ;
     Int_t iRichCalEventNr[RICHCALMAX],iRichCalAddress[RICHCALMAX];
     Short_t iRichCalSector[RICHCALMAX];
     Char_t iRichCalRow[RICHCALMAX],iRichCalCol[RICHCALMAX];
     Float_t fRichCalCharge[RICHCALMAX];
     Int_t nRichCalData; //number of data object in tclones.
     
     TBranch *bRichCalNrLeaves,*bRichCalEventNr,*bRichCalAddress;
     TBranch *bRichCalCharge,*bRichCalSector,*bRichCalRow,*bRichCalCol;   
     
     bRichCalNrLeaves = T->GetBranch("HRichCal.fData_"); 
     bRichCalCharge   = T->GetBranch("HRichCal.fData.fCharge");
     bRichCalSector   = T->GetBranch("HRichCal.fData.fSector");
     bRichCalRow      = T->GetBranch("HRichCal.fData.fRow");
     bRichCalCol      = T->GetBranch("HRichCal.fData.fCol");  
     bRichCalNrLeaves -> SetAddress(&nRichCalData);  
     bRichCalCharge   -> SetAddress(fRichCalCharge);
     bRichCalSector   -> SetAddress(iRichCalSector);
     bRichCalRow      -> SetAddress(iRichCalRow);
     bRichCalCol      -> SetAddress(iRichCalCol);  
     
// *** RICH HIT ***

const Int_t RICHHITMAX = 20;
      Float_t fRichHitTheta[RICHHITMAX], fRichHitPhi[RICHHITMAX];
      Int_t iRichHitPatMat[RICHHITMAX], iRichHitHouTra[RICHHITMAX];
      Int_t iRichHitSector[RICHHITMAX];
      Short_t iRichHitLocMax4[RICHHITMAX];
      Int_t iRichHitRingX[RICHHITMAX], iRichHitRingY[RICHHITMAX];
      Int_t iRichHitEvtNr[RICHHITMAX];
      Int_t nRichHitData;
      
      TBranch *bRichHitNrLeaves, *bRichHitTheta, *bRichHitPhi;
      TBranch *bRichHitRingX, *bRichHitRingY, *bRichHitSector;
      TBranch *bRichHitEventNr;
      TBranch *bRichHitpatmat, *bRichHithoutra;
      TBranch *bRichHitLocMax;
      bRichHitNrLeaves = T-> GetBranch("HRichHit.fData_");
      bRichHitTheta    = T-> GetBranch("HRichHit.fData.fTheta");
      bRichHitPhi      = T-> GetBranch("HRichHit.fData.fPhi");
      bRichHitRingX    = T-> GetBranch("HRichHit.fData.iRingX");
      bRichHitRingY    = T-> GetBranch("HRichHit.fData.iRingY");
      bRichHitSector   = T-> GetBranch("HRichHit.fData.nSector");    
      bRichHitpatmat   = T-> GetBranch("HRichHit.fData.iRingPatMat");
      bRichHithoutra   = T-> GetBranch("HRichHit.fData.iRingHouTra");
      bRichHitLocMax   = T-> GetBranch("HRichHit.fData.iRingLocalMax4");
      bRichHitNrLeaves -> SetAddress(&nRichHitData);
      bRichHitTheta    -> SetAddress(fRichHitTheta);
      bRichHitPhi      -> SetAddress(fRichHitPhi);
      bRichHitRingX    -> SetAddress(iRichHitRingX);
      bRichHitRingY    -> SetAddress(iRichHitRingY);
      bRichHitSector   -> SetAddress(iRichHitSector);
      bRichHitpatmat   -> SetAddress(iRichHitPatMat);
      bRichHithoutra   -> SetAddress(iRichHitHouTra);
      bRichHitLocMax   -> SetAddress(iRichHitLocMax4);
 
// *** RICH HIT HEADER ***

      const Int_t RICHHITHEADERMAX = 10;

      Int_t iRichHitHeaderRingNr[RICHHITHEADERMAX];
      Int_t iRichHitHeaderEventNr[RICHHITHEADERMAX];
      Int_t nRichHitHeaderData;    
       
      TBranch *bRichHitHeaderNrLeaves, *bRichHitHeaderRingNr;
      TBranch *bRichHitHeaderEventNr;

      bRichHitHeaderNrLeaves  = T-> GetBranch("HRichHitHeader.fData_");
      bRichHitHeaderRingNr    = T-> GetBranch("HRichHitHeader.fData.iRingNr");
      bRichHitHeaderNrLeaves -> SetAddress(&nRichHitHeaderData);
      bRichHitHeaderRingNr   -> SetAddress(iRichHitHeaderRingNr);
      
      Int_t nBytesRichHitHeader = 0;
      
      Int_t nBytesRichHit       = 0;
      
      Int_t nBytesRichCal       = 0;
      Int_t count = 0;
      //TArc *circ = 0;

      char *filena = new char[12];
      char *evtnum = new char[15];
      char *ringnum = new char[15];
      for (Int_t i=nMinEntry; i<nMaxEntry; i++){
    
       nBytesRichCal += bRichCalNrLeaves -> GetEntry(i); 
       nBytesRichCal += bRichCalCharge   -> GetEntry(i); //charge
       nBytesRichCal += bRichCalSector   -> GetEntry(i); //sector
       nBytesRichCal += bRichCalRow      -> GetEntry(i); //col
       nBytesRichCal += bRichCalCol      -> GetEntry(i); //row  
       nBytesRichHitHeader += bRichHitHeaderNrLeaves -> GetEntry(i);
       nBytesRichHitHeader += bRichHitHeaderRingNr   -> GetEntry(i);
       nBytesRichHit += bRichHitNrLeaves -> GetEntry(i);
       nBytesRichHit += bRichHitTheta    -> GetEntry(i);
       nBytesRichHit += bRichHitPhi      -> GetEntry(i);
       nBytesRichHit += bRichHitRingX    -> GetEntry(i);
       nBytesRichHit += bRichHitRingY    -> GetEntry(i);
       nBytesRichHit += bRichHitSector   -> GetEntry(i);
       nBytesRichHit += bRichHitpatmat   -> GetEntry(i);
       nBytesRichHit += bRichHithoutra   -> GetEntry(i);
       nBytesRichHit += bRichHitLocMax   -> GetEntry(i);

       Int_t iNoDoubleImages = 0;

       for (Int_t j=0; j<nRichHitHeaderData; j++){
	   if ( iRichHitHeaderRingNr[j] == 1 ){
		   
	       hist0->Reset();
		   
		   for (Int_t n=0; n<nRichHitData; n++){
		       
		       if ( iNoDoubleImages == 0 && iRichHitHouTra[n] > 80 && 
			    iRichHitPatMat[n] > 550 && nRichCalData < 100 &&
			    iRichHitLocMax4 >=8){

			   iNoDoubleImages++;
			        
			   for (Int_t m=0; m<nRichCalData; m++){
		   
			       Int_t col = (Int_t) iRichCalCol[m];
			       Int_t row = (Int_t) iRichCalRow[m];
			       Float_t fak = 0.;
			       
			       //Float_t fTheta,fPhi;
			       //pPad = pPadsPar->getPad(col,row);
			       
			       //if (pPad) {
				//   fTheta = pPad->getTheta();
				//   fPhi   = pPad->getPhi(iRichCalSector[m]);
			       //}

			       // ====================================================
			       // this part is used when displaying in pad units 
			       // instead of lepton angles
			       if (iRichCalSector[m] == 5) {
				   fak = 0;
			       }else{
				   fak = (iRichCalSector[m] + 1)*60 - 120;  // fix me !! is wrong ??!!
			       }
			       
			       Float_t x = TMath::Sin((col + fak)/57.296) * 
				   TMath::Sin(row/57.296);
			       Float_t y = TMath::Cos((col + fak)/57.296) * 
				   TMath::Sin(row/57.296);
			       //=====================================================

			       //fak = -90.;
			       
			       //Float_t x = TMath::Sin((fPhi + fak)/57.296) * 
			       //  TMath::Sin(fTheta/57.296);
			       //Float_t y = TMath::Cos((fPhi + fak)/57.296) * 
				//   TMath::Sin(fTheta/57.296);
		   
			       //cout<<"Sector, Col, Row :"<<iRichCalSector[m]<<" "<<col<<" "<<row<<endl;
			       //cout<<"x, y, fak, chrg :"<<x<<" "<<y<<" "<<fak<<" "<<fRichCalCharge[m]<<endl;
		   
			      
			       hist0->Fill(x,y,fRichCalCharge[m]);
			   }
	       
			   count++;
			   sprintf(filena,"ring.%04d",count-1);
			   strcat(filena,".gif");
			   
			   pad0->cd();
			   hist0->SetMaximum(100);
			   hist0->GetZaxis()->SetNdivisions(6);
			   hist0->GetXaxis()->SetLabelOffset(10000);
			   hist0->GetYaxis()->SetLabelOffset(10000);
			   hist0->GetXaxis()->SetTickLength(.00001);
			   hist0->GetYaxis()->SetTickLength(.00001);
			   pad0->SetFrameLineColor(10);
			   hist0->GetXaxis()->SetAxisColor(10);
			   hist0->GetYaxis()->SetAxisColor(10);
			   hist0->Draw("colz");
			   
			   lin->Draw();
			   lin1->Draw();
			   lin2->Draw();
			   cir->Draw();
			   pl->Draw();
			   
			   filelabel->Draw();
			   persplabel->Draw();
			   sprintf(evtnum,"event: %07d",i);
			   evtlabel->SetLabel(evtnum);
			   evtlabel->Draw();
			   sprintf(ringnum,"ring: %05d",count);
			   ringlabel->SetLabel(ringnum);
			   ringlabel->Draw();

			   canvas1D -> Modified();
			   canvas1D -> Update();
			   
			   char* decision = new char[2];
			   scanf("%[^\n]%*c",decision);
			   if(!strcmp(decision,"p")) {
			       canvas1D->Print(filena);
			   }
		       } // endif conditions for ring quality
		   } // endloop over rings in event
	   } // endif condition for number of rings in event 
       } // endloop over rings in event
 } // end of event loop
} // end of function

void printRings(char *cFileIn){
    // print rings in a histogram showing the fired pads
 //  char *opt1 = new char[2];
//   Bool_t net_conn = kFALSE; 
//   cout<<"TNetFile via rootd [0] or local file [1] ? ";
//   scanf("%[^\n]%*c",opt1);
//   if (!strcmp(opt1,"0")){
//       net_conn = kTRUE;
//   }
//   char* opt = new char[2];
//   cout<<"provide a filename [0] or use already open file [1]: "; 
//   scanf("%[^\n]%*c",opt);
//   if(!strcmp(opt,"0")){
//       cout<<"filename :";
//       char* iname= new char[256];
//       scanf("%[^\n]%*c",iname);
//       if (net_conn) {
//  	 TFile *f = TFile::Open(iname,"read");
//       }else{
//  	 TFile *f = new TFile (iname, "READ");
//       }
//   }else{
//       if (net_conn) {
//  	 TFile *f = TFile::Open(iname,"read");
//       }else{
//  	 TFile *f = new TFile (gFile->GetName(), "READ");
//       }
//   }
 TFile *f = new TFile (cFileIn, "READ");
printHelp();

// put first entry here
Stat_t nMinEntry = 1; // first entry to get from Tree 

// *** DISPLAY

gStyle->SetOptStat(0);
gStyle->SetPalette(1);

TH2F *hist0 = new TH2F("hist0","Sector1",96,1,96,96,1,96);

TCanvas* canvas1D = new TCanvas("RICH_Ring_Monitor","RICH_Ring_Monitor",100,100,400,450);
//TPad* pad0 = new TPad("pad0","pad0",0.00,1.0,0.00,1.0);
TPad* pad0 = new TPad("pad0","pad0",0.00,0.00,1.0,1.0);
pad0->SetLeftMargin(0.1);
pad0->SetRightMargin(0.15);
pad0->SetBottomMargin(0.15);
pad0->SetTopMargin(0.15);
pad0->Modified();

canvas1D->cd();
pad0->Draw();


hist0->SetYTitle("y:=Row [Pads]");
hist0->SetTitleOffset(1.2,"y");
hist0->SetXTitle("x:=Col [Pads]");

// ***

//TFile *f = new TFile (iname, "READ");
HTree *T = (HTree *) f->Get ("T");

// *** ENTRIES 

Stat_t nMaxEntry = T->GetEntries();
Stat_t nMaxEntry = 100;
cout << "Total number of entries in File : " << nMaxEntry << endl;

f->cd("dirRich");

// *** RICH CAL ***

const Int_t RICHCALMAX =100000 ; // max number of objects in a TClones array per event.
      Int_t iRichCalEventNr[RICHCALMAX],iRichCalAddress[RICHCALMAX];

      short iRichCalSector[RICHCALMAX];
      char iRichCalRow[RICHCALMAX];
      char iRichCalCol[RICHCALMAX];
      Float_t fRichCalCharge[RICHCALMAX];
      Int_t nRichCalData; //number of data object in tclones.

      TBranch *bRichCalNrLeaves,*bRichCalEventNr,*bRichCalAddress;
      TBranch *bRichCalCharge,*bRichCalSector,*bRichCalRow,*bRichCalCol;   

      bRichCalNrLeaves = T->GetBranch("HRichCal.fData_"); //!! The branch name= bclones+_ 

      bRichCalCharge   = T->GetBranch("HRichCal.fData.fCharge");
      bRichCalSector   = T->GetBranch("HRichCal.fData.fSector");
      bRichCalRow      = T->GetBranch("HRichCal.fData.fRow");
      bRichCalCol      = T->GetBranch("HRichCal.fData.fCol");  
      
      bRichCalNrLeaves -> SetAddress(&nRichCalData);  //!! Set Address  
      bRichCalCharge   -> SetAddress(fRichCalCharge);
      bRichCalSector   -> SetAddress(iRichCalSector);
      bRichCalRow      -> SetAddress(iRichCalRow);
      bRichCalCol      -> SetAddress(iRichCalCol);  

//}      

// *** RICH HIT ***

const Int_t RICHHITMAX = 20;
      Float_t fRichHitTheta[RICHHITMAX], fRichHitPhi[RICHHITMAX];
      Int_t iRichHitPatMat[RICHHITMAX];
      Int_t iRichHitSector[RICHHITMAX];
      Int_t iRichHitRingX[RICHHITMAX], iRichHitRingY[RICHHITMAX];
      Int_t iRichHitEvtNr[RICHHITMAX];
      Int_t nRichHitData;
      
      TBranch *bRichHitNrLeaves, *bRichHitTheta, *bRichHitPhi;
      TBranch *bRichHitRingX, *bRichHitRingY, *bRichHitSector;
      TBranch *bRichHitEventNr;
      TBranch *bRichHitpatmat;

      bRichHitNrLeaves = T-> GetBranch("HRichHit.fData_");
      bRichHitTheta    = T-> GetBranch("HRichHit.fData.fTheta");
      bRichHitPhi      = T-> GetBranch("HRichHit.fData.fPhi");
      bRichHitRingX    = T-> GetBranch("HRichHit.fData.iRingX");
      bRichHitRingY    = T-> GetBranch("HRichHit.fData.iRingY");
      bRichHitSector   = T-> GetBranch("HRichHit.fData.nSector");    
      bRichHitpatmat = T-> GetBranch("HRichHit.fData.iRingPatMat");

      bRichHitNrLeaves -> SetAddress(&nRichHitData);
      bRichHitTheta    -> SetAddress(fRichHitTheta);
      bRichHitPhi      -> SetAddress(fRichHitPhi);
      bRichHitRingX    -> SetAddress(iRichHitRingX);
      bRichHitRingY    -> SetAddress(iRichHitRingY);
      bRichHitSector   -> SetAddress(iRichHitSector);
      bRichHitpatmat   -> SetAddress(iRichHitPatMat);
 
//}

// *** RICH HIT HEADER ***

const Int_t RICHHITHEADERMAX = 10;

      Int_t iRichHitHeaderRingNr[RICHHITHEADERMAX];
      Int_t iRichHitHeaderEventNr[RICHHITHEADERMAX];
      Int_t nRichHitHeaderData;    
       
      TBranch *bRichHitHeaderNrLeaves, *bRichHitHeaderRingNr;
      TBranch *bRichHitHeaderEventNr;

      bRichHitHeaderNrLeaves  = T-> GetBranch("HRichHitHeader.fData_");
      bRichHitHeaderRingNr    = T-> GetBranch("HRichHitHeader.fData.iRingNr");
      bRichHitHeaderNrLeaves -> SetAddress(&nRichHitHeaderData);
      bRichHitHeaderRingNr   -> SetAddress(iRichHitHeaderRingNr);
      Int_t nBytesRichHitHeader = 0;
      Int_t nBytesRichHit       = 0;
      Int_t nBytesRichCal       = 0;

TArc *circ = 0;
Int_t count = 0;
char *filena = new char[12]; 
for (Int_t i=nMinEntry; i<nMaxEntry; i++){
    
       nBytesRichCal += bRichCalNrLeaves -> GetEntry(i); 
       nBytesRichCal += bRichCalCharge   -> GetEntry(i); //charge
       nBytesRichCal += bRichCalSector   -> GetEntry(i); //sector
       nBytesRichCal += bRichCalRow      -> GetEntry(i); //col
       nBytesRichCal += bRichCalCol      -> GetEntry(i); //row  
       nBytesRichHitHeader += bRichHitHeaderNrLeaves -> GetEntry(i);
       nBytesRichHitHeader += bRichHitHeaderRingNr   -> GetEntry(i);
       nBytesRichHit += bRichHitNrLeaves -> GetEntry(i);
       nBytesRichHit += bRichHitTheta    -> GetEntry(i);
       nBytesRichHit += bRichHitPhi      -> GetEntry(i);
       nBytesRichHit += bRichHitRingX    -> GetEntry(i);
       nBytesRichHit += bRichHitRingY    -> GetEntry(i);
       nBytesRichHit += bRichHitSector   -> GetEntry(i);
       nBytesRichHit += bRichHitpatmat   -> GetEntry(i);


       	for (Int_t j=0; j<nRichHitHeaderData; j++){
	    if ( iRichHitHeaderRingNr[j] == 1){
		hist0->Reset();
		for (Int_t n=0; n<nRichHitData; n++){
		    for (Int_t m=0; m<nRichCalData; m++){
			if (  iRichCalSector[m] == iRichHitSector[n] ){
			    
			    Int_t col = (Int_t) iRichCalCol[m];
			    Int_t row = (Int_t) iRichCalRow[m];
			    
			    hist0->Fill(iRichCalCol[m],iRichCalRow[m],
					fRichCalCharge[m]);
			    
			}
		    }
		    count++;
		    sprintf(filena,"ring.%04d",count-1);
		    strcat(filena,".gif");
		    cout<<filena<<endl;
		    pad0->cd();
		    hist0->SetMaximum(100);
		    hist0->Draw("colz");
		    circ = new TArc(iRichHitRingX[0],iRichHitRingY[0],4,0,360);
		    circ->SetLineColor(2);
		    circ->SetLineWidth(1);
		    circ->Draw();
		    canvas1D -> Modified();
		    canvas1D -> Update();
		    char* decision = new char[2];
		    scanf("%[^\n]%*c",decision);
		    if(!strcmp(decision,"p")) {
			canvas1D->Print(filena);
		    }
		}   
	    }
	}
	
	
}//  end of for loop over events in Tree
	   
	   
	   
} // end of function


void printAllRings(){

    TIter next(gROOT->GetListOfFiles());
    TFile *f=0;
    Int_t count = 0;
    while ( f = (TFile*) next() ){
	
	Stat_t nMinEntry = 1; // first entry to get from Tree 

     gStyle->SetOptStat(0);
     gStyle->SetPalette(1);
     TH2F *hist0 = new TH2F("hist0","Sector1",96,1,96,96,1,96);
     TCanvas* canvas1D = new TCanvas("RICH_Ring_Monitor",
				     "RICH_Ring_Monitor",100,100,400,400);
     TPad* pad0 = new TPad("pad0","pad0",0.00,0.00,1.0,1.0);
     pad0->SetLeftMargin(0.15);
     pad0->SetRightMargin(0.15);
     pad0->SetBottomMargin(0.15);
     pad0->SetTopMargin(0.15);
     pad0->Modified();
     canvas1D->cd();
     pad0->Draw();
     hist0->SetYTitle("y:=Row [Pads]");
     hist0->SetTitleOffset(1.2,"y");
     hist0->SetXTitle("x:=Col [Pads]");

     HTree *T = (HTree *) f->Get ("T");
     Stat_t nMaxEntry = T->GetEntries();
     cout << "Total number of entries in "<<f->GetName()<<": " << nMaxEntry << endl;
     f->cd("dirRich");
     
     const Int_t RICHCALMAX =100000 ;
     Int_t iRichCalEventNr[RICHCALMAX],iRichCalAddress[RICHCALMAX];
     Short_t iRichCalSector[RICHCALMAX];
     Char_t iRichCalRow[RICHCALMAX],iRichCalCol[RICHCALMAX];
     Float_t fRichCalCharge[RICHCALMAX];
     Int_t nRichCalData; //number of data object in tclones.
     
     TBranch *bRichCalNrLeaves,*bRichCalEventNr,*bRichCalAddress;
     TBranch *bRichCalCharge,*bRichCalSector,*bRichCalRow,*bRichCalCol;   
     
     bRichCalNrLeaves = T->GetBranch("HRichCal.fData_"); 
     bRichCalCharge   = T->GetBranch("HRichCal.fData.fCharge");
     bRichCalSector   = T->GetBranch("HRichCal.fData.fSector");
     bRichCalRow      = T->GetBranch("HRichCal.fData.fRow");
     bRichCalCol      = T->GetBranch("HRichCal.fData.fCol");  
     bRichCalNrLeaves -> SetAddress(&nRichCalData);  
     bRichCalCharge   -> SetAddress(fRichCalCharge);
     bRichCalSector   -> SetAddress(iRichCalSector);
     bRichCalRow      -> SetAddress(iRichCalRow);
     bRichCalCol      -> SetAddress(iRichCalCol);  
     
// *** RICH HIT ***

const Int_t RICHHITMAX = 20;
      Float_t fRichHitTheta[RICHHITMAX], fRichHitPhi[RICHHITMAX];
      Int_t iRichHitPatMat[RICHHITMAX], iRichHitHouTra[RICHHITMAX];
      Int_t iRichHitSector[RICHHITMAX];
      Short_t iRichHitLocMax4[RICHHITMAX];
      Int_t iRichHitRingX[RICHHITMAX], iRichHitRingY[RICHHITMAX];
      Int_t iRichHitEvtNr[RICHHITMAX];
      Int_t nRichHitData;
      
      TBranch *bRichHitNrLeaves, *bRichHitTheta, *bRichHitPhi;
      TBranch *bRichHitRingX, *bRichHitRingY, *bRichHitSector;
      TBranch *bRichHitEventNr;
      TBranch *bRichHitpatmat, *bRichHithoutra;
      TBranch *bRichHitLocMax;
      bRichHitNrLeaves = T-> GetBranch("HRichHit.fData_");
      bRichHitTheta    = T-> GetBranch("HRichHit.fData.fTheta");
      bRichHitPhi      = T-> GetBranch("HRichHit.fData.fPhi");
      bRichHitRingX    = T-> GetBranch("HRichHit.fData.iRingX");
      bRichHitRingY    = T-> GetBranch("HRichHit.fData.iRingY");
      bRichHitSector   = T-> GetBranch("HRichHit.fData.nSector");    
      bRichHitpatmat   = T-> GetBranch("HRichHit.fData.iRingPatMat");
      bRichHithoutra   = T-> GetBranch("HRichHit.fData.iRingHouTra");
      bRichHitLocMax   = T-> GetBranch("HRichHit.fData.iRingLocalMax4");
      bRichHitNrLeaves -> SetAddress(&nRichHitData);
      bRichHitTheta    -> SetAddress(fRichHitTheta);
      bRichHitPhi      -> SetAddress(fRichHitPhi);
      bRichHitRingX    -> SetAddress(iRichHitRingX);
      bRichHitRingY    -> SetAddress(iRichHitRingY);
      bRichHitSector   -> SetAddress(iRichHitSector);
      bRichHitpatmat   -> SetAddress(iRichHitPatMat);
      bRichHithoutra   -> SetAddress(iRichHitHouTra);
      bRichHitLocMax   -> SetAddress(iRichHitLocMax4);
 
// *** RICH HIT HEADER ***

      const Int_t RICHHITHEADERMAX = 10;

      Int_t iRichHitHeaderRingNr[RICHHITHEADERMAX];
      Int_t iRichHitHeaderEventNr[RICHHITHEADERMAX];
      Int_t nRichHitHeaderData;    
       
      TBranch *bRichHitHeaderNrLeaves, *bRichHitHeaderRingNr;
      TBranch *bRichHitHeaderEventNr;

      bRichHitHeaderNrLeaves  = T-> GetBranch("HRichHitHeader.fData_");
      bRichHitHeaderRingNr    = T-> GetBranch("HRichHitHeader.fData.iRingNr");
      bRichHitHeaderNrLeaves -> SetAddress(&nRichHitHeaderData);
      bRichHitHeaderRingNr   -> SetAddress(iRichHitHeaderRingNr);
      
      Int_t nBytesRichHitHeader = 0;
      
      Int_t nBytesRichHit       = 0;
      
      Int_t nBytesRichCal       = 0;
      //Int_t count = 0;
      //TArc *circ = 0;

      char *filena = new char[12]; 
      for (Int_t i=nMinEntry; i<nMaxEntry; i++){
    
       nBytesRichCal += bRichCalNrLeaves -> GetEntry(i); 
       nBytesRichCal += bRichCalCharge   -> GetEntry(i); //charge
       nBytesRichCal += bRichCalSector   -> GetEntry(i); //sector
       nBytesRichCal += bRichCalRow      -> GetEntry(i); //col
       nBytesRichCal += bRichCalCol      -> GetEntry(i); //row  
       nBytesRichHitHeader += bRichHitHeaderNrLeaves -> GetEntry(i);
       nBytesRichHitHeader += bRichHitHeaderRingNr   -> GetEntry(i);
       nBytesRichHit += bRichHitNrLeaves -> GetEntry(i);
       nBytesRichHit += bRichHitTheta    -> GetEntry(i);
       nBytesRichHit += bRichHitPhi      -> GetEntry(i);
       nBytesRichHit += bRichHitRingX    -> GetEntry(i);
       nBytesRichHit += bRichHitRingY    -> GetEntry(i);
       nBytesRichHit += bRichHitSector   -> GetEntry(i);
       nBytesRichHit += bRichHitpatmat   -> GetEntry(i);
       nBytesRichHit += bRichHithoutra   -> GetEntry(i);
       nBytesRichHit += bRichHitLocMax   -> GetEntry(i);
       Int_t iNoDoubleImages = 0;
       	for (Int_t j=0; j<nRichHitHeaderData; j++){
	       if ( iRichHitHeaderRingNr[j] >= 2 ){
		   
		   if (circ) delete circ;
		   hist0->Reset();
		   
		   for (Int_t n=0; n<nRichHitData; n++){
		       
		       if ( iNoDoubleImages == 0 && iRichHitHouTra[n] > 80 && 
			    iRichHitPatMat[n] > 550 && nRichCalData < 100 &&
			    iRichHitLocMax4 >=8){
			   iNoDoubleImages++;
			   for (Int_t m=0; m<nRichCalData; m++){
			       if (  iRichCalSector[m] == iRichHitSector[n] ){
				   
				   hist0->Fill(iRichCalCol[m],iRichCalRow[m],
					       fRichCalCharge[m]);
				   
			       }
			   }

		       
		       count++;
		       sprintf(filena,"ring.%04d",count-1);
		       strcat(filena,".gif");
		       cout<<filena<<endl;
		       pad0->cd();
		       hist0->SetMaximum(500);
		       hist0->Draw("colz");

		       //circ = new TArc(iRichHitRingX[0],iRichHitRingY[0],4,0,360);
		       
		       //circ->SetLineColor(2);
		       //circ->SetLineWidth(1);
		       //circ->Draw();
		       
		       canvas1D -> Modified();
		       canvas1D -> Update();


		       //char* decision = new char[2];
		       //scanf("%[^\n]%*c",decision);
		       
		       //if(!strcmp(decision,"f")) {
		       canvas1D->Print(filena);
		       //   cout<< " canvas written to file: ring.ps"<<endl;
			   // }
		       } // end fat ring condition
		   }
		   
	       }
	}      
	
}//  end of for loop over events in Tree
      
      
    }	   
} // end of macro

















