APP_NAME     := showeralign
SOURCE_FILES := showeralign.cc
export USES_RFIO    := yes
export USES_CERNLIB := yes
export USES_ORACLE  := yes
export USES_GFORTRAN := yes

include $(HADDIR)/hades.def.mk
HYDRA_LIBS += -lAlignment
include $(HADDIR)/hades.app.mk
