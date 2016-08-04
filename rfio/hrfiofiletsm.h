//////////////////////////////////////////////////////
//  HRfioFile
//
//  Class for reading events from files on taperobot.
//
//////////////////////////////////////////////////////

#ifndef HRFIOFILETSM_H
#define HRFIOFILETSM_H

#include "TObject.h"
#include "rawapin.h"

#include <iostream>
using namespace std;

class HRFIOTsmSignalHandler;

class HRFIOFileTSM : public TObject
{
 protected:
		RFILE *file;   //! pointer to file in taperobot
	 Int_t status;    //! status variable
		streampos pos; //! position in file
	 Char_t *fname;   //! name of file
	 Int_t fmode;     //! mode of file open
        HRFIOTsmSignalHandler *handler;

 public:
		HRFIOFileTSM(const Char_t *name="", Int_t mode = ios::in);
		~HRFIOFileTSM(void);

	 Int_t open(const Char_t *name, Int_t mode = ios::in);
	 Int_t close(void);
		static Int_t access(const Char_t *name, Int_t mode = 0);
	 Int_t read(Char_t *buffer, Int_t len);
	 Int_t write(const Char_t *buffer, Int_t len);
		HRFIOFileTSM& seekp(streampos n);
		HRFIOFileTSM& seekg(streampos offs, Int_t p);
		streampos tellg(void);
		void ignore(Int_t max=1, Int_t term = EOF);
		Bool_t good(void);
		Bool_t eof(void);
		Bool_t fail(void);
		Bool_t bad(void);
	 Int_t rdstat(void);
	 Int_t stat(Long_t *id, Long_t *size, Long_t *flags, Long_t *modtime);
	 Char_t *serror(void);
		HRFIOFileTSM& operator>>(HRFIOFileTSM& u);
		HRFIOFileTSM& operator<<(const HRFIOFileTSM& u);
		static Int_t unlink(Char_t *name);
		ClassDef(HRFIOFileTSM,0) // RFIO file accessing the TSM robot
};

#endif
