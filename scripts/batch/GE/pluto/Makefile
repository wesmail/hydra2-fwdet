
APP_NAME     := run_pluto
SOURCE_FILES := analysis.cc

PLUTO_DIR=/cvmfs/hades.gsi.de/install/5.34.34/pluto/v5_44


USES_ORACLE=yes
USES_GFORTRAN=yes
USES_RFIO=no

include $(HADDIR)/hades.def.mk
INC_DIRS += $(PLUTO_DIR)/include
LIB_DIRS += $(PLUTO_DIR)
SO_LOADER_DIRS  += $(PLUTO_DIR)
APP_LOADER_DIRS += $(PLUTO_DIR)
LIBS            += -lPluto
#HYDRA_LIBS +=

.PHONY:  default
default: clean build install

include $(HADDIR)/hades.app.mk
