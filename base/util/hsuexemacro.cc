#include "hsuexemacro.h"
#include "hcategory.h"
#include "iostream"
#include "TMath.h"

#include <getopt.h>

// -----------------------------------------------------------------------------
// $Id: hsuexemacro.cc,v 1.8 2008-05-16 16:25:12 halo Exp $
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// *-- Author : Marcin Jaskula
// *-- Modified : Jacek Otwinowski 06/02/2006
// -----------------------------------------------------------------------------

using namespace std; 

HSUBranchElement::HSUBranchElement(const Char_t* pBranchName, EBranchType eType)
{
    if((eType < 0) || (eType >= kMax))
    {
        Error("HSUBranchElement", "Unknown type %d\n", eType);
        gSystem->Exit(-1);
    }

    m_pArray  = NULL;
    m_pBranch = NULL;

    m_eType = eType;
    m_pName = new TString(pBranchName);
}

// -----------------------------------------------------------------------------

HSUBranchElement::~HSUBranchElement(void)
{
    SAFE_DELETE_ROOT(m_pArray);
    SAFE_DELETE(m_pName);
}

// -----------------------------------------------------------------------------

const Char_t* HSUBranchElement::getBranchNameSufix(void)
{
static const Char_t* asHSUBranchElementBranchesSufix[HSUBranchElement::kMax]
        = {".fData","."};

    return asHSUBranchElementBranchesSufix[m_eType];
}

//______________________________________________________________________________
// A class to make live easier with chains and branches in an executive macro.
// It is dedicated for making complicate correlation between branches.
//
// It supports:
// - TChain with files consists of HADES 'T' Tree
// - automatic access to branches' data by HCategory or TClonesArray
// - easy access to program arguments
// - easy access to parameters in a parameter files
//
// A simple program which shows possibility may looks like:
//
// Int_t main(Int_t iArg, Char_t* vArg[])
// {
// HSUExeMacro eEM;
//
//     eEM.loadArgs(iArg, vArg);
//     eEM.openInput();
//     eEM.openParamFile();
//     eEM.openOutput();
//
// HCategory *pMdcCategory;
// TCutG     *pCut = NULL;
// HMdcSeg   *pSeg;
//
//     if(eEM.getOptBool())
//         pCut = (TCutG *)eEM.getParamObject("some_cut");
//
//     pMdcCategory = eEM.getHCategory("HMdcSeg");
//
//     while(eEM.nextEvent() >= 0)
//     {
//         for(i = 0; i < pMdcCategory->getEntries(); i++)
//         {
//             if((pSeg = (HMdcSeg * )pMdcCategory->getObject(i)) == NULL)
//                 continue;
//
//             ...
//         }
//     }
//
//     eEM.writeOutput();
//
//     return 0;
// }
//
// Arguments of the program supported by the class:
// -i file_name - (may appear many times) define a list of input files.
//                If the file is *.root it is added to the chain.
//                Otherwise it is taken as a text file with a list of root files
//                all empty lines and lines starting with # are skipped
// -o file_name - a name of an output file
// -e number    - maximum number of events to process
// -p file_name - a name of an optional file with some object for the program;
//                they are easy to get by getParamObject() method
// -h           - prints a help message
//
// Other program arguments which may be used by the program:
// -s string    - string can be get by getOptString() method
// -n Int_t       - can be get by getOptInt()
// -f Float_t     - can be get by getOptFloat()
// -b           - existence of the parameter can be get by getOptBool()
//
//______________________________________________________________________________

ClassImp(HSUExeMacro)

// -----------------------------------------------------------------------------

HSUExeMacro::HSUExeMacro(void)
{
// A constructor of the class

    m_pChain       = new TChain("T");
    m_pProgress    = NULL;
    m_iEvents      = -1;
    m_pOutputFile  = NULL;
    m_iCurrentFile = -1;
    m_iEvent       = -1;
    m_bExitOnError = kTRUE;

    m_pInputArray  = new TObjArray();
    m_pInputArray->SetOwner();

    m_pActiveBranches = new TObjArray();
    m_pActiveBranches->SetOwner();

    m_pParamFile   = NULL;

    m_iOptInt      = 0;
    m_fOptFloat    = 0.0f;
    m_bOptBool     = kFALSE;

    m_bNotFileChange = kFALSE;
}

