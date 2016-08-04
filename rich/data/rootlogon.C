{
cout<<"This HYDRA 532 muc gen3"<<endl;
 printf("\n");
 if (gSystem->Load("libPhysics.so")!=0) {
   printf("Shared library libPhysics.so could not load\n");
 }
 if (gSystem->Load("libProof.so")!=0) {
   printf("Shared library libProof.so could not load\n");
 }
 if (gSystem->Load("libTreePlayer.so")!=0) {
   printf("Shared library libTreePlayer.so could not load\n");
 }
 if (gSystem->Load("libHydra.so")==0) {
   printf("Shared library libHydra.so loaded\n");
 } else {
   printf("Unable to load libHydra.so\n");
 } 
  if (gSystem->Load("libTof.so")==0) {
   printf("Shared library libTof.so loaded\n");
 } else {
   printf("Unable to load libTof.so\n");
 }
 if (gSystem->Load("libMdc.so")==0) {
   printf("Shared library libMdc.so loaded\n");
 } else {
   printf("Unable to load libMdc.so\n");
 }
 if (gSystem->Load("libMdcTrackD.so")==0) {
   printf("Shared library libMdcTrackD.so loaded\n");
 } else {
   printf("Unable to load libMdcTrackD.so\n");
 }
 if (gSystem->Load("libMdcTrackS.so")==0) {
   printf("Shared library libMdcTrackS.so loaded\n");
 } else {
   printf("Unable to load libMdcTrackS.so\n");
 }
 if (gSystem->Load("libTofino.so")==0) {
   printf("Shared library libTofino.so loaded\n");
 } else {
   printf("Unable to load libTofino.so\n");
 }
 if (gSystem->Load("libStart.so")==0) {
   printf("Shared library libStart.so loaded\n");
 } else {
   printf("Unable to load libStart.so\n");
 }
 if (gSystem->Load("libShower.so")==0) {
   printf("Shared library libShower.so loaded\n");
 } else {
   printf("Unable to load libShower.so\n");
 } 
 if (gSystem->Load("libShowerTofino.so")==0) {
   printf("Shared library libShowerTofino.so loaded\n");
 } else {
   printf("Unable to load libShowerTofino.so\n");
 } 

 if (gSystem->Load("libKick.so")==0) {
   printf("Shared library libKick.so loaded\n");
 } else {
   printf("Unable to load libKick.so\n");
 } 

 if (gSystem->Load("libRich.so")==0) {
   printf("Shared library libRich.so loaded\n");
 } else {
   printf("Unable to load libRich.so\n");
 } 
 if (gSystem->Load("libRichUtil.so")==0) {
   printf("Shared library libRichUtil.so loaded\n");
 } else {
   printf("Unable to load libRichUtil.so\n");
 } 

 //  if (gSystem->Load("libOnline.so")==0) {
//     printf("Shared library libOnline.so loaded\n");
//   } else {
//     printf("Unable to load libOnline.so\n");
//   } 
 if (gSystem->Load("libTrigger.so")==0) {
   printf("Shared library libTrigger.so loaded\n");
 } else {
   printf("Unable to load libTrigger.so\n");
 } 
 if (gSystem->Load("/home/lfabbi/hydra_v531/lib/Pluto.so")==0) {
   printf("Shared library Pluto.so 355 loaded\n");
 } else {
   printf("Unable to load Pluto.so 355\n");
 }
 if (gSystem->Load("libPhyAna.so")==0) {
   printf("Shared library libPhyAna.so loaded\n");
 } else {
   printf("Unable to load libPhyAna.so\n");
 } 
//   if (gSystem->Load("libOra.so")==0) {
//     printf("Shared library libOra.so loaded\n");
//   } else {
//     printf("Unable to load libOra.so\n");
//   }

//   if (gSystem->Load("HRevLib.so")==0) {
//     printf("Shared library HRevLib.so loaded\n");
//   } else {
//     printf("Unable to load HRevLib.so\n");
//   } 
 
//   if (gSystem->Load("libRevt.so")==0) {
//     printf("Shared library libRevt.so loaded\n");
//   } else {
//     printf("Unable to load libRevt.so\n");
//   }

 if (gROOT->LoadMacro("analfunc.C")==0) {
	printf("\n\"Analysis\" macro loaded\n");
 } else {
  printf("\nUnable to load \"Analysis\" macro\n");
 }
TVirtualTreePlayer::SetPlayer("HTreePlayer");
gStyle->SetPalette(1);

// HDebug debuger;
 printf("\nWelcome to the Hydra Root environment\n\n");
}







