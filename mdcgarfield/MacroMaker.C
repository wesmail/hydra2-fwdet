#include "hmdcgarmacromaker.h"
void MacroMaker(Int_t mdc,Int_t angle,Char_t macroname[])
{

    //gROOT->Macro("/misc/kempter/mydir/batch/macros/rootlogon.C");
    Int_t cathodhv[4]={-1750,-1800,-2000,-2200};
    Int_t fieldhv [4]={-1750,-1800,-2000,-2200};
    Int_t maxLoop[4][18]={{25,28,29,31,33,34,35,35,36,36,36,35,35,34,33,31,29,28},
                          {30,33,35,36,38,39,39,40,40,40,40,39,38,37,35,33,31,29},
			  {60,64,67,69,71,72,72,73,72,71,70,68,65,62,59,55,50,46},
			  {70,75,78,81,83,85,86,87,86,85,84,82,79,75,71,67,62,56 }};

    Int_t  minLoop[4][18]={{25,26,26,26,27,28,29,31,33,36,33,31,29,28,27,26,26,26},
                           {30,31,31,32,32,32,35,37,40,37,34,32,31,29,28,27,27,27},
			   {60,61,61,63,64,67,70,70,63,57,53,49,47,45,43,42,41,41},
			   {70,71,72,73,75,78,81,86,78,71,66,62,58,56,54,52,51,51}};


    HMdcGarMacroMaker *macro=new HMdcGarMacroMaker();
    macro->setFileNameOut(macroname);
    macro->setPrintSetup("cell,option,gas,field,drift,signal");

    macro->setMdcType(mdc);
    macro->setAngle(angle);  // degree  : 0->perpendicular track
    macro->setCathodHv(&cathodhv[0]);
    macro->setFieldHv (&fieldhv [0]);
    //macro->setPsFileName("test.ps");// switch on ps-mode
    macro->setWriteDataSets(kFALSE);
    macro->setNLines(100);
    macro->setIsochrones(0.005);
    //macro->setMaxLoop(&maxLoop[0][0]);
    macro->setMinLoop(&minLoop[0][0]);
    macro->setNSignals(100); // 100

    macro->printMacro();

    Char_t command[300];
    sprintf(command,"%s%s","mygarfield7big -batch >/dev/null 2>&1 < ",macroname);
    cout<<"processing garfield: "<<command<<endl;

    gSystem->Exec(command);

}