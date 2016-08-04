/**
 * @file   hdstrealdata.h
 * @author Simon Lang
 * @date   Fri May  5 15:26:25 2006
 * 
 * @brief  This Class provides Functions for a DST Production from real Data
 * 
 * $Id: hdstembedding.h,v 1.1 2007-06-25 19:00:39 slang Exp $
 *
 */

#ifndef HDSTEMBEDDING_H
#define HDSTEMBEDDING_H


#include "hdstproduction.h"
#include "TArrayI.h"

class HDstEmbedding : public HDstProduction
{
   protected:
      TArrayI richUnpackers;
      TArrayI mdcUnpackers;
      TArrayI showerUnpackers;
      TArrayI tofUnpackers;
      TArrayI rpcUnpackers;
      TArrayI wallUnpackers;
      TArrayI startUnpackers;
      TArrayI latchUnpackers;
      TArrayI tboxUnpackers;

   public:
      HDstEmbedding();
      ~HDstEmbedding() {};

      void setupInput();
      void setupTaskList();

      ClassDef( HDstEmbedding, 0 ) // Base class used for DST Production from real data with admixture of simulated tracks
};


#endif   // HDSTEMBEDDING_H
