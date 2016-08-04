#include "hdebug.h"
#include <stdio.h>

ClassImp(HDebug)

  HDebug::HDebug(void) {
    fIdentLevel=0;
    gDebuger=this;
}

HDebug::~HDebug(void) {
}

void HDebug::message(const Text_t *format,...) {
    va_list ap;
    va_start(ap,format);
    for (Int_t i=0;i<fIdentLevel*2;i++) putchar(' ');
    vprintf(format,ap);
    putchar('\n');
    va_end(ap);
}

void HDebug::enterFunc(const Text_t *func) {
  message("entering %s",func);
  incIdentLevel();
}

void HDebug::leaveFunc(const Text_t *func) {
  message("returning from %s",func);
  decIdentLevel();
}

HDebug *gDebuger=0;
