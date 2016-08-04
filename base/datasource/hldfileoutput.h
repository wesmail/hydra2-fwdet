#ifndef HLDFILEOUTPUT_H
#define HLDFILEOUTPUT_H

using namespace std;
#include <fstream>
#include "TObject.h"
#include "TString.h" 

class HldEvt;
class HldSource;

class HldFileOutput : public TObject {
protected:
  HldEvt* evt;           // Pointer to the HLD event
  UInt_t numTotal;       // Total number of events written
  UInt_t numFiltered;    // Number of filtered events written
  ofstream* fout;        // File output
  TString fDir;          // File directory
  TString fileSuffix;    // File suffix (default f_)
  TString fOption;       // File option
  UChar_t padding[64];   // Byte array for padding

  HldFileOutput(void);
public:
  HldFileOutput(HldSource*,const Text_t*,const Text_t*,const Option_t* pOption="NEW");
  ~HldFileOutput(void);
  void setHldSource(HldSource*);
  void setDirectory(const Text_t*);
  void setFileSuffix(const Text_t*);
  void setFileOption(const Option_t* pOption="NEW");
  Bool_t open(const Text_t*);
  void close();
  void writeEvent();
  UInt_t getNumTotalEvt() { return numTotal; }
  UInt_t getNumFilteredEvt() { return numFiltered; }

  ClassDef(HldFileOutput,0) //! Writes LMD files;
};
#endif /* !HLDFILEOUTPUT_H */
  
