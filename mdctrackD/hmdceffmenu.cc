using namespace std;
#include <iostream>
#include <iomanip>
#include "hmdceffmenu.h"
#include "hades.h"
#include "heventheader.h"
#include "hevent.h"

#include "TGFrame.h"
#include "TList.h"
#include "TGLabel.h"
#include "TGTextEntry.h"
#include "TGButton.h"
#include "TColor.h"

#include "TStyle.h"
#include <stdlib.h>
#include "hmdcgetcontainers.h"

HMdcEffMenu* HMdcEffMenu::fEffMenu=0;

HMdcEffMenu* HMdcEffMenu::makeMenu(HMdcEfficiency *feff) {
  if(fEffMenu) deleteMenu();
  fEffMenu = new HMdcEffMenu(feff);
  return fEffMenu;
}

void HMdcEffMenu::deleteMenu(void) {
  if(fEffMenu) delete fEffMenu;
  fEffMenu=0;
}

HMdcEffMenu::HMdcEffMenu(HMdcEfficiency *feff) {
  width    = 320;
  height   = 500;
  fWidgets = new TList;
  const TGWindow *main = gClient->GetRoot();
  fDialog  = new TGTransientFrame(main, main, 10, 10);
  fWidgets->Add(fDialog);
  fDialog->SetWMSize(width, height);
  
  
  mdcEff=feff;
  for(Int_t i=0; i<6; i++) indLay[i]=1;
  indPlot[0]  = 0;
  indPlot[1]  = 0;
  indPlot[2]  = 0;
  indHist[0]  = 1;
  indHist[1]  = 0;
  indHist[2]  = 0;
  indHist[3]  = 0;
  indHist[4]  = 0;
  indHist[5]  = 0;
  indHistMb   = 0;
  indXYlayers = 0;
  indTimes    = 0;
  sumBnXY     = 0;
  totModsSel  = 0;
  nTotEv      = 0;
  indSummary  = 0;
  typePlot    = 2;
  numLay      = 6;
  numHists    = 1;
  sumBnTimes  = 1;
  indGroup[0] = 1;
  indGroup[1] = 0;
  eventLoopFlag = kFALSE;
  nEvLoopAdd    = 0;
  text          = new TText();
  lbEvTot       = NULL;
  lbEvLoop      = NULL;
  endOfData     = kFALSE;
  stopLoop      = kFALSE;
    
  gStyle->ToggleEventStatus();
  menu();
}

HMdcEffMenu::~HMdcEffMenu() {
  if(fDialog) fDialog->CloseWindow();
  if(fWidgets) {
    fWidgets->Delete();
    delete fWidgets;
    fWidgets=0;
  }
  fDialog=0;
  if(!fEffMenu) return;
}

void HMdcEffMenu::setSecMod(Int_t s, Int_t m){
  indMod[s][m]^=1;
  (indMod[s][m]) ? totModsSel++:totModsSel--;
  Int_t indSecOld=indSec[s];
  indSec[s]+=(indMod[s][m]) ? +1:-1;
  bSecMod[s][m]->SetTextColor(TColor::Number2Pixel(indMod[s][m]*1+1));
  doDraw(indMod[s][m]);
  if(indSec[s]<=1 && indSecOld<=1) doPlots(indMod[s][m]);
}

void HMdcEffMenu::setLayer(Int_t l){
  indLay[l]^=1;
  bLayer[l]->SetTextColor(TColor::Number2Pixel(indLay[l]*1+1));
  numLay+=indLay[l]*2-1;
  setListLayer();
  if(indGroup[0] || indGroup[1] || indXYlayers) doDraw(indLay[l]);
}

void HMdcEffMenu::setListPads(){
  for(Int_t sec=0; sec<6; sec++) {
    if(!&(*mdcEff)[sec]) continue;
    feffSec=&(*mdcEff)[sec];
    for(Int_t mod=0; mod<4; mod++) {
      feffMod=&(*feffSec)[mod];
      if(!feffMod) continue;
      for(Int_t lay=0; lay<6; lay++) {
        feffLay=&(*feffMod)[lay];
        feffLay->setListPads(indHist);
      }
    }
  }
}

