#ifndef HSEQARRI_H
#define HSEQARRI_H

#include "TObject.h"

class HSeqArrBlock : public TObject {
  protected:
    enum {blockSize=0x40000,      // size of one block of array in bytes
          nBits=18,               // index>>nBits - is number of array block
          bitMask=0x3FFFF};       // index&BITSMASK - is element index in block
    Char_t        arr[blockSize];
    UInt_t        blockNumber;    // array block number
    HSeqArrBlock* nextBlock;      // pointer to the next array block

  public:  // All functions in this class are NOT for user!!!
    HSeqArrBlock(void);
    HSeqArrBlock(HSeqArrBlock* p);
    virtual       ~HSeqArrBlock(void);
    Bool_t        expand(void);
    static UInt_t blockIndex(UInt_t n)   {return n>>nBits;}
    static UInt_t indexInBlock(UInt_t i) {return i&bitMask;}
    Char_t*       getArr(void)           {return arr;}
    UInt_t        getBlockNumber(void)   {return blockNumber;}
    HSeqArrBlock* getNextBlock(void)     {return nextBlock;}
    
  ClassDef(HSeqArrBlock,0)
};


class HSeqArrIter;

enum ESeqArrType {kNoType,
                  kChar,kUChar,kShort,kUShort,kInt,kUInt,kLong64,kULong64,
                  kFloat,kDouble,
                  kBool};

class HSeqArr : public TObject {
  protected:
    ESeqArrType   type;        // array type
    HSeqArrBlock* firstBlock;  // pinter to the first block of array
    HSeqArrBlock* lastBlock;   // pinter to the last block of array
    UInt_t        wordLength;  // length of word in array
    
    UInt_t        size;        // total size of array in bytes
    UInt_t        nBlocks;     // number of blocks in array
    UInt_t        blockLength; // size of HSeqArrBlock object in bytes
    
    HSeqArrBlock* block;       // pointer to the current HSeqArrBlock object
    UInt_t        index;       // current index of element in full array (0-size)
    Char_t*       cElem;       // pointer to the current element
    Bool_t        isFrst;      // ???

  protected:
    void   init(ESeqArrType tp);
    Bool_t setCurrBlock(UInt_t n);
    Bool_t err(ESeqArrType tp);
    Bool_t setNotFirst(void)        {isFrst=kFALSE; return kTRUE;}
    Bool_t next(ESeqArrType tp);
    Bool_t nextAExp(ESeqArrType tp);
    
    Bool_t set(Char_t    &e) {*(Char_t*   )cElem=e; return setNotFirst();}   
    Bool_t set(UChar_t   &e) {*(UChar_t*  )cElem=e; return setNotFirst();}
    Bool_t set(Short_t   &e) {*(Short_t*  )cElem=e; return setNotFirst();}
    Bool_t set(UShort_t  &e) {*(UShort_t* )cElem=e; return setNotFirst();} 
    Bool_t set(Int_t     &e) {*(Int_t*    )cElem=e; return setNotFirst();}    
    Bool_t set(UInt_t    &e) {*(UInt_t*   )cElem=e; return setNotFirst();}   
    Bool_t set(Long64_t  &e) {*(Long64_t* )cElem=e; return setNotFirst();} 
    Bool_t set(ULong64_t &e) {*(ULong64_t*)cElem=e; return setNotFirst();}
    Bool_t set(Float_t   &e) {*(Float_t*  )cElem=e; return setNotFirst();}  
    Bool_t set(Double_t  &e) {*(Double_t* )cElem=e; return setNotFirst();} 
    Bool_t set(Bool_t    &e) {*(Bool_t*   )cElem=e; return setNotFirst();}
    
