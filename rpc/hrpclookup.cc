//*-- AUTHORS :  Pablo Cabanelas / Hector Alvarez Pol
//*-- created : 13/10/2005

////////////////////////////////////////////////////////////////////
// HRpcLookup
//
// Container class for mapping Channel to Cell 
//   needed by the RPC unpacker
//
// FIXME: codigo solo para Nov05 test
//
////////////////////////////////////////////////////////////////////

using namespace std;
#include "hrpclookup.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HRpcLookupChan)
ClassImp(HRpcLookup)

HRpcLookup::HRpcLookup(const Char_t* name,const Char_t* title,
                       const Char_t* context,Int_t nChan) : HParSet(name,title,context) {
  // The constructor creates an array of pointers of type HRpcLookupChan
  // The container name is set to "RpcLookup".
  strcpy(detName,"Rpc");
  array = new TObjArray(nChan);
  //printf("Antes %i\n",nChan);
  for(Int_t i=0;i<nChan;i++) array->AddAt(new HRpcLookupChan(),i);
  //printf("Despues\n");
  nChannels=180;  // Nov05 test
  maxChannel=nChan-1;
  clear();
}

HRpcLookup::~HRpcLookup() {
  // destructor
  array->Delete();
  delete array;
}

Bool_t HRpcLookup::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HRpcParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HRpcLookup::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HRpcParIo");
  if (out) return out->write(this);
  return -1;
}

void HRpcLookup::clear() {
  // clears the container
  for(Int_t c=0;c<=maxChannel;c++)  (*this)[c].clear();
  status=kFALSE;
  resetInputVersions();
}

void HRpcLookup::printParam() {
  // prints the lookup table
  printf("Lookup table for the RPC unpacker (nov05 test)\n");
  printf("channel  cell  side\n");
  for(Int_t c=0;c<getSize();c++) {  // bucle en canales
    HRpcLookupChan &chan= (*this)[c];    // getChannel(c);
    if (c>=0) printf("%4i   %4i   %c\n",c,chan.getCell(),chan.getSide());
  }
}

void HRpcLookup::fill(Int_t channel, Int_t cell, Char_t side) {
  // Expands the array, if necessary, creates the objects of type HRpcLookupChan,
  // and then calls the fill function. 
  if (channel>maxChannel) {
    array->Expand(nChannels+1); 
    for(Int_t i=maxChannel+1;i<=nChannels;i++) array->AddAt(new HRpcLookupChan(),i);
    maxChannel=nChannels-1;
  }
  (*this)[channel].fill(cell,side);
}

void HRpcLookup::readline(const Char_t *buf, Int_t *set) {
  // decodes one line read from ascii file I/O
  Int_t chan, cell;
  Char_t side[2];
  sscanf(buf,"%i%i%s",&chan,&cell,side);
  fill(chan,cell,side[0]);

  //Solo hay un modulo y un sector. Cambiar si hay mas
  if (set[0])  set[0]=999;
}

void HRpcLookup::putAsciiHeader(TString& header) {
  // puts the ascii header to the string used in HRpcParAsciiFileIo
  header=
    "# Lookup table for the RPC unpacker (only for Nov05 test)\n"
    "# Format:\n"
    "# channel   cell   side\n";
}

Bool_t HRpcLookup::writeline(Char_t *buf, Int_t chan) {
  // writes one line to the buffer used by ascii file I/O
  HRpcLookupChan& c=(*this)[chan];
  if (c.getCell()>=0) {
    sprintf(buf,"%4i   %4i   %c\n",
          chan,c.getCell(),c.getSide());
    return kTRUE;
  }
  return kFALSE;
}
