#ifndef HRICH700RINGFITTERCOP
#define HRICH700RINGFITTERCOP

#include "hrich700data.h"

#include "TObject.h"


#include <iostream>
#include <cmath>


class HRich700RingFitterCOP
{
public:
    HRich700RingFitterCOP () {}

    static void FitRing(HRich700Ring* ring);

private:

    static void CalcChi2(HRich700Ring* ring);
public:
};

#endif
