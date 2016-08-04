#ifndef HDATASOURCE_H
#define HDATASOURCE_H

//#include "haddef.h"
#include "TObject.h"
class HEvent;

enum EDsState {kDsOk=0,kDsEndFile=1,kDsEndData=2,kDsError=3,kDsSkip=4};

class HDataSource : public TObject {
protected:
    HEvent **fEventAddr; //! Address of the event to fill
    Int_t     fForcedID; //! (default =-1, not used, any other value between 1-15 will replace the ID in the eventheader!)
public:
   HDataSource() : fForcedID (-1) { ; }
   void setEventAddress(HEvent **ev);
   void forceID(Int_t id) { if (id>0&&id<16) { fForcedID = id ; } else { Error("forceID()","Id is outside allowd range 1-15. ignored!");} }
   virtual EDsState skipEvents(Int_t nEv);
   virtual EDsState getNextEvent(Bool_t doUnpack=kTRUE)=0;
   virtual void setCursorToPreviousEvent(){};
   virtual Bool_t init(void)=0;
   virtual Bool_t reinit(void)=0;
   virtual Bool_t finalize(void)=0;
   virtual Bool_t rewind(void)=0;
   virtual Int_t getCurrentRunId(void)=0;
   virtual Int_t getCurrentRefId(void)=0; //Run Id used for initialization
   virtual Text_t const *getCurrentFileName(void)=0;
   ClassDef(HDataSource,1) //Event's input data manager.
};

#endif /* !HDATASOURCE_H */
