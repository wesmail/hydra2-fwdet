#ifndef HTBOXUNPACKER_H
#define HTBOXUNPACKER_H

#include "hldunpack.h"
#include "hlocation.h"
#include "hstartdef.h"

class HCategory;

class HTBoxUnpacker: public HldUnpack {

private:

   Int_t       fSubEvtId;                       //! subevent id
   HLocation   fLoc;                            //! location in the matrix category for tbox data
   HCategory*  fCat;                            //! pointer to the TBox category

public:

   HTBoxUnpacker(Int_t id = 0x8800);
   ~HTBoxUnpacker(void) {}

   Bool_t init(void);
   Int_t  execute();
   Int_t  getSubEvtId() const;

   ClassDef(HTBoxUnpacker, 0) // unpack scaler data
};

inline Int_t HTBoxUnpacker::getSubEvtId() const
{
   return fSubEvtId;
}

#endif /* !HTBOXUNPACKER_H */











