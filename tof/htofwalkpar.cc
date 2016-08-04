//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : G. Kornakov
//
// HTofWalkPar
//
// Container class for the TOF walking correction in HTofHitF2
//
// (Condition Style)
//
// The walking effect is described by a functional with two clearly
// definded regions by a constant. Then the first part, at low charges
// is described by an exponential and the second part is described by an
// linear combiantion of an exponential and an potential function.
//
////////////////////////////////////////////////////////////////////////////
#include "htofwalkpar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hparamlist.h"
#include "htofdigipar.h"
#include "htofcalpar.h"
#include "htofgeompar.h"
#include "htofraw.h"

ClassImp(HTofWalkPar)

HTofWalkPar::HTofWalkPar(const Char_t* name,const Char_t* title,
                       const Char_t* context)
    : HParCond(name,title,context)
{
    fWPars.Set(6*8*8*4*20*9); // 6 sectors x 8 modules x 8 cells x (2 x 2) sides x 20 bins x 9 parameters
    fWErrors.Set(6*8*8*4*20); // 6 sectors x 8 modules x 8 cells x (2 x 2) sides x 20 bins x 1 parameters
    fSideIsUsed.Set(6*8*8*2);
    fParsSX.Set(6*8*8*3);
    fParsDX.Set(6*8*8*5);
    fParsGeantToData.Set(2);
    fParsGroupVelocityData.Set(6*8*8);
    fParsGroupVelocityScale =1;
    func          = new TF1("func",HTofWalkPar::ffunc,0,5000,9);

    fdxsigma      = new TF1("fdxsigma" ,"[0]+[1]*x**[2]",0,25);
    fdxoffset     = new TF1("fdxoffset","[0]+[1]*exp(x*[2])+[3]*exp(x*[4])",0,25);
    fscaleDxSigma = new TF1("fscaleDxSigma","pol2",0,1);
    clear();
}
HTofWalkPar::~HTofWalkPar()
{
    // destructor

    delete fdxsigma;
    delete fdxoffset;
    delete fscaleDxSigma;
    delete func;

}
void HTofWalkPar::getContainers()
{
    fCalPar      =(HTofCalPar  *)gHades->getRuntimeDb()->getContainer("TofCalPar");
    fTofGeometry =(HTofGeomPar *)gHades->getRuntimeDb()->getContainer("TofGeomPar");
    fDigiPar     =(HTofDigiPar *)gHades->getRuntimeDb()->getContainer("TofDigiPar");
}

void HTofWalkPar::clear()
{

    fWPars.Reset(0.);
    fWErrors.Reset(0.);
    fSideIsUsed.Reset(1);
    fParsSX.Reset(0);
    fParsDX.Reset(0);
    fParsGeantToData.Reset(0);
    fParsGroupVelocityData.Reset(0);
    fParsGroupVelocityScale =1;
    status = kFALSE;
    resetInputVersions();
    changed = kFALSE;

}
void HTofWalkPar::putParams(HParamList* l)
{
    // Puts all params of HTofWalkPar to the parameter list of
    // HParamList (which is used by the io);
    if (!l) return;
    l->add("fWPars"                 ,fWPars      );
    l->add("fWErrors"               ,fWErrors    );
    l->add("fSideIsUsed"            ,fSideIsUsed );
    l->add("fParsSX"                ,fParsSX      );
    l->add("fParsDX"                ,fParsDX      );
    l->add("fParsGeantToData"       ,fParsGeantToData);
    l->add("fParsGroupVelocityData" ,fParsGroupVelocityData);
    l->add("fParsGroupVelocityScale",fParsGroupVelocityScale);

}
Bool_t HTofWalkPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("fWPars"                 , &fWPars     ))) return kFALSE;
    if(!( l->fill("fWErrors"               , &fWErrors   ))) return kFALSE;
    if(!( l->fill("fSideIsUsed"            , &fSideIsUsed))) return kFALSE;
    if(!( l->fill("fParsSX"                , &fParsSX    ))) return kFALSE;
    if(!( l->fill("fParsDX"                , &fParsDX    ))) return kFALSE;
    if(!( l->fill("fParsGeantToData"       , &fParsGeantToData))) return kFALSE;
    if(!( l->fill("fParsGroupVelocityData" , &fParsGroupVelocityData))) return kFALSE;
    if(!( l->fill("fParsGroupVelocityScale", &fParsGroupVelocityScale))) return kFALSE;

    return kTRUE;
}

