#ifndef HSUEXEMACRO_H
#define HSUEXEMACRO_H

// -----------------------------------------------------------------------------
// $Id: hsuexemacro.h,v 1.8 2008-09-18 13:04:57 halo Exp $
// Author: Marcin Jaskula
// -----------------------------------------------------------------------------

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TString.h"
#include "TObjString.h"
#include "TMap.h"

#include <hsuprogress.h>

// -----------------------------------------------------------------------------

class HSUExeMacro;
class HCategory;

// -----------------------------------------------------------------------------

class HSUBranchElement : public TObject
{
public:
    enum EBranchType
    {
        kArray = 0,
        kCategory,
        kMax
    };

    // -------------------------------------------------------------------------
private:

    TString        *m_pName;    // name of branch
    TObject        *m_pArray;   // pointer to array or category
    TBranch        *m_pBranch;  // pointer to branch

    EBranchType     m_eType;    // type of array

    // -------------------------------------------------------------------------

                    HSUBranchElement(const Char_t* pBranchName,
                            EBranchType eType);
                   ~HSUBranchElement(void);

    // -------------------------------------------------------------------------

    const Char_t*     getBranchNameSufix(void);

    // -------------------------------------------------------------------------

    friend class HSUExeMacro;
};

// -----------------------------------------------------------------------------

class HSUExeMacro : public TObject
{
protected:
    TChain          *m_pChain;          // chain of input files
    Int_t            m_iCurrentFile;    // current file in the chain

    HSUProgress     *m_pProgress;       // progress bar

    TFile           *m_pOutputFile;     // output file
    TObjArray       *m_pActiveBranches; // array with active branches

    Bool_t           m_bExitOnError;    // if exit on any error

    TString          m_sOutputName;     // name of the output file
    TObjArray       *m_pInputArray;     // list of input files

    Int_t            m_iEvents;         // max number of events
    Int_t            m_iEvent;          // current event

    TString          m_sParamFileName;  // name of a file with parameters
    TFile           *m_pParamFile;      // file with parameters

    TString          m_sOptString;      // string from parameters
    Int_t            m_iOptInt;         // Int_t from parameters
    Float_t          m_fOptFloat;       // Float_t from parameters
    Bool_t           m_bOptBool;        // boolean from parameters

    Bool_t           m_bNotFileChange;  // notify when the file changes

public:
                     HSUExeMacro();
    virtual         ~HSUExeMacro();

    // -------------------------------------------------------------------------

    virtual Int_t    loadArgs(Int_t iArg, Char_t* vArg[]);

    // -------------------------------------------------------------------------

    Bool_t           openInput(void);
    Bool_t           addInputFile(const Char_t *pStr);
    Bool_t           addFilesFromList(const Char_t *pStr);

    // -------------------------------------------------------------------------

    Bool_t           openOutput(void);
    Bool_t           writeOutput(void);

    // -------------------------------------------------------------------------

    Int_t            loadTree(Int_t iEvent);
    Bool_t           checkBranches(void);
    void             clear(void);
    void             getBranchesEntry(Int_t iEntry);
    Int_t            getEvent(Int_t iEvent);
    Int_t            nextEvent(void);

    // -------------------------------------------------------------------------

    TClonesArray*    getTClonesArray(const Char_t *pBranchName);
    HCategory*       getHCategory(const Char_t *pCategoryName);
    TBranch*         getBranch(const Char_t *pBranchName);

    // -------------------------------------------------------------------------

    virtual void     printInfo(const Char_t *pProgramName) const;

    // -------------------------------------------------------------------------

    Bool_t           getExitOnError(void) const       { return m_bExitOnError; }
    void             setExitOnError(Bool_t b = kTRUE) { m_bExitOnError = b;    }

    // -------------------------------------------------------------------------

    void             setProgress(HSUProgress *pProgress);
    void             setProgress(Bool_t bSet = kTRUE);
    HSUProgress*     getProgress(void) const;
    void             deleteProgress(void);

    // -------------------------------------------------------------------------

    Bool_t           openParamFile(const Char_t *pParamFileName = NULL);
    TObject*         getParamObject(const Char_t *pName);

    const TString&   getParamFileName(void) const;

    Bool_t           isParamFileOpened(void) const
                                                { return m_pParamFile != NULL; }

    // -------------------------------------------------------------------------

    const TString&   getOptString(void) const         { return m_sOptString;   }
    Int_t            getOptInt(void) const            { return m_iOptInt;      }
    Float_t          getOptFloat(void) const          { return m_fOptFloat;    }
    Bool_t           getOptBool(void) const           { return m_bOptBool;     }

    // -------------------------------------------------------------------------

    Int_t            getEvents(void) const            { return m_iEvents;      }
    Int_t            getEvent(void) const             { return m_iEvent;       }

    // -------------------------------------------------------------------------

    Float_t          getMdcPhi(Int_t iSector, Float_t fPhiMdc) const;
    Float_t          getMdcTheta(Float_t fThetaMdc) const;

    // -------------------------------------------------------------------------

    Bool_t           getNotFileChange(void)       { return m_bNotFileChange;   }
    void             setNotFileChange(Bool_t b)   { m_bNotFileChange = b;      }

    // -------------------------------------------------------------------------

private:
    HSUBranchElement* findAciveBranch(const Char_t *pBranchName);
    TClonesArray*     getNewArrayFromBranch(const Char_t *pBranchName);
    HCategory*        getNewHCategoryFromBranch(const Char_t *pBranchName);
    Bool_t            notifyBranches(void);

    // -------------------------------------------------------------------------

    ClassDef(HSUExeMacro, 0) //! Easy acces to branches in exe macro
};

// -----------------------------------------------------------------------------

#ifndef SAFE_DELETE
#define SAFE_DELETE(A)      { if(A != NULL) { delete A; A = NULL; } }
#endif

#ifndef SAFE_DELETE_ROOT
#define SAFE_DELETE_ROOT(A) { if(A != NULL) { A->Delete(); A = NULL; } }
#endif

// -----------------------------------------------------------------------------

#endif //!HSUEXEMACRO_H
