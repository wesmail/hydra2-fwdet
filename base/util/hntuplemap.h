#ifndef HNTUPLEMAP_H
#define HNTUPLEMAP_H

#include "TObject.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TString.h"
#include <map>
#include <string>
using namespace std;


class HNtupleMap : public TObject {

public:

    TNtuple* ntuple;      //! pointer to the ntuple
    TFile*   file;        //! pointer to the input file
    map<string, Int_t> c; //! map object : key==leaf name, mapped index
    Float_t* a;           //! pointer to the argument array of the ntuple

    HNtupleMap();

    ~HNtupleMap();

    void createMap(TString in, TString ntupleName);
    void printMap();
    Float_t& operator[](const std::string& val);
    ClassDef(HNtupleMap,1) // map class arround TNtuple

};

#endif  /*!HNtupleMap_H*/