Double_t HTofWalkPar::ffunc(Double_t* x, Double_t* par)
{
    if(x[0]<par[8]) return par[2]+par[0]*exp(x[0]*par[1]);
    else            return par[7]+par[3]*exp(x[0]*par[4])+par[5]*pow(x[0],par[6]);
}

Float_t HTofWalkPar::getOffset(Float_t x, Int_t side, Float_t q, Int_t sector, Int_t module, Int_t cell)
{

    Double_t corrs[4] = {0.,0.,0.,0.};
    Double_t locX [4] = {0.,0.,0.,0.};

    HTofDigiParCell& cellg = (*fDigiPar)[sector][module][cell];

    Float_t hl = cellg.getHalfLen();
    if(x!=x)x = 0;
    Float_t xtemp = hl;
    if(side==0 || side==1) {
	xtemp = hl - x;
    } else {
	xtemp = hl + x;
    }
    if(xtemp>hl*2.) xtemp = hl*2.;
    if(xtemp<0)     xtemp = 0.;
    Int_t xbin  = floor(xtemp/(hl*2.)*20);
    Int_t xbint = floor((xtemp + hl/20.)/(hl*2.)*20);
    if(xbin<0)    xbin = 0;
    if(xbin>19)   xbin = 19;
    if(xbint<0)  xbint = 0;
    if(xbint>19) xbint = 19;
    Int_t binsX[4] = {0,1,2,3};
    Int_t binUsed = 0;
    if(xbin==19) {
	binsX[0] = 16;
	binsX[1] = 17;
	binsX[2] = 18;
	binsX[3] = 19;
	binUsed = 3;

    } else if(xbin>1){
	binsX[0] =xbin-2;
	binsX[1] =xbin-1;
	binsX[2] =xbin;
	binsX[3] =xbin+1;

        binUsed = 2;
    }
    for(Int_t i=0;i<4;i++) {
        if(binsX[i]<20)
        locX[i] = hl*2./20.*(binsX[i]) + hl/20.;
        else
        locX[i] = hl*2./20.*(binsX[i]) + hl/20.;
    }

    const Int_t nPar=func->GetNpar();
    Double_t pars[nPar];
    for(Int_t i=0;i<4;i++) {
	const Float_t* par= getPars(sector,module,cell,side,binsX[i]);
	for(Int_t j=0;j<nPar;j++) {
             pars[j]=par[j];
	}
	func->SetParameters(pars);
	corrs[i] = func->Eval(q);
    }
    // Interpolate between bins the correct value!
    if(binsX[binUsed]>0 && binsX[binUsed]<19) {
	if(xtemp<locX[binUsed]) {
	    return corrs[binUsed-1]+(xtemp-locX[binUsed-1])*(corrs[binUsed]-corrs[binUsed-1])/(locX[binUsed]-locX[binUsed-1]);
	} else {
	    return corrs[binUsed]+(xtemp-locX[binUsed])*(corrs[binUsed+1]-corrs[binUsed])/(locX[binUsed+1]-locX[binUsed]);
	}
    } else if(binsX[binUsed]==0) {
	return corrs[binUsed]+(xtemp-locX[binUsed])*(corrs[binUsed+1]-corrs[binUsed])/(locX[binUsed+1]-locX[binUsed]);
    } else {
	return corrs[binUsed-1]+(xtemp-locX[binUsed-1])*(corrs[binUsed]-corrs[binUsed-1])/(locX[binUsed]-locX[binUsed-1]);
    }
}
void HTofWalkPar::getOffsets(Float_t x, Float_t ql,Float_t qr,
			     Int_t sector, Int_t module, Int_t cell,
			     Float_t& const1,Float_t& const2,Float_t& const3,Float_t& const4)
{
    const1 = getOffset(x, 0, ql, sector, module, cell);
    const2 = getOffset(x, 2, qr, sector, module, cell);
    const3 = getOffset(x, 1, qr, sector, module, cell);
    const4 = getOffset(x, 3, ql, sector, module, cell);
}

