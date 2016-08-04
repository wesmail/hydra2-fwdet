//*-- AUTHOR : J. Markert

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HMdcGarMacroMaker
// Class to create a GARFIELD Macro for Batch farm processing to simulate
// a couple of tracks for each sample point (distance from wire). The produced
// signals on the sense wire will be written to an ascii file, which can be read in
// via HMdcGarSignalReader afterwards.
// As input you have to specify the Mdc (0-3) and the angle (0-85 degree)
// where 0 degree is perpendicular impact of the track.
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>
#include <iostream> 
#include <iomanip>
#include "hmdcgarmacromaker.h"
#include "TString.h"
#include "TObjString.h"
#include "TMath.h"
#include "TList.h"
                                                  
ClassImp(HMdcGarMacroMaker)

Float_t HMdcGarMacroMaker::cathodth   [4]={0.01 ,0.01 ,0.01 ,0.01 };
Float_t HMdcGarMacroMaker::senseth    [4]={0.002,0.002,0.002,0.002};
Float_t HMdcGarMacroMaker::fieldth    [4]={0.01 ,0.01 ,0.01 ,0.01 };

Float_t HMdcGarMacroMaker::cathodpitch[4]={0.2,0.2,0.3,0.4};
Float_t HMdcGarMacroMaker::sensepitch [4]={0.5,0.6,1.2,1.4};
Float_t HMdcGarMacroMaker::fieldpitch [4]={0.5,0.6,1.2,1.4};

Int_t   HMdcGarMacroMaker::cathodhv   [4]={-1750,-1800,-2000,-2400};
Int_t   HMdcGarMacroMaker::sensehv    [4]={    0,    0,    0,    0};
Int_t   HMdcGarMacroMaker::fieldhv    [4]={-1750,-1800,-2000,-2400};

Int_t   HMdcGarMacroMaker::cathodhvmod   [4]={0,0,0,0};
Int_t   HMdcGarMacroMaker::sensehvmod    [4]={0,0,0,0};
Int_t   HMdcGarMacroMaker::fieldhvmod    [4]={0,0,0,0};

Int_t   HMdcGarMacroMaker::ncathod    [4]={10,10,13,13};
Int_t   HMdcGarMacroMaker::nsense     [4]={3 ,3 ,3 ,3 };
Int_t   HMdcGarMacroMaker::nfield     [4]={4 ,4 ,4 ,4 };

Float_t HMdcGarMacroMaker::dc_s       [4]={0.25,0.25,0.4,0.5};
Float_t HMdcGarMacroMaker::cathodx    [4]={0.9 ,0.9 ,1.8 ,2.4};
Float_t HMdcGarMacroMaker::sensex     [4]={0.5 ,0.6 ,1.2 ,1.4};
Float_t HMdcGarMacroMaker::fieldx     [4]={0.75,0.9 ,1.8 ,2.1};

Float_t HMdcGarMacroMaker::area    [4][4]={{-0.30,-0.30, 0.30, 0.30},
                                           {-0.30,-0.30, 0.30, 0.30},
                                           {-0.91,-0.41, 0.91, 0.41},
					   {-0.81,-0.52, 0.81, 0.52}};

Int_t   HMdcGarMacroMaker::cwire      [4]={43,43,52,52};
Int_t   HMdcGarMacroMaker::cwire_foil [4]={6,6,6,6};
Float_t HMdcGarMacroMaker::time_up    [4]={0.3,0.3,0.5,0.5};
Float_t HMdcGarMacroMaker::time_low   [4]={0.0,0.0,0.0,0.0};

Float_t HMdcGarMacroMaker::B_components[3]={0.0 ,0.0 ,0.0};

/*
Int_t   HMdcGarMacroMaker::maxLoop[4][18]={{25,26,26,26,27,28,29,31,33,36,33,31,29,28,27,26,26,26},
                                           {30,31,31,32,32,32,35,37,40,37,34,32,31,29,28,27,27,27},
                                           {60,61,61,63,64,67,70,70,63,57,53,49,47,45,43,42,41,41},
					   {70,71,72,73,75,78,81,86,78,71,66,62,58,56,54,52,51,51}};

Int_t   HMdcGarMacroMaker::minLoop[4][18]={{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                                           {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                                           {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
					   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};
                                           */

