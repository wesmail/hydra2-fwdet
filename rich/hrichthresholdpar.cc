//////////////////////////////////////////////////////////////////////////////
//
// @(#)hydraTrans/richNew:$Id: $
//*-- Author: Martin Jurkovic   2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichThresholdPar
//
//  Parameter container for threshold data.
//
//////////////////////////////////////////////////////////////////////////////

#include "hdetpario.h"
#include "hpario.h"
#include "hrichthresholdpar.h"
#include "richdef.h"

#include <fstream>

using namespace std;

ClassImp(HRichThresholdPar)

HRichThresholdPar::HRichThresholdPar(const Char_t* name,
                                     const Char_t* title,
                                     const Char_t* context)
   : HParSet(name, title, context)
{
   clear();
}

void HRichThresholdPar::clear()
{
// initialize all parameters to -1

   fCalParVers = -1;

   for (Int_t i = 0; i < 6; ++i) {
      fSigmaMultiplier[i]  = -1;
   }

   status = kFALSE;
   resetInputVersions();
}

Bool_t HRichThresholdPar::init(HParIo* inp, Int_t* set)
{
// Initializes the container from an input

   HDetParIo* input = inp->getDetParIo("HRichParIo");
   if (NULL != input) {
      Bool_t returnValue = input->init(this, set);
      cout << "==> Return value: " << returnValue << endl;
#if DEBUG_LEVEL > 3
      printParams();
#endif
      return returnValue;
   }
   return kFALSE;
}

Int_t HRichThresholdPar::write(HParIo* output)
{
// Writes the container to an output

   HDetParIo* out = output->getDetParIo("HRichParIo");
   if (NULL != out) {
      return out->write(this);
   }
   return -1;
}

Bool_t HRichThresholdPar::readline(const Char_t * buf)
{
// Decodes one line read from ASCII file I/O and fills the cells

   cout << "==> Reading line..." << endl;

   sscanf(buf, "%d %f %f %f %f %f %f", &fCalParVers,
          &fSigmaMultiplier[0],
          &fSigmaMultiplier[1],
          &fSigmaMultiplier[2],
          &fSigmaMultiplier[3],
          &fSigmaMultiplier[4],
          &fSigmaMultiplier[5]);
   return kTRUE;
}

void HRichThresholdPar::write(fstream& fout)
{
// Writes the Rich Threshold Parameters to an ASCII file.

   fout.width(8);
   fout << left << fCalParVers;
   for (Int_t sec = 0; sec < 6; ++sec) {
      fout.width(6);
      fout << left << fSigmaMultiplier[sec];
   }
   fout << endl;
   fout << "#########################################################################" << endl;

}

void HRichThresholdPar::printParams()
{
// Prints the lookup table

   TString header;
   putAsciiHeader(header);
   cout << header;

   cout.width(8);
   cout << left << fCalParVers;
   for (Int_t sec = 0; sec < 6; ++sec) {
      cout.width(6);
      cout << left << fSigmaMultiplier[sec];
   }
   cout << endl;
   cout << "#########################################################################" << endl;

}

void HRichThresholdPar::putAsciiHeader(TString& header)
{
   // puts the ASCII header to the string used in HSpecParAsciiFileIo
   header =
      "#########################################################################\n"
      "# Format:\n"
      "# first parameter:      CalParVer\n"
      "# remaining parameters: sigma multiplier for 6 sectors\n";
}
