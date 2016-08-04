// @(#)$Id: hsudummyrec.cc,v 1.7 2009-07-15 11:38:42 halo Exp $
//*-- Author : Marcin Jaskula 02/11/2002
//*-- Modifed: Marcin Jaskula 11/07/2003
//             *Events support added
//*-- Modifed: Jacek Otwinowski 10/02/2007

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HSUDummyRec
//                                                                            //
// Dummy reconstructor which calls user defined functions                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "hsudummyrec.h"

#include "hevent.h"
#include "hades.h"

#include "hcategory.h"
#include "hiterator.h"

#include "hdatasource.h"

#include "TROOT.h"
#include "TError.h"

using namespace std;
// -----------------------------------------------------------------------------

ClassImp(HSUDummyRec)
ClassImp(HSUIteratorObject)

// -----------------------------------------------------------------------------

HSUDummyRec::HSUDummyRec(Bool_t bVerbose)
    : HReconstructor("SuDummyRec", "Dummy reconstructor")
{
// Default constructor.

    setDefault();
    setVerbose(bVerbose);
}

// -----------------------------------------------------------------------------

HSUDummyRec::HSUDummyRec(const Char_t *pOutFileName, Bool_t bVerbose)
    : HReconstructor("SuDummyRec", "Dummy reconstructor")
{
// Constructor with the file name

    setDefault();
    setVerbose(bVerbose);
    setOutFileName(pOutFileName);
}

// -----------------------------------------------------------------------------

HSUDummyRec::HSUDummyRec(const Text_t name[],const Text_t title[], Bool_t bVerbose)
    : HReconstructor(name, title)
{
// Constructor with names

    setDefault();
    setVerbose(bVerbose);
}

// -----------------------------------------------------------------------------

HSUDummyRec::HSUDummyRec(const Char_t *pOutFileName,
                const Text_t name[],const Text_t title[], Bool_t bVerbose)
    : HReconstructor(name, title)
{
// Constructor with all names

    setDefault();
    setVerbose(bVerbose);
    setOutFileName(pOutFileName);
}

// -----------------------------------------------------------------------------

HSUDummyRec::~HSUDummyRec(void)
{
// Delete all objects stored in lToDelete list
// Done automaticaly by the destructor of the TList
// Delete the output file

    if(pOutFile != NULL)
        delete pOutFile;
}

// -----------------------------------------------------------------------------

void HSUDummyRec::setDefault(void)
{
// Called in constructors to set default valuest to the members

    lToDelete.SetOwner();

    bVerbose   = kTRUE;
    bInitOk    = kFALSE;

    pOutFile   = NULL;

    iEvents      = 0;
    bWriteEvents = kFALSE;
}

// -----------------------------------------------------------------------------

HCategory* HSUDummyRec::getCategory(Cat_t cat, Bool_t bRaport)
{
// Static function which returns pointer to a category by its id
// bRaport true indicates warning message where the category is not found

HCategory    *pCat;
HEvent       *pEvent;

    if((gHades == NULL) || ((pEvent = gHades->getCurrentEvent()) == NULL))
    {
        ::Error("HSUDummyRec::getCategory", "Cannot access current event");
        return NULL;
    }

    if((pCat = pEvent->getCategory(cat)) != NULL)
        return pCat;

    if(bRaport)
        ::Warning("HSUDummyRec::getCategory", "No category %d", cat);

    return NULL;
}

// -----------------------------------------------------------------------------

HCategory* HSUDummyRec::getCategory(const Char_t* pName, Bool_t bRaport)
{
// Static function which returns pointer to a category by its name
// bRaport true indicates warning message where the category is not found.
// The search is done from cat = 0 to 1000

Int_t      iMax = 1000;
Int_t      i;

HCategory  *pCat;
HEvent     *pEvent;

    if(pName == NULL)
    {
        :: Error("HSUDummyRec::getCategory", "pName == NULL");
        return NULL;
    }

    if((gHades == NULL) || ((pEvent = gHades->getCurrentEvent()) == NULL))
    {
        ::Error("HSUDummyRec::getCategory", "Cannot access current event");
        return NULL;
    }

    for(i = 0; i < iMax; i++)
    {
        if((pCat = pEvent->getCategory(Cat_t(i))) == NULL)
            continue;

        if(strcmp(pCat->getClassName(), pName) == 0)
            return pCat;
    }

    if(bRaport)
        ::Warning("HSUDummyRec::getCategory", "No category %s", pName);

    return NULL;
}

