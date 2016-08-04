//*-- Author  : D. Gonzalez-Diaz
//*-- Created : 08/06/2006
//*-- Modified: 24/08/2007 by P.Cabanelas/D.Gonzalez-Diaz

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////
//
//  HRpcCal
//
//  Class for the Calibrated RPC data
//
////////////////////////////////////////////////////

#ifndef HRPCCAL_H
#define HRPCCAL_H

#include "hlocateddataobject.h"
#include "hrpcraw.h"
#include "TObject.h"

class HRpcCal : public HLocatedDataObject {
protected:
  Float_t rightTime;     //right leading time     [ns]
  Float_t leftTime;      //left leading time      [ns]
  Float_t rightTime2;    //2nd right leading time [ns]
  Float_t leftTime2;     //2nd left leading time  [ns]
  Float_t rightCharge;   //right charge           [pC]
  Float_t leftCharge;    //left  charge           [pC]
  Float_t rightCharge2;  //2nd right charge       [pC]
  Float_t leftCharge2;   //2nd left  charge       [pC]
  Short_t address;       //Geometrical address (sec, col, cell)
  Short_t logBit;        //Data Info
  			 //See: http://hades-wiki.gsi.de/cgi-bin/view/RPC/SoftwareDocumentation1 

public:
  HRpcCal();
  ~HRpcCal();

  // Functions getVariable  
  Float_t getRightTime()        { return rightTime;    }
  Float_t getLeftTime()         { return leftTime;     }
  Float_t getRightTime2()       { return rightTime2;   }
  Float_t getLeftTime2()        { return leftTime2;    }
  Float_t getRightCharge()      { return rightCharge;  }
  Float_t getLeftCharge()       { return leftCharge;   }
  Float_t getRightCharge2()     { return rightCharge2; }
  Float_t getLeftCharge2()      { return leftCharge2;  }
  Short_t getLogBit()           { return logBit;       }
  Short_t getAddress()          { return address;      }

  Int_t   getNLocationIndex()   { return 3;}

  inline Int_t getLocationIndex(Int_t i);

  Int_t   getSector()          const  { return (address>>10)  & 7;  }
  Int_t   getColumn()          const  { return (address>>7)   & 7;   }
  Int_t   getCell()            const  { return  address       & 127; }

  Int_t   getNLeadings()       { if(logBit<0)  return -1; return logBit>>12;         }
  Int_t   getNTrailings()      { if(logBit<0)  return -1; return (logBit>>8) & 15;   }
  Int_t   getLastChargeFlag()  { if(logBit<0)  return -1; return (logBit>>7) & 1;    }
  Int_t   getDataPairFlag()    { if(logBit<0)  return -1; return (logBit>>6) & 1;    }
  Int_t   getDataVersion()     { if(logBit<0)  return -1; return (logBit>>4) & 3;    }
  Int_t   getDataType()        { if(logBit<0)  return -1; return (logBit>>1) & 7;    }

  // Functions setVariable
  void    setRightTime(Float_t arightTime)             { rightTime=arightTime;       }
  void    setLeftTime(Float_t aleftTime)               { leftTime=aleftTime;         }
  void    setRightTime2(Float_t arightTime2)           { rightTime2=arightTime2;     }
  void    setLeftTime2(Float_t aleftTime2)             { leftTime2=aleftTime2;       }
  void    setRightCharge(Float_t arightCharge)         { rightCharge=arightCharge;   }
  void    setLeftCharge(Float_t aleftCharge)           { leftCharge=aleftCharge;     }
  void    setRightCharge2(Float_t arightCharge2)       { rightCharge2=arightCharge2; }
  void    setLeftCharge2(Float_t aleftCharge2)         { leftCharge2=aleftCharge2;   }
  void    setLogBit(Short_t alogBit)                   { logBit=alogBit;             }
  void    setAddress(Short_t aAddress)                 { address=aAddress;           }

  inline void    setAddress(Int_t sec, Int_t col, Int_t cell);

  
  ClassDef(HRpcCal,3) //RPC cal data class
};

inline void  HRpcCal::setAddress(Int_t sec, Int_t col, Int_t cell) { 
  address = (sec<<10) + (col<<7) + cell; }

inline Int_t HRpcCal::getLocationIndex(Int_t i) {
  switch (i) {
  case 0 : return getSector(); break;
  case 1 : return getColumn(); break;
  case 2 : return getCell(); break;
  }
  return -1;
}


#endif /* !HRPCCAL_H */
