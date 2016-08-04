//*-- AUTHOR  : Georgy Kornakov
//*-- created : 29/04/2013

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HRpcLookupGeom
//
//  Container class wich provides access to the information
//  stored in the RpcLookup container and mapping from cell
//  to channel.
//
//  Needed for recalculation of the charge offsets if the RAW
//  category is not accesible but CAL it is.
//
//  A write function is only privided for an Ascii I/O.
//
/////////////////////////////////////////////////////////////


#include "hrpclookupgeom.h"
#include "hrpctrb2lookup.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hrpcdetector.h"

#include "hpario.h"
#include "hdetpario.h"
#include "hmessagemgr.h"
#include "TClass.h"

ClassImp(HRpcLookupGCell)
ClassImp(HRpcLookupGCol)
ClassImp(HRpcLookupGSec)
ClassImp(HRpcLookupGeom)


HRpcLookupGCol::HRpcLookupGCol(Int_t n) {
  // constructor creates an array of pointers of type
  // HRpcLookupGCell (array of all cells in a layer)
  array = new TObjArray(n);
  for (Int_t i = 0; i < n; i++) {
    array->AddAt(new HRpcLookupGCell(),i);
  }
}

HRpcLookupGCol::~HRpcLookupGCol() {
  // destructor
  array->Delete();
  delete array;
}

HRpcLookupGSec::HRpcLookupGSec(Int_t n) {
  // constructor creates an array of pointers of type
  // HRpcLookupGCol
  array = new TObjArray(n);
  for(Int_t i = 0; i < n; i++) array->AddAt(new HRpcLookupGCol(32),i);
}

HRpcLookupGSec::~HRpcLookupGSec() {
  // destructor
  array->Delete();
  delete array;
}

HRpcLookupGeom::HRpcLookupGeom(const Char_t* name,const Char_t* title,
                             const Char_t* context,Int_t n)
              : HParSet(name,title,context) {
  // constructor creates an array of pointers of type HRpcLookupGSec
  // creates the container RpcTrb2Lookup
  array = new TObjArray(n);
  for(Int_t i=0;i<n;i++) array->AddAt(new HRpcLookupGSec(6),i);
  gHades->getRuntimeDb()->getContainer("RpcTrb2Lookup");
}

HRpcLookupGeom::~HRpcLookupGeom() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HRpcLookupGeom::init(HParIo*) {
  // intitializes the container from RpcTrb2Lookup container
  HRpcTrb2Lookup* p =
     (HRpcTrb2Lookup*)gHades->getRuntimeDb()->findContainer("RpcTrb2Lookup");
  if (!p) return kFALSE;
  Int_t v1=p->getInputVersion(1);
  Int_t v2=p->getInputVersion(2);
  if (v1==versions[1] && v2==versions[2]) return kTRUE;
  // needs reinitialization
  clear();

  Int_t sector, column, cell;
  Char_t side;

  for(Int_t b = 0; b<p->getSize();b++ ){
      HRpcTrb2LookupBoard* rBoard = (*p)[b];
      if(!rBoard)continue;
      for(Int_t c = 0; c< rBoard->getSize(); c++) {
	  HRpcTrb2LookupChan& rChan = (*rBoard)[c];
          if(&rChan==0) continue;

	  sector = rChan.getSector();
	  column = rChan.getColumn();
	  cell   = rChan.getCell();
          side   = rChan.getSide();

	  if(sector<0 || column<0 || cell<0 || cell>=32) continue;

	  HRpcLookupGCell& fRcell = (*this)[sector][column][cell];
          if( side == 'l')
	      fRcell.setLeftFEEAddress(  rChan.getFeAddress() );
          if( side == 'r')
	      fRcell.setRightFEEAddress( rChan.getFeAddress() );

      }
  }
  versions[1]=v1;
  versions[2]=v2;
  changed=kTRUE;                          
  return kTRUE;
}

Int_t HRpcLookupGeom::write(HParIo* output) {
  // only to an output if the output is an ascii file
  if (strcmp(output->IsA()->GetName(),"HParAsciiFileIo")==0) {
    HDetParIo* out=output->getDetParIo("HRpcParIo");
    if (out) return out->write(this);
  }
  changed=kFALSE;
  return 0;
}

void HRpcLookupGeom::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HRpcParAsciiFileIo
  header=
    "# Rpc lookup table (geometry->hardware)\n"
    "# Format:\n"
    "# sector column cell leftFEEadd rightFEEadd\n";
}

Bool_t HRpcLookupGeom::writeline(Char_t *buf, Int_t sec, Int_t col, Int_t cell) {
    //writes one line to the buffer used by ascii file I/O
    HRpcLookupGCell& cellAdd=(*this)[sec][col][cell];
    if(cellAdd.getRightFEEAddress()<0 || cellAdd.getLeftFEEAddress()<0) return kFALSE;
    sprintf(buf,"%1i %1i %1i %i %i\n",
            sec, col, cell, cellAdd.getLeftFEEAddress(), cellAdd.getRightFEEAddress() );
    return kTRUE;
}

