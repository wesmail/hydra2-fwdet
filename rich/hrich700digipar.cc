//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700DigiPar
//
//  This class handles parameters for RICH digitizer
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700digipar.h"
#include "hparamlist.h"
#include "richdef.h"
#include "hhistconverter.h"

#include "TRandom.h"
#include "TMath.h"

#include <sstream>
#include <iostream>
ClassImp(HRich700DigiPar)

    using namespace std;;

HRich700DigiPar::HRich700DigiPar(const Char_t* name,const Char_t* title,
				       const Char_t* context)
    : HParCond(name,title,context)
{
    clear();
}

HRich700DigiPar::~HRich700DigiPar()
{
  // destructor
}

void HRich700DigiPar::clear()
{
    fArrayPmt.Reset();
    fArrayPhiMean.Reset();
    fArrayThetaMean.Reset();
    fhxyPhiMean =0;
    fhxyThetaMean =0;
    resetInputVersions();
}

void HRich700DigiPar::printParam(void)
{
    printf("----------------------------------------------------------------------\n");
    printf("HRich700DigiPar: \n");
    printf("fNofPixelsInRow       = %i \n" , fNofPixelsInRow);
    printf("fPmtSize              = %f \n" , fPmtSize);
    printf("fPmtGap               = %f \n" , fPmtGap);
    printf("fPmtSensSize          = %f \n" , fPmtSensSize);
    printf("fCollectionEfficiency = %f \n" , fCollectionEfficiency);
    printf("fCrossTalkProbability = %f \n" , fCrossTalkProbability);
    printf("fNofNoiseHits         = %i \n" , fNofNoiseHits);
    printf("----------------------------------------------------------------------\n");


    printf("Sec PmtId IndX IndY       x          y          z     pmtType     theta     phi  \n");
    for(Int_t i = 0; i < fArrayPmt.GetSize()/NPARPMT; i ++){
        Int_t id =getPMTId  ( (Float_t)fArrayPmt[i*NPARPMT+3], (Float_t) fArrayPmt[i*NPARPMT+4]);
        if(id!= (Int_t)fArrayPmt[i*NPARPMT+0] ) cout <<"missmatch : "<<id<<" "<<(Int_t)fArrayPmt[i*NPARPMT+0]<<endl;

        printf("%3i %5i %4i %4i %10.3f %10.3f %10.3f %5i %10.3f %10.3f \n",
            getSector((Float_t)fArrayPmt[i*NPARPMT+3],(Float_t)fArrayPmt[i*NPARPMT+4]),
            (Int_t)fArrayPmt[i*NPARPMT+0],
            (Int_t)fArrayPmt[i*NPARPMT+1],
            (Int_t)fArrayPmt[i*NPARPMT+2],
            fArrayPmt[i*NPARPMT+3],
            fArrayPmt[i*NPARPMT+4],
            fArrayPmt[i*NPARPMT+5],
            (Int_t)fArrayPmt[i*NPARPMT+6],
            fArrayPmt[i*NPARPMT+7],
            fArrayPmt[i*NPARPMT+8]
        );
    }

    TH2F* htmp    = (TH2F*)HHistConverter::createHist("htmp"  ,fArrayPhiMean);
    TArrayD arTmp;
    HHistConverter::fillArray(htmp,arTmp,"fArrayPhiMean",10,10,kTRUE);
    delete htmp;

    htmp    = (TH2F*)HHistConverter::createHist("htmp"  ,fArrayThetaMean);
    HHistConverter::fillArray(htmp,arTmp,"fArrayThetaMean",10,10,kTRUE);
    delete htmp;

    printf("----------------------------------------------------------------------\n");

}

string  HRich700DigiPar::getStringForParTxtFile()
{
    stringstream ss;

    ss << "##############################################################################" << endl;
    ss << "# Class:   HRich700DigiPar" << endl;
    ss << "# Context: Rich700DigiParProduction" << endl;
    ss << "##############################################################################" << endl;
    ss << "[Rich700DigiPar]" << endl;
    ss << "//----------------------------------------------------------------------------" << endl;
    ss << "fNofPixelsInRow:  Int_t  " << fNofPixelsInRow << endl;
    ss << "fPmtSize:  Double_t  " << fPmtSize << endl;
    ss << "fPmtGap:  Double_t  " << fPmtGap << endl;
    ss << "fPmtSensSize:  Double_t  " << fPmtSensSize << endl;
    ss << "fCollectionEfficiency:  Double_t  " << fCollectionEfficiency << endl;
    ss << "fCrossTalkProbability: Double_t " << fCrossTalkProbability << endl;
    ss << "fNofNoiseHits: Int_t " << fNofNoiseHits << endl;

    return ss.str();
}

