#ifndef HTRFIOFILE__H
#define HTRFIOFILE__H

#include "TFile.h"
#include "TUrl.h"
#include "TSystem.h"

class HTRFIOFile : public TFile
{
private:
   TUrl      fUrl;        //URL of file

   HTRFIOFile(void) : fUrl("dummy") { }

   // Interface to basic system I/O routines
   Int_t    SysOpen(const Char_t *pathname, Int_t flags, UInt_t mode);
   Int_t    SysClose(Int_t fd);
   Int_t    SysRead(Int_t fd, void *buf, Int_t len);
   Int_t    SysWrite(Int_t fd, const void *buf, Int_t len);
   Long64_t SysSeek(Int_t fd, Long64_t offset, Int_t whence);
   Int_t    SysStat(Int_t fd, Long_t *id, Long64_t *size, Long_t *flags, Long_t *modtime);
   Int_t    SysSync(Int_t) { /* no fsync for RFIO */ return 0; }
   Int_t    SysAccess(const Char_t *filepath, Int_t mode);

	public:
   HTRFIOFile(const Char_t *url,const Option_t *option="", const Char_t *ftitle="", Int_t compress=1);
   ~HTRFIOFile();

   Int_t   GetErrno(void) const;
   void    ResetErrno(void) const;

   ClassDef(HTRFIOFile,0)  //A ROOT file that reads/writes via a rfiod server
};

class HTRFIOSystem : public TSystem {

private:
   void    *fDirp;   // directory handler

   void    *GetDirPtr() const { return fDirp; }

public:
   HTRFIOSystem();
   virtual ~HTRFIOSystem() { }

   Int_t       MakeDirectory(const Char_t *name);
   void       *OpenDirectory(const Char_t *name);
   void        FreeDirectory(void *dirp);
   const Char_t *GetDirEntry(void *dirp);
   Int_t       GetPathInfo(const Char_t *path, FileStat_t &buf);
   Bool_t      AccessPathName(const Char_t *path, EAccessMode mode);

   ClassDef(HTRFIOSystem,0)  // Directory handler for RFIO
};


#endif
