#ifndef HMDCWIRESTAT_H
#define HMDCWIRESTAT_H
#include "TArrayF.h"
#include "TArrayI.h"
#include "TCanvas.h"
#include "hparcond.h"

class HParamList;


class HMdcWireStat : public HParCond {
protected:

    TArrayI mb        ; // mapping sector,module,layer,wire to mother board  [6*4*6*220]
    TArrayI tdc       ; // mapping sector,module,layer,wire to tdc           [6*4*6*220]
    TArrayI statusmap ; // status of the wire: 1=working,-1=not connected, -3=no correct offset, -7=dead, -10=init [6*6*4*220]
    TArrayF efficiency; // efficiency of the wire: 0-1  [6*4*6*220]
    TArrayF offset    ; // offset of the wire in ns (for simulation)  [6*4*6*220]


    Int_t index(Int_t s,Int_t m,Int_t l,Int_t c){ return s*5280 + m*1320 + l*220 + c;} // 4*6*220=5280, 6*220=1320

public:
    HMdcWireStat(const Char_t* name   ="MdcWireStat",
		 const Char_t* title  ="Mdc lookup table for wire stat",
		 const Char_t* context="MdcWireStatProduction");
    ~HMdcWireStat();

    //------------------------------------------------------
    // set + get

    void     setMbo       (Int_t sec,Int_t mod,Int_t lay,Int_t cell,Int_t mbo)  {mb        .SetAt(mbo ,index(sec,mod,lay,cell));}
    void     setTdc       (Int_t sec,Int_t mod,Int_t lay,Int_t cell,Int_t t)    {tdc       .SetAt(t   ,index(sec,mod,lay,cell));}
    void     setStatus    (Int_t sec,Int_t mod,Int_t lay,Int_t cell,Int_t stat) {statusmap .SetAt(stat,index(sec,mod,lay,cell));}
    void     setEfficiency(Int_t sec,Int_t mod,Int_t lay,Int_t cell,Float_t eff){efficiency.SetAt(eff ,index(sec,mod,lay,cell));}
    void     setOffset    (Int_t sec,Int_t mod,Int_t lay,Int_t cell,Float_t off){offset    .SetAt(off ,index(sec,mod,lay,cell));}

    Int_t    getMbo       (Int_t sec,Int_t mod,Int_t lay,Int_t cell)            {return mb        [index(sec,mod,lay,cell)];}
    Int_t    getTdc       (Int_t sec,Int_t mod,Int_t lay,Int_t cell)            {return tdc       [index(sec,mod,lay,cell)];}
    Int_t    getStatus    (Int_t sec,Int_t mod,Int_t lay,Int_t cell)            {return statusmap [index(sec,mod,lay,cell)];}
    Float_t  getEfficiency(Int_t sec,Int_t mod,Int_t lay,Int_t cell)            {return efficiency[index(sec,mod,lay,cell)];}
    Float_t  getOffset    (Int_t sec,Int_t mod,Int_t lay,Int_t cell)            {return offset    [index(sec,mod,lay,cell)];}

    //------------------------------------------------------
    // plot functions
    Int_t    getDeadWires();
    Int_t    getDeadWires(Int_t,Int_t);
    TCanvas* plotStatusMbVsTdc();
    TCanvas* plotEfficiencyMbVsTdc();
    TCanvas* plotOffsetMbVsTdc();
    TCanvas* plotStatus    (Int_t,Int_t);
    TCanvas* plotEfficiency(Int_t,Int_t);
    TCanvas* plotOffset    (Int_t,Int_t);
    //------------------------------------------------------
    // standard functions
    void     printStats();
    void     printStatsSum();
    void     putParams(HParamList*);
    Bool_t   getParams(HParamList*);
    void     clear();
    void     printParam(void);
    ClassDef(HMdcWireStat,1) //  MDC lookup table (wire stat, noise)
};
#endif  /*!HMDCWIRESTAT_H*/
