/**
 * @file   hdstsimulation.h
 * @author Simon Lang
 * @date   Fri May  5 15:26:25 2006
 * 
 * @brief  This Class provides Functions for a DST Production from 
 *         simulated Data
 * 
 * $Id: hdstsimulation.h,v 1.2 2006-10-05 12:57:29 slang Exp $
 *
 */

#ifndef HDSTSIMULATION_H
#define HDSTSIMULATION_H


#include "hdstproduction.h"


class HDstSimulation : public HDstProduction
{
   public:
      HDstSimulation() : HDstProduction() {};
      ~HDstSimulation() {};

      void setupInput();
      void setupTaskList();
      
      ClassDef( HDstSimulation, 0 ) // Base class used for DST Production from simulated Data
};


#endif   // HDSTSIMULATION_H
