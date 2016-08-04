#ifndef HTOFWALKPAR_H
#define HTOFWALKPAR_H
#include "hparcond.h"


#include "TArrayF.h"
#include "TArrayD.h"
#include "TF1.h"


class  HTofDigiPar;
class  HTofCalPar;
class  HTofGeomPar;
class  HParamList;
class  HTofRaw;

class HTofWalkPar : public HParCond {
protected:
    TArrayF fWPars;      // Parameters for the walking effect correction.
    TArrayF fWErrors;    // Parameters for the errors for each cell and side.
    TArrayI fSideIsUsed; // Array for the working or not properly PMT+FEE


    //----------------------------------------------
    // RK-dx normalization params
    TArrayD fParsSX;               // linearized sec*mod*cell*3
    TArrayD fParsDX;               // linearized sec*mod*cell*5
    TArrayD fParsGeantToData;      // slope + offset to align geant charge with data charge
    TArrayD fParsGroupVelocityData;// vgroup real data (used to scale sigmaX to take care about other vgroup in sim)
    TArrayD fParsGroupVelocityScale; // vgroup global scaleing
    //----------------------------------------------


    //Methods and members for the use of this container in the HTofHitF2

    HTofDigiPar *fDigiPar;                        //!
    HTofCalPar  *fCalPar;                         //!
    HTofGeomPar *fTofGeometry;                    //!
    TF1* func;                                    //!
    static Double_t ffunc(Double_t* x, Double_t* par);

    TF1* fdxsigma;           //!
    TF1* fdxoffset;          //!
    TF1* fscaleDxSigma;      //!
public:
    HTofWalkPar(const Char_t* name   ="TofWalkPar",
		const Char_t* title  ="Walking correciton parameters for the TOF Detector",
		const Char_t* context="TofWalkParProduction");
    ~HTofWalkPar();
    const   Float_t*  getPars  (Int_t sec, Int_t mod, Int_t cell, Int_t side, Int_t bin) { return &fWPars     [8*8*4*20*9*sec + 8*4*20*9*mod + 4*20*9*cell + 20*9*side + 9*bin];}
            Float_t   getError (Int_t sec, Int_t mod, Int_t cell, Int_t side, Int_t bin) { return  fWErrors   [8*8*4*20  *sec + 8*4*20  *mod + 4*20  *cell + 20  *side + bin];}
            Float_t   getIsUsed(Int_t sec, Int_t mod, Int_t cell, Int_t side)            { return  fSideIsUsed[8*8*2     *sec + 8*2     *mod + 2     *cell +      side];}
    const   Double_t* getParsSX(Int_t sec, Int_t mod, Int_t cell)                        { return &fParsSX    [8*8*3*sec + 8*3*mod + 3*cell];}
    const   Double_t* getParsDX(Int_t sec, Int_t mod, Int_t cell)                        { return &fParsDX    [8*8*5*sec + 8*5*mod + 5*cell];}
    const   Double_t* getParsGeantToData()                                               { return &fParsGeantToData [0];}
            Double_t  getGroupVelocity(Int_t sec, Int_t mod, Int_t cell)                 { return  fParsGroupVelocityData[8*8*sec + 8*mod + cell];}
            Double_t* getGroupVelocityScale()                                            { return &fParsGroupVelocityScale[0]; }
    void    setPars  (Int_t sec, Int_t mod, Int_t cell, Int_t side, Int_t bin, Int_t npar, Float_t* pars)  { for(Int_t i =0;i<npar;i++){ fWPars[8*8*4*20*9*sec + 8*4*20*9*mod + 4*20*9*cell + 20*9*side + 9*bin + i ] = pars[i];} }
    void    setErrors(Int_t sec, Int_t mod, Int_t cell, Int_t side, Int_t bin, Float_t par)  { fWErrors   [8*8*4*20*sec + 8*4*20*mod + 4*20*cell + 20*side + bin] = par; }
    void    setIsUsed(Int_t sec, Int_t mod, Int_t cell, Int_t side,Float_t par)              { fSideIsUsed[8*8*2   *sec + 8*2   *mod + 2   *cell +    side] = par; }
    void    setParsSX(Int_t sec, Int_t mod, Int_t cell, Double_t* pars)                      { for(Int_t i =0;i<3;i++){ fParsSX[8*8*3*sec + 8*3*mod + 3*cell + i ] = pars[i];} }
    void    setParsDX(Int_t sec, Int_t mod, Int_t cell, Double_t* pars)                      { for(Int_t i =0;i<5;i++){ fParsDX[8*8*5*sec + 8*5*mod + 5*cell + i ] = pars[i];} }
    void    setParsGeantToData( Double_t* pars)                                              { for(Int_t i =0;i<2;i++){ fParsGeantToData[ i ] = pars[i];} }
    void    setGroupVelocity(Int_t sec, Int_t mod, Int_t cell,Double_t vel)                  { fParsGroupVelocityData[8*8*sec + 8*mod + cell] = vel;}
    void    setGroupVelocityScale(Double_t* scale)                                           { for(Int_t i =0;i<3;i++) {fParsGroupVelocityScale[i] = scale[i]; }  }

    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();


    //Methods and members for the use of this container in the HTofHitF2
    void    getContainers();
    Float_t getOffset (Float_t x, Int_t side, Float_t q, Int_t sector, Int_t module, Int_t cell);
    void    getOffsets(Float_t x, Float_t ql, Float_t qr,Int_t sector, Int_t module, Int_t cell, Float_t& const1,Float_t& const2,Float_t& const3,Float_t& const4);
    void    getErrors (Float_t x, Float_t ql, Float_t qr,Int_t sector, Int_t module, Int_t cell, Float_t& const1,Float_t& const2,Float_t& const3,Float_t& const4);
    void    getTofPos (HTofRaw* tofraw,Float_t& tof,Float_t& pos, Float_t x, Float_t startTMP);

    Double_t getDxSigma (Int_t s,Int_t m,Int_t c,Double_t eloss);
    Double_t getDxSigmaDigi (Int_t s,Int_t m,Int_t c,Double_t eloss,Double_t vgroup,Double_t beta);
    Double_t getDxOffset(Int_t s,Int_t m,Int_t c,Double_t eloss);
    Double_t getNormedDX(Int_t s,Int_t m,Int_t c,Double_t eloss,Double_t rkDX,Bool_t sim);

    Double_t scaleGeantToData(Double_t charge){ return charge*fParsGeantToData[0]+fParsGeantToData[1]; }

    ClassDef(HTofWalkPar,2) // Container for the TOF hit finder 2 parameters
};
#endif  /*!HTOFWALKPAR_H*/
