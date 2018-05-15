#ifndef HEMCDETECTOR_H
#define HEMCDETECTOR_H

#include "hdetector.h"

class HEmcDetector : public HDetector {

private:
  Int_t numCells;     // number of cells (without spares)

public:

  static const Int_t cellMap[163];

  HEmcDetector(void);
  virtual ~HEmcDetector(void);

  HCategory *buildCategory(Cat_t cat);
  HCategory *buildMatrixCategory(const Text_t *,Float_t );
  HCategory *buildLinearCategory(const Text_t *,Float_t );

  void activateParIo(HParIo* io);
  Bool_t write(HParIo* io);

  Int_t getNumCells()   const {return numCells;}
  Int_t getMaxSecInSetup(void);

  static Int_t getCell(const Char_t row, const Char_t col);
  static void  getRowCol(const Int_t cell, Char_t& row, Char_t& col); 

  /** evaluate cell id in database from mounting position (number) of the ecal modules*/
  static Int_t getCellFromPosition(Int_t pos);

  /** evaluate mounting position (number) of the ecal modules from cell id in the database lookup table*/
  static Int_t getPositionFromCell(Int_t system);


  ClassDef(HEmcDetector,0) // EMC detector class
};

#endif /* !HEMCDETECTOR_H */
