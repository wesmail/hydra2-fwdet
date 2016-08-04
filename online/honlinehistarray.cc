#include "honlinehistarray.h"
#include "TMath.h"
#include "TLine.h"
#include "TStyle.h"
#include "TPostScript.h"
#include "TROOT.h"
ClassImp(HOnlineNamedLegend)
ClassImp(HOnlineLegendDrawer)
ClassImp(HOnlineTrendArray)
ClassImp(HOnlineHistArray)
ClassImp(HOnlineHistArray2)

HOnlineNamedLegend::HOnlineNamedLegend (const Char_t* name,
			    const Char_t* title,HOnlineMonHistAddon* h)
    :TNamed(name,title)
{
    if(h)
    {
	Int_t s1,s2;
	TString temp;
      
	legend=new TLegend(0.8,0.8,0.89,0.89);

	legend->SetFillStyle(0);
        legend->SetFillColor(19);
        legend->SetLineColor(1);
        legend->SetLineStyle(1);
        legend->SetDrawOption("lego");

	if(strcmp(h->ClassName(),"HOnlineHistArray")==0)
	{
            ((HOnlineHistArray*)h)->getDim(&s1,&s2);
	    for(Int_t i=0;i<s1;i++){
		for(Int_t j=0;j<s2;j++){
		    if(s1>1){sprintf((Char_t*)temp.Data()," %i %i",i,j);}
		    else {sprintf((Char_t*)temp.Data()," %i",j);}
                    TH1* myhist=((TH1*)(((HOnlineHistArray*)h)->getP(i,j)));
		    legend->AddEntry(myhist,temp.Data(),"l");
		}
	    }

	}
	else if(strcmp(h->ClassName(),"HOnlineTrendArray")==0)
	{
	    ((HOnlineTrendArray*)h)->getDim(&s1,&s2);
	    for(Int_t i=0;i<s1;i++){
		for(Int_t j=0;j<s2;j++){
		    if(s1>1){sprintf((Char_t*)temp.Data(),"%i %i",i,j);}
		    else {sprintf((Char_t*)temp.Data(),"%i",j);}
		    TH1* myhist=((TH1*)(((HOnlineTrendArray*)h)->getP(i,j)));
		    legend->AddEntry(myhist,temp.Data(),"l");
		}
	    }
	}
    }
}
HOnlineNamedLegend::~HOnlineNamedLegend()
{
    if(legend)delete legend;
}

//###########################################################

HOnlineLegendDrawer::HOnlineLegendDrawer(const Char_t* name,
			     const Char_t* title)
    :TNamed(name,title)
{
   array=new TObjArray();
}
HOnlineLegendDrawer::~HOnlineLegendDrawer()
{
    if(array)array->Delete();
}
void HOnlineLegendDrawer::draw(HOnlineMonHistAddon* h)
{
    if(strcmp(h->ClassName(),"HOnlineHistArray")==0 || strcmp(h->ClassName(),"HOnlineTrendArray")==0 )
    {
        TString strname;
	strname="Leg";
	strname+=h->GetName();
	HOnlineNamedLegend* myleg=((HOnlineNamedLegend*)(array->FindObject(strname.Data())));
	if(myleg)
	{
	    ((TLegend*) (((HOnlineNamedLegend*)myleg)->getP()))->Draw("same");
	}
	else
	{
 	    myleg =add(h);
	    ((TLegend*) (((HOnlineNamedLegend*)myleg)->getP()))->Draw("same");
	}
    }
}
HOnlineNamedLegend* HOnlineLegendDrawer::add (HOnlineMonHistAddon* h)
{
    TString strname;
    strname="Leg";
    strname+=h->GetName();

    HOnlineNamedLegend* myleg=new HOnlineNamedLegend(strname.Data(),h->GetTitle(),h);
    array->AddLast(myleg);
    return (HOnlineNamedLegend*)myleg;
}

//###################################################################################


