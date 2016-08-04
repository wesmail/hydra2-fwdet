
APP_NAME     := hadesonlineserver.exe
SOURCE_FILES := hadesonlineserver.cc


export USES_RFIO  ?= no
export USES_ORACLE  ?= yes
export USES_GFORTRAN ?= yes


include $(HADDIR)/hades.def.mk

HYDRA_LIBS +=  -lOnline -lRevt

include $(HADDIR)/hades.app.mk
