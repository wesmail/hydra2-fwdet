// @(#)root/test:$Name: not supported by cvs2svn $:$Id: hhistory.cxx,v 1.5 2008-09-18 13:13:55 halo Exp $
// Author: Fons Rademakers   07/03/98
// Author: Modified by A.Sadovski FOR HADES-DST-QA

#include <stdlib.h>
#include "stdio.h"
#include "fstream.h"
#include "iostream.h"


#include "TROOT.h"
#include "TApplication.h"
#include "TVirtualX.h"

#include "TGListBox.h"
#include "TGClient.h"
#include "TGFrame.h"
#include "TGIcon.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "TGTextEntry.h"
#include "TGNumberEntry.h"
#include "TGMsgBox.h"
#include "TGMenu.h"
#include "TGCanvas.h"
#include "TGComboBox.h"
#include "TGTab.h"
#include "TGSlider.h"
#include "TGDoubleSlider.h"
#include "TGFileDialog.h"
#include "TGTextEdit.h"
#include "TGShutter.h"
#include "TGProgressBar.h"
#include "TGColorSelect.h"
#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TEnv.h"
#include "TObjString.h"
#include "TFile.h"
#include "TKey.h"
#include "TRegexp.h"
#include "TText.h"
#include "TStyle.h"
#include "TArc.h"
#include "TGraph.h"

#define iMaxHistogramNumber  500 //maximal number of histograms we can read from file
#define iMaxAnalFilesNumber  999 //maximal number of files to can be analised. May be better to use vector here...?

enum ETestCommandIdentifiers {
   M_FILE_OPEN,
   M_FILE_SAVE,
   M_FILE_SAVEAS,
   M_FILE_PRINT,
   M_FILE_PRINTSETUP,
   M_FILE_EXIT,

   M_TEST_DLG,
   M_TEST_MSGBOX,
   M_TEST_SLIDER,
   M_TEST_SHUTTER,
   M_TEST_PROGRESS,
   M_TEST_NUMBERENTRY,
   M_TEST_NEWMENU,

   M_HELP_CONTENTS,
   M_HELP_SEARCH,
   M_HELP_ABOUT,

   M_CASCADE_1,
   M_CASCADE_2,
   M_CASCADE_3,

   M_NEW_REMOVEMENU,

   VId1,
   HId1,
   VId2,
   HId2,

   VSId1,
   HSId1,
   VSId2,
   HSId2,

   ColorSel,

   FILE_LOAD_QaRootDir,
   FILE_LOAD_FileHistMap,
   FILE_SAVE_FileHistMap,
   FILE_LOAD_SUPPRESS_DIR_READ,
   SELECT_QA_ROOT_Histogram,
   SELECT_QA_ROOT_FileNumber,
   SELECT_QA_ROOT_isGoodHistogram,
   SELECT_QA_ROOT_isWarningHistogram,
   SELECT_QA_ROOT_isBadHistogram,
   PRESET_allFilesToWarning,
   PRINT_BadFiles,
   PRINT_OkFiles,
   PRINT_AllFilesStatus,
   SHOW_good_HistFiles,
   SHOW_warn_HistFiles,
   SHOW_bad_HistFiles,
   HIST_PLAYER_Play,
   HIST_PLAYER_Step_f,
   HIST_PLAYER_Pause,
   HIST_PLAYER_Step_b,
   HIST_PLAYER_Stop,
   HIST_PLAYER_DELAY_TIME_Slider,
   SCAN_FOR_MEAN_VALUES,
   SCAN_FOR_MEAN_VALUES_BUT_FROM_X_INTERVAL,
   SCAN_FOR_MEAN_VALUES_SELECT_X_INTERVAL,
   SCAN_FOR_MEAN_VALUES_SELECT,
   SCAN_FOR_MEAN_VALUES_SELECTED_MARK,
   SCAN_FOR_MEAN_VALUES_SELECTED_MARK_PRINT,
   SCAN_FOR_MEAN_VALUES_SELECT_BUT_IGNORE_PREVIOUS_SELECTION,
   SELECT_GOOD_SIGMA_BUTTON
   
};

const char *mapfiletypes[] = { "File/Hist MAP files",    "*.map",
//t.k root gliuchit s refresh  "All files",              "*"    ,
                                0,                        0      };


  //class ListBoxMemoryHelper {
  //  private:
  //    Int_t id[iMaxAnalFilesNumber];
  //    Int_t iPos;
  //  public:
  //    ListBoxMemoryHelper(){
  //      iPos=0;
  //    }
  //   ~ListBoxMemoryHelper(){}
  //
  //    void AddLast(){
  //    }
  //};


  struct Stroka {
    Char_t st[92]; //tipically ROOTclass name
    Char_t tp[30]; //tipically ROOTclass type
  };

  class StArray {
    //Purpose: to keep an array of histogram names, which has to be alive
    //even after the destruction of objects themself

    private:
      static const Int_t maxLetters = 64;
      static const Int_t maxSize  = iMaxHistogramNumber; //maximal number of strings 
      Stroka data[maxSize];
      Int_t iPos;

    public:
       StArray(){
         Init();
       }

      ~StArray(){
       }

       void Init(){
         iPos = 0;         //initial position of array
       }

       void AddLast(Char_t *str, Char_t *typ){
          if(iPos<maxSize){
             //data[iPos].st = str;
             sprintf(data[iPos].st, "%s", str);
             sprintf(data[iPos].tp, "%s", typ);
             //version_1.00// << "StArray::str = " <<data[iPos].st << " type: " << data[iPos].tp << endl;
             iPos++;
          }else{
             //version_1.00// << "Array StArray is overflown!!!!"<< endl;
          }
       }

       Int_t GetSize(){
          return iPos;
       }
 
       Char_t* At(Int_t iP){
          if(iP>=0 && iP<iPos){
            return (Char_t*)data[iP].st;
          }else{
            //version_2.00// << "Stroka:: out of range request!!!" << endl;
            return 0;
          }
       }

       Char_t* TypeAt(Int_t iP){
          if(iP>=0 && iP<iPos){
            return (Char_t*)data[iP].tp;
          }else{
            //version_2.00// << "Stroka:: out of range request!!!" << endl;
            return 0;
          }
       }
  };

//-------------------------------------------------------------------

class MineField {
  private:
    //This is a inner map of good/warning/bad status of file-histogram
    //used to store and show user current status of histogram for any of
    //loaded files. In current realisation is limited by fixed array width
    //which has to be improved in future...?
    short int iFHfm[iMaxAnalFilesNumber][iMaxHistogramNumber];
    //Char_t *fMapFile;
    Char_t fMapFile[200];

  public:
    MineField(){ Init(); }
   ~MineField(){}

    void Init(){
      //fMapFile = "qaFileHisto.map";
      sprintf(fMapFile,"%s","qaFileHisto.map");
      for(   Int_t ix=0; ix<iMaxAnalFilesNumber; ix++){
         for(Int_t iy=0; iy<iMaxHistogramNumber; iy++){
            iFHfm[ix][iy]=0;
         }
      }
    }
   
    void SetMapFileName(Char_t *newMapFileName){
       //fMapFile = newMapFileName;
       sprintf(fMapFile,"%s", newMapFileName);
       printf("  ======== Important ========\n");
       printf("    Selecting  %s\n", fMapFile);
       printf("    to be MAP file for work\n");
       printf("  ===========================\n");
    }

    Int_t GetField(Int_t ix, Int_t iy){ 
      if(ix>=0 && ix<iMaxAnalFilesNumber){
         if(iy>=0 && iy<iMaxHistogramNumber){
           return (int) iFHfm[ix][iy]; 
         }else{
            printHistNumOverflow(iy);
         }
      }else{
         printFileNumOverflow(ix);
      }
      return -1; //error code, here it means overflow...
    }

    void SetField(Int_t ix, Int_t iy, short int iFval){ 
      if(ix>=0 && ix<iMaxAnalFilesNumber){
         if(iy>=0 && iy<iMaxHistogramNumber){
            iFHfm[ix][iy] = iFval; 
         }else{
            printHistNumOverflow(iy);
         }
      }else{
         printFileNumOverflow(ix);
      }
    }

    Bool_t writeFieldIntoFile(){
       Bool_t iOK;
       FILE *fieldFile;
       fieldFile = fopen( (Char_t*)fMapFile , "w");
       //version_1.00// << "Can open file?= "<<(int)fieldFile << endl;
       printf("  ======== Important ========\n");
       printf("    Saving  %s\n", fMapFile);
       printf("    to be MAP file for work\n");
       printf("  ===========================\n");
       if( (int)fieldFile != 0 ){
         //version_1.00// << "qaFileHisto.map Can be opened" << endl;
         fprintf(fieldFile,"%s","FORMAT:[colomns=fileNumber]:[raws=histogramNumber]\n");

         fprintf(fieldFile,"%i\n",iMaxAnalFilesNumber);
         fprintf(fieldFile,"%i\n",iMaxHistogramNumber);

         for(Int_t iy=0; iy<iMaxHistogramNumber; iy++){
         for(Int_t ix=0; ix<iMaxAnalFilesNumber; ix++){
            fprintf(fieldFile,"%i ", iFHfm[ix][iy]);
         }
         fprintf(fieldFile,"\n");
         }
         fclose(fieldFile);
         iOK=kTRUE;
       }else{
         printf("  ============================ Sorry =============================\n");
         printf("  qaFileHisto.map cannot be opened... May be it's write protected?\n");
         printf("  ---------------                                 ---------------\n");
         iOK=kFALSE;
       }
       //delete fieldFile;
       return iOK;
    }

    Bool_t readFieldFromFile(){
       Bool_t iOK;
       int iValue;
       Char_t TitleString[300];
       FILE *fieldFile;

       //version_1.00// << "Loadind map from" << fMapFile << endl;

       fieldFile = fopen( (Char_t*)fMapFile , "r");
       //version_1.00// << "Can open file?= "<<(int)fieldFile << endl;
       if( (int)fieldFile != 0 ){
         ////version_1.00// << fieldFile <<" Can be opened" << endl;
         //printf("%s Can be opened.",fieldFile);
         fscanf(fieldFile,"%s",TitleString); //here we do not need it
         //version_1.00// << "TitleString=" << TitleString << endl;

         Int_t iMaxAnalFilesNumber_loc=0;
         Int_t iMaxHistogramNumber_loc=0;
         fscanf(fieldFile,"%i", &iMaxAnalFilesNumber_loc);
         fscanf(fieldFile,"%i", &iMaxHistogramNumber_loc);
         if(iMaxAnalFilesNumber_loc != iMaxAnalFilesNumber || iMaxHistogramNumber_loc != iMaxHistogramNumber_loc){
            printf("  ============================ Sorry ======================\n");
            printf("  inner program iMaxAnalFilesNumber or iMaxHistogramNumber \n");
            printf("  is different form the value in a map file                \n");
            printf("  hhistory: iMaxAnalFilesNumber = %i\n", iMaxAnalFilesNumber  );
            printf("  map-file: iMaxAnalFilesNumber = %i\n", iMaxAnalFilesNumber_loc);
            printf("  hhistory: iMaxHistogramNumber = %i\n", iMaxHistogramNumber  );
            printf("  map-file: iMaxHistogramNumber = %i\n", iMaxHistogramNumber_loc);
            printf("  =========================================================\n");
            printf("   You would better use corresponding version of hhistory  \n");
            printf("  =========================================================\n");
            iOK=kFALSE;
         }
         
         for(Int_t iy=0; iy<iMaxHistogramNumber; iy++){
         for(Int_t ix=0; ix<iMaxAnalFilesNumber; ix++){
            fscanf(fieldFile,"%i", &iValue);
            //if(iy==0){if(ix<5){
            //  //version_2.00// <<" iValue= "<<iValue << endl;
            //}}
            iFHfm[ix][iy] = iValue;
            //
            //if(iy==0){if(ix<5){cout <<" val= "<<iFHfm[ix][iy] << endl;}}
            //
         }
         //sprintf(fieldFile,"\n");
         }
         fclose(fieldFile);
         iOK=kTRUE;
       }else{
         printf("  =========================== Comment ============================\n");
         printf("  New project: %s does not exist or read protected...\n", fMapFile);
         printf("  ================================================================\n");
         iOK=kFALSE;
       }
       //delete fieldFile;
       return iOK;
    }


    void printFileNumOverflow(Int_t ix){
       printf("   *--- Significant Problem: ----------------------*\n");
       printf("   |-                                             -|\n");
       printf("   |- Impossible  to  adress  so  many  files     -|\n");
       printf("   |- due to insufficient of memory allocated     -|\n");
       printf("   |- for the FILE .vs. HISTOGRAMM   field...     -|\n");
       printf("   |- Please contact A.Sadovski@fz-rossendorf.de  -|\n");
       printf("   |- or correct the value of iMaxAnalFilesNumber -|\n");
       printf("   *-----------------------------------------------*\n");
       printf("    program assume:  iMaxAnalFilesNumber < %i\n", iMaxAnalFilesNumber);
       printf("    But You are trying to read %i files !\n", ix);
    }

    void printHistNumOverflow(Int_t iy){
       printf("   *--- Significant Problem: ----------------------*\n");
       printf("   |-                                             -|\n");
       printf("   |- Impossible to adress so many histograms     -|\n");
       printf("   |- due to insufficient of memory allocated     -|\n");
       printf("   |- for the FILE .vs. HISTOGRAMM   field...     -|\n");
       printf("   |- Please contact A.Sadovski@fz-rossendorf.de  -|\n");
       printf("   |- or correct the value of iMaxHistogramNumber -|\n");
       printf("   *-----------------------------------------------*\n");
       printf("    program assume:  iMaxHistogramNumber < %i\n", iMaxHistogramNumber);
       printf("    But You are trying to read %i histograms !\n", iy);
    }

};


//-------------------------------------------------------------------

class TestMainFrame : public TGMainFrame {

private:
   //TGCompositeFrame   *fStatusFrame;
   //asd2//TGCanvas           *fCanvasWindow;

   TGMenuBar          *fMenuBar;
   TGPopupMenu        *fMenuHelp;
   TGLayoutHints      *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;

   //-asd--//
   TGLayoutHints *fLayout;
   TGTextButton  *fButtonFHgood, *fButtonFHbad, *fButtonFHwarn; //file-histogramm good, warning and bad marker button
   TGTextButton  *fButton1, *fButton2, *fButton3, *fButton4, *fButton5;
   TGButton      *fOkButton, *fCancelButton, *fStartB, *fStepfB, *fPauseB, *fStepbB, *fStopB;
   TGLabel       *fSliderLabel;
   TGLabel       *fHistosLabel, *frFilesLabel;
   TGHSlider     *fHslider1;

   //
   //TGCompositeFrame    *fContainer;
   TGCompositeFrame    *fFrame1, *fF1, *fF2, *fF3, *fF5, *fF6, *fF6H, *fF6V, *fF6R, *fF7;
   TGGroupFrame    *fF4A;  //, *fF4B;
   TGCompositeFrame *fF4btn1, *fF4btn2, *fF4btn3, *fF4btn4;
   TGGroupFrame        *fF4, *fF4F;
   TGLabel             *fBtnMarkAllAsWarningLabel;
   TGButton            *fBtnMarkAllAsWarning;
   TGLabel             *fBtnPrintBadFilesLabel;
   TGButton            *fBtnPrintBadFiles;
   TGCheckButton       *fBtnShowGood, *fBtnShowWarn, *fBtnShowBad, *fBtnIgnorePreviousSelection;

