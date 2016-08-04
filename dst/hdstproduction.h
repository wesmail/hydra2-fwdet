/**
 * @file   hdstproduction.h
 * @author Simon Lang
 * @date   Thu Oct  5 13:58:07 2006
 * 
 * @brief  Base class of DST Production Classes
 * 
 * $Id: hdstproduction.h,v 1.12 2009-02-04 17:41:07 jurkovic Exp $
 *
 */

#ifndef HDSTPRODUCTION_H
#define HDSTPRODUCTION_H


// Root include files
#include "TObject.h"
// no forward declaration here, since derived classes might not use TString
#include "TString.h"

// forward declarations
class HTaskSet;


class HDstProduction : public TObject
{
  public:
      class DstConfiguration_t
      {
	 public:
	    Int_t triggerModules[1];   // we have just one trigger system
	    Int_t startModules[10];    // start, veto, ?, ?, ?, ? detector
	    Int_t richModules[1];      // whole RICH detector is one module
	    Int_t mdcModules[6][4];    // 6 chambers (sectors) per plane
	    Int_t showerModules[3];    // 3 cell layers in shower
	    Int_t tofModules[22];      // 22 x 8 stripes assemble our TOF
	    Int_t rpcModules[1];       // yet unused
	    Int_t wallModules[1];      // yet unused

	    Bool_t triggerIsUsed;      // set to kTRUE, if any related module
	    Bool_t startIsUsed;        //    of a detector is used
	    Bool_t richIsUsed;
	    Bool_t mdcIsUsed;
	    Bool_t showerIsUsed;
	    Bool_t tofIsUsed;
	    Bool_t rpcIsUsed;
	    Bool_t wallIsUsed;
	    
	    TString dstId;
	    TString expId;
	    TString inputUrl;
	    TString embeddingInput;
	    TString outputDir;
	    TString outputFile;
	    TString qaOutputDir;
	    TString notPersistentCategories;
	    TString disabledCategories;
	    TString firstParameterSource;
	    TString secondParameterSource;
	    TString historyDate;
	    TString parameterOutputFile;

	    Int_t referenceRunId;
	    Int_t startWithEvent;
	    Int_t qaEventInterval;
	    Int_t eventCounterInterval;
	    Int_t processNumEvents;

      };

   protected:
      // we use a large buffer for Root type output trees
      static const Int_t TREE_BUFFER_SIZE;
      static Bool_t isBatch;
      DstConfiguration_t dstCfg;

   public:
      HDstProduction();
      ~HDstProduction();

      const DstConfiguration_t& getConfiguration() { return dstCfg; }
      void configure(Int_t argc, Char_t** argv);
      void printConfiguration();
      void printHelp();
      void setupSpectrometer();
      void setupParameterSources();
      void setupParameterOutput();
      void setupAllParameterOutput();
      void setupRootInput();
      void setupOutput();
      void initialize();
      void runEventLoop();
      void finalize();
      void terminate(Int_t exit_code);
      static void setBatchMode(Bool_t batch) { isBatch = batch; }
      
      ClassDef( HDstProduction, 0 ) // Base class of DST Production Classes
};


#endif   // HDSTPRODUCTION_H
