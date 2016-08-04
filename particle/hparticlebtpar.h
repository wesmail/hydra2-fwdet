#ifndef HPARTICLEBTPAR_H
#define HPARTICLEBTPAR_H
#include "TArrayF.h"
#include "hparcond.h"

#include <iostream>

using namespace std;


class HParamList;

class HParticleBtPar : public HParCond {

protected:


    Float_t fNSigma;          // nSigma
    Int_t   fSizeMatrix;      // matrix size
    Float_t fPhiOffsetPar;    // phiOffsetPar
    Float_t fMinimumSigmaVal; // Minimum Sigma value for parametrization
    Int_t   fMaximumType;     //MaximumType:7,8,4

    TArrayI fRingFitPar;      // nRingSegments, nVertex, nParMean, nParSigma
    TArrayF fChargeThres;     // sector-wise charge thresholds
    TArrayF fGeomConst;       // padWidthX, padWidthY thetaAngleMin , thetaAngleMax
    TArrayI fRingSeg;         // ringSegStep, ringSegOffset;
    TArrayF fSharedCharge;    // 3 x diagonal pads;3 x vertical pads;3 x hporizontal pads
    TArrayF fSigmaGaus;       // 2 x SigmaGauss;2 x SigmaGaussRange
    TArrayF fParThetaAngle;   // min and max theta angles
    TArrayF fPhiOffset;       // 6 x 2 Phi offsets

    TArrayF fSigmaRange;      // Sigma, Sigma Small, Sigma Max, PadError
    TArrayI fClusterLimits;   // MaximaLimit, PadLimit
    TArrayF fVertexPos;       // pos Min, pos Max, step width
    TArrayI fRichSeg;         // nRichSeg, x-border, y-border
    TArrayI fNeighbourPad;    // up ,upRight, right, downRight, down, downLeft, left, upLeft
    TArrayF fTF2ParMean;      // Radius Parametrization functions (nRingSeg,nVertex,8)
    TArrayF fTF2ParSigma;     // Sigma Parametrization functions (nRingSeg,nVertex,8)
    TArrayF fLeptonSelection; // Cuts: beta RPC, beta TOF, MDC dEdx


public:
    HParticleBtPar(const Char_t* name ="ParticleBtPar",
		   const Char_t* title  ="Parameters for particle back tracking to RICH",
		   const Char_t* context="ParticleBtParProduction");
    ~HParticleBtPar();

    //--------------Get functions----------------

    Int_t          getNRingSegments()                    { return fRingFitPar[0]; }
    Int_t          getNVertex()                          { return fRingFitPar[1]; }
    Int_t          getNParMean()                         { return fRingFitPar[2]; }
    Int_t          getNParSigma()                        { return fRingFitPar[3]; }

    Float_t*       getChargeThres()                      { return &fChargeThres[0];}
    Float_t*       getChargeThresMax()                   { return &fChargeThres[6];}

    Float_t        getNSigma()                           { return fNSigma;}
    Int_t          getMaximumType()                      { return fMaximumType;}
    Int_t          getSizeMatrix()                       { return fSizeMatrix;}
    Int_t          getRingSegStep()                      { return fRingSeg[0];}
    Int_t          getRingSegOffset()                    { return fRingSeg[1];}

    Float_t*       getSharedChargeDiagonal()             { return &fSharedCharge[0];}
    Float_t*       getSharedChargeVertical()             { return &fSharedCharge[3];}
    Float_t*       getSharedChargeHorizontal()           { return &fSharedCharge[6];}

    Float_t*       getSigmaGaus()                        { return &fSigmaGaus[0];}
    Float_t*       getSigmaGausRange()                   { return &fSigmaGaus[2];}
    Float_t        getParThetaAngleMin()                 { return fParThetaAngle[0];}
    Float_t        getParThetaAngleMax()                 { return fParThetaAngle[1];}

    Float_t        getSigmaRange()                       { return fSigmaRange[0];}
    Float_t        getSigmaRangeSmall()                  { return fSigmaRange[1];}
    Float_t        getSigmaRangeMax()                    { return fSigmaRange[2];}
    Float_t        getSigmaRangePadError()               { return fSigmaRange[3];}

    Float_t*       getPhiOffset()                        { return &fPhiOffset[0];}
    Float_t*       getPhiOffset2()                       { return &fPhiOffset[6];}
    Float_t        getOffsetPar()                        { return fPhiOffsetPar;}

    Float_t        getPadWidthX()                        { return fGeomConst[0];}
    Float_t        getPadWidthY()                        { return fGeomConst[1];}
    Float_t        getThetaAngleMin()                    { return fGeomConst[2];}
    Float_t        getThetaAngleMax()                    { return fGeomConst[3];}

   
    Int_t          getClusMaximaLimit()                  { return fClusterLimits[0];}
    Int_t          getClusSizeLimit()                    { return fClusterLimits[1];}

    Float_t        getVertexPosMin()                     { return fVertexPos[0];}
    Float_t        getVertexPosMax()                     { return fVertexPos[1];}
    Float_t        getVertexStep()                       { return fVertexPos[2];}

    Int_t          getNRichSeg()                         { return fRichSeg[0];}
    Int_t          getRichSegBorderX()                   { return fRichSeg[1];}
    Int_t          getRichSegBorderY()                   { return fRichSeg[2];}

    Int_t          getNeighbourPad(Int_t par)            { return fNeighbourPad[0+par];}