// -----------------------------------------------------------------------------

HIterator* HSUDummyRec::getIterator(Cat_t cat, Bool_t bDelete)
{
// Return pointer to iterator over category cat
// If bDetele is true (default), then the iterator will be automaticly deleted
// in the HSUDummyRec destructor

HIterator *pI;
HCategory *pCat;

    if((pCat = getCategory(cat, bVerbose)) == NULL)
        return NULL;

    if((pI = (HIterator *) pCat->MakeIterator()) == NULL)
        Error("getIterator", "Cannot create iterator for category %d", cat);
    else
    {
        if(bDelete)
            lToDelete.Add(new HSUIteratorObject(pI));
    }

    return pI;
}

// -----------------------------------------------------------------------------

HIterator* HSUDummyRec::getIterator(const Char_t* pName, Bool_t bDelete)
{
// Return pointer to iterator over category cat
// If bDetele is true (default), then the iterator will be automaticly deleted
// in the HSUDummyRec destructor

HIterator *pI;
HCategory *pCat;

    if((pCat = getCategory(pName, bVerbose)) == NULL)
        return NULL;

    if((pI = (HIterator *) pCat->MakeIterator()) == NULL)
        Error("getIterator", "Cannot create iterator for category %s", pName);
    else
    {
        if(bDelete)
            lToDelete.Add(new HSUIteratorObject(pI));
    }

    return pI;
}

// -----------------------------------------------------------------------------

void HSUDummyRec::addObjectToWrite(TObject *pObj, Bool_t bDelete)
{
// Add the object to the list of objects being written in writeObjects()
// If bDetele is true (default), then the object will be automaticly deleted
// in the HSUDummyRec destructor

    if(pObj == NULL)
        return;

    lToWrite.Add(pObj);

    if(bDelete)
        lToDelete.Add(pObj);
}

// -----------------------------------------------------------------------------

Bool_t HSUDummyRec::removeObject(TObject *pObj)
{
// Remove the object from the lists for writing and deleting.
// Returns kTRUE if the object was on any list

Bool_t bReturn = kFALSE;

    bReturn |= (lToDelete.Remove(pObj) != NULL);
    bReturn |= (lToWrite.Remove(pObj) != NULL);

    return bReturn;
}

// -----------------------------------------------------------------------------

Bool_t HSUDummyRec::removeIterator(HIterator *pI)
{
// Remove the iterator created by the getIterator() method.
// The iteraror is no deleted !!!

TIterator *i = lToDelete.MakeIterator();
TObject   *p;

    i->Reset();
    while((p = i->Next()) != NULL)
    {
        if(p->IsA() != HSUIteratorObject::Class())
            continue;

        if(((HSUIteratorObject *)p)->get() != pI)
            continue;

        lToDelete.Remove(p);
        delete p;

        return kTRUE;
    }

    return kFALSE;
}

// -----------------------------------------------------------------------------

Int_t HSUDummyRec::writeObjects(TFile *pFile)
{
// Writes the object from lToWrite list into pFile file
// If pFile == NULL then the gFile is used.

TFile *pTmp = gFile;
Int_t  iWritten;

    if(pFile == NULL)
        pFile = gFile;

    if((pFile == NULL) || ( ! pFile->IsOpen()) || ( ! pFile->IsWritable()))
    {
        Error("writeObjects", "Cannot write to file: %s",
                (pFile == NULL) ? "(nil)" : pFile->GetName());
        return 0;
    }

    pFile->cd();
    iWritten = lToWrite.Write();

    if(pTmp != NULL)
        pTmp->cd();

    return iWritten;
}

// -----------------------------------------------------------------------------