   TGLayoutHints       *fL1, *fL2, *fL3, *fL4, *fL4A, *fLmXcmp;
   TGTab               *fTab;   
   TGTextEntry         *fTxt1, *fTxt2, *fTxt3, *tXgoodSigma, *tXwarnSigma, *teXminVal, *teXmaxVal;
   TGButton            *fBtn1, *fBtn2, *fChk1, *fChk2, *fRad1, *fRad2;
   TGComboBox          *fCombo;
   TGComboBox          *fFileCombo;
   TGCheckButton       *fCheckMulti;
   TGListBox           *fListBox;
   TGListBox           *fListBoxFile;

   TRootEmbeddedCanvas *fEc1, *fEc2, *fColoredPlatz;
   Int_t                fFirstEntry;  //for histogramm
   Int_t                fLastEntry;   //for histogramm
   Int_t                fFirstEntryFile; //for fileNumber
   Int_t                fLastEntryFile;  //for fileNumber
   Bool_t               fFillHistos;
   TH1F                *fHpx;
   TH2F                *fHpxpy;
   TList               *fCleanup;
   TH1F                *hScannedMeanValue;
   TH1F                *hLocalScannedMeanValue;
   TH1F                *hMeanValueLine; //histogram which shows only the mean value for all scanned histograms
   TH1F                *hLocalMeanValueLine;
   TH1F                *hSelectedHistogramsUsingMeanValue;

   //Some date for ROOT applications
   Bool_t FileSuppresDirRead;
   Bool_t SCAN_FOR_MEAN_VALUES_SELECTED_MARK_PRINT_justPressed;
   Int_t iFL; //root file enumerator
   const Char_t* DST_HIST_ROOT_fileDir;
   const Char_t* DST_HIST_ROOT_fileSpecification;
   TList*  rootFileName;
   StArray histograName;

   Int_t  selectedHistogram_num;
   Bool_t selectedHistogram_num_wasSelected;
   Char_t selectedHistogram[100];
   Char_t selectedHistogramType[100];
   Int_t  iLastScannedForMeanValueHistogram; //to store the last scanned histogram name and use it in "MARK" and "SELECT" operations
   Int_t iFileCurrPos;  //to remember the current position of the displayed file 
                        //-- if button continue will come to strart not from very 
                        //begining but from the current position

   Int_t fixedUpdateRate;         //should be fixed (constant) tipical rate
   Int_t tUpdateHistoDelayTime;   //rate adjustement

   MineField fhmf; //[iMaxAnalFilesNumber][iMaxHistogramNumber] good/warning/bad flag storage

   Bool_t bShowHF_g, bShowHF_w, bShowHF_b, bIgnorePrevSelection, bSelectFromSubinterval;
   ULong_t green, yellow, blue, red, gray;


public:
   TestMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~TestMainFrame();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);

   void   placeRootFileNamesIntoListBox();
   void replaceRootFileNamesIntoListBox();

   void PlaySelectedHistoAlongTheFileNumber(Int_t ind);
   void ScanForMeanValueForSelectedHistoAlongTheFileNumber(Int_t ind);
   void SelectFromMeanValueForSelectedHistoAlongTheFileNumber();
   void MarkAndCommentButton(Char_t* msg);
   void saveThisConfiguration();
   void indicateByColorLight();
   void createTheMeanValueHistogram(); 
   Double_t getMeanValueFromSubinterval(TH1 *hist, Double_t xMin, Double_t xMax);
   void showSubintervalForSelectedHistogram();
   void writeBadFilesForTheHistogramToFile();
   void writeOkFilesForTheHistogramToFile();
   void writeAllFilesStatusForTheHistogramToFile();
};


