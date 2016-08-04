#ifndef __HPARTICLEMDC_H__
#define __HPARTICLEMDC_H__

#include "TObject.h"
#include "hparticledef.h"

class HMdcSeg;
class HMdcTrkCand;

class HParticleMdc : public TObject
{
private:
    Short_t fIndex;       // own index
    Int_t   fMdcFlag;    // Cluster info: number of cells, bins and merged clusters
    Short_t fMdc1x;      // x1,y1 - cross point of the segment line with first mdc layer (in sec.coor.sys.)
    Short_t fMdc1y;      // Convert to mm: Float_t(x1)*0.1; Float_t(y1)*0.1;
    Short_t fMdc2x;      // x2,y2 - cross point of the segment line with second mdc layer (in sec.coor.sys.)
    Short_t fMdc2y;      // Convert to mm: Float_t(x2)*0.1; Float_t(y2)*0.1;
    Short_t fMdc3x;      // x1,y1 - cross point of the segment line with third mdc layer (in sec.coor.sys.)
    Short_t fMdc3y;      // Convert to mm: Float_t(x1)*0.1; Float_t(y1)*0.1;
    Short_t fMdc4x;      // x2,y2 - cross point of the segment line with 4th mdc layer (in sec.coor.sys.)
    Short_t fMdc4y;      // Convert to mm: Float_t(x2)*0.1; Float_t(y2)*0.1;

    SmallFloat dedxInner;            // mean value of t2-t1 for inner segment
    SmallFloat dedxSigmaInner;       // sigma of t2-t1 distribution in inner segment
    SmallFloat dedxOuter;            // mean value of t2-t1 for outer segment
    SmallFloat dedxSigmaOuter;       // sigma of t2-t1 distribution in outer segment
    SmallFloat dedxCombined;         // mean value of t2-t1 for inner+outer segment
    SmallFloat dedxSigmaCombined;    // sigma of t2-t1 distribution in inner+outer segment


    UChar_t    dedxNWireCutCombined; // number of wires in inner+outer segment cutted by truncated mean procedure
    UChar_t    dedxNWireCutInner;    // number of wires in inner segment cutted by truncated mean procedure
    UChar_t    dedxNWireCutOuter;    // number of wires in outer segment cutted by truncated mean procedure

    SmallFloat dedx[4];              // mean value of t2-t1 per module
    SmallFloat dedxSigma[4];         // sigma of t2-t1 distribution per module
    UChar_t    dedxNWire[4];         // number of wires per module before truncated mean procedure
    UChar_t    dedxNWireCut[4];      // number of wires per module cutted by truncated mean procedure

public:

    HParticleMdc();
    ~HParticleMdc();

    void    setIndex(Short_t ind)       { fIndex = ind; }
    Short_t getIndex()                  { return fIndex;}

    void    setFlag(const Int_t fl)     {  fMdcFlag=fl; }
    Int_t   getNBinsClus  (void) const  { return fMdcFlag >> 16;}
    Int_t   getNCellsClus (void) const  { return (fMdcFlag & 0xFFFF)/100;}
    Int_t   getNMergedClus(void) const  { return (fMdcFlag & 0xFFFF)%100;}

    Float_t getMdc1X(void) const        { return Float_t(fMdc1x) * 0.1;}  // in [mm]
    Float_t getMdc1Y(void) const        { return Float_t(fMdc1y) * 0.1;}  // in [mm]
    Float_t getMdc2X(void) const        { return Float_t(fMdc2x) * 0.1;}  // in [mm]
    Float_t getMdc2Y(void) const        { return Float_t(fMdc2y) * 0.1;}  // in [mm]
    Float_t getMdc3X(void) const        { return Float_t(fMdc3x) * 0.1;}  // in [mm]
    Float_t getMdc3Y(void) const        { return Float_t(fMdc3y) * 0.1;}  // in [mm]
    Float_t getMdc4X(void) const        { return Float_t(fMdc4x) * 0.1;}  // in [mm]
    Float_t getMdc4Y(void) const        { return Float_t(fMdc4y) * 0.1;}  // in [mm]

    Int_t   getNRemovedCellsInnerSeg();

    Float_t getdedx(Int_t mod)        { return dedx        [mod]; }
    Float_t getSigmadedx(Int_t mod)   { return dedxSigma   [mod]; }
    UChar_t getNWirededx(Int_t mod)   { return dedxNWire   [mod]; }
    UChar_t getNWireCutdedx(Int_t mod){ return dedxNWireCut[mod]; }

    Float_t getdedxSeg        (Int_t seg);
    Float_t getSigmadedxSeg   (Int_t seg);
    UChar_t getNWireCutdedxSeg(Int_t seg);

    void    fill(HMdcSeg*);
    void    fill(HMdcTrkCand*);

    ClassDef(HParticleMdc,2)  // A simple object for investigation purpose
};


#endif // __HPARTICLEDEMDC_H__
