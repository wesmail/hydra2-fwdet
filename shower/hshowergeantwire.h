#ifndef __HShowerGeantWire_H
#define __HShowerGeantWire_H
using namespace std;
#include "TObject.h"
#include <iostream> 
#include <iomanip>

class HShowerGeantWire : public TObject {
public:
        HShowerGeantWire(void);
//      initialization could be used e.g. to clear the data element

        ~HShowerGeantWire(void) {}
        void Clear(void);

        Float_t getCharge(void) const {return fCharge;}
        void getXY(Float_t* pfX, Float_t* pfY);
        Int_t getWireNr(){return nWireNr;}
        Int_t getModule(){return nModule;}
        Int_t getSector(){return nSector;}
        Int_t getTrack(){return nTrack;}
        void setCharge(Float_t q) {fCharge=q;}
        void setXY(Float_t fX, Float_t fY);
        void setWireNr(Int_t nNr){nWireNr = nNr;}
        void setModule(Int_t nMod){nModule = nMod;}
        void setSector(Int_t nSect){nSector = nSect;}
        void setTrack(Int_t nTrk){nTrack = nTrk;}
        friend ostream& operator<< (ostream& output, HShowerGeantWire& element);

protected:
        Float_t fCharge; // Charge for each hit on the wire
        Float_t fXWire;  // X position of hit on the wire
        Float_t fYWire;  // Y position of hit on the wire
        Int_t nWireNr;   // number of the fired wire
        Int_t nModule;   // module number
        Int_t nSector;   // sector number
        Int_t nTrack;    // track number from Kine
public:
        ClassDef(HShowerGeantWire,1) //Shower raw data
};

#endif

