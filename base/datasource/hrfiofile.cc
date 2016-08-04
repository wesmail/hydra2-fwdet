//*-- Author : J.Wuestenfeld
//*-- Modified : 11/19/2003 by J.Wuestenfeld

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////
//HRfioFile
//
//  Class used for reading HLD events from file on taperobot
//  Instantiates real access depending on robot (adsm or tsm)
//
/////////////////////////////////////////////////////

using namespace std;

#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "hrfiofile.h"
#include "hrfiofiletsm.h"

ClassImp(HRFIOFile)

  HRFIOFile::HRFIOFile(const Char_t *name, Int_t mode)
{
  // Default constructor, open file

  fname = new TString(name);
  fmode = mode;
  status = ios::badbit;
  pos = 0;
  if(fname->BeginsWith("lxgstore"))
	{
    file = new HRFIOFileTSM(name,mode);
  	file->open(name,mode);
	}
}

HRFIOFile::~HRFIOFile(void)
{
  // Default destructor
  if(close())
    cout << "Error closing file" << endl;
  if(fname)
    delete fname;
}

Int_t HRFIOFile::open(const Char_t *name, Int_t mode)
{
  // open file on taperobot with name and mode. Tests first if file is accessible.
  // Return 0 on success, -1 otherwise.
  if(file)
    return file->open(name,mode);
  else
    return -1;
}

Int_t HRFIOFile::close(void)
{
  // Closes file. Return 0 if closed, -1 if no file opened.
  if(file)
    return file->close();
  else
    return -1;
}

Int_t HRFIOFile::access(const Char_t *name, Int_t mode)
{
  // Tests for accessibility of file on taperobot.
  // Returns 0 on success, -1 in case of failure.
  // If mode is set to 0 returns 1.
  TString *tmp;

  tmp = new TString(name);
  if(tmp->BeginsWith("lxgstore"))
    {
      delete tmp;
      return HRFIOFileTSM::access(name,mode);
    }
  else
    delete tmp;
  return -1;
}

Int_t HRFIOFile::read(Char_t *buffer, Int_t len)
{
  // Reads a buffer with length len from the file
  if(file)
    return file->read(buffer,len);
  else
    return -1;
}

Int_t HRFIOFile::write(const Char_t *buffer, Int_t len)
{
  // Writes buffer to file. NOT TESTED!!
  if(file)
    return file->write(buffer,len);
  else
    return -1;
}

HRFIOFileTSM& HRFIOFile::seekp(streampos n)
{
  // Moves accesspointer in file relative to current position.
  return seekg(n,(ios::seekdir)ios::cur);
}

HRFIOFileTSM& HRFIOFile::seekg(streampos offs, Int_t p)
{
  // Moves accesspointer in file relative to position p.
  // p can be one of : ios::beg, ios::cur, ios::end.
  return file->seekg(offs,p);
}

streampos HRFIOFile::tellg(void)
{
  // Returns current position in file.
  if(file)
    return file->tellg();
  else
    return -1;
}

void HRFIOFile::ignore(Int_t max, Int_t term)
{
  // Ignore max bytes in file. Moves access pointer to new position
  seekp(max);
}

Bool_t HRFIOFile::good(void)
{
  // Returns kTRUE, if lase operation was successfull, kFALSE otherwise.
  if(file)
    return file->good();
  else return kFALSE;
}

Bool_t HRFIOFile::eof(void)
{
  // Returns kTRUE if accesspointer has reached the end of file marker, kFALSE otherwise.
  if(file)
    return file->eof();
  else
    return -1;
}

Bool_t HRFIOFile::fail(void)
{
  // Returns kTRUE if last operation failed, kFALSE otherwise.
  // File unusable!
  return (status & ios::failbit) ? kTRUE:kFALSE;
}

Bool_t HRFIOFile::bad(void)
{
  // Returns kTRUE, if last operation failed, but error can be repaired.
  if(file)
    return file->bad();
  else
    return -1;
}

Int_t HRFIOFile::rdstat(void)
{
  // Returns status.
  if(file)
    return file->rdstat();
  else
    return -1;
}

HRFIOFile& HRFIOFile::operator>>(HRFIOFile& u)
{
  // Operator to write to the file. NOT IMPLEMENTED!
  return *this;
}

HRFIOFile& HRFIOFile::operator<<( const HRFIOFile& u)
{
  // Operator to read from the file. NOT IMPLEMENTED!
  return *this;
}