void HMdcEffMenu::setListLayer(){
  for(Int_t sec=0; sec<6; sec++) {
    if(!&(*mdcEff)[sec]) continue;
    feffSec = &(*mdcEff)[sec];
    for(Int_t mod=0; mod<4; mod++) {
      feffMod=&(*feffSec)[mod];
      if(feffMod) feffMod->setListLayers(indLay);
    }
  }
}

void HMdcEffMenu::drawPlot(Int_t n) {
  indPlot[n]^=1;
  bPlot[n]->SetTextColor(TColor::Number2Pixel(indPlot[n]*1+1));
  if(n<2) doPlots(indPlot[n]);
  else doEventPlots(indPlot[n]);
}

void HMdcEffMenu::clustSel() {
  mdcEff->drawClusSelCh();
  Bool_t drSCl = mdcEff->getDrawClusFlag();
  bDrClSel->SetTextColor(TColor::Number2Pixel(drSCl ? 3:1));
  if(indPlot[0] != 0) {
    drawPlot(0);
    drawPlot(0);
  }
  if(indPlot[2] != 0) {
    drawPlot(2);
    drawPlot(2);
  }
}
    

void HMdcEffMenu::setPlotType(Int_t n){
  if(n==typePlot) return;
  bTypePlot[typePlot]->SetTextColor(TColor::Number2Pixel(1));
//  bTypePlot[typePlot]->Modified();
  typePlot=n;
  bTypePlot[typePlot]->SetTextColor(TColor::Number2Pixel(3));
  if(indPlot[0]) doPlots(indPlot[0]);
  if(indPlot[2]) doEventPlots(indPlot[2]);
}

void HMdcEffMenu::drawHist(Int_t n) {
  indHist[n] ^= 1;
  bHist[n]->SetTextColor(TColor::Number2Pixel(indHist[n]*1+1));
  numHists += indHist[n]*2-1;         // increase or decrease numHists
  setListPads();
  doDraw(indHist[n]);
}

void HMdcEffMenu::drawXYlayer() {
  indXYlayers ^= 1;
  bXYlayer->SetTextColor(TColor::Number2Pixel(indXYlayers*1+1));
  doDraw(indXYlayers);
}

void HMdcEffMenu::drawTimes() {
  indTimes^=1;
  bTimes->SetTextColor(TColor::Number2Pixel(indTimes*1+1));
  doDraw(indTimes);
}

void HMdcEffMenu::drawSummary() {
  indSummary^=1;
  bSummary->SetTextColor(TColor::Number2Pixel(indSummary*1+1));
  if(!calcEffFlag) mdcEff->calcEfficiency();
  calcEffFlag = kTRUE;
  if(indSummary) mdcEff->drawSummary(kTRUE);
  else           mdcEff->drawSummary(kFALSE);
}

void HMdcEffMenu::drawHistMb(){
  indHistMb^=1;
  bHistMb->SetTextColor(TColor::Number2Pixel(indHistMb*1+1));
  if(!calcEffFlag) mdcEff->calcEfficiency();
  
  doDraw(indHistMb);
}

void HMdcEffMenu::setGroup(Int_t n) {
  indGroup[n]^=1;
  bGroupMdcLay[n]->SetTextColor(TColor::Number2Pixel(indGroup[n]*1+1));
  doDraw(indGroup[n]);
}

TGTextEntry* HMdcEffMenu::drawTextLine(Int_t x, Int_t y,const Char_t* name,Int_t textColor) { 
  x = sWidth  + x*dWidth;
  y = sHeight + y*dHeight-2;
  TGHorizontalFrame* fr = new TGHorizontalFrame(fDialog);
  TGLayoutHints *lh = new TGLayoutHints(kLHintsNormal,x, 0,y, 0);
  fDialog->AddFrame(fr, lh);
  TGTextEntry *text = new TGTextEntry(fDialog,name);
  text->SetTextColor(TColor::Number2Pixel(textColor));
  text->SetEnabled(kFALSE);
  fr->AddFrame(text,lh);
  fWidgets->Add(fr);
  fWidgets->Add(lh);
  fWidgets->Add(text);
  return text;
}

