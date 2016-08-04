#pragma interface
#ifndef HGEANTREADER_H
#define HGEANTREADER_H

#include "TObject.h"
#include "TFile.h"

class HGeantReader : public TObject {
protected:
  TFile *fInputFile; //Pointer to the input file.
public:
  HGeantReader(void);
  ~HGeantReader(void);
  virtual Bool_t execute(void);
  virtual Bool_t init(void);
  virtual Bool_t finalize(void) {return kTRUE;}
  virtual void setInput(TFile *file);
  ClassDef(HGeantReader,1) //Base class for the different GEANT readers
};

#endif /* !HGEANTREADER_H */
