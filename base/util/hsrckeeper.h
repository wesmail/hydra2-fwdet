//*-- Author : Jochen Markert 19.11.2014

#ifndef  __HSRCKEEPER_H__
#define  __HSRCKEEPER_H__


#include "TNamed.h"
#include "TObjArray.h"
#include "TCollection.h"
#include "TMacro.h"
#include "TString.h"


class HSrcKeeper : public TNamed {

private:

    TObjArray source;  //
    TMacro*  newFile(TString fname);

public:
    HSrcKeeper(const char *name = "",const char *title = "");
    ~HSrcKeeper(void);
    Bool_t     addSourceFile          (TString file);
    Int_t      addSourceFiles         (TString dir,TString regexp="\\..*[cCh]+$");
    Int_t      addSourceFilesRecursive(TString dir,TString regexp="\\..*[cCh]+$");
    TObjArray& getSource() {return source;}
    void       print  (TString regexp=".*",Bool_t show=kFALSE);
    void       extract(TString destinationdir,TString replacedir="", TString regexp=".*");
    TMacro*    getFile(TString name,Bool_t usePath=kTRUE);
    virtual Long64_t Merge(TCollection *list);
    ClassDef(HSrcKeeper,1);
};
#endif /* !__HSRCKEEPER_H__ */







