## File: $RCSfile: unpacker.make,v $
## Version: $Revision: 1.4 $
## Modified by $Author: zumbruch $ on $Date: 2006-05-12 16:46:47 $
################################################################################
include progs.makefile.base.make

MAIN:=unpacker

FLAVOURS:=Nasty Quiet Stubborn \
          CleanBadEvents NastyDebug NastyDebugStubborn \
          NastyStubborn NastyCleanBadEvents QuietStubborn \
          BlackListStubborn BlackList \
          RecoverBlackListStubborn RecoverBlackList \
          RecoverBlackListBlackListStubborn RecoverBlackListBlackList \
	  QuietRecoverBlackListStubborn QuietRecoverBlackList \
          QuietRecoverBlackListBlackListStubborn QuietRecoverBlackListBlackList \
	  FileGrep NastyFileGrep QuietFileGrep StubbornFileGrep \
	  CleanBadEventsFileGrep NastyDebugFileGrep NastyDebugStubbornFileGrep \
	  NastyStubbornFileGrep NastyCleanBadEventsFileGrep QuietStubbornFileGrep \
	  BlackListStubbornFileGrep BlackListFileGrep \
	  BlackListStubbornFileGrep BlackListFileGrep \
	  RecoverBlackListStubbornFileGrep RecoverBlackListFileGrep \
	  RecoverBlackListBlackListStubbornFileGrep RecoverBlackListBlackListFileGrep \
	  QuietRecoverBlackListStubbornFileGrep QuietRecoverBlackListFileGrep \
          QuietRecoverBlackListBlackListStubbornFileGrep QuietRecoverBlackListBlackListFileGrep \

.PHONY: all verbose backup clean test help
help: 
	@echo
	@echo possible calls:
	@echo
	@echo -e "  make all \t--> complete set of executables, incl. backup";
	@echo -e "  make $(MAIN) \t--> default unpacker";
	@for item in `echo $(FLAVOURS)`;\
	do \
	        echo -e "  make $(MAIN) $${item}  \t--> $(MAIN)$${item}"; \
	done; 
	@echo -e "  make clean \t--> clean all executables";
	@echo -e "  make verbose \t--> do nothing, just printouts";
	@echo -e "  make backup \t--> create backup of existing set of files";
	@echo -e "  make test \t--> print out some important variables";
	@echo -e "  make help \t--> this help";
	@echo

all: backup verbose $(MAIN) $(FLAVOURS)

$(MAIN): $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)
	@echo ... done

Stubborn: $(MAIN).cc
	echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D STUBBORN=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

CleanBadEvents: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D CLEANBAD=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

Nasty: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D NASTY=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

NastyDebug: Nasty
	@echo '--------------------------------------------------------------------------------'
	@echo "soft link $(MAIN)$@ -> $(MAIN)$<" 
	@ ln -sf $(MAIN)$< $(MAIN)$@

NastyStubborn: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D STUBBORN=on -D NASTY=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

NastyDebugStubborn: NastyStubborn
	@echo '--------------------------------------------------------------------------------'
	@echo "soft link $(MAIN)$@ -> $(MAIN)$<" 
	@ ln -sf $(MAIN)$< $(MAIN)$@

NastyCleanBadEvents: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D CLEANBAD=on -D NASTY=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

Quiet: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietStubborn: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D STUBBORN=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietCleanBad: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D CLEANBAD=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

BlackList: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D BLACKLIST=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

BlackListStubborn: $(MAIN).cc 
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D BLACKLIST=on -D STUBBORN=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

RecoverBlackList: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

RecoverBlackListStubborn: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D STUBBORN=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

RecoverBlackListBlackList: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D BLACKLIST=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

RecoverBlackListBlackListStubborn: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D BLACKLIST=on -D STUBBORN=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietRecoverBlackList: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietRecoverBlackListStubborn: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D STUBBORN=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietRecoverBlackListBlackList: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D BLACKLIST=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietRecoverBlackListBlackListStubborn: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D BLACKLIST=on -D STUBBORN=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done
FileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

StubbornFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on -D STUBBORN=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

CleanBadEventsFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on -D CLEANBAD=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

NastyFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on -D NASTY=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

NastyDebugFileGrep: NastyFileGrep
	@echo '--------------------------------------------------------------------------------'
	@echo "soft link $(MAIN)$@ -> $(MAIN)$<" 
	@ ln -sf $(MAIN)$< $(MAIN)$@

NastyStubbornFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on -D STUBBORN=on -D NASTY=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

NastyDebugStubbornFileGrep: NastyStubbornFileGrep
	@echo '--------------------------------------------------------------------------------'
	@echo "soft link $(MAIN)$@ -> $(MAIN)$<" 
	@ ln -sf $(MAIN)$< $(MAIN)$@

NastyCleanBadEventsFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on -D CLEANBAD=on -D NASTY=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietStubbornFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on -D STUBBORN=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietCleanBadFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on -D CLEANBAD=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

BlackListFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on -D BLACKLIST=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

BlackListStubbornFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D FILEGREP=on -D BLACKLIST=on -D STUBBORN=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

RecoverBlackListFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D FILEGREP=on 
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

RecoverBlackListStubbornFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D STUBBORN=on -D FILEGREP=on 
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

RecoverBlackListBlackListFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D BLACKLIST=on -D FILEGREP=on 
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

RecoverBlackListBlackListStubbornFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D BLACKLIST=on -D STUBBORN=on -D FILEGREP=on 
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietRecoverBlackListFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D FILEGREP=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietRecoverBlackListStubbornFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D STUBBORN=on -D QUIET=on -D FILEGREP=on 
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietRecoverBlackListBlackListFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D BLACKLIST=on -D FILEGREP=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

QuietRecoverBlackListBlackListStubbornFileGrep: $(MAIN).cc
	@echo '--------------------------------------------------------------------------------'
	@echo "compiling $@ "  
	@$(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D RECOVERBLACKLIST=on -D BLACKLIST=on -D STUBBORN=on -D FILEGREP=on -D QUIET=on
	@echo ... done
	@echo "linking $@ " 
	@$(LD) $(MAIN).o -Wl,-rpath,$(RPATH) $(LINKERLIBS) -o $(MAIN)$@
	@echo ... done

verbose: 
	@echo '--------------------------------------------------------------------------------'
	@echo compiling:; for item in `echo $(CXX) $(MAIN).cc $(CXXFLAGS) -c -o $(MAIN).o -D\"options\"`; do echo $${item}; done;
	@echo '--------------------------------------------------------------------------------'
	@MYRPATH=$(RPATH); echo linking  :; for item in `echo $(LD) $(MAIN).o -Wl,-rpath,$${MYRPATH} $(LINKERLIBS) -o $(MAIN)\"mode\"`; do echo $${item}; done;

clean: 
	@ls *.o >/dev/null 2>&1; \
         if [ $$? -eq 0 ]; \
         then \
            echo removing \*.o files; \
            rm *.o ; \
         else \
            echo 'nothing to do, no *.o files there' ;\
         fi  
	@if [ -f "$(MAIN)" ]; then echo removing ... $(MAIN); rm $(MAIN); fi; 
	@unset name; \
         for item in $$(echo $(FLAVOURS)); \
	 do \
	    name=$(MAIN)$${item};\
	    if [ -f "$${name}" ]; \
	       then echo removing ... $${name}; \
	       rm $${name}; \
	    fi; \
	 done; \
	 unset name;
         
test: 
	@echo --------------------------------------------------------------------------------	
	@echo libs:---------------------------------------------------------------------------	
	@echo $(LIBS)
	@echo --------------------------------------------------------------------------------	
	@echo rpath:--------------------------------------------------------------------------	
	@echo $(RPATH)	
	@echo --------------------------------------------------------------------------------	
	@echo hydralibs:----------------------------------------------------------------------	
	@echo $(HYDRALIBS)X
	@echo --------------------------------------------------------------------------------	
	@echo linkerlibs:----------------------------------------------------------------------	
	@echo $(LINKERLIBS)
	@echo --------------------------------------------------------------------------------	
	@echo CXXFLAGS:------------------------------------------------------------------------	
	@echo $(CXXFLAGS)
	@echo --------------------------------------------------------------------------------	

backup:
	@if [ ! -d $(MAIN)_backup ] ; \
         then \
            mkdir $(MAIN)_backup; \
         fi
	@for item in $$(echo ${FLAVOURS} $(MAIN)); \
         do \
            if [ -f $(MAIN)$${item} ];\
               then \
                  cp $${item} $(MAIN)_backup/$${item}_`date +%d-%m-%y_%H\.%M\.%S`;\
            fi; \
         done     

