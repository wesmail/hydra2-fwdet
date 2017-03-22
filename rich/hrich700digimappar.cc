//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700DigiMapPar
//
//  This class handles the mapping between xy position and PMT
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700digimappar.h"
#include "hparamlist.h"
#include "richdef.h"
#include "hhistconverter.h"

#include "TRandom.h"
#include "TMath.h"

#include <iostream>
ClassImp(HRich700DigiMapPar)

    using namespace std;;

HRich700DigiMapPar::HRich700DigiMapPar(const Char_t* name,const Char_t* title,
				       const Char_t* context)
    : HParCond(name,title,context)
{
    clear();
}

HRich700DigiMapPar::~HRich700DigiMapPar()
{
  // destructor
}

void HRich700DigiMapPar::clear()
{
    fArrayPmt.Reset();
    fArrayPhiMean.Reset();
    fArrayThetaMean.Reset();
    fhxyPhiMean =0;
    fhxyThetaMean =0;
    resetInputVersions();
}

void HRich700DigiMapPar::printParam(void)
{
    printf("----------------------------------------------------------------------\n");
    printf("HRich700DigiMapPar : \n");
    printf("fNofPixelsInRow  = %i \n" , fNofPixelsInRow);
    printf("fPmtSize         = %f \n" , fPmtSize);
    printf("fPmtGap          = %f \n" , fPmtGap);
    printf("----------------------------------------------------------------------\n");


    printf("Sec PmtId IndX IndY       x          y          z         theta     phi  \n");
    for(Int_t i = 0; i < fArrayPmt.GetSize()/NPARPMT; i ++){
        Int_t id =getPMTId  ( (Float_t)fArrayPmt[i*NPARPMT+3], (Float_t) fArrayPmt[i*NPARPMT+4]);
	if(id!= (Int_t)fArrayPmt[i*NPARPMT+0] ) cout <<"missmatch : "<<id<<" "<<(Int_t)fArrayPmt[i*NPARPMT+0]<<endl;

	printf("%3i %5i %4i %4i %10.3f %10.3f %10.3f %10.3f %10.3f \n",
               getSector((Float_t)fArrayPmt[i*NPARPMT+3],(Float_t)fArrayPmt[i*NPARPMT+4]),
	       (Int_t)fArrayPmt[i*NPARPMT+0],
	       (Int_t)fArrayPmt[i*NPARPMT+1],
	       (Int_t)fArrayPmt[i*NPARPMT+2],
	       fArrayPmt[i*NPARPMT+3],
	       fArrayPmt[i*NPARPMT+4],
	       fArrayPmt[i*NPARPMT+5],
	       fArrayPmt[i*NPARPMT+6],
	       fArrayPmt[i*NPARPMT+7]
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

Bool_t HRich700DigiMapPar::init(HParIo* inp,Int_t* set)
{
    // intitializes the container from an input
  Bool_t rc = HParCond::init(inp,set);
  if (rc && changed){
      // make hists new
      fillMaps();
  }

  return rc;
}


void HRich700DigiMapPar::putParams(HParamList* l)
{
    if (!l) return;
    l->add("fNofPixelsInRow" , fNofPixelsInRow);
    l->add("fPmtSize"        , fPmtSize);
    l->add("fPmtGap"         , fPmtGap);
    l->add("fArrayPmt"       , fArrayPmt);
    l->add("fArrayPhiMean"   , fArrayPhiMean);
    l->add("fArrayThetaMean" , fArrayThetaMean);
}

Bool_t HRich700DigiMapPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("fNofPixelsInRow", &fNofPixelsInRow))) return kFALSE;
    if(!( l->fill("fPmtSize"       , &fPmtSize)))        return kFALSE;
    if(!( l->fill("fPmtGap"        , &fPmtGap)))         return kFALSE;
    if(!( l->fill("fArrayPmt"      , &fArrayPmt)))       return kFALSE;
    if(!( l->fill("fArrayPhiMean"  , &fArrayPhiMean)))   return kFALSE;
    if(!( l->fill("fArrayThetaMean", &fArrayThetaMean))) return kFALSE;
    return kTRUE;
}
void HRich700DigiMapPar::fillMaps(void)
{
    if(fhxyPhiMean)   delete fhxyPhiMean;
    if(fhxyThetaMean) delete fhxyThetaMean;

    fhxyPhiMean   = (TH2F*)HHistConverter::createHist("fhxyPhiMean"  ,fArrayPhiMean);
    fhxyThetaMean = (TH2F*)HHistConverter::createHist("fhxyThetaMean",fArrayThetaMean);

    fPmtDataMapPmtId.clear();
    fPmtDataMapXY   .clear();

    fmaxX = -1000;
    fmaxY = -1000;

    for(Int_t i = 0; i < fArrayPmt.GetSize()/NPARPMT; i++){
        Int_t j = i*NPARPMT;
	HRich700PmtData pmtData;
	pmtData.fPmtId = (Int_t)fArrayPmt[j+0];
	pmtData.fIndX  = (Int_t)fArrayPmt[j+1];
	pmtData.fIndY  = (Int_t)fArrayPmt[j+2];
	pmtData.fX     = fArrayPmt[j+3];
	pmtData.fY     = fArrayPmt[j+4];
	pmtData.fZ     = fArrayPmt[j+5];
	pmtData.fTheta = fArrayPmt[j+6];
	pmtData.fPhi   = fArrayPmt[j+7];

        if(pmtData.fX > fmaxX) fmaxX = pmtData.fX ;
	if(pmtData.fY > fmaxY) fmaxY = pmtData.fY ;

	pmtData.fSector = getSector( (Float_t)pmtData.fX, (Float_t)pmtData.fY);

	fPmtDataMapPmtId[pmtData.fPmtId] = pmtData;
	pair<Int_t,Int_t> xyInd(pmtData.fIndX, pmtData.fIndY);
	fPmtDataMapXY[xyInd] = pmtData;
    }
}
Int_t HRich700DigiMapPar::getSector(Float_t x, Float_t y)
{
    // In PMT geometry there are no sectors any more ... lets take it for phi
    // phi calculated from x,y on PMT plane
    Float_t phi = TMath::ACos(x/sqrt(x*x+y*y));
    if (y<0) phi = 2.*TMath::Pi()-phi;
    Int_t sectorPhi = (Int_t)(phi/1.0471975)-1;   // get sector from phi angle
    if (sectorPhi == -1) sectorPhi = 5;           // 1st sector is along y axis!
    return sectorPhi;
}

