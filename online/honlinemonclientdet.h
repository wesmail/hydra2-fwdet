#ifndef HONLINEMONCLIENTDET_H
#define HONLINEMONCLIENTDET_H

#include "TNamed.h"
#include "TList.h"


class HOnlineMonClientMain;

class HOnlineMonClientDet : public TNamed {
public:
	TList listWindows;
	
	HOnlineMonClientDet();
	~HOnlineMonClientDet();
	
	void CreateDet(HOnlineMonClientMain *clientmain);
	void DestroyDet(HOnlineMonClientMain *clientmain);
	
	void SetActive(Bool_t state);
	Bool_t IsActive();
	
private:
	Bool_t active;
	ClassDef(HOnlineMonClientDet, 1);
};
#endif
