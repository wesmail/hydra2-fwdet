using namespace std;
#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"
#include <iostream> 
#include <iomanip>
#include "TObjString.h"
#include "TList.h"

ClassImp(HOnlineMonHistAddon)
ClassImp(HOnlineMonHist)
ClassImp(HOnlineMonHist2)

HOnlineMonHistAddon::HOnlineMonHistAddon (const Char_t* name,
			      const Char_t* title,
			      Int_t active,Int_t resetable,
			      Int_t nbinsX,Float_t xlo,Float_t xup,
			      Int_t nbinsY,Float_t ylo,Float_t yup)
    : TNamed(name,title)
{
    arraySize1=0;
    arraySize2=0;
    directory="no";
    XTitle="no";
    YTitle="no";
    ZTitle="no";
    Option="no";
    HistType="";
    format="";
    
    nBinsX      =nbinsX;
    Xlo         =xlo;
    Xup         =xup;
    nBinsY      =nbinsY;
    Ylo         =ylo;
    Yup         =yup;
    fIsActive   =active;
    fisResetable=resetable;
    refreshrate =500;
    send        =1;
    stat        =0;
    max         =-99;
    min         =-99;
    linecol     =0;
    linestyle   =0;
    fillcol     =0;
    fillstyle   =0;
    markercol   =0;
    markerstyle =0;
    markersize  =0;

    logX        =0;
    logY        =0;
    logZ        =0;
    gridX       =0;
    gridY       =0;
    drawSame    = kFALSE;

    legend = 0;
    legxmin =-1;
    legymin =-1;
    legxmax =-1;
    legymax =-1;
    legform ="no";

}
HOnlineMonHistAddon::HOnlineMonHistAddon (Text_t* txt)
{
    arraySize1=0;
    arraySize2=0;
    directory="no";
    XTitle="no";
    YTitle="no";
    ZTitle="no";
    Option="no";
    HistType="";
    format="";
    
    nBinsX      =0;
    Xlo         =0;
    Xup         =0;
    nBinsY      =0;
    Ylo         =0;
    Yup         =0;
    fIsActive   =1;
    fisResetable=1;
    refreshrate =500;
    send        =1;
    stat        =0;
    max         =-99;
    min         =-99;
    linecol     =0;
    linestyle   =0;
    fillcol     =0;
    fillstyle   =0;
    markercol   =0;
    markerstyle =0;
    markersize  =0;

    logX        =0;
    logY        =0;
    logZ        =0;
    gridX       =0;
    gridY       =0;
    drawSame    = kFALSE;

    legend = 0;
    legxmin =-1;
    legymin =-1;
    legxmax =-1;
    legymax =-1;
    legform ="no";
    read(txt);
    send=1;
}
void HOnlineMonHistAddon::copySet(HOnlineMonHistAddon& add)
{
    SetName(add.GetName());
    SetTitle(add.GetTitle());

    arraySize1  =add.getSize1();
    arraySize2  =add.getSize2();
    directory   =add.getDirectory();
    XTitle      =add.getXTitle();
    YTitle      =add.getYTitle();
    ZTitle      =add.getZTitle();
    Option      =add.getOption();
    HistType    =add.getHistType();

    nBinsX      =add.getNBinsX();
    Xlo         =add.getXLo();
    Xup         =add.getXUp();
    nBinsY      =add.getNBinsY();
    Ylo         =add.getYLo();
    Yup         =add.getYUp();
    fIsActive   =add.getActive();
    fisResetable=add.getResetable();
    refreshrate =add.getRefreshRate();
    send        =add.getSend();
    stat        =add.getStat();
    max         =add.getMax();
    min         =add.getMin();

    linecol     =add.getLineCol();
    linestyle   =add.getLineStyle();
    fillcol     =add.getFillCol();
    fillstyle   =add.getFillStyle();
    markercol   =add.getMarkerCol();
    markerstyle =add.getMarkerStyle();
    markersize  =add.getMarkerSize();

    logX        =add.getLogX();
    logY        =add.getLogY();
    logZ        =add.getLogZ();
    gridX       =add.getGridX();
    gridY       =add.getGridY();
    drawSame    =add.getDrawSame();

    add.getLegDim(legxmin,legymin,legxmax,legymax,legform);

}
void HOnlineMonHistAddon::getSet(HOnlineMonHistAddon& add)
{
    add.setSize1(arraySize1);
    add.setSize2(arraySize2);
    add.setDirectory(directory);
    add.setXTitle(XTitle);
    add.setYTitle(YTitle);
    add.setZTitle(ZTitle);
    add.setOption(Option);
    add.setHistType(HistType);

    add.setNBinsX(nBinsX);
    add.setXLo(Xlo );
    add.setXUp(Xup);
    add.setNBinsY(nBinsY);
    add.setYLo(Ylo);
    add.setYUp(Yup);
    add.setActive(fIsActive);
    add.setResetable(fisResetable);
    add.setRefreshRate(refreshrate);
    add.setSend(send);
    add.setStat(stat);
    add.setMax(max);
    add.setMin(min);

    add.setLineCol(linecol);
    add.setLineStyle(linestyle);
    add.setFillCol(fillcol);
    add.setFillStyle(fillstyle);
    add.setMarkerCol(markercol);
    add.setMarkerStyle(markerstyle);
    add.setMarkerSize(markersize);

    add.setLogX(logX);
    add.setLogY(logY);
    add.setLogZ(logZ);
    add.setGridX(gridX);
    add.setGridY(gridY);
    add.setDrawSame(drawSame);

    add.setLegDim(legxmin,legymin,legxmax,legymax,legform);

}
void HOnlineMonHistAddon::read(const Text_t* txt)
{

    Char_t array [100];
    Char_t array1[100];
    Char_t array2[100];

    TString s1=txt;
    Ssiz_t len=s1.Length();
    if(len!=0)
    {
	TObjArray* myarguments = s1.Tokenize(" ");
	TObjString *stemp;
	TString argument;

	TIterator* myiter=myarguments->MakeIterator();

	// iterate over the lis of arguments and compare the
	// to known key words.
	while ((stemp=(TObjString*)myiter->Next())!= 0)
	{
	    argument=stemp->GetString();
	    if(argument.Contains("FORMAT#")==1)
	    {
		argument.ReplaceAll("FORMAT#","");
		format=argument;
	    }
	    else if(argument.Contains("TYPE#")==1)
	    {
		argument.ReplaceAll("TYPE#","");
		HistType=argument;
	    }
	    else if(argument.Contains("NAME#")==1)
	    {
		argument.ReplaceAll("NAME#","");
		this->SetName(argument);
	    }
	    else if(argument.Contains("TITLE#")==1)
	    {
		argument.ReplaceAll("TITLE#","");
		this->SetTitle(argument);
	    }
	    else if(argument.Contains("ACTIVE#")==1)
	    {
		argument.ReplaceAll("ACTIVE#","");
		sscanf(argument.Data(),"%i",&fIsActive);
	    }
	    else if(argument.Contains("RESET#")==1)
	    {
		argument.ReplaceAll("RESET#","");
		sscanf(argument.Data(),"%i",&fisResetable);
	    }
	    else if(argument.Contains("REFRESH#")==1)
	    {
		argument.ReplaceAll("REFRESH#","");
		sscanf(argument.Data(),"%i",&refreshrate);
	    }
	    else if(argument.Contains("BIN#")==1)
	    {
		argument.ReplaceAll("BIN#","");
		argument.ReplaceAll(":"," ");
		sscanf(argument.Data(),"%i%f%f%i%f%f",&nBinsX,&Xlo,&Xup,&nBinsY,&Ylo,&Yup);
	    }
	    else if(argument.Contains("SIZE#")==1)
	    {
		argument.ReplaceAll("SIZE#","");
		argument.ReplaceAll(":"," ");
		sscanf(argument.Data(),"%i%i",&arraySize1,&arraySize2);
	    }
	    else if(argument.Contains("AXIS#")==1)
	    {
		argument.ReplaceAll("AXIS#","");
		argument.ReplaceAll(":"," ");

		sscanf(argument.Data(),"%s%s%s",array,array1,array2);
		XTitle=array;
		XTitle.ReplaceAll("_"," ");
		YTitle=array1;
		YTitle.ReplaceAll("_"," ");
		ZTitle=array2;
		ZTitle.ReplaceAll("_"," ");
	    }
	    else if(argument.Contains("DIR#")==1)
	    {
		argument.ReplaceAll("DIR#","");
		directory=argument;
	    }
	    else if(argument.Contains("OPT#")==1)
	    {
		argument.ReplaceAll("OPT#","");
		Option=argument;
	    }
	    else if(argument.Contains("STATS#")==1)
	    {
		argument.ReplaceAll("STATS#","");
		sscanf(argument.Data(),"%i",&stat);
	    }
	    else if(argument.Contains("LOG#")==1)
	    {
		argument.ReplaceAll("LOG#","");
		argument.ReplaceAll(":"," ");
		sscanf(argument.Data(),"%i%i%i",&logX,&logY,&logZ);
	    }
	    else if(argument.Contains("GRID#")==1)
	    {
		argument.ReplaceAll("GRID#","");
		argument.ReplaceAll(":"," ");
		sscanf(argument.Data(),"%i%i",&gridX,&gridY);
	    }
	    else if(argument.Contains("LINE#")==1)
	    {
		argument.ReplaceAll("LINE#","");
		argument.ReplaceAll(":"," ");
		sscanf(argument.Data(),"%i%i",&linecol,&linestyle);
	    }
	    else if(argument.Contains("FILL#")==1)
	    {
		argument.ReplaceAll("FILL#","");
		argument.ReplaceAll(":"," ");
		sscanf(argument.Data(),"%i%i",&fillcol,&fillstyle);
	    }
	    else if(argument.Contains("MARKER#")==1)
	    {
		argument.ReplaceAll("MARKER#","");
		argument.ReplaceAll(":"," ");
		sscanf(argument.Data(),"%i%i%f",&markercol,&markerstyle,&markersize);
	    }
	    else if(argument.Contains("RANGE#")==1)
	    {
		argument.ReplaceAll("RANGE#","");
		argument.ReplaceAll(":"," ");
		sscanf(argument.Data(),"%i%i",&min,&max);
	    }
	    else if(argument.Contains("LEGEND#")==1)
	    {
		argument.ReplaceAll("LEGEND#","");
		argument.ReplaceAll(":"," ");
                Char_t format[100];
		sscanf(argument.Data(),"%f%f%f%f%s",&legxmin,&legymin,&legxmax,&legymax,format);
                legform = format;
	    }
	    else
	    {
		Error("HOnlineMonHistAddon:read()","unknown argument %s in addon %s!",argument.Data(),GetName());
	    }
	}
	delete myiter;

	myarguments->Delete();
        delete myarguments;
    }
}
void HOnlineMonHistAddon::getText(Text_t* txt)
{
    Char_t temp[20];
    TString s2;
    TString s="FORMAT#";
           s+=format;
	   s+=" TYPE#";
	   s+=HistType;
	   s+=" NAME#";
	   s+=GetName();
	   s+=" TITLE#";
	   s+=GetTitle();
	   s+=" ACTIVE#";
	   s+=fIsActive;
	   s+=" RESET#";
	   s+=fisResetable;
	   s+=" REFRESH#";
	   s+=refreshrate;
	   s+=" BIN#";
	   s+=nBinsX;
	   s+=":";
	   sprintf(temp,"%f",Xlo);
	   s+=temp;
	   s+=":";
	   sprintf(temp,"%f",Xup);
	   s+=temp;
	   s+=":";
	   s+=nBinsY;
	   s+=":";
	   sprintf(temp,"%f",Ylo);
	   s+=temp;
	   s+=":";
	   sprintf(temp,"%f",Yup);
	   s+=temp;
	   s+=" SIZE#";
	   s+=arraySize1;
	   s+=":";
	   s+=arraySize2;
	   s+=" AXIS#";
	   s2=XTitle;
	   s2.ReplaceAll(" ","_");
	   s+=s2;
	   s+=":";
	   s2=YTitle;
	   s2.ReplaceAll(" ","_");
	   s+=s2;
	   s+=":";
	   s2=ZTitle;
	   s2.ReplaceAll(" ","_");
	   s+=s2;
	   s+=" DIR#";
	   s+=directory;
	   s+=" OPT#";
	   s+=Option;
	   s+=" STATS#";
	   s+=stat;
           s+=" LOG#";
	   s+=logX;
	   s+=":";
	   s+=logY;
	   s+=":";
	   s+=logZ;
           s+=" GRID#";
	   s+=gridX;
	   s+=":";
	   s+=gridY;
           s+=" LINE#";
	   s+=linecol;
	   s+=":";
	   s+=linestyle;
           s+=" FILL#";
	   s+=fillcol;
	   s+=":";
	   s+=fillstyle;
           s+=" MARKER#";
	   s+=markercol;
	   s+=":";
	   s+=markerstyle;
	   s+=":";
	   sprintf(temp,"%f",markersize);
           s+=temp;
	   s+=" RANGE#";
	   s+=max;
	   s+=":";
	   s+=min;
	   TString leg = Form("LEGEND#%f:%f:%f:%f:%s",legxmin,legymin,legxmax,legymax,legform.Data());
           s+=leg;


 	   sprintf(txt,"%s",s.Data());
}
void HOnlineMonHistAddon::setHist(TH1* p)
{
	if(XTitle.CompareTo("no")!=0)p->SetXTitle(XTitle);
        if(YTitle.CompareTo("no")!=0)p->SetYTitle(YTitle);
        if(Option.CompareTo("no")!=0)p->SetOption(Option);
        p->SetStats(stat);
	if(linecol!=0)p->SetLineColor(linecol);
        if(linestyle!=0)p->SetLineStyle(linestyle);
        if(fillcol!=0)p->SetFillColor(fillcol);
        if(fillstyle!=0)p->SetFillStyle(fillstyle);
        if(markercol!=0)p->SetMarkerColor(markercol);
        if(markerstyle!=0)p->SetMarkerStyle(markerstyle);
        if(markersize!=0)p->SetMarkerSize(markersize);
        if(max!=-99)p->SetMaximum(max);
        if(min!=-99)p->SetMinimum(min);
};
void HOnlineMonHistAddon::setHist(TH2* p)
{
	if(XTitle.CompareTo("no")!=0)p->SetXTitle(XTitle);
        if(YTitle.CompareTo("no")!=0)p->SetYTitle(YTitle);
        if(ZTitle.CompareTo("no")!=0)p->SetZTitle(ZTitle);
	if(Option.CompareTo("no")!=0)p->SetOption(Option);
        p->SetStats(stat);
        if(max!=-99)p->SetMaximum(max);
        if(min!=-99)p->SetMinimum(min);
};
void HOnlineMonHistAddon::setCanvas(TCanvas* c)
{
    c->SetGrid(gridX,gridY);
    c->SetLogx(logX);
    c->SetLogy(logY);
    c->SetLogz(logZ);
}

