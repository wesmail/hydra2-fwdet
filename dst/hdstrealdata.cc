/**
 * @file   hdstrealdata.cc
 * @author Simon Lang
 * @date   Fri May  5 16:15:16 2006
 *
 * @brief  This Class provides Functions for a DST Production from real Data
 *
 * $Id: hdstrealdata.cc,v 1.14 2009-09-09 14:13:37 jurkovic Exp $
 *
 */

// Root headers
#include "TObjString.h"
#include "TSystem.h"
#include "TUrl.h"

// Hydra headers
#include "hades.h"
#include "htool.h"
#include "hdstrealdata.h"
#include "hdstutilities.h"

#include "hldfilesource.h"
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
#include "hrpctaskset.h"
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
//  Base Class for a DST Production from real Data.
//
////////////////////////////////////////////////////////////////////////////////
ClassImp(HDstRealData)



HDstRealData::HDstRealData() : HDstProduction()
{
    // Default Constructor: default unpacker IDs set

    // these are the default unpacker subevent ID's (derived from Sep05)



    mdcUnpackers.Set(12);
    mdcUnpackers[0] = 0x1100;
    mdcUnpackers[1] = 0x1110;
    mdcUnpackers[2] = 0x1120;
    mdcUnpackers[3] = 0x1130;
    mdcUnpackers[4] = 0x1140;
    mdcUnpackers[5] = 0x1150;
    mdcUnpackers[6] = 0x1000;
    mdcUnpackers[7] = 0x1010;
    mdcUnpackers[8] = 0x1020;
    mdcUnpackers[9] = 0x1030;
    mdcUnpackers[10] = 0x1040;
    mdcUnpackers[11] = 0x1050;

    richUnpackers.Set(3);
    richUnpackers[0] = 0x8300;
    richUnpackers[1] = 0x8310;
    richUnpackers[2] = 0x8320;

    rpcUnpackers.Set(2);
    rpcUnpackers[0] = 0x8400;
    rpcUnpackers[1] = 0x8410;

    showerUnpackers.Set(6);
    showerUnpackers[0] = 0x3200;
    showerUnpackers[1] =0x3210;
    showerUnpackers[2] =0x3220;
    showerUnpackers[3] =0x3230;
    showerUnpackers[4] =0x3240;
    showerUnpackers[5] =0x3250;

    tofUnpackers.Set(1);
    tofUnpackers[0] = 0x8600;


    wallUnpackers.Set(1);
    wallUnpackers[0] = 0x8700;

    startUnpackers.Set(1);
    startUnpackers[0] = 0x8800;

    latchUnpackers.Set(1);
    latchUnpackers[0] = 0x8800;

    tboxUnpackers.Set(1);
    tboxUnpackers [0] = 0x8800;
}



