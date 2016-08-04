#ifndef HREPORT_H
#define HREPORT_H
#include "TNamed.h"
#include "TObject.h"
#include "TString.h"
#include "TCanvas.h"
#include "TDatime.h"
#include <fstream>

class HReport : public TNamed {
protected:
    TString texfile;          // name of the report tex file
    TString texdir;           // name of the temp directory
    //------------------------------------------------------
    // options
    Bool_t  colorlinks;       //!  switch pdf links
    Bool_t  makeTOC;          //!  table of contents
    Bool_t  makeListFigures;  //!  list of figures
    Bool_t  makeListTables;   //!  list of tables
    Bool_t  makeListListings; //!  list of listings
    Bool_t  draft;            //!  replace pics by place holders
    Int_t   font_size;        //!  font size
    TString single_double;    //!  oneside or twoside paper
    TString orientation;      //!  landscape or potrait
    TString column;           //!  onecolumn twocolumn layout
    TString viewer;           //!  gv or acroread
    Bool_t  keeppics;         //!  keep old pictures or remake

    TString include_packages; //!  include additional packages if needed

    TString title;            //!  title of the report
    TString author;           //!  author of the report
    TString date;             //!  date of creation
    TDatime daytime;          //!  time object

    ofstream *fout;           //!  stream to output file

    Bool_t setopt;            //!  flag
    Bool_t includepack;       //!  flag
    Bool_t opentex;           //!  flag
    Bool_t opendoc;           //!  flag
    Bool_t closedoc;          //!  flag
    Bool_t useviewer;         //!  flag
    Int_t  linebreak;         //!  line width in tex file
    Int_t  linect;            //!  counter of character in line
    Bool_t always_skip_curly; //!  force skipping curly braces for line break

    void    initVariables ();
public:
    HReport(const Char_t* name="",const Char_t* title="");
    ~HReport  ();
    void  setOptions(TString option="");
    void  includePackages(TString packages="");
    void  openTex(TString tdir="",TString tfile="", TString titlename="Report",TString authorname="");
    void  openDocument();
    void  closeDocument();
    void  addText(TString text);
    void  figure(TCanvas* canvas=0,TString cap="",TString shcap="",TString lab="",Float_t width=-1,Int_t x1=-1,Int_t y1=-1,Int_t x2=-1,Int_t y2=-1,Int_t rotate=0,TString filename="",Bool_t force=kFALSE,TString format="pdf");
    void  figureSideCaption(TCanvas* canvas=0,TString cap="",TString shcap="",TString lab="",Float_t widthpic=0.5,Float_t widthtxt=0.45,Float_t space=0.02,Int_t x1=-1,Int_t y1=-1,Int_t x2=-1,Int_t y2=-1,Int_t rotate=0,TString filename="",Bool_t force=kFALSE,TString format="pdf");
    void  table(Float_t* val=0,Float_t* valerr=0,Int_t sx=0,Int_t sy=0,TString titlex="",TString titley="",TString format="",TString line="",TString cap="",TString shcap="",TString label="");
    void  openTable (TString format="");
    void  closeTable(TString cap="",TString shcap="",TString lab="");
    void  openAppendix();
    void  closeAppendix();
    void  includeSource(TString source="",TString cap="",TString shcap="",TString label="",Int_t firstline=-1,Int_t lastline=-1,TString option="");
    void  makepdf();
    void  openpdf();
    void  printOrder();
    void  printOptions();
    void  printSettings();
    Int_t saveToPdf(TCanvas* canvas=0,TString filename="",Bool_t separate=kFALSE,Bool_t force=kFALSE);
    Int_t saveToPng(TCanvas* canvas=0,TString filename="",Bool_t separate=kFALSE,Bool_t force=kFALSE);
    TString* parseString(TString options,Int_t& size,TString separator="",Bool_t tolower=kTRUE,Bool_t removespace=kTRUE);
    void  setLineBreak(Int_t width){linebreak=width;}
    void  allwaysSkipCurly(){always_skip_curly=kTRUE;}
    void  writeTex(TString text="",Bool_t skip_curly=kTRUE);
    ClassDef(HReport,1) // report class
};
#endif  /*!HREPORT_H*/
