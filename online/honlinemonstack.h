#ifndef HMONSTACK_H
#define HMONSTACK_H
using namespace std;
#include "honlinemonhist.h"
#include "TNamed.h"
#include "TString.h"
#include "TObjArray.h"
#include "TH1.h"
#include "TH2.h"
#include <iostream> 
#include <iomanip>
#include <map>




/** Stack class of hists dreived from HOnlineMonHistAddon
* supports TLegend,TGraph.
*/
class HOnlineMonStack: public HOnlineMonHistAddon {
protected:
    TObjArray* stack;
    TString firstName;
    map <TString,TString> options;
public:
    HOnlineMonStack(const Char_t* name   ="", const Char_t* title  ="");

    HOnlineMonStack(HOnlineMonStack&);
    virtual ~HOnlineMonStack();
    /// get the pointer to the internal histogram (parameters are not used yet. call getP() )
    TObjArray*    getStack()  { return stack; }
    /// reset the internal histogram
    void    reset(Int_t level,Int_t count);
    /// draw the internal histogram
    void    draw(Bool_t allowHiddenZero=1);
    /// add the HOnlineMonHist h2 to the own hist
    void    add(HOnlineMonHistAddon* h2);
    void    addToStack(TObject* obj,TString option="");
    void    addTimeStamp(TString timestamp);
    virtual void getMinMax(Double_t& min,Double_t& max);
    ClassDef(HOnlineMonStack,1) // mon hist
};

#endif  /*!HMONSTACK_H*/
