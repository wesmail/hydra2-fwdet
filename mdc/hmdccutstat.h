#ifndef HMDCCUTSTAT_H
#define HMDCCUTSTAT_H

#include "hparset.h"
#include <stdio.h>
#include "TObjArray.h"

class HMdcDetector;

class HMdcCutStatMod : public TObject {
protected:
  Int_t Calibrater1cutT1;   // Cut 1:  cutin time1
  Int_t Calibrater1cutT2;   // Cut 2:  cut in time2
  Int_t Calibrater1cutT12;  // Cut 3:  cut in t2-t1
  Int_t Calibrater1cutWire; // Cut 4:  cut wires<4
  Int_t Calibrater1cutT1T12;// Cut 5:  cut t1 and t12
  Int_t Calibrater1cutBump; // Cut 6:  bump cut t1 + tot
  Int_t Calibrater1All;     // Cut 7:  all
  Int_t Calibrater1cutAll;  // Cut 8:  cut all

public:
  HMdcCutStatMod() { clear(); }
  HMdcCutStatMod(HMdcCutStatMod &tc):TObject(tc) {fill(tc);}
  ~HMdcCutStatMod() {}
  void setCal1StatCutT1() {
       Calibrater1cutT1++;
  }
  void setCal1StatCutT2() {
       Calibrater1cutT2++;
  }
  void setCal1StatCutT12() {
       Calibrater1cutT12++;
  }
  void setCal1StatCutWire() {
       Calibrater1cutWire++;
  }
  void setCal1StatCutT1T12() {
       Calibrater1cutT1T12++;
  }
  void setCal1StatCutBump() {
       Calibrater1cutBump++;
  }
  void setCal1StatCutAll() {
       Calibrater1cutAll++;
  }
  void setCal1StatAll() {
       Calibrater1All++;
  }
  Int_t getCal1StatCutT1(void)   { return Calibrater1cutT1; }
  Int_t getCal1StatCutT2(void)   { return Calibrater1cutT2; }
  Int_t getCal1StatCutT12(void)  { return Calibrater1cutT12; }
  Int_t getCal1StatCutWire(void) { return Calibrater1cutWire; }
  Int_t getCal1StatCutT1T12(void){ return Calibrater1cutT1T12; }
  Int_t getCal1StatCutBump(void) { return Calibrater1cutBump; }
  Int_t getCal1StatCutAll(void)  { return Calibrater1cutAll; }
  Int_t getCal1StatAll(void)     { return Calibrater1All; }
  
  void fill(HMdcCutStatMod &tc) {
    Calibrater1cutT1   = tc.Calibrater1cutT1;
    Calibrater1cutT2   = tc.Calibrater1cutT2;
    Calibrater1cutT12  = tc.Calibrater1cutT12;
    Calibrater1cutWire = tc.Calibrater1cutWire;
    Calibrater1cutT1T12= tc.Calibrater1cutT1T12;
    Calibrater1cutBump = tc.Calibrater1cutBump;
    Calibrater1cutAll  = tc.Calibrater1cutAll;
    Calibrater1All     = tc.Calibrater1All;
  }
  void fill(Int_t cT1,Int_t cT2,Int_t cT12,Int_t cWire,Int_t cT1T12,Int_t cBump,Int_t cAll,Int_t all) {
      Calibrater1cutT1   = cT1;
      Calibrater1cutT2   = cT12;
      Calibrater1cutT12  = cT12;
      Calibrater1cutWire = cWire;
      Calibrater1cutT1T12= cT1T12;
      Calibrater1cutT1T12= cBump;
      Calibrater1cutAll  = cAll;
      Calibrater1All     = all;
  }
  void clear(void) {
    fill(0,0,0,0,0,0,0,0);
  }
  void print(void) {
    printf("%10i %10i %10i %10i %10i %10i %10i %10i \n",
	   Calibrater1cutT1,Calibrater1cutT2,Calibrater1cutT12,Calibrater1cutWire,
	   Calibrater1cutT1T12,Calibrater1cutBump,Calibrater1cutAll,Calibrater1All);
  }
  void printRatio(void) {

      if(Calibrater1All!=0)
      {
	  printf("%5i%% %5i%% %5i%% %5i%% %5i%% %5i%% %5i%% %10i cts \n",
		 100*Calibrater1cutT1/Calibrater1All   ,100*Calibrater1cutT2/Calibrater1All,
		 100*Calibrater1cutT12/Calibrater1All  ,100*Calibrater1cutWire/Calibrater1All,
		 100*Calibrater1cutT1T12/Calibrater1All,100*Calibrater1cutBump/Calibrater1All,
		 100*Calibrater1cutAll/Calibrater1All  ,Calibrater1All);
      }
      else
      {
       	  printf("%5i%% %5i%% %5i%% %5i%% %5i%% %5i%% %5i%% %10i cts \n",0,0,0,0,0,0,0,0);
      }
  }