TestMainFrame::TestMainFrame(const TGWindow *p, UInt_t w, UInt_t h)
      : TGMainFrame(p, w, h)
{

   //Working class variables initialisation
   FileSuppresDirRead = kFALSE;
   SCAN_FOR_MEAN_VALUES_SELECTED_MARK_PRINT_justPressed = kFALSE;
   rootFileName = new TList();
   iFileCurrPos = 0;
   selectedHistogram_num=0;
   selectedHistogram_num_wasSelected=kFALSE;
   fixedUpdateRate = 40000; //fixed
   tUpdateHistoDelayTime = 100000;
   bShowHF_g=kTRUE; //
   bShowHF_w=kTRUE; // Initial settings of checkBox show all histograms
   bShowHF_b=kTRUE; //
   bIgnorePrevSelection = kTRUE;
   bSelectFromSubinterval = kFALSE;
   hLocalScannedMeanValue=0;
   hLocalMeanValueLine=0;
   hSelectedHistogramsUsingMeanValue=0;
   //End of Working class variables init...

   //Colors initialisation:
   fClient->GetColorByName("yellow", yellow);
   fClient->GetColorByName("green" , green);
   fClient->GetColorByName("red"   , red);
   fClient->GetColorByName("blue"  , blue);
   fClient->GetColorByName("gray"  , gray);


   //asd2//fCanvasWindow = new TGCanvas(this, 600, 240);
   //asd2//fCanvasWindow->Resize(200,100);
   //asd3//fContainer = new TGCompositeFrame(this, 60, 20, kHorizontalFrame | kSunkenFrame); 
   //asd2//fCanvasWindow->SetContainer(fContainer);

   // Used to store GUI elements that need to be deleted in the ctor.
   fCleanup = new TList;

   //// Create status frame containing a button and a text entry widget
   ////fStatusFrame = new TGCompositeFrame(this, 60, 20, kHorizontalFrame | kSunkenFrame);
   //fStatusFrame = new TGHorizontalFrame(this, 160, 20, kFixedWidth);

   //fOkButton = new TGTextButton(fStatusFrame, "&Ok", 1);
   //fOkButton->Associate(this);
   //fCancelButton = new TGTextButton(fStatusFrame, "&Cancel", 2);
   //fCancelButton->Associate(this);

   //fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2, 2, 2);
   //fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);

   //fCanvasWindow->AddFrame(fOkButton, fL1);
   //fCanvasWindow->AddFrame(fCancelButton, fL1);

   //fCanvasWindow->Resize(150, fOkButton->GetDefaultHeight());
   //AddFrame(fCanvasWindow, fL2);

   //--------- create Tab widget and some composite frames for Tab testing

   fTab = new TGTab(this, 300, 300);
   fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2);

   TGCompositeFrame *tf;
   //?
   tf = new TGCompositeFrame(fClient->GetRoot(), 500, 300, kVerticalFrame);
   //?

   //###### Tab 1 ######//
   TGTextButton *bt;
   TGTextButton *btOpenFHfieldFile;
   TGTextButton *btSaveFHfieldFile;

   tf = fTab->AddTab("LoadDir...");
   fF1 = new TGCompositeFrame(tf, 60, 40, kVerticalFrame);
   fF1->AddFrame(fTxt1 = new TGTextEntry(fF1, new TGTextBuffer(300)), fL3);
   fF1->AddFrame(fTxt2 = new TGTextEntry(fF1, new TGTextBuffer(100)), fL3);
   fF1->AddFrame(bt = new TGTextButton(fF1, "&LOAD ROOT FILES", FILE_LOAD_QaRootDir), fL3);
   bt->Associate(this);

   fF1->AddFrame(fChk1 = new TGCheckButton(fF1, "Freeze the List of files", FILE_LOAD_SUPPRESS_DIR_READ), fL3);
   fChk1->Associate(this);

   fF1->AddFrame(fTxt3 = new TGTextEntry(fF1, new TGTextBuffer(100)), fL3);
   fF1->AddFrame(btOpenFHfieldFile = new TGTextButton(fF1, "LOAD &MAP FILE", FILE_LOAD_FileHistMap), fL3);
   btOpenFHfieldFile->Associate(this);

   fF1->AddFrame(btSaveFHfieldFile = new TGTextButton(fF1, "SAVE MAP FILE", FILE_SAVE_FileHistMap), fL3);
   btSaveFHfieldFile->Associate(this);

   Char_t fString[300];
   FILE *CFGfile;
   CFGfile = fopen( "qaSetup.cfg" , "r");
   Int_t iCanOpenFile = (int)CFGfile;
   ////version_1.00// << iCanOpenFile << endl;
   if( iCanOpenFile != 0 ){
     //version_1.00// << "qaSetup.cfg Can be opened" << endl;
     fscanf(CFGfile,"%s", fString);
     fTxt1->SetText(fString);
     fscanf(CFGfile,"%s", fString);
     fTxt2->SetText(fString);
     fscanf(CFGfile,"%s", fString);
     fTxt3->SetText(fString);
     fclose(CFGfile);
   }else{
     //version_1.00// << "qaSetup.cfg absent, we'll create one..." << endl;
   }

   tf->AddFrame(fF1, fL3);
   fTxt1->Resize(400, fTxt1->GetDefaultHeight());
   fTxt2->Resize(400, fTxt2->GetDefaultHeight());
   fTxt3->Resize(200, fTxt3->GetDefaultHeight());

   //######### TAB 4 #########//
   tf = fTab->AddTab("Hi/File");
   tf->SetLayoutManager(new TGHorizontalLayout(tf));

   //fF4B = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   //Char_t  buff_HistosTitle[100], buff_rFilesTitle[100];
   //sprintf(buff_HistosTitle,"%s" ,"  Histogram Names                                       ");
   //sprintf(buff_rFilesTitle,"%s" ,"  ROOT files List");
   //fHistosLabel = new TGLabel( fF4B, buff_HistosTitle );
   //frFilesLabel = new TGLabel( fF4B, buff_rFilesTitle );
   //fF4B->AddFrame(fHistosLabel, fL3 );
   //fF4B->AddFrame(frFilesLabel, fL3 );
   //tf->AddFrame(fF4B, fL3);   

   fF4 = new TGGroupFrame(tf, "Histograms", kVerticalFrame);
   //fF4 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   //fF4->AddFrame(bt = new TGTextButton(fF4, "&Bad", SELECT_QA_ROOT_isBadHistogram), fL3);
   //bt->Associate(this);
   //fCleanup->Add(bt);
   //// fF4->AddFrame(bt = new TGTextButton(fF4, "Remove &Entry", 91), fL3);
   //// bt->Associate(this);
   //// fCleanup->Add(bt);
   ////////////////////////////////////
   //Here we add new List of histograms
   //
   fF4->AddFrame(fListBox = new TGListBox(fF4, SELECT_QA_ROOT_Histogram), fL3);
   fListBox->Associate(this);

   //fF4->AddFrame(fCheckMulti = new TGCheckButton(fF4, "&Mutli Selectable", 92), fL3);
   //fCheckMulti->Associate(this);

   fF4btn1 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   fF4btn1->AddFrame(fBtnMarkAllAsWarning = new TGTextButton(fF4btn1, " ", PRESET_allFilesToWarning), fL4);
   fBtnMarkAllAsWarning->Associate(this);
   fBtnMarkAllAsWarningLabel = new TGLabel( fF4btn1, "Preset all files for the histogram to warning" );
   fF4btn1->AddFrame(fBtnMarkAllAsWarningLabel, fL3 );
   fF4->AddFrame(fF4btn1, fL3);

   fF4btn2 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   fF4btn2->AddFrame(fBtnPrintBadFiles = new TGTextButton(fF4btn2, " ", PRINT_BadFiles), fL4);
   fBtnPrintBadFiles->Associate(this);
   fBtnPrintBadFilesLabel = new TGLabel( fF4btn2, "Print bad files for the histogram" );
   fF4btn2->AddFrame(fBtnPrintBadFilesLabel, fL3 );
   fF4->AddFrame(fF4btn2, fL3);

   fF4btn3 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   fF4btn3->AddFrame(fBtnPrintBadFiles = new TGTextButton(fF4btn3, " ", PRINT_OkFiles), fL4);
   fBtnPrintBadFiles->Associate(this);
   fBtnPrintBadFilesLabel = new TGLabel( fF4btn3, "Print good&warning files for the histogram" );
   fF4btn3->AddFrame(fBtnPrintBadFilesLabel, fL3 );
   fF4->AddFrame(fF4btn3, fL3);

   fF4btn4 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   fF4btn4->AddFrame(fBtnPrintBadFiles = new TGTextButton(fF4btn4, " ", PRINT_AllFilesStatus), fL4);
   fBtnPrintBadFiles->Associate(this);
   fBtnPrintBadFilesLabel = new TGLabel( fF4btn4, "Print files-status for the histogram" );
   fF4btn4->AddFrame(fBtnPrintBadFilesLabel, fL3 );
   fF4->AddFrame(fF4btn4, fL3);


   tf->AddFrame(fF4, fL3);
   fFirstEntry = 0;
   fLastEntry  = 0;
   ////----------------------------------------------------------
   ////here we take all histograms from already created TList
   ////version_1.00// << "take all histograms afrom the TList" << endl;
   ////version_1.00// << "Nhistograms = " << histograName->GetSize() << endl;
   //for(Int_t i=0; i<histograName->GetSize(); i++){
   //   //version_1.00// << "histogramm TList: " << ((TObjString*) histograName->At(i))->GetString() << endl;
   //   fListBox->AddEntry(((TObjString*) histograName->At(i))->GetString(), i);
   //}
   //fFirstEntry = 0;
   //fLastEntry  = histograName->GetSize();
   ////version_1.00// << "end of taking and menu forming" << endl;
   ////end of checking if all histograms are settled in the TList
   ////----------------------------------------------------------
   fListBox->Resize(290, 410);


   fF4F = new TGGroupFrame(tf, "Files", kVerticalFrame);
   tf->AddFrame(fF4F, fL3);
   ////////////////////////////////////////////////////////////////////////////
   //Here we add list of root-files to allow posiibility to start from each one
   //
   fF4F->AddFrame(fListBoxFile = new TGListBox(fF4F, SELECT_QA_ROOT_FileNumber), fL3);
   //for (Int_t i=0; i < 20; ++i) {
   //   char tmp[20];
   //   sprintf(tmp, "FileNane_xx0009999000199%i.RoOt", i+1);
   //   fListBoxFile->AddEntry(tmp, i+1);
   //}
   fFirstEntryFile = 1;
   fLastEntryFile  = 0;
   fListBoxFile->Associate(this);
   fListBoxFile->Resize(320, 450);
   fF4F->AddFrame(fBtnShowGood = new TGCheckButton(fF4F, "Show good files for the histogram", SHOW_good_HistFiles), fL3);
   fF4F->AddFrame(fBtnShowWarn = new TGCheckButton(fF4F, "Show ???? files for the histogram", SHOW_warn_HistFiles), fL3);
   fF4F->AddFrame(fBtnShowBad  = new TGCheckButton(fF4F, "Show bad  files for the histogram", SHOW_bad_HistFiles ), fL3);
   fBtnShowGood->SetState( (EButtonState) 1);
   fBtnShowWarn->SetState( (EButtonState) 1);
   fBtnShowBad->SetState(  (EButtonState) 1);
   fBtnShowGood->Associate(this);   
   fBtnShowWarn->Associate(this);
   fBtnShowBad->Associate(this);
   
   //fF4A = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   //fF4A->AddFrame(fButtonFHbad = new TGTextButton(fF4A, "BadHst", SELECT_QA_ROOT_isBadHistogram), fL3);
   //fButtonFHbad->Associate(this);
   //fF4A->AddFrame(fButtonFHwarn = new TGTextButton(fF4A, "Warning", SELECT_QA_ROOT_isWarningHistogram), fL3);
   //fButtonFHwarn->Associate(this);
   //tf->AddFrame(fF4A, fL3);

   //####### end TAB 4 ########//



   //######### TAB 3 ###########//
   //-------------- embedded canvas demo
   fFillHistos = kFALSE;
   fHpx   = 0;
   fHpxpy = 0;
   tf = fTab->AddTab("HH-Player");
   fF3 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   fStartB = new TGTextButton(fF3, "&Play", HIST_PLAYER_Play);
   fStepbB = new TGTextButton(fF3, "<&b<"   , HIST_PLAYER_Step_b);
   fPauseB = new TGTextButton(fF3, "&||"  , HIST_PLAYER_Pause);
   fStepfB = new TGTextButton(fF3, ">&f>"   , HIST_PLAYER_Step_f);
   fStopB  = new TGTextButton(fF3, "&Stop", HIST_PLAYER_Stop);
   fStartB->Associate(this);
   fStepbB->Associate(this);
   fPauseB->Associate(this);
   fStepfB->Associate(this);
   fStopB->Associate(this);
   fF3->AddFrame(fStartB, fL3);
   fF3->AddFrame(fStepbB, fL3);
   fF3->AddFrame(fPauseB, fL3);
   fF3->AddFrame(fStepfB, fL3);
   fF3->AddFrame(fStopB , fL3);

   Char_t buff[30];
   sprintf(buff,"%s" ,"    Delay");
   fSliderLabel = new TGLabel( fF3, buff );
   fF3->AddFrame(fSliderLabel, fL3 );

   fHslider1 = new TGHSlider(fF3, 150, kSlider1 | kScaleBoth, HIST_PLAYER_DELAY_TIME_Slider);
   fHslider1->Associate(this);
   fHslider1->SetPosition(Int_t(tUpdateHistoDelayTime/fixedUpdateRate));
   fHslider1->SetRange(1,20);
   fF3->AddFrame(fHslider1,fL3);
   fF3->AddFrame(fButtonFHgood = new TGTextButton(fF3, "&Good", SELECT_QA_ROOT_isGoodHistogram)   , fL3);
   fF3->AddFrame(fButtonFHwarn = new TGTextButton(fF3, "&???" , SELECT_QA_ROOT_isWarningHistogram), fL3);
   fF3->AddFrame(fButtonFHbad  = new TGTextButton(fF3, "Ba&D" , SELECT_QA_ROOT_isBadHistogram)    , fL3);
   fButtonFHgood->Associate(this);
   fButtonFHwarn->Associate(this);
   fButtonFHbad->Associate( this);
   // paint good button into green
   //ULong_t green;
   fClient->GetColorByName("lightgreen", green);
   fButtonFHgood->ChangeBackground(green);

   // paint warning button into yellow
   //ULong_t yellow;
   fClient->GetColorByName("Orange", yellow);
   fButtonFHwarn->ChangeBackground(yellow);

   // paint warning button into red
   //ULong_t red;
   fClient->GetColorByName("Magenta", red);
   fButtonFHbad->ChangeBackground(red);

   fColoredPlatz = new TRootEmbeddedCanvas("ColoredPlatz", fF3, 25, 25);
   fColoredPlatz->GetCanvas()->SetBorderMode(0);

   fF3->AddFrame(fColoredPlatz , fL3); //just will indicat the color
   fF3->AddFrame(fButtonFHgood , fL3);
   fF3->AddFrame(fButtonFHwarn , fL3);
   fF3->AddFrame(fButtonFHbad  , fL3);


   fF5 = new TGCompositeFrame(tf, 60, 60, kHorizontalFrame);

   fL4 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX |
                           kLHintsExpandY, 5, 5, 5, 5);
   fEc1 = new TRootEmbeddedCanvas("ec1", fF5, 500, 500);
   fF5->AddFrame(fEc1, fL4);
   //fEc2 = new TRootEmbeddedCanvas("ec2", fF5, 100, 100);
   //fF5->AddFrame(fEc2, fL4);

   tf->AddFrame(fF3, fL3);
   tf->AddFrame(fF5, fL4);

   fEc1->GetCanvas()->SetBorderMode(0);
   //fEc2->GetCanvas()->SetBorderMode(0);

   // make tab yellow
   //ULong_t yellow;
   fClient->GetColorByName("yellow", yellow);
   TGTabElement *tabel = fTab->GetTabTab(2);
   tabel->ChangeBackground(yellow);
   //-------------- end embedded canvas demo
   //####### end TAB 3 ##########//



   //###### Begin of TAB <mean value> comparator #######
   tf = fTab->AddTab("<x>cmp");
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2);
   //tf->SetLayoutManager(new TGHorizontalLayout(tf));
   tf->SetLayoutManager(new TGVerticalLayout(tf));
   fF6 = new TGGroupFrame(tf, "Cut options", kVerticalFrame);
   //tf->AddFrame(fF6, fL1);

   // 3 column, 0...n rows
   fF6->SetLayoutManager(new TGMatrixLayout(fF6, 0, 6, 4, 4));
   fF6->AddFrame(bt = new TGTextButton(fF6, "S&CAN", SCAN_FOR_MEAN_VALUES), fL1);
   bt->Associate(this);

   //fF6->AddFrame(new TGLabel(fF6, " ") );
   fF6->AddFrame(fBtnIgnorePreviousSelection = new TGCheckButton(fF6, "From", SCAN_FOR_MEAN_VALUES_BUT_FROM_X_INTERVAL), fL1);
   fBtnIgnorePreviousSelection->SetState(  (EButtonState) 0);
   fBtnIgnorePreviousSelection->Associate(this);   
   //fF6->AddFrame(new TGLabel(fF6, "From X -") );
   fF6->AddFrame(new TGLabel(fF6, "interval") );

   TGTextBuffer *tXminValBuff = new TGTextBuffer(5);
   tXminValBuff->AddText(0, "0.0");

   teXminVal = new TGTextEntry(fF6, tXminValBuff, SCAN_FOR_MEAN_VALUES_SELECT_X_INTERVAL);
   teXminVal->Associate(this);
   teXminVal->Resize(45, teXminVal->GetDefaultHeight());
   teXminVal->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
   fCleanup->Add(teXminVal);
   fF6->AddFrame(teXminVal);

   fF6->AddFrame(new TGLabel(fF6, "< xVal <") );

   TGTextBuffer *tXmaxValBuff = new TGTextBuffer(5);
   tXmaxValBuff->AddText(0, ".999");
   teXmaxVal = new TGTextEntry(fF6, tXmaxValBuff, SCAN_FOR_MEAN_VALUES_SELECT_X_INTERVAL);
   teXmaxVal->Associate(this);
   teXmaxVal->Resize(45, teXmaxVal->GetDefaultHeight());
   teXmaxVal->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
   fCleanup->Add(teXmaxVal);
   fF6->AddFrame(teXmaxVal);
   fF6->Resize(fF6->GetDefaultSize());
   //
   //
   fF6->Resize(fF6->GetDefaultSize());


   //
   fF6H = new TGGroupFrame(tf, "Quality selection intervals in Sigma units", kVerticalFrame);
   fF6H->SetLayoutManager(new TGMatrixLayout(fF6H, 0, 6, 4, 4));
   //fF6H->SetLayoutManager(new TGHorizontalLayout(fF6H));
   //
   fF6H->AddFrame(new TGLabel(fF6H, "  good < ") );
   TGTextBuffer *tXgoodSigmaBuff = new TGTextBuffer(5);
   tXgoodSigmaBuff->AddText(0, "1.0");
   //fF6H->AddFrame(tXgoodSigma = new TGTextEntry(fF6H, tXgoodSigmaBuff, SELECT_GOOD_SIGMA_BUTTON));
   tXgoodSigma = new TGTextEntry(fF6H, tXgoodSigmaBuff, SELECT_GOOD_SIGMA_BUTTON);
   tXgoodSigma->Associate(this);
   tXgoodSigma->Resize(45, tXgoodSigma->GetDefaultHeight());
   tXgoodSigma->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
   fCleanup->Add(tXgoodSigma);
   fF6H->AddFrame(tXgoodSigma, fL3);
   //
   fF6H->AddFrame(new TGLabel(fF6H, "< warn < ") );
   //
   TGTextBuffer *tXwarnSigmaBuff = new TGTextBuffer(5);
   tXwarnSigmaBuff->AddText(0, "2.0");
   //TGTextEntry  *tXwarnSigma = new TGTextEntry(fF6H, tXwarnSigmaBuff);
   tXwarnSigma = new TGTextEntry(fF6H, tXwarnSigmaBuff, SELECT_GOOD_SIGMA_BUTTON);
   tXwarnSigma->Associate(this);
   tXwarnSigma->Resize(45, tXwarnSigma->GetDefaultHeight());
   tXwarnSigma->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
   fCleanup->Add(tXwarnSigma);
   fF6H->AddFrame(tXwarnSigma, fL3);
   //
   fF6H->AddFrame(new TGLabel(fF6H, "<   bad") );
   fF6H->Resize(fF6H->GetDefaultSize());



   //fF6V = new TGCompositeFrame(tf, 60, 40, kVerticalFrame);
   fF6V = new TGCompositeFrame(tf, 60, 40, kHorizontalFrame);
   fF6V->AddFrame(bt = new TGTextButton(fF6V, "SELEC&T", SCAN_FOR_MEAN_VALUES_SELECT), fL1);
   bt->Associate(this);
   //
   fF6V->AddFrame(new TGLabel(fF6V, "  ") );
   fF6V->AddFrame(fBtnIgnorePreviousSelection = new TGCheckButton(fF6V, "Ignore previous selection ", SCAN_FOR_MEAN_VALUES_SELECT_BUT_IGNORE_PREVIOUS_SELECTION), fL1);
   //
   fF6V->AddFrame(bt = new TGTextButton(fF6V, "MAR&K as Selected", SCAN_FOR_MEAN_VALUES_SELECTED_MARK), fL1);
   bt->Associate(this);
   fBtnIgnorePreviousSelection->SetState( (EButtonState) 1);
   fBtnIgnorePreviousSelection->Associate(this);

   fF6V->AddFrame(bt = new TGTextButton(fF6V, "Print selection", SCAN_FOR_MEAN_VALUES_SELECTED_MARK_PRINT), fL1);
   bt->Associate(this);



   fF6R = new TGCompositeFrame(tf, 60, 60, kVerticalFrame);
   fLmXcmp = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);
   fEc2 = new TRootEmbeddedCanvas("ec2", fF6R, 500, 200);
   fF6R->AddFrame(fEc2, fLmXcmp);
   tf->AddFrame(fF6, fL1);
   tf->AddFrame(fF6H, fL1);
   tf->AddFrame(fF6V, fL1);
   tf->AddFrame(fF6R, fLmXcmp);
   fEc2->GetCanvas()->SetBorderMode(0);
   
   //####### End of TAB <mean value> comparator ########


   ////###### Tab 2 ######//
   //tf = fTab->AddTab("Tab2");
   //fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 200, 2, 2, 2);
   //fF2 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   //fF2->AddFrame(fBtn1 = new TGTextButton(fF2, "&Button 1", 0), fL1);
   //fF2->AddFrame(fBtn2 = new TGTextButton(fF2, "B&utton 2", 0), fL1);
   //fF2->AddFrame(fChk1 = new TGCheckButton(fF2, "C&heck 1", 0), fL1);
   //fF2->AddFrame(fChk2 = new TGCheckButton(fF2, "Chec&k 2", 0), fL1);
   //fF2->AddFrame(fRad1 = new TGRadioButton(fF2, "&Radio 1", 81), fL1);
   //fF2->AddFrame(fRad2 = new TGRadioButton(fF2, "R&adio 2", 82), fL1);
   //fCombo = new TGComboBox(fF2, 88);
   //fF2->AddFrame(fCombo, fL3);
   //tf->AddFrame(fF2, fL3);
   //int i;
   //for (i = 0; i < 20; i++) {
   //   char tmp[20];
   //
   //   sprintf(tmp, "Entry %i", i+1);
   //   fCombo->AddEntry(tmp, i+1);
   //}
   //fCombo->Resize(150, 20);
   //fBtn1->Associate(this);
   //fBtn2->Associate(this);
   //fChk1->Associate(this);
   //fChk2->Associate(this);
   //fRad1->Associate(this);
   //fRad2->Associate(this);
   ////######## end TAB 2 ########//
   //
   //
   //
   ////########### tab 5 ##########
   //tf = fTab->AddTab("Tab 5");
   //tf->SetLayoutManager(new TGHorizontalLayout(tf));
   //
   //fF6 = new TGGroupFrame(tf, "Options", kVerticalFrame);
   //tf->AddFrame(fF6, fL3);
   //
   //// 2 column, n rows
   //fF6->SetLayoutManager(new TGMatrixLayout(fF6, 0, 2, 10));
   //char buff[100];
   //int j;
   //for (j = 0; j < 4; j++) {
   //   sprintf(buff, "Module %i", j+1);
   //   fF6->AddFrame(new TGLabel(fF6, new TGHotString(buff)));
   //
   //   TGTextBuffer *tbuf = new TGTextBuffer(10);
   //   tbuf->AddText(0, "0.0");
   //
   //   TGTextEntry  *tent = new TGTextEntry(fF6, tbuf);
   //   tent->Resize(50, tent->GetDefaultHeight());
   //   tent->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
   //   fCleanup->Add(tent);
   //   fF6->AddFrame(tent);
   //}
   //fF6->Resize(fF6->GetDefaultSize());
   //
   //// another matrix with text and buttons
   //fF7 = new TGGroupFrame(tf, "Tab Handling", kVerticalFrame);
   //tf->AddFrame(fF7, fL3);
   //
   //fF7->SetLayoutManager(new TGMatrixLayout(fF7, 0, 1, 10));
   //
   //fF7->AddFrame(bt = new TGTextButton(fF7, "Remove Tab", 101));
   //bt->Associate(this);
   //bt->Resize(90, bt->GetDefaultHeight());
   //fCleanup->Add(bt);
   //
   //fF7->AddFrame(bt = new TGTextButton(fF7, "Add Tab", 103));
   //bt->Associate(this);
   //bt->Resize(90, bt->GetDefaultHeight());
   //fCleanup->Add(bt);
   //
   //fF7->AddFrame(bt = new TGTextButton(fF7, "Remove Tab 5", 102));
   //bt->Associate(this);
   //bt->Resize(90, bt->GetDefaultHeight());
   //fCleanup->Add(bt);
   //
   //fF7->Resize(fF6->GetDefaultSize());
   //
   ////--- end of last tab
   
   TGLayoutHints *fL5 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
                                          kLHintsExpandY, 2, 2, 5, 1);
   AddFrame(fTab, fL5);

   MapSubwindows();
   Resize(GetDefaultSize());

   SetWindowName("DST QA-viewer");
   SetIconName("DST QA-viewer");

   //MapSubwindows();


   MapWindow();
}

TestMainFrame::~TestMainFrame()
{
   // Delete all created widgets.

   //delete fStatusFrame;
   //asd2//delete fCanvasWindow;

   delete fMenuBarLayout;
   delete fMenuBarItemLayout;
   delete fMenuBarHelpLayout;

   delete fMenuBar;
   delete fMenuHelp;
}

void TestMainFrame::CloseWindow()
{
   // Got close message for this MainFrame. Terminate the application
   // or returns from the TApplication event loop (depending on the
   // argument specified in TApplication::Run()).

   gApplication->Terminate(0);
}