void HRich700DigiMapPar::getLocation(Int_t pmtId, Float_t x, Float_t y, Int_t *loc)
{
    UInt_t ufNofPixelsInRow = (UInt_t)fNofPixelsInRow;

    loc[0]=0; loc[1]=0; loc[2]=0;
    map<Int_t, HRich700PmtData>::iterator it = fPmtDataMapPmtId.find(pmtId);
    if (it == fPmtDataMapPmtId.end()){
	Warning("getLocation","No PMT with id: %i!", pmtId);
	return;
    }

    HRich700PmtData pmtData = it->second;

    Double_t dx = fPmtSize / (Double_t)fNofPixelsInRow;
    Double_t halfPmtSize = fPmtSize / 2.;
    // local pixels address
    UInt_t indX = (UInt_t)((x + halfPmtSize) / dx);
    UInt_t indY = (UInt_t)((y + halfPmtSize)/ dx);

    if (indX >= ufNofPixelsInRow){
	indX = ufNofPixelsInRow - 1;
    }

    if (indY >= ufNofPixelsInRow){
	indY = ufNofPixelsInRow - 1;
    }

    Int_t sectorPhi = getSector(x,y);

    //----------------------------------------------------------------------------------
    // location of the pixel
    loc[0] = sectorPhi;
    loc[1] = indX + pmtData.fIndX * ufNofPixelsInRow; // col
    loc[2] = indY + pmtData.fIndY * ufNofPixelsInRow; // row

}


