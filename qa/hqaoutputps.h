//*-- Author : Dan Magestro (magestro@gsi.de)
//*-- Modified : 24/01/2002
#ifndef HQAOUTPUTPS_H
#define HQAOUTPUTPS_H

#include "TPostScript.h"
#include "TPDF.h"

class TCanvas;
class TPad;

//class HQAOutputPS : public TPostScript {
class HQAOutputPS : public TPDF {

public:
   HQAOutputPS() {}
   HQAOutputPS(TString fname);
   ~HQAOutputPS();


   void writeHist(TList *h);
   //void writeHist(TList *h1, TList *h2); //for 2 lists //asd
   void writeHist(TList *h1, TList *h2, TString fnamePS); //for 2 lists into another root file //asd
   void makeHist(TList *h);
   void makeText(TList *h);
   void saveScal(TList *h, TString fnamePS); //asd txt-file output for scalers

   void closePS();

   void setNHistPerPage(Int_t nh) {
      nHistPerPage = nh;
   }

   inline void setStats(Int_t nEvents, Int_t nQAProcessed = 0);
   inline void setDSTFileName(const Char_t *file) {
      fnameDST = file;
   }

private:
   TCanvas *fCanvas;       //! canvas
   TPad    *fHistPad;      //! histogram pad

   Int_t pageCount;        //! counts pages of a certain type
   Int_t histCount;        //! counts histograms for page breaks
   Int_t nProcessed;       //! number of events QA'd
   Int_t nHistPerPage;     //! user-supplied parameter (default=6)
   Int_t nEvent;           //! total number of events in output file

   Bool_t kFIRST;          //! needed for one-time formatting
   Bool_t kPDF;            //! generate PDF file using ps2pdf; default = kTRUE

   TString fnamePS;        //! PS filename
   TString fnameDST;       //! DST filename (used in page header)
   Int_t   fdevice;        //!
   TString pageTitle;      //!

   void makeNewPage(TString type);
   void setStyle();

   ClassDef(HQAOutputPS, 1) // QA Postscript file generator
};

inline void HQAOutputPS::setStats(Int_t nEvents, Int_t nQAProcessed)
{
   nEvent = nEvents;
   nProcessed = nQAProcessed;
}

#endif
