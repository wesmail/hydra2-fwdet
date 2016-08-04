#ifndef HQAFITFUNCTIONS_H
#define HQAFITFUNCTIONS_H

#include "TMath.h"

Double_t GaussFitFunc(Double_t* x_val, Double_t* par)
{
    Double_t x, y, par0, par1, par2;
    par0  = par[0];
    par1  = par[1];
    par2  = par[2];
    x = x_val[0];
    y = par0*TMath::Gaus(x,par1,par2,0);
    return y;
}

#endif
