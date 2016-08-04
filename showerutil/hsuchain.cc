// @(#)$Id: hsuchain.cc,v 1.5 2008-09-18 13:04:57 halo Exp $
//*-- Author : Marcin Jaskula 03/06/2003

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HSUChain                                                                   //
//                                                                            //
// Chain with access to the HCategory objects                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "hsuchain.h"
#include "hcategory.h"
#include "TDirectory.h"
#include "TFile.h"

// -----------------------------------------------------------------------------

ClassImp(HSUChain)

// -----------------------------------------------------------------------------

HSUChain::HSUChain(const Char_t *pName)
    : TChain(pName, "HSUChain")
{
}

// -----------------------------------------------------------------------------

HSUChain::~HSUChain(void)
{
}

// -----------------------------------------------------------------------------

Int_t HSUChain::GetEntry(Int_t entry, Int_t getall)
{
// Fill the barnches with the data. Take care of the files change.

Int_t iTreeN = GetTreeNumber();
Int_t iR;

    if((iR = TChain::GetEntry(entry, getall)) <= 0)
        return iR;

    if(iTreeN == GetTreeNumber())
        return iR;

    Notify();

    return GetEntry(entry, getall);
}

// -----------------------------------------------------------------------------

Bool_t HSUChain::Notify(void)
{
TIterator *it;
HCategory *pCat;
TString    str;
TBranch   *pBranch;

    if((it = arr.MakeIterator()) == NULL)
    {
        Error("Notify", "Cannot make iterator");
        return kFALSE;
    }

    while((pCat = (HCategory *) it->Next()) != NULL)
    {
        str = pCat->getClassName();
        str += ".";

        if((pBranch = GetBranch(str.Data())) == NULL)
        {
            Error("Notify", "No branch %s in file %s", str.Data(),
                    GetFile()->GetName());
        }
        else
            pBranch->SetAddress(&pCat);
    }

    return kTRUE;
}

// -----------------------------------------------------------------------------

HCategory* HSUChain::getCategory(const Char_t *pName)
{
TString    str = pName;
TBranch   *pBranch;
HCategory *pCat;

    str += ".";

    if((pBranch = GetBranch(str.Data())) == NULL)
    {
        Error("getCategory", "No category: %s", pName);
        return NULL;
    }

TDirectory *pDirSav = gDirectory;

    GetDirectory()->cd();

    if((pCat = (HCategory *)gDirectory->FindObjectAny(pName)) == NULL)
    {
        pDirSav->cd();

        Error("getCategory", "No HCategory %s in Tree", pName);

        return NULL;
    }

    pDirSav->cd();

    pCat = (HCategory*) pCat->Clone();

    SetBranchStatus(str.Data(), 1);

    pBranch->SetAddress(&pCat);

    arr.AddLast(pCat);

    return pCat;
}
