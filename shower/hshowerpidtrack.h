#ifndef HSHOWERPIDTRACK_H
#define HSHOWERPIDTRACK_H

#pragma interface

#include "hshowerpid.h"

class HShowerPIDTrack : public HShowerPID{
protected:
  Int_t nTrack;   // Track number

public:
  HShowerPIDTrack(void) : nTrack(0){}
  ~HShowerPIDTrack(void) {}
  void clear(void);
  inline void setTrack(const Int_t track) {nTrack=track;} //set track number
  inline Int_t getTrack(void) const {return nTrack;} //get track number

  Bool_t IsSortable() const { return kTRUE; }
  Int_t Compare(const TObject *obj) const;

  HShowerPIDTrack& operator=(HShowerPIDTrack& pt);
  HShowerPIDTrack& operator=(HShowerPID& pt);

  ClassDef(HShowerPIDTrack,1) // SHOWER PID data tied with track number
};

inline Int_t HShowerPIDTrack::Compare(const TObject *obj) const {
   if (nTrack==((HShowerPIDTrack*)obj)->getTrack()) return 0;
   return (nTrack > ((HShowerPIDTrack*)obj)->getTrack() ? 1 : -1);
}


#endif /* !HSHOWERPIDTRACK_H */

