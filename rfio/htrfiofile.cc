//*-- Author : J.Wuestenfeld
//*-- Modified : 05/06/2006 by J.Wuestenfeld

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////
//HTRfioFile
//
//  Class used for reading ROOT file on taperobot
//
/////////////////////////////////////////////////////

#include "htrfiofile.h"
#include "TROOT.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <dirent.h>
#include "rawcommn.h"
#include "rawclin.h"

ClassImp(HTRFIOFile)
ClassImp(HTRFIOSystem)

#ifndef RFIO_READOPT
#define RFIO_READOPT 1
#endif

#define RFILE srawAPIFile

extern "C" {
   Int_t   rfio_open(const Char_t *filepath, Int_t flags, Int_t mode);
   Int_t   rfio_close(Int_t s);
   Int_t   rfio_read(Int_t s, void *ptr, Int_t size);
   Int_t   rfio_write(Int_t s, const void *ptr, Int_t size);
   Int_t   rfio_lseek(Int_t s, Int_t offset, Int_t how);
   Int_t   rfio_access(const Char_t *filepath, Int_t mode);
   Int_t   rfio_unlink(const Char_t *filepath);
   Int_t   rfio_parse(const Char_t *name, Char_t **host, Char_t **path);
   Int_t   rfio_stat(const Char_t *path, struct stat *statbuf);
   Int_t   rfio_fstat(Int_t s, struct stat *statbuf);
   void  rfio_perror(const Char_t *msg);
   Char_t *rfio_serror();
   Int_t   rfiosetopt(Int_t opt, Int_t *pval, Int_t len);
   Int_t   rfio_mkdir(const Char_t *path, Int_t mode);
   void *rfio_opendir(const Char_t *dirpath);
   Int_t   rfio_closedir(void *dirp);
   void *rfio_readdir(void *dirp);
   FILE *rfio_fopen(Char_t *pcFile, Char_t *pcOptions);
   Int_t rfio_fclose(RFILE *pRemFile);
#ifdef R__WIN32
   Int_t  *C__serrno(void);
   Int_t  *C__rfio_errno (void);
#endif
};

#ifdef R__WIN32

// Thread safe rfio_errno. Note, C__rfio_errno is defined in Cglobals.c rather
// than rfio/error.c.
#define rfio_errno (*C__rfio_errno())

// Thread safe serrno. Note, C__serrno is defined in Cglobals.c rather
// rather than serror.c.
#define serrno (*C__serrno())

#else

extern Int_t rfio_errno;
extern Int_t serrno;

#endif

HTRFIOFile::HTRFIOFile(const Char_t *url,const Option_t *option, const Char_t *ftitle, Int_t compress)
  : TFile(url, "NET", ftitle, compress), fUrl(url)
{
  // Create a RFIO file object. A RFIO file is the same as a TFile
  // except that it is being accessed via a rfiod server. The url
  // argument must be of the form: rfio:/path/file.root (where file.root
  // is a symlink of type /shift/aaa/bbb/ccc) or rfio:server:/path/file.root.
  // If the file specified in the URL does not exist, is not accessable
  // or can not be created the kZombie bit will be set in the TRFIOFile
  // object. Use IsZombie() to see if the file is accessable.
  // For a description of the option and other arguments see the TFile ctor.
  // The preferred interface to this constructor is via TFile::Open().
  //
  // The option "RECREATE" is not supported by the GSI gStore system.

  fOption = option;
  fOption.ToUpper();

  // tell RFIO to not read large buffers, ROOT does own buffering
  Int_t readopt = 0;
  ::rfiosetopt(RFIO_READOPT, &readopt, 4);
  if (fOption == "NEW")
    fOption = "CREATE";

  Bool_t create   = (fOption == "CREATE") ? kTRUE : kFALSE;
  Bool_t recreate = (fOption == "RECREATE") ? kTRUE : kFALSE;
  Bool_t update   = (fOption == "UPDATE") ? kTRUE : kFALSE;
  Bool_t read     = (fOption == "READ") ? kTRUE : kFALSE;
  if (!create && !recreate && !update && !read) {
    read    = kTRUE;
    fOption = "READ";
  }

  TString stmp;
  Char_t *fname;
  if ((fname = gSystem->ExpandPathName(fUrl.GetFile()))) {
    if (!strstr(fname, ":/")) {
      Char_t *host;
      Char_t *name;
     if (::rfio_parse(fname, &host, &name))
        stmp = Form("%s:%s", host, name);
      else
        stmp = fname;
    }
		else
      stmp = fname;
    delete [] fname;
    fname = (Char_t *)stmp.Data();
  } else {
    Error("HTRFIOFile", "error expanding path %s", fUrl.GetFile());
    goto zombie;
  }

  if (recreate) {
    if (SysAccess(fname, kFileExists) == 0)
      ::rfio_unlink(fname);
    recreate = kFALSE;
    create   = kTRUE;
    fOption  = "CREATE";
  }
  if (create && SysAccess(fname, kFileExists) == 0) {
    Error("HTRFIOFile", "file %s already exists", fname);
    goto zombie;
  }
  if (update) {
    if (SysAccess(fname, kFileExists) != 0) {
      update = kFALSE;
      create = kTRUE;
    }
    if (update && SysAccess(fname, kWritePermission) != 0) {
      Error("HTRFIOFile", "no write permission, could not open file %s", fname
           );
      goto zombie;
    }
  }
  if (read) {
    if (SysAccess(fname, kFileExists) != 0) {
      Error("HTRFIOFile", "file %s does not exist", fname);
      goto zombie;
    }
    if (SysAccess(fname, kReadPermission) != 0) {
      Error("HTRFIOFile", "no read permission, could not open file %s", fname)
          ;
      goto zombie;
    }
  }

   // Connect to file system stream
  if (create || update) {
#ifndef WIN32
      fD = SysOpen(fname, O_RDWR | O_CREAT, 0644);
#else
      fD = SysOpen(fname, O_RDWR | O_CREAT | O_BINARY, S_IREAD | S_IWRITE);
#endif
      if (fD == -1) {
  SysError("HTRFIOFile", "file %s can not be opened", fname);
  goto zombie;
      }
      fWritable = kTRUE;
  } else {
#ifndef WIN32
      fD = SysOpen(fname, O_RDONLY, 0644);
#else
      fD = SysOpen(fname, O_RDONLY | O_BINARY, S_IREAD | S_IWRITE);
#endif
      if (fD == -1) {
  SysError("HTRFIOFile", "file %s can not be opened for reading", fname);
  goto zombie;
      }
      fWritable = kFALSE;
  }
  UseCache(0);

  Init(create);

  return;

zombie:
   // error in file opening occured, make this object a zombie
    MakeZombie();
   gDirectory = gROOT;
}

