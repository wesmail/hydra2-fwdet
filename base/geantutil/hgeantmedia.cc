#include "hgeantmedia.h"

#include <iostream>

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////
//
//  HGeantMedia
// 
//  List of GEANT media
//  
//  It is used to store the GEANT media common block in the ROOT output file
//  and maps the medium ids to the medium names
//
/////////////////////////////////////////////////////////////////////////////

ClassImp(HGeantMedia)

void HGeantMedia::addMedium(Int_t medId, const char* medName) {
  // adds the medium to the map
  // used in hgeant2/geant/hgeahropen to copy the GEANT media common block
  TString name(medName);
  if(name.CompareTo("") != 0 && medId > 0 ) media[medId] = name;
}

TString HGeantMedia::getMediumName (Int_t medId) {
  // returns the name of the medium with id medId
  TString name;
  if(medId > 0) {
    iter = media.find(medId);
    if (iter != media.end()) name = iter->second;
  }  
  return name;
}

Int_t HGeantMedia::getMediumId (const TString& medName) {
  // returns the id of the medium with name medName
  Int_t id = -1;
  for (iter=media.begin(); iter!=media.end() && id<0; ++iter) {
    if (iter->second.CompareTo(medName)==0) id = iter->first;
  }
  return id;
}

void HGeantMedia::print() {
  // prints all media
  for (iter=media.begin(); iter!=media.end(); ++iter) {
    cout << iter->first << "  " << iter->second << '\n';
  }
}
