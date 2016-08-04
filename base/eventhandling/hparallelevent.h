#ifndef __HPARALLELEVENT_H__
#define __HPARALLELEVENT_H__

#include "TObject.h"
#include "TObjArray.h"

#include "hreconstructor.h"
#include "htree.h"
#include "hrecevent.h"

#include "TFile.h"
#include "TTree.h"

#include <map>
#include <vector>
using namespace std;


class HParallelEvent : public TNamed
{
private:

    HRecEvent*         fCurrentEvent;      //! Event under reconstruction
    HTree*             fTree;              //! input tree for event
    TFile*             fFile;              //! inputfile
    Int_t              fCursor;            //! entry pointer to current event in tree
    void activateTree(TTree *tree);
public:

    HParallelEvent(const Text_t *name = "",const Text_t *title ="");
    virtual ~HParallelEvent();

    HRecEvent*  getRecEvent() { return fCurrentEvent; }
    HTree*      getTree()  { return fTree; }
    TFile*      getFile()  { return fFile; }

    Int_t       getEntry(Int_t eventN)   { fCursor=eventN; return fTree->GetEvent(eventN); }
    Int_t       getCursor()              { return   fCursor ;}
    Bool_t      setInputFile(TString name,Bool_t print=kTRUE);
    void        closeInputFile();

    void        mergeGeantEvent(HRecEvent*  targetEvent);

    ClassDef(HParallelEvent,0)  // A task to create an fitered  root output
};


#endif // __HPARALLELEVENT_H__
