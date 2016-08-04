// @(#)$Id: hsuchain.h,v 1.3 2008-09-18 13:04:57 halo Exp $
//*-- Author : Marcin Jaskula 03/06/2003

#ifndef HSUChain_H
#define HSUChain_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HSUChain                                                                   //
//                                                                            //
// Chain with access to the HCategory objects                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "TChain.h"
#include "TObjArray.h"

// -----------------------------------------------------------------------------

class HCategory;

// -----------------------------------------------------------------------------

class HSUChain : public TChain
{
public:
                        HSUChain(const Char_t *pName = "T");
                       ~HSUChain(void);


    virtual Int_t   Add(const Char_t *name, Int_t nentries = 0)
                                    { return TChain::Add(name, nentries); }

    virtual Int_t   GetEntry(Int_t entry = 0, Int_t getall = 0);

    virtual Bool_t  Notify(void);

    HCategory*      getCategory(const Char_t *pName);

    // -------------------------------------------------------------------------

protected:


    TObjArray       arr;

    // -------------------------------------------------------------------------

    ClassDef(HSUChain, 0)         // Chain with access to the HCategory objects
};

// -----------------------------------------------------------------------------

#endif //HSUChain_H

