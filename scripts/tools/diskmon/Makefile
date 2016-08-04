###############################################################################
#
#  The global Makefile to build all Hydra modules from scratch.
#
#  Examples and explanations can be found in the file README
#
###############################################################################

# no external dependencies for a total build 
export HADDIR   := $(shell pwd)
INSTALL_DIR     = $(HADDIR)/install
export USES_RFIO       ?= no
export USES_GFORTRAN   ?= yes
export USES_ORACLE     ?= no

MODULES ?=	diskspace

include $(HADDIR)/hades.def.mk
include $(HADDIR)/hades.rules.mk
