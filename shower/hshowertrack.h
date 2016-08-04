#ifndef __HShowerTrack_H
#define __HShowerTrack_H
#pragma interface

#include "TObject.h"

class HShowerTrack : public TObject {
public:
        HShowerTrack(){}
//      initialization could be used e.g. to clear the data element
       ~HShowerTrack() {}

        Int_t clear();

        Int_t getAddress(void) {return m_nAddress;}
        Int_t getTrack(void) {return m_nTrack;}

        void setAddress(Int_t nAddr) {m_nAddress = nAddr;}
        void setTrack(Int_t nTrack) {m_nTrack = nTrack;}

        //The ShowerTrack objects are made "sortable" by the pad address
        //Laura's extension
        Bool_t IsSortable() const { return kTRUE; }
        inline Int_t Compare(const TObject *obj) const;

        //-------------------------------------------------------

        ClassDef(HShowerTrack,1) //ROOT extension

private:
       Int_t m_nAddress; //address of pad in SMRRCC convention
       Int_t m_nTrack;   //track number

       //additional data member

};

 //The ShowerTrack objects are made "sortable" by the pad address
 //Laura's extension

inline Int_t HShowerTrack::Compare(const TObject *obj) const {
   if (m_nAddress==((HShowerTrack*)obj)->getAddress()) {
      if (m_nTrack==((HShowerTrack*)obj)->getTrack()) return 0;
      return (m_nTrack > ((HShowerTrack*)obj)->getTrack()) ? 1 : -1;
   } else
     return (m_nAddress > ((HShowerTrack*)obj)->getAddress()) ? 1 : -1;
}

#endif

