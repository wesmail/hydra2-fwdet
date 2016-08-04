#ifndef HGEOMMDCWIREPLANES_H_
#define HGEOMMDCWIREPLANES_H_

#include "hgeomtransform.h"

#include "TObject.h"
#include "TString.h"

#include <vector>

class HGeomMdcWirePlane  : public TObject {
  public:
    TString        planeName;      // name of GEANT volume which contains the wires
    Int_t          planeType;      // 0: cathode plane, 1: sens wire plane, -1: undefined
    Int_t          numWires;       // number of sens and field wires
    Float_t        centralWireNr;  // number of wire which goes through the geometrical center of MDC
    Float_t        wireDist;       // distance between wires
    Float_t        wireOrient;     // inclination angle
    Float_t        wireRadius[2];  // radius of wires (0: cathode wires, 1: sens wires or cathode 4 wires)
    TString        wireMedium[2];  // medium name of wires (0: cathode wires, 1: sens wires)

    HGeomMdcWirePlane(TString n="undefined", Int_t t=-1);
    ~HGeomMdcWirePlane(){};

    void clear();
    void print();

    ClassDef(HGeomMdcWirePlane,1) // Class for MDC wire plane in Geant
};


class HGeomMdcWirePlanes : public TObject {
  private:
    vector <HGeomMdcWirePlane> fplanes;    // vector of all wire planes
  public:
    HGeomMdcWirePlanes() {
      fplanes.reserve(52);
    }
    ~HGeomMdcWirePlanes() {}

    vector<HGeomMdcWirePlane>& getWirePlanes() { return fplanes; }

    HGeomMdcWirePlane& addWirePlane(TString planeName, Int_t planeType);
    void printWirePlanes();

    ClassDef(HGeomMdcWirePlanes,1) // Class for all MDC wire planes in Geant

};

#endif  /* !HGEOMMDCWIREPLANES_H */
