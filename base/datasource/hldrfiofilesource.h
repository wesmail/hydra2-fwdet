//////////////////////////////////////////////////////
//  HldRfioFileSource
//
//  Datasource for reading events from files on taperobot
//
//////////////////////////////////////////////////////
#ifndef HLDRFIOFILESOURCE_H
#define HLDRFIOFILESOURCE_H

#include "hldfilesource.h"

class HldRFIOFileSource : public HldFileSource
{
 protected:
		TString fCurrentArchive;  //! name of archive on taperobot
		TString fCurrentRobot;    //! name of robot
		
 public:
		HldRFIOFileSource(void);
		
		Bool_t rewind();
		Bool_t getNextFile(void);
		EDsState getNextEvent(Bool_t doUnpack=kTRUE);
		Int_t getRunId(const Text_t *fileName);
		void setArchive(const Char_t *archiveName){fCurrentArchive = archiveName;};
		void setRobot(const Char_t *robotName);
		virtual void addFile(const Text_t *fileName, Int_t refId=-1);
		
		ClassDef(HldRFIOFileSource,0) // Datasource to read HLD files from taperobot
};

#endif