HOnlineMonHistAddon* HOnlineMonHistAddon::createHist()
{
    TString format = getFormat();
    TString type   = getHistType();

    HOnlineMonHistAddon* addon = 0 ;
    if     (format == "array") {
	if(type.Contains("1") ) { addon = new HOnlineHistArray (*this); }
	else                    { addon = new HOnlineHistArray2(*this); }
    }
    else if(format == "trend")      { addon = new HOnlineTrendHist (*this); }
    else if(format == "trendarray") { addon = new HOnlineTrendArray(*this); }
    else if(format == "mon")    {
	if(type.Contains("1") ) {  addon = new HOnlineMonHist (*this); }
	else                    {  addon = new HOnlineMonHist2(*this); }
    }
    else   {
	Error("createHist()","Unknown format of histogram: %s for hist %s !",format.Data(),addon->GetName());
    }
    return addon;
}



void HOnlineMonHistAddon::draw(Bool_t allowHiddenZero)  {
    Error("HOnlineMonHistAddon::draw()" ,"virtual function called!");
    cerr << GetName() << " *** " << GetTitle() << endl;
}
void HOnlineMonHistAddon::reset(Int_t level,Int_t count){
    Error("HOnlineMonHistAddon::reset()","virtual function called!");
    cerr << GetName() << " *** " << GetTitle() << endl;
}
void HOnlineMonHistAddon::add(HOnlineMonHistAddon*)           {
    Error("HOnlineMonHistAddon::add()"  ,"virtual function called!");
    cerr << GetName() << " *** " << GetTitle() << endl;
}
void HOnlineMonHistAddon::fill(Stat_t  i,Stat_t j){
    Error("HOnlineMonHistAddon::fill(Stat_t  i,Stat_t j)"  ,"virtual function called!");
    cerr << GetName() << " *** " << GetTitle() << endl;
}
void HOnlineMonHistAddon::fill(Int_t,Int_t,Stat_t,Stat_t){
    Error("HOnlineMonHistAddon::fill(Int_t,Int_t,Stat_t,Stat_t)"  ,"virtual function called!");
    cerr << GetName() << " *** " << GetTitle() << endl;
}
TH1* HOnlineMonHistAddon::getP(Int_t s,Int_t m)         {
    Error("HOnlineMonHistAddon::getP()" ,"virtual function called!");
    cerr << GetName() << " *** " << GetTitle() << endl;
    return(0);
}
HOnlineMonHistAddon::~HOnlineMonHistAddon(){}


