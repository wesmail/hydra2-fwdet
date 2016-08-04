//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HShowerHitDigitizer
//
// This class digitizes  Pre-Shower wires. The value of the charge and
// its position
// on the sense wire are calculated. Due to do it there are needed geometry
// and digitisation parameters.
// These data are stored in HShowerGeantWire category.
//
// The Shower digitization is split into several tasks as shown
//  in the flow diagram below.
//
//   ----------------------
//  |     HShowerUnpacker  |                                                                              //
//  |   (embedding mode)   | \                                                                            //
//  |                      |  \      ------------------                                                   //
//   ----------------------   |     |  HGeantShower    |                                                  //
//                            |      ------------------\                                                  //
//                            |                         \                                                 //
//                            |      ------------------  \------------->  ----------------------          //
//                            |     |  HGeantWire      |   <------------ |  HShowerHitDigitizer |         //
//                            |      ------------------\                  ----------------------          //
//                            |                         \                                                 //
//                 -------------     ------------------  \------------->  -----------------------         //
//             -- | HShowerRaw  |   |  HShowerRawMatr  |   <------------ |  HShowerPadDigitizer  |        //
//            |    -------------     ------------------\                 |( creates track objects|        //
//            |                                         \                |  for real tracks in   |        //
//   ----------------------          ------------------  \               |  embedding mode too)  |        //
//  |   HShowerCalibrater  |        |  HShowerTrack    |  \<------------  -----------------------         //
//  |   (embedding mode)   |         ------------------\   \                                              //
//   ----------------------                             \   \             -----------------------         //
//            |                      ------------------  \   ----------> |   HShowerCopy         |        //
//             -------------------> |  HShowerCal      |  \<------------ |(add charge of real hit|        //
//                                   ------------------\   \             | in embedding too )    |        //
//                                                      \   \             -----------------------         //
//                                   ------------------  ----\--------->  -----------------------         //
//                                  |  HShowerHitHdr   |   <--\--------- |  HShowerHitFinder     |        //
//                                   ------------------        \          -----------------------         //
//                                   ------------------         \        |                                //
//                                  |  HShowerPID      |   <-----\-------|                                //
//                                   ------------------           \      |                                //
//                                   ------------------            \     |                                //
//                                  |  HShowerHit      |   <--------\----|                                //
//                                   ------------------ <            \                                    //
//                                                       \            \                                   //
//                                                        \-------------> ------------------------        //
//                                                                       | HShowerHitTrackMatcher |       //
//                                                                        ------------------------        //
//
//
//  In the case of TRACK EMBEDDING of simulated tracks into
//  experimental data the real data are written by the HShowerUnpacker into
//  HShowerRaw category. The real hits are taken into
//  account by the digitizer (adding of charges, sorting by first hit in
//  photo multiplier). The embedding mode is recognized
//  automatically by analyzing the
//  gHades->getEmbeddingMode() flag.
//            Mode ==0 means no embedding
//                 ==1 realistic embedding (first real or sim hit makes the game)
//                 ==2 keep GEANT tracks   (as 1, but GEANT track numbers will always
//                     win against real data. besides the tracknumber the output will
//                     be the same as in 1)
//
///////////////////////////////////////////////////////////////////////////
#include "hshowerhitdigitizer.h"

#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "hcategory.h"
#include "hmatrixcategory.h"
#include "hlinearcategory.h"
#include "hlocation.h"
#include "hgeantshower.h"
#include "hshowergeantwire.h"
#include "hshowerdigipar.h"
#include "hshowergeometry.h"
#include "hdebug.h"
#include "hades.h"
#include "showerdef.h"
#include <math.h>
#include <stdlib.h>
#include "TRandom.h"