TGLabel* HMdcEffMenu::drawLabel(Int_t x, Int_t y,const Char_t* name,Int_t textColor) {
  y = sHeight + y*dHeight;
  TGHorizontalFrame *fr = new TGHorizontalFrame(fDialog);
  TGLayoutHints     *lh = new TGLayoutHints(kLHintsNormal,x, 0,y, 0);
  fDialog->AddFrame(fr, lh);
  TGLabel *label = new TGLabel(fDialog,name);
  label->SetTextColor(TColor::Number2Pixel(textColor));
  fr->AddFrame(label,lh);
  fWidgets->Add(fr);
  fWidgets->Add(lh);
  fWidgets->Add(label);
  return label;
}

TGTextButton* HMdcEffMenu::drawButton(Int_t x, Int_t y,const Char_t* name,
    const Char_t* doThis,Int_t textColor) {
  x = sWidth  + x*dWidth;
  y = sHeight + y*dHeight;
  TGHorizontalFrame *fr = new TGHorizontalFrame(fDialog);
  TGLayoutHints     *lh = new TGLayoutHints(kLHintsNormal,x, 0,y, 0);
  fDialog->AddFrame(fr, lh);
  TGTextButton      *b  = new TGTextButton(fDialog,name,doThis);
//  b->SetFont("-misc-fixed-medium-r-normal--12-140-75-75-c-90-iso8859-15",kTRUE);
  b->SetFont("fixed",kTRUE);
  b->SetTextColor(TColor::Number2Pixel(textColor));
  fr->AddFrame(b,lh);
  fWidgets->Add(fr);
  fWidgets->Add(lh);
  fWidgets->Add(b);
  return b;
}

TGTextButton* HMdcEffMenu::drawButtonXp(Int_t px, Int_t y,const Char_t* name,
    const Char_t* doThis,Int_t textColor) {
//-  x = sWidth  + x*dWidth;
  Int_t py = sHeight + y*dHeight;
  TGHorizontalFrame *fr = new TGHorizontalFrame(fDialog);
  TGLayoutHints     *lh = new TGLayoutHints(kLHintsNormal,px, 0,py, 0);
  fDialog->AddFrame(fr, lh);
  TGTextButton      *b  = new TGTextButton(fDialog,name,doThis);
//  b->SetFont("-misc-fixed-medium-r-normal--12-140-75-75-c-90-iso8859-15",kTRUE);
  b->SetFont("fixed",kTRUE);
  b->SetTextColor(TColor::Number2Pixel(textColor));
  fr->AddFrame(b,lh);
  fWidgets->Add(fr);
  fWidgets->Add(lh);
  fWidgets->Add(b);
  return b;
}

void HMdcEffMenu::setInitPoint(Int_t sw,Int_t dw,Int_t sh,Int_t dh) {
  sHeight = sh;
  dHeight = dh;
  sWidth  = sw; 
  dWidth  = dw; 
}

