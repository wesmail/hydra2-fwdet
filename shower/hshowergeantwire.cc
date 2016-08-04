using namespace std;
#include "hshowergeantwire.h"
#include <iostream> 
#include <iomanip>

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////
// HShowerGeantWire
//
// This class describes  of the hit on the wire of pre-SHOWER
// detector
//
//////////////////////////////////////////////////////

ClassImp(HShowerGeantWire)

HShowerGeantWire::HShowerGeantWire(void) {
  Clear();
}

void HShowerGeantWire::Clear(void) {
//clearing data
  setCharge(0.0);
  setXY(0.0, 0.0);
  setWireNr(-1);
  setSector(-1);
  setModule(-1);
  setTrack(-1);
}

void HShowerGeantWire::getXY(Float_t* pfX, Float_t* pfY) {
//return position x,y of the hit on the wire
  *pfX = fXWire;
  *pfY = fYWire;
}

void HShowerGeantWire::setXY(Float_t fX, Float_t fY) {
//set position x,y of the hit on the wire
  fXWire = fX;
  fYWire = fY;
}


ostream& operator<< (ostream& output, HShowerGeantWire& raw) {
        output<<"charge:"<<raw.fCharge;
        output<<"X wire:"<<raw.fXWire;
        output<<"Y wire:"<<raw.fYWire;
        return output;
}    