//*-- Author : Leszek Kidon & Jacek Otwinowski
//*-- Modified: 01/06/2000 L.Kidon
//*-- Modified: 22/04/2000 J.Otwinowski
//*-- Modified : 26/8/99 R. Holzmann
//*-- Modified : July 99 Leszek Kidon
//*-- Modified : 04-09/07/2001 M.Jaskula:
//*--            - Gauss->Landau charge distribution
//*--            - New weights for distributsion on the wires
//*-- Modified : 02/08/2001 M.Jaskula:
//*--            - New parameters
//*--            - Efficiency (beta)
//*-- Modified : 03/10/2006 J. Otwinowski

ClassImp(HShowerHitDigitizer)

//------------------------------------------------------------------------------

HShowerHitDigitizer::HShowerHitDigitizer(const Text_t *name,const Text_t *title,
                            Float_t unit) : HShowerDigitizer(name,title)
{
    piconst = 2 * acos(0.);
    fUnit = unit;  // HGeant length unit (1. for cm, 10. for mm)
}

HShowerHitDigitizer::HShowerHitDigitizer()
{
    piconst = 2*acos(0.);
    fUnit = 10.;
}

HShowerHitDigitizer::~HShowerHitDigitizer(void)
{
}

Bool_t HShowerHitDigitizer::init() {
// creates GeantShower(input) and ShowerGeantWire(output) categories
// and adds them to the current event

    printf("initialization of shower hit digitizer \n");

HCategory *pCat;
HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                           ->getDetector("Shower");

    pCat=gHades->getCurrentEvent()->getCategory(catShowerGeantRaw);
    if( ! pCat)
    {
        pCat=pShowerDet->buildCategory(catShowerGeantRaw);

        if( ! pCat)
            return kFALSE;
        else
            gHades->getCurrentEvent()
                         ->addCategory(catShowerGeantRaw, pCat, "Simul");
    }

    setInCat(pCat);

    pCat=gHades->getCurrentEvent()->getCategory(catShowerGeantWire);
    if( ! pCat)
    {
        pCat=pShowerDet->buildCategory(catShowerGeantWire);

        if( ! pCat)
            return kFALSE;
        else
            gHades->getCurrentEvent()
                         ->addCategory(catShowerGeantWire, pCat, "Shower");
    }

    setOutCat(pCat);

    return HShowerDigitizer::init();
}

HShowerHitDigitizer& HShowerHitDigitizer::operator=(HShowerHitDigitizer &c)
{
// It should have been done
    return c;
}

Bool_t HShowerHitDigitizer::digitize(TObject *pHit)
{
  Bool_t nResult = kFALSE;

#if DEBUG_LEVEL>2
    gDebuger->enterFunc("HShowerHitDigitizer::execute");
    gDebuger->message("category points to %p", pHit);
#endif

//    printf("In digitize(): %p\n", pHit);
  HGeantShower *pGeantHit = (HGeantShower*)pHit;
    if (pGeantHit)
    {
//        printf("sec: %d mod: %d\n", pGeantHit->getSector(), pGeantHit->getModule());
        nResult = digiHits(pGeantHit);
    }

#if DEBUG_LEVEL>2
    gDebuger->leaveFunc("HShowerHitDigitizer::execute");
#endif

    return nResult;
}