Bool_t TestMainFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Handle messages send to the TestMainFrame object. E.g. all menu button
   // messages.
   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         ////version_1.00// << "Command" << endl;
         switch (GET_SUBMSG(msg)) {

            case kCM_TAB:
              //printf("Button was pressed, id = %ld\n", parm1);
              if(parm1==2){
                ////we need to plot current histogram
                /////
                ////version_1.00// << "hist="<<selectedHistogram_num<<" F="<<iFileCurrPos <<endl;
                ////
                //// Taken from  HIST_PLAYER_Step_b button as it does identical business
                //fFillHistos = kTRUE;
                //PlaySelectedHistoAlongTheFileNumber(-1);
                //fFillHistos = kFALSE;
                ////
                ////
              }
              break;

            case kCM_BUTTON:
              //printf("Button was pressed, id = %ld\n", parm1);
                switch(parm1){

                  case FILE_LOAD_QaRootDir:
                    //version_1.00// << "FILE_LOAD_QaRootDir" << endl;
                    ////version_1.00// << fTxt1->GetText() << endl;
                    DST_HIST_ROOT_fileDir = fTxt1->GetText();
                    //version_1.00// << DST_HIST_ROOT_fileDir << endl;

                    ////version_1.00// << fTxt2->GetText() << endl;
                    DST_HIST_ROOT_fileSpecification = fTxt2->GetText();
                    //version_1.00// << DST_HIST_ROOT_fileSpecification << endl;

                    fhmf.SetMapFileName((Char_t*) fTxt3->GetText());

                    saveThisConfiguration();

                    //Before the opening the root files
                    //we cheeck if our TList is not empty, which is
                    //usually the case then user run this menu more
                    //then the first time
                    if(rootFileName->GetSize()>0){
                      //version_1.00// << "TList--rootFileName is not empty " << rootFileName->GetSize() << endl;
                      //---------------------------------------------
                      //Some checking that do we have...............
                      for(Int_t i=0; i<rootFileName->GetSize(); i++){
                         ////version_1.00// << "From Last session: "<<((TObjString*) rootFileName->At(i))->GetString() << endl;
                         delete (((TObjString*) rootFileName->At(i)));
                      }
                      ////version_1.00// << "now we have deleted them" << endl;
                      
                      //for(Int_t i=0; i<rootFileName->GetSize(); i++){
                      //   //version_1.00// << "After deletion: "<<((TObjString*) rootFileName->At(i))->GetString() << endl;
                      //}
                    
                    
                      rootFileName->Clear();
                      //version_1.00// << "TList--rootFileName after deletion: " << rootFileName->GetSize() << endl;
                    } //---------------------------------------------
                    
                   
                    //Now we will open the first root file from directory
                    //and try to get the list of histograms in it
                    Char_t  listToFileCommand[300];
                    sprintf(listToFileCommand,"ls -l %s/%s > files.txt",DST_HIST_ROOT_fileDir, DST_HIST_ROOT_fileSpecification );
                    if(!FileSuppresDirRead){
                      gSystem->Exec(listToFileCommand);
                    }
                    Char_t ffString[300];
                    iFL=0;
                    Int_t iFileSize;
                    FILE *fSC;
                    fSC = fopen( "files.txt" , "r");
                    while( fscanf(fSC,"%s", ffString) != EOF ){
                       ////version_1.00// << "\n" << ffString << endl;
                       fscanf(fSC,"%s", ffString);
                       ////version_1.00// << "\n" << ffString << endl;
                       fscanf(fSC,"%s", ffString);
                       ////version_1.00// << "\n" << ffString << endl;
                       fscanf(fSC,"%s", ffString);
                       ////version_1.00// << "\n" << ffString << endl;
                       fscanf(fSC,"%i", &iFileSize);
                       //version_1.00// << "\n" << iFileSize << endl;
                       fscanf(fSC,"%s", ffString);
                       ////version_1.00// << "\n" << ffString << endl;
                       fscanf(fSC,"%s", ffString);
                       ////version_1.00// << "\n" << ffString << endl;
                       fscanf(fSC,"%s", ffString);
                       ////version_1.00// << "\n" << ffString << endl;
                       fscanf(fSC,"%s", ffString);
                       //version_1.00// << "\n" << ffString << endl;

                       //Now we will work under this root file to get histogramm

                       if(iFileSize>100){
                         //Here we suppress empty files
                         TObjString *flnm = new TObjString(ffString);                       
                         //version_1.00// << "flnm=" << flnm->GetString() << endl;
                         rootFileName->AddLast( flnm );
                         iFL++;
                       }

                       ////version_1.00// << "DST-QA root file loaded: " << rootFileName[iFL] << endl;
                       //printf("DST-QA-root file: %s", rootFileName[iFL]);
                       //delete flnm;
                    }
                    {//---------------------------------------------
                      //Some checking that do we have...............
                      for(Int_t i=0; i<iFL; i++){
                         //version_1.00// << ((TObjString*) rootFileName->At(i))->GetString() << endl;
                      }
                      //version_1.00// << "--end-of-checking--TOTAL NUMBER is="<<iFL << endl;
                      //version_1.00// << "                    TOTAL SIZE is ="<<rootFileName->GetSize() << endl;
                    }//---------------------------------------------

                    //Here we open a first file from a list and
                    //read names of all histograms
                    {//---------------------------------------------
                      //Some checking that do we have...............
                      for(Int_t i=0; i<1/*iFL*/; i++){
                         //version_1.00// << "Attempt to get histogram names from file: " <<((TObjString*) rootFileName->At(i))->GetString() << endl;
                         TFile *f1=NULL;
                         const Char_t *ttFile = ((TObjString*) rootFileName->At(i))->GetString() ;
                         //version_1.00// << "ttFile=" << ttFile << endl;
                         f1 = new TFile( ttFile, "READONLY", "",1 );
                         f1->cd();

                         //////////////////////////////////////////////////////////////if (gDirectory) gDirectory->ls();
                         //------reading-fistograms-from-a-file------------------------------------------------

                         //here we check if TList for histograms is not empty
                         //version_1.00// << "number of hists in TList = " << histograName.GetSize() << endl;
                         if( histograName.GetSize() != 0){
                            //that tipically means we have already red histogram list
                            //and we skip this read for any next time as it is not needed
                            //version_1.00// << "!! Histogram list is already prepared !!" << endl;
                         }else{
                            //in case it was not filled before (not yet red)
                            //we will start of extraction procedure and read it
                         
                            //TCanvas c1;
                            TString reg = "*";
                            TRegexp re(reg, kTRUE);
                            TKey *key; 
                            TIter next(gDirectory->GetListOfKeys());
                            while ((key = (TKey *) next())) {
                               //TString s = key->GetName();
                               ////version_1.00// << "KeyName" << key->GetName() << endl;
                               // //if (s.Index(re) == kNPOS) continue;
                               //key->ls();                 //*-* Loop on all the keys
                               if( ((TObject*)key->ReadObj())->InheritsFrom("TH1") || ((TObject*)key->ReadObj())->InheritsFrom("TCanvas")){
                                   //((TH1*)key->ReadObj())->DrawCopy();
                                   //c1.Update();
                                   const Char_t * histName = ((TObject*)key->ReadObj())->GetName();
                                   //version_1.00// << "Accepted histogram= "<< histName << endl;
                                   //version_1.00// << "Accepted histogram= "<< (Char_t*)(((TObject*)key->ReadObj())->GetName()) << endl;
                         
                                   if(strlen(histName)){ //to suppress adding "zero" names
                                      //--now we put this histogram in the TList
                                      Char_t hType[30];
                                      if(((TObject*)key->ReadObj())->InheritsFrom("TH1")){
                                         sprintf(hType,"%s","TH1");
                                      }
                                      if(((TObject*)key->ReadObj())->InheritsFrom("TCanvas")){
                                         sprintf(hType,"%s","TCanvas");
                                      }

                                      //version_1.00// << "Selected object Type:"<< hType << endl;
                                      //histograName.AddLast( (Char_t*)((TObject*)key->ReadObj())->GetName() );
                                      histograName.AddLast( (Char_t*)histName, (Char_t*)hType );
                                      //--end of  putting histogram in the TList
                                   }
                         
                               }
                            }
                            //------------------------------------------------------
                            //here we check if all histograms are settled in the TList
                            //version_1.00// << "check if all histograms are settled in the TList" << endl;
                            //TCanvas c2;
                            for(Int_t i=0; i<histograName.GetSize(); i++){
                               ////version_1.00// << "histogramm TList: " << ((TObjString*) histograName->At(i))->GetString() << endl;
                               //version_1.00// << "histogramm TList: " << histograName.At(i) << " type: "<< histograName.TypeAt(i)  <<endl;
                               //((TH1*) histograName->At(i))->DrawCopy();
                               //c2.Update();
                            }
                            //c2.Close();
                            //version_1.00// << "end of cheecking" << endl;
                            //end of checking if all histograms are settled in the TList
                            //-------------------------------------------------------
                         
                         
                            //Here we fill histogram names into the LixtBox
                            //------------------------------------------------------
                            //here we take all histograms from already created TList
                            //version_1.00// << "ON BUTTON: take all histograms afrom the TList" << endl;
                            //version_1.00// << "ON BUTTON: Nhistograms = " << histograName.GetSize() << endl;
                            //fListBox->Clear();
                            //fListBox->RemoveEntries();
                            for(Int_t i=0; i<histograName.GetSize(); i++){
                               ////version_1.00// << "ON BUTTON:histogramm TList: " << ((TObjString*) histograName->At(i))->GetString() << endl;
                               //version_1.00// << "ON BUTTON:histogramm TList: " << histograName.At(i) << endl;
                               //fListBox->AddEntry(((TObjString*) histograName->At(i))->GetString(), i);
                               fLastEntry++;
                               fListBox->AddEntry( histograName.At(i), fLastEntry);
                            }
                            fFirstEntry = 1;
                            //version_1.00// << "end of taking and menu forming" << endl;
                            //-------------------------------------------------------
                         
                            //-//fListBox->RemoveEntry(fFirstEntry);
                            //-//fLastEntry++;
                            //+//sprintf(tmp, "Entry %i", fLastEntry);
                            //+//fListBox->AddEntry(tmp, fLastEntry);
                            //+//fListBox->MapSubwindows();
                            //+//fListBox->Layout();
                            fListBox->MapSubwindows();
                            fListBox->Layout();


                            placeRootFileNamesIntoListBox();

                         }
                         //------reading-fistograms-from-a-file------------------------------------------------

                      }
                    }//---------------------------------------------

                    //create mean value histogramm
                    createTheMeanValueHistogram();
                    break;

                  case FILE_LOAD_FileHistMap:
                    //{
                    //  static TString dir(".");
                    // TGFileInfo fi;
                    //  fi.fFileTypes = mapfiletypes;
                    //  fi.fIniDir    = StrDup(dir.Data());
                    //  printf("fIniDir = %s\n", fi.fIniDir);
                    //  new TGFileDialog(gClient->GetRoot(), fF1, kFDOpen, &fi);
                    //  printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
                    //  dir = fi.fIniDir;
                    //  ////version_1.00// << fi.fFilename << endl;
                    //  fhmf.SetMapFileName((Char_t*)fi.fFilename);
                    //}

                    fhmf.SetMapFileName((Char_t*) fTxt3->GetText());
                    saveThisConfiguration();
                    fhmf.readFieldFromFile();                    
                    break;

                  case FILE_SAVE_FileHistMap:
                    fhmf.SetMapFileName((Char_t*) fTxt3->GetText());
                    if(strlen(fTxt3->GetText())){
                      fhmf.writeFieldIntoFile();
                    }else{
                      //version_1.00// << "Can not write into empty file" << endl;
                      //version_1.00// << "Use qaFileHisto.map by default" << endl;
                      fhmf.SetMapFileName("qaFileHisto.map");
                      fhmf.writeFieldIntoFile();
                    }
                    break;


                  case SELECT_QA_ROOT_isGoodHistogram:
                    ////version_1.00// << "iFileCurrPos=" << iFileCurrPos << "selectedHistogram_num"<<selectedHistogram_num<<endl;
                    if(rootFileName->GetSize()>0){
                      ////version_1.00// << "Histogramm is good, please report!!! "<< endl;
                      fhmf.SetField(iFileCurrPos, selectedHistogram_num, 0);
                      indicateByColorLight();
                      MarkAndCommentButton("good:");
                    }
                    break;

                  case SELECT_QA_ROOT_isWarningHistogram:
                    ////version_1.00// << "Histogramm is warning, please report!!! "<< endl;
                    if(rootFileName->GetSize()>0){
                      fhmf.SetField(iFileCurrPos, selectedHistogram_num, 1);
                      indicateByColorLight();
                      MarkAndCommentButton("warn:");
                    }
                    break;

                  case SELECT_QA_ROOT_isBadHistogram:
                    ////version_1.00// << "Histogramm is bad, please report!!! "<< endl;
                    if(rootFileName->GetSize()>0){
                      fhmf.SetField(iFileCurrPos, selectedHistogram_num, 2);
                      indicateByColorLight();
                      MarkAndCommentButton("bad!:");
                    }
                    break;

                  case HIST_PLAYER_Play:
                    //version_1.00// << "PLAY button activated"<< endl;
                    fFillHistos = kTRUE;
                    PlaySelectedHistoAlongTheFileNumber(0);
                    break;

                  case HIST_PLAYER_Step_f:
                    //version_1.00// << "STEP FORWARD button activated"<< endl;
                    fFillHistos = kTRUE;
                    PlaySelectedHistoAlongTheFileNumber(1);
                    fFillHistos = kFALSE;
                    break;

                  case HIST_PLAYER_Pause:
                    //version_1.00// << "PAUSE button activated"<< endl;
                    fFillHistos = kFALSE;
                    break;

                  case HIST_PLAYER_Step_b:
                    //version_1.00// << "STEP BACKWARD button activated"<< endl;
                    fFillHistos = kTRUE;
                    PlaySelectedHistoAlongTheFileNumber(-1);
                    fFillHistos = kFALSE;
                    break;

                  case HIST_PLAYER_Stop:
                    fFillHistos = kFALSE;
                    iFileCurrPos = 0;
                    //version_1.00// << "STOP button activated"<< endl;
                    break;

                  case PRESET_allFilesToWarning:
                    //cout << "+" << endl;
                    if(selectedHistogram_num_wasSelected){
                      //cout << "make it for histogram="<<selectedHistogram_num << endl;
                      for(Int_t ifile=0; ifile<iFL; ifile++){
                        fhmf.SetField(ifile, selectedHistogram_num, 1);
                        //indicateByColorLight();
                      }
                      replaceRootFileNamesIntoListBox();

                      //to redraw histogram yellow status in a picture window
                      // Taken from  HIST_PLAYER_Step_b button as it does identical business
                      fFillHistos = kTRUE;
                      iFileCurrPos++;
                      PlaySelectedHistoAlongTheFileNumber(-1);
                      fFillHistos = kFALSE;
                      //
                    }
                   break;

                  case PRINT_BadFiles:
                    cout << "PrintBadFiles" << endl;
                    writeBadFilesForTheHistogramToFile();
                   break;

                  case PRINT_OkFiles:
                    cout << "PrintGoodFiles" << endl;
                    writeOkFilesForTheHistogramToFile();
                   break;

                  case PRINT_AllFilesStatus:
                    cout << "PrintBadFiles" << endl;
                    writeAllFilesStatusForTheHistogramToFile();
                   break;

                  case SCAN_FOR_MEAN_VALUES:
                    //version_2.00// << "Mean value scaning procedure started" << endl;
                    if(selectedHistogram_num_wasSelected){
                      ScanForMeanValueForSelectedHistoAlongTheFileNumber(-1);
                    }else{
                      //nothing to scan: histogram is not defined by user
                    }
                    break;

                  case SCAN_FOR_MEAN_VALUES_SELECT:
                    //version_2.00// << "Mean value selection procedure started" << endl;
                    if(selectedHistogram_num_wasSelected){
                       SelectFromMeanValueForSelectedHistoAlongTheFileNumber();
                    }else{
                       //nothing to select: histogram is not defined by user
                    }
                    break;

                  case SCAN_FOR_MEAN_VALUES_SELECTED_MARK:
                    //version_2.00// << "Mean value selection procedure mark the selected files" << endl;
                    {
                      //First we get for which histogramm we did the selection to be saved
                      Int_t theSelectedHistogramName = (Int_t)hSelectedHistogramsUsingMeanValue->GetBinContent(rootFileName->GetSize()+1);
                      
                      for(Int_t ifile=0; ifile<rootFileName->GetSize(); ifile++){
                         fhmf.SetField(ifile, theSelectedHistogramName, (Int_t)hSelectedHistogramsUsingMeanValue->GetBinContent(ifile+1) );
                         //version_2.00// << "ifile=" << ifile << " flag=" << (Int_t)hSelectedHistogramsUsingMeanValue->GetBinContent(ifile) << endl;
                      }
                      //version_2.00// << "Marking for histogram="<< theSelectedHistogramName<<" done" << endl;
                    }
                    //Emit("SELECT_QA_ROOT_Histogram");
                    {//Instead of signal-slot we simply copy the body of corresponding function
                       //version_1.00// << "The histogram selected: " << histograName.At(parm2-1) << endl;
                       //cout << "+++" << endl;
                       //selectedHistogram_num = parm2-1;
                       selectedHistogram_num_wasSelected=kTRUE;
                       //sprintf(selectedHistogram    ,"%s", histograName.At( selectedHistogram_num ) );
                       //version_1.00// << "The histogram selected: " << selectedHistogram << endl;
                       sprintf(selectedHistogramType,"%s", histograName.TypeAt(selectedHistogram_num) );
                       //version_1.00// << "The histogram selected type of: "<< selectedHistogramType << endl;

                       replaceRootFileNamesIntoListBox();

                       //we need to plot current histogram
                       //
                       //version_1.00// << "hist="<<selectedHistogram_num<<" F="<<iFileCurrPos <<endl;
                       //
                       // Taken from  HIST_PLAYER_Step_b button as it does identical business
                       fFillHistos = kTRUE;
                       iFileCurrPos++;
                       PlaySelectedHistoAlongTheFileNumber(-1);
                       fFillHistos = kFALSE;
                       //
                    }
                    //SelectFromMeanValueForSelectedHistoAlongTheFileNumber(); //modification of 15Feb2005
                    break;

                  case SCAN_FOR_MEAN_VALUES_SELECTED_MARK_PRINT:
                    {
                    SCAN_FOR_MEAN_VALUES_SELECTED_MARK_PRINT_justPressed=kTRUE;
                    SelectFromMeanValueForSelectedHistoAlongTheFileNumber();
                    }
                    break;

                  default:
                    break;
                }
              break;

            case kCM_MENUSELECT:
               //printf("Pointer over menu entry, id=%ld\n", parm1);
               break;

            case kCM_CHECKBUTTON:
               switch (parm1){
                 case FILE_LOAD_SUPPRESS_DIR_READ:
                   //printf("Check button pressed, id=%ld id2=%ld\n", parm1, parm2);
                   if(FileSuppresDirRead == kFALSE){
                      FileSuppresDirRead=kTRUE;
                      //version_1.00// << "Selecting this ON you suppress rereading the directory" << endl;
                      //version_1.00// << "so what you can edit file \"files.txt\" and to delete" << endl;
                      //version_1.00// << "some undesirible strings (with file names)..." << endl;
                   }else{
                      FileSuppresDirRead=kFALSE;
                      //version_1.00// << "To allow rereading the QA-Directory" << endl;
                      //version_1.00// << "will regenerate the list of files" << endl;
                      //version_1.00// << "only if you push LOAD button afterwards." << endl;
                   }
                   break;

                 case SHOW_good_HistFiles:
                   if(bShowHF_g){bShowHF_g=kFALSE;
                   }else{        bShowHF_g=kTRUE;}
                   replaceRootFileNamesIntoListBox();
                   break;

                 case SHOW_warn_HistFiles:
                   if(bShowHF_w){bShowHF_w=kFALSE;
                   }else{        bShowHF_w=kTRUE;}
                   replaceRootFileNamesIntoListBox();
                   break;

                 case SHOW_bad_HistFiles:
                   if(bShowHF_b){bShowHF_b=kFALSE;
                   }else{        bShowHF_b=kTRUE;}
                   replaceRootFileNamesIntoListBox();
                   break;

                 case SCAN_FOR_MEAN_VALUES_SELECT_BUT_IGNORE_PREVIOUS_SELECTION:
                   if(bIgnorePrevSelection){bIgnorePrevSelection=kFALSE;
                   }else{                   bIgnorePrevSelection=kTRUE;}
                   //version_2.00// <<"bIgnorePrevSelection = "<<bIgnorePrevSelection<< endl;
                   break;

                 case SCAN_FOR_MEAN_VALUES_BUT_FROM_X_INTERVAL:
                   if(bSelectFromSubinterval){bSelectFromSubinterval=kFALSE;
                     if(selectedHistogram_num_wasSelected){
                       showSubintervalForSelectedHistogram();
                     }else{
                       //nothing to show, as the histogramm was not selected
                     }
                   }else{                     bSelectFromSubinterval=kTRUE;
                     //in addition it has to plot a histogramm and to show the actual borders
                     //only in case bSelectFromSubinterval==kTRUE
                     //showSubintervalForSelectedHistogram();
                     if(selectedHistogram_num_wasSelected){
                       showSubintervalForSelectedHistogram();
                     }else{
                       //nothing to show, as the histogramm was not selected
                     }

                   }
                   //version_2.00// <<"bSelectFromSubinterval = "<<bSelectFromSubinterval<< endl;
                   //in addition it has to plot a histogramm and to show the actual borders
                   //only in case bSelectFromSubinterval==kTRUE
                   break;

               }
               break;

            case kCM_LISTBOX:
              //printf("TListBox pressed id = %ld iList = %ld\n", parm1, parm2);

              switch (parm1){
                 case SELECT_QA_ROOT_Histogram:
                   //version_1.00// << "The histogram selected: " << histograName.At(parm2-1) << endl;
                   //version_2.00// << "+++" << endl;
                   selectedHistogram_num = parm2-1;
                   selectedHistogram_num_wasSelected=kTRUE;
                   sprintf(selectedHistogram    ,"%s", histograName.At(    parm2-1) );
                   //version_1.00// << "The histogram selected: " << selectedHistogram << endl;
                   sprintf(selectedHistogramType,"%s", histograName.TypeAt(parm2-1) );
                   //version_1.00// << "The histogram selected type of: "<< selectedHistogramType << endl;

                   //if(iFileCurrPos==0){iFileCurrPos = 1;} //lets assume it if we just started

                   replaceRootFileNamesIntoListBox();

                   //we need to plot current histogram
                   //
                   //version_1.00// << "hist="<<selectedHistogram_num<<" F="<<iFileCurrPos <<endl;
                   //
                   // Taken from  HIST_PLAYER_Step_b button as it does identical business
                   fFillHistos = kTRUE;
                   iFileCurrPos++;
                   PlaySelectedHistoAlongTheFileNumber(-1);
                   fFillHistos = kFALSE;
                   //
                   //Show the histogram on the selection pannel 
                   showSubintervalForSelectedHistogram();
                   //
                   break;

                 case SELECT_QA_ROOT_FileNumber:
                   //version_1.00// << "The current ROOT file Number is selected: "<<parm2 << endl;
                   iFileCurrPos = parm2;
                   //
                   // Taken from  HIST_PLAYER_Step_b button as it does identical business
                   fFillHistos = kTRUE;
                   PlaySelectedHistoAlongTheFileNumber(-1);
                   fFillHistos = kFALSE;
                   //
                   //
                   break;

                 default:
                   break;
              }

              break;
           
            case kCM_MENU:
               switch (parm1) {



                  case M_FILE_SAVE:
                     printf("M_FILE_SAVE\n");
                     break;

                  case M_FILE_PRINT:
                     printf("M_FILE_PRINT\n");
                     printf("Hiding itself, select \"Print Setup...\" to enable again\n");
                     break;

                  case M_FILE_PRINTSETUP:
                     printf("M_FILE_PRINTSETUP\n");
                     printf("Enabling \"Print\"\n");
                     break;

                  case M_FILE_EXIT:
                     CloseWindow();   // this also terminates theApp
                     break;

                  case M_TEST_MSGBOX:
                     //version_1.00// << "case M_TEST_MSGBOX" << endl;
                     break;

                  case M_TEST_SLIDER:
                     //version_1.00// << "case M_TEST_SLIDER" << endl;
                     break;

                  case M_TEST_NUMBERENTRY:
                     //version_1.00// << "case M_TEST_NUMBERENTRY" << endl;
                     break;

                  case M_NEW_REMOVEMENU:
                     {
                        fMenuBar->RemovePopup("New 1");
                        fMenuBar->RemovePopup("New 2");
                        fMenuBar->Layout();
                     }
                     break;

                  default:
                     break;
               }
            default:
               break;
         }

      case kC_HSLIDER:
         ////version_1.00// << "Slider" << endl;
         switch (GET_SUBMSG(msg)) {
            case kSL_POS:
              //printf("Slider update time tuning, id = %ld, %ld\n", parm1, parm2);
              tUpdateHistoDelayTime = fixedUpdateRate*parm2;
              break;

            default:
              break;
         }
         break;

      case kC_TEXTENTRY:
         //cout <<"kC_TEXTENTRY"<<endl;
         switch (GET_SUBMSG(msg)) {
           case kTE_ENTER:
               switch (parm1) {
                  case SELECT_GOOD_SIGMA_BUTTON:
                     {
                        //first we have to take care about that user has inserted
                        Double_t oneSigmaMult = atof(tXgoodSigma->GetText());
                        Double_t twoSigmaMult = atof(tXwarnSigma->GetText());
                        Bool_t change=kFALSE;
                        if(oneSigmaMult<0.0){oneSigmaMult=0.0; change=kTRUE;}
                        if(twoSigmaMult<oneSigmaMult){twoSigmaMult=oneSigmaMult; change=kTRUE;}
                        
                        if(change){ //to fix the user error
                           Char_t  chOneSig[5];
                           sprintf(chOneSig,"%1.3f",oneSigmaMult);
                           tXgoodSigma->SetText(chOneSig);

                           Char_t  chTwoSig[5];
                           sprintf(chTwoSig,"%1.3f",twoSigmaMult);
                           tXwarnSigma->SetText(chTwoSig);
                        }

                        //version_2.00// << "Refresh the selection" << endl;
                        if(selectedHistogram_num_wasSelected){
                           SelectFromMeanValueForSelectedHistoAlongTheFileNumber();
                        }else{
                           //nothing to select: histogram is not defined by user
                        }
                     }
                     break;
                  case SCAN_FOR_MEAN_VALUES_SELECT_X_INTERVAL:
                     {
                        //first we have to take care about that user has inserted
                        Double_t xMinOfInterval = atof(teXminVal->GetText());
                        Double_t xMaxOfInterval = atof(teXmaxVal->GetText());
                        Bool_t change=kFALSE;
                        if(xMinOfInterval<0.0){xMinOfInterval=0.0; change=kTRUE;}
                        if(xMaxOfInterval>1.0){xMaxOfInterval=1.0; change=kTRUE;}
                        if(xMaxOfInterval<xMinOfInterval){xMaxOfInterval=xMinOfInterval; change=kTRUE;}
                        
                        if(change){ //to fix the user error
                           Char_t  chXmin[5];
                           sprintf(chXmin,"%1.3f",xMinOfInterval);
                           teXminVal->SetText(chXmin);

                           Char_t  chXmax[5];
                           sprintf(chXmax,"%1.3f",xMaxOfInterval);
                           teXmaxVal->SetText(chXmax);
                        }

                        //version_2.00// << "Refresh the X-interval selection" << endl;
                        if(selectedHistogram_num_wasSelected){
                           showSubintervalForSelectedHistogram();
                        }else{
                           //nothing to show: histogram is not defined by user
                        }
                     }
                     break;
               }
               break;
           default:
             break;
         }
         break;

      default:
         break;
   }
   return kTRUE;
}

