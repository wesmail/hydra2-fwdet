#ifndef HRPCGEOMCELLPAR_H
#define HRPCGEOMCELLPAR_H
#include "TArrayF.h"
#include "hparcond.h"

class HParamList;

class HRpcGeomCellPar : public HParCond {
protected:
    TArrayF fXcell;     // X position at module (left-down cell corner) [mm] 
    TArrayF fYcell;     // Y position at module (left-down cell corner) [mm]
    TArrayF fLcell;     // Cell lenght (down edge) [mm] 
    TArrayF fWcell;     // Cell width [mm]
    Float_t fDPlanes;   // Efective distance between layers ( center of the active regions of the two layers) [mm]
    Float_t fDeltaZ;    // Thickness of the cell active region (between inner and outer gaps) [mm]
public:
    HRpcGeomCellPar(const Char_t* name ="RpcGeomCellPar",
	      const Char_t* title  ="Geometry parameters of cells for the Rpc Detector",
	      const Char_t* context="RpcGeomCellProduction");
    ~HRpcGeomCellPar();

    Float_t getX(Int_t col,Int_t cell)                  {return (cell<0||cell>31||col<0||col>5)? -10000 :fXcell[32*col+cell];}
    void    setX(Int_t col,Int_t cell,Float_t x)        {fXcell[32*col+cell]=x;}

    Float_t getY(Int_t col,Int_t cell)                  {return (cell<0||cell>31||col<0||col>5)? -10000 :fYcell[32*col+cell];}
    void    setY(Int_t col,Int_t cell,Float_t y)        {fYcell[32*col+cell]=y;}

    Float_t getLength(Int_t col,Int_t cell)             {return (cell<0||cell>31||col<0||col>5)? -10000 :fLcell[32*col+cell];}
    void    setLength(Int_t col,Int_t cell,Float_t len) {fLcell[32*col+cell]=len;}

    Float_t getWidth(Int_t col,Int_t cell)              {return (cell<0||cell>31||col<0||col>5)? -10000 :fWcell[32*col+cell];}
    void    setWidth(Int_t col,Int_t cell,Float_t wid)  {fWcell[32*col+cell]=wid;}

    Float_t getDPlanes()                                {return fDPlanes;}
    void    setDPlanes(Float_t dp)                      {fDPlanes=dp;}

    Float_t getDeltaZ()                                 {return fDeltaZ;}
    void    setDeltaZ(Float_t dz)                       {fDeltaZ=dz;}

    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();

    ClassDef(HRpcGeomCellPar,1) // Container for the RPC cells geometry parameters
};
#endif  /*!HRPCGEOMCELLPAR_H*/