void HMdcEffMenu::menu(){
  gHades->eventLoop(1);
  nTotEv=1;
  TString fileName;
  HMdcGetContainers::getFileName(fileName);
  fileName+=" MdcEffMon";
  fDialog->SetWindowName(fileName.Data());
  for(Int_t s=0; s<6; s++){
    indSec[s]=0;
    for(Int_t m=0; m<4; m++){
      indMod[s][m]=0;
    }
  }
  setListLayer();
  setListPads();
  
  Int_t w03 = 12;
  setInitPoint(119,27,5,23);
  drawLabel(w03,0,"Sector :   ",1);
  drawLabel(w03,1,"Module :   ",1);
  
  for(Int_t s=0; s<6; s++) {
    sprintf(name,"%i",s+1);
    drawLabel(sWidth+s*dWidth+8,0,name,1);
    if(!&(*mdcEff)[s]) continue;
    HMdcEfficiencySec& effSec=(*mdcEff)[s];
    for(Int_t m=0; m<4; m++) {
      if(!&effSec[m]) continue;
      TString modName;
      if(m==0) modName="I  ";
      else if(m==1) modName="II ";
      else if(m==2) modName="III";
      else if(m==3) modName="IV ";
      sprintf(doThis,"effMenu->setSecMod(%i,%i);",s,m);
      bSecMod[s][m] = drawButton(s,m+1,(Char_t*)modName.Data(),doThis,indMod[s][m]*1+1);
    }
  }

  setInitPoint(119,27,10,23);
  drawLabel(w03,5,"Layer:  ",1);
  for(Int_t l=0; l<6; l++) {
    sprintf(name," %i ",l+1);
    sprintf(doThis,"effMenu->setLayer(%i);",l);
    bLayer[l] = drawButton(l,5,name,doThis,indLay[l]*1+1);
  }

  // Histogram buttons:  
  setInitPoint(62,74,165,23);
//  drawLabel(w03,0,"Grouping :     :",1);
  bGroupMdcLay[0] = drawButton(1,0,"    MDC    ","effMenu->setGroup(0)",indGroup[0]*1+1);
  bGroupMdcLay[1] = drawButton(2,0,"   Layer   ","effMenu->setGroup(1)",indGroup[1]*1+1);
  
  bHist[2] = drawButton(0,1,"Efficiency ","effMenu->drawHist(2)",indHist[2]*1+1);
  bHist[0] = drawButton(1,1,"  Pattern  ","effMenu->drawHist(0)",indHist[0]*1+1);
  bHist[3] = drawButton(2,1,"  Single   ","effMenu->drawHist(3)",indHist[3]*1+1);
  
  bHist[5] = drawButton(0,2,"  Ratio    ","effMenu->drawHist(5)",indHist[5]*1+1);
  bHist[1] = drawButton(1,2,"Tracks|Hits","effMenu->drawHist(1)",indHist[1]*1+1);
  bHist[4] = drawButton(2,2,"Probability","effMenu->drawHist(4)",indHist[4]*1+1);
  
  setInitPoint(5,74,239,33);
  bHistMb  = drawButton(0,0,"Eff.vs.Mb","effMenu->drawHistMb()",indHistMb*1+1);
  bXYlayer = drawButton(0,1,"Eff.vs.XY   ","effMenu->drawXYlayer()",indXYlayers*1+1);
  bTimes   = drawButton(0,2,"Time 1,2,2-1","effMenu->drawTimes()",indTimes*1+1);
  
  bSummary = drawButton(0,3,"Eff.Summary","effMenu->drawSummary()",indSummary*1+1);
  
  setInitPoint(230,28,239,33);
  button[17] = drawButton(0,0,"Refresh","effMenu->calcEff()",1);
  
  setInitPoint(5,28,239,33);
  button[0] = drawButton(6,1,"-1","effMenu->rebinXY(-1)",4);
  button[1] = drawButton(8,1,"+1","effMenu->rebinXY(+1)",4);
  pvLSumXY  = drawTextLine(7,1," 1 ",3);
  
  button[6] = drawButton(5,2,"-5","effMenu->rebinTimes(-5)",4);
  button[7] = drawButton(6,2,"-1","effMenu->rebinTimes(-5)",4);
  button[8] = drawButton(8,2,"+1","effMenu->rebinTimes(+1)",4);
  button[9] = drawButton(9,2,"+5","effMenu->rebinTimes(+5)",4);
  pvLSumTimes = drawTextLine(7,2," 1 ",3);

  // Buttons for plots:
  setInitPoint(90,28,200,23);
  drawLabel(w03,8,"Plots:",1);
  bTypePlot[0] = drawButton(0,7," A ","effMenu->setPlotType(0)",(typePlot==0)?3:1);
  bTypePlot[1] = drawButton(1,7," B ","effMenu->setPlotType(1)",(typePlot==1)?3:1);
  bTypePlot[2] = drawButton(2,7," C ","effMenu->setPlotType(2)",(typePlot==2)?3:1);
  bPlot[2]     = drawButton(3,7,"Event   ","effMenu->drawPlot(2)",indPlot[2]*1+1);
  bPlot[0]     = drawButton(0,8,"Projections ","effMenu->drawPlot(0)",indPlot[0]*1+1);
  bPlot[1]     = drawButton(3,8,"MDC hits","effMenu->drawPlot(1)",indPlot[1]*1+1);
  button[10]   = drawButton(0,9,"Print clusters","effMenu->printClusters()",1);
  Bool_t drSCl = mdcEff->getDrawClusFlag();
  bDrClSel     = drawButton(4,9,"GoodClusters","effMenu->clustSel()",drSCl ? 3:1);

   // Buttons for event loop:
  setInitPoint(35,42,200,23);
//  drawLabel(w03,11,"Event loop:",1);
  drawNEvents(0);
  button[11] = drawButton(2,11,"Next ","effMenu->loopEvents(1)",1);
  button[12] = drawButton(3,11,"All   ","effMenu->loopEvents(-1)",1);
  button[13] = drawButton(0,12,"100  ","effMenu->loopEvents(100)",1);
  button[14] = drawButton(1,12,"1000 ","effMenu->loopEvents(1000)",1);
  button[15] = drawButton(2,12,"10000","effMenu->loopEvents(10000)",1);
//  button[16] = drawButton(3,12,"100000","effMenu->loopEvents(100000)",1);
  button[19] = drawButton(3,12," Stop ","effMenu->stpLoop()",1);
  button[18] = drawButton(6,12,"Quit","effMenu->closeDial(); delete gHades;exit(0);",1);
  button[20] = drawButtonXp(sWidth+5*dWidth+40,11,"PDF","effMenu->saveHist()",1);
//button[19] = drawButtonXp(3,12,"Stop","effMenu->stpLoop()",1);
//  button[21] = drawButton(5,12,"Quit ROOT ",".q",1);
//  drawNEvents(0);
  
   
  fDialog->MapSubwindows(); 
  
  fDialog->MapWindow();
//  gClient->WaitFor(fDialog);
}

