#ifndef HLDSOURCE_H
#define HLDSOURCE_H

#include "hdatasource.h"
#include "TList.h"

class HldUnpack;
class HldEvt;
class HEventHeader;
class HTrbNetUnpacker;

class HldSource : public HDataSource {
protected:
    friend class HldFileOutput;
    HTrbNetUnpacker *hubUnpacker;
public:
    HldSource(void);
    virtual ~HldSource(void);
    virtual EDsState getNextEvent(Bool_t doUnpack=kTRUE)=0;
    Bool_t   initUnpacker(void);
    Bool_t   reinit(void);
    Bool_t   finalizeUnpacker(void);
    Bool_t   rewind(void) {return kTRUE;}
    void     addUnpacker(HldUnpack *unpacker);
    Bool_t   finalize(void);
    EDsState showIt(HldEvt *evt);
    EDsState dumpEvt(void);
    EDsState scanIt(HldEvt *evt);
    EDsState scanEvt(void);
    Bool_t   getDecodingStyle() {return oldDecodingStyle;}
    void     setOldDecodingStyle(Bool_t decodingStyle=kTRUE)  {oldDecodingStyle= decodingStyle;}
    void     setScanned(Bool_t scanned=kTRUE) {isScanned=scanned;}
    HTrbNetUnpacker *getTrbNetUnpacker(void){return hubUnpacker;};
protected:
    Bool_t isDumped,isScanned;
    Bool_t oldDecodingStyle;
    void decodeHeader(HEventHeader *dest);
    TList* fUnpackerList; //! List of the unpackers used to extract data
    HldEvt* fReadEvent;   //! Buffer where the data is first read.

public:
    inline void setDump(){isDumped=kTRUE;}
public:
    ClassDef(HldSource,0) //Data source to read LMD data
};
#endif /* !HLDSOURCE_H */