void TestMainFrame::placeRootFileNamesIntoListBox(){
   //Here we fill file names into the LixtBox
   //------------------------------------------------------
   //version_1.00// << "Now RootfileNames are filled into ListBox" << endl;
   for(Int_t i=0; i<rootFileName->GetSize(); i++){
      fLastEntryFile++;
      //version_1.00// << "Add: "<< ((TObjString*) rootFileName->At(i))->GetString() << endl;
      TString theFileNameToListBox( ((TObjString*) rootFileName->At(i))->GetString()  );
      //theFileNameToListBox  = theFileNameToListBox( theFileNameToListBox.Last('/')+1,theFileNameToListBox.Last('.')-theFileNameToListBox.Last('/')-1 ); 
      theFileNameToListBox  = theFileNameToListBox( theFileNameToListBox.Last('/')+1,theFileNameToListBox.Length()-theFileNameToListBox.Last('/')-1 ); 

      //theFileNameToListBox = "[x]  "+theFileNameToListBox(0,theFileNameToListBox.Length());

      //version_1.00// << theFileNameToListBox << endl;
      //fListBoxFile->AddEntry( ((TObjString*) rootFileName->At(i))->GetString() , fLastEntryFile);
      fListBoxFile->AddEntry( theFileNameToListBox , fLastEntryFile);
   }
   fFirstEntryFile = 1;
   fListBoxFile->MapSubwindows();
   fListBoxFile->Layout();
   //-------------------------------------------------------
}


void TestMainFrame::replaceRootFileNamesIntoListBox(){
   //Here we fill file names into the LixtBox
   //------------------------------------------------------
   //version_1.00// << "Now RootfileNames are REfilled into ListBox" << endl;

   Bool_t showThis;
   fListBoxFile->RemoveEntries(0, rootFileName->GetSize());
   fLastEntryFile=0;
   for(Int_t i=0; i<rootFileName->GetSize(); i++){
      fLastEntryFile++;
      ////version_1.00// << "Add: "<< ((TObjString*) rootFileName->At(i))->GetString() << endl;
      TString theFileNameToListBox( ((TObjString*) rootFileName->At(i))->GetString()  );
      theFileNameToListBox  = theFileNameToListBox( theFileNameToListBox.Last('/')+1,theFileNameToListBox.Length()-theFileNameToListBox.Last('/')-1 ); 

      //theFileNameToListBox = "[x]  "+theFileNameToListBox(0,theFileNameToListBox.Length());

      showThis=kFALSE; //initially we do not show anything
      switch( fhmf.GetField(i,selectedHistogram_num) ){
         case 0:
           theFileNameToListBox = "[-ok-]  "+theFileNameToListBox(0,theFileNameToListBox.Length());
           if(bShowHF_g){
              showThis=kTRUE;
           }
           break;
         case 1:
           theFileNameToListBox = "[····?···]  "+theFileNameToListBox(0,theFileNameToListBox.Length());
           if(bShowHF_w){
              showThis=kTRUE;
           }
           break;
         case 2:
           theFileNameToListBox = "#####  "+theFileNameToListBox(0,theFileNameToListBox.Length());
           if(bShowHF_b){
              showThis=kTRUE;
           }
           break;
         default:
           theFileNameToListBox = "     "+theFileNameToListBox(0,theFileNameToListBox.Length());
           showThis=kTRUE;
           break;
      }


      ////version_1.00// << theFileNameToListBox << endl;
      if(showThis){
        fListBoxFile->AddEntry( theFileNameToListBox , fLastEntryFile);
      }else{
        fListBoxFile->AddEntry( "+" , fLastEntryFile);
      }

   }
   fFirstEntryFile = 1;
   fListBoxFile->Select(iFileCurrPos+1, kTRUE);
   fListBoxFile->MapSubwindows();
   fListBoxFile->Layout();
   //-------------------------------------------------------
}

