//*-- Author  : Georgy Kornakov
//*-- Created : 27.01.2016
//*-- Modified : 29/07/2016 by V.Pechenov

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetStrawDigitizer
//
//  This class digitizes the Forward Straw detector data
//
//  Produce calibrated Time and energy loss and the straw number
//
//
//
//  !!! There digitization is not implemented yet !!!
//  Programm store to the output category catFwDetStrawCal tof from geant hit
//  and minimal distance from geant track to the wire.
//
/////////////////////////////////////////////////////////////

using namespace std;

#include <cstring>

using namespace std;

#include "hfwdetstrawdigitizer.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hdebug.h"
#include "hspectrometer.h"
#include "hevent.h"
#include "hcategory.h"
#include "fwdetdef.h"
#include "hgeantkine.h"
#include "hgeantfwdet.h"
#include "hfwdetdetector.h"
#include "hfwdetstrawcalsim.h"
#include "hfwdetstrawdigipar.h"
#include <TError.h>
#include <TRandom.h>

ClassImp(HFwDetStrawDigitizer);

const Float_t HFwDetStrawDigitizer::T12_z[2] =
{
   -9.424,  // z-coord. of the begining of the 1st layer in doubleLayer
   -0.674   // z-coord. of the begining of the 2nd layer in doubleLayer
};

const Int_t HFwDetStrawDigitizer::nstraws_Tx[HFwDetStrawDigitizer::nstations] =
{
   80,   //160, // number of straws per layer in module 1
   113   //226  // number of straws per layer in module 2
};

const Float_t HFwDetStrawDigitizer::Tx_x[HFwDetStrawDigitizer::nstations][2] =
{
  {
   -396.15, // x coord. of the 1st wire in 1st layer of doubleLayer in module 1 A1SV1)
   -401.19  //  x coord. of the 1st wire in 2nd layer of doubleLayer in module 1 A1SV1)
  },
  {
   -558.025, // the same like above, but for module 2 A2SV1
   -563.075
  }
};

HFwDetStrawDigitizer::HFwDetStrawDigitizer()
{
   // default constructor
   initVariables();
}

HFwDetStrawDigitizer::HFwDetStrawDigitizer(const Text_t *name, const Text_t *title) :
   HReconstructor(name, title)
{
   // constructor
   initVariables();
}

HFwDetStrawDigitizer::~HFwDetStrawDigitizer()
{
}

void HFwDetStrawDigitizer::initVariables(void)
{
   // initialize the variables in constructor
   fGeantFwDetCat    = NULL;
   fFwDetStrawCalCat = NULL;
   fStrawDigiPar     = NULL;
   fLoc.setNIndex(4);
   fLoc.set(4,0,0,0,0);
}

Bool_t HFwDetStrawDigitizer::init(void)
{
   // initializes the task

   // find the Forward detector in the HADES setup
   HFwDetDetector* pFwDet = (HFwDetDetector*)(gHades->getSetup()->getDetector("FwDet"));
   if (!pFwDet)
   {
      Error("FwDetStrawDigitizer::init","No Forward Detector found");
      return kFALSE;
   }

   // GEANT input data
   fGeantFwDetCat = gHades->getCurrentEvent()->getCategory(catFwDetGeantRaw);
   if (!fGeantFwDetCat)
   {
      Error("HFwDetStrawDigitizer::init()","HGeant FwDet input missing");
      return kFALSE;
   }

   // build the Calibration category
   fFwDetStrawCalCat=pFwDet->buildCategory(catFwDetStrawCal);
   if (!fFwDetStrawCalCat)
   {
      Error("HFwDetStrawDigitizer::init()","Cal category not created");
      return kFALSE;
   }

   // create the parameter container
   fStrawDigiPar = (HFwDetStrawDigiPar *)gHades->getRuntimeDb()->getContainer("FwDetStrawDigiPar");
   if (!fStrawDigiPar)
   {
      Error("HFwDetStrawDigitizer::init()","Parameter container for digitizer not created");
      return kFALSE;
   }

   return kTRUE;
}

