#ifndef HMDCGARCAL2MAKER_H
#define HMDCGARCAL2MAKER_H
#include "TNamed.h"
#include "TObject.h"

class TFile;
class TDirectory;
class TH1F;
class TH2F;
class TString;
class TGraph;

class HMdcGarCal2Maker : public TNamed {
protected:

    TString fNameAsciiOut;               //! file name of ascii output file
    FILE *output;                        //! file pointer to output
    TString fNameRootIn;                 //! file name of root input file
    TFile *inputRoot;                    //! root file pointer
    TFile *outputHists;                  //! root file pointer

    static Float_t cutMax[4];            //! cut at max drift time
    Float_t slope;                       //! slope of drift time/bin
    Int_t voltage[4];                    //! HV voltage for mdc types
  
    TH1F*   htime1;                      //! hist pointer for time1
    TH1F*   htime2;                      //! hist pointer for time2
    Int_t   lastvalidBin[4][18];         //! counter of non empty bins
    static Int_t   maxbinNumber[72];     //  max number of bins/hist

    Float_t mtime1    [4][18][100];      //! array of time1
    Float_t mtime2    [4][18][100];      //! array of time2
    Float_t mtime1_err[4][18][100];      //! array of error of time1
    Float_t mtime2_err[4][18][100];      //! array of error of time2

    Float_t mtime1_corr    [4][18][100]; //! array of corrected time1
    Float_t mtime2_corr    [4][18][100]; //! array of corrected time2
    Float_t mtime1_err_corr[4][18][100]; //! array of corrected error of time1
    Float_t mtime2_err_corr[4][18][100]; //! array of corrected error of time2

    Float_t max1 [4][18];                //! max 1
    Float_t max2 [4][18];                //! max 2
    Int_t max1bin[4][18];                //! bin max 1
    Int_t max2bin[4][18];                //! bin max 2

    Int_t threshold;                     //! threshold value (*stepsize=%)
    Int_t stepsize;                      //! %/step
    Bool_t isBatch;                      //! switch batch/no batch mode
    Bool_t writeHists;                   //! switch write Hists to cal2maker.root or not
    Bool_t printSpikes;                  //! switch print spikes or not
    Bool_t writeascii;                   //! switch for write/don't write asciie output
    Int_t version;                       //! old/new (1/2) file format
    Float_t maxErrTime1;                 //!
    Float_t maxErrTime2;                 //!
    Int_t removeSpike[4];                //! switch for removing spike types
    Float_t threshold1;                  //! threshold for spike removing
    Float_t threshold2;                  //! threshold for spike removing
    Int_t nTimes[4];                     //! number of smooth's per type

    Int_t nBin1D_dist;                   //!
    Int_t nBin2D_angle;                  //!
    Int_t nBin2D_dist;                   //!
    Int_t minCopy;                       //!
    Int_t maxCopy;                       //!

    Int_t activeHists_1D[4];             //!
    Int_t activeHists_2D[4];             //!

    TGraph* g[4][18][4][3];              //!
    Int_t levelColors[4];                //!
    Int_t makeMdc[4];                    //!
    Int_t makeType[4];                   //!

