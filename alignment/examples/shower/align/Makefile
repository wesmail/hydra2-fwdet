APP_NAME     := align
SOURCE_FILES := align.cc
export USES_RFIO    := yes
export USES_CERNLIB := yes
export USES_ORACLE  := yes
export USES_GFORTRAN := yes

include $(HADDIR)/hades.def.mk
HYDRA_LIBS += -lAlignment
include $(HADDIR)/hades.app.mk
