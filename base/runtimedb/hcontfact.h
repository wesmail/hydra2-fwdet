#ifndef HCONTFACT_H
#define HCONTFACT_H

#include "TList.h"
#include "TNamed.h"

class HParSet;

class HContainer : public TNamed {
private:
  HContainer();
protected:
  TList* contexts;       // available contexts for this parameter container
  TString actualContext; // actual context set by the user
public:
  HContainer( const Char_t*, const Char_t*, const Char_t*);
  ~HContainer();
  void addContext(const Char_t*);
  Bool_t setActualContext(const Char_t* c);
  const Char_t* getDefaultContext();
  const Char_t* getActualContext() { return actualContext.Data(); }
  void print();
  TString getConcatName();
  const Char_t* getContext();
  ClassDef(HContainer,0) // class for list elements in class HContFact
};

class HContFact : public TNamed {
protected:
  TList* containers;   // all parameter containers managed by this factory
public:
  HContFact();
  virtual ~HContFact();
  Bool_t addContext(const Char_t* name);
  void print();
  HParSet* getContainer(const Char_t*);
  virtual HParSet* createContainer(HContainer*) {return 0;}
protected:
  const Char_t* getActualContext(const Char_t* name) {
    return ((HContainer*)containers->FindObject(name))->getActualContext();
  }
  ClassDef(HContFact,0) // base class of all factories for parameter containers
};

#endif  /* !HCONTFACT_H */
