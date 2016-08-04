#ifndef HONLINEMONCLIENTCANVAS_H
#define HONLINEMONCLIENTCANVAS_H

#include "TNamed.h"
#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TList.h"
#include "TGWindow.h"

#include "honlinemonclienthist.h"

class HOnlineMonClientMain;

class HOnlineMonClientCanvas : public TNamed {
public:
	TList listHists;
	
	HOnlineMonClientCanvas();
	~HOnlineMonClientCanvas();
	
	void CreateCanvas(HOnlineMonClientMain* clientmain, TGWindow* window);
	void DestroyCanvas(HOnlineMonClientMain* clientmain);
	
	TCanvas* GetCanvas();
	TRootEmbeddedCanvas* GetRCanvas();
	
	void SetSplitted(Bool_t split);
	Bool_t IsSplitted();
	
	void SetWidth(Int_t width);
	Int_t GetWidth();
	
	void SetHeight(Int_t height);
	Int_t GetHeight();
	
	void SetNx(Int_t nx);
	Int_t GetNx();
	
	void SetNy(Int_t ny);
	Int_t GetNy();
		
	void SetActive(Bool_t state);
	Bool_t IsActive();
	

private:
	TRootEmbeddedCanvas *rcanvas;
	Bool_t splitted;
	Bool_t active;
	Int_t width;
	Int_t height;
	Int_t nx;
	Int_t ny;
	
	ClassDef(HOnlineMonClientCanvas, 1);
};
#endif
