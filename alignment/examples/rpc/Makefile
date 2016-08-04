APP_NAME     := rpcalign
SOURCE_FILES := rpcalign.cc
export USES_RFIO    := yes
export USES_CERNLIB := yes
export USES_ORACLE  := yes
export USES_GFORTRAN := yes

include $(HADDIR)/hades.def.mk
HYDRA_LIBS += -lAlignment
include $(HADDIR)/hades.app.mk
