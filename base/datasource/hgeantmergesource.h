#ifndef HGEANTMERGESOURCE_H
#define HGEANTMERGESOURCE_H

#include "hdatasource.h"
#include "hrootsource.h"
#include "hparallelevent.h"
#include "haddef.h"
#include "hgeantdef.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include <map>
#include "TString.h"
#include "TEventList.h"


class HRecEvent;

class HGeantMergeSource : public HRootSource {
protected:

    vector<HParallelEvent*>  fAdditionalInputs;

public:
    HGeantMergeSource(Bool_t fPersistent=kTRUE, Bool_t fMerge=kFALSE);
    ~HGeantMergeSource(void);
    virtual EDsState getNextEvent(Bool_t doUnpack=kTRUE);  // hdatasource
    virtual Bool_t   getEvent(Int_t eventN);               // hrootsource
    virtual void     Clear(void);                          // hrootsource

    Bool_t   init(void);                     // hdatasource
    Bool_t   reinit(void)  { return kTRUE;}  // hdatasource
    Bool_t   finalize(void) {return kTRUE;}  // hdatasource


    Bool_t   addFile(const Text_t file[],Bool_t print=kTRUE);
    Bool_t   addMultFiles(TString commaSeparatedList);
    Bool_t   addAdditionalInput(TString filename,Bool_t print=kTRUE);
    Bool_t   createGeantEvent(HRecEvent* fCurrentEvent);

    ClassDef(HGeantMergeSource,1) //Data source to read Root files.
};

#endif /* !HGEANTMERGESOURCE_H */