  ClassDef(HMdcCutStatMod,2)
};

class HMdcCutStatSec : public TObject {
 protected:
  TObjArray fMod;
 public:
  HMdcCutStatSec(Int_t mods=4) : fMod(mods) {
    for (Int_t i=0; i<mods; i++) fMod.AddAt(new HMdcCutStatMod,i);
  }
  ~HMdcCutStatSec(void) {}
  HMdcCutStatMod& operator[](Int_t i) {
    return *( static_cast<HMdcCutStatMod*>(fMod[i]) );
  }
  Int_t getSize() {return fMod.GetEntries();}
  ClassDef(HMdcCutStatSec,1)
};

class HMdcCutStat : public HParSet {
protected:
  TObjArray fSecs; // Sectors containins modules containing cuts
  void setNameTitle();
  HMdcDetector *fMdc; 
  static HMdcCutStat* fMdcCutStat;
  HMdcCutStat(const Char_t* name="MdcCutStat",
              const Char_t* title="Statistic  on Cuts on time1, time2 & time2-time1 and wires<4",
              const Char_t* context="",
              Int_t secs=6,Int_t mods=4);
  ~HMdcCutStat();
  void  setCal1StatCutT1(Int_t sec, Int_t mod) {
      (*this)[sec][mod].setCal1StatCutT1();
  }
  void  setCal1StatCutT2(Int_t sec, Int_t mod) {
      (*this)[sec][mod].setCal1StatCutT2();
  }
  void  setCal1StatCutT12(Int_t sec, Int_t mod) {
      (*this)[sec][mod].setCal1StatCutT12();
  }
  void  setCal1StatCutWire(Int_t sec, Int_t mod) {
      (*this)[sec][mod].setCal1StatCutWire();
  }
  Int_t  getCal1StatCutT1(Int_t sec, Int_t mod) {
    return (*this)[sec][mod].getCal1StatCutT1();
  }
  Int_t  getCal1StatCutT2(Int_t sec, Int_t mod) {
    return (*this)[sec][mod].getCal1StatCutT2();
  }
  Int_t  getCal1StatCutT12(Int_t sec, Int_t mod) {
    return (*this)[sec][mod].getCal1StatCutT12();
  }
  Int_t  getCal1StatCutWire(Int_t sec, Int_t mod) {
    return (*this)[sec][mod].getCal1StatCutWire();
  }
  Int_t  getCal1StatCutT1T12(Int_t sec, Int_t mod) {
    return (*this)[sec][mod].getCal1StatCutT1T12();
  }
  Int_t  getCal1StatCutAll(Int_t sec, Int_t mod) {
    return (*this)[sec][mod].getCal1StatCutAll();
  }
  Int_t  getCal1StatAll(Int_t sec, Int_t mod) {
    return (*this)[sec][mod].getCal1StatAll();
  }
public:
  static void deleteHMdcCutStat();
  static HMdcCutStat* getExObject();
  static HMdcCutStat* getObject();
  void getCal1StatCut(Int_t,Int_t,Int_t *,Int_t *,Int_t *,Int_t *,Int_t *,Int_t *,Int_t *,Int_t *);
  void setCal1StatCut(Int_t,Int_t,Int_t,Int_t,Int_t,Int_t,Int_t);
  Bool_t init(HParIo* input,Int_t* set);
  Int_t write(HParIo* output) ;
  void putAsciiHeader(TString&);
  Bool_t writeline(Char_t*, Int_t, Int_t);

  HMdcCutStatSec& operator[](Int_t i) {
    return *( static_cast<HMdcCutStatSec*>(fSecs[i]) );
  }
  Int_t getSize() {return fSecs.GetEntries();}
  void clear(void);
  void printParam(void) {
    printf ("HMdcCutStat:\n");
    printf("s m    t1     t2     t12    wire   t1_12   Bump  cutall       all \n");
    for (Int_t i=0;i<getSize();i++)    {
      HMdcCutStatSec &sector = (*this)[i];
      for (Int_t m=0; m<sector.getSize(); m++) {
	HMdcCutStatMod &mod = sector[m];
	printf("%i %i ",i,m);
        mod.printRatio();
      }
    }
  }
  ClassDef(HMdcCutStat,1) 
};

#endif  /*!HMDCCUTSTAT_H*/