void HMdcEffMenu::closeDial(void) {
  fDialog->CloseWindow();
}

void HMdcEffMenu::calcEff(void) {
  mdcEff->calcEfficiency(calcEffFlag);
}

void HMdcEffMenu::rebinTimes(Int_t n){
  if(sumBnTimes==1 && n<0) return;
  sumBnTimes+=n;
  if(sumBnTimes<1) sumBnTimes=1;
  if(sumBnTimes>100) sumBnTimes=100;
  sprintf(name,"%i",sumBnTimes);
  pvLSumTimes->SetText(name);
  doDraw();
}

void HMdcEffMenu::rebinXY(Int_t n) {
  if((sumBnXY==0 && n<0) || (sumBnXY==3 && n>0)) return;
  sumBnXY+=n;
  Int_t sBn=1;
  for(Int_t i=0; i<sumBnXY; i++) sBn*=2;
  sprintf(name,"%i",sBn);
  pvLSumXY->SetText(name);
  for(Int_t sec=0; sec<6; sec++) {
    if(!&(*mdcEff)[sec]) continue;
    feffSec=&(*mdcEff)[sec];
    for(Int_t mod=0; mod<4; mod++) {
     if(!&(*feffSec)[mod]) continue;
     feffMod=&(*feffSec)[mod];
     for(Int_t lay=0; lay<6; lay++) {
       feffLay=&(*feffMod)[lay];
       feffLay->setBiningXY(sBn);
     }
   }
  }
// doDraw();
}

void HMdcEffMenu::printClusters(){
  for(Int_t sec=0; sec<6; sec++) {
    if(indSec[sec]) {
      printf("\n---------------- Sector %i ---------------------------\n",sec+1);
      mdcEff->printClusters(sec); // mod ???
    }
  }
}

