
#ifndef HRICH700RINGFINDERHOUGH
#define HRICH700RINGFINDERHOUGH

#include "hrich700ringfittercop.h"
#include "hlocation.h"
#include "hreconstructor.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hgeantrich.h"
#include "hrichcalsim.h"

#include <vector>
#include <map>
#include <functional>


using std::vector;

class HRich700HoughHit {
public:

   HRich700HoughHit():
      fX(0.),
	  fY(0.),
      fX2plusY2(0.f),
      fId(0),
      fIsUsed(kTRUE)
   { }

   virtual ~HRich700HoughHit(){}

   Float_t fX;
   Float_t fY;
   Float_t fX2plusY2;
   UShort_t fId;
   Bool_t fIsUsed;
};


class HRich700HoughHitCmpUp:
public std::binary_function<
             const HRich700HoughHit,
             const HRich700HoughHit,
             bool>
{
public:

   virtual ~HRich700HoughHitCmpUp(){}

   bool operator()(
         const HRich700HoughHit &m1,
         const HRich700HoughHit &m2) const
   {
      return m1.fX < m2.fX;
   }
};

// comparator based on the selection ANN criterion.
class HRich700RingComparatorMore: public std::binary_function< const HRich700Ring, const HRich700Ring, bool>
{
public:

    virtual ~HRich700RingComparatorMore(){}

    bool operator()(
		    const HRich700Ring& ring1,
		    const HRich700Ring& ring2) const
    {
	return ring1.fHits.size() > ring2.fHits.size();
    }
};

class HRich700DigiPar;
class HRich700RingFinderPar;

class HRich700RingFinderHough : public HReconstructor
{

protected:
    static const UShort_t MAX_NOF_HITS = 65000; // maximum number of hits in RICH detector

    // parameters of the Hough Transform algorithm
    UShort_t fNofParts; // number of groups of hits for HT

    Float_t fMaxDistance;        // maximum distance between two hits
    Float_t fMinDistance;        // minimum distance between two hits
    Float_t fMinDistanceSq;      // = fMinDistance*fMinDistance
    Float_t fMaxDistanceSq;      // = fMaxDistance*fMaxDistance

    Float_t fMinRadius;          // minimum radius of the ring
    Float_t fMaxRadius;          // maximum radius of the ring

    Float_t fDx;                 // x bin width of the ring center histogram
    Float_t fDy;                 // y bin width of the ring center histogram
    Float_t fDr;                 // width of the ring radius histogram
    UShort_t fNofBinsX;          // number of bins in X direction
    UShort_t fNofBinsY;          // number of bins in Y direction
    UShort_t fNofBinsXY;         // fNofBinsX*fNofBinsY

    UShort_t fHTCut;             // cut number of entries in maximum bin of XY histogram

    UShort_t fNofBinsR;          // number of bins in radius histogram
    UShort_t fHTCutR;            // cut number of entries in maximum bin of Radius histogram

    UShort_t fMinNofHitsInArea;  // minimum number of hits in the local area

    Float_t fUsedHitsAllCut;     // percent of used hits

    Float_t fRmsCoeffCOP;
    Float_t fMaxCutCOP;

    Float_t fCurMinX;            // current minimum X position of the local area
    Float_t fCurMinY;            // current minimum Y position of the local area

    vector<HRich700HoughHit> fData; // Rich Hough hits
    vector<UShort_t> fHist;         // XY histogram
    vector<UShort_t> fHistR;        // Radius histogram
    vector< vector<UShort_t> > fHitInd; // store hit indexes for different group of hits
    vector<HRich700Ring> fFoundRings;   // store found rings

    HCategory* fCatRichCal;       //!
    HCategory* fCatRichHit;       //!
    HRich700DigiPar* fDigiPar; //!
    HRich700RingFinderPar* fRingPar; //!

    Int_t fEventNum;
    Bool_t fIsSimulation ;      // runnig sim or exp ?
    void processEvent();


public:

    Bool_t init();
    Bool_t reinit();
    Int_t  execute();
    Bool_t finalize();

    HRich700RingFinderHough (const Text_t* name ="Rich700RingFinderHough",  const Text_t* title= "Rich700RingFinderHough");

    virtual ~HRich700RingFinderHough();

    void setParameters();

    virtual void HoughTransformReconstruction();

    virtual void DefineLocalAreaAndHits( Float_t x0, Float_t y0, Int_t *indmin, Int_t *indmax);

    virtual void HoughTransform( UShort_t indmin, UShort_t indmax);

    virtual void HoughTransformGroup(UShort_t indmin,UShort_t indmax,Int_t iPart);

    void FindPeak( Int_t indmin,Int_t indmax);

    void RemoveHitsOfFoundRing(Int_t indmin, Int_t indmax, HRich700Ring* ring);

    void RingSelection();

    void AddRing(HRich700Ring* ring);

    void RecoToMc(const HRich700Ring* ring, Int_t * trackIds, Int_t * weights);

private:
    HRich700RingFinderHough(const HRich700RingFinderHough&);
    HRich700RingFinderHough& operator=(const HRich700RingFinderHough&);
public:
    ClassDef(HRich700RingFinderHough, 0)
};
#endif
