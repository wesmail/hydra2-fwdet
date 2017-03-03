//*-- Author  : Georgy Kornakov
//*-- Created : 27.01.2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetTaskSet
//
//  This class contains the tasks for the Forward Detector
//
/////////////////////////////////////////////////////////////

#include "hfwdettaskset.h"
#include "hdebug.h"
#include "haddef.h"
#include "hades.h"
#include "hfwdetstrawdigitizer.h"
#include "hfwdetscindigitizer.h"
#include "hfwdetrpcdigitizer.h"
#include "hfwdetrpchitfinder.h"
#include "hfwdetvectorfinder.h"
#include <iostream>
#include <iomanip>
#include "TObjString.h"

ClassImp(HFwDetTaskSet);

HFwDetTaskSet::HFwDetTaskSet() : HTaskSet()
{
    // Default constructor
    doStrawRaw = kFALSE;
    doStrawCal = kFALSE;
    doScinRaw  = kFALSE;
    doScinCal  = kFALSE;
    doRpcRaw   = kFALSE;
    doRpcCal   = kFALSE;
    doRpcHitF  = kFALSE;
    doVectorFinder = kFALSE;
}

HFwDetTaskSet::HFwDetTaskSet(const Text_t name[], const Text_t title[]) :
    HTaskSet(name, title)
{
    // Constructor
    doStrawRaw = kFALSE;
    doStrawCal = kFALSE;
    doScinRaw  = kFALSE;
    doScinCal  = kFALSE;
    doRpcRaw   = kFALSE;
    doRpcCal   = kFALSE;
    doRpcHitF  = kFALSE;
    doVectorFinder = kFALSE;
}

HFwDetTaskSet::~HFwDetTaskSet()
{
}

void HFwDetTaskSet::parseArguments(TString s)
{
    // parses arguments (straw, scin, rpc) to the make tasks function

    s.ToLower();
    s.ReplaceAll(" ", "");

    Ssiz_t len = s.Length();

    if (len!=0)
    {
        TObjArray* myarguments = s.Tokenize(",");
        TObjString *stemp = 0;
        TString argument;
        TIterator* myiter = myarguments->MakeIterator();
        // go over list of arguments and compare the known keys

        while ((stemp=(TObjString*)myiter->Next()) != 0)
        {
            argument=stemp->GetString();
            Info("parseArguments()", "option: %s", argument.Data());

            if (argument.CompareTo("strawraw") == 0)
            {
                doStrawRaw  = kTRUE;
            }
            else if (argument.CompareTo("strawcal") == 0)
            {
                doStrawCal  = kTRUE;
                doStrawRaw  = kTRUE;
            }
            else if( argument.CompareTo("scinraw") == 0)
            {
                doScinRaw   = kTRUE;
            }
            else if( argument.CompareTo("scincal") == 0)
            {
                doScinRaw   = kTRUE;
                doScinCal   = kTRUE;
            }
            else if (argument.CompareTo("rpcraw") == 0)
            {
                doRpcRaw    = kTRUE;
            }
            else if (argument.CompareTo("rpccal") == 0)
            {
                doRpcRaw    = kTRUE;
                doRpcCal    = kTRUE;
            }
            else if (argument.CompareTo("rpchitf") == 0)
            {
                doRpcHitF    = kTRUE;
            }
            else if (argument.CompareTo("vf") == 0)
            {
                doVectorFinder = kTRUE;
            }
            else
            {
                Error("parseArguments()", "Unknown option = %s", argument.Data());
            }
        }
        delete myiter;

        myarguments->Delete();
        delete myarguments;
    }
}

HTask* HFwDetTaskSet::make(const Char_t *select, const Option_t *option)
{
    // Returns a pointer to the FwDet task or taskset specified by 'select'
    // OPTIONS: see parseArguments()

    HTaskSet *tasks = new HTaskSet("FwDet", "List of FwDet tasks");
    TString simulation = "simulation";
    TString real = "real";

    TString sel = select;
    TString opt = option;

    sel.ToLower();
    opt.ToLower();
    parseArguments(opt);

    if(sel.CompareTo(simulation) == 0 || gHades->getEmbeddingMode() > 0)
    {
        if (doStrawCal)
            tasks->add(new HFwDetStrawDigitizer("fwdetstraw.digi", "fwdetstraw.digi"));
        if (doScinCal)
            tasks->add(new HFwDetScinDigitizer("fwdetscin.digi", "fwdetscin.digi"));
        if (doRpcCal)
            tasks->add(new HFwDetRpcDigitizer("fwdetrpc.digi", "fwdetrpc.digi"));
        if (doRpcCal)
            tasks->add(new HFwDetRpcHitFinder("fwdetrpc.hitf", "fwdetrpc.hitf"));

        if(doVectorFinder)
            tasks->add(new HFwDetVectorFinder("fwdet.vf", "fwdet.vf"));

    }
    if(sel.CompareTo(real) == 0 || gHades->getEmbeddingMode() > 0)
    {
        if (doStrawRaw)
        {
//             tasks->add(new HFwDetStrawCalibrator("fwdetstraw.cal", "fwdetstraw.cal")); // TODO
//             if(doStraw)
//                 tasks->add(new HFwDetVectorFinder("vectorFinder", "vectorFinder"));
        }
    }
    return tasks;
}
