
#ifndef HRICH700DIGIPAR_H_
#define HRICH700DIGIPAR_H_

#include "hparcond.h"
#include "hrich700data.h"

#include "TObject.h"
#include "TArrayD.h"
#include "TH2F.h"

#include <vector>
#include <utility>
#include <map>

using namespace std;


class HParamList;


#define NPARPMT 9

class HRich700DigiPar : public HParCond {

private:
    map<Int_t,HRich700PmtData> fPmtDataMapPmtId;             // pmtid       -> (x,y,z,indx,indy,pmtid)
    map<pair<Int_t,Int_t>, HRich700PmtData> fPmtDataMapXY;   // (xind,yind) -> (x,y,z,indx,indy,pmtid)

    Int_t fNofPixelsInRow;
    Double_t fPmtSize; // in mm
    Double_t fPmtSensSize; // in mm, sensitive area of PMT
    Double_t fCollectionEfficiency; // collection efficiency. Final QE = QE * fCollectionEfficiency
    Double_t fCrossTalkProbability; // cross talk probability for direct neighbour pixel
    Int_t fNofNoiseHits; // number of noise hits per event
    Double_t fPmtGap; // in mm, gap betweem PMTs
    TArrayD fArrayPmt; // linearized pmt info fArrayPmt[pmyindex*(x,y,z,indx,indy,pmtid)]
    TArrayD fArrayPhiMean; //  xy->phi  hist
    TArrayD fArrayThetaMean ; //  xy->theta hist

    TH2F*   fhxyPhiMean;      //! hist for mapping xy -> Phi   [deg]
    TH2F*   fhxyThetaMean;    //! hist for mapping xy -> Theta [deg]

    Float_t fMaxX;            //!  remember dimensions to calculate indX+indY from x+y
    Float_t fMaxY;            //!  remember dimensions to calculate indX+indY from x+y

    void fillMaps();
public:

    HRich700DigiPar(const Char_t* name   ="Rich700DigiPar",
		       const Char_t* title  ="Digitizer parameters for RICH",
		       const Char_t* context="Rich700DigiParProduction");
    virtual ~HRich700DigiPar();


    void                              getLocation(Int_t pmtId, Float_t x, Float_t y, Int_t *loc,Bool_t silent=kTRUE);
    pair<Double_t, Double_t>          getXY(Int_t* loc,Bool_t silent=kTRUE);
    pair<Double_t, Double_t>          getPmtCenter(Int_t pmtId);
    vector<pair<Double_t, Double_t> > getPmtCenters();

    Double_t getPmtSize(){ return fPmtSize; }
    Double_t getPmtSensSize(){ return fPmtSensSize; }
    Double_t getCollectionEfficiency(){ return fCollectionEfficiency; }
    Double_t getCrossTalkProbability(){ return fCrossTalkProbability; }
    Int_t getNofNoiseHits(){ return fNofNoiseHits; }
    Double_t getPmtGap (){ return fPmtGap; }
    Int_t getNPmts() { return fArrayPmt.GetSize()/NPARPMT; }
    Int_t getNPixelInRow() { return fNofPixelsInRow; }

    vector<pair<Int_t, Int_t> > getDirectNeighbourPixels  (Int_t col, Int_t row);
    vector<pair<Int_t, Int_t> > getDiagonalNeighbourPixels(Int_t col, Int_t row);
    vector<pair<Int_t, Int_t> > getNoisePixels            (UInt_t nofNoisePixels);
    map<Int_t,HRich700PmtData>  getPmtDataMapPmtId() {return fPmtDataMapPmtId;}
    Int_t            getPMTId  (Float_t x, Float_t y);
    Int_t            getPMTId  (Int_t col, Int_t row);
    HRich700PmtData* getPMTData(Int_t pmtid);

    Int_t  getInterpolatedSectorThetaPhi(Float_t x, Float_t y, Float_t& theta,Float_t& phi);
    Bool_t getInterpolatedThetaPhiPMT(Float_t x, Float_t y, Float_t& theta,Float_t& phi);
    Int_t  getSector(Float_t x, Float_t y);
    Int_t  getSectorPixels(Int_t col,Int_t row);
    Int_t  getSectorPMTInd(Int_t xind,Int_t yInd);
    Int_t  getSectorPMTId (Int_t pmtid);
    Int_t  getSectorPhiThetaDegPixels(Int_t col,Int_t row, Float_t& phiDeg,Float_t& thetaDeg);
    Int_t  getSectorPhiThetaDegPMTInd(Int_t xind,Int_t yInd, Float_t& phiDeg,Float_t& thetaDeg);
    Int_t  getSectorPhiThetaDegPMTId (Int_t pmtid, Float_t& phiDeg,Float_t& thetaDeg);


    Bool_t  init(HParIo* inp,Int_t* set);
    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();
    void    printParam(void);
    string  getStringForParTxtFile();

    ClassDef(HRich700DigiPar, 1)
};

#endif
