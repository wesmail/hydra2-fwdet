#ifndef __EDGUI__
#define __EDGUI__

#include "TObject.h"
#include "TObjArray.h"
#include "GuiTypes.h"

#include "TRootBrowser.h"
#include "TString.h"
#include "TSystem.h"

#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveGeoNode.h"

#include "TGFrame.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGNumberEntry.h"
#include "TGColorSelect.h"


//----------------------------------------------------------------
class HEDColorDef : public TObject {

private:
static HEDColorDef* gEDColorDef;

    HEDColorDef(){;}
public:

    ~HEDColorDef(){;}

    TObjArray volSEC;
    TObjArray volRICH;
    TObjArray volMDC;
    TObjArray volTOF;
    TObjArray volRPC;
    TObjArray volSHOWER;
    TObjArray volWALL;
    TObjArray volCOILS;
    TObjArray volFRAMES;

    TObjArray nodesMDC;
    TObjArray nodesTOF;
    TObjArray nodesRPC;
    TObjArray nodesSHOWER;
    TObjArray nodesWALL;

    static TEveGeoTopNode* gGeoTopNode;


    static Int_t colMDC[4];
    static Int_t colTOF;
    static Int_t colRPC;
    static Int_t colSHOWER;
    static Int_t colRICH;
    static Int_t colWALL;
    static Int_t colFRAMES;
    static Int_t colCOILS;

    static Int_t transMDC[4];
    static Int_t transTOF;
    static Int_t transRPC;
    static Int_t transSHOWER;
    static Int_t transRICH;
    static Int_t transWALL;
    static Int_t transFRAMES;
    static Int_t transCOILS;

    static  HEDColorDef* getEDColorDef(){
	if(gEDColorDef ==0) {
	    gEDColorDef = new HEDColorDef();
            gEDColorDef->setDefault();
	}
        return gEDColorDef;
    }
    static void  setDefault(){

	colMDC[0] = kRed;
	colMDC[1] = kRed;
	colMDC[2] = kRed;
	colMDC[3] = kRed;
        colTOF    = kGreen;
        colRPC    = kBlue;
        colSHOWER = kMagenta;
        colRICH   = kGray;
        colWALL   = kGray;
        colFRAMES = kYellow;
	colCOILS  = kGray;

	transMDC[0] = 60;
        transMDC[1] = 60;
        transMDC[2] = 60;
        transMDC[3] = 60;
        transTOF    = 60;
        transRPC    = 85;
        transSHOWER = 60;
        transRICH   = 60;
        transWALL   = 60;
        transFRAMES = 60;
	transCOILS  = 60;
    }
    void showSec   (Bool_t show = kTRUE,Int_t sec =-1);
    void showCoils (Bool_t show = kTRUE);
    void showFrames(Bool_t show = kTRUE);
    void showWall  (Bool_t show = kTRUE);
    void showRich  (Bool_t show = kTRUE);
    void showMdc   (Bool_t show = kTRUE, Int_t sec = -1, Int_t mod = -1);
    void showTof   (Bool_t show = kTRUE, Int_t sec = -1);
    void showRpc   (Bool_t show = kTRUE, Int_t sec = -1);
    void showShower(Bool_t show = kTRUE, Int_t sec = -1);

    void colCoils (Pixel_t pixel,Int_t trans = -1);
    void colFrames(Pixel_t pixel,Int_t trans = -1);
    void colRich  (Pixel_t pixel,Int_t trans = -1);
    void colMdc   (Pixel_t pixel,Int_t trans = -1,Int_t mod =-1);
    void colTof   (Pixel_t pixel,Int_t trans = -1);
    void colRpc   (Pixel_t pixel,Int_t trans = -1);
    void colShower(Pixel_t pixel,Int_t trans = -1);
    void colWall  (Pixel_t pixel,Int_t trans = -1);


  ClassDef(HEDColorDef,0)
};
//----------------------------------------------------------------