pair<Double_t, Double_t> HRich700DigiMapPar::getXY(Int_t* loc)
{
    UInt_t ufNofPixelsInRow = (UInt_t)fNofPixelsInRow;

    Int_t indX = loc[1];
    Int_t indY = loc[2];

    Int_t pmtIndX = (Int_t) indX / ufNofPixelsInRow;
    Int_t pmtIndY = (Int_t) indY / ufNofPixelsInRow;

    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(pmtIndX, pmtIndY));
    if (it == fPmtDataMapXY.end()){
	Warning("getXY()","No PMT with XY: %i %i!",pmtIndX,pmtIndY);
	return pair<Double_t, Double_t>(0., 0.);
    }
    HRich700PmtData pmtData = it->second;

    Int_t locIndX = indX % ufNofPixelsInRow;
    Int_t locIndY = indY % ufNofPixelsInRow;

    Double_t dx = fPmtSize / ufNofPixelsInRow;
    Double_t halfPmtSize = fPmtSize / 2.;

    Double_t xLoc = (locIndX + 0.5) * dx - halfPmtSize;
    Double_t yLoc = (locIndY + 0.5) * dx - halfPmtSize;

    pair<Double_t, Double_t> xy(xLoc + pmtData.fX, yLoc + pmtData.fY);
    return xy;
}

pair<Double_t, Double_t> HRich700DigiMapPar::getPmtCenter(Int_t pmtId)
{
    return pair<Double_t, Double_t>(fPmtDataMapPmtId[pmtId].fX, fPmtDataMapPmtId[pmtId].fY);
}

vector<pair<Double_t, Double_t> > HRich700DigiMapPar::getPmtCenters()
{
    vector<pair<Double_t, Double_t> > v;
    v.reserve(fPmtDataMapPmtId.size());
    for(map<Int_t,HRich700PmtData>::iterator it = fPmtDataMapPmtId.begin(); it != fPmtDataMapPmtId.end(); it++) {
	v.push_back(pair<Double_t,Double_t>(it->second.fX, it->second.fY));
    }

    return v;
}

vector<pair<Int_t, Int_t> > HRich700DigiMapPar::getDirectNeighbourPixels(Int_t col, Int_t row)
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

vector<pair<Int_t, Int_t> > HRich700DigiMapPar::getDiagonalNeighbourPixels(Int_t col, Int_t row)
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


vector<pair<Int_t, Int_t> > HRich700DigiMapPar::getNoisePixels(UInt_t nofNoisePixels)
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

Int_t HRich700DigiMapPar::getPMTId  (Float_t x, Float_t y)
{
    if(x>fmaxX) x= fmaxX;
    if(y>fmaxY) y= fmaxY;

    if(x<-fmaxX) x= -fmaxX;
    if(y<-fmaxY) y= -fmaxY;

    Int_t pmtIndX = ( (fmaxX+fPmtSize/2.) - x)/(fPmtSize+fPmtGap);
    Int_t pmtIndY = ( (fmaxY+fPmtSize/2.) - y)/(fPmtSize+fPmtGap);

    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(pmtIndX, pmtIndY));
    if (it != fPmtDataMapXY.end()){
	return it->second.fPmtId;
    } else {
	Warning("getPMTId()","No PMT found for (x,y) = (%f,%f).",x,y);
	return -1;
    }
}

Int_t HRich700DigiMapPar::getPMTId  (Int_t col, Int_t row)
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

HRich700PmtData* HRich700DigiMapPar::getPMTData(Int_t pmtid)
{
    map<Int_t,HRich700PmtData>::iterator it = fPmtDataMapPmtId.find(pmtid);
    if(it == fPmtDataMapPmtId. end()) return NULL;
    return &fPmtDataMapPmtId[pmtid];
}

Int_t  HRich700DigiMapPar::getInterpolatedSectorThetaPhi(Float_t x, Float_t y, Float_t& theta,Float_t& phi)
{
    theta = 0;
    phi   = 0;
    Int_t sector = getSector(x,y);

    phi   = fhxyPhiMean   ->Interpolate(x,y) ;
    theta = fhxyThetaMean ->Interpolate(x,y) ;

    return sector;
}


