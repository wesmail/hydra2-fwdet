/**
 * @file   hdstrealdata.h
 * @author Simon Lang
 * @date   Fri May  5 15:26:25 2006
 * 
 * @brief  This Class provides Functions for a DST Production from real Data
 * 
 * $Id: hdstrealdata.h,v 1.3 2007-02-24 17:46:00 jurkovic Exp $
 *
 */

#ifndef HDSTREALDATA_H
#define HDSTREALDATA_H


#include "hdstproduction.h"

#include "TArrayI.h"

class HDstRealData : public HDstProduction
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
      HDstRealData();
      ~HDstRealData() {};

      void setupInput();
      void setupTaskList();

      ClassDef( HDstRealData, 0 ) // Base class used for DST Production from real data
};


#endif   // HDSTREALDATA_H