void TestMainFrame::saveThisConfiguration(){
   //Using file as a temporary storage of used configuration
   FILE *CFGfile;
   CFGfile = fopen( "qaSetup.cfg" , "w");
   fprintf(CFGfile,"%s\n",fTxt1->GetText());
   fprintf(CFGfile,"%s\n",fTxt2->GetText());
   fprintf(CFGfile,"%s\n",fTxt3->GetText());
   fclose(CFGfile);
}

void TestMainFrame::PlaySelectedHistoAlongTheFileNumber(Int_t ind=0){
   ////version_1.00// << "<< PlaySelectedHistoAlongTheFileNumber() >>"<< endl;
   //const int kUPDATE = 10;
   static int cnt;
   cnt = 0;   


   TText txt;
   Int_t iStart_from = iFileCurrPos;
   Int_t iFinish_at  = rootFileName->GetSize();
   Int_t iPlusPlus=1;


   if(ind == 1 && iStart_from<rootFileName->GetSize()-1){
     //
     //if we have still some place to increment we allow it
     //
     iStart_from=iStart_from+1; iFinish_at=iStart_from+1; iPlusPlus= 1;
   }else{
     if(ind == 1){
       //here someone tries to look on the file after the last one
       //we better not allow him to do this
       return;
     }
   }

   if(ind ==-1 && iStart_from>0){
     //
     //if we still have some place to decrement we allow it
     //
     iStart_from=iStart_from-1; iFinish_at=iStart_from-1; iPlusPlus=-1;
   }else{
     if(ind == -1){
       //here one tries to go back, then we are in the 0 position
       //we do not allow this
       return;
     }
   }

   //version_1.00// << "iStart_from="<<iStart_from <<" iFinish_at="<<iFinish_at << " iPlusPlus="<<iPlusPlus<<endl;

   ////version_1.00// << endl;
   //Int_t i=iStart_from;
   ////version_1.00// <<"for(Int_t i=" << iStart_from << "; " << i*iPlusPlus << "<" << iFinish_at*iPlusPlus <<"; i=i+"<<iPlusPlus<<"){...}"<<endl;
   ////version_1.00// << endl;

   TCanvas *c1 = fEc1->GetCanvas();
   gStyle->SetOptStat(1111);

   for(Int_t i=iStart_from; i*iPlusPlus<iFinish_at*iPlusPlus; i=i+iPlusPlus){
      //version_1.00// << "iStart_from="<<iStart_from <<" iFinish_at="<<iFinish_at << " iPlusPlus="<<iPlusPlus<<endl;
      if(!fFillHistos) break; //to stop while updating

      cnt++;

      TFile *f1=NULL;
      TString thisFile( ((TObjString*) rootFileName->At(i))->GetString() );
      f1 = new TFile( thisFile, "READONLY", "",1 );


      gStyle->SetPalette(1);
      TString strHistTyp(selectedHistogramType);
      if( strHistTyp.Contains("TH1") ){
         //version_1.00// << "InheritsFrom(TH1) :" <<selectedHistogram << endl;
         //gROOT->SetStyle("Plain");
         TH1    *h;
         c1->cd();
         h = (TH1*) f1->Get(selectedHistogram);
         h->GetXaxis()->SetTitleOffset(0.8);
         h->DrawCopy();
         thisFile  = thisFile( thisFile.Last('/')+1,thisFile.Last('.')-thisFile.Last('/')-1 ); 
         txt.DrawTextNDC( 0.2, 0.85, thisFile);
         c1->Update();
         iFileCurrPos = i;
         fListBoxFile->Select(i+1, kTRUE); //i+1 becouse I use back function to redraw the filename selected in a ListBoxFile
         //fListBoxFile->MapSubwindows();
         //fListBoxFile->Layout();

         indicateByColorLight();

         //version_1.00// << "file=" << i << endl;

         if(ind!=0){ 
            //that means non standart play but ">>" or "<<"
            //we want buttons update canvas twice faster
            for(Int_t it=0; it<tUpdateHistoDelayTime/2; it++){
              gSystem->ProcessEvents();  // handle GUI events
            }
         }else{
            //update canvas in a normal mode
            //for usual play operation
            for(Int_t it=0; it<tUpdateHistoDelayTime; it++){
              gSystem->ProcessEvents();  // handle GUI events
            }
         }
         //gSystem->Sleep(1500);
         delete h;
      }

      if( strHistTyp.Contains("TCanvas") ){
         TCanvas *tc;
         //version_1.00// << "InheritsFrom(TCanvas)" << endl;
         tc = (TCanvas*) f1->Get(selectedHistogram);
         //tc->DrawClone();
         //c1=tc;
         c1->Update();
         gSystem->ProcessEvents();  // handle GUI events
         //gSystem->Sleep(3000);
         delete tc;
      }

      f1->Close();
      delete f1;
   }
   gStyle->SetOptStat(0000000);

}


void TestMainFrame::ScanForMeanValueForSelectedHistoAlongTheFileNumber(Int_t ind=0){
   //version_2.00// << "<< ScanForMeanValueForSelectedHistoAlongTheFileNumber() >>"<< endl;
   //const int kUPDATE = 10;
   static int cnt;
   cnt = 0;   

   TText txt;
   Int_t iStart_from = 0;                        //We start from the first file  //iFileCurrPos;
   Int_t iFinish_at  = rootFileName->GetSize();  //We finish by the last file
   Int_t iPlusPlus=1;

   //version_2.00// << "iStart_from =" << iStart_from << " iFinish_at =" << iFinish_at << endl;

   //if(ind == 1 && iStart_from<rootFileName->GetSize()-1){
   //  //
   //  //if we have still some place to increment we allow it
   //  //
   //  iStart_from=iStart_from+1; iFinish_at=iStart_from+1; iPlusPlus= 1;
   //}else{
   //  if(ind == 1){
   //    //here someone tries to look on the file after the last one
   //    //we better not allow him to do this
   //    return;
   //  }
   //}
   //
   //if(ind ==-1 && iStart_from>0){
   //  //
   //  //if we still have some place to decrement we allow it
   //  //
   //  iStart_from=iStart_from-1; iFinish_at=iStart_from-1; iPlusPlus=-1;
   //}else{
   //  if(ind == -1){
   //    //here one tries to go back, then we are in the 0 position
   //    //we do not allow this
   //    return;
   //  }
   //}
   //version_1.00// << "iStart_from="<<iStart_from <<" iFinish_at="<<iFinish_at << " iPlusPlus="<<iPlusPlus<<endl;
   ////version_1.00// << endl;
   //Int_t i=iStart_from;
   ////version_1.00// <<"for(Int_t i=" << iStart_from << "; " << i*iPlusPlus << "<" << iFinish_at*iPlusPlus <<"; i=i+"<<iPlusPlus<<"){...}"<<endl;
   ////version_1.00// << endl;

   TCanvas *c1 = fEc1->GetCanvas();
   TCanvas *c2 = fEc2->GetCanvas();
   gStyle->SetOptStat(1111);

   Double_t xMinOfInterval = atof(teXminVal->GetText());
   Double_t xMaxOfInterval = atof(teXmaxVal->GetText());
   //version_2.00// << "xMinOfInterval="<<xMinOfInterval<<" xMaxOfInterval="<<xMaxOfInterval<<endl;

   Int_t iProgress=0;
   for(Int_t i=iStart_from; i*iPlusPlus<iFinish_at*iPlusPlus; i=i+iPlusPlus){
      //version_1.00// << "iStart_from="<<iStart_from <<" iFinish_at="<<iFinish_at << " iPlusPlus="<<iPlusPlus<<endl;
      //scanning should not be interrupted//if(!fFillHistos) break; //to stop while updating
      cnt++;
      iProgress++;

      TFile *f1=NULL;
      TString thisFile( ((TObjString*) rootFileName->At(i))->GetString() );
      f1 = new TFile( thisFile, "READONLY", "",1 );

      gStyle->SetPalette(1);
      TString strHistTyp(selectedHistogramType);
      if( strHistTyp.Contains("TH1") ){
         //version_1.00// << "InheritsFrom(TH1) :" <<selectedHistogram << endl;
         //gROOT->SetStyle("Plain");
         TH1    *h;
         c1->cd();
         h = (TH1*) f1->Get(selectedHistogram);
         h->GetXaxis()->SetTitleOffset(0.8);
         h->DrawCopy();
         if(bSelectFromSubinterval){
           hScannedMeanValue->SetBinContent(i+1, getMeanValueFromSubinterval(h, xMinOfInterval, xMaxOfInterval) );
         }else{
           hScannedMeanValue->SetBinContent(i+1, h->GetMean() ); //histogram bin enumeration starts from 1 !!!STUPID!!!
         }
         hScannedMeanValue->SetBinError(i+1, 0. );
         //cout << "scan " << i << "  mean="<< h->GetMean() << endl;

         thisFile  = thisFile( thisFile.Last('/')+1,thisFile.Last('.')-thisFile.Last('/')-1 ); 
         txt.DrawTextNDC( 0.2, 0.85, thisFile);
         c1->Update();
         //-here-not-needed-//iFileCurrPos = i;
         fListBoxFile->Select(i+1, kTRUE); //i+1 becouse I use back function to redraw the filename selected in a ListBoxFile
         //fListBoxFile->MapSubwindows();
         //fListBoxFile->Layout();

         indicateByColorLight();

         //version_1.00// << "file=" << i << endl;

         if(ind!=0){ 
            //that means non standart play but ">>" or "<<"
            //we want buttons update canvas twice faster
            for(Int_t it=0; it<tUpdateHistoDelayTime/20+1; it++){
              gSystem->ProcessEvents();  // handle GUI events
            }
         }else{
            //update canvas in a normal mode
            //for usual play operation
            for(Int_t it=0; it<tUpdateHistoDelayTime/10+1; it++){
              gSystem->ProcessEvents();  // handle GUI events
            }
         }
         //gSystem->Sleep(1500);
         delete h;
      }

      f1->Close();
      delete f1;



      //some animation while scanning to amuse the user
      c2->cd();
      c2->Clear();
      c2->Range(0, 0,  10, 10);
      if(iProgress == Int_t((iFinish_at-iStart_from)/20)+1 || i==iFinish_at-1){
        //version_2.00// << "i=" << i << "  "<< 100.*Float_t(i+1)/Float_t(iFinish_at-iStart_from) << "% ready" <<endl;
        iProgress=0;
        TArc segment(5, 5, 4, 90., 90.+360.*Double_t(i+1)/Double_t(iFinish_at-iStart_from) );
        //segment.SetFillColor(kRed); 
        segment.SetFillColor(33); //33 //38 //34 //11  //38 
        segment.Draw();
        c2->Update();
      }
   }
   gStyle->SetOptStat(0000000);

   ////Here we put "selectedHistogram_num" into N+1 position to attach it to the selection pattern
   //hSelectedHistogramsUsingMeanValue->SetBinContent(rootFileName->GetSize()+1, (Float_t)iLastScannedForMeanValueHistogram);
   //change the name of histogram for Mean Value Analyser
   //version_2.00// << "selectedHistogram=>"<<selectedHistogram << endl;
   Char_t nameOfMeanValueHistorgamTrendPlot[113];
   sprintf(nameOfMeanValueHistorgamTrendPlot,"<%s> trend plot",selectedHistogram);
   hScannedMeanValue->SetNameTitle("hScannedMeanValue",nameOfMeanValueHistorgamTrendPlot);
   //


   c2->cd();
   c2->Clear();
   c2->SetFrameFillColor(196);

   Double_t meanOfHists = 0;
   for(Int_t i=0; i<rootFileName->GetSize(); i++){
      meanOfHists = meanOfHists + hScannedMeanValue->GetBinContent(i+1);
   }
   meanOfHists = meanOfHists/Double_t(rootFileName->GetSize());
   hMeanValueLine->SetBinContent(1, meanOfHists );
   hMeanValueLine->SetLineColor(2);

   Double_t sigma2Hmin = 0;
   for(Int_t i=0; i<rootFileName->GetSize(); i++){
      sigma2Hmin = sigma2Hmin + (meanOfHists - hScannedMeanValue->GetBinContent(i+1))*(meanOfHists - hScannedMeanValue->GetBinContent(i+1));
   }
   sigma2Hmin = TMath::Sqrt( sigma2Hmin/Double_t(rootFileName->GetSize()) );

   Double_t oneSigmaMult = atof(tXgoodSigma->GetText());
   Double_t twoSigmaMult = atof(tXwarnSigma->GetText());
   if(oneSigmaMult>twoSigmaMult){twoSigmaMult=oneSigmaMult;}


   //version_2.00// << "Mean=" << meanOfHists << " sigma2Hmin="<<sigma2Hmin<<endl;

   Double_t Mp1S = meanOfHists+sigma2Hmin*atof(tXgoodSigma->GetText());
   Double_t Mm1S = meanOfHists-sigma2Hmin*atof(tXgoodSigma->GetText());
   Double_t Mp2S = meanOfHists+sigma2Hmin*atof(tXwarnSigma->GetText());
   Double_t Mm2S = meanOfHists-sigma2Hmin*atof(tXwarnSigma->GetText());

   TH1F hMeanPlus_1Sigma("hMeanPlus_1Sigma","hMeanPlus_1Sigma",1,0.,rootFileName->GetSize());
   TH1F hMeanMinus1Sigma("hMeanMinus1Sigma","hMeanMinus1Sigma",1,0.,rootFileName->GetSize());
   TH1F hMeanPlus_2Sigma("hMeanPlus_2Sigma","hMeanPlus_2Sigma",1,0.,rootFileName->GetSize());
   TH1F hMeanMinus2Sigma("hMeanMinus2Sigma","hMeanMinus2Sigma",1,0.,rootFileName->GetSize());
   hMeanPlus_1Sigma.SetBinContent(1, meanOfHists+sigma2Hmin*oneSigmaMult );
   hMeanMinus1Sigma.SetBinContent(1, meanOfHists-sigma2Hmin*oneSigmaMult );
   hMeanPlus_2Sigma.SetBinContent(1, meanOfHists+sigma2Hmin*twoSigmaMult );
   hMeanMinus2Sigma.SetBinContent(1, meanOfHists-sigma2Hmin*twoSigmaMult );


   //Double_t min = hScannedMeanValue->GetMinimum();
   //Double_t max = hScannedMeanValue->GetMaximum();
   Double_t min = hScannedMeanValue->GetBinContent(hScannedMeanValue->GetMinimumBin());
   Double_t max = hScannedMeanValue->GetBinContent(hScannedMeanValue->GetMaximumBin());
   //version_2.00// << "1 Minimum="<< min << "  Maximum=" << max << endl;
   if(min>Mm2S){ min = Mm2S; }
   if(max<Mp2S){ max = Mp2S; }
   min = min-(max-min)*0.1;
   max = max+(max-min)*0.1;
   hScannedMeanValue->SetMinimum(min);
   hScannedMeanValue->SetMaximum(max);
   //version_2.00// << "2 Minimum="<< min << "  Maximum=" << max << endl;
   //hScannedMeanValue->DrawCopy("E");
   hScannedMeanValue->Draw("E");
   if(Mm2S>0.){
     hMeanPlus_2Sigma.SetFillColor(67);
     hMeanPlus_2Sigma.DrawCopy("SAME");

     hMeanPlus_1Sigma.SetFillColor(3); 
     hMeanPlus_1Sigma.DrawCopy("SAME");

     hMeanMinus1Sigma.SetFillColor(67);
     hMeanMinus1Sigma.DrawCopy("SAME");

     hMeanMinus2Sigma.SetFillColor(196);
     hMeanMinus2Sigma.DrawCopy("SAME");
   }

   if(Mm1S>0. && Mm2S<0.){
     hMeanPlus_2Sigma.SetFillColor(67);
     hMeanPlus_2Sigma.DrawCopy("SAME");

     hMeanPlus_1Sigma.SetFillColor(3); 
     hMeanPlus_1Sigma.DrawCopy("SAME");

     hMeanMinus1Sigma.SetFillColor(67);
     hMeanMinus1Sigma.DrawCopy("SAME");

     hMeanMinus2Sigma.SetFillColor(67);
     hMeanMinus2Sigma.DrawCopy("SAME");
   }

   if(Mp1S>0. && Mm1S<0.){
     hMeanPlus_2Sigma.SetFillColor(67);
     hMeanPlus_2Sigma.DrawCopy("SAME");

     hMeanPlus_1Sigma.SetFillColor(3); 
     hMeanPlus_1Sigma.DrawCopy("SAME");

     hMeanMinus2Sigma.SetFillColor(67);
     hMeanMinus2Sigma.DrawCopy("SAME");

     hMeanMinus1Sigma.SetFillColor(3);
     hMeanMinus1Sigma.DrawCopy("SAME");
   }

   if(Mp2S>0. && Mp1S<0.){
     hMeanPlus_2Sigma.SetFillColor(67);
     hMeanPlus_2Sigma.DrawCopy("SAME");

     hMeanMinus2Sigma.SetFillColor(67);
     hMeanMinus2Sigma.DrawCopy("SAME");

     hMeanMinus1Sigma.SetFillColor(3);
     hMeanMinus1Sigma.DrawCopy("SAME");

     hMeanPlus_1Sigma.SetFillColor(67); 
     hMeanPlus_1Sigma.DrawCopy("SAME");
   }

   if(Mp2S<0.){
     hMeanMinus2Sigma.SetFillColor(67);
     hMeanMinus2Sigma.DrawCopy("SAME");

     hMeanMinus1Sigma.SetFillColor(3);
     hMeanMinus1Sigma.DrawCopy("SAME");

     hMeanPlus_1Sigma.SetFillColor(67); 
     hMeanPlus_1Sigma.DrawCopy("SAME");

     hMeanPlus_2Sigma.SetFillColor(196);
     hMeanPlus_2Sigma.DrawCopy("SAME");
   }

   hMeanValueLine->Draw("SAME");
   hScannedMeanValue->Draw("ESAME");
   c2->Update();

   iLastScannedForMeanValueHistogram=selectedHistogram_num;

   //We are keeping the same histogram flashing at the selection bar as before the scaning procedure
   // Taken from  HIST_PLAYER_Step_b button as it does identical business
   fFillHistos = kTRUE;
   iFileCurrPos++;
   PlaySelectedHistoAlongTheFileNumber(-1);
   fFillHistos = kFALSE;
   //

}