Bool_t HShowerHitDigitizer::digiHits(HGeantShower *simhit)
{
HLocation         fLoc;
HShowerGeantWire *pGeantWire = NULL;
Float_t             x_exit, y_exit;
Int_t             nDet, nSector;// nRow, nCol;
Int_t             nFiredWire, nFiredWireIn, nFiredWireOut;
Float_t           fE, fTheta, fPhi, fBeta;
Float_t           fX, fY;
Float_t           fNewX, fNewY;
Float_t           fQ, fThick;
Float_t           fTan;
Int_t             i;
Float_t           fY0, fY1;

    nDet = simhit->getModule();
    nSector = simhit->getSector();
    simhit->getHit(fE, fX, fY, fBeta);
    simhit->getIncidence(fTheta, fPhi);

    fX /= fUnit;   // go from mm to cm again  (HGeant 5.0 gives mm!)
    fY /= fUnit;

// --------------  Emulation of old code  -----------------------------------
//    if( nDet==0 && ! pDigiPar->checkEfficiency(0,0,0,0,fBeta)) return kFALSE;

// --------------  New code  ------------------------------------------------
    Int_t nCol, nRow;
    HShowerPad *pPad = pGeometry->getPadParam(nDet)->getPad(fX,fY);
    if (pPad==0) return kFALSE;
    pPad->getPadPos(&nRow,&nCol);
    if(!pDigiPar->checkEfficiency(nSector,nDet,nRow,nCol,fBeta)) return kFALSE;

        // degree to radian conv.
    fTheta = (TMath::Pi()/180.0) * fTheta;
    fPhi   = (TMath::Pi()/180.0) * fPhi;

    HShowerWireTab *phWires = pGeometry->getWireTab(nDet);

    fThick        = pDigiPar->getThickDet();

    fTan = tan(fTheta);
    x_exit = fX + fThick* fTan * cos(fPhi);
    y_exit = fY + fThick* fTan * sin(fPhi);

    nFiredWireIn = phWires->lookupWire(fY);
    if(nFiredWireIn == -1) {
      //      cout<<"no nFiredWireIn for "<<nDet<<" " <<fY<<endl;
      return kFALSE;
    }

    nFiredWireOut = phWires->lookupWire(y_exit);
    if(nFiredWireOut == -1) {
      //      cout<<"no nFiredWireOut for "<<nDet<<" " <<y_exit<<endl;
      return kFALSE;
    }

    if(nFiredWireIn > nFiredWireOut) {   // swap
        i = nFiredWireIn;
        nFiredWireIn  = nFiredWireOut;
        nFiredWireOut = i;
    }

    for(nFiredWire = nFiredWireIn; nFiredWire <= nFiredWireOut; nFiredWire++)
    {
        fNewY = phWires->getWirePos(nFiredWire);

        if(nFiredWireOut == nFiredWireIn)
        {
                // track inside one cell
            fNewX = 0.5 * (fX + x_exit); // average (IN + OUT)
        }
        else
        {
                // find out Y track's corrd. inside the cell
            if(nFiredWire == nFiredWireIn)
                fY0 = fY;
            else
            {
                    // average of two sense wires (a field wire position)
                fY0 = 0.5 * (phWires->getWirePos(nFiredWire - 1) + fNewY);
            }

            if(nFiredWire == nFiredWireOut)
                fY1 = y_exit;
            else
            {
                fY1 = 0.5 * (fNewY
                            + phWires->getWirePos(nFiredWire + 1));
            }

                // X corrd. depends on the average X value inside the cell
            fNewX = fX + 0.5 * (x_exit - fX) * (fY1 + fY0 - 2 * fY)
                            / (y_exit - fY);
        }
//        fQ = pDigiPar->getCharge(0, 0, fBeta);
//        cout << "sector: " << nSector << " module: " << nDet << " charge: " << fQ << endl;

// --------------  Emulation of old code  -----------------------------------
//        if(fQ > 0.0f)

// --------------  New code  ------------------------------------------------
        if((fQ = pDigiPar->getCharge(nSector, nDet, fBeta)) > 0.0f)

        {
            pGeantWire = (HShowerGeantWire*)((HLinearCategory*)getOutCat())
                                                          ->getNewSlot(fLoc);
            if(pGeantWire != NULL)
            {
                pGeantWire = new(pGeantWire) HShowerGeantWire;
                pGeantWire->setWireNr(nFiredWire);
                pGeantWire->setXY(fNewX, fNewY);
                pGeantWire->setCharge(fQ);
                pGeantWire->setModule(nDet);
                pGeantWire->setSector(nSector);
                pGeantWire->setTrack(simhit->getTrack());
            }
        }
    }

    return kTRUE;
}
