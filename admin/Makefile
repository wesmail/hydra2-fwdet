###############################################################################
#
#  The global Makefile to build all Hydra modules from scratch.
#
#  Examples and explanations can be found in the file README
#
###############################################################################

# no external dependencies for a total build
export HADDIR   := $(shell pwd)
MYHADDIR        :=
INSTALL_DIR     ?= /misc/hadessoftware/etch32/install/hydra-dev
export USES_RFIO       ?= yes
export USES_GFORTRAN   ?= yes
export USES_ORACLE     ?= yes

MODULES ?=	base		\
		dst		\
		eventdisplay    \
		evtserver	\
		mdc		\
		mdcgarfield	\
		mdctrackD	\
		mdctrackG	\
		alignment	\
		mdcutil		\
		online          \
		particle      	\
		qa		\
		rich		\
		rpc		\
		shower		\
		showerutil	\
		emc		\
		simulation	\
		start       	\
		tof		\
		tools		\
		wall            \
		piontracker	\
		fwdet		\
		kalman

ifeq ($(strip $(USES_ORACLE)),yes)
   MODULES += ora
   MODULES += orasim
   MODULES += orautil
endif

ifeq ($(strip $(USES_RFIO)),yes)
   MODULES += rfio
endif


include $(HADDIR)/hades.def.mk
include $(HADDIR)/hades.rules.mk
