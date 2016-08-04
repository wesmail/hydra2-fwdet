#ifndef HMDCTRACKFITTERH_H
#define HMDCTRACKFITTERH_H

#include "hmdctrackfitter.h"

class HMdcTrackFitterH : public HMdcTrackFitter {
  private:
    Double_t stepNorm;          // Normalisation factor for step
    Double_t weightPar[6];      // Array with weight for parameters
    Double_t step[6];           // Array with steps for chanching of parameters
  public:
    HMdcTrackFitterH(HMdcTrackFitInOut* fIO) : HMdcTrackFitter(fIO) {;}
    virtual ~HMdcTrackFitterH(void);
    virtual Int_t execute(Double_t threshold, Int_t iter=0);
    virtual Bool_t finalize(void);
    virtual Double_t getFunctional(void);

  private:
    void setParam(void); 
    void fillOutput(void);
    void printParam(Char_t* title) const;
    void printResult(Char_t* status) const;
    
    Double_t calculationOfFunctional(Int_t iflag);
    Double_t deviation(Double_t xVertex, Double_t yVertex, Double_t zVertex,
		       Double_t xPlane, Double_t yPlane, Double_t zPlane,
		       Double_t arriveTime, Int_t iflag);

    ClassDef(HMdcTrackFitterH,0)
};

#endif
