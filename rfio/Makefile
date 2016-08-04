##############################################################################
#
#  Makefile for Hydra library libRFIOtsm.so
#
#  This makefile contains all definitions local to this module. All
#  general definitions are included from makefiles in $(HADDIR).
#
##############################################################################

LIB_NAME := RFIOtsm

SOURCE_FILES := hrfiofiletsm.cc \
	hrfiotsmsignalhandler.cc \
	htrfiofile.cc

USES_RFIO := yes

include $(HADDIR)/hades.def.mk
include $(HADDIR)/hades.module.mk

SO_LD_FLAGS += $(RFIO_LIBS)