    Bool_t get(Char_t    &e) {e=*(Char_t*   )cElem; return setNotFirst();}   
    Bool_t get(UChar_t   &e) {e=*(UChar_t*  )cElem; return setNotFirst();}
    Bool_t get(Short_t   &e) {e=*(Short_t*  )cElem; return setNotFirst();}
    Bool_t get(UShort_t  &e) {e=*(UShort_t* )cElem; return setNotFirst();} 
    Bool_t get(Int_t     &e) {e=*(Int_t*    )cElem; return setNotFirst();}    
    Bool_t get(UInt_t    &e) {e=*(UInt_t*   )cElem; return setNotFirst();}   
    Bool_t get(Long64_t  &e) {e=*(Long64_t* )cElem; return setNotFirst();} 
    Bool_t get(ULong64_t &e) {e=*(ULong64_t*)cElem; return setNotFirst();}
    Bool_t get(Float_t   &e) {e=*(Float_t*  )cElem; return setNotFirst();}  
    Bool_t get(Double_t  &e) {e=*(Double_t* )cElem; return setNotFirst();} 
    Bool_t get(Bool_t    &e) {e=*(Bool_t*   )cElem; return setNotFirst();}
    
  public:
    HSeqArr(void)                            {init(kNoType);}
    HSeqArr(ESeqArrType tp)                  {init(tp);}
    HSeqArr(Char_t)                          {init(kChar);}
    ~HSeqArr(void);
    
    Bool_t        setArrType(ESeqArrType tp);
    UInt_t        getArraySize(void) const   {return size/wordLength;}
    UInt_t        getMemSize(void) const;
    HSeqArrIter*  makeIterator(void);
    Bool_t        isBoundOk(UInt_t i) const  {return (i<size) ? kTRUE:kFALSE;}

    void          rewind(void);
    
    Bool_t        addElement(void);   // adding new element and setting cursor
    Bool_t        setCursor(Int_t indArr=-1);
    
    void          setElement(Char_t    e)    {if(tpOk(kChar   )) set(e);}
    void          setElement(UChar_t   e)    {if(tpOk(kUChar  )) set(e);}
    void          setElement(Short_t   e)    {if(tpOk(kShort  )) set(e);}
    void          setElement(UShort_t  e)    {if(tpOk(kUShort )) set(e);}
    void          setElement(Int_t     e)    {if(tpOk(kInt    )) set(e);}
    void          setElement(UInt_t    e)    {if(tpOk(kUInt   )) set(e);}
    void          setElement(Long64_t  e)    {if(tpOk(kLong64 )) set(e);}
    void          setElement(ULong64_t e)    {if(tpOk(kULong64)) set(e);}
    void          setElement(Float_t   e)    {if(tpOk(kFloat  )) set(e);}   
    void          setElement(Double_t  e)    {if(tpOk(kDouble )) set(e);}
    void          setElement(Bool_t    e)    {if(tpOk(kBool   )) set(e);}
    
    void          getElement(Char_t&    e)   {if(tpOk(kChar   )) get(e);}
    void          getElement(UChar_t&   e)   {if(tpOk(kUChar  )) get(e);}
    void          getElement(Short_t&   e)   {if(tpOk(kShort  )) get(e);}
    void          getElement(UShort_t&  e)   {if(tpOk(kUShort )) get(e);}
    void          getElement(Int_t&     e)   {if(tpOk(kInt    )) get(e);}
    void          getElement(UInt_t&    e)   {if(tpOk(kUInt   )) get(e);}
    void          getElement(Long64_t&  e)   {if(tpOk(kLong64 )) get(e);}
    void          getElement(ULong64_t& e)   {if(tpOk(kULong64)) get(e);}
    void          getElement(Float_t&   e)   {if(tpOk(kFloat  )) get(e);}   
    void          getElement(Double_t&  e)   {if(tpOk(kDouble )) get(e);}
    void          getElement(Bool_t&    e)   {if(tpOk(kBool   )) get(e);}
    