//###############################################################################################



HOnlineMonHist::HOnlineMonHist(const Char_t* name,
		   const Char_t* title,
		   Int_t active,Int_t resetable,
		   Int_t nbinsX,Float_t xlo,Float_t xup)
:  HOnlineMonHistAddon(name,title,active,resetable,
		 nbinsX,xlo,xup,0,0,0)
{
   h=0;
}
HOnlineMonHist::HOnlineMonHist(HOnlineMonHistAddon& add)
{
    copySet(add);
    h=0;
    create();
}
void HOnlineMonHist::reset(Int_t level,Int_t count)
{
    switch (level)
    {
    case 0:   h->Reset();
    break;
    case 1:   if(getResetable()==1)h->Reset();
    break;
    case 2:   if(getResetable()==1 && count%getRefreshRate()==0 && count>0){h->Reset();}
    break;
    default : Error("HOnlineMonHist::reset","UNKNOWN ARGUMENT: Level= %i !",level);
    }
}
void HOnlineMonHist::add(HOnlineMonHistAddon* h2){h->Add(((HOnlineMonHist*)h2)->getP());}

void HOnlineMonHist::draw(Bool_t allowHiddenZero)
{
    TString opt=h->GetOption();
    if(!drawSame){
	if(opt.CompareTo("")==0)       h->Draw();
	else if(opt.CompareTo("p")==0) h->Draw("p");
	else if(opt.CompareTo("pl")==0)h->Draw("pl");
	h->Draw();
    }  else {
	if(opt.CompareTo("")==0)       h->Draw("same");
	else if(opt.CompareTo("p")==0) h->Draw("psame");
	else if(opt.CompareTo("pl")==0)h->Draw("plsame");
	h->Draw();
    }
}

