//*-- AUTHOR : J. Markert

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HReport
// This class provides a wrapper arround Latex/pdflatex. The tex file will
// be created on the fly from a ROOT macro and compiled in background. The
// output is a pdf file. This class uses pdflatex with report.sty, so it needs
// latex/pdflattex to be installed on the system. For viewing acroread or gv is
// used. For converting ROOT graphics ps2pdf and convert has to be available.
// HReport creates a directory (name specified in openTex(...)). The tex file
// and the converted pictures will be created inside the directory. The pdf
// output is coppied to the working dir. A new run of HReport will remove the
// existing directory (if "keeppics" option is not specified), so make sure
// that the directory name is not equal to a directory you don't want to remove!
// The order of the different function calls is important! setOptions("") and
// includePackages("") has to be called before openTex(), otherwise they will
// have no effect.
//--------------------------------------------------------------------------
// HOW TO USE HReport:
//    HReport* report=new HReport("report","report");
//    report->setOptions("");
//    Options:
//             landscape           (default portrait)
//                                 switch the orientation of the paper
//             colorlinks          (default no colorlinks)
//                                 pdf links will be red and printed colored
//             noTOC               (default TOC)
//                                 do not make table of contents
//             noListFigures       (default ListFigures)
//                                 do not make a list of figures
//             noListTables        (default ListTables)
//                                 do not make a list of tables
//             noListListings      (default ListListings)
//                                 do not make a list of listings
//             fontsize=11 or 10   (default fontsize=12)
//                                 switch to smaler fonts 10pt and 11pt
//             oneside             (default twoside)
//                                 single sided layout
//             twocolumn           (default onecolumn)
//                                 two text columns
//             keeppics            (default do not keep pics)
//                                 keep the old pictures
//             draft               (default no draft)
//                                 place holders instead of pictures
//             gv                  (default acroread)
//                                 use gv or acroread to view the pdf output
//
//    report->includePackages("name,name2");             :: comma separated list
//                                                       :: of additional packages (optional)
//
//    report->openTex("tex_dir",                         :: temp dir in working dir (created) containing pictures,tex..
//                    "tex_file_name.tex",               :: name of the created tex file (and the created pdf output!)
//                    "Title of report",                 :: Title of the report shown on the title page
//                    "Author");                         :: Author shown on the title page
//
//    report->openDocument();                            :: open the tex file with specified options
//    report->closeDocument();                           :: closes the tex file
//    report->makepdf();                                 :: calls pdflatex in background and copies the pdf output to working dir
//    report->openpdf();                                 :: opens the pdf file in acroread
//
//--------------------------------------------------------------------------
//    Between openDocument() and closeDocument() you can add what ever you like
//    to your report.
//    Add any text using latex commands :
//    Rules:   "\"    has to be replaced by "\\"
//             "\n"   gives a new line in texfile
//             "\n\n" creates a new paragraph
//
//    report->addText("\\chapter{Report something }\n "
//		      "This report is very important....\n\n "
//                    "line can be splitted in this way if your editor does not "
//                    "support line wrapping."};
//
//
//    report->openAppendix()  opens the appendix environment
//    report->closeAppendix() close the appendix environment
//    --everything between the calls will go into the appendix
//
//---------------------------------------------------------------------------
//    For simple formated tables a template can be created :
//
//    Float_t values[6]={1.1,2.2,
//                       3.3,4.4,
//                       5.5,6.6};
//
//    Float_t valerr[6]={1.1,2.2,
//                       3.3,4.4,
//                       5.5,6.6};
//
//    report->table(&values[0],&valerr[0],                      :: pointer to 1dim array value and error of value
//                  2,3,                                        :: size in x and y of the table (2x3 only)
//		   "x1,x2","y1,y2,y3",                          :: comma separated list column header (x) and line header (y)
//		   "|c|c|",                                     :: format of the tabel (here centered with line separation)
//                 "%5.1f $\\pm$ %5.2f & %5.1f $\\pm$ %5.2f",   :: cells are seprated by &, $\\pm$ indicates usage of errors,
//                                                              :: formating as printf for floats (tipp: a simple % becomes \\%%)
//		   "caption of table",                          :: caption below the table
//                 "short caption for list of tables",          :: caption used in list of tables
//                 "label_of_table");                           :: label for referencing of the table
//
//---------------------------------------------------------------------------
//    Add Root graphics to the report:
//    Figure with caption on the binding side:
//    report->figureSideCaption(Tcanvas* canvas,                :: canvas pointer which contains the grahic
//                   "caption of figure",                       :: caption beside the figure
//                   "short caption for list of figures",       :: caption used in list of figures
//                   "label_of_figure",                         :: label for referencing of the figure
//                   0.7,0.2,0.02,                              :: size of graphic / caption / space in terms of linewidth
//                   54,30,1030,750,                            :: viewport x1,y1,x2,y2 in pixels (display ony a part or the picture)
//                                                              :: values equal -1 shows full picture
//                   -90,                                       :: rotation (- = clock wise)
//                   "filename");                               :: output name of picture ("" will generate the
//                                                                 name from name of canvas).
//                   force);                                    :: picture will be created  no matter if keeppics option is used
//
//   Figure with caption below as decribed above (without parameters for caption size and space)
//   report->figure(canv ,"caption","short cap","label",picsize,x1,y1,x2,y2,rotation,"filename");
//
//---------------------------------------------------------------------------
//   Include C++ source files / ROOT macros into your code:
//   This is a wrapper arround listings package of tex. Not all
//   features are supported but the documentation of the package
//   will help to get the maximum usage out.
//
//   report->includeSource(TString source,                     :: path/filename of the source (relative path accpeted)
//                         TString cap,                        :: caption used above the listing
//                         TString shcap,                      :: caption used for list of listings
//                         TString label,                      :: label for referencing the listing
//                         Int_t firstline,                    :: first line of code listed
//                         Int_t lastline,                     :: last line of code listed
//                         TString option);                    :: additional options can be taken into account
//
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include "hreport.h"
#include "htool.h"
#include "TSystem.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "TObjString.h"

ClassImp(HReport)

