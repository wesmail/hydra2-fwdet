NOLIBNEW=ON

include $(HADDIR_BASEDIR)/Makefile.base

CXXFLAGS += -I$(ADSM_BASE)/Linux 

LORALIB	:= -L$(ORACLE_HOME)/lib/ -lclntsh


LIBS+=$(ROOTLIBS)  -lTreePlayer -lMinuit -lProof -lThread
LIBS+=$(ROOTGLIBS) 
LIBS+=$(LORALIB)

RPATH:=`echo "$(HADDIR_TOOLS_LIB):$(HADDIR_MDC_LIB):\
	$(HADDIR_QA_LIB):\
	$(ROOTSYS)/lib:\
	$(HADDIR_BASE_LIB):\
	$(HADDIR_KICKPLANE_LIB):\
	$(HADDIR_MDCUTIL_LIB):\
	$(HADDIR_MDCTRACKS_LIB):\
	$(HADDIR_MDCTRACKD_LIB):\
	$(HADDIR_MDCTRACKG_LIB):\
	$(HADDIR_ORA_LIB):\
	$(HADDIR_RFIO_LIB):\
	$(HADDIR_RICH_LIB):\
	$(HADDIR_EVTSERVER_LIB):\
	$(HADDIR_START_LIB):\
	$(HADDIR_SHOWER_LIB):\
	$(HADDIR_SHOWERTOFINO_LIB):\
	$(HADDIR_TOF_LIB):\
	$(HADDIR_WALL_LIB):\
	$(HADDIR_TOFINO_LIB):\
	$(HADDIR_PHYANA_LIB):\
	$(ADSM_BASE)/Linux:\
	$(GSIDIR)"| sed -e 's/ //g'`

MYROOTLIBS:=-L$(ROOTSYS)/lib $(LIBS) 

OTHERLIBS := -L./

HYDRALIBS := $(HADDIR_MDCUTIL_LIB)/libMdcUtil.so \
	     $(HADDIR_KICKPLANE_LIB)/libKick.so \
	     $(HADDIR_MDCTRACKD_LIB)/libMdcTrackD.so \
	     $(HADDIR_MDCTRACKS_LIB)/libMdcTrackS.so \
	     $(HADDIR_MDCTRACKG_LIB)/libMdcTrackG.so \
	     $(HADDIR_MDC_LIB)/libMdc.so \
	     $(HADDIR_RFIO_LIB)/libRFIOtsm.so \
	     $(HADDIR_RICH_LIB)/libRich.so \
	     $(HADDIR_RICHUTIL_LIB)/libRichUtil.so \
	     $(HADDIR_SHOWER_LIB)/libShower.so \
	     $(HADDIR_SHOWERTOFINO_LIB)/libShowerTofino.so \
	     $(HADDIR_START_LIB)/libStart.so \
	     $(HADDIR_TOF_LIB)/libTof.so \
	     $(HADDIR_WALL_LIB)/libWall.so \
	     $(HADDIR_TOOLS_LIB)/libTools.so \
	     $(HADDIR_TOFINO_LIB)/libTofino.so \
	     $(HADDIR_QA_LIB)/libQA.so \
	     $(HADDIR_ORA_LIB)/libOra.so \
	     $(HADDIR_PHYANA_LIB)/libPhyAna.so \
	     $(HADDIR_BASE_LIB)/libHydra.so 

ADSMLIBS  := $(ADSM_BASE)/Linux/librawapicli.a \
	     $(ADSM_BASE)/Linux/librawserv.a 

REVLIBS	  := $(GSIDIR)/libhrevcli.so

LINKERLIBS := $(OTHERLIBS) $(MYROOTLIBS) $(HYDRALIBS) 

ALLCXXFLAGS := $(CXXFLAGS) -I$(HADDIR_MDCUTIL) -I$(HADDIR_TOOLS)

LDFLAGS :=-Wl,-rpath,$(RPATH) 

.PHONY : clean test

MAIN:=calibrate

$(MAIN):  $(MAIN).cc
	$(CXX) $(MAIN).cc $(ALLCXXFLAGS) -c -o $(MAIN).o
	$(LD) $(MAIN).o $(LDFLAGS) $(LINKERLIBS) -o $(MAIN)

clean: 
	@if [ -e $(MAIN) ]; then echo "removing $(MAIN)"; rm $(MAIN); else echo "nothing to do, no $(MAIN) is there"; fi
	@if [ -e *.o ]; then  echo removing \*.o files; rm *.o ; else echo 'nothing to do, no *.o files there' ; fi  

test: 
	@echo --------------------------------------------------------------------------------	
	@echo CXXFLAGS:---------------------------------------------------------------------------	
	@echo $(ALLCXXFLAGS)
	@echo --------------------------------------------------------------------------------	
	@echo libs:---------------------------------------------------------------------------	
	@echo $(LIBS)
	@echo --------------------------------------------------------------------------------	
	@echo rpath:--------------------------------------------------------------------------	
	@echo $(RPATH)	
	@echo --------------------------------------------------------------------------------	
	@echo hydralibs:----------------------------------------------------------------------	
	@echo $(HYDRALIBS)
	@echo --------------------------------------------------------------------------------	
	@echo linkerlibs:----------------------------------------------------------------------	
	@echo $(LINKERLIBS)
	@echo --------------------------------------------------------------------------------	
