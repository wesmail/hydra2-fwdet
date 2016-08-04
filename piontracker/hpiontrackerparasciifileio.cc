//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////
//  HPionTrackerParAsciiFileIo
//
//  Class for PionTracker parameter input/output from/into Ascii file
//
////////////////////////////////////////////////////////////////////

#include "hpiontrackerparasciifileio.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hpiontrackerdetector.h"
#include "hparset.h"
#include "hpiontrackertrb3lookup.h"
#include "hpiontrackercalpar.h"
#include "hpiontrackercalrunpar.h"

ClassImp (HPionTrackerParAsciiFileIo)

    HPionTrackerParAsciiFileIo::HPionTrackerParAsciiFileIo (fstream * f) : HDetParAsciiFileIo (f)
{
    // constructor calls the base class constructor
    fName = "HPionTrackerParIo";
}

Bool_t HPionTrackerParAsciiFileIo::init (HParSet * pPar, Int_t * set)
{
    // calls the appropriate read function for the container
    const  Text_t * name = pPar->GetName();

    if (NULL != pFile)
    {
	if (0 == strncmp (name, "PionTrackerTrb3Lookup", strlen ("PionTrackerTrb3Lookup")))
	{
	    return readFile<HPionTrackerTrb3Lookup> ( (HPionTrackerTrb3Lookup *) pPar);
	}

	if (0 == strncmp (name, "PionTrackerCalPar", strlen ("PionTrackerCalPar")))
	{
	    return read ( (HPionTrackerCalPar *) pPar, set);
	}

	Error ("init(HParSet*,Int_t*)",
	       "initialization of %s not possible from  ASCII file!", name);
	return kFALSE;
    }

    Error ("init(HParSet*,Int_t*)", "No input file open");
    return kFALSE;
}

Int_t HPionTrackerParAsciiFileIo::write (HParSet * pPar)
{
    // calls the appropriate write function for the container
    if (NULL != pFile)
    {
	const  Text_t * name = pPar->GetName();

	if (0 == strncmp (name, "PionTrackerTrb3Lookup", strlen ("PionTrackerTrb3Lookup")))
	{
	    return writeFile<HPionTrackerTrb3Lookup> ( (HPionTrackerTrb3Lookup *) pPar);
	}

	if (0 == strncmp (name, "PionTrackerCalPar", strlen ("PionTrackerCalPar")))
	{
	    return writeFile<HPionTrackerCalPar> ( (HPionTrackerCalPar *) pPar);
	}

	if (0 == strncmp(name, "PionTrackerCalRunPar", strlen ("PionTrackerCalRunPar")))
	{
	    return write((HPionTrackerCalRunPar*)pPar);
	}

	Error ("write(HParSet*)", "%s could not be written to ASCII file", name);
	return -1;
    }

    Error ("write(HParSet*)", "No output file open");
    return -1;
}

template<class T> Int_t HPionTrackerParAsciiFileIo::write(T* pPar)
{
   // template function for parameter containers
   // calls the function putAsciiHeader(TString&) of the parameter container,
   // writes the header and calls the function write(fstream&) of the class
   pPar->putAsciiHeader(fHeader);
   writeHeader(pPar->GetName(), pPar->getParamContext());
   pPar->write(*pFile);
   pFile->write(sepLine, strlen(sepLine));
   pPar->setChanged(kFALSE);
   return 1;
}

template<class T> Bool_t HPionTrackerParAsciiFileIo::read (T * pPar, Int_t * set)
{
    // template function for all parameter containers
    // searches the container in the file, reads the data line by line and
    //   called the member function readline(...) of the container class
    const Text_t * name = pPar->GetName();
    HDetector * det = gHades->getSetup()->getDetector ("PionTracker");
    Int_t nSize = det->getMaxModules();

    if (!findContainer (name)) return kFALSE;

    pPar->clear();
    const Int_t maxbuf = 155;
    Text_t buf[maxbuf];

    while (!pFile->eof())
    {
	pFile->getline (buf, maxbuf);

	if (buf[0] == '#') break;

	if (buf[0] != '/' && buf[0] != '\0') pPar->readline (buf, set);
    }

    Bool_t allFound = kTRUE;

    for (Int_t i = 0; i < nSize; i++)
    {
	if (set[i])
	{
	    if (set[i] == 999) set[i] = 1;
	    else allFound = kFALSE;
	}
    }

    if (allFound)
    {
	pPar->setInputVersion (1, inputNumber);
	pPar->setChanged();
	Info ("readFile", "%s initialized from Ascii file", pPar->GetName());
    }

    return allFound;
}