void TestMainFrame::SelectFromMeanValueForSelectedHistoAlongTheFileNumber(){
   //version_2.00// << "<< SelectForMeanValueForSelectedHistoAlongTheFileNumber() >>"<< endl;

   TCanvas *c2 = fEc2->GetCanvas();

   Int_t numOfValidFiles = 0;
   Double_t meanOfHists = 0;
   //version_2.00// << "selectedHistogram_num="<<selectedHistogram_num<<endl;
   //version_2.00// << "bIgnorePrevSelection =>"<< bIgnorePrevSelection << endl;
   for(Int_t i=0; i<rootFileName->GetSize(); i++){
      if(fhmf.GetField(i, selectedHistogram_num)<2 || bIgnorePrevSelection){
         meanOfHists = meanOfHists + hScannedMeanValue->GetBinContent(i+1);
         numOfValidFiles++;
      }
   }
   if(numOfValidFiles>0){
      meanOfHists = meanOfHists/numOfValidFiles;
   }else{
      meanOfHists = 0.;
   }
   hMeanValueLine->SetBinContent(1, meanOfHists );
   hMeanValueLine->SetLineColor(2);

   Double_t sigma2Hmin = 0;
   for(Int_t i=0; i<rootFileName->GetSize(); i++){
      if(fhmf.GetField(i, selectedHistogram_num)<2 || bIgnorePrevSelection){
        sigma2Hmin = sigma2Hmin + (meanOfHists - hScannedMeanValue->GetBinContent(i+1))*(meanOfHists - hScannedMeanValue->GetBinContent(i+1));
      }
   }
   if(numOfValidFiles>0){
      sigma2Hmin = TMath::Sqrt( sigma2Hmin/Double_t(numOfValidFiles) );
   }else{
      sigma2Hmin = 1.;
   }

   //by the moment we have estimated <x>_mean and x_Sigma
   //now we can start selection of good, warning and bad files for the histogram
   Double_t x_dev2     = 0;
   Double_t x_dev2Good = sigma2Hmin*sigma2Hmin*atof(tXgoodSigma->GetText())*atof(tXgoodSigma->GetText());
   Double_t x_dev2Warn = sigma2Hmin*sigma2Hmin*atof(tXwarnSigma->GetText())*atof(tXwarnSigma->GetText());

   //First time this loop is only to count the number of good, warn and bad points 
   //for subsequent putting them into TGraph
   Int_t nGp=0, nWp=0, nBp=0;
   for(Int_t i=0; i<rootFileName->GetSize(); i++){
      x_dev2 = (meanOfHists - hScannedMeanValue->GetBinContent(i+1))*(meanOfHists - hScannedMeanValue->GetBinContent(i+1));
      if( x_dev2 < x_dev2Warn && (fhmf.GetField(i, selectedHistogram_num)<2 || bIgnorePrevSelection) ){
        //warning or good histogram
        if( x_dev2 < x_dev2Good ){
          //it is a good one
          nGp++;
        }else{
          //it is a warning one
          nWp++;
        }
      }else{
        //it is a bad histogram
        nBp++;
      }
   }

   //Here we put "selectedHistogram_num" into N+1 position to attach it to the selection pattern
   hSelectedHistogramsUsingMeanValue->SetBinContent(rootFileName->GetSize()+1, (Float_t)iLastScannedForMeanValueHistogram);


   //Now we create 3 TGraphs for good, warn and bad ones
   TGraph grG(1);
   TGraph grW(1);
   TGraph grB(1);

   if(nGp>0){ grG.Set(nGp); }
   if(nWp>0){ grW.Set(nWp); }
   if(nBp>0){ grB.Set(nBp); }
   

   //Once again through the files, but now we are filling these TGraphs
   Int_t cntG=0, cntW=0, cntB=0;
   for(Int_t i=0; i<rootFileName->GetSize(); i++){
      x_dev2 = (meanOfHists - hScannedMeanValue->GetBinContent(i+1))*(meanOfHists - hScannedMeanValue->GetBinContent(i+1));
      if( x_dev2 < x_dev2Warn && (fhmf.GetField(i, selectedHistogram_num)<2 || bIgnorePrevSelection) ){
        //warning or good histogram
        if( x_dev2 < x_dev2Good ){
          //it is a good one
          if(nGp>0){
             hSelectedHistogramsUsingMeanValue->SetBinContent(i+1,0);
             grG.SetPoint(cntG, Double_t(i+1)-0.5, Double_t(hScannedMeanValue->GetBinContent(i+1)));
             cntG++;
          }
        }else{
          //it is a warning one
          if(nWp>0){
             hSelectedHistogramsUsingMeanValue->SetBinContent(i+1,1);
             grW.SetPoint(cntW, Double_t(i+1)-0.5, Double_t(hScannedMeanValue->GetBinContent(i+1)));
             cntW++;
          }
        }
      }else{
        //it is a bad histogram
          if(nBp>0){
             hSelectedHistogramsUsingMeanValue->SetBinContent(i+1,2);
             grB.SetPoint(cntB, Double_t(i+1)-0.5, Double_t(hScannedMeanValue->GetBinContent(i+1)));
             cntB++;
          }
      }
   }

   if(nGp>0){ grG.SetMarkerSize(0.5), grG.SetMarkerStyle(8),  grG.SetMarkerColor(kBlack); }
   if(nWp>0){ grW.SetMarkerSize(0.5), grW.SetMarkerStyle(8),  grW.SetMarkerColor(kBlue);  }
   if(nBp>0){ grB.SetMarkerSize(0.5), grB.SetMarkerStyle(21), grB.SetMarkerColor(kRed);   }

   c2->cd();
   //c2->Clear();

   //version_2.00// << "Mean=" << meanOfHists << " sigma2Hmin="<<sigma2Hmin<<endl;

   Double_t Mp1S = meanOfHists+sigma2Hmin*atof(tXgoodSigma->GetText());
   Double_t Mm1S = meanOfHists-sigma2Hmin*atof(tXgoodSigma->GetText());
   Double_t Mp2S = meanOfHists+sigma2Hmin*atof(tXwarnSigma->GetText());
   Double_t Mm2S = meanOfHists-sigma2Hmin*atof(tXwarnSigma->GetText());

   TH1F hMeanPlus_1Sigma("hMeanPlus_1Sigma","hMeanPlus_1Sigma",1,0.,rootFileName->GetSize());
   TH1F hMeanMinus1Sigma("hMeanMinus1Sigma","hMeanMinus1Sigma",1,0.,rootFileName->GetSize());
   TH1F hMeanPlus_2Sigma("hMeanPlus_2Sigma","hMeanPlus_2Sigma",1,0.,rootFileName->GetSize());
   TH1F hMeanMinus2Sigma("hMeanMinus2Sigma","hMeanMinus2Sigma",1,0.,rootFileName->GetSize());
   hMeanPlus_1Sigma.SetBinContent(1, meanOfHists+sigma2Hmin*atof(tXgoodSigma->GetText()) );
   hMeanMinus1Sigma.SetBinContent(1, meanOfHists-sigma2Hmin*atof(tXgoodSigma->GetText()) );
   hMeanPlus_2Sigma.SetBinContent(1, meanOfHists+sigma2Hmin*atof(tXwarnSigma->GetText()) );
   hMeanMinus2Sigma.SetBinContent(1, meanOfHists-sigma2Hmin*atof(tXwarnSigma->GetText()) );

   hScannedMeanValue->Draw("E");

   if(Mm2S>0.){
     hMeanPlus_2Sigma.SetFillColor(67);
     hMeanPlus_2Sigma.DrawCopy("SAME");

     hMeanPlus_1Sigma.SetFillColor(3); 
     hMeanPlus_1Sigma.DrawCopy("SAME");

     hMeanMinus1Sigma.SetFillColor(67);
     hMeanMinus1Sigma.DrawCopy("SAME");

     hMeanMinus2Sigma.SetFillColor(196);
     hMeanMinus2Sigma.DrawCopy("SAME");
   }

   if(Mm1S>0. && Mm2S<0.){
     hMeanPlus_2Sigma.SetFillColor(67);
     hMeanPlus_2Sigma.DrawCopy("SAME");

     hMeanPlus_1Sigma.SetFillColor(3); 
     hMeanPlus_1Sigma.DrawCopy("SAME");

     hMeanMinus1Sigma.SetFillColor(67);
     hMeanMinus1Sigma.DrawCopy("SAME");

     hMeanMinus2Sigma.SetFillColor(67);
     hMeanMinus2Sigma.DrawCopy("SAME");
   }

   if(Mp1S>0. && Mm1S<0.){
     hMeanPlus_2Sigma.SetFillColor(67);
     hMeanPlus_2Sigma.DrawCopy("SAME");

     hMeanPlus_1Sigma.SetFillColor(3); 
     hMeanPlus_1Sigma.DrawCopy("SAME");

     hMeanMinus2Sigma.SetFillColor(67);
     hMeanMinus2Sigma.DrawCopy("SAME");

     hMeanMinus1Sigma.SetFillColor(3);
     hMeanMinus1Sigma.DrawCopy("SAME");
   }

   if(Mp2S>0. && Mp1S<0.){
     hMeanPlus_2Sigma.SetFillColor(67);
     hMeanPlus_2Sigma.DrawCopy("SAME");

     hMeanMinus2Sigma.SetFillColor(67);
     hMeanMinus2Sigma.DrawCopy("SAME");

     hMeanMinus1Sigma.SetFillColor(3);
     hMeanMinus1Sigma.DrawCopy("SAME");

     hMeanPlus_1Sigma.SetFillColor(67); 
     hMeanPlus_1Sigma.DrawCopy("SAME");
   }

   if(Mp2S<0.){
     hMeanMinus2Sigma.SetFillColor(67);
     hMeanMinus2Sigma.DrawCopy("SAME");

     hMeanMinus1Sigma.SetFillColor(3);
     hMeanMinus1Sigma.DrawCopy("SAME");

     hMeanPlus_1Sigma.SetFillColor(67); 
     hMeanPlus_1Sigma.DrawCopy("SAME");

     hMeanPlus_2Sigma.SetFillColor(196);
     hMeanPlus_2Sigma.DrawCopy("SAME");
   }

   hMeanValueLine->Draw("SAME");
   hScannedMeanValue->Draw("ESAME");
   //hScannedMeanValue->Draw("E");
   if(nBp>0){ grB.Draw("PSAME"); }
   if(nGp>0){ grG.Draw("PSAME"); }
   if(nWp>0){ grW.Draw("PSAME"); }
   //c2->Modified();
   c2->Update();

   //Work around the problem that one is not allowed to use DrawCopy for TGraph's and theyr information is lost whyle trying to access c2(Canvas)from different function
   if(SCAN_FOR_MEAN_VALUES_SELECTED_MARK_PRINT_justPressed){
     c2->Print("scanSelection.eps");
     SCAN_FOR_MEAN_VALUES_SELECTED_MARK_PRINT_justPressed=kFALSE;
   }
}


void TestMainFrame::indicateByColorLight(){
  Int_t color;
  switch(fhmf.GetField(iFileCurrPos,selectedHistogram_num)){
    case 0:
      color = 3;
      break;
    case 1:
      color = 5;
      break;
    case 2:
      color = 2;
      break;
    default:
      color = 11;
      break;
  }

  ////version_1.00// << "we change color="<< color << endl;
  //TText   tTransparentText;
  TCanvas *colCanvas = fColoredPlatz->GetCanvas();
  colCanvas->cd();
  colCanvas->SetFillColor( color );
  //tTransparentText.DrawTextNDC(0.5, 0.5 , ".");
  colCanvas->Modified();
  colCanvas->Update();
  //delete tTransparentText;
  //delete colCanvas;
  ////version_1.00// << "we change color4" << endl;
}

