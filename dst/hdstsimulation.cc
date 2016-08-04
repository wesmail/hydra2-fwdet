/**
 * @file   hdstsimulationsep05.cc
 * @author Simon Lang
 * @date   Fri May  5 16:07:11 2006
 * 
 * @brief  This Class provides Functions for a DST Production from 
 *         simulated Data
 * 
 * $Id: hdstsimulation.cc,v 1.11 2009-02-26 17:30:40 jurkovic Exp $
 *
 */

// Root headers
#include "TObjString.h"
#include "TSystem.h"
#include "TUrl.h"

// Hydra headers
#include "hades.h"
#include "hruntimedb.h"
#include "hdstsimulation.h"
#include "hdstutilities.h"
#include "hrootsource.h"
#include "htool.h"

//#include "hrichevtfilter.h"  //???
#include "hqamaker.h"

#include "htaskset.h"
#include "htoftaskset.h"
#include "hrichtaskset.h"
#include "hmdctaskset.h"
#include "hshowertaskset.h"
#include "hsplinetaskset.h"

using namespace std;



//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//
//  Base Class for a DST Production from simulated Data.
//
////////////////////////////////////////////////////////////////////////////////
ClassImp(HDstSimulation)



void HDstSimulation::setupInput()
{
   // Setup data input file: This function determines and opens
   // one or more .root input files of HGeant output data. All necessary
   // information are provided by dstCfg structure.
   // This function supports only local file locations.

   HRootSource* source      = NULL;
   TObjArray*   files       = NULL;
   TIterator*   file        = NULL;
   TObjString*  filename    = NULL;
   TObjArray*   local_files = NULL; 
   TIterator*   local_file  = NULL;
   TString      directory;
   
   files = dstCfg.inputUrl.Tokenize( " \t" );
   file  = files->MakeIterator();
   file->Reset();
   while ((filename = (TObjString*)file->Next()))
   {
      TUrl url( filename->GetString().Data(), kTRUE );

      if (!source)
      {
	 source = new HRootSource;
	 source->replaceHeaderVersion( 0, kTRUE );
	 source->setGlobalRefId( dstCfg.referenceRunId );
	 source->setDirectory(
	    const_cast<Text_t*>(gSystem->DirName( url.GetFile() )) );
	 directory = gSystem->DirName( url.GetFile() );
      }
      if (strcmp( url.GetProtocol(), "file" ) == 0)
      {
	 local_files = HTool::glob( url.GetFile() );
	 if (!local_files)
	 {
	    Error( "setupInput", "Input file(s) do not exist!" );
	    terminate( 2 );
	 }
	 local_file = local_files->MakeIterator();
	 while ((filename = (TObjString*)local_file->Next()))
	 {
	    if (directory.CompareTo(
		   gSystem->DirName( filename->GetString().Data() ) ) != 0)
	    {
	       Error( "setupInput", "Only one input directory allowed!" );
	       terminate( 2 );
	    }
	    
	    if (!source->addFile(
		   const_cast<Text_t*>(gSystem->BaseName(
					  filename->GetString().Data() )) ))
	    {
	       Error( "setupInput", "Adding input file '%s' failed!",
					  filename->GetString().Data() );
	       terminate( 2 );
	    }
	 }
	 delete local_file;
	 delete local_files;
      }
      else
      {
	 Error( "setupInput", "Input URL must refer to a local file!" );
	 terminate( 2 );
      }
   }
   delete file;

   gHades->setDataSource( source );
}



void HDstSimulation::setupTaskList()
{
   // The default task list for DST production from simulated data.
   // All actions and definitions for a specific beam time must
   // go to a derived class, where other task list can be
   // implemented.

   HTofTaskSet*     tof_task_set        = new HTofTaskSet();
   HRichTaskSet*    rich_task_set       = new HRichTaskSet();
   HMdcTaskSet*     mdc_task_set        = new HMdcTaskSet();
   HShowerTaskSet*  shower_task_set     = new HShowerTaskSet();
   HSplineTaskSet*  spline_task_set     = new HSplineTaskSet( "", "" );

   // this includes the following tasks:
   // Spline Tracking, Spline Clustering,
   // Runge-Kutta Tracking, MetaMatch Procedure
   HTask* tracking_tasks = spline_task_set->make("", "spline,tofclust,simulation,runge" );

   // creation of quality assurance .pdf report files
   HQAMaker* qa_report_task = new HQAMaker( "qa.maker", "qa.maker" );
   qa_report_task->setOutputDir( const_cast<char*>(dstCfg.qaOutputDir.Data()));
   qa_report_task->setSamplingRate( 1 );
   qa_report_task->setIntervalSize( dstCfg.qaEventInterval );

   // Event Filter: Simulate 1st level trigger, using multiplicity of
   // TOF and TOFino hits (unusable for Pluto)

   // create control ntuple

   ////// now link together the task lists

   HTaskSet* tasks = gHades->getTaskSet( "simulation" );

   tasks->add( tof_task_set->make( "simulation" ) );
   tasks->add( rich_task_set->make( "simulation", "noiseon" ));
   tasks->add( mdc_task_set->make( "rtdb", "" ) );
   tasks->add( shower_task_set->make( "", "simulation,leprecognition" ) );
   tasks->add( tracking_tasks );
   tasks->add( qa_report_task );

   tasks->isTimed( kTRUE );
}
