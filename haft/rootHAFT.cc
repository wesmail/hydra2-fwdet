#include <TROOT.h>
#include <TSystem.h>
#include <TRint.h>

TROOT rootHAFT("HAFT","Root with HAFT function getHadesAcceptance(p,th,ph)");

int main(int argc,char *argv[]) {
  TRint *rint=new TRint("Rint",&argc,argv,0,0);
  rint->Run();
  delete rint;
  return 0;
}

