//////////////////////////////////////////////////////////////////////////////
//
// @(#)hydraTrans/richNew:$Id: $
//
//*-- Author  : RICH team member
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichCalPar
//
//  Parameter container for calibration data.
//
//////////////////////////////////////////////////////////////////////////////


#include "hdetpario.h"
#include "hpario.h"
#include "hrichcalpar.h"
#include "hrichcalparcell.h"
#include "richdef.h"

#include <fstream>

using namespace std;

ClassImp(HRichCalPar)

HRichCalPar::HRichCalPar(const Char_t* name,
                         const Char_t* title,
                         const Char_t* context)
   : HParSet(name, title, context)
{
   strcpy(detName,"Rich");
   HRichCalParCell* pCell = NULL;

   fLoc.setNIndex(3);
   fParamsTable.set(3, RICH_MAX_SECTORS, RICH_MAX_ROWS, RICH_MAX_COLS);
   fParamsTable.setCellClassName("HRichCalParCell");
   fParamsTable.makeObjTable();

   for (Int_t s = 0; s < RICH_MAX_SECTORS; ++s) {
      fLoc[0] = s;
      for (Int_t rows = 0; rows < RICH_MAX_ROWS; ++rows) {
         fLoc[1] = rows;
         for (Int_t cols = 0; cols < RICH_MAX_COLS; ++cols) {
            fLoc[2] = cols;
            pCell = static_cast<HRichCalParCell*>(fParamsTable.getSlot(fLoc));
            if (NULL != pCell) {
               pCell = new(pCell) HRichCalParCell;
               pCell->reset();
            } else {
               Error("HRichCalPar", "Slot not found:  %i %i %i", fLoc[0], fLoc[1], fLoc[2]);
            }
         }
      }
   }

   status = kFALSE;
   resetInputVersions();

}

HRichCalPar::~HRichCalPar()
{
   fParamsTable.deleteTab();
}

void
HRichCalPar::clear()
{
// initialize all cells to ZERO

   HRichCalParCell* pCell = NULL;

   for (Int_t s = 0; s < RICH_MAX_SECTORS; ++s) {
      fLoc[0] = s;
      for (Int_t r = 0; r < RICH_MAX_ROWS; ++r) {
         fLoc[1] = r;
         for (Int_t c = 0; c < RICH_MAX_COLS; ++c) {
            fLoc[2] = c;
            pCell = static_cast<HRichCalParCell*>(fParamsTable.getSlot(fLoc));
            if (NULL != pCell) {
               pCell->setParams(0., 0.);
            } else {
               Error("clear", "Slot not found:  %i %i %i", fLoc[0], fLoc[1], fLoc[2]);
               return;
            }
         }
      }
   }

   status = kFALSE;
   resetInputVersions();
}

Bool_t
HRichCalPar::init(HParIo* inp, Int_t* set)
{
// Initializes the container from an input

   HDetParIo* input = inp->getDetParIo("HRichParIo");
   if (NULL != input) {
      Bool_t returnValue = input->init(this, set);
#if DEBUG_LEVEL > 3
      printParams();
#endif
      return returnValue;
   }
   return kFALSE;
}

Int_t
HRichCalPar::write(HParIo* output)
{
// Writes the container to an output

   HDetParIo* out = output->getDetParIo("HRichParIo");
   if (NULL != out) {
      return out->write(this);
   }
   return -1;
}

Bool_t
HRichCalPar::readline(const Char_t * buf)
{
// Decodes one line read from ASCII file I/O and fills the cells

   Int_t   sec    = 0;
   Int_t   col    = 0;
   Int_t   row    = 0;
   Float_t offset = 0.0;
   Float_t sigma  = 0.0;

   sscanf(buf, "%i %i %i %f %f", &sec, &col, &row, &offset, &sigma);
   return setCell(sec, row, col, offset, sigma);
}

