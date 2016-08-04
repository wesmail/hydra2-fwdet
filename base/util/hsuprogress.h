#ifndef HSUPROGRESS_H
#define HSUPROGRESS_H
#pragma interface

// -----------------------------------------------------------------------------

#include "TStopwatch.h"
#include "TString.h"

// -----------------------------------------------------------------------------

class HSUProgress
{
public:
                HSUProgress(Int_t iMax = 100);
    virtual    ~HSUProgress();

    // -------------------------------------------------------------------------

    void        SetMaxValue(Int_t i)        { m_iMax = i;           }
    Int_t       GetMaxValue(void)           { return m_iMax;        }

    void        Reset(void);
    void        Restart(void);
    Int_t       GetLast(void)               { return m_iLast;       }
    void        Next(Int_t iSteps = 1);
    void        Final(void);

    void        SetSpinOn(Bool_t b)         { m_bSpin = b;          }
    Bool_t      GetSpinOn(void)             { return m_bSpin;       }
    void        SetSpinPer(Float_t f);
    void        SetSpinEvents(Int_t i);
    const Char_t* GetSpinChars(void)          { return m_psSpinChars.Data(); }
    void        SetSpinChars(const Char_t *pStr);
    void        SetSpinChars(Int_t iType);
    Float_t     GetSpinReal(void);

    void        SetProgressOn(Bool_t b)     { m_bProgress = b;      }
    Bool_t      GetProgressOn(void)         { return m_bProgress;   }
    void        SetProgressPer(Float_t f);
    void        SetProgressEvents(Int_t i);
    Char_t        GetProgressChar(void)       { return m_cProgressChar; }
    void        SetProgressChar(Char_t c)     { m_cProgressChar = c;  }
    Float_t       GetProgressReal(void);

    void        SetTimerOn(Bool_t b)        { m_bTimer = b;         }
    Bool_t      GetTimerOn(void)            { return m_bTimer;      }
    void        SetTimerPer(Float_t f);
    void        SetTimerEvents(Int_t i);
    Float_t       GetTimerReal(void);

    void        SetFullTimerOn(Bool_t b)    { m_bFullTimer = b;     }
    Bool_t      GetFullTimerOn(void)        { return m_bFullTimer;  }

    // -------------------------------------------------------------------------

private:

    Int_t       m_iMax;             // max numer of envents
    Int_t       m_iLast;            // current number

    Bool_t      m_bSpin;            // spin option
    Bool_t      m_bSpinInPer;       // m_fSpinSet in percents or events
    Float_t     m_fSpinSet;         // how often spin
    Float_t     m_fSpinReal;        // how often spin in events
    TString     m_psSpinChars;      // list of spin's characters
    Int_t       m_iSpinLen;         // length of the m_psSpinChars

    Bool_t      m_bProgress;        // print progress bar
    Bool_t      m_bProgressInPer;   // m_fProgressSet in percents
    Float_t     m_fProgressSet;     // how often print progress
    Float_t     m_fProgressReal;    // how often print progress / events
    Char_t        m_cProgressChar;    // a character for the progress bar

    Bool_t      m_bTimer;           // print timer value
    Bool_t      m_bTimerInPer;      // m_fTimerSet in percents
    Float_t     m_fTimerSet;        // how often print timer info
    Float_t     m_fTimerReal;       // how often print timer info in events

    Bool_t      m_bFullTimer;       // print full time info at the end

    TStopwatch  tTimer;             // timer for time info

    Bool_t      m_bWasOutOfRange;   // flag if the progress was out of range

    // -------------------------------------------------------------------------

    ClassDef(HSUProgress, 1)        //! Progress bar class
};

// -----------------------------------------------------------------------------

#endif //!HSUPROGRESS_H