// -----------------------------------------------------------------------------

HSUExeMacro::~HSUExeMacro()
{
// Destructor.
// Close all input and output files opened by the class and delete what was used

    SAFE_DELETE_ROOT(m_pChain);
    SAFE_DELETE_ROOT(m_pInputArray);
    SAFE_DELETE_ROOT(m_pOutputFile);
    SAFE_DELETE_ROOT(m_pActiveBranches);
    SAFE_DELETE(m_pProgress);
    SAFE_DELETE_ROOT(m_pParamFile);
}

// -----------------------------------------------------------------------------

void HSUExeMacro::setProgress(HSUProgress *pProgress)
{
// Set a progress bar used by the class during the events loop

    deleteProgress();
    m_pProgress = pProgress;
}

// -----------------------------------------------------------------------------

void HSUExeMacro::setProgress(Bool_t bSet)
{
// Set a default progress bar used by the class during the events loop.
// If bSet == kFALSE - removes the progress bar

    deleteProgress();

    if(bSet)
    {
        m_pProgress = new HSUProgress();
        m_pProgress->SetSpinEvents(1000);
    }
}

// -----------------------------------------------------------------------------

HSUProgress* HSUExeMacro::getProgress(void) const
{
// Return the progress bar used by the class

    return m_pProgress;
}

// -----------------------------------------------------------------------------

void HSUExeMacro::deleteProgress(void)
{
// Delete the progress bar used by the class

    SAFE_DELETE(m_pProgress);
}

// -----------------------------------------------------------------------------

Int_t HSUExeMacro::loadArgs(Int_t iArg, Char_t* vArg[])
{
// Load all arguments from the program arguments given as the parameters.
// All parameters supported by the class are listed in the class description.
Int_t   option;
Int_t   i, iError;
Float_t f;

    opterr = 0;
    iError = 0;
    while((option = getopt(iArg, vArg, "n:p:f:s:i:c:o:e:hb")) != -1)
    {
        switch(option)
        {
            case 'i':
                m_pInputArray->Add(new TObjString(optarg));
                break;

            case 'o':
                m_sOutputName = optarg;
                break;

            case 'e':
                if(sscanf(optarg, "%d", &i) != 1)
                    iError++;
                else
                    m_iEvents = i;

                break;

            case 'h':
                iError = -1000000;
                break;

            case 'p':
                m_sParamFileName = optarg;
                break;

            case 's':
                m_sOptString = optarg;
                break;

            case 'n':
                if(sscanf(optarg, "%d", &i) != 1)
                    iError++;
                else
                    m_iOptInt = i;
                break;

            case 'f':
                if(sscanf(optarg, "%f", &f) != 1)
                    iError++;
                else
                    m_fOptFloat = f;
                break;

            case 'b':
                m_bOptBool = kTRUE;
                break;

            case '?':
                break;

            default:
                Error("loadArgs", "Unknown option %c\n", optopt);
                iError++;
                break;
        }
    }

    if(iError != 0)
        printInfo(vArg[0]);

    return ((iError < 0) || ((iError > 0) && (m_bExitOnError))) ? -1 : 0;
}

// -----------------------------------------------------------------------------

void HSUExeMacro::printInfo(const Char_t *pProgramName) const
{
// Print info about all standard options. Used when `-h' is in the arguments.
    printf("Use:\n\n%s -i file -o file [-e events] [-p param_file]\n\n"
            "\t-i file - a root file (*.root) or text file with "
                    "list of files\n"
            "\t-o file - output file\n"
            "\t-e num  - number of events to process\n"
            "\t-p param_file - a root file with parameters\n\n",
            pProgramName);
}