Bool_t HRich700DigiPar::init(HParIo* inp,Int_t* set)
{
    // intitializes the container from an input
  Bool_t rc = HParCond::init(inp,set);
  if (rc && changed){
      // make hists new
      fillMaps();
  }

  return rc;
}


void HRich700DigiPar::putParams(HParamList* l)
{
    if (!l) return;
    l->add("fNofPixelsInRow" , fNofPixelsInRow);
    l->add("fPmtSize"        , fPmtSize);
    l->add("fPmtGap"         , fPmtGap);
    l->add("fPmtSensSize"    , fPmtSensSize);
    l->add("fCollectionEfficiency", fCollectionEfficiency);
    l->add("fCrossTalkProbability", fCrossTalkProbability);
    l->add("fNofNoiseHits", fNofNoiseHits);
    l->add("fArrayPmt"       , fArrayPmt);
    l->add("fArrayPhiMean"   , fArrayPhiMean);
    l->add("fArrayThetaMean" , fArrayThetaMean);
}

Bool_t HRich700DigiPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("fNofPixelsInRow", &fNofPixelsInRow))) return kFALSE;
    if(!( l->fill("fPmtSize"       , &fPmtSize)))        return kFALSE;
    if(!( l->fill("fPmtGap"        , &fPmtGap)))         return kFALSE;
    if(!( l->fill("fPmtSensSize"   , &fPmtSensSize)))    return kFALSE;
    if(!( l->fill("fCollectionEfficiency", &fCollectionEfficiency))) return kFALSE;
    if(!( l->fill("fCrossTalkProbability", &fCrossTalkProbability))) return kFALSE;
    if(!( l->fill("fNofNoiseHits", &fNofNoiseHits))) return kFALSE;
    if(!( l->fill("fArrayPmt"      , &fArrayPmt)))       return kFALSE;
    if(!( l->fill("fArrayPhiMean"  , &fArrayPhiMean)))   return kFALSE;
    if(!( l->fill("fArrayThetaMean", &fArrayThetaMean))) return kFALSE;
    return kTRUE;
}
void HRich700DigiPar::fillMaps(void)
{
    if(fhxyPhiMean)   delete fhxyPhiMean;
    if(fhxyThetaMean) delete fhxyThetaMean;

    fhxyPhiMean   = (TH2F*)HHistConverter::createHist("fhxyPhiMean"  ,fArrayPhiMean);
    fhxyThetaMean = (TH2F*)HHistConverter::createHist("fhxyThetaMean",fArrayThetaMean);

    fPmtDataMapPmtId.clear();
    fPmtDataMapXY   .clear();

    fMaxX = -1000;
    fMaxY = -1000;
    for(Int_t i = 0; i < fArrayPmt.GetSize()/NPARPMT; i++){
        Int_t j = i*NPARPMT;
        HRich700PmtData pmtData;
        pmtData.fPmtId = (Int_t)fArrayPmt[j+0];
        pmtData.fIndX = (Int_t)fArrayPmt[j+1];
        pmtData.fIndY = (Int_t)fArrayPmt[j+2];
        pmtData.fX = fArrayPmt[j+3];
        pmtData.fY = fArrayPmt[j+4];
        pmtData.fZ = fArrayPmt[j+5];
        pmtData.fPmtType = static_cast<HRich700PmtTypeEnum>(fArrayPmt[j+6]);
        pmtData.fTheta = fArrayPmt[j+7];
        pmtData.fPhi = fArrayPmt[j+8];

        if(pmtData.fX > fMaxX) fMaxX = pmtData.fX ;
        if(pmtData.fY > fMaxY) fMaxY = pmtData.fY ;

        pmtData.fSector = getSector( (Float_t)pmtData.fX, (Float_t)pmtData.fY);

        fPmtDataMapPmtId[pmtData.fPmtId] = pmtData;
        pair<Int_t,Int_t> xyInd(pmtData.fIndX, pmtData.fIndY);
        fPmtDataMapXY[xyInd] = pmtData;
    }
}
Int_t HRich700DigiPar::getSector(Float_t x, Float_t y)
{
    // In PMT geometry there are no sectors any more ... lets take it for phi
    // phi calculated from x,y on PMT plane
    if(x==0&&y==0) {
	Warning("getSector()","x and y equal zero, return sector 0");
	return 0;
    }

    Float_t phi = TMath::ACos(x/sqrt(x*x+y*y));
    if (y<0) phi = 2.*TMath::Pi()-phi;
    Int_t sectorPhi = (Int_t)(phi/1.0471975)-1;   // get sector from phi angle
    if (sectorPhi == -1) sectorPhi = 5;           // 1st sector is along y axis!
    return sectorPhi;
}

