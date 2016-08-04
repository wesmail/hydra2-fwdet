//*-- Author : Jochen Markert 18.07.2007

#include "hparticletrackcleaner.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hparticledef.h"

#include <iostream>

using namespace std;



//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleTrackCleaner
//
// This HReconstructor loops over the HPidTRackCand category and
// analysis the objects. Purpose of the procedure is to categorize
// the objects in the case of multiple usage of the single detector
// hits (RICH, InnerMDC, OuterMDC, META) as Double_t hit and to provide
// a hint which of the candidates is the best with respect to certain
// criteria (described by the bit flags below). This class is a simple
// wrapper for HParticleTrackSorter (see the corresponding documentation)
//
// The selection of good tracks first for leptons and
// second for hadrons will be performed. All flags on HParticleTrackCand
// objects will be reseted at the beginning of the event. The best tracks will
// be selected according to HParticleTrackSorter::selectXXX() functions
// and HParticleTrackSorter::kIsBestRKRKMETA criteria (RKChi2 * RK_META_MATCH_QUALITY).
//
// The user can provide his own selection functions for leptons/hadrons to replace
// the standard selection function via
// setUserSelectionLeptons(Bool_t (*function)(HParticleTrackCand* ))
// setUserSelectionHadrons(Bool_t (*function)(HParticleTrackCand* ))
// setUserSort(Bool_t (*function)(candidateSort*,candidateSort*))
// setSortType(UInt_t) // see HParticleTrackSorter::ESwitch
// See documentation of HParticleTrackSorter how to write such a selection function.
//
// For debugging purpose one cand set screen print outs to check the sorting
// (setDebug() + setPrintLevel(Int_t level)(level = 1-3).
//
// How to use HParticleTrackCleaner:
//
//   // connect the HParticleTrackCleaner after HParticleCandFiller
//   HParticleTrackCleaner = new HParticleTrackCleaner();
//   // do configurations via the static function of HParticleTrackSorter
//   //HParticleTrackSorter::setIgnoreRICH();
//   // add the task to the tasklist
//   tasks->add(cleaner);
////////////////////////////////////////////////////////////////////////////////

ClassImp(HParticleTrackCleaner)


HParticleTrackCleaner::HParticleTrackCleaner(void)
    : HReconstructor("ParticleTrackCleaner", "Particle candidate cleaner")
{
    clear();
}

HParticleTrackCleaner::HParticleTrackCleaner(const Text_t* name,const Text_t* title)
: HReconstructor(name, title)
{
    clear();
}

HParticleTrackCleaner::~HParticleTrackCleaner(void)
{

}

void HParticleTrackCleaner::clear()
{
    pUserSelectLeptons = NULL;
    pUserSelectHadrons = NULL;
    pUserSort          = NULL;
    pParticleCandCat   = NULL;
    sortType           = HParticleTrackSorter::kIsBestRK;
}

Bool_t HParticleTrackCleaner::init(void)
{
    // get the category pointer to the HParticleTrackCand category and
    // init the HParticleTrackSorter object.
    pParticleCandCat = gHades->getCurrentEvent()->getCategory(catParticleCand);
    if (!pParticleCandCat) {
	Error("HParticleTrackCleaner::init()"," No HParticleCand Input -> Switching HParticleTrackCleaner OFF");
    }

    if(pUserSort) sorter.setUserSort(pUserSort);
    sorter.backupSetup();
    return sorter.init();
}

Int_t HParticleTrackCleaner::execute(void)
{
    // perform the selection of good tracks first for leptons and
    // second for hadrons. All flags on HParticleTrackCand objects will
    // be reseted at the beginning of the event. The best tracks will
    // be selected according to HParticleTrackSorter::selectXXX() functions
    // and HParticleTrackSorter::kIsBestRKRKMETA criteria (RKChi2 * RK_META_MATCH_QUALITY).

    if(!pParticleCandCat) return 0; // nothing to do

    sorter.restoreSetup();
    // clean all flags !
    sorter.resetFlags(kTRUE,kTRUE,kTRUE,kTRUE);     // flags,reject,used,lepton

    //-------LEPTONS---------------------------
    if(pUserSelectLeptons){
	sorter.fill(pUserSelectLeptons);                  // fill only good leptons (user provided)
    }else{
	sorter.fill(HParticleTrackSorter::selectLeptons); // fill only good leptons

    }
    sorter.selectBest(sortType,HParticleTrackSorter::kIsLepton);
    //-------HADRONS---------------------------
    if(pUserSelectHadrons){
	sorter.fill(pUserSelectHadrons);                  // fill only good hadrons (user provided) (already marked good leptons will be skipped)
    } else {
	sorter.fill(HParticleTrackSorter::selectHadrons); // fill only good hadrons (already marked good leptons will be skipped)
    }
    sorter.selectBest(sortType,HParticleTrackSorter::kIsHadron);

    sorter.cleanUp();
    return 0;

}

Bool_t HParticleTrackCleaner::finalize(void)
{
    // write ntuple to file (if it exist)
    return sorter.finalize();
}