HReport::HReport(const Char_t* name,const Char_t* title)
    : TNamed(name,title)
{
    // constructor for HReport

    initVariables();

    cout<<"#------------------------------------------------------------------------------"<<endl;
    cout<<" HReport checking needed programs:"<<endl;
    Bool_t acro =kTRUE;
    Bool_t ghost=kTRUE;
    if(gSystem->Exec("which pdflatex"))
    {
	Error("HReport()","pdflatex is not installed on your system!");
        exit(1);
    }
    if(gSystem->Exec("which ps2pdf"))
    {
	Error("HReport()","ps2pdf is not installed on your system!");
        exit(1);
    }
    if(gSystem->Exec("which convert"))
    {
	Error("HReport()","convert is not installed on your system!");
        exit(1);
    }
    if(gSystem->Exec("which acroread"))
    {
	Warning("HReport()","acroread is not installed on your system! \n \
		Switch to gv for viewing! ");
	viewer="gv";
        acro  =kFALSE;
    }
    if(gSystem->Exec("which gv"))
    {
	Warning("HReport()","gv is not installed on your system! \
		Do not gv as option for openpdf()");
        ghost =kFALSE;
    }
    if(!acro&&!ghost)
    {
	Warning("HReport()","acroread and gv are not installed on your system! \
		Do not use openpdf() at all!");

	viewer   ="none";
        useviewer=kFALSE;
    }
    cout<<"#------------------------------------------------------------------------------"<<endl;

    printOrder();
    printOptions();

}
HReport::~HReport()
{
  // destructor of HReport
    if(fout)delete fout;
    fout=NULL;
}
void HReport::initVariables()
{
    // inits all variables

    fout=NULL;
    texfile ="";
    texdir  ="";

    // options
    colorlinks      =kFALSE;
    makeTOC         =kTRUE;
    makeListFigures =kTRUE;
    makeListTables  =kTRUE;
    makeListListings=kTRUE;
    draft           =kFALSE;
    font_size       =12;
    single_double   ="twoside";
    column          ="onecolumn";
    orientation     ="";
    include_packages="";
    title           ="";
    author          ="";
    date            ="";
    keeppics        =kFALSE;
    viewer          ="acroread";
    useviewer       =kTRUE;
    // checkers
    setopt          =kFALSE;
    includepack     =kFALSE;
    opentex         =kFALSE;
    opendoc         =kFALSE;
    closedoc        =kFALSE;
    linebreak       =80;
    linect          =1;
}
void HReport::printOptions()
{
    cout<<"#------------------------------------------------------------------------------"<<endl;
    cout<<"#    Options:"<<endl;
    cout<<"#             landscape           (default portrait)"<<endl;
    cout<<"#                                 switch the orientation of the paper"<<endl;
    cout<<"#             colorlinks          (default no colorlinks)"<<endl;
    cout<<"#                                 pdf links will be red and printed colored"<<endl;
    cout<<"#             noTOC               (default TOC)"<<endl;
    cout<<"#                                 do not make table of contents"<<endl;
    cout<<"#             noListFigures       (default ListFigures)"<<endl;
    cout<<"#                                 do not make a list of figures"<<endl;
    cout<<"#             noListTables        (default ListTables)"<<endl;
    cout<<"#                                 do not make a list of tables"<<endl;
    cout<<"#             noListListings      (default ListListings)"<<endl;
    cout<<"#                                 do not make a list of listings"<<endl;
    cout<<"#             fontsize=11 or 10   (default fontsize=12)"<<endl;
    cout<<"#                                 switch to smaler fonts 10pt and 11pt"<<endl;
    cout<<"#             oneside             (default twoside)"<<endl;
    cout<<"#                                 single sided layout"<<endl;
    cout<<"#             twocolumn           (default onecolumn)"<<endl;
    cout<<"#                                 two text columns "<<endl;
    cout<<"#             keeppics            (default do not keep pics)"<<endl;
    cout<<"#                                 keep the old pictures"<<endl;
    cout<<"#             draft               (default no draft)"<<endl;
    cout<<"#                                 place holders instead of pictures"<<endl;
    cout<<"#             gv                  (default acroread)"<<endl;
    cout<<"#                                 use gv or acroread to view the pdf output"<<endl;
    cout<<"#------------------------------------------------------------------------------"<<endl;
}
void HReport::printSettings()
{
    TString orient=orientation;
    orient.ReplaceAll(",","");
    cout<<"#------------------------------------------------------------------------------"<<endl;
    cout<<"#    ACTUAL SETTINGS:"<<endl;
    cout<<"#             orientation         : "<<orient.Data()<<endl;
    if(colorlinks)      cout<<"#             colorlinks          : kTRUE"<<endl;
    else                cout<<"#             colorlinks          : kFALSE"<<endl;
    if(makeTOC)         cout<<"#             noTOC               : kTRUE"<<endl;
    else                cout<<"#             noTOC               : kFALSE"<<endl;
    if(makeListFigures) cout<<"#             noListFigures       : kFALSE"<<endl;
    else                cout<<"#             noListFigures       : kTRUE"<<endl;
    if(makeListTables)  cout<<"#             noListTables        : kFALSE"<<endl;
    else                cout<<"#             noListTables        : kTRUE"<<endl;
    if(makeListListings)cout<<"#             noListListings      : kFALSE"<<endl;
    else                cout<<"#             noListListings      : kTRUE"<<endl;
    cout<<"#             fontsize            : "<<font_size<<endl;
    cout<<"#             print               : "<<single_double.Data()<<endl;
    cout<<"#             column              : "<<column.Data()<<endl;
    if(keeppics)        cout<<"#             keeppics            : kTRUE"<<endl;
    else                cout<<"#             keeppics            : kFALSE"<<endl;
    if(draft)           cout<<"#             draft               : kTRUE"<<endl;
    else                cout<<"#             draft               : kFALSE"<<endl;
    cout<<"#             viewer              : "<<viewer.Data()<<endl;
    cout<<"#             line break          : "<<linebreak<<" characters"<<endl;
    cout<<"#------------------------------------------------------------------------------"<<endl;
}
void HReport::setOptions(TString option)
{
    //    Options:
    //             landscape           (default portrait)
    //                                 switch the orientation of the paper
    //             colorlinks          (default no colorlinks)
    //                                 pdf links will be red and printed colored
    //             noTOC               (default TOC)
    //                                 do not make table of contents
    //             noListFigures       (default ListFigures)
    //                                 do not make a list of figures
    //             noListTables        (default ListTables)
    //                                 do not make a list of tables
    //             noListListings      (default ListListings)
    //                                 do not make a list of listings
    //             fontsize=11 or 10   (default fontsize=12)
    //                                 switch to smaler fonts 10pt and 11pt
    //             oneside             (default twoside)
    //                                 single sided layout
    //             twocolumn           (default onecolumn)
    //                                 two text columns
    //             keeppics            (default do not keep pics)
    //                                 keep the old pictures
    //             draft               (default no draft)
    //                                 place holders instead of pictures
    //             gv                  (default acroread)
    //                                 use gv or acroread to view the pdf output
    // This function has to be called befor openTex().

    if(setopt)
    {
	Error("HReport::setOptions()","Function called more than one time!");
	printOrder();
	exit(1);
    }
    if(opendoc)
    {
	Error("HReport::setOptions()","Function called after opening of tex document!");
        printOrder();
	exit(1);
    }
    setopt=kTRUE;

    Int_t size=0;
    TString* array=parseString(option,size,",",kTRUE,kTRUE);
    if(HTool::findString(array,size,"colorlinks"))    colorlinks      =kTRUE;
    if(HTool::findString(array,size,"notoc"))         makeTOC         =kFALSE;
    if(HTool::findString(array,size,"nolistfigures")) makeListFigures =kFALSE;
    if(HTool::findString(array,size,"nolisttables"))  makeListTables  =kFALSE;
    if(HTool::findString(array,size,"nolistlistings"))makeListListings=kFALSE;
    if(HTool::findString(array,size,"draft"))         draft           =kTRUE;
    if(HTool::findString(array,size,"fontsize=12"))   font_size       =12;
    if(HTool::findString(array,size,"fontsize=11"))   font_size       =11;
    if(HTool::findString(array,size,"fontsize=10"))   font_size       =10;
    if(HTool::findString(array,size,"oneside"))       single_double   ="oneside";
    if(HTool::findString(array,size,"landscape"))     orientation     =",landscape";
    if(HTool::findString(array,size,"twocolumn"))     column          ="twocolumn";
    if(HTool::findString(array,size,"keeppics"))      keeppics        =kTRUE;
    if(HTool::findString(array,size,"gv"))            viewer          ="gv";

}
void HReport::includePackages(TString packages)
{
    // Comma separated list of additional packages (optional)
    // report->includePackages("name,name2");
    // This function has to be called befor openTex().

    if(includepack)
    {
	Error("HReport:includePackages()","Function called more than one time!");
	printOrder();
        exit(1);
    }
    if(opendoc)
    {
	Error("HReport::includePackages()","Function called after opening of tex document!");
        printOrder();
	exit(1);
    }
    includepack     =kTRUE;
    include_packages=packages;
}