Int_t HSUDummyRec::writeObjects(const Char_t *pFileName, Bool_t bRecreate)
{
// Writes the object from lToWrite list into pFileName file.
// If bRecreate is true the file is "RECREATE"d, otherwise "UPDATE"d.
// If the file is already used and bRecreate is true the file is recreated
// as well.
// If pFileName == NULL then the gFile is used and bRecreated flag is ignored.

    if(pFileName == NULL)
        return writeObjects((TFile *)NULL);

TFile *pTmp   = gFile;
TFile *pFile  = (TFile *)gROOT->GetListOfFiles()->FindObject(pFileName);
Bool_t bClose = kTRUE;
Int_t  iWritten;

    if((pFile != NULL) && (bRecreate))
    {
        if(pTmp == pFile)
            pTmp = NULL;

        delete pFile;
        pFile  = NULL;
        bClose = kFALSE;
    }

    if(pFile == NULL)
        pFile = new TFile(pFileName, (bRecreate) ? "RECREATE" : "UPDATE");

    iWritten = writeObjects(pFile);

    if(bClose)
        delete pFile;

    if(pTmp != NULL)
        pTmp->cd();

    return iWritten;
}

// -----------------------------------------------------------------------------

void HSUDummyRec::print(void) const
{
// Print information about the class and its members

    TObject::Print();

    printf("Verbose: %s\n", (bVerbose) ? "YES" : "NO");

    printf("To write:\n");
    lToWrite.Print();

    printf("\nTo delete:\n");
    lToDelete.Print();
}

// -----------------------------------------------------------------------------

Bool_t HSUDummyRec::setOutFile(TFile *pFile, Bool_t bCloseOld)
{
// Set new output file (must be writable)
// If bCloseOld flag is on, then the old file is closed before

    if((pOutFile != NULL) && (bCloseOld == kTRUE))
        delete pOutFile;

    pOutFile = NULL;

    if(pFile == NULL)
        return kTRUE;

    if(( ! pFile->IsOpen()) || ( ! pFile->IsWritable()))
    {
        Error("setOutFile", "File %s is not writable", pFile->GetName());
        return kFALSE;
    }

    pOutFile = pFile;

    return kTRUE;
}

// -----------------------------------------------------------------------------

TFile* HSUDummyRec::openOutFile(const Char_t *pName, Bool_t bRecr)
{
// Open a new file for writing. The old file is closed.
// If bRecr is set, the file is recreated, otherwise updated.

    if(pOutFile != NULL)
    {
        delete pOutFile;
        pOutFile = NULL;
    }

    if(pName == NULL)
    {
        pName = sOutFileName.Data();

        if((pName == NULL) || (strlen(pName) < 1))
        {
            Error("openOutFile", "NULL file name");
            return NULL;
        }
    }

TFile  *pFile    = (TFile *)gROOT->GetListOfFiles()->FindObject(pName);
Bool_t  bCreated = kFALSE;

    if((pFile != NULL) && (bRecr))
    {
        delete pFile;
        pFile = NULL;
    }

    if(pFile == NULL)
    {
        pFile = new TFile(pName, (bRecr) ? "RECREATE" : "UPDATE");
        bCreated = kTRUE;
    }

    if(( ! pFile->IsOpen()) || ( ! pFile->IsWritable()))
    {
        Error("openOutFile", "Cannot open %s for writing", pName);

        if(bCreated)
            delete pFile;
    }
    else
        pOutFile = pFile;

    return pOutFile;
}

// -----------------------------------------------------------------------------

Bool_t HSUDummyRec::writeAndCloseOutFile(void)
{
// Write and close the output file

    if(pOutFile == NULL)
    {
        Error("writeAndCloseOutFile", "No output file");
        return kFALSE;
    }

    if(bVerbose)
        Warning("writeAndCloseOutFile", "writing ...");

    if(bWriteEvents)
    {
    TFile  *pOldFile = gFile;
    TNamed *pName;
    Char_t    s[1000];

        sprintf(s, "%d", iEvents);
        pName = new TNamed("events", s);

        pOutFile->cd();
        pName->Write();

        if(pOldFile != NULL)
            pOldFile->cd();

        delete pName;
    }

    pOutFile->Write();
    delete pOutFile;
    pOutFile = NULL;

    if(bVerbose)
        Warning("writeAndCloseOutFile", "ok ...");

    return kTRUE;
}

// -----------------------------------------------------------------------------

HSUIteratorObject::~HSUIteratorObject(void)
{
// Delete pIter if it is not NULL

     if(pIter != NULL)
         delete pIter;
}
