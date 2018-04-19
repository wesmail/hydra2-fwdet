//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : JAM
//*-- Revised : Joern Adamczewski-Musch <j.adamczewski@gsi.de> 2017
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700ThresholdPar
//    Treestyle parameter container with trb3 time thresholds for rich700
//    Defined after example of hrichcalpar JAM Nov2017
//
//////////////////////////////////////////////////////////////////////////////

#include "hdetpario.h"
#include "hpario.h"
#include "hrich700thresholdpar.h"
//#include "hrich700pixelthreshold.h"
#include "richdef.h"

#include <fstream>

using namespace std;

ClassImp(HRich700ThresholdPar)

HRich700ThresholdPar::HRich700ThresholdPar(const Char_t* name,
		const Char_t* title, const Char_t* context) :
		HParSet(name, title, context) {
	strcpy(detName, "Rich");

	fLoc.setNIndex(2);
	fParamsTable.set(2, RICH700_MAX_PMT+1, RICH700_MAX_PMTPIXELS+1);
	fParamsTable.setCellClassName("HRich700PixelThreshold");
	fParamsTable.makeObjTable();

	clear();

	status = kFALSE;
	resetInputVersions();

}

HRich700ThresholdPar::~HRich700ThresholdPar() {
	fParamsTable.deleteTab();
}

void HRich700ThresholdPar::clear() {
// initialize all pixels to 0, flag to -1

// NOTE: first real pmt and pixel object index is 1
// we initialize slots for 0 indices anyway to avoid root streamer trouble...
	HRich700PixelThreshold* pCell = NULL;
	for (Int_t pmt = 0; pmt <= RICH700_MAX_PMT; ++pmt) {
		fLoc[0] = pmt;
		for (Int_t pixel = 0; pixel <= RICH700_MAX_PMTPIXELS; ++pixel) {
			fLoc[1] = pixel;
			pCell = static_cast<HRich700PixelThreshold*>(fParamsTable.getSlot(
					fLoc));
			if (NULL != pCell) {
				pCell = new (pCell) HRich700PixelThreshold;
				pCell->reset();
				pCell->setPMT(pmt);
				pCell->setPixel(pixel);
			} else {
				Error("HRich700ThresholdPar", "Slot not found:  %i %i ",
						fLoc[0], fLoc[1]);
			}
		} //pix
	} //pmt

}

Bool_t HRich700ThresholdPar::init(HParIo* inp, Int_t* set) {
// Initializes the container from an input

	HDetParIo* input = inp->getDetParIo("HRichParIo");
	if (NULL != input) {
		Bool_t returnValue = input->init(this, set);
#if DEBUG_LEVEL > 3
		printParams();
#endif
		return returnValue;
	}
	return kFALSE;
}

Int_t HRich700ThresholdPar::write(HParIo* output) {
// Writes the container to an output

	HDetParIo* out = output->getDetParIo("HRichParIo");
	if (NULL != out) {
		return out->write(this);
	}
	return -1;
}

Bool_t HRich700ThresholdPar::readline(const Char_t * buf) {
// Decodes one line read from ASCII file I/O and fills the pixels
	Bool_t rc = kFALSE;
	Int_t pmt = 0;
	Int_t pix = 0;
	Double_t t_min = 0.0;
	Double_t t_max = 0.0;
	Double_t tot_min = 0.0;
	Double_t tot_max = 0.0;
	Int_t flag = 0;
	Int_t n = sscanf(buf, "%i %i %lf %lf %lf %lf %i", &pmt, &pix, &t_min, &t_max,
			&tot_min, &tot_max, &flag);
	// some debug:
//	printf("HRich700ThresholdPar::readline() sees %d %d %e %e %e %e %d\n",
//			pmt, pix, t_min, t_max, tot_min, tot_max, flag);

	if (7 == n) {
		rc = setPixelThreshold(pmt, pix, t_min, t_max, tot_min, tot_max, flag);
	} else {
		if (n < 4)
			Error("readline", "Not enough values in line %s\n", buf);
		else
			Error("readline", "Too many values in line %s\n", buf);
	}
	return rc;
}

