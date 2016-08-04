//*-- AUTHORS :  Pablo Cabanelas / Hector Alvarez Pol
//*-- Created : 18/10/2005
//*-- Modified: 28/09/2006
//*-- Modified: 22/08/2007 Pablo Cabanelas /Diego Gonzalez-Diaz

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HRpcRaw
//
//  Class for the raw data of the RPC detector
//
/////////////////////////////////////////////////////////////

#ifndef HRPCRAW_H
#define HRPCRAW_H

#include "hlocateddataobject.h"

class HRpcRaw : public HLocatedDataObject {
 protected:
  Float_t  rightTime;       //TDC: 1st leading time on the right
  Float_t  leftTime;        //TDC: 1st leading time on the left
  Float_t  rightTime2;      //TDC: 2nd leading time on the right
  Float_t  leftTime2;       //TDC: 2nd leading time on the left
  Float_t  rightTot;        //TDC: 1st Time-over-Threshold on the right
  Float_t  leftTot;         //TDC: 1st Time-over-Threshold on the left
  Float_t  rightTot2;       //TDC: 2nd Time-over-Threshold on the right
  Float_t  leftTot2;        //TDC: 2nd Time-over-Threshold on the left
  Float_t  rightTotLast;    //TDC: Time-over-Threshold on the left including all the reflexions up to 500 ns
  Float_t  leftTotLast;     //TDC: Time-over-Threshold on the right including all the reflexions up to 500 ns

  Int_t    leftFeeAddress;  //FEE address (DBO_input, DBO, MBO, TRB): see wiki for documentation
  Int_t    rightFeeAddress; //FEE address (DBO_input, DBO, MBO, TRB): see wiki for documentation

  Short_t  address;         //Geometrical address (sec, col, cell)
  Short_t  leftLogBit;      //Data Info
  Short_t  rightLogBit;     //Data Info
                            //See: http://hades-wiki.gsi.de/cgi-bin/view/RPC/SoftwareDocumentation1 

 public:
  HRpcRaw();    
  ~HRpcRaw();

  // Functions getVariable
  Float_t getRightTime()              { return rightTime;       }
  Float_t getLeftTime()               { return leftTime;        }
  Float_t getRightTime2()             { return rightTime2;      }
  Float_t getLeftTime2()              { return leftTime2;       }
  Float_t getRightTot()               { return rightTot;        }
  Float_t getLeftTot()                { return leftTot;         }
  Float_t getRightTot2()              { return rightTot2;       }
  Float_t getLeftTot2()               { return leftTot2;        }
  Float_t getRightTotLast()           { return rightTotLast;    }
  Float_t getLeftTotLast()            { return leftTotLast;     }
  Short_t getAddress()                { return address;         }
  Int_t   getLeftFeeAddress()         { return leftFeeAddress;  }
  Int_t   getRightFeeAddress()        { return rightFeeAddress; }

  Short_t getRightLogBit()            { return rightLogBit; }
  Short_t getLeftLogBit()             { return leftLogBit; }

  Int_t   getNLocationIndex()         { return 3;}
  
  inline Int_t   getLocationIndex(Int_t i);

  Int_t   getSector()          const  { return (address>>10)  & 7;  } 
  Int_t   getColumn()          const  { return (address>>7)   & 7;  }  
  Int_t   getCell()            const  { return  address       & 127; }   
 
  Int_t   getRightSubEvtId()   const  { if(rightFeeAddress<0) return -1; return (rightFeeAddress>>17) & 1023; }
  Int_t   getRightTrbChannel() const  { if(rightFeeAddress<0) return -1; return (rightFeeAddress>>10) & 127;  }
  Int_t   getRightDBOInput()   const  { if(rightFeeAddress<0) return -1; return (rightFeeAddress>>8)  & 3;    }
  Int_t   getRightDBONum()     const  { if(rightFeeAddress<0) return -1; return (rightFeeAddress>>5)  & 7;    }
  Int_t   getRightMBONum()     const  { if(rightFeeAddress<0) return -1; return (rightFeeAddress>>3)  & 3;    }
  Int_t   getRightTRBNum()     const  { if(rightFeeAddress<0) return -1; return (rightFeeAddress)     & 7;    }
  
  Int_t   getLeftSubEvtId()    const  { if(leftFeeAddress<0) return -1;  return (leftFeeAddress>>17)  & 1023; }
  Int_t   getLeftTrbChannel()  const  { if(leftFeeAddress<0) return -1;  return (leftFeeAddress>>10)  & 127;  }
  Int_t   getLeftDBOInput()    const  { if(leftFeeAddress<0) return -1;  return (leftFeeAddress>>8)   & 3;    }
  Int_t   getLeftDBONum()      const  { if(leftFeeAddress<0) return -1;  return (leftFeeAddress>>5)   & 7;    }
  Int_t   getLeftMBONum()      const  { if(leftFeeAddress<0) return -1;  return (leftFeeAddress>>3)   & 3;    }
  Int_t   getLeftTRBNum()      const  { if(leftFeeAddress<0) return -1;  return (leftFeeAddress)      & 7;    }
  
