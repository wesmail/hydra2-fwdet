#ifndef HPARSET_H
#define HPARSET_H

#include "TNamed.h"
#include "TArrayI.h"

class HParIo;

class HParSet : public TNamed {
protected:
  Text_t detName[20];      //! name of the detector the container belongs to
  Int_t versions[3];       //! versions of container in the 2 possible inputs
  Bool_t status;           //! static flag 
  Bool_t changed;          //! flag is kTRUE if parameters have changed
  TString paramContext;    // Context/purpose for parameters and conditions
  TString author;          // Author of parameters
  TString description ;    // Description of parameters
public:
  HParSet(const Char_t* name="",const Char_t* title="",const Char_t* context="");
  virtual ~HParSet() {}
  virtual Bool_t init(void) { return init(0); }
  virtual Bool_t init(HParIo* io);
  virtual Bool_t init(HParIo*,Int_t*) { return kFALSE; }
  virtual Int_t write();
  virtual Int_t write(HParIo*) { return kFALSE; }
  virtual void clear() {;}
  virtual void print();
  const Text_t* getDetectorName() {return detName;}
  void resetInputVersions();

  void setInputVersion(Int_t v=-1,Int_t i=0) {
    if (i>=0 && i<3)  versions[i]=v;
  }
  Int_t getInputVersion(Int_t i) {
    if (i>=0 && i<3) return versions[i];
    else return 0;
  }
  void setStatic(Bool_t flag=kTRUE) {status=flag;}
  Bool_t isStatic() {return status;}
  void setChanged(Bool_t flag=kTRUE) {changed=flag;}
  Bool_t hasChanged() {return changed;}

  void setParamContext(const Char_t*);
  const Char_t* getParamContext() const { return paramContext.Data(); }

  void setAuthor(const Char_t* s) {author=s;}
  const Char_t* getAuthor() const { return author.Data(); }

  void setDescription(const Char_t* s) {description=s;}
  const Char_t* getDescription() const { return description.Data(); }

  void copyComment(HParSet& r) {
    author=r.getAuthor();
    description=r.getDescription();
  }

  ClassDef(HParSet,2) // Base class for all parameter containers
};

#endif  /* !HPARSET_H */