void HRich700DigiPar::getLocation(Int_t pmtId, Float_t x, Float_t y, Int_t *loc, Bool_t silent)
{
    UInt_t ufNofPixelsInRow = (UInt_t)fNofPixelsInRow;

    loc[0]=-1; loc[1]=-1; loc[2]=-1;
    map<Int_t, HRich700PmtData>::iterator it = fPmtDataMapPmtId.find(pmtId);
    if (it == fPmtDataMapPmtId.end()){
    	if(!silent) Warning("getLocation","No PMT with id: %i!", pmtId);
    	return;
    }

    HRich700PmtData pmtData = it->second;
    Double_t dx = fPmtSensSize / (Double_t)fNofPixelsInRow;
    Double_t halfPmtSensSize = fPmtSensSize / 2.;

     // not in the sensitive area
    if (x < -halfPmtSensSize || x > halfPmtSensSize || y < -halfPmtSensSize || y > halfPmtSensSize) {
        return;
    }

    // local pixels address
    UInt_t indX = (UInt_t)((x + halfPmtSensSize) / dx);
    UInt_t indY = (UInt_t)((y + halfPmtSensSize) / dx);

    if (indX >= ufNofPixelsInRow){
	    indX = ufNofPixelsInRow - 1;
    }

    if (indY >= ufNofPixelsInRow){
	    indY = ufNofPixelsInRow - 1;
    }

    //cout << "x:" << x << " y:"<< y << " indX:" << indX << " indY:" << indY << endl;

    Int_t sectorPhi = getSector(x,y);

    //----------------------------------------------------------------------------------
    // location of the pixel
    loc[0] = sectorPhi;
    loc[1] = indX + pmtData.fIndX * ufNofPixelsInRow; // col
    loc[2] = indY + pmtData.fIndY * ufNofPixelsInRow; // row
}


pair<Double_t, Double_t> HRich700DigiPar::getXY(Int_t* loc,Bool_t silent)
{
    UInt_t ufNofPixelsInRow = (UInt_t)fNofPixelsInRow;

    Int_t indX = loc[1];
    Int_t indY = loc[2];

    Int_t pmtIndX = (Int_t) indX / ufNofPixelsInRow;
    Int_t pmtIndY = (Int_t) indY / ufNofPixelsInRow;

    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(pmtIndX, pmtIndY));
    if (it == fPmtDataMapXY.end()){
    	if(!silent)Warning("getXY()","No PMT with XY: %i %i!",pmtIndX,pmtIndY);
    	return pair<Double_t, Double_t>(0., 0.);
    }
    HRich700PmtData pmtData = it->second;

    Int_t locIndX = indX % ufNofPixelsInRow;
    Int_t locIndY = indY % ufNofPixelsInRow;

   // Double_t padding = (fPmtSize - fPmtSensSize) / 2.;
    Double_t dx = fPmtSensSize / ufNofPixelsInRow;
    Double_t halfPmtSensSize = fPmtSensSize / 2.;

    Double_t xLoc = (locIndX + 0.5) * dx - halfPmtSensSize;
    Double_t yLoc = (locIndY + 0.5) * dx - halfPmtSensSize;
    // cout << "locIndX:" << locIndX << " xLoc:" << xLoc << " locIndY:"<< locIndY <<  "  yLoc:" << yLoc << endl;
    // cout << "indX:" << indX << " x: " << xLoc + pmtData.fX << " indY:" << indY << " y:" << yLoc + pmtData.fY << endl;

    pair<Double_t, Double_t> xy(xLoc + pmtData.fX, yLoc + pmtData.fY);
    return xy;
}

