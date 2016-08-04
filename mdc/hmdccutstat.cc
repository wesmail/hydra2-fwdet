//*-- AUTHOR : Jochen Markert

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////
//HMdcCutStat
//
//  class for statistics on cut time1, time2 and time2-time1
//  and wires less than 4 per module
//////////////////////////////////////////////////////////////

#include "hmdccutstat.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hmdcparasciifileio.h"
#include "hpario.h"
#include "hmdcdetector.h"
#include "hspectrometer.h"

HMdcCutStat* HMdcCutStat::fMdcCutStat=0;
HMdcCutStat::HMdcCutStat(const Char_t* name,const Char_t* title,
			 const Char_t* context,Int_t s,Int_t m)
    : HParSet(name,title,context), fSecs(s)
{
    fMdcCutStat=this;
    for (Int_t i=0;i<s; i++) fSecs.AddAt(new HMdcCutStatSec(m),i);
    setNameTitle();
    if (gHades) {
	fMdc = (HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector("Mdc"));
    } else {
	fMdc = 0;
    }
}
HMdcCutStat::~HMdcCutStat()
{}
HMdcCutStat* HMdcCutStat::getExObject()
{
    return fMdcCutStat;
}
HMdcCutStat* HMdcCutStat::getObject()
{
    if(!fMdcCutStat) fMdcCutStat=new HMdcCutStat();
    return fMdcCutStat;
}
void HMdcCutStat::deleteHMdcCutStat()
{
    if(fMdcCutStat) delete fMdcCutStat;
    fMdcCutStat=0;
}
void HMdcCutStat::clear(void)
{
    for (Int_t i=0;i<getSize();i++)
    {
	HMdcCutStatSec &sector = (*this)[i];
	for (Int_t m=0; m<sector.getSize(); m++)
	{
	    HMdcCutStatMod &mod = sector[m];
	    mod.clear();
	}
    }
}
void HMdcCutStat::setNameTitle()
{
    strcpy(detName,"Mdc");
}
void HMdcCutStat::getCal1StatCut(Int_t s,Int_t mod, Int_t *cT1, Int_t *cT2,
				 Int_t *cT12, Int_t *cWire,Int_t* cT1T12,Int_t* cBump,Int_t* cAll,Int_t* all)
{   // returns values of different cut statistics to Int_t*
    // cT1    : cut in T1
    // cT2    : cut in T2
    // cT12   : cut in T2-T1
    // cWire  : cut in number of Wires <4
    // cT1T12 : cut in T1 and T2-T1
    // cBump  : cut in T1 and T2-T1
    // cAll   : all cuts
    // all    : all hits

    HMdcCutStatMod &m = (*this)[s][mod];
    *cT1    = m.getCal1StatCutT1();
    *cT2    = m.getCal1StatCutT2();
    *cT12   = m.getCal1StatCutT12();
    *cWire  = m.getCal1StatCutWire();
    *cT1T12 = m.getCal1StatCutT1T12();
    *cBump  = m.getCal1StatCutBump();
    *cAll   = m.getCal1StatCutAll();
    *all    = m.getCal1StatAll();
}
void HMdcCutStat::setCal1StatCut(Int_t s,Int_t mod, Int_t cT1, Int_t cT2,
				 Int_t cT12,Int_t cBump, Int_t cWire)
{
    // counts up values of different cut statistics
    // cT1    : cut in T1
    // cT2    : cut in T2
    // cT12   : cut in T2-T1
    // cWire  : cut in number of Wires <4
    // cT1T12 : cut in T1 and T2-T1 internaly filled
    // cBump  : cut in T1 and T2-T1 internaly filled
    // cAll   : all cuts            internaly filled
    // all    : all hits            internaly filled

    HMdcCutStatMod &m = (*this)[s][mod];
    if(cT1==1)    m.setCal1StatCutT1();
    if(cT2==1)    m.setCal1StatCutT2();
    if(cT12==1)   m.setCal1StatCutT12();
    if(cWire==1)  m.setCal1StatCutWire();
    if(cT1==1  ||
       cT12==1)   m.setCal1StatCutT1T12();
    if(cBump==1)  m.setCal1StatCutBump();
    if(cT1==1  ||
       cT2==1  ||
       cT12==1 ||
       cBump==1 ||
       cWire==1)  m.setCal1StatCutAll();
                  m.setCal1StatAll();
}
Bool_t HMdcCutStat::init(HParIo* inp,Int_t* set) {
    // intitializes the container from an input
    HDetParIo* input=inp->getDetParIo("HMdcParIo");
    if (input) return (input->init(this,set));
    return kFALSE;
}
Int_t HMdcCutStat::write(HParIo* output) {
    // writes the container to an output
    HDetParIo* out=output->getDetParIo("HMdcParIo");
    if (out) return out->write(this);
    return -1;
}
void HMdcCutStat::putAsciiHeader(TString& b) {
    b =
	"#######################################################################\n"
	"# Statistic onDrift time cuts for noise reduction of the MDC\n"
	"# Format:\n"
	"# sector mod ct1 ct2 ct12 cwires ct1t12 cbump call all \n"
	"#######################################################################\n";
}
Bool_t HMdcCutStat::writeline(Char_t*buf, Int_t s, Int_t m) {
    Bool_t r = kTRUE;
    if (fMdc) {
	if (fMdc->getModule(s,m) != 0) {
	    if  (s>-1 && s<getSize()) {
		HMdcCutStatSec &sector = (*this)[s];
		if (m>-1 && m<sector.getSize()) {
		    sprintf(buf,"%i %i %i %i %i %i %i %i %i %i \n",
			    s,m,
			    (*this)[s][m].getCal1StatCutT1(),
			    (*this)[s][m].getCal1StatCutT2(),
			    (*this)[s][m].getCal1StatCutT12(),
			    (*this)[s][m].getCal1StatCutWire(),
                            (*this)[s][m].getCal1StatCutT1T12(),
                            (*this)[s][m].getCal1StatCutBump(),
                            (*this)[s][m].getCal1StatCutAll(),
                            (*this)[s][m].getCal1StatAll());

		} else r = kFALSE;
	    } else r = kFALSE;
	} else { strcpy(buf,""); }
    }
    return r;
}
ClassImp(HMdcCutStat)
ClassImp(HMdcCutStatMod)
ClassImp(HMdcCutStatSec)