void
HRichCalPar::write(fstream& fout)
{
// Writes the Rich Calibration Parameters to an ASCII file.

   HRichCalParCell* pCell = NULL;

   for (Int_t s = 0; s < RICH_MAX_SECTORS; ++s) {
      fLoc[0] = s;
      for (Int_t r = 0; r < RICH_MAX_ROWS; ++r) {
         fLoc[1] = r;
         for (Int_t c = 0; c < RICH_MAX_COLS; ++c) {
            fLoc[2] = c;

            pCell = static_cast<HRichCalParCell*>(fParamsTable.getSlot(fLoc));
            if (NULL != pCell) {
               if (0 != pCell->getOffset() &&
                   0 != pCell->getSigma()) {
                  fout.width(4); // sec
                  fout << left << s;
                  fout.width(4); // col
                  fout << left << c;
                  fout.width(4); // row
                  fout << left << r;
                  fout.width(10); // offset
                  fout << left << pCell->getOffset();
                  fout.width(10); // sigma
                  fout << left << pCell->getSigma();
                  fout << endl;
               }
            } else {
               Error("write", "Slot not found:  %i %i %i", fLoc[0], fLoc[1], fLoc[2]);
               return;
            }
         }
      }
   }
}

Bool_t
HRichCalPar::setCell(Int_t sec, Int_t row, Int_t col,
                     Float_t offset, Float_t sigma)
{

   HRichCalParCell* pCell = NULL;

   if (sec >= RICH_MAX_SECTORS || row >= RICH_MAX_ROWS || col >= RICH_MAX_COLS) {
      Error("setCell", "Wrong SW pad coordinates");
      return kFALSE;
   }

   fLoc[0] = sec;
   fLoc[1] = row;
   fLoc[2] = col;

   pCell = static_cast<HRichCalParCell*>(fParamsTable.getSlot(fLoc));
   if (NULL != pCell) {
      pCell->setParams(offset, sigma);
   } else {
      Error("setCell", "Cell not found.");
      return kFALSE;
   }
   return kTRUE;
}

void
HRichCalPar::printParams()
{
// Prints the lookup table

   HRichCalParCell* pCell = NULL;

   for (Int_t s = 0; s < RICH_MAX_SECTORS; ++s) {
      fLoc[0] = s;
      for (Int_t r = 0; r < RICH_MAX_ROWS; ++r) {
         fLoc[1] = r;
         for (Int_t c = 0; c < RICH_MAX_COLS; ++c) {
            fLoc[2] = c;

            pCell = static_cast<HRichCalParCell*>(fParamsTable.getSlot(fLoc));
            if (NULL != pCell) {
               if (0 != pCell->getOffset() &&
                   0 != pCell->getSigma()) {
                  cout.width(4); // sec
                  cout << left << s;
                  cout.width(4); // col
                  cout << left << c;
                  cout.width(4); // row
                  cout << left << r;
                  cout.width(10); // offset
                  cout << left << pCell->getOffset();
                  cout.width(10); // sigma
                  cout << left << pCell->getSigma();
                  cout << endl;
               }
            } else {
               Error("printParams", "Slot not found:  %i %i %i", fLoc[0], fLoc[1], fLoc[2]);
               return;
            }
         }
      }
   }

   cout << "#########################################################################" << endl;

}

void
HRichCalPar::putAsciiHeader(TString & header)
{
   // puts the ASCII header to the string used in HDetParAsciiFileIo
   header =
      "# Rich calibration parameters\n"
      "# Format:\n"
      "# Sec Col Row Offset Sigma\n";
}

HRichCalParCell* HRichCalPar::getSlot(HLocation &loc)
{
   return static_cast<HRichCalParCell*>(fParamsTable.getSlot(loc));
}

HRichCalParCell* HRichCalPar::getObject(HLocation &loc)
{
   return static_cast<HRichCalParCell*>(fParamsTable.getObject(loc));
}