    void   initVariables();
    void   initArrays()
    {
	for(Int_t m=0;m<4;m++){
	    for(Int_t a=0;a<18;a++){
		for(Int_t b=0;b<100;b++){
		    mtime1    [m][a][b]=0;
		    mtime2    [m][a][b]=0;
		    mtime1_err[m][a][b]=0;
		    mtime2_err[m][a][b]=0;
                    mtime1_corr    [m][a][b]=0;
		    mtime2_corr    [m][a][b]=0;
		    mtime1_err_corr[m][a][b]=0;
		    mtime2_err_corr[m][a][b]=0;
		}
                max1[m][a]=0;
		max2[m][a]=0;
                max1bin[m][a]=0;
		max2bin[m][a]=0;
		lastvalidBin[m][a] =0;
		for(Int_t t=0;t<4;t++){
		    for(Int_t l=0;l<3;l++){
			g[m][a][t][l]=0;
		    }
		}
	    }
	}
    }
public:
    HMdcGarCal2Maker(const Char_t* name="",const Char_t* title="");
    ~HMdcGarCal2Maker();
    void    setFileNameOut(TString);
    void    setFileNameIn (TString);
    void    setCutMax(Float_t c0,Float_t c1,Float_t c2,Float_t c3)
    {
	cutMax[0]=c0;
        cutMax[1]=c1;
        cutMax[2]=c2;
        cutMax[3]=c3;
    }
    Int_t   getVersion()               {return version;}
    void    setVersion(Int_t vers)     {version=vers;}
    Float_t getCutMax(Int_t i)         {return  cutMax[i];}
    void    setSlope(Float_t s0)       {slope=s0;}
    Float_t getSlope()                 {return slope;}
    void    setThreshold(Int_t thresh) {threshold=thresh;}
    Int_t   getThreshold()             {return threshold;}
    void    setStepSize(Int_t size)    {stepsize=size;}
    Int_t   getStepSize()              {return stepsize;}
    void    setBatchMode(Bool_t mode)  {isBatch=mode;}
    Int_t   getBatchMode()             {return isBatch;}
    void    setPrintMode(Bool_t mode)  {printSpikes=mode;}
    Int_t   getPrintMode()             {return printSpikes;}
    void    setWriteHists(Bool_t write){writeHists=write;}
    Bool_t  getWriteHists()            {return writeHists;}
    void    setWriteAscii(Bool_t write){writeascii=write;}
    Bool_t  getWriteAscii()            {return writeascii;}
    void    setMaxErrTime1(Float_t e)  {maxErrTime1=e;}
    Float_t getMaxErrTime1()           {return maxErrTime1;}
    void    setMaxErrTime2(Float_t e)  {maxErrTime2=e;}
    Float_t getMaxErrTime2()           {return maxErrTime2;}
    void    setSpikeTypes(Int_t s0,Int_t s1,Int_t s2,Int_t s3)
    {
	removeSpike[0]=s0;
        removeSpike[1]=s1;
        removeSpike[2]=s2;
        removeSpike[3]=s3;
    }
    Int_t   getSpikeTypes(Int_t i)     {return removeSpike[i];}
    void    setSpikeThresholds(Float_t t1,Float_t t2)
    {
	threshold1=t1;
        threshold2=t2;
    }
    Float_t getSpikeThreshold1()       {return threshold1;}
    Float_t getSpikeThreshold2()       {return threshold2;}
    void    setNTimes(Int_t s0,Int_t s1,Int_t s2,Int_t s3)
    {
	nTimes[0]=s0;
        nTimes[1]=s1;
        nTimes[2]=s2;
        nTimes[3]=s3;
    }
    Int_t   getNTimes(Int_t i)         {return nTimes[i];}
    void    setNBin1D_Dist(Int_t b)    {nBin1D_dist=b;}
    Int_t   getNBin1D_Dist(       )    {return nBin1D_dist;}
    void    setNBin2D_Dist(Int_t b)    {nBin2D_dist=b;}
    Int_t   getNBin2D_Dist()           {return nBin2D_dist;}
    void    setNBin2D_Angle(Int_t b)   {nBin2D_angle=b;}
    Int_t   getNBin2D_Angle()          {return nBin2D_angle;}
    void    setMinCopy(Int_t b)        {minCopy=b;}
    Int_t   getMinCopy()               {return minCopy;}
    void    setMaxCopy(Int_t b)        {maxCopy=b;}
    Int_t   getMaxCopy()               {return maxCopy;}
    void    setHists_1D(Int_t i,Int_t j,Int_t k,Int_t l)
    {
	activeHists_1D[0]=i;
        activeHists_1D[1]=j;
	activeHists_1D[2]=k;
        activeHists_1D[3]=l;
    }
    Int_t   getHists_1D(Int_t i)       {return activeHists_1D[i];}
    void    setHists_2D(Int_t i,Int_t j,Int_t k,Int_t l)
    {
	activeHists_2D[0]=i;
        activeHists_2D[1]=j;
	activeHists_2D[2]=k;
        activeHists_2D[3]=l;
    }
    Int_t   getHists_2D(Int_t i)       {return activeHists_2D[i];}
    void    setMakeMdc(Int_t i,Int_t j,Int_t k,Int_t l)
    {
	makeMdc[0]=i;
        makeMdc[1]=j;
        makeMdc[2]=k;
        makeMdc[3]=l;
    }
    Int_t   getMakeMdc(Int_t i)       {return makeMdc[i];}
    void    setMakeType(Int_t i,Int_t j,Int_t k,Int_t l)
    {
	makeType[0]=i;
        makeType[1]=j;
        makeType[2]=k;
        makeType[3]=l;
    }
    Int_t   getMakeType(Int_t i)       {return makeType[i];}
    void    setLevelColors(Int_t i,Int_t j,Int_t k)
    {
	levelColors[0]=i;
        levelColors[1]=j;
        levelColors[2]=k;
    }
    Int_t   getLevelColors(Int_t i)    {return levelColors[i];}
    void    setVoltage(Int_t s0,Int_t s1,Int_t s2,Int_t s3)
    {
	voltage[0]=s0;
        voltage[1]=s1;
        voltage[2]=s2;
        voltage[3]=s3;
    }
    Int_t   getVoltage(Int_t i)        {return voltage[i];}
    void    setMaxBinNumber(Int_t* array)
    {
	for(Int_t i=0;i<72;i++){
           maxbinNumber[i]=array[i];
	}
    }
    Int_t   getMaxBinNumber(Int_t i)   {return maxbinNumber[i];}
    void    printStatus  (void);
    void    readInput    ();
    void    fillArrays   (TH1F*,TH1F*,Int_t,Int_t);
    void    fillControl2D(Int_t,Int_t,TString="");
    void    fillControl1D(Int_t,Int_t,TString="");
    void    findMax      (Int_t);
    void    removeSpikes (Int_t);
    void    copyToHist   (TH1F*,Int_t,Int_t,Int_t);
    void    copyToArray  (TH1F*,Int_t,Int_t,Int_t);
    Float_t coefSavGol2(Int_t,Int_t,Int_t,Int_t,Int_t,Int_t);
    void    do2dSavitzkyGolayFiltering(Float_t*,Int_t,Int_t,Bool_t);
    void    smoothHist   (TH1F*,Int_t);
    void    fitHist      (TH1F* h,Int_t,Int_t, Int_t);
    void    replaceBins  (TH1F*,Int_t,Int_t,Int_t);
    void    createGraph(Int_t,Int_t,Int_t);
    void    plotOverlay(Int_t,Int_t);
    void    writeAscii();
    Bool_t  check(Int_t,Int_t);
    void    calcBounderies(Int_t,Int_t,Float_t*,Float_t*);
    void    drawBorders(Int_t,Int_t,Int_t);
    void    make();
    ClassDef(HMdcGarCal2Maker,1) // Class for producing the MDC Cal2 parameters
};
#endif  /*!HMDCGARCAL2MAKER_H*/