void HReport::openTex(TString tdir,TString tfile, TString titlename, TString authorname)
{
    // The tex file and the converted pictures will be created inside the directory.
    // The pdf output is coppied to the working dir. A new run of HReport will
    // remove the existing directory (if "keeppics" option is not specified), so
    // make sure that the directory name is not equal to a directory you don't
    // want to remove!
    //    report->openTex("tex_dir",             :: temp dir in working dir (created) containing pictures,tex..
    //                    "tex_file_name.tex",   :: name of the created tex file (and the created pdf output!)
    //                    "Title of report",     :: Title of the report shown on the title page
    //                    "Author");             :: Author shown on the title page

    if(opentex)
    {
	Error("HReport:openTex()","Function called more than one time!");
	printOrder();
        exit(1);
    }
    opentex=kTRUE;

    if(opendoc)
    {
	Error("HReport:openTex()","Function called after opening the tex document!");
        printOrder();
	exit(1);
    }

    if(tfile.CompareTo("")==0||tdir.CompareTo("")==0)
    {
	Error("HReport::openTex()","Input for tex file or tex directory not set!");
        exit(1);
    }

    if(tfile.Contains(".tex")==0)
    {
	Error("HReport::openTex()","Tex file doen not have the correct file type .tex!");
        exit(1);
    }

    //------------------------------------------------------------
    // setting up variables of the title page
    title =titlename;
    author=authorname;
    date  =Form("%i/%i/%i %i:%02i",
	    daytime.GetDay(),
	    daytime.GetMonth(),
	    daytime.GetYear(),
	    daytime.GetHour(),
	    daytime.GetMinute());

    //------------------------------------------------------------

    texfile        =tfile;
    texdir         =tdir;
    TString path   ="";
    TString command="";

    printSettings();

    gSystem->ExpandPathName(texdir); // expand environment variables etc.

    //------------------------------------------------------------
    // check if one or more directory level should be created
    TString temp  =texdir;
    TString base  =gSystem->BaseName(texdir.Data());
    TString parent=temp.Remove(temp.Length()-base.Length());

    if(parent.CompareTo("/")!=0
       &&parent.CompareTo("")!=0
       &&gSystem->AccessPathName(parent.Data()))
    {
	Error("HReport::openTex()","Recursive mkdir not supported, check your texdir!");
        exit(1);

    }
    //------------------------------------------------------------

    //------------------------------------------------------------
    // removing old directory
    if(!keeppics)
    {   // don't remove old pictures

	if(gSystem->AccessPathName(texdir))
	{
	    cout<<"HReport:: dir "<<texdir<<" does not exist!"<<endl;
	}
	else
	{
	    cout<<"HReport::Removing old tex dir!"<<endl;
	    path = "rm -r " + texdir;
	    gSystem->Exec(path.Data());
	}

        cout<<"HReport::Creating tex dir!"<<endl;
        path = "mkdir " + texdir;
        gSystem->Exec(path.Data());
    }
    else
    {
	if(gSystem->AccessPathName(texdir))
	{
	    Error("HReport::openTex()","keeppics option used, but texdir does not exist!");
	    exit(1);
	}
    }
    //------------------------------------------------------------

    //------------------------------------------------------------
    // removing old tex files

    TString filename=texfile;
    filename.ReplaceAll(".tex","");

    if(!gSystem->AccessPathName(texdir))
    {
	const Char_t* suffix[8]={".lot",".toc",".aux",".lof",".out",".log",".lol",".tex"};
	for(Int_t i=0;i<8;i++)
	{
	    path   = texdir + "/" + filename + suffix[i];
	    command= "rm " + path;
	    if(!gSystem->AccessPathName(path)) gSystem->Exec(command.Data());
	}
    }
    //------------------------------------------------------------

    //------------------------------------------------------------
    // opening output stream to tex file
    path=texdir + "/" + texfile;
    fout = new ofstream(path.Data(), ios::out);

    if (fout){
	cout<<"HReport::Tex file opened : "<<path<<" !"<<endl;
    }
    else
    {
	Error("HReport::openTex()","Could not open tex file!");
        exit(1);

    }
    //------------------------------------------------------------
}
void HReport::openDocument()
{
    // open the tex file with specified options
    if (fout){
	cout<<"HReport::Opening document !"<<endl;
    }
    else
    {
	Error("HReport::openDocument()","No valid file pointer!");
	printOrder();
        exit(1);

    }
    if(opendoc)
    {
	Error("HReport:openDocument()","Function called more than one time!");
        printOrder();
	exit(1);
    }
    opendoc=kTRUE;

    //------------------------------------------------------------------
    // defining document class with options
    TString docclass="";
    docclass ="\\documentclass[";
    docclass +=font_size;
    docclass +="pt," + single_double + ",a4paper" + orientation + "," + column + "]{report}\n";
    *fout<<docclass.Data();

    //-------------------------------------------------------------------
    // standard packages
    *fout<<"\\usepackage{amsmath}\n"
         <<"\\usepackage{subfigure}\n"
         <<"\\usepackage{placeins}\n"
         <<"\\usepackage{caption2}\n"
	 <<"\\usepackage{floatflt}\n"
	 <<"\\usepackage{wrapfig}\n"
	 <<"\\usepackage{ifthen}\n"
	 <<"\\usepackage{listings}\n"
	 <<"\\usepackage{times}\n";

    //-------------------------------------------------------------------
    // putting additional packages into the document
    if(include_packages.CompareTo("")!=0)
    {
	Int_t size=0;
	TString* array=HTool::parseString(include_packages,size,",",kTRUE);
	for(Int_t i=0;i<size;i++)
	{
	    *fout<<"\\usepackage{"<<array[i].Data()<<"}\n";
	}
    }
    //-------------------------------------------------------------------


    //-------------------------------------------------------------------
    // pdf graphic
    if(!draft)*fout <<"\\usepackage[pdftex]{graphicx}\n";
    else      *fout <<"\\usepackage[pdftex,draft]{graphicx}\n";
    //-------------------------------------------------------------------

    //-------------------------------------------------------------------
    // pdf options
    if(colorlinks)*fout<<"\\usepackage[pdftex,colorlinks,plainpages=false]{hyperref}\n";
    else          *fout<<"\\usepackage[pdftex,plainpages=false]{hyperref}\n";
    *fout<<"\\pdfcompresslevel=9\n"
	 <<"\\pdfoutput=1\n";
    if(orientation.CompareTo(",landscape")==0)
    {
        cout<<"HReport::Setting up landscape format"<<endl;
        *fout<<"\\setlength{\\textwidth}{246mm}\n"
	     <<"\\setlength{\\oddsidemargin}{0in}\n"
	     <<"\\setlength{\\evensidemargin}{0in}\n";
    }
    else
    {
	*fout<<"\\setlength{\\textwidth}{159mm}\n"
	     <<"\\setlength{\\oddsidemargin}{0in}\n"
	     <<"\\setlength{\\evensidemargin}{0in}\n";
    }
    //-------------------------------------------------------------------

    //-------------------------------------------------------------------
    // useful commands for table/fig
    *fout<<"\\makeatletter\n"
         <<"\\newcommand\\figcaption{\\def\\@captype{figure}\\caption}\n"
         <<"\\newcommand\\tabcaption{\\def\\@captype{table}\\caption}\n"
	 <<"\\makeatother\n"
         <<"\\newcommand{\\goodgap}{\n"
	 <<"\\hspace{\\subfigtopskip}\n"
	 <<"\\hspace{\\subfigbottomskip}}\n"
	 <<"\\renewcommand{\\textfraction}{0.0003}\n";



    //--------------------------------------------------------------------
    // begin of document

    *fout<<"\\setcounter{tocdepth}{4}\n";
    *fout<<"\\setcounter{secnumdepth}{4}\n";
    *fout<<"\\begin{document}\n";
    *fout<<"\\lstset{tabsize=4,showspaces=false,showstringspaces=false,showtabs=false,breaklines=true,";
    *fout<<"postbreak=\\space,numbers=left,basicstyle=\\scriptsize\\tt,language=C++}\n";
    *fout<<"\\begin{titlepage}\n"
         <<"\\begin{center}\n";

    *fout<<"{\\bf {\\Huge " <<title.Data() <<"}}\n \\vspace{2cm}\n\n";
    *fout<<"{\\bf {\\Large "<<author.Data()<<"}}\n \\vspace{5cm}\n\n";
    *fout<<"{\\bf {\\Large "<<date.Data()  <<"}}\n\n";
     *fout<<"\\end{center}\n"
          <<"\\end{titlepage}\n";


    *fout<<"\\clearpage\n";

    //-------------------------------------------------------------------
    // header of each side
    *fout<<"\\pagestyle{headings}\n";
    //-------------------------------------------------------------------

    *fout<<"\\DeclareGraphicsExtensions{.jpg,.pdf,.png,.mps}\n"
    <<"\\pagenumbering{roman}\n";

    if(makeTOC)         *fout<<"\\tableofcontents\n"
                             <<"\\cleardoublepage\n";

    if(makeListFigures) *fout<<"\\listoffigures\n";
    if(makeListTables)  *fout<<"\\listoftables\n";
    if(makeListListings)*fout<<"\\lstlistoflistings\n";

    *fout<<"\\cleardoublepage\n"
    <<"\\pagenumbering{arabic}\n"
    <<"\\setlength{\\parskip}{1.0ex plus0.2ex minus0.2ex}\n"
    <<"\\clearpage\n";

}
void HReport::closeDocument()
{
    // closes the tex file
    if (fout){
	cout<<"HReport::Closing document !"<<endl;
    }
    else
    {
	Error("HReport::closeDocument()","No valid file pointer!");
	printOrder();
        exit(1);
    }

    if(!opendoc)
    {
	Error("HReport::closeDocument()","No tex document!");
        printOrder();
	exit(1);
    }

    *fout<<"\\clearpage\n"
         <<"\\end{document}\n";


    fout->close();
    delete fout;
    fout=NULL;
    setopt          =kFALSE;
    includepack     =kFALSE;
    opentex         =kFALSE;
    opendoc         =kFALSE;
    closedoc        =kTRUE;
}