void HRich700ThresholdPar::write(fstream& fout) {
// Writes the Rich Calibration Parameters to an ASCII file.

	HRich700PixelThreshold* pCell = NULL;
	for (Int_t pmt = 1; pmt <= RICH700_MAX_PMT; ++pmt) {
		fLoc[0] = pmt;
		for (Int_t pixel = 1; pixel <= RICH700_MAX_PMTPIXELS; ++pixel) {
			fLoc[1] = pixel;
			pCell = static_cast<HRich700PixelThreshold*>(fParamsTable.getSlot(
					fLoc));
			if (NULL != pCell) {
				if (-1 != pCell->getFlag()) {
					fout.width(4); // pmt
					fout << left << pCell->getPMT();
					fout.width(4); // pix
					fout << left << pCell->getPixel();
					fout.width(10); // tmin
					fout << left << pCell->getT_Min();
					fout.width(10); // tmax
					fout << left << pCell->getT_Max();
					fout.width(10); // totmin
					fout << left << pCell->getTot_Min();
					fout.width(10); // totmax
					fout << left << pCell->getTot_Max();
					fout.width(4); // flag
					fout << left << pCell->getFlag();
					fout << endl;
				} // flag -1, is non initialized slot

			} else {
				Error("HRich700ThresholdPar::write ", "Slot not found:  %i %i ",
						fLoc[0], fLoc[1]);
			}
		} //pix
	} //pmt

}

Bool_t HRich700ThresholdPar::setPixelThreshold(Int_t pmt, Int_t pix,
		Double_t tmin, Double_t tmax, Double_t totmin, Double_t totmax,
		Short_t flag) {
	// IMPORTANT NOTE: first used object index in table is 1
	if ((pmt <= 0) || (pmt > RICH700_MAX_PMT) || (pix <= 0)
			|| (pix > RICH700_MAX_PMTPIXELS)) {
		Error("setPixelThreshold", "Wrong pixel coordinates");
		return kFALSE;
	}

	HRich700PixelThreshold* pCell = NULL;

	fLoc[0] = pmt;
	fLoc[1] = pix;

	pCell = static_cast<HRich700PixelThreshold*>(fParamsTable.getSlot(fLoc));
	if (NULL != pCell) {
		pCell->setParams(tmin, tmax, totmin, totmax, flag);
	} else {
		Error("HRich700ThresholdPar::setPixelThreshold", "Cell not found.");
		return kFALSE;
	}
	return kTRUE;
}

void HRich700ThresholdPar::printParams() {
// Prints the lookup table

	HRich700PixelThreshold* pCell = NULL;
	for (Int_t pmt = 1; pmt <= RICH700_MAX_PMT; ++pmt) {
		fLoc[0] = pmt;
		for (Int_t pixel = 1; pixel <= RICH700_MAX_PMTPIXELS; ++pixel) {
			fLoc[1] = pixel;
			pCell = static_cast<HRich700PixelThreshold*>(fParamsTable.getSlot(
					fLoc));
			if (NULL != pCell) {
				if (-1 != pCell->getFlag()) {
					std::cout.width(4); // pmt
					std::cout << left << pCell->getPMT(); // real pmt object index, not array index!
					std::cout.width(4); // pix
					std::cout << left << pCell->getPixel(); // real pmt pixel index, not array index!
					std::cout.width(10); // tmin
					std::cout << left << pCell->getT_Min();
					std::cout.width(10); // tmax
					std::cout << left << pCell->getT_Max();
					std::cout.width(10); // totmin
					std::cout << left << pCell->getTot_Min();
					std::cout.width(10); // totmax
					std::cout << left << pCell->getTot_Max();
					std::cout.width(4); // flag
					std::cout << left << pCell->getFlag();
					std::cout << endl;
				} // flag -1, is non initialized slot

			} else {
				Error("HRich700ThresholdPar::write ", "Slot not found:  %i %i ",
						fLoc[0], fLoc[1]);
			}
		} //pix
	} //pmt

	cout
			<< "#########################################################################"
			<< endl;

}

void HRich700ThresholdPar::putAsciiHeader(TString & header) {
	// puts the ASCII header to the string used in HDetParAsciiFileIo
	header =
			"# Rich700 trb3 unpacker time threshold parameters\n"
			"# Format:\n"
			"# PMT \tPixel \tT_min (ns) \tT_max (ns) \tTot_min (ns) \tTot_max (ns) \tFlag (0=OK, 1=suppress pixel)\n";
}

HRich700PixelThreshold* HRich700ThresholdPar::getSlot(HLocation &loc) {
	return static_cast<HRich700PixelThreshold*>(fParamsTable.getSlot(loc));
}

HRich700PixelThreshold* HRich700ThresholdPar::getObject(HLocation &loc) {
	return static_cast<HRich700PixelThreshold*>(fParamsTable.getObject(loc));
}

