//*-- AUTHOR : Jerzy Pietraszko, Ilse Koenig
//*-- Created : 03/02/2006

/////////////////////////////////////////////////////////////
//
//  HTrb2Unpacker
//
//  This is a base for decoding TRB data and filling arrays. 
//  Derived classes has to provide methods for filling 
//  Detector's Raw categories.
//
/////////////////////////////////////////////////////////////

#ifndef HTRB2UNPACKER_H
#define HTRB2UNPACKER_H

#include "hldunpack.h"

class HTrb2Correction;
class HTrbnetAddressMapping;

class HTrb2Unpacker : public HldUnpack {

protected:
  HTrb2Unpacker(UInt_t id=0);
  virtual ~HTrb2Unpacker(){;}

  HTrb2Correction* trbinlcorr;          // TDC correctirons for the TRB boards
  HTrbnetAddressMapping* trbaddressmap; // mapping table trbnet-address to TRB board

  UInt_t subEvtId;                      // subevent id
  UInt_t uStartPosition;                // position at which to start decoding.
  UInt_t uTrbNetAdress;                 // TrbNetAdress
  UInt_t uSubBlockSize;                 // BlockSize of SubSubEvent (one TRB)
  UInt_t nCountWords;                   // at which data to start decoding

  Int_t trbDataVer;                     // data structure version:
                                        // info: http://hades-wiki.gsi.de/cgi-bin/view/DaqSlowControl/RpcDataStructure
  Int_t debugFlag;                      //! allows to print subevent information to the STDOUT

  Bool_t quietMode;                     //! do not print errors!
  Bool_t reportCritical;                //! report critical errors!
  Bool_t correctINL;                    //! if > 0 performs the INL correction
  Bool_t correctINLboard;               //! if > 0 performs the INL correction for this TRB
  Bool_t highResModeOn;                 //! is set the data are collected in High Res. Mode - 25ps binning
  Bool_t trbDataPairFlag;               //! data in pair mode (leading and width in one word)

  Float_t trbLeadingTime[128][10];        
  Float_t trbTrailingTime[128][10];      
  Float_t trbADC[128][10];               

  UInt_t trbExtensionSize;
  Int_t trbLeadingMult[128];            
  Int_t trbTrailingMult[128];          
  Int_t trbTrailingTotalMult[128];      // FIXME: Pablos private version; total multiplicity for trailings        
  Int_t trbDataExtension[128];

  Bool_t tryRecover_1;                    //! try to recover broken data format (blocksize out of subevent)
  Bool_t tryRecover_2;                    //! try to recover broken data format (trbnet subsub size not equal subsubsize)
public:
  
  Int_t getSubEvtId(void) const { return subEvtId; }
  Int_t getTrbDataVer(void) const { return trbDataVer; }

  virtual Int_t execute(void){ return 0;};
  virtual Bool_t init(void){ return kFALSE; };
  virtual Int_t decode(void);
  virtual Int_t correctOverflow(void);
  virtual Int_t correctRefTimeCh(Int_t); 
  virtual Int_t correctRefTimeCh31(void);
  virtual Int_t correctRefTimeCh127(void);
  virtual Int_t correctRefTimeStartDet23(void);
  virtual Float_t doINLCorrection(Int_t nTrbCh, Int_t nRawTime);

  void clearAll(void);
  void setQuietMode(void)      {quietMode      = kTRUE;}
  void setReportCritical(void) {reportCritical = kTRUE;}
  void setcorrectINL(void)     {correctINL     = kTRUE;}

  void setDebugFlag(Int_t db)  {debugFlag  = db;};
  Int_t getDebugFlag(){ return debugFlag;};

  void printTdcError(UInt_t e, UInt_t trbId);
  Bool_t fill_trail(Int_t ch,Float_t d);
  Bool_t fill_lead(Int_t ch,Float_t d);
  Bool_t fill_pair(Int_t ch,Float_t time, Float_t length);
  void   tryRecover(Bool_t trycase1=kTRUE,Bool_t trycase2=kTRUE) { tryRecover_1=trycase1; tryRecover_2=trycase2;}
public:
  ClassDef(HTrb2Unpacker,0) //Base class for TRB2 unpackers
};

#endif /* !HTRB2UNPACKER_H */