void HRpcLookupGeom::clear() {
  // clears the container
   for( Int_t s = 0; s < 6; s++) {
     HRpcLookupGSec& sec = (*this)[s];
    for( Int_t c = 0; c< 6; c++) {
      HRpcLookupGCol& col = sec[c];
      for( Int_t ce = 0; ce < 32; ce++) {
	  HRpcLookupGCell& cell = col[ce];
          cell.clear();
      }
    }
   }
  setStatic(kFALSE);
  resetInputVersions();
}

Int_t HRpcLookupGCell::getLeftMBOnum() {
    if(fLeftFeeAddress<0) return -1;
    Int_t trbnum    = fLeftFeeAddress/1000000;
    Int_t mbonum    = (fLeftFeeAddress-trbnum*1000000)/10000;
    return mbonum;
}
Int_t HRpcLookupGCell::getLeftDBOnum() {
    if(fLeftFeeAddress<0) return -1;
    Int_t  trbnum    = fLeftFeeAddress/1000000;
    Int_t  mbonum    = (fLeftFeeAddress-trbnum*1000000)/10000;
    Int_t  dbonum    = (fLeftFeeAddress-trbnum*1000000-mbonum*10000)/100;
    return dbonum;
}
Int_t HRpcLookupGCell::getLeftTRBnum() {
    if(fLeftFeeAddress<0) return -1;
    Int_t trbnum    = fLeftFeeAddress/1000000;
    return trbnum;
}
Int_t HRpcLookupGCell::getRightMBOnum() {
    if(fRightFeeAddress<0) return -1;
    Int_t trbnum    = fRightFeeAddress/1000000;
    Int_t mbonum    = (fRightFeeAddress-trbnum*1000000)/10000;
    return mbonum;
}
Int_t HRpcLookupGCell::getRightDBOnum() {
    if(fRightFeeAddress<0) return -1;
    Int_t  trbnum    = fRightFeeAddress/1000000;
    Int_t  mbonum    = (fRightFeeAddress-trbnum*1000000)/10000;
    Int_t  dbonum    = (fRightFeeAddress-trbnum*1000000-mbonum*10000)/100;
    return dbonum;
}
Int_t HRpcLookupGCell::getRightTRBnum() {
    if(fRightFeeAddress<0) return -1;
    Int_t trbnum    = fRightFeeAddress/1000000;
    return trbnum;
}
void HRpcLookupGCell::getTrbMboDboLeftNum(Int_t& trbnum,Int_t& mbonum,Int_t& dbonum) {
    if(fLeftFeeAddress < 0) {
	trbnum=-1;
	mbonum=-1;
	dbonum=-1;
    }
    else {
	trbnum    = fLeftFeeAddress/1000000;
	mbonum    = (fLeftFeeAddress-trbnum*1000000)/10000;
	dbonum    = (fLeftFeeAddress-trbnum*1000000-mbonum*10000)/100;
    }
    //dboInput  = FeAddress-trbNum*1000000-mboNum*10000-dboNum*100;
}
void HRpcLookupGCell::getTrbMboDboRightNum(Int_t& trbnum,Int_t& mbonum,Int_t& dbonum) {
    if(fRightFeeAddress < 0) {
	trbnum=-1;
	mbonum=-1;
	dbonum=-1;
    }
    else {
	trbnum    = fRightFeeAddress/1000000;
	mbonum    = (fRightFeeAddress-trbnum*1000000)/10000;
	dbonum    = (fRightFeeAddress-trbnum*1000000-mbonum*10000)/100;
    }
    //dboInput  = FeAddress-trbNum*1000000-mboNum*10000-dboNum*100;
}
void HRpcLookupGCell::getLeftRightRegAdd(Int_t sec, Int_t& leftRegNum, Int_t& rightRegNum) {
    Int_t ltrbnum, lmbonum, ldbonum;
    Int_t rtrbnum, rmbonum, rdbonum;
    this->getTrbMboDboRightNum(rtrbnum, rmbonum, rdbonum);
    this->getTrbMboDboLeftNum(ltrbnum, lmbonum, ldbonum);
    if(rtrbnum<0 || rmbonum<0 || rdbonum<0 || ltrbnum<0 || lmbonum<0 || ldbonum<0 ) {
	leftRegNum  = -1;
	rightRegNum = -1;
    } else {
	leftRegNum  = sec*64 + lmbonum*16  + ltrbnum*4 + ldbonum/2;
	rightRegNum = sec*64 + rmbonum*16  + rtrbnum*4 + rdbonum/2;
    }
}
