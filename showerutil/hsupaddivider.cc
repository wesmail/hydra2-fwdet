// @(#)$Id: hsupaddivider.cc,v 1.4 2008-09-18 13:04:57 halo Exp $
//*-- Author : Marcin Jaskula 08/04/2003

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//
// HSUPadDivider
//
////////////////////////////////////////////////////////////////////////////////

#include "hsupaddivider.h"
#include "TStyle.h"
#include "TError.h"

// -----------------------------------------------------------------------------

HSUPadDivider::HSUPadDivider(TPad *pPad, UInt_t iRows, UInt_t iCols,
                        Float_t fLeftMarg, Float_t fBottomMarg)
{
UInt_t       r, c;
TVirtualPad *pOldPad = gPad;
Double_t     dX0, dX1, dY0, dY1;
TPad        *pNew;
Char_t        s[1000];

    if((iRows <= 0) || (iCols <= 0))
    {
        ::Error("HSUPadDivider::HSUPadDivider", "Wrong numbers: %d %d",
                iRows, iCols);

        return;
    }

    if(pPad == NULL)
        pPad = new TCanvas();

    if(fLeftMarg < 0.0f)
        fLeftMarg = gStyle->GetPadLeftMargin();

    if(fBottomMarg < 0.0f)
        fBottomMarg = gStyle->GetPadBottomMargin();

    pPad->Clear();

    for(r = 0; r < iRows; r++)
    {
        pPad->cd();

        if(r == iRows - 1)
            dY0 = 0.0;
        else
            dY0 = fBottomMarg + (1.0 - fBottomMarg) * (iRows - 1 - r) / iRows;

        dY1 = fBottomMarg + (1.0 - fBottomMarg) * (iRows - r) / iRows;

        for(c = 0; c < iCols; c++)
        {
            if(c == 0)
                dX0 = 0.0;
            else
                dX0 = fLeftMarg + (1.0 - fLeftMarg) * c / iCols;

            dX1 = fLeftMarg + (1.0 - fLeftMarg) * (c + 1) / iCols;

            sprintf(s, "%s_%d_%d", pPad->GetName(), r, c);

            pNew = new TPad(s, s, dX0, dY0, dX1, dY1);
            pNew->SetTopMargin(0.0);
            pNew->SetRightMargin(0.0);

            if(r == iRows - 1)
            {
                pNew->SetBottomMargin(fBottomMarg
                            / ((1.0 - fBottomMarg) / iRows + fBottomMarg));
            }
            else
                pNew->SetBottomMargin(0.0);

            if(c == 0)
            {
                pNew->SetLeftMargin(fLeftMarg
                            / ((1.0 - fLeftMarg) / iCols + fLeftMarg));
            }
            else
                pNew->SetLeftMargin(0.0);

            pNew->Draw();

            arr.AddLast(pNew);
        }
    }

    if(pOldPad != NULL)
        pOldPad->cd();
}