void HTofWalkPar::getErrors(Float_t x, Float_t ql,Float_t qr,
			    Int_t sector, Int_t module, Int_t cell,
			    Float_t& const1,Float_t& const2,Float_t& const3,Float_t& const4)
{

    const1 = fabs(getOffset(x, 0, ql*0.93, sector, module, cell)      - getOffset(x, 0, ql*1.07, sector, module, cell));
    const2 = fabs(getOffset(x, 2, qr*0.93, sector, module, cell)      - getOffset(x, 2, qr*1.07, sector, module, cell));
    const3 = fabs(getOffset(x, 1, qr*0.93, sector, module, cell)      - getOffset(x, 1, qr*1.07, sector, module, cell));
    const4 = fabs(getOffset(x, 3, ql*0.93, sector, module, cell)      - getOffset(x, 3, ql*1.07, sector, module, cell));

    if(const1<0.02) const1 = 0.02;
    if(const2<0.02) const2 = 0.02;
    if(const3<0.02) const3 = 0.02;
    if(const4<0.02) const4 = 0.02;

}
void HTofWalkPar::getTofPos(HTofRaw* tofraw,Float_t& tof,Float_t& pos, Float_t x, Float_t startTMP) {

    Int_t sector = tofraw->getSector();
    Int_t module = tofraw->getModule();
    Int_t cell   = tofraw->getCell();
    HLocation loc;
    loc.set(3,0,0,0);
    loc[0]=sector;
    loc[1]=module;
    loc[2]=cell;


    tof = -100.;
    pos = -10000.;


    HTofCalParCell& cellp=(*fCalPar)[loc[0]][loc[1]][loc[2]];
    HTofDigiParCell& cellg=(*fDigiPar)[loc[0]][loc[1]][loc[2]];

    Float_t hl = cellg.getHalfLen();
    Float_t vg = cellp.getVGroup();

    Float_t ql = tofraw->getLeftCharge()  - cellp.getPedestalL();
    Float_t qr = tofraw->getRightCharge() - cellp.getPedestalR();
    Float_t tl = (tofraw->getLeftTime()*0.097)   - startTMP - cellp.getTimK();
    Float_t tr = (tofraw->getRightTime()*0.097)  - startTMP - cellp.getTimK();

    Float_t atof;
    Float_t axpos = x;
    Float_t xl = 0.;
    Float_t xr = 0.;

    xl = hl-axpos;
    xr = hl+axpos;


    Float_t corrl1;
    Float_t corrl2;
    Float_t corrr1;
    Float_t corrr2;

    Float_t errl1;
    Float_t errl2;
    Float_t errr1;
    Float_t errr2;

    getOffsets(axpos, ql, qr, sector, module, cell, corrl1,corrr1,corrl2,corrr2);
    getErrors (axpos, ql, qr, sector, module, cell, errl1 ,errr1 ,errl2 ,errr2);

    //Float_t corr1,corr2,corr3,corr
    Float_t wl1  = 1e-12;
    Float_t wl2  = 1e-12;
    Float_t wr1  = 1e-12;
    Float_t wr2  = 1e-12;
    Float_t sigl = 0.180;
    Float_t sigr = 0.180;

    xl = hl-axpos;
    xr = hl+axpos;
    Int_t xbinl1 = floor(xl/(hl*2.)*20);
    Int_t xbinr1 = floor(xr/(hl*2.)*20);
    if(xbinl1<0)  xbinl1 = 0;
    if(xbinl1>19) xbinl1 = 19;
    if(xbinr1<0)  xbinr1 = 0;
    if(xbinr1>19) xbinr1 = 19;

    sigl = getError(sector, module, cell, 0, xbinl1);
    sigr = getError(sector, module, cell, 3, xbinr1);

    if(sigl==0)sigl = 0.4;
    if(sigr==0)sigr = 0.4;

    // is left side used (Q)?
    if(getIsUsed(sector,module,cell,0)) {
	wl1 = 1./sqrt(sigl*sigl+errl1*errl1);
	wr2 = 1./sqrt(sigr*sigr+errr2*errr2);
    }

    sigl = getError(sector, module, cell, 1, xbinl1);
    sigr = getError(sector, module, cell, 2, xbinr1);

    if(sigl==0)sigl = 0.4;
    if(sigr==0)sigr = 0.4;

    // is right side used (Q)?
    if(getIsUsed(sector,module,cell,1)) {
	wl2 = 1./sqrt(sigl*sigl+errl2*errl2);
	wr1 = 1./sqrt(sigr*sigr+errr1*errr1);
    }

    if(ql<0 && qr<0) return;

    if(ql<0) {wl1=0.;wr2=0.; }
    if(qr<0) {wl2=0.;wr1=0.; }

    if((wl1==0.&&wl2==0.)||(wr1==0.&&wr2==0.)) return;

    axpos = 0.5 * ( ((tr-corrr1)*wr1+(tr-corrr2)*wr2)/(wr1+wr2) - ((tl-corrl1)*wl1+(tl-corrl2)*wl2)/(wl1+wl2) ) * vg ;
    atof  = ( (tl-corrl1+axpos/vg)*wl1+(tl-corrl2+axpos/vg)*wl2 + (tr-corrr1-axpos/vg)*wr1+(tr-corrr2-axpos/vg)*wr2  )/(wl1+wl2+wr1+wr2);

    tof = atof;
    pos = axpos-cellp.getPosK();


}


