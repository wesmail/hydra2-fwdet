#ifndef HONLINEMONCLIENTTAB_H
#define HONLINEMONCLIENTTAB_H

#include "TNamed.h"
#include "TList.h"
#include "TGFrame.h"
#include "TString.h"

class  HOnlineMonClientMain;

#include "honlinemonclienttabwin.h"

class HOnlineMonClientTab : public TNamed {
public:
	TList listCanvases;
	
	HOnlineMonClientTab();
	~HOnlineMonClientTab();
	
	void CreateTab(HOnlineMonClientMain* clientmain, HOnlineMonClientTabWin* clienttabwin);
	void DestroyTab(HOnlineMonClientMain* clientmain);
	
	void SetActive(Bool_t state);
	Bool_t IsActive();
	
private:
	TGCompositeFrame *tabFrame;
	Bool_t active;
	ClassDef(HOnlineMonClientTab, 1);
};
#endif