HOnlineTrendArray::HOnlineTrendArray(const Char_t* name,
			 const Char_t* title,
			 Int_t active,Int_t resetable,Int_t size1,Int_t size2,
			 Int_t nbinsX,Float_t xlo,Float_t xup)
    :HOnlineMonHistAddon (name,title,active,resetable,nbinsX,xlo,xup,0,0.F,0.F)
{
    setSize1(size1);
    setSize2(size2);
    array=0;
}
HOnlineTrendArray::HOnlineTrendArray(HOnlineMonHistAddon& add)
{
    copySet(add);
    array=0;
    create();
}
void    HOnlineTrendArray::create()
{
    if(getActive()==1)
    {
	Bool_t doLegend = kFALSE;
	Bool_t useFormat= kFALSE;
        Int_t  dim      = 0 ;
	if(legxmax != -1){
             doLegend = kTRUE;
             legend = new TLegend(legxmin,legymin,legxmax,legymax,"","brNDC");
	     if(legform != "no" && legform.Contains("%i")){

		Int_t ntimes = legform.CountChar('%');
		if(ntimes == 1 && arraySize1 == 1)                   { useFormat = kTRUE; dim = 1;}
	        if(ntimes == 2 && arraySize1 > 1 && arraySize2 > 1)  { useFormat = kTRUE; dim = 2;}
	     }
	}

	array = new TObjArray((arraySize1*arraySize2)+1);
	Char_t namehist[400];
	for(Int_t i=0;i<arraySize1;i++){
	    for(Int_t j=0;j<arraySize2;j++){
		sprintf(namehist,"%s%s%i%s%i%s",this->GetName(),"_[",i,"][",j,"]");
		if(HistType.CompareTo("1S")==0)array->AddAt(new TH1S(namehist,this->GetTitle(),nBinsX,Xlo,Xup),i*arraySize2+j);
                if(HistType.CompareTo("1F")==0)array->AddAt(new TH1F(namehist,this->GetTitle(),nBinsX,Xlo,Xup),i*arraySize2+j);
                if(HistType.CompareTo("1D")==0)array->AddAt(new TH1D(namehist,this->GetTitle(),nBinsX,Xlo,Xup),i*arraySize2+j);
                ((TH1*)((*array)[i*arraySize2+j]))->SetDirectory(0);
		setHist((TH1*)((*array)[i*arraySize2+j]));
		if(doLegend)
		{
		    if(!useFormat) legend ->AddEntry(((TH1*)((*array)[i*arraySize2+j])),Form("%s",((TH1*)((*array)[i*arraySize2+j]))->GetName()),"lpf");
		    else {
			if(dim==1) legend->AddEntry(((TH1*)((*array)[i*arraySize2+j])),Form(legform.Data(),j  ),"lpf");
			if(dim==2) legend->AddEntry(((TH1*)((*array)[i*arraySize2+j])),Form(legform.Data(),i,j),"lpf");
		    }
		}
	    }
	}
    	sprintf(namehist,"%s%s",this->GetName(),"_dummy");
	array->AddAt(new TH1S(namehist,this->GetTitle(),2,Xlo,Xup),(arraySize1*arraySize2));
	((TH1*)((*array)[arraySize1*arraySize2]))->SetDirectory(0);
	setHist((TH1*)((*array)[arraySize1*arraySize2]));

        

    }
}
void    HOnlineTrendArray::drawAll( Bool_t allowHiddenZero,Double_t minSet,Double_t maxSet)
{
    if(getActive()==1)
    {
	Bool_t useSet = kFALSE;
        if(minSet > -1e199 && maxSet < 1e199 ) useSet = kTRUE;

	Float_t* maxvalues=new Float_t [(arraySize1*arraySize2)];
	Float_t* minvalues=new Float_t [(arraySize1*arraySize2)];
        TH1* htemp;
	for(Int_t i=0;i<(arraySize1*arraySize2);i++){
            htemp=((TH1*)((*array)[i]));
	    maxvalues[i]=htemp->GetBinContent(htemp->GetMaximumBin());
	    minvalues[i]=htemp->GetBinContent(htemp->GetMinimumBin());
	}

	Int_t   loc =TMath::LocMax((arraySize1*arraySize2),maxvalues);
	Int_t   loc1=TMath::LocMin((arraySize1*arraySize2),minvalues);

        Float_t MAX,MIN;
	if(max==-99){
	     MAX=maxvalues[loc]*1.1;
	} else MAX=max;

	if(min==-99){
	    if(logY==0)MIN=minvalues[loc1];
	    else MIN=maxvalues[loc]*0.001;
	} else MIN=min;

	htemp=((TH1*)((*array)[arraySize1*arraySize2]));

	TString opt=htemp->GetOption();

	if(!drawSame){
	    if(allowHiddenZero!=0 || logY!=0 ) { htemp->SetMinimum(MIN);   }
	    else                               { htemp->SetMinimum(-1111); }

	    if(!useSet) htemp->SetMaximum(MAX);
	    else        {
		htemp->SetMinimum(1.1*minSet);
		htemp->SetMaximum(1.1*maxSet);
	    }
	    
	    if(opt.CompareTo("")==0)htemp->Draw();
	    else if(opt.CompareTo("p")==0)htemp->Draw("p");
	    else if(opt.CompareTo("pl")==0)htemp->Draw("pl");
	}

	for(Int_t i=0;i<(arraySize1*arraySize2);i++){
 	    htemp=((TH1*)((*array)[i]));
	    if(opt.CompareTo("")==0)htemp->Draw("same");
	    else if(opt.CompareTo("p")==0) htemp->Draw("psame");
	    else if(opt.CompareTo("pl")==0)htemp->Draw("plsame");
	}
	delete [] maxvalues;
	delete [] minvalues;

        if(legend && !drawSame) legend->Draw();

    }
}
void    HOnlineTrendArray::fill(Int_t s,Int_t m,Stat_t a, Stat_t b)
{
    if(array)
    {
	TH1* h=((TH1F*) (*array)[s*arraySize2+m]);
	for(Int_t i=0;i<getNBinsX();i++){h->SetBinContent(i,h->GetBinContent(i+1) );}
	h->SetBinContent(h->GetNbinsX(),a);
	//h->SetBinError(h->GetNbinsX(),b);
    }else Error("HOnlineTrendArray::fill()","ARRAY DOES NOT EXISI!");
}
void    HOnlineTrendArray::reset(Int_t level,Int_t count)
{
    switch (level)
    {
    case 0:   if(array){for(Int_t i=0;i<arraySize1*arraySize2;i++){((TH1*)(*array)[i])->Reset();}}
    break;
    case 1:   if(array&&getResetable()==1){for(Int_t i=0;i<arraySize1*arraySize2;i++){((TH1*)(*array)[i])->Reset();}}
    break;
    case 2:   if(array&&getResetable()==1 && count%getRefreshRate()==0 && count>0){for(Int_t i=0;i<arraySize1*arraySize2;i++){((TH1*)(*array)[i])->Reset();}}
    break;
    default : Error("HOnlineTrendArray::reset","UNKNOWN ARGUMENT: Level= %i !",level);
    }
}
void    HOnlineTrendArray::add(HOnlineMonHistAddon* h)
{
    if(array)
    {
	for(Int_t i=0;i<arraySize1;i++){
	    for(Int_t j=0;j<arraySize2;j++){
		((TH1*)((*array)[i*arraySize2+j]))->Add((TH1*)(((HOnlineHistArray*)h)->getP(i,j)));
	    }
	}
    }
    else Error("HOnlineTrendArray::add()","ARRAY DOES NOT EXISI!");
}
void    HOnlineTrendArray::draw( Bool_t allowHiddenZero)
{
   drawAll(allowHiddenZero);
}