pair<Double_t, Double_t> HRich700DigiPar::getPmtCenter(Int_t pmtId)
{
    return pair<Double_t, Double_t>(fPmtDataMapPmtId[pmtId].fX, fPmtDataMapPmtId[pmtId].fY);
}

vector<pair<Double_t, Double_t> > HRich700DigiPar::getPmtCenters()
{
    vector<pair<Double_t, Double_t> > v;
    v.reserve(fPmtDataMapPmtId.size());
    for(map<Int_t,HRich700PmtData>::iterator it = fPmtDataMapPmtId.begin(); it != fPmtDataMapPmtId.end(); it++) {
	    v.push_back(pair<Double_t,Double_t>(it->second.fX, it->second.fY));
    }

    return v;
}

vector<pair<Int_t, Int_t> > HRich700DigiPar::getDirectNeighbourPixels(Int_t col, Int_t row)
{
    UInt_t ufNofPixelsInRow = (UInt_t)fNofPixelsInRow;
    std::vector<pair<Int_t, Int_t> > v;

    UInt_t locCol = (UInt_t)(col % ufNofPixelsInRow);
    UInt_t locRow = (UInt_t)(row % ufNofPixelsInRow);

    if (locCol < ufNofPixelsInRow - 1) {
	    v.push_back(make_pair(col + 1, row));
    }

    if (locCol > 0) {
	    v.push_back(make_pair(col - 1, row));
    }

    if (locRow < ufNofPixelsInRow - 1) {
	    v.push_back(make_pair(col, row + 1));
    }

    if (locRow > 0) {
	    v.push_back(make_pair(col, row - 1));
    }

    return v;
}

vector<pair<Int_t, Int_t> > HRich700DigiPar::getDiagonalNeighbourPixels(Int_t col, Int_t row)
{
    UInt_t ufNofPixelsInRow = (UInt_t)fNofPixelsInRow;
    std::vector<pair<Int_t, Int_t> > v;

    UInt_t locCol = (UInt_t)(col % ufNofPixelsInRow);
    UInt_t locRow = (UInt_t)(row % ufNofPixelsInRow);

    if (locCol < ufNofPixelsInRow - 1 && locRow < ufNofPixelsInRow - 1) {
	    v.push_back(make_pair(col + 1, row  + 1));
    }

    if (locCol > 0 && locRow > 0) {
	    v.push_back(make_pair(col - 1, row  - 1));
    }

    if (locCol < ufNofPixelsInRow - 1 && locRow > 0) {
	    v.push_back(make_pair(col + 1, row - 1));
    }

    if (locCol > 0 && locRow < ufNofPixelsInRow - 1) {
	    v.push_back(make_pair(col - 1, row + 1));
    }

    return v;
}


vector<pair<Int_t, Int_t> > HRich700DigiPar::getNoisePixels(UInt_t nofNoisePixels)
{
    UInt_t ufNofPixelsInRow = (UInt_t)fNofPixelsInRow;
    std::vector<pair<Int_t, Int_t> > v;

    while(kTRUE) {
    	Int_t col = (Int_t)(gRandom->Rndm() * RICH700_MAX_COLS);
    	Int_t row = (Int_t)(gRandom->Rndm() * RICH700_MAX_ROWS);

    	Int_t pmtIndX = (Int_t) col / ufNofPixelsInRow;
    	Int_t pmtIndY = (Int_t) row / ufNofPixelsInRow;

    	map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(pmtIndX, pmtIndY));
    	if (it != fPmtDataMapXY.end()){
    	    v.push_back(make_pair(col, row));
    	    if (v.size() >= nofNoisePixels) break;
    	}
    }

    return v;
}

Int_t HRich700DigiPar::getPMTId(Float_t x, Float_t y)
{
    if(x>fMaxX) x = fMaxX;
    if(y>fMaxY) y = fMaxY;

    if(x<-fMaxX) x = -fMaxX;
    if(y<-fMaxY) y = -fMaxY;

    Int_t pmtIndX = ( (fMaxX+fPmtSize/2.) + x)/(fPmtSize+fPmtGap);
    Int_t pmtIndY = ( (fMaxY+fPmtSize/2.) + y)/(fPmtSize+fPmtGap);
    //cout <<"x: " << x << " y:" << y << " pmtX:" << pmtIndX << " pmtY:" << pmtIndY << endl;
    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(pmtIndX, pmtIndY));
    if (it != fPmtDataMapXY.end()){
	    return it->second.fPmtId;
    } else {
    	//Warning("getPMTId()","No PMT found for (x,y) = (%f,%f).",x,y);
    	return -1;
    }
}

