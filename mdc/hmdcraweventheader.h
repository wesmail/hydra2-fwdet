#ifndef HMDCRAWEVENTHEADER_H
#define HMDCRAWEVENTHEADER_H

#include "TObject.h"
#include <stdio.h>

class HMdcRawEventHeader : public TObject {
protected:
  Int_t errorWord;   // error word of each ROC
  Int_t module;      // module number [0-3]
  Int_t roc;         // read out controler (ROC) number
  Int_t sector;      // sector number [0-5]
  Int_t subEvtId;    // subevent id 
  Int_t subEvtSize;  // subevent size
  Int_t triggerType; // trigger type

public:
    HMdcRawEventHeader(void) { clear(); }
    ~HMdcRawEventHeader(void){;}
    void clear(void) {
	// reset data members to default values

	errorWord=module=roc=sector=subEvtId=subEvtSize=triggerType=-999;
    }

    void print(void)
      {
        // print the data member to the screen
	printf("%s: ",this->ClassName());
	printf("subEvtId: %4i "  ,subEvtId);
	printf("subEvtSize: %4i ",subEvtSize);
	printf("trigType: %2i "  ,triggerType);
	printf("roc: %2i "       ,roc);
	printf("errorWord: %2i " ,errorWord);
	printf("module: %2i "    ,module);
	printf("sector: %2i "    ,sector);
	printf("\n");
      }

    void set( Int_t err,   Int_t mod,   Int_t ro, 
	      Int_t sec,   Int_t subId, Int_t subSize, 
	      Int_t trigType)
      { 
	errorWord=err;
	module=mod;
	roc=ro;
	sector=sec;
	subEvtId=subId;
	subEvtSize=subSize;
	triggerType=trigType;
      };

    void setErrorWord  (const Int_t i){errorWord=i;};
    void setModule     (const Int_t i){module=i;};
    void setRoc        (const Int_t i){roc=i;};
    void setSector     (const Int_t i){sector=i;};
    void setSubEvtId   (const Int_t i){subEvtId=i;};
    void setSubEvtSize (const Int_t i){subEvtSize=i;};
    void setTriggerType(const Int_t i){triggerType=i;};

    Int_t getErrorWord  (void){return errorWord;};
    Int_t getModule     (void){return module;};
    Int_t getRoc        (void){return roc;};
    Int_t getSector     (void){return sector;};
    Int_t getSubEvtId   (void){return subEvtId;};
    Int_t getSubEvtSize (void){return subEvtSize;};
    Int_t getTriggerType(void){return triggerType;};

    ClassDef(HMdcRawEventHeader,1) // subevent header of MDC
};

#endif /* !HMDCRAWEVTHEADER_H */
