//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//
// Peter's Tools
// 
//            Author: Peter W. Zumbruch
//           Contact: P.Zumbruch@gsi.de
//           Created: Mar 21, 2002
// 
// File: $RCSfile: ptools.cc,v $ 
// Version: $Revision: 1.79 $
// Modified by $Author: halo $ on $Date: 2009-07-23 14:50:51 $  
////////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

#include "TArrayC.h"
#include "TArrayD.h"
#include "TArrayF.h"
#include "TArrayI.h"
#include "TArrayL.h"
#include "TArrayS.h"
#include "TApplication.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TError.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "THashList.h"
#include "THStack.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TLine.h"
#include "TMath.h"
#include "TObject.h"
#include "TObjString.h"
#include "TPaveText.h"
#include "TPad.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TString.h"
#include "TStyle.h"
#include "TTree.h"
#include "TUnixSystem.h"

#include "ptools.h"
#include <cmath>

ClassImp(PTools)

////////////////////////////////////////////////////////////////////////////////



PTools::PTools()
{
    ;
}
PTools::~PTools()
{
    ;
}
TPad* PTools::getDividedSubPad(TCanvas *canv, 
				 Int_t xpads, 
				 Int_t ypads) 
{
  // divides given canvas mycanvas into xpad times ypads pads 
  // returns pointer to pad of divided TPad

  TString title;
  if (canv) title = canv->GetTitle();
  else title ="";

  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title.Data(), (Char_t*)0, 0, (TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(TCanvas *canv, 
			       Int_t xpads, 
			       Int_t ypads,			       
			       const Char_t * title) 
{
  // divides given canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // returns pointer to pad of divided TPad
  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, (Char_t*)0, 0, (TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(TCanvas *canv, 
			       Int_t xpads, 
			       Int_t ypads,			       
			       const Char_t * title,
			       const Char_t * filename) 
{
  // divides given canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // returns pointer to pad of divided TPad
  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, filename, 0, (TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(TCanvas *canv, 
			       Int_t xpads, 
			       Int_t ypads, 
			       const Char_t * title, 
			       const Char_t * filename, 
			       Int_t nentries)
{
  // divides given canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // returns pointer to pad of divided TPad

  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, filename, nentries,(TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(TCanvas *canv,
			       Int_t xpads, 
			       Int_t ypads, 
			       const Char_t * title, 
			       const Char_t * filename,
			       Int_t nentries, 
			       const Char_t * info)
{
  // divides given canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // creates an info box at the right of the canvas
  // returns pointer to pad of divided TPad

  TPaveText *infotext = new TPaveText(0,0,1,1);
  infotext->SetTextSize(10);
  infotext->AddText(info);
  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, filename, nentries, infotext );
};

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(TCanvas *canv, 
			       Int_t xpads, 
			       Int_t ypads, 
			       const Char_t * title, 
			       const Char_t * filename, 
			       Int_t nentries, 
			       const TString info)
{
  // divides given canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // creates an info box at the right of the canvas
  // returns pointer to pad of divided TPad

  return (!canv) ? NULL : getDividedSubPad(canv , xpads, ypads, title, filename, nentries, info.Data());
};

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads,
			       Int_t xsize, Int_t ysize) 
{
  // divides new canvas mycanvas of size xsizy * ysize 
  // into xpad times ypads pads 
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetWindowSize(xsize,ysize);
  canv->Resize();
  canv->SetTitle(canv->GetName());

  TString title;

  if (canv) title = canv->GetTitle();
  else title ="";

  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title.Data(), (Char_t*)0, 0, (TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads,
			       Int_t xsize, Int_t ysize,
			       const Char_t * title) 
{
  // divides new canvas mycanvas of size xsizy * ysize 
  // into xpad times ypads pads 
  // adds a header with title 
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetWindowSize(xsize,ysize);
  canv->Resize();
  canv->SetTitle(title);

  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, (Char_t*)0, 0, (TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads,	
			       Int_t xsize, Int_t ysize,		       
			       const Char_t * title,
			       const Char_t * filename) 
{
  // divides new canvas mycanvas of size xsizy * ysize 
  // into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetWindowSize(xsize,ysize);
  canv->Resize();
  canv->SetTitle(title);
  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, filename, 0, (TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads,
			       Int_t xsize, Int_t ysize, 
			       const Char_t * title, 
			       const Char_t * filename, 
			       Int_t nentries)
{
  // divides new canvas mycanvas of size xsizy * ysize 
  // into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetWindowSize(xsize,ysize);
  canv->Resize();
  canv->SetTitle(title);
  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, filename, nentries,(TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads,
			       Int_t xsize, Int_t ysize, 
			       const Char_t * title, 
			       const Char_t * filename,
			       Int_t nentries, 
			       const Char_t * info)
{
  // divides new canvas mycanvas of size xsizy * ysize 
  // into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // creates an info box at the right of the canvas
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetWindowSize(xsize,ysize);
  canv->Resize();
  canv->SetTitle(title);
  TPaveText *infotext = new TPaveText(0,0,1,1);
  infotext->SetTextSize(10);
  infotext->AddText(info);
  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, filename, nentries, infotext );
};

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
 			       Int_t ypads,
			       Int_t xsize, Int_t ysize, 
			       const Char_t * title, 
			       const Char_t * filename, 
			       Int_t nentries, 
			       const TString info)
{
  // divides new canvas mycanvas of size xsizy * ysize 
  // into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // creates an info box at the right of the canvas
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetWindowSize(xsize,ysize);
  canv->Resize();
  canv->SetTitle(title);
  return (!canv) ? NULL : getDividedSubPad(canv , xpads, ypads, title, filename, nentries, info.Data());
};
////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
 			       Int_t ypads,
			       Int_t xsize, Int_t ysize, 
			       const Char_t * title, 
			       const Char_t * filename, 
			       Int_t nentries, 
			       TPaveText *info)
{
  // divides new canvas mycanvas of size xsizy * ysize 
  // into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // creates an info box at the right of the canvas
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetWindowSize(xsize,ysize);
  canv->Resize();
  canv->SetTitle(title);
  return (!canv) ? NULL : getDividedSubPad(canv , xpads, ypads, title, filename, nentries, info);
};

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads) 
{
  // divides new canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetTitle(canv->GetName());

  TString title;

  if (canv) title = canv->GetTitle();
  else title ="";

  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title.Data(), (Char_t*)0, 0, (TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads,			       
			       const Char_t * title) 
{
  // divides new canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetTitle(title);

  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, (Char_t*)0, 0, (TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads,			       
			       const Char_t * title,
			       const Char_t * filename) 
{
  // divides new canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetTitle(title);
  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, filename, 0, (TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads, 
			       const Char_t * title, 
			       const Char_t * filename, 
			       Int_t nentries)
{
  // divides new canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetTitle(title);
  return getDividedSubPad(canv, xpads, ypads, title, filename, nentries,(TPaveText*)0 );
}

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads, 
			       const Char_t * title, 
			       const Char_t * filename,
			       Int_t nentries, 
			       const Char_t * info)
{
  // divides new canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // creates an info box at the right of the canvas
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetTitle(title);
  TPaveText *infotext = new TPaveText(0,0,1,1);
  infotext->SetTextSize(10);
  TString str(info);
  while(str.Length() != 0)
    {
      if (str.First("/n") != str.Length())
	{
	  infotext->AddText((str(0,str.First("/n")-1)).Data());
	  str=str(str.First("/n")+1,str.Length());
	}
      else
	{
	  infotext->AddText(info);
	  str="";
	}
    }
  return (!canv) ? NULL : getDividedSubPad(canv, xpads, ypads, title, filename, nentries, infotext );
};

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads, 
			       const Char_t * title, 
			       const Char_t * filename, 
			       Int_t nentries, 
			       const TString info)
{
  // divides new canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // creates an info box at the right of the canvas
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use

  TCanvas *canv = new TCanvas();
  canv->SetTitle(title);
  return getDividedSubPad(canv , xpads, ypads, title, filename, nentries, info.Data());
};

////////////////////////////////////////////////////////////////////////////////

TPad* PTools::getDividedSubPad(Int_t xpads, 
			       Int_t ypads, 
			       const Char_t * title, 
			       const Char_t * filename, 
			       Int_t nentries, 
			       TPaveText *info)
{
  // divides new canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // creates an info box at the right of the canvas
  // returns pointer to pad of divided TPad
  // memory leak! only for CINT use
  
  TCanvas *canv = new TCanvas();
  canv->SetTitle(title);
  return (!canv) ? NULL : getDividedSubPad(canv , xpads, ypads, title, filename, nentries, info);
};

////////////////////////////////////////////////////////////////////////////////
TPad* PTools::getDividedSubPad(TCanvas   *mycanvas,
			       Int_t      xpads, 
			       Int_t      ypads, 
			       const Char_t  *titlehead, 
			       const Char_t  *mytextfilename, 
			       Int_t      nentries, 
			       TPaveText *info)
{
  // divides given canvas mycanvas into xpad times ypads pads 
  // adds a header with title 
  // containing filename and 
  // number of entries
  // creates an info box at the right of the canvas
  // returns pointer to pad of divided TPad

  if (!mycanvas) return NULL;

  Char_t name[200];
  mycanvas->cd();
  sprintf(name,"%s_header",gPad->GetName());
  mycanvas->SetFillColor(0);
  
  TPad *mycanvas_header = new TPad(name,name,0.01,0.95,0.99,0.99);
  mycanvas_header->Draw();
  mycanvas_header->cd();
  
  printHeader(titlehead,mytextfilename,nentries,mycanvas_header);
  
  mycanvas->cd();
  
  TPad *mycanvas_body;
  
  if (info)
    {
      sprintf(name,"%s_info",gPad->GetName());
      TPad *mycanvas_info = new TPad(name,name,0.80,0.01,0.99,0.94);
      mycanvas_info->Draw();
      mycanvas_info->cd();
      gPad->SetFillStyle(4000);
      
      info->SetX1NDC(0.051);
      info->SetY1NDC(0.02);
      info->SetX2NDC(0.95);
      info->SetY2NDC(0.98);
      info->SetTextSize(0.08);
      info->SetBorderSize(0);
//       while (info->GetListOfLines()->LastIndex() < 2*(Int_t)((1/info->GetTextSize())-2)) info->AddText(" ");
      info->SetFillColor(0);
      info->Draw();
      
      mycanvas->cd();
      
      sprintf(name,"%s_body",gPad->GetName());
      
      mycanvas_body = new TPad(name,name,0.01,0.01,0.79,0.94);
    }
  else 
    {
      sprintf(name,"%s_body",gPad->GetName());
      mycanvas_body = new TPad(name,name,0.01,0.01,0.99,0.94);
    }
  
  mycanvas_body->Draw();
  mycanvas_body->cd();
  gPad->SetFillStyle(4000);
  
  if (xpads > 1 || ypads > 1) 
    {
      mycanvas_body->Divide(xpads,ypads);
      mycanvas_body->cd(1);
    }

  return mycanvas_body;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::printHeader(const Char_t *titlehead, const Char_t *mytextfilename, Int_t nentries, TPad *parentPad)
{
  // prints header of canvas containing in the parent pad
  // header contains filename and number of entries
 
  if (!parentPad) gPad->cd();
  else parentPad->cd();

  const TDatime daytime;
  Char_t texText[200];

  sprintf(texText,"%s",(titlehead)?titlehead:" ");
  TLatex *tex = new TLatex(0.01,0.15,texText);
  tex->SetTextSize(0.35);
  tex->SetLineWidth(2);
  tex->Draw();

  if (mytextfilename!=0 && nentries!=0)
    {
      sprintf(texText,"%i Events - File %s",nentries,mytextfilename);
    }
  else if (nentries==0 && mytextfilename==0)
    {
      sprintf(texText," ");
    }
  else if (nentries==0 && mytextfilename!=0)
    {
      sprintf(texText,"File %s",mytextfilename);
    }
  else 
    {
      sprintf(texText,"%i Events",nentries);
    }

  TLatex *tex2 = new TLatex(0.01,0.6,texText);
  tex2->SetTextSize(0.35);
  tex2->SetLineWidth(2);
  tex2->Draw();
  
  sprintf(texText,"%s",daytime.AsSQLString());

  TLatex *tex3 = new TLatex(0.85,0.15,texText);
  tex3->SetTextSize(0.35);
  tex3->SetLineWidth(2);
  tex3->Draw();

  return;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::evalStopwatch(TStopwatch *timer, Int_t nevts)
{
  // evaluates Stopwatch for given number of events 
  // also plots the machine where it was running on
  
  Double_t realtime=(*timer).RealTime();
  Double_t cputime =(*timer).CpuTime();
  Int_t    realHour=(Int_t) ((realtime/3600));
  Int_t    realMin =(Int_t) (((realtime-realHour*3600)/60));
  Int_t    realSec =(Int_t) ((realtime-realHour*3600-realMin*60));
  Int_t    realmSec=(Int_t) (((realtime - (Int_t) (realtime-realHour*3600-realMin*60))*1000));
  Int_t     cpuHour=(Int_t) (( cputime/3600));
  Int_t     cpuMin =(Int_t) ((( cputime- cpuHour*3600)/60));
  Int_t     cpuSec =(Int_t) (( cputime- cpuHour*3600- cpuMin*60));
  Int_t     cpumSec=(Int_t) ((( cputime - (Int_t) ( cputime- cpuHour*3600- cpuMin*60))*1000));
  
  printf("------------------------------------------------------\n");
  printf("Events processed: %i\n",nevts);  
  printf("Real time(hh:mm:ss.ms): %02i:%02i:%02i.%03i\n",
	 realHour,realMin,realSec,realmSec);
  printf("Cpu time (hh:mm:ss.ms): %02i:%02i:%02i.%03i\n", 
	 cpuHour, cpuMin, cpuSec, cpumSec);
  if (nevts && cputime >0) printf("cpu  Performance: %f s/ev or %f.2 ev/s \n",(float) ( cputime/nevts),(float) (nevts/ cputime));
  if (nevts && realtime>0) printf("real Performance: %f s/ev or %f.2 ev/s \n",(float) (realtime/nevts),(float) (nevts/realtime));
  gSystem->Exec("cat /proc/cpuinfo | perl -ne 'print if(/cpu MHz/i || /model name/i )'");
  return;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::setTitleArts( TH1* histogramm, const Char_t *xtitle, const Char_t *ytitle, TString color, Int_t type)
{
  // sets titles of x axis, y axis, fill color and font type for histogramm
  // with colors "black","red","bright green","bright blue","yellow","hot pink","aqua","green","blue","default"

  const TString col[11]={"","black","red","bright green","bright blue","yellow","hot pink","aqua","green","blue","default"};

  color.ToLower();
  Int_t i;
  for (i = 1; i <= 10; i++)
    {
      if (color.CompareTo(col[i])==0) break;
    }
  if (i==11) 
    {
      Info("setTitleArts","no valid name for color chosen %s ... setting to default: %s",color.Data(),col[4].Data());
      i=4;
    }

  setTitleArts( histogramm, xtitle, ytitle, i , type);
}

////////////////////////////////////////////////////////////////////////////////

void PTools::setTitleArts( TH1* histogramm, const TString xtitle, const TString ytitle, TString color, Int_t type)
{
  // sets titles of x axis, y axis, fill color and font type for histogramm
  // with colors "black","red","bright green","bright blue","yellow","hot pink","aqua","green","blue","default"
  setTitleArts( histogramm, xtitle.Data(), ytitle.Data(), color, type);
}

////////////////////////////////////////////////////////////////////////////////

void PTools::setTitleArts( TH1* histogramm, const TString xtitle, const TString ytitle, 
			   Int_t fillColor, Int_t titleFont)
{
  // sets titles of x axis, y axis, fill color and font type for histogramm
  setTitleArts( histogramm, xtitle.Data(), ytitle.Data(), fillColor, titleFont);
}

////////////////////////////////////////////////////////////////////////////////

void PTools::setTitleArts( TH1* histogramm, const Char_t *xtitle, const Char_t * ytitle, 
			   Int_t fillColor, Int_t titleFont)
{
  // sets titles of x axis, y axis, fill color and font type for histogramm
  // with colors "black","red","bright green","bright blue","yellow","hot pink","aqua","green","blue","default"

  histogramm->SetXTitle(xtitle);
  histogramm->SetYTitle(ytitle);
  if (fillColor >= 0 ) histogramm->SetFillColor(fillColor);
  ((TAxis*)histogramm->GetXaxis())->CenterTitle();
  ((TAxis*)histogramm->GetXaxis())->SetTitleFont(titleFont);
  ((TAxis*)histogramm->GetXaxis())->SetTitleColor(1);
  ((TAxis*)histogramm->GetXaxis())->SetTitleOffset(1.2);
  ((TAxis*)histogramm->GetYaxis())->SetTitleFont(titleFont);
  ((TAxis*)histogramm->GetYaxis())->SetTitleOffset(1.2);
  ((TAxis*)histogramm->GetYaxis())->SetTitleColor(1);
}

////////////////////////////////////////////////////////////////////////////////

void PTools::setTitleArts( THStack* histogrammStack, const TString xtitle, const TString ytitle, 
			   Int_t titleFont)
{
  // sets titles of x axis, y axis, font type for histogramm for histogramm stack
  
  setTitleArts( histogrammStack, xtitle.Data(), ytitle.Data(), titleFont);
}

////////////////////////////////////////////////////////////////////////////////

void PTools::setTitleArts( THStack* histogrammStack, const Char_t *xtitle, const Char_t * ytitle, 
			   Int_t titleFont)
{
  // sets titles of x axis, y axis, font type for histogramm for histogramm stack
  
  ((TAxis*)histogrammStack->GetXaxis())->SetTitle(xtitle);
  ((TAxis*)histogrammStack->GetYaxis())->SetTitle(ytitle);
  ((TAxis*)histogrammStack->GetXaxis())->CenterTitle();
  ((TAxis*)histogrammStack->GetXaxis())->SetTitleFont(titleFont);
  ((TAxis*)histogrammStack->GetXaxis())->SetTitleOffset(1.2);
  ((TAxis*)histogrammStack->GetYaxis())->SetTitleFont(titleFont);
  //  ((TAxis*)histogrammStack->GetYaxis())->SetTitleOffset(1.2);
}

////////////////////////////////////////////////////////////////////////////////

TCanvas* PTools::drawCanvas(const Char_t * name, 
			    const Char_t * title, 
			    TH1 **histarray, 
			    Int_t xsize, 
			    Int_t ysize)
{
  // draws default canvas divided xsize times ysize and fills it with the contents of histarray[i] at pad(i) using plot option
  // only useful for 6 pads!!
#ifndef __CINT__
  static Int_t calls;
#else
  Int_t calls=0;
#endif

  TCanvas *canv = new TCanvas(name,title,0+calls*25,0+calls*25,1000,800);

  canv->Divide(xsize,ysize);

  for (Int_t time=0 ;time  < 6 ; time++)
    {
      canv->cd(time+1);
      if (histarray[time]->GetEntries()>0)
	{
	  histarray[time]->DrawCopy();
	  gPad->SetLogy();
	}
      else delete gPad;
    }
  
  calls++;
  return canv;
}

////////////////////////////////////////////////////////////////////////////////

TCanvas* PTools::draw32Canvas(const Char_t * name, const Char_t * title, TH1F *histarray[])
{
  // draws default canvas divided 3 times 2 and fills it with the contents of histarray[i] at pad(i) using plot option

#ifndef __CINT__
  static Int_t calls;
#else
  Int_t calls=0;
#endif

  TCanvas *canv = new TCanvas(name,title,0+calls*25,0+calls*25,1000,800);
  canv->Divide(3,2);

  for (Int_t time=0 ;time  < 6 ; time++)
    {
      canv->cd(time+1);
      if (histarray[time]->GetEntries()>0)
	{
	  histarray[time]->DrawCopy();
	  gPad->SetLogy();
	}
      else delete gPad;
    }
  
  calls++;
  return canv;
}

////////////////////////////////////////////////////////////////////////////////

TCanvas* PTools::draw32CanvasTH2(const Char_t * name, const Char_t * title, TH2F *histarray[], const Char_t * option)
{
  // draws default canvas divided 3 times 2 and fills it with the contents of histarray[i] at pad(i) using plot option "option"

#ifndef __CINT__
  static Int_t calls;
#else
  Int_t calls=0;
#endif

  TCanvas *canv = new TCanvas(name,title,0+calls*25,0+calls*25,1000,800);
  canv->Divide(3,2);

  for (Int_t time=0 ;time  < 6 ; time++)
    {
      canv->cd(time+1);

      if (histarray[time]->GetEntries()>0)
	{
	  if (!option) histarray[time]->DrawCopy();
	  else histarray[time]->DrawCopy(option);
	  //gPad->SetLogz();
	}
      else {}// delete gPad;
    }
  
  calls++;
  return canv;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::cutEnd(Char_t * infi,const Char_t * end)
{
  // cuts from infi end from the end

  Char_t textfile[200];
  sprintf(textfile,"tempvar=\"%s\" ; echo ${tempvar%%%s}",infi,end);
  Char_t *tmp = myexec(textfile);
  sprintf(infi,"%s",myexec(textfile));
  delete[] tmp;
}

////////////////////////////////////////////////////////////////////////////////

Char_t * PTools::myexec(const Char_t * in)
{
  // executes command in
  // and returns Char_t pointer to result string
  // user have to delete return string

  Char_t *tmp = new Char_t[1000];
  Char_t textfile[1000];
  FILE *pipe = gSystem->OpenPipe(in,"r");
  Int_t n=fscanf(pipe,"%s",textfile);
  if(n==0)cout<<"no value read back!"<<endl;
  gSystem->ClosePipe(pipe);
  sprintf(tmp,"%s",textfile);
  return tmp;
}

////////////////////////////////////////////////////////////////////////////////

TString PTools::myexec(TString in)
{
  // executes command in
  // and returns Char_t pointer to result string
  // user have to delete return string

  const Char_t* tmp = myexec(in.Data());
  TString st(tmp);
  delete tmp;
  return TString(st);
}

////////////////////////////////////////////////////////////////////////////////

void PTools::mdb(Int_t called)
{
  // small minidebug message, with increasing static counter
  // prints out called if called different from -1
  // useful example
  // PTools::mdb(\__LINE__)

  static Int_t call;
  if (called==-1)
    {
      call++;
      cerr << "here" << call << endl;
    }
  else cerr << "here" << called << endl;
  return;
}

////////////////////////////////////////////////////////////////////////////////

TDirectory *PTools::Mkdir(TDirectory *dirOld, 
			  const Char_t *newDirName, 
			  Int_t index, 
			  Int_t precision)    //! Makes new Dir, changes to Dir, returns pointer to new Dir
{
  // Function to create subdirectories
  // of dirOld with name newDirName
  // if index differs from -99 then index is attached to the name with the precision of precision
  // changes to the new created directory
  // returns the pointer to the new directory 
  //Thanks to J.Kempter (J.Markert)

  dirOld->cd();

  TString newName(newDirName);
  
  while (newName.Contains("/"))
  {
    TString directory = newName;
    Int_t pos = directory.First("/");
    PTools::Mkdir(gDirectory,directory.Remove(pos));
    newName = newName(pos+1,newName.Length()-pos-1).Data();
    dirOld=gDirectory;
  }
  
  static Char_t sDir[255];
  static Char_t sFmt[10];
  if (index!=-99) 
    {
      sprintf(sFmt, "%%s %%0%1ii", precision);
      sprintf(sDir, sFmt, newName.Data(), index);
    }
  else 
    {
      sprintf(sFmt, "%%s");
      sprintf(sDir, sFmt, newName.Data());
    }
  TDirectory *dirNew;
  if (!dirOld->FindKey(sDir)) 
    {
      dirNew = dirOld->mkdir(sDir);
    }
  else 
    {
      dirNew = (TDirectory *) dirOld->Get(sDir);
    }
  dirOld->cd(sDir);
  return dirNew;
}

////////////////////////////////////////////////////////////////////////////////

TDirectory *PTools::Mkdir(TDirectory *dirOld,
			  const TString newDirName, 
			  Int_t index, 
			  Int_t precision)    //! Makes new Dir, changes to Dir, returns pointer to new Dir
{
  // see:   PTools::Mkdir(TDirectory *dirOld, const Char_t *newDirName, Int_t index, Int_t precision 
  return Mkdir(dirOld, newDirName.Data(), index, precision);
}

////////////////////////////////////////////////////////////////////////////////

void PTools::getHistogramLimits(TH1* hist, 
				Int_t& nbinsx  , Axis_t& xmin  , Axis_t& xmax, 
				Int_t& nbinsy  , Axis_t& ymin  , Axis_t& ymax,
				Int_t& nbinsz  , Axis_t& zmin  , Axis_t& zmax)
{
  // returns the limits of hist

  getHistogramLimits(hist, nbinsx, xmin, xmax, nbinsy, ymin, ymax);
  if (nbinsz) nbinsz = hist->GetNbinsZ();
  if (zmin  ) zmin   = hist->GetZaxis()->GetXmin();
  if (zmax  ) zmax   = hist->GetZaxis()->GetXmax();
}

////////////////////////////////////////////////////////////////////////////////

void PTools::getHistogramLimits(TH1* hist, 
				Int_t& nbinsx  , Axis_t& xmin  , Axis_t& xmax, 
				Int_t& nbinsy  , Axis_t& ymin  , Axis_t& ymax)
{
  // returns the limits of hist

  //   if (nbinsx) nbinsx = hist->GetNbinsX();
  //   if (xmin  ) xmin   = hist->GetXaxis()->GetXmin();
  //   if (xmax  ) xmax   = hist->GetXaxis()->GetXmax();
  getHistogramLimits(hist, nbinsx, xmin, xmax);
  if (nbinsy) nbinsy = hist->GetNbinsY();
  if (ymin  ) ymin   = hist->GetYaxis()->GetXmin();
  if (ymax  ) ymax   = hist->GetYaxis()->GetXmax();

}

////////////////////////////////////////////////////////////////////////////////

void PTools::getHistogramLimits(TH1* hist, 
				Int_t& nbinsx  , Axis_t& xmin  , Axis_t& xmax)
{
  // returns the limits of hist

  if (nbinsx) nbinsx = hist->GetNbinsX();
  if (xmin  ) xmin   = hist->GetXaxis()->GetXmin();
  if (xmax  ) xmax   = hist->GetXaxis()->GetXmax();
}

////////////////////////////////////////////////////////////////////////////////

void PTools::printHistogramLimits(TH1* hist)
{
  // prints the limits of hist

  Int_t nbinsx;
  Axis_t xmin, xmax;

  getHistogramLimits(hist, nbinsx, xmin, xmax);
  printf("nbinsx: %i \nxmin: \t %f \nxmax: \t %f\n",nbinsx, xmin, xmax);
}

////////////////////////////////////////////////////////////////////////////////

void PTools::printHistogramLimits(TH2* hist)
{
  // prints the limits of hist

  Int_t nbinsx, nbinsy;
  Axis_t xmin, xmax, ymin, ymax;

  getHistogramLimits(hist, nbinsx, xmin, xmax, nbinsy, ymin, ymax);
  printf("nbinsx: %i \nxmin: \t %f \nxmax: \t %f\n",nbinsx, xmin, xmax);
  printf("nbinsy: %i \nymin: \t %f \nymax: \t %f\n",nbinsy, ymin, ymax);
}

////////////////////////////////////////////////////////////////////////////////

void PTools::printHistogramLimits(TH3* hist) 
{
  // prints the limits of hist

  Int_t nbinsx, nbinsy, nbinsz;
  Axis_t xmin, xmax, ymin, ymax, zmin, zmax;

  getHistogramLimits(hist, nbinsx, xmin, xmax, nbinsy, ymin, ymax , nbinsz, zmin, zmax);

  printf("nbinsx: %i \nxmin: \t %f \nxmax: \t %f\n",nbinsx, xmin, xmax);
  printf("nbinsy: %i \nymin: \t %f \nymax: \t %f\n",nbinsy, ymin, ymax);
  printf("nbinsz: %i \nzmin: \t %f \nzmax: \t %f\n",nbinsz, zmin, zmax);
  printHistogramLimits((TH2*) hist);
}

////////////////////////////////////////////////////////////////////////////////

Bool_t PTools::compareHistogramLimits(TH1* hist1, TH1* hist2, Bool_t exitIfNotEqual )
{
  // compares limits of hist1 with limits hist2 (nbins, min, max)
  // if exitIfNotEqual is set exit.
  
  Bool_t ok = kTRUE;

  Int_t nbinsx1, nbinsy1;
  Axis_t xmin1, xmax1, ymin1, ymax1;
  Int_t nbinsx2, nbinsy2;
  Axis_t xmin2, xmax2, ymin2, ymax2;

  getHistogramLimits(  hist1, nbinsx1, xmin1, xmax1, nbinsy1, ymin1, ymax1);
  getHistogramLimits(  hist2, nbinsx2, xmin2, xmax2, nbinsy2, ymin2, ymax2);
  
  if ( nbinsx1 != nbinsx2 ) ok = kFALSE; 
  if ( nbinsy1 != nbinsy2 ) ok = kFALSE; 
  if ( xmin1 != xmin2 ) ok = kFALSE; 
  if ( xmax1 != xmax2 ) ok = kFALSE; 
  if ( ymin1 != ymin2 ) ok = kFALSE; 
  if ( ymax1 != ymax2 ) ok = kFALSE; 

  if (!ok)
    {
      Error("checkRanges()",
	    "parameters nbinsx(%i,%i),\n nbinsy(%i,%i), \n xmin(%f,%f), \n xmax(%f,%f), \n ymin(%f,%f), \n ymax(%f,%f) of hist1:%s and hist2:%s are not identical %s",
            nbinsx1,
	    nbinsx2,
	    nbinsy1,
	    nbinsy2,
	    xmin1,
	    xmin2,
	    xmax1,
	    xmax2,
	    ymin1,
	    ymin2,
	    ymax1,
	    ymax2,
	    hist2->GetName(),
	    hist2->GetName(),
            (!exitIfNotEqual)?"!":"... exiting!"

	   );
      if (exitIfNotEqual)
	{
	  exit(EXIT_FAILURE);
	}
    }
    
  return ok;
}

////////////////////////////////////////////////////////////////////////////////

Int_t PTools::getNFilledBins(TH1 * hist, Double_t min, Double_t max)
{
  // get number of filled bins where the
  // bin contents are within the limits of min and max 
  
  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();

  if ( min==-1 && max == -1)
    {
      min = hist->GetMinimum();
      max = hist->GetMaximum();
    }
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;
  Double_t g;

  Int_t ctr=0;  

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      g = hist->GetBinContent(bin);
	      
	      if (binx==0) continue;
	      if (nbinsy !=-1) { if (biny==0) continue;}
	      if (nbinsz !=-1) { if (binz==0) continue;}
	      if (binx==nbinsx+1) continue;
	      if (nbinsy !=-1){if (biny==nbinsy+1) continue;}
	      if (nbinsz !=-1){if (binz==nbinsz+1) continue;}
	      
	      if (g >=min && g <=max) 
		{
		  if (g!=0.) ctr++;
		}

	    }
	}
    }
  return ctr;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::cleanHistogram(TH1 * hist, Double_t min, Double_t max, Int_t errorMode)
{
  // cleans histograms
  // bin contents out of the limits of min and max 
  // are set to 0 
  // errors are set to 0
  // 
  // if errorMode is not equal [default=0]
  // then instead of the binContent
  // the min-max condition is applied on the errors
  //
  // errorMode = 0: bin contents 
  // errorMode > 0: absolute Values are compared
  // errorMode < 0: relative Errors are considere
  // (for this purpose the function getErrorsOfHistogram() is used, see there for definitions of exceptions)  

  if (!hist)
    {
      ::Error("cleanHistogram"," hist is NULL pointer ... return NULL");
      return;
    }

  if (errorMode>0) errorMode = 1;
  if (errorMode<0) errorMode =-1;

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;
  Double_t g=-100.;
  bin=0;

  TH1* errHist;

  switch(errorMode)
    {
    case 0:
      errHist = NULL;
      break;
    case 1:
      errHist = (TH1*) getErrorsOfHistogram(hist,kFALSE);
      break;
    case -1:
      errHist = (TH1*) getErrorsOfHistogram(hist,kTRUE);
      break;
    default:
      errHist = NULL;
      break;
    }
  
  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      switch(errorMode)
		{
		case 0:
		  bin = hist->GetBin(binx,biny,binz);
		  g = hist->GetBinContent(bin);
		  break;
		case 1:
		  bin = errHist->GetBin(binx,biny,binz);
		  g = errHist->GetBinContent(bin);
		  break;
		case -1:
		  bin = errHist->GetBin(binx,biny,binz);
		  g = errHist->GetBinContent(bin);
		  break;
		default:
		  break;
		}

	      if(bin>=0)
		{
		  if (g < min || g > max)
		    {
		      hist->SetBinContent(bin,0);
		      hist->SetBinError(bin,0);
		    }
		}
	    }
	}
    }

  if (errorMode != 0 && errHist) 
    {
      delete errHist;
    }
}

////////////////////////////////////////////////////////////////////////////////

void PTools::prefixToName(TNamed *name, TString prefix)
{
  prefix+="_";
  prefix+= name->GetName();
  name->SetName(prefix.Data());
}

////////////////////////////////////////////////////////////////////////////////

void PTools::prefixToTitle(TNamed *name, TString prefix)
{
  prefix+=" - ";
  prefix+= name->GetTitle();
  name->SetTitle(prefix.Data());
}

////////////////////////////////////////////////////////////////////////////////

void PTools::prefixToNameTitle(TNamed *name, TString prefix)
{
  TString prefixTitle = prefix;
  prefix+="_";
  prefix+= name->GetName();
  name->SetName(prefix.Data());
  prefixTitle+=" - ";
  prefixTitle+= name->GetTitle();
  name->SetTitle(prefixTitle.Data());
  
}

////////////////////////////////////////////////////////////////////////////////

void PTools::postfixToName(TNamed *name, TString postfix)
{
  // attaches to the name of TNamed name "_postfix"
  // and      " - postfix" to its title
  
  TString postfixTitle = postfix;

  postfix=name->GetName()+TString("_")+postfix;
  name->SetName(postfix.Data());
}

////////////////////////////////////////////////////////////////////////////////

void PTools::postfixToTitle(TNamed *name, TString postfix)
{
  // attaches to TNamed name " - postfix" to its title
  
  TString postfixTitle = postfix;

  postfixTitle=name->GetTitle()+TString(" - ")+postfixTitle;
  name->SetTitle(postfixTitle.Data());
}

////////////////////////////////////////////////////////////////////////////////

void PTools::postfixToNameTitle(TNamed *name, TString postfix)
{
  // attaches to the name of TNamed name "_postfix"
  // and      " - postfix" to its title
  
  TString postfixTitle = postfix;

  postfix=name->GetName()+TString("_")+postfix;
  name->SetName(postfix.Data());

  postfixTitle=name->GetTitle()+TString(" - ")+postfixTitle;
  name->SetTitle(postfixTitle.Data());
}

// void PTools::prefixToName      (TNamed *name, Char_t  *prefix){TString aa =  prefix;  prefixToName     (name,aa);};
// void PTools::prefixToTitle     (TNamed *name, Char_t  *prefix){TString aa =  prefix;  prefixToTitle    (name,aa);};
// void PTools::prefixToNameTitle (TNamed *name, Char_t  *prefix){TString aa =  prefix;  prefixToNameTitle(name,aa);};
// void PTools::postfixToName     (TNamed *name, Char_t *postfix){TString aa = postfix; postfixToName     (name,aa);};
// void PTools::postfixToTitle    (TNamed *name, Char_t *postfix){TString aa = postfix; postfixToTitle    (name,aa);};
// void PTools::postfixToNameTitle(TNamed *name, Char_t *postfix){TString aa = postfix; postfixToNameTitle(name,aa);};

////////////////////////////////////////////////////////////////////////////////

void PTools::cutHistogram(TH2* hist, TCutG *cut, Bool_t complement)
{
  // cuts 2-dim histogram hist by using the graphical cut 
  // complement inverts the selection

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;
  Double_t gx,gy;
  
  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      gx = hist->GetXaxis()->GetBinCenter(binx);
	      gy = hist->GetYaxis()->GetBinCenter(biny);
	      
	      if ((!complement && !cut->IsInside(gx,gy)) || (complement && cut->IsInside(gx,gy)))
		{
		  hist->SetBinContent(bin,0);
		  hist->SetBinError(bin,0);
		}
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////////////

void PTools::cutHistogram(TH2* hist, TObjArray *cutarray, Bool_t complement)
{
  // cuts 2-dim histogram hist by using logical or of the graphical cuts in the cutarray
  // complement inverts the selection
  
  TIterator *iter = cutarray->MakeIterator();
  
  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;
  Double_t gx,gy;
  TCutG *cut;
  Bool_t keep;
  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      gx = hist->GetXaxis()->GetBinCenter(binx);
	      gy = hist->GetYaxis()->GetBinCenter(biny);
	      
	      cut=0;
	      iter->Reset();
	      if (!complement)
		{
		  keep=kFALSE;
		  while( (cut = (TCutG*) iter->Next()) != NULL )
		    {
		      if (cut->IsInside(gx,gy))
			{
			  keep=kTRUE;
			  break;
			}
		    }
		  if (!keep)
		    {
		      hist->SetBinContent(bin,0);
		      hist->SetBinError(bin,0);
		    }
		}
	      else
		{
		  keep=kTRUE;
		  while( (cut = (TCutG*) iter->Next()) != NULL )
		    {
		      if (cut->IsInside(gx,gy))
			{
			  keep = keep & kFALSE;
			  continue;
			}
		    }	
		  if (!keep)
		    {
		      hist->SetBinContent(bin,0);
		      hist->SetBinError(bin,0);
		    }
		}
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////////////

void  PTools::plotCounts(TH1* hist, Int_t color, TString format)
{
  // plots above each bin of hist 
  // the bin content 
  // in color
  // format can specify via e.g. %2.4f special output formats
  // but only fixed to floating precisions, so if you want to display just integers e.g. choose %4.0f

  if (hist->GetDimension()>1) 
    {    
      Error("plotCounts","this function can only be applied to 1-dimensional histograms");
      return;
    }

  Int_t first = hist->GetXaxis()->GetFirst();
  Int_t last = hist->GetXaxis()->GetLast();
  for (Int_t binx=first; binx<=last;binx++)
    {
      Double_t count = hist->GetBinContent(binx);
      Double_t xpos = hist->GetBinCenter(binx);
      Double_t ypos = (gPad->GetLogy())?log10(count):count;
      TString val="";
      if (format.IsNull())
	{
	  val+=count;
	}
      else
	{
	  val+=Form(format.Data(),count);
	}
      if (!(count>0 || count<0)) continue;

      hist->SetMaximum(hist->GetMaximum()* ((gPad->GetLogy())?5:1.05));
      
      TLatex *tex = drawLatexPadCoords(val, xpos, ypos, color, 0.025, 13, 72, 90);
      tex->SetLineWidth(2);
    }

}

////////////////////////////////////////////////////////////////////////////////

TH1F* PTools::getTH1F(TString name, TString title, 
		      Int_t xbin, Double_t xmin, Double_t xmax, 
		      TString xtitle, TString ytitle)
{
  // one command for initializing a TH1F histogramm
  // object has to be deleted by user
  TH1F *hist = new TH1F(name,title, xbin,xmin,xmax);
  setTitleArts(hist,xtitle,ytitle);
  return hist;
}
////////////////////////////////////////////////////////////////////////////////
TH2F* PTools::getTH2F(TString name, TString title, 
		      Int_t xbin, Double_t xmin, Double_t xmax, 
		      Int_t ybin, Double_t ymin, Double_t ymax, 
		      TString xtitle, TString ytitle, TString ztitle)
{
  // one command for initializing a TH2F histogramm
  // object has to be deleted by user

  TH2F *hist = new TH2F(name,title, xbin,xmin,xmax, ybin,ymin,ymax );

  if (! ztitle.IsNull()) 
    {
      hist->SetZTitle(ztitle.Data());
      ((TAxis*)hist->GetZaxis())->CenterTitle();
      ((TAxis*)hist->GetZaxis())->SetTitleFont(42);
      ((TAxis*)hist->GetZaxis())->SetTitleColor(1);
      ((TAxis*)hist->GetZaxis())->SetTitleOffset(1.2);
    }

  setTitleArts(hist,xtitle,ytitle);
  return hist;
}
////////////////////////////////////////////////////////////////////////////////
TH3F* PTools::getTH3F(TString name, TString title, 
		      Int_t xbin, Double_t xmin, Double_t xmax, 
		      Int_t ybin, Double_t ymin, Double_t ymax, 
		      Int_t zbin, Double_t zmin, Double_t zmax, 
		      TString xtitle, TString ytitle, TString ztitle)
{
  // one command for initializing a TH3F histogramm
  // object has to be deleted by user
  TH3F *hist = new TH3F(name,title, xbin,xmin,xmax, ybin,ymin,ymax, zbin,zmin,zmax );
  setTitleArts(hist,xtitle,ytitle);
  hist->SetZTitle(ztitle.Data());
  ((TAxis*)hist->GetZaxis())->CenterTitle();
  ((TAxis*)hist->GetZaxis())->SetTitleFont(42);
  ((TAxis*)hist->GetZaxis())->SetTitleColor(1);
  ((TAxis*)hist->GetZaxis())->SetTitleOffset(1.2);
  return hist;
}
////////////////////////////////////////////////////////////////////////////////

TH1D* PTools::getTH1D(TString name, TString title, 
		      Int_t xbin, Double_t xmin, Double_t xmax, 
		      TString xtitle, TString ytitle)
{
  // one command for initializing a TH1D histogramm
  // object has to be deleted by user
  TH1D *hist = new TH1D(name,title, xbin,xmin,xmax);
  setTitleArts(hist,xtitle,ytitle);
  return hist;
}
////////////////////////////////////////////////////////////////////////////////
TH2D* PTools::getTH2D(TString name, TString title, 
		      Int_t xbin, Double_t xmin, Double_t xmax, 
		      Int_t ybin, Double_t ymin, Double_t ymax, 
		      TString xtitle, TString ytitle, TString ztitle)
{
  // one command for initializing a TH2D histogramm
  // object has to be deleted by user

  TH2D *hist = new TH2D(name,title, xbin,xmin,xmax, ybin,ymin,ymax );

  if (! ztitle.IsNull()) 
    {
      hist->SetZTitle(ztitle.Data());
      ((TAxis*)hist->GetZaxis())->CenterTitle();
      ((TAxis*)hist->GetZaxis())->SetTitleFont(42);
      ((TAxis*)hist->GetZaxis())->SetTitleColor(1);
      ((TAxis*)hist->GetZaxis())->SetTitleOffset(1.2);
    }

  setTitleArts(hist,xtitle,ytitle);
  return hist;
}
////////////////////////////////////////////////////////////////////////////////
TH3D* PTools::getTH3D(TString name, TString title, 
		      Int_t xbin, Double_t xmin, Double_t xmax, 
		      Int_t ybin, Double_t ymin, Double_t ymax, 
		      Int_t zbin, Double_t zmin, Double_t zmax, 
		      TString xtitle, TString ytitle, TString ztitle)
{
  // one command for initializing a TH3D histogramm
  // object has to be deleted by user
  TH3D *hist = new TH3D(name,title, xbin,xmin,xmax, ybin,ymin,ymax, zbin,zmin,zmax );
  setTitleArts(hist,xtitle,ytitle);
  hist->SetZTitle(ztitle.Data());
  ((TAxis*)hist->GetZaxis())->CenterTitle();
  ((TAxis*)hist->GetZaxis())->SetTitleFont(42);
  ((TAxis*)hist->GetZaxis())->SetTitleColor(1);
  ((TAxis*)hist->GetZaxis())->SetTitleOffset(1.2);
  return hist;
}

////////////////////////////////////////////////////////////////////////////////
TH1* PTools::reverseXAxis(TH1* hist)
{
  // reverses the X-Axis of the histogramm
  // creates a new histogramm 

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin,bin2;

  TString name2 = hist->GetName(); name2 +="_Xreverse";

  TH1 * hist2 = (TH1*) hist->Clone(name2.Data()); 
  hist2->Reset();

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      bin2 = hist->GetBin(nbinsx+1-binx,biny,binz);
	      hist2->SetBinContent(bin2,hist->GetBinContent(bin));
	      if (hist->GetSumw2N())
		{
		  if (! hist2->GetSumw2N()) hist2->Sumw2();
		  hist2->SetBinError(bin2,hist->GetBinError(bin));
		}
	    }
	}
    }
  hist2->SetEntries(hist->GetEntries());
  return hist2;
}


////////////////////////////////////////////////////////////////////////////////
TH2* PTools::reverseYAxis(TH2* hist)
{
  // reverses the Y-Axis of the histogramm
  // creates a new histogramm 

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) {Error("reverseYAxis","dimension %i to small ... exiting!",hist->GetDimension()<2); exit(EXIT_FAILURE);} 
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin,bin2;

  TString name2 = hist->GetName(); name2 +="_Yreverse";

  TH2 * hist2 = (TH2*) hist->Clone(name2.Data()); 
  hist2->Reset();

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      bin2 = hist->GetBin(binx,nbinsy+1-biny,binz);
	      hist2->SetBinContent(bin2,hist->GetBinContent(bin));
	      if (hist->GetSumw2N())
		{
		  if (! hist2->GetSumw2N()) hist2->Sumw2();
		  hist2->SetBinError(bin2,hist->GetBinError(bin));
		}
	    }
	}
    }
  hist2->SetEntries(hist->GetEntries());
  return hist2;
}


////////////////////////////////////////////////////////////////////////////////

TH3* PTools::reverseZAxis(TH3* hist)
{
  // reverses the Z-Axis of the histogramm
  // creates a new histogramm 

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) {Error("reverseZAxis","dimension %i to small ... exiting!",hist->GetDimension()<2); exit(EXIT_FAILURE);} 
  if (hist->GetDimension()<3) {Error("reverseZAxis","dimension %i to small ... exiting!",hist->GetDimension()<2); exit(EXIT_FAILURE);} 

  Int_t binx,biny,binz,bin,bin2;

  TString name2 = hist->GetName(); name2 +="_Zreverse";

  TH3 * hist2 = (TH3*) hist->Clone(name2.Data()); 
  hist2->Reset();

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++) 
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      bin2 = hist->GetBin(binx,biny,nbinsz+1-binz);
	      hist2->SetBinContent(bin2,hist->GetBinContent(bin));
	      if (hist->GetSumw2N())
		{
		  if (! hist2->GetSumw2N()) hist2->Sumw2();
		  hist2->SetBinError(bin2,hist->GetBinError(bin));
		}
	    }
	}
    }
  hist2->SetEntries(hist->GetEntries());
  return hist2;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::plotPolarAxis(Double_t min, Double_t max, Int_t divisions, Float_t direction)
{
  // plots polar axis
  // starting from min
  // ending at max
  // pointing in direction (angle [rad])
  // with n divisions 

  direction*=(acos(-1.)/180.0);
  Float_t radius = 0.58;

  TGaxis *axis1 = new TGaxis(0,0, radius*cos(direction) ,radius*sin(direction) ,min,max,divisions,"+");
  axis1->SetLabelColor(1);
  axis1->Draw();
  //   TGaxis *axis2 = new TGaxis(0.002,0, (radius+0.002)*cos(direction) , (radius+0.002)*sin(direction) ,min,max,divisions,"+");
  //   axis2->Draw();
}