Int_t HRich700DigiPar::getPMTId(Int_t col, Int_t row)
{

    Int_t pmtIndX = col/fNofPixelsInRow;
    Int_t pmtIndY = row/fNofPixelsInRow;

    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(pmtIndX, pmtIndY));
    if (it != fPmtDataMapXY.end()){
	    return it->second.fPmtId;
    } else {
    	Warning("getPMTId()","No PMT found for (col,roe) = (%i,%i).",col,row);
    	return -1;
    }
}

HRich700PmtData* HRich700DigiPar::getPMTData(Int_t pmtid)
{
    map<Int_t,HRich700PmtData>::iterator it = fPmtDataMapPmtId.find(pmtid);
    if(it == fPmtDataMapPmtId.end()) return NULL;
    return &fPmtDataMapPmtId[pmtid];
}

Int_t  HRich700DigiPar::getInterpolatedSectorThetaPhi(Float_t x, Float_t y, Float_t& theta,Float_t& phi)
{
    theta = 0;
    phi   = 0;
    Int_t sector = getSector(x,y);

    phi   = fhxyPhiMean   ->Interpolate(x,y) ;
    theta = fhxyThetaMean ->Interpolate(x,y) ;

    return sector;
}


Bool_t  HRich700DigiPar::getInterpolatedThetaPhiPMT(Float_t x, Float_t y, Float_t& theta,Float_t& phi)
{
    Int_t pmtid = getPMTId(x,y);
    if(pmtid < 0) return kFALSE;

    HRich700PmtData& data = fPmtDataMapPmtId[pmtid];
    Int_t iX = data.fIndX;
    Int_t iY = data.fIndY;
    Double_t x0 = data.fX;
    Double_t y0 = data.fY;
    Double_t phi0 = data.fPhi;
    Double_t theta0 = data.fTheta;

    theta = theta0;
    phi = phi0;

    // indX 0 at min X, indX 23 at max X
    // indY 0 at min Y, indY 23 at max Y
    // get neighbour pmts for interpolation
    //
    //          iy+1            2     1
    //     ix+1 ix,iy ix-1         c
    //          iy-1            3     4
    //
    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator itX, itY, itXY;
    Double_t x1,x2,y1,y2;
    Double_t p11,p12,p21,p22;
    Double_t t11,t12,t21,t22;

    if(x <= x0 && y >= y0){  // right upper (1)
        itX = fPmtDataMapXY.find(make_pair(iX-1, iY));
        itY = fPmtDataMapXY.find(make_pair(iX, iY+1));
        itXY = fPmtDataMapXY.find(make_pair(iX-1, iY+1));
    } else if(x >= x0 && y >= y0){ // left upper (2)
        itX = fPmtDataMapXY.find(make_pair(iX+1, iY));
        itY = fPmtDataMapXY.find(make_pair(iX, iY+1));
        itXY = fPmtDataMapXY.find(make_pair(iX+1, iY+1));
    } else if(x >= x0 && y <= y0){ // left lower (3)
        itX = fPmtDataMapXY.find(make_pair(iX+1, iY));
        itY = fPmtDataMapXY.find(make_pair(iX, iY-1));
        itXY = fPmtDataMapXY.find(make_pair(iX+1, iY-1));
    } else if(x <= x0 && y <= y0){ // right lower (4)
        itX = fPmtDataMapXY.find(make_pair(iX-1, iY));
        itY = fPmtDataMapXY.find(make_pair(iX, iY-1));
        itXY = fPmtDataMapXY.find(make_pair(iX-1, iY-1));
    }

    x1 = x0;
    y1 = y0;
	p11 = phi0;
	t11 = theta0;

    if (itX != fPmtDataMapXY.end()){
	    x2  = itX->second.fX;
	    p21 = itX->second.fPhi;
	    t21 = itX->second.fTheta;
	} else {
	    x2  = x0;
	    p21 = phi0;
	    t21 = theta0;
	}

    if (itY != fPmtDataMapXY.end()){
	    y2  = itY->second.fY;
	    p12 = itY->second.fPhi;
	    t12 = itY->second.fTheta;
	} else {
	    y2  = y0;
	    p12 = phi0;
	    t12 = theta0;
	}

    if (itXY != fPmtDataMapXY.end()){
	    p22 = itXY->second.fPhi;
	    t22 = itXY->second.fTheta;
	} else {
	    p22 = phi0;
	    t22 = theta0;
	}

    Double_t d = 1.0*(x2-x1)*(y2-y1);
    if (d == 0) {
         theta = theta0;
	     phi = phi0;
         return kTRUE;
    }

    theta = (Float_t)(1.0*t11/d*(x2-x)*(y2-y)+1.0*t21/d*(x-x1)*(y2-y)+1.0*t12/d*(x2-x)*(y-y1)+1.0*t22/d*(x-x1)*(y-y1));
    phi = (Float_t)(1.0*p11/d*(x2-x)*(y2-y)+1.0*p21/d*(x-x1)*(y2-y)+1.0*p12/d*(x2-x)*(y-y1)+1.0*p22/d*(x-x1)*(y-y1));

    return kTRUE;
}

