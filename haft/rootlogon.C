char* whichMacro(char * macro) 
{
  char *mac; 
  mac = gSystem->Which(gROOT->GetMacroPath(),macro,kReadPermission); 
  if (!mac) 
    {
      delete [] mac ;
      return 0;
    }
  char *macrodir = new char[100];
  strcpy(macrodir,mac);
  delete [] mac;
  return macrodir;
}

void rootlogon()
{
  gStyle->SetPalette(1); 
  
  Char_t *libFiles[200] = {
    "libTreePlayer.so",
    "libPhysics.so"
  };

  Char_t *libPrivateFiles[200] = {
    "libHAFT.so"
  };

  printf("\nHALO rootlogon ... loaded from: \"%s\".\n\n...loading shared libraries\n\n",whichMacro("rootlogon.C"));

  for(Int_t i=0;libFiles[i]!=0;i++) 
    {
      char *path = gSystem->ExpandPathName(libFiles[i]);
      if (gSystem->Load(path)==0) printf("Library %s loaded\n",path);
      else Error("Load Library","Unable to load %s",path);
      path =NULL;
  }
 
  for(Int_t i=0;libPrivateFiles[i]!=0;i++) 
    {
      char *path = gSystem->ExpandPathName(libPrivateFiles[i]);
      if (!gSystem->AccessPathName(path,kReadPermission))
	{
	  if(gSystem->Load(path)==0) printf("Library %s loaded\n",path);
	  else Error("Load Library","Unable to load %s",path);
	}
      else Error("Load Library","Unable to load %s",path);
      path =NULL;
    }
  
  if (whichMacro("writeHAFT.C"))
     if (gROOT->LoadMacro("writeHAFT.C+")==0)
        printf("\n\"writeHAFT\" macro loaded\n");
     else printf("\nUnable to load \"writeHAFT\" macro\n");
  
  printf("\nWelcome to the HAFT Root environment\n\n");
  
}







  