void HReport::addText(TString text)
{
    // Between openDocument() and closeDocument() you can add what ever you like
    // to your report.
    // Add any text using latex commands :
    // Rules:   "\"    has to be replaced by "\\"
    //          "\n"   gives a new line in texfile (only needed if the line
    //                 should have an extract break here, otherwise it will
    //                 done automatically)
    //          "\n\n" creates a new paragraph
    //
    //    report->addText("\\chapter{Report something }\n "
    //		          "This report is very important....\n\n "
    //                    "line can be splitted in this way if your editor does not "
    //                    "support line wrapping."};
    //
    // The tex file is formated with a line width of linebreak characters.
    // (setLineBreak()). Do not use "\" as string break in input, it can cause
    // strange effects (use the above mentioned method).

    if(!opendoc)
    {
	Error("HReport::addText()","No tex document!");
        printOrder();
	exit(1);
    }

    if (fout)
    {
	writeTex(text);
    }
    else
    {
	Error("HReport::addText()","No valid file pointer!");
        printOrder();
	exit(1);
    }

}
void HReport::writeTex(TString text,Bool_t skip_curly)
{
    // remove all tabs and multible spaces from string
    // and insert line break in next space if line is longer than
    // linebreak characters. Does not break in {},[] or $$.
    // skip_curly= kFALSE will force to break in curly braces.
    // If alwaysSkipCurly() has been set skip_curly will
    // be overwritten to kTRUE.

    if(!fout)
    {
	Error("HReport::writeTex()","No valid file pointer!");
	printOrder();
	exit(1);
    }

    if(text.CompareTo("")!=0)
    {
	text.ReplaceAll('\t'," \t ");  // remove tabs
	text.ReplaceAll('\t'," ");     // remove tabs
	while(text.Contains("  "))     // remove multible spaces
	{
	    text.ReplaceAll("  "," ");
	}

	if(always_skip_curly){skip_curly=kTRUE;}

	Int_t curly  =0;
	Int_t box    =0;
	Int_t dollar =0;
	Int_t size   =text.Length();

	for(Int_t i=0;i<size;i++)
	{
	    if(skip_curly)
	    {
		if(text[i]=='{')curly++;           // count open braces
		if(text[i]=='}')curly--;           // count closing braces
	    }
	    if(text[i]=='$' && dollar==0)dollar=1; // inside math mode
	    if(text[i]=='$' && dollar==1)dollar=0; // outside math mode

	    if(text[i]=='[')box++;                 // count open braces
	    if(text[i]==']')box--;                 // count closing braces

	    if(text[i]=='\n') linect=1;

	    if(linect>=linebreak && text[i]==' ' && dollar==0 && curly==0 && box==0)
	    {   // add line break if line is to Long_t and no braces/math mode is open

		*fout<<"\n";
		linect=1;
	    }
	    else
	    {
		linect++;
		*fout<<text[i];
	    }
	}
    }
}
void HReport::makepdf()
{
    // calls pdflatex in background and copies
    // the pdf output to working dir.

    TString workingdir=gSystem->WorkingDirectory();

    cout<<"HReport::Creating pdf!"<<endl;
    TString command ="";
    TString filename= texdir + "/" + texfile;

    if(!gSystem->AccessPathName(texdir.Data()))
    {
	if(!gSystem->AccessPathName(filename.Data()))
	{
	    command="cd " + texdir + " && pdflatex -interaction batchmode " + texfile + " 2>&1 > /dev/null" + " && cd -";
	    gSystem->Exec(command.Data());
	    gSystem->Exec(command.Data());
	    gSystem->Exec(command.Data());

	    TString output=texfile;
	    output.ReplaceAll(".tex",".pdf");

	    filename= texdir + "/" + output;
	    if(!gSystem->AccessPathName(filename.Data()))
	    {
		TString filename2= workingdir + "/" + output;

		Int_t success=gSystem->CopyFile(filename.Data(),filename2.Data(),kTRUE);

		if(success)
		{
		    Error("HReport::makepdf()","Could not copy pdf file : %s  to %s !",filename.Data(),filename2.Data());
		    exit(1);
		}
	    }
	    else
	    {
		Error("HReport::makepdf()","Could not access pdf file : %s !",filename.Data());
		exit(1);
	    }
	}
	else
	{
	    Error("HReport::makepdf()","Could not access tex file : %s !",filename.Data());
	    exit(1);
	}
    }
    else
    {
	Error("HReport::makepdf()","Could not access path to texdir: %s !",texdir.Data());
        exit(1);
    }
}
void HReport::openpdf()
{
    // opens the pdf file in acroread or gv
    if(useviewer)
    {
	cout<<"HReport::Opening pdf in "<<viewer.Data()<<"!"<<endl;

	TString workingdir=gSystem->WorkingDirectory();
	TString output    =texfile;
	output.ReplaceAll(".tex",".pdf");
        TString filename  = workingdir + "/" + output;

	if(!gSystem->AccessPathName(filename.Data()))
	{
	    TString command="";
	    command=viewer + " " + filename + "&";
	    gSystem->Exec(command);
	}
	else
	{
	    Error("HReport::openpdf()","Could not access pdf file : %s !",filename.Data());
	    exit(1);
	}
    }
    else
    {
	cout<<"HReport::Cannot Opening pdf in viewer!"<<endl;
    }
}

