#include "hzip.h"


//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
// HZip
//
// A helper class to read/work with zip file containing
// many root files. Those files can be typically produced
// by :
//    zip -j -n root myzipname myrootfiles
//
// Note: root files will not be compressed, directory names
// ignored. It's a flat files structure. Purpose of the
// ziping of many root files into on zip archive is
// to improve the handling of many small files and reduce
// the load on the file system.
//
// To make the daily work more easy a command line executable hzip
// is provided to produce and work with those zip files:
//
// usage: hzip -o zipfile [-i filefilter] [-f filelist] [-u outputdir] [-msth]
//	          -f input ascii filelist (1 file per line)
//                -h help
//                -i input filefilter (like "be*.root")
//                -l list file in zip files
//	          -m maxsize of file [bytes] (default = 2 Gbyte, will be splitted if larger)
//	          -o outputzip file name (required)
//                -s save mode. do not overide existing zip files (default is overwrite)
//	          -t test. show what would be done
//	          -u dir unzip zip files to dir
//                -w print in which file membername is contained
//	           examples:
//                    test zip root files      : hzip -t -o test.zip -i "/mydir/be*.root"
//	              zip root files           : hzip -o test.zip -i "/mydir/be*.root"
//	              zip root files from list : hzip -o test.zip -f filelist
//	              unzip root files to dir  : hzip -i "test_*.zip" -u /mydir
//	              list files in zip files  : hzip -i "test_*.zip" -l
//
// from the normal terminal.
// HZip provides the functionality to access, list and files from
// a root macro.
//
//--------------------------------------------------------------------------
// examples:
//
//  TChain* chain =  new TChain("myTree");
//  HZip::makeChain("my.zip",chain);            // add all root files to chain
//  HZip::makeChainGlob("my*.zip",chain);       // add all root files of all matching zip files to chain
//  HZip::makeChainList("filelist.txt",chain);  // add all root files of all zip files in filelist to chain
//
//  chain->GetEntries(); // access all files and get number of entries
//  chain->ls();         // list all files in chain with number of entries
//
//  Bool_t HZip::isInside("my.zip","my.root"); // is my.root contained in my.zip?
//  Int_t  HZip::list("my.zip",".*");          // list all files which match the pattern
//  Int_t  HZip::getList("my.zip",list,".*");  // return to TList list all files which match the pattern
//  Bool_t HZip::unzip("my.zip","mydir");      // unzip file to directory
//  Bool_t HZip::addFile("my.zip","my.root");  // add this root file to the zip file
//  Bool_t HZip::addFiles("my.zip",list);      // add all root files from TList list to the zip file
////////////////////////////////////////////////////////////////////////////

ClassImp(HZip)


