// @(#)$Id: hsudummyrec.h,v 1.6 2009-07-15 11:38:42 halo Exp $
//*-- Author : Marcin Jaskula 02/11/2002
//*-- Modifed: Marcin Jaskula 11/07/2003
//             *Events support added
//*-- Modifed: Jacek Otwinowski 10/02/2007
//             reinit() function has been changed 

#ifndef HSUDummyRec_H
#define HSUDummyRec_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HSUDummyRec                                                                //
//                                                                            //
// Dummy reconstructor which calls user defined functions                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "hreconstructor.h"
#include "haddef.h"

#include "TList.h"
#include "TFile.h"

// -----------------------------------------------------------------------------

class HCategory;
class HIterator;
class TFile;

// -----------------------------------------------------------------------------

class HSUDummyRec : public HReconstructor
{
public:
                        HSUDummyRec(Bool_t bVerbose = kTRUE);
                        HSUDummyRec(const Char_t *pOutFileName,
                                            Bool_t bVerbose = kTRUE);
                        HSUDummyRec(const Text_t name[],const Text_t title[],
                                    Bool_t bVerbose = kTRUE);
                        HSUDummyRec(const Char_t *pOutFileName,
                                    const Text_t name[],const Text_t title[],
                                    Bool_t bVerbose = kTRUE);
                       ~HSUDummyRec(void);

    // -------------------------------------------------------------------------

    void                setVerbose(Bool_t b = kTRUE)      { bVerbose = b;      }
    Bool_t              getVerbose(void) const            { return bVerbose;   }

    // -------------------------------------------------------------------------

    static HCategory*   getCategory(Cat_t cat, Bool_t bRaport = kTRUE);
    static HCategory*   getCategory(const Char_t* pName, Bool_t bRaport = kTRUE);

    HIterator*          getIterator(Cat_t cat, Bool_t bDelete = kTRUE);
    HIterator*          getIterator(const Char_t* pName, Bool_t bDelete = kTRUE);

    // -------------------------------------------------------------------------

    void                addObjectToWrite(TObject *pObj, Bool_t bDelete = kTRUE);

    Bool_t              removeObject(TObject *pObj);
    Bool_t              removeIterator(HIterator *pI);

    // -------------------------------------------------------------------------

    Int_t               writeObjects(TFile *pFile);
    Int_t               writeObjects(const Char_t *pFileName,
                                Bool_t bRecreate = kTRUE);

    // -------------------------------------------------------------------------

    TFile*              getOutFile(void) const          { return pOutFile;     }
    Bool_t              setOutFile(TFile *pFile, Bool_t bCloseOld = kFALSE);
    TFile*              openOutFile(const Char_t *pName = NULL,
                                                Bool_t bRecr = kTRUE);
    Bool_t              writeAndCloseOutFile(void);

    // -------------------------------------------------------------------------

    const TString&      getOutFileName(void) const      { return sOutFileName; }
    void                setOutFileName(const Char_t *pName)
                                                        { sOutFileName = pName;}

    // -------------------------------------------------------------------------

    virtual void        Print(Option_t* option) const   { print();             }
    virtual void        print(void) const;

    // -------------------------------------------------------------------------

    virtual Int_t       execute(void)                   { return 0;            }
    virtual Bool_t      init(void)                      { return kTRUE;        }
    virtual Bool_t      finalize(void)                  { return kTRUE;        }
    virtual Bool_t      reinit(void)                    { return kTRUE;        }

    // -------------------------------------------------------------------------

    Bool_t              getInitOk(void)                 { return bInitOk;      }
    void                setInitOk(Bool_t b = kTRUE)     { bInitOk = b;         }

    // -------------------------------------------------------------------------

    void                setEvents(Int_t i)              { iEvents = i;         }
    void                incEvents(void)                 { ++iEvents;           }
    Int_t               getEvents(void) const           { return iEvents;      }

    void                setWriteEvents(Bool_t b = kTRUE){ bWriteEvents = b;    }
    Bool_t              getWriteEvents(void) const      { return bWriteEvents; }

    // -------------------------------------------------------------------------

protected:

    TList               lToDelete;          // list of allocated objects
    TList               lToWrite;           // list of objects for writing

    Bool_t              bVerbose;           // report any warnings

    Bool_t              bInitOk;            // flag if init was ok

    TFile              *pOutFile;           // output file

    TString             sOutFileName;       // name of the output file

    Int_t               iEvents;            // number of events
    Bool_t              bWriteEvents;       // write number of entries

    // -------------------------------------------------------------------------

    void                setDefault(void);

    // -------------------------------------------------------------------------

    ClassDef(HSUDummyRec, 0)  // Dummy reconstructor
};

// -----------------------------------------------------------------------------

class HSUIteratorObject : public TObject
{
public:

                HSUIteratorObject(void)          : pIter(NULL)  {}
                HSUIteratorObject(HIterator *pI) : pIter(pI)    {}
               ~HSUIteratorObject(void);

    // -------------------------------------------------------------------------

    HIterator*  get(void) const             { return pIter; }
    void        set(HIterator *pI)          { pIter = pI;   }

public:
    HIterator *pIter;

    // -------------------------------------------------------------------------

    ClassDef(HSUIteratorObject, 0)  // Wrapper HIterator -> TObject
};

// -----------------------------------------------------------------------------

#endif //HSUDummyRec_H