void HMdcEffMenu::doDraw(Int_t ind){
  //ind=1 - draw canvas
  //   =0 - close canvas
  if(!calcEffFlag || lbEvLoop != NULL) mdcEff->calcEfficiency();
  calcEffFlag = kTRUE;
  for(Int_t sec=0; sec<6; sec++) {
    if(!&(*mdcEff)[sec]) continue;
    feffSec=&(*mdcEff)[sec];
    for(Int_t mod=0; mod<4; mod++) {
      if(!&(*feffSec)[mod]) continue;
      feffMod=&(*feffSec)[mod];                          //Drawing
      if( ind && indMod[sec][mod] ) {
        if( indHistMb ) feffMod->drawCanvEffMb();
        if( indTimes ) feffMod->drawCanvTimes(sumBnTimes);
        if(numLay) {
          if(indGroup[0]) {
            if(indHist[0]) feffMod->drawCanvTotNhits();
            if(indHist[1]) feffMod->drawCanvNHits();
            if(indHist[2]) feffMod->drawCanvHistEff();
            if(indHist[3]) feffMod->drawCanvHistSing();
            if(indHist[4]) feffMod->drawCanvProbabilitis();
            if(indHist[5]) feffMod->drawCanvRatio();
          }
          if(indGroup[1]) for(Int_t lay=0; lay<6; lay++) {
            if(!indLay[lay]) continue;
            feffLay=&(*feffMod)[lay];
            if(numHists) feffLay->drawCanvas();
          }
          if(indXYlayers) for(Int_t lay=0; lay<6; lay++) {
            if(!indLay[lay]) continue;
            feffLay=&(*feffMod)[lay];
            if(numHists) feffLay->drawCanvXYlist();
          }
        }
      } else {
        if( !indHistMb || !indMod[sec][mod] )  feffMod->delCanvMb();
        if( !indTimes || !indMod[sec][mod] ) feffMod->delCanvTimes();
        Int_t indCls=(indMod[sec][mod]>0 && numLay>0) ? 0 : 1;
        if(!indHist[0] || indCls || !indGroup[0]) feffMod->delCanvTotNhits();
        if(!indHist[1] || indCls || !indGroup[0]) feffMod->delCanvNHits();
        if(!indHist[2] || indCls || !indGroup[0]) feffMod->delCanvHistEff();
        if(!indHist[3] || indCls || !indGroup[0]) feffMod->delCanvHistSing();
        if(!indHist[4] || indCls || !indGroup[0]) feffMod->delCanvProbabilitis();
        if(!indHist[5] || indCls || !indGroup[0]) feffMod->delCanvRatio();
        for(Int_t lay=0; lay<6; lay++) {
          feffLay=&(*feffMod)[lay];
          if(indMod[sec][mod]==0 || indLay[lay]==0 || numHists==0 || 
              indGroup[1]==0) feffLay->delCanvas();
          if(!indXYlayers || indCls || indLay[lay]==0) feffLay->delCanvXYlist();
        }
      }
    }
  }
}

void HMdcEffMenu::doPlots(Int_t ind){
  //ind=1 - draw canvas
  //   =0 - close canvas
  for(Int_t sec=0; sec<6; sec++) {
    if(!&(*mdcEff)[sec]) continue;
    feffSec=&(*mdcEff)[sec];
    Int_t ind1=(indPlot[0] && indSec[sec]) ? 1 : 0;
    Int_t ind2=(indPlot[1] && indSec[sec]) ? 1 : 0;
    if(ind) {
      if(ind1) {feffSec->drawCanvProj(); mdcEff->drawProjPlotsSec(sec,typePlot);}
      if(ind2) {feffSec->drawCanvHits(); mdcEff->drawHitsPlots(sec);}
    } else {
      if(!ind1) feffSec->delCanvProj();
      if(!ind2) feffSec->delCanvHits();
    }
  }
}

void HMdcEffMenu::doEventPlots(Int_t ind){
 //ind=1 - draw canvas
 //   =0 - close canvas
  if(ind) {mdcEff->drawCanvEvent(); mdcEff->drawPrPlEvent(typePlot);}
  else mdcEff->delCanvEvent();
}

