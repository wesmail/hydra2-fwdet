// ////////////////////////////////////////////////////////////////////////  //
// To plot only one histogram from all dst_pdf.root files available for the  //
// DST generation A.Sadovski 13.Nov.2002                                     //
// ////////////////////////////////////////////////////////////////////////  //

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include "TFile.h"
//#include "TImage.h"

void qa1histohistory(){
  cout << endl;
  cout << "   =========================================================================" << endl;
  cout << "   =                 Thank you for using qa1histohistory.C                 =" << endl;
  cout << "   =                      please use the correct sintax                    =" << endl;
  cout << "   =                                                                       =" << endl;
  cout << "   =                                                                       =" << endl;
  cout << "   =  root -l -n 'qa1histohistory.C(\"Dir\",\"histName\",\"outDir\",sleepTime)'  =" << endl;
  cout << "   =                                                                       =" << endl;
  cout << "   =    where:          [r]-required parameter, [o]-may be ommited         =" << endl;
  cout << "   =    [r] dir       = DST-QA hist.root  files storage dir,               =" << endl;
  cout << "   =    [r] histName  = the name of histogram to be scanned,               =" << endl;
  cout << "   =    [o] outDir    = an output directory to store pictures,             =" << endl;
  cout << "   =    [o] sleepTime = additional delay between hist. update.             =" << endl;
  cout << "   =========================================================================" << endl;
  cout << endl;
 
}

void qa1histohistory(Char_t* DST_HIST_ROOT_fileDir, Char_t* HISTONAME, Char_t* outDir="./", Int_t sleepTime=0 ){

  Char_t  listToFileCommand[300];
  sprintf(listToFileCommand,"ls -l %s/*_dst_hist.root > files.txt",DST_HIST_ROOT_fileDir);
  //gSystem->Exec("ls -l rfiles/*_dst_hist.root > files.txt");
  gSystem->Exec(listToFileCommand);

  Char_t  mkdirCommand[300];
  sprintf(mkdirCommand, "mkdir %s", outDir );
  gSystem->Exec(mkdirCommand);
  sprintf(mkdirCommand, "mkdir %s/pics_%s", outDir, HISTONAME );
  gSystem->Exec(mkdirCommand);

  //Now we open this file
  TString txtFile = "files.txt";

  cout << txtFile << endl;

  //////
  //File wich will contain the list of dst_hist.root files
  //to read one particular histogramm from
  FILE *fSC;
  //fSC = fopen( txtFile , "r");
  fSC = fopen( "files.txt" , "r");

  //////
  //html-file wich will contain links to all pictures (snapshots)
  //from this particular histogram of all dst_hist.root files
  FILE *fHTML;
  fHTML = fopen( "HistogramHistory.html" , "w");
  fprintf(fHTML,"<HTML><BR><H2><CENTER>Histograms history over the QA.root files of DST production</CENTER></H2></BR>");
  fprintf(fHTML,"<H3>DST QA report dir used: %s</H3>", DST_HIST_ROOT_fileDir );
  fprintf(fHTML,"<H3>Histogram scanned over: %s</H3>", HISTONAME );

  //TCanvas cnv;
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptStat(1000000);
  TCanvas cnv("cnv","",0,0,600,400);
  cnv.SetRightMargin(0.135);
  TText   txt;
  Char_t copyCMD[100];

  Int_t iflg = 1;
  Char_t fStringOld[300], fString[300];
  while( fscanf(fSC,"%s", fString) != EOF ){
     cout << "\n" << fString << endl;
     fscanf(fSC,"%s", fString);
     cout << "\n" << fString << endl;
     fscanf(fSC,"%s", fString);
     cout << "\n" << fString << endl;
     fscanf(fSC,"%s", fString);
     cout << "\n" << fString << endl;
     fscanf(fSC,"%s", fString);
     cout << "\n" << fString << endl;
     fscanf(fSC,"%s", fString);
     cout << "\n" << fString << endl;
     fscanf(fSC,"%s", fString);
     cout << "\n" << fString << endl;
     fscanf(fSC,"%s", fString);
     cout << "\n" << fString << endl;
     fscanf(fSC,"%s", fString);
     cout << "\n" << fString << endl;

     //Now we will work under this root file to get histogramm
     TString thisFile(fString);
     cout << thisFile << endl;
     TFile *f1=NULL;
     f1 = new TFile( thisFile, "READONLY", "",1 );

     //TH1F *h = (TH1F*) f1->Get("mdcCal1_t2mt1[1][1]");

     //TH2 * h1;
     //TH1 * h2;
     //TObject *To = (TObject*) f1->Get(HISTONAME);
     //if(To->InheritsFrom("TH2")){
     //  TH2 *h2 = (TH2*) f1->Get(HISTONAME);
     //  h2->GetXaxis()->SetTitleOffset(0.8);
     //  h2->DrawCopy();
     //}else{
     //  TH1 *h1 = (TH1*) f1->Get(HISTONAME);
     //  h1->GetXaxis()->SetTitleOffset(0.8);
     //  h1->DrawCopy();
     //}
     //

     TH1F *h = (TH1F*) f1->Get(HISTONAME);

     h->GetXaxis()->SetTitleOffset(0.8);
     h->DrawCopy();
     thisFile  = thisFile( thisFile.Last('/')+1,thisFile.Last('.')-thisFile.Last('/')-1 );
     txt.DrawTextNDC( 0.2, 0.85, thisFile);
     cnv.Update();

     cnv.Print(0,"gif");
     sprintf(copyCMD,"mv cnv.gif %s/pics_%s/p_%s.gif", outDir, HISTONAME, (Char_t*)thisFile);
     gSystem->Exec(copyCMD);
     fprintf(fHTML,"\n<BR><IMG SRC=\"p_%s.gif\" align=CENTER> <b>%s</b>",(Char_t*)thisFile, (Char_t*)thisFile );

     gSystem->Sleep(sleepTime);
     delete h;
     f1->Close();
     delete f1;
  }
  fprintf(fHTML,"</HTML>");
  fclose(fSC);
  fprintf(fHTML,"<br><i>Thank you for using</i> qa1histohistory.C");
  fclose(fHTML);

  sprintf(mkdirCommand, "mv HistogramHistory.html %s/pics_%s/", outDir, HISTONAME );
  gSystem->Exec(mkdirCommand);
}
