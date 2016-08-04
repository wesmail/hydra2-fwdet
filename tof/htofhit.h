#ifndef HTOFHIT_H
#define HTOFHIT_H

#include "TObject.h"

class HTofHit : public TObject {
private:
  Int_t  index; //Index
  Float_t  tof; //time-of-flight (ns)
  Float_t  xpos; //X coordinate in the scint. rod reference system (mm)
  Float_t  xposAdc; // X coordinate calculated from ADC data (mm)
  Float_t  edep; // energy deposited in scint. rod by particle
  Float_t  lAmp; // amplitude of the signal registered by left PMT
  Float_t  rAmp; // amplitude of the signal registered by right PMT
  Float_t  xlab; //X in the lab system (mm)
  Float_t  ylab; //Y in the lab system (mm)
  Float_t  zlab; //Z in the lab system (mm)
  Float_t  dist; //distance of the hit from the target (mm)
  Float_t  theta; //theta angle of the hit (in deg)
  Float_t  phi;   //phi angle of the hit (in deg)
  Char_t  sector, module, cell; //Location
  Int_t  flagAdc;
public:
  HTofHit(void) : index(-1),tof(0.0F), xpos(0.0F) {}
  ~HTofHit(void) {}
  Int_t getIndex(void) {return  index;}
  Char_t getSector(void) {return  sector;}
  Char_t getModule(void) {return  module;}
  Char_t getCell(void) {return  cell;}
  Float_t getTof(void) {return tof;}
  Float_t getXpos(void) {return  xpos;}
  Float_t getXposAdc(void) {return xposAdc;}
  Float_t getEdep(void) {return edep;}
  Float_t getLeftAmp(void) {return lAmp;}
  Float_t getRightAmp(void) {return rAmp;}
  void getXYZLab(Float_t &x, Float_t &y, Float_t &z) { x = xlab; y = ylab; z = zlab; }
  void getDistance(Float_t &d) { d = dist; }
  void getTheta(Float_t &th) { th = theta; }
  void getPhi(Float_t &ph) { ph = phi; }
  Int_t getAdcFlag(void) {return flagAdc; }
  void setIndex(Int_t idx) { index=idx;}
  void setSector(Char_t s) { sector=s;}
  void setModule(Char_t m) { module=m;}
  void setCell(Char_t c) { cell=c;}
  void setTof(Float_t atof) {tof=atof;}
  void setXpos(Float_t axpos) {xpos=axpos;}
  void setXposAdc(Float_t axposAdc) {xposAdc=axposAdc;}
  void setEdep(Float_t aedep) {edep=aedep;}
  void setLeftAmp(Float_t alAmp) {lAmp=alAmp;}
  void setRightAmp(Float_t arAmp) {rAmp=arAmp;}
  void setXYZLab(Float_t x, Float_t y, Float_t z) { xlab = x; ylab = y; zlab = z; }
  void setDistance(Float_t d) { dist = d; }
  void setTheta(Float_t th) { theta = th; }
  void setPhi(Float_t ph) { phi = ph; }
  void setAdcFlag(Int_t af) {flagAdc = af; }
  void setValues(Float_t atof,Float_t axpos) {tof=atof;xpos=axpos;}
  Int_t operator< (const HTofHit &hit) { return tof<hit.tof && xpos<hit.xpos;}
  Int_t operator>(const HTofHit &hit) {return tof>hit.tof && xpos>hit.xpos;}
  Int_t operator!=(const HTofHit &hit) { return tof!=hit.tof || xpos!=hit.xpos;}
  HTofHit &operator=(const HTofHit &a) {
    tof=a.tof; xpos=a.xpos;
    return *this;
  }
  ClassDef(HTofHit,2) //Class with data for a TOF hit
};

#endif /* !HTOFHIT_H */

