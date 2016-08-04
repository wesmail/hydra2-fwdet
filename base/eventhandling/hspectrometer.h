#ifndef HSPECTROMETER_H
#define HSPECTROMETER_H

#include "TNamed.h"
#include "TList.h"

class HDetector;
class HParIo;
class HGeomShapes;

class HSpectrometer : public TNamed {
protected:
  TList* detectors;  // List of detectors
  Bool_t changed;    //! flag is kTRUE if the setup changes
  HGeomShapes* shapes;  //! shape class containing the different shapes
public:
  HSpectrometer();
  ~HSpectrometer();
  void addDetector(HDetector* det);
  HDetector* getDetector(const Char_t* name);
  Bool_t init(void);
  void activateParIo(HParIo* io);
  Bool_t write();
  Bool_t write(HParIo* io);
  Bool_t hasChanged() {return changed;}
  void setChanged(Bool_t f=kTRUE) {changed=f;}
  TList* getListOfDetectors() {return detectors;}
  HGeomShapes* getShapes();
  void print();
  ClassDef(HSpectrometer,1) // Class for the spectrometer configuration
};

#endif  /* !HSPECTROMETER_H */