void HOnlineMonHist::create()
{
    if(fIsActive==1)
    {
	if(HistType.CompareTo("1S")==0)h=new TH1S(this->GetName(),this->GetTitle(),nBinsX,Xlo,Xup);
	if(HistType.CompareTo("1F")==0)h=new TH1F(this->GetName(),this->GetTitle(),nBinsX,Xlo,Xup);
	if(HistType.CompareTo("1D")==0)h=new TH1D(this->GetName(),this->GetTitle(),nBinsX,Xlo,Xup);
        h->SetDirectory(0);
	setHist(h);
    }
}

void  HOnlineMonHist::getMinMax(Double_t& min,Double_t& max)
{
    min =  1e200;
    max = -1e200;
    for(Int_t i = 0; i < h->GetNbinsX(); i++ ) {
	Double_t val = h->GetBinContent(i+1);
	if(val < min) min = val;
	if(val > max) max = val;

    }
}


HOnlineMonHist::~HOnlineMonHist(){if(h)delete h;h=0;}

//###############################################################################################

HOnlineMonHist2::HOnlineMonHist2(const Char_t* name,
			   const Char_t* title,
			   Int_t active,Int_t resetable,
			   Int_t nbinsX,Float_t xlo,Float_t xup,
			   Int_t nbinsY,Float_t ylo,Float_t yup)
