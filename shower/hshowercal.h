#ifndef HShowerCal_H
#define HShowerCal_H
#pragma interface

#include "hlocateddataobject.h"

class HShowerCal : public HLocatedDataObject {
public:
        HShowerCal() : m_nSector(-1), m_nModule(-1), m_nRow(-1), m_nCol(-1),
                       m_fCharge(0.0),
                       m_nIsLocalMax(0) {}

//      initialization could be used e.g. to clear the data element
        HShowerCal(const Float_t fCharge) :
                       m_nSector(-1), m_nModule(-1), m_nRow(-1), m_nCol(-1),
                       m_fCharge(fCharge),
                       m_nIsLocalMax(0) {}

        Int_t clear();
        Float_t getCharge() const {return m_fCharge;}
        void setCharge(Float_t fCharge){m_fCharge = fCharge;}

        Int_t getAddress(void) {return calcAddress();}
        Char_t getSector(void) {return m_nSector;}
        Char_t getModule(void) {return m_nModule;}
        Char_t getRow(void) {return m_nRow;}
        Char_t getCol(void) {return m_nCol;}

        void setSector(Char_t s) {m_nSector = s;}
        void setModule(Char_t m) {m_nModule = m;}
        void setRow(Char_t r) {m_nRow = r;}
        void setCol(Char_t c) {m_nCol = c;}

        Int_t isLocalMax(){return m_nIsLocalMax;}
        void  setLocalMax(){m_nIsLocalMax = 1;}
        void  resetLocalMax(){m_nIsLocalMax = 0;}

        Int_t getNLocationIndex(void);
        Int_t getLocationIndex(Int_t i);

        ClassDef(HShowerCal, 1) //ROOT extension

private:
        Int_t calcAddress();

        Char_t m_nSector;   //position of the pad
        Char_t m_nModule;
        Char_t m_nRow;
        Char_t m_nCol;

        Float_t m_fCharge;  //calibrated charge in the pad

        Int_t m_nIsLocalMax; //! flag is set if it has been found
                             // local maximum for this pad
};

#endif
