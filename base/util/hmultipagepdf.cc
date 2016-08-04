#include "hmultipagepdf.h"

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
//
// Simple helper class which alows to build multipage pdfs..
//
// USAGE:
//
//   HMultiPagePDF multipage("multi.pdf","landscape");
//   multipage.addPage(TCanvas* c,"my page title");
//   multipage.close();
//
////////////////////////////////////////////////////////////////////////////


ClassImp(HMultiPagePDF)

HMultiPagePDF::HMultiPagePDF(TString out,TString option)
{
    name = out;
    option.ToLower();

    if(option=="landscape"){

	c = new TCanvas("multpage","multipage",3200,2000);
	gStyle->SetPaperSize(27,20);
    }
    else if(option=="potrait"){

	c = new TCanvas("multpage","multipage",2000,3200);
	gStyle->SetPaperSize(20,27);
    } else {
	cout<<"HMultiPagePDF() : Unkown option \""<<option<<"\"! will use landscape"<<endl;
	c = new TCanvas("multpage","multipage",2000,3200);
	gStyle->SetPaperSize(20,27);
    }
    c->Print(Form("%s(" ,name.Data()));
    pageCt = 0 ;
}

HMultiPagePDF::~HMultiPagePDF()
{

}

void HMultiPagePDF::addPage(TCanvas* input, TString title )
{

    c->Clear();
    c->cd();

    input->DrawClonePad();
    c->Update();

    if(title=="")      c->Print(Form("%s" ,name.Data()));
    else if(title=="title") c->Print(Form("%s" ,name.Data()),Form("Title:%s",input->GetTitle()));
    else if(title=="name")  c->Print(Form("%s" ,name.Data()),Form("Title:%s",input->GetName()));
    else if(title=="name")  c->Print(Form("%s" ,name.Data()),Form("Title:%s",input->GetName()));
    else  c->Print(Form("%s" ,name.Data()),Form("Title:%s",title.Data()));

    pageCt++ ;

}

void HMultiPagePDF::close(){
    c->Clear();
    c->Print(Form("%s)",name.Data()));
}

