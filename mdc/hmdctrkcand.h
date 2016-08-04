#ifndef HMDCTRKCAND_H
#define HMDCTRKCAND_H

#include "TObject.h"

class HMdcTrkCand : public TObject {
protected:
  Char_t  sector;               // sector number
  Char_t  flag;                 // user flag
  Short_t nCandForSeg1;         // number of HMdcTrkCand objects 
                                // for this inner segmen
                                // =0 - no outer segment
                                // >0 this container is first
                                // =-1 one of the next containers
  Int_t   nextCandInd;          // index of next HMdcTrkCand object
                                // =-1 - this object is latest
  Int_t   firstCandInd;         // index of the first HMdcTrkCand object with
                                // the same inner segment
  Int_t   seg1Ind;              // index of HMdcSeg container (inner segment)
  Int_t   seg2Ind;              // index of HMdcSeg container (outer segment)
  Int_t   metaMatchInd;         // first HMetaMatch obj.index created for this

  Float_t dedxInner;            // mean value of t2-t1 for inner segment
  Float_t dedxSigmaInner;       // sigma of t2-t1 distribution in inner segment
  UChar_t dedxNWireInner;       // number of wires in inner segment before truncated mean procedure
  UChar_t dedxNWireCutInner;    // number of wires in inner segment cutted by truncated mean procedure
  Float_t dedxOuter;            // mean value of t2-t1 for outer segment
  Float_t dedxSigmaOuter;       // sigma of t2-t1 distribution in outer segment
  UChar_t dedxNWireOuter;       // number of wires in outer segment before truncated mean procedure
  UChar_t dedxNWireCutOuter;    // number of wires in outer segment cutted by truncated mean procedure
  Float_t dedxCombined;         // mean value of t2-t1 for inner+outer segment
  Float_t dedxSigmaCombined;    // sigma of t2-t1 distribution in inner+outer segment
  UChar_t dedxNWireCombined;    // number of wires in inner+outer segment before truncated mean procedure
  UChar_t dedxNWireCutCombined; // number of wires in inner+outer segment cutted by truncated mean procedure

  Float_t dedx[4];              // mean value of t2-t1 per module
  Float_t dedxSigma[4];         // sigma of t2-t1 distribution per module
  UChar_t dedxNWire[4];         // number of wires per module before truncated mean procedure
  UChar_t dedxNWireCut[4];      // number of wires per module cutted by truncated mean procedure

  enum {kIsSeg1Fake = BIT(14),  // bit in TObject::fBits for "fake" inner segment flag 
        kIsSeg2Fake = BIT(15)}; // bit in TObject::fBits for "fake" outer segment flag 
private: 
  HMdcTrkCand* pFirstCand;   //! first HMdcTrkCand object with
                             //! the same inner segment

public:
  HMdcTrkCand(void) {clear();}
  HMdcTrkCand(Char_t sec, Int_t s1ind, Int_t ind);
  HMdcTrkCand(HMdcTrkCand* fTrkCand, Int_t s2ind, Int_t ind);
  ~HMdcTrkCand(void) {}
  void clear(void);
  
  void setSec(Char_t s)                 {sector=s;}
  void setSeg1Ind(Int_t ind)            {seg1Ind=ind;}
  void setSeg2Ind(Int_t ind)            {seg2Ind=ind;}
  void setNCandForSeg1(Short_t n)       {nCandForSeg1=n;}
  void setFirstCandInd(Int_t i)         {firstCandInd=i;}
  void setNextCandInd(Int_t n)          {nextCandInd=n;}
  void setMetaMatchInd(Int_t ind)       {metaMatchInd=ind;}
  void addSeg2Ind(Int_t ind)            {seg2Ind=ind; nCandForSeg1=1;}
  void setFlag(Char_t fl)               {flag=fl;}
  
  void setFakeFlagSeg1(void)            { SetBit(kIsSeg1Fake); }
  void setFakeFlagSeg2(void)            { SetBit(kIsSeg2Fake); }
  void resetFakeFlagSeg1(void)          { ResetBit(kIsSeg1Fake); }
  void resetFakeFlagSeg2(void)          { ResetBit(kIsSeg2Fake); }
  Bool_t isSeg1Fake(void) const         { return TestBit(kIsSeg1Fake); }
  Bool_t isSeg2Fake(void) const         { return TestBit(kIsSeg2Fake); }
  Bool_t isFake(void) const             { return TestBit(kIsSeg1Fake) || TestBit(kIsSeg2Fake); }
  

  void setdedx(Int_t mod,Float_t dEdx)      {dedx        [mod]=dEdx;}
  void setSigmadedx(Int_t mod,Float_t Sig)  {dedxSigma   [mod]=Sig;}
  void setNWirededx(Int_t mod,UChar_t nW)   {dedxNWire   [mod]=nW;}
  void setNWireCutdedx(Int_t mod,UChar_t nW){dedxNWireCut[mod]=nW;}