void  HOnlineTrendArray::getMinMax(Double_t& min,Double_t& max)
{
    min =  1e200;
    max = -1e200;
    for(Int_t n =0 ; n < array->GetSize(); n++){
	TH1* h = (TH1*)array->At(n);
	if(!h) continue;
	for(Int_t i = 0; i < h->GetNbinsX(); i++ ) {
	    Double_t val = h->GetBinContent(i+1);
	    if(val < min) min = val;
	    if(val > max) max = val;

	}
    }
}

HOnlineTrendArray::~HOnlineTrendArray(){if(array)array->Delete();array=0; if(legend) delete legend;}


//#################################################################################################

HOnlineHistArray::HOnlineHistArray(const Char_t* name,
		       const Char_t* title,
		       Int_t active,Int_t resetable,Int_t size1,Int_t size2,
		       Int_t nbinsX,Float_t xlo,Float_t xup)
:HOnlineMonHistAddon (name,title,active,resetable,nbinsX,xlo,xup,0,0.F,0.F)
{
    setSize1(size1);
    setSize2(size2);
    array=0;
}
HOnlineHistArray::HOnlineHistArray(HOnlineMonHistAddon& add)
{
    copySet(add);
    array=0;
    create();
}
void    HOnlineHistArray::create()
{
    if(getActive()==1)
    {
	Bool_t doLegend = kFALSE;
	Bool_t useFormat= kFALSE;
        Int_t  dim      = 0 ;
	if(legxmax != -1){
             doLegend = kTRUE;
             legend = new TLegend(legxmin,legymin,legxmax,legymax,"","brNDC");
	     if(legform != "no" && legform.Contains("%i")){

		Int_t ntimes = legform.CountChar('%');
		if(ntimes == 1 && arraySize1 == 1)                   { useFormat = kTRUE; dim = 1;}
	        if(ntimes == 2 && arraySize1 > 1 && arraySize2 > 1)  { useFormat = kTRUE; dim = 2;}
	     }
	}

	array = new TObjArray((arraySize1*arraySize2)+1);
	Char_t namehist[400];
	for(Int_t i=0;i<arraySize1;i++){
	    for(Int_t j=0;j<arraySize2;j++){
		sprintf(namehist,"%s%s%i%s%i%s",this->GetName(),"_[",i,"][",j,"]");
		if(HistType.CompareTo("1S")==0)array->AddAt(new TH1S(namehist,this->GetTitle(),nBinsX,Xlo,Xup),i*arraySize2+j);
                if(HistType.CompareTo("1F")==0)array->AddAt(new TH1F(namehist,this->GetTitle(),nBinsX,Xlo,Xup),i*arraySize2+j);
                if(HistType.CompareTo("1D")==0)array->AddAt(new TH1D(namehist,this->GetTitle(),nBinsX,Xlo,Xup),i*arraySize2+j);
                ((TH1*)((*array)[i*arraySize2+j]))->SetDirectory(0);
		setHist((TH1*)((*array)[i*arraySize2+j]));
		if(doLegend)
		{
		    if(!useFormat) legend ->AddEntry(((TH1*)((*array)[i*arraySize2+j])),Form("%s",((TH1*)((*array)[i*arraySize2+j]))->GetName()),"lpf");
		    else {
			if(dim==1) legend->AddEntry(((TH1*)((*array)[i*arraySize2+j])),Form(legform.Data(),j  ),"lpf");
			if(dim==2) legend->AddEntry(((TH1*)((*array)[i*arraySize2+j])),Form(legform.Data(),i,j),"lpf");
		    }
		}
	    }
	}
	sprintf(namehist,"%s%s",this->GetName(),"_dummy");
	array->AddAt(new TH1S(namehist,this->GetTitle(),nBinsX,Xlo,Xup),(arraySize1*arraySize2));
	((TH1*)((*array)[arraySize1*arraySize2]))->SetDirectory(0);
	setHist((TH1*)((*array)[arraySize1*arraySize2]));
    }
}
void    HOnlineHistArray::drawAll(Bool_t allowHiddenZero , Double_t minSet,Double_t maxSet)
{
    if(getActive()==1)
    {
	Bool_t useSet = kFALSE;
        if(minSet > -1e199 && maxSet < 1e199 ) useSet = kTRUE;

	Float_t* maxvalues=new Float_t [(arraySize1*arraySize2)];
	Float_t* minvalues=new Float_t [(arraySize1*arraySize2)];
        TH1* htemp;
	for(Int_t i=0;i<(arraySize1*arraySize2);i++){
            htemp=((TH1*)((*array)[i]));
	    maxvalues[i]=htemp->GetBinContent(htemp->GetMaximumBin());
	    minvalues[i]=htemp->GetBinContent(htemp->GetMinimumBin());
	}

	Int_t   loc =TMath::LocMax((arraySize1*arraySize2),maxvalues);
	Int_t   loc1=TMath::LocMin((arraySize1*arraySize2),minvalues);

        Float_t MAX,MIN;
	if(max==-99){
	     MAX=maxvalues[loc]*1.1;
	} else MAX=max;
	if(min==-99){
	    if(logY==0)MIN=minvalues[loc1];
	    else MIN=maxvalues[loc]*0.001;
	} else MIN=min;

	htemp=((TH1*)((*array)[arraySize1*arraySize2]));

	TString opt=htemp->GetOption();

	if(!drawSame){
	    if(allowHiddenZero!=0 || logY!=0 ) { htemp->SetMinimum(MIN);   }
	    else                               { htemp->SetMinimum(-1111); }
            if(!useSet) htemp->SetMaximum(MAX);
	    else        {
		htemp->SetMinimum(1.1*minSet);
		htemp->SetMaximum(1.1*maxSet);
	    }
	    if(opt.CompareTo("")==0)       htemp->Draw();
	    else if(opt.CompareTo("p")==0) htemp->Draw("p");
	    else if(opt.CompareTo("pl")==0)htemp->Draw("pl");
	}

	for(Int_t i=0;i<(arraySize1*arraySize2);i++){
 	    htemp=((TH1*)((*array)[i]));
	    if(opt.CompareTo("")==0)       htemp->Draw("same");
	    else if(opt.CompareTo("p")==0) htemp->Draw("psame");
	    else if(opt.CompareTo("pl")==0)htemp->Draw("plsame");
	}
	delete [] maxvalues;
	delete [] minvalues;

        if(legend && !drawSame) legend->Draw();
    }
}
void    HOnlineHistArray::fill(Int_t s,Int_t m,Stat_t a,Stat_t b){((TH1F*) (*array)[s*arraySize2+m])->Fill(a);}
void    HOnlineHistArray::setBinContent(Int_t s,Int_t m,Int_t Nbin,Stat_t a){((TH1*) (*array)[s*arraySize2+m])->SetBinContent(Nbin,a);}
Stat_t  HOnlineHistArray::getBinContent(Int_t s,Int_t m,Int_t Nbin)          {return ((TH1*) (*array)[s*arraySize2+m])->GetBinContent(Nbin);}
void    HOnlineHistArray::setBinError(Int_t s,Int_t m,Int_t Nbin,Stat_t a)  {((TH1*) (*array)[s*arraySize2+m])->SetBinError(Nbin,a);}
Stat_t  HOnlineHistArray::getBinError(Int_t s,Int_t m,Int_t Nbin)            {return ((TH1*) (*array)[s*arraySize2+m])->GetBinError(Nbin);}
void    HOnlineHistArray::reset(Int_t level,Int_t count)
{
    switch (level)
    {
    case 0:   if(array){for(Int_t i=0;i<arraySize1*arraySize2;i++){((TH1*)(*array)[i])->Reset();}}
    break;
    case 1:   if(array&&getResetable()==1){for(Int_t i=0;i<arraySize1*arraySize2;i++){((TH1*)(*array)[i])->Reset();}}
    break;
    case 2:   if(array&&getResetable()==1 && count%getRefreshRate()==0 && count>0){for(Int_t i=0;i<arraySize1*arraySize2;i++){((TH1*)(*array)[i])->Reset();}}
    break;
    default : Error("HOnlineHistArray::reset","UNKNOWN ARGUMENT: Level= %i !",level);
    }
}
void    HOnlineHistArray::add(HOnlineMonHistAddon* h)
{
    if(array)
    {
	for(Int_t i=0;i<arraySize1;i++){
	    for(Int_t j=0;j<arraySize2;j++){
		((TH1*)((*array)[i*arraySize2+j]))->Add((TH1*)(((HOnlineHistArray*)h)->getP(i,j)));
	    }
	}
    }
    else Error("HOnlineHistArray::add()","ARRAY DOES NOT EXISI!");
}
void    HOnlineHistArray::draw( Bool_t allowHiddenZero)
{
    drawAll(allowHiddenZero);
}

