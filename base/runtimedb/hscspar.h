#ifndef HSCSPAR_H
#define HSCSPAR_H

#include "hparset.h"
#include "TObjArray.h"

class HScsPar : public HParSet {
protected:
  TObjArray* pChannels;     // Array of channels    
public:
  HScsPar(const Char_t*,const Char_t*);
  virtual ~HScsPar();
  virtual Bool_t init(HParIo* io,Int_t*);
  virtual Int_t write(HParIo*);
  virtual void clear();  
  virtual void printParams();
  virtual void putAsciiHeader(TString& s) {s="# Slow Control data\n";}
  void write(fstream&);
  TObjArray* getChannels() {return pChannels;}
protected:
  HScsPar();
  ClassDef(HScsPar,1) // Base class for Slow Control parameter containers
};

#endif  /* !HSCSPAR_H */
