#ifndef HRUNTIMEDB_H
#define HRUNTIMEDB_H

#include "TObject.h"
#include "TList.h"

class HParIo;
class HParSet;
class HRun;
class HContFact;
class HParamFileGenerator;


class HRuntimeDb : public TObject {
private:
  static HRuntimeDb* gRtdb; //!
protected:
  HRuntimeDb(void);
  TList  contFactories;    // list of container factories 
  TList* containerList;    // list of parameter containers
  TList* runs;             // list of runs
  HParIo* firstInput;      // first (prefered) input for parameters
  HParIo* secondInput;     // second input (used if not found in first input)
  HParIo* output;          // output for parameters
  HRun* currentRun;        // Current run
  TString currentFileName; // Name of current event file
  Bool_t versionsChanged;  // flag for write of list of runs (set kTRUE by each write)
  Bool_t isRootFileOutput; // flag indicating that the output is a ROOT file
  HParamFileGenerator* pParamFileGenerator; //! generator for a parameter file
public:
  static HRuntimeDb* instance(void);
  ~HRuntimeDb(void);

  Bool_t addParamContext(const Char_t*);
  void printParamContexts();
  void addContFactory(HContFact*);

  Bool_t addContainer(HParSet*);
  HParSet* getContainer(const Text_t*);
  HParSet* findContainer(const Char_t*);
  void removeContainer(Text_t*);
  void removeAllContainers(void);
  Bool_t initContainers(Int_t runId,Int_t refId=-1,const Text_t* fileName="");
  void printContainers(void);
  void setContainersStatic(Bool_t f=kTRUE);
  Bool_t writeContainers(void);
  Bool_t writeContainer(HParSet*,HRun*,HRun* refRun=0);

  HRun* addRun(Int_t runId,Int_t refId=-1);
  HRun* getRun(Int_t);
  HRun* getRun(Text_t*);
  HRun* getCurrentRun(void) {return currentRun;}
  Text_t const *getCurrentFileName() {return currentFileName.Data();}
  void clearRunList(void);
  void removeRun(Text_t*);

  Bool_t setInputVersion(Int_t run,Text_t* container,
                        Int_t version,Int_t inputNumber);
  Bool_t setRootOutputVersion(Int_t run,Text_t* container,Int_t version);
  void setVersionsChanged(Bool_t f=kTRUE) {versionsChanged=f;}
  void resetInputVersions(void);
  void resetOutputVersions(void);
  void resetAllVersions(void);

  Bool_t readAll(void);
  void writeSetup(void);
  void writeVersions(void);
  void saveOutput(void);

  Bool_t setFirstInput(HParIo*);
  Bool_t setSecondInput(HParIo*);
  Bool_t setOutput(HParIo*);
  HParIo* getFirstInput(void);
  HParIo* getSecondInput(void);
  HParIo* getOutput(void);
  void closeFirstInput(void);
  void closeSecondInput(void);
  void closeOutput(void);
  Bool_t reconnectInputs();
  void disconnectInputs();

  void print(void);

  Bool_t makeParamFile(const Char_t*,const Char_t*,
                       const Char_t* startAt="",const Char_t* endAt="");

  Int_t findOutputVersion(HParSet*);

private:
  Bool_t initContainers(void);
  Bool_t fillParamFile();

  ClassDef(HRuntimeDb,0) // Class for runtime database
};

#endif  /* !HRUNTIMEDB_H */