// -----------------------------------------------------------------------------

Bool_t HSUExeMacro::openInput(void)
{
// Create the chain from all files given as the arguments of the program
Int_t    iMax = m_pInputArray->GetEntries();
Int_t    i;
TString  rStr;

    if(iMax <= 0)
    {
        Error("openInput", "No input files");
        if(m_bExitOnError)
            gSystem->Exit(-1);

        return kFALSE;
    }

    for(i = 0; i < iMax; i++)
    {
        rStr = ((TObjString *)m_pInputArray->At(i))->GetString();

        if(rStr.EndsWith(".root", TString::kIgnoreCase))
        {
            if( ! addInputFile(rStr.Data()))
                return kFALSE;

            continue;
        }

        if( ! addFilesFromList(rStr.Data()))
            return kFALSE;
    }

    return kTRUE;
}

// -----------------------------------------------------------------------------

Bool_t HSUExeMacro::addInputFile(const Char_t* pStr)
{
// Add a root file to the chain
Int_t iRet;

    if((iRet = m_pChain->Add(pStr, -1)) <= 0)
    {
        Error("addInputFile", "Cannot open file: `%s'", pStr);
        if(m_bExitOnError)
            gSystem->Exit(-1);

        return kFALSE;
    }

    printf("Input file: %-50s %8d evts \n", pStr,
            (int)(m_pChain->GetTreeOffset()[m_pChain->GetNtrees()]
            -m_pChain->GetTreeOffset()[m_pChain->GetNtrees() - 1]));

    return kTRUE;
}

// -----------------------------------------------------------------------------

Bool_t HSUExeMacro::addFilesFromList(const Char_t *pStr)
{
// Add all root files listed in a text file given as the argument.
// All empty lines or lines starting with `#' are skipped.
// File names are trimmed before use
// (all start and tail white characters are removed)
FILE *pFile;
Char_t  s[1000];
Char_t *p1, *p2;

    if((pFile = fopen(pStr, "r")) == NULL)
    {
        Error("addFilesFromList", "Cannot open file: %s", pStr);
        if(m_bExitOnError)
            gSystem->Exit(-1);

        return kFALSE;
    }

    while(fgets(s, 1000, pFile))
    {
        for(p1 = s; isspace(*p1); p1++)
            ;

        for(p2 = p1 + strlen(p1) - 1; (p2 > p1) && (isspace(*p2)); p2--)
            ;

        *(p2 + 1) = '\0';

        if((*p1 == '#') || (p2 <= p1))
            continue;

        if( ! addInputFile(p1))
            return kFALSE;
    }

    fclose(pFile);

    return kTRUE;
}

// -----------------------------------------------------------------------------

Bool_t HSUExeMacro::openOutput(void)
{
// Open the output file
    if(m_sOutputName.Length() <= 0)
    {
        Error("openOutput", "No output file name");

        if(m_bExitOnError)
            gSystem->Exit(-1);

        return kFALSE;
    }

    if(((m_pOutputFile = new TFile(m_sOutputName, "RECREATE")) == NULL)
            || ( ! m_pOutputFile->IsOpen()))
    {
        Error("openOutput", "Cannot open file: %s", m_sOutputName.Data());

        if(m_bExitOnError)
            gSystem->Exit(-1);

        return kFALSE;
    }

    printf("Output file: %s opened\n", m_sOutputName.Data());

    return kTRUE;
}

// -----------------------------------------------------------------------------

Bool_t HSUExeMacro::writeOutput(void)
{
// Simple close the output file. All root objects created since the openOutput
// are written to the file
    if((m_pOutputFile != NULL) && (m_pOutputFile->IsOpen()))
        return m_pOutputFile->Write();

    Error("writeOutput", "Output file not opened");

    return kFALSE;
}

// -----------------------------------------------------------------------------

