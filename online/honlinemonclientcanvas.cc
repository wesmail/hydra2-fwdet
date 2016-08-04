#include "honlinemonclientcanvas.h"

#include "honlinemonclientmain.h"

ClassImp(HOnlineMonClientCanvas);


/* -------------------------------------------------------------------------- */
HOnlineMonClientCanvas::HOnlineMonClientCanvas() {
	splitted = kFALSE;
	active = kFALSE;
	width = 0;
	height = 0;
	nx = 0;
	ny = 0;
	rcanvas = 0;
}
/* -------------------------------------------------------------------------- */
HOnlineMonClientCanvas::~HOnlineMonClientCanvas() {
	// canvas is deleted by window
	listHists.Delete();	// delete the hists
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientCanvas::CreateCanvas(HOnlineMonClientMain* clientmain, TGWindow* window) {
	// create RootCanvas, need a pointer to the win
	// put histos in the main list
	
	rcanvas = new TRootEmbeddedCanvas(this->GetName(), window, this->GetWidth(), this->GetHeight());
	if (this->IsSplitted()) {
		// split canvas
		rcanvas->GetCanvas()->Divide(this->GetNx(), this->GetNy());
	}
	
	
	
	for (Int_t i=0;i<listHists.LastIndex()+1;i++) {
		HOnlineMonClientHist* hist = (HOnlineMonClientHist*) listHists.At(i);
		clientmain->AddHist(this, hist);
	}
	
	this->SetActive(kTRUE);
	
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientCanvas::DestroyCanvas(HOnlineMonClientMain* clientmain) {
	// destroy (RootCanvas is destroyed by the window)
	// remove histos from the main list
	
	this->SetActive(kFALSE);
	
	for (Int_t i=0;i<listHists.LastIndex()+1;i++) {
		HOnlineMonClientHist* hist = (HOnlineMonClientHist*) listHists.At(i);
		clientmain->RemoveHist(this, hist);
		delete hist->histo;
		hist->histo = 0;
	}	
}
/* -------------------------------------------------------------------------- */
TCanvas* HOnlineMonClientCanvas::GetCanvas() {
	if (this->rcanvas) {
		return this->rcanvas->GetCanvas();
	}
	
	return 0;
}
/* -------------------------------------------------------------------------- */
TRootEmbeddedCanvas* HOnlineMonClientCanvas::GetRCanvas() {
	return this->rcanvas;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientCanvas::SetSplitted(Bool_t split) {
	this->splitted = split;
}
/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientCanvas::IsSplitted() {
	return this->splitted;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientCanvas::SetWidth(Int_t width) {
	this->width = width;
}
/* -------------------------------------------------------------------------- */
Int_t HOnlineMonClientCanvas::GetWidth() {
	return this->width;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientCanvas::SetHeight(Int_t height) {
	this->height = height;
}
/* -------------------------------------------------------------------------- */
Int_t HOnlineMonClientCanvas::GetHeight() {
	return this->height;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientCanvas::SetNx(Int_t nx) {
	this->nx = nx;
}
/* -------------------------------------------------------------------------- */
Int_t HOnlineMonClientCanvas::GetNx() {
	return this->nx;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientCanvas::SetNy(Int_t ny) {
	this->ny = ny;
}
/* -------------------------------------------------------------------------- */
Int_t HOnlineMonClientCanvas::GetNy() {
	return this->ny;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientCanvas::SetActive(Bool_t state) {
	for (Int_t i=0;i<listHists.LastIndex()+1;i++) {
	  HOnlineMonClientHist* hist = (HOnlineMonClientHist*) listHists.At(i);
	  hist->SetActive(state);
	}
	
	active = state;
}
/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientCanvas::IsActive() {
	return active;
}
/* -------------------------------------------------------------------------- */


