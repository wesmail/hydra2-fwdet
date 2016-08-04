//*-- AUTHOR Ilse Koenig
//*-- created : 03/12/2009 by Ilse Koenig
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
// HStart2Calpar
//
// Container class for START2 calibration parameters
//
/////////////////////////////////////////////////////////////

#include "hades.h"
#include "hdetpario.h"
#include "hpario.h"
#include "hspectrometer.h"
#include "hstart2calpar.h"
#include "hstart2detector.h"

#include <iomanip>
#include <iostream>

using namespace std;

ClassImp(HStart2Calpar)
ClassImp(HStart2CalparMod)
ClassImp(HStart2CalparCell)

HStart2CalparMod::HStart2CalparMod(Int_t n)
{
   // constructor creates an array of pointers of type HStart2CalparCell
   array = new TObjArray(n);
   for (Int_t i = 0 ; i < n ; i++) {
      array->AddAt(new HStart2CalparCell(), i);
   }
}

HStart2CalparMod::~HStart2CalparMod()
{
   // destructor
   array->Delete();
   delete array;
}

HStart2Calpar::HStart2Calpar(const Char_t* name, const Char_t* title,
                             const Char_t* context)
   : HParSet(name, title, context)
{
   // constructor creates an array of pointers of type HStart2CalparMod
   strcpy(detName, "Start");
   HStart2Detector* det = (HStart2Detector*)(gHades->getSetup()->getDetector("Start"));
   Int_t nMod = det->getMaxModInSetup();
   Int_t nComp = det->getMaxComponents();
   array = new TObjArray(nMod);
   for (Int_t i = 0; i < nMod; i++) {
      if (det->getModule(-1, i) > 0) {
         array->AddAt(new HStart2CalparMod(nComp), i);
      } else {
         array->AddAt(new HStart2CalparMod(0), i);
      }
   }
}

HStart2Calpar::~HStart2Calpar()
{
   // destructor
   array->Delete();
   delete array;
}

Bool_t HStart2Calpar::init(HParIo* inp, Int_t* set)
{
   // intitializes the container from an input
   HDetParIo* input = inp->getDetParIo("HStartParIo");
   if (input) return (input->init(this, set));
   return kFALSE;
}

Int_t HStart2Calpar::write(HParIo* output)
{
   // writes the container to an output
   HDetParIo* out = output->getDetParIo("HStartParIo");
   if (out) return out->write(this);
   return -1;
}

void HStart2Calpar::clear()
{
   // clears the container
   for (Int_t i = 0; i < getSize(); i++) {
      HStart2CalparMod& mod = (*this)[i];
      for (Int_t j = 0; j < mod.getSize(); j++)
         mod[j].clear();
   }
   status = kFALSE;
   resetInputVersions();
}

void HStart2Calpar::printParam()
{
   // prints the calibration parameters
   printf("Calibration parameters for the START2 detector\n");
   printf("module  strip  TdcSlope  TdcOffset  AdcSlope  AdcOffset\n");
   Float_t data[4];
   for (Int_t i = 0; i < getSize(); i++) {
      HStart2CalparMod& mod = (*this)[i];
      for (Int_t j = 0; j < mod.getSize(); j++) {
         HStart2CalparCell& cell = mod[j];
         cell.getData(data);
         if (data[0] != 1.F || data[1] != 0.F || data[2] != 1.F || data[3] != 0.) {
            printf("%4i %4i %10.5f %10.3f %10.5f %10.3f\n",
                   i, j, data[0], data[1], data[2], data[3]);
         }
      }
   }
}

void HStart2Calpar::readline(const Char_t *buf, Int_t *set)
{
   // decodes one line read from ascii file I/O
   Int_t mod, strip;
   Float_t data[4] = {0.F, 0.F, 0.F, 0.F};
   sscanf(buf, "%i%i%f%f%f%f", &mod, &strip, &data[0], &data[1], &data[2], &data[3]);
   if (!set[mod]) return;
   (*this)[mod][strip].fill(data);
   set[mod] = 999;
}

void HStart2Calpar::putAsciiHeader(TString& header)
{
   // puts the ascii header to the string used in HStart2ParAsciiFileIo
   header =
      "# Calibration parameters for the START2 detector\n"
      "# Format:\n"
      "# module   strip   TdcSlope  TdcOffset AdcSlope  AdcOffset\n";
}

Bool_t HStart2Calpar::writeline(Char_t *buf, Int_t mod, Int_t strip)
{
   // writes one line to the buffer used by ascii file I/O
   HStart2CalparCell& cell = (*this)[mod][strip];
   Float_t data[4];
   cell.getData(data);
   if (data[0] != 1.F || data[1] != 0.F || data[2] != 1.F || data[3] != 0.F) {
      sprintf(buf, "%4i %4i %10.5f %10.3f %10.5f %10.3f\n",
              mod, strip, data[0], data[1], data[2], data[3]);
      return kTRUE;
   }
   return kFALSE;
}