HTRFIOFile::~HTRFIOFile()
{
  // RFIO file dtor. Close and flush directory structure.

  Close();

}

Int_t HTRFIOFile::SysOpen(const Char_t *pathname, Int_t flags, UInt_t mode)
{
  // Interface to system open. All arguments like in POSIX open.

  Int_t ret = ::rfio_open((Char_t *)pathname, flags, (Int_t) mode);
  if (ret < 0)
    gSystem->SetErrorStr(::rfio_serror());
  return ret;
}

Int_t HTRFIOFile::SysClose(Int_t fd)
{
  // Interface to system close. All arguments like in POSIX close.

  Int_t ret = ::rfio_close(fd);
  if (ret < 0)
    gSystem->SetErrorStr(::rfio_serror());
  return ret;
}

Int_t HTRFIOFile::SysRead(Int_t fd, void *buf, Int_t len)
{
  // Interface to system read. All arguments like in POSIX read.

  fOffset += len;
  Int_t ret = ::rfio_read(fd, (Char_t *)buf, len);
  if (ret < 0)
    gSystem->SetErrorStr(::rfio_serror());
  return ret;
}

Int_t HTRFIOFile::SysWrite(Int_t fd, const void *buf, Int_t len)
{
  // Interface to system write. All arguments like in POSIX write.

  fOffset += len;
  //cout << "fd: " << fd << " ,fOffset: " << fOffset << endl;

  Int_t ret = ::rfio_write(fd, (Char_t *)buf, len);
  if (ret < 0)
    gSystem->SetErrorStr(::rfio_serror());
  return ret;

}

Long64_t HTRFIOFile::SysSeek(Int_t fd, Long64_t offset, Int_t whence)
{
  // Interface to system lseek. All arguments like in POSIX lseek
  // except that the offset and return value are Long_t to be able to
  // handle 64 bit file systems.

  if (whence == SEEK_SET && offset == fOffset) return offset;

	//cout << "fd: " << fd << " ,offset: " << offset << endl;

  Long64_t ret = ::rfio_lseek(fd, offset, whence);

  if (ret < 0)
    gSystem->SetErrorStr(::rfio_serror());
  else
    fOffset = ret;

  return ret;
}

Int_t HTRFIOFile::SysStat(Int_t fd, Long_t *id, Long64_t *size, Long_t *flags, Long_t *modtime)
{
   // Interface to TSystem:GetPathInfo(). Generally implemented via
   // stat() or fstat().

  struct stat statbuf;

  if (::rfio_fstat(fd, &statbuf) >= 0) {
    if (id)
      *id = (statbuf.st_dev << 24) + statbuf.st_ino;
    if (size)
      *size = statbuf.st_size;
    if (modtime)
      *modtime = statbuf.st_mtime;
    if (flags) {
      *flags = 0;
      if (statbuf.st_mode & ((S_IEXEC)|(S_IEXEC>>3)|(S_IEXEC>>6)))
        *flags |= 1;
      if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
        *flags |= 2;
      if ((statbuf.st_mode & S_IFMT) != S_IFREG &&
           (statbuf.st_mode & S_IFMT) != S_IFDIR)
        *flags |= 4;
    }
    return 0;
  }

  gSystem->SetErrorStr(::rfio_serror());
  return 1;
}

