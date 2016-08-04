//*-- AUTHOR : G. Kornakov
//*-- created : 29/04/2013

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
// HRpcChargeOffsetPar
//
// Container class for RPC charge offset correction
// used by HRpcCalibrater
//
// (Condition Style)
//
// Each time slot is defined by the starting RunID and a
// array of 64 x 6 sectors parameters for each voltage
// regulator.
//
////////////////////////////////////////////////////////////////////////////


#include "hrpcchargeoffsetpar.h"
#include "hparamlist.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hades.h"
#include "hdatasource.h"
#include "hrun.h"
#include "hruntimedb.h"

ClassImp(HRpcChargeOffsetPar)

HRpcChargeOffsetPar::HRpcChargeOffsetPar(const Char_t* name,const Char_t* title,
                       const Char_t* context)
    : HParCond(name,title,context)
{

    fChargeCorrPar = TArrayF(64*6);// The size is 1 time slot. Initilised to 0
    fRunIDs        = TArrayI(2);   // The First and Last run ID
    fCurrent       = 0;
    fNRuns         = 1;

    clear();
}
HRpcChargeOffsetPar::~HRpcChargeOffsetPar()
{
  // destructor
}
void HRpcChargeOffsetPar::clear()
{

    fChargeCorrPar.Reset(0.);
    fRunIDs.Reset(-1);
    fNRuns = 1;
    fCurrent = 0;
    status = kFALSE;
    resetInputVersions();
    changed = kFALSE;

}

void HRpcChargeOffsetPar::addNewSet(Float_t* pars, Int_t startRunID)
{
    //adds a new time slot to an already created par
    fChargeCorrPar.Set((fNRuns+1)*6*64);
    fRunIDs.Set((fNRuns+1));
    for(Int_t i = 0; i < 64*6 ; i++) fChargeCorrPar.AddAt(pars[i], fNRuns*64*6+i);
    fRunIDs.AddAt(startRunID,fNRuns);
    fNRuns++;
    fCurrent++;

}

void HRpcChargeOffsetPar::addFirstSet(Float_t* pars, Int_t startRunID)
{
    //adds a new time slot to an already created par
    for(Int_t i = 0; i < 64*6 ; i++) fChargeCorrPar.AddAt(pars[i], i);
    fRunIDs.AddAt(startRunID,0);
    fCurrent++;

}

void HRpcChargeOffsetPar::addLastRunId(Int_t runId)
{
    //adds the last runid for which parameters were obtained!
    fRunIDs.Set(fNRuns+1);
    fRunIDs.AddAt(runId,fNRuns);
}

void HRpcChargeOffsetPar::addLastSet(Float_t* pars, Int_t startRunID, Int_t endRunID)
{
    //adds a new time slot to an already created par and ends the contparam if it is the last!
    fChargeCorrPar.Set((fNRuns+1)*6*64);
    fRunIDs.Set((fNRuns+2));
    for(Int_t i = 0; i < 64*6 ; i++) fChargeCorrPar.AddAt(pars[i], fNRuns*64*6+i);
    fRunIDs.AddAt(startRunID,fNRuns);
    fRunIDs.AddAt(endRunID,fNRuns+1);
    fNRuns++;
    fCurrent++;

}


void HRpcChargeOffsetPar::putParams(HParamList* l)
{
    // Puts all params of HRpcTimePosPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("fNRuns", fNRuns);
    l->add("fRunIDs", fRunIDs);
    l->add("fChargeCorrPar", fChargeCorrPar);

}
Bool_t HRpcChargeOffsetPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;

    if(!( l->fill("fNRuns", &fNRuns)))  return kFALSE;
    
    //fChargeCorrPar.Set(fNRuns * 64*6);
    //fRunIDs.Set( fNRuns + 1 );

    if(!( l->fill("fRunIDs", &fRunIDs))) return kFALSE;
    if(!( l->fill("fChargeCorrPar", &fChargeCorrPar))) return kFALSE;

    return kTRUE;
}

Bool_t HRpcChargeOffsetPar::init(HParIo* inp,Int_t* set) {
    //First check the version! if a new version is present initialise it!
    Bool_t rc=kFALSE;
    HDetParIo* input = inp->getDetParIo("HCondParIo");
    if (input) rc = input->init(this,set);

    //Int_t runId = gHades->getDataSource()->getCurrentRunId();
    HRun* run =   gHades->getRuntimeDb()->getCurrentRun();
    Int_t runId = run->getRunId();

    //otherwise, find the correct run slot for this file!
    if(runId >= this->getFirstRunID() && runId <= this->getLastRunID()  ) {
	fCurrent = this->getSlot(runId);
	return rc;
    } else if (runId < this->getFirstRunID()) {
	fCurrent = fNRuns-1; //The last slot contains only 0 -> NO CORRECTION IN THIS CASE!
	rc = kTRUE;
        return rc;
    }

    return rc;

}

/*Bool_t HRpcChargeOffsetPar::init(HParIo* inp) {
    //First check the version! if a new version is present initialise it!

    cout<<"or I am here!"<<endl;
    Bool_t rc=kFALSE;
    //HDetParIo* input=inp->getDetParIo("HCondParIo");
    //if (input) rc=input->init(this);

    Int_t runId = gHades->getDataSource()->getCurrentRunId();

    //if(runId < this->getNextRunID() || this->getNextRunID()==0 ) return rc; //the runID belongs to the same time slot or it is void! //check the version!
    //if(runId >= this->getNextRunID() && runId<=getLastRunID()  ) {
    //    fCurrent++;      //the RunID belongs to the next time slot.
	//The case were other time slot is used
	//should be contempled. Not just current++!!
    //    return rc;
    //}
    //otherwise, find the correct run slot for this file!
    if(runId >= this->getFirstRunID() && runId <= this->getLastRunID()  ) {
	fCurrent = this->getSlot(runId);
	return rc;
    }

    return rc;

}
*/

Int_t  HRpcChargeOffsetPar::getSlot(Int_t runId)
{
    //find the correct parameters slot
    Int_t slot = -1;
    for(Int_t i=0; i<fNRuns; i++) {
	if( fRunIDs[i] <= runId && fRunIDs[i+1] > runId  ) {slot = i; break;}
    }
    return slot;
}

