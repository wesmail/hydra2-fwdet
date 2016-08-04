//*-- Author : Jochen Markert 18.07.2007

#ifndef  __HPARTICLETRACKCLEANER_H__
#define  __HPARTICLETRACKCLEANER_H__

#include "hreconstructor.h"
#include "hcategory.h"
#include "hparticletracksorter.h"

#include "TString.h"

class HParticleTrackCleaner : public HReconstructor {

protected:


    HParticleTrackSorter sorter;                   //! for doing the final task of flagging
    HCategory* pParticleCandCat;                   //! HParticleCand category
    Bool_t (*pUserSelectLeptons)(HParticleCand* ); //! user provided function pointer to lepton selection
    Bool_t (*pUserSelectHadrons)(HParticleCand* ); //! user provided function pointer to hadron selection
    Bool_t (*pUserSort)(candidateSort*, candidateSort*); //! user provided function pointer to sort algo
    HParticleTrackSorter::ESwitch sortType;              //! sort type : default kIsBestRKRKMETA
    void   clear(void);
public:
    HParticleTrackCleaner(void);
    HParticleTrackCleaner(const Text_t *name,const Text_t *title);
    ~HParticleTrackCleaner(void);
    Bool_t init      (void);
    Int_t  execute   (void);
    Bool_t finalize  (void);
    static void           setDebug       (void)        { HParticleTrackSorter::setDebug();}
    static void           setPrintLevel  (Int_t level) { HParticleTrackSorter::setPrintLevel(level);}
    void                  setUserSelectionLeptons(Bool_t (*function)(HParticleCand* ))      { pUserSelectLeptons = function;}
    void                  setUserSelectionHadrons(Bool_t (*function)(HParticleCand* ))      { pUserSelectHadrons = function;}
    void                  setUserSort(Bool_t    (*function)(candidateSort*, candidateSort*)){ pUserSort = function;}
    void                  setSortType(HParticleTrackSorter::ESwitch type)                   { sortType = type;}
    HParticleTrackSorter& getSorter() {return sorter;}
    ClassDef(HParticleTrackCleaner,0);
};
#endif /* !__HPARTICLETRACKCLEANER_H__ */







