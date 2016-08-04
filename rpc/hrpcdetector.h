#ifndef HRPCDETECTOR_H
#define HRPCDETECTOR_H

#include "hdetector.h"
#include "haddef.h"
#include "TArrayI.h"

class HRpcDetector : public HDetector {

 private:
  Int_t maxColumns;
  Int_t maxCells;
  Int_t maxEmptyChannels;

 public:
  HRpcDetector(void);
  ~HRpcDetector(void);
  HCategory *buildCategory(Cat_t cat);
  HCategory *buildMatrixCategory(const Text_t *,Float_t );
  HCategory *buildLinearCategory(const Text_t *,Float_t );
  Bool_t init(void) {return kTRUE;}
  void activateParIo(HParIo* io);
  Bool_t write(HParIo* io);

  Int_t getMaxColumns()    {return maxColumns;}
  Int_t getMaxCells()      {return maxCells;}
  Int_t getEmptyChannels() {return maxEmptyChannels;}

  void setMaxColumns(Int_t amaxColumns)            {maxColumns       = amaxColumns; }
  void setMaxCells(Int_t amaxCells)                {maxCells         = amaxCells;   }
  void setMaxEmptyChannels(Int_t amaxChannels)     {maxEmptyChannels = amaxChannels;}

  ClassDef(HRpcDetector,2) // RPC detector class
};


#endif /* !HRPCDETECTOR_H */

    
