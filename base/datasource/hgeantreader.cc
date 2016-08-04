#pragma implementation
#include "hgeantreader.h"

//*-- Author : Manuel Sanchez
//*-- Modified : 18/06/98 by Manuel Sanchez
//*-- Modified : 26/02/99 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////
//HGeantReader
//
// Base class for the different GEANT readers, a "GEANT reader" is an object
//responsible of reading GEANT output data into the event structure. These data
//are organized in ntuples (TTrees in Root notation) which can be in different files,
//each reader takes data from a set of these ntuples and puts them into the event
//structure.
///////////////////////////

HGeantReader::HGeantReader(void) {
  //Default constructor
  fInputFile=NULL;
}

HGeantReader::~HGeantReader(void) {
  //Destructor
}

void HGeantReader::setInput(TFile *file) {
  //Sets the file where data are read from to "file"
  fInputFile=file;
}

Bool_t HGeantReader::execute(void) {
  //This method should be overriden by the different GEANT readers, here
  //is where each reader takes the data from the input file and puts those
  //data into the event structure.
  //
  //This method returns kTRUE if no problem has been found or kFALSE if it
  //has been unable to read the data.
  return kTRUE;
}

Bool_t HGeantReader::init(void) {
  // This method should be overriden by the different GEANT readers.
  // It is called before the event loop and can contain all functionality
  // which can be done once in advance.  
  return kTRUE;
}

ClassImp(HGeantReader)
