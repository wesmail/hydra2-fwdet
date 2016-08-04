#include "hshowerdigitizer.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "hcategory.h"
#include "hlinearcatiter.h"
#include "hlocation.h"
#include "hdebug.h"
#include "hades.h"
#include "showerdef.h"
#include "hshowercal.h"
#include "hshowercopy.h"

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

ClassImp(HShowerCopy)

HShowerCopy::HShowerCopy(const Text_t *name,const Text_t *title, Float_t masterOffset) :
  HReconstructor(name,title)
{
  m_zeroLoc.set(0); 
  m_nEvents = 0;
  fIter = NULL;
  fCalIter = NULL;
  mOffset = masterOffset;
}

HShowerCopy::HShowerCopy()
{
  m_zeroLoc.set(0); 
  m_nEvents = 0;
  fIter = NULL;
  fCalIter = NULL;
  mOffset = 0;
}


HShowerCopy::~HShowerCopy(void) {
    if (fIter)    { delete fIter   ; fIter    = NULL;}
    if (fCalIter) { delete fCalIter; fCalIter = NULL;}
}


Bool_t HShowerCopy::init() {
  printf("initialization shower copy reconstructor\n");
  HCategory *pCatMatr;
  HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                                  ->getDetector("Shower");
  pCatMatr=gHades->getCurrentEvent()->getCategory(catShowerRawMatr);
  
  if (!pCatMatr) {
    pCatMatr=pShowerDet->buildCategory(catShowerRawMatr);

    if (!pCatMatr) return kFALSE;
    else gHades->getCurrentEvent()
	       ->addCategory(catShowerRawMatr,pCatMatr,"Shower");
  }

  setInCat(pCatMatr);
  fIter = (HIterator*)pCatMatr->MakeIterator("native");

 
  pCatMatr=gHades->getCurrentEvent()->getCategory(catShowerCal);
  
  if (!pCatMatr) {
    pCatMatr=pShowerDet->buildCategory(catShowerCal);

    if (!pCatMatr) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catShowerCal,pCatMatr,"Shower");
  }
  setOutCat(pCatMatr);
  fCalIter = (HIterator*)pCatMatr->MakeIterator("native");

  return kTRUE;
}

/**************    COPY     **************/

Bool_t HShowerCopy::copy(HShowerRawMatr *pMatr)
{

    HShowerCal *pCal = NULL;
    if (pMatr) 
	{

	Float_t fCharge;

	fCharge = pMatr->getCharge();

	if (fCharge >= mOffset)
	{
	    Bool_t found=kFALSE;
	    if(gHades->getEmbeddingMode()>0)
	    {
		//-------------------------------------------------
		// loop over ShowerCal category and check
		// if the object was existing in real data.
                // if so, just add charge to the existing object

		fCalIter->Reset();
		while ((pCal=(HShowerCal *)fCalIter->Next())!=0)
		{
		    if(pCal->getAddress()==pMatr->getAddress())
		    {
			// if the object exists already just
			// add the charges
			pCal->setCharge(pCal->getCharge() + fCharge);
			found=kTRUE;
			break;
		    }
		}
		//-------------------------------------------------
	    }

	    if(!found)
	    {
		// if cal object did not exist before (always true
		// if not in embedding mode)
		pCal=(HShowerCal *)m_pOutCat->getNewSlot(m_zeroLoc);

		if (pCal != NULL){

		    pCal=new(pCal) HShowerCal;
		    pCal->setCharge( fCharge );
		    pCal->setSector( pMatr->getSector() );
		    pCal->setModule( pMatr->getModule() );
		    pCal->setRow( pMatr->getRow() );
		    pCal->setCol( pMatr->getCol() );
		}
	    }
	}
    }
    return kTRUE;

}



Int_t HShowerCopy::execute()
{
  HShowerRawMatr *pMatr;

  if(gHades->getEmbeddingMode()>0 && gHades->getEmbeddingDebug()==1) gHades->getCurrentEvent()->getCategory(catShowerCal)->Clear();


  Int_t n = 0;

  fIter->Reset();
  while((pMatr = (HShowerRawMatr*)fIter->Next()))
  {
    copy(pMatr);
    n++;
  }

  m_nEvents++;       

  return 0;
}


