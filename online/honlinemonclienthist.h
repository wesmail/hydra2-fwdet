#ifndef HONLINEMONCLIENTHIST_H
#define HONLINEMONCLIENTHIST_H

#include "TNamed.h"
#include "honlinemonhist.h"
#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"


class HOnlineMonClientHist : public TNamed {
public:
	HOnlineMonClientHist();
	~HOnlineMonClientHist();
	
	HOnlineMonHistAddon* histo;	// histogram
	
	void SetHist(HOnlineMonHistAddon* hist);
	HOnlineMonHistAddon* GetHist();
	
	void SetSubpadnumber(Int_t nr);
	Int_t GetSubpadnumber();
	
	void SetActive(Bool_t state);
	Bool_t IsActive();
	
	void SetArrayIndexS(Int_t s);
	Int_t GetArrayIndexS();
	
	void SetArrayIndexM(Int_t m);
	Int_t GetArrayIndexM();
	
	void SetType(TString histotype);
	TString GetType();
	
	void SetRealName(TString rname);
	TString GetRealName();

	void SetGuiName(TString rname);
	TString GetGuiName();

private:
	Int_t subpadnumber;		// subpad for the histogram (splitted canvas)
	Bool_t active;			// update the histogram ?
	Int_t index_s;			// array index for histogram arrays
	Int_t index_m;			// array index for histogram arrays
	TString	type;			// histogram type, "single" or "array"
	TString realname;		// this is the real name from the config file
					// and for the update request sent to the server.
					// The name inherited from TNamed is a combination
					// of the canvas name (subpad name) and the histo name
					// to ensure that the name is unique. So it is possible to
					// place the same histogram in different windows/canvases
					// because the histo name is unique, now.
					// The histo is stored in a TList, so we need unique names
        TString guiname;                // name of the gui which orders the histogram
	ClassDef(HOnlineMonClientHist, 1);
};
#endif
