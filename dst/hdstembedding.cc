/**
 * @file   hdstembedding.cc
 * @author Simon Lang
 * @date   Fri May  5 16:07:11 2006
 * 
 * @brief  This Class provides Functions for a DST Production from 
 *         real Data with an admixture of simulated Tracks
 * 
 * $Id: hdstembedding.cc,v 1.4 2009-09-09 14:13:37 jurkovic Exp $
 *
 */

// Root headers
#include "TObjString.h"
#include "TSystem.h"
#include "TUrl.h"

// Hydra headers
#include "hades.h"
#include "hruntimedb.h"
#include "hdstembedding.h"
#include "hldfilesource.h"
#include "hrootsource.h"
#include "htool.h"
#include "hdstutilities.h"

#include "hrichunpacker.h"
#include "hmdcunpacker.h"
#include "htoftrb2unpacker.h"
#include "hshowerunpacker.h"
#include "hstart2trb2unpacker.h"
#include "hwalltrb2unpacker.h"
#include "hrpctrb2unpacker.h"
#include "hlatchunpacker.h"
#include "htboxunpacker.h"

#include "hqamaker.h"
#include "hparticlecandfiller.h"
#include "hparticletrackcleaner.h"
#include "hparticleevtinfofiller.h"
#include "hmdcsetup.h"
#include "hmdcbitflipcor.h"

#include "htaskset.h"
#include "hstarttaskset.h"
#include "htoftaskset.h"
#include "hrichtaskset.h"
#include "hmdctaskset.h"
#include "hshowertaskset.h"
#include "hwalltaskset.h"
#include "hrpctaskset.h"
#include "hsplinetaskset.h"

#ifdef RFIO_SUPPORT
#  include "hldrfiofilesource.h"
#endif

using namespace std;



//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//  Base Class for a DST Production from simulated Data.
//
////////////////////////////////////////////////////////////////////////////////
ClassImp(HDstEmbedding)



    HDstEmbedding::HDstEmbedding() : HDstProduction()
{
    // Default Constructor: default unpacker IDs set

    // these are the default unpacker subevent ID's (derived from Sep05)
    mdcUnpackers.Set(2);
    mdcUnpackers[0] = 0x8100;
    mdcUnpackers[1] = 0x8110;

    richUnpackers.Set(3);
    richUnpackers[0] = 0x8300;
    richUnpackers[1] = 0x8310;
    richUnpackers[2] = 0x8320;

    rpcUnpackers.Set(2);
    rpcUnpackers[0] = 0x8400;
    rpcUnpackers[1] = 0x8410;

    showerUnpackers.Set(1);
    showerUnpackers[0] = 0x8500;

    tofUnpackers.Set(1);
    tofUnpackers[0] = 0x8600;


    wallUnpackers.Set(1);
    wallUnpackers[0] = 0x8700;

    startUnpackers.Set(1);
    startUnpackers[0] = 0x8800;

    latchUnpackers.Set(1);
    latchUnpackers[0] = 0x361;

    tboxUnpackers.Set(1);
    tboxUnpackers [0] = 0x361;
}