void  HOnlineHistArray::getMinMax(Double_t& min,Double_t& max)
{
    min =  1e200;
    max = -1e200;
    for(Int_t n =0 ; n < array->GetSize(); n++){
	TH1* h = (TH1*)array->At(n);
	if(!h) continue;
	for(Int_t i = 0; i < h->GetNbinsX(); i++ ) {
	    Double_t val = h->GetBinContent(i+1);
	    if(val < min) min = val;
	    if(val > max) max = val;

	}
    }
}


HOnlineHistArray::~HOnlineHistArray(){if(array)array->Delete();array=0; if(legend) delete legend;}

//############################################################################################################


HOnlineHistArray2::HOnlineHistArray2(const Char_t* name,
			 const Char_t* title,
			 Int_t active,Int_t resetable,Int_t size1,Int_t size2,
			 Int_t nbinsX,Float_t xlo,Float_t xup)
:HOnlineMonHistAddon (name,title,active,resetable,nbinsX,xlo,xup,0,0.F,0.F)
{
    setSize1(size1);
    setSize2(size2);
    array=0;
}
HOnlineHistArray2::HOnlineHistArray2(HOnlineMonHistAddon& add)
{
    copySet(add);
    array=0;
    create();
}
void    HOnlineHistArray2::create()
{
    if(getActive()==1)
    {

	Bool_t doLegend = kFALSE;
	Bool_t useFormat= kFALSE;
        Int_t  dim      = 0 ;
	if(legxmax != -1){
             doLegend = kTRUE;
             legend = new TLegend(legxmin,legymin,legxmax,legymax,"","brNDC");
	     if(legform != "no" && legform.Contains("%i")){

		Int_t ntimes = legform.CountChar('%');
		if(ntimes == 1 && arraySize1 == 1)                   { useFormat = kTRUE; dim = 1;}
	        if(ntimes == 2 && arraySize1 > 1 && arraySize2 > 1)  { useFormat = kTRUE; dim = 2;}
	     }
	}

	array = new TObjArray((arraySize1*arraySize2)+1);
	Char_t namehist[400];
	for(Int_t i=0;i<arraySize1;i++){
	    for(Int_t j=0;j<arraySize2;j++){
		sprintf(namehist,"%s%s%i%s%i%s",this->GetName(),"_[",i,"][",j,"]");
		if(HistType.CompareTo("2S")==0)array->AddAt(new TH2S(namehist,this->GetTitle(),nBinsX,Xlo,Xup,nBinsY,Ylo,Yup),i*arraySize2+j);
                if(HistType.CompareTo("2F")==0)array->AddAt(new TH2F(namehist,this->GetTitle(),nBinsX,Xlo,Xup,nBinsY,Ylo,Yup),i*arraySize2+j);
                if(HistType.CompareTo("2D")==0)array->AddAt(new TH2D(namehist,this->GetTitle(),nBinsX,Xlo,Xup,nBinsY,Ylo,Yup),i*arraySize2+j);
                ((TH2*)((*array)[i*arraySize2+j]))->SetDirectory(0);
		setHist((TH2*)((*array)[i*arraySize2+j]));
		if(doLegend)
		{
		    if(!useFormat) legend ->AddEntry(((TH1*)((*array)[i*arraySize2+j])),Form("%s",((TH1*)((*array)[i*arraySize2+j]))->GetName()),"lpf");
		    else {
			if(dim==1) legend->AddEntry(((TH1*)((*array)[i*arraySize2+j])),Form(legform.Data(),j  ),"lpf");
			if(dim==2) legend->AddEntry(((TH1*)((*array)[i*arraySize2+j])),Form(legform.Data(),i,j),"lpf");
		    }
		}
	    }
	}
	sprintf(namehist,"%s%s",this->GetName(),"_dummy");
	array->AddAt(new TH2S(namehist,this->GetTitle(),nBinsX,Xlo,Xup,2,Ylo,Yup),(arraySize1*arraySize2));
	((TH2*)((*array)[arraySize1*arraySize2]))->SetDirectory(0);
	setHist((TH2*)((*array)[arraySize1*arraySize2]));
    }
}
void    HOnlineHistArray2::drawAll( Bool_t allowHiddenZero,Double_t minSet,Double_t maxSet)
{

    if(getActive()==1)
    {
	Bool_t useSet = kFALSE;
        if(minSet > -1e199 && maxSet < 1e199 ) useSet = kTRUE;

	Float_t* maxvalues=new Float_t [(arraySize1*arraySize2)];
	Float_t* minvalues=new Float_t [(arraySize1*arraySize2)];
        TH2* htemp;
	for(Int_t i=0;i<(arraySize1*arraySize2);i++){
            htemp=((TH2*)((*array)[i]));
	    maxvalues[i]=htemp->GetBinContent(htemp->GetMaximumBin());
	    minvalues[i]=htemp->GetBinContent(htemp->GetMinimumBin());
	}

	Int_t   loc =TMath::LocMax((arraySize1*arraySize2),maxvalues);
	Int_t   loc1=TMath::LocMin((arraySize1*arraySize2),minvalues);

        Float_t MAX,MIN;
	if(max==-99){
	     MAX=maxvalues[loc]*1.1;
	} else MAX=max;
	if(min==-99){
	    if(logY==0)MIN=minvalues[loc1];
	    else MIN=maxvalues[loc]*0.001;
	} else MIN=min;

	htemp=((TH2*)((*array)[arraySize1*arraySize2]));

	if(!drawSame){

	    if(allowHiddenZero!=0 || logY!=0 ) { htemp->SetMinimum(MIN);   }
	    else                               { htemp->SetMinimum(-1111); }

            if(!useSet) htemp->SetMaximum(MAX);
            else {
		htemp->SetMinimum(1.1*minSet);
		htemp->SetMaximum(1.1*maxSet);
	    }

	    htemp->Draw();

	}

	for(Int_t i=0;i<(arraySize1*arraySize2);i++){
 	    htemp=((TH2*)((*array)[i]));
	    htemp->Draw("same");
	}
	delete [] maxvalues;
	delete [] minvalues;

        if(legend && !drawSame) legend->Draw();
    }
}
void    HOnlineHistArray2::draw( Bool_t allowHiddenZero)
{
    drawAll(allowHiddenZero);
}
void    HOnlineHistArray2::fill(Int_t s,Int_t m,Stat_t a,Stat_t b){((TH2*) (*array)[s*arraySize2+m])->Fill(a,b);}
void    HOnlineHistArray2::setBinContent(Int_t s,Int_t m,Int_t Nbin,Int_t N2bin,Stat_t a){((TH2*) (*array)[s*arraySize2+m])->SetBinContent(Nbin,N2bin,a);}
Stat_t  HOnlineHistArray2::getBinContent(Int_t s,Int_t m,Int_t Nbin,Int_t N2bin)          {return ((TH2*) (*array)[s*arraySize2+m])->GetBinContent(Nbin,N2bin);}
void    HOnlineHistArray2::setBinError(Int_t s,Int_t m,Int_t Nbin,Int_t N2bin,Stat_t a)  {((TH2*) (*array)[s*arraySize2+m])->SetBinError(Nbin,N2bin,a);}
Stat_t  HOnlineHistArray2::getBinError(Int_t s,Int_t m,Int_t Nbin,Int_t N2bin)            {return ((TH2*) (*array)[s*arraySize2+m])->GetBinError(Nbin,N2bin);}
void    HOnlineHistArray2::reset(Int_t level,Int_t count)
{
    switch (level)
    {
    case 0:   if(array){for(Int_t i=0;i<arraySize1*arraySize2;i++){((TH2*)(*array)[i])->Reset();}}
    break;
    case 1:   if(array&&getResetable()==1){for(Int_t i=0;i<arraySize1*arraySize2;i++){((TH2*)(*array)[i])->Reset();}}
    break;
    case 2:   if(array&&getResetable()==1 && count%getRefreshRate()==0 && count>0){for(Int_t i=0;i<arraySize1*arraySize2;i++){((TH2*)(*array)[i])->Reset();}}
    break;
    default : Error("HOnlineHistArray2::reset","UNKNOWN ARGUMENT: Level= %i !",level);
    }
}
void    HOnlineHistArray2::add(HOnlineMonHistAddon* h)
{
    if(array)
    {
	for(Int_t i=0;i<arraySize1;i++){
	    for(Int_t j=0;j<arraySize2;j++){
		((TH2*)((*array)[i*arraySize2+j]))->Add(((HOnlineHistArray2*)h)->getP(i,j));
	    }
	}
    }
    else Error("HOnlineHistArray2::add()","ARRAY DOES NOT EXISI!");
}

void HOnlineHistArray2::getMinMax(Double_t& min,Double_t& max)
{
    min =  1e200;
    max = -1e200;
    for(Int_t n =0 ; n < array->GetSize(); n++){
	TH2* h = (TH2*)array->At(n);
	if(!h) continue;

	for(Int_t i = 0; i < h->GetNbinsX(); i++ ) {
	    for(Int_t j = 0; j < h->GetNbinsY(); j++ ) {
		Double_t val = h->GetBinContent(i+1,j+1);
		if(val < min) min = val;
		if(val > max) max = val;
	    }
	}
    }
}



HOnlineHistArray2::~HOnlineHistArray2(){if(array)array->Delete();array=0; if(legend) delete legend;}