TClonesArray* HSUExeMacro::getTClonesArray(const Char_t *pBranchName)
{
// Get the TClonesArray from the file.
// To get a proper branch a `.fData' suffix is added to `pBranchName'
HSUBranchElement *pBE;

    if((pBE = findAciveBranch(pBranchName)) == NULL)
        return getNewArrayFromBranch(pBranchName);

    if(pBE->m_eType == HSUBranchElement::kArray)
        return (TClonesArray *)pBE->m_pArray;

    Error("getTClonesArray", "Branch %s has no array", pBranchName);

    if(m_bExitOnError)
        gSystem->Exit(-1);

    return NULL;
}

// -----------------------------------------------------------------------------

HCategory* HSUExeMacro::getHCategory(const Char_t *pCategoryName)
{
// Get the HCategory from the file.
// To get a proper branch a `.' suffix is added to `pCategoryName'
HSUBranchElement *pBE;

    if((pBE = findAciveBranch(pCategoryName)) == NULL)
        return getNewHCategoryFromBranch(pCategoryName);

    if(pBE->m_eType == HSUBranchElement::kCategory)
        return (HCategory *)pBE->m_pArray;

    Error("getTClonesArray", "Branch %s has no HCategory", pCategoryName);

    if(m_bExitOnError)
        gSystem->Exit(-1);

    return NULL;
}

// -----------------------------------------------------------------------------

TBranch* HSUExeMacro::getBranch(const Char_t *pBranchName)
{
// Get the brach from the `T'
// if was activated by getHCategory or getTClonesArray
HSUBranchElement *pBE;

    return ((pBE = findAciveBranch(pBranchName)) == NULL)
            ? NULL : pBE->m_pBranch;
}

// -----------------------------------------------------------------------------

TClonesArray* HSUExeMacro::getNewArrayFromBranch(const Char_t *pBranchName)
{
// Create new branch and a TClonesArray for it
HSUBranchElement *pBE;
TObjString        str(pBranchName);
TBranch          *pBranch;

    pBE = new HSUBranchElement(pBranchName, HSUBranchElement::kArray);

TString strDataBranch = str.GetString() + pBE->getBranchNameSufix();

    if((pBranch = m_pChain->GetBranch(strDataBranch.Data())) == NULL)
    {
        delete pBE;

        Error("getNewArrayFromBranch", "No branch %s in Tree",
                strDataBranch.Data());

        if(m_bExitOnError)
            gSystem->Exit(-1);

        return NULL;
    }

    pBE->m_pBranch = pBranch;
    pBE->m_pArray  = new TClonesArray(pBranchName);

    m_pActiveBranches->Add(pBE);

    m_pChain->SetBranchStatus(strDataBranch.Data(), 1);

    pBE->m_pBranch->SetAddress(&(pBE->m_pArray));

    return (TClonesArray *)pBE->m_pArray;
}

// -----------------------------------------------------------------------------

HCategory* HSUExeMacro::getNewHCategoryFromBranch(const Char_t *pBranchName)
{
// Create new branch and a HCategory for it
HSUBranchElement *pBE;
TObjString        str(pBranchName);
TBranch          *pBranch;
HCategory        *pCat;

    pBE = new HSUBranchElement(pBranchName, HSUBranchElement::kCategory);

TString strDataBranch = str.GetString() + pBE->getBranchNameSufix();

    if((pBranch = m_pChain->GetBranch(strDataBranch.Data())) == NULL)
    {
        delete pBE;

        Error("getNewArrayFromBranch", "No branch %s in Tree",
                strDataBranch.Data());

        if(m_bExitOnError)
            gSystem->Exit(-1);

        return NULL;
    }

TDirectory *pDirSav = gDirectory;
    m_pChain->GetDirectory()->cd();

    if((pCat = (HCategory *)gDirectory->FindObjectAny(pBranchName))
            == NULL)
    {
        pDirSav->cd();

        delete pBE;

        Error("getNewArrayFromBranch", "No HCategory %s in Tree", pBranchName);

        if(m_bExitOnError)
            gSystem->Exit(-1);

        return NULL;
    }

    pDirSav->cd();

    pBE->m_pBranch = pBranch;
    pBE->m_pArray  = pCat->Clone();

    m_pActiveBranches->Add(pBE);

    m_pChain->SetBranchStatus(strDataBranch.Data(), 1);

    pBE->m_pBranch->SetAddress(&(pBE->m_pArray));

    return (HCategory *)pBE->m_pArray;
}

