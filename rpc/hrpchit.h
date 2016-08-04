#ifndef HRPCHIT_H
#define HRPCHIT_H

#include "TObject.h"

class HRpcHit : public TObject {
    protected:

	Float_t tof;             // Time of flight [ns]
	Float_t charge;          // Charge [pC]
	Float_t xmod;            // X coordinate in Module system [mm]
	Float_t ymod;            // Y coordinate in Module system [mm]
	Float_t zmod;            // Z coordinate in Module system [mm]
	Float_t xsec;            // X coordinate in Sector system [mm]
	Float_t ysec;            // Y coordinate in Sector system [mm]
	Float_t zsec;            // Z coordinate in Sector system [mm]
	Float_t xlab;            // X coordinate in Lab system [mm]
	Float_t ylab;            // Y coordinate in Lab system [mm]
	Float_t zlab;            // Z coordinate in Lab system [mm]
	Float_t theta;           // Theta angle [degrees]
	Float_t phi;             // Phi angle [degrees]

	Bool_t  isInCell;        // Flag for cell outliers
                                 // (false (zero) if xMod falls out of geometric bounds; true (one) otherwise)

	Float_t sigma_x;         // Sigma of x   [mm]
	Float_t sigma_y;         // Sigma of y   [mm]
	Float_t sigma_z;         // Sigma of z   [mm]
	Float_t sigma_tof;       // Sigma of tof [ps]

	Short_t address;         //Geometrical address (sec, col, cell)
	Short_t logBit;          //Data Info
	//See: http://hades-wiki.gsi.de/cgi-bin/view/RPC/SoftwareDocumentation1

	public:
	    HRpcHit(void);
	    ~HRpcHit(void);
	    void clear(void);

	    // Functions getVariable
	    Float_t getTof()             { return  tof;       }
	    Float_t getCharge()          { return  charge;    }
	    Float_t getXMod()            { return  xmod;      }
	    Float_t getYMod()            { return  ymod;      }
	    Float_t getZMod()            { return  zmod;      }
	    Float_t getXRMS()            { return  sigma_x;   }
	    Float_t getYRMS()            { return  sigma_y;   }
	    Float_t getZRMS()            { return  sigma_z;   }
	    Float_t getTOFRMS()          { return  sigma_tof; }
	    Float_t getXSec()            { return  xsec;      }
	    Float_t getYSec()            { return  ysec;      }
	    Float_t getZSec()            { return  zsec;      }
	    Float_t getTheta()           { return  theta;     }
	    Float_t getPhi()             { return  phi;       }
	    Bool_t  getInsideCellFlag()  { return  isInCell;  }

	    void getXYZLab(Float_t &x,Float_t &y,Float_t &z) {x=xlab;y=ylab;z=zlab;}

	    Int_t   getNLocationIndex()  { return 3;          }

	    inline Int_t   getLocationIndex(Int_t i);

	    Int_t   getSector()  const   { return (address>>10) & 7;  }
	    Int_t   getColumn()  const   { return (address>>7)  & 7;  }
	    Int_t   getCell()    const   { return  address      & 127; }


	    // Functions setVariable
	    inline void setAddress(Int_t sec,Int_t col,Int_t cell);
	    void setAddress(Short_t add)                         { address = add;   }
	    void setXSec   (Float_t axsec)                       { xsec    = axsec; }
	    void setYSec   (Float_t aysec)                       { ysec    = aysec; }
	    void setZSec   (Float_t azsec)                       { zsec    = azsec; }
	    void setTheta  (Float_t atheta)                      { theta   = atheta;}
	    void setPhi    (Float_t aphi)                        { phi     = aphi;  }
	    void setInsideCellFlag(Bool_t flag)                  { isInCell = flag; }
	    void setXYZLab(Float_t x,Float_t y,Float_t z)        { xlab = x; ylab = y; zlab = z; }
	    void setLogBit(Short_t lBit)                         { logBit  = lBit; }
	    inline  void setHit(Float_t atof, Float_t acharge, Float_t axmod, Float_t aymod, Float_t azmod);
	    inline  void setRMS(Float_t asigma_tof, Float_t asigma_x, Float_t asigma_y, Float_t asigma_z);

	    ClassDef(HRpcHit,3) //RPC hit data class
};


inline void HRpcHit::setHit(Float_t atof, Float_t acharge, Float_t axmod, Float_t aymod, Float_t azmod) {
    tof    = atof;
    charge = acharge;
    xmod   = axmod;
    ymod   = aymod;
    zmod   = azmod;
}

inline void HRpcHit::setRMS(Float_t asigma_tof, Float_t asigma_x, Float_t asigma_y, Float_t asigma_z) {
    sigma_tof = asigma_tof;
    sigma_x   = asigma_x;
    sigma_y   = asigma_y;
    sigma_z   = asigma_z;
}

inline void HRpcHit::setAddress(Int_t sec,Int_t col,Int_t cell)      {
    address = (sec<<10) + (col<<7) + cell;
}

inline Int_t HRpcHit::getLocationIndex(Int_t i) {
    switch (i) {
    case 0 : return getSector(); break;
    case 1 : return getColumn(); break;
    case 2 : return getCell(); break;
    }
    return -1;
}


#endif /* !HRPCHIT_H */
