#pragma implementation

// -----------------------------------------------------------------------------

#include <Rtypes.h>
#include "TString.h"
#include <stdio.h>
#include "hsuprogress.h"

// -----------------------------------------------------------------------------
//*-- Author : Marcin Jaskula
// -----------------------------------------------------------------------------

ClassImp(HSUProgress)

// -----------------------------------------------------------------------------

static const Char_t* g_pctHSUProgressConst[] = { ".oOo", "-\\|/" };

//______________________________________________________________________________
// A progress bar class.
// It gives a simple interface for showing a progress bar of some Long_t task
// as well as it may estimate the time left to the end of the job.
// A typical output of the progress bar looks like:
//
// ##########  10 % done in     1.85 s; ~    16.65 s left;    540.54 evts/s
// ##########  20 % done in     3.65 s; ~    14.60 s left;    547.95 evts/s
// ##########  30 % done in     5.42 s; ~    12.65 s left;    553.51 evts/s
// ##########  40 % done in     7.21 s; ~    10.82 s left;    554.79 evts/s
// ##########  50 % done in     9.08 s; ~     9.08 s left;    550.66 evts/s
// ####-
// ....
//
// 10000 events done in    18.05 s;   554.02 evts/s
//
// All parameters of the display are configurable:
//
// Progress bars on the left: ("#####" in the example)
// - turn it on/off with SetProgressOn method (default on)
// - how often a new character in the bar appears may be set by:
//     - SetProgressPer
//     - SetProgressEvents
//     methods. Default: each 1% of the max events
// - the character in the bar may be changed by GetProgressChar method (def. #)
//
// Spinning character: ("-" in the example)
// - turn it on/off with SetSpinOn method (default on)
// - how often it spins may be changed by:
//     - SetSpinPer
//     - SetSpinEvents
//     methods. Default: 0.1%
// - characters which define a spin may be set by methods:
//     - SetSpinChars(Int_t iType)
//          iType = 0: ".oOo"
//                = 1: "-\\|/"
//     - SetSpinChars(const Char_t *pStr) - sets user define string
//          e.g. SetSpinChars("Hades is fine")
//
// Timing information: ("10% done ..." in the example)
// - turn in on/off with SetTimerOn method (default on)
// - how often it appears may be changed by:
//     - SetTimerPer
//     - SetProgressEvents
//     methods. Default: 10%
//
// Information about full time of the loop: (the last line)
// - turn it on/off with SetFullTimerOn method (default on)
//
//
// If the counter overflows the maximum value defined in the constructor
// a warning is reported for the first time.
//

HSUProgress::HSUProgress(Int_t iMax)
{
// Constructor in which a maximum number of steps may be set.
// The default value of iMax = 100
    Reset();
    m_iMax = iMax;
}

// -----------------------------------------------------------------------------

void HSUProgress::Reset()
{
// Reset all parameters to the default values
    m_iMax           = 100;
    m_iLast          = -1;

    m_bSpin          = kTRUE;
    m_bSpinInPer     = kFALSE;
    m_fSpinSet       = 0.1f;
    m_fSpinReal      = 0.0f;
    m_psSpinChars    = g_pctHSUProgressConst[0];
    m_iSpinLen       = strlen(m_psSpinChars);

    m_bProgress      = kTRUE;
    m_bProgressInPer = kTRUE;
    m_fProgressSet   = 1.0f;
    m_fProgressReal  = 0.0f;
    m_cProgressChar  = '#';

    m_bTimer         = kTRUE;
    m_bTimerInPer    = kTRUE;
    m_fTimerSet      = 10.0f;
    m_fTimerReal     = 0.0f;

    m_bFullTimer     = kTRUE;

    tTimer.Reset();

    m_bWasOutOfRange = kFALSE;
}

// -----------------------------------------------------------------------------

HSUProgress::~HSUProgress()
{
// An empty destructor
}

// -----------------------------------------------------------------------------

void HSUProgress::Restart(void)
{
// Reset the counter
    m_iLast = -1;
}

// -----------------------------------------------------------------------------

void HSUProgress::SetSpinPer(Float_t f)
{
// How often the spin changes - in per cent of the iMax
    m_bSpinInPer = kTRUE;
    m_fSpinSet = (f <= 0.0f) ? 0.01f : f;
}

// -----------------------------------------------------------------------------

void HSUProgress::SetSpinEvents(Int_t i)
{
// How often the spin changes - in events
    m_bSpinInPer = kFALSE;
    m_fSpinSet = (i <= 0) ? 1.0f : (Float_t)i;
}

// -----------------------------------------------------------------------------

void HSUProgress::SetSpinChars(const Char_t *pStr)
{
// Set the string of characters used by spin
    m_psSpinChars = pStr;
    m_iSpinLen    = strlen(m_psSpinChars);
}

// -----------------------------------------------------------------------------

void HSUProgress::SetSpinChars(Int_t iType)
{
// Set the string of characters used by spin from two defaults:
// 0 - ".oOo"
// 1 - "-\\|/"
    if((iType >= 0) && (iType < (Int_t)sizeof(g_pctHSUProgressConst)))
        m_psSpinChars = g_pctHSUProgressConst[iType];

    m_iSpinLen = strlen(m_psSpinChars);
}

// -----------------------------------------------------------------------------

Float_t HSUProgress::GetSpinReal(void)
{
// How often (in events) spin changes
    m_fSpinReal = (m_bSpinInPer)
                    ? m_fSpinSet : (100.0f * m_fSpinSet / m_iMax);

    return m_fSpinReal;
}

// -----------------------------------------------------------------------------

