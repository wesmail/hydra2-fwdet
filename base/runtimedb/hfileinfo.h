#ifndef HFILEINFO_H
#define HFILEINFO_H

#include "hruninfo.h"

class HFileInfo : public HRunInfo {
protected:
  Int_t magnetCurrent; // magnet current
public:
  HFileInfo(const Char_t* filename="");
  ~HFileInfo() {}
  void  setMagnetCurrent(Int_t c) {magnetCurrent=c;}
  Int_t getMagnetCurrent() {return magnetCurrent;}
  void print();
  void write(fstream&);
private:
  ClassDef(HFileInfo,0) // Class for HLD file and run information
};

#endif  /* !HFILEINFO */