    Bool_t addNext(Char_t    e) {if(nextAExp(kChar   )) return set(e); return kFALSE;}
    Bool_t addNext(UChar_t   e) {if(nextAExp(kUChar  )) return set(e); return kFALSE;}
    Bool_t addNext(Short_t   e) {if(nextAExp(kShort  )) return set(e); return kFALSE;}
    Bool_t addNext(UShort_t  e) {if(nextAExp(kUShort )) return set(e); return kFALSE;}
    Bool_t addNext(Int_t     e) {if(nextAExp(kInt    )) return set(e); return kFALSE;}
    Bool_t addNext(UInt_t    e) {if(nextAExp(kUInt   )) return set(e); return kFALSE;}
    Bool_t addNext(Long64_t  e) {if(nextAExp(kLong64 )) return set(e); return kFALSE;}
    Bool_t addNext(ULong64_t e) {if(nextAExp(kULong64)) return set(e); return kFALSE;}
    Bool_t addNext(Float_t   e) {if(nextAExp(kFloat  )) return set(e); return kFALSE;}
    Bool_t addNext(Double_t  e) {if(nextAExp(kDouble )) return set(e); return kFALSE;}
    Bool_t addNext(Bool_t    e) {if(nextAExp(kBool   )) return set(e); return kFALSE;}
    
    Bool_t getNext(Char_t&    e) {if(next(kChar   )) return get(e); return kFALSE;}
    Bool_t getNext(UChar_t&   e) {if(next(kUChar  )) return get(e); return kFALSE;}
    Bool_t getNext(Short_t&   e) {if(next(kShort  )) return get(e); return kFALSE;}
    Bool_t getNext(UShort_t&  e) {if(next(kUShort )) return get(e); return kFALSE;}
    Bool_t getNext(Int_t&     e) {if(next(kInt    )) return get(e); return kFALSE;}
    Bool_t getNext(UInt_t&    e) {if(next(kUInt   )) return get(e); return kFALSE;}
    Bool_t getNext(Long64_t&  e) {if(next(kLong64 )) return get(e); return kFALSE;}
    Bool_t getNext(ULong64_t& e) {if(next(kULong64)) return get(e); return kFALSE;}
    Bool_t getNext(Float_t&   e) {if(next(kFloat  )) return get(e); return kFALSE;}
    Bool_t getNext(Double_t&  e) {if(next(kDouble )) return get(e); return kFALSE;}
    Bool_t getNext(Bool_t&    e) {if(next(kBool   )) return get(e); return kFALSE;}
    
    // Next functions in this class are NOT for user!!!
    UInt_t        getWordLength(void) const  {return wordLength;}
    HSeqArrBlock* getFirstBlock(void)        {return firstBlock;}
    Bool_t        tpOk(ESeqArrType tp)       {if(type==tp) return kTRUE;
                                              return err(tp);}
    Bool_t        addBlock(void);
    UInt_t        getArrSize(void) const     {return size;}
    void          increaseArrSize(void)      {size += wordLength;}
    
  ClassDef(HSeqArr,0)
};

class HSeqArrIter : public TObject {
  protected:
    HSeqArr*      array;
    HSeqArrBlock* block;      // pointer to the current HSeqArrBlock object
    UInt_t        index;      // current index of element in full array (0-size)
    Char_t*       cElem;      // pointer to the current element
    Bool_t        isFrst;     // ???

  protected:
    Bool_t next(ESeqArrType tp);
    Bool_t nextAExp(ESeqArrType tp);
    Bool_t setNotFirst(void) {isFrst=kFALSE; return kTRUE;}
 
  public:
    HSeqArrIter(void) {}          // NOT for user!!!
    HSeqArrIter(HSeqArr* arr);    // NOT for user!!!
    ~HSeqArrIter(void) {}
    void   reset(void);

    Bool_t next(void);
    Bool_t nextAndExpand(void);
    Bool_t setCursor(Int_t indArr=-1);
    UInt_t getCursor(void) const {return index/array->getWordLength();}
    