// -----------------------------------------------------------------------------

HSUBranchElement* HSUExeMacro::findAciveBranch(const Char_t *pBranchName)
{
// Find an active branch by name
Int_t             i, iMax;
HSUBranchElement *pBE;

    iMax = m_pActiveBranches->GetEntries();
    for(i = 0; i < iMax; i++)
    {
        if((pBE = (HSUBranchElement *)m_pActiveBranches->At(i)) == NULL)
            continue;

        if(pBE->m_pName->CompareTo(pBranchName) == 0)
            return pBE;
    }

    return NULL;
}

// -----------------------------------------------------------------------------

Int_t HSUExeMacro::loadTree(Int_t iEvent)
{
// Load the event from tree.
// All active categories and array are filled with the new data.
// If switch of the files is needed then new branches are created automatically.
Int_t iE;

    if((iE = m_pChain->LoadTree(iEvent)) < 0)
        return iE;

    if(m_pChain->IsA() != TChain::Class())
        return iE;

    if(m_pChain->GetTreeNumber() != m_iCurrentFile)
    {
        m_iCurrentFile = m_pChain->GetTreeNumber();

        if( ! notifyBranches())
            iE = -1;

        if(m_bNotFileChange)
        {
            Warning("loadTree", "File changed to: %s\n",
                    m_pChain->GetFile()->GetName());
        }
    }

    return iE;
}

// -----------------------------------------------------------------------------

Bool_t HSUExeMacro::notifyBranches(void)
{
// Get branches from a new file in the chain.
// Called by loadTree if needed
Int_t             i, iMax;
HSUBranchElement *pBE;
TString           str;
Bool_t            bReturn = kTRUE;

    iMax = m_pActiveBranches->GetEntries();
    for(i = 0; i < iMax; i++)
    {
        if((pBE = (HSUBranchElement *)m_pActiveBranches->At(i)) == NULL)
            continue;

        str = *pBE->m_pName;
        str += pBE->getBranchNameSufix();

        if((pBE->m_pBranch = m_pChain->GetBranch(str.Data())) == NULL)
        {
            Error("notifyBranches", "No branch `%s'\n\tin file: `%s'",
                    str.Data(), m_pChain->GetFile()->GetName());

            if(m_bExitOnError)
                gSystem->Exit(-1);

            bReturn = kFALSE;
        }

        pBE->m_pBranch->SetAddress(&(pBE->m_pArray));
    }

    return bReturn;
}

// -----------------------------------------------------------------------------

void HSUExeMacro::clear(void)
{
// Clear all arrays and categories before next event
HSUBranchElement *pBE;
Int_t             i, iMax;

    iMax = m_pActiveBranches->GetEntries();
    for(i = 0; i < iMax; i++)
    {
        if((pBE = (HSUBranchElement *)m_pActiveBranches->At(i)) == NULL)
            continue;

        pBE->m_pArray->Clear();
    }
}

// -----------------------------------------------------------------------------

void HSUExeMacro::getBranchesEntry(Int_t iEntry)
{
// Fill the array or category with the data of the current event
HSUBranchElement *pBE;
Int_t             i, iMax;

    iMax = m_pActiveBranches->GetEntries();
    for(i = 0; i < iMax; i++)
    {
        if((pBE = (HSUBranchElement *)m_pActiveBranches->At(i)) != NULL)
            pBE->m_pBranch->GetEntry(iEntry);
    }
}

// -----------------------------------------------------------------------------

Int_t HSUExeMacro::getEvent(Int_t iEvent)
{
// Get all data for the event.
// If no events left return value < 0
Int_t iE;

    clear();
    if((iE = loadTree(iEvent)) >= 0)
        getBranchesEntry(iE);

    return iE;
}