Int_t HRich700DigiPar::getSectorPixels(Int_t col,Int_t row)
{
    Int_t pmtIndX = (Int_t) col / fNofPixelsInRow;
    Int_t pmtIndY = (Int_t) row / fNofPixelsInRow;

    Int_t sectorPhi=0;
    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(pmtIndX, pmtIndY));
    if (it != fPmtDataMapXY.end()){
        Float_t x = (Float_t)it->second.fX;
	    Float_t y = (Float_t)it->second.fY;

	    sectorPhi = getSector(x, y);
    }
    return sectorPhi;
}

Int_t HRich700DigiPar::getSectorPMTInd(Int_t xind,Int_t yind)
{
    Int_t sectorPhi=0;
    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(xind,yind));
    if (it != fPmtDataMapXY.end()){
	sectorPhi = it->second.fSector;
    }
    return sectorPhi;
}

Int_t HRich700DigiPar::getSectorPMTId(Int_t pmtid)
{
    Int_t sectorPhi=0;
    map<Int_t,HRich700PmtData> ::iterator it = fPmtDataMapPmtId.find(pmtid);
    if (it != fPmtDataMapPmtId.end()){
	sectorPhi = it->second.fSector;
    }
    return sectorPhi;
}

Int_t HRich700DigiPar::getSectorPhiThetaDegPixels(Int_t col,Int_t row,Float_t& phiDeg,Float_t& thetaDeg)
{
    Int_t pmtIndX = (Int_t) col / fNofPixelsInRow;
    Int_t pmtIndY = (Int_t) row / fNofPixelsInRow;

    Int_t sectorPhi = 0;
    phiDeg          = 0;
    thetaDeg        = 0;

    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(pmtIndX, pmtIndY));
    if (it != fPmtDataMapXY.end()){
        Float_t x = (Float_t)it->second.fX;
	Float_t y = (Float_t)it->second.fY;

        sectorPhi = getInterpolatedSectorThetaPhi(x,y,thetaDeg,phiDeg);

    }
    return sectorPhi;
}

Int_t HRich700DigiPar::getSectorPhiThetaDegPMTInd(Int_t xind,Int_t yind,Float_t& phiDeg,Float_t& thetaDeg)
{
    Int_t sectorPhi = 0;
    phiDeg          = 0;
    thetaDeg        = 0;
    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(xind,yind));
    if (it != fPmtDataMapXY.end()){
	sectorPhi = it->second.fSector;
        phiDeg   = (Float_t)it->second.fPhi;
        thetaDeg = (Float_t)it->second.fTheta;
    }
    return sectorPhi;
}

Int_t HRich700DigiPar::getSectorPhiThetaDegPMTId(Int_t pmtid,Float_t& phiDeg,Float_t& thetaDeg)
{
    Int_t sectorPhi = 0;
    phiDeg          = 0;
    thetaDeg        = 0;
    map<Int_t,HRich700PmtData>::iterator it = fPmtDataMapPmtId.find(pmtid);
    if (it != fPmtDataMapPmtId.end()){
	sectorPhi = it->second.fSector;
        phiDeg    = (Float_t)it->second.fPhi;
        thetaDeg  = (Float_t)it->second.fTheta;
    }
    return sectorPhi;
}
