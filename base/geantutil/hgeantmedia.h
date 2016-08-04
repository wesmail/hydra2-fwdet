#ifndef HGEANTMEDIA_H
#define HGEANTMEDIA_H

#include "TNamed.h"
#include "TString.h"
#include <map>
using namespace std;

class HGeantMedia : public TNamed {
private:
  map<Int_t, TString> media;         // List of GEANT media 
  map<Int_t,TString>::iterator iter; //! Iterator

public:
  HGeantMedia(const Char_t* name ="GeantMedia", const Char_t* title  ="List of Geant media") {}
  ~HGeantMedia(void) {}

  void addMedium(Int_t, const Char_t*);
  TString getMediumName(Int_t id);
  Int_t getMediumId(const TString&);
  void print();

  ClassDef(HGeantMedia,1) // Map of GEANT media
};

#endif  /*! HGEANTMEDIA_H */
