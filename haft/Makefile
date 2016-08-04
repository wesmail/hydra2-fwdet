include Makefile.base

PROGNAME = rootHAFT
LIBNAME = libHAFT
OCINT = haftCint
FOBJS = readHAFT.o ran.o
COBJS = $(PROGNAME).o
OBJS = $(COBJS) $(FOBJS)
LIBDEP = $(COBJS:.o=.d)

###########################################################################################

testHAFT: $(FOBJS) testHAFT.o
	g77 testHAFT.o $(FOBJS) -o $@

rootHAFT: $(OBJS) $(OCINT).o
	$(LD) $(LDFLAGS) $(OBJS) $(OCINT).o $(GLIBS) -lg2c -o $@
#	$(LD) $(LDFLAGS) $(OBJS) $(OCINT).o $(GLIBS) -lf2c -o $@

$(LIBNAME): $(OCINT).o $(FOBJS) 
	$(LD) $(LDFLAGS) $(SOFLAGS) $(FOBJS) $(OCINT).o $(GLIBS) -lg2c -o $(LIBNAME).so
#	$(LD) $(LDFLAGS) $(SOFLAGS) $(FOBJS) $(OCINT).o $(GLIBS) -lf2c -o $(LIBNAME).so

$(OCINT).cc: readHAFT.h haftLinkDef.h
	@echo Generating dictionary $(OCINT)
	rootcint -f haftCint.cc -c readHAFT.h haftLinkDef.h
clean:
	@rm -f $(PROGNAME) $(PROGNAME).so *.o *.d  *Cint* core

include $(LIBDEP)
###########################################################################################

%.o:	%.f
	g77 -c $<

%.o:	%.cc
	$(CXX) $(CXXFLAGS) -c $<

%.d:	%.cc
	@$(SHELL) -ec '$(CXX) -M $(CXXFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'


###########################################################################################