  void setdedxInnerSeg(Float_t dedx)    {dedxInner=dedx;}
  void setdedxOuterSeg(Float_t dedx)    {dedxOuter=dedx;}
  void setdedxCombinedSeg(Float_t dedx) {dedxCombined=dedx;}
  void setdedxSeg(Int_t seg,Float_t dedx)
  {
      if(seg==0)     {dedxInner=dedx;}
      else if(seg==1){dedxOuter=dedx;}
      else if(seg==2){dedxCombined=dedx;}
  }
  void setSigmadedxInnerSeg(Float_t sig)    {dedxSigmaInner=sig;}
  void setSigmadedxOuterSeg(Float_t sig)    {dedxSigmaOuter=sig;}
  void setSigmadedxCombinedSeg(Float_t sig) {dedxSigmaCombined=sig;}
  void setSigmadedxSeg(Int_t seg,Float_t sig)
  {
      if(seg==0)     {dedxSigmaInner=sig;}
      else if(seg==1){dedxSigmaOuter=sig;}
      else if(seg==2){dedxSigmaCombined=sig;}
  }
  void setNWirededxInnerSeg(UChar_t nw){dedxNWireInner=nw;}
  void setNWirededxOuterSeg(UChar_t nw){dedxNWireOuter=nw;}
  void setNWirededxCombinedSeg(UChar_t nw){dedxNWireCombined=nw;}
  void setNWirededxSeg(Int_t seg,UChar_t nw)
  {
      if(seg==0)     {dedxNWireInner=nw;}
      else if(seg==1){dedxNWireOuter=nw;}
      else if(seg==2){dedxNWireCombined=nw;}
  }
  void setNWireCutdedxInnerSeg(UChar_t nw){dedxNWireCutInner=nw;}
  void setNWireCutdedxOuterSeg(UChar_t nw){dedxNWireCutOuter=nw;}
  void setNWireCutdedxCombinedSeg(UChar_t nw){dedxNWireCutCombined=nw;}
  void setNWireCutdedxSeg(Int_t seg,UChar_t nw)
  {
      if(seg==0)     {dedxNWireCutInner=nw;}
      else if(seg==1){dedxNWireCutOuter=nw;}
      else if(seg==2){dedxNWireCutCombined=nw;}
  }


  Char_t  getSec(void) const     {return sector;}
  Int_t   getSeg1Ind(void) const {return seg1Ind;}
  Int_t   getSeg2Ind(void) const {return seg2Ind;}
  Int_t   getSegInd(const Int_t seg) const {
    return (seg==0) ? seg1Ind : ((seg==1) ? seg2Ind:-1); }
  Short_t getNCandForSeg1(void) const    {return nCandForSeg1;}
  Int_t   getNextCandInd(void) const     {return nextCandInd;}
  Int_t   getFirstCandInd(void) const    {return firstCandInd;}
  Int_t   getMetaMatchInd(void) const    {return metaMatchInd;}
  Char_t  getFlag(void) const            {return flag;}

  Float_t getdedx(Int_t mod)        {return dedx        [mod];}
  Float_t getSigmadedx(Int_t mod)   {return dedxSigma   [mod];}
  UChar_t getNWirededx(Int_t mod)   {return dedxNWire   [mod];}
  UChar_t getNWireCutdedx(Int_t mod){return dedxNWireCut[mod];}

  Float_t getdedxInnerSeg(void) const    {return dedxInner;}
  Float_t getdedxOuterSeg(void) const    {return dedxOuter;}
  Float_t getdedxCombinedSeg(void) const {return dedxCombined;}
  Float_t getdedxSeg(Int_t seg)
  {
      if(seg==0)     {return dedxInner;}
      else if(seg==1){return dedxOuter;}
      else if(seg==2){return dedxCombined;}
      else {return -1;}
  }
  Float_t getSigmadedxInnerSeg(){return dedxSigmaInner;}
  Float_t getSigmadedxOuterSeg(){return dedxSigmaOuter;}
  Float_t getSigmadedxCombinedSeg(){return dedxSigmaCombined;}
  Float_t getSigmadedxSeg(Int_t seg)
  {
      if(seg==0)     {return dedxSigmaInner;}
      else if(seg==1){return dedxSigmaOuter;}
      else if(seg==2){return dedxSigmaCombined;}
      else {return -1;}
  }
  UChar_t getNWirededxInnerSeg()    {return dedxNWireInner;}
  UChar_t getNWirededxOuterSeg()    {return dedxNWireOuter;}
  UChar_t getNWirededxCombinedSeg() {return dedxNWireCombined;}
  UChar_t getNWirededxSeg(Int_t seg)
  {
      if(seg==0)     {return dedxNWireInner;}
      else if(seg==1){return dedxNWireOuter;}
      else if(seg==2){return dedxNWireCombined;}
      else {return 0;}
  }
  UChar_t getNWireCutdedxInnerSeg(){return dedxNWireCutInner;}
  UChar_t getNWireCutdedxOuterSeg(){return dedxNWireCutOuter;}
  UChar_t getNWireCutdedxCombinedSeg(){return dedxNWireCutCombined;}
  UChar_t getNWireCutdedxSeg(Int_t seg)
  {
      if(seg==0)     {return dedxNWireCutInner;}
      else if(seg==1){return dedxNWireCutOuter;}
      else if(seg==2){return dedxNWireCutCombined;}
      else {return 0;}
  }
  virtual void print(void) const;
  
  ClassDef(HMdcTrkCand,1) // MDC track candidate
};

#endif  /* HMDCTRKCAND_H */