//----------------------------------------------------------------
class HEDNumberLabeled : public TGHorizontalFrame {

protected:
   TGNumberEntry *fEntry;  //! pointer to element to get the numbers

public:
   HEDNumberLabeled(const TGWindow *p, const char *name) : TGHorizontalFrame(p)
   {
      fEntry = new TGNumberEntry(this, 0, 6, -1, TGNumberFormat::kNESInteger);
      AddFrame(fEntry, new TGLayoutHints(kLHintsLeft));
      TGLabel *label = new TGLabel(this, name);
      AddFrame(label, new TGLayoutHints(kLHintsLeft, 10));
   }
   ~HEDNumberLabeled(){;}

   TGTextEntry*   GetEntry()         const { return fEntry->GetNumberEntry(); }
   TGNumberEntry* GetTGNumberEntry() const { return fEntry; }

   ClassDef(HEDNumberLabeled, 0)
};
//----------------------------------------------------------------


//----------------------------------------------------------------
class HEDIDList {

private:
   Int_t nID;   //! creates unique widget's IDs

public:
   HEDIDList() : nID(0) {}
   ~HEDIDList() {}
   Int_t GetUnID(void) { return ++nID; }
};
//----------------------------------------------------------------


//----------------------------------------------------------------
class HEDSetup : public TGMainFrame {

private:

    static  HEDSetup* gEDSetup;    //!

    HEDSetup(const TGWindow *p, UInt_t w, UInt_t h);
public:

    static HEDSetup*  getEDSetup(const TGWindow *p=0, UInt_t w=0, UInt_t h=0){
	if(gEDSetup == 0) {
               gEDSetup = new HEDSetup(p,w,h);
	}
        return gEDSetup;
    }

    TGCheckButton       *fCheckSector[6]; //! Check buttons Sector selection
    TGCheckButton       *fCheckFRAMES;    //! Check buttons show FRAMES
    TGCheckButton       *fCheckCOILS;     //! Check buttons show COLIS
    TGCheckButton       *fCheckRICH;      //! Check buttons show RICH
    TGCheckButton       *fCheckMDC[4];    //! Check buttons show MDC
    TGCheckButton       *fCheckTOF;       //! Check buttons show TOF
    TGCheckButton       *fCheckRPC;       //! Check buttons show RPC
    TGCheckButton       *fCheckSHOWER;    //! Check buttons show SHOWER
    TGCheckButton       *fCheckWALL;      //! Check buttons show WALL

    TGTextButton        *fNext;           //! call nextEvent() on click text button
    TGTextButton        *fNextLoop;       //! call nextEventLoop() on click text button
    TGTextButton        *fRedraw;         //! call reDraw() on click text button
    TGCheckButton       *fCheckTimed;     //! run nextEvent() with timer
    HEDNumberLabeled    *fNumberLoopTime; //! loop time in seconds

    TGCheckButton       *fCheckMDCMod[6][4]; //! swich MDC modules
    TGCheckButton       *fCheckTOFSec[6];    //! swich TOF sectors
    TGCheckButton       *fCheckRPCSec[6];    //! swich RPC    sectors
    TGCheckButton       *fCheckSHOWERSec[6]; //! swich SHOWER sectors
    HEDIDList           IDs;                 //! Widget IDs generator

    TGColorSelect*      colFRAMES;
    TGColorSelect*      colCOILS;
    TGColorSelect*      colRICH;
    TGColorSelect*      colMDC[4];
    TGColorSelect*      colTOF;
    TGColorSelect*      colRPC;
    TGColorSelect*      colSHOWER;
    TGColorSelect*      colWALL;

    TGNumberEntry*      transFRAMES;
    TGNumberEntry*      transCOILS;
    TGNumberEntry*      transRICH;
    TGNumberEntry*      transMDC[4];
    TGNumberEntry*      transTOF;
    TGNumberEntry*      transRPC;
    TGNumberEntry*      transSHOWER;
    TGNumberEntry*      transWALL;

    virtual ~HEDSetup();
    void reDraw();
    ClassDef(HEDSetup, 0)
};
//----------------------------------------------------------------



#endif  // GUI definition