Bool_t  HRich700DigiMapPar::getInterpolatedThetaPhiPMT(Float_t x, Float_t y, Float_t& theta,Float_t& phi)
{

    Int_t pmtid = getPMTId(x,y);
    if(pmtid < 0) return kFALSE;

    HRich700PmtData& data = fPmtDataMapPmtId[pmtid];
    Int_t iX = data.fIndX;
    Int_t iY = data.fIndY;
    Double_t xcenter     = data.fX;
    Double_t ycenter     = data.fY;
    Double_t phicenter   = data.fPhi;
    Double_t thetacenter = data.fTheta;

    theta = thetacenter;
    phi   = phicenter;

    // indX 0 at max x, indX 23 at min x
    // indY 0 at max y, indY 23 at min Y
    // get neighbour pmts for interpolation
    //                             y indY=0
    //          iy-1               |                       2     1
    //     ix+1 ix,iy ix-1         |-- x indX = 0             c
    //          iy+1                                       3     4
    //
    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it ;

    Double_t x1,x2,y1,y2;
    Double_t p11,p12,p21,p22;
    Double_t t11,t12,t21,t22;

    if(x> xcenter && y >  ycenter){  // right upper
        x1  = xcenter;
        y1  = ycenter;
	p11 = phicenter;
	t11 = thetacenter;

	it = fPmtDataMapXY.find(make_pair(iX-1,iY));
        if (it != fPmtDataMapXY.end()){
	    x2  = it->second.fX;
	    p21 = it->second.fPhi;
	    t21 = it->second.fTheta;
	} else {
	    x2  = xcenter;
	    p21 = phicenter;
	    t21 = thetacenter;
	}

	it = fPmtDataMapXY.find(make_pair(iX,iY-1));
        if (it != fPmtDataMapXY.end()){
	    y2  = it->second.fY;
	    p12 = it->second.fPhi;
	    t12 = it->second.fTheta;
	} else {
	    y2  = ycenter;
	    p12 = phicenter;
	    t12 = thetacenter;
	}

	it = fPmtDataMapXY.find(make_pair(iX-1,iY-1));
        if (it != fPmtDataMapXY.end()){
	    p22 = it->second.fPhi;
	    t22 = it->second.fTheta;
	} else {
	    p22 = phicenter;
	    t22 = thetacenter;
	}

    } else if(x<=xcenter && y >  ycenter){ // left upper
        x2  = xcenter;
        y1  = ycenter;
	p21 = phicenter;
	t21 = thetacenter;

	it = fPmtDataMapXY.find(make_pair(iX+1,iY));
        if (it != fPmtDataMapXY.end()){
	    x1  = it->second.fX;
	    p11 = it->second.fPhi;
	    t11 = it->second.fTheta;
	} else {
	    x1  = xcenter;
	    p11 = phicenter;
	    t11 = thetacenter;
	}

	it = fPmtDataMapXY.find(make_pair(iX,iY-1));
        if (it != fPmtDataMapXY.end()){
	    y2  = it->second.fY;
	    p22 = it->second.fPhi;
	    t22 = it->second.fTheta;
	} else {
	    y2  = ycenter;
	    p22 = phicenter;
	    t22 = thetacenter;
	}

	it = fPmtDataMapXY.find(make_pair(iX-1,iY-1));
        if (it != fPmtDataMapXY.end()){
	    p12 = it->second.fPhi;
	    t12 = it->second.fTheta;
	} else {
	    p12 = phicenter;
	    t12 = thetacenter;
	}

    } else if(x<=xcenter && y <= ycenter){ // left lower
        x2  = xcenter;
        y2  = ycenter;
	p22 = phicenter;
	t22 = thetacenter;

	it = fPmtDataMapXY.find(make_pair(iX+1,iY));
        if (it != fPmtDataMapXY.end()){
	    x1  = it->second.fX;
	    p12 = it->second.fPhi;
	    t12 = it->second.fTheta;
	} else {
	    x1  = xcenter;
	    p12 = phicenter;
	    t12 = thetacenter;
	}

	it = fPmtDataMapXY.find(make_pair(iX,iY+1));
        if (it != fPmtDataMapXY.end()){
	    y1  = it->second.fY;
	    p21 = it->second.fPhi;
	    t21 = it->second.fTheta;
	} else {
	    y1  = ycenter;
	    p21 = phicenter;
	    t21 = thetacenter;
	}

	it = fPmtDataMapXY.find(make_pair(iX+1,iY+1));
        if (it != fPmtDataMapXY.end()){
	    p11 = it->second.fPhi;
	    t11 = it->second.fTheta;
	} else {
	    p11 = phicenter;
	    t11 = thetacenter;
	}

    } else if(x> xcenter && y <= ycenter){ // right lower
        x1  = xcenter;
        y2  = ycenter;
	p12 = phicenter;
	t12 = thetacenter;

	it = fPmtDataMapXY.find(make_pair(iX-1,iY));
        if (it != fPmtDataMapXY.end()){
	    x2  = it->second.fX;
	    p22 = it->second.fPhi;
	    t22 = it->second.fTheta;
	} else {
	    x2  = xcenter;
	    p22 = phicenter;
	    t22 = thetacenter;
	}

	it = fPmtDataMapXY.find(make_pair(iX,iY+1));
        if (it != fPmtDataMapXY.end()){
	    y1  = it->second.fY;
	    p11 = it->second.fPhi;
	    t11 = it->second.fTheta;
	} else {
	    y1  = ycenter;
	    p11 = phicenter;
	    t11 = thetacenter;
	}

	it = fPmtDataMapXY.find(make_pair(iX-1,iY+1));
        if (it != fPmtDataMapXY.end()){
	    p21 = it->second.fPhi;
	    t21 = it->second.fTheta;
	} else {
	    p21 = phicenter;
	    t21 = thetacenter;
	}

    } else {

	x1 = x2 = xcenter;
        y1 = y2 = ycenter;
	p11 = p12 = p21 = p22 = phicenter;
	t11 = t12 = t21 = t22 = thetacenter;

	Warning("Interpolate()","Unknow case. Should not happen!");

    }

    Double_t d = 1.0*(x2-x1)*(y2-y1);

    if(d==0) {
         theta = thetacenter;
	 phi   = phicenter;
         return kTRUE;
    }

    theta = (Float_t)(1.0*t11/d*(x2-x)*(y2-y)+1.0*t21/d*(x-x1)*(y2-y)+1.0*t12/d*(x2-x)*(y-y1)+1.0*t22/d*(x-x1)*(y-y1));
    phi   = (Float_t)(1.0*p11/d*(x2-x)*(y2-y)+1.0*p21/d*(x-x1)*(y2-y)+1.0*p12/d*(x2-x)*(y-y1)+1.0*p22/d*(x-x1)*(y-y1));

    return kTRUE;
}