void TestMainFrame::MarkAndCommentButton(Char_t* msg){
   Char_t* ffString = 0;
   FILE *reportFile;
   reportFile = fopen( "qaReport.txt" , "append");
   Int_t iCanOpenFile = (int)reportFile;

   if(   iCanOpenFile != 0 ){
     //version_1.00// << "Report file can be opened" << endl;
     while( fscanf(reportFile,"%s", ffString) != EOF ){
       ////version_1.00// << "\n" << ffString << endl;
       fscanf(reportFile,"%s", ffString);
       ////version_1.00// << "\n" << ffString << endl;
     }
     TString thisBadFile( ((TObjString*) rootFileName->At(iFileCurrPos))->GetString() );
     thisBadFile = thisBadFile( thisBadFile.Last('/')+1,thisBadFile.Length()-thisBadFile.Last('/')-1 ); 
     ////version_1.00// << "bad: file= "<<thisBadFile<<"  hist= "<<selectedHistogram<<endl;

     Char_t strFile[200];
     //strFile = thisBadFile.Data();
     sprintf(strFile,"%s",thisBadFile.Data());
     fprintf(reportFile,"%s file=%s hist=%s\n" , msg, strFile ,selectedHistogram );

     fclose( reportFile);
   }else{
     //version_1.00// << "Strange: can not open report file..." << endl;
   }
}

void TestMainFrame::createTheMeanValueHistogram(){
  if(hLocalScannedMeanValue==0){
    delete hLocalScannedMeanValue;
    TH1F  *hLocalScannedMeanValue;
    //version_2.00// << "Number of files=" << rootFileName->GetSize() << endl;
    //version_2.00// << "selectedHistogram_num="<<selectedHistogram_num<<"selectedHistogram="<<selectedHistogram << endl;

    //if(selectedHistogram==""){ cout << "histogram was not selected we assume first one" << endl;}

    //sprintf(selectedHistogram    ,"%s", histograName.At(    parm2-1) );
    sprintf(selectedHistogram    ,"%s", histograName.At( selectedHistogram_num ) );
    //version_2.00// << "selectedHistogram_num="<<selectedHistogram_num<<"selectedHistogram="<<selectedHistogram << endl;
    //version_2.00// << "rootFileName->GetSize()" << rootFileName->GetSize() << endl;
    hLocalScannedMeanValue = new TH1F("hLocalScannedMeanValue","hLocalScannedMeanValue",rootFileName->GetSize(),0, rootFileName->GetSize() );

    delete hLocalMeanValueLine;
    TH1F  *hLocalMeanValueLine;
    hLocalMeanValueLine = new TH1F("hLocalMeanValueLine","hLocalMeanValueLine",1,0, rootFileName->GetSize() );

    hScannedMeanValue = hLocalScannedMeanValue;
    hMeanValueLine    = hLocalMeanValueLine;
    hScannedMeanValue->SetXTitle("File number");
    hScannedMeanValue->SetYTitle("Hi/File mean value");


    hSelectedHistogramsUsingMeanValue = new TH1F("hSelectedHistogramsUsingMeanValue","hSelectedHistogramsUsingMeanValue",rootFileName->GetSize()+1 ,0 ,rootFileName->GetSize()+1); //+1 bin is used to store the current histogram being scanned and selected, this is important due to name of current histogram may change, but selected pattern must always refer to its selection and not to some other (by click chance)!!!
  }else{
    //it was already created before, so kill it and recreate
    delete hLocalScannedMeanValue;
    TH1F  *hLocalScannedMeanValue;
    //version_2.00// << "New mean value histogram due to change of number of files" << endl;
    //version_2.00// << "Number of files=" << rootFileName->GetSize() << endl;
    hLocalScannedMeanValue = new TH1F("hLocalScannedMeanValue","hLocalScannedMeanValue",rootFileName->GetSize(),0, rootFileName->GetSize() );

    delete hLocalMeanValueLine;
    TH1F  *hLocalMeanValueLine;
    hLocalMeanValueLine = new TH1F("hLocalMeanValueLine","hLocalMeanValueLine",1,0, rootFileName->GetSize() );

    hScannedMeanValue = hLocalScannedMeanValue;
    hMeanValueLine    = hLocalMeanValueLine;
  }
}

Double_t TestMainFrame::getMeanValueFromSubinterval(TH1 *hist, Double_t xMin, Double_t xMax){

   Double_t hIntervalMean=0;
   Double_t massOfAllBins=0;

   //cout << "FirstBin="<< hist->GetXaxis()->GetXmin()<<" LastBin="<<hist->GetXaxis()->GetXmax()<<endl;
   //for( Int_t ib=Int_t(hist->GetXaxis()->GetXmin()); ib<Int_t(hist->GetXaxis()->GetXmax()); ib++ ){
   //Double_t Xmin  = hist->GetXaxis()->GetXmin();
   //Double_t Xmax  = hist->GetXaxis()->GetXmax();

   //version_2.00// << "xMin=" << xMin<<" xMax="<<xMax<<endl;
   
   Int_t Nbins = hist->GetNbinsX();
   Double_t x;
   Int_t iIntervalBeg = Int_t(Nbins*xMin);
   Int_t iIntervalEnd = Int_t(Nbins*xMax);
   if(iIntervalBeg<0    ){iIntervalBeg=0;    }
   if(iIntervalEnd>Nbins){iIntervalBeg=Nbins;}
   if(iIntervalEnd<iIntervalBeg){iIntervalEnd=iIntervalBeg+1;}

   //for( Int_t ib=1; ib<Nbins+1; ib++ ){
   for( Int_t ib=1+iIntervalBeg; ib<iIntervalEnd+1; ib++ ){
      x = hist->GetXaxis()->GetBinCenter(ib);
      hIntervalMean = hIntervalMean + x*TMath::Abs(hist->GetBinContent(ib));
      massOfAllBins = massOfAllBins +   TMath::Abs(hist->GetBinContent(ib));
      //cout << "ib="<<ib<< " x=" <<x <<" val="<<hist->GetBinContent(ib)<<endl;
   }
   if(massOfAllBins>0){
      hIntervalMean = hIntervalMean/massOfAllBins;
   }else{
      hIntervalMean = 0.;
   }
   
   //version_2.00// << "mass="<< massOfAllBins <<" RealMean="<<hist->GetMean()<<" calculatedMean="<<hIntervalMean<< "diff="<<hist->GetMean() - hIntervalMean <<endl;
   return hIntervalMean;  
}

void TestMainFrame::showSubintervalForSelectedHistogram(){
   TCanvas *c2 = fEc2->GetCanvas();
   c2->cd();
   gStyle->SetPalette(1);

   Double_t xMinOfInterval = atof(teXminVal->GetText());
   Double_t xMaxOfInterval = atof(teXmaxVal->GetText());

   //Double_t yInfinity = 1.797e+308;

   TFile *f1=NULL;
   TString thisFile( ((TObjString*) rootFileName->At(iFileCurrPos))->GetString() );
   f1 = new TFile( thisFile, "READONLY", "",1 );

   TString strHistTyp(selectedHistogramType);
   if( strHistTyp.Contains("TH1") ){
     TH1 *h;
     h = (TH1*) f1->Get(selectedHistogram);
     h->GetXaxis()->SetTitleOffset(0.8);

     TH1D hIntervalInside("hIntervalIncide","hIntervalIncide",h->GetNbinsX(),h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax());
     TH1D hIntervalBorder("hIntervalBorder","hIntervalBorder",h->GetNbinsX(),h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax());
     
     Int_t selColor, bordColor;
     if(bSelectFromSubinterval){selColor=33; bordColor=93; }else{selColor=93; bordColor=33;}
     hIntervalBorder.SetFillColor(selColor);
     hIntervalInside.SetFillColor(bordColor);
     for(Int_t i=1; i<h->GetNbinsX()+1; i++){
       if(i>h->GetNbinsX()*xMinOfInterval && i<h->GetNbinsX()*xMaxOfInterval+1){
         hIntervalInside.SetBinContent(i,h->GetMaximum()*100.);
         hIntervalBorder.SetBinContent(i,h->GetBinContent(i));
       }else{
         //hIntervalInside.SetBinContent(i,0.);
         if(!bSelectFromSubinterval){
            //if checkbox of "From"-interval selector is not set
            //we should show actual position of all interval
            //but not its subinterval
            hIntervalInside.SetBinContent(i,h->GetMaximum()*100.);
            hIntervalBorder.SetBinContent(i,h->GetBinContent(i));
         }
       }
     }
     c2->Clear();
     h->SetFillColor(10);
     h->DrawCopy();
     hIntervalInside.DrawCopy("same");
     h->DrawCopy("same");
     hIntervalBorder.DrawCopy("same");

     c2->Update();
   }

   f1->Close();
   delete f1;
}

//04Apr2004
void TestMainFrame::writeBadFilesForTheHistogramToFile(){
   Bool_t iOK;
   FILE *badFilesForHistogrammFile;
   badFilesForHistogrammFile = fopen( "bad_files.txt" , "w");
   //version_1.00// << "Can open file?= "<< (int)badFilesForHistogrammFile << endl;
   printf("  ======== Important ========\n");
   //printf("    Saving  %s\n", fMapFile);
   printf("    Writing bad filenames\n");
   printf("    into  [ bad_files.txt ]\n");
   printf("    for selected histogram:\n");
   printf("    %s\n",selectedHistogram);
   printf("  ===========================\n");
   if( (int)badFilesForHistogrammFile != 0 ){
     //version_1.00// << "qaFileHisto.map Can be opened" << endl;
     fprintf(badFilesForHistogrammFile,"Tested histogram: [ %s ]\n",selectedHistogram);
     for(Int_t ix=0; ix<rootFileName->GetSize() /*iMaxAnalFilesNumber*/; ix++){
        //fhmf.SetField(iFileCurrPos, selectedHistogram_num, 0);
        //fprintf(badFilesForHistogrammFile,"%i ", iFHfm[ix][iy]);
        if( fhmf.GetField(ix, selectedHistogram_num)==2 ){ 
          cout << "Bad File = " << ix << " "<< ((TObjString*)rootFileName->At(ix))->GetString() <<endl;
          //Char_t ffnn[200] = ((TObjString*)rootFileName->At(ix))->GetName();
          fprintf(badFilesForHistogrammFile,"%s\n", ((TObjString*)rootFileName->At(ix))->GetName());
        }
     }
     fclose(badFilesForHistogrammFile);
     iOK=kTRUE;
   }else{
     printf("  ============================ Sorry =============================\n");
     printf("  bad_files.txt cannot be opened... May be it is write protected?\n");
     printf("  -------------                                    ---------------\n");
     iOK=kFALSE;
   }
   ////delete fieldFile;
   //return iOK;
}

//15Feb2005
void TestMainFrame::writeOkFilesForTheHistogramToFile(){
   Bool_t iOK;
   FILE *okFilesForHistogrammFile;
   okFilesForHistogrammFile = fopen( "ok_files.txt" , "w");
   //version_1.00// << "Can open file?= "<< (int)badFilesForHistogrammFile << endl;
   printf("  ======== Important ========\n");
   //printf("    Saving  %s\n", fMapFile);
   printf("    Writing -OK- filenames\n");
   printf("    into  [ bad_files.txt ]\n");
   printf("    for selected histogram:\n");
   printf("    %s\n",selectedHistogram);
   printf("  ===========================\n");
   if( (int)okFilesForHistogrammFile != 0 ){
     //version_1.00// << "qaFileHisto.map Can be opened" << endl;
     fprintf(okFilesForHistogrammFile,"Tested histogram: [ %s ]\n",selectedHistogram);
     for(Int_t ix=0; ix<rootFileName->GetSize() /*iMaxAnalFilesNumber*/; ix++){
        if( fhmf.GetField(ix, selectedHistogram_num)==0 ){
          cout << "Good File = "  << ix << " "   << ((TObjString*)rootFileName->At(ix))->GetString() <<endl;
          //fprintf(okFilesForHistogrammFile,"%s\n", ((TObjString*)rootFileName->At(ix))->GetName());
          fprintf(okFilesForHistogrammFile,"[%i]  #%3i  %s\n", fhmf.GetField(ix, selectedHistogram_num), ix, ((TObjString*)rootFileName->At(ix))->GetName());
        }
        if( fhmf.GetField(ix, selectedHistogram_num)==1 ){
          cout << "Warn File = "  << ix << " "   << ((TObjString*)rootFileName->At(ix))->GetString() <<endl;
          //fprintf(okFilesForHistogrammFile,"%s\n", ((TObjString*)rootFileName->At(ix))->GetName());
          fprintf(okFilesForHistogrammFile,"[%i]  #%3i  %s\n", fhmf.GetField(ix, selectedHistogram_num), ix, ((TObjString*)rootFileName->At(ix))->GetName());
        }
     }
     fclose(okFilesForHistogrammFile);
     iOK=kTRUE;
   }else{
     printf("  ============================ Sorry =============================\n");
     printf("  ok_files.txt cannot be opened... May be it is write protected?\n");
     printf("  -------------                                    ---------------\n");
     iOK=kFALSE;
   }
   ////delete fieldFile;
   //return iOK;
}


//15Apr2004
void TestMainFrame::writeAllFilesStatusForTheHistogramToFile(){
   Bool_t iOK;
   FILE *statusFilesForHistogrammFile;
   statusFilesForHistogrammFile = fopen( "status_files.txt" , "w");
   printf("  ======== Important ========\n");
   printf("    Writing filename&status\n");
   printf("    into [ status_files.txt ]\n");
   printf("    for selected histogram:\n");
   printf("    %s\n",selectedHistogram);
   printf("  ===========================\n");
   if( (int)statusFilesForHistogrammFile != 0 ){
     //version_1.00// << "qaFileHisto.map Can be opened" << endl;
     fprintf(statusFilesForHistogrammFile,"Tested histogram: [ %s ]\n",selectedHistogram);
     fprintf(statusFilesForHistogrammFile,"[status], #fileNumber, [filename]\n");
     for(Int_t ix=0; ix<rootFileName->GetSize(); ix++){
        //fhmf.SetField(iFileCurrPos, selectedHistogram_num, 0);
        //fprintf(badFilesForHistogrammFile,"%i ", iFHfm[ix][iy]);
        //if( fhmf.GetField(ix, selectedHistogram_num)==2 ){ 
        cout << "Status="<< fhmf.GetField(ix, selectedHistogram_num) << "File = " << ix << " "<< ((TObjString*)rootFileName->At(ix))->GetString() <<endl;
        fprintf(statusFilesForHistogrammFile,"[%i]  #%3i  %s\n", fhmf.GetField(ix, selectedHistogram_num), ix, ((TObjString*)rootFileName->At(ix))->GetName());
        //}
     }
     fclose(statusFilesForHistogrammFile);
     iOK=kTRUE;
   }else{
     printf("  ============================= Sorry ==============================\n");
     printf("  status_files.txt cannot be opened... May be it is write protected?\n");
     printf("  ----------------                                  ----------------\n");
     iOK=kFALSE;
   }
}


//------------------------------------------------------------------------------

//---- Main program ------------------------------------------------------------

int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);

   if (gROOT->IsBatch()) {
      fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
      return 1;
   }

   TestMainFrame mainWindow(gClient->GetRoot(), 400, 220);

   theApp.Run();

   return 0;
}