    Float_t        getMinimumSigmaValue()                { return fMinimumSigmaVal;}
    Double_t       getTF2ParMean(Int_t ringSeg, Int_t vertex, Int_t par) { return fTF2ParMean[ringSeg*(15*8)+vertex*8+par];}
    Double_t       getTF2ParSigma(Int_t ringSeg, Int_t vertex, Int_t par) { return fTF2ParSigma[ringSeg*(15*13)+vertex*13+par];}

    Float_t        getBetaRPCLimit()                     { return fLeptonSelection[0];}
    Float_t        getBetaTOFLimit()                     { return fLeptonSelection[1];}
    Float_t        getMDCdEdxLimit()                     { return fLeptonSelection[2];}




    //---------------Set functions---------------

    void    setNRingSegments(Int_t val)                         {  fRingFitPar[0] = val; }
    void    setNVertex(Int_t val)                               {  fRingFitPar[1] = val; }
    void    setNParMean(Int_t val)                              {  fRingFitPar[2] = val; }
    void    setNParSigma(Int_t val)                             {  fRingFitPar[3] = val; }

    void    setChargeThres(Float_t* pars)                       {  for(Int_t i=0;i<6;i++) fChargeThres[i]   = pars[i];}
    void    setChargeThresMax(Float_t* pars)                    {  for(Int_t i=0;i<6;i++) fChargeThres[i+6] = pars[i];}

    void    setNSigma(Float_t val)                              {  fNSigma = val;}
    void    setMaximumType(Int_t val)                           {  fMaximumType = val;}
    void    setSizeMatrix(Int_t val)                            {  fSizeMatrix = val;}
    void    setRingSegStep(Int_t val)                           {  fRingSeg[0] = val;}
    void    setRingSegOffset(Int_t val)                         {  fRingSeg[1] = val;}

    void    setSharedChargeDiagonal(Float_t* pars)              {  for(Int_t i=0;i<3;i++) fSharedCharge[0+i] = pars[i];}
    void    setSharedChargeVertical(Float_t* pars)              {  for(Int_t i=0;i<3;i++) fSharedCharge[3+i] = pars[i];}
    void    setSharedChargeHorizontal(Float_t* pars)            {  for(Int_t i=0;i<3;i++) fSharedCharge[6+i] = pars[i];}

    void    setSigmaGaus(Float_t val1,Float_t val2)             {  fSigmaGaus[0] = val1; fSigmaGaus[1] = val2;}
    void    setSigmaGausRange(Float_t val1,Float_t val2)        {  fSigmaGaus[2] = val1; fSigmaGaus[3] = val2;}
    void    setParThetaAngleMin(Float_t val)                    {  fParThetaAngle[0] = val;}
    void    setParThetaAngleMax(Float_t val)                    {  fParThetaAngle[1] = val;}

    void    setSigmaRange(Float_t val)                          {  fSigmaRange[0] = val;}
    void    setSigmaRangeSmall(Float_t val)                     {  fSigmaRange[1] = val;}
    void    setSigmaRangeMax(Float_t val)                       {  fSigmaRange[2] = val;}
    void    setSigmaRangePadError(Float_t val)                  {  fSigmaRange[3] = val;}

    void    setPhiOffset(Float_t* pars)                         {  for(Int_t i=0;i<6;i++) fPhiOffset[0+i] = pars[i];}
    void    setPhiOffset2(Float_t* pars)                        {  for(Int_t i=0;i<6;i++) fPhiOffset[6+i] = pars[i];}
    void    setOffsetPar(Float_t val)                           {  fPhiOffsetPar = val;}

    void    setPadWidthX(Float_t val)                           {  fGeomConst[0] = val;}
    void    setPadWidthY(Float_t val)                           {  fGeomConst[1] = val;}
    void    setThetaAngleMin(Float_t val)                       {  fGeomConst[2] = val;}
    void    setThetaAngleMax(Float_t val)                       {  fGeomConst[3] = val;}

    void    setClusMaximaLimit(Int_t val)                       {  fClusterLimits[0] = val;}
    void    setClusSizeLimit(Int_t val)                         {  fClusterLimits[1] = val;}

    void    setVertexPosMin(Float_t val)                        {  fVertexPos[0] = val;}
    void    setVertexPosMax(Float_t val)                        {  fVertexPos[1] = val;}
    void    setVertexStep(Float_t val)                          {  fVertexPos[2] = val;}

    void    setNRichSeg(Int_t val)                              {  fRichSeg[0] = val;}
    void    setRichSegBorderX(Int_t val)                        {  fRichSeg[1] = val;}
    void    setRichSegBorderY(Int_t val)                        {  fRichSeg[2] = val;}

    void    setNeighbourPad(Int_t val,Int_t par)                {  fNeighbourPad[0+par] = val;}

    void    setMinimumSigmaValue(Float_t val)                   { fMinimumSigmaVal = val;}
    void    setTF2ParMean(Int_t ringSeg, Int_t vertex, Int_t par, Double_t val)  { fTF2ParMean[(ringSeg*15*8)+(vertex*8)+par]  = val;}
    void    setTF2ParSigma(Int_t ringSeg, Int_t vertex, Int_t par, Double_t val) { fTF2ParSigma[ringSeg*(15*13)+vertex*13+par] = val;}

    void    setBetaRPCLimit(Float_t val )                       { fLeptonSelection[0] = val;}
    void    setBetaTOFLimit(Float_t val )                       { fLeptonSelection[1] = val;}
    void    setMDCdEdxLimit(Float_t val )                       { fLeptonSelection[2] = val;}



    //--------------------------



    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();

    ClassDef(HParticleBtPar,1) // Container for the particleBT parameters

};
#endif  /*!HPARTICEBTPAR_H*/
