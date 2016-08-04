#ifndef HWALLEVENTPLANEPAR_H
#define HWALLEVENTPLANEPAR_H

#include "hparcond.h"


class HWallEventPlanePar : public HParCond
{
protected:
    Float_t fT1_cut; // time1 cut to select spectators
    Float_t fT2_cut; // time2 cut to select spectators
    Float_t fX_shift; // shift from center in x direction
    Float_t fY_shift; // shift from center in y direction
    Float_t fR0_cut;  // cuts out internal zone, which might be affected by the beam spot
    Float_t fZ1_cut_s; // common cut for small  cells: cuts out noise below Z=1 particles on calibrated dE/dx spectrum
    Float_t fZ1_cut_m; // common cut for middle cells: cuts out noise below Z=1 particles on calibrated dE/dx spectrum
    Float_t fZ1_cut_l; // common cut for large  cells: cuts out noise below Z=1 particles on calibrated dE/dx spectrum
    
public:
    HWallEventPlanePar(const Char_t* name   = "WallEventPlanePar",
		       const Char_t* title  = "EventPlane parameters for Forward Wall",
		       const Char_t* context= "WallEventPlaneProduction");
    ~HWallEventPlanePar() {;}

    Float_t getT1Cut(void){  return fT1_cut; }
    Float_t getT2Cut(void){  return fT2_cut; }
    Float_t getXShift(void){  return fX_shift; }
    Float_t getYShift(void){  return fY_shift; }
    Float_t getR0Cut(void){   return fR0_cut;  }
    Float_t getZ1_cut_s(void){  return fZ1_cut_s;}
    Float_t getZ1_cut_m(void){  return fZ1_cut_m;}
    Float_t getZ1_cut_l(void){  return fZ1_cut_l;}

    void    setT1Cut( Float_t t1){ fT1_cut = t1;}
    void    setT2Cut( Float_t t2){ fT2_cut = t2;}
    void    setXShift( Float_t x){ fX_shift = x;}
    void    setYShift( Float_t y){ fY_shift = y;}
    void    setR0Cut( Float_t r){  fR0_cut = r; }
    void    setZ1_cut_s(Float_t cs){ fZ1_cut_s = cs; }
    void    setZ1_cut_m(Float_t cm){ fZ1_cut_m = cm; }
    void    setZ1_cut_l(Float_t cl){ fZ1_cut_l = cl; }

    void    clear();

    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    ClassDef(HWallEventPlanePar,1) // Container for the WALL EventPlane parameters
};

#endif  /*!HWALLEVENTPLANEPAR_H*/
