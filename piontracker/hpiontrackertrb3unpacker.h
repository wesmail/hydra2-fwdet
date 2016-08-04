#ifndef HPIONTRACKERTRB3UNPACKER_H
#define HPIONTRACKERTRB3UNPACKER_H

#include "hlocation.h"
#include "hldunpack.h"

class HPionTrackerTrb3Lookup;
class HPionTrackerTrb3LookupBoard;

enum ptdesign { des_none, des_cosy13, des_140115 };

struct ptheaderset
{
#if GCC_VERSION > 40700
	ptdesign design = des_none;
	Int_t tsref = -1;
	Int_t evcnt = -1;
	Int_t romode = -1;
	Int_t version = -1;
	Int_t boardid = -1;
#else
	ptheaderset() : design(des_none), tsref(-1), evcnt(-1), romode(-1), version(-1), boardid(-1) {}
	ptdesign design;
	Int_t tsref;
	Int_t evcnt;
	Int_t romode;
	Int_t version;
	Int_t boardid;
#endif
};

struct ptdataset
{
#if GCC_VERSION > 40700
	Int_t strip = -1;
	Float_t adc = -1;
	Float_t ts = -1;
	Char_t pileup = -1;
	Char_t overflow = -1;
#else
	ptdataset() : strip(-1), adc(-1), ts(-1), pileup(-1), overflow(-1) {}
	Int_t strip;
	Float_t adc;
	Float_t ts;
	Char_t pileup;
	Char_t overflow;
#endif
};

class HPionTrackerTrb3Unpacker: public HldUnpack
{
protected:
	UInt_t subEvtId;                 // subevent id
	UInt_t uTrbNetAddress;           // TrbNetAdress
	UInt_t nEvt;                     // Evt SeqNumber

	Int_t  debugFlag;                //! allows to print subevent information to the STDOUT
	Bool_t quietMode;                //! do not print errors!
	Bool_t reportCritical;           //! report critical errors!

	HLocation loc;                   // location of raw cell object
	HPionTrackerTrb3Lookup* pLookup; // TRB3 lookup table for PionTracker
	HPionTrackerTrb3LookupBoard *pTrbBoard; // pointer to TRB board in lookup table 

public:
	HPionTrackerTrb3Unpacker(UInt_t id = 0);
	~HPionTrackerTrb3Unpacker(void) {}

	Int_t getSubEvtId(void) const { return subEvtId; }

	void  setDebugFlag(Int_t db) {debugFlag  = db;}
	Int_t getDebugFlag()         {return debugFlag;}

	void setQuietMode(void)      {quietMode      = kTRUE;}
	void setReportCritical(void) {reportCritical = kTRUE;}

	Bool_t init(void);
	Int_t  execute(void);

	enum ptstate { sm_wait_for_board = 1, sm_header, sm_data };

protected:
	ptheaderset decodeSubevent(UInt_t h);
	ptdataset decodeData_Cosy13(UInt_t d, const ptheaderset & hs);
	ptdataset decodeData_140115(UInt_t d, const ptheaderset & hs);

	Bool_t fillRaw(const ptdataset & ptds);
	Bool_t fillRaw(Int_t, Float_t, Float_t, Char_t, Char_t);

public:
	ClassDef(HPionTrackerTrb3Unpacker, 0) // TRB3 unpacker for PionTracker
};

#endif /* !HPIONTRACKERTRB3UNPACKER_H */
