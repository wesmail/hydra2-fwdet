#ifndef HSPLINETRACK_H
#define HSPLINETRACK_H
#include "TObject.h"
#include "hbasetrack.h"
class HSplineTrack : public HBaseTrack 
{
private:
   Float_t qSpline;
   Int_t numOfChambers;
public:
   HSplineTrack();
   virtual  ~HSplineTrack() {}
   void setNumOfChambers(Int_t nch) {numOfChambers=nch;}
   Int_t getNumOfChambers() {return numOfChambers;}
   void setQSpline(Float_t QS) {qSpline=QS;}
   Float_t getQSpline() {return qSpline;}
   ClassDef (HSplineTrack,1)
};
#endif


  
