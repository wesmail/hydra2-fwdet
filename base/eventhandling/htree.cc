#include "htree.h"
#include "TROOT.h"
#include "TKey.h"
#include "TBranch.h"
#include "TFile.h"
#include "TMath.h"
#include "TClonesArray.h"

#if ROOT_VERSION_CODE  < ROOT_VERSION(6,0,0)
  #include "Api.h"     //changedROOT6
#endif

//*-- Author : Manuel Sanchez
//*-- Modified : 12/1/2000 by R.Holzmann
//*-- Modified : 17/12/99 by Denis Bertini
//*-- Modified : 12/05/98 by Manuel Sanchez
//*-- Modified : 19/09/00 by Denis Bertini
//*-- Copyright : GENP (Univ. Santiago de Compostela)

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////
// HTree
//
//  The HTree is a TTree implementing a automatic split algorithm different
// from the Root one; allowing a more complex tree structure (when used with
// HPartialEvent ...)
//
//////////////////////////////////////////////

ClassImp(HTree)

  HTree::HTree(void) : TTree() {
}

HTree::HTree(const Text_t* name, const Text_t* title, Int_t maxvirtualsize) : TTree (name,title,maxvirtualsize) {
}

HTree::~HTree(void) {
}

void HTree::printContainerSizes(void) {
// This function returns a formatted list of data containers with the
// file size of each container.  To be used with Hydra < v6.00 (old
// branching scheme), probably can be adapted for new Root...

   printf("\n");
   printf("      File: %s  \n",this->GetCurrentFile()->GetName());
   printf("            (%i entries) \n",(int) this->GetEntries());
   printf("\n");
   printf("         Container          File Size         Size/evt\n");
   printf("      ---------------  ------------------   ------------\n");

   Int_t nBytes = 0, nBytesBranch2 = 0;
   TObjArray *array2 = 0, *array3 = 0;
   TBranch *branch1 = 0, *branch2 = 0, *branch3 = 0;

   //TObjArray *array4 = 0;
   //TBranch *branch4 = 0;

   for(Int_t i=0;i<fBranches.GetEntries();i++) {

      branch1 = (TBranch*) fBranches.At(i);
      array2 = branch1->GetListOfBranches();

      nBytesBranch2 = 0;
      for(Int_t j=0;j<array2->GetEntries();j++) {

         branch2 = (TBranch*) array2->At(j);
         nBytesBranch2 += (int) branch2->GetZipBytes();
         array3 = branch2->GetListOfBranches();
         Int_t nBranch2 = array3->GetEntries();

         for(Int_t k=0;k<nBranch2;k++) {

            branch3 = (TBranch*) array3->At(k);
            nBytesBranch2 += (int) branch3->GetZipBytes();

//             array4 = branch3->GetListOfBranches();  // needed for old scheme
//             Int_t nBranch3 = array4->GetEntries();
//
//             for(Int_t l=0;l<nBranch3;l++) {
//
//                 branch4 = (TBranch*) array4->At(l);
//                 nBytesBranch2+= (int) branch3->GetZipBytes();
//             }

         }

      }

      if(nBytesBranch2) {
	  printf(" %20s %11i (%4.1f%%)   %6.1f bytes\n",
		 branch1->GetName(),nBytesBranch2,
		 nBytesBranch2*100./this->GetZipBytes(), 1.*nBytesBranch2/branch1->GetEntries() );
      }
      nBytes += nBytesBranch2;
   }
   printf("                       ------------------   ------------\n");
   printf("                      %11i (%4.1f%%)   %6.3f kB/event\n",
         nBytes,nBytes*100./this->GetZipBytes(), nBytes/branch1->GetEntries()/1024. );
   printf("\n");

}