////////////////////////////////////////////////////////////////////////////////

void PTools::plotPolarAxis(TH2* hist, Int_t divisions, Float_t direction)
{
  // plots polar axis using the limits of hist
  // see plotPolarAxis
  if (!hist) 
    {
      Error("plotPolarAxis","hist has a NULL pointer ... nothing done");
      return;
    }
  plotPolarAxis(hist->GetYaxis()->GetXmin(),hist->GetYaxis()->GetXmax(),divisions,direction);
  
}
////////////////////////////////////////////////////////////////////////////////

void PTools::plotPolarGrid(Int_t division)
{
  // plots in polar coordiates 
  // a circular grid
  // with the hades naming of phi angles
  // 

  if (division == 0) 
    {
      Error("plotPolarGrid","division of %i not realistic ... nothing done",division);
      return;
    }
  
  Float_t radius = 0.58;
  TEllipse *kDeg; 
  TEllipse *kDeg2; 
  {
    for (Int_t iter = 1; iter <= division; iter++) 
      {
	kDeg = new TEllipse(0,0,(radius/division)*iter);
	kDeg->SetLineWidth(2);
	kDeg->SetLineColor(10);
	kDeg->Draw();
	kDeg2 = new TEllipse(0,0,(radius/division)*iter);
	kDeg2->SetLineWidth(1);
	kDeg2->Draw();
      }
  }
}


