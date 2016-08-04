//////////////////////////////////////////////////////
//  HRfioFile
//
//  Class for reading events from files on taperobot
//
//////////////////////////////////////////////////////

#ifndef HRFIOFILE_H
#define HRFIOFILE_H

#include "hrfiofiletsm.h"
#include "TObject.h"
#include "TString.h"
#include <iostream>

class HRFIOFile : public TObject
{
 protected:
    HRFIOFileTSM *file;  //! filepointer
    Int_t status;    //! status variable
    streampos pos; //! position in file
    TString *fname;   //! name of file
    Int_t fmode;     //! mode of file open

 public:
    HRFIOFile(const Char_t *name, Int_t mode = ios::in);
    ~HRFIOFile(void);

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
    HRFIOFile& operator>>(HRFIOFile& u);
    HRFIOFile& operator<<(const HRFIOFile& u);
    ClassDef(HRFIOFile,0) // RFIO file interface
	};

#endif
