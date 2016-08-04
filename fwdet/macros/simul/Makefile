APP_NAME     := analysisDST
SOURCE_FILES := analysisDST.cc

include $(HADDIR)/hades.def.mk

.PHONY:  default
default: build install

HYDRA_LIBS+=  -lOra

include $(HADDIR)/hades.app.mk