Int_t HRich700DigiMapPar::getSectorPixels(Int_t col,Int_t row)
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

Int_t HRich700DigiMapPar::getSectorPMTInd(Int_t xind,Int_t yind)
{
    Int_t sectorPhi=0;
    map<pair<Int_t, Int_t>, HRich700PmtData>::iterator it = fPmtDataMapXY.find(make_pair(xind,yind));
    if (it != fPmtDataMapXY.end()){
	sectorPhi = it->second.fSector;
    }
    return sectorPhi;
}

Int_t HRich700DigiMapPar::getSectorPMTId(Int_t pmtid)
{
    Int_t sectorPhi=0;
    map<Int_t,HRich700PmtData> ::iterator it = fPmtDataMapPmtId.find(pmtid);
    if (it != fPmtDataMapPmtId.end()){
	sectorPhi = it->second.fSector;
    }
    return sectorPhi;
}

Int_t HRich700DigiMapPar::getSectorPhiThetaDegPixels(Int_t col,Int_t row,Float_t& phiDeg,Float_t& thetaDeg)
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

Int_t HRich700DigiMapPar::getSectorPhiThetaDegPMTInd(Int_t xind,Int_t yind,Float_t& phiDeg,Float_t& thetaDeg)
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

Int_t HRich700DigiMapPar::getSectorPhiThetaDegPMTId(Int_t pmtid,Float_t& phiDeg,Float_t& thetaDeg)
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










