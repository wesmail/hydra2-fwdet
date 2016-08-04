#include "hiterator.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "hcategory.h"
#include "hlinearcatiter.h"
#include "hlocation.h"
#include "hshowerdigipar.h"
#include "hshowergeometry.h"
#include "hdebug.h"
#include "hades.h"
#include "showerdef.h"
#include "hshowerdigitizer.h"

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////
//
//  HShowerDigitizer digitizes data, puts output values into
//  HShowerRawMatr data category. The input data are read from the HGeantShower
//  category. The Shower digitization is split into several tasks as shown
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
//  account by the digitizer (adding of charges). The embedding mode is recognized
//  automatically by analyzing the
//  gHades->getEmbeddingMode() flag.
//            Mode ==0 means no embedding
//                 ==1 realistic embedding (first real or sim hit makes the game)
//                 ==2 keep GEANT tracks   (as 1, but GEANT track numbers will always
//                     win against real data. besides the tracknumber the output will
//                     be the same as in 1)
//
/////////////////////////////////////////////////////////////////////

ClassImp(HShowerDigitizer)

    HShowerDigitizer::HShowerDigitizer(const Text_t *name,const Text_t *title) :
    HReconstructor(name,title)
{
    lNrEvent   = 0;
    pDigiPar  = NULL;
    pGeometry = NULL;
    fIter     = NULL;
}

HShowerDigitizer::HShowerDigitizer()
{
    lNrEvent   = 0;
    pDigiPar  = NULL;
    pGeometry = NULL;
    fIter     = NULL;
}


HShowerDigitizer::~HShowerDigitizer(void) {
    if (fIter) delete fIter;
}

Bool_t HShowerDigitizer::init() {
    fIter = (HIterator*)getInCat()->MakeIterator("native");

    return initParameters();
}

Bool_t HShowerDigitizer::initParameters() {
    HRuntimeDb* rtdb=gHades->getRuntimeDb();

    pGeometry = (HShowerGeometry*)rtdb-> getContainer("ShowerGeometry");
    if (!pGeometry) return kFALSE;

    pDigiPar = (HShowerDigiPar*)rtdb->getContainer("ShowerDigiPar");
    if (!pDigiPar) return kFALSE;

    return kTRUE;
}

HShowerDigitizer& HShowerDigitizer::operator=(HShowerDigitizer &c) {
    return c;
}

Int_t HShowerDigitizer::execute()
{
    TObject *pHit;

    lNrEvent++;
    fIter->Reset();
    Int_t n = 0;

    while((pHit = fIter->Next()))
    {
	digitize(pHit);
	n++;
    }

    sort();   // this is used to call the track sort() in pad digitizer

    return 0;
}


