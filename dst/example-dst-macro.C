/**
 * @file   example-dst-macro.C
 * @author Simon Lang
 * @date   Fri Sep  8 09:35:56 2006
 * 
 * @brief  This file can be compiled or executed within CINT to produce
 *         a DST file of real data.
 *         The program parameter can be provides via a text configuration file 
 *         and/or on command line. If this function is called form CINT,
 *         then just provide the command line as a string argument.
 *         For details see example.cfg and the code of module "dst".
 *
 * $Id: example-dst-macro.C,v 1.2 2006-11-22 15:57:27 slang Exp $ 
 * 
 */


#ifndef __CINT__  // the following lines are compiled


#include "TROOT.h"
#include "hdstrealdata.h"
#include "hdstutilities.h"

int main(int argc, char **argv)
{
   TROOT             dst_production( "DstProductionApp", "Produce a DST File" );
   HDstRealDataSep05 real_data;

   real_data.configureDstProduction( argc, argv );


#else  // the following lines are interpreted by CINT


void DstRealData(TString command_line)
{
   int          argc = 0;
   char**       argv = NULL;
   HDstRealData real_data;

   HDstUtilities::str2CmdLineArgs( command_line.Data(), &argc, &argv );
   real_data.configure( argc, argv );
   HDstUtilities::freeCmdLineArgs( argc, argv );


#endif  // the following lines are used in any case


   real_data.printConfiguration();
   real_data.setupSpectrometer();
   real_data.setupInput();
   real_data.setupParameterSources();
   real_data.setupTaskList();
   real_data.initialize();
   real_data.setupOutput();
   real_data.runEventLoop();
   real_data.finalize();
}
