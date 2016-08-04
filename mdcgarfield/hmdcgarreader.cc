//*-- AUTHOR : J. Markert

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HMdcGarReader
// Class to read several GARFIELD formats
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>
#include <iostream> 
#include <fstream>
#include <iomanip>
#include "hmdcgarreader.h"
#include "htool.h"
#include "TMath.h"
#include "TArray.h"
#include "TStyle.h"

ClassImp(HMdcGarReader)

HMdcGarReader::HMdcGarReader(const Char_t* name,const Char_t* title)
    : TNamed(name,title)
{
    // constructor for HMdcGarReader
    initVariables();
}
HMdcGarReader::~HMdcGarReader()
{
  // destructor of HMdcGarReader
}
void HMdcGarReader::initVariables()
{
    // inits all variables
}
TGraph* HMdcGarReader::readXT(TString inputname,Int_t color)
{
    // Reads GARFIELD XT curve from ascii input "inputname" into a TGraph
    // with LineColor "color", which is returned

    gStyle->SetPalette(50);
    FILE* input=0;
    if(!HTool::openAscii(&input,inputname,"r"))
    {
	exit(1);
    }
    cout<<"HMdcGarReader::readXT() , Reading XT-data from GARFIELD file\n"<<inputname.Data()<<endl;

    TArrayF distance;
    TArrayF drifttime;

    Int_t numberOfBins=0;

    Char_t line[400];
    Int_t scan_count;
    Float_t  f_x ,f_t;
    Char_t Tag[]=" This member contains";
    Bool_t res=kTRUE;
    while(1)
    {
	if(feof(input)) break;
	res=fgets(line, sizeof(line), input);
        if(!res) cout<<"could not read next line!"<<endl;
	if (!strncmp(line, Tag ,strlen(Tag)))
	{
            sscanf(line,"%*s%*s%*s%i%*s",&numberOfBins);
	    distance .Set(numberOfBins);
            drifttime.Set(numberOfBins);
	    break;
	}
    }
    Int_t count=0;
    while (count<numberOfBins)  {
	if (feof(input)) break;

	res=fgets(line, sizeof(line), input);
        if(!res) cout<<"could not read next line!"<<endl;
	scan_count=sscanf(line,"%*s%e%e",&f_x, &f_t);

	if(scan_count==2)
	{
	    distance [count]=f_x*10;
	    drifttime[count]=f_t*1000;
	    printf("%03i distance %7.3f mm drift time %7.3f ns\n",count,distance [count],drifttime[count]);
	    count++;
	}
    }

    Float_t* gdist =new Float_t[count];
    Float_t* gdrift=new Float_t[count];

    for(Int_t i=0;i<count;i++)
    {
        gdist [i]=distance [i];
        gdrift[i]=drifttime[i];
    }

    TGraph* h=new TGraph(count,gdist,gdrift);

    h->SetMarkerStyle(29);
    h->SetMarkerColor(4);
    h->SetMarkerSize(.8);
    h->SetLineColor(color);

    HTool::closeAscii(&input);
    return h;
}
TGraph* HMdcGarReader::readXTdiffusion(TString inputname,Int_t color)
{
    // Reads GARFIELD integrated diffusion curve from ascii input "inputname" into a TGraph
    // with LineColor "color", which is returned

    gStyle->SetPalette(50);
    FILE* input=0;
    if(!HTool::openAscii(&input,inputname,"r"))
    {
	exit(1);
    }
    cout<<"HMdcGarReader::readXTdiffusion() , Reading XT-integrated diffusion data from GARFIELD file\n"<<inputname.Data()<<endl;

    TArrayF distance;
    TArrayF drifttime;

    Int_t numberOfBins=0;

    Char_t line[400];
    Int_t scan_count;
    Float_t  f_x ,f_t;
    Char_t Tag[]=" This member contains";

    Bool_t res=kTRUE;
    while(1)
    {
	if(feof(input)) break;
	res=fgets(line, sizeof(line), input);
	if(!res) cout<<"could not read next line!"<<endl;
	if (!strncmp(line, Tag ,strlen(Tag)))
	{
            sscanf(line,"%*s%*s%*s%i%*s",&numberOfBins);
	    distance .Set(numberOfBins);
            drifttime.Set(numberOfBins);
	    break;
	}
    }
    Int_t count=0;
    while (count<numberOfBins)  {
	if (feof(input)) break;

	res=fgets(line, sizeof(line), input);
	if(!res) cout<<"could not read next line!"<<endl;
	scan_count=sscanf(line,"%*s%e%*s%*s%e",&f_x, &f_t);

	if(scan_count==2)
	{
	    distance [count]=f_x*10;
	    drifttime[count]=f_t*1000;
	    printf("%03i distance %7.3f mm integrated diffusion %7.3f ns\n",count,distance [count],drifttime[count]);
	    count++;
	}
    }

    Float_t* gdist =new Float_t[count];
    Float_t* gdrift=new Float_t[count];

    for(Int_t i=0;i<count;i++)
    {
        gdist [i]=distance [i];
        gdrift[i]=drifttime[i];
    }

    TGraph* h=new TGraph(count,gdist,gdrift);

    h->SetMarkerStyle(29);
    h->SetMarkerColor(4);
    h->SetMarkerSize(.8);
    h->SetLineColor(color);

    HTool::closeAscii(&input);
    return h;
}
TGraph* HMdcGarReader::readMagboltzVdrift(TString inputname,Int_t color)
{
    // Reads MAGBOLTZ vdrift/E (E/P:Log10) curve from ascii input "inputname" into a TGraph
    // with LineColor "color", which is returned
    TGraph* h=0;

    gStyle->SetPalette(50);
    FILE* input=0;
    if(!HTool::openAscii(&input,inputname,"r"))
    {
	exit(1);
    }
    cout<<"HMdcGarReader::readMagboltz() , Reading Vdrift/E-data from MAGBOLTZ file\n"<<inputname.Data()<<endl;

    TArrayF ep;
    TArrayF Vdrift;
    ep.Set(100);
    Vdrift.Set(100);

    Int_t numberOfBins=0;
    Int_t version=0;
    Char_t line[400];
    Float_t  f_ep ,f_vdrift;
    Char_t Tag []=" The gas tables follow";
    Char_t Tag1[]=" Dimension : F";
    Char_t Tag2[]=" Version   :";
    Char_t Tag3[]=" E fields";
    Char_t Tag4[]=" E-B angles";
    TString buffer=" ";

    Int_t count=0;
    Bool_t res=kTRUE;
    while(1)
    {
	if(feof(input)) break;
	res=fgets(line, sizeof(line), input);
        if(!res) cout<<"could not read next line!"<<endl;
	if (!strncmp(line, Tag2 ,strlen(Tag2))) // version
        {   //  find version of format
            sscanf(line,"%*s%*s%i",&version);
            cout<<"version "<<version<<endl;
        }
	if (!strncmp(line, Tag1 ,strlen(Tag1)))  // E-Field values
	{   //  find number of entries
	    sscanf(line,"%*s%*s%*s%2i%*s%*s",&numberOfBins);
	    ep    .Set(numberOfBins);
            Vdrift.Set(numberOfBins);
	    break;
	}
    }
    Int_t found=0;
    while(1&&version>=4&&found==0)
    {   // read e-field in version 4
	if(feof(input)) break;
	res=fgets(line, sizeof(line), input);
        if(!res) cout<<"could not read next line!"<<endl;
	if (!strncmp(line, Tag3 ,strlen(Tag3))) // E-Field
        {
            found=1;
            while(1)
            {
                res=fgets(line, sizeof(line), input);
		if(!res) cout<<"could not read next line!"<<endl;
                if (!strncmp(line, Tag4 ,strlen(Tag4)))  //E-B angles
                {   // if next topic is entered break
                    break;
                }

                sscanf(line,"%e%e%e%e%e",&ep[count*5+0],&ep[count*5+1],&ep[count*5+2],&ep[count*5+3],&ep[count*5+4]);
                count++;
            }
        }
    }
    count=0;
    while(1)
    {
        if(feof(input)) break;
        res=fgets(line, sizeof(line), input);
        if(!res) cout<<"could not read next line!"<<endl;
        if (!strncmp(line, Tag ,strlen(Tag))) break;
    }

    
    while (count<numberOfBins)  {
	if (feof(input)) break;
	res=fgets(line, sizeof(line), input);
        if(!res) cout<<"could not read next line!"<<endl;
        if(version==3)
        {   // read old version 3
            sscanf(line,"%e%e%*s",
		   &f_ep, &f_vdrift);
            ep    [count]=f_ep;
            Vdrift[count]=f_vdrift*10;
        }
        else if(version==4)
        {   // read new version 4
            sscanf(line,"%e%*s",
		   &f_vdrift);
            Vdrift[count]=f_vdrift*10;
        }
        else if(version==6)
        {   // read new version 6
            sscanf(line,"%e%*s",
		   &f_vdrift);
            Vdrift[count]=f_vdrift*10;
        }
        printf("%03i ep %12.2f kV/cm*bar Vdrift %7.3f ns/mu\n",count,ep[count],Vdrift[count]);

	res=fgets(line, sizeof(line), input);
	res=fgets(line, sizeof(line), input);
	res=fgets(line, sizeof(line), input);
	if(!res) cout<<"could not read next line!"<<endl;

	count++;
    };

    Float_t* gep    =new Float_t[count];
    Float_t* gVdrift=new Float_t[count];

    for(Int_t i=0;i<count;i++)
    {
        gep    [i]=ep    [i];
	gVdrift[i]=Vdrift[i];
    }

    h=new TGraph(count,gep,gVdrift);

    h->SetMarkerStyle(29);
    h->SetMarkerColor(4);
    h->SetMarkerSize(.8);
    h->SetLineColor(color);

    HTool::closeAscii(&input);
    return h;
}
TGraph* HMdcGarReader::readMagboltzGasPrint(TString inputname,TString option,Int_t color)
{
    // Reads MAGBOLTZ townsend/E (E/P:Log10) curve from ascii input "inputname" into a TGraph
    // with LineColor "color", which is returned
    TGraph* h=0;

    gStyle->SetPalette(50);
    FILE* input=0;
    if(!HTool::openAscii(&input,inputname,"r"))
    {
	exit(1);
    }
    cout<<"HMdcGarReader::readMagboltzGasPrint() , Reading Gas-Print-data from MAGBOLTZ file\n"<<inputname.Data()<<endl;

    TArrayF ep;
    TArrayF data;
    ep.Set(100);
    data.Set(100);

    Char_t line[400];
    Float_t f_ep, f_data;
    Char_t Tag []="    [V/cm]";
    TString buffer=" ";
    Bool_t res=kTRUE;
    Int_t count=0;
    while(1)
    {
	if(feof(input)) break;
	res=fgets(line, sizeof(line), input);
	if(!res) cout<<"could not read next line!"<<endl;

	if (!strncmp(line, Tag ,strlen(Tag)))
        {
            //  find first line
	    break;
	}
    }

    while(1)
    {   // read
        if(feof(input)) break;
        res=fgets(line, sizeof(line), input);
        if(!res) cout<<"could not read next line!"<<endl;
	buffer=line;
        if(buffer.CompareTo("\n")==0)break;
        buffer.ReplaceAll("Not available","-1");
        if(option.CompareTo("vdrift")==0)
        {
            sscanf(buffer.Data(),"%f%f%*s%*s%*s%*s%*s%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=10*f_data;
            printf("%03i ep %12.2f kV/cm Vdrift %7.3f ns/mu\n",count,ep[count],data[count]);
        }
        else if(option.CompareTo("ionmobility")==0)
        {
            sscanf(buffer.Data(),"%f%*s%f%*s%*s%*s%*s%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm ion mobility %7.3f [cm2/V.musec]\n",count,ep[count],data[count]);
      
        }
        else if(option.CompareTo("diffusion_long")==0)
        {
            sscanf(buffer.Data(),"%f%*s%*s%f%*s%*s%*s%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm diffusion Long_t %7.3f [micron for 1 cm]\n",count,ep[count],data[count]);
        }
        else if(option.CompareTo("diffusion_trans")==0)
        {
            sscanf(buffer.Data(),"%f%*s%*s%*s%f%*s%*s%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm diffusion trans %7.3f [micron for 1 cm]\n",count,ep[count],data[count]);
  
        }
        else if(option.CompareTo("townsend")==0)
        {
            sscanf(buffer.Data(),"%f%*s%*s%*s%*s%f%*s%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm townsend %7.3f [1/cm]\n",count,ep[count],data[count]);
        }
        else if(option.CompareTo("attachment")==0)
        {
            sscanf(buffer.Data(),"%f%*s%*s%*s%*s%*s%f%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm attachment %7.3f [1/cm]\n",count,ep[count],data[count]);
        }
        else if(option.CompareTo("lorentz")==0)
        {
            sscanf(buffer.Data(),"%f%*s%*s%*s%*s%*s%*s%f",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm lorentz angle %7.3f [degrees]\n",count,ep[count],data[count]);
        }
        count++;
    }

    Float_t* gep    =new Float_t[count];
    Float_t* gdata  =new Float_t[count];

    for(Int_t i=0;i<count;i++)
    {
        gep  [i]=ep  [i];
	gdata[i]=data[i];
    }

    h=new TGraph(count,gep,gdata);

    h->SetMarkerStyle(29);
    h->SetMarkerColor(4);
    h->SetMarkerSize(.8);
    h->SetLineColor(color);

    HTool::closeAscii(&input);
    return h;
}
TGraph* HMdcGarReader::readMagboltzGasPrintVersion6(TString inputname,TString option,Int_t color)
{
    // Reads MAGBOLTZ townsend/E (E/P:Log10) curve from ascii input "inputname" into a TGraph
    // with LineColor "color", which is returned
    TGraph* h=0;

    gStyle->SetPalette(50);
    FILE* input=0;
    if(!HTool::openAscii(&input,inputname,"r"))
    {
	exit(1);
    }
    cout<<"HMdcGarReader::readMagboltzGasPrint() , Reading Gas-Print-data from MAGBOLTZ file\n"<<inputname.Data()<<endl;

    TArrayF ep;
    TArrayF data;
    ep.Set(100);
    data.Set(100);

    Char_t line[400];
    Float_t f_ep, f_data;
    Char_t Tag []="    [V/cm]";
    TString buffer=" ";

    Bool_t res=kTRUE;
    Int_t count=0;
    while(1)
    {
	if(feof(input)) break;
	res=fgets(line, sizeof(line), input);
        if(!res) cout<<"could not read next line!"<<endl;

	if (!strncmp(line, Tag ,strlen(Tag)))
        {
            //  find first line
	    break;
	}
    }

    while(1)
    {   // read
        if(feof(input)) break;
        res=fgets(line, sizeof(line), input);
        if(!res) cout<<"could not read next line!"<<endl;

	buffer=line;
        if(buffer.CompareTo("\n")==0)break;
        buffer.ReplaceAll("Not available","-1");
        if(option.CompareTo("vdrift")==0)
        {
            sscanf(buffer.Data(),"%f%f%*s%*s%*s%*s%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=10*f_data;
            printf("%03i ep %12.2f kV/cm Vdrift %7.3f ns/mu\n",count,ep[count],data[count]);
        }
        else if(option.CompareTo("ionmobility")==0)
        {
            sscanf(buffer.Data(),"%f%*s%f%*s%*s%*s%*s%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm ion mobility %7.3f [cm2/V.musec]\n",count,ep[count],data[count]);
      
        }
        else if(option.CompareTo("diffusion_long")==0)
        {
            sscanf(buffer.Data(),"%f%*s%f%*s%*s%*s%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm diffusion Long_t %7.3f [micron for 1 cm]\n",count,ep[count],data[count]);
        }
        else if(option.CompareTo("diffusion_trans")==0)
        {
            sscanf(buffer.Data(),"%f%*s%*s%f%*s%*s%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm diffusion trans %7.3f [micron for 1 cm]\n",count,ep[count],data[count]);
  
        }
        else if(option.CompareTo("townsend")==0)
        {
            sscanf(buffer.Data(),"%f%*s%*s%*s%f%*s%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm townsend %7.3f [1/cm]\n",count,ep[count],data[count]);
        }
        else if(option.CompareTo("attachment")==0)
        {
            sscanf(buffer.Data(),"%f%*s%*s%*s%*s%f%*s",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm attachment %7.3f [1/cm]\n",count,ep[count],data[count]);
        }
        else if(option.CompareTo("lorentz")==0)
        {
            sscanf(buffer.Data(),"%f%*s%*s%*s%*s%*s%f",&f_ep,&f_data);
            ep  [count]=0.001*f_ep;
            data[count]=f_data;
            printf("%03i ep %12.2f kV/cm lorentz angle %7.3f [degrees]\n",count,ep[count],data[count]);
        }
        count++;
    }

    Float_t* gep    =new Float_t[count];
    Float_t* gdata  =new Float_t[count];

    for(Int_t i=0;i<count;i++)
    {
        gep  [i]=ep  [i];
	gdata[i]=data[i];
    }

    h=new TGraph(count,gep,gdata);

    h->SetMarkerStyle(29);
    h->SetMarkerColor(4);
    h->SetMarkerSize(.8);
    h->SetLineColor(color);

    HTool::closeAscii(&input);
    return h;
}
TArrayD* HMdcGarReader::readMatrix(TString inputname,Int_t& size1,Int_t& size2)
{
    // matrix is written in linear form for 1 and 2-D
    // In 2-D case: matrix[x][y] is written as loop over y inside loop over x

    fstream input;
    input.open(inputname.Data(),ios::in);


    Char_t line[400];

    Char_t Tag []=" Dimension:";
    Char_t Tag1[]=" Sizes:";
    Char_t Tag2[]=" CONTENTS";
    Int_t dimension=0;
    size1=0; // first index
    size2=1; // second index
    while(1)
    {   // reading format information
	if(input.eof()) break;
	input.getline(line, sizeof(line));

	if (!strncmp(line, Tag ,strlen(Tag)))
	{
	    sscanf(line,"%*s%i",&dimension);
	}
	if (!strncmp(line, Tag1 ,strlen(Tag1)))
	{
	   if(dimension==1) sscanf(line,"%*s%i"  ,&size1);
           if(dimension==2) sscanf(line,"%*s%i%i",&size1,&size2);
	}
	if (!strncmp(line, Tag2 ,strlen(Tag2)))
	{   // end of format information
	    break;
	}
    }
    TArrayD* array=new TArrayD(size1*size2);
    cout<<"Dimension of array: "<<dimension<<endl;
    cout<<"Number of points  : "<<size1*size2<<endl;
    Int_t count=0;
    Int_t col,row;
    while (count<size1*size2)  {
	if (input.eof()) break;

        input>>(*array)[count];
	if(dimension==1)cout<<"point : "<<count+1<<" = "<< (*array)[count]<<endl;
	if(dimension==2)
	{
            col=(count)%(size1);
            row=(Int_t)((count)/size1);
	    cout<<"point : "<<count+1<<" col "<<col<<" row "<<row <<" = "<< (*array)[count]<<endl;
	}
	count++;
    }
    input.close();
    return array;
}
TH1F* HMdcGarReader::readMatrix1DToHist(TString inputname,Float_t binsize1,Float_t start1)
{
   Int_t size1,size2;
   TArrayD* array=readMatrix(inputname,size1,size2);
   Int_t size=array->GetSize();

   // creating hist name from input file
   TString histname=inputname;
   if(histname.Contains("/")==1)
   { // do not use path names
       histname.Replace(0,inputname.Last('/')+1,"",0);
   }
   if(histname.Contains(".")==1)
   { // skip file extensions
      histname.Replace(histname.First('.'),histname.Sizeof(),"",0);
   }

   TH1F* h=new TH1F(histname.Data(),"",size,start1,start1+binsize1*size);

   cout<<"Input file       : "<<inputname.Data()<<endl;
   cout<<"Histogram name   : "<<histname.Data() <<endl;
   cout<<"Number of bins   : "<<size<<endl;
   cout<<"Minimum x range  : "<<start1<<endl;
   cout<<"Maximum x range  : "<<start1+binsize1*size<<endl;

   for(Int_t i=0;i<size;i++){
	   h->SetBinContent(i+1,(*array)[i]);
   }
   delete array;
   return h;
}
TH2F* HMdcGarReader::readMatrix2DToHist(TString inputname,Float_t binsize1,Float_t start1,Float_t binsize2,Float_t start2)
{
   Int_t size1,size2;
   TArrayD* array=readMatrix(inputname,size1,size2);

   // creating hist name from input file
   TString histname=inputname;
   if(histname.Contains("/")==1)
   { // do not use path names
       histname.Replace(0,inputname.Last('/')+1,"",0);
   }
   if(histname.Contains(".")==1)
   { // skip file extensions
      histname.Replace(histname.First('.'),histname.Sizeof(),"",0);
   }

   TH2F* h=new TH2F(histname.Data(),"",size1,start1,start1+binsize1*size1,size2,start2,start2+binsize2*size2);

   cout<<"Input file       : "<<inputname.Data()<<endl;
   cout<<"Histogram name   : "<<histname.Data() <<endl;
   cout<<"Number of binsX  : "<<size1<<endl;
   cout<<"Minimum x range  : "<<start1<<endl;
   cout<<"Maximum x range  : "<<start1+binsize1*size1<<endl;
   cout<<"Number of binsY  : "<<size2<<endl;
   cout<<"Minimum y range  : "<<start2<<endl;
   cout<<"Maximum y range  : "<<start2+binsize2*size2<<endl;

   for(Int_t i=0;i<size1;i++){
       for(Int_t j=0;j<size2;j++){
	   h->SetBinContent(i+1,j+1,(*array)[j*size1+i]);
       }
   }
   delete array;
   return h;
}


TH1F* HMdcGarReader::readHist1D(TString inputname,Float_t scale,Float_t scalex)
{
    fstream input;
    input.open(inputname.Data(),ios::in);


    Char_t line[400];

    Char_t Tag []=" Minimum:";
    Char_t Tag1[]=" Maximum:";
    Char_t Tag2[]=" Bins:";
    Char_t Tag3[]=" CONTENTS";
    Float_t xmin=0;
    Float_t xmax=0;
    Int_t bins=0;
    while(1)
    {   // reading format information
	if(input.eof()) break;
	input.getline(line, sizeof(line));

	if (!strncmp(line, Tag ,strlen(Tag)))
	{   // minimum range
	    sscanf(line,"%*s%f",&xmin);
	}
	if (!strncmp(line, Tag1 ,strlen(Tag1)))
	{   // maximum range
	    sscanf(line,"%*s%f",&xmax);
	}
        if (!strncmp(line, Tag2 ,strlen(Tag2)))
	{   // number of bins
	    sscanf(line,"%*s%i",&bins);
	}
	if (!strncmp(line, Tag3 ,strlen(Tag2)))
	{   // end of format information
	    break;
	}
    }
    xmin=scalex*xmin;
    xmax=scalex*xmax;

    // creating hist name from input file
    TString histname=inputname;
    if(histname.Contains("/")==1)
    { // do not use path names
      histname.Replace(0,inputname.Last('/')+1,"",0);
    }
    if(histname.Contains(".")==1)
    { // skip file extensions
      histname.Replace(histname.First('.'),histname.Sizeof(),"",0);
    }

    TH1F* h=new TH1F(histname.Data(),"",bins,xmin,xmax);

    cout<<"Input file       : "<<inputname.Data()<<endl;
    cout<<"Histogram name   : "<<histname.Data() <<endl;
    cout<<"Number of bins   : "<<bins<<endl;
    cout<<"Minimum x range  : "<<xmin<<endl;
    cout<<"Maximum x range  : "<<xmax<<endl;
    Int_t count=0;
    Float_t content=0;
    Int_t binnumber=0;
    while (count<bins+1)  {
	if (input.eof()) break;
	input.getline(line, sizeof(line));
        sscanf(line,"%i%f",&binnumber,&content);
	if(count!=0){
	    h->SetBinContent(binnumber,content);
	    cout<<"bin "<<binnumber<<"   "<<content<<endl;
	}
	count++;
    }
    h->Scale(scale);
    input.close();
    return h;
}
TH1F* HMdcGarReader::readHist1D(TString inputname1,TString inputname2,Float_t scale,Float_t scalex)
{
    cout<<"Reading data points:"<<endl;
    TH1F* h   =readHist1D(inputname1,scale,scalex);
    cout<<"Reading errors:"<<endl;
    TH1F* herr=readHist1D(inputname2,scale,scalex);

    for(Int_t i=0;i<h->GetNbinsX();i++){
       h->SetBinError(i+1,herr->GetBinError(i+1));
    }
    herr->Delete();
    return h;
}
TGraph* HMdcGarReader::readHist1DToGraph(TString inputname1,Float_t scale,Float_t scalex)
{
    cout<<"Reading data points:"<<endl;
    TH1F* h   =readHist1D(inputname1,scale,scalex);
    TGraph* g=new TGraphErrors();
    Int_t binstep  =(Int_t)((h->GetXaxis()->GetXmax() - h->GetXaxis()->GetXmin())/(Float_t)(h->GetNbinsX()-1));

    for(Int_t i=0;i<h->GetNbinsX();i++){
	g->SetPoint     (i,(i+1)*binstep,h   ->GetBinContent(i+1));
    }

    h   ->Delete();
    return g;
}
TGraphErrors* HMdcGarReader::readHist1DToGraph(TString inputname1,TString inputname2,Float_t scale,Float_t scalex)
{
    cout<<"Reading data points:"<<endl;
    TH1F* h   =readHist1D(inputname1,scale,scalex);
    cout<<"Reading errors:"<<endl;
    TH1F* herr=readHist1D(inputname2,scale,scalex);

    TGraphErrors* g=new TGraphErrors();
    Int_t binstep  =(Int_t)((h->GetXaxis()->GetXmax() - h->GetXaxis()->GetXmin())/(Float_t)(h->GetNbinsX()-1));

    for(Int_t i=0;i<h->GetNbinsX();i++){
	g->SetPoint     (i,(i+1)*binstep,h   ->GetBinContent(i+1));
        g->SetPointError(i,0,herr->GetBinContent(i+1));
    }

    h   ->Delete();
    herr->Delete();

    return g;
}



TObjArray* HMdcGarReader::readSignal(TString inputName,Int_t nbins,Double_t xmin,Double_t xmax,Int_t version)
{


    FILE* inputAscii = fopen(inputName.Data(),"r");
    if (inputAscii == NULL)
    {
        ::Error("HMdcGarSignalReader::readInput()","Cannot open %s\n",inputName.Data());
        exit(1);
    }
    cout<<"HMdcGarSignalReader::readInput()   : Reading from "<<inputName.Data()<<endl;


    TObjArray* array =  new TObjArray();



    Char_t Tag []=" + VALUE_SCALE_FACTOR";
    Char_t Tag1[]="% Created";
    Char_t Tag3[]=" +";
    Char_t line[400];
    Int_t signalcount = 0;
    Int_t sigcount    = 0;
    Bool_t condition  = kFALSE;
    Int_t bin = 0;
    TH1F* hsignal = 0;
    Float_t f_time,f_current;

    cout<<"creating hist "<< signalcount<<endl;
    hsignal = new TH1F(Form("hsignal_%i",signalcount),"Current as function of drift time",nbins,xmin,xmax);
    hsignal->SetXTitle("drift time [ns]");
    hsignal->SetYTitle("current [#mu A]");
    array->Add(hsignal);

    Bool_t res=kTRUE;
    while(!feof(inputAscii))
    {   // end of file condition
        //--------------------------------- search first value ---------------------------
        while(1)
        {
            if(feof(inputAscii)) break;
            res=fgets(line, sizeof(line), inputAscii);
	    if(!res) cout<<"could not read next line!"<<endl;
	    if (!strncmp(line, Tag ,strlen(Tag))) break;  // break if " + VALUE_SCALE_FACTOR" has been found
        }
        //--------------------------------------------------------------------------------
        while(1)
        {
            if (feof(inputAscii)) break;
            res=fgets(line, sizeof(line), inputAscii);
	    if(!res) cout<<"could not read next line!"<<endl;
	    if(strncmp(line,Tag1,strlen(Tag1)) && !strncmp(line,Tag3,strlen(Tag3)) ) // read until "% Created" has been found
            {
                //--------------------------------- read signal ----------------------------------
                sscanf(&line[5],"%e%e%*s",&f_time, &f_current);

                condition=kFALSE;
                if      (version==704)                    condition=kTRUE;
                else if (version==708&&(signalcount%2==0))condition=kTRUE;

                if(condition)
                {
                    hsignal->SetBinContent(bin,f_current);
                    bin++;
                }
                //--------------------------------------------------------------------------------
            }
            else
            {
                signalcount++;

                condition=kFALSE;
                if      (version==704)                      condition=kTRUE;
                else if (version==708&&((signalcount)%2==0))condition=kTRUE;

                if(condition)
                {
                    sigcount++;
                    // new signal found
                    cout<<"creating hist "<< sigcount<<endl;
                    hsignal = new TH1F(Form("hsignal_%i",sigcount),"Current as function of drift time",nbins,xmin,xmax);
                    hsignal->SetXTitle("drift time [ns]");
                    hsignal->SetYTitle("current [#mu A]");
                    array->Add(hsignal);
                    bin = 0;
                }
                break;
            }
        }
    }
    //---------------- very last signal of the file ----------------------------------
    return array;
}

TGraph* HMdcGarReader::readGraphE(TString inputName,TString dir,Float_t min,Float_t max,Int_t color,Float_t scale,Bool_t suppressZero)
{
    // inputName = file name of GARFIELD plot e
    // dir       = x (default) coordinate along x
    //           = y           coordinate along y
    // min,max   = (default -999999) no cut on range, otherwise collect values along coordinate between min-max
    // color     = (default = 2) color of line/marker
    // scale     = (default = 10) cm -> mm
    // suppressZero = (default = kTRUE) do not fill field==0 points

    FILE* inputAscii = fopen(inputName.Data(),"r");
    if (inputAscii == NULL)
    {
        ::Error("HMdcGarSignalReader::readGraphE()","Cannot open %s\n",inputName.Data());
        exit(1);
    }
    cout<<"HMdcGarSignalReader::readGraphE()   : Reading from "<<inputName.Data()<<endl;



    Char_t line[400];
    Char_t Tag [] ="  Coordinate";
    Int_t bin     = 0;
    Float_t f_x,f_y,f_e;


    Bool_t xcoord = kTRUE;
    if(dir == "y") xcoord = kFALSE;

    TGraph* g = new TGraph();
    g->SetMarkerColor(color);
    g->SetLineColor(color);
    Bool_t res=kTRUE;
    while(!feof(inputAscii))
    {   // end of file condition
        //--------------------------------- search first value ---------------------------
        while(1)
        {
            if(feof(inputAscii)) break;
            res=fgets(line, sizeof(line), inputAscii);
	    if(!res) cout<<"could not read next line!"<<endl;
	    if (!strncmp(line, Tag ,strlen(Tag))) break;  // break if "  Coordinate" has been found
        }
        //--------------------------------------------------------------------------------
        while(1)
        {
            if (feof(inputAscii)) break;
            res=fgets(line, sizeof(line), inputAscii);
	    if(!res) cout<<"could not read next line!"<<endl;
	    sscanf(line,"%e%e%*s%e",&f_x,&f_y, &f_e);
            if(suppressZero && f_e == 0) continue;
            if(xcoord){
                if((min ==-999999 || f_x*scale >= min) &&
                   (max ==-999999 || f_x*scale <= max)){
                    g->SetPoint(bin,f_x*scale,f_e);
                    bin++;
                }
            } else {
                if((min ==-999999 || f_y*scale >= min) &&
                   (max ==-999999 || f_y*scale <= max)){
                    g->SetPoint(bin,f_y*scale,f_e);
                    bin++;
                }
            }
        }
    }
    g->Print("All");
    return g;
}
