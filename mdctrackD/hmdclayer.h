#ifndef HMDCLAYER_H
#define HMDCLAYER_H

#include "hgeomvector.h"
#include "hmdcgeomobj.h"
#include "TVector2.h"

class HMdcSizesCells;

class HMdcLayer : public TObject {


private:

    //-------------------------------------------------------------------------
    // boundary of MDC layers
    TVector2 layerP    [6][4][6][4];   //  4 corner points for each layer          (layer sys)
    TVector2 layerPcrop[6][4][6][4];   //  4 corner points for each croped layer   (layer sys)
    Double_t           xcrop[4][2];    //  module wise x at lower and upper edge  (symmetric)
    Double_t           ycrop[4][2];    //  module wise y low and up
    //-------------------------------------------------------------------------


    //-------------------------------------------------------------------------
    // RK seglane for pathlength recalulation
    HGeomVector targetMidPoint;        // lab sys
    HMdcPlane   plMdc[6];              // lab sys
    //-------------------------------------------------------------------------
public:

    HMdcLayer();

    //-------------------------------------------------------------------------
    // croped Layer stuff
    void   setMDCCrop         (Int_t m,Double_t xlowcrop,Double_t xupcrop,Double_t ylowcrop,Double_t yupcrop);
    void   fillLayerPoints    (HMdcSizesCells* fSizesCells, Bool_t secsys=kFALSE);
    void   calcCropedLayer    (Int_t s,Int_t m,Int_t l);


    Bool_t getLayersEdge      (TVector2& p,Int_t s,Int_t m,Int_t l,
			       Double_t& xmin ,Double_t& xmax,Double_t& ymin,Double_t& ymax,
			       Double_t& xmincrop ,Double_t& xmaxcrop,Double_t& ymincrop,Double_t& ymaxcrop);
    Bool_t getLayersEdge      (TVector2& p,Int_t s,Int_t m,Int_t l,
			       Double_t& xmin ,Double_t& xmax,Double_t& ymin,Double_t& ymax);
    Bool_t getCropedLayersEdge(TVector2& p,Int_t s,Int_t m,Int_t l,
			       Double_t& xmin ,Double_t& xmax,Double_t& ymin,Double_t& ymax);
    Bool_t isCroped            (TVector2& p,Int_t s,Int_t m,Int_t l);

    void   getLayerPoints      (Int_t s,Int_t m,Int_t l, TVector2& p0,TVector2& p1,TVector2& p2,TVector2& p3);
    void   getCropedLayerPoints(Int_t s,Int_t m,Int_t l, TVector2& p0,TVector2& p1,TVector2& p2,TVector2& p3);
    void printLayer       (Int_t s,Int_t m,Int_t l);
    void printCropedLayer (Int_t s,Int_t m,Int_t l);
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // RK stuff
    void   fillRKPlane         (HMdcSizesCells* fSizesCells);
    const HGeomVector&         getTargetMidPoint() { return targetMidPoint; }
    const HMdcPlane&           getMdcPlane(Int_t s){ return plMdc[s]; }
    const HMdcPlane*           getMdcPlanes()      { return &plMdc[0];}
    //-------------------------------------------------------------------------

    ClassDef(HMdcLayer,1)

};

#endif

