//*-- Author  : R. Lalik <Rafal.Lalik@ph.tum.de>
//*-- Created : 2017

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetCandSim
//
//  Sim information for track vector
//
/////////////////////////////////////////////////////////////

#include "hfwdetcandsim.h"

#include <map>

ClassImp(HFwDetCandSim);

HFwDetCandSim::HFwDetCandSim() :
    fGeantTrackRpc(-1),
    fGeantPx1(0.0), fGeantPy1(0.0), fGeantPz1(0.0), fGeantPx2(0.0), fGeantPy2(0.0), fGeantPz2(0.0),
    fGeantX1(0.0), fGeantY1(0.0), fGeantZ1(0.0), fGeantX2(0.0), fGeantY2(0.0), fGeantZ2(0.0)
{
}

HFwDetCandSim::~HFwDetCandSim() { }

void HFwDetCandSim::print(UChar_t mask) const
{
    // Print vector simulation parameters
    if (mask & 0x01)
    HFwDetCand::print();

    if (mask & 0x04)
    {
        printf("  SIM VECTOR -----\n");
        printf("   Entry vector=%f,%f,%f at %f,%f,%f\n", fGeantPx1, fGeantPy1, fGeantPz1, fGeantX1, fGeantY1, fGeantZ1);
        printf("   Exit vector=%f,%f,%f at %f,%f,%f\n", fGeantPx2, fGeantPy2, fGeantPz2, fGeantX2, fGeantY2, fGeantZ2);
        printf("   Direction vector=%f,%f at %f,%f,%f\n", getGeantTx(), getGeantTy(), fGeantX1, fGeantY1, fGeantZ1);
        printf("                       tracks= ");
        for (Int_t i = 0; i < getNofHits(); ++i)
            printf("%2d,", fGeantTrackInds[i]);
        printf("\n");
    }
}

int HFwDetCandSim::calcGeantCorrTrackIds()
{
    std::map<Int_t, Int_t> tracks;
    for (int i = 0; i < getNofHits(); ++i)
        ++tracks[fGeantTrackInds[i]];
    setGeantCorrTrackIds(tracks.size());
    return tracks.size();
}

void HFwDetCandSim::Streamer(TBuffer &R__b)
{
// Stream an object of class HFwDetCandSim.

    UInt_t R__s, R__c;
    if (R__b.IsReading()) {
        Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
        HFwDetCand::Streamer(R__b);
        HVirtualCandSim::Streamer(R__b);
        R__b >> fGeantTrackRpc;
        R__b >> fGeantPx1;
        R__b >> fGeantPy1;
        R__b >> fGeantPz1;
        R__b >> fGeantPx2;
        R__b >> fGeantPy2;
        R__b >> fGeantPz2;
        R__b >> fGeantX1;
        R__b >> fGeantY1;
        R__b >> fGeantZ1;
        R__b >> fGeantX2;
        R__b >> fGeantY2;
        R__b >> fGeantZ2;
        R__b.WriteArray(fGeantTrackInds, FWDET_STRAW_MAX_VPLANES);
        R__b.CheckByteCount(R__s, R__c, HFwDetCandSim::IsA());
    } else {
        R__c = R__b.WriteVersion(HFwDetCandSim::IsA(), kTRUE);
        HFwDetCand::Streamer(R__b);
        HVirtualCandSim::Streamer(R__b);
        R__b << fGeantTrackRpc;
        R__b << fGeantPx1;
        R__b << fGeantPy1;
        R__b << fGeantPz1;
        R__b << fGeantPx2;
        R__b << fGeantPy2;
        R__b << fGeantPz2;
        R__b << fGeantX1;
        R__b << fGeantY1;
        R__b << fGeantZ1;
        R__b << fGeantX2;
        R__b << fGeantY2;
        R__b << fGeantZ2;
        R__b.ReadStaticArray((Int_t*)fGeantTrackInds);
        R__b.SetByteCount(R__c, kTRUE);
    }
}