Int_t   HMdcGarMacroMaker::minLoop[4][18]={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                           {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                           {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

Int_t   HMdcGarMacroMaker::maxLoop[4][18]={{25,28,29,31,33,34,35,35,36,36,36,35,35,34,33,31,29,28},
                                           {30,33,35,36,38,39,39,40,40,40,40,39,38,37,35,33,31,29},
					   {60,64,67,69,71,72,72,73,72,71,70,68,65,62,59,55,50,46},
					   {70,75,78,81,83,85,86,87,86,85,84,82,79,75,71,67,62,56 }};

Double_t  HMdcGarMacroMaker::maxstep[4] = {0.001 ,0.001 ,0.003,0.003};
Double_t  HMdcGarMacroMaker::mc_dist[4] = {0.0005,0.0005,0.001,0.001};


HMdcGarMacroMaker::HMdcGarMacroMaker(const Char_t* name,const Char_t* title)
    : TNamed(name,title)
{
    // constructor for HMdcGarMacroMaker
    // The parameters are initialized with non valid values.
    initVariables();
}
HMdcGarMacroMaker::~HMdcGarMacroMaker()
{
  // destructor of HMdcGarMacroMaker
}
void HMdcGarMacroMaker::setArgon(Int_t ar)
{
   if (helium > 0)
   {
      Error("HMdcGarMacroMaker:setArgon()","ADMIXTURE OF HELIUM NOT ALLOWED!");
      exit(1);
   }
   if (ar + (ibutane > 0 ) ? ibutane : co2 > 100)
   {
      Error("HMdcGarMacroMaker:setArgon()","GAX MIXTURE EXCEEDS 100%%!");
      exit(1);
   }
   argon = ar;
}
void HMdcGarMacroMaker::setHelium(Int_t he)
{
   if (argon > 0)
   {
      Error("HMdcGarMacroMaker:setHelium()","ADMIXTURE OF ARGON NOT ALLOWED!");
      exit(1);
   }
   if (he + ibutane > 100)
   {
      Error("HMdcGarMacroMaker:setHelium()","GAX MIXTURE EXCEEDS 100%%!");
      exit(1);
   }
   helium = he;
}
void HMdcGarMacroMaker::setIButane(Int_t b)
{
   if ((helium  ?  helium  :  argon) + ibutane > 100)
   {
      Error("HMdcGarMacroMaker:setIButane()","GAX MIXTURE EXCEEDS 100%%!");
      exit(1);
   }
   ibutane = b;
}

void   HMdcGarMacroMaker::setCO2(Int_t co){

   if (helium > 0)
   {
      Error("HMdcGarMacroMaker:setCO2()","ADMIXTURE OF HELIUM NOT ALLOWED!");
      exit(1);
   }
   if(argon + ibutane > 100)
   {
      Error("HMdcGarMacroMaker:setCO2()","GAX MIXTURE EXCEEDS 100%%!");
      exit(1);
   }
   co2 = co;
}
void   HMdcGarMacroMaker::setArgonCO2(Int_t ar,Int_t co)
{
    ibutane = 0;
    helium  = 0;
    argon   = ar;
    co2     = co;

    if(argon + co2 > 100)
    {
	Error("HMdcGarMacroMaker:setArgonCO2()","GAX MIXTURE EXCEEDS 100%%!");
	exit(1);
    }
}
void   HMdcGarMacroMaker::setArgonIButane(Int_t ar,Int_t ib)
{
    ibutane = ib;
    helium  = 0;
    argon   = ar;
    co2     = 0;

    if(argon + ibutane > 100)
    {
	Error("HMdcGarMacroMaker:setArgonIButane()","GAX MIXTURE EXCEEDS 100%%!");
	exit(1);
    }
}
void   HMdcGarMacroMaker::setHeliumIButane(Int_t he,Int_t ib)
{
    ibutane = ib;
    helium  = he;
    argon   = 0;
    co2     = 0;

    if(he + ibutane > 100)
    {
	Error("HMdcGarMacroMaker:setHeliumIButane()","GAX MIXTURE EXCEEDS 100%%!");
	exit(1);
    }
}
void HMdcGarMacroMaker::setFileNameOut(TString myfile)
{
    // Sets ascii output of HMdcGarMacroMaker
    fNameAsciiOut=myfile;
    if(fNameAsciiOut.CompareTo("")==0)
    {
	Error("HMdcGarMacroMaker:setFileNameOut()","NO OUTPUT FILE SEPCIFIED!");
        exit(1);
    };
    cout<<"HMdcGarMacroMaker::setFileNameOut(): OUTPUT FILE= "<<fNameAsciiOut.Data()<<endl;
}
void HMdcGarMacroMaker::setFileNameSignals(TString myfile)
{
    // Sets ascii output of HMdcGarMacroMaker
    fNameSignalOut=myfile;
    if(fNameSignalOut.CompareTo("")==0)
    {
	Error("HMdcGarMacroMaker:setFileNameSignals()","NO OUTPUT FILE SEPCIFIED!");
        exit(1);
    };
    cout<<"HMdcGarMacroMaker::setFileNameSignals(): OUTPUT FILE= "<<fNameSignalOut.Data()<<endl;
}
void HMdcGarMacroMaker::setFileNameGasFile(TString myfile)
{
    // Sets ascii output of HMdcGarMacroMaker
    fNameGasIn=myfile;
    if(fNameGasIn.CompareTo("")==0)
    {
	Error("HMdcGarMacroMaker:setFileNameGasFile()","NO OUTPUT FILE SEPCIFIED!");
        exit(1);
    };
    cout<<"HMdcGarMacroMaker::setFileNameGasFile(): OUTPUT FILE= "<<fNameGasIn.Data()<<endl;
}
void HMdcGarMacroMaker::setPsFileName(TString myfile)
{
    // Sets ascii output of HMdcGarMacroMaker
    if(myfile.CompareTo("")==0)
    {
	Warning("HMdcGarMacroMaker:setPsFileName()","NO OUTPUT PSFILE SPECIFIED, FILE WILL BE TRACK.PS!");
    }
    else
    {
	postscriptfile=myfile;
    }
    postscript=kTRUE;
}
void HMdcGarMacroMaker::setPrintSetup(TString myset)
{
    if(myset.CompareTo("")==0)
    {
	Error("HMdcGarMacroMaker::setPrintSetup()","NO PRINT SETUP SPECIFIED!");
        exit(1);
    }
    parseArguments(myset);
}
void HMdcGarMacroMaker::setBFieldComponents(Float_t bx,Float_t by,Float_t bz)
{
    B_components[0]=bx;
    B_components[1]=by;
    B_components[2]=bz;
}

void HMdcGarMacroMaker::parseArguments(TString s1)
{
    // The option string is searched for known arguments and
    // the internal variables of the class are defined according
    // to the arguments.
    // If unknown arguments are found error messages will be displayed
    // and the program will be exited.
    // Not found options will be switched to kFALSE.

    // switch print key words
    TString scell    ="cell";
    TString soption  ="option";
    TString sgas     ="gas";
    TString sfield   ="field";
    TString sdrift   ="drift";
    TString ssignal  ="signal";
    TString s3d      ="3d";

    Int_t icell    =0;
    Int_t ioption  =0;
    Int_t igas     =0;
    Int_t ifield   =0;
    Int_t idrift   =0;
    Int_t isignal  =0;
    Int_t i3d      =0;

    TString s2=s1;  // make copy of string

    s1.ToLower();
    s1.ReplaceAll(" ","");
    Ssiz_t len = s1.Length();
    if(len != 0)
    {
	TObjArray* myarguments = s1.Tokenize(",");
	TObjString *stemp;
	TString argument;
	TIterator* myiter=myarguments->MakeIterator();

	// iterate over the lis of arguments and compare the
        // to known key words.
	while ((stemp=(TObjString*)myiter->Next())!= 0)
	{
	    argument=stemp->GetString();
	    if(argument.CompareTo(scell)==0)
	    {
                icell++;
		if(icell>1)
		{
		    Error("HMdcGarMacroMaker:parseArguments()","Multiple appearence of same arguent!");
		    cout<<"INPUT SETUPSTRING: "<<s2<<endl;
                    printKeyWords();
		    exit(1);
		}

		printcell=kTRUE;
	    }
            else if(argument.CompareTo(soption)==0)
	    {
                ioption++;
		if(ioption>1)
		{
		    Error("HMdcGarMacroMaker:parseArguments()","Multiple appearence of same arguent!");
		    cout<<"INPUT SETUPSTRING: "<<s2<<endl;
		    printKeyWords();
		    exit(1);
		}
		printoption=kTRUE;
	    }
            else if(argument.CompareTo(sgas)==0)
	    {
                igas++;
		if(igas>1)
		{
		    Error("HMdcGarMacroMaker:parseArguments()","Multiple appearence of same arguent!");
		    cout<<"INPUT SETUPSTRING: "<<s2<<endl;
		    printKeyWords();
		    exit(1);
		}
		printgas=kTRUE;
	    }
            else if(argument.CompareTo(sfield)==0)
	    {
                ifield++;
		if(ifield>1)
		{
		    Error("HMdcGarMacroMaker:parseArguments()","Multiple appearence of same arguent!");
		    cout<<"INPUT SETUPSTRING: "<<s2<<endl;
		    printKeyWords();
		    exit(1);
		}
		printfield=kTRUE;
	    }
            else if(argument.CompareTo(sdrift)==0)
	    {
                idrift++;
		if(idrift>1)
		{
		    Error("HMdcGarMacroMaker:parseArguments()","Multiple appearence of same arguent!");
		    cout<<"INPUT SETUPSTRING: "<<s2<<endl;
		    printKeyWords();
		    exit(1);
		}
		printdrift=kTRUE;
	    }
            else if(argument.CompareTo(ssignal)==0)
	    {
                isignal++;
		if(isignal>1)
		{
		    Error("HMdcGarMacroMaker:parseArguments()","Multiple appearence of same arguent!");
		    cout<<"INPUT SETUPSTRING: "<<s2<<endl;
		    printKeyWords();
		    exit(1);
		}
		printsignal=kTRUE;
	    }
            else if(argument.CompareTo(s3d)==0)
	    {
                i3d++;
		if(i3d>1)
		{
		    Error("HMdcGarMacroMaker:parseArguments()","Multiple appearence of same arguent!");
		    cout<<"INPUT SETUPSTRING: "<<s2<<endl;
		    printKeyWords();
		    exit(1);
		}
		print3d=kTRUE;
	    }
	    else
	    {
		Error("HMdcGarMacroMaker:parseArguments()","\n unknown argument %s !",argument.Data());
		cout<<"INPUT SETUPSTRING: "<<s2<<endl;
		printKeyWords();
		exit(1);
	    }
	}

	myarguments->Delete();
	delete myarguments;
        delete myiter;
    }
    if(icell==0)  printcell=kFALSE;
    if(ioption==0)printoption=kFALSE;
    if(igas==0)   printgas=kFALSE;
    if(ifield==0) printfield=kFALSE;
    if(idrift==0) printdrift=kFALSE;
    if(isignal==0)printsignal=kFALSE;
    if(i3d==0)    print3d=kFALSE;
}
void HMdcGarMacroMaker::initVariables()
{
    // inits all variables
    fNameAsciiOut      ="";
    fNameSignalOut     ="";
    fNameGasIn         ="";
    output             =0;
    postscript         =kFALSE;         // print pictures to ps
    postscriptfile     ="Track.ps";     // file name of ps output file
    mdc                =0;
    temperature        =293;
    pressure           =760;
    helium             =0;
    argon              =0;
    ibutane            =0;
    co2                =0;
    mobility           =1;
    avalanche          =44130;
    collisions         =500;
    trap_radius        = 1.01;
    particle           ="electron";
    energy             =0.7;
    range              =7.;
    timebin            =0.001;
    nTimeBin           =1000;
    angleDeg           =0;
    angleStep          =0;
    angleStepSize      =5.;
    nSignals           =100;
    writeData          =kFALSE;
    isochrones         =0.005;
    nLines             =100;
    printclusters=kFALSE;
    printcell    =kTRUE;
    printoption  =kTRUE;
    printgas     =kTRUE;
    printfield   =kTRUE;
    printdrift   =kTRUE;
    printsignal  =kTRUE;
    print3d      =kTRUE;
    cathode_foils=kFALSE;
    doRungeKutta =kTRUE;
}
void HMdcGarMacroMaker::printKeyWords()
{
    // Prints the known keywords to the screen
    printf ("--------------------------------------------------------------------------------------------\n");
    printf ("HMdcGarMacroMaker::setPrintSetup() OPTIONS: cell, option, gas, field, drift, signal, 3d\n");
}
void HMdcGarMacroMaker::printStatus(void)
{
    // prints the parameters to the screen
    printKeyWords();
    printf ("--------------------------------------------------------------------------------------------\n");
    printf ("HMdcGarMacroMaker:\n");
    printf ("Print Setup:      = ");
    if(printcell)  printf ("cell");
    if(printoption)printf (", option");
    if(printgas)   printf (", gas");
    if(printfield) printf (", field");
    if(printdrift) printf (", drift");
    if(printsignal)printf (", signal");
    if(print3d)    printf (", 3d");
    printf ("\n");
    printf ("Macro output file = %s\n",fNameAsciiOut.Data());
    if(postscript)printf ("PS output file    =%s\n",postscriptfile.Data());
    printf ("MDC TYPE          = %i\n",mdc);
    printf ("Impact angle      = %3.1f degree\n",angleDeg);
    if(cathode_foils)
    {
	printf ("Cathod foils are used\n");
    }
    if(!cathode_foils)
    {
	printf ("Cathod wires are used\n");
    }
    printf ("HV cathods        = %i V (GSI), %i V (DUBNA), %i V (FZR), %i V (ORRSAY),\n"
	    ,cathodhv[0],cathodhv[1],cathodhv[2],cathodhv[3]);
    printf ("HV field          = %i V (GSI), %i V (DUBNA), %i V (FZR), %i V (ORRSAY),\n"
	    ,fieldhv[0] ,fieldhv[1] ,fieldhv[2] ,fieldhv[3]);
    printf ("HV sense          = %i V (GSI), %i V (DUBNA), %i V (FZR), %i V (ORRSAY),\n"
	    ,sensehv[0] ,sensehv[1] ,sensehv[2] ,sensehv[3]);
    if (argon > 0 && ibutane > 0)
    {
       printf ("Gasmixture        = Ar/i-Butane(%i/%i), temperature %i K, pressure %i torr\n"
	       ,argon,ibutane,temperature,pressure);
    }
    else if (argon > 0 && co2 > 0)
    {
       printf ("Gasmixture        = Ar/CO2(%i/%i), temperature %i K, pressure %i torr\n"
	       ,argon,co2,temperature,pressure);
    }
    else if (helium > 0 && ibutane > 0)
    {
       printf ("Gasmixture        = He/i-Butane(%i/%i), temperature %i K, pressure %i torr\n"
	       ,helium,ibutane,temperature,pressure);
    } else {
	Error("HMdcGarMacroMaker:printStatus()","Unknown gas mixture!");
        exit(1);
    }

    printf ("Track Conditions  = %s, energy %3.2f GeV,\n \
	            avalanche %i, range %f cm, collisions %i\n"
	    ,particle.Data(),energy,avalanche,range,collisions);
    printf ("Signals           = %i Signals per point, %5.3f ns timebin\n",nSignals,timebin);
    printf ("Dataset flag      = %i\n",writeData);
    printf ("Isochrones        = %5.3f, nLines= %i\n",isochrones,nLines);
    printf ("--------------------------------------------------------------------------------------------\n");
}
void HMdcGarMacroMaker::putOut(const Char_t* buffer)
{
    // Prints the buffer to the screen and to the ascii output file
    printf("%s\n",buffer);
    if(output)fprintf(output,"%s",buffer);
}
void HMdcGarMacroMaker::openOutPut()
{
    // Opens the output file
    if(fNameAsciiOut.CompareTo("")!=0)
    {
	output=fopen(fNameAsciiOut.Data(),"w");
    }
    else
    {
	Error("HMdcGarMacroMaker::openOutPut()","NO OUTPUT SPECIFIED!");
	exit(1);
    }
}
void HMdcGarMacroMaker::closeOutPut()
{
    // Close the output file
    if(output)fclose(output);
}
void HMdcGarMacroMaker::printMacro()
{
    // Print the macro to the screen and the ascii file output
    printStatus();
    openOutPut();
    printMetaHeader();
    printCell();
    printOptions();
    printGeometry();
    printField();
    printMagnetic();
    printGas();
    printDrift();
    printSignal();
    print3D();
    printMetaTrailer();
    closeOutPut();
}
void HMdcGarMacroMaker::printMetaHeader()
{
    // Prints the Metafile Header of the macro
    if(postscript)
    {
	Char_t buffer[2048];
	sprintf(buffer,"*****************HEADER*********************\n"); putOut(buffer);
	sprintf(buffer,"!opt linear-x\n");                                putOut(buffer);
	sprintf(buffer,"!add meta type PostScript file-name \"%s\"\n"
		,postscriptfile.Data());                                  putOut(buffer);
        sprintf(buffer,"!open meta\n");                                   putOut(buffer);
	sprintf(buffer,"!act meta\n");                                    putOut(buffer);
	sprintf(buffer,"!reset-colours\n");                               putOut(buffer);
	sprintf(buffer,"\n");                                             putOut(buffer);
    }
}
void HMdcGarMacroMaker::printCell()
{
    // Prints the cell Id
    if(printcell)
    {
	Char_t buffer[2048];
	sprintf(buffer,"*****************CELL***********************\n");  putOut(buffer);
	sprintf(buffer,"&cell\n");                                         putOut(buffer);
	sprintf(buffer,"cell-id \"+=cathode,x=potential,o=sense\"\n");     putOut(buffer);
	sprintf(buffer,"\n");                                              putOut(buffer);
    }
}
void HMdcGarMacroMaker::printOptions()
{
    // Print some options
    if(printoption)
    {
	Char_t buffer[2048];
	sprintf(buffer,"*****************OPTIONS********************\n");  putOut(buffer);
	sprintf(buffer,"reset planes\n");                                  putOut(buffer);
	sprintf(buffer,"reset rows\n");                                    putOut(buffer);
	if(printclusters||postscript)
	{
	    sprintf(buffer,"opt layout\n");                                    putOut(buffer);
	    sprintf(buffer,"opt wire-markers\n");                              putOut(buffer);
	    sprintf(buffer,"opt cell-print\n");                                putOut(buffer);
	}
	sprintf(buffer,"\n");                                              putOut(buffer);
    }
}
void HMdcGarMacroMaker::printGeometry()
{
    // Print the Geometry of the drift cell
    Char_t buffer[2048];
    sprintf(buffer,"*****************GEOMETRY*******************\n");  putOut(buffer);
    if(!cathode_foils)
    {   // if cathode wires are used
	sprintf(buffer,"ROWS\n");                                          putOut(buffer);
	sprintf(buffer,"*label wire# thickness pitch x y voltage MDC%i\n"
		,mdc);                                                     putOut(buffer);
	sprintf(buffer,"c %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		ncathod[mdc],cathodth[mdc],cathodpitch[mdc],cathodx[mdc]
		, 3*dc_s[mdc],cathodhvmod[0]==0? cathodhv[mdc]:cathodhvmod[0]); putOut(buffer);
	sprintf(buffer,"p %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		nfield[mdc] ,fieldth[mdc] ,fieldpitch[mdc] ,fieldx[mdc]
		, 2*dc_s[mdc],fieldhvmod[0]==0? fieldhv[mdc]:fieldhvmod[0]);    putOut(buffer);
	sprintf(buffer,"S %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		nsense[mdc] ,senseth[mdc] ,sensepitch[mdc] ,sensex[mdc]
		, 2*dc_s[mdc],sensehvmod[0]==0? sensehv[mdc]:sensehvmod[0]);    putOut(buffer);
	sprintf(buffer,"c %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		ncathod[mdc],cathodth[mdc],cathodpitch[mdc],cathodx[mdc]
		,-1*dc_s[mdc],cathodhvmod[1]==0? cathodhv[mdc]:cathodhvmod[1]); putOut(buffer);
        sprintf(buffer,"c %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		ncathod[mdc],cathodth[mdc],cathodpitch[mdc],cathodx[mdc]
		, 1*dc_s[mdc],cathodhvmod[2]==0? cathodhv[mdc]:cathodhvmod[2]); putOut(buffer);
	sprintf(buffer,"p %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		nfield[mdc] ,fieldth[mdc] ,fieldpitch[mdc] ,fieldx[mdc]
		, 0*dc_s[mdc],fieldhvmod[1]==0? fieldhv[mdc]:fieldhvmod[1]);    putOut(buffer);
        sprintf(buffer,"S %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		nsense[mdc] ,senseth[mdc] ,sensepitch[mdc] ,sensex[mdc]
		, 0*dc_s[mdc],sensehvmod[1]==0? sensehv[mdc]:sensehvmod[1]);    putOut(buffer);
        sprintf(buffer,"c %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		ncathod[mdc],cathodth[mdc],cathodpitch[mdc],cathodx[mdc]
		,-3*dc_s[mdc],cathodhvmod[3]==0? cathodhv[mdc]:cathodhvmod[3]); putOut(buffer);
	sprintf(buffer,"p %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		nfield[mdc] ,fieldth[mdc] ,fieldpitch[mdc] ,fieldx[mdc]
		,-2*dc_s[mdc],fieldhvmod[2]==0? fieldhv[mdc]:fieldhvmod[2]);    putOut(buffer);
        sprintf(buffer,"S %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		nsense[mdc] ,senseth[mdc] ,sensepitch[mdc] ,sensex[mdc]
		,-2*dc_s[mdc],sensehvmod[2]==0? sensehv[mdc]:sensehvmod[2]);    putOut(buffer);

    }
    else
    { // if cathode foils are used
	sprintf(buffer,"plane y=%5.2f ,V=%5i\n",
		-1*dc_s[mdc],cathodhvmod[2]==0? cathodhv[mdc]:cathodhvmod[2]);  putOut(buffer);
        sprintf(buffer,"plane y=%5.2f ,V=%5i\n",
		1*dc_s[mdc],cathodhvmod[2]==0? cathodhv[mdc]:cathodhvmod[2]);   putOut(buffer);
	sprintf(buffer,"ROWS\n");                                          putOut(buffer);
	sprintf(buffer,"*label wire# thickness pitch x y voltage MDC%i\n"
		,mdc);                                                     putOut(buffer);
	sprintf(buffer,"p %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		nfield[mdc] ,fieldth[mdc] ,fieldpitch[mdc] ,fieldx[mdc]
		, 0*dc_s[mdc],fieldhvmod[1]==0? fieldhv[mdc]:fieldhvmod[1]);    putOut(buffer);
        sprintf(buffer,"S %2i %4.3f  (%3.2f*I)-%3.2f %5.2f %5i\n",
		nsense[mdc] ,senseth[mdc] ,sensepitch[mdc] ,sensex[mdc]
		, 0*dc_s[mdc],sensehvmod[1]==0? sensehv[mdc]:sensehvmod[1]);    putOut(buffer);

    }

    sprintf(buffer,"\n");                                              putOut(buffer);
}
void HMdcGarMacroMaker::printField()
{
    // Print the field section of the macro
    if(printfield)
    {
	Char_t buffer[2048];
	sprintf(buffer,"*****************FIELD**********************\n");  putOut(buffer);
	sprintf(buffer,"&field\n");                                        putOut(buffer);
	sprintf(buffer,"track FROM 0. 1. TO 0. -1.\n");                    putOut(buffer);
	sprintf(buffer,"grid 40 40\n");                                    putOut(buffer);
	sprintf(buffer,"*xmin ymin xmax ymax\n");                          putOut(buffer);
	if(postscript)
	{
	    if(mdc==0){sprintf(buffer,"area -0.32 -0.27 0.32 0.27\n");      putOut(buffer);}
	    if(mdc==1){sprintf(buffer,"area -0.32 -0.28 0.32 0.28\n");      putOut(buffer);}
	    if(mdc==2){sprintf(buffer,"area -0.62 -0.42 0.62 0.42\n");      putOut(buffer);}
	    if(mdc==3){sprintf(buffer,"area -0.82 -0.62 0.82 0.62\n");      putOut(buffer);}
	    sprintf(buffer,"*-------------------------------------------\n");  putOut(buffer);
	    sprintf(buffer,"plot  contour e N 50 NOLABEL\n");              putOut(buffer);
	    sprintf(buffer,"plot  contour v N 50 NOLABEL\n");              putOut(buffer);
	    sprintf(buffer,"plot  surface e\n");                           putOut(buffer);
	    sprintf(buffer,"plot  graph e\n");                             putOut(buffer);
	    sprintf(buffer,"plot  graph v\n");                             putOut(buffer);
	    if(writeData)
	    {
		sprintf(buffer,"*-------------------------------------------\n");  putOut(buffer);
		sprintf(buffer,"> graph_e_mdc%i.dat\n",mdc);               putOut(buffer);
		sprintf(buffer,"plot graph e N 200 print\n");              putOut(buffer);
		sprintf(buffer,">\n");                                     putOut(buffer);
	    }
	}
	sprintf(buffer,"\n");                                              putOut(buffer);
    }
}
void HMdcGarMacroMaker::printMagnetic()
{
    // Prints the magnetic field settings
    if(printcell)
    {
	Char_t buffer[2048];
	sprintf(buffer,"*****************MAGNETIC*******************\n");  putOut(buffer);
	sprintf(buffer,"&magnetic\n");                                     putOut(buffer);
	sprintf(buffer,"components %5.2f %5.2f %5.2f T \n",
	       B_components[0],B_components[1],B_components[2]);           putOut(buffer);
	sprintf(buffer,"\n");                                              putOut(buffer);
    }
}

void HMdcGarMacroMaker::printGas()
{
    // Print the Gas section of the macro
    if(printgas)
    {
	Char_t name[300];
	if (argon > 0 && ibutane > 0)
	{
	   sprintf(name,"ar_ibutane%i-%i-%i-%i.dat",argon,ibutane,temperature,pressure);
	}
        else if (argon > 0 && co2 > 0)
	{
	   sprintf(name,"ar_co2%i-%i-%i-%i.dat",argon,co2,temperature,pressure);
	}
        else if (helium > 0 && ibutane > 0)
	{
	   sprintf(name,"he_ibutane%i-%i-%i-%i.dat",helium,ibutane,temperature,pressure);
	} else {

	    Error("printGas()","Unknown gas mixture!");
            exit(1);
	}

	if(fNameGasIn.CompareTo("")==0)fNameGasIn=name;

	Char_t buffer[2048];
	sprintf(buffer,"*****************GAS************************\n");  putOut(buffer);
	sprintf(buffer,"&gas\n");                                          putOut(buffer);
	if (argon > 0 && ibutane > 0)
	{
	   sprintf(buffer,"gas-id \"ar/iso-butane %i/%i\"\n"
		   ,argon,ibutane);                                        putOut(buffer);
	}
	else if (argon > 0 && co2 > 0)
	{
	   sprintf(buffer,"gas-id \"ar/co2 %i/%i\"\n"
		   ,argon,co2);                                            putOut(buffer);
	}
	else if (helium > 0 && ibutane > 0)
	{
	   sprintf(buffer,"gas-id \"he/iso-butane %i/%i\"\n"
		   ,helium,ibutane);                                       putOut(buffer);
	}
	sprintf(buffer,"global gas_file `%s`\n"
		,fNameGasIn.Data());                                       putOut(buffer);
	sprintf(buffer,"temperature %i\n"
		,temperature);                                             putOut(buffer);
	sprintf(buffer,"pressure %i\n"
		,pressure);                                                putOut(buffer);
	sprintf(buffer,"Call inquire_file(gas_file,exist)\n");             putOut(buffer);
	sprintf(buffer,"If exist Then\n");                                 putOut(buffer);
	sprintf(buffer,"Say \"Gas file exists, retrieving ...\"\n");       putOut(buffer);
	sprintf(buffer,"get {gas_file}\n");                                putOut(buffer);
	sprintf(buffer,"Else\n");                                          putOut(buffer);
	sprintf(buffer,"Say \"Gas file not found, generating ...\"\n");    putOut(buffer);
	if (argon > 0 && ibutane > 0)
	{
	   sprintf(buffer,"magboltz argon %i. isobutane %i. mobility %f\n"
		   ,argon,ibutane,mobility);                               putOut(buffer);
	   sprintf(buffer,"write dataset \"%s\" gasdata remark \"magboltz-ar%i-ibutane%i\"\n"
		   ,fNameGasIn.Data(),argon,ibutane);                      putOut(buffer);
	   sprintf(buffer,"Endif\n");                                      putOut(buffer);
	   sprintf(buffer,"heed argon %i. isobutane %i.\n"
		   ,argon,ibutane);                                        putOut(buffer);
	}
	else if (argon > 0 && co2 > 0)
	{
	   sprintf(buffer,"magboltz argon %i. carbon-dioxide %i. mobility %f\n"
		   ,argon,co2,mobility);                                   putOut(buffer);
	   sprintf(buffer,"write dataset \"%s\" gasdata remark \"magboltz-ar%i-co2%i\"\n"
		   ,fNameGasIn.Data(),argon,co2);                          putOut(buffer);
	   sprintf(buffer,"Endif\n");                                      putOut(buffer);
	   sprintf(buffer,"heed argon %i. carbon-dioxide %i.\n"
		   ,argon,co2);                                        putOut(buffer);
	}
	else if (helium > 0 && ibutane > 0)
	{
	   sprintf(buffer,"magboltz helium-4 %i. isobutane %i. mobility %f\n"
		   ,helium,ibutane,mobility);                              putOut(buffer);
	   sprintf(buffer,"write dataset \"%s\" gasdata remark \"magboltz-he%i-ibutane%i\"\n"
		   ,fNameGasIn.Data(),helium,ibutane);                     putOut(buffer);
	   sprintf(buffer,"Endif\n");                                      putOut(buffer);
	   sprintf(buffer,"heed helium-4 %i. isobutane %i.\n"
		   ,helium,ibutane);                                       putOut(buffer);
	}

	sprintf(buffer,"\n");                                              putOut(buffer);
    }
}
void HMdcGarMacroMaker::printDrift()
{
    // Print the Drift section of the macro
    if(printdrift)
    {
	Char_t buffer[2048];
	sprintf(buffer,"*****************DRIFT**********************\n");  putOut(buffer);
	sprintf(buffer,"&drift\n");                                        putOut(buffer);
	if(!cathode_foils)
	{
	sprintf(buffer,"SEL %i\n"
		,cwire[mdc]);                                              putOut(buffer);
	}
        if(cathode_foils)
	{
	sprintf(buffer,"SEL %i\n"
		,cwire_foil[mdc]);                                              putOut(buffer);
	}

	if(postscript)
	{
	    if(mdc==0){sprintf(buffer,"area -0.32 -0.27 0.32 0.27\n");     putOut(buffer);}
	    if(mdc==1){sprintf(buffer,"area -0.32 -0.28 0.32 0.28\n");     putOut(buffer);}
	    if(mdc==2){sprintf(buffer,"area -0.62 -0.42 0.62 0.42\n");     putOut(buffer);}
	    if(mdc==3){sprintf(buffer,"area -0.82 -0.62 0.82 0.62\n");     putOut(buffer);}
	    sprintf(buffer,"*-------------------------------------------\n");  putOut(buffer);
	    sprintf(buffer,"plot  contour VD N 50 NOLABEL\n");             putOut(buffer);
	    sprintf(buffer,"plot  surface VD \n");                         putOut(buffer);
	    sprintf(buffer,"xt-plot\n");                                   putOut(buffer);
	    if(writeData)
	    {
		sprintf(buffer,"xt-plot DATASET \"xtmdc%i.dat\" REMARK \"xt\" \n",
			mdc);                                              putOut(buffer);
	    }
	    sprintf(buffer,"DRIFT WIRES ANGLES 0 360 LINES %i ISOCHRONES= %5.3f\n"
		    ,nLines,isochrones);                                   putOut(buffer);
	    if(writeData)
	    {
		sprintf(buffer,"WRITE-ISOCHRONES DATASET \"VDmdc%i.dat\" contVD remark \"isochrones mdc%i\" \n",
			mdc,mdc);                                          putOut(buffer);
	    }
	    sprintf(buffer,"*************Driftvelocity vs E/P***********\n");  putOut(buffer);
	    sprintf(buffer,"*-------------------------------------------\n");  putOut(buffer);
	    sprintf(buffer,"Call get_gas_data(p,t,id)\n");                     putOut(buffer);
	    sprintf(buffer,"Call get_E/p_table(ep)\n");                        putOut(buffer);
	    sprintf(buffer,"Call drift_velocity(0,0,ep*p,drift)\n");           putOut(buffer);
	    sprintf(buffer,"!opt log-x\n");                                    putOut(buffer);
	    sprintf(buffer,"Call plot_graph(ep,drift,`E/p [kV/cm bar]`,`Drift velocity [cm/microsec]`,`Drift velocity vs E/p`)\n"); putOut(buffer);
	    sprintf(buffer,"Call plot_markers(ep,drift,`function-1`)\n");      putOut(buffer);
	    sprintf(buffer,"Call plot_comment(`up-left`,`Magboltz: `/id)\n");  putOut(buffer);
	    sprintf(buffer,"Call plot_end \n");                                putOut(buffer);
	    sprintf(buffer,"!opt linear-x \n");                                putOut(buffer);
	    sprintf(buffer,"*-------------------------------------------\n");  putOut(buffer);
	}
	sprintf(buffer,"\n");                                              putOut(buffer);
    }
}
void HMdcGarMacroMaker::printSignal()
{
    // Print the Signal section of the macro
    if(printsignal)
    {
	if(fNameSignalOut.CompareTo("")==0)
	{
	    Char_t nameout[300];
            sprintf(nameout,"/tmp/s%i-mdc%i.txt",(Int_t)angleDeg,mdc);
	    fNameSignalOut=nameout;
	}
	Char_t buffer[2048];
	sprintf(buffer,"*****************SIGNAL*********************\n");  putOut(buffer);
	sprintf(buffer,"&signal\n");                                       putOut(buffer);
	sprintf(buffer,"*Track starting from left side\n");                putOut(buffer);
	sprintf(buffer,"area %5.2f %5.2f %5.2f %5.2f\n",
		area[mdc][0],area[mdc][1],area[mdc][2],area[mdc][3]);      putOut(buffer);
	if(!cathode_foils)
	{
	sprintf(buffer,"SELECT %i\n"
		,cwire[mdc]);
	                                                                   putOut(buffer);
	}
	if(cathode_foils)
	{
	sprintf(buffer,"SELECT %i\n"
		,cwire_foil[mdc]);
	                                                                   putOut(buffer);
	}
	sprintf(buffer,"AVALANCHE fixed %i\n"
		,avalanche);                                               putOut(buffer);
	sprintf(buffer,"INTEGRATION-PARAMETERS  MONTE-CARLO-COLLISIONS %i M-C-DIST-INT %f MAX-STEP %f INT-ACC 1.e-10 PROJECTED TRAP-RADIUS %f\n"
		,collisions,mc_dist[mdc],maxstep[mdc],trap_radius);        putOut(buffer);
	if(!printclusters)
	{
	    sprintf(buffer,"OPTIONS NOCLUSTER-PLOT NOCLUSTER-PRINT\n");    putOut(buffer);
	}
        sprintf(buffer,"****************Loop over distance**********\n");  putOut(buffer);
        sprintf(buffer,"For j From %i To %i Do\n"
		,minLoop[mdc][angleStep]+1
		,maxLoop[mdc][angleStep]);                                 putOut(buffer);
        sprintf(buffer,"*-------------------------------------------\n");  putOut(buffer);
	sprintf(buffer,"Global r= 0.01*j\n");                              putOut(buffer);
	sprintf(buffer,"Global alphaDEG=%3.1f\n"
		,angleDeg);                                                putOut(buffer);
        sprintf(buffer,"Global alphaRAD=pi/180.*alphaDEG\n");              putOut(buffer);
	sprintf(buffer,"Global dx=0.1\n");                                 putOut(buffer);
	sprintf(buffer,"Global dy=dx*tan(pi/2.+alphaRAD)\n");              putOut(buffer);
	sprintf(buffer,"Global x1=r*cos(alphaRAD)\n");                     putOut(buffer);
	sprintf(buffer,"Global y1=r*sin(alphaRAD)\n");                     putOut(buffer);
	sprintf(buffer,"Global x=(%5.2f-(y1-x1*tan(pi/2.+alphaRAD)))/tan(pi/2.+alphaRAD)\n"
		,dc_s[mdc]);                                               putOut(buffer);
        sprintf(buffer,"Global y= %5.2f\n"
		,dc_s[mdc]);                                               putOut(buffer);
	sprintf(buffer,"TRACK FROM {x} {y} DIRECTION {dx} {dy}  RANGE %f HEED %s energy %5.2f GeV\n"
		,range,particle.Data(),energy);                            putOut(buffer);
        sprintf(buffer,"*-------------------------------------------\n");  putOut(buffer);
	sprintf(buffer,"TIME-WINDOW %3.1f %5.3f %i\n"
		,time_low[mdc],timebin,nTimeBin);                          putOut(buffer);
        sprintf(buffer,"****************Loop over signal************\n");  putOut(buffer);
	sprintf(buffer,"For i From 1 To %i  Do\n"
		,nSignals);                                                putOut(buffer);

      	if(doRungeKutta){
	    sprintf(buffer,"SIGNAL AVALANCHE NOELECTRON-PULSE ION-TAIL RUNGE-KUTTA-DRIFT-LINES\n");
	}else{
	    sprintf(buffer,"SIGNAL AVALANCHE NOELECTRON-PULSE ION-TAIL MONTE-CARLO-DRIFT-LINES\n");
	}
	putOut(buffer);
	sprintf(buffer,"WRITE-SIGNALS DATASET \"%s\" REMARK \"p {j} n {i}\" UNITS NANO-SECOND\n",
		fNameSignalOut.Data());                                    putOut(buffer);
	if(postscript)
	{
	    if(!cathode_foils)
	    {
		sprintf(buffer,"PLOT-SIGNALS  TIME-WINDOW %3.1f %3.1f WIRE %i\n"
			,time_low[mdc],time_up[mdc],cwire[mdc]);
		putOut(buffer);
	    }
            if(cathode_foils)
	    {
		sprintf(buffer,"PLOT-SIGNALS  TIME-WINDOW %3.1f %3.1f WIRE %i\n"
			,time_low[mdc],time_up[mdc],cwire_foil[mdc]);
		putOut(buffer);
	    }

	}
	sprintf(buffer,"enddo\n");                                         putOut(buffer);
	sprintf(buffer,"****************End Loop over signal*********\n"); putOut(buffer);
	sprintf(buffer,"enddo\n");                                         putOut(buffer);
	sprintf(buffer,"****************End Loop over distance*******\n"); putOut(buffer);
	sprintf(buffer,"\n");                                              putOut(buffer);
    }
}
void HMdcGarMacroMaker::printMetaTrailer()
{
    // Print the Metafiel Trailer of the macro
    if(postscript)
    {
	Char_t buffer[2048];
	sprintf(buffer,"*****************TRAILER*********************\n"); putOut(buffer);
	sprintf(buffer,"!deact meta\n");                                   putOut(buffer);
	sprintf(buffer,"!close meta\n");                                   putOut(buffer);
	sprintf(buffer,"!del meta\n");                                     putOut(buffer);
	sprintf(buffer,"\n");                                              putOut(buffer);
    }
}
void HMdcGarMacroMaker::print3D()
{
    // Print some 3d plot of the drift cell
    if(print3d)
    {
	Char_t buffer[2048];
	sprintf(buffer,"*****************3D*************************\n");                                             putOut(buffer);
	sprintf(buffer,"*-------------------------------------------\n");                                             putOut(buffer);
	sprintf(buffer,"* cathods\n");                                                                                putOut(buffer);
	sprintf(buffer,"solids\n");                                                                                   putOut(buffer);
	sprintf(buffer,"For x From -%5.3f Step %5.3f To %5.3f Do\n"
		,cathodx[mdc],cathodpitch[mdc],ncathod[mdc]*cathodpitch[mdc]);                                        putOut(buffer);
        sprintf(buffer,"cylinder CENTRE {x}  %5.2f 0 RADIUS %5.4f HALF-LENGTH 1 DIRECTION 0 0 1 conductor-1\n"
		, 3*dc_s[mdc],0.5*cathodth[mdc]);                                                                     putOut(buffer);
        sprintf(buffer,"cylinder CENTRE {x}  %5.2f 0 RADIUS %5.4f HALF-LENGTH 1 DIRECTION 0 0 1 conductor-1\n"
		, 1*dc_s[mdc],0.5*cathodth[mdc]);                                                                     putOut(buffer);
        sprintf(buffer,"cylinder CENTRE {x}  %5.2f 0 RADIUS %5.4f HALF-LENGTH 1 DIRECTION 0 0 1 conductor-1\n"
		,-1*dc_s[mdc],0.5*cathodth[mdc]);                                                                     putOut(buffer);
        sprintf(buffer,"Enddo\n");                                                                                    putOut(buffer);
	sprintf(buffer,"*-------------------------------------------\n");                                             putOut(buffer);
	sprintf(buffer,"* potential ordered  -20,-40\n");                                                             putOut(buffer);
	sprintf(buffer,"solids\n");                                                                                   putOut(buffer);
	sprintf(buffer,"For x From -%5.3f Step %5.3f To %5.3f Do\n"
		,fieldx[mdc],fieldpitch[mdc],nfield[mdc]*fieldpitch[mdc]);                                            putOut(buffer);
        sprintf(buffer,"cylinder CENTRE {x}  %5.2f 0 RADIUS %5.4f HALF-LENGTH 1 DIRECTION 1 0 -%f conductor-2\n"
		, 2*dc_s[mdc],0.5*fieldth[mdc],tan((90-20)*TMath::Pi()/180));                                         putOut(buffer);
        sprintf(buffer,"cylinder CENTRE {x}  %5.2f 0 RADIUS %5.4f HALF-LENGTH 1 DIRECTION 1 0 -%f conductor-2\n"
		, 0*dc_s[mdc],0.5*fieldth[mdc],tan((90-40)*TMath::Pi()/180));                                         putOut(buffer);
        sprintf(buffer,"Enddo\n");                                                                                    putOut(buffer);
	sprintf(buffer,"*-------------------------------------------\n");                                             putOut(buffer);
	sprintf(buffer,"*sense ordered  -20,-40\n");                                                                  putOut(buffer);
	sprintf(buffer,"For x From -%5.3f Step %5.3f To %5.3f Do\n"
		,sensex[mdc],sensepitch[mdc],nsense[mdc]*sensepitch[mdc]);                                            putOut(buffer);
        sprintf(buffer,"cylinder CENTRE {x}  %5.2f 0 RADIUS %5.4f HALF-LENGTH 1 DIRECTION 1 0 -%f conductor-2\n"
		, 2*dc_s[mdc],0.5*senseth[mdc],tan((90-20)*TMath::Pi()/180));                                         putOut(buffer);
        sprintf(buffer,"cylinder CENTRE {x}  %5.2f 0 RADIUS %5.4f HALF-LENGTH 1 DIRECTION 1 0 -%f conductor-2\n"
		, 0*dc_s[mdc],0.5*senseth[mdc],tan((90-40)*TMath::Pi()/180));                                         putOut(buffer);
        sprintf(buffer,"Enddo\n");                                                                                    putOut(buffer);
        sprintf(buffer,"\n");                                                                                         putOut(buffer);
    }
}