void HDstRealData::setupInput()
{
    // Setup data input file: This function determines and opens
    // one or more .hld input files of raw data. All necessary
    // information are provided by dstCfg structure.
    // This function supports local or remote (TSM) file
    // locations.

    HldSource*  source      = NULL;
    TObjArray*  files       = NULL;
    TIterator*  file        = NULL;
    TObjString* filename    = NULL;
    TObjArray*  local_files = NULL;
    TIterator*  local_file  = NULL;
    TString     protocol;
    TString     directory;

    if (dstCfg.expId.IsNull()) {
	Error( "setupInput", "Experiment ID not set!" );
	terminate( 2 );
    }

    files = dstCfg.inputUrl.Tokenize( " \t" );
    file  = files->MakeIterator();
    file->Reset();
    while ((filename = (TObjString*)(file->Next())))
    {
	TUrl url( filename->GetString().Data(), kTRUE );

	if (protocol.IsNull()) {
	    protocol = url.GetProtocol();
	} else if (protocol.CompareTo( url.GetProtocol() ) != 0) {
	    Error( "setupInput", "Only one type of input protocol allowed!" );
	    terminate( 2 );
	}

	if (protocol.CompareTo( "file" ) == 0)
	{
	    if (!source) {
		source = new HldFileSource;
		((HldFileSource*)source)->setDirectory(const_cast<Text_t*>(gSystem->DirName( url.GetFile() )) );
		directory = gSystem->DirName( url.GetFile() );
	    }

	    local_files = HTool::glob( url.GetFile() );
	    if (!local_files) {
		Error( "setupInput", "Input file(s) do not exist!" );
		terminate( 2 );
	    }
	    local_file = local_files->MakeIterator();
	    while ((filename = (TObjString*)local_file->Next()))
	    {
		if (directory.CompareTo(gSystem->DirName( filename->GetString().Data() ) ) != 0)
		{
		    Error( "setupInput", "Only one input directory allowed!" );
		    terminate( 2 );
		}

		if (dstCfg.referenceRunId) {
		    ((HldFileSource*)source)->addFile(const_cast<Text_t*>(gSystem->BaseName(filename->GetString().Data() )),dstCfg.referenceRunId );
		} else  {
		    ((HldFileSource*)source)->addFile(const_cast<Text_t*>(gSystem->BaseName(filename->GetString().Data() )) );
		}
	    }
	    delete local_file;
	    delete local_files;
	}
#ifdef RFIO_SUPPORT
	else if (protocol.CompareTo( "rfio" ) == 0) {
	    if (!source)
	    {
		TString location;
		source = new HldRFIOFileSource;

		// extract robot host and archive name
		((HldRFIOFileSource*)source)->setRobot( url.GetHost() );
		location = url.GetFile();
		location.Remove( TString::kLeading, '/' );
		location.Resize( location.Index( '/' ) );
		((HldRFIOFileSource*)source)->setArchive( location.Data() );

		// remove archive name and extract directory and file name
		location = url.GetFile();
		location.Remove( TString::kLeading, '/' );
		location.Replace( 0, location.Index( '/' ) + 1, "", 0 );
		((HldRFIOFileSource*)source)->setDirectory(const_cast<Text_t*>(gSystem->DirName( location.Data() )) );
	    }

	    ((HldRFIOFileSource*)source)->addFile(const_cast<Text_t*>(gSystem->BaseName( url.GetFile() )) );
	}
#endif // RFIO_SUPPORT
	else {
	    Error( "setupInput", "Invalid input URL: %s", url.GetUrl() );
	    terminate( 2 );
	}
    }
    delete file;

    if (dstCfg.richIsUsed) {
	for(Int_t i = 0; i < richUnpackers.GetSize(); i++) {
	    HRichUnpacker* richUnp = new HRichUnpacker( richUnpackers[i]);
	    source->addUnpacker( richUnp );
	}
    }

    if  (dstCfg.mdcIsUsed) {
	for(Int_t i = 0; i < mdcUnpackers.GetSize(); i++) {
	    HMdcUnpacker* mdcUnp =  new HMdcUnpacker( mdcUnpackers[i], kFALSE );
	    mdcUnp->setQuietMode(kTRUE);
	    source->addUnpacker( mdcUnp );
	}
    }

    if (dstCfg.showerIsUsed) {
	for(Int_t i = 0; i < showerUnpackers.GetSize(); i++) {
	    HShowerUnpacker* showerUnp = new HShowerUnpacker( showerUnpackers[i] );
	    source->addUnpacker( showerUnp );
	}
    }

    if (dstCfg.tofIsUsed) {
	for(Int_t i = 0; i < tofUnpackers.GetSize(); i++) {
	    HTofTrb2Unpacker* tofUnp = new HTofTrb2Unpacker( tofUnpackers[i] );
	    tofUnp->setDebugFlag(0);
	    tofUnp->setQuietMode();
	    tofUnp->removeTimeRef();
	    tofUnp->shiftTimes(7200.);
	    source->addUnpacker( tofUnp  );
	}
    }


    if (dstCfg.rpcIsUsed) {
	for(Int_t i=0; i< rpcUnpackers.GetSize(); i++){
	    HRpcTrb2Unpacker *rpcTrb2Unpacker = new HRpcTrb2Unpacker(rpcUnpackers[i]);
	    rpcTrb2Unpacker->setQuietMode();
	    rpcTrb2Unpacker->storeSpareChannelsData();
	    source->addUnpacker(rpcTrb2Unpacker);
	}
    }


    if (dstCfg.wallIsUsed) {
	for(Int_t i=0; i< wallUnpackers.GetSize(); i++) {
	    HWallTrb2Unpacker* wallUnpacker=new HWallTrb2Unpacker(wallUnpackers[i]);
	    wallUnpacker->setQuietMode();
	    wallUnpacker->removeTimeRef();
	    wallUnpacker->shiftTimes(8200.);
	    source->addUnpacker( wallUnpacker);
	}
    }

    if (dstCfg.startIsUsed) {
	for(Int_t i = 0; i < startUnpackers.GetSize(); i++) {
	    HStart2Trb2Unpacker *startUnp = new HStart2Trb2Unpacker(startUnpackers[i]);
	    startUnp->setDebugFlag(0);
	    startUnp->setQuietMode();
	    source->addUnpacker( startUnp );
	}
    }

    if (dstCfg.triggerIsUsed) {
	for(Int_t i = 0; i < latchUnpackers.GetSize(); i++) {
	    source->addUnpacker( new HLatchUnpacker( latchUnpackers[i] ) );
	}
	for(Int_t i = 0; i < tboxUnpackers.GetSize(); i++) {
	    source->addUnpacker( new HTBoxUnpacker( tboxUnpackers[i] ) );
	}
    }

    gHades->setDataSource( source );
}



void HDstRealData::setupTaskList()
{
    // The default task list for DST production from real data.
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
    HTask* tracking_tasks = spline_task_set->make("", "spline,doclustering,runge" );

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