    Bool_t set(Char_t    e)   {*(Char_t*   )cElem=e; return setNotFirst();} // Set element in cursor position
    Bool_t set(UChar_t   e)   {*(UChar_t*  )cElem=e; return setNotFirst();} // ...
    Bool_t set(Short_t   e)   {*(Short_t*  )cElem=e; return setNotFirst();} // ...
    Bool_t set(UShort_t  e)   {*(UShort_t* )cElem=e; return setNotFirst();} // ...
    Bool_t set(Int_t     e)   {*(Int_t*    )cElem=e; return setNotFirst();} // ...
    Bool_t set(UInt_t    e)   {*(UInt_t*   )cElem=e; return setNotFirst();} // ...
    Bool_t set(Long64_t  e)   {*(Long64_t* )cElem=e; return setNotFirst();} // ...
    Bool_t set(ULong64_t e)   {*(ULong64_t*)cElem=e; return setNotFirst();} // ...
    Bool_t set(Float_t   e)   {*(Float_t*  )cElem=e; return setNotFirst();} // ...
    Bool_t set(Double_t  e)   {*(Double_t* )cElem=e; return setNotFirst();} // ...
    Bool_t set(Bool_t    e)   {*(Bool_t*   )cElem=e; return setNotFirst();} // ...
    
    Bool_t get(Char_t    &e)  {e=*(Char_t*   )cElem; return setNotFirst();} // Get element from cursor position
    Bool_t get(UChar_t   &e)  {e=*(UChar_t*  )cElem; return setNotFirst();} // ...
    Bool_t get(Short_t   &e)  {e=*(Short_t*  )cElem; return setNotFirst();} // ...
    Bool_t get(UShort_t  &e)  {e=*(UShort_t* )cElem; return setNotFirst();} // ...
    Bool_t get(Int_t     &e)  {e=*(Int_t*    )cElem; return setNotFirst();} // ...
    Bool_t get(UInt_t    &e)  {e=*(UInt_t*   )cElem; return setNotFirst();} // ...
    Bool_t get(Long64_t  &e)  {e=*(Long64_t* )cElem; return setNotFirst();} // ...
    Bool_t get(ULong64_t &e)  {e=*(ULong64_t*)cElem; return setNotFirst();} // ...
    Bool_t get(Float_t   &e)  {e=*(Float_t*  )cElem; return setNotFirst();} // ...
    Bool_t get(Double_t  &e)  {e=*(Double_t* )cElem; return setNotFirst();} // ...
    Bool_t get(Bool_t    &e)  {e=*(Bool_t*   )cElem; return setNotFirst();} // ...
    
