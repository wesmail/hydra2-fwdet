//*-- AUTHOR : Jerzy Pietraszko, Ilse Koenig
//*-- Created : 03/02/2006

/////////////////////////////////////////////////////////////
//
//  HTrbBaseUnpacker
//
//  This is a base for decoding TRB data and filling arrays. 
//  Derived classes has to provide mathods for filling 
//	Detector's Raw categories.
//
/////////////////////////////////////////////////////////////

#ifndef HTRBBASEUNPACKER_H
#define HTRBBASEUNPACKER_H

#include "hldunpack.h"
class HTrbLookup;

class HTrbBaseUnpacker : public HldUnpack {

protected:
  HTrbBaseUnpacker(UInt_t id=0);
  virtual ~HTrbBaseUnpacker(){;}

  HTrbLookup* lookup;

  UInt_t subEvtId;                    
  Int_t trbDataVer; // data structure version:
                     // info: http://hades-wiki.gsi.de/cgi-bin/view/DaqSlowControl/RpcDataStructure
  Int_t trbLeadingTime[128][10];        
  Int_t trbTrailingTime[128][10];      
  Int_t trbADC[128][10];               
  Int_t trbLeadingMult[128];            
  Int_t trbTrailingMult[128];          
  Int_t trbTrailingTotalMult[128];  // FIXME: Pablos private version; total multiplicity for trailings        
  Int_t trbDataExtension[128];
  Int_t trbExtensionSize;
  Bool_t trbDataPairFlag;

  Int_t debugFlag;  //! allows to print subevent information
		    // to the STDOUT	
  Bool_t quietMode; //! do not print errors!
  Bool_t reportCritical;//! report critical errors!
public:
  

  Int_t getSubEvtId(void) const { return subEvtId; }
  Int_t getTrbDataVer(void) const { return trbDataVer; }
  virtual Int_t execute(void){ return 0;};
  virtual Bool_t init(void){ return kFALSE; };
  virtual Int_t decode(void);
  virtual Int_t correctOverflow(void);
  virtual Int_t correctRefTimeCh31(void);
  virtual Int_t correctRefTimeCh127(void);
  void clearAll(void);
  void setQuietMode(void){quietMode=kTRUE;}
  void setReportCritical(void){reportCritical=kTRUE;}

  void setDebugFlag(Int_t db){ debugFlag = db;};
  Int_t getDebugFlag(){ return debugFlag;};

  void PrintTdcError(UInt_t e, UInt_t trbId);
  Bool_t fill_trail(Int_t ch,Int_t d);
  Bool_t fill_lead(Int_t ch,Int_t d);
  Bool_t fill_pair(Int_t ch,Int_t time,Int_t length);

public:
  ClassDef(HTrbBaseUnpacker,0) //Base class for TRB unpacker


};

#endif /* !HTRBBASEUNPACKER_H */


