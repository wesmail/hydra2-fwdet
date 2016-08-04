APP_NAME     := hadesonlineclient.exe
SOURCE_FILES :=	hadesonlineclient.cc

export USES_RFIO  ?= no
export USES_ORACLE  ?= yes
export USES_GFORTRAN ?= yes


include $(HADDIR)/hades.def.mk

HYDRA_LIBS += -lRevt -lOnline

include $(HADDIR)/hades.app.mk