////////////////////////////////////////////////////////////////////////////////

void PTools::plotPolarSectorGridHADES()
{
  // plots in polar coordiates 
  // a line axis grid
  // with the hades naming of phi angles

  TLine *lDeg[6];
  TLine *lDeg2[6];
  TString label[6]={"180#circ","120#circ","60#circ","0#circ","300#circ","240#circ"};
  TLatex * tex[6];
  
  for (Int_t iter = 0; iter < 6; iter++)
    {
      Float_t radius = 0.65;
      lDeg[iter] = new TLine(0,0,radius*cos(iter*60*acos(-1.)/180),radius*sin(iter*60*acos(-1.)/180));
      //lDeg[iter]->SetLineStyle(2);
      lDeg[iter]->SetLineWidth(2);
      lDeg[iter]->SetLineColor(10);
      lDeg[iter]->Draw(); 
      lDeg2[iter] = new TLine(0,0,radius*cos(iter*60*acos(-1.)/180),radius*sin(iter*60*acos(-1.)/180));
      //      lDeg2[iter]->SetLineStyle(2);
      lDeg2[iter]->SetLineWidth(1);
      lDeg2[iter]->SetLineColor(1);
      lDeg2[iter]->Draw(); 
      
      radius+=0.07;
      tex[iter]  = new TLatex(radius*cos(iter*60*acos(-1.)/180),radius*sin(iter*60*acos(-1.)/180),label[iter].Data());
      tex[iter]->Draw();
    }
}


/////////////////////////////////////////////////////////////////////////////////

void PTools::saveToPdf(TCanvas* canvas,TString path,TString filename,Bool_t separate, Int_t selection)
{
  // saves canvas to pdf
  // to path
  // using filename 
  // if separate is used 
  // each sub pad is printed to its own file
  // (requieres external function ps2pdf)
  // Thanks to Jochen Markert, GSI
  // selection selects one specific subpad number if selection > 0
  // produced now output in compatiblity mode 1.4 (Acrobat 5 and higher)
  saveToPdf((TPad*) canvas,path,filename,separate, selection);
}

/////////////////////////////////////////////////////////////////////////////////

