#ifndef HRPCCLUSTER_H
#define HRPCCLUSTER_H

#include "TObject.h"

class HRpcCluster : public TObject {
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

	Float_t sigma_x;         // Sigma of x   [mm]
	Float_t sigma_y;         // Sigma of y   [mm]
	Float_t sigma_z;         // Sigma of z   [mm]
	Float_t sigma_tof;       // Sigma of tof [ps]

	Int_t   isInCell;        // Flag for cell outliers (xMod out of geometric cell bounds)
	                         // If cluster type == 2
	                         //   0 if both hits in the cluster are out of bounds
	                         //   1 if both are in
	                         //   2 if only first hit is in
	                         //   3 if only second hit is in
	                         // If cluster type ==1
	                         //   0 if out of bounds
	                         //   1 otherwise

	Short_t sector;          // Sector
	Short_t index;           // Linear index of "this" object in category

	Short_t detID1;          // Detector ID of the first  cell in the cluster
	Short_t detID2;          // Detector ID of the second cell in the cluster

	Short_t type;            // 1: Single hit, 2: Two-cell cluster

	public:
	    HRpcCluster(void);
	    ~HRpcCluster(void);

	    // Functions getVariable
	    Float_t getTof()             { return  tof;      }
	    Float_t getCharge()          { return  charge;   }
	    Float_t getXSec()            { return  xsec;     }
	    Float_t getYSec()            { return  ysec;     }
	    Float_t getZSec()            { return  zsec;     }
	    Float_t getXMod()            { return  xmod;     }
	    Float_t getYMod()            { return  ymod;     }
	    Float_t getZMod()            { return  zmod;     }
	    Float_t getTheta()           { return  theta;    }
	    Float_t getPhi()             { return  phi;      }
	    Float_t getXRMS()            { return  sigma_x;  }
	    Float_t getYRMS()            { return  sigma_y;  }
	    Float_t getZRMS()            { return  sigma_z;  }
	    Float_t getTOFRMS()          { return  sigma_tof;}
	    Short_t getSector()          { return  sector;   }
	    Short_t getIndex()           { return  index;    }
	    Int_t   getClusterType()     { return  Int_t(type);}
	    Int_t   getInsideCellFlag()  { return isInCell;  }

	    Int_t   getSector1()  const  { if(detID1<0) return -1; return (detID1>>9) & 7;  }
	    Int_t   getColumn1()  const  { if(detID1<0) return -1; return (detID1>>6) & 7;  }
	    Int_t   getCell1()    const  { if(detID1<0) return -1; return  detID1     & 63; }

	    Int_t   getSector2()  const  { if(detID2<0) return -1; return (detID2>>9) & 7;  }
	    Int_t   getColumn2()  const  { if(detID2<0) return -1; return (detID2>>6) & 7;  }
	    Int_t   getCell2()    const  { if(detID2<0) return -1; return  detID2     & 63; }

	    void    getXYZLab(Float_t &x,Float_t &y,Float_t &z) {x=xlab;y=ylab;z=zlab;}

	    // Functions setVariable

	    void         setClusterType   (Short_t atype)              { type = atype; }
	    void         setInsideCellFlag(Int_t flag)                 { isInCell = flag; }
	    inline void  setDetID1(Int_t sec,Int_t col,Int_t cell);
	    void         setDetID1(Short_t aDetID1)                    { detID1 = aDetID1;  }
	    inline void  setDetID2(Int_t sec,Int_t col,Int_t cell);
	    void         setDetID2(Short_t aDetID2)                    { detID2 = aDetID2;  }
	    inline  void setCluster(Float_t atof,Float_t acharge, Float_t axmod,Float_t aymod, Float_t azmod);
	    inline  void setRSEC   (Float_t axsec, Float_t aysec, Float_t azsec);
	    inline  void setRMS    (Float_t asigma_tof, Float_t asigma_x, Float_t asigma_y, Float_t asigma_z);
	    inline  void setAddress(Short_t asector, Short_t aindex);

	    void setTheta (Float_t atheta)                            { theta   = atheta;}
	    void setPhi   (Float_t aphi)                              { phi     = aphi;  }
	    void setXYZLab(Float_t x,Float_t y,Float_t z)             { xlab = x; ylab = y; zlab = z; }


	    ClassDef(HRpcCluster,1) //RPC cluster data class
};


inline void HRpcCluster::setCluster(Float_t atof,Float_t acharge, Float_t axmod,
				    Float_t aymod, Float_t azmod) {
    tof    = atof;
    charge = acharge;
    xmod   = axmod;
    ymod   = aymod;
    zmod   = azmod;
}

inline void HRpcCluster::setRSEC(Float_t axsec, Float_t aysec, Float_t azsec) {
    xsec   = axsec;
    ysec   = aysec;
    zsec   = azsec;
}

inline void HRpcCluster::setRMS(Float_t asigma_tof, Float_t asigma_x, Float_t asigma_y, Float_t asigma_z) {
    sigma_tof = asigma_tof;
    sigma_x   = asigma_x;
    sigma_y   = asigma_y;
    sigma_z   = asigma_z;
}

inline void HRpcCluster::setAddress(Short_t asector,Short_t aindex) {
    sector = asector;
    index  = aindex;
}

inline void HRpcCluster::setDetID1(Int_t sec,Int_t col,Int_t cell)      {
    if (sec<0 || col <0 || cell<0) detID1=-1;
    else detID1 = (sec<<9) + (col<<6) + cell;
}

inline void HRpcCluster::setDetID2(Int_t sec,Int_t col,Int_t cell)      {
    if (sec<0 || col <0 || cell<0) detID2=-1;
    else detID2 = (sec<<9) + (col<<6) + cell;
}

#endif /* !HRPCCLUSTER_H */
