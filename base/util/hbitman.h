#ifndef __HBITMAN_H__
#define __HBITMAN_H__

#include "TObject.h"
#include "TString.h"

#include <vector>
#include <map>

class HBitMan : public TObject {

protected:

    typedef struct {
	TString name;                        // lable of the word
	Int_t offset;                        // offset in bits in word
	Int_t width ;                        // width in bits
	Bool_t sign ;                        // kTRUE if highest bit in word is sign bit
    } word;

    std::vector < word > list;               //!
    std::map < TString, word> wordmap;       //!


public:


    HBitMan();
    ~HBitMan();


    //-----------------------------------------------------------
    // direct bit setting
    static void   printBits(UInt_t data);
    static void   setBit   (UInt_t &data,Int_t bit);
    static void   unsetBit (UInt_t &data,Int_t bit);
    static Bool_t getBit   (UInt_t &data,Int_t bit);

    static void   setWord  (UInt_t &data,Int_t num,Int_t width, Int_t val,Bool_t sign=kFALSE);
    static void   unsetWord(UInt_t &data,Int_t num,Int_t width);
    Int_t         getWord  (UInt_t &data,Int_t num,Int_t width,Bool_t sign=kFALSE);
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    // bit setting using names
    void          createWord(const TString name,Int_t width, Bool_t sign=kFALSE);
    void          init();

    void          setWord  (UInt_t &data,const TString name,Int_t val);
    void          unsetWord(UInt_t &data,const TString name);
    Int_t         getWord  (UInt_t &data,const TString name);
    //-----------------------------------------------------------

    ClassDef(HBitMan,0)

};


#endif //__HBITMAN_H__
