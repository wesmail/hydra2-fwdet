//#include "hparticlebtpar.h"
#include "hparticlebtpar.cc"
#include "hades.h"
#include "hparora2io.h"
#include "hparasciifileio.h"
#include "hruntimedb.h"

void fillParContainer(){

    Hades* hades = new Hades();

    HRuntimeDb* rtdb = gHades->getRuntimeDb();

    HParOra2Io* ora=new HParOra2Io;
    ora->open();
    rtdb->setFirstInput(ora);

    HParAsciiFileIo* output=new HParAsciiFileIo;
    output->open("/hera/hades/user/sellheim/svn/hydra_dev/hparticlebtparams.txt","out");
    rtdb->setOutput(output);

    HParticleBtPar* pRingPar = new HParticleBtPar();

   
    pRingPar->       setNRingSegments(45);
    pRingPar->       setNVertex(15);
    pRingPar->       setNParMean(8);
    pRingPar->       setNParSigma(13);
   
    Float_t chargeThres[6]     = {65.,65.,65.,55.,65.,65.};
    Float_t chargeThresMax[6]  = {75.,75.,75.,65.,75.,75.};
    pRingPar->       setChargeThres(chargeThres);
    pRingPar->       setChargeThresMax(chargeThresMax);
    pRingPar->       setNSigma(3);
    pRingPar->       setSizeMatrix(16);
    pRingPar->       setRingSegStep(8) ;
    pRingPar->       setRingSegOffset(4);
   
    Float_t chargeParDiagonal[3]   = {5.54835e-02 ,3.01966e-02 ,-8.74852e-01 };
    Float_t chargeParVertical[3]   = {3.84285e+00 ,2.00086e-03 ,-7.04712e-01 };
    Float_t chargeParHorizontal[3] = {-2.09239e+02 , 4.44753e-06 ,-4.47654e-01 };
    pRingPar->       setSharedChargeDiagonal(chargeParDiagonal);
    pRingPar->       setSharedChargeVertical(chargeParVertical);
    pRingPar->       setSharedChargeHorizontal(chargeParHorizontal);

    
    pRingPar->       setSigmaGaus(0.299,0.315);
    pRingPar->       setSigmaGausRange(3*8.6e-3,3*1.35e-3);
    pRingPar->       setParThetaAngleMin(20.);
    pRingPar->       setParThetaAngleMax(85.);
    pRingPar->       setSigmaRange(3.);
    pRingPar->       setSigmaRangeSmall(3.);
    pRingPar->       setSigmaRangeMax(6.);
    pRingPar->       setSigmaRangePadError(0.5);
    
    Float_t phiOff[6]  ={90.,150.,210.,270.,330.,30.};
    Float_t phiOff2[6] ={60.,120.,180.,240.,300.,0.};
    pRingPar->       setPhiOffset(phiOff);
    pRingPar->       setPhiOffset2(phiOff2);
    pRingPar->       setOffsetPar(30);
    
    pRingPar->       setPadWidthX(6.6);
    pRingPar->       setPadWidthX(5.7);//Mean Value
    pRingPar->       setThetaAngleMin(15.);
    pRingPar->       setThetaAngleMax(85.);

    pRingPar->       setClusMaximaLimit(8);
    pRingPar->       setClusSizeLimit(25);

    pRingPar->       setVertexPosMin(-55.);
    pRingPar->       setVertexPosMax(-51.5);
    pRingPar->       setVertexStep(-3.5);

    pRingPar->       setNRichSeg(3);
    pRingPar->       setRichSegBorderX(48);
    pRingPar->       setRichSegBorderY(33);
    

    pRingPar->setNeighbourPad(1    ,0);
    pRingPar->setNeighbourPad(101  ,1);
    pRingPar->setNeighbourPad(100  ,2);
    pRingPar->setNeighbourPad(99   ,3);
    pRingPar->setNeighbourPad(-1   ,4);
    pRingPar->setNeighbourPad(-101 ,5);
    pRingPar->setNeighbourPad(-100 ,6);
    pRingPar->setNeighbourPad(-99  ,7);


    pRingPar->setMinimumSigmaValue(0.5);

    pRingPar->setBetaRPCLimit(0.95);
    pRingPar->setBetaTOFLimit(0.92);
    pRingPar->setMDCdEdxLimit(10.0);




    const Int_t nRingSeg  = 45;
    const Int_t nVertex   = 15;
    const Int_t nParMean  =  8;
    const Int_t nParSigma =  13;

    Float_t paramsMean[10000];
    Float_t paramsSigma[10000];
    Int_t nParamsMean  = 0;
    Int_t nParamsSigma = 0;

        
    TString path="TF2ParOutputMean.txt";
    ifstream in (path);
    while(in.good()) {
	in >> paramsMean[nParamsMean];
	nParamsMean++;
    }
    in.close();
    cout << nParamsMean-1 << " parameters successfully loaded for TF2 Mean function" << endl;
     
    path="TF2ParOutputSigma.txt";
    ifstream in2 (path);
    while(in2.good()) {
	in2 >> paramsSigma[nParamsSigma];
	nParamsSigma++;
    }
    in2.close();
    cout << nParamsSigma-1 << " parameters successfully loaded for TF2 Sigma function" << endl;
    
    Int_t bin        = 0;
    Int_t vertex     = 0;
    Int_t par        = 0;
    cout << nParamsMean << " " << nParamsSigma << endl;
    for(Int_t i = 0; i < nParamsMean-1 ; i++ ){
	cout <<"==== " << i << " ==== ";
        cout << bin << " " << vertex << " " << par << " " << paramsMean[i] << endl;
	pRingPar->setTF2ParMean(bin,vertex,par,paramsMean[i]);

	par++;
	if(par%nParMean == 0 && par!=0){
	    par = 0;
            vertex++;
	}
	if(vertex%nVertex == 0 && vertex!=0){
	    vertex = 0;
	    bin++;
	}
	if(bin%nRingSeg == 0 && bin!=0){
	    bin = 0;
	}

    }

    bin        = 0;
    vertex     = 0;
    par        = 0;
    for(Int_t i = 0; i < nParamsSigma-1 ; i++ ){
        pRingPar->setTF2ParSigma(bin,vertex,par,paramsSigma[i]);

	par++;
	if(par%nParSigma == 0 && par!=0){
	    par = 0;
            vertex++;
	}
	if(vertex%nVertex == 0 && vertex!=0){
	    vertex = 0;
	    bin++;
	}
	if(bin%nRingSeg == 0 && bin!=0){
	    bin = 0;
	}
    }

    pRingPar->write(output);
    rtdb->saveOutput();
    rtdb->print();
    rtdb->closeOutput();

}
