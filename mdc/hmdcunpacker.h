///////////////////////////////////////////////////////////
// Author : J.Wuestenfeld
///////////////////////////////////////////////////////////
#ifndef HMDCUNPACKER__H
#define HMDCUNPACKER__H

#include "hldunpack.h"
#include "haddef.h"

#include "TString.h"

#define MAX_OEP_DECODE_VERSIONS 2

class HMdcUnpackerPar;
class HMdcRawStruct;
class HMdcLookupGeom;
class HCategory;
class HMdcDataword;
class HParSet;
class HMdcRaw;
class HMdcOepAddrCorrPar;
class HDetector;
class HTrbNetUnpacker;

class HMdcUnpacker: public HldUnpack
{
protected:
    HMdcDataword	*dataword;			      	//! Pointer to dataword
    HMdcRawStruct	*rawc;				      	//! Pointer to lookup table
    HMdcLookupGeom	*lookupGeom;				//! Pointer to lookup table channel to wire number
    HCategory		*pMdcDataWordCat;		       	//! Pointer to category to store raw event information in
    HCategory		*pMdcOepStatusCat;			//! Pointer to category to store Oep Status words in
    HMdcOepAddrCorrPar	*addrCorrPar;			       	//! Pointer to OEP address correction map
    HDetector		*pDetector;			      	//! Pointer to Detector
    HTrbNetUnpacker     *trbNetUnpacker;            //!

    HMdcUnpackerPar   	*pUnpackerPar;			      	//! Pointer to unpacker parameters for data recovery
    Bool_t	         continueDecodingAfterInconsistency;	//! Flag to enable stubborn behaviour of unpacker
    Bool_t		 debugMode;			    	//! switch on debugging mode of unpacker
    Int_t		 decodeVersion;			      	//! Version number of data word decoding
    Bool_t		 doAddrCorrection;		      	//! enable address correction of OEP's
    Bool_t		 doMapOutUnusedChannels;		//! enable mapping of unused channels
    Bool_t		 consistencyCheck;		     	//! enable consistency check on data
    Bool_t		 fillDataWord;			      	//! Flag to enable filling of the dataword
    Bool_t		 fillOepStatus;			//! Flag to enable oepstatus word filling
    Bool_t		 mdcDataWordCatPersistency;	        //! Flag for persistency of HMdcDataWord category
    Bool_t		 mdcOepStatusCatPersistency;		//! Flag for persistency of HMdcOepStatusData category
    Bool_t               doBitFlipCorr;                         //! correct known bitflips of MBos
    TString              bitFlipVersion;                        //! default = "auto"
    Int_t		 module;				//! Module handled.
    Bool_t		 noComment;				//! Flag to enable basic debug printouts
    Int_t		 sector;				//! Sector handled.
    Int_t		 subEvtId;				//! Sub event Id this unpacker is responsible for.
    Int_t		 tdcMode;				//! tdcMode defines the mode the tdc has taken data:
                                                                //! The TDC can be operated in 2 different modes:
                                                                //! mode 0 (default) : trigger on leading and trailing edge
                                                                //! mode 1           : trigger on 2 leading edges:
    Int_t		 badEventsCounter;    			//! Counter for events wich have errors and get skipped

    Bool_t      checkConsistency(Int_t sector, Int_t module, Int_t mbo, Int_t tdc, Int_t channel);
    HCategory  *initCategory(Cat_t cat, const char *catname=" ", const char *detector="Mdc");
    Int_t      	fillData(void);
    Int_t       fillMdcDataWord(Int_t sector,Int_t module,Int_t mbo,Int_t tdc);
    Int_t       fillStatus(void);
    Bool_t     	getContinueDecodingAfterInconsistency(void){return continueDecodingAfterInconsistency;};
    Bool_t	getDebug(void){return debugMode;};
    Bool_t	getDoAddrCorrection(void){return doAddrCorrection;};
    HMdcRaw*	getFreeOrExistingSlot(Int_t sector,Int_t module,Int_t mbo,Int_t tdc);
    void	setDoAddrCorrection(Bool_t val){doAddrCorrection = val;};
    void	setContinueDecodingAfterInconsistency(Bool_t cont=kTRUE);
    void 	setDefaults(void);
    void        correctBitFlip(HMdcRaw*,Int_t year,Int_t month,Int_t day);

public:
    HMdcUnpacker(Int_t id, Bool_t dbg=kFALSE, Bool_t consisCheck=kFALSE);
    HMdcUnpacker(HMdcUnpacker &unp);
    ~HMdcUnpacker(void);

    Int_t  execute(void);
    Bool_t init(void);
    Bool_t reinit(void);
    Bool_t finalize(void);

    Bool_t getDebugMode     (void) const {return debugMode;};
    UInt_t getDecodeVersion (void)       {return decodeVersion;};
    Int_t  getSubEvtId      (void) const {return subEvtId;};
    Int_t  getTdcMode       (void)       {return tdcMode;};

    void   setFillDataWord(Bool_t b){fillDataWord=b;};
    void   setFillOepStatus(Bool_t b){fillOepStatus=b;};
    void   setPersistencyDataword(Bool_t b=kTRUE){mdcDataWordCatPersistency=b;};
    void   setPersistencyOepStatusData(Bool_t b=kTRUE){mdcOepStatusCatPersistency=b;};
    void   setQuietMode(Bool_t quiet=kTRUE,Bool_t warn=kTRUE);
    void   setDoBitFlipCorr(Bool_t corr=kTRUE,TString version="auto") { doBitFlipCorr=corr; bitFlipVersion = version; }
    ClassDef(HMdcUnpacker,0)
};

#endif