void PTools::saveToPdf(TPad* pad,TString path,TString filename,Bool_t separate, Int_t selection)
{
  // saves pad to pdf
  // to path
  // using filename 
  // if separate is used 
  // each sub pad is printed to its own file
  // (requieres external function ps2pdf)
  // Thanks to Jochen Markert, GSI
  // selection selects one specific subpad number if selection > 0
  // produced now output in compatiblity mode 1.4 (Acrobat 5 and higher)

  if (filename.IsNull()) filename=pad->GetName();

  TString File    =filename;
  TString Path    =path;
  if (!Path.EndsWith("/") && !Path.IsNull()) 
    {
      Path = Path + "/";
    }
  TString command ="";
  TString totalin ="";
  TString totalout="";
  TString converter="ps2pdf -dCompatibilityLevel=1.4 -dEmbedAllFonts=true";

  TString check = TString("which ")+ converter + TString(" > /dev/null");
  
  if (gSystem->Exec(check.Data()))
  {
    Error("saveToPdf","external program \"%s\" not available ... nothing is done",converter.Data());
    return;
  }

  File.ReplaceAll(".ps","");
  if(!separate)
    {
      totalin = Path + File + ".ps";
      totalout= Path + File + ".pdf";
      pad->SaveAs(totalin.Data());
      command= converter + " " + totalin + " " + totalout;
      gSystem->Exec(command.Data());
      command= "rm " + totalin;
      gSystem->Exec(command.Data());
    }else{
      TString subpad="";
      Int_t npads= pad->GetListOfPrimitives()->GetSize();
      for(Int_t i=1;i<=npads;i++)
        {
	  if (selection > 0 && i!=selection) 
	    {
	      continue;
	    }
	  subpad="";
	  subpad+=i;
	  totalin = Path + File + "_" + subpad + ".ps";
	  totalout= Path + File + "_" + subpad + ".pdf";
	  pad->cd(i);
	  TPad *clone = (TPad*) gPad->Clone();
	  Bool_t batch = gROOT->IsBatch();
	  gROOT->SetBatch();
	  TCanvas *dummy = new TCanvas("dummy","dummy",1000,800);
	  dummy->cd();
	  clone->Draw();
	  clone->SetPad(0,0,1,1);
	  dummy->SaveAs(totalin.Data());
	  command= converter + " " + totalin + " " + totalout;
	  gSystem->Exec(command.Data());
	  command= "rm " + totalin;
	  gSystem->Exec(command.Data());
	  dummy->Close();
	  gROOT->SetBatch(batch);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////

void PTools::saveToPng(TCanvas* canvas,TString path,TString filename,Bool_t separate, Int_t selection)
{
  // saves canvas to png
  // to path
  // using filename 
  // if separate is used 
  // each sub pad is printed to its own file
  // (requieres external function convert)
  // Thanks to Jochen Markert, GSI
  // selection selects one specific subpad number if selection > 0

  saveToPng((TPad*)canvas,path, filename, separate, selection);
}

/////////////////////////////////////////////////////////////////////////////////

void PTools::saveToPng(TPad* pad,TString path,TString filename,Bool_t separate, Int_t selection)
{
  // saves pad to png
  // to path
  // using filename 
  // if separate is used 
  // each sub pad is printed to its own file
  // (requieres external function convert)
  // Thanks to Jochen Markert, GSI
  // selection selects one specific subpad number if selection > 0

  if (filename.IsNull()) {filename=pad->GetName();}

  TString File    =filename;
  TString Path    =path;
  if (!Path.EndsWith("/") && !Path.IsNull()) 
    {
      Path = Path + "/";
    }
  TString command ="";
  TString totalin ="";
  TString totalout="";
  TString converter="convert";

  TString check = TString("which ")+ converter  + TString(" > /dev/null");
  
  if (gSystem->Exec(check.Data()))
  {
    Error("saveToPng","external program \"%s\" not available ... nothing is done",converter.Data());
    return;
  }

  File.ReplaceAll(".png","");
  if(!separate)
    {
      totalin = Path + File + ".ps";
      totalout= Path + File + ".png";
      pad->SaveAs(totalin.Data());
      command= converter + " " + totalin + " " + totalout;
      gSystem->Exec(command.Data());
      command= converter + " -rotate 90 " + totalout + " " + totalout;
      gSystem->Exec(command.Data());
      command= "rm " + totalin;
      gSystem->Exec(command.Data());
    }else{
      TString subpad="";
      Int_t npads= pad->GetListOfPrimitives()->GetSize();
      for(Int_t i=1;i<=npads;i++)
        {
	  if (selection > 0 && i!=selection) 
	    {
	      continue;
	    }
	  subpad="";
	  subpad+=i;
	  totalin = Path + File + "_" + subpad + ".ps";
	  totalout= Path + File + "_" + subpad + ".png";
	  pad->cd(i);
	  TPad *clone = (TPad*) gPad->Clone();
	  Bool_t batch = gROOT->IsBatch();
	  gROOT->SetBatch();
	  TCanvas *dummy = new TCanvas("dummy","dummy",1000,800);
	  dummy->cd();
	  clone->Draw();
	  clone->SetPad(0,0,1,1);
	  dummy->SaveAs(totalin.Data());
	  command= converter + " " + totalin + " " + totalout;
	  gSystem->Exec(command.Data());
	  command= converter + " -rotate 90 " + totalout + " " + totalout;
	  gSystem->Exec(command.Data());
	  command= "rm " + totalin;
	  gSystem->Exec(command.Data());
	  dummy->Close();
	  gROOT->SetBatch(batch);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////

void PTools::saveGifToPdf(TCanvas* canvas,TString path,TString filename,Bool_t separate, Int_t selection)
{
  // saves canvas to gif
  // to path
  // using filename 
  // if separate is used 
  // each sub pad is printed to its own file
  // (requieres external function convert)
  // Thanks to Jochen Markert, GSI
  // selection selects one specific subpad number if selection > 0

  saveGifToPdf((TPad*)canvas , path, filename, separate, selection);
}

/////////////////////////////////////////////////////////////////////////////////

void PTools::saveGifToPdf(TPad* pad,TString path,TString filename,Bool_t separate, Int_t selection)
{
  // saves pad to gif
  // to path
  // using filename 
  // if separate is used 
  // each sub pad is printed to its own file
  // (requieres external function convert)
  // Thanks to Jochen Markert, GSI
  // selection selects one specific subpad number if selection > 0

  if (filename.IsNull()) filename=pad->GetName();

  TString File    =filename;
  TString Path    =path;
  if (!Path.EndsWith("/") && !Path.IsNull()) 
    {
      Path = Path + "/";
    }
  TString command ="";
  TString totalin ="";
  TString totalout="";
  TString converter="convert";
  TString check = TString("which ")+ converter  + TString(" > /dev/null");
  
  if (gSystem->Exec(check.Data()))
  {
    Error("saveGifToPdf","external program \"%s\" not available ... nothing is done",converter.Data());
    return;
  }

  File.ReplaceAll(".pdf","");
  if(!separate)
    {
      totalin = Path + File + ".gif";
      totalout= Path + File + ".pdf";
      pad->SaveAs(totalin.Data());
      command= converter + " " + totalin + " " + totalout;
      gSystem->Exec(command.Data());
      command= "rm " + totalin;
      gSystem->Exec(command.Data());
    }
  else
    {
      TString subpad="";
      Int_t npads= pad->GetListOfPrimitives()->GetSize();
      for(Int_t i=1;i<=npads;i++)
        {
	  if (selection > 0 && i!=selection) 
	    {
	      continue;
	    }
	  subpad="";
	  subpad+=i;
	  totalin = Path + File + "_" + subpad + ".ps";
	  totalout= Path + File + "_" + subpad + ".pdf";
	  pad->cd(i);
	  TPad *clone = (TPad*) gPad->Clone();
	  Bool_t batch = gROOT->IsBatch();
	  gROOT->SetBatch();
	  TCanvas *dummy = new TCanvas("dummy","dummy",1000,800);
	  dummy->cd();
	  clone->Draw();
	  clone->SetPad(0,0,1,1);
	  dummy->SaveAs(totalin.Data());
	  command= converter + " " + totalin + " " + totalout;
	  gSystem->Exec(command.Data());
	  command= "rm " + totalin;
	  gSystem->Exec(command.Data());
	  dummy->Close();
	  gROOT->SetBatch(batch);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////

Int_t PTools::getColorJM(Int_t i)
{
  // Thanks to Jochen Markert, GSI
  Int_t colors[]   ={2,4,6,8,38,46,14,1,30,43};
  i%=((int)(sizeof(colors)/sizeof(Int_t)));
  return colors[i];
}

/////////////////////////////////////////////////////////////////////////////////

Int_t PTools::getMarkerJM(Int_t i)
{
  // Thanks to Jochen Markert, GSI
  Int_t style[]   ={20,21,22,23,24,25,26,27,28,29};
  i%=((int)(sizeof(style)/sizeof(Int_t)));
  return style[i];
}

/////////////////////////////////////////////////////////////////////////////////

void PTools::setGraph(TGraph* graph,Int_t mycolorindex,Int_t markerstyle,Int_t markercolor,Float_t markersize,Int_t linecolor)
{
  // Thanks to Jochen Markert, GSI
  if(mycolorindex<0)
    {
      graph->SetMarkerColor(markercolor);
      graph->SetMarkerSize(markersize);
      graph->SetMarkerStyle(markerstyle);
      graph->SetLineColor(linecolor);
    }
  else
    {
      if     (markercolor>=0)graph->SetMarkerColor(PTools::getColorJM(mycolorindex));
      else if(markercolor<0) graph->SetMarkerColor(PTools::getColorJM(-markercolor));
      graph->SetMarkerSize(markersize);
      if     (markerstyle>=0)graph->SetMarkerStyle(PTools::getMarkerJM(mycolorindex));
      else if(markerstyle<0) graph->SetMarkerStyle(PTools::getMarkerJM(-markerstyle));
      graph->SetLineColor(linecolor);
    }
}

/////////////////////////////////////////////////////////////////////////////////

void PTools::setHist(TH1* hist,Int_t mycolorindex,Int_t markerstyle,Int_t markercolor,Float_t markersize,Int_t linecolor)
{
  // Thanks to Jochen Markert, GSI
  if(mycolorindex==-99)
    {
      hist->SetMarkerColor(markercolor);
      hist->SetMarkerSize(markersize);
      hist->SetMarkerStyle(markerstyle);
      hist->SetLineColor(linecolor);
    }
  else
    {
      if     (markercolor>=0)hist->SetMarkerColor(PTools::getColorJM(mycolorindex));
      else if(markercolor<0) hist->SetMarkerColor(PTools::getMarkerJM(-markercolor));
      hist->SetMarkerSize(markersize);
      if     (markerstyle>=0)hist->SetMarkerStyle(PTools::getMarkerJM(mycolorindex));
      else if(markerstyle<0) hist->SetMarkerStyle(PTools::getMarkerJM(-markerstyle));
      hist->SetLineColor(linecolor);
    }
}

/////////////////////////////////////////////////////////////////////////////////

TLegend* PTools::plotLegend(TString pos,TString Title,
			    Float_t scaleX,Float_t scaleY,
			    Float_t offsetX,Float_t offsetY,
			    TString Comment,
			    Int_t commencolor)
{
  // Thanks to Jochen Markert, GSI
    Float_t left  =gPad->GetLeftMargin()*1.15;
    Float_t right =1-gPad->GetRightMargin();
    Float_t top   =1-gPad->GetTopMargin();
    Float_t bottom=gPad->GetBottomMargin()*1.15;
    Float_t mid   =gPad->GetLeftMargin() + (1-(gPad->GetRightMargin()+gPad->GetLeftMargin()))/2;
    Float_t width =(right-left)/2;
    Float_t heith =(top-bottom)/2;
    TLegend* legend = NULL;
    TLine* dummy=new TLine();
    dummy->SetLineColor(10);

    if(pos.CompareTo("left_top")==0)    legend=new TLegend(left+offsetX,top+offsetY-(scaleY*heith),left+offsetX+(scaleX*width),top+offsetY,Title.Data());                           // left top
    if(pos.CompareTo("mid_top")==0)     legend=new TLegend(mid+offsetX-((scaleX*width)/2),top+offsetY-(scaleY*heith),mid+offsetX+((scaleX*width)/2),top+offsetY,Title.Data());      // mid up
    if(pos.CompareTo("right_top")==0)   legend=new TLegend(right+offsetX-(scaleX*width),top+offsetY-(scaleY*heith),right+offsetX,top+offsetY,Title.Data());                         // right top
    if(pos.CompareTo("left_bottom")==0) legend=new TLegend(left+offsetX,bottom+offsetY,left+offsetX+(scaleX*width),bottom+offsetY+(scaleY*heith),Title.Data());                     // left bottom
    if(pos.CompareTo("mid_bottom")==0)  legend=new TLegend(mid+offsetX-((scaleX*width)/2),bottom+offsetY,mid+offsetX+((scaleX*width)/2),bottom+offsetY+(scaleY*heith),Title.Data());// mid down
    if(pos.CompareTo("right_bottom")==0)legend=new TLegend(right+offsetX-(scaleX*width),bottom+offsetY,right+offsetX,bottom+offsetY+(scaleY*heith),Title.Data());                   // right bottom

    if (!legend) return NULL;
    legend->SetFillStyle(0);
    legend->SetBorderSize(0);
    legend->SetMargin(0.15);

    if(Comment.CompareTo("")!=0)
    {   // comment below header
        TLegendEntry* entry=legend->AddEntry(dummy,Comment.Data());
        entry->SetTextColor(1);
        entry->SetTextFont(62);
        entry->SetOption("l");
    }
    return legend;
}

/////////////////////////////////////////////////////////////////////////////////

TLegendEntry* PTools::setLegendEntry(TLegend* legend,TObject* object,TString label,Int_t col,TString opt)
{
    // add entry for object with label and color and option to legend
    //
  // Thanks to Jochen Markert, GSI
    TLegendEntry* entry=legend->AddEntry((TObject*)object,label.Data());
    entry->SetTextColor(col);
    entry->SetOption(opt.Data());
    return entry;
}

/////////////////////////////////////////////////////////////////////////////////

TLatex* PTools::plotTopLegend(const Char_t* label,Float_t x,Float_t y,Float_t size,Int_t color)
{
  // Thanks to Jochen Markert, GSI
    // coordinates in NDC!
    if(x<0||y<0)
    {   // defaults
        x=gPad->GetLeftMargin()*1.15;
        y=(1-gPad->GetTopMargin())*1.02;
    }
    TLatex* text=new TLatex(x,y,label);
    text->SetTextSize(size);
    text->SetNDC();
    text->SetTextColor(color);
    text->Draw();
    return text;
}

/////////////////////////////////////////////////////////////////////////////////

Double_t PTools::getMeanErrorRangeUser(TH1* hist, Axis_t minX, Axis_t maxX, TString pos)
{
  // calculates the error of arithmetic mean of a TH1 
  // in a range between minX and maxX
  // position: determines the value of the bin taken
  //           "center" : bin center value [default]
  //           "up"     : bin upper edge
  //           "low"    : bin lower edge value

  if (!hist) {::Error("getMeanErrorRangeUser","no valid histogramm NULL pointer"); exit(EXIT_FAILURE);}
  return getMeanErrorRange(hist, hist->FindBin(minX), hist->FindBin(maxX),pos);
}

/////////////////////////////////////////////////////////////////////////////////

Double_t PTools::getMeanErrorRange(TH1* hist, Int_t minX, Int_t maxX, TString pos)
{
  // calculates the error of arithmetic mean of a TH1 
  // in a range between bin minX and bin maxX
  // position: determines the value of the bin taken
  //           "center" : bin center value [default]
  //           "up"     : bin upper edge
  //           "low"    : bin lower edge value

  pos.ToLower();
  Int_t p=-10;
  
  if (pos.CompareTo("center")==0) p=0;
  if (pos.CompareTo("up")==0) p=1;
  if (pos.CompareTo("low")==0) p=-1;
  
  if (p==-10) { Error("getMeanErrorRange","wrong position parameter %s",pos.Data());exit(EXIT_FAILURE);}

  if (!hist) {::Error("getMeanErrorRange","no valid histogramm NULL pointer"); exit(EXIT_FAILURE);}

  Int_t nbinsx = hist->GetNbinsX();
  
  if (hist->GetDimension()>2) {::Error("getMeanErrorRange","cannot be used for TH2 and TH3"); exit(EXIT_FAILURE);} 
  
  Int_t binx;
  
  Double_t bin,binval=0,binError;
  Double_t sumw2=0;
  Double_t sum=0;
  Double_t sumv=0;
   
  for (binx=0;binx<=nbinsx+1;binx++)
    {
      if (minX>binx) continue;
      if (maxX<binx) continue;
      switch (p)
	{
	case 0:
	  binval=hist->GetXaxis()->GetBinCenter(binx);
	  break;
	case -1:
	  binval=hist->GetXaxis()->GetBinLowEdge(binx);
	  break;
	case 1:
	  binval=hist->GetXaxis()->GetBinUpEdge(binx);
	  break;
	default:
	  binval=0;
	  break;
	}
      bin = hist->GetBinContent(binx);
      sum+=bin;
      sumv+=(bin*binval);
    }

  for (binx=0;binx<=nbinsx+1;binx++)
    {
      if (minX>binx) continue;
      if (maxX<binx) continue;
      switch (p)
	{
	case 0:
	  binval=hist->GetXaxis()->GetBinCenter(binx);
	  break;
	case -1:
	  binval=hist->GetXaxis()->GetBinLowEdge(binx);
	  break;
	case 1:
	  binval=hist->GetXaxis()->GetBinUpEdge(binx);
	  break;
	default:
	  binval=0;
	  break;
	}
      
      bin = hist->GetBinContent(binx);
      binError = hist->GetBinError(binx);
      sumw2+=( ((binval*sum)-sumv)*binError*((binval*sum)-sumv)*binError);
    }
  
  // cout << sumv << " " << (sumv/sum) << endl;
  
  sumw2=(((sum*sum*sum*sum)>0) || ((sum*sum*sum*sum)<0))?sumw2/(sum*sum*sum*sum):-1;
  return (sumw2<0)?-1: ::std::sqrt(sumw2);
}

////////////////////////////////////////////////////////////////////////////////

Double_t PTools::getMeanRangeUser(TH1* hist, Axis_t minX, Axis_t maxX, TString pos)
{
  // calculates the arithmetic mean of a TH1 
  // in a range between minX and maxX
  // position: determines the value of the bin taken
  //           "center" : bin center value [default]
  //           "up"     : bin upper edge
  //           "low"    : bin lower edge value

  if (!hist) {::Error("getMeanErrorRangeUser","no valid histogramm NULL pointer"); exit(EXIT_FAILURE);}
  return getMeanErrorRange(hist, hist->FindBin(minX), hist->FindBin(maxX),pos);
}

/////////////////////////////////////////////////////////////////////////////////

Double_t PTools::getMeanRange(TH1* hist, Int_t minX, Int_t maxX, TString pos)
{
  // calculates the arithmetic mean of a TH1 
  // in a range between bin minX and bin maxX
  // position: determines the value of the bin taken
  //           "center" : bin center value [default]
  //           "up"     : bin upper edge
  //           "low"    : bin lower edge value

  pos.ToLower();
  Int_t p=-10;
  
  if (pos.CompareTo("center")==0) p=0;
  if (pos.CompareTo("up")==0) p=1;
  if (pos.CompareTo("low")==0) p=-1;
  
  if (p==-10) { Error("getMeanErrorRange","wrong position parameter %s",pos.Data());exit(EXIT_FAILURE);}

  if (!hist) {::Error("getMeanErrorRange","no valid histogramm NULL pointer"); exit(EXIT_FAILURE);}

  Int_t nbinsx = hist->GetNbinsX();
  
  if (hist->GetDimension()>2) {::Error("getMeanErrorRange","cannot be used for TH2 and TH3"); exit(EXIT_FAILURE);} 
  
  Int_t binx;
  
  Double_t bin,binval;
  Double_t sum=0;
  Double_t sumv=0;
   
  for (binx=0;binx<=nbinsx+1;binx++)
    {
      if (minX>binx) continue;
      if (maxX<binx) continue;
      switch (p)
	{
	case 0:
	  binval=hist->GetXaxis()->GetBinCenter(binx);
	  break;
	case -1:
	  binval=hist->GetXaxis()->GetBinLowEdge(binx);
	  break;
	case 1:
	  binval=hist->GetXaxis()->GetBinUpEdge(binx);
	  break;
	default:
	  binval=0;
	  break;
	}

      bin = hist->GetBinContent(binx);
      sum+=bin;
      sumv+=(bin*binval);
    }

  return (sum>0 || sum <0)?sumv/sum:-1;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::sqrt(TH1* hist)
{
  // calculates the sqrt of each histogram bin

  if (! hist->GetSumw2N()) hist->Sumw2();

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<3) nbinsz = -1;
  if (hist->GetDimension()<2) nbinsy = -1;

  Int_t binx,biny,binz,bin;
  
  for (binz=0;binz<=nbinsz+1;binz++)
    {
      if (nbinsz!=-1)
	{
	  if (binz == 0       ) continue;
	  if (binz == nbinsz+1) continue;
	}
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  if (nbinsy!=-1)
	    {
	      if (biny == 0       ) continue;
	      if (biny == nbinsy+1) continue;
	    }
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      if (nbinsx!=-1)
		{
		  if (binx == 0       ) continue;
		  if (binx == nbinsx+1) continue;
		}
	      bin = hist->GetBin(binx,biny,binz);

	      if (hist->GetBinContent(bin) < 0)
		{
		  Error("sqrt()","histogram %s contains negative values, cannot perform sqrt() operation ... exiting",
			hist->GetName());
		  exit(EXIT_FAILURE);
		}
	    }
	}
    }

  Stat_t val=0;
  Stat_t err=0;
  for (binz=0;binz<=nbinsz+1;binz++)
    {
      if (nbinsz!=-1)
	{
	  if (binz == 0       ) continue;
	  if (binz == nbinsz+1) continue;
	}
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  if (nbinsy!=-1)
	    {
	      if (biny == 0       ) continue;
	      if (biny == nbinsy+1) continue;
	    }
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      if (nbinsx!=-1)
		{
		  if (binx == 0       ) continue;
		  if (binx == nbinsx+1) continue;
		}
	      bin = hist->GetBin(binx,biny,binz);

	      val = hist->GetBinContent(bin);
	      err = hist->GetBinError(bin);
	      
	      hist->SetBinContent(bin, ::std::sqrt(val));

	      if (val==0.0 ||  ::std::sqrt(val) == 0.0)
		{
		  hist->SetBinError(bin,0);
		}
	      else
		{
		    hist->SetBinError(bin, ( err*0.5 * 1./ ::std::sqrt(val)));
		}
	    }
	}
    }

}

////////////////////////////////////////////////////////////////////////////////

TNamed* PTools::Clone(TNamed *name, TString newname, TString title)
{
  // Clones named object name and sets the name no newname and its title to title 
  // if newname and or title are empty
  // title and name are extended by "_clone"
  
  if (newname.IsNull())
    {
      newname=name->GetName();
      newname+="_clone";
    }
  if (title.IsNull())
    {
      title=name->GetTitle();
      title+="_clone";
    }

  TNamed *named = (TNamed*) name->Clone(newname.Data());
  named->SetTitle(title.Data());

  return named;
}

//memomory leak?
////////////////////////////////////////////////////////////////////////////////

TH1* PTools::copyToTHxD(TH1 * hist)
{
  // copies any THx histogramm to THxD histograms
  // you have to cast to the corresponding type
  // attaches to name "_THxD"

  if (! hist)
    {
      ::Error("copyToTHxD","histogram has NULL pointer ... exiting");
      exit(EXIT_FAILURE);
      return NULL;
    }
  
  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();

  TH1* histD = NULL;

  switch(hist->GetDimension())
    {
    case 1:
      {
	Int_t s = 0;
	s = 
	  ((int)(hist->GetXaxis()->GetXbins()->fN > 0)) * 1; 
	TString name = hist->GetName()+TString("_TH1D");

	switch(s)
	  {
	  case 0:
	    histD = new TH1D(name.Data(),hist->GetTitle(),
			     nbinsx,
			     hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax());
	    break;
	  case 1:
	    histD = new TH1D(name.Data(),hist->GetTitle(),
			     nbinsx,
			     hist->GetXaxis()->GetXbins()->fArray);
	    break;
	  default:
	    
	    break;
	  }
	break;
      }
    case 2:
      {
	Int_t s = 0;
	s = ((int)(hist->GetXaxis()->GetXbins()->fN > 0)) * 1 + ((int)( hist->GetYaxis()->GetXbins()->fN > 0)) * 10;
	TString name = hist->GetName()+TString("_TH2D");
	
	switch(s)
	  {
	  case 0:
	    histD = new TH2D(name.Data(),hist->GetTitle(),
			     nbinsx,
			     hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax(),
			     nbinsy,
			     hist->GetYaxis()->GetXmin(),hist->GetYaxis()->GetXmax());
	    break;
	  case 1:
	    histD = new TH2D(name.Data(),hist->GetTitle(),
			     nbinsx,
			     hist->GetXaxis()->GetXbins()->fArray,
			     nbinsy,
			     hist->GetYaxis()->GetXmin(),hist->GetYaxis()->GetXmax());
	    
	    break;
	  case 10:
	    histD = new TH2D(name.Data(),hist->GetTitle(),
			     nbinsx,
			     hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax(),
			     nbinsy,
			     hist->GetYaxis()->GetXbins()->fArray);
	    break;
	  default:
	    
	    break;
	  }
	break;
      }
    case 3:
      {
	TString name = hist->GetName()+TString("_TH3D");

	histD = new TH3D(name.Data(),hist->GetTitle(),
			 nbinsx,
			 hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax(),
			 nbinsy,
			 hist->GetYaxis()->GetXmin(),hist->GetYaxis()->GetXmax(),
			 nbinsz,
			 hist->GetZaxis()->GetXmin(),hist->GetZaxis()->GetXmax());

	importAxisAttributes(hist->GetXaxis(), histD->GetXaxis(), kTRUE);
	importAxisAttributes(hist->GetYaxis(), histD->GetYaxis(), kTRUE);
	importAxisAttributes(hist->GetZaxis(), histD->GetZaxis(), kTRUE);
	break;
      }
    }
  
  importAxisAttributes(hist->GetXaxis(), histD->GetXaxis(), kTRUE);
  importAxisAttributes(hist->GetYaxis(), histD->GetYaxis(), kTRUE);
  importAxisAttributes(hist->GetZaxis(), histD->GetZaxis(), kTRUE);

  histD->SetEntries(hist->GetEntries());
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;
  
  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      histD->SetBinContent(bin, hist->GetBinContent(bin));
	      if (hist->GetSumw2N())
		{
		  histD->SetBinError(bin,hist->GetBinError(bin));
		}
	    }
	}
    }
  return histD;
}

////////////////////////////////////////////////////////////////////////////////

TH1D PTools::copyToTH1D(TH1 * hist)
{
  // copy any TH1 histogramm to TH1D histogramm
  
  TH1D* tmp = (TH1D*) copyToTHxD(hist);
  TH1D h(*tmp);
  tmp->Delete();
  return TH1D(h);
}

////////////////////////////////////////////////////////////////////////////////

TH2D PTools::copyToTH2D(TH2 * hist)
{
  // copy any TH2 histogramm to TH2D histogramm
  TH2D* tmp = (TH2D*) copyToTHxD(hist);
  TH2D h(*tmp);
  tmp->Delete();
  return TH2D(h);
}

////////////////////////////////////////////////////////////////////////////////

TH3D PTools::copyToTH3D(TH3 * hist)
{
  // copy any TH3 histogramm to TH3D histogramm
  TH3D* tmp = (TH3D*) copyToTHxD(hist);
  TH3D h(*tmp);
  tmp->Delete();
  return TH3D(h);
}

////////////////////////////////////////////////////////////////////////////////

TH1D* PTools::createTH1D(TH1 * hist)
{
  // copy any TH1 histogramm to TH1D histogramm
  // returned histogram has to be deleted by user

  return (hist) ? (TH1D*) copyToTHxD(hist) :  NULL;
}

////////////////////////////////////////////////////////////////////////////////

TH2D* PTools::createTH2D(TH2 * hist)
{
  // copy any TH2 histogramm to TH2D histogramm
  // returned histogram has to be deleted by user
  
  return (hist) ? (TH2D*) copyToTHxD(hist) :  NULL;
}

////////////////////////////////////////////////////////////////////////////////

TH3D* PTools::createTH3D(TH3 * hist)
{
  // copy any TH3 histogramm to TH3D histogramm
  // returned histogram has to be deleted by user

  return (hist) ? (TH3D*) copyToTHxD(hist) :  NULL;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::importAxisAttributes(TAxis *in, TAxis *out, Bool_t includeLabels)
{
// Copy TAxis attributes 

  if (!in || !out)
    {
      Warning("importAxisAttributes","in or output axis is NULL ... nothing done");
    }

   out->SetAxisColor  (in->GetAxisColor());
   out->SetTitleColor (in->GetTitleColor());
   out->SetTitleFont  (in->GetTitleFont());
   out->SetLabelColor (in->GetLabelColor());
   out->SetLabelFont  (in->GetLabelFont());
   out->SetLabelSize  (in->GetLabelSize());
   out->SetLabelOffset(in->GetLabelOffset());
   out->SetTickLength (in->GetTickLength());
   out->SetTitle      (in->GetTitle());
   out->SetTitleOffset(in->GetTitleOffset());
   out->SetTitleSize  (in->GetTitleSize());
   out->SetBit(TAxis::kCenterTitle, in->TestBit(TAxis::kCenterTitle));
   out->SetBit(TAxis::kRotateTitle, in->TestBit(TAxis::kRotateTitle));
   out->SetBit(TAxis::kNoExponent,   in->TestBit(TAxis::kNoExponent));
   out->SetBit(TAxis::kTickPlus,     in->TestBit(TAxis::kTickPlus));
   out->SetBit(TAxis::kTickMinus,    in->TestBit(TAxis::kTickMinus));
   out->SetTimeFormat(in->GetTimeFormat());
   if (includeLabels) 
     {
       importAxisLabels(in,out);
     }
}

////////////////////////////////////////////////////////////////////////////////

void PTools::importAxisLabels(TAxis *in, TAxis *out)
{
  // copies bin labels from one axis to the other
  // if bin numbers differ overlap is taken 

  THashList* labels = in->GetLabels();

  if(labels !=NULL)
    {
      if(in->GetNbins() != out->GetNbins())
	{
	  ::Warning("ImportAxisAttributes","axises do not have same binning copying labels for overlap");
	}
      TIter next(labels);
      TObjString *obj;
      while((obj=(TObjString*) next()))
	{
	  out->SetBinLabel((Int_t)obj->GetUniqueID(),obj->GetName());
	}
    }
}

////////////////////////////////////////////////////////////////////////////////

Int_t  PTools::checkHistogramCompatibility(TH1* h1, TH1 *h2)
{
  // checks whether h1 and h1 
  //
  //   * have the same type       +(0000001)
  //   * have the same dimension  +(0000100)
  //   * have the same binning    +(0010000)
  //   * have the same limits     +(1000000)
  //
  // returns the sum of conditions not matched
  // i.e. not same type && not same binning =>  1000001;
  // returns 0, if all conditions match
  // returns -1, if h1 or h2 is a NULL pointer 

  // check for not NULL pointer
  if (!h1 || !h2)
    {
      Error("checkHistogrammCompatibility","can't compare, h1 (%p) or h2 (%p) is null pointer, returning -1",h1, h2);
      return -1;
    }

  Int_t val = 0;

  // type
  const TString n1 = h1->ClassName();
  const TString n2 = h2->ClassName();

  if (n1.CompareTo(n2))
    {
      val += 1;
    }

  // dimension
  if (h1->GetDimension() != h2->GetDimension())
    {
      val += 100;
    }

  //   binning
  if ( h1->GetNbinsX() != h2->GetNbinsX() || 
       h1->GetNbinsY() != h2->GetNbinsY() || 
       h1->GetNbinsZ() != h2->GetNbinsZ())
    {
      val += 10000;
    }
  
  // variable sized binning

  if (!areArraysCompatible(h1->GetXaxis()->GetXbins(),h2->GetXaxis()->GetXbins()) ||
      !areArraysCompatible(h1->GetYaxis()->GetXbins(),h2->GetYaxis()->GetXbins()) ||
      !areArraysCompatible(h1->GetZaxis()->GetXbins(),h2->GetZaxis()->GetXbins()))
    {
      val += 100000;
    }

  // limits
  if (h1->GetXaxis()->GetXmin() != h2->GetXaxis()->GetXmin() ||
      h1->GetXaxis()->GetXmax() != h2->GetXaxis()->GetXmax() ||
      h1->GetYaxis()->GetXmin() != h2->GetYaxis()->GetXmin() ||
      h1->GetYaxis()->GetXmax() != h2->GetYaxis()->GetXmax() ||
      h1->GetZaxis()->GetXmin() != h2->GetZaxis()->GetXmin() ||
      h1->GetZaxis()->GetXmax() != h2->GetZaxis()->GetXmax()) 
    {
      val += 1000000;
    }
  return val;
}

////////////////////////////////////////////////////////////////////////////////

Bool_t PTools::areHistogramsCompatible(TH1* h1, TH1 *h2)
{
  // checks whether h1 and h1 
  //
  //   * have the same binning
  //   * have the same dimension
  //   * have the same limits
  //
  //   NOTE: it allows differences in the types (ClassNames)
  //   if you want to check for this use checkHistogramCompatibility directly
  //
  // returns kTRUE if they are compatible to these checks
  // returns kFALSE if not
  // returns also kFALSE, if h1 or h2 is a NULL pointer 

  Int_t val = checkHistogramCompatibility(h1, h2);
  
  if ( -1 < val && val < 100)
    {
      return kTRUE;
    }
  
  return kFALSE;
}

////////////////////////////////////////////////////////////////////////////////

Bool_t PTools::areArraysCompatible(const TArray* array1, const TArray* array2)
{
  // compares two TArrays 
  // in size, type and values
  //
  // if identical kTRUE is returned
  // else kFALSE
  // returns also kFALSE, if array1 or array2 is a NULL pointer 
  // 
  // supported types are TArrayC, TArrayD, TArrayF, TArrayI, TArrayL, TArrayS
  //
 
  // check for not NULL pointer
  if (!array1 || !array2)
    {
      Error("areArraysCompatible","can't compare, array1 (%p) or array2 (%p) is null pointer, returning -1",array1, array2);
      return -1;
    }

  // size 

  Int_t arraySize1 = array1->GetSize();
  Int_t arraySize2 = array2->GetSize();

  if (arraySize1 != arraySize2) 
    { 
      return kFALSE; 
    }

  // class names
  TString n1 = array1->IsA()->GetName();
  TString n2 = array2->IsA()->GetName();
  
  // type ???
  if (n1.CompareTo(n2))
    {
      return kFALSE;
    }

  // determine type number
  Int_t number = -1;

  if ( array1->IsA()->InheritsFrom("TArrayC") ) { number = 1; }
  if ( array1->IsA()->InheritsFrom("TArrayD") ) { number = 2; }
  if ( array1->IsA()->InheritsFrom("TArrayF") ) { number = 3; }
  if ( array1->IsA()->InheritsFrom("TArrayI") ) { number = 4; }
  if ( array1->IsA()->InheritsFrom("TArrayL") ) { number = 5; }
  if ( array1->IsA()->InheritsFrom("TArrayS") ) { number = 6; }

  // values

  switch(number)
    {
    case 1:
      for ( Int_t index=0; index < arraySize1; index++)
	{
	  if ( ((TArrayC*)array1)->At(index) != ((TArrayC*)array2)->At(index))
	    {
	      return kFALSE;
	    }
	}
      break;
    case 2:
      for ( Int_t index=0; index < arraySize1; index++)
	{
	  if ( ((TArrayD*)array1)->At(index) != ((TArrayD*)array2)->At(index))
	    {
	      return kFALSE;
	    }
	}
      break;
    case 3:
      for ( Int_t index=0; index < arraySize1; index++)
	{
	  if ( ((TArrayF*)array1)->At(index) != ((TArrayF*)array2)->At(index))
	    {
	      return kFALSE;
	    }
	}
      break;
    case 4:
      for ( Int_t index=0; index < arraySize1; index++)
	{
	  if ( ((TArrayI*)array1)->At(index) != ((TArrayI*)array2)->At(index))
	    {
	      return kFALSE;
	    }
	}
      break;
    case 5:
      for ( Int_t index=0; index < arraySize1; index++)
	{
	  if ( ((TArrayL*)array1)->At(index) != ((TArrayL*)array2)->At(index))
	    {
	      return kFALSE;
	    }
	}
      break;
    case 6:
      for ( Int_t index=0; index < arraySize1; index++)
	{
	  if ( ((TArrayS*)array1)->At(index) != ((TArrayS*)array2)->At(index))
	    {
	      return kFALSE;
	    }
	}
      break;
    default:
      ::Error("areArraysCompatible","unsupported type: \"%s\", returning kFALSE",array1->IsA()->ClassName());
      return kFALSE;
      break;
    }
  return kTRUE;

}

////////////////////////////////////////////////////////////////////////////////

TGraphErrors* PTools::fillGraphErr(TH1 *hist, TString name, TString position, Bool_t xerrors)
{
  // fills an TH1 histogramm hist to a TGraphError
  // depending on position
  // the x value is chosen 
  // "center" or "middle": in the middle of the bin
  // "low" or "left"     : at the lower edge of the bin
  // "up" or "right"     : at the upper edge of the bin
  //
  // if name is set, name is assigned as the objects name, otherwise "_graph" is put to the name of hist
  //
  // if xerrors is set xerrors are taken as 50% of the bin width
  //
  // returns pointer to TGraphError
  // if hist does not exists,
  //   NULL pointer ist returned
  // if invalid position is chosen (case insensitive)
  //   NULL pointer ist returned

  if (!hist)
    {
      Error(" fillGraphErr","hist is NULL, returning NULL pointer");
      return NULL;
    }

  if (name.IsNull())
    {
      name=hist->GetName();
      name+="_graph";
    }

  position.ToLower();
  Int_t swit = -1;
  if (!position.CompareTo("center")) {swit=1;}
  if (!position.CompareTo("middle")) {swit=1;}
  if (!position.CompareTo("low"))    {swit=2;}
  if (!position.CompareTo("left"))   {swit=2;}
  if (!position.CompareTo("up"))     {swit=3;}
  if (!position.CompareTo("right"))  {swit=3;}

  if (swit == -1)
    {
      Error("fillGraphErr",
	    "no valid position name %s (center,middle,low,left,up,right) chosen, returning NULL",
	    position.Data());
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();

  TArrayD xval(nbinsx);
  TArrayD yval(nbinsx);
  TArrayD xerrval(nbinsx);
  TArrayD yerrval(nbinsx);

  Int_t bin;

  for (Int_t binx=1; binx <= nbinsx; binx++)
    {
      bin = hist->GetBin(binx);
      yval[bin-1] = hist->GetBinContent(bin);
      yerrval[bin-1] = hist->GetBinError(bin);
      xerrval[bin-1] = 0.5*hist->GetBinWidth(bin);
      
      switch(swit)
	{
	case 1: // center middle
	  xval[bin-1] = hist->GetXaxis()->GetBinCenter(bin);
	  break;
	case 2: // left low
	  xval[bin-1] = hist->GetXaxis()->GetBinLowEdge(bin);
	  break;
	case 3: // right up
	  xval[bin-1] = hist->GetXaxis()->GetBinUpEdge(bin);
	  break;
	}
    }


  TGraphErrors* graph = NULL;
  if (xerrors)
    {
      graph = new TGraphErrors(nbinsx, 
			       xval.GetArray(), 
			       yval.GetArray(), 
			       xerrval.GetArray(),  
			       yerrval.GetArray());
    }
  else
    {
      graph = new TGraphErrors(nbinsx, 
			       xval.GetArray(), 
			       yval.GetArray(), 
			       0,  
			       yerrval.GetArray());
    }

  return graph;
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::getHistStats(TH1 *hist, Bool_t quiet)
{
  // returns for a given (plotted) histogramm the pointer to the statistics TPaveStat
  
  if (hist)
    {
      TPaveStats* stats = ((TPaveStats*) hist->GetListOfFunctions()->FindObject("stats"));
      if (stats)
	{
	  return stats;
	}
      else
	{
	  if (! quiet) {::Error("getHistStats","stats do not exists, returning NULL pointer");}
	  return stats;
	}
    }
  else
    {
      ::Error("getHistStats","histogram is NULL pointer, returning NULL pointer");
      return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::getHistStats(TPad* pad, TString name, Bool_t quiet, Bool_t noUpdate)
{
  // returns for a given TCanvas/TPad pointer to the statistics TPaveStat of a
  // histogram with name "name"
  // before calling getHistStats(hist) a pad->Update() is executed
  // 

  if (pad)
    {
      if (!noUpdate) pad->Update();
      return getHistStats(((TH1*) pad->FindObject(name.Data())), quiet);
    }  
  else
    {
      if (!quiet){::Error("getHistStats()","pad does not exist, returning NULL pointer");}
      return NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::setHistStatsYPosNDC(TH1 *hist, Double_t y1, Double_t y2)
{
  // sets y1 and y2 position in NDC coordinates of stats box of histogramm
  // returns pointer to stats' TPaveStats

  TPaveStats *stats = getHistStats(hist);
    if (stats)
      {
	stats->SetY1NDC(y1);
	stats->SetY2NDC(y2);
      }
    else
      {
	::Warning("setHistStatsYPosNDC","couldn't change position");
      }
    return stats;
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::setHistStatsYPosNDC(TPad* pad, TString name, Double_t y1, Double_t y2)
{
  // sets y1 and y2 position in NDC coordinates of stats box of histogramm
  // with name "name" in pad 
  // returns pointer to stats' TPaveStats

  if (pad)
    {
      return setHistStatsYPosNDC(((TH1*) pad->FindObject(name.Data())),y1,y2);
    }  
  else
    {
      ::Error("setHistStatsYPosNDC()","pad does not exist, returning NULL pointer");
      return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::setHistStatsXPosNDC(TH1 *hist, Double_t x1, Double_t x2)
{
  // sets x1 and x2 position in NDC coordinates of stats box of histogramm
  // returns pointer to stats' TPaveStats

  TPaveStats *stats = getHistStats(hist);
    if (stats)
      {
	stats->SetX1NDC(x1);
	stats->SetX2NDC(x2);
      }
    else
      {
	::Warning("setHistStatsXPosNDC","couldn't change position");
      }
    return stats;
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::setHistStatsXPosNDC(TPad* pad, TString name, Double_t x1, Double_t x2)
{
  // sets x1 and x2 position in NDC coordinates of stats box of histogramm
  // with name "name" in pad 
  // returns pointer to stats' TPaveStats

  if (pad)
    {
      return setHistStatsXPosNDC(((TH1*) pad->FindObject(name.Data())),x1,x2);
    }  
  else
    {
      ::Error("setHistStatsXPosNDC()","pad does not exist, returning NULL pointer");
      return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::setHistStatsPosNDC(TH1 *hist, Double_t x1, Double_t x2, Double_t y1, Double_t y2)
{
  // sets x1,y1 and x2,y2 position in NDC coordinates of stats box of histogramm
  // returns pointer to stats' TPaveStats

  TPaveStats *stats = getHistStats(hist);
    if (stats)
      {
	stats->SetX1NDC(x1);
	stats->SetX2NDC(x2);
	stats->SetY1NDC(y1);
	stats->SetY2NDC(y2);
      }
    else
      {
	::Warning("setHistStatsPosNDC","couldn't change position");
      }
    return stats;
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::setHistStatsPosNDC(TPad* pad, TString name, Double_t x1, Double_t x2, Double_t y1, Double_t y2)
{
  // sets x1,y1 and x2,y2 position in NDC coordinates of stats box of histogramm
  // with name "name" in pad 
  // returns pointer to stats' TPaveStats

  if (pad)
    {
      return setHistStatsPosNDC(((TH1*) pad->FindObject(name.Data())),x1,x2,y1,y2);
    }  
  else
    {
      ::Error("setHistStatsPosNDC()","pad does not exist, returning NULL pointer");
      return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::getHistStatsYPosNDC(TH1 *hist, Double_t& y1, Double_t& y2)
{
  // gets y1 and y2 position in NDC coordinates of stats box of histogramm
  // returns pointer to stats' TPaveStats

  TPaveStats *stats = getHistStats(hist);
    if (stats)
      {
	y1 = stats->GetY1NDC();
	y2 = stats->GetY2NDC();
      }
    else
      {
	::Warning("getHistStatsYPosNDC","couldn't change position");
      }
    return stats;
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::getHistStatsYPosNDC(TPad* pad, TString name, Double_t& y1, Double_t& y2)
{
  // gets y1 and y2 position in NDC coordinates of stats box of histogramm
  // with name "name" in pad 
  // returns pointer to stats' TPaveStats

  if (pad)
    {
      return getHistStatsYPosNDC(((TH1*) pad->FindObject(name.Data())),y1,y2);
    }  
  else
    {
      ::Error("getHistStatsYPosNDC()","pad does not exist, returning NULL pointer");
      return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::getHistStatsXPosNDC(TH1 *hist, Double_t& x1, Double_t& x2)
{
  // gets x1 and x2 position in NDC coordinates of stats box of histogramm
  // returns pointer to stats' TPaveStats

  TPaveStats *stats = getHistStats(hist);
    if (stats)
      {
	x1 = stats->GetX1NDC();
	x2 = stats->GetX2NDC();
      }
    else
      {
	::Warning("getHistStatsXPosNDC","couldn't change position");
      }
    return stats;
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::getHistStatsXPosNDC(TPad* pad, TString name, Double_t& x1, Double_t& x2)
{
  // gets x1 and x2 position in NDC coordinates of stats box of histogramm
  // with name "name" in pad 
  // returns pointer to stats' TPaveStats

  if (pad)
    {
      return getHistStatsXPosNDC(((TH1*) pad->FindObject(name.Data())),x1,x2);
    }  
  else
    {
      ::Error("getHistStatsXPosNDC()","pad does not exist, returning NULL pointer");
      return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::getHistStatsPosNDC(TH1 *hist, Double_t& x1, Double_t& x2, Double_t& y1, Double_t& y2)
{
  // gets x1,y1 and x2,y2 position in NDC coordinates of stats box of histogramm
  // returns pointer to stats' TPaveStats

  TPaveStats *stats = getHistStats(hist);
    if (stats)
      {
	x1 = stats->GetX1NDC();
	x2 = stats->GetX2NDC();
	y1 = stats->GetY1NDC();
	y2 = stats->GetY2NDC();
      }
    else
      {
	::Warning("getHistStatsPosNDC","couldn't change position");
      }
    return stats;
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::getHistStatsPosNDC(TPad* pad, TString name, Double_t& x1, Double_t& x2, Double_t& y1, Double_t& y2)
{
  // gets x1,y1 and x2,y2 position in NDC coordinates of stats box of histogramm
  // with name "name" in pad 
  // returns pointer to stats' TPaveStats

  if (pad)
    {
      return getHistStatsPosNDC(((TH1*) pad->FindObject(name.Data())),x1,x2,y1,y2);
    }  
  else
    {
      ::Error("getHistStatsPosNDC()","pad does not exist, returning NULL pointer");
      return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::setHistStatsTextColor(TH1 *hist, Int_t color)
{
  // sets text color of stats box of histogramm
  // returns pointer to stats' TPaveStats

  TPaveStats *stats = getHistStats(hist);
    if (stats)
      {
	stats->SetTextColor(color);
      }
    else
      {
	::Warning("setHistStatsTextColorNDC","couldn't change textColorition");
      }
    return stats;
}

////////////////////////////////////////////////////////////////////////////////

TPaveStats* PTools::setHistStatsTextColor(TPad* pad, TString name, Int_t color)
{
  // sets text color of stats box of histogramm
  // with name "name" in pad 
  // returns pointer to stats' TPaveStats

  if (pad)
    {
      return setHistStatsTextColor(((TH1*) pad->FindObject(name.Data())),color);
    }  
  else
    {
      ::Error("setHistStatsTextColor()","pad does not exist, returning NULL pointer");
      return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////

void PTools::setLatexArts(TLatex * tex, 
			  Double_t x, Double_t y, 
			  Int_t color, Float_t size, Int_t align, 
			  Int_t font, Float_t angle, Bool_t ndc)
{
  // sets attributes of TLatex object tex:
  // x,y positions (choose setLatexArtsNDC for NDC coords)
  // color
  // size
  // alignment
  // font
  // angle
  // ndc: use ndc coordinates
  //
  // see TAttText for detailed information

  if (tex)
    {
      tex->SetNDC(ndc);
      tex->SetX(x);
      tex->SetY(y);
      tex->SetTextColor(color);
      tex->SetTextSize(size);
      tex->SetTextAlign(align);
      tex->SetTextFont(font);
      tex->SetTextAngle(angle);
    }
  else
    {
      ::Error("setLatexArts()","tex pointer does not exist, no changes");
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void PTools::setLatexArtsNDC(TLatex * tex, 
			     Double_t x, Double_t y, 
			     Int_t color, Float_t size, Int_t align, 
			     Int_t font, Float_t angle)
{
  // sets attributes of TLatex object tex:
  // x,y positions in NDC coords
  // color
  // size
  // alignment
  // font
  // angle
  // see TAttText for detailed information

  setLatexArts(tex,x,y,color,size,align,font,angle,kTRUE);
}

////////////////////////////////////////////////////////////////////////////////

Bool_t  PTools::loadMacro(const Char_t *macro)
{
  // loads Macro only if macro has not yet been loaded before
  // returns true if macro is loaded
  // returns true if no action is necessary

    if (!gInterpreter 
	|| 
	(gInterpreter && (!gInterpreter->IsLoaded(macro))))
      {
	gROOT->LoadMacro(macro);
	return kTRUE;
      }
    return kFALSE;
}

////////////////////////////////////////////////////////////////////////////////

Bool_t  PTools::loadMacro(TString macro)
{
  // loads Macro only if macro has not yet been loaded before
  // returns true if macro is loaded
  // returns true if no action is necessary
  
  return loadMacro(macro.Data());
}

////////////////////////////////////////////////////////////////////////////////

Int_t PTools::getLinearIndex( Int_t x1, UInt_t x1max, 
			      Int_t x2, UInt_t x2max, 
			      Int_t x3, UInt_t x3max, 
			      Int_t x4, UInt_t x4max, 
			      Int_t x5, UInt_t x5max)
{
  // Translates i.e. array coordinate 0,2,3 of array[3][8][1]
  // in a linear unique coordinate starting from 0
  // 
  // coordinates starting from 0 !!!
  //
  // if x(i) is < 0 then only the x(i-1) coords are taken into account
  // if indizes are out of bounds -1 is returned

  Int_t x[5];
  UInt_t xmax[5];

  x[0] = x1;
  x[1] = x2;
  x[2] = x3;
  x[3] = x4;
  x[4] = x5;

  xmax[0] = x1max;
  xmax[1] = x2max;
  xmax[2] = x3max;
  xmax[3] = x4max;
  xmax[4] = x5max;

  UInt_t dim = 5;
  for (UInt_t iter = 1; iter < dim; iter++)
    {
      if (x[iter] < 0 )
	{
	  dim =iter;

	  for (UInt_t fiter = iter-1; fiter < dim-1; fiter++)
	    {
	      x[iter] = 0;
	      xmax[fiter]=0;	      
	    }
	  break;
	}
    }

  for (UInt_t iter = 1; iter < dim; iter++)
    {
      if ((UInt_t) x[iter] >= xmax[iter])
	{
	  cout << x[iter] << endl;
	  cout << xmax[iter] << endl;
	  ::Error("getLinearIndex","index %i out of bounds %i, returning -1",x[iter],xmax[iter]);
	  return -1;
	}
    }

  Int_t index = x[0];
  Int_t add;
  for (UInt_t iter = 1; iter < dim; iter++)
    {
      add = 1;
      for (UInt_t iter2 = 0; iter2 < iter; iter2++)
	{	
	  add *= xmax[iter2];
	}
      index += x[iter]*add;
    }

  return index;  
}

////////////////////////////////////////////////////////////////////////////////

Int_t PTools::getLinearIndex( Int_t *x, UInt_t *xmax, UInt_t dim)
{
  // Translates i.e. array coordinate 0,2,3 of array[3][8][1]
  // in a linear unique coordinate starting from 0
  // 
  // coordinates starting from 0 !!!
  //
  // if x(i) is < 0 then only the x(i-1) coords are taken into account
  // x is contains the coordinates and has dimension dim
  // xmax are the sizes and is of dimension dim
  // if indizes are out of bounds -1 is returned

  if (x[0]<0)
    {
      ::Error("getLinearIndex","Bad Index return -1");
    }
   

  for (UInt_t iter = 1; iter < dim; iter++)
    {
      if (x[iter] < 0 )
	{
	  for (UInt_t fiter = iter-1; fiter < dim-1; fiter++)
	    {
	      x[iter] = 0;
	      xmax[fiter]=0;	      
	    }
	  break;
	}
    }

  for (UInt_t iter = 1; iter < dim; iter++)
    {
      if ((UInt_t) x[iter] >= xmax[iter])
	{
// 	  cout << x[iter] << endl;
// 	  cout << xmax[iter] << endl;
	  ::Error("getLinearIndex","index %i out of bounds %i, returning -1",x[iter],xmax[iter]);
	  return -1;
	}
    }

//   UInt_t index = x[dim-1];
//   for (UInt_t ai = dim-1; ai > 1 ; ai--)
//     {
//       index = x[ai-1] + xmax[ai-1] * index;
//     }

  Int_t index = x[0];
  Int_t add;
  for (UInt_t iter = 1; iter < dim; iter++)
    {
      add = 1;
      for (UInt_t iter2 = 0; iter2 < iter; iter2++)
	{	
	  add *= xmax[iter2];
	}
      index += x[iter]*add;
    }

  return index;  
}

////////////////////////////////////////////////////////////////////////////////

TLatex * PTools::drawLatexWC(TString text, Double_t x, Double_t y, 
			     Int_t color, Float_t size, Int_t align, Int_t font, Float_t angle)
{
  // Draws text as TLatex in current pad 
  // at pos x,y not in NDC but it WC
  // using setLatexArtsNDC
  // returns pointer to new created TLatex object (user must care of removal)
  TLatex *tex   = new TLatex(0,0,text.Data());
  PTools::setLatexArts(tex, x, y, color, size, align, font, angle);
  tex->Draw();
  return tex;
}

////////////////////////////////////////////////////////////////////////////////

TLatex * PTools::drawLatexWC(const Char_t* text, Double_t x, Double_t y, 
			   Int_t color, Float_t size, Int_t align, Int_t font, Float_t angle)
{
  // Draws text as TLatex in current pad 
  // at pos x,y not in NDC but it WC
  // using setLatexArtsNDC
  // returns pointer to new created TLatex object (user must care of removal)
  return drawLatexWC(TString(text), x, y, color, size, align, font, angle);
}

////////////////////////////////////////////////////////////////////////////////

TLatex * PTools::drawLatexNDC(const Char_t* text, Double_t x, Double_t y, 
			      Int_t color, Float_t size, Int_t align, Int_t font, Float_t angle)
{
  // Draws text as TLatex in current pad 
  // at pos x,y in NDC
  // using setLatexArtsNDC
  // returns pointer to new created TLatex object (user must care of removal)
  return drawLatexPadCoords(text, x, y, color , size, align ,font , angle);
}

////////////////////////////////////////////////////////////////////////////////

TLatex * PTools::drawLatexNDC(TString text, Double_t x, Double_t y, 
			      Int_t color, Float_t size, Int_t align, Int_t font, Float_t angle)
{
  // Draws text as TLatex in current pad 
  // at pos x,y in NDC
  // using setLatexArtsNDC
  // returns pointer to new created TLatex object (user must care of removal)
  return drawLatexPadCoords(text, x, y, color , size, align ,font , angle);
}

////////////////////////////////////////////////////////////////////////////////

TLatex * PTools::drawLatex(TString text, Double_t x, Double_t y, 
			   Int_t color, Float_t size, Int_t align, Int_t font, Float_t angle)
{
  // Draws text as TLatex in current pad 
  // at pos x,y in NDC
  // using setLatexArtsNDC
  // returns pointer to new created TLatex object (user must care of removal)
  TLatex *tex   = new TLatex(0,0,text.Data());
  PTools::setLatexArtsNDC(tex, x, y, color, size, align, font, angle);
  tex->Draw();
  return tex;
}

////////////////////////////////////////////////////////////////////////////////

TLatex * PTools::drawLatex(const Char_t* text, Double_t x, Double_t y, 
			   Int_t color, Float_t size, Int_t align, Int_t font, Float_t angle)
{
  // Draws text as TLatex in current pad 
  // at pos x,y in NDC
  // using setLatexArtsNDC
  // returns pointer to new created TLatex object (user must care of removal)
  return drawLatex(TString(text), x, y, color, size, align, font, angle);
}

////////////////////////////////////////////////////////////////////////////////

TLatex * PTools::drawLatexPadCoords(TString text, Double_t x, Double_t y, 
				    Int_t color, Float_t size, Int_t align, Int_t font, Float_t angle)
{
  // Draws text as TLatex in current pad 
  // at pos x,y in NDC
  // using setLatexArts
  // returns pointer to new created TLatex object (user must care of removal)
  TLatex *tex   = new TLatex(0,0,text.Data());
  PTools::setLatexArts(tex, x, y, color, size, align, font, angle);
  tex->Draw();
  return tex;
}

////////////////////////////////////////////////////////////////////////////////

TLatex * PTools::drawLatexPadCoords(const Char_t* text, Double_t x, Double_t y, 
				    Int_t color, Float_t size, Int_t align, Int_t font, Float_t angle)
{
  // Draws text as TLatex in current pad 
  // at pos x,y in NDC
  // using setLatexArts
  // returns pointer to new created TLatex object (user must care of removal)
  return drawLatexPadCoords(TString(text), x, y, color, size, align, font, angle);
}

////////////////////////////////////////////////////////////////////////////////

TH1* PTools::getErrorsOfHistogram(TH1 * hist, Bool_t relativeError)
{
  // creates histogram 
  // which contains the errors of hist
  // if an error occurs NULL is returned
  // the name and title of the new histogramm will expanded by "_Error" or "_relError"
  // relativeError is kTRUE then |relativeError| is calculated
  //   (if hist does not contain in a bin the relative error is set to -1)
  // otherwise the absolute error is given

  if (!hist)
    {
      ::Error("getErrorsOfHistogram"," hist is NULL pointer ... return NULL");
      return NULL;
    }

  TH1* err = (TH1*) hist->Clone();
  err->Reset();

  TString name  = hist->GetName();
  TString title = hist->GetTitle();

  if (!relativeError)
    {
      name+="_Error";
      title+="_Error";
    }
  else
    {
      name+="_relError";
      title+="_relError";
    }

  err->SetName(name.Data());
  err->SetTitle(title.Data());

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      if (! relativeError)
		{
		  err->SetBinContent(bin,hist->GetBinError(bin));
		}
	      else
		{
		  if (hist->GetBinContent(bin))
		    {
		     err->SetBinContent(bin,fabs(hist->GetBinError(bin)/hist->GetBinContent(bin)));
		    }
		  else
		    {
		      err->SetBinContent(bin,-1);
		    }
		}
	    }
	}
    }
  return err;

}

////////////////////////////////////////////////////////////////////////////////

void PTools::setErrorsOfHistogram(TH1 * hist, TH1 *err)
{
  // sets the errors of histogram hist with the bin contents of hist err
  // provided the histograms are compatible
  //
  // if hist or err are NULL pointer, or the histograms are not compatible, 
  // an error message is issued, but nothing happens  
   
  if (!hist)
    {
      ::Error("setErrorsOfHistogram"," hist is NULL pointer ... nothing done");
      return;
    }

  if (!err)
    {
      ::Error("setErrorsOfHistogram"," err is NULL pointer ... nothing done");
      return;
    }

  // compatibility

  Int_t check = checkHistogramCompatibility(hist, err);
  if (check > 0)
    {
      ::Error("setErrorsOfHistogram"," err and hist are not compatible %i ... nothing done",check);
      return;
    }

  //

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      hist->SetBinError(bin, err->GetBinContent(bin));
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////////////

TH1* PTools::copyToUnitBinsPlusLabels(TH1* hist , Double_t binningX, Double_t binningY, Double_t binningZ)
{
  // copies any THx histogramm to THxD histograms
  // you have to cast to the corresponding type
  // attaches to name "_THxD"
  // the histogramm has as many bins as the original but a binning of binningXYZ [default =1]

  if (! hist)
    {
      ::Error("copyToUnitBinsPlusLabels","histogram has NULL pointer ... returning NULL");
      return NULL;
    }
  
  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();

  TH1* histD = NULL;

  switch(hist->GetDimension())
    {
    case 1:
      {
	TString name = hist->GetName()+TString("_TH1D");
	histD = new TH1D(name.Data(),hist->GetTitle(),
			 nbinsx,
			 0, 
			 binningX*nbinsx);
	break;
      }
    case 2:
      {
	TString name = hist->GetName()+TString("_TH2D");
	
	histD = new TH2D(name.Data(),hist->GetTitle(),
			 nbinsx,
			 0, 
			 binningX*nbinsx,
			 nbinsy,
			 0, 
			 binningY*nbinsy);
	break;
      }
    case 3:
      {
	TString name = hist->GetName()+TString("_TH3D");

	histD = new TH3D(name.Data(),hist->GetTitle(),
			 nbinsx,
			 0, 
			 binningX*nbinsx,
			 nbinsy,
			 0, 
			 binningY*nbinsy,
			 nbinsz,
			 0, 
			 binningZ*nbinsz);

	break;
      }
    }
  
  importAxisAttributes(hist->GetXaxis(), histD->GetXaxis(), kTRUE);
  importAxisAttributes(hist->GetYaxis(), histD->GetYaxis(), kTRUE);
  importAxisAttributes(hist->GetZaxis(), histD->GetZaxis(), kTRUE);

  histD->SetEntries(hist->GetEntries());
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;
  
  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      histD->SetBinContent(bin, hist->GetBinContent(bin));
	      if (hist->GetSumw2N())
		{
		  histD->SetBinError(bin,hist->GetBinError(bin));
		}
	    }
	}
    }
  return histD;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::substractFromHist(TH1 * hist, Double_t val, Bool_t inverse)
{
  // substracts from each bin of histogram the value val newbin = bin - val
  // if inverse = kTRUE: newbin = val - bin

  if (!hist)
    {
      ::Error("substractFromHist"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      if (!inverse)
		{
		  hist->SetBinContent(bin,hist->GetBinContent(bin) - val);
		}
	      else
		{
		  hist->SetBinContent(bin,val - hist->GetBinContent(bin));
		}
	    }
	}
    }
  return;

}

////////////////////////////////////////////////////////////////////////////////
TH1D *PTools::getBinContentDistribution(TH1* hist, Int_t binning, Bool_t excludeUnderAndOverFlowBins)
{
    // returns a pointer to a histogram containing the distribution of the bin contents
    // by default in 1000 bins
    //
    // The histogram range is determined by searching for the minimum and maximum bin content
    // and then setting the output histograms minimum to the found minimum,
    // the maximum to maximum+(maximum - minimum)/binning, i.e. maximum+binWidth
    // this one bin more is accounted for by having not binning, but binning + 1 bins.
    //
    // if hist is NULL ... NULL is returned
    //
    // Also the underflow and overflow bins are by default taken into account
    // -- those can be excluded by setting excludeUnderAndOverFlowBins to kTRUE

    if (!hist)
    {
	::Error("getBinContentDistribution"," hist is NULL pointer ... cannot act on a NULL pointer.");
	return NULL;
    }

    Int_t nbinsx = hist->GetNbinsX();
    Int_t nbinsy = hist->GetNbinsY();
    Int_t nbinsz = hist->GetNbinsZ();

    if (hist->GetDimension()<2) nbinsy = -1;
    if (hist->GetDimension()<3) nbinsz = -1;

    Int_t binx,biny,binz,bin;

    Double_t value;
    Double_t maximum = -FLT_MAX/10;
    Double_t minimum =  FLT_MAX/10;

    for (binz=0;binz<=nbinsz+1;binz++)
    {
	if (excludeUnderAndOverFlowBins && nbinsz != -1 && ( binz == 0 || binz == nbinsz+1)) { continue; }
	for (biny=0;biny<=nbinsy+1;biny++)
	{
	    if (excludeUnderAndOverFlowBins && nbinsy != -1 && ( biny == 0 || biny == nbinsy+1)) { continue; }
	    for (binx=0;binx<=nbinsx+1;binx++)
	    {
		if (excludeUnderAndOverFlowBins && nbinsx != -1 && ( binx == 0 || binx == nbinsx+1)) { continue; }
		bin = hist->GetBin(binx,biny,binz);
		value = hist->GetBinContent(bin);

		if (value > maximum) { maximum = value;}
		if (value < minimum) { minimum = value;}
	    }
	}
    }

    if (binning<1)
    {
	binning=1000;
    }

    TH1D* dist = new TH1D(Form("binContent_of_%s",hist->GetName()),
			  Form("bin content distribution of : %s",hist->GetTitle()),
			  binning+1,minimum,maximum+(maximum-minimum)/binning);
    for (binz=0;binz<=nbinsz+1;binz++)
    {
	if (excludeUnderAndOverFlowBins && nbinsz != -1 && ( binz == 0 || binz == nbinsz+1)) { continue; }
	for (biny=0;biny<=nbinsy+1;biny++)
	{
	    if (excludeUnderAndOverFlowBins && nbinsy != -1 && ( biny == 0 || biny == nbinsy+1)) { continue; }
	    for (binx=0;binx<=nbinsx+1;binx++)
	    {
		if (excludeUnderAndOverFlowBins && nbinsx != -1 && ( binx == 0 || binx == nbinsx+1)) { continue; }
		bin = hist->GetBin(binx,biny,binz);
		dist->Fill(hist->GetBinContent(bin));
	    }
	}
    }
    return dist;
}

////////////////////////////////////////////////////////////////////////////////

TH1 *PTools::getOneOverHist(TH1* hist)
{
  // returns a pointer to a histogram containing for each bin the value 1/value
  // if value = 0, the ratio is set to 0
  //
  // the name is "OneOverHist" and the title "1/hist"
  // if hist is NULL ... NULL is returned

  if (!hist)
    {
      ::Error("getOneOverHist"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;

  Double_t value, error;
  
  TH1* inv = (TH1*) copyToTHxD(hist);
  TString name = Form("OneOver_%s",inv->GetName());
  TString title = Form("1./%s",inv->GetTitle());

  inv->SetNameTitle(name.Data(), title.Data());
  inv->Reset();
  
  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      value = hist->GetBinContent(bin);
	      error = hist->GetBinError(bin);
	      inv->SetBinContent(bin,(value==0)?0.:1./value);
	      inv->SetBinError(bin,(pow(value,2)==0)?0.:error/pow(value,2));
	    }
	}
    }
  return inv;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::setHistErrors(TH1* h,TH1* h2)
{
  // sets bincontent of hist h2 as errors of h
  // lent from jochen
  if(h!=0&&h2!=0)
    {
      TString classname=h->ClassName();
      Int_t type=0;
      
      if(classname.Contains("TH1")==1)type=1;
      if(classname.Contains("TH2")==1)type=2;
      
      Int_t binsX=0,binsY=0;
      
      binsX=h->GetNbinsX();
      if(type==2)binsY=h->GetNbinsY();
      
      Double_t bincontent;
      
      if(type==1)
        {
	  for(Int_t x=0;x<=binsX;x++)
            {
	      bincontent= h2->GetBinContent(x+1);
	      h->SetBinError(x+1,bincontent);
            }
        }
      if(type==2)
        {
	  for(Int_t x=0;x<=binsX;x++)
            {
	      for(Int_t y=0;y<=binsY;y++){
		bincontent= h2->GetBinContent(x+1,y+1);
		h->SetBinError(x+1,y+1,bincontent);
	      }
            }
        }
    }
    else
      {
        cout<<"Warning: PTools::cleanHist : ZERO pointer for hist recieved!"<<endl;
      }
}
////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////


// TH1* PTools::divide(const TH1 *h1, const TH1 *h2, Double_t c1, Double_t c2, Option_t *option)
// {
//   // following calculation
//   //
//   // h1 = c1*h1/(c2*h2)
//   //
//   //   if errors are defined (see TH1::Sumw2), errors are also recalculated
//   //   Note that if h1 or h2 have Sumw2 set, Sumw2 is automatically called for this
//   //   if not already set.
//   //   The resulting errors are calculated assuming uncorrelated histograms.
//   //   However, if option ="B" is specified, Binomial errors are computed.
//   //
//   // IMPORTANT NOTE: If you intend to use the errors of this histogram later
//   // you should call Sumw2 before making this operation.
//   // This is particularly important if you fit the histogram after TH1::Divide

//    TString opt = option;
//    opt.ToLower();
//    Bool_t binomial = kFALSE;
//    if (opt.Contains("b")) binomial = kTRUE;

//   if (!h1)
//     {
//       ::Error("setErrorsOfHistogram"," h1 is NULL pointer ... nothing done");
//       return;
//     }

//   if (!h2)
//     {
//       ::Error("setErrorsOfHistogram"," h2 is NULL pointer ... nothing done");
//       return;
//     }

//   // compatibility

//   Int_t check = checkHistogramCompatibility(h1, h2);
//   if (check > 0)
//     {
//       ::Error("setErrorsOfHistogram"," h2 and h1 are not compatible %i ... nothing done",check);
//       return;
//     }

//   //

//   Int_t nbinsx = h1->GetNbinsX();
//   Int_t nbinsy = h1->GetNbinsY();
//   Int_t nbinsz = h1->GetNbinsZ();
  
//   if (h1->GetDimension()<2) nbinsy = -1;
//   if (h1->GetDimension()<3) nbinsz = -1;

//   //    Create Sumw2 if h1 or h2 have Sumw2 set
//   if (h1->GetSumw2N() == 0 || h2->GetSumw2N() != 0) h1->Sumw2();
//   if (h1->GetSumw2N() != 0 || h2->GetSumw2N() == 0) h2->Sumw2();

//   //    Reset the kCanRebin option. Otherwise SetBinContent on the overflow bin
//   //    would resize the axis limits!
//   ResetBit(kCanRebin);

//    Int_t bin, binx, biny, binz;
//    Double_t b1,b2,w,d1,d2;
//    d1 = c1*c1;
//    d2 = c2*c2;
//    for (binz=0;binz<=nbinsz+1;binz++) {
//       for (biny=0;biny<=nbinsy+1;biny++) {
//          for (binx=0;binx<=nbinsx+1;binx++) {
//             bin = binx +(nbinsx+2)*(biny + (nbinsy+2)*binz);
//             b1  = h1->GetBinContent(bin);
//             b2  = h2->GetBinContent(bin);
//             if (b2) w = c1*b1/(c2*b2);
//             else    w = 0;
//             SetBinContent(bin,w);
//             fEntries++;
//             if (fSumw2.fN) {
//                Double_t e1 = h1->GetBinError(bin);
//                Double_t e2 = h2->GetBinError(bin);
//                Double_t b22= b2*b2*d2;
//                if (!b2) { fSumw2.fArray[bin] = 0; continue;}
//                if (binomial) {
//                   fSumw2.fArray[bin] = TMath::Abs(w*(1-w)/b2);
//                } else {
//                   fSumw2.fArray[bin] = d1*d2*(e1*e1*b2*b2 + e2*e2*b1*b1)/(b22*b22);
//                }
//             }
//          }
//       }
//    }

//   Int_t binx,biny,binz,bin;

//   for (binz=0;binz<=nbinsz+1;binz++)
//     {
//       for (biny=0;biny<=nbinsy+1;biny++)
// 	{
// 	  for (binx=0;binx<=nbinsx+1;binx++)
// 	    {
// 	      bin = hist->GetBin(binx,biny,binz);
// 	      hist->SetBinError(bin, err->GetBinContent(bin));
// 	    }
// 	}
//     }
// }


//   // modified Divide from TH1 of 400-08 
// //   -*-*-*Replace contents of this histogram by the division of h1 by h2*-*-*
// //         ==============================================================
// //
// //   hist = c1*h1/(c2*h2)
// //

// //   - Reset statistics
//    Double_t nEntries = fEntries;
//    fEntries = fTsumw = 0;

//    SetMinimum();
//    SetMaximum();

// //   - Loop on bins (including underflows/overflows)


//    Stat_t s[10];
//    GetStats(s);
//    PutStats(s);
//    if (nEntries == 0) nEntries = h1->GetEntries();
//    if (nEntries == 0) nEntries = 1;
//    SetEntries(nEntries);
// }


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

TH2D *PTools::reBinHistX(TH2* hist,Int_t factor)
{
  // returns a pointer to a histogram containing for each bin the value 1/value
  // if value = 0, the ratio is set to 0
  //
  // the name is "OneOverHist" and the title "1/hist"
  // if hist is NULL ... NULL is returned

  if (!hist)
    {
      ::Error("getOneOverHist"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin=0;

  Double_t value, error;
  
  TString name = hist->GetName();
  name+="_rebin";

  TH2D* inv = new TH2D(name.Data(),name.Data(),
		       Int_t (nbinsx/factor), hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax(),
		       Int_t (nbinsy), hist->GetYaxis()->GetXmin(),hist->GetYaxis()->GetXmax());
  
  importAxisAttributes(hist->GetXaxis(), inv->GetXaxis(), kTRUE);
  importAxisAttributes(hist->GetYaxis(), inv->GetYaxis(), kTRUE);
  importAxisAttributes(hist->GetZaxis(), inv->GetZaxis(), kTRUE);

  inv->Reset();
  Int_t index=0;

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      if (binx==0 || binx == nbinsx+1)
		{
		  bin = hist->GetBin(binx,biny,binz);
		  value = hist->GetBinContent(bin);
		  error = hist->GetBinError(bin);
		  inv->SetBinContent(bin,(value==0)?0.:1./value);
		  inv->SetBinError(bin,(pow(value,2)==0)?0.:error/pow(value,2));
		}
	      else
		{
		  value=0;
		  error=0;
		  for (Int_t fctr=0; fctr < factor; fctr++)
		    {
		      if (fctr==nbinsx+1) {break;}
		      
		      bin    = hist->GetBin(binx+fctr,biny,binz);
		      value += hist->GetBinContent(bin);
		      error += pow(hist->GetBinError(bin),2);
		    }
		  inv->SetBinContent(index,value);
		  inv->SetBinError(index,::std::sqrt(error));
		  binx+=(factor-1);
		}
	      index++;
	    }
	}
    }
  inv->SetStats(0);
  return inv;
}

////////////////////////////////////////////////////////////////////////////////

void PTools::cleanHistogramPtyOnP(TH2 * hist, Double_t mass, Double_t min, Double_t max, Double_t replaceBy)
{
  // cleans histograms
  // bin contents out of the limits for p min and max 
  // are set to replaceBy
  // errors are set to error 0
  // mass of particle needed 

  if (!hist)
    {
      ::Error("cleanHistogram"," hist is NULL pointer ... return NULL");
      return;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) nbinsy = -1;
  nbinsz = -1;

  Int_t binx,biny,binz,bin;
  Double_t p=0.;
  Double_t pt;
  Double_t y;

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  pt = hist->GetYaxis()->GetBinCenter(biny);
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      y = hist->GetXaxis()->GetBinCenter(binx);
	      p = sinh(y)*::std::sqrt(2 *(pow(pt,2)+pow(mass,2)));

	      if (p < min || p > max)
		{
		  hist->SetBinContent(bin,replaceBy);
		  hist->SetBinError(bin,0);
		}
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////////////

Int_t PTools::findEndOfDistribution(TH1* hist, Int_t min, Double_t threshold, Bool_t compact)
{
  // returns bin number where distribution ends 
  // i.e where no data is beyond threshold
  // starting from left to right at bin number min
  // returns -1 if hist is NULL
  // if compact is set no bin larger min is accepted to be below threshold
  // otherwise wholes of 1 bins are allowed

  if (!hist)
    {
      ::Error("findEndOfDistribution"," hist is NULL pointer ... return -1");
      return -1;
    }

  if (min<1) {min=1;}

  Int_t nbinsx = hist->GetNbinsX();

  Int_t binIter;

  for (binIter=min; binIter<=nbinsx; binIter++)
    {
      if (hist->GetBinContent(binIter) > threshold) { continue;}
      if (compact)
	{
	  if ((binIter+1 > nbinsx) ||(hist->GetBinContent(binIter+1) <= threshold)) break;
	}
      else
	{
	  if ( ((binIter+1 > nbinsx) ||(hist->GetBinContent(binIter+1) <= threshold)) &&
	       ((binIter+2 > nbinsx) ||(hist->GetBinContent(binIter+2) <= threshold))) break;
	}
    }
  return binIter;
} 

////////////////////////////////////////////////////////////////////////////////

Int_t PTools::findBeginOfDistribution(TH1* hist, Int_t max, Double_t threshold, Bool_t compact)
{
  // returns bin number where distribution begins 
  // i.e where no data is beyond threshold
  // starting from right to left at bin number max
  // returns -1 if hist is NULL
  // if compact is set no bin smaller than max is accepted to be below threshold
  // otherwise wholes of 1 bins are allowed

  if (!hist)
    {
      ::Error("findBeginOfDistribution"," hist is NULL pointer ... return -1");
      return -1;
    }

  Int_t nbinsx = hist->GetNbinsX();

  Int_t binIter;
  if (max>nbinsx) { max = nbinsx; }

  for (binIter=max; binIter>0; binIter--)
    {
      if (hist->GetBinContent(binIter) > threshold) { continue;}
      if (compact)
	{
	  if ( ((binIter-1 <= 0) ||(hist->GetBinContent(binIter-1) <= threshold))) break;
	}
      else
	{
	  if ( ((binIter-1 <= 0) ||(hist->GetBinContent(binIter-1) <= threshold)) &&
	       ((binIter-2 <= 0) ||(hist->GetBinContent(binIter-2) <= threshold))) break;
	}

    }
  return binIter;
} 

////////////////////////////////////////////////////////////////////////////////

void PTools::scaleHistErrors(TH1* hist, Double_t scaleFactor)
{
  // scales Errors of hist by scaleFactor
  
  if (!hist)
    {
      ::Error("scaleHistErrors"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  if (hist->GetDimension()<2) nbinsy = -1;
  if (hist->GetDimension()<3) nbinsz = -1;

  Int_t binx,biny,binz,bin;

  
  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      hist->SetBinError(bin,hist->GetBinError(bin)*scaleFactor);
	    }
	}
    }
  return;
}

////////////////////////////////////////////////////////////////////////////////

TH1* PTools::copyBinSubsetOfHist(TH1* hist, 
					Int_t xbinmin, Int_t xbinmax, 
					Int_t ybinmin, Int_t ybinmax, 
					Int_t zbinmin, Int_t zbinmax  
					)
{
  // copies subsection of any THx histogramm hist to THxD histograms
  // you have to speciy via xbinmin [default:-1], xbinmax [default:-1], 
  //                        ybinmin [default:-1], ybinmax [default:-1], 
  //                        zbinmin [default:-1], zbinmax [default:-1]
  // the bin range you want to select
  // (NOTE: To choose the coordinates you have to use ((TAxis*)hist->GetXaxis())->FindBin(value); )
  // (NOTE: you have to cast to the corresponding type of histogram)
  //
  // attaches to name "_subset_X_xminbin-xmaxbin"
  //                  "_subset_X_xminbin-xmaxbin_Y_yminbin-ymaxbin"
  //                  "_subset_X_xminbin-xmaxbin_Y_yminbin-ymaxbin_Z_zminbin-zmaxbin
  //
  // if hist is NULL, NULL is returned
  // in case dimension doesn't match with bin minima and maxima dimensions
  // the choice is ignored.
  // if xbinmin, xbinmax, ybinmin, yzbinmax, zbinmin, yxbinmax are negativ the full range is taken
  // if minima > maxima NULL is returned
  //
  // The entries are copied from the original histogram, because they cannot calculated 
  
  // check for valid histogram pointer
  if (! hist)
    {
      ::Error("copyBinSubsetOfHist","histogram has NULL pointer ... returning NULL");
      return NULL;
    }

  // check for valid limit order / excluding negativ values for full range
  if ( xbinmin > xbinmax && xbinmax > 0) 
    {
      ::Error("copyBinSubsetOfHist","invalid choice of bin ranges xmin (%i) > xmax (%i) ... returning NULL",
	      xbinmin, xbinmax);
      return NULL;
    }
  if ( ybinmin > ybinmax && ybinmax > 0) 
    {
      ::Error("copyBinSubsetOfHist","invalid choice of bin ranges ymin (%i) > ymax (%i) ... returning NULL",
	      ybinmin, ybinmax);
      return NULL;
    }
  if ( zbinmin > zbinmax && zbinmax > 0) 
    {
      ::Error("copyBinSubsetOfHist","invalid choice of bin ranges zmin (%i) > zmax (%i) ... returning NULL",
	      zbinmin, zbinmax);
      return NULL;
    }
  
  // get number of bins
  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();

  // get dimension;
  Int_t dimension = hist->GetDimension();

  // check if choice exceeds maximum bin
 if (dimension >= 1)
    {
      if ( xbinmax > nbinsx )
	{
	  ::Error("copyBinSubsetOfHist","xbinmax: %i exceeds histograms bin range of %i ... returning NULL"
		  , xbinmax, nbinsx);
	  return NULL;
	}
    }

 if (dimension >= 2)
    {
      if ( ybinmax > nbinsy )
	{
	  ::Error("copyBinSubsetOfHist","ybinmax: %i exceeds histograms bin range of %i ... returning NULL"
		  , ybinmax, nbinsy);
	  return NULL;
	}
    }
  if (dimension >= 3)
    {
      if ( zbinmax > nbinsz )
	{
	  ::Error("copyBinSubsetOfHist","zbinmax: %i exceeds histograms bin range of %i ... returning NULL"
		  , zbinmax, nbinsz);
	  return NULL;
	}
    }


  // if ranges are negativ take maximum/minimum possible value;
  if (xbinmin < 0 ) xbinmin = 0;
  if (xbinmax < 0 ) xbinmax = nbinsx+1;
  if (ybinmin < 0 ) ybinmin = 0;
  if (ybinmax < 0 ) ybinmax = nbinsy+1;
  if (zbinmin < 0 ) zbinmin = 0;
  if (zbinmax < 0 ) zbinmax = nbinsz+1;

  if (dimension<2) {nbinsy = -1; ybinmin=0; ybinmax=0;}
  if (dimension<3) {nbinsz = -1; zbinmin=0; zbinmax=0;}

  // create new histogram
  TString name;
  TString title;
  TH1* histD = NULL;

  switch(dimension)
    {
    case 1:
      {
	name = Form("%s_subset_X_%i-%i",
		    hist->GetName(),
		    (xbinmin==0       )? 1     :xbinmin,
		    (xbinmax==nbinsx+1)? nbinsx:xbinmax);
	title = Form("%s ( subset X: bin %i - bin %i )",
		     hist->GetTitle(),
		    (xbinmin==0       )? 1     :xbinmin,
		    (xbinmax==nbinsx+1)? nbinsx:xbinmax);
	histD = new TH1D(name.Data(),title.Data(),
			 xbinmax-xbinmin+1,
			 hist->GetXaxis()->GetBinLowEdge(xbinmin), 
			 hist->GetXaxis()->GetBinUpEdge (xbinmax)
			 );
	break;
      }
    case 2:
      {
	name = Form("%s_subset_X_%i-%i_Y_%i-%i",
		    hist->GetName(),
		    (xbinmin==0       )? 1     :xbinmin,
		    (xbinmax==nbinsx+1)? nbinsx:xbinmax,
		    (ybinmin==0       )? 1     :ybinmin,
		    (ybinmax==nbinsy+1)? nbinsy:ybinmax);
	title = Form("%s ( subset X: bin %i - bin %i, Y: bin %i - bin %i )",
		     hist->GetTitle(),
		     (xbinmin==0       )? 1     :xbinmin,
		     (xbinmax==nbinsx+1)? nbinsx:xbinmax,
		     (ybinmin==0       )? 1     :ybinmin,
		     (ybinmax==nbinsy+1)? nbinsy:ybinmax);
	histD = new TH2D(name.Data(),title.Data(),
			 xbinmax-xbinmin+1,
			 hist->GetXaxis()->GetBinLowEdge(xbinmin), 
			 hist->GetXaxis()->GetBinUpEdge (xbinmax),
			 ybinmax-ybinmin+1,
			 hist->GetYaxis()->GetBinLowEdge(ybinmin), 
			 hist->GetYaxis()->GetBinUpEdge (ybinmax)
			 );
	break;
      }
    case 3:
      {
	name = Form("%s_subset_X_%i-%i_Y_%i-%i_Z_%i-%i",
		    hist->GetName(),
		    (xbinmin==0       )? 1     :xbinmin,
		    (xbinmax==nbinsx+1)? nbinsx:xbinmax,
		    (ybinmin==0       )? 1     :ybinmin,
		    (ybinmax==nbinsy+1)? nbinsy:ybinmax,
		    (zbinmin==0       )? 1     :zbinmin,
		    (zbinmax==nbinsz+1)? nbinsz:zbinmax);
	title = Form("%s ( subset X: bin %i - bin %i, Y: bin %i - bin %i, Z: bin %i - bin %i )",
		     hist->GetTitle(),
		    (xbinmin==0       )? 1     :xbinmin,
		    (xbinmax==nbinsx+1)? nbinsx:xbinmax,
		    (ybinmin==0       )? 1     :ybinmin,
		    (ybinmax==nbinsy+1)? nbinsy:ybinmax,
		    (zbinmin==0       )? 1     :zbinmin,
		    (zbinmax==nbinsz+1)? nbinsz:zbinmax);
	histD = new TH3D(name.Data(),title.Data(),
			 xbinmax-xbinmin+1,
			 hist->GetXaxis()->GetBinLowEdge(xbinmin), 
			 hist->GetXaxis()->GetBinUpEdge (xbinmax),
			 ybinmax-ybinmin+1,
			 hist->GetYaxis()->GetBinLowEdge(ybinmin), 
			 hist->GetYaxis()->GetBinUpEdge (ybinmax),
			 zbinmax-zbinmin+1,
			 hist->GetZaxis()->GetBinLowEdge(zbinmin), 
			 hist->GetZaxis()->GetBinUpEdge (zbinmax)
			 );
	break;
      }
    default:
      {
	::Error("copyBinSubsetOfHist","invalid dimension: %i ... returning NULL",dimension);
	return NULL;
	break;
      }
    }
  
  // copy axis attributes
  importAxisAttributes(hist->GetXaxis(), histD->GetXaxis(), kTRUE);
  importAxisAttributes(hist->GetYaxis(), histD->GetYaxis(), kTRUE);
  importAxisAttributes(hist->GetZaxis(), histD->GetZaxis(), kTRUE);

  // misc settings
  histD->SetEntries(hist->GetEntries());
  if (hist->GetSumw2N()) {hist->Sumw2();}

  Int_t binx,biny,binz,bin,newbin;
  
  for (binz=zbinmin;binz<=zbinmax;binz++)
    {
      for (biny=ybinmin;biny<=ybinmax;biny++)
	{
	  for (binx=xbinmin;binx<=xbinmax;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      newbin = histD->GetBin(
				     histD->GetXaxis()->FindBin( hist->GetXaxis()->GetBinCenter(binx)),
				     histD->GetYaxis()->FindBin( hist->GetYaxis()->GetBinCenter(biny)),
				     histD->GetZaxis()->FindBin( hist->GetZaxis()->GetBinCenter(binz))
				     );
	      histD->SetBinContent(newbin, hist->GetBinContent(bin));
	      if (hist->GetSumw2N())
		{
		  histD->SetBinError(newbin,hist->GetBinError(bin));
		}
	    }
	}
    }
  return histD;
}

////////////////////////////////////////////////////////////////////////////////
const Double_t PTools::copySubsetOfHistDefaultValue = -1.35791e-300;

TH1* PTools::copySubsetOfHist(TH1* hist,  
			      Double_t xmin, Double_t xmax, 
			      Double_t ymin, Double_t ymax, 
			      Double_t zmin, Double_t zmax  
			      )
{
  // copies subsection of any THx histogramm hist to THxD histograms
  // you have to speciy via xmin [default: PTools::copySubsetOfHistDefaultValue], 
  //                        xmax [default: PTools::copySubsetOfHistDefaultValue], 
  //                        ymin [default: PTools::copySubsetOfHistDefaultValue], 
  //                        ymax [default: PTools::copySubsetOfHistDefaultValue], 
  //                        zmin [default: PTools::copySubsetOfHistDefaultValue], 
  //                        zmax [default: PTools::copySubsetOfHistDefaultValue]
  // the range you want to select
  // via ((TAxis*)hist->GetXaxis())->FindBin(value); the corresponding bin range is calculated
  //
  // Following copyBinSubsetOfHist is called and afterwards the name and title of the histogram are just adopted
  // 
  //          to name "_subset_X_xmin-xmax"
  //                  "_subset_X_xmin-xmax_Y_ymin-ymax"
  //                  "_subset_X_xmin-xmax_Y_ymin-ymax_Z_zmin-zmax
  //
  // if hist is NULL, NULL is returned
  // in case dimension doesn't match with bin minima and maxima dimensions
  // the choice is ignored.
  // if xbinmin, xbinmax, ybinmin, yzbinmax, zbinmin, yxbinmax are PTools::copySubsetOfHistDefaultValue
  // the full range is taken
  // if minima > maxima NULL is returned
  //
  // The entries are copied from the original histogram, because they cannot calculated 
  //
  // (NOTE: you have to cast to the corresponding type of histogram)

  // check for valid histogram pointer
  if (! hist)
    {
      ::Error("copySubsetOfHist","histogram has NULL pointer ... returning NULL");
      return NULL;
    }
  
  Int_t dimension = hist->GetDimension();
  Int_t xbinmin=-1,xbinmax=-1,ybinmin=-1,ybinmax=-1,zbinmin=-1,zbinmax=-1;

  // get number of bins
  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();

  if (dimension > 0)
    {
      xbinmin = (xmin==PTools::copySubsetOfHistDefaultValue) ? -1 : hist->GetXaxis()->FindBin(xmin);
      // overflow/underflow bin check
      if (xbinmin<1 || xbinmin>nbinsx)
	{
	  ::Error("copySubsetOfHist","xmin: %f exceeds range of x-axis [%f,%f] ... returning NULL",
		  xmin,hist->GetXaxis()->GetBinLowEdge(1),hist->GetXaxis()->GetBinUpEdge(nbinsx));
	  return NULL;
	}
      xbinmax = (xmax==PTools::copySubsetOfHistDefaultValue) ? -1 : hist->GetXaxis()->FindBin(xmax);
      // overflow/underflow bin check
      if (xbinmax<1 || xbinmax>nbinsx)
	{
	  ::Error("copySubsetOfHist","xmax: %f exceeds range of x-axis [%f,%f] ... returning NULL",
		  xmax,hist->GetXaxis()->GetBinLowEdge(1),hist->GetXaxis()->GetBinUpEdge(nbinsx));
	  return NULL;
	}
      if (xbinmax!=-1) xbinmax--;
    }
  if (dimension > 1)
    {
      ybinmin = (ymin==PTools::copySubsetOfHistDefaultValue) ? -1 : hist->GetYaxis()->FindBin(ymin);
      // overflow/underflow bin check
      if (ybinmin<1 || ybinmin>nbinsy)
	{
	  ::Error("copySubsetOfHist","ymin: %f exceeds range of y-axis [%f,%f] ... returning NULL",
		  ymin,hist->GetYaxis()->GetBinLowEdge(1),hist->GetYaxis()->GetBinUpEdge(nbinsy));
	  return NULL;
	}
      ybinmax = (ymax==PTools::copySubsetOfHistDefaultValue) ? -1 : hist->GetYaxis()->FindBin(ymax);
      // overflow/underflow bin check
      if (ybinmax<1 || ybinmax>nbinsy)
	{
	  ::Error("copySubsetOfHist","ymax: %f exceeds range of y-axis [%f,%f] ... returning NULL",
		  ymax,hist->GetYaxis()->GetBinLowEdge(1),hist->GetYaxis()->GetBinUpEdge(nbinsy));
	  return NULL;
	}
      if (ybinmax!=-1) ybinmax--;
    }
  if (dimension > 2)
    {
      zbinmin = (zmin==PTools::copySubsetOfHistDefaultValue) ? -1 : hist->GetZaxis()->FindBin(zmin);
      // overflow/underflow bin check
      if (zbinmin<1 || zbinmin>nbinsz)
	{
	  ::Error("copySubsetOfHist","zmin: %f exceeds range of z-axis [%f,%f] ... returning NULL",
		  zmin,hist->GetZaxis()->GetBinLowEdge(1),hist->GetZaxis()->GetBinUpEdge(nbinsz));
	  return NULL;
	}
      zbinmax = (zmax==PTools::copySubsetOfHistDefaultValue) ? -1 : hist->GetZaxis()->FindBin(zmax);
      // overflow/underflow bin check
      if (zbinmax<1 || zbinmax>nbinsz)
	{
	  ::Error("copySubsetOfHist","zmax: %f exceeds range of z-axis [%f,%f] ... returning NULL",
		  zmax,hist->GetZaxis()->GetBinLowEdge(1),hist->GetZaxis()->GetBinUpEdge(nbinsz));
	  return NULL;
	}
      if (ybinmax!=-1) zbinmax--;
    }

  TH1* histD = copyBinSubsetOfHist(hist, xbinmin, xbinmax, ybinmin, ybinmax, zbinmin, zbinmax);  

  // check for valid pointer
  if (!histD) return NULL;

  // rename histogram

  // create new histogram
  TString name;
  TString title;

  switch(dimension)
    {
    case 1:
      {
	name = Form("%s_subset_X_%2.2g-%2.2g",
		    hist->GetName(),
		    (xbinmin==0       )? hist->GetXaxis()->GetBinLowEdge(1)     :xmin,
		    (xbinmax==nbinsx+1)? hist->GetXaxis()->GetBinLowEdge(nbinsx):xmax);
	title = Form("%s ( subset X:  %2.2g -  %2.2g )",
		     hist->GetTitle(),
		    (xbinmin==0       )? hist->GetXaxis()->GetBinLowEdge(1)     :xmin,
		    (xbinmax==nbinsx+1)? hist->GetXaxis()->GetBinLowEdge(nbinsx):xmax);
	break;
      }
    case 2:
      {
	name = Form("%s_subset_X_%2.2g-%2.2g_Y_%2.2g-%2.2g",
		    hist->GetName(),
		    (xbinmin==0       )? hist->GetXaxis()->GetBinLowEdge(1)     :xmin,
		    (xbinmax==nbinsx+1)? hist->GetXaxis()->GetBinLowEdge(nbinsx):xmax,
		    (ybinmin==0       )? hist->GetYaxis()->GetBinLowEdge(1)     :ymin,
		    (ybinmax==nbinsy+1)? hist->GetYaxis()->GetBinLowEdge(nbinsy):ymax);
	title = Form("%s ( subset X:  %2.2g -  %2.2g, Y:  %2.2g -  %2.2g )",
		     hist->GetTitle(),
		     (xbinmin==0       )? hist->GetXaxis()->GetBinLowEdge(1)     :xmin,
		     (xbinmax==nbinsx+1)? hist->GetXaxis()->GetBinLowEdge(nbinsx):xmax,
		     (ybinmin==0       )? hist->GetYaxis()->GetBinLowEdge(1)     :ymin,
		     (ybinmax==nbinsy+1)? hist->GetYaxis()->GetBinLowEdge(nbinsy):ymax);
	break;
      }
    case 3:
      {
	name = Form("%s_subset_X_%2.2g-%2.2g_Y_%2.2g-%2.2g_Z_%2.2g-%2.2g",
		    hist->GetName(),
		    (xbinmin==0       )? hist->GetXaxis()->GetBinLowEdge(1)     :xmin,
		    (xbinmax==nbinsx+1)? hist->GetXaxis()->GetBinLowEdge(nbinsx):xmax,
		    (ybinmin==0       )? hist->GetYaxis()->GetBinLowEdge(1)     :ymin,
		    (ybinmax==nbinsy+1)? hist->GetYaxis()->GetBinLowEdge(nbinsy):ymax,
		    (zbinmin==0       )? hist->GetZaxis()->GetBinLowEdge(1)     :zmin,
		    (zbinmax==nbinsz+1)? hist->GetZaxis()->GetBinLowEdge(nbinsz):zmax);
	title = Form("%s ( subset X: %2.2g - %2.2g, Y:  %2.2g -  %2.2g, Z:  %2.2g -  %2.2g )",
		     hist->GetTitle(),
		    (xbinmin==0       )? hist->GetXaxis()->GetBinLowEdge(1)     :xmin,
		    (xbinmax==nbinsx+1)? hist->GetXaxis()->GetBinLowEdge(nbinsx):xmax,
		    (ybinmin==0       )? hist->GetYaxis()->GetBinLowEdge(1)     :ymin,
		    (ybinmax==nbinsy+1)? hist->GetYaxis()->GetBinLowEdge(nbinsy):ymax,
		    (zbinmin==0       )? hist->GetZaxis()->GetBinLowEdge(1)     :zmin,
		    (zbinmax==nbinsz+1)? hist->GetZaxis()->GetBinLowEdge(nbinsz):zmax);
	break;
      }
    default:
      {
	::Error("copySubsetOfHist","invalid dimension: %i ... returning NULL",dimension);
	return NULL;
	break;
      }
    }

  histD->SetNameTitle(name.Data(),title.Data());

  return histD;
}