Int_t HTRFIOFile::SysAccess(const Char_t *filepath, Int_t mode)
{
	srawAPIFile *fRemote = NULL;
 Int_t res=0;


	fRemote = (srawAPIFile *)rfio_fopen((Char_t*)filepath, "r");
	if (fRemote)
    {
      res = rfio_fclose(fRemote);
      if(res!=0)
      {
        return 0;
      }
      else
      {
				gSystem->SetErrorStr(::rfio_serror());
        return 0;
      }
    }
    else
    {
      return 1;
    }
 }

Int_t HTRFIOFile::GetErrno(void) const
{
  // Method returning rfio_errno. For RFIO files must use this
  // function since we need to check rfio_errno then serrno and finally errno.

  if (rfio_errno)
    return rfio_errno;
  if (serrno)
    return serrno;
  return TSystem::GetErrno();

}

void HTRFIOFile::ResetErrno(void) const
{
  // Method resetting the rfio_errno, serrno and errno.

  rfio_errno = 0;
  serrno = 0;
  TSystem::ResetErrno();

}

//______________________________________________________________________________
HTRFIOSystem::HTRFIOSystem() : TSystem("-rfio", "RFIO Helper System")
{
   // Create helper class that allows directory access via rfiod.

   // name must start with '-' to bypass the TSystem singleton check
   SetName("rfio");

   fDirp = 0;
}

//______________________________________________________________________________
Int_t HTRFIOSystem::MakeDirectory(const Char_t *dir)
{
   // Make a directory via rfiod.

   TUrl url(dir);

   Int_t ret = ::rfio_mkdir(url.GetFile(), 0755);
   if (ret < 0)
      gSystem->SetErrorStr(::rfio_serror());
   return ret;
}

//______________________________________________________________________________
void *HTRFIOSystem::OpenDirectory(const Char_t *dir)
{
   // Open a directory via rfiod. Returns an opaque pointer to a dir
   // structure. Returns 0 in case of error.

   if (fDirp) {
      Error("OpenDirectory", "invalid directory pointer (should never happen)");
      fDirp = 0;
   }

   TUrl url(dir);

   struct stat finfo;

   if (::rfio_stat(url.GetFile(), &finfo) < 0)
      return 0;

   if ((finfo.st_mode & S_IFMT) != S_IFDIR)
      return 0;

   fDirp = (void*) ::rfio_opendir(url.GetFile());

   if (!fDirp)
      gSystem->SetErrorStr(::rfio_serror());

   return fDirp;
}

//______________________________________________________________________________
void HTRFIOSystem::FreeDirectory(void *dirp)
{
   // Free directory via rfiod.

   if (dirp != fDirp) {
      Error("FreeDirectory", "invalid directory pointer (should never happen)");
      return;
   }

   if (dirp)
      ::rfio_closedir(dirp);

   fDirp = 0;
}

//______________________________________________________________________________
const Char_t *HTRFIOSystem::GetDirEntry(void *dirp)
{
   // Get directory entry via rfiod. Returns 0 in case no more entries.

   if (dirp != fDirp) {
      Error("GetDirEntry", "invalid directory pointer (should never happen)");
      return 0;
   }

   struct dirent *dp;

   if (dirp) {
      dp = (struct dirent *) ::rfio_readdir(dirp);
      if (!dp)
         return 0;
      return dp->d_name;
   }
   return 0;
}

//______________________________________________________________________________
Int_t HTRFIOSystem::GetPathInfo(const Char_t *path, FileStat_t &buf)
{
   // Get info about a file. Info is returned in the form of a FileStat_t
   // structure (see TSystem.h).
   // The function returns 0 in case of success and 1 if the file could
   // not be stat'ed.

   TUrl url(path);

   struct stat sbuf;

   if (path && ::rfio_stat(url.GetFile(), &sbuf) >= 0) {

      buf.fDev    = sbuf.st_dev;
      buf.fIno    = sbuf.st_ino;
      buf.fMode   = sbuf.st_mode;
      buf.fUid    = sbuf.st_uid;
      buf.fGid    = sbuf.st_gid;
      buf.fSize   = sbuf.st_size;
      buf.fMtime  = sbuf.st_mtime;
      buf.fIsLink = kFALSE;

      return 0;
   }
   return 1;
}

//______________________________________________________________________________
Bool_t HTRFIOSystem::AccessPathName(const Char_t *path, EAccessMode mode)
{
   // Returns FALSE if one can access a file using the specified access mode.
   // Mode is the same as for the Unix access(2) function.
   // Attention, bizarre convention of return value!!

	srawAPIFile *fRemote = NULL;
 Int_t res=0;

   TUrl url(path);

	fRemote = (srawAPIFile *)rfio_fopen((Char_t*)url.GetFile(), "r");
	if (fRemote)
    {
      res = rfio_fclose(fRemote);
      if(res!=0)
      {
        return kFALSE;
      }
      else
      {
				gSystem->SetErrorStr(::rfio_serror());
        return kFALSE;
      }
    }
    else
    {
      return kTRUE;
    }
  }