Int_t HFwDetStrawDigitizer::execute(void)
{
    // Digitization of GEANT hits and storage in HFwDetStrawCalSim
    // gErrorIgnoreLevel = kFatal;

    Int_t entries = fGeantFwDetCat->getEntries();
    for(Int_t i = 0; i < entries; ++i)
    {
        HGeantFwDet* ghit = (HGeantFwDet*)fGeantFwDetCat->getObject(i);
        if (ghit)
        {
            ghit->getAddress(module, doubleLayer);
            Int_t mod = module;
                            fLoc[0] = mod;
                            fLoc[1] = doubleLayer;

            if(mod > FWDET_STRAW_MAX_MODULES)
                continue; // skip the other detectors of the FwDet

            ghit->getHit(xHit, yHit,  zHit, pxHit, pyHit, pzHit, tofHit, trackLength, eHit);
            trackNumber = ghit->getTrackNumber();
        //cout << trackNumber << " " << mod << " " << doubleLayer << " " << xHit << " " << yHit << endl;

            Float_t tanTr = pxHit/pzHit;
            for(Int_t lay=0;lay<2;lay++) {
                fLoc[2] = lay;
                // Calculate range of cells (cell1-cell2) were track can give signal:
                Float_t z1 = T12_z[lay] - zHit;                                       // It eq. to zWire-straw_diam/2 (doubleLayer coor.sys)
                Float_t z2 = z1 + straw_diam ;                                        // It eq. to zWire+straw_diam/2 (doubleLayer coor.sys)
                Float_t x1 = tanTr*z1 + xHit;                                         // x coor. of the cross point of the track with plane z=z1
                Float_t x2 = tanTr*z2 + xHit;                                         // x coor. of the cross point of the track with plane z=z2
                Float_t c1 = calcCellNumber(x1,mod,lay);
                Float_t c2 = calcCellNumber(x2,mod,lay);
                if(c1 > c2) {Float_t c=c1; c1=c2; c2=c;}                              // c1 must be < c2
                if(c2 < 0.) continue;                                                 // out of acceptance
                Int_t   cell1 = TMath::Max(Int_t(c1),0);
                Int_t   cell2 = TMath::Min(Int_t(c2),nstraws_Tx[mod]-1);
                if(cell1 >= nstraws_Tx[mod]) continue;                               // out of acceptance

                for(Int_t c=cell1; c<=cell2; c++) {
                fLoc[3] = c;
                // Calculate the minimal distance from track to the wire:
                Float_t xPos = Tx_x[mod][lay] + straw_diam*c;                                           // wire position
                Float_t minDist = TMath::Abs(( (x1 + x2)/2. - xPos))/TMath::Sqrt(1.+tanTr*tanTr);       // minimal distance from track to the wire

                if(minDist < straw_diam/2.) fillStrawCalSim(minDist, xPos, T12_z[lay]+straw_diam/2.);
                }
            }
        }
    }
    return 0;
}

Bool_t HFwDetStrawDigitizer::fillStrawCalSim( Float_t radius, Float_t posX, Float_t posZ)
{
    // function creat and fill object HFwDetStrawCalSim
    // return kFALSE if dat was not stored to the category

    const Float_t resol = 0.2; // !!! - straw tube resolution 200 um (interim solution)
    Int_t first = 1;

    //printf("loc= %i %i %i %i  %.2f\n",fLoc[0],fLoc[1],fLoc[2],fLoc[3],radius);
    HFwDetStrawCalSim * cal = (HFwDetStrawCalSim*)fFwDetStrawCalCat->getObject(fLoc);
    if (cal != NULL)
    {                                     // straw tube ocupaed by another track:
        /*AZ
        Float_t time,elos, radi, X, Z;
        Int_t StrawN;
        cal->getHit(time, elos, radi, X, Z, StrawN);
        if(radi <= radius) return kFALSE;                      // FIXME Sorting need to be done by signale time !
        */
        //*AZ
        if (cal->getDriftId().begin()->first <= radius)
        {
            cal->addTrack(radius,trackNumber);
            return kFALSE;
        }
        first = 0;
        //*/
    }
    else
    {
        cal = (HFwDetStrawCalSim*)fFwDetStrawCalCat->getSlot(fLoc);
    }
    if (cal)
    {
//         cal = new(cal) HFwDetStrawCalSim;
        if (first) cal = new(cal) HFwDetStrawCalSim;
        cal->addTrack(radius,trackNumber);
        cal->setAddress(fLoc[0], fLoc[1], fLoc[2], fLoc[3]);
//         cal->setHit(tofHit, eHit, radius, posX, posZ, 0);
        Float_t coord = radius + gRandom->Gaus() * resol;
        coord = TMath::Max(Float_t(0), coord);
        coord = TMath::Min(coord, straw_diam/2);
        cal->setHit(tofHit, eHit, coord, posX, posZ, 0);
        cal->setTrack(trackNumber);
    }
    else
        return kFALSE;

    return kTRUE;
}
