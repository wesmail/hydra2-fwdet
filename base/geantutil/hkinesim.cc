// File: hkinesim.cc
//
// Author: Laura Fabbietti <L.Fabbietti@physik.tu-muenchen.de>
// Last update by Laura Fabbietti: 99/12/17 14:38:48
//
using namespace std;
#include "hades.h"
#include "hgeantkine.h"
#include "hkinesim.h"
#include "hlinearcategory.h"
#include <iostream> 
#include <iomanip>
#include "hdebug.h"

HKineSim::HKineSim(void){
    iter = 0;
    fGeantKineCat = 0;

    fGeantKineCat = (HLinearCategory*)gHades->getCurrentEvent()
	->getCategory(catGeantKine);
    if (!fGeantKineCat) { cout<<"Laura non c'e !"<<endl; }

    iter = (HIterator*)(fGeantKineCat->MakeIterator());
}

HKineSim::~HKineSim(void) {}

HGeantKine* HKineSim::getObj(Int_t nTrack) {
    return (HGeantKine*)(fGeantKineCat->getObject(nTrack-1));
}

Bool_t HKineSim::checkChargedPart(Int_t nTrack) {
    Int_t nTrackKine=0;
    Int_t  nId = 0;

    Bool_t aTrue = 0;
    getObj(nTrack)->getParticle(nTrackKine,nId);
    if(nId==14 || nId==8 || nId==9 || nId==2 || nId==3) aTrue = 1;
    else aTrue = 0;
    return aTrue;
}

ClassImp(HKineSim)





