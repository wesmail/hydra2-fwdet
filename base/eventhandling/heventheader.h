#ifndef HEVENTHEADER_H
#define HEVENTHEADER_H

#include "TObject.h"
#include "hgeomvector.h"

class HVertex : public TObject {
public:
 HVertex(void) { iterations=-1; }
 ~HVertex(void) {}

 void setPos(HGeomVector &p)      { pos = p; }
 void setIterations(Short_t i)    { iterations = i; }
 void setChi2(Float_t c)          { chi2 = c; }
 void setSumOfWeights(Float_t sw) { sumOfWeights = sw; }

 Short_t      getIterations(void)     { return iterations; }
 Float_t      getChi2(void)           { return chi2; }
 Float_t      getSumOfWeights(void)   { return sumOfWeights; }
 Double_t     getX()                  { return pos.getX(); }
 Double_t     getY()                  { return pos.getY(); }
 Double_t     getZ()                  { return pos.getZ(); }
 HGeomVector &getPos(void)            { return pos; }

protected:
 HGeomVector pos;          // Vertex position
 Short_t     iterations;   // Iterations until convergence. < 0, if no convergence
 Float_t     chi2; 
 Float_t     sumOfWeights; 

public:
 ClassDef(HVertex,2) // Event primary vertex
};


class HEventHeader : public TObject
{
private:
  HVertex fVertex;            // Vertex information (x, y, z (in mm) in lab system)
  HVertex fVertexCluster;     // Vertex information (x, y, z (in mm) in lab system from Clusters)
  HVertex fVertexReco;        // Vertex information (x, y, z (in mm) in lab system from Clusters)
  Int_t   timeInSpill;        // Time in spill
  UInt_t  downscaling;        // DAQ downscaling factor
  UInt_t  downscalingFlag;    // DAQ downscaling flag (bit 4 of EvtId)
  UInt_t  fDate;              // Date in DAQ format
  UInt_t  fErrorBit;          // Error bit (the MSB of EvtId)
  UInt_t  fEventDecoding;     // Decoding flag
  UInt_t  fEventPad;          // 64 bit alignment
  UInt_t  fEventRunNumber;    // Run number
  UInt_t  fEventSeqNumber;    // Event number
  UInt_t  fEventSize;         // Event size in bytes
  UInt_t  fId;                // Event id (bits 0...3 of EvtId)
  UInt_t  fTBit;              // Trigger bits (8 bits of LVL1 trigger box)
  UInt_t  fTime;              // Time in DAQ format
  UInt_t  fVersion;           // Version info (bits 12...15 of EvtId)
  UInt_t  triggerDecision;    // LVL2 decision in lower 16 bit (bits 5...7 of EvtId)
  UInt_t  triggerDecisionEmu; // MU emulation of LVL2 decission 

public:
  HEventHeader(void) : 
    timeInSpill(0),
    downscaling(0),
    downscalingFlag(0),
    fDate(0),
    fErrorBit(0),
    fEventDecoding(0),
    fEventPad(0),
    fEventRunNumber(0),
    fEventSeqNumber(0),
    fEventSize(0),
    fId(0),
    fTBit(0),
    fTime(0),
    fVersion(0),
    triggerDecision(0),
    triggerDecisionEmu(0) {;}

  ~HEventHeader(void) {;}

  void setDate(UInt_t date)                     { fDate = date; }
  void setDownscaling(UInt_t d)                 { downscaling = d; }
  void setDownscalingFlag(UInt_t df)            { downscalingFlag = (df & 0x01); }
  void setErrorBit(UInt_t eb)                   { fErrorBit = (eb & 0x01); }
  void setEventDecoding(UInt_t eventDecod)      { fEventDecoding = eventDecod; }
  void setEventPad(UInt_t evPad)                { fEventPad = evPad; }
  void setEventRunNumber(UInt_t evFileNr)       { fEventRunNumber = evFileNr; }
  void setEventSeqNumber(UInt_t evSeqNr)        { fEventSeqNumber = evSeqNr; }
  void setEventSize(UInt_t eventSize)           { fEventSize = eventSize; }
  void setId(UInt_t id)                         { fId = id; }
  void setMuEmuTriggerDecision(UInt_t td)       { triggerDecisionEmu = (td & 0xFF); }
  void setTBit(UInt_t bits)                     { fTBit = (bits & 0xFFFFFFFF); }
  void setTime(UInt_t time)                     { fTime = time; }
  void setTimeInSpill(Int_t val)                { timeInSpill = val; }
  void setTriggerDecision(UInt_t td)            { triggerDecision = (td & 0x07); }
  void setVersion(Int_t v)                      { fVersion = (v & 0x0F); }

  Bool_t   isTBit(Int_t i)                      { return ((fTBit >> i) & 1); }
  Double_t getVertexZ()                         { return fVertex.getZ(); }
  Double_t getVertexClusterZ()                  { return fVertexCluster.getZ(); }
  Double_t getVertexRecoZ()                     { return fVertexReco.getZ(); }
  HVertex  &getVertex(void)                     { return fVertex; }
  HVertex  &getVertexCluster(void)              { return fVertexCluster; }
  HVertex  &getVertexReco(void)                 { return fVertexReco; }
  Int_t    getTimeInSpill()                     { return timeInSpill; }
  UInt_t   getDate(void)                        { return fDate; }
  UInt_t   getDownscaling()                     { return downscaling; }
  UInt_t   getDownscalingFlag()                 { return downscalingFlag; }
  UInt_t   getErrorBit(void)                    { return fErrorBit; }
  UInt_t   getEventDecoding(void)               { return fEventDecoding; }
  UInt_t   getEventPad(void)                    { return fEventPad; }
  UInt_t   getEventRunNumber(void)              { return fEventRunNumber; }
  UInt_t   getEventSeqNumber(void)              { return fEventSeqNumber; }
  UInt_t   getEventSize(void)                   { return fEventSize; }
  UInt_t   getId(void)                          { return fId; }
  UInt_t   getMuEmuTriggerDecision()            { return triggerDecisionEmu; }
  UInt_t   getTBit(void)                        { return fTBit; }
  UInt_t   getTime(void)                        { return fTime; }
  UInt_t   getTriggerDecision()                 { return triggerDecision; }
  UInt_t   getVersion()                         { return fVersion; }
  inline Bool_t isNewDAQ()                      { return (fVersion==1); }   // new DAQ version (>= Sep03)
  inline Bool_t isOldDAQ()                      { return (fVersion==0); }   // old DAQ version (< Sep03)

  ClassDef(HEventHeader,9) // HADES event header class
};

#endif /* !HEVENTHEADER_H */