Double_t HTofWalkPar::getDxSigma (Int_t s,Int_t m,Int_t c,Double_t eloss)
{
    const   Double_t* SX = getParsSX(s,m,c);
    fdxsigma->SetParameters( SX );
    return fdxsigma -> Eval(eloss);
}
Double_t HTofWalkPar::getDxSigmaDigi (Int_t s,Int_t m,Int_t c,Double_t eloss,Double_t vgroup,Double_t beta)
{
    const   Double_t* SX = getParsSX(s,m,c);
    fdxsigma->SetParameters( SX );
    if(beta<0.3)  beta = 0.3;
    if(beta>0.96) beta = 0.96;
    fscaleDxSigma ->SetParameters(fParsGroupVelocityScale.GetArray());
    Double_t scale = fscaleDxSigma ->Eval(beta);
    return fdxsigma -> Eval(eloss) * (getGroupVelocity(s,m,c)/vgroup) * scale;
}

Double_t HTofWalkPar::getDxOffset(Int_t s,Int_t m,Int_t c,Double_t eloss)
{
    const   Double_t* DX = getParsDX(s,m,c);
    fdxoffset->SetParameters( DX );
    return fdxoffset -> Eval(eloss);
}

Double_t HTofWalkPar::getNormedDX(Int_t s,Int_t m,Int_t c,Double_t eloss,Double_t rkDX,Bool_t sim){

    Double_t sigma = HTofWalkPar::getDxSigma(s,m,c,eloss);
    if(sim){
	return rkDX/sigma;
    } else {
	Double_t offset = HTofWalkPar::getDxOffset(s,m,c,eloss);
	return (rkDX-offset)/sigma;
    }
}


void HTofWalkPar::Streamer(TBuffer &R__b)
{
   // Stream an object of class HTofWalkPar.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HParCond::Streamer(R__b);
      fWPars.Streamer(R__b);
      fWErrors.Streamer(R__b);
      fSideIsUsed.Streamer(R__b);
      if(R__v < 2){
           fParsSX.Set(6*8*8*3);
	   fParsDX.Set(6*8*8*5);
	   fParsGeantToData.Set(2);
           fParsSX.Reset(0);
           fParsDX.Reset(0);
	   fParsGeantToData.Reset(0);
	   fParsGroupVelocityData .Set(6*8*8);
	   fParsGroupVelocityData .Reset(0);
	   fParsGroupVelocityScale.Set(3);
           fParsGroupVelocityScale.Reset(0);
      } else {
	  fParsSX.Streamer(R__b);
	  fParsDX.Streamer(R__b);
	  fParsGeantToData.Streamer(R__b);
	  fParsGroupVelocityData.Streamer(R__b);
	  fParsGroupVelocityScale.Streamer(R__b);

      }

      R__b.CheckByteCount(R__s, R__c, HTofWalkPar::IsA());
   } else {
      R__c = R__b.WriteVersion(HTofWalkPar::IsA(), kTRUE);
      HParCond::Streamer(R__b);
      fWPars.Streamer(R__b);
      fWErrors.Streamer(R__b);
      fSideIsUsed.Streamer(R__b);
      fParsSX.Streamer(R__b);
      fParsDX.Streamer(R__b);
      fParsGeantToData.Streamer(R__b);
      fParsGroupVelocityData.Streamer(R__b);
      fParsGroupVelocityScale.Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}