// -----------------------------------------------------------------------------

Bool_t HSUExeMacro::checkBranches(void)
{
// Check if all activated branches exist in the every file in the chain
Int_t  i, iMax;
Bool_t bReturn = kTRUE;

    iMax = m_pChain->GetNtrees();

    for(i = 0; i < iMax; i++)
    {
        if(getEvent(m_pChain->GetTreeOffset()[i]) < 0)
            bReturn = kFALSE;
    }

    m_iCurrentFile = -1;

    return bReturn;
}

// -----------------------------------------------------------------------------

Int_t HSUExeMacro::nextEvent(void)
{
// Get next event
// Return >= 0 if ok, or < 0 when at the end of the events list
    if(m_iEvent < 0)
    {
    Int_t iE;

        if(((iE = (Int_t)m_pChain->GetEntries()) < m_iEvents)
                || (m_iEvents < 0))
        {
            m_iEvents = iE;
        }

        if(m_pProgress != NULL)
            m_pProgress->SetMaxValue(m_iEvents);
    }

    if(++m_iEvent >= m_iEvents)
        return -1;

    if(m_pProgress != NULL)
        m_pProgress->Next();

    if(getEvent(m_iEvent) < 0)
        return -2;

    return m_iEvent;
}

// -----------------------------------------------------------------------------

Bool_t HSUExeMacro::openParamFile(const Char_t *pParamFileName)
{
// Open a file with parameters. The parameters from the file may be get by:
// HSUExeMacro::getParamObject method
    if(m_pParamFile != NULL)
    {
        m_pParamFile->Close();
        m_pParamFile->Delete();
    }

    if(pParamFileName != NULL)
        m_sParamFileName = pParamFileName;

    if(m_sParamFileName.Length() <= 0)
    {
        Error("openParamFile", "No param file name !");

        if(m_bExitOnError)
            gSystem->Exit(-1);

        return kFALSE;
    }

    if(((m_pParamFile = new TFile(m_sParamFileName, "READ")) == NULL)
            || ( ! m_pParamFile->IsOpen()))
    {
        Error("openParamFile", "Cannot open param file: %s",
                m_sParamFileName.Data());

        if(m_pParamFile != NULL)
        {
            m_pParamFile->Close();
            m_pParamFile->Delete();

            m_pParamFile = NULL;
        }

        if(m_bExitOnError)
            gSystem->Exit(-1);

        return kFALSE;
    }

    return kTRUE;
}

// -----------------------------------------------------------------------------

TObject* HSUExeMacro::getParamObject(const Char_t *pName)
{
// Get an object from the parameter file (if opened)
    if(m_pParamFile == NULL)
    {
        Error("getParamObject", "Param file not opened");

        if(m_bExitOnError)
            gSystem->Exit(-1);

        return NULL;
    }

    return m_pParamFile->Get(pName);
}

// -----------------------------------------------------------------------------

const TString& HSUExeMacro::getParamFileName(void) const
{
// Return param file name or empty string if the name not set
    return m_sParamFileName;
}

// -----------------------------------------------------------------------------

Float_t HSUExeMacro::getMdcPhi(Int_t iSector, Float_t fPhiMdc) const
{
// Convert MDC's phi angle to the coordinate system used in other detectors
static Float_t R2D = 180.0 / TMath::Pi();
Float_t fPhi;

    fPhi = R2D * fPhiMdc;

    switch(iSector)
    {
        case 0:
            break;

        case 1:
        case 2:
        case 3:
        case 4:
            fPhi += 60.0f * iSector;
            break;

        default:
            fPhi -= 60.0f;
            break;
    }

    return fPhi;
}

// -----------------------------------------------------------------------------

Float_t HSUExeMacro::getMdcTheta(Float_t fThetaMdc) const
{
// Convert MDC's theta angle to the coordinate system used in other detectors
static Float_t R2D = 180.0 / TMath::Pi();

    return fThetaMdc * R2D;
}