void  HReport::figure(TCanvas* canvas,TString cap,TString shcap,TString lab,
             Float_t width,Int_t x1,Int_t y1,Int_t x2,Int_t y2,Int_t rotate,
             TString filename,Bool_t force,TString format)
{
    // Add Root graphics to the report:
    // The canvas will be saved as .ps and converted to pdf
    // by ps2pdf. The pdf pic will be stored in tex_dir.
    // Figure with caption below:
    // report->figureSideCaption(Tcanvas* canvas,                :: canvas pointer which contains the grahic
    //                "caption of figure",                       :: caption beside the figure
    //                "short caption for list of figures",       :: caption used in list of figures
    //                "label_of_figure",                         :: label for referencing of the figure
    //                0.7,                                       :: size of graphic in terms of linewidth
    //                54,30,1030,750,                            :: viewport x1,y1,x2,y2 in pixels (display ony a part or the picture)
    //                                                           :: values equal -1 shows full picture
    //                -90,                                       :: rotation (- = clock wise)
    //                "filename",                                :: output name of picture ("" will generate the
    //                                                              name from name of canvas).
    //                force);                                    :: picture will be created  no matter if keeppics option is used
    if(!fout)
    {
	Error("HReport::figure()","No valid file pointer!");
        printOrder();
	exit(1);
    }
    if(!opendoc)
    {
	Error("HReport::figure()","No tex document!");
        printOrder();
	exit(1);
    }

    if(canvas==0)
    {
        Error("HReport::figure()","Zero pointer received for Canvas!");
        exit(1);
    }
    if(filename.CompareTo("")==0)
    {   // create name automatic from canvas name
        filename=canvas->GetName();
        filename+="_auto_";
    }
    if(format.CompareTo("pdf")==0)
    {
	saveToPdf(canvas,filename,kFALSE,force);
        filename=filename + ".pdf";
    }
    if(format.CompareTo("png")==0)
    {
	saveToPng(canvas,filename,kFALSE,force);
        filename=filename + ".png";
    }

    TString scaption ="";
    TString slabel   ="";
    TString swidth   ="";
    TString sviewport="";
    TString sgraphic ="";
    TString sangle   ="";


    if(width>0)
    {   // if width is defined
	swidth =Form(",width=%5.2f\\linewidth",width);
    }
    else
    {   // take full line width
	swidth=",width=\\linewidth";
    }

    if(x1<0||y1<0||x2<0||y2<0)
    {   // if viewport is not defined
	sviewport="";
    }
    else
    {   // if viewport is defined
	sviewport=Form(",viewport=%i %i %i %i",x1,y1,x2,y2);
    }

    sangle  =Form("angle=%i",rotate);
    scaption="\\caption[" + shcap +  "]{" + cap + "}\n";
    sgraphic="\\includegraphics[" + sangle + sviewport + swidth + ",clip=true]{" + filename + "}\n";
    slabel  ="\\label{"+ lab +"}\n";

    writeTex("\\begin{figure}[\\htb]\n");
    writeTex("\\begin{center}\n");
    writeTex(sgraphic,kFALSE);
    writeTex(scaption,kFALSE);
    if(lab.CompareTo("")!=0)writeTex(slabel);
    writeTex("\\end{center}\n");
    writeTex("\\end{figure}\n");

}
void  HReport::figureSideCaption(TCanvas* canvas,TString cap,TString shcap,TString lab,
             Float_t widthpic,Float_t widthtxt,Float_t space,Int_t x1,Int_t y1,Int_t x2,Int_t y2,Int_t rotate,
             TString filename,Bool_t force,TString format)
{
    // Add Root graphics to the report:
    // The canvas will be saved as .ps and converted to pdf
    // by ps2pdf. The pdf pic will be stored in tex_dir.
    // Figure with caption on the binding side:
    // report->figureSideCaption(Tcanvas* canvas,                :: canvas pointer which contains the grahic
    //                "caption of figure",                       :: caption beside the figure
    //                "short caption for list of figures",       :: caption used in list of figures
    //                "label_of_figure",                         :: label for referencing of the figure
    //                0.7,0.2,0.02,                              :: size of graphic / caption / space in terms of linewidth
    //                54,30,1030,750,                            :: viewport x1,y1,x2,y2 in pixels (display ony a part or the picture)
    //                                                           :: values equal -1 shows full picture
    //                -90,                                       :: rotation (- = clock wise)
    //                "filename",                                :: output name of picture ("" will generate the
    //                                                              name from name of canvas).
    //                force);                                    :: picture will be created  no matter if keeppics option is used

    if(!fout)
    {
	Error("HReport::figureSideCaption()","No valid file pointer!");
        printOrder();
        exit(1);
    }

    if(!opendoc)
    {
	Error("HReport::figureSideCaption()","No tex document!");
        printOrder();
	exit(1);
    }

    if(canvas==0)
    {
        Error("HReport::figureSideCaption()","Zero pointer received for Canvas!");
        exit(1);
    }
    if(filename.CompareTo("")==0)
    {   // create name automatic from canvas name
        filename=canvas->GetName();
        filename+="_auto_";
    }

    if(format.CompareTo("pdf")==0)
    {
	saveToPdf(canvas,filename,kFALSE,force);
        filename=filename + ".pdf";
    }
    if(format.CompareTo("png")==0)
    {
	saveToPng(canvas,filename,kFALSE,force);
        filename=filename + ".png";
    }

    TString scaption ="";
    TString sviewport="";
    TString swidth   ="";
    TString sgraphic ="";
    TString sangle   ="";
    TString slabel   ="";
    TString temp     ="";

    // take full line width
    swidth=",width=\\linewidth";


    if(x1<0||y1<0||x2<0||y2<0)
    {   // if viewport is not defined
	sviewport="";
    }
    else
    {   // if viewport is defined
	sviewport=Form(",viewport=%i %i %i %i",x1,y1,x2,y2);
    }

    sangle  =Form("angle=%i",rotate);
    scaption="\\caption[" + shcap +  "]{" + cap + "}\n";
    sgraphic="\\includegraphics[" + sangle + sviewport + swidth + ",clip=true]{" + filename + "}\n";
    slabel  ="\\label{"+ lab +"}\n";

    writeTex("\\begin{figure}[\\htb]\n");
    writeTex("\\centering\n");
    temp=" \\ifthenelse{\\isodd{\\pageref{" + lab +"}}}\n";
    writeTex(temp);
    writeTex("  {%BEGIN ODD-PAGE\n");
    temp=Form("  \\begin{minipage}[c]{%5.2f\\linewidth}\n",widthtxt);
    writeTex(temp);
    writeTex("  \\centering\n");
    writeTex(scaption,kFALSE);
    if(lab.CompareTo("")!=0)writeTex(slabel);
    writeTex("  \\end{minipage}\n");
    temp=Form("  \\hspace{%5.2f\\linewidth}\n",space);
    writeTex(temp);
    temp=Form("  \\begin{minipage}[c]{%5.2f\\linewidth}\n",widthpic);
    writeTex(temp);
    writeTex(sgraphic,kFALSE);
    writeTex("  \\end{minipage}\n");
    writeTex("  }% END ODD-PAGE\n");
    writeTex("  {% BEGIN EVEN-PAGE\n");
    temp=Form("  \\begin{minipage}[c]{%5.2f\\linewidth}\n",widthpic);
    writeTex(temp);
    writeTex(sgraphic,kFALSE);
    writeTex("  \\end{minipage}\n");
    temp=Form("  \\hspace{%5.2f\\linewidth}\n",space);
    writeTex(temp);
    temp=Form("  \\begin{minipage}[c]{%5.2f\\linewidth}\n",widthtxt);
    writeTex(temp);
    writeTex("  \\centering\n");
    writeTex(scaption,kFALSE);
    if(lab.CompareTo("")!=0)writeTex(slabel);
    writeTex("  \\end{minipage}\n");
    writeTex("  }% END EVEN-PAGE\n");
    writeTex("\\end{figure}\n");

}
void HReport::table(Float_t* val,Float_t* valerr,Int_t sx,Int_t sy,
		    TString titlex,TString titley,
		    TString format,TString line,
		    TString cap,TString shcap,TString label)
{
    //  For simple formated tables a template can be created :
    //
    //  Float_t values[6]={1.1,2.2,
    //                     3.3,4.4,
    //                     5.5,6.6};
    //
    //  Float_t valerr[6]={1.1,2.2,
    //                     3.3,4.4,
    //                     5.5,6.6};
    //
    //  report->table(&values[0],&valerr[0],                      :: pointer to 1dim array value and error of value
    //                2,3,                                        :: size in x and y of the table (2x3 only)
    //	              "x1,x2","y1,y2,y3",                         :: comma separated list column header (x) and line header (y)
    //		      "|c|c|",                                    :: format of the tabel (here centered with line separation)
    //                "%5.1f $\\pm$ %5.2f & %5.1f $\\pm$ %5.2f",  :: cells are seprated by &, $\\pm$ indicates usage of errors,
    //                                                            :: formating as printf for floats (tipp: a simple % becomes \\%%)
    //		      "caption of table",                         :: caption below the table
    //                "short caption for list of tables",         :: caption used in list of tables
    //                "label_of_table");                          :: label for referencing of the table
    //
    // if valerr=0 no errors will be used. Which value will be printed with errors
    // is defined by the line format.

    if(!fout)
    {
	Error("HReport::table()","No valid file pointer!");
        printOrder();
        exit(1);
    }

    if(!opendoc)
    {
	Error("HReport::table()","No tex document!");
        printOrder();
	exit(1);
    }

    if(val==0)
    {
	Error("HReport::table()","Pointer to data is zero!");
        exit(1);
    }

    if(titlex.CompareTo("")==0||titley.CompareTo("")==0||line.CompareTo("")==0)
    {
      	Error("HReport::table()","column or line definitions are empty!");
        exit(1);
    }

    if(sx>10)
    {
	Warning("HReport::table()","More than 10 x values not supported!");
    }

    Int_t isx,isy,iline;
    TString* arrayx=parseString(titlex,isx  ,",",kFALSE,kFALSE);
    TString* arrayy=parseString(titley,isy  ,",",kFALSE,kFALSE);
    TString* arrayf=parseString(line  ,iline,"&",kFALSE,kFALSE);

    if(isx!=sx || isy!=sy || iline!=sx)
    {
	Error("HReport::table()","Dimension of array does not match format string / title strings!");
        exit(1);
    }

    openTable(format);

    //----header of table-------------
    *fout<<"\\hline \n";
    *fout<<" & ";
    for(Int_t x=0;x<iline;x++)
    {
	          *fout<<arrayx[x].Data();
	if(x<sx-1)*fout<<" & ";
    }
    *fout<<" \\\\ \n";
    *fout<<"\\hline \n";
    //--------------------------------
        
    //----body of table---------------
    for(Int_t y=0;y<sy;y++)
    {  // loop
	*fout<<arrayy[y].Data()<<" & ";
        for(Int_t x=0;x<iline;x++)
	{
	    if(arrayf[x].Contains("\\pm")==0)
	    {
		*fout<<Form(arrayf[x].Data(),val[y*sx+x]);
	    }
	    else if(arrayf[x].Contains("\\pm")!=0&&valerr!=0)
	    {
		*fout<<Form(arrayf[x].Data(),val[y*sx+x],valerr[y*sx+x]);
	    }
	    else
	    {   // in case there is \\pm but no valerr pointer
                *fout<<Form(arrayf[x].Data(),val[y*sx+x]);
	    }
            if(x<sx-1)*fout<<" & ";
        }
        *fout<<" \\\\ \n";
	*fout<<"\\hline \n";
    }
    //--------------------------------
    closeTable(cap,shcap,label);
}
void HReport::openTable(TString format)
{
    // Header of the table environment.
    // format specifies the tabel ("|c|c|l|")

    if(!fout)
    {
	Error("HReport::openTable()","No valid file pointer!");
        printOrder();
        exit(1);
    }

    if(!opendoc)
    {
	Error("HReport::openTable()","No tex document!");
        printOrder();
	exit(1);
    }

    if(format.CompareTo("")==0)
    {
      	Error("HReport::openTable()","Format string is empty!");
        exit(1);
    }

    *fout<<"\\begin{table}[\\htb] \n";
    *fout<<"\\centering \n";
    *fout<<"\\begin{tabular}{"<<format.Data()<<"} \n";
}
void HReport::closeTable(TString cap,TString shcap,TString label)
{
    //   "cap"      :: caption below the table
    //   "shcap"    :: caption used in list of tables
    //   "label"    :: label for referencing of the table
   
    if(!fout)
    {
	Error("HReport::closeTable()","No valid file pointer!");
        printOrder();
        exit(1);
    }

    if(!opendoc)
    {
	Error("HReport::closeTable()","No tex document!");
        printOrder();
	exit(1);
    }

    if(cap.CompareTo("")==0)
    {
      	Error("HReport::closeTable()","Format string is empty!");
        exit(1);
    }
    *fout<<"\\end{tabular} \n";

    TString scaption ="";
    TString slabel   ="";

    scaption="\\caption[" + shcap +  "]{" + cap + "}\n";
    slabel  ="\\label{"+ label +"}\n";

    writeTex(scaption,kFALSE);
    writeTex(slabel,kFALSE);
    writeTex("\\end{table} \n");
}
void HReport::openAppendix()
{
    // opens the appendix environment. All text added until closeAppendix()
    // will be contained in appendix.
    if(!fout)
    {
	Error("HReport::openAppendix()","No valid file pointer!");
        printOrder();
        exit(1);
    }
    if(!opendoc)
    {
	Error("HReport::openAppendix()","No tex document!");
        printOrder();
	exit(1);
    }
    *fout<<"\\clearpage \n ";
    *fout<<"\\begin{appendix}\n";
}
void HReport::closeAppendix()
{
    // close the appendix environment. All text added between openAppendix()
    // and closeAppendix() will be contained in appendix.
    if(!fout)
    {
	Error("HReport::closeAppendix()","No valid file pointer!");
        printOrder();
        exit(1);
    }
    if(!opendoc)
    {
	Error("HReport::closeAppendix()","No tex document!");
        printOrder();
	exit(1);
    }
    *fout<<"\\end{appendix}\n\n";
}
void HReport::includeSource(TString source,TString cap,TString shcap,TString label,Int_t firstline,Int_t lastline,TString option)
{
    //   Include C++ source files / ROOT macros into your code:
    //   This is a wrapper arround listings package of tex. Not all
    //   features are supported but the documentation of the package
    //   will help to get the maximum out of the package.
    //
    //   report->includeSource(TString source,                     :: path/filename of the source (relative path accpeted)
    //                         TString cap,                        :: caption used above the listing
    //                         TString shcap,                      :: caption used for list of listings
    //                         TString label,                      :: label for referencing the listing
    //                         Int_t firstline,                    :: first line of code listed
    //                         Int_t lastline,                     :: last line of code listed
    //                         TString option);                    :: additional options can be taken into account
    //
    // source and cap are mandatory, the is optional. Part of the source can be listed with firstline/lastline arguments.
    // firstline==-1 && lastline!=1 will list from beginning of file to lastline. firstline!=-1 && lastline==-1 will list
    // from firstline to end of file. Both equal -1 will list the full file (default).

    if(!fout)
    {
	Error("HReport::includeSource()","No valid file pointer!");
        printOrder();
        exit(1);
    }
    if(!opendoc)
    {
	Error("HReport::includeSource()","No tex document!");
        printOrder();
	exit(1);
    }

    if(source.CompareTo("")==0||cap.CompareTo("")==0)
    {
 	Error("HReport::includeSource()","source file or caption not specified!");
        exit(1);
    }

    TString caption="";
    TString lines  ="";
    TString opt    ="";
    TString lab    ="";

    if(shcap.CompareTo("")!=0)
    {
	caption="caption={["+ shcap + "] " + cap + "}";
    }
    else
    {
	caption="caption={" + cap + "}";
    }

    if(firstline!=-1&&lastline!=-1)
    {   // both values specified
	lines=", firstline=";
	lines+=firstline;
	lines+=", lastline=";
        lines+=lastline;
    }
    else if(firstline!=-1&&lastline==-1)
    {   // only first value specified
     	lines=", firstline=";
	lines+=firstline;
    }
    else if(firstline==-1&&lastline!=-1)
    {   // only value value specified
     	lines=", lastline=";
	lines+=lastline;
    }

    if(option.CompareTo("")!=0)
    {
      opt=", " + option;
    }

    if(label.CompareTo("")!=0)
    {
      opt=", label=" + label;
    }

    *fout<<"\\lstinputlisting["<<caption.Data()<<lab.Data()<<lines.Data()<<opt.Data()<<"]{"<<source.Data()<<"}\n";

}
void HReport::printOrder()
{
    cout<<"#------------------------------------------------------------------------------"<<endl;
    cout<<"# USEAGE of HReport"<<endl;
    cout<<"#    HReport* report=new HReport(\"report\",\"report\");"<<endl;
    cout<<"#    report->setOption(\"......\");       (optional)"<<endl;
    cout<<"#    report->includePackages(\"......\"); (optional)"<<endl;
    cout<<"#    report->openTex(\"......\");"<<endl;
    cout<<"#    report->openDocument();"<<endl;
    cout<<"#"<<endl;
    cout<<"#    add your text,figures,tables() etc.  (optional)"<<endl;
    cout<<"#"<<endl;
    cout<<"#    report->openAppendix();              (optional)"<<endl;
    cout<<"#    add something                        (optional)"<<endl;
    cout<<"#    report->closeAppendix();             (optional)"<<endl;
    cout<<"#    report->closeDocument();"<<endl;
    cout<<"#    report->makepdf();"<<endl;
    cout<<"#    report->openpdf();                   (optional)"<<endl;
    cout<<"#------------------------------------------------------------------------------"<<endl;

}
Int_t HReport::saveToPdf(TCanvas* canvas,TString filename,Bool_t separate,Bool_t force)
{
    // saves a canvas to pdf file by storing it as ps and calling
    // ps2pdf to convert it to pdf. The ps file will be removed.
    // canvas is the pointer of the canvas which you want to save
    // path is the path to the directory where you want to store the pdf
    // filename is the name of the pdf which wou create
    // separate=kTRUE will loop over all pads and save the pictures single
    // (path/filename_i) separate=kFALSE as the canvas in one pdf.

    if(keeppics&&!force) return 0;

    if(canvas==0)
    {
        Error("HReport::saveToPdf()","Zero pointer received for Canvas!");
        exit(1);
    }
    if(filename.CompareTo("")==0)
    {   // create name automatic from canvas name
        filename=canvas->GetName();
        filename+="_auto_";
    }

    TString File    =filename;
    TString Path    =texdir;
    Path            =Path + "/";
    TString command ="";
    TString totalin ="";
    TString totalout="";

    File.ReplaceAll(".ps","");
    File.ReplaceAll(".pdf","");
    if(!separate)
    {
        totalin = Path + File + ".ps";
        totalout= Path + File + ".pdf";
        canvas->SaveAs(totalin.Data());
        command= "ps2pdf " + totalin + " " + totalout;
        gSystem->Exec(command.Data());
        command= "rm " + totalin;
        gSystem->Exec(command.Data());
    }else{
        TString subpad="";
        Int_t npads= canvas->GetListOfPrimitives()->GetSize();
        for(Int_t i=1;i<=npads;i++)
        {
            subpad=i;
            totalin = Path + File + "_" + subpad + ".ps";
            totalout= Path + File + "_" + subpad + ".pdf";
            canvas->cd(i);
            gPad->SaveAs(totalin.Data());
            command= "ps2pdf " + totalin + " " + totalout;
            gSystem->Exec(command.Data());
            command= "rm " + totalin;
            gSystem->Exec(command.Data());
        }
    }
    return 0;
}
Int_t HReport::saveToPng(TCanvas* canvas,TString filename,Bool_t separate,Bool_t force)
{
    // saves a canvas to png file by storing it as ps and calling
    // conver to convert it to png. The ps file will be removed.
    // canvas is the pointer of the canvas which you want to save
    // path is the path to the directory where you want to store the png
    // filename is the name of the png which wou create
    // separate=kTRUE will loop over all pads and save the pictures single
    // (path/filename_i) separate=kFALSE as the canvas in one png.

    if(keeppics&&!force) return 0;

    if(canvas==0)
    {
        Error("HReport::saveToPdf()","Zero pointer received for Canvas!");
        exit(1);
    }
    if(filename.CompareTo("")==0)
    {   // create name automatic from canvas name
        filename=canvas->GetName();
        filename+="_auto_";
    }

    TString File    =filename;
    TString Path    =texdir;
    Path            =Path + "/";
    TString command ="";
    TString totalin ="";
    TString totalout="";

    File.ReplaceAll(".ps","");
    File.ReplaceAll(".png","");
    if(!separate)
    {
        totalin = Path + File + ".ps";
        totalout= Path + File + ".png";
        canvas->SaveAs(totalin.Data());
        command= "convert " + totalin + " " + totalout;
        gSystem->Exec(command.Data());
        command= "rm " + totalin;
        gSystem->Exec(command.Data());
    }else{
        TString subpad="";
        Int_t npads= canvas->GetListOfPrimitives()->GetSize();
        for(Int_t i=1;i<=npads;i++)
        {
            subpad=i;
            totalin = Path + File + "_" + subpad + ".ps";
            totalout= Path + File + "_" + subpad + ".png";
            canvas->cd(i);
            gPad->SaveAs(totalin.Data());
            command= "convert " + totalin + " " + totalout;
            gSystem->Exec(command.Data());
            command= "rm " + totalin;
            gSystem->Exec(command.Data());
        }
    }
    return 0;
}

TString*  HReport::parseString(TString options,Int_t& size,TString separator,Bool_t tolower,Bool_t removespace)
{
    // loops over TString options and find substrings separated by separator
    // and puts them to an array of TStrings. size will hold the size of this
    // array and the pointer to the array is returned. If tolower is kTRUE options
    // will be made toLower. if removespace=kTRUE spaces will be remove from the string.
    if(tolower)options.ToLower();
    if(removespace)options.ReplaceAll(" ","");
    Ssiz_t len=options.Length();
    TString* sarray=0;
    size=0;
    if(len!=0)
    {
        TObjArray* array = options.Tokenize(separator.Data());
        sarray = new TString [array->GetEntries()];

	for(Int_t i=0;i<array->GetEntries();i++)
        {
	    sarray[i]= ((TObjString*)(array->At(i)))->GetString();
	}

	size=array->GetEntries();
	array->Delete();
        delete array;
    }
    return sarray;
}