:  HOnlineMonHistAddon(name,title,active,resetable,
		       nbinsX,xlo,xup,
		       nbinsY,ylo,yup)
{
  h=0;
}

HOnlineMonHist2::HOnlineMonHist2(HOnlineMonHistAddon& add)
{
    copySet(add);
    h=0;
    create();
}

void HOnlineMonHist2::reset(Int_t level,Int_t count)
{
    switch (level)
    {
    case 0:   h->Reset();
    break;
    case 1:   if(getResetable()==1)h->Reset();
    break;
    case 2:   if(getResetable()==1 && count%getRefreshRate()==0 && count>0){h->Reset();}
    break;
    default : Error("HOnlineMonHist2::reset","UNKNOWN ARGUMENT: Level= %i !",level);
    }
}

void HOnlineMonHist2::add(HOnlineMonHistAddon* h2){h->Add(((HOnlineMonHist2*)h2)->getP());}

void HOnlineMonHist2::draw(Bool_t allowHiddenZero)
{
    if(!drawSame)h->Draw();
    else         h->Draw("same");
}

void HOnlineMonHist2::create()
{
    if(fIsActive==1)
    {
	if(HistType.CompareTo("2S")==0)h=new TH2S(this->GetName(),this->GetTitle(),nBinsX,Xlo,Xup,nBinsY,Ylo,Yup);
	if(HistType.CompareTo("2F")==0)h=new TH2F(this->GetName(),this->GetTitle(),nBinsX,Xlo,Xup,nBinsY,Ylo,Yup);
	if(HistType.CompareTo("2D")==0)h=new TH2D(this->GetName(),this->GetTitle(),nBinsX,Xlo,Xup,nBinsY,Ylo,Yup);
        h->SetDirectory(0);
	setHist(h);
    };
}

void  HOnlineMonHist2::getMinMax(Double_t& min,Double_t& max)
{
    min =  1e200;
    max = -1e200;
    for(Int_t i = 0; i < h->GetNbinsX(); i++ ) {
	for(Int_t j = 0; j < h->GetNbinsY(); j++ ) {
	    Double_t val = h->GetBinContent(i+1,j+1);
	    if(val < min) min = val;
	    if(val > max) max = val;

	}
    }
}

HOnlineMonHist2::~HOnlineMonHist2(){if(h)delete h;h=0;}