void HSUProgress::SetProgressPer(Float_t f)
{
// How often the progress bar changes - in per cent
    m_bProgressInPer = kTRUE;
    m_fProgressSet = (f <= 0.0f) ? 0.01f : f;
}

// -----------------------------------------------------------------------------

void HSUProgress::SetProgressEvents(Int_t i)
{
// How often the progress bar changes - in events
    m_bProgressInPer = kFALSE;
    m_fProgressSet = (i <= 0) ? 1.0f : (Float_t)i;
}

// -----------------------------------------------------------------------------

Float_t HSUProgress::GetProgressReal(void)
{
// How often (in events) progress bar changes
    m_fProgressReal = (m_bProgressInPer)
                    ? m_fProgressSet : (100.0f * m_fProgressSet / m_iMax);

    return m_fProgressReal;
}

// -----------------------------------------------------------------------------

void HSUProgress::SetTimerPer(Float_t f)
{
// How often the timer info appears - in per cent
    m_bTimerInPer = kTRUE;
    m_fTimerSet = (f <= 0.0f) ? 0.01f : f;
}

// -----------------------------------------------------------------------------

void HSUProgress::SetTimerEvents(Int_t i)
{
// How often the timer info appears - in events
    m_bTimerInPer = kFALSE;
    m_fTimerSet = (i <= 0) ? 1.0f : (Float_t)i;
}

// -----------------------------------------------------------------------------

Float_t HSUProgress::GetTimerReal(void)
{
// How often (in events) the timer info appears
    m_fTimerReal = (m_bTimerInPer)
                    ? m_fTimerSet : (100.0f * m_fTimerSet / m_iMax);

    return m_fTimerReal;
}

// -----------------------------------------------------------------------------

void HSUProgress::Next(Int_t iSteps)
{
// Execute the progress for the next step
// Default iSteps = 1
Float_t fPrev;
Float_t fCur;
Int_t   iChangeSpin     = 0;
Bool_t  bChangeSpin     = kFALSE;
Int_t   iChangeProgress = 0;
Bool_t  bChangeTimer    = kFALSE;
Int_t   i;

    if(m_iMax <= 0)
    {
        if( ! m_bSpin)
            return;

        if(m_iLast < 0)
        {
            fPrev = 0.0;
            m_iLast = iSteps;
            m_fTimerReal = ((m_bTimerInPer) || (m_fTimerSet <= 0))
                                ? 10 : m_fTimerSet;
        }
        else
        {
            fPrev    = m_iLast / m_fTimerReal;
            m_iLast += iSteps;
        }

        fCur = m_iLast / m_fTimerReal;

        if(Int_t(fCur) != Int_t(fPrev))
        {
            iChangeSpin = Int_t(fCur);
            bChangeSpin = kTRUE;
        }
    }
    else
    {
        if(m_iLast >= m_iMax)
        {
            if( ! m_bWasOutOfRange)
            {
                printf("\nProgress out of range: %d / %d\n", m_iLast, m_iMax);
                m_bWasOutOfRange = kTRUE;
            }

            m_iLast += iSteps;

            return;
        }

        if(m_iLast < 0)
        {
            GetSpinReal();
            GetProgressReal();
            GetTimerReal();

            fPrev   = 0.0f;
            m_iLast = iSteps;
            fCur    = (100.0f * m_iLast) / m_iMax;
            bChangeSpin = m_bSpin;
            tTimer.Start(kTRUE);
        }
        else
        {
            fPrev    = (100.0f * m_iLast) / m_iMax;
            m_iLast += iSteps;
            fCur     = (100.0f * m_iLast) / m_iMax;
        }

        if(m_bSpin)
        {
            iChangeSpin = (Int_t)(fCur / m_fSpinReal);
            if((Int_t)(fPrev / m_fSpinReal) != iChangeSpin)
                bChangeSpin = kTRUE;
        }

        if(m_bProgress)
        {
            iChangeProgress = (Int_t)(fCur / m_fProgressReal)
                                - (Int_t)(fPrev / m_fProgressReal);
        }

        if(m_bTimer)
        {
            if((Int_t)(fPrev / m_fTimerReal) != (Int_t)(fCur / m_fTimerReal))
                bChangeTimer = kTRUE;
        }
    }

    if((m_bSpin) && ((iChangeProgress > 0) || (bChangeTimer)))
        bChangeSpin = kTRUE;

        // clear spin
    if((bChangeSpin != 0) && (fPrev != 0.0f))
        printf("\b");

        // write progress
    for(i = 0; i < iChangeProgress; i++)
        printf("%c", m_cProgressChar);

        // write the timer info
    if(bChangeTimer)
    {
        printf(" %3d %% done in %8.2f s; ~ %8.2f s left;  %8.2f evts/s\n",
                (Int_t)fCur, tTimer.RealTime(),
                (100.0f - fCur) * tTimer.RealTime() / fCur,
                ((double)m_iLast / tTimer.RealTime()));

        tTimer.Continue();
    }

        // write the spin or the total time
    if((m_iMax <= 0) || (m_iLast < m_iMax))
    {
        if(bChangeSpin)
            printf("%c", m_psSpinChars[iChangeSpin % m_iSpinLen]);
    }
    else
    {
        if(m_bFullTimer)
            Final();
    }

    if((bChangeSpin) || (iChangeProgress > 0) || (bChangeTimer))
        fflush(stdout);
}

// -----------------------------------------------------------------------------

void HSUProgress::Final(void)
{
// Print the final line of the progress bar
    if(m_iLast <= m_iMax)
    {
        printf("\n%d events done in %8.2f s; %8.2f evts/s\n",
            m_iLast, tTimer.RealTime(), ((double)m_iLast / tTimer.RealTime()));
    }

    m_iLast = m_iMax + 1;
}
