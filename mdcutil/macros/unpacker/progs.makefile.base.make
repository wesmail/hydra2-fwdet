## File: $RCSfile: progs.makefile.base.make,v $ 
## Version: $Revision: 1.1 $
## Modified by $Author: zumbruch $ on $Date: 2006-04-03 12:05:13 $  
###############################################################################

NOLIBNEW=ON

include $(HADDIR_BASEDIR)/Makefile.base

CXXFLAGS += -I$(ADSM_BASE)/Linux

LIBS+=$(ROOTLIBS) -lTreePlayer -lMinuit -lThread -lProof
LIBS+=$(ROOTGLIBS) 

## private additional classes

ifdef HADDIR_MDCDEV_LIB
ifneq ($(HADDIR_MDCDEV_LIB),)
ADD_EXTRAS_LIB=$(HADDIR_MDCDEV_LIB):
ADD_EXTRAS_LIB_SO=$(HADDIR_MDCDEV_LIB)/libMdcDev.so	
ADD_MDCDEV_DIR_INC=-I$(HADDIR_MDCDEV)
endif
endif

ifdef RFIO_SUPPORT
ifneq ($(RFIO_SUPPORT),)
ADD_RFIO_LIB=$(HADDIR_RFIO_LIB):
ADD_RFIO_LIB_SO=$(HADDIR_RFIO_LIB)/libRFIOtsm.so 
endif
endif

ifdef ORACLE_HOME
ifneq ($(ORACLE_HOME),)
ADD_ORACLE_LIB=$(HADDIR_ORA_LIB):
ADD_ORACLE_LIB_SO=$(HADDIR_ORA_LIB)/libOra.so	
LORALIB	= -L$(ORACLE_HOME)/lib/ -lclntsh 
LIBS+=$(LORALIB)
endif
endif

ifdef ONLINEHYDRA
ifneq ($(ONLINEHYDRA),)
ADD_EVTSERVER_LIB=$(HADDIR_EVTSERVER_LIB):
ADD_EVTSERVER_LIB_SO=$(HADDIR_EVTSERVER_LIB)/libRevt.so	
endif
endif

RPATH=$$(echo  "$(HADDIR_PID_LIB):\
	        $(HADDIR_MDCTRACKD_LIB):\
		$(ADD_RFIO_LIB)\
		$(HADDIR_QA_LIB):\
		$(HADDIR_RICH_LIB):\
		$(HADDIR_RICHUTIL_LIB):\
		$(HADDIR_MDC_LIB):\
		$(ROOTSYS)/lib:\
		$(HADDIR_BASE_LIB):\
		$(HADDIR_KICKPLANE_LIB):\
		$(HADDIR_MDCTRACKS_LIB):\
		$(HADDIR_MDCTRACKG_LIB):\
		$(ADD_ORACLE_LIB)\
		$(ADD_EVTSERVER_LIB)\
		$(HADDIR_START_LIB):\
		$(HADDIR_SHOWER_LIB):\
		$(HADDIR_SHOWERUTIL_LIB):\
		$(HADDIR_SHOWERTOFINO_LIB):\
		$(HADDIR_TOF_LIB):\
		$(HADDIR_TOFINO_LIB):\
		$(HADDIR_TRIGGER_LIB):\
		$(HADDIR_TRIGGERUTIL_LIB):\
		$(HADDIR_TOOLS_LIB):\
		$(HADDIR_PIDUTIL_LIB):\
	        $(HADDIR_PAIRS_LIB):\
	        $(HADDIR_PHYANA_LIB):\
		$(HADDIR_WALL_LIB):\
		$(ADD_EXTRAS_LIB)\
		$(ADSM_BASE)/Linux:\
		$(GSIDIR)" \
		| sed -e 's/ //g'\
		| perl -e '$$path = <>; $$path =~ s/\/\//\//g; print $$path;' \
		| perl -e 'my $$path = <>;                \
	 		   my %seen = ();                 \
	 		   my @uniq = ();                 \
			   my @paths = split /:/, $$path; \
			   foreach (@paths)               \
			   {                              \
			     unless ($$seen{$$_})         \
			     {                            \
			       $$seen{$$_} = 1;           \
			       push (@uniq, $$_);         \
			     }                            \
			   }                              \
			   $$path = join(":", @uniq);     \
			   print $$path;'                 \
		) 

MYROOTLIBS =-L$(ROOTSYS)/lib $(LIBS) 

EXTRASLIBS = $(ADD_EXTRAS_LIB_SO)

EXTRASINCLUDE = $(ADD_MDCDEV_DIR_INC)

ADSMLIBS   = $(ADSM_BASE)/Linux/librawapicli.a \
	     $(ADSM_BASE)/Linux/librawserv.a 

REVLIBS	   = $(GSIDIR)/libhrevcli.so

OTHERLIBS  = -L./

HYDRALIBS  = $(HADDIR_PID_LIB)/libPid.so \
             $(HADDIR_MDC_LIB)/libMdc.so \
             $(ADD_RFIO_LIB_SO) \
	     $(HADDIR_SHOWERUTIL_LIB)/libShowerUtil.so \
	     $(HADDIR_TRIGGER_LIB)/libTrigger.so \
	     $(HADDIR_TRIGGERUTIL_LIB)/libTriggerUtil.so \
	     $(HADDIR_PIDUTIL_LIB)/libPidUtil.so \
             $(HADDIR_PAIRS_LIB)/libPairs.so \
             $(HADDIR_MDCTRACKG_LIB)/libMdcTrackG.so \
             $(HADDIR_MDCTRACKD_LIB)/libMdcTrackD.so \
             $(HADDIR_MDCTRACKS_LIB)/libMdcTrackS.so \
             $(HADDIR_KICKPLANE_LIB)/libKick.so \
             $(HADDIR_RICH_LIB)/libRich.so \
             $(HADDIR_RICH_LIB)/libRich.so \
             $(HADDIR_RICHUTIL_LIB)/libRichUtil.so \
             $(HADDIR_SHOWER_LIB)/libShower.so \
	     $(ADD_EVTSERVER_LIB_SO) \
             $(HADDIR_SHOWERTOFINO_LIB)/libShowerTofino.so \
             $(HADDIR_PHYANA_LIB)/libPhyAna.so \
             $(HADDIR_START_LIB)/libStart.so \
             $(HADDIR_TOF_LIB)/libTof.so \
             $(HADDIR_WALL_LIB)/libWall.so \
             $(HADDIR_TOFINO_LIB)/libTofino.so \
             $(HADDIR_TOOLS_LIB)/libTools.so \
             $(HADDIR_QA_LIB)/libQA.so \
	     $(ADD_ORACLE_LIB_SO) \
             $(HADDIR_BASE_LIB)/libHydra.so

LINKERLIBS = $$(echo "$(OTHERLIBS) $(MYROOTLIBS) $(HYDRALIBS) $(ADSMLIBS) $(REVLIBS) $(EXTRASLIBS)" \
		| perl -e 'my $$path = <>;                \
	 		   my %seen = ();                 \
	 		   my @uniq = ();                 \
			   my @paths = split / /, $$path; \
			   foreach (@paths)               \
			   {                              \
			     unless ($$seen{$$_})         \
			     {                            \
			       $$seen{$$_} = 1;           \
			       push (@uniq, $$_);         \
			     }                            \
			   }                              \
			   $$path = join(" ", @uniq);     \
			   print $$path;'                 \
		| perl -e '$$path = <>; $$path =~ s/\/\//\//g; print $$path')


ALLCXXFLAGS = $(EXTRASINCLUDE) $(CXXFLAGS)

