#ifndef HMDCEFFMENU_H
#define HMDCEFFMENU_H

#include "TObject.h"
#include "hmdcefficiency.h"

#include "TDialogCanvas.h"
#include "TButton.h"
#include "TPaveLabel.h"
#include "TText.h"

class TGTransientFrame;
class TList;
class TGLabel;
class TGTextEntry;
class TGTextButton;

class HMdcEffMenu : public TObject {
  protected:
    TGTransientFrame* fDialog;
    TList*            fWidgets;
    Int_t width;                 // dialog frame size 
    Int_t height;                // dialog frame size
    Int_t sHeight;               // start pozition for buttons
    Int_t dHeight;               // step size for buttons
    Int_t sWidth;                // start pozition for buttons
    Int_t dWidth;                // step size for buttons
    Bool_t stopLoop;
  
    static HMdcEffMenu* fEffMenu;
    HMdcEfficiency* mdcEff;
    HMdcEfficiencySec* feffSec;
    HMdcEfficiencyMod* feffMod;
    HMdcEfficiencyLayer* feffLay;
    TGTextButton *bSecMod[6][4];
    TGTextButton *bLayer[6];
    TGTextButton *bGroupMdcLay[2];
    TGTextButton *button[30];
    TGTextButton *bPlot[3];
    TGTextButton *bTypePlot[3];
    TGTextButton *bDrClSel;
    TGTextButton *bHist[6];
    TGTextButton *bHistMb;
    TGTextButton *bXYlayer;
    TGTextButton *bTimes;
    TGTextButton *bSummary;
    Int_t         indSec[6];
    Int_t         indMod[6][4];
    Int_t         indLay[6];          //for all selected modules
    Int_t         numLay;
    Int_t         indPlot[3];
    Int_t         typePlot;
    Int_t         indHist[6];
    Int_t         indHistMb;
    Int_t         numHists;
    Int_t         indXYlayers;
    Int_t         indTimes;
    Int_t         indSummary;
    Int_t         indGroup[2];
    Int_t         sumBnXY;
    Int_t         sumBnTimes;
    TGTextEntry  *pvLSumXY;
    TGTextEntry  *pvLSumTimes;
    Char_t        name[100];
    Char_t        doThis[100];
    TText*        text;
    Int_t         totModsSel;
    Int_t         nTotEv;
    Bool_t        calcEffFlag;
    Bool_t        eventLoopFlag;
    Int_t         nEvLoopAdd;
    Bool_t        endOfData;
    TGLabel      *lbEvTot;
    TGLabel      *lbEvLoop;
    
    HMdcEffMenu(HMdcEfficiency *feff);
    ~HMdcEffMenu();
    void menu();
    TGTextEntry* drawTextLine(Int_t x, Int_t y,const Char_t* name,Int_t textColor);
    TGLabel* drawLabel(Int_t x, Int_t y,const Char_t* name,Int_t textColor);
    TGTextButton* drawButton(Int_t x, Int_t y,const Char_t* name,const Char_t* doThis,Int_t textColor);
    TGTextButton* drawButtonXp(Int_t px, Int_t ,const Char_t* name,const Char_t* doThis,Int_t textColor);
    void setInitPoint(Int_t sw,Int_t dw,Int_t sh,Int_t dh);
    void drawNEvents(Int_t nEv);
    
  public:
    static HMdcEffMenu* makeMenu(HMdcEfficiency *feff);
    static void deleteMenu(void);
    void closeDial(void);
    void setSecMod(Int_t s, Int_t m);
    void setLayer(Int_t l);
    void setListPads();
    void setListLayer();
    void drawPlot(Int_t n);
    void setPlotType(Int_t n);
    void drawHist(Int_t n);
    void drawXYlayer();
    void drawTimes();
    void drawSummary();
    void drawHistMb();
    void setGroup(Int_t n);
    void rebinTimes(Int_t n);
    void rebinXY(Int_t n);
    void printClusters();
    void doDraw(Int_t ind=1);
    void doPlots(Int_t ind=1);
    void doEventPlots(Int_t ind=1);
    void loopEvents(Int_t nEv);
    void calcEff(void);
    void saveHist(void) {mdcEff->saveHist();}
    void clustSel();
    void stpLoop()      {stopLoop = kTRUE;}
    ClassDef(HMdcEffMenu,0) //Digitizes MDC data.
};
    
#endif
