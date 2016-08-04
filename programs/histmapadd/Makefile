export USES_RFIO       ?= no
export USES_GFORTRAN   ?= yes
export USES_ORACLE     ?= yes

APP_NAME     := histmapadd
SOURCE_FILES := histmapadd.cc

include $(HADDIR)/hades.def.mk

HYDRA_LIBS = -lHydra

include $(HADDIR)/hades.app.mk
