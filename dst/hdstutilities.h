/**
 * @file   hdstutilities.h
 * @author Simon Lang
 * @date   Fri May  5 15:26:25 2006
 * 
 * @brief  This Class provides Utility Functions for a DST Production
 * 
 * $Id: hdstutilities.h,v 1.4 2008-05-09 16:08:19 halo Exp $
 *
 */


#ifndef HDSTUTILITIES_H
#define HDSTUTILITIES_H


#include "TObject.h"


class HDstUtilities : public TObject
{
   private:
      static const Int_t MAX_LINE_LENGTH = 1024;

   private:
      HDstUtilities() {};

   public:
      ~HDstUtilities() {};

      static Bool_t readCmdLineArgsFromFile(
	 const Char_t* filename, Int_t* argc, Char_t*** argv);
      static void str2CmdLineArgs(const Char_t* input, Int_t* argc, Char_t*** argv);
      static void freeCmdLineArgs(Int_t argc, Char_t** argv);
      static Char_t* extractArg(Int_t* argc, Char_t*** argv, const Char_t* arg);
      static TString GetFileStem(const TString& dir, const TString& file);

   ClassDef( HDstUtilities, 0 ) // Utility Functions for a DST Production
};


#endif   // HDSTUTILITIES_H