    Bool_t get(Char_t    **e) {*e=(Char_t*   )cElem; return setNotFirst();} // Get element from cursor position
    Bool_t get(UChar_t   **e) {*e=(UChar_t*  )cElem; return setNotFirst();} // ...
    Bool_t get(Short_t   **e) {*e=(Short_t*  )cElem; return setNotFirst();} // ...
    Bool_t get(UShort_t  **e) {*e=(UShort_t* )cElem; return setNotFirst();} // ...
    Bool_t get(Int_t     **e) {*e=(Int_t*    )cElem; return setNotFirst();} // ...
    Bool_t get(UInt_t    **e) {*e=(UInt_t*   )cElem; return setNotFirst();} // ...
    Bool_t get(Long64_t  **e) {*e=(Long64_t* )cElem; return setNotFirst();} // ...
    Bool_t get(ULong64_t **e) {*e=(ULong64_t*)cElem; return setNotFirst();} // ...
    Bool_t get(Float_t   **e) {*e=(Float_t*  )cElem; return setNotFirst();} // ...
    Bool_t get(Double_t  **e) {*e=(Double_t* )cElem; return setNotFirst();} // ...
    Bool_t get(Bool_t    **e) {*e=(Bool_t*   )cElem; return setNotFirst();} // ...
    
    
    Bool_t setNext(Char_t    e) {if(nextAExp(kChar   )) return set(e); return kFALSE;}
    Bool_t setNext(UChar_t   e) {if(nextAExp(kUChar  )) return set(e); return kFALSE;}
    Bool_t setNext(Short_t   e) {if(nextAExp(kShort  )) return set(e); return kFALSE;}
    Bool_t setNext(UShort_t  e) {if(nextAExp(kUShort )) return set(e); return kFALSE;}
    Bool_t setNext(Int_t     e) {if(nextAExp(kInt    )) return set(e); return kFALSE;}
    Bool_t setNext(UInt_t    e) {if(nextAExp(kUInt   )) return set(e); return kFALSE;}
    Bool_t setNext(Long64_t  e) {if(nextAExp(kLong64 )) return set(e); return kFALSE;}
    Bool_t setNext(ULong64_t e) {if(nextAExp(kULong64)) return set(e); return kFALSE;}
    Bool_t setNext(Float_t   e) {if(nextAExp(kFloat  )) return set(e); return kFALSE;}   
    Bool_t setNext(Double_t  e) {if(nextAExp(kDouble )) return set(e); return kFALSE;}
    Bool_t setNext(Bool_t    e) {if(nextAExp(kBool   )) return set(e); return kFALSE;}
    
    Bool_t getNext(Char_t&    e) {if(next(kChar   )) return get(e); return kFALSE;}
    Bool_t getNext(UChar_t&   e) {if(next(kUChar  )) return get(e); return kFALSE;}
    Bool_t getNext(Short_t&   e) {if(next(kShort  )) return get(e); return kFALSE;}
    Bool_t getNext(UShort_t&  e) {if(next(kUShort )) return get(e); return kFALSE;}
    Bool_t getNext(Int_t&     e) {if(next(kInt    )) return get(e); return kFALSE;}
    Bool_t getNext(UInt_t&    e) {if(next(kUInt   )) return get(e); return kFALSE;}
    Bool_t getNext(Long64_t&  e) {if(next(kLong64 )) return get(e); return kFALSE;}
    Bool_t getNext(ULong64_t& e) {if(next(kULong64)) return get(e); return kFALSE;}
    Bool_t getNext(Float_t&   e) {if(next(kFloat  )) return get(e); return kFALSE;}   
    Bool_t getNext(Double_t&  e) {if(next(kDouble )) return get(e); return kFALSE;}
    Bool_t getNext(Bool_t&    e) {if(next(kBool   )) return get(e); return kFALSE;}
    
    Bool_t getNext(Char_t    **e) {if(next(kChar   )) return get(e); return kFALSE;}
    Bool_t getNext(UChar_t   **e) {if(next(kUChar  )) return get(e); return kFALSE;}
    Bool_t getNext(Short_t   **e) {if(next(kShort  )) return get(e); return kFALSE;}
    Bool_t getNext(UShort_t  **e) {if(next(kUShort )) return get(e); return kFALSE;}
    Bool_t getNext(Int_t     **e) {if(next(kInt    )) return get(e); return kFALSE;}
    Bool_t getNext(UInt_t    **e) {if(next(kUInt   )) return get(e); return kFALSE;}
    Bool_t getNext(Long64_t  **e) {if(next(kLong64 )) return get(e); return kFALSE;}
    Bool_t getNext(ULong64_t **e) {if(next(kULong64)) return get(e); return kFALSE;}
    Bool_t getNext(Float_t   **e) {if(next(kFloat  )) return get(e); return kFALSE;}   
    Bool_t getNext(Double_t  **e) {if(next(kDouble )) return get(e); return kFALSE;}
    Bool_t getNext(Bool_t    **e) {if(next(kBool   )) return get(e); return kFALSE;}
        
  ClassDef(HSeqArrIter,0)
};

#endif  /*!HSEQARRI_H*/
