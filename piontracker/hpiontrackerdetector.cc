//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////
//
//  HPionTrackerDetector
//
//  Class for detector "PionTracker"
//  stores the setup in a linear array (size: maximum number
//  of modules defined in the hdef.h definition file)
//
//  Always two modules belong to the same detector
//  (even module number: front side, odd module number: back side)
//
//////////////////////////////////////////////////////////////////

#include "hpiontrackerdetector.h"
#include "hpiontrackerdef.h"
#include "hades.h"
#include "hcategory.h"
#include "hevent.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparrootfileio.h"
#include "hpiontrackerparrootfileio.h"
#include "hparasciifileio.h"
#include "hpiontrackerparasciifileio.h"
#include "TClass.h"

using namespace std;

ClassImp (HPionTrackerDetector)

HPionTrackerDetector::HPionTrackerDetector()
{
	// constructor
	fName         = "PionTracker";
	maxModules    = PIONTRACKER_MAX_MODULES;
	maxComponents = PIONTRACKER_MAX_COMPONENTS;
	modules       = new TArrayI (maxModules);

	for (Int_t i = 0; i < maxModules; i++)
	{
		modules->AddAt (0, i);
	}
}

HPionTrackerDetector::~HPionTrackerDetector()
{
	// destructor
	if (NULL != modules)
	{
		delete modules;
		modules = NULL;
	}
}

Bool_t HPionTrackerDetector::init (void)
{
	// nothing to be done
	return kTRUE;
}

void HPionTrackerDetector::activateParIo (HParIo * io)
{
	// activates the input/output class for the parameters
	// needed by the PionTracker
	if (strcmp (io->IsA()->GetName(), "HParOraIo") == 0)
	{
		io->setDetParIo ("HPionTrackerParIo");
		return;
	}

	if (strcmp (io->IsA()->GetName(), "HParRootFileIo") == 0)
	{
		HPionTrackerParRootFileIo * p = new HPionTrackerParRootFileIo ( ( (HParRootFileIo *) io)->getParRootFile());
		io->setDetParIo (p);
	}

	if (strcmp (io->IsA()->GetName(), "HParAsciiFileIo") == 0)
	{
		HPionTrackerParAsciiFileIo * p = new HPionTrackerParAsciiFileIo ( ( (HParAsciiFileIo *) io)->getFile());
		io->setDetParIo (p);
	}
}


Bool_t HPionTrackerDetector::write (HParIo * output)
{
	// writes the PionTracker setup to output
	HDetParIo * out = output->getDetParIo ("HPionTrackerParIo");

	if (out) return out->write (this);

	return kFALSE;
}


HCategory * HPionTrackerDetector::buildMatrixCategory (const Text_t * className, Float_t fillRate)
{
	Int_t maxMod = getMaxModInSetup();

	if (maxMod == 0) return 0;

	Int_t * sizes = new Int_t[2]; // 2 levels
	sizes[0] = maxMod;
	sizes[1] = maxComponents;
	HMatrixCategory * category = new HMatrixCategory (className, 2, sizes, fillRate);
	delete [] sizes;
	return category;
}

HCategory * HPionTrackerDetector::buildLinearCategory (const Text_t * className)
{
	Int_t size = getMaxModInSetup() * maxComponents;

	if (size)
	{
		HLinearCategory * category = new HLinearCategory (className, size);
		return category;
	}

	return 0;
}

HCategory * HPionTrackerDetector::buildCategory (Cat_t cat)
{
	// gets the category if existing
	// builts and adds if not existing
	// returns the pointer to the category or zero
	HCategory * pcat;
	pcat = gHades->getCurrentEvent()->getCategory (cat);

	if (pcat) return (pcat); // already existing

	switch (cat)
	{
		case catPionTrackerRaw:
			pcat = buildMatrixCategory("HPionTrackerRaw", 0.5);
			break;

		case catPionTrackerCal:
			pcat = buildMatrixCategory("HPionTrackerCal", 0.5);
			break;

		case catPionTrackerHit:
			pcat = buildMatrixCategory("HPionTrackerHit", 0.5);
			break;

		case catPionTrackerTrack:
			pcat = buildLinearCategory("HPionTrackerTrack");
			break;

		default:
			return NULL;
	}

	if (pcat) gHades->getCurrentEvent()->addCategory (cat, pcat, "PionTracker");

	return (pcat);
}

Int_t HPionTrackerDetector::getMaxModInSetup (void)
{
	// returns the maximum number of modules in the actual setup
	Int_t maxMod = -1;

	for (Int_t i = 0; i < maxModules; i++)
	{
		if (modules->At (i)) maxMod = (i > maxMod) ? i : maxMod;
	}

	maxMod++;
	return maxMod;
}