  Int_t   getRightNLeadings()       const  { if(rightLogBit<0) return  0; return  rightLogBit>>12;      }
  Int_t   getRightNTrailings()      const  { if(rightLogBit<0) return  0; return (rightLogBit>>8) & 15; }
  Int_t   getRightLastTotFlag()     const  { if(rightLogBit<0) return -1; return (rightLogBit>>7) & 1;  }
  Int_t   getRightDataPairFlag()    const  { if(rightLogBit<0) return -1; return (rightLogBit>>6) & 1;  }
  Int_t   getRightDataVersion()     const  { if(rightLogBit<0) return -1; return (rightLogBit>>4) & 3;  }
  Int_t   getRightDataType()        const  { if(rightLogBit<0) return -1; return (rightLogBit>>1) & 7;  }

  Int_t   getLeftNLeadings()        const  { if(leftLogBit<0) return  0; return  leftLogBit>>12;      }
  Int_t   getLeftNTrailings()       const  { if(leftLogBit<0) return  0; return (leftLogBit>>8) & 15; }
  Int_t   getLeftLastTotFlag()      const  { if(leftLogBit<0) return -1; return (leftLogBit>>7) & 1;  }
  Int_t   getLeftDataPairFlag()     const  { if(leftLogBit<0) return -1; return (leftLogBit>>6) & 1;  }
  Int_t   getLeftDataVersion()      const  { if(leftLogBit<0) return -1; return (leftLogBit>>4) & 3;  }
  Int_t   getLeftDataType()         const  { if(leftLogBit<0) return -1; return (leftLogBit>>1) & 7;  }

 
  // Functions setVariable
  void    setRightTime(Float_t arightTime)         { rightTime=arightTime;        }
  void    setLeftTime(Float_t aleftTime)           { leftTime=aleftTime;          }
  void    setRightTime2(Float_t arightTime2)       { rightTime2=arightTime2;      }
  void    setLeftTime2(Float_t aleftTime2)         { leftTime2=aleftTime2;        }
  void    setRightTot(Float_t arightTot)           { rightTot=arightTot;          }
  void    setLeftTot(Float_t aleftTot)             { leftTot=aleftTot;            }
  void    setRightTot2(Float_t arightTot2)         { rightTot2=arightTot2;        }
  void    setLeftTot2(Float_t aleftTot2)           { leftTot2=aleftTot2;          }
  void    setRightTotLast(Float_t arightTotLast)   { rightTotLast=arightTotLast;  }
  void    setLeftTotLast(Float_t aleftTotLast)     { leftTotLast=aleftTotLast;    }

  inline void    setAddress(Int_t sec,Int_t col,Int_t cell);
  inline void    setRightFeeAddress(Int_t sevtid,Int_t chan,Int_t dbi,Int_t dbn,Int_t mbn,Int_t trbn);
  inline void    setLeftFeeAddress(Int_t sevtid,Int_t chan,Int_t dbi,Int_t dbn,Int_t mbn,Int_t trbn);
  inline void    setRightLogBit(Int_t lm,Int_t tm,Int_t ltf,Int_t pf,Int_t ref,Int_t type,Int_t ex);
  inline void    setLeftLogBit(Int_t lm,Int_t tm,Int_t ltf,Int_t pf,Int_t ref,Int_t type,Int_t ex);
  


  ClassDef(HRpcRaw,2) // Raw data class of RPC detector
};

inline void HRpcRaw::setAddress(Int_t sec,Int_t col,Int_t cell) { 
  address = (sec<<10) + (col<<7) + cell; }

inline void HRpcRaw::setRightFeeAddress(Int_t sevtid,Int_t chan,Int_t dbi,Int_t dbn,Int_t mbn,Int_t trbn) {
  if(sevtid<0 || chan<0 || dbi<0 || dbn<0 || mbn<0 || trbn<0) rightFeeAddress=-1;
  else rightFeeAddress = (sevtid<<17) + (chan<<10) + (dbi<<8) + (dbn<<5) + (mbn<<3) + trbn; 
}

inline void HRpcRaw::setLeftFeeAddress(Int_t sevtid,Int_t chan,Int_t dbi,Int_t dbn,Int_t mbn,Int_t trbn) {
  if(sevtid<0 || chan<0 || dbi<0 || dbn<0 || mbn<0 || trbn<0) leftFeeAddress=-1;
  else leftFeeAddress  = (sevtid<<17) + (chan<<10) + (dbi<<8) + (dbn<<5) + (mbn<<3) + trbn; 
}

inline void HRpcRaw::setRightLogBit(Int_t lm,Int_t tm,Int_t ltf,Int_t pf,Int_t ref,Int_t type,Int_t ex) {
  if(lm<0 || tm<0 || ltf<0 || pf<0 || ref<0 || type<0 || ex<0) rightLogBit=-1;
  else rightLogBit = (lm<<12) + (tm<<8) + (ltf<<7) + (pf<<6) + (ref<<4) + (type<<1) + ex;
}

inline void HRpcRaw::setLeftLogBit(Int_t lm,Int_t tm,Int_t ltf,Int_t pf,Int_t ref,Int_t type,Int_t ex) {
  if(lm<0 || tm<0 || ltf<0 || pf<0 || ref<0 || type<0 || ex<0) leftLogBit=-1;
  else leftLogBit = (lm<<12) + (tm<<8) + (ltf<<7) + (pf<<6) + (ref<<4) + (type<<1) + ex;
}

inline Int_t HRpcRaw::getLocationIndex(Int_t i) { 
  switch (i) {
  case 0 : return getSector(); break;
  case 1 : return getColumn(); break;
  case 2 : return getCell(); break;
  }
  return -1;
}


#endif /* !HRPCRAW_H */