void HMdcEffMenu::drawNEvents(Int_t nEv) {
  Char_t buf[20];
  sprintf(buf,"%08i    U",nTotEv);
  if(lbEvTot == NULL) {
    lbEvTot  = drawLabel(6,10,buf,1);
    drawLabel(62,10,"events   ",1);
    lbEvLoop = drawLabel(6,11,"Loop :   ",1);
  } else lbEvTot->ChangeText(buf);
  
  if(endOfData) {
    lbEvLoop->ChangeText(" End!       ");
    lbEvLoop->SetTextColor(TColor::Number2Pixel(2));
  } if(nEv > 0) {
    sprintf(buf,"%8i     UU",nEv);
    lbEvLoop->ChangeText(buf);
    lbEvLoop->SetTextColor(TColor::Number2Pixel(2));
  } else if(nEv<0 || nEvLoopAdd<0) {
    lbEvLoop->ChangeText("All ev.     ");
    lbEvLoop->SetTextColor(TColor::Number2Pixel(2));
  } else {
    lbEvLoop->ChangeText("Loop :      ");
    lbEvLoop->SetTextColor(TColor::Number2Pixel(1));
  }
}

void HMdcEffMenu::loopEvents(Int_t nEv) {
  if(endOfData) return;
  if(eventLoopFlag) {
    if(nEv<1) nEvLoopAdd = -1;
    else      nEvLoopAdd += nEv;
    drawNEvents(nEvLoopAdd);
    return;
  }
  drawNEvents(nEv);
  eventLoopFlag = kTRUE;
  gHades->setQuietMode(2);
  Int_t nEvDo = (nEv<10) ? 1 : nEv/10; 
  Int_t nfor  = nEv/nEvDo;
  if(nEv == -1) {
    nEvDo = 1000;
    nfor  = 10;
  }

  calcEffFlag = kTRUE;
  if(totModsSel == 0 || (!indGroup[0]&&!indGroup[1]&&!indHistMb&&!indXYlayers&&!indTimes))
      calcEffFlag = kFALSE;
//   if(!calcEffFlag && nEv>0 && !indPlot[0] && !indPlot[1] && !indPlot[2] ) {
//     nEvDo = nEv;
//     nfor  = 1;
//   }
  Int_t evN = 0;
  for(Int_t nTm=0; nTm<nfor; nTm++) {
    if(stopLoop) break;
    evN = gHades->eventLoop(nEvDo);
    if(nTm+1 < nfor) mdcEff->calcEfficiency(calcEffFlag);
    if(evN <= 0) break;
    if(nEv == -1) nTm=-1;
    nTotEv += evN;
    if(evN<nEvDo) break;
    if(nTm<nfor-1 && nEvDo>50) {
      if(nEv<0) drawNEvents(-1);
      else      drawNEvents(nfor*nEvDo-nEvDo*(nTm+1)+nEvLoopAdd);
    }
    if(nTm<nfor-1 && nEvDo>100) printf("--- Total num. of events processed: %i ---\n",nTotEv);
  }
  if(stopLoop) stopLoop = kFALSE;
  
  if(evN>0 && evN>=nEvDo) {
    HEvent* event=gHades->getCurrentEvent();
    if(event) { 
      HEventHeader* header=event->getHeader();
      if(header) {
        while(kTRUE) {
          Int_t evId = header->getId();
          if((evId==0 || evId==1) && header->getEventSeqNumber()==mdcEff->getEventId()) break;
          Int_t evN = gHades->eventLoop(1);
          if(evN<=0) break;
          nTotEv += evN;
        }
      }
    }
  }
  mdcEff->calcEfficiency(calcEffFlag);
  if(evN > 0) printf("==== Total num. of events processed: %i ====\n",nTotEv);
  else if(evN < 0) printf("==== The error of reading event. evN=%i\n",evN);
  if(evN>=0 && evN<nEvDo) {
    endOfData = kTRUE;
    drawNEvents(0);
    printf("==== End of file! Total num. of events processed: %i ====\n",nTotEv);
    return;
  }
  eventLoopFlag = kFALSE;
  if(nEvLoopAdd > 0) {
    Int_t nAdd = nEvLoopAdd;
    nEvLoopAdd = 0;
    loopEvents(nAdd);
  }
  drawNEvents(0);
}

ClassImp(HMdcEffMenu)