Int_t HTree::MakeCode(const Char_t* filename) {
//----
// MakeCode adapted for Htree with split level 2
// <D.Bertini@gsi.de>

// Connect output file
   Char_t *tfile = new char[1000];
   if (filename) strcpy(tfile,filename);
   else          sprintf(tfile,"%s.C",GetName());
   FILE *fp = fopen(tfile,"w");
  if (!fp) {
      Error("MakeCode","Cannot open output file:%s\n",tfile);
      return -1;
   }
//connect the htree file
   Char_t *treefile = new char[1000];
   if (fDirectory && fDirectory->GetFile())
                strcpy(treefile,fDirectory->GetFile()->GetName());
   else         strcpy(treefile,"Memory Directory");

   TDatime td;
   fprintf(fp,"{\n");
   fprintf(fp,"///////////////////////////////////////////////////////////\n");
   fprintf(fp,"// This file has been automatically generated \n");
   fprintf(fp,"// (%s by ROOT version%s)\n",td.AsString(),gROOT->GetVersion());
   fprintf(fp,"// from HTree %s/%s\n",this->GetName(),this->GetTitle());
   fprintf(fp,"// found on file: %s\n",treefile);
   fprintf(fp,"///////////////////////////////////////////////////////////\n");
   fprintf(fp,"\n");
   fprintf(fp,"\n");

// Reset and file connect
   fprintf(fp,"//Reset ROOT and connect tree file\n");
   fprintf(fp,"   gROOT->Reset();\n");
   fprintf(fp,"   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(\"%s\");\n",treefile);
   fprintf(fp,"   if (!f) {\n");
   fprintf(fp,"      f = new TFile(\"%s\");\n",treefile);
   if (gDirectory != gFile) {
      fprintf(fp,"      f->cd(\"%s\");\n",gDirectory->GetPath());
   }
   fprintf(fp,"   }\n");
   fprintf(fp,"   HTree *%s = (HTree*)gDirectory->Get(\"%s\");\n\n",
                  GetName(),GetName());
   fprintf(fp,"   if(!%s) {cout << \"No tree in file!\" << endl; exit(1);}\n",
                  GetName());

   fprintf(fp,"   HEventHeader* fHead =(HEventHeader*)f->Get(\"EventHeader\");\n");
   fprintf(fp,"   T->SetBranchAddress(\"EventHeader.\",&fHead);\n\n");

//**** Scanning of Htree 
//**** for the moment 
   TFile* f = fDirectory->GetFile(); 
   TList* total = f->GetListOfKeys(); 
   Int_t entries = total->GetSize();

//**** Only work in split mode 2 for the moment
   Char_t sl=*(strchr(GetTitle(),'.')+1);
   switch (sl) {
      case '0' : Error("MakeCode", "split 0 not yet implemented");
                 return -1; 
                 break;
      case '1' : Error("MakeCode", "split 0 not yet implemented");
                 return -1; 
                 break;
   } 


// Scan the File structure linked to the Htree and parse 
// the relevant subdirectories
   for(Int_t i=0;i<entries;i++){
     TKey* akey = (TKey*) total->At(i);
     Text_t ptr[500];
     Text_t ptrObj[500];
     Text_t branch[500];
     const Int_t npartial = 13;
     const Char_t* partial[npartial]={"Mdc","Tof","Tofino","Start","Rich","Shower","Simul","Tracks","Trigger","Particle","Rpc","TrbNet","Emc"};
     for(Int_t k=0; k<npartial; k++){
       if(strstr(akey->GetName(),partial[k]) != 0){
	 f->cd(akey->GetName());
	 fprintf(fp,"   f->cd(\"%s\"); \n",akey->GetName());
	 TList* subkey = gDirectory->GetListOfKeys();
	 Int_t s2 = subkey->GetSize();
	 for(Int_t j=0;j<s2;j++){ 
	   TKey* akey2 = (TKey*) subkey->At(j);
	   if(strstr(akey2->GetClassName(),"LinearCategory") != 0){
	     sprintf(ptrObj,"ptl%s",akey2->GetName());     
	     fprintf(fp,"   %s* %s = new %s();\n",akey2->GetName(),
		     ptrObj,akey2->GetName());  
	     sprintf(ptr,"catl%s",akey2->GetName()); 
	     fprintf(fp,"   HLinearCategory* %s = (HLinearCategory*) gDirectory->Get(\"%s\");\n",ptr,akey2->GetName());
	     sprintf(branch,"brl%s",akey2->GetName());
	     fprintf(fp,"   TBranch* %s = %s->GetBranch(\"%s\");\n",
		     branch,this->GetName(),akey2->GetName()); 
	     fprintf(fp,"   if(%s) %s->SetAddress(&%s);\n\n",branch, branch, ptr);  
	   }

	   if(strstr(akey2->GetClassName(),"MatrixCategory") != 0){
	     sprintf(ptrObj,"ptm%s",akey2->GetName());     
	     fprintf(fp,"   %s* %s = new %s();\n",akey2->GetName(),
		     ptrObj,akey2->GetName());  
	     sprintf(ptr,"catm%s",akey2->GetName()); 
	     fprintf(fp,"   HMatrixCategory* %s = (HMatrixCategory*) gDirectory->Get(\"%s\");\n",ptr,akey2->GetName());
	     sprintf(branch,"brm%s",akey2->GetName());
	     fprintf(fp,"   TBranch* %s = %s->GetBranch(\"%s\");\n",
		     branch,this->GetName(),akey2->GetName()); 
	     fprintf(fp,"   if(%s) %s->SetAddress(&%s);\n\n",branch, branch, ptr);
	   }

	 }
       }
     }
   }

   fprintf(fp,"   f->cd();\n");

//Generate instructions to make the loop on entries
   fprintf(fp,"\n//     This is the loop skeleton\n");
   fprintf(fp,"//       To read only selected branches, Insert statements like:\n");
   fprintf(fp,"// %s->SetBranchStatus(\"*\",0);  // disable all branches\n",GetName());
   fprintf(fp,"// %s->SetBranchStatus(\"branchname\",1);  // activate branchname\n",GetName());
   fprintf(fp,"\n   Int_t nentries = %s->GetEntries();\n",GetName());
   fprintf(fp,"\n   Int_t nbytes = 0;\n");
   fprintf(fp,"//   for (Int_t i=0; i<nentries;i++) {\n");
   fprintf(fp,"//   For Accessing  the Data you can:  \n");
   fprintf(fp,"//    1)Use of a Total Io on theHTree \n");
   fprintf(fp,"//      nbytes += %s->GetEntry(i);\n",GetName());
   fprintf(fp,"//    2)Use of a specific branch Io on the Htree \n");
   fprintf(fp,"//      nbytes += branchname->GetEntry(i);\n");
   fprintf(fp,"\n");
   fprintf(fp,"//   To loop over Data-Objects:\n");
   fprintf(fp,"//      TIter iter = pCat->MakeIterator();\n");
   fprintf(fp,"//      iter->Reset();\n");
   fprintf(fp,"//      while ( (pObj=(HDataObjName*)iter->Next()) != 0 ){ \n");
   fprintf(fp,"//         pObj->getValue();\n");
 
   fprintf(fp,"//      }\n");
   fprintf(fp,"//   pCat->Clear();\n");
   fprintf(fp,"//   }\n");
   fprintf(fp,"}\n");

   fclose(fp);
   printf("Macro %s generated from HTree: %s\n",filename,GetName());

   delete [] treefile; 
   delete [] tfile;
   return 0;
}

TBranch *HTree::GetBranch(const Char_t * name) {
  TBranch *r = 0;
  
  r = TTree::GetBranch(name);
  if (!r) {
    TString cad = name;
    cad += ".";
    r = TTree::GetBranch(cad.Data());
    if (r) {
      Warning("GetBranch","Branch %s not found. Using %s instead",
	      name,cad.Data());
    }
  }
  
  return r;
}
TTree *gTree;