void HDstEmbedding::setupInput()
{
    // Setup first and second data input file for track embedding: This function
    // determines and opens one or more .hld and .root input files of real data
    // and HGeant output data and adds those files as data sources. All
    // necessary information are provided by dstCfg structure.
    // This function supports only local file locations for simulation data.

    HRootSource* root_source = NULL;
    HldSource*   hld_source  = NULL;
    TObjArray*   files       = NULL;
    TIterator*   file        = NULL;
    TObjString*  filename    = NULL;
    TObjArray*   local_files = NULL;
    TIterator*   local_file  = NULL;
    TString      protocol;
    TString      directory;

    // switch analysis to embedding mode
    gHades->setEmbeddingMode( 1 );

    if (dstCfg.expId.IsNull())
    {
	Error( "setupInput", "Experiment ID not set!" );
	terminate( 2 );
    }


    ////// add HLD data source


    files = dstCfg.inputUrl.Tokenize( " \t" );
    file  = files->MakeIterator();
    file->Reset();
    while ((filename = (TObjString*)(file->Next())))
    {
	TUrl url( filename->GetString().Data(), kTRUE );

	if (protocol.IsNull())
	{
	    protocol = url.GetProtocol();
	}
	else if (protocol.CompareTo( url.GetProtocol() ) != 0)
	{
	    Error( "setupInput", "Only one type of input protocol allowed!" );
	    terminate( 2 );
	}

	if (protocol.CompareTo( "file" ) == 0)
	{
	    if (!hld_source)
	    {
		hld_source = new HldFileSource;
		((HldFileSource*)hld_source)->setDirectory(
							   const_cast<Text_t*>(gSystem->DirName( url.GetFile() )) );
		directory = gSystem->DirName( url.GetFile() );
	    }

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

		if (dstCfg.referenceRunId)
		{
		    ((HldFileSource*)hld_source)->addFile(
							  const_cast<Text_t*>(gSystem->BaseName(
												filename->GetString().Data() )),
							  dstCfg.referenceRunId );
		}
		else
		{
		    ((HldFileSource*)hld_source)->addFile(
							  const_cast<Text_t*>(gSystem->BaseName(
												filename->GetString().Data() )) );
		}
	    }
	    delete local_file;
	    delete local_files;
	}
#ifdef RFIO_SUPPORT
	else if (protocol.CompareTo( "rfio" ) == 0)
	{
	    if (!hld_source)
	    {
		TString location;
		hld_source = new HldRFIOFileSource;

		// extract robot host and archive name
		((HldRFIOFileSource*)hld_source)->setRobot( url.GetHost() );
		location = url.GetFile();
		location.Remove( TString::kLeading, '/' );
		location.Resize( location.Index( '/' ) );
		((HldRFIOFileSource*)hld_source)->setArchive( location.Data() );

		// remove archive name and extract directory and file name
		location = url.GetFile();
		location.Remove( TString::kLeading, '/' );
		location.Replace( 0, location.Index( '/' ) + 1, "", 0 );
		((HldRFIOFileSource*)hld_source)->setDirectory(
							       const_cast<Text_t*>(gSystem->DirName( location.Data() )) );
	    }

	    ((HldRFIOFileSource*)hld_source)->addFile(
						      const_cast<Text_t*>(gSystem->BaseName( url.GetFile() )) );
	}
#endif // RFIO_SUPPORT
	else
	{
	    Error( "setupInput", "Invalid input URL: %s", url.GetUrl() );
	    terminate( 2 );
	}
    }
    delete file;

    if (dstCfg.richIsUsed) {
	for(Int_t i = 0; i < richUnpackers.GetSize(); i++) {
	    HRichUnpacker* richUnp = new HRichUnpacker( richUnpackers[i]);
	    hld_source->addUnpacker( richUnp );
	}
    }

    if  (dstCfg.mdcIsUsed) {
	for(Int_t i = 0; i < mdcUnpackers.GetSize(); i++) {
	    HMdcUnpacker* mdcUnp =  new HMdcUnpacker( mdcUnpackers[i], kFALSE );
	    mdcUnp->setQuietMode(kTRUE);
	    hld_source->addUnpacker( mdcUnp );
	}
    }

    if (dstCfg.showerIsUsed) {
	for(Int_t i = 0; i < showerUnpackers.GetSize(); i++) {
	    HShowerUnpacker* showerUnp = new HShowerUnpacker( showerUnpackers[i] );
	    hld_source->addUnpacker( showerUnp );
	}
    }

    if (dstCfg.tofIsUsed) {
	for(Int_t i = 0; i < tofUnpackers.GetSize(); i++) {
	    HTofTrb2Unpacker* tofUnp = new HTofTrb2Unpacker( tofUnpackers[i] );
	    tofUnp->setDebugFlag(0);
	    tofUnp->setQuietMode();
	    tofUnp->removeTimeRef();
	    tofUnp->shiftTimes(6200.);
	    hld_source->addUnpacker( tofUnp  );
	}
    }


    if (dstCfg.rpcIsUsed) {
	for(Int_t i=0; i< rpcUnpackers.GetSize(); i++){
	    HRpcTrb2Unpacker *rpcTrb2Unpacker = new HRpcTrb2Unpacker(rpcUnpackers[i]);
	    rpcTrb2Unpacker->setQuietMode();
	    hld_source->addUnpacker(rpcTrb2Unpacker);
	}
    }


    if (dstCfg.wallIsUsed) {
	for(Int_t i=0; i< wallUnpackers.GetSize(); i++) {
	    HWallTrb2Unpacker* wallUnpacker=new HWallTrb2Unpacker(wallUnpackers[i]);
	    wallUnpacker->setQuietMode();
	    wallUnpacker->setQuietMode();
	    wallUnpacker->removeTimeRef();
	    wallUnpacker->shiftTimes(6200.);
	    hld_source->addUnpacker( wallUnpacker);
	}
    }

    if (dstCfg.startIsUsed) {
	for(Int_t i = 0; i < startUnpackers.GetSize(); i++) {
	    HStart2Trb2Unpacker *startUnp = new HStart2Trb2Unpacker(startUnpackers[i]);
	    startUnp->setDebugFlag(0);
	    startUnp->setQuietMode();
	    hld_source->addUnpacker( startUnp );
	}
    }

    if (dstCfg.triggerIsUsed) {
	for(Int_t i = 0; i < latchUnpackers.GetSize(); i++) {
	    hld_source->addUnpacker( new HLatchUnpacker( latchUnpackers[i] ) );
	}
	for(Int_t i = 0; i < tboxUnpackers.GetSize(); i++) {
	    hld_source->addUnpacker( new HTBoxUnpacker( tboxUnpackers[i] ) );
	}
    }


    gHades->setDataSource( hld_source );


    ////// add Root data source


    files = dstCfg.embeddingInput.Tokenize( " \t" );
    file  = files->MakeIterator();
    file->Reset();
    while ((filename = (TObjString*)file->Next()))
    {
	TUrl url( filename->GetString().Data(), kTRUE );

	if (!root_source)
	{
	    root_source = new HRootSource( kTRUE, kTRUE );
	    root_source->setDirectory(
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

		if (!root_source->addFile(
					  const_cast<Text_t*>(gSystem->BaseName(
										filename->GetString().Data() )) ))
		{
		    Error( "setupInput","Adding input file '%s' failed!",
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

    gHades->setSecondDataSource( root_source );
}



void HDstEmbedding::setupTaskList()
{
    // The default task list for DST production from real data where simulated
    // tracks are embedded into the data stream.
    // All actions and definitions for a specific beam time must
    // go to a derived class, where other task list can be
    // implemented.

    HStartTaskSet*   start_task_set    = new HStartTaskSet();
    HTofTaskSet*     tof_task_set      = new HTofTaskSet();
    HRichTaskSet*    rich_task_set     = new HRichTaskSet();
    HMdcTaskSet*     mdc_task_set      = new HMdcTaskSet();
    HShowerTaskSet*  shower_task_set   = new HShowerTaskSet();
    HRpcTaskSet*     rpc_task_set      = new HRpcTaskSet();
    HWallTaskSet*    wall_task_set     = new HWallTaskSet();
    HSplineTaskSet*  spline_task_set   = new HSplineTaskSet( "", "" );

    HTaskSet* mdc_bit_flip_correction_task = new HTaskSet( "mdc bitflipcor", "mdc bitflipcor" );
    mdc_bit_flip_correction_task->connect( new HMdcBitFlipCor( "mdcbitflipcor", "mdcbitflipcor" ) );

    // this includes the following tasks:
    // Spline Tracking, Spline Clustering,
    // Runge-Kutta Tracking, MetaMatch Procedure
    HTask* tracking_tasks = spline_task_set->make("", "spline,tofclust,runge" );

    //-----------------------------------------------------------------------
    HParticleCandFiller   *pParticleCandFiller = new HParticleCandFiller  ("particlecandfiller","particlecandfiller","");
    HParticleTrackCleaner *pParticleCleaner    = new HParticleTrackCleaner("particlecleaner"   ,"particlecleaner");
    HParticleEvtInfoFiller *pParticleEvtInfo   = new HParticleEvtInfoFiller("particleevtinfo"  ,"particleevtinfo");
    //-----------------------------------------------------------------------

    HQAMaker* qa_report_task = new HQAMaker( "qa.maker", "qa.maker" );
    qa_report_task->setOutputDir( const_cast<char*>(dstCfg.qaOutputDir.Data()));
    qa_report_task->setSamplingRate( 1 );
    qa_report_task->setIntervalSize( dstCfg.qaEventInterval );

    ////// now link together the task lists

    HTaskSet* tasks = gHades->getTaskSet( "real" );

    tasks->add( start_task_set  ->make(""    , "controlhists" ) );
    tasks->add( tof_task_set    ->make("real", "clust" ) );
    tasks->add( rich_task_set   ->make("real", "") );
    tasks->add( shower_task_set ->make() );
    tasks->add( rpc_task_set    ->make("real") );
    tasks->add( wall_task_set   ->make("real") );
    tasks->add( mdc_bit_flip_correction_task );
    tasks->add( mdc_task_set    ->make( "rtdb" ) );
    tasks->add( tracking_tasks );
    tasks->add( pParticleCandFiller);
    tasks->add( pParticleCleaner);
    tasks->add( pParticleEvtInfo);
    tasks->add( qa_report_task );

    tasks->isTimed( kTRUE );
}
